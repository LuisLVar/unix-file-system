#include "fdisk.h"
#include <iostream>
#include <string.h>
#include <algorithm>
#include <fstream>

fDisk::fDisk()
{

}

void fDisk::administrarParticion(string size, string unit, string path,
                                 string fit, string type, string cdelete, string name, string add)
{
    if (unit == "")
    {
        unit = "K";
    }
    if (fit == "")
    {
        fit = "W";
    }
    if (type == "")
    {
        type = "P";
    }

    //Add to Partition
    if (add != "")
    {
        fDisk::addParticion(add, unit, name, path); //ESPERA
    }
    else if (cdelete != "") //Delete Partition
    {
        fDisk::deleteParticion(cdelete, path, name); //ALMOST DONE
    }
    else //Create a Partition
    {
        fDisk::createPartition(size, unit, name, path, fit, type);
    }
}

void fDisk::createPartition(string size, string unit, string name, string path, string fit, string type)
{ // Fit: BF, FF, WF. Default:(WF)   -  Type: P, E, L.   Default: (P)   -   Unit: B, K, M.  Default: (K)

    Structs::Particion partition;

    int numSize = stoi(size);

    if (unit == "M" || unit == "m")
    {
        numSize = numSize * 1024 * 1024;
    }
    else if (unit == "K" || unit == "k")
    {
        numSize = numSize * 1024;
    }
    else if (unit == "B" || unit == "b")
    {
    }
    else
    {
        cout << "Error en la unidad de la particion" << endl;
        return;
    }

    if (numSize < 0)
    {
        cout << "Error, tamanio del disco demasiado grande." << endl;
        return;
    }

    partition.Estado = '1';
    partition.fit = fit[0];
    strcpy(partition.name, name.c_str());
    partition.part_start = 0;
    partition.size = numSize;
    partition.type = toupper(type[0]);

    if (type == "P")
    {
    }
    else if (type == "E")
    {
    }
    else if (type == "L" || type == "l")
    {
        //cout << "Particion Logica"<<endl;
        crearLogica(partition, path);
        listarParticiones(path);
        return;
    }
    else
    {
        cout << "Error en el tipo de particion" << endl;
        return;
    }

    Structs::MBR discoEditar;
    FILE *bfile2 = fopen(path.c_str(), "rb+");
    if (bfile2 != NULL)
    {
        rewind(bfile2);
        fread(&discoEditar, sizeof(discoEditar), 1, bfile2);
    }
    else
    {
        cout << "Error. Path no existente." << endl;
        return;
    }
    fclose(bfile2);

    /* Check that it worked */
    //cout << "Disco Leido:\nSize: " << discoEditar.size << "\nDate: " << discoEditar.date << "\nFit: " << discoEditar.fit << endl;

    //Editar disco.

    bool discoLLeno = true;
    int conteoExtendida = 0;
    bool tamanioPasado = false;
    int part_startExtendida = 0;
    bool ultimaParticion = false;

    //Valida numero de extendidas
    for (int i = 0; i < 4; i++)
    {
        if (discoEditar.mbr_particiones[i].type == 'E')
        {
            conteoExtendida++;
        }
    }

    //Valida si el disco esta lleno
    for (int i = 0; i < 4; i++)
    {
        if (discoEditar.mbr_particiones[i].Estado == '0')
        {
            discoLLeno = false;
            break;
        }
    }

    //Este solo ordena y hace el primer ajuste.
    for (int i = 0; i < 4; i++)
    {
        if (discoEditar.mbr_particiones[i].Estado != '0')
        {
            if (!(partition.type == 'E' && conteoExtendida > 0))
            {
                //PRIMER AJUSTE
                if (i == 0)
                {
                    if (discoEditar.mbr_particiones[i].part_start - sizeof(Structs::MBR) >= partition.size)
                    {
                        partition.part_start = sizeof(Structs::MBR);
                        break;
                    }
                }
                else
                {
                    if (discoEditar.mbr_particiones[i].part_start - (discoEditar.mbr_particiones[i - 1].part_start + discoEditar.mbr_particiones[i - 1].size) >= partition.size)
                    {
                        partition.part_start = discoEditar.mbr_particiones[i - 1].part_start + discoEditar.mbr_particiones[i - 1].size;

                        break;
                    }
                }
            }
            else
            {
                cout << "Error: ya existe una particion extendida, no puedes crear otra." << endl;
                tamanioPasado = true;
                return;
            }
        }
        else
        {
            if (i == 0)
            {
                //PRIMERA POSICION
                partition.part_start = sizeof(Structs::MBR);
                break;
            }
            else
            {
                //ULTIMA POSICION
                ultimaParticion = true;
                break;
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        if (discoEditar.mbr_particiones[i].Estado != '1')
        {
            if (ultimaParticion)
            {
                partition.part_start = discoEditar.mbr_particiones[i - 1].part_start + discoEditar.mbr_particiones[i - 1].size;
                if (discoEditar.size < (partition.part_start + partition.size))
                {
                    tamanioPasado = true;
                }
            }
            discoEditar.mbr_particiones[i] = partition;
            //Es extendida, creo de una el EBR.
            if (partition.type == 'E')
            {
                part_startExtendida = partition.part_start;
            }
            break;
        }
    }

    //METODO DE ORDENAMIENTO BURBUJA.
    Structs::Particion aux;
    for (int i = 1; i < 4; i++)
    {
        for (int j = 0; j < 4 - i; j++)
        {
            if ((discoEditar.mbr_particiones[j].part_start > discoEditar.mbr_particiones[j + 1].part_start) && discoEditar.mbr_particiones[j + 1].Estado != '0')
            {
                aux = discoEditar.mbr_particiones[j + 1];
                discoEditar.mbr_particiones[j + 1] = discoEditar.mbr_particiones[j];
                discoEditar.mbr_particiones[j] = aux;
            }
        }
    }

    if (!discoLLeno && !tamanioPasado)
    {
        FILE *bfile = fopen(path.c_str(), "rb+");
        if (bfile != NULL)
        {
            rewind(bfile);
            fwrite(&discoEditar, sizeof(Structs::MBR), 1, bfile);

            //Crea primer EBR
            if (part_startExtendida != 0)
            {
                rewind(bfile);
                Structs::EBR logica;
                logica.part_start = part_startExtendida;
                strcpy(logica.name, "NoName");
                fseek(bfile, logica.part_start, SEEK_SET);
                fwrite(&logica, sizeof(Structs::EBR), 1, bfile);
            }
        }
        fclose(bfile);
        listarParticiones(path);
    }
    else
    {
        if (discoLLeno)
        {
            cout << "Error: numero de particiones maximas creadas." << endl;
            return;
        }
        else
        {
            cout << "Error: tamanio de la particion sobre pasa el disco." << endl;
            return;
        }
    }
}

void fDisk::deleteParticion(string cdelete, string path, string name)
{ //Full or Fast
    cout << "Seguro que deseas eliminar la particion? [s/n]" << endl;
    string respuesta;
    cin >> respuesta;

    int inicioExtendida = -1;

    if (respuesta == "s")
    {


        //ELIMINAR LOGICA.
        Structs::EBR logicaD;
        FILE *bfile = fopen(path.c_str(), "rb+");
        if (bfile != NULL)
        {
            Structs::MBR discoEdit;
            rewind(bfile);
            fread(&discoEdit, sizeof(Structs::MBR), 1, bfile);
            for (int i = 0; i < 4; i++)
            {
                if (discoEdit.mbr_particiones[i].type == 'E')
                {
                    inicioExtendida = discoEdit.mbr_particiones[i].part_start;
                }
            }

            if(inicioExtendida != -1){
                fseek(bfile, inicioExtendida, SEEK_SET);
                fread(&logicaD, sizeof(Structs::EBR), 1, bfile);
                while(logicaD.part_next != -1){
                    if(strcmp(logicaD.name, name.c_str()) == 0){
                        logicaD.Estado = '0'; // Elimino la particion logica.
                        fseek(bfile, logicaD.part_start, SEEK_SET);
                        fwrite(&logicaD, sizeof(Structs::EBR), 1, bfile);
                        cout << "Particion logica eliminada exitosamente." << endl;
                        fclose(bfile);
                        return;
                    }
                    fseek(bfile, logicaD.part_next, SEEK_SET);
                    fread(&logicaD, sizeof(Structs::EBR), 1, bfile);
                }
            }
            fclose(bfile);
        }else{
            cout << "Path no existente en la eliminacion de particion." << endl;
            return;
        }


        //ELIMINACION DE PRIMARIAS Y EXTENDIDAS.
        if (fDisk::toLowerCase(cdelete) == "fast")
        {
            Structs::MBR discoEditar;
            FILE *bfile2 = fopen(path.c_str(), "rb+");
            if (bfile2 != NULL)
            {
                rewind(bfile2);
                fread(&discoEditar, sizeof(discoEditar), 1, bfile2);

                //ELIMINANDO FAST
                for (int i = 0; i < 4; i++)
                {
                    if (strcmp(discoEditar.mbr_particiones[i].name, name.c_str()) == 0)
                    {
                        discoEditar.mbr_particiones[i].Estado = '0';

                        cout << "Particion eliminada correctamente./Fast" << endl;
                        int j = i;
                        Structs::Particion aux;
                        while (j != 3)
                        {
                            if (discoEditar.mbr_particiones[j + 1].Estado != '0')
                            {
                                aux = discoEditar.mbr_particiones[j];
                                discoEditar.mbr_particiones[j] = discoEditar.mbr_particiones[j + 1];
                                discoEditar.mbr_particiones[j + 1] = aux;
                            }
                            j++;
                        }
                        break;
                    }
                }

                rewind(bfile2);
                fwrite(&discoEditar, sizeof(Structs::MBR), 1, bfile2);
            }
            else
            {
                cout << "Error: Path no existente en la eliminacion de particion." << endl;
                return;
            }
            fclose(bfile2);
        }
        else if (fDisk::toLowerCase(cdelete) == "full")
        {
            Structs::MBR discoEditar;
            FILE *bfile2 = fopen(path.c_str(), "rb+");
            if (bfile2 != NULL)
            {
                rewind(bfile2);
                fread(&discoEditar, sizeof(discoEditar), 1, bfile2);

                //ELIMINANDO FULL
                for (int i = 0; i < 4; i++)
                {
                    if (strcmp(discoEditar.mbr_particiones[i].name, name.c_str()) == 0)
                    {
                        discoEditar.mbr_particiones[i].Estado = '0';
                        discoEditar.mbr_particiones[i].fit = ' ';
                        strcpy(discoEditar.mbr_particiones[i].name, "");
                        discoEditar.mbr_particiones[i].part_start = 0;
                        discoEditar.mbr_particiones[i].size = 0;
                        discoEditar.mbr_particiones[i].type = ' ';

                        cout << "Particion eliminada correctamente. /Full" << endl;
                        int j = i;
                        Structs::Particion aux;
                        while (j != 3) //ORDENAMIENTO
                        {
                            if (discoEditar.mbr_particiones[j + 1].Estado != '0')
                            {
                                aux = discoEditar.mbr_particiones[j];
                                discoEditar.mbr_particiones[j] = discoEditar.mbr_particiones[j + 1];
                                discoEditar.mbr_particiones[j + 1] = aux;
                            }
                            j++;
                        }
                        break;
                    }
                }

                rewind(bfile2);
                fwrite(&discoEditar, sizeof(Structs::MBR), 1, bfile2);
            }
            else
            {
                cout << "Error: Path no existente en la eliminacion de particion." << endl;
                return;
            }
            fclose(bfile2);
        }
        else
        {
            cout << "Error: Valor de delete es incorrecto." << endl;
        }
    }
    else
    {
        cout << "Comando cancelado" << endl;
    }
}

void fDisk::addParticion(string add, string unit, string name, string path)
{ // Positive or negative number
    int addSize = stoi(add);

    if (unit == "M" || unit == "m")
    {
        addSize = addSize * 1024 * 1024;
    }
    else if (unit == "K" || unit == "k")
    {
        addSize = addSize * 1024;
    }
    else if (unit == "B" || unit == "b")
    {
    }
    else
    {
        cout << "Error en la unidad de la particion" << endl;
        return;
    }

    Structs::MBR discoEditar;
    FILE *bfile2 = fopen(path.c_str(), "rb+");
    if (bfile2 != NULL)
    {
        rewind(bfile2);
        fread(&discoEditar, sizeof(Structs::MBR), 1, bfile2);

        for (int i = 0; i < 4; i++)
        {
            if (strcmp(discoEditar.mbr_particiones[i].name, name.c_str()) == 0)
            {
                if (((discoEditar.mbr_particiones[i].size + (addSize)) > 0))
                {
                    if (discoEditar.mbr_particiones[i + 1].part_start != 0)
                    {
                        if (((discoEditar.mbr_particiones[i].size + (addSize) + discoEditar.mbr_particiones[i].part_start) <=
                             discoEditar.mbr_particiones[i + 1].part_start))
                        {
                            discoEditar.mbr_particiones[i].size = discoEditar.mbr_particiones[i].size + (addSize);
                            cout << "Size modificado correctamente" << endl;
                            break;
                        }
                        else
                        {
                            cout << "Error: size incorrecta, sobrepasa limite de particiones." << endl;
                        }
                    }
                    else
                    {
                        if ((discoEditar.mbr_particiones[i].size + (addSize) + discoEditar.mbr_particiones[i].part_start) <= discoEditar.size)
                        {
                            discoEditar.mbr_particiones[i].size = discoEditar.mbr_particiones[i].size + (addSize);
                            cout << "Size modificado correctamente" << endl;
                            break;
                        }
                        else
                        {
                            cout << "Error: size incorrecta, sobrepasa limite de disco." << endl;
                        }
                    }
                }
                else
                {
                    cout << "Error: Size menor a cero, no se puede reducir." << endl;
                }
            }
        }
        rewind(bfile2);
        fwrite(&discoEditar, sizeof(Structs::MBR), 1, bfile2);
    }
    else
    {
        cout << "Error: Path no existente en la eliminacion de particion." << endl;
        return;
    }
    fclose(bfile2);
}

void fDisk::crearLogica(Structs::Particion particion, string path){

    Structs::EBR newLogica;

    newLogica.Estado = '1';
    newLogica.fit = particion.fit;
    strcpy(newLogica.name, particion.name);
    newLogica.part_next = -1;
    newLogica.size = particion.size;

    bool existeExtendida = false;
    int posExtendida = 0;
    Structs::MBR discoEditar;
    FILE *bfile2 = fopen(path.c_str(), "rb+");
    if (bfile2 != NULL)
    {
        rewind(bfile2);
        fread(&discoEditar, sizeof(Structs::MBR), 1, bfile2);
        for(int i = 0; i < 4; i++){
            if(discoEditar.mbr_particiones[i].type == 'E'){
                existeExtendida = true;
                posExtendida = i;
            }
        }
        if(existeExtendida){
            int posicion = discoEditar.mbr_particiones[posExtendida].part_start;
            Structs::EBR getLogica;
            fseek(bfile2, posicion, SEEK_SET);
            fread(&getLogica, sizeof(Structs::EBR), 1, bfile2);
            bool finalizar = false;
            do{
                if(getLogica.Estado == '0' && getLogica.part_next == -1){
                    newLogica.part_start = getLogica.part_start;
                    newLogica.part_next = newLogica.part_start + newLogica.size;
                    fseek(bfile2, newLogica.part_start, SEEK_SET);
                    fwrite(&newLogica, sizeof(Structs::EBR), 1, bfile2);
                    fseek(bfile2, newLogica.part_next, SEEK_SET);
                    Structs::EBR addLogic;
                    addLogic.part_start = newLogica.part_next;
                    fseek(bfile2, addLogic.part_start, SEEK_SET);
                    fwrite(&addLogic, sizeof(Structs::EBR), 1, bfile2);
                    finalizar = true;
                    cout << " Particion logica creada exitosamente, " << newLogica.name << endl;
                }
                fseek(bfile2, getLogica.part_next, SEEK_SET);
                fread(&getLogica, sizeof(Structs::EBR), 1, bfile2);
            }while(!finalizar);
        }else{
            cout << "Error, no existe particion extendida." << endl;
        }
    }else
    {
        cout << "Error: Path no existente en la creacion de particion logica." << endl;
        return;
    }
    fclose(bfile2);
}


string fDisk::toLowerCase(string comando)
{
    string data = comando;
    transform(data.begin(), data.end(), data.begin(),
              [](unsigned char c) { return std::tolower(c); });
    return data;
}

void fDisk::listarParticiones(string path){
    Structs::MBR discoEditar;
    FILE *bfilel = fopen(path.c_str(), "rb+");
    if (bfilel != NULL)
    {
        rewind(bfilel);
        fread(&discoEditar, sizeof(Structs::MBR), 1, bfilel);
        cout << "<<------------------------ PARTICIONES ---------------------------->>" << endl;
         for (int i = 0; i < 4; i++)
         {
             if(discoEditar.mbr_particiones[i].Estado != '0'){
                 cout << "<< ------------------- " << i << " ------------------->>" << endl;
                 cout << "Estado: " << discoEditar.mbr_particiones[i].Estado << endl;
                 cout << "Nombre: " << discoEditar.mbr_particiones[i].name << endl;
                 cout << "Fit: " << discoEditar.mbr_particiones[i].fit << endl;
                 cout << "Part_start: " << discoEditar.mbr_particiones[i].part_start << endl;
                 cout << "Size: " << discoEditar.mbr_particiones[i].size << endl;
                 cout << "Type: " << discoEditar.mbr_particiones[i].type << endl;
                 if (discoEditar.mbr_particiones[i].type == 'E')
                 {
                     Structs::EBR logicaR;
                     fseek(bfilel, discoEditar.mbr_particiones[i].part_start, SEEK_SET);
                     fread(&logicaR, sizeof(Structs::EBR), 1, bfilel);

                     while(logicaR.part_next != -1){
                         cout << "<< -------------------- Particion Logica --------------------->> " << endl;
                         cout << "Nombre: " << logicaR.name << endl;
                         cout << "Fit: " << logicaR.fit << endl;
                         cout << "Part_start: " << logicaR.part_start << endl;
                         cout << "Size: " << logicaR.size << endl;
                         cout << "Part_next: " << logicaR.part_next << endl;
                         cout << "Estado: " << logicaR.Estado << endl;

                         fseek(bfilel, logicaR.part_next, SEEK_SET);
                         fread(&logicaR, sizeof(Structs::EBR), 1, bfilel);
                     }
                 }
             }
         }
         fclose(bfilel);
    }else{
        cout << "No se pueden mostrar las particiones por path incorrecto.";
    }
}
