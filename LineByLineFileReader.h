//
//  LineByLineFileReader.h
//
//  Copyright (c) 2012 Alex Moffat. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface LineByLineFileReader : NSObject

@property (nonatomic, strong) NSInputStream *inputStream;

// Process the file identified by path calling block with each line to be processed or any
// error. Either line or error will be non nil.
- (void)processFile:(NSString *)path withEncoding:(NSStringEncoding)fileEncoding usingBlock:(void (^)(NSString *line, NSError *error))block;

- (void)closeStream;
@end
