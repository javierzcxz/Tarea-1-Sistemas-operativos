#include <iostream>
#include <unistd.h> //usleep()
#include <sys/wait.h>
using namespace std;

int main( int argc, char* argv[]){
    int k;
    int ejecutandose = 0;
    int status;
   
// entraron los 3 que te pdio funciona
    if( argc == 3 )
    {
       k =  stoi(argv[2]);
       ejecutandose = 0;
    }
    else
    {
       cout << "Error" << endl;
       exit(1);
    }


    // ver si se pueden ejecutar más cosas
    if (ejecutandose < k)
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
        
    }
    else {cout << "Error al crear proceso " << endl;}

    

       }
       else // si no es mayor qu ek esperamos a que termine uno de los hijos 
       {
         pid_t terminado = waitpid(-1, &status, 0);
         ejecutandose --;
         if (WIFEXITED(status)) // si terminó normalmente)
         {

      
            if ( WEXITSTATUS(status) == 0)
            {
              cout << "Actividad terminó correctamente" << endl;
            }
            else {cout << "Actividad falló " << endl;}

         }
       }
       

 

}