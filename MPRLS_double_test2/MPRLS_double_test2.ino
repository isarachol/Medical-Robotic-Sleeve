#include <Wire.h>
#include <Adafruit_MPRLS.h>

// Define Reset pins for both sensors
#define RESET_SENSOR_1 9
#define RESET_SENSOR_2 10

Adafruit_MPRLS mpr; // Only one instance needed since both sensors share the same I²C address

void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(RESET_SENSOR_1, OUTPUT);
    pinMode(RESET_SENSOR_2, OUTPUT);

    // Start with both sensors in RESET mode
    digitalWrite(RESET_SENSOR_1, LOW);
    digitalWrite(RESET_SENSOR_2, LOW);
   
    delay(100); // Ensure sensors are fully reset

    Serial.println("Initializing sensors...");

    // Initialize Sensor 1
    digitalWrite(RESET_SENSOR_1, HIGH); // Activate Sensor 1
    delay(10); // Allow time to power up
    if (!mpr.begin()) {
        Serial.println("Failed to initialize MPRLS Sensor 1!");
    }
    digitalWrite(RESET_SENSOR_1, LOW); // Reset Sensor 1

    // Initialize Sensor 2
    digitalWrite(RESET_SENSOR_2, HIGH); // Activate Sensor 2
    delay(10);
    if (!mpr.begin()) {
        Serial.println("Failed to initialize MPRLS Sensor 2!");
    }
    digitalWrite(RESET_SENSOR_2, LOW); // Reset Sensor 2
}

void loop() {
    float pressure1, pressure2;

    // Read from Sensor 1
    digitalWrite(RESET_SENSOR_1, HIGH); // Activate Sensor 1
    delay(10); // Allow time for the sensor to wake up
    pressure1 = mpr.readPressure();
    digitalWrite(RESET_SENSOR_1, LOW); // Reset Sensor 1
    Serial.print("Sensor 1 Pressure: ");
    Serial.print(pressure1);
    Serial.println(" hPa");

    // Read from Sensor 2
    digitalWrite(RESET_SENSOR_2, HIGH); // Activate Sensor 2
    delay(10);
    pressure2 = mpr.readPressure();
    digitalWrite(RESET_SENSOR_2, LOW); // Reset Sensor 2
    Serial.print("Sensor 2 Pressure: ");
    Serial.print(pressure2);
    Serial.println(" hPa");

    delay(1000);
}