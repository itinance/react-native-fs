package com.rnfs;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.BufferedInputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.net.URL;
import java.net.URLConnection;
import java.net.HttpURLConnection;
import java.util.*;
import java.util.concurrent.atomic.AtomicBoolean;

import android.util.Log;

import android.os.AsyncTask;

import com.facebook.react.bridge.ReadableMapKeySetIterator;

public class Downloader extends AsyncTask<DownloadParams, int[], DownloadResult> {
  private DownloadParams mParam;
  private AtomicBoolean mAbort = new AtomicBoolean(false);

  protected DownloadResult doInBackground(DownloadParams... params) {
    mParam = params[0];

    DownloadResult res = new DownloadResult();

    try {
      this.download(mParam, res);
      mParam.onTaskCompleted.onTaskCompleted(res);
    } catch (Exception ex) {
      res.exception = ex;
      mParam.onTaskCompleted.onTaskCompleted(res);
      return res;
    }

    return res;
  }

  private void download(DownloadParams param, DownloadResult res) throws Exception {
    InputStream input = null;
    OutputStream output = null;
    HttpURLConnection connection = null;

    try {
      connection = (HttpURLConnection)param.src.openConnection();

      ReadableMapKeySetIterator iterator = param.headers.keySetIterator();

      while (iterator.hasNextKey()) {
        String key = iterator.nextKey();
        String value = param.headers.getString(key);
        connection.setRequestProperty(key, value);
      }

      connection.setConnectTimeout(5000);
      connection.setReadTimeout(15000);
      connection.connect();

      int statusCode = connection.getResponseCode();
      int lengthOfFile = connection.getContentLength();

      boolean isRedirect = (
        statusCode != HttpURLConnection.HTTP_OK &&
        (
          statusCode == HttpURLConnection.HTTP_MOVED_PERM ||
          statusCode == HttpURLConnection.HTTP_MOVED_TEMP
        )
      );

      if (isRedirect) {
        String redirectURL = connection.getHeaderField("Location");
        connection.disconnect();

        connection = (HttpURLConnection) new URL(redirectURL).openConnection();
        connection.setConnectTimeout(5000);
        connection.connect();

        statusCode = connection.getResponseCode();
        lengthOfFile = connection.getContentLength();
      }

      Map<String, List<String>> headers = connection.getHeaderFields();

      Map<String, String> headersFlat = new HashMap<String, String>();

      for (Map.Entry<String, List<String>> entry : headers.entrySet()) {
        String headerKey = entry.getKey();
        String valueKey = entry.getValue().get(0);

        if (headerKey != null && valueKey != null) {
          headersFlat.put(headerKey, valueKey);
        }
      }

      mParam.onDownloadBegin.onDownloadBegin(statusCode, lengthOfFile, headersFlat);

      input = new BufferedInputStream(connection.getInputStream(), 8 * 1024);
      output = new FileOutputStream(param.dest);

      byte data[] = new byte[8 * 1024];
      int total = 0;
      int count;
      double lastProgressValue = 0;

      while ((count = input.read(data)) != -1) {
        if (mAbort.get()) throw new Exception("Download has been aborted");

        total += count;
        if (param.progressDivider <= 0) {
          publishProgress(new int[]{lengthOfFile, total});
        } else {
          double progress = Math.round(((double) total * 100) / lengthOfFile);
          if (progress % param.progressDivider == 0) {
            if ((progress != lastProgressValue) || (total == lengthOfFile)) {
              Log.d("Downloader", "EMIT: " + String.valueOf(progress) + ", TOTAL:" + String.valueOf(total));
              lastProgressValue = progress;
              publishProgress(new int[]{lengthOfFile, total});
            }
          }
        }
        output.write(data, 0, count);
      }

      output.flush();

      res.statusCode = statusCode;
      res.bytesWritten = total;
    } finally {
      if (output != null) output.close();
      if (input != null) input.close();
      if (connection != null) connection.disconnect();
    }
  }

  protected void stop() {
    mAbort.set(true);
  }

  @Override
  protected void onProgressUpdate(int[]... values) {
    super.onProgressUpdate(values);
    mParam.onDownloadProgress.onDownloadProgress(values[0][0], values[0][1]);
  }

  protected void onPostExecute(Exception ex) {

  }
}
