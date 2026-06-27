# Guia Fritzing — montar o `.fzz` nativo

Este guia permite reproduzir um arquivo Fritzing **100% funcional** no aplicativo, já que o `.fzz` incluído neste repositório é apenas **best-effort** (ver [README](README.md#-aviso-sobre-o-arquivo-fritzing-fzzfz)).

## 1. Pré-requisitos
- [Fritzing](https://fritzing.org) instalado (0.9.x+).
- Algumas peças não fazem parte do core do Fritzing e precisam ser importadas do gerenciador de peças / repositórios da comunidade:
  - **Arduino Mega 2560** — incluído no core (Fritzing → Arduino).
  - **L298N (módulo)** — importar (busque "L298N" em *Parts → Import* ou no fórum/Adafruit/SparkFun libs).
  - **HC-SR04** — disponível em libs da comunidade ("HC-SR04 ultrasonic").
  - **Sensor IR / TCRT5000 / FC-51** — peça genérica de 3 pinos (VCC/GND/OUT) serve para IR de linha e de obstáculo.
  - **HuskyLens** — pode não existir; use uma peça genérica de 4 pinos (header) rotulada +/−/T/R.
  - **Motor DC**, **bateria**, **chave (switch)** — core do Fritzing.

## 2. Lista de peças (BOM resumida)
| Qtd | Peça | Pinos usados |
|-----|------|--------------|
| 1 | Arduino Mega 2560 | 5,6,20,21,22,23,24,25,30,31,32,A0,A1,A2,5V,GND |
| 1 | L298N (driver) | ENA,IN1,IN2,IN3,IN4,ENB,VS,GND,OUT1-4 |
| 2 | Motor DC | 2 terminais cada |
| 1 | HC-SR04 | VCC,TRIG,ECHO,GND |
| 3 | Módulo IR de linha | VCC,GND,OUT |
| 1 | Módulo IR de obstáculo | VCC,GND,OUT |
| 1 | Gravity HuskyLens | +,−,T,R (I²C) |
| 1 | Bateria 6–7.4V | +,− |
| 1 | Chave ON/OFF | 2 terminais |

## 3. Netlist (todas as conexões)

Formato: `ComponenteA.pino — ComponenteB.pino`.

```
# Energia
Bateria.+        — Chave.1
Chave.2          — L298N.VS
Bateria.-        — L298N.GND
L298N.GND        — Mega.GND
Mega.5V          — RAIL_5V
Mega.GND         — RAIL_GND

# 5V para sensores
RAIL_5V — HC-SR04.VCC
RAIL_5V — IR_Linha_E.VCC
RAIL_5V — IR_Linha_C.VCC
RAIL_5V — IR_Linha_D.VCC
RAIL_5V — IR_Obstaculo.VCC
RAIL_5V — HuskyLens.+

# GND para sensores
RAIL_GND — HC-SR04.GND
RAIL_GND — IR_Linha_E.GND
RAIL_GND — IR_Linha_C.GND
RAIL_GND — IR_Linha_D.GND
RAIL_GND — IR_Obstaculo.GND
RAIL_GND — HuskyLens.-

# Motores (controle)
Mega.5   — L298N.ENA
Mega.22  — L298N.IN1
Mega.23  — L298N.IN2
Mega.24  — L298N.IN3
Mega.25  — L298N.IN4
Mega.6   — L298N.ENB
L298N.OUT1 — MotorEsq.A
L298N.OUT2 — MotorEsq.B
L298N.OUT3 — MotorDir.A
L298N.OUT4 — MotorDir.B

# Ultrassônico
Mega.30  — HC-SR04.TRIG
Mega.31  — HC-SR04.ECHO

# IR de linha (analógico)
Mega.A0  — IR_Linha_E.OUT
Mega.A1  — IR_Linha_C.OUT
Mega.A2  — IR_Linha_D.OUT

# IR de obstáculo (digital)
Mega.32  — IR_Obstaculo.OUT

# HuskyLens (I²C) — atenção: T=SCL, R=SDA
Mega.20(SDA) — HuskyLens.R
Mega.21(SCL) — HuskyLens.T
```

## 4. Passo a passo no Fritzing
1. Abra o Fritzing e crie um novo sketch; salve como `autonomous-car.fzz`.
2. Na aba **Breadboard**, arraste as peças da lista (importe as que faltarem).
3. Adicione duas linhas de alimentação (use as faixas + e − da protoboard) e ligue `Mega.5V`→linha vermelha e `Mega.GND`→linha azul/preta.
4. Faça as conexões da **netlist** acima, uma a uma. Dica: clique no pino e arraste até o destino.
5. Use cores de fio coerentes: vermelho=5V, preto=GND, azul=controle de motor, laranja=saída de motor, verde=IR analógico, roxo=I²C.
6. Confira na aba **Schematic** se não há fios soltos (pinos vermelhos = sem conexão).
7. Para a bateria dos motores, **não** ligue VS ao 5V do Arduino — é uma fonte separada com GND comum.
8. Salve. O `.fzz` resultante é o arquivo nativo fiel.

## 5. Conferência final
Compare a montagem com:
- [`README.md`](README.md) (matriz de conexões),
- [`../hardware.md`](../hardware.md) (pinout),
- [`../../include/config.h`](../../include/config.h) (fonte de verdade dos pinos).

Qualquer divergência → corrija e atualize os três lugares.
