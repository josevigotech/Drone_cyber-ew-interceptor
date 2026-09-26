#include <iostream>
#include <string>
#include <vector>
#include <memory> // Requerido para std::unique_ptr y std::make_unique

// 1. CLASE BASE
class Amenaza {
private:
    std::string id_identificador;
    int velocidad_kmh;

public:
    Amenaza(std::string id, int velocidad) 
        : id_identificador(id), velocidad_kmh(velocidad) {}

    virtual ~Amenaza() {} // Destructor virtual esencial para liberación polimórfica

    std::string getId() const { return id_identificador; }
    int getVelocidad() const { return velocidad_kmh; }

    virtual void rastrear() const {
        std::cout << "[SISTEMA GENERAL] ID: " << id_identificador << "\n";
    }
};

// 2. MISIL
class Misil : public Amenaza {
private:
    int altitud_metros;

public:
    Misil(std::string id, int velocidad, int altitud)
        : Amenaza(id, velocidad), altitud_metros(altitud) {}

    void rastrear() const override {
        std::cout << "[ALERTA RADARES] MISIL BALÍSTICO ID: " << getId()
                  << " | Altitud: " << altitud_metros << " m.\n";
    }
};

// 3. DRON ESPACIAL
class DronEspacial : public Amenaza {
private:
    std::string tipo_frecuencia;
    std::string bando;
    bool hackeado;
    bool es_hacker;

public:
    DronEspacial(std::string id, int velocidad, std::string frecuencia, std::string bando_inicial, bool hacker = false)
        : Amenaza(id, velocidad), tipo_frecuencia(frecuencia), 
          bando(bando_inicial), hackeado(false), es_hacker(hacker) {}

    // Acepta cualquier Amenaza* y valida dinámicamente si es hackeable
    void hackearAmenaza(Amenaza* objetivo) {
        if (!this->es_hacker) {
            std::cout << "[ERROR] El dron " << getId() << " no tiene módulos de hackeo.\n";
            return;
        }

        // Intenta convertir el puntero genérico a DronEspacial*
        DronEspacial* objetivoDron = dynamic_cast<DronEspacial*>(objetivo);

        if (objetivoDron != nullptr) {
            std::cout << "\n[DRON HACKER " << getId() << "] -> Interceptando frecuencia " 
                      << objetivoDron->tipo_frecuencia << " GHz de " << objetivoDron->getId() << "...\n";
            objetivoDron->recibirHackeo(this->bando);
        } else {
            std::cout << "\n[FALLO DE HACKEO] " << getId() << " intentó hackear " << objetivo->getId() 
                      << " pero NO es un objetivo cibernético (es un sistema balístico/cinético).\n\n";
        }
    }

    void recibirHackeo(std::string nuevo_bando) {
        if (!hackeado) {
            hackeado = true;
            bando = nuevo_bando;
            std::cout << ">>> ¡ÉXITO! Dron " << getId() << " reprogramado a bando: " << bando << " <<<\n\n";
        }
    }

    void rastrear() const override {
        std::cout << "[DRON " << (es_hacker ? "HACKER" : "ESTÁNDAR") << "] ID: " << getId()
                  << " | Bando: " << bando 
                  << " | Estado: " << (hackeado ? "REPROGRAMADO" : "OPERATIVO") << "\n";
    }
};

// 4. MAIN CON GESTIÓN DE MEMORIA AUTOMÁTICA
int main() {
    // Uso de std::unique_ptr para evitar leaks y eliminar la necesidad de 'delete'
    std::vector<std::unique_ptr<Amenaza>> listaAmenazas;

    listaAmenazas.push_back(std::make_unique<DronEspacial>("CYBER-01", 300, "5.8", "ALIADO", true));
    listaAmenazas.push_back(std::make_unique<DronEspacial>("DRON-BETA", 250, "5.8", "ENEMIGO", false));
    listaAmenazas.push_back(std::make_unique<Misil>("MISIL-ALPHA", 5000, 25000));

    std::cout << "--- ESTADO INICIAL EN RADAR ---\n";
    for (const auto& a : listaAmenazas) {
        a->rastrear();
    }

    // Obtenemos punteros raw no propietarios (.get()) solo para pasar referencias
    DronEspacial* dronHacker = dynamic_cast<DronEspacial*>(listaAmenazas[0].get());

    if (dronHacker) {
        // Prueba 1: Intentar hackear el misil (falla de forma segura)
        dronHacker->hackearAmenaza(listaAmenazas[2].get());

        // Prueba 2: Intentar hackear el dron enemigo (éxito)
        dronHacker->hackearAmenaza(listaAmenazas[1].get());
    }

    std::cout << "--- ESTADO FINAL EN RADAR ---\n";
    for (const auto& a : listaAmenazas) {
        a->rastrear();
    }

    // La memoria se libera automáticamente al salir del scope de listaAmenazas
    return 0;
}