//
//  PJSUA.h
//  ipjsua
//
//  Created by Chaitanya Pandit on 21/04/16.
//  Copyright © 2016 Teluu. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <pjsua-lib/pjsua.h>

@interface PJSUA : NSObject

+ (PJSUA *)sharedInstance;
- (void)pjsuaStart;

@end
