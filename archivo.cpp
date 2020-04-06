#include "archivo.h"

Archivo::Archivo()
{

}

void Archivo::makeFile(string id, string path, int p, string size, string cont, Mount montaje, bool esBitacora){

    string pathD= "";
    int sizeParticion;
    int startParticion;
    int error = 0;
    string nombreParticion;
    int numSize = 0;

    if(size != ""){
        numSize = stoi(size);
    }else{
        numSize = 0;
    }

    Mkfs mkfs;
    mkfs.getDatosID(id, montaje, &pathD, &startParticion, &sizeParticion, &nombreParticion, &error);

    if(error == 1){
        return;
    }

    Structs::SuperBloque superBloque;
    FILE *bfile = fopen(pathD.c_str(), "rb+");
    fseek(bfile, startParticion, SEEK_SET);
    fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

    vector<string> pathArray;
    stringstream total(path);
    string tmp;

    while (getline(total, tmp, '/'))
    {
        if(tmp!=""){
            pathArray.push_back(tmp);
        }
    }

    Structs::arbolVirtual raiz;
    fseek(bfile, superBloque.start_arbol_directorio, SEEK_SET);
    fread(&raiz, sizeof(Structs::arbolVirtual), 1, bfile);

    vector<string> pathCarpeta = pathArray;
    pathCarpeta.pop_back();


    //Nos aseguramos de crear toda la ruta hacia el archivo.
    fclose(bfile);
    Carpeta folder;
    folder.crearCarpeta(raiz, pathCarpeta, pathD, superBloque, 0);// Aqui se edita el superbloque.

    FILE *bfile1 = fopen(pathD.c_str(), "rb+");
    fseek(bfile1, startParticion, SEEK_SET);
    fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile1); //Leo el superbloque al inicio de la particion

    fseek(bfile1, superBloque.start_arbol_directorio, SEEK_SET);
    fread(&raiz, sizeof(Structs::arbolVirtual), 1, bfile);

    fclose(bfile1);
    recorrerRuta(raiz, pathArray, pathD, superBloque, numSize, cont);

    if(!esBitacora){
        string fechaActual = mkfs.getFecha();

        Structs::logBitacora bitacora;
        strcpy(bitacora.path, path.c_str());
        bitacora.tipo = '1';
        strcpy(bitacora.tipo_operacion, "mkfile");
        bitacora.size = stoi(size);
        strcpy(bitacora.log_fecha, fechaActual.c_str());
        if(cont.size()>50){
            strcpy(bitacora.contenido, cont.substr(0, 49).c_str());
        }else{
            strcpy(bitacora.contenido, cont.c_str());
        }


        FILE *bfile2 = fopen(pathD.c_str(), "rb+");

        Structs::logBitacora BitacoraRaiz;
        fseek(bfile2, superBloque.start_log, SEEK_SET);
        fread(&BitacoraRaiz, sizeof(Structs::logBitacora), 1, bfile2);

        //Lo escribimos en el first free de la bitacora.
        fseek(bfile2, superBloque.start_log + (BitacoraRaiz.size * sizeof(Structs::logBitacora)), SEEK_SET);
        fwrite(&bitacora, sizeof(Structs::logBitacora), 1, bfile2);

        BitacoraRaiz.size++;
        fseek(bfile2, superBloque.start_log, SEEK_SET);
        fwrite(&BitacoraRaiz, sizeof(Structs::logBitacora), 1, bfile2);

        fclose(bfile2);
    }
}


//CREAR ARCHIVOS.
void Archivo::recorrerRuta(Structs::arbolVirtual avd, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, int size, string cont){

    FILE *bfile = fopen(pathDisco.c_str(), "rb+");
    char uno = '1';
    Mkfs mkfs;
    int apuntador = 0;
    for(int i = 0; i<6; i++){
        apuntador = avd.array_subdirectorios[i];
        Structs::arbolVirtual carpetaHijo;
        fseek(bfile, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
        fread(&carpetaHijo, sizeof(Structs::arbolVirtual), 1, bfile);

        if(carpetaHijo.nombre_directorio == path[0]){
            path.erase(path.begin());  
            if(path.size() == 1){
                apuntador = carpetaHijo.detalle_directorio;
                Structs::detalleDirectorio Archivos;
                fseek(bfile, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
                fread(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);
                fclose(bfile);
                crearArchivo(Archivos, apuntador, path, pathDisco, superBloque, size, cont);
                return;
            }else{
                fclose(bfile);
                recorrerRuta(carpetaHijo, path, pathDisco, superBloque, size, cont);
                return;
            }
        }
    }

    //NINGUNO CUMPLE, SE MUEVE AL APUNTADOR INDIRECTO
    apuntador = avd.avd_siguiente;
    Structs::arbolVirtual carpetaIndirecta;
    fseek(bfile, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
    fread(&carpetaIndirecta, sizeof(Structs::arbolVirtual), 1, bfile);

    fclose(bfile);
    recorrerRuta(carpetaIndirecta, path, pathDisco, superBloque, size, cont);
    return;
}


void Archivo::crearArchivo(Structs::detalleDirectorio Archivos, int apuntador, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, int size, string cont){
    char uno = '1';
    Mkfs mkfs;
    FILE *bfile = fopen(pathDisco.c_str(), "rb+");

    for(int i = 0; i<5; i++){
        int pointer = Archivos.archivos[i].noInodo;
        if(pointer == -1){ //Aqui escribo el archivo.
            Structs::detalleArchivo detalle;
            strcpy(detalle.date_creacion, mkfs.getFecha().c_str());
            strcpy(detalle.date_modificacion, mkfs.getFecha().c_str());
            detalle.noInodo = superBloque.first_free_inodo;
            strcpy(detalle.nombre_directorio, path[0].c_str());

            //Asigno el detalle de archivo al detalle de directorio
            Archivos.archivos[i] = detalle;

            //Escribimos el detalle de directorio.
            fseek(bfile, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
            fwrite(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);

            int pointInode = superBloque.first_free_inodo;

            //Seteamos el inodo.
            Structs::InodoArchivo inodo;
            inodo.count_inodo = pointInode;
            inodo.size_archivo = size;
            int cantidadBloques = floor((double) size/25 +0.99);
            if(cantidadBloques <= 4){
                inodo.count_bloques_asignados = cantidadBloques;
            }else{
                inodo.count_bloques_asignados = 4;
            }

            superBloque.inodos_free--;
            superBloque.first_free_inodo++;

            //Escribimos en el bitmap de inodo.
            fseek(bfile, (superBloque.start_bm_inodos + inodo.count_inodo), SEEK_SET);
            fwrite(&uno, sizeof(char), 1, bfile);

            //Escribimos el SB
            fseek(bfile, (superBloque.start_bm_arbol_directorio - sizeof(Structs::SuperBloque)), SEEK_SET);
            fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile);

            fclose(bfile);
            crearInodo(inodo, path[0], pathDisco, superBloque, cont, cantidadBloques);
            return;
        }
    }

    //Si no lo encuentra se va al apuntador indirecto.
    int apuntadorA = Archivos.dd_siguiente;
    if(apuntadorA != -1){
        Structs::detalleDirectorio detalleIndirecto;
        fseek(bfile, (superBloque.start_detalle_directorio+(apuntadorA*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en la carpeta hija
        fread(&detalleIndirecto, sizeof(Structs::detalleDirectorio), 1, bfile);

        fclose(bfile);
        crearArchivo(detalleIndirecto, apuntadorA, path, pathDisco, superBloque, size, cont);
        return;
    }else{
        Structs::detalleDirectorio nuevoIndirecto;

        int posicion = superBloque.start_detalle_directorio + (superBloque.first_free_dd * sizeof(Structs::detalleDirectorio));
        apuntadorA = superBloque.first_free_dd;
        Archivos.dd_siguiente = superBloque.first_free_dd;

        //Escribimos el DD indirecto
        fseek(bfile, posicion, SEEK_SET);
        fwrite(&nuevoIndirecto, sizeof(Structs::detalleDirectorio), 1, bfile);
        //Escribimos en el bitmap dd
        fseek(bfile, (superBloque.start_bm_detalle_directorio + superBloque.first_free_dd), SEEK_SET); //Ocupamos su posicion en bitmap.
        fwrite(&uno, sizeof(char), 1, bfile);

        superBloque.detalle_directorio_free--;
        superBloque.first_free_dd++;

        //Escribimos el detalle de directorio raiz.
        fseek(bfile, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
        fwrite(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);

        //Escribimos el SB
        fseek(bfile, (superBloque.start_bm_arbol_directorio - sizeof(Structs::SuperBloque)), SEEK_SET);
        fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile);

        fclose(bfile);
        crearArchivo(nuevoIndirecto, apuntadorA, path, pathDisco, superBloque, size, cont);
        return;
    }
}

void Archivo::crearInodo(Structs::InodoArchivo inodo, string nombre, string pathDisco, Structs::SuperBloque superBloque, string cont, int bloques){
    FILE *bfile = fopen(pathDisco.c_str(), "rb+");
    char uno = '1';
    string abc = "abcdefghijklmnopqrstuvwxy";
    for(int i = 0; i<inodo.count_bloques_asignados  ; i++){

        if(cont == ""){
            cont = abc;
        }

        //Creamos nuevo bloque.
        Structs::bloqueDatos block;
        strcpy(block.db_data, cont.substr(0, 25).c_str());
        if(cont.size() > 25){
            cont = cont.substr(25, cont.size());
        }else{
            cont = "";
        }

        inodo.array_bloques[i] = superBloque.first_free_bloque;

        //Escribimos el bloque.
        fseek(bfile, (superBloque.start_bloques + (superBloque.first_free_bloque * sizeof(Structs::bloqueDatos))), SEEK_SET);
        fwrite(&block, sizeof(Structs::bloqueDatos), 1, bfile);

        //Escribimos en el bitmap de bloques
        fseek(bfile, (superBloque.start_bm_bloques + superBloque.first_free_bloque), SEEK_SET);
        fwrite(&uno, sizeof(char), 1, bfile);

        //Escribimos el inodo.
        fseek(bfile, (superBloque.start_inodos + (inodo.count_inodo*(sizeof(Structs::InodoArchivo)))), SEEK_SET);
        fwrite(&inodo, sizeof(Structs::InodoArchivo), 1, bfile);

        superBloque.bloques_free--;
        superBloque.first_free_bloque++;
        //Escribimos el SB
        fseek(bfile, (superBloque.start_bm_arbol_directorio - sizeof(Structs::SuperBloque)), SEEK_SET);
        fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile);
    }

    if(bloques > 4){
        bloques = bloques - 4;

        Structs::InodoArchivo inodoIndirecto;
        inodoIndirecto.count_inodo = superBloque.first_free_inodo;
        inodo.ap_indirecto = inodoIndirecto.count_inodo;
        inodoIndirecto.size_archivo = inodo.size_archivo;

        superBloque.inodos_free--;
        superBloque.first_free_inodo++;

        //Escribimos el inodo.
        fseek(bfile, (superBloque.start_inodos + (inodo.count_inodo*(sizeof(Structs::InodoArchivo)))), SEEK_SET);
        fwrite(&inodo, sizeof(Structs::InodoArchivo), 1, bfile);

        //Escribimos en el bitmap de inodo.
        fseek(bfile, (superBloque.start_bm_inodos + inodoIndirecto.count_inodo), SEEK_SET);
        fwrite(&uno, sizeof(char), 1, bfile);

        //Escribimos el SB
        fseek(bfile, (superBloque.start_bm_arbol_directorio - sizeof(Structs::SuperBloque)), SEEK_SET);
        fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile);

        if(bloques <= 4){
            inodoIndirecto.count_bloques_asignados = bloques;
        }else{
            inodoIndirecto.count_bloques_asignados = 4;
        }

        fclose(bfile);
        crearInodo(inodoIndirecto, nombre, pathDisco, superBloque, cont, bloques);
        return;
    }

    fclose(bfile);

}


void Archivo::showContent(string file){ //CAT

}
void Archivo::removeFile(string path){

}
void Archivo::editFile(string path, string cont){//Append

}
