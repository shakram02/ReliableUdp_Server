//
// Created by ahmed on 12/25/16.
//

#ifndef ENHANCEDUDPSERVER_GLOBALDEF_H
#define ENHANCEDUDPSERVER_GLOBALDEF_H


// MAX UDP transfer is 256 bytes,
// don't allocate lots of memory, stack size runs out
//#define FRAGMENT_SIZE 64


#define WND_SIZE 4
#define MAX_FAIL_COUNT 8


#define TIMEO_REDIRECT_MILLIS 20

#define TO_MILLISEC 1000
#define TIMEO_SEC 0
#define TIMEO_MICRO_SEC TO_MILLISEC * 5

//#define PCKT_SLEEP 500

#define LOSS_THRESH 1
#endif //ENHANCEDUDPSERVER_GLOBALDEF_H
