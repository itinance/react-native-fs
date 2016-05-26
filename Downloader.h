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
@property        bool background;                     // Whether to continue download when app is in background

@end

@interface Downloader : NSObject <NSURLSessionDelegate, NSURLSessionDownloadDelegate>

- (void)downloadFile:(DownloadParams*)params;
- (void)stopDownload;

@end
