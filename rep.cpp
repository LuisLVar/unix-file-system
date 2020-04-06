        #include "rep.h"
    #include "mkfs.h"

    Rep::Rep()
    {

    }

    void Rep::crearReporte(string path, string name, string id, Mount montaje, string ruta)
    {
        char pathDisco[100] = "";

        string pathD= "";
        int sizeParticion;
        int startParticion;
        int error = 0;
        string nombreParticion;

        Mkfs mk;
        mk.getDatosID(id, montaje, &pathD, &startParticion, &sizeParticion, &nombreParticion, &error);
        strcpy(pathDisco, pathD.c_str());

        if(error == 1){
            return;
        }

        Structs::MBR discoEditar;
        FILE *bfile2 = fopen(pathDisco, "rb+");
        if (bfile2 != NULL)
        {
            rewind(bfile2);
            fread(&discoEditar, sizeof(discoEditar), 1, bfile2);
        }
        else
        {
            cout << "Error. Path no existente, disco no existente." << endl;
            return;
        }
        fclose(bfile2);

        if (name == "mbr")
        {
            string codigoInterno = "";
            string size = to_string(discoEditar.size);
            string date(discoEditar.date);
            string firma = to_string(discoEditar.disk_signature);
            string fit = "";
            //fit.push_back(discoEditar.fit);

            codigoInterno = "<TR>\n"
                            "<TD><B>MBR_Tamanio</B></TD>\n"
                            "<TD>" +
                            size + "</TD>\n"
                                   "</TR>\n"
                                   "<TR>\n"
                                   "<TD><B>MBR_Fecha_Creacion</B></TD>\n"
                                   "<TD>" +
                            date + "</TD>\n"
                                   "</TR>\n"
                                   "<TR>\n"
                                   "<TD><B>MBR_Disk_Signature</B></TD>\n"
                                   "<TD>" +
                            firma + "</TD>\n"
                                    "</TR>\n"
                                    "<TR>\n"
                                    "<TD><B>MBR_Disk_Fit</B></TD>\n"
                                    "<TD>" +
                            fit + "</TD>\n"
                                  "</TR>\n";

            string codigoParticiones = "";
            for (int i = 0; i < 4; i++)
            {
                if (discoEditar.mbr_particiones[i].Estado == '1')
                {
                    string size = to_string(discoEditar.mbr_particiones[i].size);
                    string name(discoEditar.mbr_particiones[i].name);
                    string Estado = "";
                    Estado.push_back(discoEditar.mbr_particiones[i].Estado);
                    string fit = "";
                    fit.push_back(discoEditar.mbr_particiones[i].fit);
                    string type = "";
                    type.push_back(discoEditar.mbr_particiones[i].type);
                    string part_start = to_string(discoEditar.mbr_particiones[i].part_start);
                    string indice = to_string((i + 1));

                    codigoParticiones = codigoParticiones +
                                        "<TR>\n"
                                        "<TD><B>part_Estado_" +
                                        indice + "</B></TD>\n"
                                                 "<TD>" +
                                        Estado + "</TD>\n"
                                                 "</TR>\n"
                                                 "<TR>\n"
                                                 "<TD><B>part_Type_" +
                                        indice + "</B></TD>\n"
                                                 "<TD>" +
                                        type + "</TD>\n"
                                               "</TR>\n"
                                               "<TR>\n"
                                               "<TD><B>part_Fit_" +
                                        indice + "</B></TD>\n"
                                                 "<TD>" +
                                        fit + "</TD>\n"
                                              "</TR>\n"
                                              "<TR>\n"
                                              "<TD><B>part_Start_" +
                                        indice + "</B></TD>\n"
                                                 "<TD>" +
                                        part_start + "</TD>\n"
                                                     "</TR>\n"
                                                     "<TR>\n"
                                                     "<TD><B>part_Name_" +
                                        indice + "</B></TD>\n"
                                                 "<TD>" +
                                        name + "</TD>\n"
                                               "</TR>\n";
                }
            }

            string codigo = "digraph  {\n"
                            "graph[ratio = fill];\n"
                            " node [label=\"\N\", fontsize=15, shape=plaintext];\n"
                            "graph [bb=\"0,0,352,154\"];\n"
                            "arset [label=<\n"
                            " <TABLE ALIGN=\"LEFT\">\n"
                            "<TR>\n"
                            " <TD><B>Nombre</B></TD>\n"
                            "<TD><B> Valor </B></TD>\n"
                            "</TR>\n"
                            + codigoInterno +
                            codigoParticiones +
                            "</TABLE>\n"
                            ">, ];\n"
                            "}";

            string path1 = path;
            string pathPng = path1.substr(0, path1.size() - 4);
            pathPng = pathPng + ".png";

            FILE *validar = fopen(path1.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpng " + path1 + " -o " + pathPng;

                system(comando.c_str());
                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpng " + path1 + " -o " + pathPng;
                system(comando.c_str());
            }
        }
        else if (name == "disk")
        {
            string codigoInterno = "";
            string size = to_string(discoEditar.size);
            string date(discoEditar.date);
            string firma = to_string(discoEditar.disk_signature);
            string fit = "";
            //fit.push_back(discoEditar.fit);
            int acumulado = 0;

            string codigoParticiones = "";
            string codigoLogicas = "";
            for (int i = 0; i < 4; i++)
            {
                if (discoEditar.mbr_particiones[i].Estado == '1')
                {
                    string name(discoEditar.mbr_particiones[i].name);
                    string Estado = "";
                    Estado.push_back(discoEditar.mbr_particiones[i].Estado);
                    string type = "";
                    type.push_back(discoEditar.mbr_particiones[i].type);
                    int porcentaje = ((discoEditar.mbr_particiones[i].size * 100) / discoEditar.size);
                    acumulado = acumulado + porcentaje;
                    string porcentajeString = to_string(porcentaje);

                    if (type == "P")
                    {
                        codigoParticiones = codigoParticiones +
                                            "<TD>    <TABLE BORDER=\"0\">\n"
                                            "<TR><TD>" +
                                            name + " (" + type + ")"
                                                                 "</TD></TR>\n"
                                                                 "<TR><TD>" +
                                            porcentajeString + "%</TD></TR>\n"
                                                               "</TABLE>\n"
                                                               "</TD>\n";
                    }
                    else if (type == "E")
                    {
                        int inicioLogicas = discoEditar.mbr_particiones[i].part_start;
                        Structs::EBR logicaI;
                        FILE *bfilel = fopen(pathDisco, "rb+");
                        fseek(bfilel, inicioLogicas, SEEK_SET);
                        fread(&logicaI, sizeof(Structs::EBR), 1, bfilel);
                        while(logicaI.part_next != -1){
                            if(logicaI.Estado != '0'){
                                codigoLogicas = codigoLogicas + "<TD> Logica: "+logicaI.name+"</TD>\n <TD>EBR</TD>/n";
                            }
                            fseek(bfilel, logicaI.part_next, SEEK_SET);
                            fread(&logicaI, sizeof(Structs::EBR), 1, bfilel);
                        }
                        fclose(bfilel);


                        codigoParticiones = codigoParticiones +
                                            "<TD> <TABLE BORDER=\"1\">\n"
                                                                     "<TR><TD>"+name+ " (EXTENDIDA) -- "+porcentajeString+"% --</TD></TR>\n"
                                                                     "<TR><TD><TABLE ALIGN=\"LEFT\">\n"
                                                                     "<TR>\n"
                                                                     "<TD>EBR</TD>\n" + codigoLogicas +
                                                                     "</TR></TABLE></TD></TR>\n"
                                                                     "</TABLE></TD>\n";
                    }
                }
            }
            codigoLogicas = "";
            int libre = 100 - acumulado;
            string libreString = to_string(libre);
            if(acumulado<100){
                        codigoParticiones = codigoParticiones +
                                            "<TD>    <TABLE BORDER=\"0\">\n"
                                            "<TR><TD> Libre "
                                                                 "</TD></TR>\n"
                                                                 "<TR><TD>" +libreString + "%</TD></TR>\n"
                                                               "</TABLE>\n"
                                                               "</TD>\n";

            }

            string codigo = "digraph  {\n"
                            "graph[ratio = fill];\n"
                            " node [label=\"\N\", fontsize=15, shape=plaintext];\n"
                            "graph [bb=\"0,0,352,154\"];\n"
                            "arset [label=<\n"
                            " <TABLE ALIGN=\"LEFT\">\n"
                            "<TR>\n"
                            "<TD>MBR</TD>\n"
                            +codigoParticiones +
                            "</TR>\n" +
                            "</TABLE>\n"
                            ">, ];\n"
                            "}";

            string path1 = path;
            string pathPng = path1.substr(0, path1.size() - 4);
            pathPng = pathPng + ".png";

            FILE *validar = fopen(path1.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpng " + path1 + " -o " + pathPng;

                system(comando.c_str());
                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpng " + path1 + " -o " + pathPng;
                system(comando.c_str());
            }
        }
        else if (name == "bm_arbdir")
        {
            string codigo = "";

            Structs::SuperBloque superBloque;
            FILE *bfile = fopen(pathD.c_str(), "rb+");
            fseek(bfile, startParticion, SEEK_SET);
            fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

            char bit = ' ';
            for(int i = 0; i<superBloque.arbol_virtual_count; i++){
                fseek(bfile, (superBloque.start_bm_arbol_directorio + i), SEEK_SET);
                fread(&bit, sizeof(char), 1, bfile);
                codigo = codigo + bit + " | ";
                if((i+1)%20 == 0){
                    codigo = codigo + "\n";
                }
            }
            fclose(bfile);

            FILE *validar = fopen(path.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path);
                outfile << codigo.c_str() << endl;
                outfile.close();

                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path);
                outfile << codigo.c_str() << endl;
                outfile.close();
            }
        }
        else if (name == "bm_detdir")
        {

            string codigo = "";

            Structs::SuperBloque superBloque;
            FILE *bfile = fopen(pathD.c_str(), "rb+");
            fseek(bfile, startParticion, SEEK_SET);
            fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

            char bit = ' ';
            for(int i = 0; i<superBloque.detalle_directorio_count; i++){
                fseek(bfile, (superBloque.start_bm_detalle_directorio + i), SEEK_SET);
                fread(&bit, sizeof(char), 1, bfile);
                codigo = codigo + bit + " | ";
                if((i+1)%20 == 0){
                    codigo = codigo + "\n";
                }
            }
            fclose(bfile);

            FILE *validar = fopen(path.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path);
                outfile << codigo.c_str() << endl;
                outfile.close();

                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path);
                outfile << codigo.c_str() << endl;
                outfile.close();
            }

        }
        else if (name == "bm_inode")
        {
            string codigo = "";

            Structs::SuperBloque superBloque;
            FILE *bfile = fopen(pathD.c_str(), "rb+");
            fseek(bfile, startParticion, SEEK_SET);
            fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

            char bit = ' ';
            for(int i = 0; i<superBloque.inodos_count; i++){
                fseek(bfile, (superBloque.start_bm_inodos + i), SEEK_SET);
                fread(&bit, sizeof(char), 1, bfile);
                codigo = codigo + bit + " | ";
                if((i+1)%20 == 0){
                    codigo = codigo + "\n";
                }
            }
            fclose(bfile);

            FILE *validar = fopen(path.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path);
                outfile << codigo.c_str() << endl;
                outfile.close();

                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path);
                outfile << codigo.c_str() << endl;
                outfile.close();
            }
        }
        else if (name == "bm_block")
        {
            string codigo = "";

            Structs::SuperBloque superBloque;
            FILE *bfile = fopen(pathD.c_str(), "rb+");
            fseek(bfile, startParticion, SEEK_SET);
            fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

            char bit = ' ';
            for(int i = 0; i<superBloque.bloques_count; i++){
                fseek(bfile, (superBloque.start_bm_bloques + i), SEEK_SET);
                fread(&bit, sizeof(char), 1, bfile);
                codigo = codigo + bit + " | ";
                if((i+1)%20 == 0){
                    codigo = codigo + "\n";
                }
            }
            fclose(bfile);

            FILE *validar = fopen(path.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path);
                outfile << codigo.c_str() << endl;
                outfile.close();

                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path);
                outfile << codigo.c_str() << endl;
                outfile.close();
            }
        }
        else if (name == "sb")
        {

            Structs::SuperBloque superBloque;
            FILE *bfile = fopen(pathD.c_str(), "rb+");
            fseek(bfile, startParticion, SEEK_SET);
            fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

            string codigoInterno = "";

            codigoInterno = "<TR>\n"
                            "<TD><B>Nombre_hd</B></TD>\n"
                            "<TD>" +
                    string(superBloque.nombre_hd) + "</TD>\n"
                                            "</TR>\n"
                                            "<TR>\n"
                                            "<TD><B>Arbol_virtual_count</B></TD>\n"
                                            "<TD>" +
                    to_string(superBloque.arbol_virtual_count) + "</TD>\n"
                                                      "</TR>\n"
                                                      "<TR>\n"
                                                      "<TD><B>Detalle_Directorio_count</B></TD>\n"
                                                      "<TD>" +
                    to_string(superBloque.detalle_directorio_count) + "</TD>\n"
                                                           "</TR>\n"
                                                           "<TR>\n"
                                                           "<TD><B>Inodos_Count</B></TD>\n"
                                                           "<TD>" +
                    to_string(superBloque.inodos_count) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Bloques_Count</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.bloques_count) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Arbol_Virtual_free</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.arbol_virtual_free) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Detalle_Directorio_free</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.detalle_directorio_free) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Inodos_free</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.inodos_free) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Bloques_free</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.bloques_free) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Fecha_creacion</B></TD>\n"
                          "<TD>" +
                    string(superBloque.date_creacion) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Fecha_Ultimo_montaje</B></TD>\n"
                          "<TD>" +
                    string(superBloque.date_ultimo_montaje) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Montajes_Count</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.montajes_count) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Start_Bitmap_Arbol_Directorio</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.start_bm_arbol_directorio) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Start_Arbol_Directorio</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.start_arbol_directorio) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Start_Bitmap_Detalle_Directorio</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.start_bm_detalle_directorio) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Start_Detalle_Directorio</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.start_detalle_directorio) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Start_Bitmap_Inodos</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.start_bm_inodos) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Start_Inodos</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.start_inodos) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Start_Bitmap_Bloques</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.start_bm_bloques) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Start_Bloques</B></TD>\n"
                          "<TD>" +
                   to_string(superBloque.start_bloques) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Start_Log</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.start_log) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Size_Struct_AVD</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.size_struct_avd) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Size_Struct_DD</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.size_struct_dd) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Size_Struct_Inodo</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.size_struct_inodo) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Size_Struct_Bloque</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.size_struct_bloque) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>First_free_avd</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.first_free_avd) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>First_free_dd</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.first_free_dd) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>First_free_Inodo</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.first_free_inodo) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>First_free_Bloque</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.first_free_bloque) + "</TD>\n"
                          "</TR>\n"
                          "<TR>\n"
                          "<TD><B>Magic_Number</B></TD>\n"
                          "<TD>" +
                    to_string(superBloque.magic_num) + "</TD>\n"
                          "</TR>\n";


            string codigo = "digraph  {\n"
                            "graph[ratio = fill];\n"
                            " node [label=\"\N\", fontsize=15, shape=plaintext];\n"
                            "graph [bb=\"0,0,352,154\"];\n"
                            "arset [label=<\n"
                            " <TABLE ALIGN=\"LEFT\">\n"
                            "<TR>\n"
                            "<TD colspan = \"3\"><B>SUPERBLOQUE</B></TD>\n"
                            "</TR>\n"
                            "<TR>\n"
                            " <TD><B>Nombre</B></TD>\n"
                            "<TD><B> Valor </B></TD>\n"
                            "</TR>\n"
                            +codigoInterno +
                            "</TABLE>\n"
                            ">, ];\n"
                            "}";

            string path1 = path;
            string pathPng = path1.substr(0, path1.size() - 4);
            pathPng = pathPng + ".png";

            FILE *validar = fopen(path1.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpng " + path1 + " -o " + pathPng;

                system(comando.c_str());
                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpng " + path1 + " -o " + pathPng;
                system(comando.c_str());
            }

        }
        else if (name == "ls")
        {

        }
        else if (name == "bitacora")
        {
            Structs::SuperBloque superBloque;
            FILE *bfile = fopen(pathD.c_str(), "rb+");
            fseek(bfile, startParticion, SEEK_SET);
            fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

            Structs::logBitacora raizBitacora;
            fseek(bfile, superBloque.start_log, SEEK_SET);
            fread(&raizBitacora, sizeof(Structs::logBitacora), 1, bfile);
            fclose(bfile);
            string nodos = "";
            string nombre = "";
            string texto = "";
            string enlace = "";
            Structs::logBitacora bitacora;

            for(int i =1; i<raizBitacora.size ;i++){
            FILE *bfile = fopen(pathD.c_str(), "rb+");
            fseek(bfile, superBloque.start_log + (i*sizeof(Structs::logBitacora)), SEEK_SET);
            fread(&bitacora, sizeof(Structs::logBitacora), 1, bfile);

            nombre = "Log "+to_string(i);
            texto = "nodeb"+to_string(i)+" [label=\"{"+nombre+"|{{Tipo_Operacion|"+string(bitacora.tipo_operacion)+"}|"
            "{Tipo|"+bitacora.tipo+"}|{Path|"+string(bitacora.path)+"}|{Contenido|"+string(bitacora.contenido)+"}|{Fecha Log|"
            ""+string(bitacora.log_fecha)+"}|{Size|"+to_string(bitacora.size)+"}}}\"];\n";

            nodos = nodos + texto;

            if(i != 1){
                enlace = enlace + "nodeb"+to_string(i-1)+" -> nodeb"+to_string(i)+" [color=\"white\"];\n";
            }

            fclose(bfile);
            }

            string codigo =
            "digraph D {\n"
                "node [shape=record fontname=\"Aria\" style=filled, fillcolor=azure1];\n"
                + nodos + enlace +
            "}";



            string path1 = path;
            string pathPdf = path1.substr(0, path1.size() - 4);
            pathPdf = pathPdf + ".pdf";

            FILE *validar = fopen(path1.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpdf " + path1 + " -o " + pathPdf;

                std::ofstream outfile1("/home/luis/Escritorio/directorio.txt");
                outfile1 << codigo.c_str() << endl;
                outfile1.close();

                system(comando.c_str());
                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();

                std::ofstream outfile1("/home/luis/Escritorio/bitacora.txt");
                outfile1 << codigo.c_str() << endl;
                outfile1.close();
                string comando = "dot -Tpdf " + path1 + " -o " + pathPdf;
                system(comando.c_str());
            }
        }
        else if (name == "directorio")
        {
            Structs::SuperBloque superBloque;
            FILE *bfile = fopen(pathD.c_str(), "rb+");
            fseek(bfile, startParticion, SEEK_SET);
            fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

            Structs::arbolVirtual raiz;
            fseek(bfile, superBloque.start_arbol_directorio, SEEK_SET);
            fread(&raiz, sizeof(Structs::arbolVirtual), 1, bfile);
            fclose(bfile);

            string nodos = "";
            string enlaces = "";

            getDirectorio(raiz, pathD, superBloque, &nodos, &enlaces, 0);

            string codigo =
            "digraph D {\n"
                "node [shape=record fontname=\"Aria\" style=filled, fillcolor=azure1];\n"
                + nodos + enlaces +
            "}";



            string path1 = path;
            string pathPdf = path1.substr(0, path1.size() - 4);
            pathPdf = pathPdf + ".pdf";

            FILE *validar = fopen(path1.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpdf " + path1 + " -o " + pathPdf;

                std::ofstream outfile1("/home/luis/Escritorio/directorio.txt");
                outfile1 << codigo.c_str() << endl;
                outfile1.close();

                system(comando.c_str());
                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();

                std::ofstream outfile1("/home/luis/Escritorio/directorio.txt");
                outfile1 << codigo.c_str() << endl;
                outfile1.close();
                string comando = "dot -Tpdf " + path1 + " -o " + pathPdf;
                system(comando.c_str());
            }
        }
        else if (name == "tree_file")
        {

            string route;
            if(ruta != ""){
                route = ruta;
            }else{
                cout << "Ingresa la direccion del archivo que deseas reportar: " << endl;
                cin >> route;
            }

            vector<string> pathArray;
            stringstream total(route);
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

            string nodos = "";
            string enlaces = "";

            getTreeFile(raiz, pathD, superBloque, &nodos, &enlaces, 0, pathArray);

            string codigo =
            "digraph D {\n"
                "rankdir=\"LR\";"
                "node [shape=record fontname=\"Aria\" style=filled, fillcolor=azure1];\n"
                + nodos + enlaces +
            "}";

            string path1 = path;
            string pathPdf = path1.substr(0, path1.size() - 4);
            pathPdf = pathPdf + ".pdf";

            FILE *validar = fopen(path1.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpdf " + path1 + " -o " + pathPdf;

                system(comando.c_str());
                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpdf " + path1 + " -o " + pathPdf;
                system(comando.c_str());
            }

        }
        else if (name == "tree_directorio")
        {


            string route;
            if(ruta != ""){
                route = ruta;
            }else{
                cout << "Ingresa la direccion de la carpeta que deseas reportar: " << endl;
                cin >> route;
            }


            vector<string> pathArray;
            stringstream total(route);
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

            string nodos = "";
            string enlaces = "";

            getTreeDirectorio(raiz, pathD, superBloque, &nodos, &enlaces, 0, pathArray);

            string codigo =
            "digraph D {\n"
                "node [shape=record fontname=\"Aria\" style=filled, fillcolor=azure1];\n"
                + nodos + enlaces +
            "}";

            string path1 = path;
            string pathPdf = path1.substr(0, path1.size() - 4);
            pathPdf = pathPdf + ".pdf";

            FILE *validar = fopen(path1.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpdf " + path1 + " -o " + pathPdf;

                system(comando.c_str());
                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();
                string comando = "dot -Tpdf " + path1 + " -o " + pathPdf;
                system(comando.c_str());
            }
        }
        else if (name == "tree_complete")
        {

            Structs::SuperBloque superBloque;
            FILE *bfile = fopen(pathD.c_str(), "rb+");
            fseek(bfile, startParticion, SEEK_SET);
            fread(&superBloque, sizeof(Structs::SuperBloque), 1, bfile); //Leo el superbloque al inicio de la particion

            Structs::arbolVirtual raiz;
            fseek(bfile, superBloque.start_arbol_directorio, SEEK_SET);
            fread(&raiz, sizeof(Structs::arbolVirtual), 1, bfile);
            fclose(bfile);

            string nodos = "";
            string enlaces = "";

            getTreeComplete(raiz, pathD, superBloque, &nodos, &enlaces, 0);

            string codigo =
            "digraph D {\n"
                "node [shape=record fontname=\"Aria\" style=filled, fillcolor=azure1];\n"
                + nodos + enlaces +
            "}";

            string path1 = path;
            string pathPdf = path1.substr(0, path1.size() - 4);
            pathPdf = pathPdf + ".pdf";

            FILE *validar = fopen(path1.c_str(), "r");
            if (validar != NULL)
            {
                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();

                std::ofstream outfile1("/home/luis/Escritorio/complete.txt");
                outfile1 << codigo.c_str() << endl;
                outfile1.close();

                string comando = "dot -Tpdf " + path1 + " -o " + pathPdf;

                system(comando.c_str());
                fclose(validar);
            }
            else
            {
                string comando1 = "mkdir -p \"" + path + "\"";
                string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                system(comando2.c_str());

                std::ofstream outfile(path1);
                outfile << codigo.c_str() << endl;
                outfile.close();

                std::ofstream outfile1("/home/luis/Escritorio/complete.txt");
                outfile1 << codigo.c_str() << endl;
                outfile1.close();

                string comando = "dot -Tpdf " + path1 + " -o " + pathPdf;
                system(comando.c_str());
            }
        }
        else //inode y file.
        {
            cout << "Error: nombre de reporte incorrecto." << endl;
        }
    }

    void Rep::getDirectorio(Structs::arbolVirtual avd, string pathD, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, int pointer){
        int apuntador = 0;



        string primero = to_string(avd.array_subdirectorios[0]);
        string segundo = to_string(avd.array_subdirectorios[1]);
        string tercero = to_string(avd.array_subdirectorios[2]);
        string cuarto = to_string(avd.array_subdirectorios[3]);
        string quinto = to_string(avd.array_subdirectorios[4]);
        string sexto = to_string(avd.array_subdirectorios[5]);
        string detalle = to_string(avd.array_subdirectorios[6]);
        string indirecto = to_string(avd.array_subdirectorios[7]);

        string nombre = avd.nombre_directorio;
        if(nombre == "raiz"){
            nombre = " ";
        }

        string texto = "node"+to_string(pointer)+" [label=\"{\\\\"+nombre+"|{{"+primero+"|<c"+primero+">}|{"+segundo+"|<c"+segundo+">}|{"+tercero+"|"
                                                                                                                                                  "<c"+tercero+">}|{"+cuarto+"|<c"+cuarto+">}|{"+quinto+"|<c"+quinto+">}|{"+sexto+"|<c"+sexto+">}|"+detalle+"|{"+indirecto+"|<c"+indirecto+">}}}\"];\n";

        string enlace = "";


        for(int i = 0; i<6; i++){
            apuntador = avd.array_subdirectorios[i];
            if(apuntador == -1){
                break;
            }
            enlace = enlace + "node"+to_string(pointer)+":c"+to_string(apuntador)+" -> node"+to_string(apuntador)+";\n";
        }

        *codigo = *codigo + texto;
        *codigoEnlaces = *codigoEnlaces + enlace;


        for(int i = 0; i<6; i++){
            FILE *bfileC = fopen(pathD.c_str(), "rb+");
            apuntador = avd.array_subdirectorios[i];

            if(apuntador == -1){
                fclose(bfileC);
                return;
            }

            Structs::arbolVirtual carpetaHijo;
            fseek(bfileC, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
            fread(&carpetaHijo, sizeof(Structs::arbolVirtual), 1, bfileC);
            fclose(bfileC);
            getDirectorio(carpetaHijo, pathD, superBloque, codigo, codigoEnlaces, apuntador);
        }



        //NINGUNO CUMPLE, SE MUEVE AL APUNTADOR INDIRECTO
        apuntador = avd.avd_siguiente;

        if(apuntador == -1){
            return;
        }

        FILE *bfile1 = fopen(pathD.c_str(), "rb+");
        Structs::arbolVirtual carpetaIndirecta;
        fseek(bfile1, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
        fread(&carpetaIndirecta, sizeof(Structs::arbolVirtual), 1, bfile1);

        enlace = "node"+to_string(pointer)+":c"+to_string(apuntador)+" -> node"+to_string(apuntador)+";\n";
        *codigoEnlaces = *codigoEnlaces + enlace;
        fclose(bfile1);
        getDirectorio(carpetaIndirecta, pathD, superBloque, codigo, codigoEnlaces, apuntador);


        return;
    }

    void Rep::getTreeFile(Structs::arbolVirtual avd, string pathD, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, int pointer, vector<string> path){

        FILE *bfile = fopen(pathD.c_str(), "rb+");

        string primero = to_string(avd.array_subdirectorios[0]);
        string segundo = to_string(avd.array_subdirectorios[1]);
        string tercero = to_string(avd.array_subdirectorios[2]);
        string cuarto = to_string(avd.array_subdirectorios[3]);
        string quinto = to_string(avd.array_subdirectorios[4]);
        string sexto = to_string(avd.array_subdirectorios[5]);
        string detalle = to_string(avd.array_subdirectorios[6]);
        string indirecto = to_string(avd.array_subdirectorios[7]);

        string nombre = avd.nombre_directorio;
        if(nombre == "raiz"){
            nombre = " ";
        }

        string texto = "node"+to_string(pointer)+" [label=\"{\\\\"+nombre+"|{{"+primero+"|<c"+primero+">}|{"+segundo+"|<c"+segundo+">}|{"+tercero+"|"
                                                                                                                                                  "<c"+tercero+">}|{"+cuarto+"|<c"+cuarto+">}|{"+quinto+"|<c"+quinto+">}|{"+sexto+"|<c"+sexto+">}|{"+detalle+"|<d"+detalle+">}|{"+indirecto+"|<c"+indirecto+">}}}\"];\n";

        *codigo = *codigo + texto;

        string enlace = "";

        int apuntador = 0;
        for(int i = 0; i<6; i++){
            apuntador = avd.array_subdirectorios[i];
            Structs::arbolVirtual carpetaHijo;
            fseek(bfile, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
            fread(&carpetaHijo, sizeof(Structs::arbolVirtual), 1, bfile);


            if(carpetaHijo.nombre_directorio == path[0]){
                path.erase(path.begin());
                if(path.size() == 1){  // Me direcciono a un Detalle de Directorio.
                    apuntador = carpetaHijo.detalle_directorio;
                    Structs::detalleDirectorio Archivos;
                    fseek(bfile, (superBloque.start_detalle_directorio+(apuntador*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
                    fread(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);
                    fclose(bfile);
                    //Creo el enlace
                    enlace = "node"+to_string(apuntador)+":d"+to_string(apuntador)+" -> noded"+to_string(apuntador)+";\n";
                    *codigoEnlaces = *codigoEnlaces + enlace;

                    enlace = "node"+to_string(pointer)+":c"+to_string(apuntador)+" -> node"+to_string(apuntador)+";\n";
                    *codigoEnlaces = *codigoEnlaces + enlace;

                    string textod = "node"+to_string(apuntador)+" [label=\"{\\\\"+string(carpetaHijo.nombre_directorio)+"|{|}|{|}|{|}|{|}|{|}|{|}|"
                                                                                                                        "{"+to_string(apuntador)+"|<d"+to_string(apuntador)+">}|{|}}\"];\n";

                    *codigo = *codigo + textod;

                    recorrerDetalle(Archivos, apuntador, path, pathD, superBloque, codigo, codigoEnlaces, carpetaHijo.nombre_directorio);
                    return;
                }else{
                    fclose(bfile);
                    enlace = "node"+to_string(pointer)+":c"+to_string(apuntador)+" -> node"+to_string(apuntador)+";\n";
                    *codigoEnlaces = *codigoEnlaces + enlace;
                    *codigo = *codigo + texto;
                    getTreeFile(carpetaHijo, pathD, superBloque, codigo, codigoEnlaces, apuntador, path);
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

        enlace = "node"+to_string(pointer)+":c"+to_string(apuntador)+" -> node"+to_string(apuntador)+";\n";
        *codigoEnlaces = *codigoEnlaces + enlace;

        getTreeFile(carpetaIndirecta, pathD, superBloque, codigo, codigoEnlaces, apuntador, path);
        return;
    }


    void Rep::recorrerDetalle(Structs::detalleDirectorio Archivos, int apuntador, vector<string> path,
                              string pathDisco, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, string nombreCarpeta){

        FILE *bfile = fopen(pathDisco.c_str(), "rb+");

        string primero = to_string(Archivos.archivos[0].noInodo);
        string segundo = to_string(Archivos.archivos[1].noInodo);
        string tercero = to_string(Archivos.archivos[2].noInodo);
        string cuarto = to_string(Archivos.archivos[3].noInodo);
        string quinto = to_string(Archivos.archivos[4].noInodo);
        string primeroN = string(Archivos.archivos[0].nombre_directorio);
        string segundoN = string(Archivos.archivos[1].nombre_directorio);
        string terceroN = string(Archivos.archivos[2].nombre_directorio);
        string cuartoN = string(Archivos.archivos[3].nombre_directorio);
        string quintoN = string(Archivos.archivos[4].nombre_directorio);
        string indirecto = to_string(Archivos.dd_siguiente);



        string texto = "noded"+to_string(apuntador)+" [label=\"{DD "+nombreCarpeta+"|{"+primeroN+"|"+primero+"|<i"+primero+">}"
     "|{"+segundoN+"|"+segundo+"|<i"+segundo+">}|{"+terceroN+"|"+tercero+"|<i"+tercero+">}|{"+cuartoN+"|"+cuarto+""
     "|<i"+cuarto+">}|{"+quintoN+"|"+quinto+"|<i"+quinto+">}|{*|"+indirecto+"|<d"+indirecto+">}}\"];\n";

        *codigo = *codigo + texto;


        for(int i = 0; i<5; i++){
            int pointer = Archivos.archivos[i].noInodo;

            if(path[0] == Archivos.archivos[i].nombre_directorio){

                string enlace = "noded"+to_string(apuntador)+":i"+to_string(pointer)+" -> nodei"+to_string(pointer)+";\n";
                *codigoEnlaces = *codigoEnlaces + enlace;
                //Mando a crear el inodo.
                Structs::InodoArchivo inodo;
                fseek(bfile, (superBloque.start_inodos + (pointer*(sizeof(Structs::InodoArchivo)))), SEEK_SET);
                fread(&inodo, sizeof(Structs::InodoArchivo), 1, bfile);
                fclose(bfile);
                recorrerInodo(inodo, superBloque, pathDisco, codigo, codigoEnlaces, path[0]);
                return;
            }
        }

        //Si no lo encuentra se va al apuntador indirecto.
        int apuntadorA = Archivos.dd_siguiente;
        Structs::detalleDirectorio detalleIndirecto;
        fseek(bfile, (superBloque.start_detalle_directorio+(apuntadorA*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en la carpeta hija
        fread(&detalleIndirecto, sizeof(Structs::detalleDirectorio), 1, bfile);
        fclose(bfile);

        string enlace = "noded"+to_string(apuntador)+":i"+to_string(apuntadorA)+" -> noded"+to_string(apuntadorA)+";\n";
        *codigoEnlaces = *codigoEnlaces + enlace;

        recorrerDetalle(detalleIndirecto, apuntadorA, path,  pathDisco, superBloque, codigo, codigoEnlaces, nombreCarpeta);
        return;
    }



    void Rep::recorrerInodo(Structs::InodoArchivo inodo, Structs::SuperBloque superBloque, string pathDisco, string * codigo, string * codigoEnlaces, string nombre){

        string primero = to_string(inodo.array_bloques[0]);
        string segundo = to_string(inodo.array_bloques[1]);
        string tercero = to_string(inodo.array_bloques[2]);
        string cuarto = to_string(inodo.array_bloques[3]);
        int indirecto = inodo.ap_indirecto;

        string texto = "nodei"+to_string(inodo.count_inodo)+" [label=\"{Inodo "+nombre+"|{"+primero+"|<b"+primero+">}|{"+segundo+"|"
                                                                                                                                 "<b"+segundo+">}|{"+tercero+"|<b"+tercero+">}|{"+cuarto+"|<b"+cuarto+">}|{*"+to_string(indirecto)+"|<i"+to_string(indirecto)+">}}\"];\n";

        *codigo = *codigo + texto;

        for(int i = 0; i<inodo.count_bloques_asignados;i++){
            int pointer = inodo.array_bloques[i];

            FILE *bfile = fopen(pathDisco.c_str(), "rb+");
            Structs::bloqueDatos bloque;
            fseek(bfile, (superBloque.start_bloques + (pointer * sizeof(Structs::bloqueDatos))), SEEK_SET);
            fread(&bloque, sizeof(Structs::bloqueDatos), 1, bfile);
            fclose(bfile);

            string textoB = "nodeb"+to_string(pointer)+" [label=\""+bloque.db_data+"\"];\n";

            *codigo = *codigo + textoB;

            string enlace = "nodei"+to_string(inodo.count_inodo)+":b"+to_string(pointer)+" -> nodeb"+to_string(pointer)+";\n";
            *codigoEnlaces = *codigoEnlaces + enlace;
        }

        //INDIRECTO
        if(inodo.ap_indirecto != -1){
            FILE *bfile2 = fopen(pathDisco.c_str(), "rb+");
            Structs::InodoArchivo inodoIndirecto;
            fseek(bfile2, (superBloque.start_inodos + (indirecto*(sizeof(Structs::InodoArchivo)))), SEEK_SET);
            fread(&inodoIndirecto, sizeof(Structs::InodoArchivo), 1, bfile2);
            fclose(bfile2);
            string enlace2 = "nodei"+to_string(inodo.count_inodo)+":i"+to_string(indirecto)+" -> nodei"+to_string(indirecto)+";\n";
            *codigoEnlaces = *codigoEnlaces + enlace2;

            recorrerInodo(inodoIndirecto, superBloque, pathDisco, codigo, codigoEnlaces, nombre);
        }
    }


void Rep::getTreeComplete(Structs::arbolVirtual avd, string pathD, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, int pointer){

    int apuntador = 0;

    string primero = to_string(avd.array_subdirectorios[0]);
    string segundo = to_string(avd.array_subdirectorios[1]);
    string tercero = to_string(avd.array_subdirectorios[2]);
    string cuarto = to_string(avd.array_subdirectorios[3]);
    string quinto = to_string(avd.array_subdirectorios[4]);
    string sexto = to_string(avd.array_subdirectorios[5]);
    string detalle = to_string(avd.array_subdirectorios[6]);
    string indirecto = to_string(avd.array_subdirectorios[7]);
    int detail = avd.array_subdirectorios[6];

    string nombre = avd.nombre_directorio;
    if(nombre == "raiz"){
        nombre = " ";
    }

    string texto = "node"+to_string(pointer)+" [label=\"{\\\\"+nombre+"|{{"+primero+"|<c"+primero+">}|{"+segundo+"|<c"+segundo+">}|{"+tercero+"|"
      "<c"+tercero+">}|{"+cuarto+"|<c"+cuarto+">}|{"+quinto+"|<c"+quinto+">}|{"+sexto+"|<c"+sexto+">}|{"+detalle+"|<d"+detalle+">}|{"+indirecto+"|<c"+indirecto+">}}}\"];\n";

    string enlace = "";


    //Recorro Carpetas para generar enlaces
    for(int i = 0; i<6; i++){
        apuntador = avd.array_subdirectorios[i];
        if(apuntador == -1){
            break;
        }
        enlace = enlace + "node"+to_string(pointer)+":c"+to_string(apuntador)+" -> node"+to_string(apuntador)+";\n";
    }

    *codigo = *codigo + texto;
    *codigoEnlaces = *codigoEnlaces + enlace;

    //Valido si existen archivos en su detalle de directorio.
    FILE *bfile = fopen(pathD.c_str(), "rb+");
    Structs::detalleDirectorio Archivos;
    fseek(bfile, (superBloque.start_detalle_directorio+(detail*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
    fread(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);
    fclose(bfile);

    if(Archivos.archivos[0].noInodo != -1){ //Tiene al menos un archivo, debo graficarlo.
        //Creo el enlace
        enlace = "node"+to_string(pointer)+":d"+to_string(detail)+" -> noded"+to_string(detail)+";\n";
        *codigoEnlaces = *codigoEnlaces + enlace;
        recorrerDetalleComplete(Archivos, detail, pathD, superBloque, codigo, codigoEnlaces, nombre);
    }

    //Creo carpetas
    for(int i = 0; i<6; i++){
        apuntador = avd.array_subdirectorios[i];

        if(apuntador == -1){
            return;
        }
        FILE *bfileC = fopen(pathD.c_str(), "rb+");
        Structs::arbolVirtual carpetaHijo;
        fseek(bfileC, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
        fread(&carpetaHijo, sizeof(Structs::arbolVirtual), 1, bfileC);
        fclose(bfileC);
        getTreeComplete(carpetaHijo, pathD, superBloque, codigo, codigoEnlaces, apuntador);
    }



    //NINGUNO CUMPLE, SE MUEVE AL APUNTADOR INDIRECTO
    apuntador = avd.avd_siguiente;

    if(apuntador == -1){
        return;
    }

    FILE *bfile1 = fopen(pathD.c_str(), "rb+");
    Structs::arbolVirtual carpetaIndirecta;
    fseek(bfile1, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
    fread(&carpetaIndirecta, sizeof(Structs::arbolVirtual), 1, bfile1);

    enlace = "node"+to_string(pointer)+":c"+to_string(apuntador)+" -> node"+to_string(apuntador)+";\n";
    *codigoEnlaces = *codigoEnlaces + enlace;
    fclose(bfile1);
    getTreeComplete(carpetaIndirecta, pathD, superBloque, codigo, codigoEnlaces, apuntador);
}


void Rep::recorrerDetalleComplete(Structs::detalleDirectorio Archivos, int apuntador,
                          string pathDisco, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, string nombreCarpeta){

    string primero = to_string(Archivos.archivos[0].noInodo);
    string segundo = to_string(Archivos.archivos[1].noInodo);
    string tercero = to_string(Archivos.archivos[2].noInodo);
    string cuarto = to_string(Archivos.archivos[3].noInodo);
    string quinto = to_string(Archivos.archivos[4].noInodo);
    string primeroN = string(Archivos.archivos[0].nombre_directorio);
    string segundoN = string(Archivos.archivos[1].nombre_directorio);
    string terceroN = string(Archivos.archivos[2].nombre_directorio);
    string cuartoN = string(Archivos.archivos[3].nombre_directorio);
    string quintoN = string(Archivos.archivos[4].nombre_directorio);
    string indirecto = to_string(Archivos.dd_siguiente);



    string texto = "noded"+to_string(apuntador)+" [label=\"{DD "+nombreCarpeta+"|{"+primeroN+"|"+primero+"|<i"+primero+">}"
            "|{"+segundoN+"|"+segundo+"|<i"+segundo+">}|{"+terceroN+"|"+tercero+"|<i"+tercero+">}|{"+cuartoN+"|"+cuarto+""
            "|<i"+cuarto+">}|{"+quintoN+"|"+quinto+"|<i"+quinto+">}|{*|"+indirecto+"|<d"+indirecto+">}}\"];\n";

    *codigo = *codigo + texto;

    for(int i = 0; i<5; i++){
        int pointer = Archivos.archivos[i].noInodo;
        string nombreArchivo = Archivos.archivos[i].nombre_directorio;
        if(pointer == -1){
            return;
        }
        FILE *bfile = fopen(pathDisco.c_str(), "rb+");
        string enlace = "noded"+to_string(apuntador)+":i"+to_string(pointer)+" -> nodei"+to_string(pointer)+";\n";
        *codigoEnlaces = *codigoEnlaces + enlace;
        //Mando a crear el inodo.
        Structs::InodoArchivo inodo;
        fseek(bfile, (superBloque.start_inodos + (pointer*(sizeof(Structs::InodoArchivo)))), SEEK_SET);
        fread(&inodo, sizeof(Structs::InodoArchivo), 1, bfile);
        fclose(bfile);
        recorrerInodo(inodo, superBloque, pathDisco, codigo, codigoEnlaces, nombreArchivo);
    }

    //se va al apuntador indirecto.
    int apuntadorA = Archivos.dd_siguiente;
    if(apuntadorA != -1){
        FILE *bfile = fopen(pathDisco.c_str(), "rb+");
        Structs::detalleDirectorio detalleIndirecto;
        fseek(bfile, (superBloque.start_detalle_directorio+(apuntadorA*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en la carpeta hija
        fread(&detalleIndirecto, sizeof(Structs::detalleDirectorio), 1, bfile);
        fclose(bfile);

        string enlace = "noded"+to_string(apuntador)+":d"+to_string(apuntadorA)+" -> noded"+to_string(apuntadorA)+";\n";
        *codigoEnlaces = *codigoEnlaces + enlace;

        recorrerDetalleComplete(detalleIndirecto, apuntadorA, pathDisco, superBloque, codigo, codigoEnlaces, nombreCarpeta);
    }
}


void Rep::getTreeDirectorio(Structs::arbolVirtual avd, string pathD, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, int pointer, vector<string> path){

    FILE *bfile = fopen(pathD.c_str(), "rb+");

    if(path.size() == 0){
        string primero = to_string(avd.array_subdirectorios[0]);
        string segundo = to_string(avd.array_subdirectorios[1]);
        string tercero = to_string(avd.array_subdirectorios[2]);
        string cuarto = to_string(avd.array_subdirectorios[3]);
        string quinto = to_string(avd.array_subdirectorios[4]);
        string sexto = to_string(avd.array_subdirectorios[5]);
        string detalle = to_string(avd.array_subdirectorios[6]);
        string indirecto = to_string(avd.array_subdirectorios[7]);
        int detail = avd.array_subdirectorios[6];

        string nombre = avd.nombre_directorio;
        if(nombre == "raiz"){
            nombre = " ";
        }

        string texto = "node"+to_string(pointer)+" [label=\"{\\\\"+nombre+"|{{"+primero+"|<c"+primero+">}|{"+segundo+"|<c"+segundo+">}|{"+tercero+"|"
        "<c"+tercero+">}|{"+cuarto+"|<c"+cuarto+">}|{"+quinto+"|<c"+quinto+">}|{"+sexto+"|<c"+sexto+">}|{"+detalle+"|<d"+detalle+">}|{"+indirecto+"|<c"+indirecto+">}}}\"];\n";

        *codigo = *codigo + texto;

        string enlace = "";

        //Creo el enlace
        enlace = "node"+to_string(pointer)+":d"+to_string(detail)+" -> noded"+to_string(detail)+";\n";
        *codigoEnlaces = *codigoEnlaces + enlace;

        Structs::detalleDirectorio Archivos;
        fseek(bfile, (superBloque.start_detalle_directorio+(detail*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en el detalle de directorio.
        fread(&Archivos, sizeof(Structs::detalleDirectorio), 1, bfile);
        fclose(bfile);

        recorrerDetalleDirectorio(Archivos, detail, pathD, superBloque, codigo, codigoEnlaces, avd.nombre_directorio);

        return;
    }


    int apuntador = 0;
    for(int i = 0; i<6; i++){
        apuntador = avd.array_subdirectorios[i];
        Structs::arbolVirtual carpetaHijo;
        fseek(bfile, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
        fread(&carpetaHijo, sizeof(Structs::arbolVirtual), 1, bfile);

        if(carpetaHijo.nombre_directorio == path[0]){
            path.erase(path.begin());
            fclose(bfile);
            getTreeDirectorio(carpetaHijo, pathD, superBloque, codigo, codigoEnlaces, apuntador, path);
            return;
        }
    }

    //NINGUNO CUMPLE, SE MUEVE AL APUNTADOR INDIRECTO
    apuntador = avd.avd_siguiente;
    Structs::arbolVirtual carpetaIndirecta;
    fseek(bfile, (superBloque.start_arbol_directorio+(apuntador*sizeof(Structs::arbolVirtual))), SEEK_SET);//nos posicionamos en la carpeta hija
    fread(&carpetaIndirecta, sizeof(Structs::arbolVirtual), 1, bfile);
    fclose(bfile);

    getTreeDirectorio(carpetaIndirecta, pathD, superBloque, codigo, codigoEnlaces, apuntador, path);
}

void Rep::recorrerDetalleDirectorio(Structs::detalleDirectorio Archivos, int apuntador,
                                    string pathDisco, Structs::SuperBloque superBloque, string * codigo, string * codigoEnlaces, string nombreCarpeta){

    string primero = to_string(Archivos.archivos[0].noInodo);
    string segundo = to_string(Archivos.archivos[1].noInodo);
    string tercero = to_string(Archivos.archivos[2].noInodo);
    string cuarto = to_string(Archivos.archivos[3].noInodo);
    string quinto = to_string(Archivos.archivos[4].noInodo);
    string primeroN = string(Archivos.archivos[0].nombre_directorio);
    string segundoN = string(Archivos.archivos[1].nombre_directorio);
    string terceroN = string(Archivos.archivos[2].nombre_directorio);
    string cuartoN = string(Archivos.archivos[3].nombre_directorio);
    string quintoN = string(Archivos.archivos[4].nombre_directorio);
    string indirecto = to_string(Archivos.dd_siguiente);



    string texto = "noded"+to_string(apuntador)+" [label=\"{DD "+nombreCarpeta+"|{"+primeroN+"|"+primero+"|<i"+primero+">}"
    "|{"+segundoN+"|"+segundo+"|<i"+segundo+">}|{"+terceroN+"|"+tercero+"|<i"+tercero+">}|{"+cuartoN+"|"+cuarto+""
    "|<i"+cuarto+">}|{"+quintoN+"|"+quinto+"|<i"+quinto+">}|{*|"+indirecto+"|<d"+indirecto+">}}\"];\n";

    *codigo = *codigo + texto;


    //se va al apuntador indirecto.
    int apuntadorA = Archivos.dd_siguiente;
    if(apuntadorA != -1){
        FILE *bfile = fopen(pathDisco.c_str(), "rb+");
        Structs::detalleDirectorio detalleIndirecto;
        fseek(bfile, (superBloque.start_detalle_directorio+(apuntadorA*sizeof(Structs::detalleDirectorio))), SEEK_SET);//nos posicionamos en la carpeta hija
        fread(&detalleIndirecto, sizeof(Structs::detalleDirectorio), 1, bfile);
        fclose(bfile);

        string enlace = "noded"+to_string(apuntador)+":d"+to_string(apuntadorA)+" -> noded"+to_string(apuntadorA)+";\n";
        *codigoEnlaces = *codigoEnlaces + enlace;

        recorrerDetalleDirectorio(detalleIndirecto, apuntadorA, pathDisco, superBloque, codigo, codigoEnlaces, nombreCarpeta);
    }
}




















