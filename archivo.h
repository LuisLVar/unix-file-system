#ifndef ARCHIVO_H
#define ARCHIVO_H

#include <string>
#include "mount.h"
#include "mkfs.h"
#include "carpeta.h"
#include <cmath>

using namespace std;


class Archivo
{
public:
    Archivo();
    void makeFile(string id, string path, int p, string size, string cont, Mount montaje, bool esBitacora);
    void showContent(string file); //CAT
    void removeFile(string path);
    void editFile(string path, string cont); //Append

    //CREAR ARCHIVOS
    void recorrerRuta(Structs::arbolVirtual avd, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, int size, string cont);
    void crearArchivo(Structs::detalleDirectorio Archivos, int apuntador, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, int size, string cont);
    void crearInodo(Structs::InodoArchivo inodo, string path, string pathDisco, Structs::SuperBloque superBloque, string cont, int cantidadBloques);
};

#endif // ARCHIVO_H
