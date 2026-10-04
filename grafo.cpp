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
        int cantidad = it->second.dependencias.size();

        for (int i = 0; i < cantidad; i++)
        {
            if (grafo.count(it->second.dependencias[i]) == 0)
            {
                cout << "Error: la actividad " << it->first
                     << " depende de una actividad que no existe" << endl;
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

    int cantidad = grafo[id].dependencias.size();

    for (int i = 0; i < cantidad; i++)
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
        if (hayCiclo(it->first, grafo, visitado, revisando))
        {
            cout << "Error: el plan tiene un ciclo" << endl;
            return false;
        }
    }
    return true;
}

int abortarDependientes(string idFallida,
                        map<string, Actividad>& grafo,
                        map<string, Estado>& estados)
{
    int abortadas = 0;
    map<string, Actividad>::iterator it;

    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        int cantidad = it->second.dependencias.size();

        for (int i = 0; i < cantidad; i++)
        {
            if (it->second.dependencias[i] == idFallida && estados[it->first] == PENDIENTE)
            {
                estados[it->first] = ABORTADA;
                abortadas++;
                abortadas = abortadas + abortarDependientes(it->first, grafo, estados);
            }
        }
    }
    return abortadas;
}

void iniciarEstados(map<string, Actividad>& grafo,
                    map<string, Estado>& estados)
{
    map<string, Actividad>::iterator it;

    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        estados[it->first] = PENDIENTE;
    }
}

void prepararEspera(map<string, Actividad>& grafo,
                    map<string, int>& faltan,
                    map<string, vector<string> >& dependientes,
                    vector<string>& listas)
{
    map<string, Actividad>::iterator it;

    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        int cantidad = it->second.dependencias.size();
        faltan[it->first] = cantidad;

        if (cantidad == 0)
        {
            listas.push_back(it->first);
        }

        for (int i = 0; i < cantidad; i++)
        {
            dependientes[it->second.dependencias[i]].push_back(it->first);
        }
    }
}

void avisarTerminada(string id,
                     map<string, int>& faltan,
                     map<string, vector<string> >& dependientes,
                     vector<string>& listas)
{
    int cantidad = dependientes[id].size();

    for (int i = 0; i < cantidad; i++)
    {
        string dependiente = dependientes[id][i];
        faltan[dependiente]--;

        if (faltan[dependiente] == 0)
        {
            listas.push_back(dependiente);
        }
    }
}