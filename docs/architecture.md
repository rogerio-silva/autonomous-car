# Arquitetura — autonomous-car

## 1. Visão geral

O sistema é embarcado e organizado em **camadas**: percepção (sensores + visão) → decisão (máquina de estados/autonomia) → atuação (motores). Tudo roda no Arduino Mega 2560.

```
            +--------------------------------------------------+
            |                  Arduino Mega 2560               |
            |                                                  |
 HuskyLens  |   +-----------+     +------------------+         |
 (I2C) ----->-->|  vision   |     |                  |         |
            |   +-----------+ --> |                  |   +-----------+   +--------+
 HC-SR04 -->|   +-----------+     |   AUTONOMIA      |-->|  motors   |-->| L298N  |--> Motores
 IR linha ->|-->|  sensors  | --> |  (máq. estados)  |   +-----------+   +--------+
 IR obst. ->|   +-----------+     |                  |                              (2WD)
            |                     +------------------+                              |
            |                              |  Serial 115200 (telemetria/depuração)  |
            +--------------------------------------------------+
```

## 2. Camadas de software

| Camada | Módulo | Arquivos | Responsabilidade |
|--------|--------|----------|------------------|
| Percepção – visão | `vision` | `include/vision.h`, `src/vision.cpp` | Lê resultados da HuskyLens (I²C) e expõe alvo primário + erro horizontal. |
| Percepção – sensores | `sensors` | `include/sensors.h`, `src/sensors.cpp` | Distância (HC-SR04), seguidor de linha (IR), obstáculo (IR). |
| Decisão | `main` | `src/main.cpp` | Máquina de estados que combina percepção e comanda atuação. |
| Atuação | `motors` | `include/motors.h`, `src/motors.cpp` | Locomoção 2WD via L298N (frente/ré/curvas/PWM). |
| Configuração | — | `include/config.h` | Pinos e constantes globais. |

### Princípios
- **Baixo acoplamento:** cada módulo expõe uma interface mínima por `namespace`; o `main` orquestra.
- **Configuração única:** nenhum pino fora de `config.h`.
- **Não bloqueante (meta de produção):** o loop deve ler percepção a cada iteração; `delay()` longos só no esqueleto.

## 3. Máquina de estados (evolução)

Esqueleto atual (M1): `IDLE → CRUISE ⇄ AVOID`.

```
        +--------+   inicialização   +--------+
        |  IDLE  | ----------------> | CRUISE |
        +--------+                   +--------+
                                       |   ^
                          obstáculo    |   | concluído
                                       v   |
                                     +--------+
                                     | AVOID  |
                                     +--------+
```

Estados a introduzir nas próximas milestones: `FOLLOW_LINE` (M3), `TRACK_TARGET` (M4, segue alvo da HuskyLens), `NAVIGATE` (M5, fusão completa).

## 4. Arquitetura de hardware

Detalhamento elétrico, BOM e pinout em [`hardware.md`](hardware.md). Resumo dos barramentos:

- **PWM/Motores:** pinos 5,6 (EN) + 22–25 (direção) → L298N → 2 motores.
- **I²C (HuskyLens):** SDA=20, SCL=21 (hardware do Mega).
- **Ultrassom:** TRIG=30, ECHO=31.
- **IR linha:** A0/A1/A2 (analógico). **IR obstáculo:** 32 (digital).

## 5. Energia (diretriz)

Alimentação dos motores **separada** da lógica recomendada: bateria dos motores no L298N (VS), Arduino por sua própria fonte/USB, com **GND comum**. Detalhes e dimensionamento em `hardware.md` (M2).
