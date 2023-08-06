#import <React/RCTBridgeModule.h>

@interface RNException : NSException
- (id) initWithName: (NSString*)name details: (NSString*)details;
- (NSError*) error;
- (RNException*) log;
- (void) reject:(RCTPromiseRejectBlock)reject;
- (void) reject:(RCTPromiseRejectBlock)reject details:(NSString*)details;
+ (RNException*) from: (NSException*)exception;
+ (RNException*) name: (NSString*)name;
+ (RNException*) name: (NSString*)name details: (NSString*)details;

+ (RNException*) NOT_IMPLEMENTED;

@property(readonly) NSInteger code;
@end
