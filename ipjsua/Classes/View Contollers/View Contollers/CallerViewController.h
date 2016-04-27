//
//  CallerViewController.h
//  ipjsua
//
//  Created by include tech. on 27/04/16.
//  Copyright © 2016 Teluu. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PJSIP.h"
#import "ReceiverViewController.h"
#define SERVER @"107.170.46.82"


@interface CallerViewController : UIViewController<UITextFieldDelegate>

@property (strong, nonatomic) IBOutlet UIView *previewWindowContainer;
@property (strong, nonatomic) IBOutlet UITextField *registerTextField;
@property (strong, nonatomic) IBOutlet UITextField *callToTextField;
@property (strong, nonatomic) IBOutlet UIButton *loginButton;
@property (strong, nonatomic) IBOutlet UIButton *callButton;

@end
