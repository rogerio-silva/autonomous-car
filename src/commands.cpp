/**
 * commands.cpp — CLI serial estilo WASD + modos autônomos (M3)
 * Ver include/commands.h, docs/serial-control.md
 *
 * Mapa de teclas (resumo — ver printHelp):
 *   w/s/a/d  mover (só MANUAL) · espaço ou x  parar/emergência (volta a MANUAL)
 *   + / -    velocidade (setpoint)
 *   1/2 3/4  trim esquerdo / direito
 *   m/l/v    modo MANUAL / SEGUIR-LINHA / DESVIO
 *   c/f/g    calibração IR: ver / capturar fundo / capturar linha
 *   k        salvar config (trim+IR) na EEPROM · n  resetar config
 *   o        status · t  telemetria contínua · h ou ?  ajuda
 */
#include "commands.h"
#include "config.h"
#include "motors.h"
#include "sensors.h"
#include "vision.h"
#include "behaviors.h"
#include "storage.h"

namespace {

// Setpoint de velocidade ajustável pelo usuário (PWM) — usado no MANUAL.
const uint8_t  SPEED_MIN  = 60;
const uint8_t  SPEED_STEP = 15;
uint8_t        g_speed    = MOTOR_SPEED_CRUISE;

// Telemetria contínua.
bool           g_telemetry   = false;
uint32_t       g_lastTelemMs = 0;
const uint32_t TELEM_PERIOD_MS = 500;

bool isManual() { return behaviors::mode() == behaviors::MANUAL; }

uint8_t addSpeed(uint8_t s, int16_t delta) {
    int16_t v = (int16_t)s + delta;
    if (v < SPEED_MIN)         v = SPEED_MIN;
    if (v > MOTOR_SPEED_MAX)   v = MOTOR_SPEED_MAX;
    return (uint8_t)v;
}

void printVision() {
    vision::update();
    vision::Target t = vision::primaryTarget();
    Serial.print(F("[visao] alvo="));
    if (t.found) {
        Serial.print(F("sim id="));   Serial.print(t.id);
        Serial.print(F(" x="));       Serial.print(t.x);
        Serial.print(F(" h="));       Serial.print(t.height);
        Serial.print(F(" erroX="));   Serial.println(vision::horizontalError());
    } else {
        Serial.println(F("nao"));
    }
}

void printIR() {
    int16_t raw[3];
    sensors::readLineRaw(raw);
    sensors::LineState ls = sensors::readLine();
    const storage::Config& c = storage::data();
    Serial.print(F("[ir] raw="));
    Serial.print(raw[0]); Serial.print(','); Serial.print(raw[1]); Serial.print(','); Serial.print(raw[2]);
    Serial.print(F("  limiar="));
    Serial.print(c.irThreshold[0]); Serial.print(','); Serial.print(c.irThreshold[1]); Serial.print(','); Serial.print(c.irThreshold[2]);
    Serial.print(F("  linha(E,C,D)="));
    Serial.print(ls.left); Serial.print(','); Serial.print(ls.center); Serial.print(','); Serial.println(ls.right);
}

void handleKey(char c) {
    switch (c) {
        // --- Movimento (somente em MANUAL) ---
        case 'w': if (isManual()) motors::forward(g_speed);  break;
        case 's': if (isManual()) motors::backward(g_speed); break;
        case 'a': if (isManual()) motors::turnLeft(g_speed); break;
        case 'd': if (isManual()) motors::turnRight(g_speed);break;
        case 'x':
        case ' ': behaviors::setMode(behaviors::MANUAL); motors::stop();
                  Serial.println(F("[stop] parado (MANUAL)")); break;

        // --- Velocidade ---
        case '+':
        case '=': g_speed = addSpeed(g_speed,  SPEED_STEP);
                  Serial.print(F("velocidade=")); Serial.println(g_speed); break;
        case '-':
        case '_': g_speed = addSpeed(g_speed, -SPEED_STEP);
                  Serial.print(F("velocidade=")); Serial.println(g_speed); break;

        // --- Trim (calibração de offset dos motores) ---
        case '1': motors::adjustTrimLeft(-TRIM_STEP);  commands::printStatus(); break;
        case '2': motors::adjustTrimLeft( TRIM_STEP);  commands::printStatus(); break;
        case '3': motors::adjustTrimRight(-TRIM_STEP); commands::printStatus(); break;
        case '4': motors::adjustTrimRight( TRIM_STEP); commands::printStatus(); break;

        // --- Modos ---
        case 'm': behaviors::setMode(behaviors::MANUAL);       Serial.println(F("[modo] MANUAL"));       break;
        case 'l': behaviors::setMode(behaviors::FOLLOW_LINE);  Serial.println(F("[modo] SEGUIR-LINHA")); break;
        case 'v': behaviors::setMode(behaviors::AVOID);        Serial.println(F("[modo] DESVIO"));        break;
        case 'p': behaviors::setMode(behaviors::TRACK_TARGET); Serial.println(F("[modo] RASTREIO"));      break;
        case 'N': behaviors::setMode(behaviors::NAVIGATE);     Serial.println(F("[modo] NAVEGACAO"));     break;

        // --- Info de visão ---
        case 'i': printVision(); break;

        // --- Calibração IR ---
        case 'c': printIR(); break;
        case 'f': sensors::sampleBackground();
                  Serial.println(F("[cal] fundo capturado; posicione sobre a linha e tecle 'g'")); break;
        case 'g':
            if (sensors::sampleLine()) {
                Serial.println(F("[cal] linha capturada; limiares atualizados ('k' p/ salvar)"));
                printIR();
            } else {
                Serial.println(F("[cal] capture o FUNDO primeiro (tecle 'f')"));
            }
            break;

        // --- Persistência (trim + IR) ---
        case 'k': storage::save();        Serial.println(F("[OK] config salva na EEPROM")); break;
        case 'n': storage::resetDefaults(); motors::stop();
                  Serial.println(F("[OK] config resetada ao padrao")); commands::printStatus(); break;

        // --- Info ---
        case 'o': commands::printStatus(); break;
        case 't': g_telemetry = !g_telemetry;
                  Serial.print(F("telemetria=")); Serial.println(g_telemetry ? F("ON") : F("OFF")); break;
        case 'h':
        case '?': commands::printHelp(); break;

        case '\r':
        case '\n': break;
        default: break;
    }
}

} // namespace

namespace commands {

void begin() {
    printHelp();
    printStatus();
}

void poll() {
    while (Serial.available() > 0) {
        handleKey((char)Serial.read());
    }

    if (g_telemetry && (millis() - g_lastTelemMs >= TELEM_PERIOD_MS)) {
        g_lastTelemMs = millis();
        printStatus();
    }
}

void printHelp() {
    Serial.println(F("\n===== autonomous-car :: controle serial (M3) ====="));
    Serial.println(F("Movimento : w=frente s=re a=esq d=dir  (espaco|x)=parar/emergencia"));
    Serial.println(F("Velocidade: + aumenta   - diminui"));
    Serial.println(F("Trim      : 1/2 esq(-/+)   3/4 dir(-/+)"));
    Serial.println(F("Modos     : m=MANUAL l=SEGUIR-LINHA v=DESVIO p=RASTREIO N=NAVEGACAO"));
    Serial.println(F("Calib. IR : c=ver  f=capturar fundo  g=capturar linha"));
    Serial.println(F("Visao     : i=info do alvo (HuskyLens)"));
    Serial.println(F("Config    : k=salvar(EEPROM)  n=resetar"));
    Serial.println(F("Info      : o=status  t=telemetria on/off  h|?=ajuda"));
    Serial.println(F("Movimento manual so funciona no modo MANUAL."));
    Serial.println(F("=================================================\n"));
}

void printStatus() {
    Serial.print(F("[status] modo="));  Serial.print(behaviors::modeName());
    if (behaviors::mode() == behaviors::NAVIGATE) {
        Serial.print(F("("));  Serial.print(behaviors::navLayer());  Serial.print(F(")"));
    }
    Serial.print(F(" vel="));           Serial.print(g_speed);
    Serial.print(F(" pwmL="));          Serial.print(motors::currentLeft());
    Serial.print(F(" pwmR="));          Serial.print(motors::currentRight());
    Serial.print(F(" trimL="));         Serial.print(motors::trimLeft(), 2);
    Serial.print(F(" trimR="));         Serial.print(motors::trimRight(), 2);
    Serial.print(F(" dist="));          Serial.print(sensors::distanceCm());
    Serial.println(F("cm"));
}

} // namespace commands
