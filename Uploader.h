#import <Foundation/Foundation.h>

#if !TARGET_OS_OSX
#import <MobileCoreServices/MobileCoreServices.h>
#endif

typedef void (^UploadCompleteCallback)(NSString*, NSURLResponse *);
typedef void (^UploadErrorCallback)(NSError*);
typedef void (^UploadBeginCallback)(void);
typedef void (^UploadProgressCallback)(NSNumber*, NSNumber*);

@interface RNFSUploadParams : NSObject

@property (copy) NSString* toUrl;
@property (copy) NSArray* files;
@property (copy) NSDictionary* headers;
@property (copy) NSDictionary* fields;
@property (copy) NSString* method;
@property (assign) BOOL binaryStreamOnly;
@property (copy) UploadCompleteCallback completeCallback;    // Upload has finished (data written)
@property (copy) UploadErrorCallback errorCallback;         // Something gone wrong
@property (copy) UploadBeginCallback beginCallback;         // Upload has started
@property (copy) UploadProgressCallback progressCallback;   // Upload is progressing

@end

@interface RNFSUploader : NSObject <NSURLConnectionDelegate>

- (void)uploadFiles:(RNFSUploadParams*)params;
- (void)stopUpload;

@end
