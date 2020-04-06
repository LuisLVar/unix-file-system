#include "interprete.h"
#include "mkdisk.h"
#include "rmdisk.h"
#include "fdisk.h"
#include "rep.h"
#include "mkfs.h"
#include "carpeta.h"
#include "archivo.h"
#include "recovery.h"

Interprete::Interprete()
{
}


void Interprete::interpretar()
{
    bool finalizar = false;
    while (finalizar != true)
    {
        cout << "Insertar comando: " << endl;
        bool comandoMultiple = false;
        string comando = "";

        //COMANDO MULTILINE
        do{
            //Obtener comando.
            string cmd;
            getline(cin, cmd);
            if(comando.length() > 0){
                comando = comando.substr(0, comando.length()-2);
            }
            comando = comando + cmd;

            int tamanio = cmd.length();
            if(tamanio > 2){
                string salto = cmd.substr(tamanio-2, tamanio-1);
                if(salto == "\\^"){
                    comandoMultiple = true;
                }else{
                    comandoMultiple = false;
                }
            }
        }while(comandoMultiple);

        if (comando == "x")
        {
            finalizar = true;
            break;
        }
        else
        {
            if (comando != "")
            {
                //cout << "Comando: \n" << comando << endl;
                Interprete::lineaComando(comando);
            }
        }
    }
}

void Interprete::lineaComando(string comando)
{
    bool comentario = false;

    vector<string> commandArray;
    stringstream total(comando);
    string tmp;

    while (getline(total, tmp, ' '))
    {
        if(tmp!=""){
            commandArray.push_back(tmp);
        }
    }

    char comentarioLetra = commandArray[0].at(0);

    if (comentarioLetra == '#')
    {
        comentario = true;
    }

    if (comentario)
    {
        //cout << "Has escrito un comentario" << endl;
        cout << comando << endl;
    }
    else
    {
        Interprete::ejecutarComando(commandArray);
    }
}

void Interprete::ejecutarComando(vector<string> commandArray)
{
    string data = Interprete::toLowerCase(commandArray[0]);
    if (data == "exec")
    {
        Interprete::fexec(commandArray); //DONE
        //cout << "Comando Exec" << endl;
    }
    else if (data == "mkdisk")
    {
        Interprete::fmkDisk(commandArray); //DONE
        //cout << "Comando mkdisk" << endl;
    }
    else if (data == "rmdisk")
    {
        Interprete::frmDisk(commandArray); //DONE
//        cout << "Comando rmdisk" << endl;
    }
    else if (data == "fdisk")
    {
        Interprete::ffDisk(commandArray); //DONE
//        cout << "Entro fdisk" << endl;
    }
    else if (data == "mount")
    {
        Interprete::fmount(commandArray); //DONE
//        cout << "Entro mount" << endl;
    }
    else if (data == "unmount")
    {
        Interprete::funmount(commandArray); //DONE
//        cout << "Entro unmount" << endl;
    }
    else if (data == "rep")
    {
        Interprete::frep(commandArray); //PENDIENTE
//        cout << "Entro rep" << endl;
    }
    else if (data == "pause")
    {
        Interprete::fpause(); //DONE
    }
    else if (data == "mkfs")
    {
        Interprete::fmkfs(commandArray); //PENDIENTE
    }
    else if (data == "mkfile")
    {
        Interprete::fmkfile(commandArray); //PENDIENTE
    }
    else if (data == "cat")
    {
        Interprete::fcat(commandArray); //PENDIENTE
    }
    else if (data == "rm")
    {
        Interprete::frem(commandArray); //PENDIENTE
    }
    else if (data == "edit")
    {
        Interprete::fedit(commandArray); //PENDIENTE
    }
    else if (data == "ren")
    {
        Interprete::fren(commandArray); //PENDIENTE
    }
    else if (data == "mkdir")
    {
        Interprete::fmkdir(commandArray); //PENDIENTE
    }
    else if (data == "cp")
    {
        Interprete::fcp(commandArray); //PENDIENTE
    }
    else if (data == "mv")
    {
        Interprete::fmv(commandArray); //PENDIENTE
    }
    else if (data == "find")
    {
        Interprete::ffind(commandArray); //PENDIENTE
    }
    else if (data == "recovery")
    {
        Interprete::frecovery(commandArray); //PENDIENTE
    }
    else if (data == "loss")
    {
        Interprete::floss(commandArray); //PENDIENTE
    }
    else
    {
        Interprete::errorComando(commandArray[0]);
        //cout << "Entro error" << endl;
    }
}

//EJECUCION DE COMANDOS.

void Interprete::fexec(vector<string> commandArray)
{
    string pathCommand = Interprete::getPath(commandArray);
    string path = Interprete::getAtributo(pathCommand);
    path.erase(remove(path.begin(), path.end(), '\"'), path.end());
    //Scout << "Path: " << path << endl;
    Interprete::leerArchivo(path);
}


void Interprete::fmkDisk(vector<string> commandArray)
{
    //cout << "Entro mkDisk" << endl;
    string size = "";
    string unit = "";
    string path = "";
    string name = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        if (comando == "size")
        {
            size = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "name")
        {
            name = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "unit")
        {
            unit = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
    }

    path.append(name);
    mkDisk disco;
    disco.crearDisco(size, unit, path);
}

void Interprete::frmDisk(vector<string> commandArray)
{
    //cout << "Entro rmDisk" << endl;
    string pathCommand = Interprete::getPath(commandArray);
    string path = Interprete::getAtributo(pathCommand);
    path.erase(remove(path.begin(), path.end(), '\"'), path.end());
    //cout << "Path a eliminar: " << path << endl;
    cout << "Seguro que deseas eliminar el disco? [s/n]" << endl;
    string respuesta;
    cin >> respuesta;
    if (respuesta == "s")
    {
        rmDisk disco;
        disco.eliminarDisco(path);
    }
    else
    {
        cout << "Comando cancelado" << endl;
    }
}

void Interprete::ffDisk(vector<string> commandArray)
{
    //cout << "Entro fDisk" << endl;

    string size = "";
    string unit = "";
    string path = "";
    string fit = "";
    string type = "";
    string cdelete = "";
    string name = "";
    string add = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        string comandoDelete = Interprete::toLowerCase(commandArray[i].substr(1, 6));
        if (comando == "size")
        {
            size = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "fit-")
        {
            fit = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "unit")
        {
            unit = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "type")
        {
            type = Interprete::getAtributo(commandArray[i]);
        }
        else if (comandoDelete == "delete")
        {
            cdelete = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "add-")
        {
            add = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "name")
        {
            name = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
    }

    if (unit == "")
    {
        unit = "K";
    }
    if (type == "")
    {
        type = "P";
    }
    if (fit == "")
    {
        fit = "WF";
    }

    fDisk disco;
    disco.administrarParticion(size, unit, path, fit, type, cdelete, name, add);
}


void Interprete::fmount(vector<string> commandArray)
{
    string name = "";
    string path = "";
    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        if (comando == "name")
        {
            name = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
    }
    Interprete::montaje.montarParticion(path, name);
}

void Interprete::funmount(vector<string> commandArray)
{
    string id = Interprete::getAtributo(commandArray[1]);
    ;
    cout << "ID: " << id << endl;
    Interprete::montaje.desmontarParticion(toLowerCase(id));
}

void Interprete::frep(vector<string> commandArray)
{
    string name = "";
    string path = "";
    string id = "";
    string ruta = "";
    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        string comandoID = Interprete::toLowerCase(commandArray[i].substr(1, 2));
        if (comando == "name")
        {
            name = Interprete::getAtributo(commandArray[i]);
        }
        else if (comandoID == "id")
        {
            id = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
        else if (comando == "ruta")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                ruta = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                ruta = Interprete::getFullPath(commandArray, i);
            }
        }
    }
    Rep reporte;
    reporte.crearReporte(path, toLowerCase(name), toLowerCase(id), montaje, ruta);
}


//OPERACIONES DE ARCHIVO Y TEXTO.

void Interprete::leerArchivo(string path)
{

    ifstream infile(path.c_str());

    std::string comando;
    string comandoCompleto = "";
    while (std::getline(infile, comando))
    {
        std::istringstream iss(comando); //comando is a line

        if(comandoCompleto.length() > 0){
            comandoCompleto = comandoCompleto.substr(0, comandoCompleto.length()-2);
        }

        comandoCompleto = comandoCompleto + comando;

        int tamanio = comandoCompleto.length();
        if(tamanio > 2){
            string salto = comandoCompleto.substr(tamanio-2, tamanio-1);
            if(salto == "\\^"){
                //comandoMultiple = true;
                continue;
            }else{
                if (comandoCompleto != "")
                {
                    char comentarioLetra = comandoCompleto.at(0);
                    if (comentarioLetra == '#')
                    {
                        cout << comandoCompleto << endl;
                        comandoCompleto = "";
                    }else{
                        cout << comandoCompleto << endl;
                        Interprete::lineaComando(comandoCompleto);
                        comandoCompleto = "";
                    }
                }
            }
        }
    }
}

void Interprete::errorComando(string error)
{
    cout << "Error en el comando: " << error << endl;
}

string Interprete::getAtributo(string comando)
{
    vector<string> commandArray;
    stringstream total(comando);
    string tmp;

    while (getline(total, tmp, '>'))
    {
        commandArray.push_back(tmp);
    }
    if (!commandArray[1].empty())
    {
        return commandArray[1];
    }
    else
    {
        cout << "Error en recoleccion de atributo" << endl;
    }
}

string Interprete::getPath(vector<string> commandArray)
{
    string path = commandArray[1];
    for (int i = 2; i < commandArray.size(); i++)
    {
        path = path + " " + commandArray[i];
    }
    if (path.substr(path.size() - 1, 1) == " ")
    {
        path = path.substr(0, path.size() - 1);
    }
    return path;
}

string Interprete::toLowerCase(string comando)
{
    string data = comando;
    transform(data.begin(), data.end(), data.begin(),
              [](unsigned char c) { return std::tolower(c); });
    return data;
}

string Interprete::getFullPath(vector<string> commandArray, int j)
{
    string path = commandArray[j].substr(7, commandArray[j].size() - 5);
    for (int i = j + 1; i < commandArray.size(); i++)
    {
        if (commandArray[i].substr(0, 1) != "&")
        {
            path = path + " " + commandArray[i];
        }
        else
        {
            break;
        }
    }
    if (path.substr(path.size() - 1, 1) == " ")
    {
        path = path.substr(0, path.size() - 1);
    }

    path.erase(remove(path.begin(), path.end(), '\"'), path.end());

    return path;
}

string Interprete::getFullDest(vector<string> commandArray, int j)
{
    string path = commandArray[j].substr(10, commandArray[j].size() - 5);
    for (int i = j + 1; i < commandArray.size(); i++)
    {
        if (commandArray[i].substr(0, 1) != "&")
        {
            path = path + " " + commandArray[i];
        }
        else
        {
            break;
        }
    }
    if (path.substr(path.size() - 1, 1) == " ")
    {
        path = path.substr(0, path.size() - 1);
    }

    path.erase(remove(path.begin(), path.end(), '\"'), path.end());

    return path;
}

void Interprete::fpause()
{
    cout << "Comando pause: ingresa un texto para continuar..." << endl;
    string respuesta;
    cin >> respuesta;
}



//------------------------------------------SEGUNDA FASE  - FILE SYSTEM ---------------------------------------------//

void Interprete::fmkfs(vector<string> commandArray)
{
    string id = "";
    string type = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        string comando2 = Interprete::toLowerCase(commandArray[i].substr(1, 2));
        if (comando == "type")
        {
            type = Interprete::toLowerCase(Interprete::getAtributo(commandArray[i]));
        }
        else if (comando2 == "id")
        {
            id = Interprete::toLowerCase(Interprete::getAtributo(commandArray[i]));
        }
    }

    if(type == ""){
        type = "full";
    }

    Mkfs fileSystem;
    fileSystem.formatearFS(id, type, montaje);
}


//------------------------------------------- ARCHIVOS -----------------------------------//
void Interprete::fmkfile(vector<string> commandArray)
{
    string path = "";
    string size = "";
    string cont = "";
    string id = "";
    int p = 0;

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        string comandoP = Interprete::toLowerCase(commandArray[i].substr(1, 1));
        string comando2 = Interprete::toLowerCase(commandArray[i].substr(1, 2));
        if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
        else if (comando == "size")
        {
            size = Interprete::getAtributo(commandArray[i]);
        }
        else if (comando == "cont")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                cont = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                cont = Interprete::getFullPath(commandArray, i);
            }
        }
        else if (comandoP == "p")
        {
            p = 1;
        }
        else if (comando2 == "id")
        {
            id = Interprete::toLowerCase(Interprete::getAtributo(commandArray[i]));
        }
    }

    Archivo file;
    file.makeFile(id, path, p, size, cont, montaje, false);
}
void Interprete::fcat(vector<string> commandArray)
{
    string path = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        if (comando == "file")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
    }

    Archivo file;
    file.showContent(path);
}
void Interprete::frem(vector<string> commandArray)
{
    string path = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
    }

    Archivo file;
    file.removeFile(path);
}
void Interprete::fedit(vector<string> commandArray)
{
    string path = "";
    string cont = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
        else if (comando == "cont")
        {
            cont = Interprete::getAtributo(commandArray[i]);
        }
    }

    Archivo file;
    file.editFile(path, cont);
}

//------------------------------------------- CARPETAS -----------------------------------//

void Interprete::fren(vector<string> commandArray)
{
    string id = "";
    string path = "";
    string name = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        string comando2 = Interprete::toLowerCase(commandArray[i].substr(1, 2));
        if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
        else if (comando == "name")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                name = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                name = Interprete::getFullPath(commandArray, i);
            }
        }
        else if (comando2 == "id")
        {
            id = Interprete::toLowerCase(Interprete::getAtributo(commandArray[i]));
        }
    }

    Carpeta directory;
    directory.renameFile(id, path, name, montaje);
}

void Interprete::fmkdir(vector<string> commandArray)
{
    string path = "";
    int p = 0;
    string id = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        string comandoP = Interprete::toLowerCase(commandArray[i].substr(1, 1));
        string comando2 = Interprete::toLowerCase(commandArray[i].substr(1, 2));
        if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
        else if (comandoP == "p")
        {
            p = 1;
        }
        else if (comando2 == "id")
        {
            id = Interprete::toLowerCase(Interprete::getAtributo(commandArray[i]));
        }
    }

    Carpeta directory;
    directory.makeDirectory(path, p, id, montaje, false);
}
void Interprete::fcp(vector<string> commandArray)
{
    string path = "";
    string dest = "";
    string id = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        string comando2 = Interprete::toLowerCase(commandArray[i].substr(1, 2));
        string comando3 = Interprete::toLowerCase(commandArray[i].substr(1, 7));
        if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
        else if (comando3 == "destiny")
        {
            if (commandArray[i].substr(10, 1) != "\"")
            {
                dest = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                dest = Interprete::getFullDest(commandArray, i);
            }
        }
        else if (comando2 == "id")
        {
            id = Interprete::toLowerCase(Interprete::getAtributo(commandArray[i]));
        }
    }

    Carpeta directory;
    directory.copyFile(path, dest, montaje, id);
}
void Interprete::fmv(vector<string> commandArray)
{
    string path = "";
    string dest = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
        else if (comando == "dest")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                dest = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                dest = Interprete::getFullPath(commandArray, i);
            }
        }
    }

    Carpeta directory;
    directory.moveFile(path, dest, montaje);
}
void Interprete::ffind(vector<string> commandArray)
{
    string path = "";
    string name = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 4));
        if (comando == "path")
        {
            if (commandArray[i].substr(7, 1) != "\"")
            {
                path = Interprete::getAtributo(commandArray[i]);
            }
            else
            {
                path = Interprete::getFullPath(commandArray, i);
            }
        }
        else if (comando == "name")
        {
            name = Interprete::getAtributo(commandArray[i]);
        }
    }

    Carpeta directory;
    directory.findFile(path, name, montaje);
}

void Interprete::frecovery(vector<string> commandArray)
{
    string id = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 2));
        if (comando == "id")
        {
            id = Interprete::getAtributo(commandArray[i]);
        }
    }

    Recovery fileSystem;
    fileSystem.recoveryFS(id, montaje);
}
void Interprete::floss(vector<string> commandArray)
{
    string id = "";

    for (int i = 0; i < commandArray.size(); i++)
    {
        string comando = Interprete::toLowerCase(commandArray[i].substr(1, 2));
        if (comando == "id")
        {
            id = Interprete::getAtributo(commandArray[i]);
        }
    }

    Recovery fileSystem;
    fileSystem.simulateLoss(id, montaje);
}


























