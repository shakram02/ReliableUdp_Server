//
// Created by ahmed on 12/24/16.
//

#ifndef ENHANCEDUDPSERVER_PATHLOSS_H
#define ENHANCEDUDPSERVER_PATHLOSS_H


#include <stdlib.h>

class PathLossProbability
{
    /**
     * Calculates the probability that will determine if a packet will be lost
     * @return whether a packet will be lost
     */
    static bool GetProbability();

    const static float loss_threshold = 0.8;
};


//
// Created by Helana on 12/23/16.
//


bool PathLossProbability::GetProbability()
{
    return ((float) rand() / (float) (RAND_MAX)) < PathLossProbability::loss_threshold;
}

#endif //ENHANCEDUDPSERVER_PATHLOSS_H
