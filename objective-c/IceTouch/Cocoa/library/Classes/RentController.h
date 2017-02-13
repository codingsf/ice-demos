// **********************************************************************
//
// Copyright (c) 2003-2017 ZeroC, Inc. All rights reserved.
//
// **********************************************************************

#import <Cocoa/Cocoa.h>


@interface RentController : NSWindowController
{
    IBOutlet NSTextField* renterField;
}

@property (weak, readonly) NSString* renter;

-(void)ok:(id)sender;
-(void)cancel:(id)sender;

@end
