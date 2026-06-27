/**
 * motors.cpp — Implementação do controle 2WD (L298N)
 * Ver include/motors.h e include/config.h
 *
 * NOTA: esqueleto da Fundação (M1). A lógica de rampa/aceleração e
 * a calibração de offset entre motores serão tratadas no M2.
 */
#include "motors.h"
#include "config.h"

namespace {

// Aciona um motor: speed > 0 frente, < 0 ré, 0 parado.
void driveMotor(uint8_t enPin, uint8_t in1, uint8_t in2, int16_t speed) {
    bool forward = speed >= 0;
    uint16_t pwm = (uint16_t)min((int16_t)abs(speed), (int16_t)MOTOR_SPEED_MAX);
    digitalWrite(in1, forward ? HIGH : LOW);
    digitalWrite(in2, forward ? LOW : HIGH);
    analogWrite(enPin, pwm);
}

} // namespace

namespace motors {

void begin() {
    const uint8_t pins[] = {
        MOTOR_L_EN, MOTOR_L_IN1, MOTOR_L_IN2,
        MOTOR_R_EN, MOTOR_R_IN1, MOTOR_R_IN2
    };
    for (uint8_t p : pins) pinMode(p, OUTPUT);
    stop();
}

void drive(int16_t leftSpeed, int16_t rightSpeed) {
    driveMotor(MOTOR_L_EN, MOTOR_L_IN1, MOTOR_L_IN2, leftSpeed);
    driveMotor(MOTOR_R_EN, MOTOR_R_IN1, MOTOR_R_IN2, rightSpeed);
}

void forward(uint8_t speed)  { drive(speed, speed); }
void backward(uint8_t speed) { drive(-(int16_t)speed, -(int16_t)speed); }
void turnLeft(uint8_t speed)  { drive(-(int16_t)speed, speed); }
void turnRight(uint8_t speed) { drive(speed, -(int16_t)speed); }
void stop()                  { drive(0, 0); }

} // namespace motors
