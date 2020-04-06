#include "recovery.h"
#include "archivo.h"
#include "carpeta.h"

Recovery::Recovery()
{

}


void Recovery::recoveryFS(string id, Mount montaje){
    string pathD= "";
    int sizeParticion;
    int startParticion;
    int error = 0;
    string nombreParticion;

    Mkfs mkfs;
    mkfs.getDatosID(id, montaje, &pathD, &startParticion, &sizeParticion, &nombreParticion, &error);

    if(error == 1){
        return;
    }

    int nEstructuras = (sizeParticion - (2*sizeof(Structs::SuperBloque)))/
            (27+sizeof(Structs::arbolVirtual)+ sizeof(Structs::detalleDirectorio) +
             5*(sizeof(Structs::InodoArchivo)) + 20*(sizeof(Structs::bloqueDatos)) + sizeof(Structs::logBitacora));


    Structs::SuperBloque superBloque;
    FILE *bfile = fopen(pathD.c_str(), "rb+");
    fseek(bfile, startParticion, SEEK_SET);
    fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

    int noAVD = nEstructuras;
    int noDD = nEstructuras;
    int noInodos = 5*nEstructuras;
    int noBloques = 20*nEstructuras;
    int noBitacora = nEstructuras;

    string fechaActual = mkfs.getFecha();


    //FORMATEAR, ASIGNAR ESPACIO A CADA SECTOR DE LA PARTICION
    //ASIGNAR SUPER BLOQUE.
    strcpy(superBloque.nombre_hd, nombreParticion.c_str());
    superBloque.arbol_virtual_count = noAVD;
    superBloque.detalle_directorio_count = noDD;
    superBloque.inodos_count = noInodos;
    superBloque.bloques_count = noBloques;
    superBloque.arbol_virtual_free = noAVD; //Se crea carpeta raiz.
    superBloque.detalle_directorio_free = noDD; //Detalle de directorio de la carpeta raiz
    superBloque.inodos_free = noInodos;
    superBloque.bloques_free = noBloques;
    strcpy(superBloque.date_creacion, fechaActual.c_str());
    strcpy(superBloque.date_ultimo_montaje, fechaActual.c_str());
    superBloque.montajes_count = 0;
    superBloque.start_bm_arbol_directorio = startParticion + sizeof(Structs::SuperBloque);
    superBloque.start_arbol_directorio = superBloque.start_bm_arbol_directorio + nEstructuras;
    superBloque.start_bm_detalle_directorio = superBloque.start_arbol_directorio + (nEstructuras*sizeof(Structs::arbolVirtual));
    superBloque.start_detalle_directorio = superBloque.start_bm_detalle_directorio + nEstructuras;
    superBloque.start_bm_inodos = superBloque.start_detalle_directorio + (nEstructuras * sizeof(Structs::detalleDirectorio));
    superBloque.start_inodos = superBloque.start_bm_inodos + (5*nEstructuras);
    superBloque.start_bm_bloques = superBloque.start_inodos + (5*nEstructuras*sizeof(Structs::InodoArchivo));
    superBloque.start_bloques = superBloque.start_bm_bloques + (20*nEstructuras);
    superBloque.start_log = superBloque.start_bloques + (20*nEstructuras*sizeof(Structs::bloqueDatos)); //Bitacora.
    superBloque.first_free_avd = 0;
    superBloque.first_free_dd = 0;
    superBloque.first_free_inodo = 0;
    superBloque.first_free_bloque = 0;


    Structs::logBitacora BitacoraRaiz;
    fseek(bfile, superBloque.start_log, SEEK_SET);
    fread(&BitacoraRaiz, sizeof(Structs::logBitacora), 1, bfile);

    fseek(bfile, startParticion, SEEK_SET);
    fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

    fclose(bfile);

    Structs::logBitacora bitacora;
    for(int i = 1; i< BitacoraRaiz.size; i++){
        FILE *bfile1 = fopen(pathD.c_str(), "rb+");
        fseek(bfile1, superBloque.start_log + (i*sizeof(Structs::logBitacora)), SEEK_SET);
        fread(&bitacora, sizeof(Structs::logBitacora), 1, bfile1);
        fclose(bfile1);

        if(string(bitacora.tipo_operacion) ==  "mkdir"){
            Carpeta folder;
            folder.makeDirectory(string(bitacora.path), bitacora.size, id, montaje, true);

        }else if(string(bitacora.tipo_operacion) ==  "mkfile"){
            Archivo file;
            file.makeFile(id, string(bitacora.path), 1, to_string(bitacora.size),  string(bitacora.contenido), montaje, true);
        }else{
            cout<<"Comando de operacion no reconocido"<<endl;
        }
    }

    cout << "Recuperacion del sistema exitosa." << endl;
}


void Recovery::simulateLoss(string id, Mount montaje){
    string pathD= "";
    int sizeParticion;
    int startParticion;
    int error = 0;
    string nombreParticion;

    Mkfs mkfs;
    mkfs.getDatosID(id, montaje, &pathD, &startParticion, &sizeParticion, &nombreParticion, &error);
    int nEstructuras = -1;

    nEstructuras = (sizeParticion - (2*sizeof(Structs::SuperBloque)))/
            (27+sizeof(Structs::arbolVirtual)+ sizeof(Structs::detalleDirectorio) +
             5*(sizeof(Structs::InodoArchivo)) + 20*(sizeof(Structs::bloqueDatos)) + sizeof(Structs::logBitacora));


    if(error == 1){
        return;
    }

    FILE *bfile = fopen(pathD.c_str(), "rb+");
    Structs::SuperBloque superBloque;
    fseek(bfile, startParticion, SEEK_SET);
    fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Escribo el superbloque al inicio de la particion

    //FORMATEO.
    //fseek(bfile, superBloque.start_bm_arbol_directorio, SEEK_SET); //bitmap de avd
    char cero = '0';
    for(int i = 0; i<nEstructuras; i++){
        fseek(bfile, (superBloque.start_bm_arbol_directorio + i), SEEK_SET);
        fwrite(&cero, sizeof(char), 1, bfile);
    }

    Structs::arbolVirtual avd;
    for(int i = 0; i<nEstructuras; i++){
        fseek(bfile, (superBloque.start_arbol_directorio + (i*sizeof(Structs::arbolVirtual))), SEEK_SET);//avd
        fwrite(&avd, sizeof(Structs::arbolVirtual), 1, bfile);
    }

    //fseek(bfile, superBloque.start_bm_detalle_directorio, SEEK_SET); //bitmap dd
    for(int i = 0; i<nEstructuras; i++){
        fseek(bfile, (superBloque.start_bm_detalle_directorio + i), SEEK_SET);//avd
        fwrite(&cero, sizeof(char), 1, bfile);
    }


    //fseek(bfile, superBloque.start_detalle_directorio, SEEK_SET); // dd
    Structs::detalleDirectorio dd;
    for(int i = 0; i<nEstructuras; i++){
        fseek(bfile, (superBloque.start_detalle_directorio + (i*sizeof(Structs::detalleDirectorio))), SEEK_SET);
        fwrite(&dd, sizeof(Structs::detalleDirectorio), 1, bfile);
    }


    //fseek(bfile, superBloque.start_bm_inodos, SEEK_SET); //bitmap inodos
    for(int i = 0; i<(5*nEstructuras); i++){
        fseek(bfile, (superBloque.start_bm_inodos + i), SEEK_SET);//avd
        fwrite(&cero, sizeof(char), 1, bfile);
    }

    //fseek(bfile, superBloque.start_inodos, SEEK_SET); //inodos
    Structs::InodoArchivo inodo;
    for(int i = 0; i<(5*nEstructuras); i++){
        fseek(bfile, (superBloque.start_inodos + (i*sizeof(Structs::InodoArchivo))), SEEK_SET);
        fwrite(&inodo, sizeof(Structs::InodoArchivo), 1, bfile);
    }

    //fseek(bfile, superBloque.start_bm_bloques, SEEK_SET); //bitmap bloques
    for(int i = 0; i<(20*nEstructuras); i++){
        fseek(bfile, (superBloque.start_bm_bloques + i), SEEK_SET);//avd
        fwrite(&cero, sizeof(char), 1, bfile);
    }


    //fseek(bfile, superBloque.start_bloques, SEEK_SET); //bloques
    Structs::bloqueDatos bloque;
    for(int i = 0; i<(20*nEstructuras); i++){
        fseek(bfile, (superBloque.start_bloques + (i*sizeof(Structs::bloqueDatos))), SEEK_SET);
        fwrite(&bloque, sizeof(Structs::bloqueDatos), 1, bfile);
    }

    //Actualizo la copia del superbloque
    int start_copia_sb = superBloque.start_log + (superBloque.arbol_virtual_count* sizeof(Structs::logBitacora));
    fseek(bfile, start_copia_sb, SEEK_SET); //copia super bloque
    fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile);

    //LIMPIO EL SUPERBLOQUE.
    Structs::SuperBloque sbVacio;
    sbVacio.start_log = superBloque.start_log;
    sbVacio.start_arbol_directorio = superBloque.start_arbol_directorio;
    sbVacio.start_bm_arbol_directorio = superBloque.start_bm_arbol_directorio;
    sbVacio.start_bloques = superBloque.start_bloques;
    sbVacio.start_bm_bloques =superBloque.start_bm_bloques;
    sbVacio.start_bm_detalle_directorio = superBloque.start_bm_detalle_directorio;
    sbVacio.start_bm_inodos = superBloque.start_bm_inodos;
    sbVacio.start_detalle_directorio = superBloque.start_detalle_directorio;
    sbVacio.start_inodos = superBloque.start_inodos;
    sbVacio.inodos_count = superBloque.inodos_count;
    sbVacio.bloques_count = superBloque.bloques_count;
    sbVacio.arbol_virtual_count = superBloque.arbol_virtual_count;
    sbVacio.detalle_directorio_count = superBloque.detalle_directorio_count;

    fseek(bfile, startParticion, SEEK_SET);
    fwrite(&sbVacio, sizeof(Structs::SuperBloque), 1, bfile); //Escribo el superbloque al inicio de la particion

    fclose(bfile);

    cout << "Simulacion de perdida efectuada con exito." << endl;

}
