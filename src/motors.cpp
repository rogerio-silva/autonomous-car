/**
 * motors.cpp — Locomoção 2WD não-bloqueante (L298N)
 * Ver include/motors.h, include/config.h e docs/serial-control.md
 */
#include "motors.h"
#include "config.h"
#include "storage.h"

namespace {

// Estado das rampas (alvo x atual) por lado, com sinal (-255..255).
int16_t  g_targetL = 0, g_targetR = 0;
int16_t  g_currL   = 0, g_currR   = 0;

// Trim por lado (fator multiplicativo aplicado ao PWM).
float    g_trimL = TRIM_DEFAULT, g_trimR = TRIM_DEFAULT;

// Temporização da rampa e do failsafe.
uint32_t g_lastRampMs = 0;
uint32_t g_lastCmdMs  = 0;

int16_t clampSpeed(int16_t v) {
    if (v >  MOTOR_SPEED_MAX) return  MOTOR_SPEED_MAX;
    if (v < -MOTOR_SPEED_MAX) return -MOTOR_SPEED_MAX;
    return v;
}

float clampTrim(float t) {
    if (t > 1.0f)      return 1.0f;
    if (t < TRIM_MIN)  return TRIM_MIN;
    return t;
}

// Aproxima `current` de `target` em no máximo MOTOR_RAMP_STEP por chamada.
int16_t stepToward(int16_t current, int16_t target) {
    if (current < target) return (int16_t)min(current + MOTOR_RAMP_STEP, (int)target);
    if (current > target) return (int16_t)max(current - MOTOR_RAMP_STEP, (int)target);
    return current;
}

// Escreve no hardware um lado, aplicando o trim. speed: -255..255.
void applySide(uint8_t enPin, uint8_t in1, uint8_t in2, int16_t speed, float trim) {
    bool forward = speed >= 0;
    int16_t mag  = (int16_t)(abs(speed) * trim);
    if (mag > MOTOR_SPEED_MAX) mag = MOTOR_SPEED_MAX;
    digitalWrite(in1, forward ? HIGH : LOW);
    digitalWrite(in2, forward ? LOW : HIGH);
    analogWrite(enPin, (uint8_t)mag);
}

} // namespace

namespace motors {

void begin() {
    const uint8_t pins[] = {
        MOTOR_L_EN, MOTOR_L_IN1, MOTOR_L_IN2,
        MOTOR_R_EN, MOTOR_R_IN1, MOTOR_R_IN2
    };
    for (uint8_t p : pins) pinMode(p, OUTPUT);

    // Carrega trim persistido (ou padrões se a EEPROM estiver virgem).
    storage::Config cfg;
    storage::load(cfg);
    g_trimL = clampTrim(cfg.trimLeft);
    g_trimR = clampTrim(cfg.trimRight);

    brake();
}

void setTarget(int16_t leftSpeed, int16_t rightSpeed) {
    g_targetL = clampSpeed(leftSpeed);
    g_targetR = clampSpeed(rightSpeed);
    g_lastCmdMs = millis();           // renova o failsafe
}

void forward(uint8_t speed)  { setTarget(speed, speed); }
void backward(uint8_t speed) { setTarget(-(int16_t)speed, -(int16_t)speed); }
void turnLeft(uint8_t speed)  { setTarget(-(int16_t)speed, speed); }
void turnRight(uint8_t speed) { setTarget(speed, -(int16_t)speed); }
void stop()                  { setTarget(0, 0); }

void brake() {
    g_targetL = g_targetR = 0;
    g_currL = g_currR = 0;
    applySide(MOTOR_L_EN, MOTOR_L_IN1, MOTOR_L_IN2, 0, g_trimL);
    applySide(MOTOR_R_EN, MOTOR_R_IN1, MOTOR_R_IN2, 0, g_trimR);
}

void update() {
    uint32_t now = millis();

    // Failsafe: se há movimento (ou intenção) e o último comando expirou,
    // zera o alvo para desacelerar até parar.
    if ((g_targetL != 0 || g_targetR != 0) &&
        (now - g_lastCmdMs > FAILSAFE_TIMEOUT_MS)) {
        g_targetL = g_targetR = 0;
    }

    // Avança a rampa em intervalos fixos (não-bloqueante).
    if (now - g_lastRampMs < MOTOR_RAMP_INTERVAL_MS) return;
    g_lastRampMs = now;

    g_currL = stepToward(g_currL, g_targetL);
    g_currR = stepToward(g_currR, g_targetR);

    applySide(MOTOR_L_EN, MOTOR_L_IN1, MOTOR_L_IN2, g_currL, g_trimL);
    applySide(MOTOR_R_EN, MOTOR_R_IN1, MOTOR_R_IN2, g_currR, g_trimR);
}

// --- Trim / calibração ---
void setTrim(float left, float right) {
    g_trimL = clampTrim(left);
    g_trimR = clampTrim(right);
}
void adjustTrimLeft(float delta)  { g_trimL = clampTrim(g_trimL + delta); }
void adjustTrimRight(float delta) { g_trimR = clampTrim(g_trimR + delta); }
void resetTrim()                  { g_trimL = g_trimR = TRIM_DEFAULT; }

void saveTrim() {
    storage::Config cfg;
    cfg.trimLeft  = g_trimL;
    cfg.trimRight = g_trimR;
    storage::save(cfg);
}

float trimLeft()  { return g_trimL; }
float trimRight() { return g_trimR; }

// --- Telemetria ---
int16_t currentLeft()  { return g_currL; }
int16_t currentRight() { return g_currR; }
bool    isMoving()     { return g_currL != 0 || g_currR != 0; }

} // namespace motors
