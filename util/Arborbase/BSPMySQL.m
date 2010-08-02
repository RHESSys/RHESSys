//
//  BSPMySQL.m
//  Arborbase
//
//  Created by David Groulx on 10/7/22.
//  Copyright 2010 UCSB. All rights reserved.
//

#import "BSPMySQL.h"


@implementation BSPMySQL

- (id)init {
	if (![super init])
		return nil;
	
	server = "localhost";
	user = "root";
	password = "";
	database = "rhessys_defs";
	
	errorAlert = TRUE;
	return self;
}

- (void)connect {
	NSLog(@"Attempting to connect...");	
	
	connection = mysql_init(NULL);
	
	if ( !mysql_real_connect(connection, server, user, password, database, 0, NULL, 0) ) {
		NSLog(@"%s", mysql_error(connection));
	} else {
		NSLog(@"Successfully connected to %s", server);
	}
}

- (BOOL)query:(NSString*)q result:(NSMutableArray**)r {
	MYSQL_RES *result;
	MYSQL_ROW row;
	
	NSLog(@"Query: %@", q);
	if ( mysql_query(connection, [q UTF8String]) ) {
		NSLog(@"%s", mysql_error(connection));
		return FALSE;		
	} else {			
		// Check if results must be processed
		result = mysql_store_result(connection);
		if (result) {
			int num_rows = mysql_num_rows(result);
			int num_fields = mysql_num_fields(result);	
			MYSQL_FIELD *fields = mysql_fetch_fields(result);
			NSMutableArray* rows = [NSMutableArray arrayWithCapacity:num_rows];
			
			while (row = mysql_fetch_row(result)) {
				NSMutableDictionary* currentRow = [NSMutableDictionary dictionaryWithCapacity:num_fields];
				int i;
				for (i = 0; i < num_fields; ++i) {
					if (row[i] == NULL) {
						row[i] = "";
					}
					[currentRow setValue:[NSString stringWithUTF8String:row[i]] forKey:[NSString stringWithUTF8String:fields[i].name]];
				}
				
				[rows addObject:currentRow];
			}
			
			mysql_free_result(result);
			if (r != nil)
				*r = rows;
		} 
	}
	
	return TRUE;
}

- (BOOL)fieldsForTable:(NSString*)table result:(NSMutableArray**)r {
	MYSQL_RES *result;
	MYSQL_ROW row;
	
	NSString* q = [NSString stringWithFormat:@"SELECT column_name FROM information_schema.columns WHERE table_name=\"%@\"", table];
	NSLog(@"fields querry: %@", q);
	if ( mysql_query(connection, [q UTF8String]) ) {
		NSLog(@"%s", mysql_error(connection));
		return FALSE;
	} else {
		result = mysql_store_result(connection);
		if (result) {
			int num_rows = mysql_num_rows(result);
			NSMutableArray* names = [NSMutableArray arrayWithCapacity:num_rows];
		
			while (row = mysql_fetch_row(result)) {
				[names addObject:[NSString stringWithUTF8String:row[0]]];
			}
				
			mysql_free_result(result);
			if (r != nil)
				*r = names;
		}
	}
		
	return TRUE;
}

- (NSString*)error {
	return [NSString stringWithUTF8String:mysql_error(connection)];
}

- (void)close {
	mysql_close(connection);
}

@end
