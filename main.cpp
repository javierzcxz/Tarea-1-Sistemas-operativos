#include <iostream>
#include <unistd.h> //usleep()
#include <sys/wait.h>
#include <map>
#include <ctime>
#include <string>
#include <cstdlib>
#include "grafo.h"
#include "leerplan.h"
using namespace std;

void crearAct (string idActividad, int tiempo_ms, int &ejecutandose, map<pid_t, string> &procesos_activos )
{
   pid_t pid = fork();
   if( pid == 0)
       {
         //hijo
         //simulamos que tenemos la act
         usleep(tiempo_ms * 1000);
         exit (0);

       }
       else if (pid > 0){

         ejecutandose ++;
         procesos_activos[pid] = idActividad;// así sabemos cual act tiene cual pid 
        
    }
    else {cout << "Error al crear proceso " << endl;}

}

void esperaract (int &ejecutandose, map<pid_t, string> &procesos_activos,
                 map<string, Actividad> &grafo, map<string, Estado> &estados,
                 map<string, int> &faltan, map<string, vector<string> > &dependientes,
                 vector<string> &listas)
{
   int status;
   pid_t terminado = waitpid(-1, &status, 0);
   ejecutandose --;

   string id = procesos_activos[terminado];
   procesos_activos.erase(terminado);

   if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
   {
      estados[id] = TERMINADA;
      avisarTerminada(id, faltan, dependientes, listas);
      cout << "Actividad " << id << " terminó correctamente" << endl;
   }
   else
   {
      estados[id] = ABORTADA;
      abortarDependientes(id, grafo, estados);
      cout << "Actividad " << id << " falló" << endl;
   }
}

int main( int argc, char* argv[]){
    int k;
    int ejecutandose = 0;
    map<pid_t, string> procesos_activos;
    map<string, Actividad> grafo;
    map<string, Estado> estados;
    map<string, int> faltan;
    map<string, vector<string> > dependientes;
    vector<string> listas;

    if( argc == 3 )
    {
       k = stoi(argv[2]);
       if (k <= 0)
       {
         cout << "K debe ser mayor que 0" << endl;
         exit(1);
       }
    }
    else
    {
       cout << "Error" << endl;
       exit(1);
    }

    srand(time(NULL));

    if (!leerPlan(argv[1], grafo) || !revisarGrafo(grafo))
    {
       return 1;
    }

    iniciarEstados(grafo, estados);
    prepararEspera(grafo, faltan, dependientes, listas);

    while (listas.size() > 0 || ejecutandose > 0)
    {
        if (listas.size() > 0 && ejecutandose < k)
        {
            string id = listas.back();
            listas.pop_back();
            estados[id] = EJECUTANDO;
            crearAct(id, grafo[id].tiempo_ms, ejecutandose, procesos_activos);
        }
        else
        {
            esperaract(ejecutandose, procesos_activos, grafo, estados, faltan, dependientes, listas);
        }
    }

    return 0;
}