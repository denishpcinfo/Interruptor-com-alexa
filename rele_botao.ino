#include <ESP8266WiFi.h>
#include <Espalexa.h>

#define LED_PIN1 5
#define LED_PIN2 4
#define BUTTON_PIN 16

byte lastButtonState = HIGH;
byte ledState = LOW;
unsigned long debounceDuration = 50;
unsigned long lastTimeButtonStateChanged = 0;
const char* ssid = "House_1";
const char* password = "euvejotudo";
String Device_1_Name = "Luz Quarto";
boolean wifiConnected = false;

void firstSwitchChanged(uint8_t brightness);

Espalexa espalexa;

EspalexaDevice* d;

void firstSwitchChanged(uint8_t brightness){
  
byte buttonState = digitalRead(BUTTON_PIN);
lastButtonState = buttonState;
        
  if (brightness == 255){
      ledState = LOW;
      digitalWrite(LED_PIN1, LOW );
      digitalWrite(LED_PIN2, LOW );
    }else{
      ledState = HIGH;
      digitalWrite(LED_PIN1, HIGH);
      digitalWrite(LED_PIN2, HIGH);
  }
}

boolean connectWifi(){
  
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Conectando WiFi");
  Serial.print("Conectando...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false; break;
    }
    i++;
  }-

  Serial.println("");
  if (state) {
    Serial.print("Conectado a ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("ESP MAC Address:  ");
    Serial.println(WiFi.macAddress());
    WiFi.persistent(true);
    WiFi.setAutoReconnect(true);
  }
  else {
    Serial.println("Conexao falhou!");
  }
  return state;
}

void addDevices(){
  Serial.println("addDevices");
  d = new EspalexaDevice(Device_1_Name, firstSwitchChanged);
  espalexa.addDevice(d);
  espalexa.begin();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

    wifiConnected = connectWifi();

  if (wifiConnected){
    addDevices();
  }
  else{
    Serial.println("Não é possível conectar ao WiFi!");
    delay(1000);
  }
}

void loop() {

    espalexa.loop();
    
  if (millis() - lastTimeButtonStateChanged > debounceDuration) {
    byte buttonState = digitalRead(BUTTON_PIN);
    
    if (buttonState != lastButtonState) {
      lastTimeButtonStateChanged = millis();
      lastButtonState = buttonState;
      if (buttonState == LOW) {
        ledState = (ledState == HIGH) ? LOW: HIGH;
        digitalWrite(LED_PIN1, ledState);
        digitalWrite(LED_PIN2, ledState);
        
        if(ledState == HIGH){
            d->setValue(0);
          }else{
            d->setValue(255);
          }
      }
    }
  }
}
