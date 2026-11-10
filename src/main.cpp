/**
 * ============================================================================
 *  Test CanUniversal v3.1.0 — CAN interne (TWAI) en mode loopback
 * ============================================================================
 *  Matériel nécessaire : une carte ESP32, C'EST TOUT.
 *  Le mode loopback reboucle tout ce qui est émis : aucune paire CAN,
 *  aucun transceiver, aucun fil à brancher.
 *
 *  Ce que teste ce programme :
 *    1. l'initialisation (provider + CanInit::begin)
 *    2. la validité du bus (tolérance aux erreurs)
 *    3. l'envoi non bloquant de trames 11 bits (compteur)
 *    4. l'envoi de trames 29 bits (format Locoduino/Märklin)
 *    5. la réception (loopback matériel du TWAI)
 *
 *  Sur le moniteur série (115200) on doit voir :
 *    [OK] CAN0 prêt — loopback
 *    puis toutes les secondes : [TX] ... puis [RX] ...
 *    Si TX part sans jamais de RX : problème (mais en loopback, ça marche).
 * ============================================================================
 */

#include <Arduino.h>
#include <CanUniversal.h>

// -----------------------------------------------------------------------------
// ÉTAPE 1 : la configuration — fournie par l'APPLICATION, pas par la biblio.
// -----------------------------------------------------------------------------
struct TestConfig : public CanConfigProvider
{

    CanBusConfig buses[1] = {
        //          speed   tx         rx         cs  int  sck  mosi miso quartz loopback
        CanBusConfig(true, 500000,
                     GPIO_NUM_4, GPIO_NUM_5,   // CAN interne (TWAI)
                     GPIO_NUM_NC, GPIO_NUM_NC, // pas de MCP2515
                     GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC,
                     0,    // quartz inutile en TWAI
                     true) // LOOPBACK : rien à brancher
    };

    uint8_t busCount() const override { return 1; }
    const CanBusConfig &bus(uint8_t index) const override { return buses[index]; }
};

TestConfig canConfig;
uint32_t compteur = 0;

// -----------------------------------------------------------------------------
// ÉTAPE 2 : initialisation tolérante aux erreurs.
// -----------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    delay(2000); // laisser le temps d'ouvrir le moniteur série

    Serial.println("\n=====================================");
    Serial.println("  Test CanUniversal v3.1.0 (loopback)");
    Serial.println("=====================================\n");

    CanInit::begin(canConfig);

    if (CanBus::bus(0).isValid())
    {
        // Activer le loopback LOGICIEL pour voir les messages reçus
        CanBus::bus(0).enableSoftwareLoopback();
        Serial.println("[OK] CAN0 prêt — loopback TWAI + LOGICIEL\n");
    }
    else
    {
        Serial.println("[ERREUR] CAN0 invalide !");
        Serial.println("Vérifie la version de CanUniversal (3.1.0).");
    }
}

// -----------------------------------------------------------------------------
// ÉTAPE 3 : la boucle de test.
// -----------------------------------------------------------------------------
void loop()
{
    CanBus &can0 = CanBus::bus(0);

    // --- Trame 11 bits : un compteur qui s'incrémente ---
    compteur++;
    CanMsg tx11((uint16_t)0x100, {(uint8_t)(compteur >> 24), (uint8_t)(compteur >> 16),
                                  (uint8_t)(compteur >> 8), (uint8_t)compteur});

    if (can0.send(tx11))
    {
        Serial.print("[TX 11b] ID=0x100 compteur=");
        Serial.println(compteur);
    }
    else
    {
        Serial.println("[TX 11b] ECHOUE (bus invalide ou occupé)");
    }

    // Délai pour laisser le buffer de transmission se vider
    // En mode loopback TWAI, cela évite la saturation
    delay(10);

    // --- Trame 29 bits : style Locoduino (prio | cmde | resp | nodeId) ---
    // Même trame à chaque fois : commande "ping" (0x18) de laBox 0x0010.
    CanMsg tx29((uint32_t)0x00086010, {0x00, 0x10, 0x00, 0x00}); // prio 0, cmde 0x18
    //            ^^^^^^^^^^ le cast dit "c'est du 29 bits" : sans lui, le
    //            compilateur ne sait pas choisir entre les 2 constructeurs.
    if (can0.send(tx29))
        Serial.println("[TX 29b] ID=0x00086010 (ping laBox)");
    else
        Serial.println("[TX 29b] ECHOUE");

    // --- Réception : en loopback, on doit retrouver nos propres trames ---
    delay(100); // laisser le circuit reboucler
    CanMsg rx;
    while (can0.receive(rx))
    {
        Serial.print("[RX    ] ID=0x");
        Serial.print(rx.id, HEX);
        Serial.print(" DLC=");
        Serial.print(rx.dlc);
        Serial.print(" DATA=");
        for (uint8_t i = 0; i < rx.dlc; i++)
        {
            if (rx.data[i] < 0x10)
                Serial.print('0');
            Serial.print(rx.data[i], HEX);
            Serial.print(' ');
        }
        Serial.println();
    }

    Serial.println("----");
    delay(1000);
}
