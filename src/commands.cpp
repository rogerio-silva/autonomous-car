/**
 * commands.cpp — CLI serial estilo WASD
 * Ver include/commands.h, docs/serial-control.md
 *
 * Mapa de teclas (resumo — ver printHelp):
 *   w/s/a/d  mover · espaço ou x  parar
 *   + / -    velocidade (setpoint)
 *   1/2 3/4  trim esquerdo / direito
 *   k        salvar trim na EEPROM · n  resetar trim
 *   o        status · t  telemetria contínua · h ou ?  ajuda
 */
#include "commands.h"
#include "config.h"
#include "motors.h"

namespace {

// Setpoint de velocidade ajustável pelo usuário (PWM).
const uint8_t  SPEED_MIN  = 60;   // abaixo disso o motor tende a travar
const uint8_t  SPEED_STEP = 15;
uint8_t        g_speed    = MOTOR_SPEED_CRUISE;

// Telemetria contínua.
bool           g_telemetry   = false;
uint32_t       g_lastTelemMs = 0;
const uint32_t TELEM_PERIOD_MS = 500;

uint8_t addSpeed(uint8_t s, int16_t delta) {
    int16_t v = (int16_t)s + delta;
    if (v < SPEED_MIN)         v = SPEED_MIN;
    if (v > MOTOR_SPEED_MAX)   v = MOTOR_SPEED_MAX;
    return (uint8_t)v;
}

void handleKey(char c) {
    switch (c) {
        // --- Movimento ---
        case 'w': motors::forward(g_speed);  break;
        case 's': motors::backward(g_speed); break;
        case 'a': motors::turnLeft(g_speed); break;
        case 'd': motors::turnRight(g_speed);break;
        case 'x':
        case ' ': motors::stop();            break;

        // --- Velocidade ---
        case '+':
        case '=': g_speed = addSpeed(g_speed,  SPEED_STEP);
                  Serial.print(F("velocidade=")); Serial.println(g_speed); break;
        case '-':
        case '_': g_speed = addSpeed(g_speed, -SPEED_STEP);
                  Serial.print(F("velocidade=")); Serial.println(g_speed); break;

        // --- Trim (calibração) ---
        case '1': motors::adjustTrimLeft(-TRIM_STEP);  commands::printStatus(); break;
        case '2': motors::adjustTrimLeft( TRIM_STEP);  commands::printStatus(); break;
        case '3': motors::adjustTrimRight(-TRIM_STEP); commands::printStatus(); break;
        case '4': motors::adjustTrimRight( TRIM_STEP); commands::printStatus(); break;
        case 'k': motors::saveTrim();  Serial.println(F("[OK] trim salvo na EEPROM")); break;
        case 'n': motors::resetTrim(); Serial.println(F("[OK] trim resetado")); commands::printStatus(); break;

        // --- Info ---
        case 'o': commands::printStatus(); break;
        case 't': g_telemetry = !g_telemetry;
                  Serial.print(F("telemetria=")); Serial.println(g_telemetry ? F("ON") : F("OFF")); break;
        case 'h':
        case '?': commands::printHelp(); break;

        // Ignora CR/LF e demais.
        case '\r':
        case '\n': break;
        default: break;
    }
}

} // namespace

namespace commands {

void begin() {
    printHelp();
    printStatus();
}

void poll() {
    while (Serial.available() > 0) {
        handleKey((char)Serial.read());
    }

    if (g_telemetry && (millis() - g_lastTelemMs >= TELEM_PERIOD_MS)) {
        g_lastTelemMs = millis();
        printStatus();
    }
}

void printHelp() {
    Serial.println(F("\n===== autonomous-car :: controle serial (M2) ====="));
    Serial.println(F("Movimento : w=frente  s=re  a=girar-esq  d=girar-dir  (espaco|x)=parar"));
    Serial.println(F("Velocidade: + aumenta   - diminui"));
    Serial.println(F("Trim esq  : 1 (-)  2 (+)      Trim dir : 3 (-)  4 (+)"));
    Serial.println(F("Calibracao: k=salvar(EEPROM)  n=resetar"));
    Serial.println(F("Info      : o=status  t=telemetria on/off  h|?=ajuda"));
    Serial.println(F("Failsafe  : para sozinho se nenhum comando chegar a tempo"));
    Serial.println(F("=================================================\n"));
}

void printStatus() {
    Serial.print(F("[status] vel="));   Serial.print(g_speed);
    Serial.print(F(" pwmL="));           Serial.print(motors::currentLeft());
    Serial.print(F(" pwmR="));           Serial.print(motors::currentRight());
    Serial.print(F(" trimL="));          Serial.print(motors::trimLeft(), 2);
    Serial.print(F(" trimR="));          Serial.print(motors::trimRight(), 2);
    Serial.print(F(" mov="));            Serial.println(motors::isMoving() ? F("sim") : F("nao"));
}

} // namespace commands
