/**
 * motors.h — Controle de locomoção 2WD (driver L298N)
 *
 * Controle NÃO-BLOQUEANTE: comandos definem uma velocidade-alvo por lado e
 * update() aproxima a velocidade atual do alvo em rampa, a cada loop.
 * Inclui trim por motor (calibração) e failsafe por timeout de comando.
 *
 * Implementação: src/motors.cpp
 */
#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

namespace motors {

// Inicializa pinos e carrega o trim persistido (EEPROM). Chamar no setup().
void begin();

// Avança as rampas e aplica o PWM ao hardware. Chamar a cada loop().
// Também aplica o failsafe (para se o último comando expirou).
void update();

// --- Comandos de alto nível (definem alvo; a rampa cuida da transição) ---
// `speed` em 0–255. Cada chamada renova o prazo do failsafe.
void forward(uint8_t speed);
void backward(uint8_t speed);
void turnLeft(uint8_t speed);   // giro no próprio eixo (lados opostos)
void turnRight(uint8_t speed);
void stop();                    // alvo 0 (desacelera em rampa)
void brake();                   // zera imediatamente (sem rampa)

// Alvo independente por lado (-255 a 255; negativo = ré).
void setTarget(int16_t leftSpeed, int16_t rightSpeed);

// --- Calibração (trim) ---
// Fatores no intervalo [TRIM_MIN, 1.0]. Aplicados ao PWM de cada lado.
void  setTrim(float left, float right);
void  adjustTrimLeft(float delta);
void  adjustTrimRight(float delta);
void  resetTrim();
void  saveTrim();               // persiste o trim atual na EEPROM
float trimLeft();
float trimRight();

// --- Introspecção (telemetria) ---
int16_t currentLeft();          // PWM atual aplicado (com sinal)
int16_t currentRight();
bool    isMoving();

} // namespace motors

#endif // MOTORS_H
