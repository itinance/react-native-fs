#import <Foundation/Foundation.h>
#import <MobileCoreServices/MobileCoreServices.h>

typedef void (^UploadCompleteCallback)(NSString*);
typedef void (^UploadErrorCallback)(NSError*);
typedef void (^UploadBeginCallback)();
typedef void (^UploadProgressCallback)(NSNumber*, NSNumber*);

@interface UploadParams : NSObject

@property (copy) NSString* toUrl;
@property (copy) NSArray* files;
@property (copy) NSDictionary* headers;
@property (copy) NSDictionary* fields;
@property (copy) NSString* method;
@property (copy) UploadCompleteCallback completeCallback;   // Upload has finished (data written)
@property (copy) UploadErrorCallback errorCallback;         // Something gone wrong
@property (copy) UploadBeginCallback beginCallback;         // Upload has started
@property (copy) UploadProgressCallback progressCallback;   // Upload is progressing

@end

@interface Uploader : NSObject <NSURLConnectionDelegate>

- (void)uploadFiles:(UploadParams*)params;
- (void)stopUpload;

@end
