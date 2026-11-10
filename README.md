# Test CanUniversal 3.1.0 — ESP32 + PlatformIO

[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-blue)](https://platformio.org/)
[![CanUniversal](https://img.shields.io/badge/CanUniversal-3.1.0-green)](https://registry.platformio.org/libraries/prune74/CanUniversal)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**Projet de test pour la bibliothèque CanUniversal v3.1.0** — CAN bus universel pour ESP32 avec support natif du contrôleur TWAI intégré et du MCP2515.

---

## 📋 À propos de CanUniversal

**CanUniversal** est une bibliothèque **universelle CAN bus** pour ESP32 qui prend en charge :

- ✅ **Contrôleur TWAI intégré** (CAN natif de l'ESP32)
- ✅ **MCP2515 externe** (via SPI)
- ✅ **Multi-bus** — Gestion de plusieurs bus CAN simultanément
- ✅ **Non-bloquant** — Pas d'interruptions bloquantes
- ✅ **Filtrage matériel** — Filtrage des messages CAN au niveau matériel
- ✅ **Identifiants 11-bit et 29-bit** — Support des identifiants étendus (Locoduino/Märklin)
- ✅ **Aucune dépendance externe** — Bibliothèque autonome

**Auteur** : [prune74](https://github.com/prune74/CanUniversal)

---

## 🚀 Prérequis

- **PlatformIO** (version 6.0+ recommandée)
- **ESP32 Dev Module** (ou toute carte ESP32 compatible)
- **Framework Arduino** pour ESP32

---

## 📦 Installation

### Méthode 1 : Via PlatformIO Library Manager (recommandé)

La bibliothèque est disponible dans le registre officiel PlatformIO :

```ini
# platformio.ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    prune74/CanUniversal@^3.1.0
```

### Méthode 2 : Installation manuelle

1. Télécharger la bibliothèque depuis [PlatformIO Registry](https://registry.platformio.org/libraries/prune74/CanUniversal)
2. Placer le dossier `CanUniversal` dans le dossier `lib` de votre projet

---

## 🧪 Configuration du projet

Ce projet est configuré pour tester CanUniversal avec le **mode loopback** du contrôleur TWAI, ce qui signifie :

- **Aucun câblage nécessaire** ✅
- Le CAN interne de l'ESP32 est configuré pour envoyer et recevoir ses propres messages
- Parfait pour les tests unitaires et le développement

### Fichier `platformio.ini`

```ini
[env:esp32dev]
platform = espressif32@6.9.0
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    prune74/CanUniversal@^3.1.0
```

---

## 🔧 Configuration matérielle

### Mode Loopback (TWAI intégré)

**Aucun matériel supplémentaire requis** — Le test utilise le mode loopback du TWAI.

### Mode Normal (avec MCP2515)

Si vous souhaitez utiliser un module MCP2515 :

| MCP2515 | ESP32 Dev Module |
|---------|------------------|
| CS      | GPIO 5           |
| SCK     | GPIO 18 (HSPI)   |
| SI (MOSI) | GPIO 23 (HSPI)  |
| SO (MISO) | GPIO 19 (HSPI)  |
| INT     | GPIO 25 (optionnel) |

---

## 🏃‍♂️ Exécution des tests

### Compilation et téléversement

```bash
# Compiler le projet
pio run

# Télécharger sur l'ESP32 (après avoir connecté la carte)
pio run --target upload

# Ouvrir le moniteur série
pio device monitor
```

### Moniteur série

Le projet configure automatiquement le moniteur série à **115200 bauds**.

---

## 📂 Structure du projet

```
Test CanUniversal 3.1.0/
├── include/          # Fichiers d'en-tête personnels
├── lib/              # Bibliothèques externes (si installation manuelle)
├── src/              # Code source principal
│   └── main.cpp      # Programme principal de test
├── test/             # Tests unitaires (PlatformIO Test Runner)
├── platformio.ini    # Configuration PlatformIO
└── README.md         # Ce fichier
```

---

## 🔍 Tests unitaires

Le dossier `test/` contient des tests pour la bibliothèque CanUniversal.

Pour exécuter les tests unitaires :

```bash
# Exécuter les tests unitaires
pio test
```

> ⚠️ **Note** : Les tests unitaires nécessitent une configuration spécifique et peuvent nécessiter un environnement de test natif.

---

## 📞 Support et contribution

- **Signaler un bug** : [Ouvrir une issue sur GitHub](https://github.com/prune74/CanUniversal/issues)
- **Contribuer** : Les pull requests sont les bienvenues !
- **Licence** : MIT

---

## 🎯 Exemple de code (main.cpp)

```cpp
#include <Arduino.h>
#include <CanUniversal.h>

// Configuration pour le mode loopback (TWAI intégré)
#define CAN_RX_PIN GPIO_NUM_22
#define CAN_TX_PIN GPIO_NUM_21

void setup() {
    Serial.begin(115200);
    while (!Serial);
    
    Serial.println("Démarrage du test CanUniversal...");
    
    // Initialisation du bus CAN en mode loopback
    CanBus::begin(CAN_SPEED_125KBPS, CAN_MODE_LOOPBACK);
    
    Serial.println("Bus CAN initialisé en mode loopback !");
}

void loop() {
    // Envoyer un message CAN
    CanMsg msg;
    msg.id = 0x123;
    msg.len = 8;
    for (int i = 0; i < 8; i++) {
        msg.data[i] = i;
    }
    
    CanBus::send(msg);
    
    // Recevoir le message (en mode loopback, on reçoit ce qu'on envoie)
    if (CanBus::receive(msg)) {
        Serial.print("Message reçu : ID=0x");
        Serial.print(msg.id, HEX);
        Serial.print(", Data=");
        for (int i = 0; i < msg.len; i++) {
            Serial.print(msg.data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
    
    delay(1000);
}
```

---

## 📖 Documentation

- [Documentation officielle CanUniversal](https://github.com/prune74/CanUniversal)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [TWAI Controller Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html)

---

*Projet créé avec ❤️ par [prune74](https://github.com/prune74)*
*Dernière mise à jour : Novembre 2026*
