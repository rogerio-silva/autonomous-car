# Requisitos — autonomous-car

Documento vivo de requisitos. Cada entrega registra aqui seu requisito antes do deploy, conforme o processo definido em [`git-workflow.md`](git-workflow.md).

---

## 1. Requisitos gerais do produto

### 1.1 Objetivo
Construir um carro autônomo embarcado capaz de **navegar evitando obstáculos** e **reagir ao ambiente** com base em **visão computacional** (HuskyLens) e **sensores** (ultrassom + infravermelho), sem qualquer software externo.

### 1.2 Requisitos funcionais (RF)
- **RF01** — O carro deve locomover-se (frente, ré, curvas) com controle de velocidade (PWM).
- **RF02** — O carro deve medir a distância de obstáculos à frente (ultrassom) e parar/desviar abaixo de um limiar.
- **RF03** — O carro deve detectar uma linha/pista por sensores IR refletivos.
- **RF04** — O carro deve detectar obstáculos próximos por sensor IR.
- **RF05** — O carro deve obter resultados de visão da HuskyLens (objeto/linha/tag/cor) via I²C.
- **RF06** — O carro deve combinar visão + sensores para decidir a navegação (autonomia).
- **RF07** — O sistema deve emitir telemetria de depuração pela serial (115200 baud).

### 1.3 Requisitos não funcionais (RNF)
- **RNF01** — Firmware em C++ (Arduino), compilável via PlatformIO (`pio run`).
- **RNF02** — Código modular; configuração de hardware centralizada em `config.h`.
- **RNF03** — Operação 100% embarcada (sem PC/app/web em runtime).
- **RNF04** — Documentação e histórico de git em PT-BR.
- **RNF05** — Loop de controle não bloqueante (evitar `delay()` longos em produção).

### 1.4 Restrições e decisões
- MCU: Arduino Mega 2560 · Tração: 2WD · Driver: L298N
- Visão: Gravity HuskyLens via I²C · Build: PlatformIO

---

## 2. Requisitos por entrega

### Entrega M1 — Fundação do Projeto  (issue #1)
**Requisito:** estabelecer a base do projeto para que o desenvolvimento subsequente seja reprodutível e organizado.

**Critérios de aceite:**
- [x] Estrutura de diretórios PlatformIO (`include/`, `src/`, `lib/`).
- [x] `platformio.ini` configurado para `megaatmega2560` com `lib_deps`.
- [x] Esqueleto de firmware modular (motors, sensors, vision, main) com máquina de estados mínima.
- [x] `README.md` completo.
- [x] `CLAUDE.md` com diretrizes de desenvolvimento.
- [x] Documentos em `docs/`: requirements, architecture, hardware, vision-huskylens, git-workflow, roadmap.
- [x] Milestones (M1–M5), labels e issue desta entrega criados no GitHub.
- [x] `.gitignore`.

**Fora de escopo (entregas futuras):** lógica real de navegação, calibração de sensores, integração completa da HuskyLens, controle PID.

---

> **Próximas entregas** (M2–M5) terão seus requisitos detalhados aqui no início de cada uma. Ver [`roadmap.md`](roadmap.md).
