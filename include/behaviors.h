/**
 * behaviors.h — Comportamentos autônomos (M3)
 *
 * Modos de operação não-bloqueantes que usam `sensors` para comandar
 * `motors`. O MANUAL é tratado por `commands`; os demais por este módulo.
 * Implementação: src/behaviors.cpp
 */
#ifndef BEHAVIORS_H
#define BEHAVIORS_H

#include <Arduino.h>

namespace behaviors {

enum Mode {
    MANUAL,       // pilotado pela serial (commands)
    FOLLOW_LINE,  // segue a pista pelo array IR (controle proporcional)
    AVOID         // anda evitando obstáculos (manobra de desvio)
};

void begin();

// Define o modo atual (para os motores e reinicia a sub-máquina do AVOID).
void setMode(Mode m);
Mode mode();

// Nome legível do modo atual (para telemetria).
const __FlashStringHelper* modeName();

// Executa o comportamento ativo. Chamar a cada loop() quando mode != MANUAL.
void update();

} // namespace behaviors

#endif // BEHAVIORS_H
