#include <WiFi.h>
#include <WiFiClient.h>
#define BLYNK_TEMPLATE_ID "TMPL6gzwWKyCw"
#define BLYNK_TEMPLATE_NAME "esp32"
#define BLYNK_PRINT Serial


#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP32Servo.h>


#define DHTPIN 4         // DHT22 sensörünün bağlı olduğu pin
#define DHTTYPE DHT22    // DHT tipi (DHT11, DHT22, DHT21)


int ENB = 18; // PWM sinyali için
int IN4 = 19; // Yön kontrolü için


#define ENB 18 // PWM sinyali için
#define IN4 19 // Yön kontrolü için


WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;


char auth[] = "QL0C5XHx3OQrk9anq_m3-UNwFQZXu6ci";
const char* ssid = "proje";     //FiberHGW_ZT63JY       proje
const char* password = "12345679";    //YXPhsu4TkfTH          12345679
const char* ap_ssid = "ESP32"; // AP adı
const char* ap_password = "12345679"; // AP şifresi


int GREEN_LED_PIN = 12;
int RED_LED_PIN = 13;


#define LED_PIN_1 13 // ESP32'deki GPIO pin numarası
#define LED_PIN_2 12 // ESP32'deki GPIO pin numarası
#define SERVO_PIN 5 // ESP32'deki GPIO pin numarası


String ledStateGreen = "OFF";
String ledStateRed = "OFF";
int servoAngle = 0; // Servo motorun açısı


bool apStarted = false; // AP modunun başlatılıp başlatılmadığını takip etmek için bayrak
bool serverStarted = false; 


void setup() {


  pinMode(ENB, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  myservo.attach(5); // Servo motorun bağlı olduğu pin
  dht.begin();
  Serial.begin(9600);
  delay(2000);
  connectToWifi();




  if (WiFi.status() == WL_CONNECTED){


    Blynk.begin(auth, ssid, password);


  }
 


}


void loop() {


  


  if(!serverStarted) {    // server çalışmadıysa
    Blynk.run();
    sendSensor();
    if (!Blynk.connected()) {
    beginServer();
    serverStarted = true;
  } 


  } else {
       server.handleClient();
  }



  if (!apStarted) { // AP modu başlatılmadıysa, normal çalışmaya devam et
    server.handleClient();
    if (WiFi.status() != WL_CONNECTED) {
      // WiFi bağlantısı yoksa AP modunu başlat
      startAPMode();
    }
  } else {
    server.handleClient();
  }
}


void connectToWifi() {
  int attempts = 0;
  while (attempts < 10) {
    WiFi.begin(ssid, password);
    Serial.println("WiFi bağlantısı kuruluyor...");
    
    unsigned long startTime = millis();
    while (millis() - startTime < 3000) { // Her bir deneme arasında 3 saniye bekleyin


      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi bağlandı");
        Serial.println("IP adresi: ");
        Serial.println(WiFi.localIP());
        return;
      }
      delay(100);
    }
    
    attempts++;
    
    if (attempts >= 10) {
      Serial.println("\nFailed to connect to WiFi after 5 attempts. Starting AP mode.");
      startAPMode();
      return;
    }
  }
}





void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 


  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  // Blynk uygulamasındaki sanal pinlere nem ve sıcaklık değerlerini yazdırın
  Blynk.virtualWrite(V4, h); // V4 pinine nem değerini yazdırın (Humidity)
  Blynk.virtualWrite(V3, t); // V3 pinine sıcaklık değerini yazdırın (Temperature)
}


BLYNK_WRITE(V0) {
  int led1State = param.asInt();
  digitalWrite(LED_PIN_1, led1State);
}


BLYNK_WRITE(V1) {
  int led2State = param.asInt();
  digitalWrite(LED_PIN_2, led2State);
}


BLYNK_WRITE(V5) {
  int sliderValue = param.asInt(); // Slider'dan gelen değeri oku (0-255)
  
  analogWrite(ENB, sliderValue); // Motor hızını belirleyen PWM sinyalini gönderir
  
  if (sliderValue == 0) {
    digitalWrite(IN4, LOW); // Motoru durdur
  } else {
    digitalWrite(IN4, HIGH); // Motoru çalıştır
  }
}


BLYNK_WRITE(V2) {
  int servoState = param.asInt();
  if (servoState == 1) {
    myservo.write(85); // Servo 90 dereceye dönüyor
  } else {
    myservo.write(5);  // Servo 0 dereceye dönüyor
  }
}


void beginServer() {
  server.on("/", handleRoot);
  server.on("/setservo", HTTP_POST, handleSetServo); // Servo açısı ayarlama endpoint'i
  server.on("/controlled", handleControlled); // LED kontrolü endpoint'i
  server.on("/motorcontrol", handleMotorControl);
  server.begin();
  Serial.println("HTTP server started");
}


void handleRoot() {
  String page = "<!DOCTYPE html><html lang=en-EN><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  page += "<title>ESP32 WebServer - İlyas Arslan</title>";
  page += "<style> body { background-color: #f2f2f2; font-family: Arial, Helvetica, sans-serif; text-align: center; }</style>";
  page += "<style> .container { display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; }</style>";
  page += "<style> button { margin: 10px; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; }</style>";
  page += "</head><body><div class='container'>";
  page += "<h1>ESP32 WebServer - İlyas ARSLAN</h1>";
  page += "<h3>Yeşil LED: " + ledStateGreen + "</h3>";
  page += "<form action='/controlled' method='POST'>";
  page += "<button name='green' value='1' style='background-color: #4CAF50; color: white;' >AÇ</button>";
  page += "<button name='green' value='0' style='background-color: #f44336; color: white;' >KAPA</button>";
  page += "</form>";
  page += "<h3>Kırmızı LED: " + ledStateRed + "</h3>";
  page += "<form action='/controlled' method='POST'>";
  page += "<button name='red' value='1' style='background-color: #4CAF50; color: white;' >AÇ</button>";
  page += "<button name='red' value='0' style='background-color: #f44336; color: white;' >KAPA</button>";
  page += "</form>";
  page += "<h3>Sıcaklık: " + String(dht.readTemperature()) + " °C</h3>";
  page += "<h3>Nem: " + String(dht.readHumidity()) + " %</h3>";
  page += "<h3>Servo Motor Açısı: " + String(servoAngle) + " °</h3>";
  page += "<form action='/setservo' method='POST'>";
  page += "<button name='servo' value='85'>AÇ</button>";
  page += "<button name='servo' value='5'>KAPA</button>";
  page += "</form>";
  page += "<h3>Motor Kontrolü:</h3>";
  page += "<form action='/motorcontrol' method='POST'>";
  page += "<button name='speed' value='0'>Dur</button>";
  page += "<button name='speed' value='1'>Düşük Hız</button>";
  page += "<button name='speed' value='2'>Orta Hız</button>";
  page += "<button name='speed' value='3'>Yüksek Hız</button>";
  page += "</form>";
  page += "</div></body></html>";
  server.send(200, "text/html", page);
}


void handleControlled() {
  if (server.hasArg("green")) {
    String greenValue = server.arg("green");
    if (greenValue == "1") {
      digitalWrite(GREEN_LED_PIN, HIGH);
      ledStateGreen = "ON";
    } else {
      digitalWrite(GREEN_LED_PIN, LOW);
      ledStateGreen = "OFF";
    }
  }


  if (server.hasArg("red")) {
    String redValue = server.arg("red");
    if (redValue == "1") {
      digitalWrite(RED_LED_PIN, HIGH);
      ledStateRed = "ON";
    } else {
      digitalWrite(RED_LED_PIN, LOW);
      ledStateRed = "OFF";
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}


void handleMotorControl() {
  if (server.hasArg("speed")) {
    int motorSpeed = server.arg("speed").toInt();
    int pwmValue;


if (motorSpeed == 0) {
  pwmValue = 0; // Dur
} else if (motorSpeed == 1) {
  pwmValue = 84; // Düşük Hız
} else if (motorSpeed == 2) {
  pwmValue = 168; // Orta Hız
} else if (motorSpeed == 3) {
  pwmValue = 255; // Yüksek Hız
}
    analogWrite(ENB, pwmValue); // Motor hızını belirleyen PWM sinyalini gönderir
    if (motorSpeed == 0) {
      digitalWrite(IN4, LOW); // Motoru durdur
    } else {
      digitalWrite(IN4, HIGH); // Motoru çalıştır
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}


void handleSetServo() {
  if (server.hasArg("servo")) {
    String servoValue = server.arg("servo");
    servoAngle = servoValue.toInt();
    myservo.write(servoAngle);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}


void startAPMode() {
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  
  Serial.println("AP mode started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  apStarted = true; // AP modu başlatıldı
  beginServer();
}
