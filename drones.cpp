#include <iostream>
#include <string>
#include <vector>

// ============================================================================
// 1. CLASE BASE (El molde general para cualquier amenaza)
// ============================================================================
class Amenaza {
private:
    // ENCAPSULAMIENTO: Estos atributos son privados. Ningún código externo o
    // función fuera de esta clase puede modificarlos directamente.
    std::string id_identificador;
    int velocidad_kmh;

public:
    // CONSTRUCTOR: Inicializa los datos protegidos al crear la instancia.
    Amenaza(std::string id, int velocidad) 
        : id_identificador(id), velocidad_kmh(velocidad) {}

    // DESTRUCTOR VIRTUAL: Buena práctica esencial en POO con herencia.
    // Libera memoria correctamente al destruir objetos derivados mediante punteros.
    virtual ~Amenaza() {}

    // ABSTRACCIÓN: Métodos 'getter' y 'setter' que permiten leer/modificar de
    // forma segura los atributos privados sin exponer la memoria interna.
    std::string getId() const { return id_identificador; }
    int getVelocidad() const { return velocidad_kmh; }

    // ABSTRACCIÓN Y POLIMORFISMO: 'virtual' indica a C++ que las clases hijas
    // pueden redefinir esta función con su propia lógica específica.
    virtual void rastrear() const {
        std::cout << "[SISTEMA GENERAL] Rastreando objeto no identificado ID: " 
                  << id_identificador << " a " << velocidad_kmh << " km/h.\n";
    }
};

// ============================================================================
// 2. HERENCIA (Clase Derivada 1: Misil)
// ============================================================================
// 'Misil' hereda todos los métodos y atributos públicos de 'Amenaza'
class Misil : public Amenaza {
private:
    int altitud_metros; // Atributo propio y exclusivo de los misiles

public:
    // El constructor de Misil llama al constructor de la clase padre (Amenaza)
    Misil(std::string id, int velocidad, int altitud)
        : Amenaza(id, velocidad), altitud_metros(altitud) {}

    // POLIMORFISMO: Sobrescribimos el método rastrear() usando 'override'.
    // Esto reacciona de forma única cuando el objeto es un Misil.
    void rastrear() const override {
        std::cout << "[ALERTA RADARES] Rastreando MISIL BALÍSTICO ID: " << getId()
                  << " | Velocidad: " << getVelocidad() << " km/h"
                  << " | Altitud: " << altitud_metros << " m.\n";
    }
};

// ============================================================================
// 3. HERENCIA (Clase Derivada 2: Dron)
// ============================================================================
class Dron : public Amenaza {
private:
    std::string tipo_frecuencia; // Atributo exclusivo para drones

public:
    Dron(std::string id, int velocidad, std::string frecuencia)
        : Amenaza(id, velocidad), tipo_frecuencia(frecuencia) {}

    // POLIMORFISMO: Lógica específica para rastrear señales de un Dron.
    void rastrear() const override {
        std::cout << "[RECONOCIMIENTO] Rastreando DRON ENEMIGO ID: " << getId()
                  << " | Frecuencia de control: " << tipo_frecuencia << " GHz.\n";
    }
};

// ============================================================================
// 4. USO PRÁCTICO EN CÓDIGO (Instanciación y Polimorfismo en Tiempo de Ejecución)
// ============================================================================
int main() {
    std::cout << "=== INICIANDO SISTEMA DE CONTROL DE INTEGRACIÓN ===\n\n";

    // INSTANCIACIÓN DIRECTA (Objetos en la pila / Stack):
    Misil misil1("TH-901", 3500, 12000);
    Dron dron1("DR-004", 250, "5.8");

    // Llama al método polimórfico de cada uno:
    misil1.rastrear();
    dron1.rastrear();

    std::cout << "\n--- DEMOSTRACIÓN DE POLIMORFISMO EN VECTOR --- \n";

    // APLICACIÓN REAL EN INTEGRACIÓN DE SOFTWARE:
    // Un solo contenedor maneja diferentes clases hijas mediante la clase base (Amenaza).
    std::vector<Amenaza*> listaAmenazas;

    listaAmenazas.push_back(new Misil("MISIL-ALPHA", 5000, 25000));
    listaAmenazas.push_back(new Dron("DRON-BETA", 180, "2.4"));
    listaAmenazas.push_back(new Misil("MISIL-GAMMA", 4200, 18000));

    // Bucle unificado: El sistema llama a .rastrear() en cada iteración
    // sin necesitar saber si el elemento actual es un misil o un dron.
    for (const Amenaza* elemento : listaAmenazas) {
        elemento->rastrear(); // C++ decide dinámicamente cuál método ejecutar
    }

    // LIMPIEZA DE MEMORIA (Punteros):
    for (Amenaza* elemento : listaAmenazas) {
        delete elemento;
    }

    return 0;
}