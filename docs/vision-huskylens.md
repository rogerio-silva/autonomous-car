# Visão Computacional — Gravity HuskyLens

## 1. Papel no projeto

A **Gravity HuskyLens** é uma câmera com IA embarcada: executa a visão computacional **localmente** (sem PC) e entrega resultados já processados ao Arduino. Ela é a **base da autonomia** — o Mega lê os resultados e decide a navegação combinando-os com os sensores.

> Não escrevemos algoritmos de visão "do zero": configuramos a HuskyLens, treinamos objetos/linhas quando aplicável e **lemos** os resultados via I²C.

## 2. Algoritmos disponíveis na HuskyLens

| Algoritmo | Uso potencial no carro |
|-----------|------------------------|
| Object Tracking | Seguir um alvo específico |
| Object Recognition | Reconhecer objetos/sinais aprendidos |
| Line Tracking | Seguir pista/linha (visão, complementa o IR) |
| Color Recognition | Reagir a cores (ex.: faixas, semáforos) |
| Tag (AprilTag) Recognition | Marcadores de navegação/estações |
| Face Recognition | (não prioritário) |

**Algoritmo primário escolhido (M4): Tag (AprilTag) Recognition.** O modo RASTREIO segue o maior bloco de tag detectado. Os demais algoritmos permanecem possíveis (a leitura por "maior bloco" funciona para tags, objetos e cores).

## 3. Conexão (I²C)

- **Ligação física:** SDA→20, SCL→21, VCC→5V, GND→GND (ver [`hardware.md`](hardware.md)).
- **Configuração na HuskyLens:** menu *Settings → Protocol Type → I²C*.
- **Endereço I²C:** `0x32` (`HUSKYLENS_I2C_ADDR` em `config.h`).
- **Biblioteca:** `dfrobot/HUSKYLENS` (declarada em `platformio.ini`).

## 4. Modelo de software

Módulo [`vision`](../include/vision.h):

```cpp
vision::begin();              // inicia I²C e valida o dispositivo
vision::update();             // a cada loop: lê os blocos do quadro
vision::Target t = vision::primaryTarget();   // maior/principal alvo
int16_t err = vision::horizontalError();      // -160..160 p/ controle de direção
```

- `update()` requisita os dados, ignora o quadro se nada foi aprendido e seleciona o **maior bloco** como alvo primário.
- `horizontalError()` fornece o desvio do alvo em relação ao centro do quadro (320×240 → centro X=160), pronto para um controlador proporcional de direção.

## 5. Fluxo de aprendizado (operacional)

1. Selecionar o algoritmo na HuskyLens (botão de função).
2. Apontar para o objeto/linha e pressionar **learn** para atribuir um ID.
3. O firmware passa a receber blocos/setas com esse ID em `update()`.

## Operação do modo RASTREIO (M4)

O modo é selecionado pela serial com a tecla **`p`** (ver [`serial-control.md`](serial-control.md)). Comportamento (`behaviors::trackTarget`):
- **Esterço:** proporcional ao `horizontalError()` (alvo à direita → vira à direita).
- **Aproximação:** avança a `VISION_APPROACH_SPEED` enquanto o alvo aparece pequeno e **para** quando a altura do bloco atinge `VISION_TARGET_HEIGHT` (proxy de distância). O ultrassom é uma trava de segurança: para antes de colidir.
- **Alvo perdido:** gira na direção do último erro por `VISION_SEARCH_MS` para reencontrá-lo; se não achar, para.

Use **`i`** para inspecionar o alvo atual (id, x, altura, erro horizontal). Ganhos e limiares ficam em [`../include/config.h`](../include/config.h) (`VISION_*`).

## Roadmap da visão

- **M4 ✅:** integração funcional — leitura por I²C, modo RASTREIO (Tag) com aproximação e busca.
- **M5:** fusão com sensores na lógica de navegação (`NAVIGATE`).

## 7. Referências

- Documentação oficial HuskyLens (DFRobot) — wiki do produto.
- Biblioteca Arduino: `HUSKYLENS` (DFRobot).
