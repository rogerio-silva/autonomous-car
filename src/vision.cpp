/**
 * vision.cpp — Implementação da leitura da Gravity HuskyLens (I2C)
 * Ver include/vision.h e docs/vision-huskylens.md
 *
 * NOTA: esqueleto da Fundação (M1). A integração completa (escolha de
 * algoritmo, aprendizado de IDs, fusão com sensores) é o foco do M4.
 */
#include "vision.h"
#include "config.h"
#include "HUSKYLENS.h"
#include <Wire.h>

namespace {
HUSKYLENS huskylens;
vision::Target g_primary = {false, 0, 0, 0, 0, 0};

// Centro do quadro da HuskyLens (resolução 320x240).
const int16_t FRAME_CENTER_X = 160;
}

namespace vision {

bool begin() {
    Wire.begin();
    // Tenta estabelecer comunicação; o chamador decide o que fazer no erro.
    return huskylens.begin(Wire);
}

void update() {
    g_primary = {false, 0, 0, 0, 0, 0};
    if (!huskylens.request()) return;
    if (!huskylens.isLearned()) return;
    if (!huskylens.available()) return;

    // Seleciona o maior bloco do quadro como alvo primário.
    HUSKYLENSResult best;
    bool has = false;
    while (huskylens.available()) {
        HUSKYLENSResult r = huskylens.read();
        if (r.command != COMMAND_RETURN_BLOCK) continue;
        if (!has || (uint32_t)r.width * r.height > (uint32_t)best.width * best.height) {
            best = r;
            has = true;
        }
    }
    if (has) {
        g_primary = {true, best.xCenter, best.yCenter,
                     (uint16_t)best.width, (uint16_t)best.height, best.ID};
    }
}

Target primaryTarget() { return g_primary; }

int16_t horizontalError() {
    if (!g_primary.found) return 0;
    return g_primary.x - FRAME_CENTER_X;
}

} // namespace vision
