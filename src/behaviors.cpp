/**
 * behaviors.cpp — Comportamentos autônomos (M3–M5)
 * Ver include/behaviors.h, include/config.h
 *
 * Modos isolados: FOLLOW_LINE, AVOID, TRACK_TARGET.
 * Modo NAVIGATE (M5): arbitrador em camadas por prioridade —
 *   Segurança (desvio) > Alvo visual (aproximar) > Seguir-linha > Ocioso (busca).
 * Os modos isolados e o NAVIGATE compartilham os mesmos "núcleos" de controle.
 */
#include "behaviors.h"
#include "config.h"
#include "motors.h"
#include "sensors.h"
#include "vision.h"

namespace {

behaviors::Mode g_mode = behaviors::MANUAL;

// Sub-máquina não-bloqueante do desvio de obstáculo (manobra latcheada).
enum AvoidPhase { AVP_CRUISE, AVP_BACK, AVP_TURN };
AvoidPhase g_phase      = AVP_CRUISE;
uint32_t   g_phaseStart = 0;
bool       g_turnRight  = true;   // alterna o lado do giro a cada desvio

// Última direção de erro (linha e visão) para reencontrar o que se perdeu.
int8_t     g_lastError   = 0;
uint32_t   g_visLastSeen = 0;
int16_t    g_visLastDir  = 1;

// Camada ativa do NAVIGATE (telemetria). Inicia nulo (F() não é permitido
// em inicializador global); o getter devolve "-" enquanto não definida.
const __FlashStringHelper* g_navLayer = nullptr;

// ======================= Núcleos de controle =======================

// Esterço proporcional pelo array IR (assume que há linha sob algum sensor).
void steerByLine(const sensors::LineState& ls) {
    int8_t error = (int8_t)((ls.right ? 1 : 0) - (ls.left ? 1 : 0));
    g_lastError = error;
    int16_t left  = FOLLOW_BASE_SPEED + (int16_t)FOLLOW_TURN_GAIN * error;
    int16_t right = FOLLOW_BASE_SPEED - (int16_t)FOLLOW_TURN_GAIN * error;
    motors::setTarget(left, right);
}

// Esterço + aproximação pelo alvo visual (assume primaryTarget().found).
void steerByVision() {
    vision::Target t = vision::primaryTarget();
    g_visLastSeen = millis();
    int16_t err = vision::horizontalError();
    g_visLastDir = (err >= 0) ? 1 : -1;

    int16_t turn = (int16_t)((int32_t)err * VISION_TURN_GAIN / (VISION_FRAME_W / 2));
    int16_t fwd  = (t.height >= VISION_TARGET_HEIGHT) ? 0 : VISION_APPROACH_SPEED;
    motors::setTarget(fwd + turn, fwd - turn);
}

// --- Manobra de desvio (latcheada): segura o controle até concluir ---
bool avoidActive() { return g_phase != AVP_CRUISE; }

void startAvoid() {
    motors::stop();
    g_phase = AVP_BACK;
    g_phaseStart = millis();
}

// Avança a manobra (ré -> giro). Ao terminar, volta a AVP_CRUISE.
void stepAvoid() {
    uint32_t now = millis();
    switch (g_phase) {
        case AVP_BACK:
            motors::backward(AVOID_SPEED);
            if (now - g_phaseStart >= AVOID_BACK_MS) { g_phase = AVP_TURN; g_phaseStart = now; }
            break;
        case AVP_TURN:
            if (g_turnRight) motors::turnRight(AVOID_SPEED);
            else             motors::turnLeft(AVOID_SPEED);
            if (now - g_phaseStart >= AVOID_TURN_MS) {
                g_turnRight = !g_turnRight;
                g_phase = AVP_CRUISE;
            }
            break;
        default: break;
    }
}

// ======================= Modos isolados =======================

void followLine() {
    if (sensors::obstacleAhead()) { motors::stop(); return; }
    sensors::LineState ls = sensors::readLine();
    if (ls.left || ls.center || ls.right) { steerByLine(ls); return; }
    // Linha perdida: gira na direção do último erro para reencontrá-la.
    int16_t dir = (g_lastError >= 0) ? 1 : -1;
    motors::setTarget(dir * FOLLOW_SEARCH_SPEED, -dir * FOLLOW_SEARCH_SPEED);
}

void avoidObstacles() {
    if (avoidActive())             { stepAvoid();  return; }
    if (sensors::obstacleAhead())  { startAvoid(); return; }
    motors::forward(MOTOR_SPEED_CRUISE);
}

void trackTarget() {
    vision::update();
    if (sensors::obstacleAhead()) { motors::stop(); return; }
    if (vision::primaryTarget().found) { steerByVision(); return; }
    // Alvo perdido: gira procurando por uma janela; depois para.
    if (millis() - g_visLastSeen < VISION_SEARCH_MS) {
        motors::setTarget(g_visLastDir * VISION_SEARCH_SPEED,
                          -g_visLastDir * VISION_SEARCH_SPEED);
    } else {
        motors::stop();
    }
}

// ======================= NAVIGATE (arbitrador) =======================

void navigate() {
    // Latch de segurança: conclui uma manobra de desvio em andamento.
    if (avoidActive())            { g_navLayer = F("SEGURANCA"); stepAvoid();  return; }
    // Prioridade 1 — segurança: obstáculo dispara o desvio.
    if (sensors::obstacleAhead()) { g_navLayer = F("SEGURANCA"); startAvoid(); return; }
    // Prioridade 2 — objetivo: alvo visual.
    vision::update();
    if (vision::primaryTarget().found) { g_navLayer = F("ALVO"); steerByVision(); return; }
    // Prioridade 3 — caminho: linha.
    sensors::LineState ls = sensors::readLine();
    if (ls.left || ls.center || ls.right) { g_navLayer = F("LINHA"); steerByLine(ls); return; }
    // Ocioso — gira procurando (na direção do último alvo visto).
    g_navLayer = F("BUSCA");
    int16_t dir = (g_visLastDir >= 0) ? 1 : -1;
    motors::setTarget(dir * NAV_SEARCH_SPEED, -dir * NAV_SEARCH_SPEED);
}

} // namespace

namespace behaviors {

void begin() {
    g_mode  = MANUAL;
    g_phase = AVP_CRUISE;
}

void setMode(Mode m) {
    g_mode  = m;
    g_phase = AVP_CRUISE;
    g_lastError   = 0;
    g_visLastSeen = 0;            // força "alvo perdido" até a primeira detecção
    g_navLayer    = F("-");
    motors::stop();
}

Mode mode() { return g_mode; }

const __FlashStringHelper* modeName() {
    switch (g_mode) {
        case FOLLOW_LINE:  return F("SEGUIR-LINHA");
        case AVOID:        return F("DESVIO");
        case TRACK_TARGET: return F("RASTREIO");
        case NAVIGATE:     return F("NAVEGACAO");
        default:           return F("MANUAL");
    }
}

const __FlashStringHelper* navLayer() { return g_navLayer ? g_navLayer : F("-"); }

void update() {
    switch (g_mode) {
        case FOLLOW_LINE:  followLine();     break;
        case AVOID:        avoidObstacles(); break;
        case TRACK_TARGET: trackTarget();    break;
        case NAVIGATE:     navigate();       break;
        case MANUAL:       default:          break;
    }
}

} // namespace behaviors
