#import <Foundation/Foundation.h>

typedef void (^DownloaderCallback)(NSNumber*, NSNumber*);
typedef void (^ErrorCallback)(NSError*);
typedef void (^BeginCallback)(NSNumber*, NSNumber*, NSDictionary*);
typedef void (^ProgressCallback)(NSNumber*, NSNumber*);

@interface DownloadParams : NSObject

@property (copy) NSString* fromUrl;
@property (copy) NSString* toFile;
@property (copy) DownloaderCallback callback;         // Download has finished (data written)
@property (copy) ErrorCallback errorCallback;         // Something went wrong
@property (copy) BeginCallback beginCallback;         // Download has started (headers received)
@property (copy) ProgressCallback progressCallback;   // Download is progressing

@end

@interface Downloader : NSObject <NSURLConnectionDelegate>

- (void)downloadFile:(DownloadParams*)params;
- (void)stopDownload;

@end
