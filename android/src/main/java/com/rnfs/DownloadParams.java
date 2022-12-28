package com.rnfs;

import java.io.File;
import java.net.URL;
import java.util.*;

import com.facebook.react.bridge.ReadableMap;

public class DownloadParams {
  public interface OnTaskCompleted {
    void onTaskCompleted(DownloadResult res);
  }

  public interface OnDownloadBegin {
    void onDownloadBegin(int statusCode, long contentLength, Map<String, String> headers);
  }

  public interface OnDownloadProgress {
    void onDownloadProgress(long contentLength, long bytesWritten);
  }

  public URL src;
  public File dest;
  public ReadableMap headers;
  public int progressInterval;
  public float progressDivider;
  public int readTimeout;
  public int connectionTimeout;
  public OnTaskCompleted onTaskCompleted;
  public OnDownloadBegin onDownloadBegin;
  public OnDownloadProgress onDownloadProgress;
}