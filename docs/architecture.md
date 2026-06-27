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
| Percepção – visão | `vision` | `include/vision.h`, `src/vision.cpp` | Lê a HuskyLens (I²C), seleciona o maior bloco (alvo/Tag) e expõe ID, posição e erro horizontal. |
| Percepção – sensores | `sensors` | `include/sensors.h`, `src/sensors.cpp` | Distância **filtrada por mediana** (HC-SR04), seguidor de linha (IR) com limiares calibráveis, obstáculo (IR). |
| Decisão / autonomia | `behaviors` | `include/behaviors.h`, `src/behaviors.cpp` | Modos MANUAL / SEGUIR-LINHA / DESVIO; lógica não-bloqueante que liga `sensors` a `motors`. |
| Comando (serial) | `main` + `commands` | `src/main.cpp`, `include/commands.h`, `src/commands.cpp` | CLI WASD, seleção de modo e calibração IR. |
| Atuação | `motors` | `include/motors.h`, `src/motors.cpp` | Locomoção 2WD via L298N: rampas **não-bloqueantes**, trim por motor e failsafe. |
| Persistência | `storage` | `include/storage.h`, `src/storage.cpp` | Cache central de configuração (trim + limiares/polaridade IR) na EEPROM, validado por magic+versão. **Fonte única** lida por `motors` e `sensors`. |
| Configuração | — | `include/config.h` | Pinos e constantes globais. |

### Loop de controle (não-bloqueante)
O `loop()` não usa `delay()`: `commands::poll()` lê as teclas (pilotagem manual, modo, calibração); se o modo não é MANUAL, `behaviors::update()` executa o comportamento autônomo ativo (lendo sensores e definindo alvos dos motores); e `motors::update()` avança as rampas, aplica o PWM (com trim) e o failsafe. Persistência é centralizada em `storage` (fonte única, em EEPROM).

### Princípios
- **Baixo acoplamento:** cada módulo expõe uma interface mínima por `namespace`; o `main` orquestra.
- **Configuração única:** nenhum pino fora de `config.h`.
- **Não bloqueante (meta de produção):** o loop deve ler percepção a cada iteração; `delay()` longos só no esqueleto.

## 3. Modos de operação (evolução)

- **M2 — MANUAL:** locomoção confiável e calibrada via CLI serial.
- **M3 — modos por sensores:** `behaviors` adiciona **SEGUIR-LINHA** (proporcional pelo array IR) e **DESVIO** (manobra não-bloqueante por obstáculo).
- **M4 — visão:** modo **RASTREIO** (`TRACK_TARGET`) — segue um alvo da HuskyLens (Tag), esterçando pelo erro horizontal e aproximando-se até uma distância-alvo, com busca ao perder o alvo.
- **M5 (atual) — autonomia:** modo **NAVEGAÇÃO** (`NAVIGATE`) — funde tudo por prioridade. Reaproveita os núcleos dos modos isolados.

### Modos isolados (M3–M4)
```
        +--------+   l   +--------------+
        | MANUAL | ----> | SEGUIR-LINHA |  erro ponderado (3× IR) -> diferencial
        |        | <---- |              |
        |        |   v   +--------------+
        |        | ----> |   DESVIO     |  cruzeiro -> ré -> giro -> cruzeiro
        |        | <---- |              |  (alterna o lado a cada obstáculo)
        |        |   p   +--------------+
        |        | ----> |   RASTREIO   |  esterça (erro visual) + aproxima/para;
        +--------+ <---- +--------------+  alvo perdido -> gira procurando
                     m   (espaço/x = emergência -> MANUAL em qualquer modo)
```

### NAVEGAÇÃO — arbitrador por prioridade (M5)
O modo `NAVIGATE` (tecla `N`) não é um comportamento novo: é um **arbitrador subsumption** que, a cada loop, escolhe a camada de maior prioridade e delega ao núcleo correspondente. A telemetria expõe a camada ativa: `modo=NAVEGACAO(CAMADA)`.

```
  a cada loop:
  ┌────────────────────────────────────────────────────────────┐
  │ manobra de desvio em andamento? ── sim ─► [SEGURANCA] conclui manobra
  │            │ não
  │ obstáculo à frente? ───────────── sim ─► [SEGURANCA] inicia desvio (latch)
  │            │ não
  │ alvo visual (Tag) detectado? ──── sim ─► [ALVO] esterça + aproxima
  │            │ não
  │ linha sob algum sensor IR? ────── sim ─► [LINHA] segue a linha
  │            │ não
  │ nada ──────────────────────────────────► [BUSCA] gira procurando
  └────────────────────────────────────────────────────────────┘
```

Princípio: **segurança nunca é preemptada** (a manobra de desvio é latcheada até concluir); abaixo dela, o objetivo (alvo visual) tem precedência sobre o caminho (linha); sem nada, o carro busca ativamente.

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
