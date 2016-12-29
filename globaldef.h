//
// Created by ahmed on 12/25/16.
//

#ifndef ENHANCEDUDPSERVER_GLOBALDEF_H
#define ENHANCEDUDPSERVER_GLOBALDEF_H

#define SLEEP_MILLIS 10    // Avoid packet corruption in the OS

// MAX UDP transfer is 256 bytes,
// don't allocate lots of memory, stack size runs out
#define FRAGMENT_SIZE 64

#define MAX_FAIL_COUNT 2
#define SERV_FILESZ_HEADER "FILESIZE-"
#define WND_SZ 8
/**
 * Redirection message, this is a constant message on the client and server side
 */
#define REDIRECT_SUCCESS "REDIRECT SUCCESSFUL"
#define SERV_REDIRECT_OK "OK"
#define MAX_FILE_PATH_LENGTH 256


#define TO_MILLISEC 1000
#define TIMEO_SEC 0
#define TIMEO_MICRO_SEC TO_MILLISEC * 2

#define LOSS_THRESH 0.7
#endif //ENHANCEDUDPSERVER_GLOBALDEF_H
