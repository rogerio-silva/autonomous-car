/**
 * main.cpp — Ponto de entrada do firmware do carro autônomo
 *
 * Plataforma: Arduino Mega 2560
 *
 * M3 — Sensoriamento: além do modo MANUAL (CLI serial), o carro tem modos
 * autônomos não-bloqueantes (SEGUIR-LINHA e DESVIO) que usam os sensores
 * para comandar os motores. A visão (HuskyLens) entra no M4.
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
#include "behaviors.h"
#include "commands.h"

void setup() {
    Serial.begin(SERIAL_BAUD);
    storage::begin();          // carrega trim e limiares IR (antes de motors/sensors)
    motors::begin();
    sensors::begin();
    behaviors::begin();
    commands::begin();

    Serial.print(F("[OK] autonomous-car pronto (M3). Config EEPROM: "));
    Serial.println(storage::wasLoaded() ? F("carregada") : F("padrao"));
}

void loop() {
    commands::poll();

    if (behaviors::mode() != behaviors::MANUAL) {
        behaviors::update();
    }

    motors::update();
}
