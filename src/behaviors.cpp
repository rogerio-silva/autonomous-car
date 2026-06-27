/**
 * behaviors.cpp — Comportamentos autônomos (M3)
 * Ver include/behaviors.h, include/config.h
 */
#include "behaviors.h"
#include "config.h"
#include "motors.h"
#include "sensors.h"

namespace {

behaviors::Mode g_mode = behaviors::MANUAL;

// Sub-máquina não-bloqueante do desvio de obstáculo.
enum AvoidPhase { AVP_CRUISE, AVP_BACK, AVP_TURN };
AvoidPhase g_phase      = AVP_CRUISE;
uint32_t   g_phaseStart = 0;
bool       g_turnRight  = true;   // alterna o lado do giro a cada desvio

// Última direção de erro do seguir-linha (para reencontrar a linha perdida).
int8_t     g_lastError  = 0;

// ---- Seguir-linha: controle proporcional pelo erro ponderado ----
void followLine() {
    // Segurança: se houver obstáculo muito próximo, para (não manobra).
    if (sensors::obstacleAhead()) { motors::stop(); return; }

    sensors::LineState ls = sensors::readLine();
    bool any = ls.left || ls.center || ls.right;

    if (!any) {
        // Linha perdida: gira devagar na direção do último erro para reencontrá-la.
        int16_t dir = (g_lastError >= 0) ? 1 : -1;
        motors::setTarget(dir * FOLLOW_SEARCH_SPEED, -dir * FOLLOW_SEARCH_SPEED);
        return;
    }

    // erro: +1 = linha à direita, -1 = à esquerda, 0 = centralizada.
    int8_t error = (int8_t)((ls.right ? 1 : 0) - (ls.left ? 1 : 0));
    g_lastError = error;

    int16_t left  = FOLLOW_BASE_SPEED + (int16_t)FOLLOW_TURN_GAIN * error;
    int16_t right = FOLLOW_BASE_SPEED - (int16_t)FOLLOW_TURN_GAIN * error;
    motors::setTarget(left, right);
}

// ---- Desvio de obstáculo: cruzeiro -> ré -> giro -> cruzeiro ----
void avoidObstacles() {
    uint32_t now = millis();
    switch (g_phase) {
        case AVP_CRUISE:
            if (sensors::obstacleAhead()) {
                motors::stop();
                g_phase = AVP_BACK; g_phaseStart = now;
            } else {
                motors::forward(MOTOR_SPEED_CRUISE);
            }
            break;

        case AVP_BACK:
            motors::backward(AVOID_SPEED);
            if (now - g_phaseStart >= AVOID_BACK_MS) {
                g_phase = AVP_TURN; g_phaseStart = now;
            }
            break;

        case AVP_TURN:
            if (g_turnRight) motors::turnRight(AVOID_SPEED);
            else             motors::turnLeft(AVOID_SPEED);
            if (now - g_phaseStart >= AVOID_TURN_MS) {
                g_turnRight = !g_turnRight;       // alterna o lado no próximo desvio
                g_phase = AVP_CRUISE; g_phaseStart = now;
            }
            break;
    }
}

} // namespace

namespace behaviors {

void begin() {
    g_mode  = MANUAL;
    g_phase = AVP_CRUISE;
}

void setMode(Mode m) {
    g_mode  = m;
    g_phase = AVP_CRUISE;
    g_lastError = 0;
    motors::stop();
}

Mode mode() { return g_mode; }

const __FlashStringHelper* modeName() {
    switch (g_mode) {
        case FOLLOW_LINE: return F("SEGUIR-LINHA");
        case AVOID:       return F("DESVIO");
        default:          return F("MANUAL");
    }
}

void update() {
    switch (g_mode) {
        case FOLLOW_LINE: followLine();    break;
        case AVOID:       avoidObstacles();break;
        case MANUAL:      default:         break;
    }
}

} // namespace behaviors
