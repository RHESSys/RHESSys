//
//  ArborbaseAppDelegate.h
//  Arborbase
//
//  Created by David Groulx on 10/7/22.
//  Copyright 2010 UCSB. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "BSPMySQL.h"
#import "EditorController.h"
#import "LoginController.h"

@interface ArborbaseAppDelegate : NSObject <NSApplicationDelegate> {
    IBOutlet NSWindow* loginSheet;
	IBOutlet NSWindow* window;
	IBOutlet NSButton* loginButton;
	BSPMySQL *db;
	EditorController* editor;
	LoginController* login;
	
	NSMutableArray* watersheds;
	NSMutableArray* zones;
	NSMutableArray* soils;
	NSMutableArray* strata;
	NSMutableArray* landUses;
}

@property (readwrite, assign) IBOutlet NSWindow *window;
@property (readwrite, assign) NSMutableArray* watersheds;
@property (readwrite, assign) NSMutableArray* zones;
@property (readwrite, assign) NSMutableArray* soils;
@property (readwrite, assign) NSMutableArray* strata;
@property (readwrite, assign) NSMutableArray* landUses;


- (IBAction)upload:(id)sender;
- (IBAction)showLogin:(id)sender;
- (IBAction)login:(id)sender;
- (IBAction)cancelLogin:(id)sender;

- (void)editorForID:(id)default_ID type:(NSString*)type;
- (void)zoneEditorForID:(id)default_ID;
- (void)soilEditorForID:(id)default_ID;
- (void)stratumEditorForID:(id)default_ID;
- (void)landUseEditorForID:(id)default_ID;
- (void)updateFromDB;


@end
