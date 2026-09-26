#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <memory>
#include <cstdlib>

// Estructura para posición 2D
struct Posicion {
    double x;
    double y;
};

// 1. SISTEMA DE GUERRA ELECTRÓNICA (JAMMER)
class JammerEW {
private:
    Posicion ubicacion;
    double radio_cobertura_km;
    double frecuencia_bloqueada_ghz; // p. ej., 1.575 GHz (GPS)

public:
    JammerEW(Posicion pos, double radio, double freq)
        : ubicacion(pos), radio_cobertura_km(radio), frecuencia_bloqueada_ghz(freq) {}

    // Comprueba si un objetivo está dentro del campo de interferencia
    bool estaEnRangoInterferencia(Posicion posObjetivo) const {
        double dx = posObjetivo.x - ubicacion.x;
        double dy = posObjetivo.y - ubicacion.y;
        double distancia = std::sqrt(dx * dx + dy * dy);
        return distancia <= radio_cobertura_km;
    }

    Posicion getPosicion() const { return ubicacion; }
};

// 2. CLASE CLAVE: DRON CON NAVEGACIÓN Y SENSIBILIDAD A EW
class DronAutonomo {
private:
    std::string id;
    Posicion posicion_actual;
    Posicion destino_base;
    double calidad_gps; // 1.0 = Señal perfecta, 0.0 = Bloqueado (Jammed)
    double error_inercial_acumulado; // Deriva acumulada por falta de GPS
    bool destruido;
    bool fuera_de_control;

public:
    DronAutonomo(std::string id_dron, Posicion inicio, Posicion destino)
        : id(id_dron), posicion_actual(inicio), destino_base(destino),
          calidad_gps(1.0), error_inercial_acumulado(0.0), 
          destruido(false), fuera_de_control(false) {}

    // Simula 1 segundo de vuelo bajo condiciones de EW
    void actualizarVuelo(const std::vector<JammerEW>& red_jammers) {
        if (destruido) return;

        // A. Verificar si entra en zona de interferencia (Jamming)
        bool bajo_ataque_ew = false;
        for (const auto& jammer : red_jammers) {
            if (jammer.estaEnRangoInterferencia(posicion_actual)) {
                bajo_ataque_ew = true;
                break;
            }
        }

        // B. Lógica de Navegación (GPS vs INS)
        if (bajo_ataque_ew) {
            calidad_gps = 0.0; // ¡Bloqueo de señal electromagnética!
            error_inercial_acumulado += 1.5; // El error de acelerómetros/giroscopios crece
            
            std::cout << "[ALERTA DRON " << id << "] ¡JAMMING DETECTADO! Perdiendo señal GPS. Nivel de deriva inercial: " 
                      << error_inercial_acumulado << "m\n";
        } else {
            calidad_gps = 1.0;
            // Si recupera GPS, el error inercial se corrige
            if (error_inercial_acumulado > 0) error_inercial_acumulado = 0.0;
        }

        // C. Evaluar estado de orientación
        if (error_inercial_acumulado > 5.0) {
            fuera_de_control = true;
            // Desviación errática simulada por pérdida de orientación
            posicion_actual.x += (rand() % 10 - 5); 
            posicion_actual.y += (rand() % 10 - 5);
            std::cout << ">>> [PERDIDA DE ORIENTACIÓN] Dron " << id << " se desvía erráticamente y cae en zona deshabitada.\n";
        } else {
            // Avanzar hacia el objetivo
            posicion_actual.x += 1.0; 
            posicion_actual.y += 1.0;
        }
    }

    void interceptarPorCRAM() {
        destruido = true;
        std::cout << "💥 [DEFENSA C-RAM] Dron " << id << " interceptado y destruido por fuego de artillería cinética a 20mm.\n";
    }

    Posicion getPosicion() const { return posicion_actual; }
    bool estaFueraDeControl() const { return fuera_de_control; }
    bool estaDestruido() const { return destruido; }
    std::string getId() const { return id; }
};

// 3. MAIN: SIMULACIÓN DE ESCENARIO TÁCTICO REAL
int main() {
    // Definir infraestructura de defensa
    Posicion base_aliada = {10.0, 10.0};
    
    // Crear un Jammer de Guerra Electrónica cubriendo la zona previa a la base
    std::vector<JammerEW> defensas_ew;
    defensas_ew.push_back(JammerEW({5.0, 5.0}, 4.0, 1.575)); // Cobertura de 4km a 1.575 GHz

    // Dron atacante dirigiéndose a la base
    DronAutonomo dronEnemigo("SHAHED-101", {0.0, 0.0}, base_aliada);

    std::cout << "--- INICIANDO SIMULACIÓN DE DEFENSA EW DE CAPAS ---\n\n";

    // Simular el avance del dron en varios pasos de tiempo
    for (int segundo = 1; segundo <= 6; ++segundo) {
        std::cout << "=== T + " << segundo << "s ===\n";
        
        dronEnemigo.actualizarVuelo(defensas_ew);

        // Si el dron pierde orientación por la EW, deja de ser amenaza
        if (dronEnemigo.estaFueraDeControl()) {
            std::cout << "-> Amenaza neutralizada electrónicamente por degradación inercial.\n";
            break;
        }

        // Si la EW no es suficiente y el dron sigue acercándose, entra el sistema cinético (C-RAM)
        if (segundo == 5 && !dronEnemigo.estaFueraDeControl()) {
            dronEnemigo.interceptarPorCRAM();
            break;
        }
        
        std::cout << "\n";
    }

    return 0;
}