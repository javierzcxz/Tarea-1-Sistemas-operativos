#include <iostream>
#include <unistd.h> //usleep()
#include <sys/wait.h>
#include <map>
#include <ctime>
#include <string>
#include <cstdlib>
#include "grafo.h"
#include "leerplan.h"
#include <signal.h>

using namespace std;

bool ctrlc = false;

void crearAct (string idActividad, int tiempo_ms, int &ejecutandose, map<pid_t, string> &procesos_activos, map<pid_t, int> &pipes_activos)
{

//viene el pipe
 int fd[2]; // fd[0] para lectura, fd[1] para escritura
    // Crear el pipe
    if (pipe(fd) == -1)
    {
        perror("pipe");// imprime pq no funciona
        _exit(1);
    }

   pid_t pid = fork();
   if( pid == 0)
       {
         //hijo
        close(fd[0]);
        usleep(tiempo_ms * 1000);
        write(fd[1], idActividad.c_str(), idActividad.size());//lo q envio, cuanto envio el c_str de string a formato write
        close(fd[1]);
        _exit (0);

       }
       else if (pid > 0) //padre
       {
        close(fd[1]);

         ejecutandose ++;
         procesos_activos[pid] = idActividad;// así sabemos cual act tiene cual pid
        pipes_activos[pid] = fd[0];

        }
        else
          {
            cout << "Error al crear proceso " << endl;
          }

}


void esperaract (int &ejecutandose, map<pid_t, string> &procesos_activos,
                 map<string, Actividad> &grafo, map<string, Estado> &estados,
                 map<string, int> &faltan, map<string, vector<string> > &dependientes,
                 vector<string> &listas, map<pid_t, int> &pipes_activos)
{
   int status;
   pid_t terminado = waitpid(-1, &status, 0); // PID

   if(terminado == -1 && ctrlc == true) // si se hixo la señal y si waitpid manda error
   {
      return; //para dejar de uasr esperar act
   }

   ejecutandose --;

   string id = procesos_activos[terminado]; //ID

   if (WIFEXITED(status)) // si terminó normalmente osea usando el exit
   {

      if ( WEXITSTATUS(status) == 0) // toma el numero con el que terminó la act
      {

         cout << "Actividad " << id << " terminó correctamente" << endl;
         procesos_activos.erase(terminado);
         int fd_terminado = pipes_activos[terminado];

         char buffer[100];
         int nbytes = read(fd_terminado, buffer, sizeof(buffer));
         close(fd_terminado);
         pipes_activos.erase(terminado);
         estados[id]= TERMINADA;

         avisarTerminada(id, faltan, dependientes, listas);
      }
      else
      {
         cout << "Actividad falló " << endl;
         int fd_terminado = pipes_activos[terminado];
         procesos_activos.erase(terminado);
         close(fd_terminado);
         pipes_activos.erase(terminado);
         estados[id] = ABORTADA;
         abortarDependientes(id,grafo,estados);
      }

   }
}


void controlc(int signal)
{
    ctrlc = true;
}


int main( int argc, char* argv[]){
    int k;
    int ejecutandose = 0;
    map <pid_t, string > procesos_activos;
    map<pid_t, int> pipes_activos; // para guardar cada pipe de cada hijo

    //datos javi
    map<string, Actividad> grafo;
    map <string,  Estado> estados;
    map<string, int> faltan;
    map<string, vector<string> > dependientes;
    vector<string> listas;


      if( argc == 3 )
    {
       k =  stoi(argv[2]);

       if (k <= 0)
       {
         cout << "K debe ser mayor que 0" << endl;
         exit(1);
       }
    }
    else
    {
       cout << "Error" << endl;
       _exit(1);
    }


    srand(time(NULL));

    if (!leerPlan(argv[1], grafo) || !revisarGrafo(grafo))
    {
       return 1;
    }

    iniciarEstados(grafo, estados);
    prepararEspera(grafo, faltan, dependientes, listas);


    signal(SIGINT, controlc);


    while (listas.size() > 0 || ejecutandose > 0)
    {

      if(ctrlc == true)
      {
        for (auto iterador3 = procesos_activos.begin(); iterador3 != procesos_activos.end();iterador3++)
        {
          kill(iterador3->first, SIGTERM );
          waitpid(iterador3->first, NULL, 0); // para que no hayan zombies
        }

        for( auto iterador4 = pipes_activos.begin(); iterador4 != pipes_activos.end(); iterador4++)
        {
          close(iterador4->second);
        }

        break;

      }


        if (listas.size() > 0 && ejecutandose < k)
        {
            string id = listas.back();
            listas.pop_back();
            estados[id] = EJECUTANDO;

            crearAct(id, grafo[id].tiempo_ms, ejecutandose, procesos_activos,pipes_activos);
        }
        else
        {
            esperaract(ejecutandose,procesos_activos,grafo,estados,faltan,dependientes,listas,pipes_activos);
        }
    }


    return 0;
}