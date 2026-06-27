/**
 * sensors.cpp — Implementação da leitura de sensores
 * Ver include/sensors.h e include/config.h
 *
 * NOTA: esqueleto da Fundação (M1). Filtragem (média móvel), calibração
 * dos IR e fusão serão tratadas em M3.
 */
#include "sensors.h"
#include "config.h"
#include <NewPing.h>

namespace {
NewPing sonar(ULTRA_TRIG, ULTRA_ECHO, ULTRA_MAX_DIST_CM);
}

namespace sensors {

void begin() {
    pinMode(IR_OBSTACLE_FRONT, INPUT);
    // Entradas analógicas (IR de linha) não exigem pinMode.
}

uint16_t distanceCm() {
    return sonar.ping_cm(); // 0 quando não há eco dentro do alcance
}

bool obstacleAhead() {
    uint16_t d = distanceCm();
    return (d > 0 && d <= OBSTACLE_STOP_CM) || irObstacle();
}

LineState readLine() {
    LineState s;
    s.left   = analogRead(IR_LINE_LEFT)   > IR_LINE_THRESHOLD;
    s.center = analogRead(IR_LINE_CENTER) > IR_LINE_THRESHOLD;
    s.right  = analogRead(IR_LINE_RIGHT)  > IR_LINE_THRESHOLD;
    return s;
}

bool irObstacle() {
    return digitalRead(IR_OBSTACLE_FRONT) == LOW;
}

} // namespace sensors
