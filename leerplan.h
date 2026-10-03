#ifndef LEERPLAN_H
#define LEERPLAN_H

#include <string>
#include <map>
#include "actividad.h"

using namespace std;

bool leerPlan(string nombreArchivo, map<string, Actividad>& grafo);

#endif