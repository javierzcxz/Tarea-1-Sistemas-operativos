#ifndef ACTIVIDAD_H
#define ACTIVIDAD_H
#include <string>
#include <vector>

using namespace std;

struct actividad {
    string id;
    string nombre;
    int tiempo;
    vector<string> dependencias;
};
#endif