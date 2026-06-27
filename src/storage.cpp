/**
 * storage.cpp — Implementação da persistência em EEPROM (cache em RAM)
 * Ver include/storage.h e include/config.h
 */
#include "storage.h"
#include "config.h"
#include <EEPROM.h>

namespace {

// Cabeçalho + dados gravados na EEPROM. O magic/versão permitem detectar
// EEPROM "virgem" ou layout antigo e cair nos padrões com segurança.
struct Persisted {
    uint16_t        magic;
    uint8_t         version;
    storage::Config cfg;
};

storage::Config g_cfg;
bool            g_loaded = false;

void fillDefaults(storage::Config& c) {
    c.trimLeft   = TRIM_DEFAULT;
    c.trimRight  = TRIM_DEFAULT;
    c.irThreshold[0] = IR_LINE_THRESHOLD;
    c.irThreshold[1] = IR_LINE_THRESHOLD;
    c.irThreshold[2] = IR_LINE_THRESHOLD;
    c.irLineHigh = IR_LINE_HIGH_DEFAULT;
}

} // namespace

namespace storage {

void begin() {
    Persisted p;
    EEPROM.get(EEPROM_CONFIG_ADDR, p);
    if (p.magic == EEPROM_MAGIC && p.version == EEPROM_VERSION) {
        g_cfg    = p.cfg;
        g_loaded = true;
    } else {
        fillDefaults(g_cfg);
        g_loaded = false;
    }
}

Config& data() { return g_cfg; }

void save() {
    Persisted p;
    p.magic   = EEPROM_MAGIC;
    p.version = EEPROM_VERSION;
    p.cfg     = g_cfg;
    // EEPROM.put só grava bytes alterados (poupa ciclos de escrita).
    EEPROM.put(EEPROM_CONFIG_ADDR, p);
}

void resetDefaults() { fillDefaults(g_cfg); }

bool wasLoaded() { return g_loaded; }

} // namespace storage
