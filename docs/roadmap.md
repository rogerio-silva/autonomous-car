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

## M3 — Sensoriamento  ✅
**Objetivo:** o carro percebe o ambiente.
- HC-SR04: leitura filtrada (mediana), parada/desvio por obstáculo.
- Array IR de linha: limiares calibráveis (runtime + EEPROM), seguir-linha proporcional.
- IR de obstáculo: integrado ao desvio.
- Módulo `behaviors`: modos MANUAL / SEGUIR-LINHA / DESVIO.

**Entregável:** desvio de obstáculo e seguir-linha por sensores (validado por compilação; calibração e teste físico na bancada).

---

## M4 — Visão Computacional  ✅
**Objetivo:** integrar a HuskyLens como percepção principal.
- Conexão I²C; algoritmo **Tag (AprilTag)** como primário.
- Módulo `vision`: leitura do maior bloco, ID e erro horizontal.
- Modo **RASTREIO** (`TRACK_TARGET`): esterça pelo erro, aproxima/mantém distância, busca ao perder o alvo.

**Entregável:** o carro reage a alvos detectados pela HuskyLens (validado por compilação; aprendizado de tags e teste físico na bancada).

---

## M5 — Autonomia  ✅
**Objetivo:** navegação autônoma por fusão de dados.
- Modo `NAVIGATE`: arbitrador por prioridade (segurança → alvo → linha → busca).
- Manobra de desvio latcheada; núcleos de controle reutilizados dos modos isolados.
- Telemetria da camada ativa; boot em MANUAL.

**Entregável:** carro navegando de forma autônoma combinando visão e sensores (validado por compilação; sintonia e teste físico na bancada).

---

### Convenção de status
✅ concluído · 🚧 em andamento · ⬜ planejado

| Milestone | Status |
|-----------|--------|
| M1 | ✅ |
| M2 | ✅ |
| M3 | ✅ |
| M4 | ✅ |
| M5 | ✅ |
