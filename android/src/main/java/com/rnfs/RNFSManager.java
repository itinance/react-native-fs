package com.rnfs;

import java.io.IOException;
import java.util.Map;
import java.util.HashMap;

import android.os.Environment;
import android.os.StatFs;
import android.util.Base64;
import android.support.annotation.Nullable;
import android.util.SparseArray;

import java.io.File;
import java.io.OutputStream;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.net.URL;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.WritableMap;
import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.WritableArray;
import com.facebook.react.modules.core.RCTNativeAppEventEmitter;

public class RNFSManager extends ReactContextBaseJavaModule {

  private static final String NSDocumentDirectoryPath = "NSDocumentDirectoryPath";
  private static final String NSExternalDirectoryPath = "NSExternalDirectoryPath";
  private static final String NSPicturesDirectoryPath = "NSPicturesDirectoryPath";
  private static final String NSTemporaryDirectoryPath = "NSTemporaryDirectoryPath";
  private static final String NSCachesDirectoryPath = "NSCachesDirectoryPath";
  private static final String NSDocumentDirectory = "NSDocumentDirectory";

  private static final String NSFileTypeRegular = "NSFileTypeRegular";
  private static final String NSFileTypeDirectory = "NSFileTypeDirectory";

  private SparseArray<Downloader> downloaders = new SparseArray<Downloader>();

  public RNFSManager(ReactApplicationContext reactContext) {
    super(reactContext);
  }

  @Override
  public String getName() {
    return "RNFSManager";
  }

  @ReactMethod
  public void writeFile(String filepath, String base64Content, Promise promise) {
    try {
      byte[] bytes = Base64.decode(base64Content, Base64.DEFAULT);

      FileOutputStream outputStream = new FileOutputStream(filepath, false);
      outputStream.write(bytes);
      outputStream.close();

      promise.resolve(null);
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, filepath, ex);
    }
  }

  @ReactMethod
  public void appendFile(String filepath, String base64Content, Promise promise) {
    try {
      byte[] bytes = Base64.decode(base64Content, Base64.DEFAULT);

      FileOutputStream outputStream = new FileOutputStream(filepath, true);
      outputStream.write(bytes);
      outputStream.close();

      promise.resolve(null);
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, filepath, ex);
    }
  }

  @ReactMethod
  public void exists(String filepath, Promise promise) {
    try {
      File file = new File(filepath);
      promise.resolve(file.exists());
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, filepath, ex);
    }
  }

  @ReactMethod
  public void readFile(String filepath, Promise promise) {
    try {
      File file = new File(filepath);

      if (file.isDirectory()) {
        rejectFileIsDirectory(promise);
        return;
      }

      if (!file.exists()) {
        rejectFileNotFound(promise, filepath);
        return;
      }

      FileInputStream inputStream = new FileInputStream(filepath);
      byte[] buffer = new byte[(int)file.length()];
      inputStream.read(buffer);

      String base64Content = Base64.encodeToString(buffer, Base64.NO_WRAP);

      promise.resolve(base64Content);
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, filepath, ex);
    }
  }

  @ReactMethod
  public void moveFile(String filepath, String destPath, Promise promise) {
    try {
      File inFile = new File(filepath);

      if (!inFile.renameTo(new File(destPath))) {
        copyFile(filepath, destPath);

        inFile.delete();
      }

      promise.resolve(true);
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, filepath, ex);
    }
  }

  @ReactMethod
  public void copyFile(String filepath, String destPath, Promise promise) {
    try {
      copyFile(filepath, destPath);

      promise.resolve(null);
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, filepath, ex);
    }
  }

  private void copyFile(String filepath, String destPath) throws IOException {
    InputStream in = new FileInputStream(filepath);
    OutputStream out = new FileOutputStream(destPath);

    byte[] buffer = new byte[1024];
    int length;
    while ((length = in.read(buffer)) > 0) {
      out.write(buffer, 0, length);
    }
    in.close();
    out.close();
  }

  @ReactMethod
  public void readDir(String directory, Promise promise) {
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

      promise.resolve(fileMaps);
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, directory, ex);
    }
  }

  @ReactMethod
  public void stat(String filepath, Promise promise) {
    try {
      File file = new File(filepath);

      if (!file.exists()) throw new Exception("File does not exist");

      WritableMap statMap = Arguments.createMap();

      statMap.putInt("ctime", (int)(file.lastModified() / 1000));
      statMap.putInt("mtime", (int)(file.lastModified() / 1000));
      statMap.putInt("size", (int)file.length());
      statMap.putInt("type", file.isDirectory() ? 1 : 0);

      promise.resolve(statMap);
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, filepath, ex);
    }
  }

  @ReactMethod
  public void unlink(String filepath, Promise promise) {
    try {
      File file = new File(filepath);

      if (!file.exists()) throw new Exception("File does not exist");

      DeleteRecursive(file);

      promise.resolve(null);
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, filepath, ex);
    }
  }

  private void DeleteRecursive(File fileOrDirectory) {
    if (fileOrDirectory.isDirectory()) {
      for (File child : fileOrDirectory.listFiles()) {
        DeleteRecursive(child);
      }
    }

    fileOrDirectory.delete();
  }

  @ReactMethod
  public void mkdir(String filepath, ReadableMap options, Promise promise) {
    try {
      File file = new File(filepath);

      file.mkdirs();

      boolean exists = file.exists();

      if (!exists) throw new Exception("Directory could not be created");

      promise.resolve(null);
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, filepath, ex);
    }
  }

  private void sendEvent(ReactContext reactContext, String eventName, @Nullable WritableMap params) {
    reactContext
    .getJSModule(RCTNativeAppEventEmitter.class)
    .emit(eventName, params);
  }

  @ReactMethod
  public void downloadFile(final ReadableMap options, final Promise promise) {
    try {
      File file = new File(options.getString("toFile"));
      URL url = new URL(options.getString("fromUrl"));
      final int jobId = options.getInt("jobId");
      ReadableMap headers = options.getMap("headers");
      int progressDivider = options.getInt("progressDivider");

      DownloadParams params = new DownloadParams();

      params.src = url;
      params.dest = file;
      params.headers = headers;
      params.progressDivider = progressDivider;

      params.onTaskCompleted = new DownloadParams.OnTaskCompleted() {
        public void onTaskCompleted(DownloadResult res) {
          if (res.exception == null) {
            WritableMap infoMap = Arguments.createMap();

            infoMap.putInt("jobId", jobId);
            infoMap.putInt("statusCode", res.statusCode);
            infoMap.putInt("bytesWritten", res.bytesWritten);

            promise.resolve(infoMap);
          } else {
            reject(promise, options.getString("toFile"), res.exception);
          }
        }
      };

      params.onDownloadBegin = new DownloadParams.OnDownloadBegin() {
        public void onDownloadBegin(int statusCode, int contentLength, Map<String, String> headers) {
          WritableMap headersMap = Arguments.createMap();

          for (Map.Entry<String, String> entry : headers.entrySet()) {
            headersMap.putString(entry.getKey(), entry.getValue());
          }

          WritableMap data = Arguments.createMap();

          data.putInt("jobId", jobId);
          data.putInt("statusCode", statusCode);
          data.putInt("contentLength", contentLength);
          data.putMap("headers", headersMap);

          sendEvent(getReactApplicationContext(), "DownloadBegin-" + jobId, data);
        }
      };

      params.onDownloadProgress = new DownloadParams.OnDownloadProgress() {
        public void onDownloadProgress(int contentLength, int bytesWritten) {
          WritableMap data = Arguments.createMap();

          data.putInt("jobId", jobId);
          data.putInt("contentLength", contentLength);
          data.putInt("bytesWritten", bytesWritten);

          sendEvent(getReactApplicationContext(), "DownloadProgress-" + jobId, data);
        }
      };

      Downloader downloader = new Downloader();

      downloader.execute(params);

      this.downloaders.put(jobId, downloader);
    } catch (Exception ex) {
      ex.printStackTrace();
      reject(promise, options.getString("toFile"), ex);
    }
  }

  @ReactMethod
  public void stopDownload(int jobId) {
    Downloader downloader = this.downloaders.get(jobId);

    if (downloader != null) {
      downloader.stop();
    }
  }

  @ReactMethod
  public void pathForBundle(String bundleNamed, Promise promise) {
    // TODO: Not sure what equilivent would be?
  }

  @ReactMethod
  public void getFSInfo(Promise promise) {
    File path = Environment.getDataDirectory();
    StatFs stat = new StatFs(path.getPath());
    long totalSpace;
    long freeSpace;
    if (android.os.Build.VERSION.SDK_INT >= 18) {
      totalSpace = stat.getTotalBytes();
      freeSpace = stat.getFreeBytes();
    } else {
      long blockSize = stat.getBlockSize();
      totalSpace = blockSize * stat.getBlockCount();
      freeSpace = blockSize * stat.getAvailableBlocks();
    }
    WritableMap info = Arguments.createMap();
    info.putDouble("totalSpace", (double)totalSpace);   // Int32 too small, must use Double
    info.putDouble("freeSpace", (double)freeSpace);
    promise.resolve(info);
  }

  private void reject(Promise promise, String filepath, Exception ex) {
    if (ex instanceof FileNotFoundException) {
      rejectFileNotFound(promise, filepath);
      return;
    }

    promise.reject(null, ex.getMessage());
  }

  private void rejectFileNotFound(Promise promise, String filepath) {
    promise.reject("ENOENT", "ENOENT: no such file or directory, open '" + filepath + "'");
  }

  private void rejectFileIsDirectory(Promise promise) {
    promise.reject("EISDIR", "EISDIR: illegal operation on a directory, read");
  }

  @Override
  public Map<String, Object> getConstants() {
    final Map<String, Object> constants = new HashMap<>();
    constants.put(NSDocumentDirectory, 0);
    constants.put(NSDocumentDirectoryPath, this.getReactApplicationContext().getFilesDir().getAbsolutePath());
    constants.put(NSTemporaryDirectoryPath, null);
    File externalDirectory = this.getReactApplicationContext().getExternalFilesDir(null);
    if (externalDirectory != null) {
      constants.put(NSExternalDirectoryPath, externalDirectory.getAbsolutePath());
    } else {
      constants.put(NSExternalDirectoryPath, null);
    }
    constants.put(NSPicturesDirectoryPath, Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).getAbsolutePath());
    constants.put(NSCachesDirectoryPath, this.getReactApplicationContext().getCacheDir().getAbsolutePath());
    constants.put(NSFileTypeRegular, 0);
    constants.put(NSFileTypeDirectory, 1);
    return constants;
  }
}
