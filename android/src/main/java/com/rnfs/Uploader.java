package com.rnfs;

import android.os.AsyncTask;
import android.webkit.MimeTypeMap;

import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.NoSuchKeyException;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.ReadableMapKeySetIterator;
import com.facebook.react.bridge.WritableArray;
import com.facebook.react.bridge.WritableMap;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

public class Uploader extends AsyncTask<UploadParams, int[], UploadResult> {
    private UploadParams mParams;
    private UploadResult res;
    private AtomicBoolean mAbort = new AtomicBoolean(false);

    @Override
    protected UploadResult doInBackground(UploadParams... uploadParams) {
        mParams = uploadParams[0];
        res = new UploadResult();
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    upload(mParams, res);
                    mParams.onUploadComplete.onUploadComplete(res);
                } catch (Exception e) {
                    res.exception = e;
                    mParams.onUploadComplete.onUploadComplete(res);
                }
            }
        }).start();
        return res;
    }

    private void upload(UploadParams params, UploadResult result) throws Exception {
        HttpURLConnection connection = null;
        DataOutputStream request = null;
        String crlf = "\r\n";
        String twoHyphens = "--";
        String boundary = "*****";
        int Readed, bufferSize, totalSize, byteRead, statusCode, bufferAvailable;
        int fileCount = 1;
        BufferedInputStream responseStream = null;
        BufferedReader responseStreamReader = null;
        int maxBufferSize = 1 * 1024 * 1024;
        String name, filename, filetype;
        Map<String, List<String>> responseHeader;
        try {
            connection = (HttpURLConnection) params.src.openConnection();
            connection.setUseCaches(false);
            connection.setDoOutput(true);
            connection.setDoInput(true);
            ReadableMapKeySetIterator headerIterator = params.headers.keySetIterator();
            connection.setRequestMethod(params.method);
            connection.setRequestProperty("Content-Type", "multipart/form-data;boundary=" + boundary);
            while (headerIterator.hasNextKey()) {
                String key = headerIterator.nextKey();
                String value = params.headers.getString(key);
                connection.setRequestProperty(key, value);
            }

            request = new DataOutputStream(connection.getOutputStream());

            ReadableMapKeySetIterator fieldsIterator = params.fields.keySetIterator();

            while (fieldsIterator.hasNextKey()) {
                request.writeBytes(twoHyphens + boundary + crlf);
                String key = fieldsIterator.nextKey();
                String value = params.fields.getString(key);
                request.writeBytes("Content-Disposition: form-data; name=\"" + key + "\"" + crlf);
                request.writeBytes(crlf);
                request.writeBytes(value);
                request.writeBytes(crlf);
            }
            mParams.onUploadBegin.onUploadBegin();
            for (ReadableMap map : params.files) {
                try {
                    name = map.getString("name");
                    filename = map.getString("filename");
                    filetype = map.getString("filetype");
                } catch (NoSuchKeyException e) {
                    name = map.getString("filename");
                    filename = map.getString("filename");
                    filetype = getMimeType(map.getString("filepath"));
                }
                request.writeBytes(twoHyphens + boundary + crlf);
                File file = new File(map.getString("filepath"));
                request.writeBytes(
                        "Content-Disposition: form-data; name=\"" + name + "\";filename=\"" + filename + "\"" + crlf);
                request.writeBytes("Content-Type: " + filetype + crlf);
                request.writeBytes(crlf);
                byte[] b = new byte[(int) file.length()];

                FileInputStream fileInputStream = new FileInputStream(file);

                Readed = 0;
                bufferAvailable = 4096;
                bufferSize = Math.min(bufferAvailable, maxBufferSize);
                byteRead = fileInputStream.read(b, 0, bufferSize);
                totalSize = b.length;
                Readed += byteRead;
                while (byteRead > 0) {
                    if (mAbort.get())
                        throw new Exception("Upload has been aborted");
                    request.write(b, 0, bufferSize);
                    byteRead = fileInputStream.read(b, 0, bufferSize);
                    if (byteRead == -1) {
                        mParams.onUploadProgress.onUploadProgress(fileCount, totalSize, Readed);
                    } else {
                        Readed += byteRead;
                        mParams.onUploadProgress.onUploadProgress(fileCount, totalSize, Readed);
                    }
                }
                request.writeBytes(crlf);
                fileCount++;
            }
            request.writeBytes(twoHyphens + boundary + twoHyphens + crlf);
            request.flush();

            responseStream = new BufferedInputStream(connection.getInputStream());
            responseStreamReader = new BufferedReader(new InputStreamReader(responseStream));
            WritableMap responseHeaders = Arguments.createMap();
            Map<String, List<String>> map = connection.getHeaderFields();
            for (Map.Entry<String, List<String>> entry : map.entrySet()) {
                int count = 0;
                responseHeaders.putString(entry.getKey(), entry.getValue().get(count));
            }
            StringBuilder stringBuilder = new StringBuilder();
            String line = "";

            while ((line = responseStreamReader.readLine()) != null) {
                stringBuilder.append(line).append("\n");
            }

            String response = stringBuilder.toString();
            statusCode = connection.getResponseCode();
            res.headers = responseHeaders;
            res.body = response;
            res.statusCode = statusCode;
        } finally {
            if (connection != null)
                connection.disconnect();
            if (request != null)
                request.close();
            if (responseStream != null)
                responseStream.close();
            if (responseStreamReader != null)
                responseStreamReader.close();
        }
    }

    protected String getMimeType(String path) {
        String type = null;
        String extension = MimeTypeMap.getFileExtensionFromUrl(path);
        if (extension != null) {
            type = MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension);
        }
        return type;
    }

    protected void stop() {
        mAbort.set(true);
    }

}
