
#ifdef RCT_NEW_ARCH_ENABLED
#import "RNReactNativeFsSpec.h"

@interface ReactNativeFs : NSObject <NativeReactNativeFsSpec>
#else
#import <React/RCTBridgeModule.h>

@interface ReactNativeFs : NSObject <RCTBridgeModule>
#endif

@end
