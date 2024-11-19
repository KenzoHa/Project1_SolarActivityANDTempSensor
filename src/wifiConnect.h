/*
Bronnen: 
eerdere casusopdracht wifi
- https://canvas.kdg.be/courses/49815/pages/wi-fi-basisfuncties?module_item_id=1098880
- eerdere opdracht tijdens hoorcollege WiFi 
- Anthropic AI (2024) - Claude 3.5 Sonnet - https://claude.ai
*/

#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"


// Function declarations
void WiFiEvent(WiFiEvent_t event);
void wifiInit();
void searchDevices();
void wifiReconnect(WiFiEvent_t event, WiFiEventInfo_t info);
void stopProgram();

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
    case ARDUINO_EVENT_WIFI_READY:               Serial.println("WiFi interface ready"); break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:           Serial.println("Completed scan for access points"); break;
    case ARDUINO_EVENT_WIFI_STA_START:           Serial.println("WiFi client started"); break;
    case ARDUINO_EVENT_WIFI_STA_STOP:            Serial.println("WiFi clients stopped"); break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:       Serial.println("Connected to access point"); break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:    Serial.println("Disconnected from WiFi access point"); break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE: Serial.println("Authentication mode of access point has changed"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:        Serial.println("Lost IP address and IP address is reset to 0"); break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:          Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_FAILED:           Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:          Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_PIN:              Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode"); break;
    case ARDUINO_EVENT_WIFI_AP_START:           Serial.println("WiFi access point started"); break;
    case ARDUINO_EVENT_WIFI_AP_STOP:            Serial.println("WiFi access point  stopped"); break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:    Serial.println("Client connected"); break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: Serial.println("Client disconnected"); break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:   Serial.println("Assigned IP address to client"); break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:  Serial.println("Received probe request"); break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:         Serial.println("AP IPv6 is preferred"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:        Serial.println("STA IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_GOT_IP6:             Serial.println("Ethernet IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_START:               Serial.println("Ethernet started"); break;
    case ARDUINO_EVENT_ETH_STOP:                Serial.println("Ethernet stopped"); break;
    case ARDUINO_EVENT_ETH_CONNECTED:           Serial.println("Ethernet connected"); break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:        Serial.println("Ethernet disconnected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:              Serial.println("Obtained IP address"); break;
    default:                                    break;
  }
}


// Configure ESP32 as Station + AP
void wifiInit() {
    WiFi.mode(WIFI_AP_STA);  // Set ESP32 as station AND access point
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);    
    WiFi.setHostname(HOSTNAME);
    
    // Start AP
    WiFi.softAP("KenzoH_ESP32-AP", PASSWORD);
    
    // Connect to WiFi network
    WiFi.begin(SSID, PASSWORD);
    Serial.println("Connecting to WiFi...");
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("AP IP Address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("\nFailed to connect to WiFi.");
    }
}

void searchDevices(){
    Serial.println("Scanning for networks...");
    int numNetworks = WiFi.scanNetworks();        //after scan, generate a number to see how many networks are available
  
    if (numNetworks == 0) {
        Serial.println("No networks found");
    } else {
        Serial.print(numNetworks);
        Serial.println(" networks found");
    
        for (int element = 0; element < numNetworks; ++element) {   //initializes a counter variable named element and sets it to 0 ; keep looping as long as element is less than numNetworks ; increments the element variable by 1
            Serial.print(element + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(element));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(element));
            Serial.print(")");
            //Serial.println((WiFi.encryptionType(element) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
  
  Serial.println("");
  delay(5000); // Wait for 5 seconds before scanning again
}

void stopProgram() {
    while(true) {
        Serial.println("Failed to reconnect. Check connection details and restart");
        delay(1000);
    }
}

void wifiReconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("Disconnected from WiFi, attempting to reconnect...");
    WiFi.disconnect(true);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        WiFi.begin(SSID, PASSWORD);
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nReconnected successfully!");
    } else {
        Serial.println("\nFailed after 10 attempts.");
        stopProgram(); // Stop the program after 10 failed attempts
    }
}