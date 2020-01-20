package com.rnfs;

import com.facebook.react.bridge.ReadableMap;

import java.net.URL;
import java.util.ArrayList;

public class UploadParams {
    public interface onUploadComplete{
        void onUploadComplete(UploadResult res);
    }
    public interface onUploadProgress{
        void onUploadProgress(int totalBytesExpectedToSend,int totalBytesSent);
    }
    public interface onUploadBegin{
        void onUploadBegin();
    }
    public URL src;
    public ArrayList<ReadableMap> files;
    public boolean binaryStreamOnly;
    public String name;
    public ReadableMap headers;
    public ReadableMap fields;
    public String method;
    public onUploadComplete onUploadComplete;
    public onUploadProgress onUploadProgress;
    public onUploadBegin onUploadBegin;
}
