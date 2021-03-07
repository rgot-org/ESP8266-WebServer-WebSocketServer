/*********
 * SERVER WEB / WEBSOCKET pour ESP8266
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

// Replace with your network credentials
const char *ssid = "Point_d_acces";
const char *password = "mot_de_passe";

bool ledState = 0;
const int ledPin = LED_BUILTIN;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients(String type, int value) {
	StaticJsonDocument<200> doc;
	doc["key"] = type;
	doc["value"] = value;
	String json;
	serializeJson(doc, json);
	ws.textAll(json);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
	AwsFrameInfo *info = (AwsFrameInfo*)arg;
	if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
		StaticJsonDocument<200> doc;
		deserializeJson(doc, (char*)data, len);
		if (doc["operation"] == "toggle")
		{
			ledState = !ledState;
			digitalWrite(ledPin, !ledState);
			notifyClients("led", ledState);
		}
		if
			(doc["operation"] == "somme") {
			int nb1 = doc["nb1"];
			int nb2 = doc["nb2"];
			int resultat = nb1 + nb2;
			Serial.printf("nb1 = %d nb2= %d resultat= %d", nb1, nb2, resultat);
			notifyClients("somme", resultat);
		}
	}
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
	void *arg, uint8_t *data, size_t len) {
	switch (type) {
	case WS_EVT_CONNECT:
		Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
		notifyClients("led", ledState);
		break;
	case WS_EVT_DISCONNECT:
		Serial.printf("WebSocket client #%u disconnected\n", client->id());
		break;
	case WS_EVT_DATA:
		handleWebSocketMessage(arg, data, len);
		break;
	case WS_EVT_PONG:
	case WS_EVT_ERROR:
	default:
		break;
	}
}

void initWebSocket() {
	ws.onEvent(onEvent);
	server.addHandler(&ws);
}

String processor(const String& var) {
	Serial.println(var);
	if (var == "STATE") {
		if (ledState) {
			return "ON";
		}
		else {
			return "OFF";
		}
	}
}

void setup() {
	// Serial port for debugging purposes
	Serial.begin(115200);
	Serial.println();
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);
	if (!SPIFFS.begin())
	{
		Serial.println("Erreur SPIFFS...");
		return;
	}

	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
		// recupere le nom du fichier | get filename
		Serial.print(dir.fileName());
		Serial.print(" - ");
		// et sa taille | and the size
		if (dir.fileSize()) {
			File f = dir.openFile("r");
			Serial.println(f.size());
			f.close();
		}
		else {
			Serial.println("file is empty");
		}
	}

	// Connect to Wi-Fi
	WiFi.begin(ssid, password);
	Serial.print("Tentative de connexion...");

	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(100);
	}

	Serial.println("\n");
	Serial.println("Connexion etablie!");
	Serial.print("Adresse IP: ");
	Serial.println(WiFi.localIP());

	initWebSocket();

	server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
	// Start server
	server.begin();
}

void loop() {
	ws.cleanupClients();

}
