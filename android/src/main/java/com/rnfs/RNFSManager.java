package com.rnfs;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;

import android.os.Environment;
import android.os.AsyncTask;
import android.util.Base64;
import android.content.Context;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.BufferedInputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.net.URL;
import java.net.URLConnection;

import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.Callback;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.WritableMap;
import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.WritableArray;

public class RNFSManager extends ReactContextBaseJavaModule {

  private static final String NSDocumentDirectoryPath = "NSDocumentDirectoryPath";
  private static final String NSDocumentDirectory = "NSDocumentDirectory";

  private static final String NSFileTypeRegular = "NSFileTypeRegular";
  private static final String NSFileTypeDirectory = "NSFileTypeDirectory";

  public RNFSManager(ReactApplicationContext reactContext) {
    super(reactContext);
  }

  @Override
  public String getName() {
    return "RNFSManager";
  }

  @ReactMethod
  public void writeFile(String filepath, String base64Content, ReadableMap options, Callback callback) {
    try {
      byte[] bytes = Base64.decode(base64Content, Base64.DEFAULT);

      FileOutputStream outputStream = new FileOutputStream(filepath);
      outputStream.write(bytes);
      outputStream.close();

      callback.invoke(null, true, filepath);
    } catch (Exception ex) {
      ex.printStackTrace();
      callback.invoke(makeErrorPayload(ex));
    }
  }

  @ReactMethod
  public void readFile(String filepath, Callback callback) {
    try {
      File file = new File(filepath);

      if (!file.exists()) throw new Exception("File does not exist");

      FileInputStream inputStream = new FileInputStream(filepath);
      byte[] buffer = new byte[(int)file.length()];
      inputStream.read(buffer);

      String base64Content = Base64.encodeToString(buffer, Base64.NO_WRAP);

      callback.invoke(null, base64Content);
    } catch (Exception ex) {
      ex.printStackTrace();
      callback.invoke(makeErrorPayload(ex));
    }
  }

  @ReactMethod
  public void readDir(String directory, Callback callback) {
    try {
      File file = new File(directory);

      if (!file.exists()) throw new Exception("Folder does not exist");

      File[] files = file.listFiles();

      WritableArray fileMaps = Arguments.createArray();

      for (File childFile : files) {
        WritableMap fileMap = Arguments.createMap();

        fileMap.putString("name", childFile.getName());
        fileMap.putString("path", childFile.getAbsolutePath());
        fileMap.putInt("size", (int)childFile.length());
        fileMap.putInt("type", childFile.isDirectory() ? 1 : 0);

        fileMaps.pushMap(fileMap);
      }

      callback.invoke(null, fileMaps);

    } catch (Exception ex) {
      ex.printStackTrace();
      callback.invoke(makeErrorPayload(ex));
    }
  }

  @ReactMethod
  public void stat(String filepath, Callback callback) {
    try {
      File file = new File(filepath);

      if (!file.exists()) throw new Exception("File does not exist");

      WritableMap statMap = Arguments.createMap();

      statMap.putInt("ctime", (int)(file.lastModified() / 1000));
      statMap.putInt("mtime", (int)(file.lastModified() / 1000));
      statMap.putInt("size", (int)file.length());
      statMap.putInt("type", file.isDirectory() ? 1 : 0);

      callback.invoke(null, statMap);
    } catch (Exception ex) {
      ex.printStackTrace();
      callback.invoke(makeErrorPayload(ex));
    }
  }

  @ReactMethod
  public void unlink(String filepath, Callback callback) {
    try {
      File file = new File(filepath);

      if (!file.exists()) throw new Exception("File does not exist");

      boolean success = DeleteRecursive(file);

      callback.invoke(null, success, filepath);
    } catch (Exception ex) {
      ex.printStackTrace();
      callback.invoke(makeErrorPayload(ex));
    }
  }

  private boolean DeleteRecursive(File fileOrDirectory) {
    if (fileOrDirectory.isDirectory()) {
      for (File child : fileOrDirectory.listFiles()) {
        DeleteRecursive(child);
      }
    }

    return fileOrDirectory.delete();
}

  @ReactMethod
  public void mkdir(String filepath, Boolean excludeFromBackup, Callback callback) {
    try {
      File file = new File(filepath);

      file.mkdirs();

      boolean success = file.exists();

      callback.invoke(null, success, filepath);
    } catch (Exception ex) {
      ex.printStackTrace();
      callback.invoke(makeErrorPayload(ex));
    }
  }

  @ReactMethod
  public void downloadFile(String urlStr, final String filepath, final Callback callback) {
    try {
      File file = new File(filepath);
      URL url = new URL(urlStr);

      DownloadParams params = new DownloadParams();
      params.src = url;
      params.dest = file;
      params.onTaskCompleted = new OnTaskCompleted() {
        public void onTaskCompleted(Exception ex) {
          if (ex == null) {
            boolean success = true;
            callback.invoke(null, success, filepath);
          } else {
            callback.invoke(makeErrorPayload(ex));
          }
        }
      };

      DownloadTask downloadTask = new DownloadTask();
      downloadTask.execute(params);
    } catch (Exception ex) {
      ex.printStackTrace();
      callback.invoke(makeErrorPayload(ex));
    }
  }

  private class DownloadParams {
    public URL src;
    public File dest;
    public OnTaskCompleted onTaskCompleted;
  }

  public interface OnTaskCompleted {
    void onTaskCompleted(Exception ex);
  }

  private class DownloadTask extends AsyncTask<DownloadParams, Void, Exception> {
    protected Exception doInBackground(DownloadParams... params) {
      try {
        this.download(params[0]);
        params[0].onTaskCompleted.onTaskCompleted(null);
      } catch (Exception ex) {
        params[0].onTaskCompleted.onTaskCompleted(ex);
        return ex;
      }

      return null;
    }

    private void download(DownloadParams param) throws IOException {
      InputStream input = null;
      OutputStream output = null;

      try {
        URLConnection connection = param.src.openConnection();

        connection.setConnectTimeout(5000);
        connection.connect();

        int lengthOfFile = connection.getContentLength();

        input = new BufferedInputStream(param.src.openStream(), 8192);
        output = new FileOutputStream(param.dest);

        byte data[] = new byte[1024];
        long total = 0;
        int count;

        while ((count = input.read(data)) != -1) {
          total += count;
          //int progress = (int)((total * 100) / lengthOfFile);
          output.write(data, 0, count);
        }

        output.flush();
      } finally {
        if (output != null) output.close();
        if (input != null) input.close();
      }
    }

    protected void onPostExecute(Exception ex) {

    }
 }

  @ReactMethod
  public void pathForBundle(String bundleNamed, Callback callback) {
    // TODO: Not sure what equilivent would be?
  }

  private WritableMap makeErrorPayload(Exception ex) {
    WritableMap error = Arguments.createMap();
    error.putString("message", ex.getMessage());
    return error;
  }

  @Override
  public Map<String, Object> getConstants() {
    final Map<String, Object> constants = new HashMap<>();
    constants.put(NSDocumentDirectory, 0);
    constants.put(NSDocumentDirectoryPath, this.getReactApplicationContext().getFilesDir().getAbsolutePath());
    constants.put(NSFileTypeRegular, 0);
    constants.put(NSFileTypeDirectory, 1);
    return constants;
  }
}
