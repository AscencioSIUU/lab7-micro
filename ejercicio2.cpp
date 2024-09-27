#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

const int CAPACIDAD_SILO = 5;
const int TOTAL_BOLSAS = 400;
int cafe_en_silo = 0;
int bolsas_en_bodega = 0;

pthread_mutex_t candado = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_tostadora = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_empacadora = PTHREAD_COND_INITIALIZER;

bool produccion_terminada = false;

void *tostadora(void *arg)
{
    int id = *(int *)arg;
    while (!produccion_terminada)
    {
        pthread_mutex_lock(&candado);
        while (cafe_en_silo >= CAPACIDAD_SILO && !produccion_terminada)
        {
            pthread_cond_wait(&cond_tostadora, &candado);
        }
        if (!produccion_terminada && bolsas_en_bodega + cafe_en_silo < TOTAL_BOLSAS)
        {
            cafe_en_silo++;
            cout << "Tostadora " << id << " produjo: 1 lb de cafe tostado" << endl;
            pthread_cond_signal(&cond_empacadora);
        }
        pthread_mutex_unlock(&candado);
        sleep(1);  // Simula el tiempo de producción
    }
    return nullptr;
}

void *empacadora(void *arg)
{
    while (bolsas_en_bodega < TOTAL_BOLSAS || cafe_en_silo > 0)
    {
        pthread_mutex_lock(&candado);
        while (cafe_en_silo == 0 && bolsas_en_bodega < TOTAL_BOLSAS)
        {
            pthread_cond_wait(&cond_empacadora, &candado);
        }
        if (cafe_en_silo > 0 && bolsas_en_bodega < TOTAL_BOLSAS)
        {
            cafe_en_silo--;
            bolsas_en_bodega++;
            cout << "Empacadora produjo: 1 bolsa de cafe. Total bolsas en bodega: " << bolsas_en_bodega << endl;
            if (bolsas_en_bodega >= TOTAL_BOLSAS)
            {
                produccion_terminada = true;
            }
            pthread_cond_broadcast(&cond_tostadora);
        }
        pthread_mutex_unlock(&candado);
        sleep(1);  // Simula el tiempo de empaque
    }
    return nullptr;
}

int main()
{
    pthread_t hilos_tostadoras[2];
    pthread_t hilo_empacadora;
    int ids[2] = {1, 2};

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

    cout << "Lbs de cafe en silo: " << cafe_en_silo << endl;
    cout << "Lbs de cafe en bodega: " << bolsas_en_bodega << endl;

    pthread_mutex_destroy(&candado);
    pthread_cond_destroy(&cond_tostadora);
    pthread_cond_destroy(&cond_empacadora);

    return 0;
}