#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <random> // C++ moderno para aleatoriedad

struct Posicion {
    double x;
    double y;
};

// Función útil para calcular distancias geométricas
double calcularDistancia(Posicion p1, Posicion p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

class JammerEW {
private:
    Posicion ubicacion;
    double radio_cobertura_km;
    double frecuencia_bloqueada_ghz;

public:
    JammerEW(Posicion pos, double radio, double freq)
        : ubicacion(pos), radio_cobertura_km(radio), frecuencia_bloqueada_ghz(freq) {}

    bool estaEnRangoInterferencia(Posicion posObjetivo) const {
        return calcularDistancia(posObjetivo, ubicacion) <= radio_cobertura_km;
    }
};

class DronAutonomo {
private:
    std::string id;
    Posicion posicion_actual;
    Posicion destino_base;
    double calidad_gps; 
    double error_inercial_acumulado; 
    bool destruido;
    bool fuera_de_control;

    // Generador de números aleatorios moderno
    std::mt19937 generador{std::random_device{}()};

public:
    DronAutonomo(std::string id_dron, Posicion inicio, Posicion destino)
        : id(id_dron), posicion_actual(inicio), destino_base(destino),
          calidad_gps(1.0), error_inercial_acumulado(0.0), 
          destruido(false), fuera_de_control(false) {}

    void actualizarVuelo(const std::vector<JammerEW>& red_jammers) {
        if (destruido) return;

        // Si ya perdió el control total, su vuelo es errático permanente hasta caer
        if (fuera_de_control) {
            std::uniform_real_distribution<double> desvio(-3.0, 3.0);
            posicion_actual.x += desvio(generador);
            posicion_actual.y += desvio(generador);
            std::cout << ">>> [CAÍDA] Dron " << id << " vuela sin rumbo en coordenadas (X: " 
                      << posicion_actual.x << ", Y: " << posicion_actual.y << ")\n";
            return;
        }

        // A. Verificar Jamming
        bool bajo_ataque_ew = false;
        for (const auto& jammer : red_jammers) {
            if (jammer.estaEnRangoInterferencia(posicion_actual)) {
                bajo_ataque_ew = true;
                break;
            }
        }

        // B. Lógica de Navegación
        if (bajo_ataque_ew) {
            calidad_gps = 0.0;
            error_inercial_acumulado += 1.5; 
            std::cout << "[ALERTA DRON " << id << "] ¡JAMMING! Señal GPS perdida. Deriva inercial: " 
                      << error_inercial_acumulado << "m\n";
        } else {
            calidad_gps = 1.0;
            if (error_inercial_acumulado > 0) error_inercial_acumulado = 0.0;
        }

        // C. Evaluar estado de orientación e incremento de posición
        if (error_inercial_acumulado > 5.0) {
            fuera_de_control = true;
            std::cout << ">>> [PÉRDIDA DE CONTROL] SISTEMAS INS SATURADOS. El dron " << id << " pierde el rumbo comercial.\n";
        } else {
            // Avanzar progresivamente hacia el destino real (Base Aliada)
            if (posicion_actual.x < destino_base.x) posicion_actual.x += 1.5;
            if (posicion_actual.y < destino_base.y) posicion_actual.y += 1.5;
        }
    }

    void interceptarPorCRAM() {
        destruido = true;
        std::cout << "[DEFENSA C-RAM] Dron " << id << " interceptado por fuego cinético a 20mm. ¡AMENAZA ELIMINADA!\n";
    }

    Posicion getPosicion() const { return posicion_actual; }
    bool estaFueraDeControl() const { return fuera_de_control; }
    bool estaDestruido() const { return destruido; }
    std::string getId() const { return id; }
};

int main() {
    Posicion base_aliada = {10.0, 10.0};
    
    std::vector<JammerEW> defensas_ew;
    // Jammer estratégico en camino
    defensas_ew.push_back(JammerEW({4.0, 4.0}, 3.5, 1.575)); 

    DronAutonomo dronEnemigo("SHAHED-101", {0.0, 0.0}, base_aliada);

    std::cout << "--- INICIANDO SIMULACIÓN DE DEFENSA EN CAPAS AUTOMATIZADA ---\n\n";

    for (int segundo = 1; segundo <= 10; ++segundo) {
        std::cout << "=== T + " << segundo << "s ===\n";
        
        dronEnemigo.actualizarVuelo(defensas_ew);

        if (dronEnemigo.estaFueraDeControl()) {
            std::cout << "-> Éxito: Amenaza neutralizada electrónicamente por la EW.\n";
            // Dejamos que corra un segundo más para ver el mensaje de caída y cerramos
            dronEnemigo.actualizarVuelo(defensas_ew);
            break;
        }

        // Medir distancia a la base para la defensa de última capa (C-RAM)
        double distancia_a_base = calcularDistancia(dronEnemigo.getPosicion(), base_aliada);
        std::cout << "Distancia restante a la Base: " << distancia_a_base << " km\n";

        if (distancia_a_base <= 2.5 && !dronEnemigo.estaDestruido()) {
            dronEnemigo.interceptarPorCRAM();
            break;
        }
        
        std::cout << "\n";
    }

    return 0;
}
