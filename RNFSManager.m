//
//  RNFSManager.m
//  RNFSManager
//
//  Created by Johannes Lumpe on 08/05/15.
//  Copyright (c) 2015 Johannes Lumpe. All rights reserved.
//

#import "RNFSManager.h"
#import "RCTConvert.h"
#import "RCTBridge.h"
#import "NSArray+Map.h"

@implementation RNFSManager

static int MainBundleDirectory = 999;

@synthesize bridge = _bridge;
RCT_EXPORT_MODULE();


RCT_EXPORT_METHOD(readDir:(NSString*)directory inFolder:(NSNumber*)folder callback:(RCTResponseSenderBlock)callback){
  NSString *path;
  int folderInt = [folder integerValue];
  //NSLog(@"%d %d", folderInt, MainBundleDirectory);
  if (folderInt == MainBundleDirectory) {
    path = [[NSBundle mainBundle] bundlePath];
  } else {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(folderInt, NSUserDomainMask, YES);
    path = [paths objectAtIndex:0];
  }
  
  NSFileManager *fileManager = [NSFileManager defaultManager];
  NSError *error;
  NSString * dirPath = [path stringByAppendingPathComponent:directory];
  NSArray *contents = [fileManager contentsOfDirectoryAtPath:dirPath error:&error];
  
  contents = [contents mapObjectsUsingBlock:^id(id obj, NSUInteger idx) {
    return @{
      @"name": (NSString*)obj,
      @"path": [dirPath stringByAppendingPathComponent:(NSString*)obj]
    };
  }];
  
  if (error) {
    return callback([self makeErrorPayload:error]);
  }
  
  callback(@[[NSNull null], contents]);
}

RCT_EXPORT_METHOD(stat:(NSString*)filepath callback:(RCTResponseSenderBlock)callback){
  NSError *error;
  NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:filepath error:&error];
  //  NSLog(@"%@", attributes);
  
  if (error) {
    return callback([self makeErrorPayload:error]);
  }
  
  attributes = @{
                 @"ctime": [self dateToTimeIntervalNumber:(NSDate*)[attributes objectForKey:NSFileCreationDate]],
                 @"mtime": [self dateToTimeIntervalNumber:(NSDate*)[attributes objectForKey:NSFileModificationDate]],
                 @"size": [attributes objectForKey:NSFileSize],
                 @"type": [attributes objectForKey:NSFileType],
                 @"mode": [NSNumber numberWithInteger:[[NSString stringWithFormat:@"%o", [(NSNumber*)[attributes objectForKey:NSFilePosixPermissions] integerValue]] integerValue]]
                 };
  
  callback(@[[NSNull null], attributes]);
}

RCT_EXPORT_METHOD(readFile:(NSString*)filepath callback:(RCTResponseSenderBlock)callback){
  NSData *content = [[NSFileManager defaultManager] contentsAtPath:filepath];
  NSString *base64Content = [content base64EncodedStringWithOptions:NSDataBase64EncodingEndLineWithLineFeed];
  NSLog(@"%@", base64Content);
  if (!base64Content) {
    return callback(@[[NSString stringWithFormat:@"Could not read file at path %@", filepath]]);
  }
  
  callback(@[[NSNull null], base64Content]);
}

- (NSNumber*) dateToTimeIntervalNumber:(NSDate*)date {
  return [NSNumber numberWithDouble:[date timeIntervalSince1970]];
}

- (NSArray*) makeErrorPayload:(NSError*)error {
  return @[@{
    @"description": error.localizedDescription,
    @"code": [NSNumber numberWithInteger:error.code]
  }];
}

- (NSDictionary *)constantsToExport
{
  return @{
    @"NSCachesDirectory": [NSNumber numberWithInteger:NSCachesDirectory],
    @"NSDocumentDirectory": [NSNumber numberWithInteger:NSDocumentDirectory],
    @"MainBundleDirectory": [NSNumber numberWithInteger:MainBundleDirectory],
    @"NSFileTypeRegular": NSFileTypeRegular,
    @"NSFileTypeDirectory": NSFileTypeDirectory
  };
}

@end
