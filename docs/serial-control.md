# Controle Serial (M2)

No M2 o carro é pilotado pelo **Monitor Serial** (sem software externo), com uma CLI estilo **WASD**. O firmware é não-bloqueante: as teclas definem alvos e as rampas/telemetria rodam no loop.

## Como usar

1. Gravar o firmware: `pio run -t upload`
2. Abrir o monitor a **115200 baud**: `pio device monitor`
3. Enviar as teclas abaixo (configure o monitor para enviar **caractere a caractere**, sem precisar de Enter, para pilotar em tempo real).

> **Failsafe:** se nenhum comando chegar dentro de `FAILSAFE_TIMEOUT_MS` (padrão **1500 ms**) com os motores em movimento, eles param sozinhos. Para manter o carro andando por longos períodos, reenvie a tecla de movimento periodicamente. Isso protege contra queda da conexão serial.

## Tabela de comandos

| Tecla | Ação |
|-------|------|
| `w` | Frente |
| `s` | Ré |
| `a` | Girar à esquerda (no eixo) |
| `d` | Girar à direita (no eixo) |
| `espaço` ou `x` | Parar (desacelera em rampa) |
| `+` / `=` | Aumenta a velocidade (setpoint PWM) |
| `-` / `_` | Diminui a velocidade |
| `1` / `2` | Trim do motor **esquerdo** − / + |
| `3` / `4` | Trim do motor **direito** − / + |
| `k` | Salva o trim atual na **EEPROM** |
| `n` | Reseta o trim para o padrão |
| `m` | Modo **MANUAL** (pilotagem pela serial) |
| `l` | Modo **SEGUIR-LINHA** (autônomo, array IR) |
| `v` | Modo **DESVIO** (autônomo, evita obstáculos) |
| `p` | Modo **RASTREIO** (autônomo, segue alvo da HuskyLens) |
| `N` | Modo **NAVEGAÇÃO** (autônomo, funde sensores+visão) |
| `i` | Info de visão (alvo, ID, erro horizontal) |
| `c` | Mostra leituras/limiares dos IR de linha |
| `f` | Calibração: captura amostra de **fundo** |
| `g` | Calibração: captura amostra de **linha** (recalcula limiares) |
| `o` | Imprime o status atual |
| `t` | Liga/desliga a telemetria contínua |
| `h` ou `?` | Mostra a ajuda |

> **Movimento manual (`w/a/s/d`) só funciona no modo MANUAL.** Nos modos autônomos, `espaço`/`x` é **parada de emergência** e retorna a MANUAL.

## Velocidade

O setpoint começa em `MOTOR_SPEED_CRUISE` (180) e varia entre `SPEED_MIN` (60) e `MOTOR_SPEED_MAX` (255), em passos de 15. Abaixo de ~60 o motor DC tende a não vencer o atrito (não gira).

## Rampas de aceleração

As velocidades não mudam de forma abrupta: a cada `MOTOR_RAMP_INTERVAL_MS` (10 ms) o PWM atual caminha `MOTOR_RAMP_STEP` (8) em direção ao alvo. Isso suaviza partidas/paradas e protege a mecânica. Ajuste esses valores em [`config.h`](../include/config.h).

## Calibração de trim (andar reto)

Motores DC raramente são idênticos — o carro pode "puxar" para um lado. O **trim** é um fator multiplicativo por lado (intervalo `[TRIM_MIN, 1.0]`) aplicado ao PWM.

Procedimento sugerido:
1. Mande o carro à frente (`w`) numa superfície reta.
2. Se ele puxa para a **direita**, o lado esquerdo está mais forte → reduza o trim esquerdo (`1`) — ou aumente o direito (`4`).
3. Repita até andar reto.
4. Salve com `k` (persiste na EEPROM e é recarregado no próximo boot).

Use `o` para ver `trimL`/`trimR` a qualquer momento.

## Modos autônomos (M3)

| Tecla | Modo | Descrição |
|-------|------|-----------|
| `m` | MANUAL | Pilotagem manual (padrão). |
| `l` | SEGUIR-LINHA | Segue a pista pelo array IR com controle proporcional. Para se houver obstáculo muito próximo. |
| `v` | DESVIO | Anda em cruzeiro e, ao detectar obstáculo, executa a manobra **parar → recuar → girar → retomar** (alternando o lado). |
| `p` | RASTREIO | Segue um alvo da HuskyLens (Tag): esterça pelo erro horizontal, **aproxima e mantém distância** (para quando o alvo fica grande/perto). Alvo perdido → gira procurando. Use `i` para ver o alvo. |
| `N` | NAVEGAÇÃO | **Autonomia total**: arbitrador por prioridade — **Segurança (desvio) > Alvo (Tag) > Linha > Busca**. O status mostra a camada ativa, ex.: `modo=NAVEGACAO(ALVO)`. |

> **Boot em MANUAL:** ao ligar, o carro fica parado em MANUAL; acione a autonomia (`N`, `l`, `v`, `p`) por comando. `espaço`/`x` volta a MANUAL a qualquer momento.

Em qualquer modo autônomo, `espaço` ou `x` **para imediatamente** e volta para MANUAL (emergência). Ganhos e durações ficam em [`config.h`](../include/config.h): `FOLLOW_BASE_SPEED`, `FOLLOW_TURN_GAIN`, `FOLLOW_SEARCH_SPEED`, `AVOID_SPEED`, `AVOID_BACK_MS`, `AVOID_TURN_MS`.

## Calibração dos sensores IR (seguir-linha)

Os limiares linha/fundo variam com a superfície e a iluminação. Procedimento (persistente em EEPROM):
1. Posicione o array **sobre o fundo** (sem a linha) e tecle `f` (captura o fundo).
2. Posicione o array **sobre a linha** e tecle `g` (captura a linha e recalcula os limiares e a polaridade de cada sensor).
3. Tecle `c` para conferir `raw`, `limiar` e o estado `linha(E,C,D)`.
4. Tecle `k` para salvar na EEPROM (recarrega no próximo boot). `n` reseta tudo ao padrão.

A calibração detecta automaticamente a polaridade (se a linha lê mais alto ou mais baixo que o fundo) por sensor.

## Telemetria

`t` ativa impressões periódicas (500 ms) no formato:

```
[status] modo=MANUAL vel=180 pwmL=180 pwmR=176 trimL=1.00 trimR=0.98 dist=35cm
```
