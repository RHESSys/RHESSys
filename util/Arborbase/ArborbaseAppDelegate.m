//
//  ArborbaseAppDelegate.m
//  Arborbase
//
//  Created by David Groulx on 10/7/22.
//  Copyright 2010 UCSB. All rights reserved.
//

#import "ArborbaseAppDelegate.h"

@implementation ArborbaseAppDelegate

@synthesize window;
@synthesize watersheds;
@synthesize zones;
@synthesize soils;
@synthesize strata;
@synthesize landUses;

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
	db = [[BSPMySQL alloc] init];
	[db connect];
	[self updateFromDB];	
}

- (void)applicationWillTerminate:(NSNotification *)notification {
	[db close];	
}

// Pulls down all data from the remote DB and resets the array controllers in a KVO compliant manner
- (void)updateFromDB {
	NSMutableArray* result;
	[db query:@"SELECT watershed_name FROM Watershed" result:&result];
	[self setWatersheds:result];
	[db query:@"SELECT filename, zone_default_ID FROM Zone" result:&result];
	[self setZones:result];
	[db query:@"SELECT filename, patch_default_ID FROM Soil" result:&result];
	[self setSoils:result];
	[db query:@"SELECT filename, stratum_default_ID FROM Stratum" result:&result];
	[self setStrata:result];
	[db query:@"SELECT filename, landuse_default_ID FROM Land_Use" result:&result];
	[self setLandUses:result];
}

- (void)editorForID:(id)default_ID type:(NSString*)type {
	if (!editor) {
		editor = [[EditorController alloc] initWithDatabase:db];
	}

	[editor showWindow:self];
	[editor setDefaultID:[default_ID intValue] type:type];
}

- (void)zoneEditorForID:(id)default_ID {
	[self editorForID:default_ID type:@"Zone"];
}

- (void)soilEditorForID:(id)default_ID {
	[self editorForID:default_ID type:@"Soil"];
}

- (void)stratumEditorForID:(id)default_ID {
	[self editorForID:default_ID type:@"Stratum"];	
}

- (void)landUseEditorForID:(id)default_ID {
	[self editorForID:default_ID type:@"Land_Use"];
}


- (IBAction)upload:(id)sender {
	NSOpenPanel* openPanel = [NSOpenPanel openPanel];
	[openPanel setCanChooseFiles:YES];
	
	if ([openPanel runModalForDirectory:nil file:nil] == NSOKButton) {
		NSArray* files = [openPanel filenames];
		
		// These will be used for splitting the input file strings and cleaning out blank lines
		NSPredicate* blanksPredicate = [NSPredicate predicateWithFormat:@"SELF != \"\""];
		NSCharacterSet* ws = [NSCharacterSet whitespaceAndNewlineCharacterSet];
		
		int i;
		for ( i = 0; i < [files count]; ++i) {
			NSString* file = [NSString stringWithContentsOfFile:[files objectAtIndex:i] encoding:NSUTF8StringEncoding error:nil];
			NSArray* lines = [file componentsSeparatedByString:@"\n"];
			
			

			// Check the first line to get the type of def file we are adding
			NSString* type = [[[[lines objectAtIndex:0] componentsSeparatedByCharactersInSet:ws] filteredArrayUsingPredicate:blanksPredicate] objectAtIndex:1];
			NSLog(@"type: <%@>", type);
			NSString* tableName;
			if ([type isEqualToString:@"zone_default_ID"]) {
				tableName  = @"Zone";
			} else if ([type isEqualToString:@"patch_default_ID"]) {
				tableName = @"Soil";
			} else if ([type isEqualToString:@"landuse_default_ID"]) {
				tableName = @"Land_Use";
			} else if ([type isEqualToString:@"stratum_default_ID"]) {
				tableName = @"Stratum";
			} else {
				NSLog(@"Could not find a matching table for type %@", type);
				return;
			}
			
			NSString* refTableName = [NSString stringWithFormat:@"%@_Reference", tableName];
			
			NSLog(@"table name: %@", tableName);
			
			NSMutableString* columnName = [NSMutableString stringWithCapacity:40];
			NSMutableString* columnValue = [NSMutableString stringWithCapacity:40];
			NSMutableString* refValue = [NSMutableString stringWithCapacity:40];
			
			[columnName appendString:@"filename"];
			NSString* filename = [[files objectAtIndex:0] lastPathComponent];
			[columnValue appendFormat:@"\"%@\"", filename];
			
			for (NSString* line in lines) {
				NSArray* fields = [line componentsSeparatedByCharactersInSet:ws];
				
				// Filter out blanks
				fields = [fields filteredArrayUsingPredicate:blanksPredicate];
				
				// Check there are at least 2 elements, and that the first element is not blank, which is probably 
				// a blank line
				if ([fields count] > 1 && ![[fields objectAtIndex:0] isEqualToString:@""]) {
					// RHESSys def files use periods in some field names, SQL demands those be delimmited by
					// backticks
					[columnName appendFormat:@", `%@`", [fields objectAtIndex:1]];
					
					// Non-numeric values must be quoted 
					NSScanner* isNumeric = [NSScanner scannerWithString:[fields objectAtIndex:0]];
					if ([isNumeric scanFloat:NULL]) {
						[columnValue appendFormat:@", %@", [fields objectAtIndex:0]];
					} else {
						[columnValue appendFormat:@", \"%@\"", [fields objectAtIndex:0]];
					}
				}
				
			}
						
			if ( ![db query:[NSString stringWithFormat:@"INSERT INTO %@ (%@) VALUES(%@)", tableName, columnName, columnValue] result:nil] ) {
				NSAlert* alert = [NSAlert alertWithMessageText:[NSString stringWithFormat:@"Error importing %@", filename]
												 defaultButton:nil
											   alternateButton:nil
												   otherButton:nil
									 informativeTextWithFormat:@"%@", [db error]];
				[alert beginSheetModalForWindow:window modalDelegate:self 
								 didEndSelector:nil 
									contextInfo:NULL];
			}
			
			[self updateFromDB];
			// Deal with ciations after it fucking works
			//[db query:[NSString stringWithFormat:@"INSERT INTO %@ (%@) VALUES (%@)", refTableName, columnName, refValue]];
		}
	}
}

- (IBAction)showLogin:(id)sender {
	[NSApp beginSheet:loginSheet
	   modalForWindow:[self window]
		modalDelegate:nil
	   didEndSelector:NULL
		  contextInfo:NULL];
}

- (IBAction)login:(id)sender {
	
}

- (IBAction)cancelLogin:(id)sender {
	[NSApp endSheet:loginSheet];
	[loginSheet orderOut:sender];
}

@end
