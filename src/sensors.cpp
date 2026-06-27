/**
 * sensors.cpp — Implementação da leitura de sensores
 * Ver include/sensors.h, include/config.h e include/storage.h
 */
#include "sensors.h"
#include "config.h"
#include "storage.h"
#include <NewPing.h>

namespace {

NewPing sonar(ULTRA_TRIG, ULTRA_ECHO, ULTRA_MAX_DIST_CM);

const uint8_t IR_PINS[3] = { IR_LINE_LEFT, IR_LINE_CENTER, IR_LINE_RIGHT };

// Amostra de fundo capturada na calibração (RAM; o resultado vira limiar).
int16_t g_bg[3]   = {0, 0, 0};
bool    g_hasBg   = false;

// Decide se o sensor i está "sobre a linha" conforme limiar e polaridade.
bool onLine(uint8_t i, int16_t raw) {
    const storage::Config& c = storage::data();
    bool high = (c.irLineHigh >> i) & 0x01;
    return high ? (raw > c.irThreshold[i]) : (raw < c.irThreshold[i]);
}

} // namespace

namespace sensors {

void begin() {
    pinMode(IR_OBSTACLE_FRONT, INPUT);
    // Entradas analógicas (IR de linha) não exigem pinMode.
    // Limiares/polaridade vêm de storage::data() (carregado no setup).
}

uint16_t distanceCm() {
    // Mediana de várias amostras: descarta ecos espúrios do HC-SR04.
    // Mais lento (vários pings) — uso para telemetria, não no loop de controle.
    unsigned int us = sonar.ping_median(5);
    return (uint16_t)NewPing::convert_cm(us); // 0 se fora de alcance
}

bool obstacleAhead() {
    // Ping único (rápido) para manter os loops de controle responsivos.
    uint16_t d = sonar.ping_cm();
    return (d > 0 && d <= OBSTACLE_STOP_CM) || irObstacle();
}

LineState readLine() {
    LineState s;
    s.left   = onLine(0, (int16_t)analogRead(IR_LINE_LEFT));
    s.center = onLine(1, (int16_t)analogRead(IR_LINE_CENTER));
    s.right  = onLine(2, (int16_t)analogRead(IR_LINE_RIGHT));
    return s;
}

bool irObstacle() {
    return digitalRead(IR_OBSTACLE_FRONT) == LOW;
}

// --- Calibração ---
void readLineRaw(int16_t out[3]) {
    for (uint8_t i = 0; i < 3; i++) out[i] = (int16_t)analogRead(IR_PINS[i]);
}

void sampleBackground() {
    readLineRaw(g_bg);
    g_hasBg = true;
}

bool sampleLine() {
    if (!g_hasBg) return false;
    int16_t line[3];
    readLineRaw(line);

    storage::Config& c = storage::data();
    uint8_t highMask = 0;
    for (uint8_t i = 0; i < 3; i++) {
        c.irThreshold[i] = (int16_t)((g_bg[i] + line[i]) / 2);
        if (line[i] > g_bg[i]) highMask |= (1 << i); // linha lê mais alto que o fundo
    }
    c.irLineHigh = highMask;
    return true;
}

} // namespace sensors
