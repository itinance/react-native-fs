package com.rnfs;

import com.facebook.react.bridge.WritableMap;

public class UploadResult {
    public int statusCode;
    public WritableMap headers;
    public Exception exception;
    public String body;
}
