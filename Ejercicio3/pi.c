//Ejercicio 3 
//Tarea Programada 2 
//Estructuras de Computadores Digitales 2 II-2018 
//Alejandro Rojas Cordero B36049

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Variables Globales
int X, Y;
double gpie = 3.0;
pthread_mutex_t mutex;

//Funcion PI
void* Pii(void* arg)
{
    long j = (long)arg;    
    long lower = (X/Y)*(j-1)+1;
    long upper = ((X/Y)*(j));
    double SumaParcial = 0;
  

    for(long i = lower; i <= upper; i++)
    {        
        if(i % 2 == 0){
            SumaParcial -= 4.0/((2*i)*(2*i+1)*(2*i+2)); 
            
                  }
        else{
            SumaParcial += 4.0/((2*i)*(2*i+1)*(2*i+2));
          
                  }


    }
    pthread_mutex_lock (&mutex);
    gpie = gpie + SumaParcial;
    pthread_mutex_unlock (&mutex);    

    pthread_exit(0);
    //return NULL;
}

int main(int argc, char **argv)
{

    if(argc != 3) {
        printf("Error: Must send it 2 parameters, you sent %d\n", argc-1);
        exit(1);
    }
    X = atoi(argv[1]);
    Y = atoi(argv[2]); 


    if(X <= Y) {
        printf("Error: Number of terms must be greater then number of threads.\n");
        exit(1);    
    }



    //launch threads
    pthread_attr_t attr;
    pthread_t *tids = (pthread_t *) calloc(Y, sizeof(pthread_t));
    if(tids == NULL) {
        fprintf(stderr, "Memory allocation problem\n");
        exit(1);
    }


        pthread_mutex_init(&mutex, NULL);
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(long i = 1; i<=Y; i++)
    {

        int r = pthread_create(&tids[i], &attr, Pii, (void*)i);
        if(r<0) {
            printf("ERROR: pthread_create() returned %d\n", r);
            exit(2);
        }
    }

    //wait for threads...
    for(int k = 1; k<=Y; k++)
    {
        pthread_join(tids[k], NULL);
    }


    printf("pi computed with %d terms in %d threads is %.20f\n", X, Y, gpie);

    pthread_mutex_destroy(&mutex);    
    pthread_attr_destroy(&attr);

    free(tids);

    return 0;
}
