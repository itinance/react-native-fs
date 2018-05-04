package com.rnfs;

import com.facebook.react.bridge.ReadableArray;
import com.facebook.react.bridge.ReadableMap;

import java.io.File;
import java.lang.reflect.Array;
import java.net.URL;
import java.util.ArrayList;

public class UploadParams {
    public interface onUploadComplete{
        void onUploadComplete(UploadResult res);
    }
    public interface onUploadProgress{
        void onUploadProgress(int fileCount,int totalBytesExpectedToSend,int totalBytesSent);
    }
    public interface onUploadBegin{
        void onUploadBegin(int totalBytesExpectedToSend);
    }
    public URL src;
    public ArrayList<ReadableMap> files;
    public String name;
    public ReadableMap headers;
    public String method;
    public onUploadComplete onUploadComplete;
    public onUploadProgress onUploadProgress;
    public onUploadBegin onUploadBegin;
}
