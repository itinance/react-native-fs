#import "Downloader.h"

@implementation DownloadParams

@end

@interface Downloader()

@property (copy) DownloadParams* params;

@property (retain) NSURLConnection* connection;
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

  NSMutableURLRequest* downloadRequest = [NSMutableURLRequest requestWithURL:url
                                                                 cachePolicy:NSURLRequestUseProtocolCachePolicy
                                                             timeoutInterval:30];

  [[NSFileManager defaultManager] createFileAtPath:_params.toFile contents:nil attributes:nil];

  _fileHandle = [NSFileHandle fileHandleForWritingAtPath:_params.toFile];

  if (!_fileHandle) {
    NSError* error = [NSError errorWithDomain:@"Downloader" code:NSURLErrorFileDoesNotExist userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat: @"Failed to create target file at path: %@", _params.toFile]}];

    return _params.errorCallback(error);
  }

  _connection = [[NSURLConnection alloc] initWithRequest:downloadRequest delegate:self startImmediately:NO];

  [_connection scheduleInRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];

  [_connection start];
}

- (void)connection:(NSURLConnection*)connection didFailWithError:(NSError*)error
{
  [_fileHandle closeFile];

  return _params.errorCallback(error);
}

- (void)connection:(NSURLConnection*)connection didReceiveResponse:(NSURLResponse*)response
{
  NSHTTPURLResponse* httpUrlResponse = (NSHTTPURLResponse*)response;

  _statusCode = [NSNumber numberWithLong:httpUrlResponse.statusCode];
  _contentLength = [NSNumber numberWithLong: httpUrlResponse.expectedContentLength];
  
  return _params.beginCallback(_statusCode, _contentLength, httpUrlResponse.allHeaderFields);
}

- (void)connection:(NSURLConnection*)connection didReceiveData:(NSData*)data
{
  if ([_statusCode isEqualToNumber:[NSNumber numberWithInt:200]]) {
    [_fileHandle writeData:data];

    _bytesWritten = [NSNumber numberWithUnsignedInteger:[_bytesWritten unsignedIntegerValue] + data.length];

    return _params.progressCallback(_contentLength, _bytesWritten);
  }
}

- (void)connectionDidFinishLoading:(NSURLConnection*)connection
{
  [_fileHandle closeFile];

  return _params.callback(_statusCode, _bytesWritten);
}

- (void)stopDownload
{
  [_connection cancel];
}

@end
