/*
 * Projet: Multi Air Instrument
 * Section: 31-A
 *
 * Ce programme Arduino configuré pour une seule main
 * permet de récupérer les données des capteurs et de les envoyer
 * via Bluetooth à un appareil Android.
 */


#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_LSM6DS3.h>

// UUID du service Bluetooth personnalisé
#define SERVICE_UUID        "12ba8c73-556d-47d6-997c-5ac9ca5f5e10"
// UUID de la caractéristique Bluetooth personnalisée
#define CHARACTERISTIC_UUID "83c25633-9cb3-46f6-9ef6-7d76ebf8fe3c"
//On fixe le délaie entre chaque envoi de données
#define DELAI 250


// Structure de données pour stocker les valeurs des capteurs
struct SensorData {
    float accelerometre[3];
    float gyroscope[3];
    float magnetometre[3];
    float flex[5];
};

// Caractéristique Bluetooth personnalisée pour envoyer les valeurs de capteur
BLECharacteristic attribut_capteur(CHARACTERISTIC_UUID, BLERead | BLENotify, sizeof(SensorData));

void setup() {
    Serial.begin(9600);
    while (!Serial);

    // Initialisation du module BLE
    if (!BLE.begin()) {
        Serial.println("Erreur BLE");
        while (1);
    }

    // Initialisation du capteur de mouvement
    if (!IMU.begin()) {
        Serial.println("Erreur d'initialisation de l'IMU");
        while (1);
    }

    // Création du service Bluetooth personnalisé
    BLEService service_capteur(SERVICE_UUID);

    // Ajout de la caractéristique Bluetooth personnalisée au service
    service_capteur.addCharacteristic(attribut_capteur);

    // Publication du service Bluetooth personnalisé
    BLE.advertise(service_capteur);

    Serial.println("Attente de connexion Bluetooth...");
}

void loop() {
    // Attendre une connexion Bluetooth
    BLEDevice central = BLE.central();

    // Si une connexion Bluetooth est établie
    if (central) {
        Serial.print("Connexion à ");
        Serial.println(central.address());

        // Tant que la connexion Bluetooth est active
        while (central.connected()) {
            // Lecture des valeurs de l'accéléromètre
            float accel_values[3] = {IMU.acceleration().x(), IMU.acceleration().y(), IMU.acceleration().z()};

            // Lecture des valeurs du gyroscope
            float gyro_values[3] = {IMU.gyroscope().x(), IMU.gyroscope().y(), IMU.gyroscope().z()};

            // Lecture des valeurs du magnétomètre
            float mag_values[3] = {IMU.magneticField().x(), IMU.magneticField().y(), IMU.magneticField().z()};

            // Lecture des valeurs des capteurs de flexion
            float flex_values[5] = {analogRead(A0), analogRead(A1), analogRead(A2), analogRead(A3), analogRead(A4)};

            // Regroupement des valeurs des capteurs dans une structure de données
            SensorData sensor_data = {accel_values, gyro_values, mag_values, flex_values};

            // Envoi des valeurs des capteurs via la caractéristique Bluetooth personnalisée
            attribut_capteur.writeValue((byte*)&sensor_data, sizeof(SensorData));

            delay(DELAI);
        }
        // Déconnexion Bluetooth
        Serial.println("Déconnexion Bluetooth");
    }
}