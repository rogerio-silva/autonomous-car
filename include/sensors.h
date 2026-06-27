/**
 * sensors.h — Leitura de sensores (distância e infravermelho)
 *
 * Distância com filtragem (mediana) e IR de linha com limiares calibráveis
 * (persistidos via storage). Implementação: src/sensors.cpp
 */
#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

namespace sensors {

// Estado do array seguidor de linha (true = sobre a linha).
struct LineState {
    bool left;
    bool center;
    bool right;
};

void begin();

// Distância frontal em cm, filtrada por mediana (0 = fora de alcance).
uint16_t distanceCm();

// True se há obstáculo dentro do limiar OBSTACLE_STOP_CM (ultrassom ou IR).
bool obstacleAhead();

// Leitura do array IR, já aplicando limiares e polaridade calibrados.
LineState readLine();

// Sensor IR digital frontal de obstáculo (LOW = obstáculo detectado).
bool irObstacle();

// --- Calibração dos IR de linha (runtime + EEPROM via storage) ---
// Leituras analógicas cruas dos 3 sensores (E, C, D).
void readLineRaw(int16_t out[3]);

// Captura a leitura atual como "fundo" (superfície sem linha).
void sampleBackground();

// Captura a leitura atual como "linha" e recalcula limiares + polaridade
// (precisa de uma amostra de fundo prévia). Retorna false se faltar o fundo.
bool sampleLine();

} // namespace sensors

#endif // SENSORS_H
