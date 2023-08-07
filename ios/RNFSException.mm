#import "RNFSException.h"

static NSString * const ERROR_DOMAIN = @"RNFS";

@implementation RNFSException;

- (id) initWithName:(NSString*)name details:(NSString*)details
{
  self = [super initWithName:name reason:details userInfo:nil];
  return self;
}

/**
 * Creates a new NSError object based on this RNFSException
 */
- (NSError*) error
{
  return [NSError
          errorWithDomain: ERROR_DOMAIN
          code: self.code
          userInfo: self.userInfo
  ];
}

- (RNFSException*) log
{
  NSLog(@"%@: %@", self.name, self.reason);
  return self;
}

- (void) reject: (RCTPromiseRejectBlock)reject
{
  reject(self.name, self.reason, [self error]);
}

- (void) reject: (RCTPromiseRejectBlock)reject details: (NSString*) details
{
  NSString *reason = self.reason;
  if (details != nil) reason = [reason stringByAppendingString:details];
  reject(self.name, reason, [self error]);
}

+ (RNFSException*) from: (NSException*)exception
{
  return [[RNFSException alloc]
          initWithName: exception.name
          reason: exception.reason
          userInfo: exception.userInfo
  ];
}

+ (RNFSException*) name: (NSString*)name
{
  return [[RNFSException alloc] initWithName:name details:nil];
}

+ (RNFSException*) name: (NSString*)name details:(NSString*)details
{
  return [[RNFSException alloc] initWithName:name details:details];
}

+ (RNFSException*) NOT_IMPLEMENTED
{
  return [
    [RNFSException alloc]
    initWithName:@"NOT_IMPLEMENTED"
    details:@"This method is not implemented for iOS"
  ];
}

@end;
