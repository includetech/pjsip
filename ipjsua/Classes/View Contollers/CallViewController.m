//
//  CallViewController.m
//  ipjsua
//
//  Created by Chaitanya Pandit on 22/04/16.
//  Copyright © 2016 Teluu. All rights reserved.
//

#import "CallViewController.h"
#import "PJSUA.h"
#define SERVER @"107.170.46.82"

@interface CallViewController ()

@property (nonatomic) BOOL pjsuaStarted;

@end

@implementation CallViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(displayCallWindow) name:@"incoming" object:nil];

    // Do any additional setup after loading the view.
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    if (!self.pjsuaStarted) {
        
        int status = -1;
        status = [[PJSIP sharedPJSIP] addAccountWithUsername:@"foo" password:@"foo" toServer:SERVER];
        self.pjsuaStarted = YES;
        
//        if (status == 0) {
//            [[PJSIP sharedPJSIP] makeCallTo:@"foo"];
//            [self displayVideoPreview];
//        }
        
        [NSTimer scheduledTimerWithTimeInterval:2.0 target:self selector:@selector(updateWindow) userInfo:nil repeats:YES];
    }
}

- (void)updateWindow {
    
    NSArray *subviews = self.view.subviews;
    for (UIView *view in subviews) {
        
        [view removeFromSuperview];
    }
    
    [self displayCallWindow];
}

- (void)displayVideoPreview {
    
    pjsua_vid_win_id wid = pjsua_vid_preview_get_win(0);
    [self displayWindow:wid];
}

- (void)displayCallWindow {
    
    [self displayWindow:0];
}

- (void)displayWindow:(pjsua_vid_win_id)wid
{
    int i, last;
    
    i = (wid == PJSUA_INVALID_ID) ? 0 : wid;
    last = (wid == PJSUA_INVALID_ID) ? PJSUA_MAX_VID_WINS : wid+1;
    
    for (;i < last; ++i) {
        pjsua_vid_win_info wi;
        
        if (pjsua_vid_win_get_info(i, &wi) == PJ_SUCCESS) {
            UIView *parent = self.view;
            UIView *view = (__bridge UIView *)wi.hwnd.info.ios.window;
            
            if (view) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    /* Add the video window as subview */
                    if (![view isDescendantOfView:parent])
                        [parent addSubview:view];
                    
                    if (!wi.is_native) {
                        /* Resize it to fit width */
                        view.bounds = CGRectMake(0, 0, parent.bounds.size.width,
                                                 (parent.bounds.size.height *
                                                  1.0*parent.bounds.size.width/
                                                  view.bounds.size.width));
                        /* Center it horizontally */
                        view.center = CGPointMake(parent.bounds.size.width/2.0,
                                                  view.bounds.size.height/2.0);
                    } else {
                        /* Preview window, move it to the bottom */
                        view.center = CGPointMake(parent.bounds.size.width/2.0,
                                                  parent.bounds.size.height-
                                                  view.bounds.size.height/2.0);
                    }
                });
            }
        }
    }
}

@end
