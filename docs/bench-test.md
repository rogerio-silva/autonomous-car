# Teste de bancada — autonomous-car

Guia incremental e **seguro** para validar o carro fisicamente. Cada etapa valida um milestone. Marque os itens e anote observações na [tabela de registro](#9-registro-de-resultados).

> **Regra de ouro de segurança:** na **primeira energização e em todo teste de motor**, mantenha as **rodas suspensas** (chassi apoiado sobre uma caixa/suporte). Só coloque o carro no chão depois que MANUAL e o failsafe estiverem confirmados.

---

## 1. Pré-requisitos

- Montagem concluída conforme [`assembly/README.md`](assembly/README.md) (todas as conexões conferidas contra `include/config.h`).
- PlatformIO Core instalado (já configurado neste ambiente: `pio --version`).
- Cabo USB ligando o **Mega 2560** ao computador.
- Bateria dos motores (separada) com **GND comum** ao Arduino; **chave** acessível.

## 2. Permissão de porta serial (Linux)

No Linux a porta do Arduino (`/dev/ttyUSB*` ou `/dev/ttyACM*`) exige que o usuário esteja no grupo `dialout`. Verifique e, se preciso, adicione (uma vez):

```bash
id -nG | tr ' ' '\n' | grep dialout || echo "FALTA dialout"
sudo usermod -aG dialout "$USER"      # depois faça logout/login (ou reinicie a sessão)
```

Confirme a porta após conectar a placa:

```bash
pio device list        # deve aparecer um /dev/ttyUSB0 ou /dev/ttyACM0 com Hardware ID
```

## 3. Gravar e monitorar

```bash
pio run                 # compila (sanidade)
pio run -t upload       # grava no Mega (detecta a porta automaticamente)
pio device monitor      # abre o monitor serial a 115200 baud
```

No monitor, configure o envio **caractere a caractere** (sem precisar de Enter) para pilotar em tempo real. Ao abrir, você deve ver o cabeçalho de ajuda, o `[status]` e:

```
[OK] autonomous-car pronto (M5, modo MANUAL). Config EEPROM: padrao
```

> Referência de todos os comandos: [`serial-control.md`](serial-control.md). Tecle `h` a qualquer momento para a ajuda.

---

## Etapa A — Montagem e energização

1. [ ] Conferir todas as ligações contra [`assembly/README.md`](assembly/README.md) e `include/config.h`.
2. [ ] **Rodas suspensas.**
3. [ ] Ligar o Arduino (USB). Gravar o firmware (seção 3).
4. [ ] Ligar a **chave** da bateria dos motores.
5. [ ] No monitor: ver a mensagem de boot e o `[status]`.

✅ **Esperado:** boot em **MANUAL**, sem os motores girarem sozinhos.

---

## Etapa 1 — Motores (M2)  · rodas no ar

> Modo MANUAL. Velocidade inicial = 180. Lembre: o **failsafe** para os motores se nenhuma tecla chegar em ~1,5 s — é normal precisar repetir a tecla para manter o giro.

1. [ ] `w` → ambas as rodas giram **para frente**.
2. [ ] `s` → giram em **ré**.
3. [ ] `a` / `d` → giro no eixo (lados opostos) à **esquerda** / **direita**.
4. [ ] `espaço` ou `x` → **param** (desacelerando em rampa).
5. [ ] **Rampa:** ao mandar `w`, a velocidade **sobe suave** (não dá tranco).
6. [ ] **Failsafe:** mande `w` uma vez e aguarde — os motores **param sozinhos** em ~1,5 s.
7. [ ] `+` / `-` mudam a velocidade (veja `velocidade=` no monitor).
8. [ ] **Sentido correto?** Se uma roda gira ao contrário, inverta os fios **OUT** daquele motor no L298N (ou IN1/IN2 ↔).

### Calibração de trim (andar reto)
9. [ ] Coloque o carro no chão (superfície reta) e mande `w`.
10. [ ] Se puxa para um lado, ajuste o trim: `1`/`2` (motor esq.) e `3`/`4` (motor dir.). Reduza o lado mais forte.
11. [ ] `o` mostra `trimL`/`trimR`. Quando andar reto, `k` **salva na EEPROM**.
12. [ ] Reinicie a placa: o `[status]` deve mostrar `Config EEPROM: carregada` e os trims preservados.

✅ **Esperado:** locomoção controlada, rampas suaves, failsafe ativo, trim persistido.

---

## Etapa 2 — Sensores (M3)

### Distância (HC-SR04)
1. [ ] `t` liga a telemetria contínua; aproxime/afaste a mão à frente e veja `dist=` mudar.
2. [ ] Cubra de muito perto (< 20 cm) — `dist` deve cair para a faixa de parada.

### IR de linha (calibração)
3. [ ] Posicione o array **sobre o fundo** (sem a linha) e tecle `f`.
4. [ ] Posicione **sobre a linha** e tecle `g` (recalcula limiares + polaridade).
5. [ ] `c` mostra `raw`, `limiar` e `linha(E,C,D)`. Sobre a linha, os sensores certos devem indicar `1`.
6. [ ] `k` salva a calibração na EEPROM.

### IR de obstáculo
7. [ ] Aproxime um objeto do sensor IR frontal; em `c`/telemetria o estado deve refletir a detecção (e `obstacleAhead` passa a valer no DESVIO/NAVEGAÇÃO).

✅ **Esperado:** distância coerente, IR de linha calibrado e persistido, IR de obstáculo respondendo.

---

## Etapa 3 — Comportamentos por sensores (M3)

> Rodas no chão, área livre. `espaço`/`x` é a **parada de emergência** (volta a MANUAL).

### DESVIO
1. [ ] `v` entra no modo DESVIO. O carro anda em cruzeiro.
2. [ ] Coloque um obstáculo à frente — ele deve **parar → recuar → girar → retomar**, alternando o lado a cada obstáculo.

### SEGUIR-LINHA
3. [ ] Sobre uma pista (linha contrastante), `l` entra no modo SEGUIR-LINHA.
4. [ ] O carro deve seguir a linha; em curvas, esterça proporcional ao erro. Ao perder a linha, gira procurando.
5. [ ] Ajuste fino (se necessário) em `include/config.h`: `FOLLOW_BASE_SPEED`, `FOLLOW_TURN_GAIN`, `FOLLOW_SEARCH_SPEED` (recompilar/gravar).

✅ **Esperado:** desvio confiável e seguir-linha estável.

---

## Etapa 4 — Visão / HuskyLens (M4)

1. [ ] Na HuskyLens: *Settings → Protocol Type → **I²C***.
2. [ ] Selecionar o algoritmo **Tag Recognition** e **aprender** uma tag (atribuir um ID).
3. [ ] No monitor, `i` mostra `[visao] alvo=sim id=.. x=.. h=.. erroX=..` quando a tag está no quadro.
4. [ ] `p` entra no modo RASTREIO: o carro **esterça** em direção à tag e **se aproxima**, parando quando ela fica grande (perto). Ao sumir, **gira procurando**.
5. [ ] Ajuste fino em `config.h`: `VISION_TURN_GAIN`, `VISION_APPROACH_SPEED`, `VISION_TARGET_HEIGHT`, `VISION_SEARCH_*`.

> Se `i` sempre mostra `alvo=nao`: confira fiação I²C (R→20/SDA, T→21/SCL), o *Protocol Type: I²C* e se a tag foi aprendida. O boot avisa se a HuskyLens não respondeu na I²C.

✅ **Esperado:** detecção da tag e rastreio com aproximação.

---

## Etapa 5 — Navegação autônoma (M5)

1. [ ] `N` entra no modo NAVEGAÇÃO. `t` deixa a telemetria ligada.
2. [ ] Observe a **camada ativa** no `[status]`: `modo=NAVEGACAO(CAMADA)`:
   - `SEGURANCA` quando desvia de obstáculo (tem prioridade máxima);
   - `ALVO` quando segue uma tag;
   - `LINHA` quando segue a pista;
   - `BUSCA` quando não há nada e gira procurando.
3. [ ] Cenário combinado: com linha + uma tag + um obstáculo, verifique a **ordem de prioridade** (segurança interrompe tudo; alvo tem precedência sobre a linha).

✅ **Esperado:** navegação coerente com a política de prioridade.

---

## 9. Registro de resultados

Preencha com ✅ OK · ⚠️ parcial · ❌ falhou.

| Etapa | Item | Status | Observações |
|------|------|:-----:|-------------|
| A | Boot em MANUAL, sem mover |  |  |
| 1 | Frente/ré/curvas |  |  |
| 1 | Rampa suave |  |  |
| 1 | Failsafe (~1,5 s) |  |  |
| 1 | Velocidade `+`/`-` |  |  |
| 1 | Trim calibrado e salvo |  |  |
| 2 | Distância (HC-SR04) |  |  |
| 2 | IR de linha calibrado |  |  |
| 2 | IR de obstáculo |  |  |
| 3 | DESVIO |  |  |
| 3 | SEGUIR-LINHA |  |  |
| 4 | HuskyLens detecta tag |  |  |
| 4 | RASTREIO (aproxima) |  |  |
| 5 | NAVEGAÇÃO (prioridades) |  |  |

---

## 10. Troubleshooting

| Sintoma | Provável causa / ação |
|---------|-----------------------|
| Porta não aparece em `pio device list` | Cabo/driver USB; tente outra porta/cabo; confira `dmesg`. |
| "Permission denied" na porta | Faltou o grupo `dialout` (seção 2); refaça login. |
| `upload` falha | Porta errada/ocupada (feche o monitor); pressione reset; confirme o board `megaatmega2560`. |
| Motor gira ao contrário | Inverta OUT do motor no L298N (ou IN1/IN2). |
| Carro puxa para um lado | Calibre o trim (`1`–`4`) e salve (`k`). |
| Roda não gira em baixa | Aumente a velocidade (`+`); abaixo de ~60 o motor trava. |
| Reinicia ao acionar motores | Alimentação: não use o 5V do Arduino para os motores; bateria separada + GND comum. |
| HuskyLens `alvo=nao` sempre | *Protocol Type: I²C*, fiação R→SDA(20)/T→SCL(21), tag aprendida. |
| IR de linha invertido | Refaça a calibração `f`/`g` (a polaridade é detectada automaticamente). |

---

> Dúvidas de comandos: [`serial-control.md`](serial-control.md) · pinos: [`hardware.md`](hardware.md) / [`assembly/README.md`](assembly/README.md) · parâmetros: [`../include/config.h`](../include/config.h).
