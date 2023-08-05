#import <React/RCTEventEmitter.h>

#ifdef RCT_NEW_ARCH_ENABLED
#import "RNReactNativeFsSpec.h"

@interface ReactNativeFs : RCTEventEmitter <NativeReactNativeFsSpec>
#else
#import <React/RCTBridgeModule.h>

@interface ReactNativeFs : RCTEventEmitter <RCTBridgeModule>
#endif

@property (retain) NSMutableDictionary* downloaders;
@property (retain) NSMutableDictionary* uuids;
@property (retain) NSMutableDictionary* uploaders;

@end
