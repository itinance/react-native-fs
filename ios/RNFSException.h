#import <React/RCTBridgeModule.h>

@interface RNFSException : NSException
- (id) initWithName: (NSString*)name details: (NSString*)details;
- (NSError*) error;
- (RNFSException*) log;
- (void) reject:(RCTPromiseRejectBlock)reject;
- (void) reject:(RCTPromiseRejectBlock)reject details:(NSString*)details;
+ (RNFSException*) from: (NSException*)exception;
+ (RNFSException*) name: (NSString*)name;
+ (RNFSException*) name: (NSString*)name details: (NSString*)details;

+ (RNFSException*) NOT_IMPLEMENTED;

@property(readonly) NSInteger code;
@end
