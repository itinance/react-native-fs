#import "Downloader.h"

@implementation RNFSDownloadParams

@end

@interface RNFSDownloader()

@property (copy) RNFSDownloadParams* params;

@property (retain) NSURLSession* session;
@property (retain) NSURLSessionDownloadTask* task;
@property (retain) NSNumber* statusCode;
@property (assign) NSTimeInterval lastProgressEmitTimestamp;
@property (retain) NSNumber* lastProgressValue;
@property (retain) NSNumber* contentLength;
@property (retain) NSNumber* bytesWritten;
@property (retain) NSData* resumeData;

@property (retain) NSFileHandle* fileHandle;

@end

@implementation RNFSDownloader

- (NSString *)downloadFile:(RNFSDownloadParams*)params
{
    NSString *uuid = nil;

    _params = params;

  _lastProgressEmitTimestamp = 0;
  _bytesWritten = 0;

  NSURL* url = [NSURL URLWithString:_params.fromUrl];

  if ([[NSFileManager defaultManager] fileExistsAtPath:_params.toFile]) {
    _fileHandle = [NSFileHandle fileHandleForWritingAtPath:_params.toFile];

    if (!_fileHandle) {
      NSError* error = [NSError errorWithDomain:@"Downloader" code:NSURLErrorFileDoesNotExist
                                userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat: @"Failed to write target file at path: %@", _params.toFile]}];

      _params.errorCallback(error);
      return nil;
    } else {
      [_fileHandle closeFile];
    }
  }

  NSURLSessionConfiguration *config;
  if (_params.background) {
    uuid = [[NSUUID UUID] UUIDString];
    config = [NSURLSessionConfiguration backgroundSessionConfigurationWithIdentifier:uuid];
    config.discretionary = _params.discretionary;
  } else {
    config = [NSURLSessionConfiguration defaultSessionConfiguration];
  }

  if (!_params.cacheable) {
    config.URLCache = nil;
  }

  config.HTTPAdditionalHeaders = _params.headers;
  config.timeoutIntervalForRequest = [_params.readTimeout intValue] / 1000.0;
  config.timeoutIntervalForResource = [_params.backgroundTimeout intValue] / 1000.0;

  _session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil];
  _task = [_session downloadTaskWithURL:url];
  [_task resume];

    return uuid;
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didWriteData:(int64_t)bytesWritten totalBytesWritten:(int64_t)totalBytesWritten totalBytesExpectedToWrite:(int64_t)totalBytesExpectedToWrite
{
  NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)downloadTask.response;
  if (_params.beginCallback && !_statusCode) {
    _statusCode = [NSNumber numberWithLong:httpResponse.statusCode];
    _contentLength = [NSNumber numberWithLong:httpResponse.expectedContentLength];
    return _params.beginCallback(_statusCode, _contentLength, httpResponse.allHeaderFields);
  }

  if (_params.progressCallback && [_statusCode isEqualToNumber:[NSNumber numberWithInt:200]]) {
    _bytesWritten = @(totalBytesWritten);

    if(_params.progressInterval.integerValue > 0){
      NSTimeInterval timestamp = [[NSDate date] timeIntervalSince1970];
      if(timestamp - _lastProgressEmitTimestamp > _params.progressInterval.integerValue / 1000.0){
        _lastProgressEmitTimestamp = timestamp;
        return _params.progressCallback(_contentLength, _bytesWritten);
      }
    }else if (_params.progressDivider.integerValue <= 0) {
      return _params.progressCallback(_contentLength, _bytesWritten);
    } else {
      double doubleBytesWritten = (double)[_bytesWritten longValue];
      double doubleContentLength = (double)[_contentLength longValue];
      double doublePercents = doubleBytesWritten / doubleContentLength * 100;
      NSNumber* progress = [NSNumber numberWithUnsignedInt: floor(doublePercents)];
      if ([progress unsignedIntValue] % [_params.progressDivider integerValue] == 0) {
        if (([progress unsignedIntValue] != [_lastProgressValue unsignedIntValue]) || ([_bytesWritten unsignedIntegerValue] == [_contentLength longValue])) {
            NSLog(@"---Progress callback EMIT--- %u", [progress unsignedIntValue]);
          _lastProgressValue = [NSNumber numberWithUnsignedInt:[progress unsignedIntValue]];
          return _params.progressCallback(_contentLength, _bytesWritten);
        }
      }
    }
  }
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didFinishDownloadingToURL:(NSURL *)location
{
  NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)downloadTask.response;
  if (!_statusCode) {
    _statusCode = [NSNumber numberWithLong:httpResponse.statusCode];
  }
  NSURL *destURL = [NSURL fileURLWithPath:_params.toFile];
  NSFileManager *fm = [NSFileManager defaultManager];
  NSError *error = nil;
  if([_statusCode integerValue] >= 200 && [_statusCode integerValue] < 300) {
    [fm removeItemAtURL:destURL error:nil];       // Remove file at destination path, if it exists
    [fm moveItemAtURL:location toURL:destURL error:&error];
    // There are no guarantees about how often URLSession:downloadTask:didWriteData: will fire,
    // so we read an authoritative number of bytes written here.
    _bytesWritten = @([fm attributesOfItemAtPath:_params.toFile error:nil].fileSize);
  }
  if (error) {
    NSLog(@"RNFS download: unable to move tempfile to destination. %@, %@", error, error.userInfo);
  }

  return _params.completeCallback(_statusCode, _bytesWritten);
}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error
{
  if (error && error.code != NSURLErrorCancelled) {
      _resumeData = error.userInfo[NSURLSessionDownloadTaskResumeData];
      if (_resumeData != nil) {
        if (_params.resumableCallback) {
            _params.resumableCallback();
        }
      } else {
          _params.errorCallback(error);
      }
  }
}

- (void)stopDownload
{
  if (_task.state == NSURLSessionTaskStateRunning) {
    [_task cancelByProducingResumeData:^(NSData * _Nullable resumeData) {
        if (resumeData != nil) {
            self.resumeData = resumeData;
            if (self->_params.resumableCallback) {
                self->_params.resumableCallback();
            }
        } else {
            NSError *error = [NSError errorWithDomain:@"RNFS"
                                                 code:0 //used to pass an NSString @"Aborted" here, but it needs an NSInteger
                                             userInfo:@{
                                                        NSLocalizedDescriptionKey: @"Download has been aborted"
                                                        }];

            self->_params.errorCallback(error);
        }
    }];

  }
}

- (void)resumeDownload
{
    if (_resumeData != nil) {
        _task = [_session downloadTaskWithResumeData:_resumeData];
        [_task resume];
        _resumeData = nil;
    }
}

- (BOOL)isResumable
{
    return _resumeData != nil;
}

@end
