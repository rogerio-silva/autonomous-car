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
| Percepção – visão | `vision` | `include/vision.h`, `src/vision.cpp` | Lê resultados da HuskyLens (I²C) e expõe alvo primário + erro horizontal. *(integração em M4)* |
| Percepção – sensores | `sensors` | `include/sensors.h`, `src/sensors.cpp` | Distância (HC-SR04), seguidor de linha (IR), obstáculo (IR). *(integração em M3)* |
| Comando / decisão | `main` + `commands` | `src/main.cpp`, `include/commands.h`, `src/commands.cpp` | M2: CLI serial WASD (modo manual). Evoluirá para máquina de estados autônoma (M5). |
| Atuação | `motors` | `include/motors.h`, `src/motors.cpp` | Locomoção 2WD via L298N: rampas **não-bloqueantes**, trim por motor e failsafe. |
| Persistência | `storage` | `include/storage.h`, `src/storage.cpp` | Salva/recupera a calibração (trim) na EEPROM, validada por magic+versão. |
| Configuração | — | `include/config.h` | Pinos e constantes globais. |

### Loop de controle (M2 — não-bloqueante)
O `loop()` não usa `delay()`: a cada iteração `commands::poll()` lê as teclas e atualiza os alvos/telemetria, e `motors::update()` avança as rampas, aplica o PWM (com trim) e o failsafe. Esse padrão não-bloqueante é a base sobre a qual a autonomia (M3–M5) será construída.

### Princípios
- **Baixo acoplamento:** cada módulo expõe uma interface mínima por `namespace`; o `main` orquestra.
- **Configuração única:** nenhum pino fora de `config.h`.
- **Não bloqueante (meta de produção):** o loop deve ler percepção a cada iteração; `delay()` longos só no esqueleto.

## 3. Modos de operação (evolução)

- **M2 (atual) — MANUAL:** o `loop()` apenas pilota via CLI serial; não há decisão autônoma. Foco em locomoção confiável e calibrada.
- **M3 — sensoriamento:** introduz leitura/uso de sensores (parada por obstáculo, seguir-linha) e o estado `FOLLOW_LINE`.
- **M4 — visão:** estado `TRACK_TARGET` (segue alvo da HuskyLens).
- **M5 — autonomia:** máquina de estados completa com fusão sensores+visão (`NAVIGATE`).

Esboço da máquina de estados autônoma (alvo do M5):

```
        +--------+   comando   +--------+   obstáculo   +--------+
        | MANUAL | <---------> | NAVIGATE| -----------> | AVOID  |
        +--------+             +--------+ <----------- +--------+
                                  |   ^   concluído
                       alvo visto |   | alvo perdido
                                  v   |
                              +-------------+
                              | TRACK_TARGET|
                              +-------------+
```

## 4. Arquitetura de hardware

Detalhamento elétrico, BOM e pinout em [`hardware.md`](hardware.md). Resumo dos barramentos:

- **PWM/Motores:** pinos 5,6 (EN) + 22–25 (direção) → L298N → 2 motores.
- **I²C (HuskyLens):** SDA=20, SCL=21 (hardware do Mega).
- **Ultrassom:** TRIG=30, ECHO=31.
- **IR linha:** A0/A1/A2 (analógico). **IR obstáculo:** 32 (digital).

## 5. Energia (diretriz)

Alimentação dos motores **separada** da lógica recomendada: bateria dos motores no L298N (VS), Arduino por sua própria fonte/USB, com **GND comum**. Detalhes e dimensionamento em `hardware.md` (M2).
