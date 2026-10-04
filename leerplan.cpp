#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>
#include "actividad.h"
#include "leerplan.h"

using namespace std;

string limpiar(string texto)
{
    int inicio = 0;
    int final = texto.length() - 1;

    while (inicio <= final && (texto[inicio] == ' ' || texto[inicio] == '\r'))
    {
        inicio++;
    }
    while (final >= inicio && (texto[final] == ' ' || texto[final] == '\r'))
    {
        final--;
    }

    string resultado = "";
    for (int i = inicio; i <= final; i++)
    {
        resultado = resultado + texto[i];
    }
    return resultado;
}

bool leerPlan(string nombreArchivo, map<string, Actividad>& grafo)
{
    ifstream archivo(nombreArchivo);

    if (!archivo.is_open())
    {
        cout << "Error al abrir el archivo" << endl;
        return false;
    }

    string linea;

    while (getline(archivo, linea))
    {
        if (limpiar(linea) == "")
        {
            continue;
        }

        stringstream ss(linea);
        string id, nombre, tiempo, dependencias;

        getline(ss, id, ':');
        getline(ss, nombre, ':');
        getline(ss, tiempo, ':');
        getline(ss, dependencias);

        Actividad actividad;
        actividad.id = limpiar(id);
        actividad.nombre = limpiar(nombre);

        tiempo = limpiar(tiempo);
        if (tiempo == "")
        {
            actividad.tiempo_ms = 100 + rand() % 4901;
        }
        else
        {
            actividad.tiempo_ms = stoi(tiempo);
        }

        stringstream ssDependencias(dependencias);
        string dependencia;
        while (getline(ssDependencias, dependencia, ','))
        {
            dependencia = limpiar(dependencia);
            if (dependencia != "")
            {
                actividad.dependencias.push_back(dependencia);
            }
        }

        grafo[actividad.id] = actividad;
    }

    archivo.close();
    return true;
}