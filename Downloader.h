#import <Foundation/Foundation.h>

typedef void (^DownloadCompleteCallback)(NSNumber*, NSNumber*);
typedef void (^DownloadErrorCallback)(NSError*);
typedef void (^DownloadBeginCallback)(NSNumber*, NSNumber*, NSDictionary*);
typedef void (^DownloadProgressCallback)(NSNumber*, NSNumber*);

@interface DownloadParams : NSObject

@property (copy) NSString* fromUrl;
@property (copy) NSString* toFile;
@property (copy) DownloadCompleteCallback completeCallback;         // Download has finished (data written)
@property (copy) DownloadErrorCallback errorCallback;         // Something went wrong
@property (copy) DownloadBeginCallback beginCallback;         // Download has started (headers received)
@property (copy) DownloadProgressCallback progressCallback;   // Download is progressing

@end

@interface Downloader : NSObject <NSURLConnectionDelegate>

- (void)downloadFile:(DownloadParams*)params;
- (void)stopDownload;

@end
