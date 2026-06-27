/**
 * config.h — Mapeamento de pinos e constantes globais
 *
 * Plataforma: Arduino Mega 2560
 * Tração:     2WD (2 motores DC traseiros) via driver L298N
 * Visão:      Gravity HuskyLens (I2C — SDA=20, SCL=21 no Mega)
 *
 * Centralize TODA configuração de hardware aqui. Nenhum número de pino
 * deve aparecer "solto" nos módulos.
 */
#ifndef CONFIG_H
#define CONFIG_H

// ===================== Serial / Depuração =====================
#define SERIAL_BAUD          115200

// ===================== Motores (L298N) ========================
// Motor esquerdo (traseiro)
#define MOTOR_L_EN            5   // ENA — PWM (velocidade)
#define MOTOR_L_IN1          22   // IN1 — sentido
#define MOTOR_L_IN2          23   // IN2 — sentido
// Motor direito (traseiro)
#define MOTOR_R_EN            6   // ENB — PWM (velocidade)
#define MOTOR_R_IN1          24   // IN3 — sentido
#define MOTOR_R_IN2          25   // IN4 — sentido

#define MOTOR_SPEED_MAX     255   // PWM máximo (0–255)
#define MOTOR_SPEED_CRUISE  180   // velocidade de cruzeiro padrão

// ===================== Sensor ultrassônico (HC-SR04) ==========
#define ULTRA_TRIG           30
#define ULTRA_ECHO           31
#define ULTRA_MAX_DIST_CM   200   // distância máxima de leitura
#define OBSTACLE_STOP_CM     20   // limiar de parada por obstáculo

// ===================== Sensores IR seguidor de linha ==========
// Array de 3 sensores reflexivos (entradas analógicas)
#define IR_LINE_LEFT         A0
#define IR_LINE_CENTER       A1
#define IR_LINE_RIGHT        A2
#define IR_LINE_THRESHOLD   500   // limiar linha/fundo (0–1023) — calibrar

// ===================== Sensor IR de obstáculo =================
#define IR_OBSTACLE_FRONT    32   // saída digital (LOW = obstáculo)

// ===================== HuskyLens (I2C) ========================
// SDA=20, SCL=21 são fixos no Mega 2560 (barramento I2C de hardware).
#define HUSKYLENS_I2C_ADDR  0x32

// ===================== Locomoção / rampas (M2) ================
// Passo de PWM aplicado a cada tick de rampa e intervalo entre ticks.
// Quanto maior o passo / menor o intervalo, mais rápida a aceleração.
#define MOTOR_RAMP_STEP        8   // unidades de PWM por tick
#define MOTOR_RAMP_INTERVAL_MS 10  // intervalo entre ticks de rampa

// Failsafe: se nenhum comando chegar nesse intervalo enquanto os motores
// estão em movimento, eles param sozinhos (segurança no controle serial).
#define FAILSAFE_TIMEOUT_MS  1500

// Trim (calibração de offset entre motores): fator multiplicativo por lado,
// no intervalo [TRIM_MIN, 1.0]. 1.0 = sem correção. Reduz-se o lado mais
// "forte" para o carro andar reto. Valores padrão e limites de ajuste:
#define TRIM_DEFAULT          1.00f
#define TRIM_MIN              0.50f
#define TRIM_STEP             0.02f  // incremento por tecla de calibração

// ===================== EEPROM (persistência) =================
// Endereço base e assinatura para validar os dados salvos.
#define EEPROM_CONFIG_ADDR    0
#define EEPROM_MAGIC          0xA5C2
#define EEPROM_VERSION        1

#endif // CONFIG_H
