//
//  ReceiverViewController.m
//  ipjsua
//
//  Created by include tech. on 27/04/16.
//  Copyright © 2016 Teluu. All rights reserved.
//

#import "ReceiverViewController.h"

@implementation ReceiverViewController

- (void)viewDidLoad {
    
    [super viewDidLoad];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(displayCallWindow) name:@"incoming" object:nil];

    // Do any additional setup after loading the view.
}

- (void) viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
     [NSTimer scheduledTimerWithTimeInterval:2.0 target:self selector:@selector(updateWindow) userInfo:nil repeats:YES];
}

- (void)updateWindow {
    
    NSArray *subviews = self.videoContainerView.subviews;
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
            UIView *parent = self.videoContainerView;
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
