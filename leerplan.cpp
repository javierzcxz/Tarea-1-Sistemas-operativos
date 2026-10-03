#include <iostream>
#include <fstream>
#include <string>
#include "actividad.h"
#include "leerplan.h"

using namespace std;

void leerPlan(string nombreArchivo, Actividad actividades[], int &cantidad)
{
    ifstream archivo(nombreArchivo);

    if (!archivo.is_open())
    {
        cout << "Error al abrir el archivo" << endl;
        return;
    }

    string linea;

    while (getline(archivo, linea))
    {
        cout << linea << endl;
    }

    archivo.close();
}