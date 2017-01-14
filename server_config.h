//
// Created by ahmed on 12/25/16.
//

#ifndef ENHANCEDUDPSERVER_GLOBALDEF_H
#define ENHANCEDUDPSERVER_GLOBALDEF_H


// MAX UDP transfer is 256 bytes,
// don't allocate lots of memory, stack size runs out
//#define FRAGMENT_SIZE 64

//#define SERVER_IP "192.168.1.7"
#define SERVER_IP "127.0.0.1"
#define PORT_NUM 62135

#define MAX_FAIL_COUNT 8
#define SERV_FILESZ_HEADER "FILESIZE-"
#define WND_SIZE 4
/**
 * Redirection message, this is a constant message on the client and server side
 */
#define REDIRECT_SUCCESS "REDIRECT SUCCESSFUL"
#define SERV_REDIRECT_OK "OK"
#define MAX_FILE_PATH_LENGTH 256

#define TIMEO_REDIRECT_MILLIS 20

#define TO_MILLISEC 1000
#define TIMEO_SEC 0
#define TIMEO_MICRO_SEC TO_MILLISEC * 5

#define PCKT_SLEEP 5

#define LOSS_THRESH 1
#endif //ENHANCEDUDPSERVER_GLOBALDEF_H
