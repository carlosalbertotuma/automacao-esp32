#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Rede"; // rede Wifi
const char* password = "SENHA-WIFI"; // Senha wifi
const char* adminPassword = "SENHA-PAGINA";  // Senha para acessar a página

WebServer server(80);  // Declare the server object

// Pinos dos relés
const int numRelays = 10;  // Número total de relés
int relayPins[numRelays] = {12, 13, 14, 15, 16, 17, 18, 19, 21, 22};  // Pinos dos relés
String relayNames[numRelays] = {"Sala", "Quarto", "Cozinha", "Banheiro", "Garagem", "Jardim", "Pátio", "Escritório", "Sótão", "Porão"};

bool relayStates[numRelays];  // Estado inicial dos relés

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial

  // Conecta-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  
  Serial.println("Conectado ao WiFi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Inicializa os pinos dos relés como saída
  for (int i = 0; i < numRelays; i++) {
    pinMode(relayPins[i], OUTPUT);
  }

  // Rota para servir a página da web
  server.on("/", HTTP_GET, handleRoot);

  // Rota para ligar/desligar os relés
  for (int i = 0; i < numRelays; i++) {
    server.on("/relay" + String(i + 1), HTTP_GET, [i]() {
      toggleRelay(i);
      server.send(200, "text/html", getPage());
    });
  }

  // Inicializa o servidor
  server.begin();
}

String getPage() {
  String page = "<html><head><meta charset='UTF-8'></head><body>";
  page += "<h1>Controle de Relés</h1>";
  for (int i = 0; i < numRelays; i++) {
    page += "<p>" + relayNames[i] + ": <a href='/relay" + String(i + 1) + "'>";
    page += relayStates[i] ? "<button style='background-color: green; color: white;'>Ligado</button>" : "<button style='background-color: red; color: white;'>Desligado</button>";
    page += "</a></p>";
  }
  page += "</body></html>";
  return page;
}

void handleRoot() {
  if (!server.authenticate("admin", adminPassword)) {
    return server.requestAuthentication();
  }
  server.send(200, "text/html", getPage());
}

void toggleRelay(int relayIndex) {
  relayStates[relayIndex] = !relayStates[relayIndex];  // Inverte o estado do relé
  digitalWrite(relayPins[relayIndex], relayStates[relayIndex] ? HIGH : LOW);  // Define o estado do relé
}

void loop() {
  // Lida com as solicitações dos clientes
  server.handleClient();
}