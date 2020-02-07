package com.rnfs;

import android.os.AsyncTask;
import android.webkit.MimeTypeMap;

import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.NoSuchKeyException;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.ReadableMapKeySetIterator;
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
        String tail = crlf + twoHyphens + boundary + twoHyphens + crlf;
        String metaData = "", stringData = "";
        String[] fileHeader;
        int statusCode, byteSentTotal;
        int fileCount = 0;
        long totalFileLength = 0;
        BufferedInputStream responseStream = null;
        BufferedReader responseStreamReader = null;
        String name, filename, filetype;
        try {
            Object[] files = params.files.toArray();
            boolean binaryStreamOnly = params.binaryStreamOnly;

            connection = (HttpURLConnection) params.src.openConnection();
            connection.setDoOutput(true);
            ReadableMapKeySetIterator headerIterator = params.headers.keySetIterator();
            connection.setRequestMethod(params.method);
            if (!binaryStreamOnly) {
                connection.setRequestProperty("Content-Type", "multipart/form-data;boundary=" + boundary);
            }
            while (headerIterator.hasNextKey()) {
                String key = headerIterator.nextKey();
                String value = params.headers.getString(key);
                connection.setRequestProperty(key, value);
            }

            ReadableMapKeySetIterator fieldsIterator = params.fields.keySetIterator();

            while (fieldsIterator.hasNextKey()) {
                String key = fieldsIterator.nextKey();
                String value = params.fields.getString(key);
                metaData += twoHyphens + boundary + crlf + "Content-Disposition: form-data; name=\"" + key + "\"" + crlf + crlf + value +crlf;
            }
            stringData += metaData;
            fileHeader = new String[files.length];
            for (ReadableMap map : params.files) {
                try {
                    name = map.getString("name");
                    filename = map.getString("filename");
                    filetype = map.getString("filetype");
                } catch (NoSuchKeyException e) {
                    name = map.getString("name");
                    filename = map.getString("filename");
                    filetype = getMimeType(map.getString("filepath"));
                }
                File file = new File(map.getString("filepath"));
                long fileLength = file.length();
                totalFileLength += fileLength;
                if (!binaryStreamOnly) {
                    String fileHeaderType = twoHyphens + boundary + crlf +
                            "Content-Disposition: form-data; name=\"" + name + "\"; filename=\"" + filename + "\"" + crlf +
                            "Content-Type: " + filetype + crlf;
                    ;
                    if (files.length - 1 == fileCount){
                        totalFileLength += tail.length();
                    }

                    String fileLengthHeader = "Content-length: " + fileLength + crlf;
                    fileHeader[fileCount] = fileHeaderType + fileLengthHeader + crlf;
                    stringData += fileHeaderType + fileLengthHeader + crlf;
                }
                fileCount++;
            }
            fileCount = 0;
            if (mParams.onUploadBegin != null) {
                mParams.onUploadBegin.onUploadBegin();
            }
            if (!binaryStreamOnly) {
                long requestLength = totalFileLength;
                requestLength += stringData.length() + files.length * crlf.length();
                connection.setRequestProperty("Content-length", "" +(int) requestLength);
                connection.setFixedLengthStreamingMode((int)requestLength);
            }
            connection.connect();

            request = new DataOutputStream(connection.getOutputStream());
            if (!binaryStreamOnly) {
                request.writeBytes(metaData);
            }

            byteSentTotal = 0;
            Runtime run = Runtime.getRuntime();

            for (ReadableMap map : params.files) {
                if (!binaryStreamOnly) {
                    request.writeBytes(fileHeader[fileCount]);
                }
                File file = new File(map.getString("filepath"));
                int fileLength = (int) file.length();
                int bytes_read = 0;
                BufferedInputStream bufInput = new BufferedInputStream(new FileInputStream(file));
                int buffer_size =(int) Math.ceil(fileLength / 100.f);
                if(buffer_size > run.freeMemory() / 10.f) {
                    buffer_size = (int) Math.ceil(run.freeMemory() / 10.f);
                }
                byte[] buffer = new byte[buffer_size];
                while ((bytes_read = bufInput.read(buffer)) != -1) {
                    request.write(buffer, 0, bytes_read);
                    if (mParams.onUploadProgress != null) {
                        byteSentTotal += bytes_read;
                        mParams.onUploadProgress.onUploadProgress((int) totalFileLength, byteSentTotal);
                    }
                }
                if (!binaryStreamOnly) {
                    request.writeBytes(crlf);
                }
                fileCount++;
                bufInput.close();
            }
            if (!binaryStreamOnly) {
                request.writeBytes(tail);
            }
            request.flush();
            request.close();

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
            type = MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension.toLowerCase());
        }
        if (type == null) {
            type = "*/*";
        }
        return type;
    }

    protected void stop() {
        mAbort.set(true);
    }
}
