/**
 * storage.h — Persistência de configuração na EEPROM do Mega 2560
 *
 * Guarda a calibração de trim dos motores entre reinicializações.
 * Os dados são validados por uma assinatura (magic) e versão.
 * Implementação: src/storage.cpp
 */
#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

namespace storage {

// Configuração persistida. Mantenha compatível com EEPROM_VERSION;
// ao mudar o layout, incremente a versão em config.h.
struct Config {
    float trimLeft;
    float trimRight;
};

// Lê a configuração da EEPROM. Retorna true se válida (magic+versão
// conferem); caso contrário preenche `out` com os padrões e retorna false.
bool load(Config& out);

// Grava a configuração na EEPROM (com magic+versão).
void save(const Config& cfg);

// Preenche `out` com os valores padrão (sem tocar na EEPROM).
void defaults(Config& out);

} // namespace storage

#endif // STORAGE_H
