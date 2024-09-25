#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <iomanip> // para los decimales en los costos

using namespace std;

const int num_productos = 8;  // 8 hilos para cada mes, 16 en total

struct Producto {
    string nombre;
    double precio_unitario;
    double costo_fijo;
    int unidades_vendidas;
    double ventas;
    double utilidad;
};

Producto productos_julio[num_productos];
Producto productos_agosto[num_productos];

double total_ventas = 0.0;
double total_utilidad = 0.0;

mutex mtx;

struct ThreadData {
    Producto* producto;
    int id;  // Identificador del hilo
};

void* calcular(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    Producto* producto = data->producto;
    int id = data->id;

    // Imprimir mensaje de inicio
    cout << "calculo hilo " << id << " iniciado" << endl;

    // Realizar cálculo de ventas y utilidad
    producto->ventas = producto->unidades_vendidas * producto->precio_unitario;
    producto->utilidad = producto->ventas - (producto->unidades_vendidas * producto->costo_fijo);

    // Bloquear acceso a las variables globales para actualizarlas
    lock_guard<mutex> lock(mtx);
    total_ventas += producto->ventas;
    total_utilidad += producto->utilidad;

    // Imprimir mensaje de fin
    cout << "calculo hilo " << id << " terminado" << endl;

    pthread_exit(nullptr);
}

void calcularVentasYUtilidades(Producto productos[], const string& mes) {
    total_ventas = 0.0; 
    total_utilidad = 0.0;

    pthread_t hilos[num_productos];
    ThreadData datos_hilos[num_productos];

    // Crear hilos
    for (int i = 0; i < num_productos; ++i) {
        datos_hilos[i].producto = &productos[i];
        datos_hilos[i].id = i;
        pthread_create(&hilos[i], nullptr, calcular, &datos_hilos[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < num_productos; ++i) {
        pthread_join(hilos[i], nullptr);
    }

    // Imprimir los reportes.
    cout << "\n------------------------------------------" << endl;
    cout << "REPORTE DEL MES DE " << mes << endl;
    cout << "--- Monto Ventas por Producto ---" << endl;
    for (int i = 0; i < num_productos; ++i) {
        cout << "- " << productos[i].nombre << ": Q" << std::fixed << std::setprecision(2) << productos[i].ventas << endl;
    }
    cout << "--- Utilidad por Producto ---" << endl;
    for (int i = 0; i < num_productos; ++i) {
        cout << productos[i].nombre << ": Q" << std::fixed << std::setprecision(2) << productos[i].utilidad << endl;
    }
    cout << "--- Monto Total Ventas del Mes: Q" << std::fixed << std::setprecision(2) << total_ventas << endl;
    cout << "--- Utilidad del mes: Q" << std::fixed << std::setprecision(2) << total_utilidad << endl;
    cout << "------------------------------------------" << endl;
}

int main() {
    // Inicializar productos para julio
    productos_julio[0] = {"Porción pastel de chocolate", 60, 20, 300};
    productos_julio[1] = {"White mocha", 32, 19.2, 400};
    productos_julio[2] = {"Café americano 8onz", 22, 13.2, 1590};
    productos_julio[3] = {"Latte 8onz", 24, 17.2, 200};
    productos_julio[4] = {"Toffee coffee", 28, 20.1, 390};
    productos_julio[5] = {"Cappuccino 8onz", 24, 17.2, 1455};
    productos_julio[6] = {"S'mores Latte", 32, 23, 800};
    productos_julio[7] = {"Café tostado molido", 60, 20, 60};

    // Inicializar productos para agosto
    productos_agosto[0] = {"Porción pastel de chocolate", 60, 20, 250};
    productos_agosto[1] = {"White mocha", 32, 19.2, 380};
    productos_agosto[2] = {"Café americano 8onz", 22, 13.2, 800};
    productos_agosto[3] = {"Latte 8onz", 24, 17.2, 250};
    productos_agosto[4] = {"Toffee coffee", 28, 20.1, 600};
    productos_agosto[5] = {"Cappuccino 8onz", 24, 17.2, 1200};
    productos_agosto[6] = {"S'mores Latte", 32, 23, 1540};
    productos_agosto[7] = {"Café tostado molido", 60, 20, 15};

    // Calcular ventas para julio con 8 hilos
    calcularVentasYUtilidades(productos_julio, "JULIO");

    // Calcular ventas para agosto con otros 8 hilos
    calcularVentasYUtilidades(productos_agosto, "AGOSTO");

    return 0;
}