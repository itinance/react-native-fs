//
//  NSData+BKExtension.h
//  RNFS
//
//  Created by leejunhui on 2021/5/26.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSData (BKExtension)
/**
 NSData转化成string

 @return 返回nil的解决方案
 */
-(NSString *)convertedToUtf8String;
@end

NS_ASSUME_NONNULL_END
