#include "mount.h"

Mount::Mount()
{

}

void Mount::montarParticion(string path, string name)
{
    int inicioExtendida = 0;
    FILE *validar = fopen(path.c_str(), "rb+");
    if (validar == NULL)
    {
        cout << "Disco no existente, validar path" << endl;
        fclose(validar);
        return;
    }else{
        Structs::MBR discoParticion;
        bool existeParticion = false;
        rewind(validar);
        fread(&discoParticion, sizeof(Structs::MBR), 1, validar);

        for(int i = 0 ; i<4; i++){

            if(discoParticion.mbr_particiones[i].type == 'E'){
                inicioExtendida = discoParticion.mbr_particiones[i].part_start;
            }

            if(discoParticion.mbr_particiones[i].name == name){
                existeParticion = true;
            }
        }

        //VALIDAR SI ES UNA PARTICION LOGICA.
        if(!existeParticion && inicioExtendida !=0){
            Structs::EBR logicaM;
            fseek(validar, inicioExtendida, SEEK_SET);
            fread(&logicaM, sizeof(Structs::EBR), 1, validar);
            while(logicaM.Estado != '0' && logicaM.part_next != -1){
                if(logicaM.name == name){
                    existeParticion = true;
                }
                fseek(validar, logicaM.part_next, SEEK_SET);
                fread(&logicaM, sizeof(Structs::EBR), 1, validar);
            }
        }


        if(!existeParticion){
            cout << "Nombre de particion no existente." << endl;
            fclose(validar);
            return;
        }
        fclose(validar);
    }

    bool existePath = false;
    int posicionPath = 0;
    for (int i = 0; i < 26; i++)
    {
        if (Mount::discos[i].path == path)
        {
            existePath = true;
            posicionPath = i;
        }
    }

    //SI NO EXISTE EL DISCO MONTADO, SE MONTA.
    if (!existePath)
    {
        for (int j = 0; j < 99; j++)
        {
            if (Mount::discos[j].estado == 0)
            {
                Mount::discos[j].estado = 1;
                Mount::discos[j].letra = j + 97;
                strcpy(Mount::discos[j].path, path.c_str());
                posicionPath = j;
                existePath = true;
                break;
            }
        }
    }

    //DISCO YA MONTADO, SOLO SE AGREGA LA PARTICION.
    if (existePath)
    {
        for (int j = 0; j < 99; j++)
        {
            if (Mount::discos[posicionPath].particiones[j].estado == 0)
            {
                Mount::discos[posicionPath].particiones[j].estado = 1;
                Mount::discos[posicionPath].particiones[j].numero = j + 1;
                strcpy(Mount::discos[posicionPath].particiones[j].name, name.c_str());
                cout << "Particion montada exitosamente, id: vd" << Mount::discos[posicionPath].letra << Mount::discos[posicionPath].particiones[j].numero << endl;
                break;
            }
        }
    }

    leerMontajes();
}

void Mount::desmontarParticion(string id)
{
    char letra = id.c_str()[2];
    char numero = id.c_str()[3];
    bool desmontado = false;
    int numeroInt = (int) numero - 48;

    for (int i = 0; i < 26; i++)
    {
        if (Mount::discos[i].letra == letra)
        {
            for (int j = 0; j < 99; j++)
            {
                if (Mount::discos[i].particiones[j].numero == numeroInt)
                {
                    cout << "Particion desmontada: vd" << Mount::discos[i].letra << Mount::discos[i].particiones[j].numero << endl;
                    Mount::discos[i].particiones[j].estado = 0;
                    desmontado = true;
                }
            }
        }
    }

    if(!desmontado){
        cout << "Error: id no existe, no se desmonto la particion." <<endl;
    }
    leerMontajes();
}


void Mount::leerMontajes()
{
    cout << "<<<-------------------------- MONTAJES ---------------------->>>\n"
         << endl;
    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 99; j++)
        {
            if (Mount::discos[i].particiones[j].estado == 1)
            {
                cout << "vd" << Mount::discos[i].letra << Mount::discos[i].particiones[j].numero << endl;
            }
        }
    }
}
