/**
 * storage.cpp — Implementação da persistência em EEPROM
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

} // namespace

namespace storage {

void defaults(Config& out) {
    out.trimLeft  = TRIM_DEFAULT;
    out.trimRight = TRIM_DEFAULT;
}

bool load(Config& out) {
    Persisted p;
    EEPROM.get(EEPROM_CONFIG_ADDR, p);
    if (p.magic != EEPROM_MAGIC || p.version != EEPROM_VERSION) {
        defaults(out);
        return false;
    }
    out = p.cfg;
    return true;
}

void save(const Config& cfg) {
    Persisted p;
    p.magic   = EEPROM_MAGIC;
    p.version = EEPROM_VERSION;
    p.cfg     = cfg;
    // EEPROM.put usa update interno (só grava bytes alterados — poupa ciclos).
    EEPROM.put(EEPROM_CONFIG_ADDR, p);
}

} // namespace storage
