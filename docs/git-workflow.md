# Fluxo de Trabalho — Git, Issues, Milestones e PRs

Processo obrigatório para **cada entrega**. Garante rastreabilidade entre requisito → issue → código → PR.

## 1. Antes de codar: planejar e alinhar

1. **Planejar** a entrega e **apresentar perguntas de alinhamento** ao usuário.
2. Só após a confirmação, **registrar o requisito** em [`requirements.md`](requirements.md).

## 2. Milestones

Representam as fases do roadmap. Já criados:

| Milestone | Tema |
|-----------|------|
| M1 - Fundação do Projeto | Estrutura, docs, build, workflow |
| M2 - Plataforma & Locomoção | Chassi, L298N, controle de motores |
| M3 - Sensoriamento | Ultrassom, IR (linha/obstáculo) |
| M4 - Visão Computacional | Integração HuskyLens |
| M5 - Autonomia | Fusão sensores+visão, navegação |

## 3. Issues

Cada entrega tem **uma issue** com:
- **Milestone** correspondente.
- **Labels** — combine `type:*` + `area:*` + `priority:*`.

### Labels disponíveis

**Tipo:** `type:feat` · `type:fix` · `type:docs` · `type:firmware` · `type:hardware` · `type:vision` · `type:infra` · `type:bug`

**Área:** `area:drivetrain` · `area:sensors` · `area:vision` · `area:autonomy` · `area:setup`

**Prioridade:** `priority:high` · `priority:medium` · `priority:low`

```bash
gh issue create \
  --title "Controle de motores 2WD" \
  --milestone "M2 - Plataforma & Locomocao" \
  --label "type:firmware,area:drivetrain,priority:high" \
  --body "..."
```

## 4. Branches

Uma branch por entrega, a partir de `main`:

```
feat/<assunto>     # nova funcionalidade
fix/<assunto>      # correção
docs/<assunto>     # documentação
chore/<assunto>    # manutenção/infra
```

```bash
git checkout main && git pull
git checkout -b feat/controle-motores
```

## 5. Commits

`tipo: descrição no imperativo (#issue)` — ver convenções em [`../CLAUDE.md`](../CLAUDE.md).

```bash
git commit -m "feat: implementa curvas diferenciais 2WD (#7)"
```

## 6. Pull Request

```bash
git push -u origin feat/controle-motores
gh pr create \
  --base main \
  --title "feat: controle de motores 2WD" \
  --body "Closes #7\n\n## Resumo\n..."
```

- Use `Closes #N` para fechar a issue automaticamente no merge.
- Descreva o que foi feito, como testar e impactos de hardware.

## 7. Merge

```bash
gh pr merge --squash --delete-branch
```

- Estratégia padrão: **squash** (histórico limpo na `main`).
- Remover a branch após o merge.

## 8. Resumo do ciclo

```
planejar+alinhar → requirements.md → issue (milestone+labels)
   → branch → commits → PR (Closes #N) → merge squash → branch removida
```
