#include <iostream> 
#include <unistd.h> //usleep() 
#include <sys/wait.h> 
#include <map> 
#include "actividad.h"
#include "grafo.h"
using namespace std; 
 
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
  
 
void esperaract (int &ejecutandose , map<pid_t, string> &procesos_activos,  map <string,  Estado> &estados,  map<string, Actividad> &grafo, map<pid_t, int> &pipes_activos) 
{ 
   int status; 
   pid_t terminado = waitpid(-1, &status, 0); // PID
         ejecutandose --; 
         string act_terminada = procesos_activos[terminado]; //ID
         if (WIFEXITED(status)) // si terminó normalmente osea usando el exit 
         { 
 
       
            if ( WEXITSTATUS(status) == 0) // toma el numero con el que terminó la act 
            { 
              
              cout << "Actividad " << act_terminada << " terminó correctamente" << endl; 
              procesos_activos.erase(terminado);
              int fd_terminado = pipes_activos[terminado];

              char buffer[100];
              int nbytes = read(fd_terminado, buffer, sizeof(buffer));
              close(fd_terminado);
              pipes_activos.erase(terminado);
              estados[act_terminada]= TERMINADA;
            } 
            else 
            {
            cout << "Actividad falló " << endl;
            int fd_terminado = pipes_activos[terminado];
            procesos_activos.erase(terminado);
            close(fd_terminado);
            pipes_activos.erase(terminado);
            estados[act_terminada] = ABORTADA;
            abortarDependientes(act_terminada,grafo,estados);
            }
 
         } 
} 
 
int main( int argc, char* argv[]){ 
    int k; 
    int ejecutandose = 0; 
    map <pid_t, string > procesos_activos;
    map<pid_t, int> pipes_activos; // para guardar cada pipe de cada hijo 
   
    //datos javi 
     int idActividad; 
     int tiempo_ms; 
     map <string,  Estado> estados;
     
     
     
 
      if( argc == 3 ) 
    { 
       k =  stoi(argv[2]); 
    } 
    else 
    { 
       cout << "Error" << endl; 
       _exit(1); 
    } 
    
  while(true)
  {
      
      for (auto iterador = grafo.begin(); iterador != grafo.end(); iterador++)
     {
         
       if (estaLista(iterador->second, estados ) && ejecutandose < k)
       {
          
          crearAct(iterador-> first, iterador->second.tiempo_ms, ejecutandose, procesos_activos,pipes_activos); 
          estados[iterador->first] = EJECUTANDO;
          
       }
     }
       if(ejecutandose > 0)
       {
           esperaract(ejecutandose,procesos_activos,estados,grafo, pipes_activos);
       }
       
       //first -> ID; second -> estado
        bool quedanActividades = false;
         for (auto iterador_dos = estados.begin(); iterador_dos != estados.end(); iterador_dos++)
         {
             if (iterador_dos->second == PENDIENTE || iterador_dos->second == EJECUTANDO)
             {
                 quedanActividades= true;
             }
         }
       
       if (quedanActividades == false) // termina el while 
       {
           break;
       }
       
       
  }


 
 
 
 
 
 
    
 
   
 
  
 
}