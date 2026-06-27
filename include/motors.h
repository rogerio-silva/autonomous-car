/**
 * motors.h — Controle de locomoção 2WD (driver L298N)
 *
 * Abstrai os dois motores traseiros em comandos de alto nível.
 * Implementação: src/motors.cpp
 */
#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

namespace motors {

// Inicializa pinos dos motores. Chamar uma vez no setup().
void begin();

// Movimentos básicos. `speed` em 0–255 (PWM).
void forward(uint8_t speed);
void backward(uint8_t speed);
void turnLeft(uint8_t speed);
void turnRight(uint8_t speed);
void stop();

// Controle independente por lado (-255 a 255; negativo = ré).
void drive(int16_t leftSpeed, int16_t rightSpeed);

} // namespace motors

#endif // MOTORS_H
