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
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

bool ctrlc = false;

void crearpipes(map<string, Actividad> &grafo) //crea un pipe por cad aact
{
    for (auto iterador5 = grafo.begin(); iterador5 != grafo.end(); iterador5++)
    {
        string nombrepipe = "actividad_" + iterador5->first;

        unlink(nombrepipe.c_str()); // si hay un pipe antiguo se borra 

        mkfifo(nombrepipe.c_str(), 0666);
    }
}

void abrirpipes(map<string, Actividad> &grafo, map<string, int> &pipes_lectura)
{
    for (auto iterador6 = grafo.begin(); iterador6 != grafo.end(); iterador6++)
    {
        string nombrepipe = "actividad_" + iterador6->first;

        pipes_lectura[iterador6->first] = open(nombrepipe.c_str(), O_RDONLY | O_NONBLOCK);
    }
}

void crearAct (string idActividad, int tiempo_ms, int &ejecutandose, map<pid_t, string> &procesos_activos,  map<string, vector<string> > &dependientes)
{

pid_t pid  = fork();
   if( pid == 0)
       {
         signal(SIGINT, SIG_DFL);// para que ignore la señal 
         //hijo
        usleep(tiempo_ms * 1000);
        int cantidad = dependientes[idActividad].size();
        for(int i = 0; i < cantidad; i++) // recoore patra sabr que act depende de otr act
         {
       // toma el id de la act dependiendte 
         string nombrepipe = "actividad_" + dependientes[idActividad][i];

         int fd = open(nombrepipe.c_str(), O_WRONLY); // abre el pipe a usar , usando el nombre y usandolo solo para escribri 

         write(fd, idActividad.c_str(), idActividad.size()); // donde , que , cuanto 

         close(fd);
         
         }

         exit(0);

   }
       else if (pid > 0) //padre
       {
        
         ejecutandose ++;
         procesos_activos[pid] = idActividad;// así sabemos cual act tiene cual pid
        

        }
        else
          {
            cout << "Error al crear proceso " << endl;
          }

}


void esperaract (int &ejecutandose, map<pid_t, string> &procesos_activos,
                 map<string, Actividad> &grafo, map<string, Estado> &estados,
                 map<string, int> &faltan, map<string, vector<string> > &dependientes,
                 vector<string> &listas, map<string, int> &pipes_lectura)
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
         estados[id] = TERMINADA;
         procesos_activos.erase(terminado);

          int cantidadDep = dependientes[id].size();
          for(int i = 0; i < cantidadDep; i++)

         {
            string iddependiente = dependientes[id][i];

            char buffer[100];

            read(pipes_lectura[iddependiente], buffer, sizeof(buffer));
         }

         avisarTerminada(id, faltan, dependientes, listas);

      }
      else
      {
         cout << "Actividad falló " << endl;
         procesos_activos.erase(terminado);
      
         estados[id] = ABORTADA;
         abortarDependientes(id,grafo,estados);
      }

   }
}

void controlc(int)
{
    ctrlc = true;
}


int main( int argc, char* argv[]){
    int k;
    int ejecutandose = 0;
    map <pid_t, string > procesos_activos;
    map<string, int> pipes_lectura;
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
    crearpipes(grafo);
    abrirpipes(grafo,pipes_lectura);


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

        for( auto iterador4 = pipes_lectura.begin(); iterador4 != pipes_lectura.end(); iterador4++)
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

            crearAct(id, grafo[id].tiempo_ms, ejecutandose, procesos_activos,dependientes);
        }
        else
        {
            esperaract(ejecutandose,procesos_activos,grafo,estados,faltan,dependientes,listas,pipes_lectura);
        }
    }


    for(auto iterador7 = grafo.begin(); iterador7 != grafo.end(); iterador7++)
      {
         string nombrepipe = "actividad_" + iterador7->first;

         close(pipes_lectura[iterador7->first]);

         unlink(nombrepipe.c_str());
      }


    return 0;




}