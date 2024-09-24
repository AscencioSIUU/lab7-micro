#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>

using namespace std;

const int CAPACIDAD_SILO = 5;
const int TOTAL_BOLSAS = 100;
int cafe_en_silo = 0;
int bolsas_en_bodega = 0;

pthread_mutex_t candado;
pthread_cond_t cond_tostadora;
pthread_cond_t cond_empacadora;

int main()
{
     pthread_t
         retunr 0;
}
