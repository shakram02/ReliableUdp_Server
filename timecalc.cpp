//
// Created by ahmed on 12/18/16.
//
#include <time.h>
#include <malloc.h>

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