//
//  EditorController.m
//  Arborbase
//
//  Created by David Groulx on 10/7/23.
//  Copyright 2010 UCSB. All rights reserved.
//

#import "EditorController.h"


@implementation EditorController

@synthesize fields;


- (id)initWithDatabase:(BSPMySQL*)database {
	if (![super initWithWindowNibName:@"Editor"])
		return nil;
	
	doNotWrite = [NSArray arrayWithObjects:@"rhessys_version", @"filename", @"username", nil];
	db = database;
	
	return self;
}

- (void)updateFromDB {
	NSMutableArray* result;
	NSMutableArray* newFields;
	
	[db fieldsForTable:type result:&result];
	// So apparently in order for key value coding to work, it can't be an array of NSStrings, fields
	// needs to be an NSMutableArray of NSMutableDictionary, with each dictionary only having a single
	// key
	newFields = [NSMutableArray arrayWithCapacity:[result count]];
	for (NSString* columnName in result) {
		[newFields addObject:[NSMutableDictionary dictionaryWithObject:columnName forKey:@"field"]];
	}
	
	[db query:[NSString stringWithFormat:@"SELECT * FROM %@ WHERE %@ = %d", type, id_field, default_ID] result:&result];
//	NSLog(@"results: %@", result);
	// Result should be a single row. Copy this row into the values field
	if ([result count] > 1) {
		NSLog(@"Search for single record produced multiple records, this should not happen");
	}
	NSDictionary* row = [result objectAtIndex:0];
	
	for (NSMutableDictionary* dict in newFields) {
		[dict setObject:[row objectForKey:[dict objectForKey:@"field"]] forKey:@"value"];
	}
	
	[self setFields:newFields];
}

- (void)setDefaultID:(int)i type:(NSString*)t {
	type = t;
	
	if ([type isEqualToString:@"Zone"]) {
		id_field = @"zone_default_ID";
	} else if ([type isEqualToString:@"Soil"]) {
		id_field = @"patch_default_ID";
	} else if ([type isEqualToString:@"Land_Use"]) {
		id_field = @"landuse_default_ID";
	} else if ([type isEqualToString:@"Stratum"]) {
		id_field = @"stratum_default_ID";
	} else {
		NSLog(@"Invalid definition type %@", type);
	}

	default_ID = i;
	[self updateFromDB];
}

- (IBAction)updateToDB:(id)sender {
	NSMutableString* assignments = [[NSMutableString alloc] init];

	for (NSDictionary* dict in fields) {
		if (![[dict objectForKey:@"value"] isEqualToString:@""]) { // make sure value is not a blank
			NSString* field;
			NSString* value;
			field = [dict objectForKey:@"field"];
			value = [dict objectForKey:@"value"];
			
			NSScanner* isNumeric = [NSScanner scannerWithString:value];
			if (![isNumeric scanFloat:NULL]) { // not numeric, so quote it
				value = [NSString stringWithFormat:@"\"%@\"", value];
			}
			
			[assignments appendFormat:@"`%@` = %@, ", field, value];
		}
	}
	// Trim the trailing comma from the assignments line
	[assignments deleteCharactersInRange:NSMakeRange([assignments length]-2, 2)];
	
	NSString* q = [NSString stringWithFormat:@"UPDATE %@ SET %@ WHERE %@ = %d", type, assignments, id_field, default_ID];	
	[db query:q result:nil];
}

- (IBAction)export:(id)sender {
	NSOpenPanel* openPanel = [NSOpenPanel openPanel];
	[openPanel setCanChooseFiles:NO];
	[openPanel setCanChooseDirectories:YES];
	[openPanel setAllowsMultipleSelection:NO];
	
	if([openPanel runModal] == NSFileHandlingPanelOKButton) {
		NSLog(@"file was chosen: %@", [[openPanel URL] absoluteString]);
		NSString* filename = nil;
		
		NSMutableData* outdata = [NSMutableData dataWithCapacity:500];
		for (NSMutableDictionary* dict in fields) {
			if (![doNotWrite containsObject:[dict objectForKey:@"field"]]) { // Make sure this field is not on the do not write list
				NSString* newline = [NSString stringWithFormat:@"%@\t%@\n", [dict objectForKey:@"value"], [dict objectForKey:@"field"]];
				[outdata appendBytes:[newline UTF8String] length:[newline length]];	
				
				
			} else if ([[dict objectForKey:@"field"] isEqualToString:@"filename"]) {
				filename = [dict objectForKey:@"value"];
				NSLog(@"Found filename %@", filename);
			}
		}
		NSLog(@"done putting together data");
//		NSArray* arrayWithCompoents = [NSArray arrayWithObjects:[[openPanel URL] absoluteString], filename, nil];
//		NSString* path = [NSString pathWithComponents:arrayWithCompoents];
		NSString* path = [NSString pathWithComponents:[NSArray arrayWithObjects:[[openPanel URL] path], filename, nil]];
		NSLog(@"writing file to %@", path);
		[outdata writeToFile:path atomically:YES];
	}
}

@end
