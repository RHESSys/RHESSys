//
//  EditorController.h
//  Arborbase
//
//  Created by David Groulx on 10/7/23.
//  Copyright 2010 UCSB. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "BSPMySQL.h"

@interface EditorController : NSWindowController {
	NSString* type;
	int default_ID;
	NSString* id_field;
	BSPMySQL *db;
	NSArray* doNotWrite; // List of fields that should not be written to a text file when exporting
	NSMutableArray* fields;
}

@property (readwrite, assign) NSMutableArray* fields;


- (id)initWithDatabase:(BSPMySQL*)database;
- (void)updateFromDB;
- (void)setDefaultID:(int)i type:(NSString*)t;

- (IBAction)updateToDB:(id)sender;
- (IBAction)export:(id)sender;

@end
