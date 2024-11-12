#include <ESP32_NOW.h>
#include <ESP32_NOW_Serial.h>
#include <WiFi.h>

const int motionSensorPin = 33;  // Broche du capteur de mouvement
const int ledPin = 25;           // Broche de la LED (lampe)

// Structure pour les données à envoyer
typedef struct struct_message {
  int mouvement;
} struct_message;

// Créer une structure de message
struct_message myData;

// Adresse MAC de l'ESP32 n°2 (à remplacer par l'adresse MAC réelle)
uint8_t broadcastAddress[] = {0x24, 0x0a, 0xc4, 0xc1, 0x21, 0x4c};

// Fonction de callback pour ESP-NOW
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nStatut de l'envoi dernier paquet:");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Succès" : "Échec");
}

void setup() {
  Serial.begin(115200);          // Initialiser la communication série
  pinMode(motionSensorPin, INPUT); // Définir la broche du capteur comme entrée
  pinMode(ledPin, OUTPUT);        // Définir la broche de la LED comme sortie

  // Initialiser WiFi
  WiFi.mode(WIFI_STA);

  // Initialiser ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erreur d'initialisation d'ESP-NOW");
    return;
  }

  // Enregistrer la fonction de callback
  esp_now_register_send_cb(OnDataSent);

  // Enregistrer l'ESP32 n°2 comme pair
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Ajouter le pair
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Échec d'ajout du pair");
    return;
  }
}

void loop() {
  if (digitalRead(motionSensorPin) == HIGH) { // Si le capteur détecte un mouvement
    Serial.println("Mouvement détecté !");   // Afficher un message dans le moniteur série
    digitalWrite(ledPin, HIGH);   // Éteindre la LED sur ESP32 n°1
    myData.mouvement = 1;        // Envoyer 1 pour indiquer le mouvement
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)); // Envoyer le message à l'ESP32 n°2
    if (result == ESP_OK) {
      Serial.println("Envoi du message réussi");
    } else {
      Serial.println("Échec de l'envoi du message");
    }
    delay(3000);                 // Attendre 3 secondes (2000 millisecondes)
    digitalWrite(ledPin, LOW);   // Rallumer la LED sur ESP32 n°1
    myData.mouvement = 0;        // Envoyer 0 pour indiquer l'absence de mouvement
    //esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)); // Envoyer le message à l'ESP32 n°2
  } else {
    Serial.println("Pas de mouvement.");   // Afficher un message dans le moniteur série
  }
  delay(100); // Attendre un court instant avant la prochaine lecture
}