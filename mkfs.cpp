#include "mkfs.h"

Mkfs::Mkfs()
{
}

void Mkfs::formatearFS(string id, string type, Mount montaje)
{

    int nEstructuras = -1;
    string pathD= "";
    int sizeParticion;
    int startParticion;
    int error = 0;
    string nombreParticion;

    Mkfs::getDatosID(id, montaje, &pathD, &startParticion, &sizeParticion, &nombreParticion, &error);

    if(error == 1){
        return;
    }

    char pathDisco[100] = "";
    strcpy(pathDisco,  pathD.c_str());


    //FORMULA.

    nEstructuras = (sizeParticion - (2*sizeof(Structs::SuperBloque)))/
            (27+sizeof(Structs::arbolVirtual)+ sizeof(Structs::detalleDirectorio) +
             5*(sizeof(Structs::InodoArchivo)) + 20*(sizeof(Structs::bloqueDatos)) + sizeof(Structs::logBitacora));

    int noAVD = nEstructuras;
    int noDD = nEstructuras;
    int noInodos = 5*nEstructuras;
    int noBloques = 20*nEstructuras;
    int noBitacora = nEstructuras;

    string fechaActual = Mkfs::getFecha();


    //FORMATEAR, ASIGNAR ESPACIO A CADA SECTOR DE LA PARTICION
    //ASIGNAR SUPER BLOQUE.
    Structs::SuperBloque superBloque;
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


    FILE *bfile = fopen(pathDisco, "rb+");
    fseek(bfile, startParticion, SEEK_SET);
    fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Escribo el superbloque al inicio de la particion

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


    //fseek(bfile, superBloque.start_log, SEEK_SET); //bitacora
    Structs::logBitacora log;
    for(int i = 0; i < nEstructuras; i++){
        fseek(bfile, (superBloque.start_log + (i*sizeof(Structs::logBitacora))), SEEK_SET);
        fwrite(&log, sizeof(Structs::logBitacora), 1, bfile);
    }

    //SETEO LA RAIZ DE LAS BITACORAS.
    log.size = 1;
    fseek(bfile, (superBloque.start_log), SEEK_SET);
    fwrite(&log, sizeof(Structs::logBitacora), 1, bfile);


    int start_copia_sb = superBloque.start_log + (nEstructuras* sizeof(Structs::logBitacora));
    fseek(bfile, start_copia_sb, SEEK_SET); //copia super bloque
    fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile);


    fclose(bfile);

    //CREAR RAIZ
    Carpeta folder;
    folder.makeDirectory("/", 0, id, montaje, false);
}

void Mkfs::getDatosID(string id, Mount montaje, string * path, int *inicioParticion, int * sizePart, string *nombrePart, int * error){
    //Obtener particion y su size.
    char letra = id.c_str()[2];
    char numero = id.c_str()[3];
    bool existePath = false;
    int numeroInt = (int)numero - 48;
    string nombreParticion = "";

    char pathDisco[100] = "";

    for (int i = 0; i < 26; i++)
    {
        if (montaje.discos[i].letra == letra && montaje.discos[i].particiones[numeroInt-1].estado == 1)
        {
            strcpy(pathDisco, montaje.discos[i].path);
            nombreParticion = montaje.discos[i].particiones[numeroInt-1].name;
            existePath = true;
            break;
        }
    }

    if (!existePath)
    {
        cout << "Error: id no existe, path no existente." << endl;
        return;
    }
    int sizeParticion = -1;
    int startParticion = -1;

    FILE *bfile1 = fopen(pathDisco, "rb+");
    if(bfile1 != NULL){
        rewind(bfile1);
        Structs::MBR discoLeer;
        fread(&discoLeer, sizeof(Structs::MBR), 1, bfile1);
        for(int i = 0; i < 4; i++){
            if(discoLeer.mbr_particiones[i].name == nombreParticion){
                sizeParticion = discoLeer.mbr_particiones[i].size;
                startParticion = discoLeer.mbr_particiones[i].part_start;
            }
        }

        fclose(bfile1);
    }else{
        cout << "Error: Path no existente." << endl;
        *error = 1;
        return;
    }

    *nombrePart = string(nombreParticion);
    *error = 0;
    *path = string(pathDisco);
    *inicioParticion = startParticion;
    *sizePart = sizeParticion;
}

string Mkfs::getFecha(){
    //fecha
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    string fechaActual(buffer);
    return fechaActual;
}

