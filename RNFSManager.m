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

@implementation RNFSManager

static int MainBundleDirectory = 999;

RCT_EXPORT_MODULE();

- (dispatch_queue_t)methodQueue
{
  return dispatch_queue_create("pe.lum.rnfs", DISPATCH_QUEUE_SERIAL);
}

RCT_EXPORT_METHOD(readDir:(NSString *)directory
                  inFolder:(nonnull NSNumber *)folder
                  callback:(RCTResponseSenderBlock)callback)
{
  NSString *path;
  NSInteger folderInt = [folder integerValue];

  if (folderInt == MainBundleDirectory) {
    path = [[NSBundle mainBundle] bundlePath];
  } else {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(folderInt, NSUserDomainMask, YES);
    path = [paths firstObject];
  }

  NSFileManager *fileManager = [NSFileManager defaultManager];
  NSError *error = nil;
  NSString *dirPath = [path stringByAppendingPathComponent:directory];
  NSArray *contents = [fileManager contentsOfDirectoryAtPath:dirPath error:&error];

  contents = [contents rnfs_mapObjectsUsingBlock:^id(NSString *obj, NSUInteger idx) {
    return @{
      @"name": obj,
      @"path": [dirPath stringByAppendingPathComponent:obj]
    };
  }];

  if (error) {
    return callback([self makeErrorPayload:error]);
  }

  callback(@[[NSNull null], contents]);
}

RCT_EXPORT_METHOD(stat:(NSString *)filepath
                  callback:(RCTResponseSenderBlock)callback)
{
  NSError *error = nil;
  NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:filepath error:&error];

  if (error) {
    return callback([self makeErrorPayload:error]);
  }

  attributes = @{
    @"ctime": [self dateToTimeIntervalNumber:(NSDate *)[attributes objectForKey:NSFileCreationDate]],
    @"mtime": [self dateToTimeIntervalNumber:(NSDate *)[attributes objectForKey:NSFileModificationDate]],
    @"size": [attributes objectForKey:NSFileSize],
    @"type": [attributes objectForKey:NSFileType],
    @"mode": @([[NSString stringWithFormat:@"%ld", (long)[(NSNumber *)[attributes objectForKey:NSFilePosixPermissions] integerValue]] integerValue])
  };

  callback(@[[NSNull null], attributes]);
}

RCT_EXPORT_METHOD(writeFile:(NSString *)filepath
                  contents:(NSString *)base64Content
                  attributes:(NSDictionary *)attributes
                  callback:(RCTResponseSenderBlock)callback)
{
  NSData *data = [[NSData alloc] initWithBase64EncodedString:base64Content options:NSDataBase64DecodingIgnoreUnknownCharacters];
  BOOL success = [[NSFileManager defaultManager] createFileAtPath:filepath contents:data attributes:attributes];

  if (!success) {
    return callback(@[[NSString stringWithFormat:@"Could not write file at path %@", filepath]]);
  }

  callback(@[[NSNull null], [NSNumber numberWithBool:success]]);
}

RCT_EXPORT_METHOD(unlink:(NSString*)filepath
                  callback:(RCTResponseSenderBlock)callback)
{
  NSFileManager *manager = [NSFileManager defaultManager];
  BOOL exists = [manager fileExistsAtPath:filepath isDirectory:false];

  if (!exists) {
    return callback(@[[NSString stringWithFormat:@"File at path %@ does not exist", filepath]]);
  }
  NSError *error = nil;
  BOOL success = [manager removeItemAtPath:filepath error:&error];

  if (!success) {
    return callback([self makeErrorPayload:error]);
  }

  callback(@[[NSNull null], [NSNumber numberWithBool:success], filepath]);
}

RCT_EXPORT_METHOD(readFile:(NSString *)filepath
                  callback:(RCTResponseSenderBlock)callback)
{
  NSData *content = [[NSFileManager defaultManager] contentsAtPath:filepath];
  NSString *base64Content = [content base64EncodedStringWithOptions:NSDataBase64EncodingEndLineWithLineFeed];

  if (!base64Content) {
    return callback(@[[NSString stringWithFormat:@"Could not read file at path %@", filepath]]);
  }

  callback(@[[NSNull null], base64Content]);
}

RCT_EXPORT_METHOD(pathForBundle:(NSString *)bundleNamed
                  callback:(RCTResponseSenderBlock)callback)
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
        callback(@[[NSNull null], path]);
    } else {
        callback(@[[NSError errorWithDomain:NSPOSIXErrorDomain
                                       code:NSFileNoSuchFileError
                                   userInfo:nil].localizedDescription,
                   [NSNull null]]);
    }
}

- (NSNumber *)dateToTimeIntervalNumber:(NSDate *)date
{
  return @([date timeIntervalSince1970]);
}

- (NSArray *)makeErrorPayload:(NSError *)error
{
  return @[@{
    @"description": error.localizedDescription,
    @"code": @(error.code)
  }];
}

- (NSString *)getPathForDirectory:(int)directory
{
  NSArray *paths = NSSearchPathForDirectoriesInDomains(directory, NSUserDomainMask, YES);
  return [paths firstObject];
}

- (NSDictionary *)constantsToExport
{
  return @{
    @"NSCachesDirectoryPath": [self getPathForDirectory:NSCachesDirectory],
    @"NSDocumentDirectoryPath": [self getPathForDirectory:NSDocumentDirectory],
    @"NSCachesDirectory": [NSNumber numberWithInteger:NSCachesDirectory],
    @"NSDocumentDirectory": [NSNumber numberWithInteger:NSDocumentDirectory],
    @"MainBundleDirectory": [NSNumber numberWithInteger:MainBundleDirectory],
    @"NSFileTypeRegular": NSFileTypeRegular,
    @"NSFileTypeDirectory": NSFileTypeDirectory
  };
}

@end
