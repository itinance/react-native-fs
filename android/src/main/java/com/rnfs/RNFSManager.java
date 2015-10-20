package com.rnfs;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;

import android.os.Environment;
import android.util.Base64;
import android.content.Context;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;

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

      callback.invoke();
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

      String base64Content = Base64.encodeToString(buffer, Base64.DEFAULT);

      callback.invoke(null, base64Content);
    } catch (Exception ex) {
      ex.printStackTrace();
      callback.invoke(makeErrorPayload(ex));
    }
  }

  @ReactMethod
  public void readDir(String directory, Integer folder, Callback callback) {
    try {
      if (folder != 0) throw new Exception("Only NSDocumentDirectory supported");

      String folderPath = this.getReactApplicationContext().getFilesDir().getAbsolutePath();

      File file = new File(folderPath, directory);

      if (!file.exists()) throw new Exception("File does not exist");

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
  public void mkdir(String filepath, Callback callback) {
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
