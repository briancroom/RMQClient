#import <Foundation/Foundation.h>
@import Mantle;
#import "AMQParser.h"

@protocol AMQEncoding <NSObject>
- (nonnull NSData *)amqEncoded;
@end

@protocol AMQParseable <NSObject>
- (nonnull instancetype)initWithParser:(nonnull AMQParser *)parser;
@end

@protocol AMQFieldValue <NSObject,AMQEncoding,AMQParseable>
- (nonnull NSData *)amqFieldValueType;
@end

@interface AMQOctet : MTLModel<AMQEncoding,AMQParseable>
@property (nonatomic, readonly) NSUInteger integerValue;
- (nonnull instancetype)init:(char)octet;
@end

@interface AMQBoolean : MTLModel<AMQEncoding,AMQFieldValue,AMQParseable>
@property (nonatomic, readonly) BOOL boolValue;
- (nonnull instancetype)init:(BOOL)boolean;
@end

@interface AMQShort : MTLModel<AMQFieldValue>
@property (nonatomic, readonly) NSUInteger integerValue;
- (nonnull instancetype)init:(NSUInteger)val;
@end

@interface AMQLong : MTLModel<AMQFieldValue>
@property (nonatomic, readonly) NSUInteger integerValue;
- (nonnull instancetype)init:(NSUInteger)val;
@end

@interface AMQLonglong : MTLModel<AMQFieldValue>
@property (nonatomic, readonly) uint64_t integerValue;
- (nonnull instancetype)init:(uint64_t)val;
@end

@interface AMQShortstr : MTLModel<AMQFieldValue>
@property (nonnull, nonatomic, copy, readonly) NSString *stringValue;
- (nonnull instancetype)init:(nonnull NSString *)string;
@end

@interface AMQLongstr : MTLModel<AMQFieldValue>
@property (nonnull, nonatomic, copy, readonly) NSString *stringValue;
- (nonnull instancetype)init:(nonnull NSString *)string;
@end

@interface AMQTable : MTLModel<AMQFieldValue>
- (nonnull instancetype)init:(nonnull NSDictionary *)dictionary;
@end

@interface AMQTimestamp : MTLModel<AMQFieldValue>
- (nonnull instancetype)init:(nonnull NSDate *)date;
@end

@interface AMQFieldValuePair : MTLModel<AMQEncoding>
- (nonnull instancetype)initWithFieldName:(nonnull NSString *)fieldName
                               fieldValue:(nonnull id <AMQEncoding,AMQFieldValue>)fieldValue;
@end

@interface AMQCredentials : AMQLongstr
- (nonnull instancetype)initWithUsername:(nonnull NSString *)username
                                password:(nonnull NSString *)password;
@end

@protocol AMQReplyContext <NSObject>
@property (nonnull, nonatomic, readonly) NSNumber *channelMax;
@property (nonnull, nonatomic, readonly) NSNumber *frameMax;
@property (nonnull, nonatomic, readonly) NSNumber *heartbeat;
- (nonnull AMQCredentials *)credentials;
@end

@protocol AMQIncomingCallbackContext <NSObject>
- (void)close:(void (^ _Nonnull)())onClose;
@end

@protocol AMQPayload <NSObject, AMQEncoding>
- (nonnull NSNumber *)frameTypeID;
@end

@protocol AMQMethod <NSObject, AMQPayload>
+ (nonnull NSArray *)frame;
- (nonnull instancetype)initWithDecodedFrame:(nonnull NSArray *)frame;
- (BOOL)hasContent;
- (nonnull NSNumber *)classID;
@end

@protocol AMQAwaitServerMethod <NSObject>
@end

@protocol AMQOutgoingSync <NSObject, AMQAwaitServerMethod>
@end

@protocol AMQOutgoingPrecursor <NSObject>
- (nonnull id<AMQMethod>)nextRequest;
@end

@protocol AMQIncomingSync <NSObject,AMQMethod>
- (nonnull id<AMQMethod>)replyWithContext:(nonnull id<AMQReplyContext>)context;
@end

@protocol AMQIncomingCallback <NSObject>
- (void)didReceiveWithContext:(nonnull id<AMQIncomingCallbackContext>)context;
@end

@interface AMQContentHeader : MTLModel<AMQPayload>
- (nonnull instancetype)initWithClassID:(nonnull NSNumber *)classID
                               bodySize:(nonnull NSNumber *)bodySize
                             properties:(nonnull NSArray *)properties;
- (nonnull instancetype)initWithParser:(nonnull AMQParser *)parser;
@end

@interface AMQContentHeaderNone : MTLModel<AMQPayload>
@end

@interface AMQContentBody : MTLModel<AMQPayload>
@property (nonnull, nonatomic, readonly) NSData *data;
- (nonnull instancetype)initWithData:(nonnull NSData *)data;
- (nonnull instancetype)initWithParser:(nonnull AMQParser *)parser
                           payloadSize:(UInt32)payloadSize;
@end

@interface AMQFrameset : MTLModel<AMQEncoding>
@property (nonnull, nonatomic, copy, readonly) id<AMQMethod>method;
@property (nonnull, nonatomic, readonly) NSArray *contentBodies;
@property (nonnull, nonatomic, copy, readonly) NSNumber *channelID;
@property (nonnull, nonatomic, readonly) NSArray *frames;
- (nonnull instancetype)initWithChannelID:(nonnull NSNumber *)channelID
                                   method:(nonnull id<AMQMethod>)method
                            contentHeader:(nonnull id<AMQPayload>)contentHeader
                            contentBodies:(nonnull NSArray *)contentBodies;
@end

@interface AMQFrame : MTLModel<AMQEncoding,AMQParseable>
@property (nonnull, nonatomic, readonly) id<AMQPayload> payload;
- (nonnull instancetype)initWithChannelID:(nonnull NSNumber *)channelID
                                  payload:(nonnull id<AMQEncoding>)payload;
@end