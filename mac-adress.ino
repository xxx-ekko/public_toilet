#include <ESP32_NOW.h>
#include <ESP32_NOW_Serial.h>
#include <WiFi.h>

const int ledPin = 32;           // Broche de la LED sur l'ESP32 n°2

// Structure pour les données à recevoir
typedef struct struct_message {
  int mouvement;
} struct_message;

// Créer une structure de message
struct_message myData;

// Fonction de callback pour ESP-NOW (réception des données)
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  // Copier les données reçues dans la structure "myData"
  memcpy(&myData, incomingData, sizeof(myData));
  
  Serial.print("Mouvement reçu : ");
  Serial.println(myData.mouvement);
  
  if (myData.mouvement == 1) {
    digitalWrite(ledPin, LOW);   // Allumer la LED sur ESP32 n°2
    delay(3000);                 // Attendre 5 secondes
    digitalWrite(ledPin, HIGH);    // Éteindre la LED sur ESP32 n°2
  }
}

void setup() {
  Serial.begin(115200);          // Initialiser la communication série
  pinMode(ledPin, OUTPUT);        // Définir la broche de la LED comme sortie

  // Initialiser WiFi
  WiFi.mode(WIFI_STA);

  // Initialiser ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erreur d'initialisation d'ESP-NOW");
    return;
  }

  // Enregistrer la fonction de callback pour la réception des données
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Rien à faire ici, la LED est contrôlée par l'ESP32 n°1
}
