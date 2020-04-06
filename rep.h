#ifndef REP_H
#define REP_H

#include "interprete.h"

class Rep
{
public:
    Rep();
    void crearReporte(string path, string name, string id, Mount montaje, string ruta);
    void getDirectorio(Structs::arbolVirtual avd, string pathD, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, int apuntador);
    void getTreeFile(Structs::arbolVirtual avd, string pathD, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, int pointer, vector<string> ruta);
    void recorrerDetalle(Structs::detalleDirectorio Archivos, int apuntador, vector<string> path,
                              string pathDisco, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, string nombreCarpeta);
    void recorrerInodo(Structs::InodoArchivo inodo, Structs::SuperBloque superBloque, string pathDisco, string * codigo, string * codigoEnlaces, string nombre);
    void getTreeComplete(Structs::arbolVirtual avd, string pathD, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, int pointer);
    void recorrerDetalleComplete(Structs::detalleDirectorio Archivos, int apuntador,
                              string pathDisco, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, string nombreCarpeta);
    void recorrerDetalleDirectorio(Structs::detalleDirectorio Archivos, int apuntador,
                                        string pathDisco, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, string nombreCarpeta);
    void getTreeDirectorio(Structs::arbolVirtual avd, string pathD, Structs::SuperBloque superBloque,
                                string * codigo, string * codigoEnlaces, int pointer, vector<string> path);

};


#endif // REP_H
