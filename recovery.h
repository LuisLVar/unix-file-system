#ifndef RECOVERY_H
#define RECOVERY_H

#include "mkfs.h"

class Recovery
{
public:
    Recovery();
    void recoveryFS(string id, Mount montaje);
    void simulateLoss(string id, Mount montaje);
};

#endif // RECOVERY_H
