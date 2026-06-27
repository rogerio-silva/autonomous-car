/**
 * main.cpp — Ponto de entrada do firmware do carro autônomo
 *
 * Plataforma: Arduino Mega 2560
 *
 * M2 — Plataforma & Locomoção: modo MANUAL não-bloqueante. O carro é
 * pilotado pelo Monitor Serial (CLI estilo WASD), com rampas de aceleração,
 * trim persistido (EEPROM) e failsafe. A autonomia (fusão com sensores e
 * visão) é introduzida em M3–M5.
 *
 * Loop de controle (não-bloqueante, sem delay()):
 *   commands::poll()  -> lê teclas e atualiza alvos/telemetria
 *   motors::update()  -> avança rampas, aplica PWM e o failsafe
 */
#include <Arduino.h>
#include "config.h"
#include "motors.h"
#include "commands.h"

void setup() {
    Serial.begin(SERIAL_BAUD);
    motors::begin();
    commands::begin();
    Serial.println(F("[OK] autonomous-car pronto (modo manual M2)."));
}

void loop() {
    commands::poll();
    motors::update();
}
