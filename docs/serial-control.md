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
| `o` | Imprime o status atual |
| `t` | Liga/desliga a telemetria contínua |
| `h` ou `?` | Mostra a ajuda |

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

## Telemetria

`t` ativa impressões periódicas (500 ms) no formato:

```
[status] vel=180 pwmL=180 pwmR=176 trimL=1.00 trimR=0.98 mov=sim
```
