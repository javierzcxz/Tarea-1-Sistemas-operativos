#include <iostream>
#include <map>
#include <string>
#include "actividad.h"
#include "grafo.h"

using namespace std;

bool dependenciasValidas(map<string, Actividad>& grafo)
{
    map<string, Actividad>::iterator it;

    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        for (int i = 0; i < it->second.dependencias.size(); i++)
        {
            string dependencia = it->second.dependencias[i];

            if (grafo.count(dependencia) == 0)
            {
                cout << "Error en la actividad " << it->first
                     << " depende de " << dependencia
                     << " que no existe" << endl;
                return false;
            }
        }
    }
    return true;
}

bool hayCiclo(string id, map<string, Actividad>& grafo,
              map<string, bool>& visitado,
              map<string, bool>& revisando)
{
    if (revisando[id])
    {
        return true;
    }
    if (visitado[id])
    {
        return false;
    }

    visitado[id] = true;
    revisando[id] = true;

    for (int i = 0; i < grafo[id].dependencias.size(); i++)
    {
        if (hayCiclo(grafo[id].dependencias[i], grafo, visitado, revisando))
        {
            return true;
        }
    }

    revisando[id] = false;
    return false;
}

bool revisarGrafo(map<string, Actividad>& grafo)
{
    if (!dependenciasValidas(grafo))
    {
        return false;
    }

    map<string, bool> visitado;
    map<string, bool> revisando;
    map<string, Actividad>::iterator it;

    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        if (!visitado[it->first])
        {
            if (hayCiclo(it->first, grafo, visitado, revisando))
            {
                cout << "Error: el plan tiene un ciclo" << endl;
                return false;
            }
        }
    }
    return true;
}

bool estaLista(Actividad& actividad, map<string, Estado>& estados)
{
    if (estados[actividad.id] != PENDIENTE)
    {
        return false;
    }

    for (int i = 0; i < actividad.dependencias.size(); i++)
    {
        if (estados[actividad.dependencias[i]] != TERMINADA)
        {
            return false;
        }
    }
    return true;
}

int abortarDependientes(string idFallida,
                        map<string, Actividad>& grafo,
                        map<string, Estado>& estados)
{
    int cantidad = 0;
    map<string, Actividad>::iterator it;

    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        if (estados[it->first] != PENDIENTE)
        {
            continue;
        }

        for (int i = 0; i < it->second.dependencias.size(); i++)
        {
            if (it->second.dependencias[i] == idFallida)
            {
                estados[it->first] = ABORTADA;
                cantidad++;
                cantidad = cantidad + abortarDependientes(it->first, grafo, estados);
                break;
            }
        }
    }
    return cantidad;
}