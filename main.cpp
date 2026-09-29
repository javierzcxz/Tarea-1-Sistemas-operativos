#include <iostream>
#include <unistd.h> //usleep()
#include <sys/wait.h>
#include <map>
using namespace std;

void crearAct (int idActividad, int tiempo_ms, int &ejecutandose, map<pid_t, int> &procesos_activos )
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

void esperaract (int &ejecutandose , map<pid_t, int> &procesos_activos)
{
   int status;
   pid_t terminado = waitpid(-1, &status, 0);
         ejecutandose --;
         if (WIFEXITED(status)) // si terminó normalmente)
         {

      
            if ( WEXITSTATUS(status) == 0)
            {
              int act_terminada = procesos_activos[terminado];
              cout << "Actividad " << act_terminada << " terminó correctamente" << endl;
              procesos_activos.erase(terminado);
            }
            else {cout << "Actividad falló " << endl;}
            procesos_activos.erase();

         }
}

int main( int argc, char* argv[]){
    int k;
    int ejecutandose = 0;
    map <pid_t, int > procesos_activos;
    //datos javi
     int idActividad;
     int tiempo_ms;



      if( argc == 3 )
    {
       k =  stoi(argv[2]);
    }
    else
    {
       cout << "Error" << endl;
       exit(1);
    }

   
 while (//javi coloca la condición de las actividades listas con sus dependencias etc)
 {


   // ver si se pueden ejecutar más cosas
    if (ejecutandose < k)
    {
       crearAct(idActividad, tiempo_ms,ejecutandose,procesos_activos);

       }
       else // si no es mayor qu ek esperamos a que termine uno de los hijos 
       {
        esperaract(ejecutandose, procesos_activos);
       }
 }

 while (ejecutandose > 0)
 {
   esperaract(ejecutandose,procesos_activos);
 }
 





   

  

 

}