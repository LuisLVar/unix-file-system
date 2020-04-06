#include "rmdisk.h"

rmDisk::rmDisk()
{
}

void rmDisk::eliminarDisco(std::string path)
{
    if (remove(path.c_str()) == 0)
    {
        std::cout << "Disco eliminado exitosamente." << std::endl;
    }
    else
    {
        std::cout << "Disco no existente" << std::endl;
    }
}
