#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>

using namespace std;

const int CAPACIDAD_SILO = 5;
const int TOTAL_BOLSAS = 400;
int cafe_en_silo = 0;
int bolsas_en_bodega = 0;

pthread_mutex_t candado;
pthread_cond_t cond_tostadora;
pthread_cond_t cond_empacadora;

bool produccion_terminada = false;
bool parada_global = false; // Nueva bandera para detener la producción

void *tostadora(void *arg)
{
     int id = *(int *)arg;
     while (true)
     {
          pthread_mutex_lock(&candado);

          // Revisar la bandera de parada global
          if (parada_global)
          {
               pthread_mutex_unlock(&candado);
               break; // Detener la producción si la bandera está activada
          }

          if (bolsas_en_bodega >= TOTAL_BOLSAS)
          {
               parada_global = true; // Marcar la producción como terminada
               pthread_cond_signal(&cond_empacadora);
               pthread_mutex_unlock(&candado);
               break; // Terminar la producción cuando se alcanzan las 400 bolsas
          }

          while (cafe_en_silo >= CAPACIDAD_SILO && !parada_global)
          {
               pthread_cond_wait(&cond_tostadora, &candado);
          }

          if (parada_global)
          {
               pthread_mutex_unlock(&candado);
               break; // Verificar si ya hemos alcanzado el límite de bolsas
          }

          cafe_en_silo++;
          cout << "Tostadora: " << id << " produjo: 1 lb de cafe tostado" << endl;
          pthread_cond_signal(&cond_empacadora);
          pthread_mutex_unlock(&candado);
          sleep(1);
     }

     pthread_exit(nullptr);
}

void *empacadora(void *arg)
{
     while (true)
     {
          pthread_mutex_lock(&candado);

          // Revisar si ya debemos detenernos
          if (parada_global && cafe_en_silo == 0)
          {
               pthread_mutex_unlock(&candado);
               break; // Detener si ya alcanzamos las 400 bolsas y el silo está vacío
          }

          while (cafe_en_silo == 0 && !parada_global && bolsas_en_bodega < TOTAL_BOLSAS)
          {
               pthread_cond_wait(&cond_empacadora, &candado);
          }

          // Revisar nuevamente la condición de parada
          if (parada_global && cafe_en_silo == 0)
          {
               pthread_mutex_unlock(&candado);
               break; // Detener si ya hemos producido suficientes bolsas
          }

          if (cafe_en_silo > 0)
          {
               cafe_en_silo--;
               bolsas_en_bodega++;
               cout << "Empacadora produjo: 1 bolsa de cafe. Total bolsas en bodega: " << bolsas_en_bodega << endl;

               // Si alcanzamos el total, marcar el final de la producción
               if (bolsas_en_bodega >= TOTAL_BOLSAS)
               {
                    parada_global = true;
               }
          }

          pthread_cond_signal(&cond_tostadora);
          pthread_mutex_unlock(&candado);
          sleep(1);
     }

     pthread_exit(nullptr);
}

int main()
{
     pthread_t hilos_tostadoras[2];
     pthread_t hilo_empacadora;
     int ids[2] = {1, 2};

     pthread_mutex_init(&candado, NULL);
     pthread_cond_init(&cond_tostadora, NULL);
     pthread_cond_init(&cond_empacadora, NULL);

     for (int i = 0; i < 2; i++)
     {
          pthread_create(&hilos_tostadoras[i], nullptr, tostadora, &ids[i]);
     }
     pthread_create(&hilo_empacadora, nullptr, empacadora, nullptr);

     for (int i = 0; i < 2; i++)
     {
          pthread_join(hilos_tostadoras[i], nullptr);
     }
     pthread_join(hilo_empacadora, nullptr);

     pthread_mutex_destroy(&candado);
     pthread_cond_destroy(&cond_tostadora);
     pthread_cond_destroy(&cond_empacadora);

     cout << "Lbs de cafe en silo: " << cafe_en_silo << endl;
     cout << "Lbs de cafe en bodega: " << bolsas_en_bodega << endl;

     return 0;
}
