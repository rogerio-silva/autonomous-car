/**
 * main.cpp — Ponto de entrada do firmware do carro autônomo
 *
 * Plataforma: Arduino Mega 2560
 * Esqueleto da Fundação (M1): inicializa os módulos e roda uma máquina de
 * estados mínima. A lógica de navegação autônoma evolui em M2–M5.
 *
 * Módulos:
 *   - motors  : locomoção 2WD (L298N)
 *   - sensors : distância (HC-SR04) e infravermelho (linha/obstáculo)
 *   - vision  : percepção via Gravity HuskyLens (I2C)
 */
#include <Arduino.h>
#include "config.h"
#include "motors.h"
#include "sensors.h"
#include "vision.h"

// Estados de alto nível do carro.
enum class State { IDLE, CRUISE, AVOID };
static State state = State::IDLE;

void setup() {
    Serial.begin(SERIAL_BAUD);
    motors::begin();
    sensors::begin();

    if (!vision::begin()) {
        Serial.println(F("[AVISO] HuskyLens nao encontrada na I2C."));
    }
    Serial.println(F("[OK] autonomous-car inicializado."));
    state = State::CRUISE;
}

void loop() {
    vision::update();

    switch (state) {
        case State::IDLE:
            motors::stop();
            break;

        case State::CRUISE:
            // Esqueleto: segue em frente até detectar obstáculo.
            if (sensors::obstacleAhead()) {
                state = State::AVOID;
                break;
            }
            motors::forward(MOTOR_SPEED_CRUISE);
            break;

        case State::AVOID:
            // Esqueleto: para, recua e gira. Estratégia real virá no M5.
            motors::stop();
            delay(150);
            motors::backward(MOTOR_SPEED_CRUISE);
            delay(300);
            motors::turnRight(MOTOR_SPEED_CRUISE);
            delay(300);
            state = State::CRUISE;
            break;
    }
}
