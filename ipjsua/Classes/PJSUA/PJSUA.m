//
//  PJSUA.m
//  ipjsua
//
//  Created by Chaitanya Pandit on 21/04/16.
//  Copyright © 2016 Teluu. All rights reserved.
//

#import "PJSUA.h"
#import <pjlib.h>
#import <pjsua.h>
#import <pj/log.h>
#include "pjsua_app.h"
#include "pjsua_app_config.h"
#define THIS_FILE	"PJSUA.m"

PJSUA      *app;
static pjsua_app_cfg_t  app_cfg;
static char           **restartArgv;
static int              restartArgc;

static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);

@implementation PJSUA

+ (PJSUA *)sharedInstance
{
    static PJSUA *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[PJSUA alloc] init];
    });
    return sharedInstance;
}

static void displayMsg(const char *msg)
{
    NSString *str = [NSString stringWithFormat:@"%s", msg];
    NSLog(@"%@", str);
}

static void pjsuaOnStartedCb(pj_status_t status, const char* msg)
{
    char errmsg[PJ_ERR_MSG_SIZE];
    
    if (status != PJ_SUCCESS && (!msg || !*msg)) {
        pj_strerror(status, errmsg, sizeof(errmsg));
        PJ_LOG(3,(THIS_FILE, "Error: %s", errmsg));
        msg = errmsg;
    } else {
        PJ_LOG(3,(THIS_FILE, "Started: %s", msg));
    }
    
    displayMsg(msg);
}

static void pjsuaOnStoppedCb(pj_bool_t restart,
                             int argc, char** argv)
{
    PJ_LOG(3,("ipjsua", "CLI %s request", (restart? "restart" : "shutdown")));
    if (restart) {
        displayMsg("Restarting..");
        pj_thread_sleep(100);
        app_cfg.argc = argc;
        app_cfg.argv = argv;
    } else {
        displayMsg("Shutting down..");
        pj_thread_sleep(100);
    }
}

static void pjsuaOnAppConfigCb(pjsua_app_config *cfg)
{
    PJ_UNUSED_ARG(cfg);
}

- (void)pjsuaStart
{
    // TODO: read from config?
    const char **argv = pjsua_app_def_argv;
    int argc = PJ_ARRAY_SIZE(pjsua_app_def_argv) -1;
    pj_status_t status;
    
    displayMsg("Starting..");
    
    pj_bzero(&app_cfg, sizeof(app_cfg));
    if (restartArgc) {
        app_cfg.argc = restartArgc;
        app_cfg.argv = restartArgv;
    } else {
        app_cfg.argc = argc;
        app_cfg.argv = (char**)argv;
    }
    app_cfg.on_started = &pjsuaOnStartedCb;
    app_cfg.on_stopped = &pjsuaOnStoppedCb;
    app_cfg.on_config_init = &pjsuaOnAppConfigCb;
    app_config.cfg.cb.on_incoming_call = &on_incoming_call;

    status = pjsua_app_init(&app_cfg);
    if (status != PJ_SUCCESS) {
        char errmsg[PJ_ERR_MSG_SIZE];
        pj_strerror(status, errmsg, sizeof(errmsg));
        displayMsg(errmsg);
        pjsua_app_destroy();
        return;
    } else {
        
       status = pj_add_account(0, "receiver", "anyhting", "107.170.46.82");
        if (status != PJ_SUCCESS) {
            NSLog(@"%@", @"Error ");
        }
        
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
           //pj_status_t status = pj_make_call(0, "receiver");
            // NSLog(@"%@", @"NO Error ");
        });
    }
}

void displayWindow(pjsua_vid_win_id wid)
{
#if PJSUA_HAS_VIDEO
    int i, last;
    
    i = (wid == PJSUA_INVALID_ID) ? 0 : wid;
    last = (wid == PJSUA_INVALID_ID) ? PJSUA_MAX_VID_WINS : wid+1;
    
    for (;i < last; ++i) {
        pjsua_vid_win_info wi;
        
        if (pjsua_vid_win_get_info(i, &wi) == PJ_SUCCESS) {
            UIView *parent = nil;
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
    
    
#endif
}

pj_bool_t showNotification(pjsua_call_id call_id)
{
    // Create a new notification
    UILocalNotification* alert = [[UILocalNotification alloc] init];
    if (alert)
    {
        alert.repeatInterval = 0;
        alert.alertBody = @"Incoming call received...";
        /* This action just brings the app to the FG, it doesn't
         * automatically answer the call (unless you specify the
         * --auto-answer option).
         */
        alert.alertAction = @"Activate app";
        
        [[UIApplication sharedApplication] presentLocalNotificationNow:alert];
    }
    
    return PJ_FALSE;
}

static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                             pjsip_rx_data *rdata)
{
    pjsua_call_info ci;
    pjsua_call_get_info(call_id, &ci);
    
    pjsua_call_setting settings;
    settings.aud_cnt = 1;
    settings.vid_cnt = 1;
    
    //    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer2(call_id, &settings, 200, NULL, NULL);
    
    // Get the window of call
    int vid_idx;
    pjsua_vid_win_id wid = -1;
    
    vid_idx = pjsua_call_get_vid_stream_idx(call_id);
    if (vid_idx >= 0) {
        pjsua_call_info ci;
        
        pjsua_call_get_info(call_id, &ci);
        wid = ci.media[vid_idx].stream.vid.win_in;
        pjsua_vid_win_set_show(wid, PJ_TRUE);
    }

    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"SHOWCALLWINDOW" object:@(wid)];
}

@end
