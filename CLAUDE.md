# CLAUDE.md — Diretrizes de desenvolvimento

Orientações para qualquer agente (humano ou IA) que desenvolva neste repositório. Leia antes de iniciar qualquer tarefa.

## 1. O que é este projeto

Carro autônomo **100% embarcado** em **Arduino Mega 2560**, com visão computacional pela **Gravity HuskyLens** (I²C) e sensoriamento ultrassônico + infravermelho. **Não há** software de PC, app ou interface web — entregamos apenas firmware Arduino (C++) e a integração física dos componentes.

Decisões fixas do projeto:
- **MCU:** Arduino Mega 2560
- **Tração:** 2WD (4 rodas, 2 motores DC traseiros) via driver **L298N**
- **Visão:** Gravity HuskyLens via **I²C** (SDA=20, SCL=21)
- **Build:** PlatformIO (`env:megaatmega2560`)
- **Idioma:** documentação, commits, issues e PRs em **PT-BR**

## 2. Como trabalhamos (processo obrigatório)

Para **cada solicitação** do usuário:

1. **Planejar** a entrega e **apresentar perguntas de alinhamento** antes de iniciar o deploy. Só prosseguir após confirmação.
2. **Documentar o requisito** em `docs/requirements.md` (ou doc específico em `docs/`).
3. **Criar a issue** no GitHub, associada ao **milestone** correto e às **labels** adequadas (`type:*`, `area:*`, `priority:*`).
4. Criar uma **branch dedicada** por entrega: `feat/...`, `fix/...`, `docs/...`, `chore/...`.
5. Implementar + documentar; **commits pequenos** referenciando a issue (`#N`).
6. Abrir **pull request** descritivo (fecha a issue com `Closes #N`).
7. Fazer o **merge** (squash). **Preservar a branch** após o merge — não remover (mantemos as branches como histórico de cada atualização).

Detalhes em [`docs/git-workflow.md`](docs/git-workflow.md).

## 3. Convenções de código (firmware)

- **C++** no framework Arduino. Cada subsistema é um **módulo** com par `.h`/`.cpp` em `include/` + `src/`, dentro de um `namespace`.
- **Nenhum número de pino solto no código** — tudo em [`include/config.h`](include/config.h). Constantes mágicas viram `#define`/`const`.
- `setup()` inicializa os módulos (`*::begin()`); `loop()` mantém uma **máquina de estados** clara, sem lógica de hardware embutida.
- Evite `delay()` longos em código de produção (bloqueiam a leitura de sensores/visão). No esqueleto eles são aceitáveis e marcados como provisórios.
- Comentários em **PT-BR**, objetivos. Explique o "porquê", não o óbvio.
- Mantenha o firmware **compilável** a cada merge (`pio run`).

## 4. Convenções de commit

Formato: `tipo: descrição curta no imperativo (#issue)`

Tipos: `feat`, `fix`, `docs`, `refactor`, `chore`, `test`, `hardware`.

Exemplos:
- `feat: adiciona controle PID de seguir-linha (#12)`
- `docs: documenta pinout do HC-SR04 (#3)`

## 5. Estrutura de pastas

Ver [`README.md`](README.md#-estrutura-do-repositório). Resumo:
- `include/` headers · `src/` implementação · `lib/` libs privadas
- `docs/` toda a documentação · `platformio.ini` build

## 6. Hardware — ao alterar

Qualquer mudança de pino/componente deve atualizar **simultaneamente**:
1. [`include/config.h`](include/config.h)
2. [`docs/hardware.md`](docs/hardware.md) (BOM + tabela de pinout)

## 7. Roadmap

Milestones M1–M5 em [`docs/roadmap.md`](docs/roadmap.md). Não pule etapas: cada milestone assume o anterior estável.
