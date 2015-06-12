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

@synthesize bridge = _bridge;
RCT_EXPORT_MODULE();

- (dispatch_queue_t)methodQueue {
    return dispatch_queue_create("pe.lum.rnfs", DISPATCH_QUEUE_SERIAL);
}

RCT_EXPORT_METHOD(mkdir:(NSString*)path mode:(NSString*)mode callback:(RCTResponseSenderBlock)callback){
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    NSError *error;
    BOOL success = [fileManager createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:&error];
    if(!success){
        callback(@[[self makeErrorPayload:error]]);
    }
    
    callback(@[[NSNull null]]);
}

RCT_EXPORT_METHOD(readdir:(NSString*)directory callback:(RCTResponseSenderBlock)callback){
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error = nil;
    NSArray *contents = [fileManager contentsOfDirectoryAtPath:directory error:&error];
    
    if (error) {
        callback(@[[self makeErrorPayload:error]]);
    }else {
        callback(@[[NSNull null], contents]);
    }
}

RCT_EXPORT_METHOD(stat:(NSString*)filepath callback:(RCTResponseSenderBlock)callback){
    NSError *error = nil;
    NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:filepath error:&error];
    
    if (error) {
        callback(@[[self makeErrorPayload:error]]);
        return;
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

RCT_EXPORT_METHOD(writeFile:(NSString*)filepath contents:(NSString*)contents attributes:(NSDictionary*)attributes callback:(RCTResponseSenderBlock)callback){
    NSError *error;
    BOOL success =  [contents writeToFile:filepath atomically:NO encoding:NSUTF8StringEncoding error:&error];
    if (!success) {
        callback(@[[NSString stringWithFormat:@"Could not write file at path %@", filepath]]);
    }else {
        callback(@[[NSNull null]]);
    }
}

RCT_EXPORT_METHOD(unlink:(NSString*)filepath callback:(RCTResponseSenderBlock)callback) {
    NSFileManager *manager = [NSFileManager defaultManager];
    BOOL exists = [manager fileExistsAtPath:filepath isDirectory:false];
    
    if (!exists) {
        return callback(@[[NSString stringWithFormat:@"File at path %@ does not exist", filepath]]);
    }
    NSError *error = nil;
    BOOL success = [manager removeItemAtPath:filepath error:&error];
    
    if (!success) {
        callback(@[[self makeErrorPayload:error]]);
    }else {
        callback(@[[NSNull null]]);
    }
}

RCT_EXPORT_METHOD(readFile:(NSString*)filepath options:(NSDictionary*)options callback:(RCTResponseSenderBlock)callback){
    NSError *error = nil;
    NSString *utf8String =  [[NSString alloc]
                             initWithContentsOfFile:filepath encoding:NSUTF8StringEncoding error:&error];
    
    if (error) {
        callback(@[[self makeErrorPayload:error]]);
    }else {
        callback(@[[NSNull null], utf8String]);
    }
}


- (NSNumber*) dateToTimeIntervalNumber:(NSDate*)date {
    return [NSNumber numberWithDouble:[date timeIntervalSince1970]];
}

- (NSDictionary*) makeErrorPayload:(NSError*)error {
    return @{
             @"message": error.localizedDescription,
             @"code": [NSNumber numberWithInteger:error.code]
             };
    
}

- (NSString*) getPathForDirectory:(int)directory {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(directory, NSUserDomainMask, YES);
    return [paths objectAtIndex:0];
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
