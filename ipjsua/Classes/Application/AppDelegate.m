//
//  AppDelegate.m
//  Foo
//
//  Created by Chaitanya Pandit on 21/04/16.
//  Copyright © 2016 include tech. All rights reserved.
//

#import "AppDelegate.h"
#import "PJSUA.h"

@interface AppDelegate ()

@property (nonatomic) BOOL pjsuaStarted;
@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    return YES;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    if (!self.pjsuaStarted) {
        [[PJSUA sharedInstance] pjsuaStart];
        self.pjsuaStarted = YES;
    }
}

@end
