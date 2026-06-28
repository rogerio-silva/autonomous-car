# Prompt de IA — projeto de chassi + bolha (carroceria)

Prompt reutilizável para gerar, via IA (ChatGPT ou equivalente), o **projeto do chassi**
e da **bolha/carroceria** do carro autônomo deste repositório.

**Como usar:** confirme as 3 medidas marcadas como _"a confirmar"_ na tabela de
componentes (powerbank, motor/roda e roda dianteira) e cole o bloco abaixo no chat de IA.
O prompt já embute as restrições reais do projeto e os requisitos de posicionamento
sensorial definidos em [`README.md`](README.md) e [`../hardware.md`](../hardware.md).

---

````markdown
# Prompt — Projeto de chassi + bolha (carroceria) para carro autônomo Arduino

## Papel
Você é um **engenheiro de produto/mecatrônica** especializado em projeto de chassis para robótica móvel e em modelagem para **impressão 3D (FDM)** e **corte a laser (MDF/acrílico)**. Gere um projeto completo, fabricável e cotado em milímetros.

## Objetivo
Projetar **dois componentes** para um carro autônomo didático:
1. **Chassi** (placa estrutural + suportes) que aloje toda a eletrônica e fixe os sensores nas posições corretas.
2. **Bolha / carroceria** (casca superior removível, estilo "bubble") que cubra a eletrônica, deixando **aberturas/janelas** para os sensores funcionarem, com visual agradável.

O carro é **100% embarcado** (sem PC). Tração **2WD traseira**. Priorize **simplicidade de montagem, acesso à eletrônica e posicionamento sensorial correto**.

## Restrições fixas do projeto (não alterar)
- **Tração:** 2WD — 2 motores DC traseiros (TT gearmotor amarelo) + 2 rodas dianteiras livres (ou 1 roda boba/caster).
- **Controlador:** Arduino **Mega 2560** (placa grande).
- **Driver de motor:** módulo **L298N** (com dissipador alto).
- **Visão:** câmera **Gravity HuskyLens** (frontal, elevada, visão desobstruída).
- **Sensores:** 1× ultrassônico **HC-SR04** frontal; 1× **IR de obstáculo** frontal; **array de 3× IR de linha** embaixo, à frente, virados para o chão.
- **Energia:** pack **2S 18650** (motores) + **powerbank** (lógica), com **chave geral** acessível.
- **Idioma das anotações:** PT-BR.

## Componentes a alojar (com dimensões aproximadas — confirme e ajuste folgas)
| Componente | Dimensões (mm) | Observações de montagem |
|---|---|---|
| Arduino Mega 2560 | 101,5 × 53,3 (furos padrão Mega) | parafusos M3; acesso ao USB e ao barrel jack na lateral |
| Módulo L298N | ~43 × 43 × 27 (alt. c/ dissipador) | 4 furos M3; bornes e dissipador para cima/ventilados |
| HuskyLens | ~52 × 44,5 (PCB) | em suporte/torre frontal elevada; tela visível; lente livre |
| HC-SR04 | 45 × 20 × 15 (2 cilindros Ø16) | berço frontal; "olhos" passando pela carroceria |
| IR obstáculo | ~30 × 14 | frontal, regulável (trimpot acessível) |
| IR de linha (3×) | módulo ~10 × 48 (sensor TCRT5000) | embaixo, rente ao chão **3–10 mm**, espaçados na largura |
| Motor DC | corpo 70 × 32 × 22 | 2 atrás; eixo de saída na face de 32 mm |
| Roda + pneu (motriz) | Ø66 (raio 33) × 25 de largura | 2 traseiras motrizes |
| Roda dianteira _(a confirmar: 2 livres ou 1 caster)_ | livre/caster | manter o carro nivelado |
| Pack 2S 18650 | ~2×(18 × 65) + suporte | baixo e centrado (peso/CG) |
| Powerbank | 85 × 45 × 45 | acesso ao botão/USB; baixo e centrado |
| Chave geral / fusível | — | acessível por fora |

## Requisitos de posicionamento sensorial (críticos)
- **HC-SR04 e IR de obstáculo:** frontais, apontando reto para frente, **feixe livre** (nada do chassi/bolha na frente). Altura ~3–8 cm do chão.
- **Array IR de linha:** **na frente das rodas motrizes**, virado para baixo, **3–10 mm** do chão; os 3 sensores espalhados na largura de modo que, na linha reta, só o **central** veja a linha. Prever **regulagem de altura** (rasgos/oblongos).
- **HuskyLens:** **elevada e à frente**, olhando para frente (nível, com opção de leve inclinação para baixo p/ visão de linha). Evitar reflexo/contraluz; lente nunca obstruída pela bolha.
- **Peso (baterias):** baixo e centrado, ligeiramente à frente do eixo traseiro, sem empinar.
- **GND/cabos:** prever canaletas/passagens de cabo e separação entre potência e sinal.

## Requisitos do chassi
- Placa base (sugerir material: **acrílico 3 mm**, **MDF 3 mm** ou **PETG/PLA impresso**) com **furação cotada** para todos os módulos.
- Suportes/torres impressas para HuskyLens e berço do HC-SR04.
- **Modularidade:** sensores em peças separadas, com **rasgos oblongos** para ajuste fino.
- Vão livre (ground clearance) compatível com o IR de linha.
- Acesso ao **USB do Mega** e ao **botão do powerbank** sem desmontar.

## Requisitos da bolha / carroceria
- Casca superior **removível** (encaixe por clipes/parafusos), cobrindo a eletrônica.
- **Aberturas funcionais:** janela frontal para a lente da HuskyLens, dois furos para os "olhos" do HC-SR04, fresta para o IR de obstáculo, recortes de ventilação sobre o L298N, e acesso à chave geral.
- Estilo "bubble"/aerodinâmico, mas **fabricável em FDM** (pensar em ângulos sem suporte ou dividir em partes).
- Folga interna (offset) para cabos e conectores.

## Entregáveis (responda nesta ordem)
1. **Dimensões gerais** finais do carro (C × L × A) e justificativa do CG.
2. **Layout em vista de cima** (ASCII art cotado) e **vista lateral** com alturas dos sensores.
3. **Lista de peças** do chassi + bolha (nome, material, processo, qtd).
4. **Desenho/croqui cotado** de cada peça (descreva furação, rasgos, em mm). Se possível, ASCII/diagrama.
5. **Estratégia de fabricação:** o que imprimir em 3D vs. cortar a laser; orientação de impressão e necessidade de suporte.
6. **Passo a passo de montagem** (ordem de fixação, parafusos M3/M2, espaçadores).
7. **Geração de modelo:** forneça **código OpenSCAD parametrizado** (variáveis no topo: comprimento, largura, espessura, posições dos furos) para o chassi e para a bolha, pronto para renderizar/exportar STL. Onde a geometria for complexa, explique como ajustar os parâmetros.
8. **Lista de melhorias/alternativas** e pontos de atenção (CG, vibração, folgas).

## Formato da resposta
- PT-BR, técnico e objetivo, tudo em **milímetros**.
- Use tabelas para dimensões/furação e blocos de código para OpenSCAD.
- Antes de gerar, **liste as suposições** que fez sobre dimensões não fornecidas e peça confirmação apenas se algo for crítico; caso contrário, adote valores razoáveis e siga.
````

---

## Antes de usar — medidas

Medidas reais já preenchidas na tabela de componentes:

- **Powerbank:** 85 × 45 × 45 mm.
- **Motor DC:** corpo 70 × 32 × 22 mm.
- **Roda + pneu (motriz):** Ø66 mm (raio 33) × 25 mm de largura.

Ainda **a confirmar** (único item em aberto):

- **Roda dianteira** — 2 rodas livres ou 1 roda boba (caster)?

## Por que pedimos OpenSCAD

O prompt exige **código OpenSCAD parametrizado** para que o resultado seja
imediatamente renderizável e exportável como **STL** (impressão 3D), sem depender
de imagens. Basta ajustar as variáveis do topo do script (dimensões e furação).

## Referências no repositório

- Posicionamento físico e matriz de ligação: [`README.md`](README.md)
- BOM + pinout + energia: [`../hardware.md`](../hardware.md)
- Esquema elétrico: [`schematic.svg`](schematic.svg)
