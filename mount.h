#ifndef MOUNT_H
#define MOUNT_H

#include "structs.h"
#include <string>
#include <string.h>
#include <iostream>

using namespace std;

class Mount
{
public:
    Mount();

    typedef struct ParticionM
    {
        int numero = 0;
        int estado = 0;
        char name[16] = "";
    } particionMontada;

    typedef struct discoM
    {
        char path[100] = "";
        char letra;
        int estado = 0;
        particionMontada particiones[99];
    } discoMontado;

    discoMontado discos[26];

    void montarParticion(string path, string name);
    void leerMontajes();
    void desmontarParticion(string id);
};

#endif // MOUNT_H
