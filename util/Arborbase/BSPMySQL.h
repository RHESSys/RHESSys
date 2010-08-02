//
//  BSPMySQL.h
//  Arborbase
//
//  Created by David Groulx on 10/7/22.
//  Copyright 2010 UCSB. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <mysql.h>

@interface BSPMySQL : NSObject {
	MYSQL *connection;
		
	BOOL errorAlert;
	char *server;
	char *user;
	char *password;
	char *database;
}

- (void)connect;
- (BOOL)query:(NSString*)q result:(NSMutableArray**)r;
- (BOOL)fieldsForTable:(NSString*)table result:(NSMutableArray**)r;
- (NSString*)error;
- (void)close;

@end
