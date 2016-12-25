//
// Created by ahmed on 12/25/16.
//

#ifndef ENHANCEDUDPSERVER_GLOBALDEF_H
#define ENHANCEDUDPSERVER_GLOBALDEF_H

#define SLEEP_MILLIS 10    // Avoid packet corruption in the OS

// MAX UDP transfer is 256 bytes,
// don't allocate lots of memory, stack size runs out
#define FRAGMENT_SIZE 64
//#define PCKT_DATA_SZ 128

#define MAX_FAIL_COUNT 10
#define SERV_FILESZ_HEADER "FILESIZE-"
#define WIN_SZ 5
/**
 * Redirection message, this is a constant message on the client and server side
 */
#define REDIRECT_SUCCESS "REDIRECT SUCCESSFUL"
#define SERV_REDIRECT_OK "OK"
#define MAX_FILE_PATH_LENGTH 256

#endif //ENHANCEDUDPSERVER_GLOBALDEF_H
