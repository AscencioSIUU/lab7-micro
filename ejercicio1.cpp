#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>

using namespace std;

const int num_productos = 8;

struct Producto
{
     string nombre;
     double precio_unitario;
     double costo_fijo;
     int unidades_vendidas;
     double ventas;
     double utilidad;
};

Producto productos[num_productos];
double total_ventas = 0.0;
double total_utilidad = 0.0;

mutex mtx;

void *calcular(void *arg)
{
     int id = *(int *)arg;

     cout << "Inicio de cálculo del hilo No. " << id + 1 << endl;
     sleep(1); // Simula trabajo en paralelo

     productos[id].ventas = productos[id].unidades_vendidas * productos[id].precio_unitario;
     productos[id].utilidad = productos[id].ventas - (productos[id].unidades_vendidas * productos[id].costo_fijo);

     lock_guard<mutex> lock(mtx);
     total_ventas += productos[id].ventas;
     total_utilidad += productos[id].utilidad;

     cout << "Fin de cálculo del hilo No. " << id + 1 << endl;
     pthread_exit(nullptr);
}

int main()
{
     // Inicializar productos (esto debe extraerse del archivo Excel)
     productos[0] = {"Porción pastel de chocolate", 60, 20, 300};
     productos[1] = {"White mocha", 32, 19.2, 400};
     productos[2] = {"Café americano 8onz", 22, 13.2, 1590};
     productos[3] = {"Latte 8onz", 24, 17.2, 200};
     productos[4] = {"Toffee coffee", 28, 20.1, 390};
     productos[5] = {"Cappuccino 8onz", 24, 17.2, 1455};
     productos[6] = {"S'mores Latte", 32, 23, 800};
     productos[7] = {"Café tostado molido", 60, 20, 60};

     pthread_t hilos[num_productos];
     int ids[num_productos];

     // Crear hilos
     for (int i = 0; i < num_productos; ++i)
     {
          ids[i] = i;
          pthread_create(&hilos[i], nullptr, calcular, &ids[i]);
     }

     // Esperar a que todos los hilos terminen
     for (int i = 0; i < num_productos; ++i)
     {
          pthread_join(hilos[i], nullptr);
     }

     // Impresion de Reportes.
     cout << "REPORTE DEL MES DE JULIO" << endl;
     cout << "--- Monto Ventas por Producto ---" << endl;
     for (int i = 0; i < num_productos; ++i)
     {
          cout << productos[i].nombre << ": " << productos[i].ventas << endl;
     }
     cout << "------------------------------------" << endl;
     cout << "--- Utilidad por Producto ---" << endl;
     for (int i = 0; i < num_productos; ++i)
     {
          cout << productos[i].nombre << ": " << productos[i].utilidad << endl;
     }
     cout << "------------------------------------" << endl;

     cout << "Total ventas del mes: " << total_ventas << endl;
     cout << "Total utilidad del mes: " << total_utilidad << endl;

     return 0;
}