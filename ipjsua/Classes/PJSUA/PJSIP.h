//
//  PJSIP.h
//  ipjsua
//
//  Created by include tech. on 25/04/16.
//  Copyright © 2016 Teluu. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "pjsua_app.h"
#include "pjsua_app_common.h"

@interface PJSIP : NSObject

+(PJSIP *)sharedPJSIP;
-(pj_status_t)startPJSIP;
-(pj_status_t)addAccountWithUsername:(NSString *)username password:(NSString *)password toServer:(NSString *)server;
- (pj_status_t) makeCallTo:(NSString *)user;

@property (nonatomic) int accountID;

@end

