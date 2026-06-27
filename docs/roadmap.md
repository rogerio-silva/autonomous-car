# Roadmap — autonomous-car

Desenvolvimento incremental em 5 milestones. Cada fase assume a anterior estável e compilável.

---

## M1 — Fundação do Projeto  ✅
**Objetivo:** base reprodutível do projeto.
- Estrutura PlatformIO, esqueleto de firmware modular.
- Documentação completa (`README`, `CLAUDE.md`, `docs/`).
- Milestones, labels e issue inicial; `.gitignore`.

**Entregável:** repositório documentado, esqueleto compilável, workflow definido.

---

## M2 — Plataforma & Locomoção  ✅
**Objetivo:** o carro se move de forma controlada.
- Diagrama de ligação L298N + seção de energia (`hardware.md`) e montagem completa (`assembly/`).
- Módulo `motors`: rampas de aceleração não-bloqueantes, trim por motor, curvas diferenciais, failsafe.
- Persistência da calibração em EEPROM (`storage`).
- CLI serial estilo WASD (`commands`) para pilotar/testar.

**Entregável:** comandos de movimento confiáveis via serial (validado por compilação; teste físico na bancada).

---

## M3 — Sensoriamento  🚧
**Objetivo:** o carro percebe o ambiente.
- HC-SR04: leitura filtrada (mediana), parada/desvio por obstáculo.
- Array IR de linha: limiares calibráveis (runtime + EEPROM), seguir-linha proporcional.
- IR de obstáculo: integrado ao desvio.
- Módulo `behaviors`: modos MANUAL / SEGUIR-LINHA / DESVIO.

**Entregável:** desvio de obstáculo e seguir-linha por sensores (validado por compilação; calibração e teste físico na bancada).

---

## M4 — Visão Computacional
**Objetivo:** integrar a HuskyLens como percepção principal.
- Conexão I²C validada, escolha de algoritmo (tracking/recognition/line/tag).
- Módulo `vision`: leitura robusta de alvos, IDs e erro horizontal.
- Estado `TRACK_TARGET` (seguir alvo visual).

**Entregável:** o carro reage a alvos detectados pela HuskyLens.

---

## M5 — Autonomia
**Objetivo:** navegação autônoma por fusão de dados.
- Fusão visão + sensores; máquina de estados completa (`NAVIGATE`).
- Política de decisão (prioridade segurança → objetivo).
- Telemetria de depuração e ajustes finais.

**Entregável:** carro navegando de forma autônoma combinando visão e sensores.

---

### Convenção de status
✅ concluído · 🚧 em andamento · ⬜ planejado

| Milestone | Status |
|-----------|--------|
| M1 | ✅ |
| M2 | ✅ |
| M3 | 🚧 |
| M4 | ⬜ |
| M5 | ⬜ |
