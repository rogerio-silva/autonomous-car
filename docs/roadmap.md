# Roadmap — autonomous-car

Desenvolvimento incremental em 5 milestones. Cada fase assume a anterior estável e compilável.

---

## M1 — Fundação do Projeto  ✅ (em andamento)
**Objetivo:** base reprodutível do projeto.
- Estrutura PlatformIO, esqueleto de firmware modular.
- Documentação completa (`README`, `CLAUDE.md`, `docs/`).
- Milestones, labels e issue inicial; `.gitignore`.

**Entregável:** repositório documentado, esqueleto compilável, workflow definido.

---

## M2 — Plataforma & Locomoção
**Objetivo:** o carro se move de forma controlada.
- Montagem do chassi 2WD + driver L298N (diagrama elétrico, energia).
- Módulo `motors`: rampas de aceleração, calibração de offset entre motores, curvas diferenciais.
- Testes de movimento (frente/ré/curvas) e ajuste de PWM.

**Entregável:** comandos de movimento confiáveis via serial.

---

## M3 — Sensoriamento
**Objetivo:** o carro percebe o ambiente.
- HC-SR04: leitura filtrada, parada por obstáculo.
- Array IR de linha: calibração de limiar, seguir-linha básico.
- IR de obstáculo: integração.
- Estado `FOLLOW_LINE`.

**Entregável:** desvio de obstáculo e seguir-linha por sensores.

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
| M1 | 🚧 |
| M2 | ⬜ |
| M3 | ⬜ |
| M4 | ⬜ |
| M5 | ⬜ |
