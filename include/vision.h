/**
 * vision.h — Camada de percepção via Gravity HuskyLens (I2C)
 *
 * A HuskyLens executa a visão computacional embarcada (reconhecimento de
 * objetos, rastreio de linha, tags, faces, cores). Este módulo apenas LÊ
 * os resultados pela I2C e os entrega à lógica de autonomia.
 *
 * Implementação: src/vision.cpp
 */
#ifndef VISION_H
#define VISION_H

#include <Arduino.h>

namespace vision {

// Resultado simplificado de um bloco/seta retornado pela HuskyLens.
struct Target {
    bool     found;
    int16_t  x;       // centro X (0–320)
    int16_t  y;       // centro Y (0–240)
    uint16_t width;
    uint16_t height;
    int16_t  id;      // ID aprendido (>0) ou 0 se não reconhecido
};

// Inicializa a comunicação I2C com a HuskyLens.
// Retorna false se o dispositivo não responder.
bool begin();

// Atualiza os dados a partir da HuskyLens (chamar a cada loop).
void update();

// Alvo mais relevante (maior/mais central) do quadro atual.
Target primaryTarget();

// Desvio horizontal do alvo em relação ao centro do quadro (-160..160).
// Útil para controle proporcional de direção. 0 se não houver alvo.
int16_t horizontalError();

} // namespace vision

#endif // VISION_H
