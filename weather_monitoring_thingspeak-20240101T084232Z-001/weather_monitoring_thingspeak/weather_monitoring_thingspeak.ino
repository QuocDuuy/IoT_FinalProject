#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

#define DHTPIN 2  // Replace with the GPIO number you are using for DHT sensor
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

const char *ssid = "The Coffee House";
const char *password = "thecoffeehouse";
const char *api_key = "P6K3YEFOZA7MOC8N";
const int channel_id = 2393287;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Initialize DHT sensor
  dht.begin();

  // Initialize BMP180 sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1);
  }
}

void loop() {
  Serial.println("Reading sensors...");

  // Read DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Read BMP180 sensor
  sensors_event_t event;
  bmp.getEvent(&event);
  float pressure = event.pressure;

  // Read rain sensor (assuming it's connected to analog pin A0)
  int rainValue = analogRead(A0);
  int rainPercentage = map(rainValue, 0, 1023, 0, 100);

  // Print sensor readings
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Pressure: ");
  Serial.println(pressure);
  Serial.print("Rain: ");
  Serial.println(rainPercentage);

  // Update ThingSpeak channel with sensor data
  if (ThingSpeak.writeField(channel_id, 1, temperature, api_key) == 200 &&
      ThingSpeak.writeField(channel_id, 2, humidity, api_key) == 200 &&
      ThingSpeak.writeField(channel_id, 3, pressure, api_key) == 200 &&
      ThingSpeak.writeField(channel_id, 4, rainPercentage, api_key) == 200) {
    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.println("Failed to send data to ThingSpeak");
  }

  delay(2000);
}
