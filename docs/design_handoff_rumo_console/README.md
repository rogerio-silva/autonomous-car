# Handoff: Rumo — Console & Design System

Pacote de handoff para desenvolver/estender o **Rumo Design System** e o **Rumo Console** em um codebase real, usando Claude Code.

> Idioma do produto: **PT-BR**. Modos e chaves de telemetria preservam a grafia do firmware (`modo=NAVEGACAO(ALVO)`, `pwmL`, `dist=35cm`).

---

## Overview

**Rumo** é a marca e o design system do projeto **`autonomous-car`** (carro autônomo embarcado em Arduino Mega 2560, visão pela HuskyLens, sensores ultrassônico + IR, arbitrador de autonomia por prioridade).

O firmware é **headless** — não há app nem UI. O **Rumo Console** é o companheiro de software *projetado*: um console de controle de solo (ground-control) que consome o stream serial real de telemetria e expõe o conjunto real de comandos. Tudo no design mapeia para algo que existe no firmware (`include/config.h`, `docs/serial-control.md`, `docs/architecture.md`, `docs/hardware.md`).

A estética é de **instrumento**: superfícies grafite, telemetria em monoespaçada, e quatro cores-sinal em semântica de trânsito (âmbar = autonomia, lima = siga, vermelho = obstáculo, ciano = visão).

---

## About the Design Files

Os arquivos referenciados aqui são **referências de design feitas em HTML/React (JSX)** — protótipos que mostram a aparência e o comportamento pretendidos, **não** código de produção para copiar diretamente.

A tarefa do desenvolvedor é **recriar estes designs no ambiente do codebase de destino** (React, Vue, SwiftUI, Flutter, etc.), usando os padrões e bibliotecas já estabelecidos ali. Se ainda não existir ambiente, escolher o framework mais adequado e implementar.

O design system completo **já vive na raiz deste repositório** e é a fonte da verdade:

| Caminho | O que é |
|---|---|
| `styles.css` | Ponto de entrada — `@import` de todos os tokens + fontes. Consumidores linkam só este arquivo. |
| `tokens/` | `colors.css`, `typography.css`, `spacing.css`, `elevation.css`, `motion.css`, `fonts.css` |
| `components/core/` | `Button`, `IconButton`, `KeyCap`, `StatusLed`, `Badge`, `Card`, `Switch` |
| `components/telemetry/` | `ModeBadge`, `TelemetryStat`, `Gauge`, `SensorBar` |
| `ui_kits/console/` | **Rumo Console** interativo (`index.html` + telas em JSX) |
| `assets/` | `rumo-mark.svg` (logo), `schematic.svg` (diagrama de ligação real) |
| `SKILL.md` | Manifesto Agent-Skill — leitura inicial para qualquer agente |
| `README.md` (raiz) | Guia de design completo (content fundamentals, visual foundations, iconografia) |

Cada componente tem um `.prompt.md` ao lado com uso e variantes — leia-os.

> **Por que não duplicamos os componentes aqui:** um compilador varre o projeto inteiro e registra cada par `Name.jsx` + `Name.d.ts`. Copiar os componentes para esta pasta criaria nomes duplicados e quebraria o design system. Por isso este handoff **aponta** para os arquivos vivos em vez de copiá-los.

---

## Fidelity

**Alta-fidelidade (hifi).** Cores, tipografia, espaçamento e interações são finais. Recrie a UI fielmente usando as bibliotecas/padrões do codebase de destino. Os valores exatos estão na seção **Design Tokens**.

---

## Como o Claude Code deve começar

1. Ler `SKILL.md` e `README.md` (raiz) — absorvem a linguagem visual e as regras.
2. Linkar/portar `styles.css` (ou os tokens) para o codebase de destino.
3. Recriar os primitivos (`components/`) como componentes nativos do framework de destino, lendo cada `.jsx` + `.d.ts` + `.prompt.md`.
4. Recriar as telas do console (`ui_kits/console/`) compondo esses primitivos.
5. Substituir o motor de telemetria simulado (`App.jsx`) pela conexão real (Web Serial API no navegador, ou bridge no backend) que lê o stream `[status] …` a 115200 baud.

---

## Screens / Views

O console tem 4 telas, uma sidebar (248px) e uma topbar (52px). Largura de design **1280×720**. Tema escuro por padrão.

### 1. Telemetria (Dashboard) — `ui_kits/console/DashboardScreen.jsx`
- **Propósito:** monitorar o carro ao vivo e trocar de modo.
- **Layout:** grid de 2 colunas `1.7fr / 1fr`, gap 16, padding 18.
- **Coluna esquerda:**
  - *Card "Modo de operação"*: 5 botões de modo (`MANUAL m`, `SEGUIR-LINHA l`, `DESVIO v`, `RASTREIO p`, `NAVEGACAO N`). O modo ativo usa `Button variant="primary"` (âmbar). Quando `NAVEGACAO`, mostra a fileira de camadas `SEGURANCA › ALVO › LINHA › BUSCA` com a ativa em âmbar.
  - *Card "Readouts"*: 4 `TelemetryStat` (Distância cm, Velocidade PWM, PWM esq, PWM dir). Distância fica `tone="alert"` se ≤ 20cm.
  - *Card "Instrumentos"* (com grid texture): `Gauge` distância (max 200, threshold 20), `Gauge` velocidade (60–255), 2 `Gauge` de trim (50–100%).
- **Coluna direita:**
  - *Card "Visão · HuskyLens"* (`live` quando RASTREIO): viewport 4:3 com grid 24px; caixa de alvo ciano posicionada por `errX`; `TelemetryStat` Tag ID + Erro X.
  - *Card "Array IR · seguir-linha"* (`live` quando SEGUIR-LINHA): `SensorBar` 3 células E/C/D.
  - *Card "Eventos"*: log monoespaçado (column-reverse), última linha destacada.

### 2. Serial CLI — `ui_kits/console/SerialScreen.jsx`
- **Propósito:** a recreação mais fiel — o monitor serial WASD real.
- **Layout:** grid `1.6fr / 1fr`.
- **Esquerda:** *Card "Monitor serial · /dev/ttyACM0 · 115200"* — terminal mono, fundo `--surface-inset`, linhas coloridas por tipo (eco `>` âmbar, `[status]` corpo, `OBSTACULO/FAILSAFE` vermelho, `OK/salvo` lima), cursor piscante.
- **Direita:** *Card "Pilotagem WASD"* — `KeyCap` em cruz (W em cima; A S D; ␣ embaixo), acende âmbar quando pressionado. *Card "Referência de comandos"* — tabela chave→ação (clicável envia a tecla).
- **Comportamento:** input de teclado real (keydown global) e cliques disparam o mesmo handler `onKey`. Movimento `w/a/s/d` só funciona em `MANUAL`; `espaço/x` é parada de emergência → `MANUAL`.

### 3. Calibração — `ui_kits/console/CalibrationScreen.jsx`
- **Propósito:** calibrar o array IR e o trim dos motores; persistir em EEPROM.
- **Layout:** grid `1.2fr / 1fr`.
- **Esquerda:** *Card "Calibração do array IR"* — `SensorBar` + 3 cartões raw/estado por sensor + botões `Capturar fundo (f)`, `Capturar linha (g)`, `Conferir (c)`. *Card "Procedimento"* — lista numerada.
- **Direita:** *Card "Calibração de trim"* — 2 linhas de trim (− / barra / +), botões `Salvar EEPROM (k)` / `Resetar (n)`. *Card "Persistência"* — `StatusLed` sincronizado + badges `v2 · 0xA5C2`.

### 4. Hardware & Pinout — `ui_kits/console/HardwareScreen.jsx`
- **Propósito:** referência de pinos (de `config.h`) e energia.
- **Layout:** grid `1.5fr / 1fr`. Esquerda: 4 cards de pinout (Motores L298N, HC-SR04, Array IR, IR obstáculo + HuskyLens) — cada pino com sinal, número (âmbar), tag de tipo (PWM/DIG/ANA/I²C) e descrição. Direita: *Card "Diagrama de montagem"* (schematic.svg em superfície branca) + *Card "Energia · 2S Li-ion"*.

---

## Interactions & Behavior

- **Conexão:** overlay "Conectar ao carro" (scrim + blur 2px) até `Conectar`; então o stream começa.
- **Tick de telemetria:** a cada 700ms o estado avança conforme o modo (random walk de distância, rampas de PWM, sensores, alvo de visão, arbitragem de camada em NAVEGACAO) e anexa uma linha `[status] …`. Eventos de obstáculo (`dist ≤ 20`) anexam linha de alerta vermelha.
- **Rampas:** PWM caminha em passos em direção ao setpoint (espelha `MOTOR_RAMP_STEP`/`INTERVAL_MS`) — nunca salta. Easing mecânico `--ease-mech`.
- **Estados de hover/press:** hover clareia 1 passo de superfície ou vai a `--accent-hover`; press primário usa `--accent-press`; `KeyCap` ativo afunda (`translateY(1px)` + borda inferior fina) e acende âmbar; `Button variant="danger"` inverte para preenchimento vermelho no hover.
- **Pulsos "live":** LED pisca, cursor pisca, scanline — todos respeitam `prefers-reduced-motion`.
- **Reduced motion:** `--pulse-period: 0s` desliga os loops.

---

## State Management

Estado central (em `App.jsx`), a ser replicado:

- `connected: boolean`
- `screen: 'dashboard' | 'serial' | 'calibration' | 'hardware'`
- `t` (telemetria): `mode` (`MANUAL|SEGUIR-LINHA|DESVIO|RASTREIO|NAVEGACAO`), `layer` (`SEGURANCA|ALVO|LINHA|BUSCA`), `speed` (PWM 60–255), `dist` (cm), `pwmL`, `pwmR`, `trimL`, `trimR` (0.5–1.0), `sensors[3]` (`{id, raw 0–1023, line}`), `tag` (id|null), `errX`, `telemetryOn`, `irThreshold` (500), `eepromSaved`, `serial[]`, `log[]`.
- `heldKey`: tecla destacada momentaneamente.

Transições disparadas por `onKey(char)` (teclado ou clique), espelhando a CLI: movimento, velocidade `+/-`, trim `1/2/3/4`, salvar/resetar `k/n`, modos `m/l/v/p/N`, info `o/t/i/c`, calibração `f/g`, ajuda `h`. **Em produção, este estado vem do stream serial real** — não simulado.

---

## Design Tokens

Valores exatos (fonte: `tokens/`). Use as CSS custom properties; aqui ficam os literais.

### Cores — sinais
- Âmbar (autonomia/live/ação primária): `#FFB020` (hover `#FFC558`, press `#E08A00`)
- Lima (siga/linha/OK): `#A6E22E`
- Ciano (visão/dados): `#38D6CE`
- Vermelho (obstáculo/emergência): `#FF5247`

### Cores — grafite (escuro → claro)
`950 #0B0D0E` · `900 #101315` · `850 #15191B` · `800 #1B2023` · `750 #1F2629` · `700 #242B2F` · `600 #323A3F` · `500 #475157` · `400 #69757C` · `300 #8C979D` · `200 #B6BFC4` · `100 #D9DEE1` · `050 #EEF1F2`

### Semânticos (tema escuro padrão)
- `surface-app #0B0D0E` · `surface-panel #101315` · `surface-raised #15191B` · `surface-inset #08090A` · `surface-hover #1B2023`
- `border-subtle rgba(255,255,255,.06)` · `border-default .10` · `border-strong .18`
- `text-strong #F4F7F8` · `text-body #D9DEE1` · `text-muted #8C979D` · `text-faint #69757C` · `text-on-accent #0B0D0E`
- Status: ok=lima · warn=âmbar · alert=vermelho · info=ciano · idle=#69757C
- Tema claro: `[data-theme="light"]` (ver `tokens/colors.css`)

### Tipografia
- Display/UI: **Space Grotesk** (700/600), tracking `-0.02em`
- Corpo: **IBM Plex Sans** (400/500/600), 15px / 1.45
- Dados/telemetria: **IBM Plex Mono** — *todos* os números, pinos, labels, serial; `font-feature-settings: "tnum" 1, "zero" 1`
- Escala (px): 2xs 11 · xs 12 · sm 13 · base 15 · md 16 · lg 18 · xl 22 · 2xl 28 · 3xl 36 · 4xl 48 · 5xl 64 · 6xl 84
- Micro-label: mono, uppercase, tracking `0.12em`

### Espaçamento (base 4px)
`1 4` · `2 8` · `3 12` · `4 16` · `5 20` · `6 24` · `8 32` · `10 40` · `12 48` · `16 64` · `20 80` · `24 96`
Rails: sidebar 248 · inspector 320 · topbar 52 · content-max 1200

### Radii (pequenos/mecânicos)
xs 2 · sm 4 · md 6 · lg 10 · xl 14 · 2xl 20 · full 999

### Sombras & glows
- `shadow-sm/md/lg/xl` (drops ambientes em preto)
- `elev-panel` = highlight de topo + shadow-md; `elev-inset` para campos
- `glow-amber/lime/cyan/red` — **só para estados LIVE** (LED aceso, modo engajado)

### Motion
- Durações: instant 80 · fast 140 · base 220 · slow 360 (ms)
- Easings: `ease-standard`, `ease-out`, `ease-in`, `ease-mech` (mecânico in-out)
- `pulse-period 1.6s` (0s sob reduced-motion)

---

## Assets

- `assets/rumo-mark.svg` — logo: ponteiro de proa (âmbar) + ponto-sensor (lima) em tile grafite. (Marca criada para este sistema; o firmware não tinha logo.)
- `assets/schematic.svg` — diagrama de ligação **real** do repo (`docs/assembly/`). Exibir sobre fundo branco.
- **Ícones: [Lucide](https://lucide.dev)** via CDN — **substituição** (o firmware não traz ícones). Trocar pelo set do codebase de destino se houver.
- **Fontes:** Space Grotesk, IBM Plex Sans/Mono via Google Fonts CDN (`tokens/fonts.css`). Para self-host, baixar `.woff2` para `assets/fonts/` e trocar o `@import` por `@font-face`. As fontes são as **reais**, não substitutas.
- **Status nunca é ícone** — é LED colorido (`StatusLed`) ou badge-sinal, em semântica de trânsito.

---

## Files (referências no repositório)

- Telas: `ui_kits/console/{index.html, App.jsx, Sidebar.jsx, Topbar.jsx, DashboardScreen.jsx, SerialScreen.jsx, CalibrationScreen.jsx, HardwareScreen.jsx}`
- Primitivos: `components/core/*`, `components/telemetry/*` (cada um com `.jsx`, `.d.ts`, `.prompt.md`)
- Tokens: `tokens/*.css` + `styles.css`
- Specimens visuais: `cards/*.html`
- Guia: `README.md` (raiz), `SKILL.md`

### Fonte do firmware (contexto de domínio)
- GitHub: `rogerio-silva/autonomous-car` — <https://github.com/rogerio-silva/autonomous-car>
- Pinos/constantes: `include/config.h` · CLI/telemetria: `docs/serial-control.md` · arbitrador: `docs/architecture.md` · BOM/pinout: `docs/hardware.md`
