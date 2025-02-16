#include <Arduino.h>
#include <painlessMesh.h>
#include <WiFi.h>
#include "config.h"

Scheduler userScheduler;
painlessMesh mesh;

// WiFi credentials (for AP mode)
const char* ssid = "ESP32_Mesh_Leader";
const char* password = "12345678";

WiFiServer server(80);

void sendMessage() {
    String msg = "Hello from Leader Node";
    mesh.sendBroadcast(msg);
}

void receivedCallback(uint32_t from, String &msg) {
    Serial.printf("Received from %u: %s\n", from, msg.c_str());
}

void setup() {
    Serial.begin(115200);

    // Start Mesh Network
    mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive(&receivedCallback);

    // Start WiFi AP mode
    WiFi.softAP(ssid, password);
    server.begin();
    Serial.println("WiFi Access Point Started");
    Serial.println(WiFi.softAPIP());
}

void loop() {
    mesh.update();

    WiFiClient client = server.available();
    if (client) {
        String request = client.readStringUntil('\r');
        Serial.println(request);
        client.flush();

        if (request.indexOf("/send") != -1) {
            sendMessage();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println();
            client.println("Message Sent");
        } else {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println();
            client.println("ESP32 Mesh Leader");
        }

        client.stop();
    }
}
