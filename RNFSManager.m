//
//  RNFSManager.m
//  RNFSManager
//
//  Created by Johannes Lumpe on 08/05/15.
//  Copyright (c) 2015 Johannes Lumpe. All rights reserved.
//

#import "RNFSManager.h"
#import "RCTBridge.h"
#import "NSArray+Map.h"
#import "Downloader.h"
#import "Uploader.h"
#import "RCTEventDispatcher.h"

@interface RNFSManager()

@property (retain) NSMutableDictionary* downloaders;
@property (retain) NSMutableDictionary* uploaders;

@end

@implementation RNFSManager

@synthesize bridge = _bridge;

RCT_EXPORT_MODULE();

- (dispatch_queue_t)methodQueue
{
  return dispatch_queue_create("pe.lum.rnfs", DISPATCH_QUEUE_SERIAL);
}

RCT_EXPORT_METHOD(readDir:(NSString *)dirPath
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSFileManager *fileManager = [NSFileManager defaultManager];
  NSError *error = nil;

  NSArray *contents = [fileManager contentsOfDirectoryAtPath:dirPath error:&error];

  contents = [contents rnfs_mapObjectsUsingBlock:^id(NSString *obj, NSUInteger idx) {
    NSString *path = [dirPath stringByAppendingPathComponent:obj];
    NSDictionary *attributes = [fileManager attributesOfItemAtPath:path error:nil];

    return @{
             @"name": obj,
             @"path": path,
             @"size": [attributes objectForKey:NSFileSize],
             @"type": [attributes objectForKey:NSFileType]
             };
  }];

  if (error) {
    return [self reject:reject withError:error];
  }

  resolve(contents);
}

RCT_EXPORT_METHOD(exists:(NSString *)filepath
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(__unused RCTPromiseRejectBlock)reject)
{
  BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:filepath];

  resolve([NSNumber numberWithBool:fileExists]);
}

RCT_EXPORT_METHOD(stat:(NSString *)filepath
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSError *error = nil;
  NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:filepath error:&error];

  if (error) {
    return [self reject:reject withError:error];
  }

  attributes = @{
                 @"ctime": [self dateToTimeIntervalNumber:(NSDate *)[attributes objectForKey:NSFileCreationDate]],
                 @"mtime": [self dateToTimeIntervalNumber:(NSDate *)[attributes objectForKey:NSFileModificationDate]],
                 @"size": [attributes objectForKey:NSFileSize],
                 @"type": [attributes objectForKey:NSFileType],
                 @"mode": @([[NSString stringWithFormat:@"%ld", (long)[(NSNumber *)[attributes objectForKey:NSFilePosixPermissions] integerValue]] integerValue])
                 };

  resolve(attributes);
}

RCT_EXPORT_METHOD(writeFile:(NSString *)filepath
                  contents:(NSString *)base64Content
                  attributes:(NSDictionary *)attributes
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSData *data = [[NSData alloc] initWithBase64EncodedString:base64Content options:NSDataBase64DecodingIgnoreUnknownCharacters];

  BOOL success = [[NSFileManager defaultManager] createFileAtPath:filepath contents:data attributes:attributes];

  if (!success) {
    return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no such file or directory, open '%@'", filepath], nil);
  }

  return resolve(nil);
}

RCT_EXPORT_METHOD(appendFile:(NSString *)filepath
                  contents:(NSString *)base64Content
                  attributes:(NSDictionary *)attributes
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSData *data = [[NSData alloc] initWithBase64EncodedString:base64Content options:NSDataBase64DecodingIgnoreUnknownCharacters];

  NSFileManager *fM = [NSFileManager defaultManager];

  if (![fM fileExistsAtPath:filepath])
  {
    BOOL success = [[NSFileManager defaultManager] createFileAtPath:filepath contents:data attributes:attributes];

    if (!success) {
      return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no such file or directory, open '%@'", filepath], nil);
    } else {
      return resolve(nil);
    }
  }

  @try {
    NSFileHandle *fH = [NSFileHandle fileHandleForUpdatingAtPath:filepath];

    [fH seekToEndOfFile];
    [fH writeData:data];

    return resolve(nil);
  } @catch (NSException *e) {
    return [self reject:reject withError:e];
  }
}

RCT_EXPORT_METHOD(unlink:(NSString*)filepath
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSFileManager *manager = [NSFileManager defaultManager];
  BOOL exists = [manager fileExistsAtPath:filepath isDirectory:false];

  if (!exists) {
    return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no such file or directory, open '%@'", filepath], nil);
  }

  NSError *error = nil;
  BOOL success = [manager removeItemAtPath:filepath error:&error];

  if (!success) {
    return [self reject:reject withError:error];
  }

  resolve(nil);
}

RCT_EXPORT_METHOD(mkdir:(NSString*)filepath
                  excludeFromBackup:(BOOL)excludeFromBackup
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSFileManager *manager = [NSFileManager defaultManager];

  NSError *error = nil;
  BOOL success = [manager createDirectoryAtPath:filepath withIntermediateDirectories:YES attributes:nil error:&error];

  if (!success) {
    return [self reject:reject withError:error];
  }

  NSURL *url = [NSURL fileURLWithPath:filepath];

  success = [url setResourceValue: [NSNumber numberWithBool: excludeFromBackup] forKey: NSURLIsExcludedFromBackupKey error: &error];

  if (!success) {
    return [self reject:reject withError:error];
  }

  resolve(nil);
}

RCT_EXPORT_METHOD(readFile:(NSString *)filepath
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSData *content = [[NSFileManager defaultManager] contentsAtPath:filepath];
  NSString *base64Content = [content base64EncodedStringWithOptions:NSDataBase64EncodingEndLineWithLineFeed];

  if (!base64Content) {
    return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no such file or directory, open '%@'", filepath], nil);
  }

  resolve(base64Content);
}

RCT_EXPORT_METHOD(moveFile:(NSString *)filepath
                  destPath:(NSString *)destPath
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSFileManager *manager = [NSFileManager defaultManager];

  NSError *error = nil;
  BOOL success = [manager moveItemAtPath:filepath toPath:destPath error:&error];

  if (!success) {
    return [self reject:reject withError:error];
  }

  resolve(nil);
}

RCT_EXPORT_METHOD(downloadFile:(NSDictionary *)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  DownloadParams* params = [DownloadParams alloc];

  NSNumber* jobId = options[@"jobId"];
  params.fromUrl = options[@"fromUrl"];
  params.toFile = options[@"toFile"];
  NSDictionary* headers = options[@"headers"];
  params.headers = headers;
  NSNumber* background = options[@"background"];
  params.background = [background boolValue];
  NSNumber* progressDivider = options[@"progressDivider"];
  params.progressDivider = progressDivider;

  params.completeCallback = ^(NSNumber* statusCode, NSNumber* bytesWritten) {
    NSMutableDictionary* result = [[NSMutableDictionary alloc] initWithDictionary: @{@"jobId": jobId,
                                                                                     @"statusCode": statusCode}];
    if (bytesWritten) {
      [result setObject:bytesWritten forKey: @"bytesWritten"];
    }
    return resolve(result);
  };

  params.errorCallback = ^(NSError* error) {
    return [self reject:reject withError:error];
  };

  params.beginCallback = ^(NSNumber* statusCode, NSNumber* contentLength, NSDictionary* headers) {
    [self.bridge.eventDispatcher sendAppEventWithName:[NSString stringWithFormat:@"DownloadBegin-%@", jobId]
                                                 body:@{@"jobId": jobId,
                                                        @"statusCode": statusCode,
                                                        @"contentLength": contentLength,
                                                        @"headers": headers}];
  };

  params.progressCallback = ^(NSNumber* contentLength, NSNumber* bytesWritten) {
    [self.bridge.eventDispatcher sendAppEventWithName:[NSString stringWithFormat:@"DownloadProgress-%@", jobId]
                                                 body:@{@"jobId": jobId,
                                                        @"contentLength": contentLength,
                                                        @"bytesWritten": bytesWritten}];
  };

  if (!self.downloaders) self.downloaders = [[NSMutableDictionary alloc] init];

  Downloader* downloader = [Downloader alloc];

  [downloader downloadFile:params];

  [self.downloaders setValue:downloader forKey:[jobId stringValue]];
}

RCT_EXPORT_METHOD(stopDownload:(nonnull NSNumber *)jobId)
{
  Downloader* downloader = [self.downloaders objectForKey:[jobId stringValue]];

  if (downloader != nil) {
    [downloader stopDownload];
  }
}

RCT_EXPORT_METHOD(uploadFiles:(NSDictionary *)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  UploadParams* params = [UploadParams alloc];

  NSNumber* jobId = options[@"jobId"];
  params.toUrl = options[@"toUrl"];
  params.files = options[@"files"];
  NSDictionary* headers = options[@"headers"];
  NSDictionary* fields = options[@"fields"];
  NSString* method = options[@"method"];
  params.headers = headers;
  params.fields = fields;
  params.method = method;

  params.completeCallback = ^(NSString* body, NSURLResponse *resp) {

    NSMutableDictionary* result = [[NSMutableDictionary alloc] initWithDictionary: @{@"jobId": jobId,
                                                                                     @"body": body,
                                                                                     @"response": body}];
    if ([resp isKindOfClass:[NSHTTPURLResponse class]]) {
      [result setValue:((NSHTTPURLResponse *)resp).allHeaderFields forKey:@"headers"];
      [result setValue:[NSNumber numberWithUnsignedInteger:((NSHTTPURLResponse *)resp).statusCode] forKey:@"statusCode"];
    }
    return resolve(result);
  };

  params.errorCallback = ^(NSError* error) {
    return [self reject:reject withError:error];
  };

  params.beginCallback = ^() {
    [self.bridge.eventDispatcher sendAppEventWithName:[NSString stringWithFormat:@"UploadBegin-%@", jobId]
                                                 body:@{@"jobId": jobId}];
  };

  params.progressCallback = ^(NSNumber* totalBytesExpectedToSend, NSNumber* totalBytesSent) {
    [self.bridge.eventDispatcher sendAppEventWithName:[NSString stringWithFormat:@"UploadProgress-%@", jobId]
                                                 body:@{@"jobId": jobId,
                                                        @"totalBytesExpectedToSend": totalBytesExpectedToSend,
                                                        @"totalBytesSent": totalBytesSent}];
  };

  if (!self.uploaders) self.uploaders = [[NSMutableDictionary alloc] init];

  Uploader* uploader = [Uploader alloc];

  [uploader uploadFiles:params];

  [self.uploaders setValue:uploader forKey:[jobId stringValue]];
}

RCT_EXPORT_METHOD(stopUpload:(nonnull NSNumber *)jobId)
{
  Uploader* uploader = [self.uploaders objectForKey:[jobId stringValue]];

  if (uploader != nil) {
    [uploader stopUpload];
  }
}

RCT_EXPORT_METHOD(pathForBundle:(NSString *)bundleNamed
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSString *path = [[NSBundle mainBundle].bundlePath stringByAppendingFormat:@"/%@.bundle", bundleNamed];
  NSBundle *bundle = [NSBundle bundleWithPath:path];

  if (!bundle) {
    bundle = [NSBundle bundleForClass:NSClassFromString(bundleNamed)];
    path = bundle.bundlePath;
  }

  if (!bundle.isLoaded) {
    [bundle load];
  }

  if (path) {
    resolve(path);
  } else {
    NSError *error = [NSError errorWithDomain:NSPOSIXErrorDomain
                                         code:NSFileNoSuchFileError
                                     userInfo:nil];

    [self reject:reject withError:error];
  }
}

RCT_EXPORT_METHOD(getFSInfo:(RCTPromiseResolveBlock)resolve rejecter:(RCTPromiseRejectBlock)reject)
{
  unsigned long long totalSpace = 0;
  unsigned long long totalFreeSpace = 0;

  __autoreleasing NSError *error = nil;
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSDictionary *dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:[paths lastObject] error:&error];

  if (dictionary) {
    NSNumber *fileSystemSizeInBytes = [dictionary objectForKey: NSFileSystemSize];
    NSNumber *freeFileSystemSizeInBytes = [dictionary objectForKey:NSFileSystemFreeSize];
    totalSpace = [fileSystemSizeInBytes unsignedLongLongValue];
    totalFreeSpace = [freeFileSystemSizeInBytes unsignedLongLongValue];

    resolve(@{
      @"totalSpace": [NSNumber numberWithUnsignedLongLong:totalSpace],
      @"freeSpace": [NSNumber numberWithUnsignedLongLong:totalFreeSpace]
    });
  } else {
    [self reject:reject withError:error];
  }
}

- (NSNumber *)dateToTimeIntervalNumber:(NSDate *)date
{
  return @([date timeIntervalSince1970]);
}

- (void)reject:(RCTPromiseRejectBlock)reject withError:(NSError *)error
{
  NSString *codeWithDomain = [NSString stringWithFormat:@"E%@%zd", error.domain.uppercaseString, error.code];
  reject(codeWithDomain, error.localizedDescription, error);
}

- (NSString *)getPathForDirectory:(int)directory
{
  NSArray *paths = NSSearchPathForDirectoriesInDomains(directory, NSUserDomainMask, YES);
  return [paths firstObject];
}

- (NSDictionary *)constantsToExport
{
  return @{
           @"MainBundlePath": [[NSBundle mainBundle] bundlePath],
           @"NSCachesDirectoryPath": [self getPathForDirectory:NSCachesDirectory],
           @"NSDocumentDirectoryPath": [self getPathForDirectory:NSDocumentDirectory],
           @"NSExternalDirectoryPath": [NSNull null],
           @"NSTemporaryDirectoryPath": NSTemporaryDirectory(),
           @"NSLibraryDirectoryPath": [self getPathForDirectory:NSLibraryDirectory],
           @"NSFileTypeRegular": NSFileTypeRegular,
           @"NSFileTypeDirectory": NSFileTypeDirectory
           };
}

@end
