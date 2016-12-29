//
// Created by Helana on 12/23/16.
//

#ifndef ENHANCEDUDPSERVER_PLP_H
#define ENHANCEDUDPSERVER_PLP_H

#include <stdlib.h>

#define LOSS_THRESH 0.8

/**
 * Calculates the probability that will determine if a packet will be lost
 * @return whether a packet will be lost
 */
int will_be_lost()
{
    return ((float) rand() / (float) (RAND_MAX)) < LOSS_THRESH;
}


#endif //ENHANCEDUDPSERVER_PLP_H
