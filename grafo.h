#ifndef GRAFO_H
#define GRAFO_H

#include <string>
#include <map>
#include <vector>
#include "actividad.h"

using namespace std;

enum Estado {
    PENDIENTE,
    EJECUTANDO,
    TERMINADA,
    ABORTADA
};

bool dependenciasValidas(map<string, Actividad>& grafo);

bool hayCiclo(string id, map<string, Actividad>& grafo,
              map<string, bool>& visitado,
              map<string, bool>& revisando);

bool revisarGrafo(map<string, Actividad>& grafo);


int abortarDependientes(string idFallida,
                        map<string, Actividad>& grafo,
                        map<string, Estado>& estados);

void iniciarEstados(map<string, Actividad>& grafo,
                    map<string, Estado>& estados);

void prepararEspera(map<string, Actividad>& grafo,
                    map<string, int>& faltan,
                    map<string, vector<string> >& dependientes,
                    vector<string>& listas);

void avisarTerminada(string id,
                     map<string, int>& faltan,
                     map<string, vector<string> >& dependientes,
                     vector<string>& listas);

#endif