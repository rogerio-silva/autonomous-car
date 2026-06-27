/**
 * commands.h — Interface de comando serial (CLI estilo WASD)
 *
 * Lê teclas do Monitor Serial e as traduz em comandos de locomoção e
 * calibração. Não bloqueia: poll() processa o que houver no buffer.
 *
 * Implementação: src/commands.cpp
 */
#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>

namespace commands {

// Imprime o cabeçalho/ajuda inicial. Chamar no setup() (após Serial.begin).
void begin();

// Processa as teclas disponíveis e emite telemetria periódica. Chamar no loop().
void poll();

// Imprime a tabela de comandos.
void printHelp();

// Imprime o status atual (velocidade, PWM L/R, trims).
void printStatus();

} // namespace commands

#endif // COMMANDS_H
