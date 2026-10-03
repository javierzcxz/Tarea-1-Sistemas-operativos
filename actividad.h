#ifndef ACTIVIDAD_H
#define ACTIVIDAD_H

#include <string>
#include <vector>

using namespace std;

struct Actividad {
    string id;
    string nombre;
    int tiempo_ms;
    vector<string> dependencias;
};

#endif