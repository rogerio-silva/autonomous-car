/**
 * sensors.h — Leitura de sensores (distância e infravermelho)
 * Implementação: src/sensors.cpp
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

// Distância frontal em cm (0 = sem eco / fora de alcance).
uint16_t distanceCm();

// True se há obstáculo dentro do limiar OBSTACLE_STOP_CM.
bool obstacleAhead();

// Leitura do array IR seguidor de linha.
LineState readLine();

// Sensor IR digital frontal de obstáculo (LOW = obstáculo detectado).
bool irObstacle();

} // namespace sensors

#endif // SENSORS_H
