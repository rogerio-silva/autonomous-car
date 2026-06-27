/**
 * storage.h — Configuração persistente (EEPROM) com cache em RAM
 *
 * Fonte única de verdade para os parâmetros calibráveis: trim dos motores
 * e limiares/polaridade dos sensores IR de linha. Os módulos leem e
 * escrevem em `data()` e chamam `save()` para persistir.
 * Implementação: src/storage.cpp
 */
#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

namespace storage {

// Configuração persistida. Ao mudar o layout, incremente EEPROM_VERSION
// em config.h (dados antigos viram padrão automaticamente).
struct Config {
    float   trimLeft;        // [TRIM_MIN, 1.0]
    float   trimRight;
    int16_t irThreshold[3];  // limiar linha/fundo por sensor (E, C, D)
    uint8_t irLineHigh;      // bits 0..2: "sobre a linha" = leitura > limiar
};

// Carrega a config da EEPROM para o cache (ou aplica padrões se inválida).
// Chamar uma vez no setup(), ANTES de motors/sensors.
void begin();

// Cache mutável (fonte de verdade em runtime).
Config& data();

// Persiste o cache atual na EEPROM.
void save();

// Restaura o cache para os valores padrão (não grava sozinho).
void resetDefaults();

// True se a EEPROM continha dados válidos no begin().
bool wasLoaded();

} // namespace storage

#endif // STORAGE_H
