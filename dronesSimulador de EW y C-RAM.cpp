#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <random>

struct Posicion {
    double x;
    double y;
};

// Función auxiliar para calcular distancias geométricas
double calcularDistancia(Posicion p1, Posicion p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

// 1. SISTEMA DE DETECCIÓN 
class RadarAlertaTemprana {
private:
    Posicion ubicacion;
    double rango_deteccion_km;
    std::string modelo_radar;

public:
    RadarAlertaTemprana(std::string modelo, Posicion pos, double rango)
        : modelo_radar(modelo), ubicacion(pos), rango_deteccion_km(rango) {}

    // Escanea el espacio aéreo en busca de amenazas
    bool detectarObjetivo(Posicion posObjetivo, const std::string& idDron) const {
        double distancia = calcularDistancia(posObjetivo, ubicacion);
        if (distancia <= rango_deteccion_km) {
            std::cout << "[RADAR " << modelo_radar << "] ¡CONTACTO! Detectada amenaza " 
                      << idDron << " a " << distancia << "km de distancia.\n";
            return true;
        }
        return false;
    }
};

// 2. SISTEMA DE GUERRA ELECTRÓNICA con frecuencia.
class JammerEW {
private:
    Posicion ubicacion;
    double radio_cobertura_km;
    double frecuencia_bloqueada_ghz; //  variable  1.575 GHz GPS.

public:
    JammerEW(Posicion pos, double radio, double freq) 
        : ubicacion(pos), radio_cobertura_km(radio), frecuencia_bloqueada_ghz(freq) {}

    bool estaEnRangoInterferencia(Posicion posObjetivo) const {
        return calcularDistancia(posObjetivo, ubicacion) <= radio_cobertura_km;
    }
};

// 3. DRON ENEMY CON CAPACIDAD DE DEGRADACIÓN INERCIAL
//.Newton's First Law
class DronAutonomo {
private:
    std::string id;
    Posicion posicion_actual;
    Posicion destino_base;
    double calidad_gps; 
    double error_inercial_acumulado; 
    bool destruido;
    bool fuera_de_control;
    std::mt19937 generador{std::random_device{}()};

public:
    DronAutonomo(std::string id_dron, Posicion inicio, Posicion destino)
        : id(id_dron), posicion_actual(inicio), destino_base(destino),
          calidad_gps(1.0), error_inercial_acumulado(0.0), 
          destruido(false), fuera_de_control(false) {}

    void actualizarVuelo(const std::vector<JammerEW>& red_jammers) {
        if (destruido) return;

        if (fuera_de_control) {
            std::uniform_real_distribution<double> desvio(-3.0, 3.0);
            posicion_actual.x += desvio(generador);
            posicion_actual.y += desvio(generador);
            std::cout << ">>> [DRIV EN CAÍDA] El dron " << id << " da tumbos ciegos en (X: " 
                      << posicion_actual.x << ", Y: " << posicion_actual.y << ")\n";
            return;
        }

        bool bajo_ataque_ew = false;
        for (const auto& jammer : red_jammers) {
            if (jammer.estaEnRangoInterferencia(posicion_actual)) {
                bajo_ataque_ew = true;
                break;
            }
        }

        if (bajo_ataque_ew) {
            calidad_gps = 0.0;
            error_inercial_acumulado += 2.0; // Incremento de error inercial por segundo
            std::cout << "[ALERTA DRON] Sistemas EW saturando antenas. Deriva inercial: " 
                      << error_inercial_acumulado << "m\n";
        } else {
            calidad_gps = 1.0;
            if (error_inercial_acumulado > 0) error_inercial_acumulado = 0.0;
        }

        if (error_inercial_acumulado > 5.0) {
            fuera_de_control = true;
            std::cout << ">>> [FALLO CRÍTICO] Pérdida total de orientación inercial en el dron.\n";
        } else {
            // Avanza hacia la base aliada
            if (posicion_actual.x < destino_base.x) posicion_actual.x += 1.5;
            if (posicion_actual.y < destino_base.y) posicion_actual.y += 1.5;
        }
    }

    void interceptarPorCRAM() {
        destruido = true;
        std::cout << "[DEFENSA CINÉTICA] ¡Fuego C-RAM activado! Ráfaga de 20mm destruye al dron.\n";
    }

    Posicion getPosicion() const { return posicion_actual; }
    bool estaFueraDeControl() const { return fuera_de_control; }
    bool estaDestruido() const { return destruido; }
    std::string getId() const { return id; }
};

// 4. COORDINADOR CENTRAL DE LA BASE*
class CentroOperacionesTacticas {
private:
    Posicion ubicacion_base;
    RadarAlertaTemprana radar;
    std::vector<JammerEW> red_ew;
    double rango_fuego_cram;

public:
    CentroOperacionesTacticas(Posicion base, RadarAlertaTemprana r, double cram_rango)
        : ubicacion_base(base), radar(r), rango_fuego_cram(cram_rango) {}

    void agregarDefensaEW(JammerEW jammer) {
        red_ew.push_back(jammer);
    }

    // Gestiona el combate automatizado basándose en el Radar
    void procesarAmenaza(DronAutonomo& dron) {
        // Capa 1: El radar busca el dron en el espacio aéreo
        bool detectado = radar.detectarObjetivo(dron.getPosicion(), dron.getId());
        
        if (!detectado) {
            std::cout << "[CENTRO CONTROL] Amenaza fuera de rango de radar. Monitoreando...\n";
            // El dron se mueve pero nadie lo ataca porque no ha sido detectado aún
            dron.actualizarVuelo({}); 
            return;
        }

        // Capa 2: Si está detectado, actualiza su vuelo evaluando si los Jammers lo alcanzan
        dron.actualizarVuelo(red_ew);

        if (dron.estaFueraDeControl() || dron.estaDestruido()) return;

        // Capa 3: Si sobrevive a la EW, el C-RAM actúa de forma cinética
        double distancia_a_base = calcularDistancia(dron.getPosicion(), ubicacion_base);
        std::cout << "[INFO CONTROL] Distancia a la zona de impacto: " << distancia_a_base << "km\n";

        if (distancia_a_base <= rango_fuego_cram) {
            dron.interceptarPorCRAM();
        }
    }
};

// 5. MAIN INTEGRADO Y AUTOMATIZADO
int main() {
    Posicion base_aliada = {10.0, 10.0};
    
    // Instanciar el radar militar
    RadarAlertaTemprana radarAN_FPS("AN/FPS-117", base_aliada, 12.0); // Cobertura de 12km
    
    // Crear el Jammer estratégico: {posición}, radio, frecuencia
    JammerEW inhibidorCamino({5.0, 5.0}, 3.5, 1.575); 
    
    // Configurar el Centro de Mando Operativo
    CentroOperacionesTacticas toc(base_aliada, radarAN_FPS, 2.5); // C-RAM a menos de 2.5km
    toc.agregarDefensaEW(inhibidorCamino);

    // Dron hostil aproximándose desde el origen
    DronAutonomo dronEnemigo("SHAHED-101", {0.0, 0.0}, base_aliada);

    std::cout << "--- SIMULACIÓN TÁCTICA AUTOMATIZADA: RADAR + EW + C-RAM ---\n\n";

    for (int segundo = 1; segundo <= 10; ++segundo) {
        std::cout << "=== TIEMPO: T + " << segundo << "s ===\n";
        
        toc.procesarAmenaza(dronEnemigo);

        if (dronEnemigo.estaFueraDeControl()) {
            std::cout << "[SISTEMA] Amenaza neutralizada por Guerra Electrónica (Soft-Kill).\n";
            dronEnemigo.actualizarVuelo({}); // Muestra un paso extra para ver el bucle de caída errática
            break;
        }
        if (dronEnemigo.estaDestruido()) {
            std::cout << "[SISTEMA] Amenaza destruida por Artillería Cinética (Hard-Kill).\n";
            break;
        }
        
        std::cout << "\n";
    }

    return 0;
}
