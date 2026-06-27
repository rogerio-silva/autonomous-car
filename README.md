# 🚗 autonomous-car

Carro autônomo inteligente baseado em **Arduino Mega 2560**, com visão computacional embarcada pela **Gravity HuskyLens** e sensoriamento por **ultrassom** e **infravermelho**. O projeto é 100% embarcado: não há interface gráfica nem software externo — apenas o firmware Arduino e a integração dos componentes.

> Status: **roadmap M1–M5 concluído** — locomoção, sensoriamento, visão (HuskyLens) e navegação autônoma. Veja o [roadmap](docs/roadmap.md).

---

## ✨ Visão geral

| Item | Definição |
|------|-----------|
| Microcontrolador | Arduino Mega 2560 |
| Tração | 2WD — 4 rodas, 2 motores DC traseiros |
| Driver de motor | L298N (ponte H dupla) |
| Visão computacional | Gravity HuskyLens (I²C) |
| Sensor de distância | HC-SR04 (ultrassônico frontal) |
| Sensores IR | Array seguidor de linha (3×) + IR de obstáculo |
| Build system | PlatformIO |
| Linguagem | C++ (Arduino framework) |

A HuskyLens executa a visão computacional **a bordo** (reconhecimento/rastreio de objetos, linha, tags e cores) e entrega os resultados ao Mega via I²C. O firmware funde esses dados com os sensores para decidir a navegação.

---

## 📁 Estrutura do repositório

```
autonomous-car/
├── platformio.ini          # Configuração de build (board, libs)
├── include/                # Headers (.h) — interfaces dos módulos
│   ├── config.h            # Mapa de pinos e constantes
│   ├── motors.h            # locomoção 2WD (rampas, trim, failsafe)
│   ├── sensors.h           # HC-SR04 (mediana) + IR calibrável
│   ├── behaviors.h         # modos autônomos (linha / desvio / rastreio)
│   ├── commands.h          # CLI serial (WASD + modos)
│   ├── storage.h           # config persistente (EEPROM)
│   └── vision.h            # alvo da HuskyLens (I²C)
├── src/                    # Implementação (.cpp)
│   ├── main.cpp            # setup()/loop() não-bloqueante
│   ├── motors.cpp          # locomoção 2WD (L298N)
│   ├── sensors.cpp         # HC-SR04 + IR (limiares calibráveis)
│   ├── behaviors.cpp       # seguir-linha + desvio + rastreio visual
│   ├── commands.cpp        # parsing de comandos serial
│   ├── storage.cpp         # EEPROM (trim + limiares IR)
│   └── vision.cpp          # leitura da HuskyLens (maior bloco/Tag)
├── lib/                    # Bibliotecas privadas do projeto
└── docs/                   # Documentação
    ├── requirements.md     # Requisitos do projeto e por entrega
    ├── architecture.md     # Arquitetura de hardware e software
    ├── hardware.md         # Lista de materiais (BOM) + pinout
    ├── vision-huskylens.md # Integração HuskyLens
    ├── git-workflow.md     # Fluxo de git/issues/PRs
    └── roadmap.md          # Milestones M1–M5
```

---

## 🛠️ Pré-requisitos

- [PlatformIO Core](https://docs.platformio.org/page/core/installation.html) (CLI) ou a extensão PlatformIO no VS Code
- Driver USB do Arduino Mega 2560
- Componentes de hardware listados em [`docs/hardware.md`](docs/hardware.md)

Instalação do PlatformIO Core (exemplo):

```bash
python3 -m pip install -U platformio
```

---

## 🚀 Build e gravação

```bash
# Compilar o firmware
pio run

# Compilar e gravar no Mega 2560 (conectado via USB)
pio run --target upload

# Monitor serial (depuração) — 115200 baud
pio device monitor
```

As bibliotecas externas (`HUSKYLENS`, `NewPing`) são baixadas automaticamente pelo PlatformIO na primeira compilação, conforme declarado em `platformio.ini`.

---

## 🔌 Ligações

O mapa completo de pinos está em [`docs/hardware.md`](docs/hardware.md) e centralizado em código em [`include/config.h`](include/config.h). **Toda alteração de pino deve ser feita nesses dois lugares.**

## 🎮 Pilotagem via serial (M2)

A partir do M2 o carro é pilotado pelo Monitor Serial (115200 baud), com CLI estilo **WASD**, rampas de aceleração, calibração de trim persistida em EEPROM e failsafe.

| Tecla | Ação | | Tecla | Ação |
|-------|------|-|-------|------|
| `w`/`s` | frente / ré | | `1`/`2` | trim esq − / + |
| `a`/`d` | girar esq / dir | | `3`/`4` | trim dir − / + |
| `espaço`/`x` | parar | | `k`/`n` | salvar / resetar trim |
| `+`/`-` | velocidade | | `o`/`t`/`h` | status / telemetria / ajuda |

**Modos:** `m` MANUAL · `l` SEGUIR‑LINHA · `v` DESVIO · `p` RASTREIO (HuskyLens) · **`N` NAVEGAÇÃO (autonomia total)** · `c`/`f`/`g` calibração dos IR · `i` info de visão. Guia completo em [`docs/serial-control.md`](docs/serial-control.md).

O modo **NAVEGAÇÃO** funde sensores + visão por prioridade: **Segurança (desvio) > Alvo visual (Tag) > Seguir‑linha > Busca**. O boot é sempre em MANUAL (o carro não se move ao energizar).

---

## 📚 Documentação

- [Requisitos](docs/requirements.md)
- [Arquitetura](docs/architecture.md)
- [Hardware / BOM / Pinout](docs/hardware.md)
- [Montagem completa (esquemático + Fritzing)](docs/assembly/README.md)
- [Controle serial (M2)](docs/serial-control.md)
- [Visão computacional (HuskyLens)](docs/vision-huskylens.md)
- [Fluxo de trabalho Git](docs/git-workflow.md)
- [Roadmap](docs/roadmap.md)
- [Diretrizes de desenvolvimento (CLAUDE.md)](CLAUDE.md)

---

## 🤝 Contribuição

O desenvolvimento segue um fluxo estruturado: cada entrega tem **requisito documentado**, **issue** ligada a **milestone** e **labels**, **branch dedicada**, **pull request** e **merge**. Detalhes em [`docs/git-workflow.md`](docs/git-workflow.md).

## 📄 Licença

A definir.
