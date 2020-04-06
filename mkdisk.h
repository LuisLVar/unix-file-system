#ifndef MKDISK_H
#define MKDISK_H

#include <string>
#include <iostream>
#include "string.h"

using namespace std;

class mkDisk
{
public:
    mkDisk();
    void crearDisco(string size, string unit, string path);
};

#endif // MKDISK_H
