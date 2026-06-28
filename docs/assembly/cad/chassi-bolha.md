# Projeto de chassi + bolha para carro autônomo Arduino Mega 2560 — v2

## Arquivos desta versão
- `chassi-bolha.scad`
- `chassi-bolha.md`

---

## 0. Revisão na integração ao repositório

O arquivo gerado pela IA foi **revisado e corrigido** ao ser integrado ao repositório.
Correções aplicadas (validadas renderizando o `tray`/`assembly` no OpenSCAD):

1. **Dimensões reais do powerbank** — `85 × 45 × 45 mm` (estava `92 × 52 × 46`, divergente do componente medido).
2. **Colisão entre nichos eliminada** — na geração original, o nicho do **L298N** avançava ~13 mm sobre o nicho do **pack 2S** no canto dianteiro direito (sobreposição de sólidos no `tray`). O layout foi refeito:
   - **baterias (powerbank + pack 2S) na traseira**, sobre o eixo de tração — concentra a massa nas rodas motrizes (melhor tração/estabilidade, alinhado ao requisito do prompt);
   - **Arduino Mega + L298N à frente**.
   Os quatro nichos ficam agora **sem sobreposição** e contidos no `tray`.
3. **Non-manifold do suporte HC-SR04** — os rasgos de fixação tocavam exatamente a borda da placa (face coincidente, aviso *"not a valid 2-manifold"*). Foram recuados (`±20 mm`); `hc_sr04` e `assembly` passam a renderizar limpos.
4. **Rasgos da cinta do powerbank** — parametrizados pela posição do nicho (`pbank_cx/cy`), antes fixos em coordenadas da posição antiga.

> As correções de estilo da bolha (alinhamento fino da janela da HuskyLens, divisão da casca em partes) seguem como refinamento futuro — ver seção 10.

---

## 1. O que foi corrigido na versão v2 (pela IA)

### 1.1 Correção do `tray`
O `tray` da versão anterior apresentava dois problemas geométricos principais:

1. **Footprint menor que o conjunto de nichos**, causando regiões onde as paredes dos encaixes ultrapassavam a área útil da base do próprio `tray`.
2. **Nicho do pack 2S excessivamente próximo à borda**, o que gerava áreas vazadas e desalinhamento visual/estrutural.

### Solução adotada
- O `tray` passou a ter uma **placa-base própria de 241 × 151 × 2,4 mm**, compatível com a placa estrutural.
- Todos os nichos foram reposicionados para ficarem **inteiramente contidos** nessa área.
- O nicho do **pack 2S** foi deslocado para `cx = 198 mm`, `cy = 44 mm`.
- O `tray` agora exporta como peça independente com **origem consistente**, apoiando corretamente sobre a placa base no `assembly`.

---

## 2. Conceito da nova bolha

A nova `bubble` deixou de ser uma meia elipsoide genérica e passou a ter linguagem de **sedã premium**, inspirada nas proporções de um **Mercedes C300**, sem copiar detalhamento automotivo literal.

### Características adotadas
- **capô longo e baixo**;
- **cabine recuada**;
- **linha de teto arqueada**;
- **traseira curta e limpa**;
- **grade frontal retangular arredondada**;
- **janela frontal da HuskyLens integrada como “para-brisa técnico”**;
- **recortes laterais tipo janela** para melhorar proporção visual;
- **alívios de paralama** na região das rodas.

### Compatibilidade funcional mantida
- abertura frontal da **HuskyLens**;
- dois furos do **HC-SR04**;
- fresta frontal do **IR de obstáculo**;
- ventilação superior do **L298N**;
- acesso lateral ao **USB/barrel do Mega**;
- acesso lateral ao **powerbank**;
- acesso lateral à **chave geral**;
- fixação por **6 pontos M3** alinhados à base.

---

## 3. Dimensões principais

## 3.1 Base estrutural
| Item | Valor |
|---|---:|
| Comprimento | 245 mm |
| Largura | 155 mm |
| Espessura | 3 mm |
| Raio dos cantos | 12 mm |

## 3.2 Tray corrigido
| Item | Valor |
|---|---:|
| Comprimento | 241 mm |
| Largura | 151 mm |
| Espessura da placa do tray | 2,4 mm |
| Altura dos nichos baixos | 10 mm |
| Altura dos nichos altos | 14 mm |
| Espessura de parede típica | 2,4 mm |
| Folga interna nominal | 1,2 mm |

## 3.3 Bolha / carroceria
| Item | Valor aproximado |
|---|---:|
| Comprimento externo total | 254 mm |
| Largura máxima externa | ~132 mm na parte superior + flange 159 mm |
| Altura máxima da casca | ~88 mm acima da flange |
| Espessura da parede | 2,0 mm |
| Espessura da flange inferior | 3,0 mm |

> Observação: a bolha é gerada por seções suavizadas (`body_sections`) e, portanto, a largura e altura variam continuamente ao longo do comprimento.

---

## 4. Layout interno revisado

| Componente | Centro X | Centro Y | Envelope reservado |
|---|---:|---:|---:|
| Powerbank | 58 | -26 | 85 × 45 × 45 |
| Pack 2S | 58 | 26 | 82 × 46 × 24 |
| Arduino Mega | 175 | -40 | 107 × 58 |
| L298N | 160 | 40 | 50 × 50 × 27 |

### Critério de layout (revisado)
- **Powerbank + Pack 2S**: na **traseira**, lado a lado sobre o eixo de tração — as duas baterias pesadas ficam sobre as rodas motrizes (tração/estabilidade).
- **Arduino Mega**: setor dianteiro esquerdo, com acesso lateral ao USB.
- **L298N**: setor dianteiro direito, com ventilação superior.
- Os quatro nichos ficam **sem sobreposição** e contidos no `tray` (verificado por render).

---

## 5. Peças exportáveis no OpenSCAD

Defina a variável:

```scad
part = "base";
```

Valores disponíveis:

- `"base"`
- `"tray"`
- `"husky"`
- `"hc_sr04"`
- `"line_ir"`
- `"bubble"`
- `"assembly"`

---

## 6. Observações de modelagem

## 6.1 Tray
O `tray` foi redesenhado como uma peça composta por:
- placa-base integral;
- quatro nichos com fundo e paredes;
- batentes cilíndricos de retenção;
- canaletas para organização de cabos.

### Aberturas dos nichos
| Nicho | Abertura |
|---|---|
| Powerbank | lateral esquerda |
| Mega | lateral esquerda |
| L298N | sem abertura lateral |
| Pack 2S | frontal |

---

## 6.2 Bolha
A `bubble` foi construída pela união/hull de seções elipsoidais ao longo do eixo longitudinal.

### Seções principais (`body_sections`)
Cada seção tem o formato:

```text
[x, largura, altura, z_centro]
```

Isso permite ajustar facilmente:
- volume do capô;
- altura do teto;
- inclinação do para-brisa;
- volume traseiro.

### Ajuste fino de estilo
Se quiser deixar a carroceria:
- **mais esportiva**: reduza alturas nas seções intermediárias e encurte a traseira;
- **mais alta/interna**: aumente os valores de altura (`h`) e `z_centro`;
- **mais larga**: aumente as larguras (`w`) das seções centrais;
- **mais próxima de hatch**: aumente a altura da traseira e reduza o volume do capô.

---

## 7. Posições e funções das aberturas da bolha

| Função | Implementação |
|---|---|
| Janela HuskyLens | abertura frontal integrada como para-brisa |
| HC-SR04 | dois furos frontais embutidos na grade |
| IR de obstáculo | fresta frontal inferior |
| Ventilação L298N | rasgos superiores sobre a região do driver |
| Mega USB | janela lateral esquerda |
| Powerbank | janela lateral esquerda |
| Chave geral | janela lateral direita |
| Alívio das rodas | recortes cilíndricos laterais |

---

## 8. Estratégia de fabricação recomendada

## 8.1 Corte a laser
- **Placa base**: MDF 3 mm ou acrílico 3 mm.

## 8.2 Impressão 3D FDM
- **Tray**: PETG recomendado.
- **Bolha**: PETG ou PLA+.
- **Suportes de sensores**: PETG.

### Orientação sugerida
| Peça | Orientação |
|---|---|
| Tray | deitado, fundo sobre a mesa |
| Bolha | flange apoiada na mesa |
| HuskyLens | base sobre a mesa |
| HC-SR04 | placa frontal deitada |
| Line IR | deitado |

### Parâmetros sugeridos
| Parâmetro | Valor |
|---|---:|
| Altura de camada | 0,20 mm |
| Paredes | 4 |
| Top/Bottom | 5 camadas |
| Infill do tray | 25–35% |
| Infill da bolha | 12–20% |
| Material preferencial | PETG |

---

## 9. Pontos de atenção

1. **Verifique clones**: clones do Arduino Mega e L298N podem variar alguns milímetros.
2. **Confirme o sentido do USB** antes de imprimir a bolha final.
3. **Confira altura da HuskyLens** para garantir que a janela frontal coincida com a lente.
4. **Use uma impressão de teste rápida** do trecho frontal da bolha para validar HC-SR04, HuskyLens e IR.
5. **Se a bolha ficar muito pesada**, reduza espessura de parede para 1,6 mm ou imprima com menos infill.
6. **Se houver interferência com rodas**, aumente o diâmetro dos recortes em `wheel_arch_cut()`.

---

## 10. Próximo passo recomendado

Os próximos refinamentos mais úteis são:

1. **incluir suportes explícitos dos motores traseiros e das rodas dianteiras livres no OpenSCAD**;
2. **quebrar a bolha em duas peças** (frente + traseira ou esquerda + direita) para impressão mais limpa;
3. **adicionar encaixes rápidos** da bolha no lugar de apenas parafusos;
4. **gerar uma base 2D DXF** derivada do `base_plate()` para corte direto a laser.

---

## 11. Arquivo principal

Use o arquivo:

```text
chassi-bolha.scad
```
