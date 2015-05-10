//
//  NSArray+Map.h
//  RNFS
//
// taken from http://stackoverflow.com/questions/6127638/nsarray-equivalent-of-map

#import <Foundation/Foundation.h>

@interface NSArray (Map)

- (NSArray *)rnfs_mapObjectsUsingBlock:(id (^)(id obj, NSUInteger idx))block;

@end