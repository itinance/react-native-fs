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
    void onDownloadBegin(int statusCode, int contentLength, Map<String, String> headers);
  }

  public interface OnDownloadProgress {
    void onDownloadProgress(int contentLength, int bytesWritten);
  }

  public URL src;
  public File dest;
  public ReadableMap headers;
  public float progressDivider;
  public OnTaskCompleted onTaskCompleted;
  public OnDownloadBegin onDownloadBegin;
  public OnDownloadProgress onDownloadProgress;
}