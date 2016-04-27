//
//  CallerViewController.m
//  ipjsua
//
//  Created by include tech. on 27/04/16.
//  Copyright © 2016 Teluu. All rights reserved.
//

#import "CallerViewController.h"
#import "AppDelegate.h"

@implementation CallerViewController

- (void)viewDidLoad {
    
    [super viewDidLoad];
    
    _callButton.enabled = false;
    _registerTextField.delegate = self;
    _callToTextField.delegate = self;
}

- (void)viewWillAppear:(BOOL)animated {
    
    [super viewWillAppear:animated];
    
    UIAlertController * alert=   [UIAlertController
                                  alertControllerWithTitle:@"Error"
                                  message:@"Something Went Wrong Please try again aftersometime."
                                  preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction* yesButton = [UIAlertAction
                                actionWithTitle:@"Okay!"
                                style:UIAlertActionStyleDefault
                                handler:^(UIAlertAction * action)
                                {
                                    //Handel your yes please button action here
                                    [alert dismissViewControllerAnimated:YES completion:nil];
                                    
                                }];
    
    [alert addAction:yesButton];
    
    [self.tabBarController.selectedViewController presentViewController:alert animated:YES completion:nil];
}

- (void)updateWindow {
    
    NSArray *subviews = self.previewWindowContainer.subviews;
    
    for (UIView *view in subviews) {
        
        [view removeFromSuperview];
    }
    [self displayVideoPreview];
}

- (void)displayVideoPreview {
    
    pjsua_vid_win_id wid = pjsua_vid_preview_get_win(0);
    [self displayWindow:wid];
}

- (void)displayWindow:(pjsua_vid_win_id)wid
{
    int i, last;
    
    i = (wid == PJSUA_INVALID_ID) ? 0 : wid;
    last = (wid == PJSUA_INVALID_ID) ? PJSUA_MAX_VID_WINS : wid+1;
    
    for (;i < last; ++i) {
        pjsua_vid_win_info wi;
        
        if (pjsua_vid_win_get_info(i, &wi) == PJ_SUCCESS) {
            UIView *parent = self.previewWindowContainer;
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


- (IBAction)callButtonAction:(id)sender {
    if (_callToTextField.text.length > 0) {
        int status = -1;
        status = [[PJSIP sharedPJSIP] makeCallTo:_callToTextField.text];
    }
}

- (IBAction)loginAction:(id)sender {
    
    if (self.registerTextField.text.length > 0) {
        int status = -1;
        status = [[PJSIP sharedPJSIP] addAccountWithUsername:self.registerTextField.text password:self.registerTextField.text toServer:SERVER];
        if (status == 0) {
            [NSTimer scheduledTimerWithTimeInterval:2.0 target:self selector:@selector(updateWindow) userInfo:nil repeats:YES];
            _callButton.enabled = true;
            _registerTextField.text = @"";
            _loginButton.hidden = true;
            _registerTextField.hidden = true;
        }
    }
}

-(BOOL)textFieldShouldReturn:(UITextField *)textField{
    return textField.resignFirstResponder;
}

@end