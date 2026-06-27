/**
 * main.cpp — Ponto de entrada do firmware do carro autônomo
 *
 * Plataforma: Arduino Mega 2560
 *
 * M4 — Visão: além do MANUAL (CLI serial) e dos modos por sensores
 * (SEGUIR-LINHA, DESVIO), há o modo RASTREIO, que segue um alvo visual da
 * HuskyLens (Tag) via I²C, aproximando-se e mantendo distância.
 *
 * Loop de controle (não-bloqueante, sem delay()):
 *   commands::poll()    -> teclas: pilotagem manual, modos e calibração
 *   behaviors::update() -> executa o comportamento autônomo ativo (se != MANUAL)
 *   motors::update()    -> avança rampas, aplica PWM (com trim) e o failsafe
 */
#include <Arduino.h>
#include "config.h"
#include "storage.h"
#include "motors.h"
#include "sensors.h"
#include "vision.h"
#include "behaviors.h"
#include "commands.h"

void setup() {
    Serial.begin(SERIAL_BAUD);
    storage::begin();          // carrega trim e limiares IR (antes de motors/sensors)
    motors::begin();
    sensors::begin();
    behaviors::begin();

    if (!vision::begin()) {
        Serial.println(F("[AVISO] HuskyLens nao encontrada na I2C (modo RASTREIO indisponivel)."));
    }
    commands::begin();

    Serial.print(F("[OK] autonomous-car pronto (M4). Config EEPROM: "));
    Serial.println(storage::wasLoaded() ? F("carregada") : F("padrao"));
}

void loop() {
    commands::poll();

    if (behaviors::mode() != behaviors::MANUAL) {
        behaviors::update();
    }

    motors::update();
}
