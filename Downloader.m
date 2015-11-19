#import "Downloader.h"

@interface Downloader()

@property (copy) DownloaderCallback callback;
@property (copy) ErrorCallback errorCallback;
@property (copy) DownloaderCallback progressCallback;

@property (retain) NSNumber* statusCode;
@property (retain) NSNumber* contentLength;
@property (retain) NSNumber* bytesWritten;

@property (retain) NSFileHandle* fileHandle;

@end

@implementation Downloader

- (void)downloadFile:(NSString*)urlStr toFile:(NSString*)downloadPath callback:(DownloaderCallback)callback errorCallback:(ErrorCallback)errorCallback progressCallback:(DownloaderCallback)progressCallback
{
  _callback = callback;
  _errorCallback = errorCallback;
  _progressCallback = progressCallback;

  _bytesWritten = 0;

  NSURL* url = [NSURL URLWithString:urlStr];

  NSMutableURLRequest* downloadRequest = [NSMutableURLRequest requestWithURL:url
                                                                 cachePolicy:NSURLRequestUseProtocolCachePolicy
                                                             timeoutInterval:30];

  [[NSFileManager defaultManager] createFileAtPath:downloadPath contents:nil attributes:nil];

  _fileHandle = [NSFileHandle fileHandleForWritingAtPath:downloadPath];

  if (!_fileHandle) {
    NSError* error = [NSError errorWithDomain:@"Downloader" code:NSURLErrorFileDoesNotExist userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat: @"Failed to create target file at path: %@", downloadPath]}];

    return _errorCallback(error);
  }

  NSURLConnection* connection = [[NSURLConnection alloc] initWithRequest:downloadRequest delegate:self startImmediately:NO];

  [connection scheduleInRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];

  [connection start];
}

- (void)connection:(NSURLConnection*)connection didFailWithError:(NSError*)error
{
  [_fileHandle closeFile];

  return _errorCallback(error);
}

- (void)connection:(NSURLConnection*)connection didReceiveResponse:(NSURLResponse*)response
{
  NSHTTPURLResponse* httpUrlResponse = (NSHTTPURLResponse*)response;

  _statusCode = [NSNumber numberWithLong:httpUrlResponse.statusCode];
  _contentLength = [NSNumber numberWithLong: httpUrlResponse.expectedContentLength];
}

- (void)connection:(NSURLConnection*)connection didReceiveData:(NSData*)data
{
  if ([_statusCode isEqualToNumber:[NSNumber numberWithInt:200]]) {
    [_fileHandle writeData:data];

    _bytesWritten = [NSNumber numberWithUnsignedInteger:[_bytesWritten unsignedIntegerValue] + data.length];

    return _progressCallback(_statusCode, _contentLength, _bytesWritten);
  }
}

- (void)connectionDidFinishLoading:(NSURLConnection*)connection
{
  [_fileHandle closeFile];

  return _callback(_statusCode, _contentLength, _bytesWritten);
}

@end
