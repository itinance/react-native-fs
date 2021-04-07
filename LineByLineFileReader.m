//
//  LineByLineFileReader.m
//
//  Copyright (c) 2012 Alex Moffat. All rights reserved.
//

#import "LineByLineFileReader.h"
@interface LineByLineFileReader () <NSStreamDelegate>

// The encoding we're expecting in the file
@property (nonatomic, assign) NSStringEncoding encoding;

// Buffer to hold any unprocessed string data after each loop.
@property (nonatomic, strong) NSString *stringBuffer;

// Block that is called to process each line as it is read.
@property (nonatomic, copy) void (^lineProcessor)(NSString *line, NSError *error);
@end

@implementation LineByLineFileReader

// The maximum amount of data to try to read from the stream each time
// we're called.
unsigned int const TRY_TO_READ = 1024;

// Buffer used to hold the data that is read from the stream
uint8_t buf[TRY_TO_READ];

// Start processing the file at "path".
- (void)startProcessing:(NSString *)path
{
    // Ensure we have a path.
    if (!path) {
        NSError *error = [NSError errorWithDomain:NSCocoaErrorDomain code:NSFileNoSuchFileError userInfo:nil];
        self.lineProcessor(nil, error);
        return;
    }
    // Create the stream.
    NSInputStream *instream = [[NSInputStream alloc] initWithFileAtPath:path];
    self.inputStream = instream;
    if (!instream) {
        self.lineProcessor(nil, [instream streamError]);
        return;
    }
    // Set up so that our stream:handleEvent method gets called.
    [instream setDelegate:self];
    [instream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [instream open];
    
    // Check for any errors at this point.
    if ([instream streamStatus] == NSStreamStatusError) {
        self.lineProcessor(nil, [instream streamError]);
        return;        
    }
}

// Close the stream.
- (void)closeStream:(NSStream *)theStream
{
    [theStream close];
    [theStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

// Read some data from the stream.
- (void)readDataFromStream:(NSStream *)theStream
{
    unsigned int len = [(NSInputStream *)theStream read:buf maxLength:TRY_TO_READ];
    if (len) {
        // We read something.
        if (self.stringBuffer) {
            // Some data left from the last time this method was called so
            // append the new data.
            self.stringBuffer = [self.stringBuffer stringByAppendingString:[[NSString alloc] initWithBytes:buf length:len encoding:self.encoding]];
        } else {
            // No data left over from last time.
            self.stringBuffer = [[NSString alloc] initWithBytes:buf length:len encoding:self.encoding];
        }
        // Split on newlines.
        NSArray *lines = [self.stringBuffer componentsSeparatedByCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"\r\n"]];
        // Lines are processed in arrears, that is each time round the loop we send
        // to the lineProcessor the line read on the previous time round. This is because
        // we might not get a complete last line. However, if the whole stringBuffer ends
        // with the newlines then we get an empty string and so know we read a complete
        // line. Any remaining data is stored in stringBuffer and used on the next loop
        // or sent when the stream is closed.
        NSString *lineToProcess = nil;
        BOOL needFinishReading = NO;
        for (NSString *line in lines) {
            if (lineToProcess) {
                self.lineProcessor(lineToProcess, nil);
                // Use an empty string here so that files
                // that end with a newline have a final empty
                // line just like if reading with stringWithContentsOfFile
                // and then splitting.
                lineToProcess = @"";
            }
            if (![line isEqualToString:@""]) {
                lineToProcess = line;
            }
        }
        // Leave any remaining data in the buffer.
        self.stringBuffer = lineToProcess;
        if (needFinishReading) {
            [self finishReading];
        }
    }    
}

// NSStreamDelegate method that is called when events happen on the stream.
- (void)stream:(NSStream *)theStream handleEvent:(NSStreamEvent)streamEvent
{
    if (streamEvent & NSStreamEventErrorOccurred) {
        self.lineProcessor(nil, [theStream streamError]);
        return;
    }
    if (streamEvent & NSStreamEventHasBytesAvailable) {
        [self readDataFromStream:theStream];
    }
    if (streamEvent & NSStreamEventEndEncountered) {
        [self finishReading];
    }
}

- (void)finishReading
{
    if (self.inputStream) {
        [self closeStream:self.inputStream];
    }
    // Treat anything left in stringBuffer as the remaining line.
    if (self.stringBuffer) {
        self.lineProcessor(self.stringBuffer, nil);
        self.stringBuffer = nil;
    }
    self.lineProcessor = nil;
}

#pragma mark - public methods
// Process the file identified by path calling block with each line to be processed or any
// error. Either line or error will be non nil.
- (void)processFile:(NSString *)path withEncoding:(NSStringEncoding)fileEncoding usingBlock:(void (^)(NSString *line, NSError *error))block;
{
    self.encoding = fileEncoding;
    self.lineProcessor = block;
    [self startProcessing:path];
}

- (void)closeStream
{
    [self finishReading];
}



@end
