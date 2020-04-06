#ifndef STRUCTS_H
#define STRUCTS_H


class Structs
{
public:
    Structs();

    typedef struct particion
    {
        char Estado = '0';
        char type = ' ';
        char fit = ' ';
        int part_start = 0;
        int size = 0;
        char name[16] = "";
    } Particion; //typedef

    typedef struct MBR_Disco
    {
        int size;
        char date[20];
        int disk_signature;
        Particion mbr_particiones[4];

    } MBR;

    typedef struct EBR_Logical
    {
        char Estado = '0';
        char fit = ' ';
        int part_start = 0;
        int size = 0;
        int part_next = -1;
        char name[16] = "NoName";
    } EBR; //typedef


    //---------------- FASE2 -----------------------//
    // SUPER BLOQUE
    typedef struct
    {
        char nombre_hd[16] = "";
        int arbol_virtual_count = 0;
        int detalle_directorio_count = 0;
        int inodos_count = 0;
        int bloques_count = 0;
        int arbol_virtual_free = 0;
        int detalle_directorio_free = 0;
        int inodos_free = 0;
        int bloques_free = 0;
        char date_creacion[20] = "";
        char date_ultimo_montaje[20] = "";
        int montajes_count = 0;
        int start_bm_arbol_directorio = 0;
        int start_arbol_directorio = 0;
        int start_bm_detalle_directorio = 0;
        int start_detalle_directorio = 0;
        int start_bm_inodos = 0;
        int start_inodos = 0;
        int start_bm_bloques = 0;
        int start_bloques = 0;
        int start_log; //Bitacora.
        int size_struct_avd = sizeof(arbolVirtual);
        int size_struct_dd = sizeof(detalleDirectorio);
        int size_struct_inodo = sizeof(InodoArchivo);
        int size_struct_bloque = sizeof(bloqueDatos);
        int first_free_avd = 0;
        int first_free_dd = 0;
        int first_free_inodo = 0;
        int first_free_bloque = 0;
        int magic_num = 201701023;
    } SuperBloque; //typedef

    typedef struct
    {
        char date_creacion[20] = "";
        char nombre_directorio[20] = "";
        int array_subdirectorios[6] = {-1, -1, -1, -1, -1, -1};
        int detalle_directorio = -1;
        int avd_siguiente = -1;
        int avd_owner = -1;
    }arbolVirtual; //typedef

    typedef struct
    {
        char nombre_directorio[20] = "";
        int noInodo = -1;
        char date_creacion[20] = "";
        char date_modificacion[20] = "";
    }detalleArchivo; //typedef

    typedef struct
    {
        detalleArchivo archivos[5];
        int dd_siguiente = -1;
    }detalleDirectorio; //typedef

    typedef struct
    {
        int count_inodo;
        int size_archivo;
        int count_bloques_asignados;
        int array_bloques[4] = {-1, -1, -1, -1};
        int ap_indirecto = -1;
        int id_owner = -1;
    }InodoArchivo; //typedef

    typedef struct
    {
        char db_data[25] = "";
    }bloqueDatos; //typedef

    typedef struct
    {
        char tipo_operacion[10] = "";
        char tipo = '-1';
        char path[40] = "";
        char contenido[50] = ""; // Cuanto?
        char log_fecha[20] = "";
        int size = 0;
    }logBitacora; //typedef

};

#endif // STRUCTS_H
