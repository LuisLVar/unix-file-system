#ifndef MKFS_H
#define MKFS_H

#include "mount.h"
#include "interprete.h"
#include "structs.h"
#include "carpeta.h"
#include "archivo.h"

class Mkfs
{
public:
    Mkfs();
    void formatearFS(string id, string type, Mount montaje);
    void getDatosID(string id, Mount montaje, string * path, int *inicioParticion, int * sizePart, string * nombrePart, int * error);
    string getFecha();
};

#endif // MKFS_H
