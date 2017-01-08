//
// Created by Helana on 12/23/16.
//

#ifndef ENHANCEDUDPSERVER_PLP_H
#define ENHANCEDUDPSERVER_PLP_H

#include <stdlib.h>
#include <UdpLibGlobaldefs.h>


/**
 * Calculates the probability that will determine if a packet will be lost
 * @return whether a packet will be lost
 */
int will_be_sent()
{
    return ((float) rand() / (float) (RAND_MAX)) < LOSS_THRESH;
}


#endif //ENHANCEDUDPSERVER_PLP_H
