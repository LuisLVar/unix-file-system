#include "interprete.h"


int main()
{
    string inicio = "Bienvenido a la consola de comandos... ('x' para finalizar)";
    cout << inicio << endl;
    Interprete inter;
    inter.interpretar();
    return 0;
}
