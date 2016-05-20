#import "Downloader.h"

@implementation DownloadParams

@end

@interface Downloader()

@property (copy) DownloadParams* params;

@property (retain) NSURLSession* session;
@property (retain) NSURLSessionTask* task;
@property (retain) NSNumber* statusCode;
@property (retain) NSNumber* contentLength;
@property (retain) NSNumber* bytesWritten;

@property (retain) NSFileHandle* fileHandle;

@end

@implementation Downloader

- (void)downloadFile:(DownloadParams*)params
{
  _params = params;

  _bytesWritten = 0;

  NSURL* url = [NSURL URLWithString:_params.fromUrl];

  [[NSFileManager defaultManager] createFileAtPath:_params.toFile contents:nil attributes:nil];
  _fileHandle = [NSFileHandle fileHandleForWritingAtPath:_params.toFile];

  if (!_fileHandle) {
    NSError* error = [NSError errorWithDomain:@"Downloader" code:NSURLErrorFileDoesNotExist userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat: @"Failed to create target file at path: %@", _params.toFile]}];

    return _params.errorCallback(error);
  }

  NSURLSessionConfiguration *config;
  config = [NSURLSessionConfiguration defaultSessionConfiguration];
  // TODO: use the following config for session objects:
  //config = [NSURLSessionConfiguration backgroundSessionConfigurationWithIdentifier:fromUrl];

  _session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil];
  _task = [_session downloadTaskWithURL:url];
  [_task resume];
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didReceiveResponse:(NSURLResponse *)response
{
  NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)downloadTask.response;
  _statusCode = [NSNumber numberWithLong:httpResponse.statusCode];
  _contentLength = [NSNumber numberWithLong:httpResponse.expectedContentLength];

  return _params.beginCallback(_statusCode, _contentLength, httpResponse.allHeaderFields);
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didWriteData:(NSData *)data
{
  if ([_statusCode isEqualToNumber:[NSNumber numberWithInt:200]]) {
    [_fileHandle writeData:data];

    _bytesWritten = [NSNumber numberWithUnsignedInteger:[_bytesWritten unsignedIntegerValue] + data.length];

    return _params.progressCallback(_contentLength, _bytesWritten);
  }
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didFinishDownloadingToURL:(NSURL *)location
{
  [_fileHandle closeFile];

  return _params.completeCallback(_statusCode, _bytesWritten);
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionTask *)downloadTask didCompleteWithError:(NSError *)error
{
  [_fileHandle closeFile];
  return _params.errorCallback(error);
}


- (void)stopDownload
{
  [_task cancel];
}

@end
