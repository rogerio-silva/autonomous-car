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

### Entrega M2 — Plataforma & Locomoção  (issue #5)
**Requisito:** o carro deve se locomover de forma controlada e confiável, comandável pelo Monitor Serial, sem software externo (atende RF01; base para RNF05).

**Critérios de aceite:**
- [x] Módulo `motors` não-bloqueante com **rampas** de aceleração/desaceleração (`update()` por loop).
- [x] Controle diferencial (frente/ré/curvas) com **setpoint de velocidade** (PWM) ajustável.
- [x] **Trim** por motor (corrige puxada lateral), ajustável em runtime e **persistido em EEPROM** (módulo `storage`).
- [x] **CLI serial** estilo WASD (`commands`): mover, velocidade, trim, salvar/resetar, status, ajuda.
- [x] **Failsafe**: motores param se nenhum comando chegar dentro de `FAILSAFE_TIMEOUT_MS`.
- [x] `main.cpp` refatorado para loop **não-bloqueante** (sem `delay()`), modo manual.
- [x] **Telemetria** de depuração pela serial.
- [x] Documentação: `docs/serial-control.md`, diagrama L298N + energia em `hardware.md`, atualização de `architecture.md` e `roadmap.md`.
- [x] `pio run` compila sem erros/warnings.

**Validação:** compilação verificada (`pio run` SUCCESS, sem warnings). **Teste físico de movimento e ajuste fino do trim ficam para a bancada do usuário** (sem hardware no ambiente de build).

**Fora de escopo (futuro):** uso de sensores na decisão (M3), visão (M4), navegação autônoma (M5).

---

### Entrega M3 — Sensoriamento  (issue #9)
**Requisito:** o carro deve perceber o ambiente e agir sobre ele — desviar de obstáculos e seguir uma linha — usando os sensores, mantendo a arquitetura não-bloqueante (atende RF02, RF03, RF04; base para RF06).

**Critérios de aceite:**
- [x] `sensors`: distância **filtrada por mediana** (HC-SR04) e API de **calibração dos limiares IR**.
- [x] `storage`: cache central de configuração; **limiares + polaridade IR persistidos** na EEPROM junto com o trim (versão da EEPROM incrementada).
- [x] Módulo `behaviors` com modos **MANUAL / SEGUIR-LINHA / DESVIO**, não-bloqueantes.
- [x] **Seguir-linha** por erro ponderado (controle proporcional no diferencial), com busca ao perder a linha.
- [x] **Desvio de obstáculo** como manobra não-bloqueante (para → recua → gira → retoma), alternando o lado.
- [x] `commands`: teclas de **modo** e **calibração IR**; em AUTO, `espaço`/`x` é parada de emergência (volta a MANUAL).
- [x] `main`: `storage::begin()` + dispatch por modo.
- [x] `pio run` compila sem erros/warnings.

**Validação:** compilação verificada (SUCCESS, sem warnings). **Calibração dos IR, ajuste de ganhos e teste físico ficam para a bancada** (sem hardware no ambiente de build).

**Fora de escopo (futuro):** visão (M4) e fusão completa sensores+visão (M5).

---

### Entrega M4 — Visão Computacional  (issue #11)
**Requisito:** o carro deve perceber e **reagir a um alvo visual** detectado pela HuskyLens (Tag/AprilTag) via I²C, aproximando-se e mantendo distância (atende RF05; base para RF06).

**Critérios de aceite:**
- [x] `vision`: leitura via I²C do **maior bloco** (alvo), com ID, posição e **erro horizontal**; `vision::begin()` chamado no setup.
- [x] Novo modo **TRACK_TARGET (RASTREIO)** no `behaviors`:
  - esterço **proporcional** ao erro horizontal;
  - **aproxima e para** a uma distância-alvo (tamanho do bloco) com segurança por ultrassom;
  - **alvo perdido → gira procurando** (janela de busca) e depois para.
- [x] `commands`: tecla **`p`** (RASTREIO) e **`i`** (info de visão: alvo/ID/erro).
- [x] `main`: `vision::begin()` no setup + dispatch por modo.
- [x] `config`: constantes de visão (ganho, aproximação, limiar de tamanho, busca).
- [x] `pio run` compila sem erros/warnings.

**Validação:** compilação verificada (SUCCESS, sem warnings). **Aprendizado das tags, ajuste de ganhos/limiar e teste físico ficam para a bancada** (sem HuskyLens no ambiente de build).

**Fora de escopo (futuro):** fusão completa sensores+visão e navegação (M5).

---

### Entrega M5 — Autonomia  (issue #13)
**Requisito:** o carro deve **navegar de forma autônoma** fundindo sensores e visão, com política de decisão por prioridade (segurança antes do objetivo) — atende RF06 e fecha o produto.

**Critérios de aceite:**
- [x] Modo **NAVIGATE** no `behaviors`: arbitrador em camadas — **Segurança (desvio) > Alvo visual (aproximar Tag) > Seguir-linha > Ocioso (girar procurando)**.
- [x] Manobra de desvio **latcheada** (conclui antes de re-arbitrar).
- [x] Refator com **núcleos reutilizáveis** (`steerByLine`, `steerByVision`, manobra de desvio) compartilhados pelos modos isolados e pelo NAVIGATE.
- [x] `commands`: tecla **`N`** (NAVEGAÇÃO) e telemetria com a **camada ativa** (`modo=NAVEGACAO(CAMADA)`).
- [x] **Boot em MANUAL** (o carro não se move ao energizar).
- [x] `config`: constante de busca (ocioso).
- [x] `pio run` compila sem erros/warnings.

**Validação:** compilação verificada (SUCCESS, sem warnings). **Sintonia de prioridades/ganhos e teste físico ficam para a bancada** (sem hardware no ambiente de build).

---

> **Roadmap concluído (M1–M5).** Evoluções futuras (ex.: PID no seguir-linha, telemetria avançada, novos comportamentos) entram como novas entregas seguindo o mesmo processo. Ver [`roadmap.md`](roadmap.md).

---

## Trilha futura — Rumo Console (M6/M7)

**Requisito (futuro, não implementar agora):** desenvolver o **Rumo Console**, software companheiro de *ground-control* que consome o stream serial real de telemetria (`[status] …` a 115200 baud) e expõe o conjunto real de comandos (CLI WASD), recriando fielmente o **design system Rumo**. Especificação completa (tokens, primitivos, 4 telas, estado, integração serial) em [`design_handoff_rumo_console/`](design_handoff_rumo_console/).

Planejamento rastreado no **épico [#28](https://github.com/rogerio-silva/autonomous-car/issues/28)** e nas issues `status:futuro` dos milestones:

- **M6 — Fundação do Console:** setup/framework (#29), tokens (#30), primitivos core (#31), primitivos de telemetria (#32), shell + navegação + branding (#33).
- **M7 — Telas & Integração:** Dashboard (#34), Serial CLI (#35), Calibração (#36), Hardware & Pinout (#37), camada de estado + simulação (#38), integração serial real + acessibilidade (#39).

> Esta trilha é **explicitamente futura**: nenhum software dela é implementado na etapa atual. O firmware permanece o foco do produto. Ver nota em [`../CLAUDE.md`](../CLAUDE.md).

---

### Entrega de apoio — Projeto CAD do chassi + bolha  (issue #42)
**Requisito:** integrar ao repositório o **projeto CAD** (modelo OpenSCAD parametrizado + documentação) do chassi e da bolha/carroceria, gerado por IA a partir de [`assembly/prompt-chassi-bolha.md`](assembly/prompt-chassi-bolha.md), e **corrigir as inconsistências** do modelo.

**Critérios de aceite:**
- [x] Arquivos em [`assembly/cad/`](assembly/cad/): `chassi-bolha.scad` + `chassi-bolha.md`, com link a partir de [`assembly/README.md`](assembly/README.md).
- [x] Inconsistências corrigidas e **validadas por render no OpenSCAD**: dimensões reais do powerbank (85×45×45); colisão entre os nichos do L298N e do pack 2S no `tray`; aviso *non-manifold* no suporte HC-SR04; parametrização dos rasgos da cinta do powerbank.

**Fora de escopo (futuro):** refino estético da bolha, divisão da casca para impressão, suportes de motor/rodas e base 2D DXF (ver seção 10 do doc CAD).

---

### Entrega de apoio — Prompt de IA para chassi + bolha  (issue #23)
**Requisito:** disponibilizar um **prompt em Markdown**, detalhado e reutilizável, para gerar via IA o **projeto do chassi** e da **bolha/carroceria**, embutindo as restrições reais do projeto e o posicionamento sensorial já definido.

**Critérios de aceite:**
- [x] Arquivo [`assembly/prompt-chassi-bolha.md`](assembly/prompt-chassi-bolha.md) com o prompt completo em PT-BR (papel, restrições fixas, componentes cotados, posicionamento sensorial, entregáveis incl. **OpenSCAD parametrizado**).
- [x] Requisito registrado aqui.
- [x] Link a partir de [`assembly/README.md`](assembly/README.md) (seção "Projeto mecânico").

**Fora de escopo:** o projeto mecânico em si (gerado externamente pela IA a partir do prompt); validação física na bancada.
