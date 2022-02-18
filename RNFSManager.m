//
//  RNFSManager.m
//  RNFSManager
//
//  Created by Johannes Lumpe on 08/05/15.
//  Copyright (c) 2015 Johannes Lumpe. All rights reserved.
//

#import "RNFSManager.h"

#import "NSArray+Map.h"
#import "Downloader.h"
#import "Uploader.h"

#import <React/RCTEventDispatcher.h>
#import <React/RCTUtils.h>

#if __has_include(<React/RCTImageLoader.h>)
#import <React/RCTImageLoader.h>
#else
#import <React/RCTImageLoaderProtocol.h>
#endif

#import <CommonCrypto/CommonDigest.h>
#import <Photos/Photos.h>


@interface RNFSManager()

@property (retain) NSMutableDictionary* downloaders;
@property (retain) NSMutableDictionary* uuids;
@property (retain) NSMutableDictionary* uploaders;

@end

@implementation RNFSManager

static NSMutableDictionary *completionHandlers;

RCT_EXPORT_MODULE();

- (dispatch_queue_t)methodQueue
{
  return dispatch_queue_create("pe.lum.rnfs", DISPATCH_QUEUE_SERIAL);
}

+ (BOOL)requiresMainQueueSetup
{
  return NO;
}

RCT_EXPORT_METHOD(readDir:(NSString *)dirPath
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSFileManager *fileManager = [NSFileManager defaultManager];
  NSError *error = nil;

  NSArray *contents = [fileManager contentsOfDirectoryAtPath:dirPath error:&error];
  NSMutableArray *tagetContents = [[NSMutableArray alloc] init];
  for (NSString *obj in contents) {
    NSString *path = [dirPath stringByAppendingPathComponent:obj];
    NSDictionary *attributes = [fileManager attributesOfItemAtPath:path error:nil];
    if(attributes != nil) {
        [tagetContents addObject:@{
            @"ctime": [self dateToTimeIntervalNumber:(NSDate *)[attributes objectForKey:NSFileCreationDate]],
            @"mtime": [self dateToTimeIntervalNumber:(NSDate *)[attributes objectForKey:NSFileModificationDate]],
            @"name": obj,
            @"path": path,
            @"size": [attributes objectForKey:NSFileSize],
            @"type": [attributes objectForKey:NSFileType]
            }];
    }
  }

  if (error) {
    return [self reject:reject withError:error];
  }

  resolve(tagetContents);
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
                  options:(NSDictionary *)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSData *data = [[NSData alloc] initWithBase64EncodedString:base64Content options:NSDataBase64DecodingIgnoreUnknownCharacters];

  NSMutableDictionary *attributes = [[NSMutableDictionary alloc] init];

  if ([options objectForKey:@"NSFileProtectionKey"]) {
    [attributes setValue:[options objectForKey:@"NSFileProtectionKey"] forKey:@"NSFileProtectionKey"];
  }

  BOOL success = [[NSFileManager defaultManager] createFileAtPath:filepath contents:data attributes:attributes];

  if (!success) {
    return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no such file or directory, open '%@'", filepath], nil);
  }

  return resolve(nil);
}

RCT_EXPORT_METHOD(appendFile:(NSString *)filepath
                  contents:(NSString *)base64Content
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSData *data = [[NSData alloc] initWithBase64EncodedString:base64Content options:NSDataBase64DecodingIgnoreUnknownCharacters];

  NSFileManager *fM = [NSFileManager defaultManager];

  if (![fM fileExistsAtPath:filepath])
  {
    BOOL success = [[NSFileManager defaultManager] createFileAtPath:filepath contents:data attributes:nil];

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
  } @catch (NSException *exception) {
    NSMutableDictionary * info = [NSMutableDictionary dictionary];
    [info setValue:exception.name forKey:@"ExceptionName"];
    [info setValue:exception.reason forKey:@"ExceptionReason"];
    [info setValue:exception.callStackReturnAddresses forKey:@"ExceptionCallStackReturnAddresses"];
    [info setValue:exception.callStackSymbols forKey:@"ExceptionCallStackSymbols"];
    [info setValue:exception.userInfo forKey:@"ExceptionUserInfo"];
    NSError *err = [NSError errorWithDomain:@"RNFS" code:0 userInfo:info];
    return [self reject:reject withError:err];
  }
}

RCT_EXPORT_METHOD(write:(NSString *)filepath
                  contents:(NSString *)base64Content
                  position:(NSInteger)position
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSData *data = [[NSData alloc] initWithBase64EncodedString:base64Content options:NSDataBase64DecodingIgnoreUnknownCharacters];

  NSFileManager *fM = [NSFileManager defaultManager];

  if (![fM fileExistsAtPath:filepath])
  {
    BOOL success = [[NSFileManager defaultManager] createFileAtPath:filepath contents:data attributes:nil];

    if (!success) {
      return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no such file or directory, open '%@'", filepath], nil);
    } else {
      return resolve(nil);
    }
  }

  @try {
    NSFileHandle *fH = [NSFileHandle fileHandleForUpdatingAtPath:filepath];

    if (position >= 0) {
      [fH seekToFileOffset:position];
    } else {
      [fH seekToEndOfFile];
    }
    [fH writeData:data];

    return resolve(nil);
  } @catch (NSException *e) {
    return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: error writing file: '%@'", filepath], nil);
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

RCT_EXPORT_METHOD(mkdir:(NSString *)filepath
                  options:(NSDictionary *)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSFileManager *manager = [NSFileManager defaultManager];

  NSMutableDictionary *attributes = [[NSMutableDictionary alloc] init];

  if ([options objectForKey:@"NSFileProtectionKey"]) {
      [attributes setValue:[options objectForKey:@"NSFileProtectionKey"] forKey:@"NSFileProtectionKey"];
  }

  NSError *error = nil;
    BOOL success = [manager createDirectoryAtPath:filepath withIntermediateDirectories:YES attributes:attributes error:&error];

  if (!success) {
    return [self reject:reject withError:error];
  }

  NSURL *url = [NSURL fileURLWithPath:filepath];

  if ([[options allKeys] containsObject:@"NSURLIsExcludedFromBackupKey"]) {
    NSNumber *value = options[@"NSURLIsExcludedFromBackupKey"];
    success = [url setResourceValue: value forKey: NSURLIsExcludedFromBackupKey error: &error];

    if (!success) {
      return [self reject:reject withError:error];
    }
  }

  resolve(nil);
}

RCT_EXPORT_METHOD(readFile:(NSString *)filepath
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:filepath];

  if (!fileExists) {
    return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no such file or directory, open '%@'", filepath], nil);
  }

  NSError *error = nil;

  NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:filepath error:&error];

  if (error) {
    return [self reject:reject withError:error];
  }

  if ([attributes objectForKey:NSFileType] == NSFileTypeDirectory) {
    return reject(@"EISDIR", @"EISDIR: illegal operation on a directory, read", nil);
  }

  NSData *content = [[NSFileManager defaultManager] contentsAtPath:filepath];
  NSString *base64Content = [content base64EncodedStringWithOptions:NSDataBase64EncodingEndLineWithLineFeed];

  resolve(base64Content);
}

RCT_EXPORT_METHOD(read:(NSString *)filepath
                  length: (NSInteger *)length
                  position: (NSInteger *)position
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:filepath];

    if (!fileExists) {
        return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no such file or directory, open '%@'", filepath], nil);
    }

    NSError *error = nil;

    NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:filepath error:&error];

    if (error) {
        return [self reject:reject withError:error];
    }

    if ([attributes objectForKey:NSFileType] == NSFileTypeDirectory) {
        return reject(@"EISDIR", @"EISDIR: illegal operation on a directory, read", nil);
    }

    // Open the file handler.
    NSFileHandle *file = [NSFileHandle fileHandleForReadingAtPath:filepath];
    if (file == nil) {
        return reject(@"EISDIR", @"EISDIR: Could not open file for reading", nil);
    }

    // Seek to the position if there is one.
    [file seekToFileOffset: (int)position];

    NSData *content;
    if ((int)length > 0) {
        content = [file readDataOfLength: (int)length];
    } else {
        content = [file readDataToEndOfFile];
    }

    NSString *base64Content = [content base64EncodedStringWithOptions:NSDataBase64EncodingEndLineWithLineFeed];

    resolve(base64Content);
}

RCT_EXPORT_METHOD(hash:(NSString *)filepath
                  algorithm:(NSString *)algorithm
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:filepath];

  if (!fileExists) {
    return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no such file or directory, open '%@'", filepath], nil);
  }

  NSError *error = nil;

  NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:filepath error:&error];

  if (error) {
    return [self reject:reject withError:error];
  }

  if ([attributes objectForKey:NSFileType] == NSFileTypeDirectory) {
    return reject(@"EISDIR", @"EISDIR: illegal operation on a directory, read", nil);
  }

  NSData *content = [[NSFileManager defaultManager] contentsAtPath:filepath];

  NSArray *keys = [NSArray arrayWithObjects:@"md5", @"sha1", @"sha224", @"sha256", @"sha384", @"sha512", nil];

  NSArray *digestLengths = [NSArray arrayWithObjects:
    @CC_MD5_DIGEST_LENGTH,
    @CC_SHA1_DIGEST_LENGTH,
    @CC_SHA224_DIGEST_LENGTH,
    @CC_SHA256_DIGEST_LENGTH,
    @CC_SHA384_DIGEST_LENGTH,
    @CC_SHA512_DIGEST_LENGTH,
    nil];

  NSDictionary *keysToDigestLengths = [NSDictionary dictionaryWithObjects:digestLengths forKeys:keys];

  int digestLength = [[keysToDigestLengths objectForKey:algorithm] intValue];

  if (!digestLength) {
    return reject(@"Error", [NSString stringWithFormat:@"Invalid hash algorithm '%@'", algorithm], nil);
  }

  unsigned char buffer[digestLength];

  if ([algorithm isEqualToString:@"md5"]) {
    CC_MD5(content.bytes, (CC_LONG)content.length, buffer);
  } else if ([algorithm isEqualToString:@"sha1"]) {
    CC_SHA1(content.bytes, (CC_LONG)content.length, buffer);
  } else if ([algorithm isEqualToString:@"sha224"]) {
    CC_SHA224(content.bytes, (CC_LONG)content.length, buffer);
  } else if ([algorithm isEqualToString:@"sha256"]) {
    CC_SHA256(content.bytes, (CC_LONG)content.length, buffer);
  } else if ([algorithm isEqualToString:@"sha384"]) {
    CC_SHA384(content.bytes, (CC_LONG)content.length, buffer);
  } else if ([algorithm isEqualToString:@"sha512"]) {
    CC_SHA512(content.bytes, (CC_LONG)content.length, buffer);
  } else {
    return reject(@"Error", [NSString stringWithFormat:@"Invalid hash algorithm '%@'", algorithm], nil);
  }

  NSMutableString *output = [NSMutableString stringWithCapacity:digestLength * 2];
  for(int i = 0; i < digestLength; i++)
    [output appendFormat:@"%02x",buffer[i]];

  resolve(output);
}

RCT_EXPORT_METHOD(moveFile:(NSString *)filepath
                  destPath:(NSString *)destPath
                  options:(NSDictionary *)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSFileManager *manager = [NSFileManager defaultManager];

  NSError *error = nil;
  BOOL success = [manager moveItemAtPath:filepath toPath:destPath error:&error];

  if (!success) {
    return [self reject:reject withError:error];
  }

  if ([options objectForKey:@"NSFileProtectionKey"]) {
    NSMutableDictionary *attributes = [[NSMutableDictionary alloc] init];
    [attributes setValue:[options objectForKey:@"NSFileProtectionKey"] forKey:@"NSFileProtectionKey"];
    BOOL updateSuccess = [manager setAttributes:attributes ofItemAtPath:destPath error:&error];

    if (!updateSuccess) {
      return [self reject:reject withError:error];
    }
  }

  resolve(nil);
}

RCT_EXPORT_METHOD(copyFile:(NSString *)filepath
                  destPath:(NSString *)destPath
                  options:(NSDictionary *)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSFileManager *manager = [NSFileManager defaultManager];

  NSError *error = nil;
  BOOL success = [manager copyItemAtPath:filepath toPath:destPath error:&error];

  if (!success) {
    return [self reject:reject withError:error];
  }

  if ([options objectForKey:@"NSFileProtectionKey"]) {
    NSMutableDictionary *attributes = [[NSMutableDictionary alloc] init];
    [attributes setValue:[options objectForKey:@"NSFileProtectionKey"] forKey:@"NSFileProtectionKey"];
    BOOL updateSuccess = [manager setAttributes:attributes ofItemAtPath:destPath error:&error];

    if (!updateSuccess) {
      return [self reject:reject withError:error];
    }
  }

  resolve(nil);
}

- (NSArray<NSString *> *)supportedEvents
{
    return @[@"UploadBegin",@"UploadProgress",@"DownloadBegin",@"DownloadProgress",@"DownloadResumable"];
}

RCT_EXPORT_METHOD(downloadFile:(NSDictionary *)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  RNFSDownloadParams* params = [RNFSDownloadParams alloc];

  NSNumber* jobId = options[@"jobId"];
  params.fromUrl = options[@"fromUrl"];
  params.toFile = options[@"toFile"];
  NSDictionary* headers = options[@"headers"];
  params.headers = headers;
  NSNumber* background = options[@"background"];
  params.background = [background boolValue];
  NSNumber* discretionary = options[@"discretionary"];
  params.discretionary = [discretionary boolValue];
  NSNumber* cacheable = options[@"cacheable"];
  params.cacheable = cacheable ? [cacheable boolValue] : YES;
  NSNumber* progressInterval= options[@"progressInterval"];
  params.progressInterval = progressInterval;
  NSNumber* progressDivider = options[@"progressDivider"];
  params.progressDivider = progressDivider;
  NSNumber* readTimeout = options[@"readTimeout"];
  params.readTimeout = readTimeout;
  NSNumber* backgroundTimeout = options[@"backgroundTimeout"];
  params.backgroundTimeout = backgroundTimeout;
  bool hasBeginCallback = [options[@"hasBeginCallback"] boolValue];
  bool hasProgressCallback = [options[@"hasProgressCallback"] boolValue];
  bool hasResumableCallback = [options[@"hasResumableCallback"] boolValue];

  __block BOOL callbackFired = NO;

  params.completeCallback = ^(NSNumber* statusCode, NSNumber* bytesWritten) {
    if (callbackFired) {
      return;
    }
    callbackFired = YES;

    NSMutableDictionary* result = [[NSMutableDictionary alloc] initWithDictionary: @{@"jobId": jobId}];
    if (statusCode) {
      [result setObject:statusCode forKey: @"statusCode"];
    }
    if (bytesWritten) {
      [result setObject:bytesWritten forKey: @"bytesWritten"];
    }
    return resolve(result);
  };

  params.errorCallback = ^(NSError* error) {
    if (callbackFired) {
      return;
    }
    callbackFired = YES;
    return [self reject:reject withError:error];
  };

  if (hasBeginCallback) {
    params.beginCallback = ^(NSNumber* statusCode, NSNumber* contentLength, NSDictionary* headers) {
        if (self.bridge != nil)
            [self sendEventWithName:@"DownloadBegin" body:@{@"jobId": jobId,
                                                                                            @"statusCode": statusCode,
                                                                                            @"contentLength": contentLength,
                                                                                            @"headers": headers ?: [NSNull null]}];
    };
  }

  if (hasProgressCallback) {
    params.progressCallback = ^(NSNumber* contentLength, NSNumber* bytesWritten) {
        if (self.bridge != nil)
          [self sendEventWithName:@"DownloadProgress"
                                                  body:@{@"jobId": jobId,
                                                          @"contentLength": contentLength,
                                                          @"bytesWritten": bytesWritten}];
    };
  }

  if (hasResumableCallback) {
    params.resumableCallback = ^() {
        if (self.bridge != nil)
            [self sendEventWithName:@"DownloadResumable" body:@{@"jobId": jobId}];
    };
  }

  if (!self.downloaders) self.downloaders = [[NSMutableDictionary alloc] init];

  RNFSDownloader* downloader = [RNFSDownloader alloc];

  NSString *uuid = [downloader downloadFile:params];

  [self.downloaders setValue:downloader forKey:[jobId stringValue]];
    if (uuid) {
        if (!self.uuids) self.uuids = [[NSMutableDictionary alloc] init];
        [self.uuids setValue:uuid forKey:[jobId stringValue]];
    }
}

RCT_EXPORT_METHOD(stopDownload:(nonnull NSNumber *)jobId)
{
  RNFSDownloader* downloader = [self.downloaders objectForKey:[jobId stringValue]];

  if (downloader != nil) {
    [downloader stopDownload];
  }
}

RCT_EXPORT_METHOD(resumeDownload:(nonnull NSNumber *)jobId)
{
    RNFSDownloader* downloader = [self.downloaders objectForKey:[jobId stringValue]];

    if (downloader != nil) {
        [downloader resumeDownload];
    }
}

RCT_EXPORT_METHOD(isResumable:(nonnull NSNumber *)jobId
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject
)
{
    RNFSDownloader* downloader = [self.downloaders objectForKey:[jobId stringValue]];

    if (downloader != nil) {
        resolve([NSNumber numberWithBool:[downloader isResumable]]);
    } else {
        resolve([NSNumber numberWithBool:NO]);
    }
}

RCT_EXPORT_METHOD(completeHandlerIOS:(nonnull NSNumber *)jobId
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    if (self.uuids) {
        NSString *uuid = [self.uuids objectForKey:[jobId stringValue]];
        CompletionHandler completionHandler = [completionHandlers objectForKey:uuid];
        if (completionHandler) {
            completionHandler();
            [completionHandlers removeObjectForKey:uuid];
        }
    }
    resolve(nil);
}

RCT_EXPORT_METHOD(uploadFiles:(NSDictionary *)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  RNFSUploadParams* params = [RNFSUploadParams alloc];

  NSNumber* jobId = options[@"jobId"];
  params.toUrl = options[@"toUrl"];
  params.files = options[@"files"];
  params.binaryStreamOnly = [[options objectForKey:@"binaryStreamOnly"] boolValue];
  NSDictionary* headers = options[@"headers"];
  NSDictionary* fields = options[@"fields"];
  NSString* method = options[@"method"];
  params.headers = headers;
  params.fields = fields;
  params.method = method;
  bool hasBeginCallback = [options[@"hasBeginCallback"] boolValue];
  bool hasProgressCallback = [options[@"hasProgressCallback"] boolValue];

  params.completeCallback = ^(NSString* body, NSURLResponse *resp) {
    [self.uploaders removeObjectForKey:[jobId stringValue]];

    NSMutableDictionary* result = [[NSMutableDictionary alloc] initWithDictionary: @{@"jobId": jobId,
                                                                                     @"body": body}];
    if ([resp isKindOfClass:[NSHTTPURLResponse class]]) {
      [result setValue:((NSHTTPURLResponse *)resp).allHeaderFields forKey:@"headers"];
      [result setValue:[NSNumber numberWithUnsignedInteger:((NSHTTPURLResponse *)resp).statusCode] forKey:@"statusCode"];
    }
    return resolve(result);
  };

  params.errorCallback = ^(NSError* error) {
    [self.uploaders removeObjectForKey:[jobId stringValue]];
    return [self reject:reject withError:error];
  };

  if (hasBeginCallback) {
    params.beginCallback = ^() {
        if (self.bridge != nil)
          [self sendEventWithName:@"UploadBegin"
                                                  body:@{@"jobId": jobId}];
    };
  }

  if (hasProgressCallback) {
    params.progressCallback = ^(NSNumber* totalBytesExpectedToSend, NSNumber* totalBytesSent) {
        if (self.bridge != nil)
            [self sendEventWithName:@"UploadProgress"
                                                  body:@{@"jobId": jobId,
                                                          @"totalBytesExpectedToSend": totalBytesExpectedToSend,
                                                          @"totalBytesSent": totalBytesSent}];
    };
  }

  if (!self.uploaders) self.uploaders = [[NSMutableDictionary alloc] init];

  RNFSUploader* uploader = [RNFSUploader alloc];

  [uploader uploadFiles:params];

  [self.uploaders setValue:uploader forKey:[jobId stringValue]];
}

RCT_EXPORT_METHOD(stopUpload:(nonnull NSNumber *)jobId)
{
  RNFSUploader* uploader = [self.uploaders objectForKey:[jobId stringValue]];

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

RCT_EXPORT_METHOD(pathForGroup:(nonnull NSString *)groupId
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  NSURL *groupURL = [[NSFileManager defaultManager]containerURLForSecurityApplicationGroupIdentifier: groupId];

  if (!groupURL) {
    return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no directory for group '%@' found", groupId], nil);
  } else {
    resolve([groupURL path]);
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


// [PHAsset fetchAssetsWithALAssetURLs] is deprecated and not supported in Mac Catalyst
#if !TARGET_OS_UIKITFORMAC && !TARGET_OS_OSX
/**
 * iOS Only: copy images from the assets-library (camera-roll) to a specific path, asuming
 * JPEG-Images.
 *
 * Video-Support:
 *
 * One can use this method also to create a thumbNail from a video.
 * Currently it is impossible to specify a concrete position, the OS will decide wich
 * Thumbnail you'll get then.
 * To copy a video from assets-library and save it as a mp4-file, use the method
 * copyAssetsVideoIOS.
 *
 * It is also supported to scale the image via scale-factor (0.0-1.0) or with a specific
 * width and height. Also the resizeMode will be considered.
 */
RCT_EXPORT_METHOD(copyAssetsFileIOS: (NSString *) imageUri
                  toFilepath: (NSString *) destination
                  width: (NSInteger) width
                  height: (NSInteger) height
                  scale: (CGFloat) scale
                  compression: (CGFloat) compression
                  resizeMode: (RCTResizeMode) resizeMode
                  resolver: (RCTPromiseResolveBlock) resolve
                  rejecter: (RCTPromiseRejectBlock) reject)

{
    CGSize size = CGSizeMake(width, height);

    NSURL* url = [NSURL URLWithString:imageUri];
    PHFetchResult *results = nil;
    if ([url.scheme isEqualToString:@"ph"]) {
        results = [PHAsset fetchAssetsWithLocalIdentifiers:@[[imageUri substringFromIndex: 5]] options:nil];
    } else {
        results = [PHAsset fetchAssetsWithALAssetURLs:@[url] options:nil];
    }

    if (results.count == 0) {
        NSString *errorText = [NSString stringWithFormat:@"Failed to fetch PHAsset with local identifier %@ with no error message.", imageUri];

        NSMutableDictionary* details = [NSMutableDictionary dictionary];
        [details setValue:errorText forKey:NSLocalizedDescriptionKey];
        NSError *error = [NSError errorWithDomain:@"RNFS" code:500 userInfo:details];
        [self reject: reject withError:error];
        return;
    }

    PHAsset *asset = [results firstObject];
    PHImageRequestOptions *imageOptions = [PHImageRequestOptions new];

    // Allow us to fetch images from iCloud
    imageOptions.networkAccessAllowed = YES;


    // Note: PhotoKit defaults to a deliveryMode of PHImageRequestOptionsDeliveryModeOpportunistic
    // which means it may call back multiple times - we probably don't want that
    imageOptions.deliveryMode = PHImageRequestOptionsDeliveryModeHighQualityFormat;

    BOOL useMaximumSize = CGSizeEqualToSize(size, CGSizeZero);
    CGSize targetSize;
    if (useMaximumSize) {
        targetSize = PHImageManagerMaximumSize;
        imageOptions.resizeMode = PHImageRequestOptionsResizeModeNone;
    } else {
        targetSize = CGSizeApplyAffineTransform(size, CGAffineTransformMakeScale(scale, scale));
        imageOptions.resizeMode = PHImageRequestOptionsResizeModeExact;
    }

    PHImageContentMode contentMode = PHImageContentModeAspectFill;
    if (resizeMode == RCTResizeModeContain) {
        contentMode = PHImageContentModeAspectFit;
    }

    // PHImageRequestID requestID =
    [[PHImageManager defaultManager] requestImageForAsset:asset
                                               targetSize:targetSize
                                              contentMode:contentMode
                                                  options:imageOptions
                                            resultHandler:^(UIImage *result, NSDictionary<NSString *, id> *info) {
        if (result) {

            NSData *imageData = UIImageJPEGRepresentation(result, compression );
            [imageData writeToFile:destination atomically:YES];
            resolve(destination);

        } else {
            NSMutableDictionary* details = [NSMutableDictionary dictionary];
            [details setValue:info[PHImageErrorKey] forKey:NSLocalizedDescriptionKey];
            NSError *error = [NSError errorWithDomain:@"RNFS" code:501 userInfo:details];
            [self reject: reject withError:error];

        }
    }];
}
#endif

// [PHAsset fetchAssetsWithALAssetURLs] is deprecated and not supported in Mac Catalyst
#if !TARGET_OS_UIKITFORMAC && !TARGET_OS_OSX
/**
 * iOS Only: copy videos from the assets-library (camera-roll) to a specific path as mp4-file.
 *
 * To create a thumbnail from the video, refer to copyAssetsFileIOS
 */
RCT_EXPORT_METHOD(copyAssetsVideoIOS: (NSString *) imageUri
                  atFilepath: (NSString *) destination
                  resolver: (RCTPromiseResolveBlock) resolve
                  rejecter: (RCTPromiseRejectBlock) reject)
{
  NSURL* url = [NSURL URLWithString:imageUri];
  //unused?
  //__block NSURL* videoURL = [NSURL URLWithString:destination];
  __block NSError *error = nil;

  PHFetchResult *phAssetFetchResult = nil;
  if ([url.scheme isEqualToString:@"ph"]) {
      phAssetFetchResult = [PHAsset fetchAssetsWithLocalIdentifiers:@[[imageUri substringFromIndex: 5]] options:nil];
  } else {
      phAssetFetchResult = [PHAsset fetchAssetsWithALAssetURLs:@[url] options:nil];
  }

  PHAsset *phAsset = [phAssetFetchResult firstObject];

  PHVideoRequestOptions *options = [[PHVideoRequestOptions alloc] init];
  options.networkAccessAllowed = YES;
  options.version = PHVideoRequestOptionsVersionOriginal;
  options.deliveryMode = PHVideoRequestOptionsDeliveryModeAutomatic;

  dispatch_group_t group = dispatch_group_create();
  dispatch_group_enter(group);

  [[PHImageManager defaultManager] requestAVAssetForVideo:phAsset options:options resultHandler:^(AVAsset *asset, AVAudioMix *audioMix, NSDictionary *info) {

    if ([asset isKindOfClass:[AVURLAsset class]]) {
      NSURL *url = [(AVURLAsset *)asset URL];
      NSLog(@"Final URL %@",url);
      BOOL writeResult = false;
        
      if (@available(iOS 9.0, *)) {
          NSURL *destinationUrl = [NSURL fileURLWithPath:destination relativeToURL:nil];
          writeResult = [[NSFileManager defaultManager] copyItemAtURL:url toURL:destinationUrl error:&error];
      } else {
          NSData *videoData = [NSData dataWithContentsOfURL:url];
          writeResult = [videoData writeToFile:destination options:NSDataWritingAtomic error:&error];
      }
        
      if(writeResult) {
        NSLog(@"video success");
      }
      else {
        NSLog(@"video failure");
      }
      dispatch_group_leave(group);
    }
  }];
  dispatch_group_wait(group,  DISPATCH_TIME_FOREVER);

  if (error) {
    NSLog(@"RNFS: %@", error);
    return [self reject:reject withError:error];
  }

  return resolve(destination);
}
#endif

RCT_EXPORT_METHOD(touch:(NSString*)filepath
                  mtime:(NSDate *)mtime
                  ctime:(NSDate *)ctime
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    NSFileManager *manager = [NSFileManager defaultManager];
    BOOL exists = [manager fileExistsAtPath:filepath isDirectory:false];

    if (!exists) {
        return reject(@"ENOENT", [NSString stringWithFormat:@"ENOENT: no such file, open '%@'", filepath], nil);
    }

    NSMutableDictionary *attr = [NSMutableDictionary dictionary];

    if (mtime) {
        [attr setValue:mtime forKey:NSFileModificationDate];
    }
    if (ctime) {
        [attr setValue:ctime forKey:NSFileCreationDate];
    }

    NSError *error = nil;
    BOOL success = [manager setAttributes:attr ofItemAtPath:filepath error:&error];

    if (!success) {
        return [self reject:reject withError:error];
    }

    resolve(nil);
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
           @"RNFSMainBundlePath": [[NSBundle mainBundle] bundlePath],
           @"RNFSCachesDirectoryPath": [self getPathForDirectory:NSCachesDirectory],
           @"RNFSDocumentDirectoryPath": [self getPathForDirectory:NSDocumentDirectory],
           @"RNFSExternalDirectoryPath": [NSNull null],
           @"RNFSExternalStorageDirectoryPath": [NSNull null],
           @"RNFSTemporaryDirectoryPath": NSTemporaryDirectory(),
           @"RNFSLibraryDirectoryPath": [self getPathForDirectory:NSLibraryDirectory],
           @"RNFSFileTypeRegular": NSFileTypeRegular,
           @"RNFSFileTypeDirectory": NSFileTypeDirectory,
           @"RNFSFileProtectionComplete": NSFileProtectionComplete,
           @"RNFSFileProtectionCompleteUnlessOpen": NSFileProtectionCompleteUnlessOpen,
           @"RNFSFileProtectionCompleteUntilFirstUserAuthentication": NSFileProtectionCompleteUntilFirstUserAuthentication,
           @"RNFSFileProtectionNone": NSFileProtectionNone
          };
}

+(void)setCompletionHandlerForIdentifier: (NSString *)identifier completionHandler: (CompletionHandler)completionHandler
{
    if (!completionHandlers) completionHandlers = [[NSMutableDictionary alloc] init];
    [completionHandlers setValue:completionHandler forKey:identifier];
}

@end
