#include <ESP8266WiFi.h>
#include <Espalexa.h>
#include <WiFiManager.h>

#define LED_PIN1 5
#define LED_PIN2 4
#define BUTTON_PIN 16

byte lastButtonState = HIGH;
byte ledState = LOW;
unsigned long debounceDuration = 50;
unsigned long lastTimeButtonStateChanged = 0;
unsigned long buttonPressStartTime = 0;
const unsigned long buttonHoldDuration = 5000;
boolean buttonLongPressed = false;

const char* ssid = "xxxx";
const char* password = "xxxx";
String Device_1_Name = "Luz Quarto";
boolean wifiConnected = false;
boolean naoconectar = false;

WiFiEventHandler wifiConnectHandler;

void firstSwitchChanged(uint8_t brightness);
WiFiManager wifiManager;
Espalexa espalexa;

EspalexaDevice* d;

void firstSwitchChanged(uint8_t brightness) {
    byte buttonState = digitalRead(BUTTON_PIN);
    lastButtonState = buttonState;

    if (brightness == 255) {
        ledState = LOW;
        digitalWrite(LED_PIN1, LOW);
        digitalWrite(LED_PIN2, LOW);
    } else {
        ledState = HIGH;
        digitalWrite(LED_PIN1, HIGH);
        digitalWrite(LED_PIN2, HIGH);
    }
}

boolean connectWifi() {
    boolean state = true;
    int i = 0;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Conectando WiFi");
    Serial.print("Conectando...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (i > 20) {
            state = false; 
            break;
        }
        i++;
    }

    if (state) {
        Serial.print("Conectado a ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("ESP MAC Address:  ");
        Serial.println(WiFi.macAddress());
        WiFi.persistent(true);
        WiFi.setAutoReconnect(true);
    } else {
        Serial.println("Conexão falhou!");
    }
    return state;
}

void addDevices() {
    d = new EspalexaDevice(Device_1_Name, firstSwitchChanged);
    espalexa.addDevice(d);
    espalexa.begin();
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {      
    if (!naoconectar) {
        WiFi.begin(ssid, password);
        naoconectar = true;
        espalexa.loop(); 
        uint8_t bri = d->getValue();

        if (ledState == HIGH) {
            if (wifiConnected) {
                d->setValue(0);
            }
        } else {
            if (wifiConnected) {
                d->setValue(255);  
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN1, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    wifiConnected = connectWifi();

    if (wifiConnected) {
        addDevices();
    } else {
        Serial.println("Não é possível conectar ao WiFi!");
        delay(1000);
        Serial.println("Iniciado WiFiManager");
        wifiManager.autoConnect("ESP8266_AP1");
    }

    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
}

void loop() {
    if (wifiConnected) {
        espalexa.loop();
    }

    if (millis() - lastTimeButtonStateChanged > debounceDuration) {
        byte buttonState = digitalRead(BUTTON_PIN);

        if (buttonState != lastButtonState) {
            lastTimeButtonStateChanged = millis();

            if (buttonState == LOW) {
                buttonPressStartTime = millis();
            } else {
                buttonPressStartTime = 0;
                buttonLongPressed = false;
            }

            lastButtonState = buttonState;
        }

        if (buttonState == LOW && !buttonLongPressed) {
            if (millis() - buttonPressStartTime >= buttonHoldDuration) {
                buttonLongPressed = true;
                Serial.println("Botão pressionado por 5 segundos! Inicializando WiFiManager...");
                wifiManager.startConfigPortal("ESP8266_Config");
            }
        }
    }
}
