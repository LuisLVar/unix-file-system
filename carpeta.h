#ifndef CARPETA_H
#define CARPETA_H
#include <string>
#include "mount.h"
#include "structs.h"
#include "mkfs.h"

using namespace std;


class Carpeta
{
public:
    Carpeta();
    void makeDirectory(string path, int p, string id, Mount montaje, bool esBitacora);
    void copyFile(string path, string dest, Mount montaje, string id);
    void moveFile(string path, string dest, Mount montaje);
    void renameFile(string id, string path, string name, Mount montaje);
    void findFile(string path, string name, Mount montaje);

    //CREAR CARPETAS
    void recorrerRuta(Structs::arbolVirtual avd, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, string nombre, int pointer);
    void crearCarpeta(Structs::arbolVirtual avd, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, int pointer);
    Structs::arbolVirtual getAVD(Structs::arbolVirtual avd, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, int pointer);
    int getintAVD(Structs::arbolVirtual avd, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, int pointer);
};

#endif // CARPETA_H
