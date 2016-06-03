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
    NSError* error = [NSError errorWithDomain:@"Downloader" code:NSURLErrorFileDoesNotExist
                              userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat: @"Failed to create target file at path: %@", _params.toFile]}];

    return _params.errorCallback(error);
  } else {
    [_fileHandle closeFile];
  }

  NSURLSessionConfiguration *config;
  if (_params.background) {
    NSString *uuid = [[NSUUID UUID] UUIDString];
    config = [NSURLSessionConfiguration backgroundSessionConfigurationWithIdentifier:uuid];
  } else {
    config = [NSURLSessionConfiguration defaultSessionConfiguration];
  }

  config.HTTPAdditionalHeaders = _params.headers;

  _session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil];
  _task = [_session downloadTaskWithURL:url];
  [_task resume];
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didWriteData:(int64_t)bytesWritten totalBytesWritten:(int64_t)totalBytesWritten totalBytesExpectedToWrite:(int64_t)totalBytesExpectedToWrite
{
  NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)downloadTask.response;
  if (!_statusCode) {
    _statusCode = [NSNumber numberWithLong:httpResponse.statusCode];
    _contentLength = [NSNumber numberWithLong:httpResponse.expectedContentLength];
    return _params.beginCallback(_statusCode, _contentLength, httpResponse.allHeaderFields);
  }

  if ([_statusCode isEqualToNumber:[NSNumber numberWithInt:200]]) {
    _bytesWritten = @(totalBytesWritten);
    return _params.progressCallback(_contentLength, _bytesWritten);
  }
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didFinishDownloadingToURL:(NSURL *)location
{
  NSURL *destURL = [NSURL fileURLWithPath:_params.toFile];
  NSFileManager *fm = [NSFileManager defaultManager];
  NSError *error = nil;
  [fm removeItemAtURL:destURL error:nil];       // Remove file at destination path, if it exists
  [fm moveItemAtURL:location toURL:destURL error:&error];
  if (error) {
    NSLog(@"RNFS download: unable to move tempfile to destination. %@, %@", error, error.userInfo);
  }

  return _params.completeCallback(_statusCode, _bytesWritten);
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionTask *)downloadTask didCompleteWithError:(NSError *)error
{
  return _params.errorCallback(error);
}


- (void)stopDownload
{
  [_task cancel];
}

@end
