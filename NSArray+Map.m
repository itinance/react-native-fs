//
//  NSArray+Map.m
//  RNFS

#import "NSArray+Map.h"

@implementation NSArray (Map)

- (NSArray *)rnfs_mapObjectsUsingBlock:(id (^)(id obj, NSUInteger idx))block
{
  NSMutableArray *result = [NSMutableArray arrayWithCapacity:[self count]];

  [self enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
    NSDictionary *parsedBlock = block(obj, idx);
    if (!parsedBlock) {
        return;
    }
    [result addObject:parsedBlock];
  }];

  return result;
}

@end
