#include "carpeta.h"

Carpeta::Carpeta()
{

}

bool copiarArchivo = false;

void Carpeta::makeDirectory(string path, int p, string id, Mount montaje, bool esBitacora)
{
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

    Structs::SuperBloque superBloque;
    FILE *bfile = fopen(pathD.c_str(), "rb+");
    fseek(bfile, startParticion, SEEK_SET);
    fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion



    char uno = '1';
    if(path == "/"){

        Structs::arbolVirtual carpeta;
        strcpy(carpeta.nombre_directorio, "raiz");
        strcpy(carpeta.date_creacion, mkfs.getFecha().c_str());
        carpeta.detalle_directorio = superBloque.first_free_dd;
        fseek(bfile, superBloque.start_arbol_directorio, SEEK_SET); //Escribimos el avd raiz
        fwrite(&carpeta, sizeof(Structs::arbolVirtual), 1, bfile);
        fseek(bfile, superBloque.start_bm_arbol_directorio, SEEK_SET); //Ocupamos su posicion en bitmap.
        fwrite(&uno, sizeof(char), 1, bfile);
        superBloque.arbol_virtual_free--;
        superBloque.first_free_avd++;

        //Escribiendo el detalle de directorio.
        Structs::detalleDirectorio detalle;
        fseek(bfile, superBloque.start_detalle_directorio, SEEK_SET);
        fwrite(&detalle, sizeof(Structs::detalleDirectorio), 1, bfile);
        fseek(bfile, superBloque.start_bm_detalle_directorio, SEEK_SET); //Ocupamos su posicion en bitmap.
        fwrite(&uno, sizeof(char), 1, bfile);
        superBloque.detalle_directorio_free--;
        superBloque.first_free_dd++;
        fseek(bfile, startParticion, SEEK_SET);
        fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile);
        fclose(bfile);
    }else{

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
        //FILE *bfile = fopen(pathD.c_str(), "rb+");
        fseek(bfile, superBloque.start_arbol_directorio, SEEK_SET);
        fread(&raiz, sizeof(Structs::arbolVirtual), 1, bfile);

        fclose(bfile);

        crearCarpeta(raiz, pathArray, pathD, superBloque, 0);
        //VALIDAR P
    }

    if(!esBitacora){
        string fechaActual = mkfs.getFecha();

        Structs::logBitacora bitacora;
        strcpy(bitacora.path, path.c_str());
        bitacora.tipo = '0';
        strcpy(bitacora.tipo_operacion, "mkdir");
        bitacora.size = p;
        strcpy(bitacora.log_fecha, fechaActual.c_str());


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


void Carpeta::crearCarpeta(Structs::arbolVirtual avd, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, int pointer){
    FILE *bfile = fopen(pathDisco.c_str(), "rb+");
    char uno = '1';
    Mkfs mkfs;

    if(path.empty()){
        fclose(bfile);
        return;
    }
    int apuntador = 0;
    int pointerAvd = 0;
    for(int i = 0; i<6; i++){
        apuntador = avd.array_subdirectorios[i];

        if(apuntador == -1){
            Structs::arbolVirtual carpeta;
            strcpy(carpeta.nombre_directorio, path[0].c_str());
            strcpy(carpeta.date_creacion, mkfs.getFecha().c_str());
            carpeta.detalle_directorio = superBloque.first_free_dd;
            avd.array_subdirectorios[i] = superBloque.first_free_avd;
            pointerAvd = superBloque.first_free_avd;
            int posicion = superBloque.start_arbol_directorio + (superBloque.first_free_avd * sizeof(Structs::arbolVirtual));
            fseek(bfile, posicion, SEEK_SET); //Escribimos el avd
            fwrite(&carpeta, sizeof(Structs::arbolVirtual), 1, bfile);
            fseek(bfile, (superBloque.start_bm_arbol_directorio + superBloque.first_free_avd), SEEK_SET); //Ocupamos su posicion en bitmap.
            fwrite(&uno, sizeof(char), 1, bfile);
            superBloque.arbol_virtual_free--;
            superBloque.first_free_avd++;

            //Reescribo avd actual -AQUI ESTA MALO BRO.
            fseek(bfile, superBloque.start_arbol_directorio + (pointer * sizeof(Structs::arbolVirtual)), SEEK_SET); //Escribimos el avd
            fwrite(&avd, sizeof(Structs::arbolVirtual), 1, bfile);


            //Escribiendo el detalle de directorio.
            fseek(bfile, (superBloque.start_bm_detalle_directorio + superBloque.first_free_dd), SEEK_SET); //Ocupamos su posicion en bitmap.
            fwrite(&uno, sizeof(char), 1, bfile);
            superBloque.detalle_directorio_free--;
            superBloque.first_free_dd++;
            int posSB= superBloque.start_bm_arbol_directorio - sizeof(Structs::SuperBloque);
            fseek(bfile, posSB, SEEK_SET);
            fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile);

//            fseek(bfile, (superBloque.start_bm_arbol_directorio - sizeof(Structs::SuperBloque)), SEEK_SET);
//            Structs::SuperBloque sb;
//            fread(&sb, sizeof(Structs::SuperBloque), 1, bfile);

            path.erase(path.begin());
            fclose(bfile);
            crearCarpeta(carpeta, path, pathDisco, superBloque, pointerAvd);
            return;
        }else{
            Structs::arbolVirtual carpetaHijo;
            fseek(bfile, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
            fread(&carpetaHijo, sizeof(Structs::arbolVirtual), 1, bfile);
            if(carpetaHijo.nombre_directorio == path[0]){
                path.erase(path.begin());
                fclose(bfile);
                crearCarpeta(carpetaHijo, path, pathDisco, superBloque, apuntador);
                return;
            }
        }
    }

    //NINGUNO CUMPLE, SE MUEVE AL APUNTADOR INDIRECTO
    apuntador = avd.avd_siguiente;
    if(apuntador == -1){
            Structs::arbolVirtual carpeta;
            strcpy(carpeta.nombre_directorio, avd.nombre_directorio);
            strcpy(carpeta.date_creacion, mkfs.getFecha().c_str());
            carpeta.detalle_directorio = superBloque.first_free_dd;
            avd.avd_siguiente = superBloque.first_free_avd;
            pointerAvd = superBloque.first_free_avd;
            int posicion = superBloque.start_arbol_directorio + (superBloque.first_free_avd * sizeof(Structs::arbolVirtual));
            fseek(bfile, posicion, SEEK_SET); //Escribimos el avd
            fwrite(&carpeta, sizeof(Structs::arbolVirtual), 1, bfile);
            fseek(bfile, (superBloque.start_bm_arbol_directorio + superBloque.first_free_avd), SEEK_SET); //Ocupamos su posicion en bitmap.
            fwrite(&uno, sizeof(char), 1, bfile);
            superBloque.arbol_virtual_free--;
            superBloque.first_free_avd++;

            //Reescribo avd actual
            fseek(bfile, superBloque.start_arbol_directorio + (pointer * sizeof(Structs::arbolVirtual)), SEEK_SET); //Escribimos el avd
            fwrite(&avd, sizeof(Structs::arbolVirtual), 1, bfile);

            //Escribiendo el detalle de directorio.
            fseek(bfile, (superBloque.start_bm_detalle_directorio + superBloque.first_free_dd), SEEK_SET); //Ocupamos su posicion en bitmap.
            fwrite(&uno, sizeof(char), 1, bfile);
            superBloque.detalle_directorio_free--;
            superBloque.first_free_dd++;
            fseek(bfile, (superBloque.start_bm_arbol_directorio - sizeof(Structs::SuperBloque)), SEEK_SET);
            fwrite(&superBloque, sizeof(Structs::SuperBloque), 1, bfile);

            fclose(bfile);
            crearCarpeta(carpeta, path, pathDisco, superBloque, pointerAvd);
            return;
        }else{
            Structs::arbolVirtual carpetaIndirecta;
            fseek(bfile, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
            fread(&carpetaIndirecta, sizeof(Structs::arbolVirtual), 1, bfile);

            fclose(bfile);
            crearCarpeta(carpetaIndirecta, path, pathDisco, superBloque, apuntador);
            return;
        }
}

void Carpeta::copyFile(string path, string dest, Mount montaje, string id)
{
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

    vector<string> pathArray;
    stringstream total(path);
    string tmp;

    while (getline(total, tmp, '/'))
    {
        if(tmp!=""){
            pathArray.push_back(tmp);
        }
    }

    vector<string> destinoArray;
    stringstream total2(dest);
    string tmp2;

    while (getline(total2, tmp2, '/'))
    {
        if(tmp2!=""){
            destinoArray.push_back(tmp2);
        }
    }

    Structs::SuperBloque superBloque;
    FILE *bfile = fopen(pathD.c_str(), "rb+");
    fseek(bfile, startParticion, SEEK_SET);
    fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

    Structs::arbolVirtual raiz;
    fseek(bfile, superBloque.start_arbol_directorio, SEEK_SET);
    fread(&raiz, sizeof(Structs::arbolVirtual), 1, bfile);

    fclose(bfile);

    Structs::arbolVirtual avdDest = getAVD(raiz, destinoArray, pathD, superBloque, 0);
    int avdDestNo = getintAVD(raiz, destinoArray, pathD, superBloque, 0);
    int avdRuta = getintAVD(raiz, pathArray, pathD, superBloque, 0);

    FILE *bfile2 = fopen(pathD.c_str(), "rb+");

    if(copiarArchivo){
        int apuntador = avdDest.detalle_directorio;
        Structs::detalleDirectorio Archivos;
        fseek(bfile2, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
        fread(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile2);
        for(int j=0;j<5;j++){
            if(Archivos.archivos[j].noInodo == -1){
                Archivos.archivos[j].noInodo = avdRuta;
                fseek(bfile2, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
                fwrite(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile2);
                copiarArchivo = false;
                break;
            }
        }
    }else{
        for(int j=0;j<6;j++){
            if(avdDest.array_subdirectorios[j] == -1){
                avdDest.array_subdirectorios[j] = avdRuta;
                fseek(bfile2, superBloque.start_arbol_directorio+(avdDestNo*sizeof(Structs::arbolVirtual)), SEEK_SET);
                fwrite(&avdDest, sizeof(Structs::arbolVirtual), 1, bfile2);
                break;
            }
        }
    }
    fclose(bfile2);
}

void Carpeta::moveFile(string path, string dest, Mount montaje)
{
}

void Carpeta::renameFile(string id, string path, string name, Mount montaje)
{
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

    vector<string> pathArray;
    stringstream total(path);
    string tmp;

    while (getline(total, tmp, '/'))
    {
        if(tmp!=""){
            pathArray.push_back(tmp);
        }
    }


    Structs::SuperBloque superBloque;
    FILE *bfile = fopen(pathD.c_str(), "rb+");
    fseek(bfile, startParticion, SEEK_SET);
    fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

    Structs::arbolVirtual raiz;
    fseek(bfile, superBloque.start_arbol_directorio, SEEK_SET);
    fread(&raiz, sizeof(Structs::arbolVirtual), 1, bfile);

    fclose(bfile);

    recorrerRuta(raiz, pathArray, pathD, superBloque, name, 0);


}

void Carpeta::findFile(string path, string name, Mount montaje)
{
}


void Carpeta::recorrerRuta(Structs::arbolVirtual avd, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, string nombre, int pointer){
    bool esArchivo = false;
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
            string name = path[0];
            path.erase(path.begin());
            if(path.size() == 1){ // O es archivo o es carpeta.
                apuntador = carpetaHijo.detalle_directorio;
                Structs::detalleDirectorio Archivos;
                fseek(bfile, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
                fread(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);


                for(int j=0;j<5;j++){
                    if(Archivos.archivos[j].nombre_directorio == path[0]){
                        cout<< "Archivo Renombrado"<< endl;
                        esArchivo = true;
                        strcpy(Archivos.archivos[j].nombre_directorio, nombre.c_str());
                        fseek(bfile, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
                        fwrite(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);
                        fclose(bfile);
                        return;
                    }
                }

                //ES CARPETA.
                if(!esArchivo){
                    for(int k = 0; k < 6; k++){
                        int pointer = carpetaHijo.array_subdirectorios[k];
                        Structs::arbolVirtual carpeta;
                        fseek(bfile, (superBloque.start_arbol_directorio+(pointer*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
                        fread(&carpeta, sizeof(Structs::arbolVirtual), 1, bfile);
                        fclose(bfile);
                        if(carpeta.nombre_directorio == path[0]){
                            cout<< "Carpeta Renombrada"<< endl;
                            strcpy(carpeta.nombre_directorio, nombre.c_str());
                            fseek(bfile, (superBloque.start_arbol_directorio+(pointer*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
                            fread(&carpeta, sizeof(Structs::arbolVirtual), 1, bfile);
                            fclose(bfile);
                            return;
                        }
                    }
                }
                cout<< "Archivo No encontrado"<< endl;
                fclose(bfile);
                return;
            }else if(path.size() == 0){
                if(carpetaHijo.nombre_directorio == name){
                    cout<< "Carpeta Renombrada"<< endl;
                    strcpy(carpetaHijo.nombre_directorio, nombre.c_str());
                    fseek(bfile, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
                    fwrite(&carpetaHijo, sizeof(Structs::arbolVirtual), 1, bfile);
                    fclose(bfile);
                    return;
                }else{
                    apuntador = avd.detalle_directorio;
                    Structs::detalleDirectorio Archivos;
                    fseek(bfile, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
                    fread(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);
                    for(int j=0;j<5;j++){
                        if(Archivos.archivos[j].nombre_directorio == path[0]){
                            cout<< "Archivo Renombrado"<< endl;
                            esArchivo = true;
                            strcpy(Archivos.archivos[j].nombre_directorio, nombre.c_str());
                            fseek(bfile, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
                            fwrite(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);
                            fclose(bfile);
                            return;
                        }
                    }
                }
         }else{
                fclose(bfile);
                recorrerRuta(carpetaHijo, path, pathDisco, superBloque, nombre, apuntador);
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
    recorrerRuta(carpetaIndirecta, path, pathDisco, superBloque, nombre, apuntador);
    return;
}



Structs::arbolVirtual Carpeta::getAVD(Structs::arbolVirtual avd, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, int pointer){
    bool esArchivo = false;
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
            string name = path[0];
            path.erase(path.begin());
            if(path.size() == 1){ // O es archivo o es carpeta.
                apuntador = carpetaHijo.detalle_directorio;
                Structs::detalleDirectorio Archivos;
                fseek(bfile, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
                fread(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);


                for(int j=0;j<5;j++){
                    if(Archivos.archivos[j].nombre_directorio == path[0]){
                        cout<< "Archivo Encontrado"<< endl;
                        copiarArchivo = true;
                        fclose(bfile);
                        return carpetaHijo;
                    }
                }

                //ES CARPETA.
                if(!esArchivo){
                    for(int k = 0; k < 6; k++){
                        int pointer = carpetaHijo.array_subdirectorios[k];
                        Structs::arbolVirtual carpeta;
                        fseek(bfile, (superBloque.start_arbol_directorio+(pointer*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
                        fread(&carpeta, sizeof(Structs::arbolVirtual), 1, bfile);
                        if(carpeta.nombre_directorio == path[0]){
                            cout<< "Carpeta Encontrada"<< endl;
                            fclose(bfile);
                            return carpeta;
                        }
                    }
                }
                cout<< "Archivo No encontrado"<< endl;
                fclose(bfile);
                return avd;
            }else if(path.size() == 0){
                if(carpetaHijo.nombre_directorio == name){
                    cout<< "Carpeta Encontrada"<< endl;
                    fclose(bfile);
                    return carpetaHijo;
                }else{
                    copiarArchivo = true;
                    fclose(bfile);
                    return avd;
                }
         }else{
                fclose(bfile);
                //recorrerRuta(carpetaHijo, path, pathDisco, superBloque, nombre, apuntador);
                return getAVD(carpetaHijo, path, pathDisco, superBloque, apuntador);            }
        }
    }

    //NINGUNO CUMPLE, SE MUEVE AL APUNTADOR INDIRECTO
    apuntador = avd.avd_siguiente;
    Structs::arbolVirtual carpetaIndirecta;
    fseek(bfile, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
    fread(&carpetaIndirecta, sizeof(Structs::arbolVirtual), 1, bfile);

    fclose(bfile);
    return getAVD(carpetaIndirecta, path, pathDisco, superBloque, apuntador);
}

int Carpeta::getintAVD(Structs::arbolVirtual avd, vector<string> path, string pathDisco, Structs::SuperBloque superBloque, int pointer){
    bool esArchivo = false;
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
            string name = path[0];
            path.erase(path.begin());
            if(path.size() == 1){ // O es archivo o es carpeta.
                apuntador = carpetaHijo.detalle_directorio;
                Structs::detalleDirectorio Archivos;
                fseek(bfile, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
                fread(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);


                for(int j=0;j<5;j++){
                    if(Archivos.archivos[j].nombre_directorio == path[0]){
                        cout<< "Archivo Encontrado"<< endl;
                        copiarArchivo = true;
                        fclose(bfile);
                        return Archivos.archivos[j].noInodo;
                    }
                }

                //ES CARPETA.
                if(!esArchivo){
                    for(int k = 0; k < 6; k++){
                        int pointer = carpetaHijo.array_subdirectorios[k];
                        Structs::arbolVirtual carpeta;
                        fseek(bfile, (superBloque.start_arbol_directorio+(pointer*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
                        fread(&carpeta, sizeof(Structs::arbolVirtual), 1, bfile);
                        fclose(bfile);
                        if(carpeta.nombre_directorio == path[0]){
                            cout<< "Carpeta Encontrada"<< endl;
                            return pointer;
                        }
                    }
                }
                cout<< "Archivo No encontrado"<< endl;
                fclose(bfile);
                return pointer;
            }else if(path.size() == 0){
                if(carpetaHijo.nombre_directorio == name){
                    cout<< "Carpeta Encontrada"<< endl;
                    fclose(bfile);
                    return apuntador;
                }else{
                    copiarArchivo = true;
                    fclose(bfile);
                    return pointer;
                }
         }else{
                fclose(bfile);
                //recorrerRuta(carpetaHijo, path, pathDisco, superBloque, nombre, apuntador);
                return getintAVD(carpetaHijo, path, pathDisco, superBloque, apuntador);
            }
        }
    }

    //NINGUNO CUMPLE, SE MUEVE AL APUNTADOR INDIRECTO
    apuntador = avd.avd_siguiente;
    Structs::arbolVirtual carpetaIndirecta;
    fseek(bfile, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
    fread(&carpetaIndirecta, sizeof(Structs::arbolVirtual), 1, bfile);

    fclose(bfile);
    return getintAVD(carpetaIndirecta, path, pathDisco, superBloque, apuntador);
}

























