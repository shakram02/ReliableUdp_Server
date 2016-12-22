//
// Created by ahmed on 12/18/16.
//

#ifndef ENHANCEDUDPSERVER_TIMECALC_H
#define ENHANCEDUDPSERVER_TIMECALC_H

#include <time.h>
#include <malloc.h>
/**
 * Prints the current time
 * @return String containing the current time
 */
char *get_time()
{
    time_t rawtime;
    struct tm *timeinfo;
    int time_length = 20;
    char *buffer = (char *) calloc(time_length, sizeof(char));

    // TODO use calloc everywhere

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 20, "%T %p", timeinfo);
    return buffer;
}

#endif //ENHANCEDUDPSERVER_TIMECALC_H
