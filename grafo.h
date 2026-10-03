#ifndef GRAFO_H
#define GRAFO_H

#include <map>
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

bool estaLista(Actividad& actividad, map<string, Estado>& estados);

int abortarDependientes(string idFallida,
                        map<string, Actividad>& grafo,
                        map<string, Estado>& estados);

#endif