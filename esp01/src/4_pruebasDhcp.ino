
#include   "Configuracion_def.h"
#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>
#include <MQTT.h>
// La velocidad depende del modelo de ESP-01
// siendo habituales 9600 y 115200
WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;
bool estadoPlayload = false;
bool estadoEnviar = false;
int estado = -1;

IPAddress ip(192, 168, 1, ***);  // cambiar por la deseada
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void connect() {

  delay(1000);
#ifdef EXC_DEBUG_MODE
  Serial.print("checking wifi...");
#endif

  while (WiFi.status() != WL_CONNECTED) {
#ifdef EXC_DEBUG_MODE
    Serial.print(".");
#endif
    delay(1000);
  }
#ifdef EXC_DEBUG_MODE
  Serial.print("\nconnecting...");
#endif

  while (!client.connect("arduino", "try", "try")) {
#ifdef EXC_DEBUG_MODE
    Serial.print(".");
#endif
    delay(1000);
  }
#ifdef EXC_DEBUG_MODE
  Serial.println("\nconnected!");
#endif

  client.subscribe(Topic);
}

void messageReceived(String &topic, String &payload) {
#ifdef EXC_DEBUG_MODE
  Serial.println("incoming: " + topic + " - " + payload);
#endif
  if (payload == "1" && estadoPlayload == false) {
    estadoPlayload = true;
    Serial.print("portero encendido\n");
  }

  else if (payload == "0" && estadoPlayload == true) {
    estadoPlayload = false;
    Serial.print("portero apagado\n");
  }
}

void setup() {
  // Modo de los pines
  //  pinMode(pinRele, OUTPUT);
  Serial.begin(9600);
#ifdef EXC_DEBUG_MODE
  Serial.println("probando");
#endif
setup_wifiFija();
}


  void setup_wifiFija(){
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
#ifdef EXC_DEBUG_MODE
  Serial.print("Conectando a:\t");
  Serial.println(ssid);
#endif

  // Esperar a que nos conectemos
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
#ifdef EXC_DEBUG_MODE
    Serial.print('.');
#endif
  }



    
  }
  /*
  void setup_wifi() {  //dhcp

  delay(10);
  // We start by connecting to a WiFi network
#ifdef EXC_DEBUG_MODE
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef EXC_DEBUG_MODE
    Serial.print(".");
#endif
  }

  randomSeed(micros());
#ifdef EXC_DEBUG_MODE
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

  // Mostrar mensaje de exito y dirección IP asignada
#ifdef EXC_DEBUG_MODE
  Serial.println("Conexión establecida");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
#endif
  client.begin(ServerIp, Port, net);
  client.onMessage(messageReceived);

  connect();
}*/

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  leerSerial();
  confirmarEstados();
}


void leerSerial () {
  String activa = "activaPorteroMqtt\n";
  String desactiva = "desactivaPorteroMqtt\n";

  if (Serial.available() > 0) {
    String entrada = Serial.readString ();
    if (activa.equals(entrada)) {
      estadoEnviar = true;
      estado = 1;
#ifdef EXC_DEBUG_MODE
      Serial.println("portSim enciende portero envio a server ");
#endif
    }
    if (desactiva.equals(entrada)) {
      estadoEnviar = true;
      estado = 0;
#ifdef EXC_DEBUG_MODE
      Serial.println("portSim apaga portero envio a server ");
#endif
    }
  }
}
void confirmarEstados() {

  if (estadoEnviar == true) {     // orden enviar
    if ((estado == 0) && (estadoPlayload == true)) {   // orden apagar  y estado encendido
      client.publish(Topic, "0");              // apagamos
#ifdef EXC_DEBUG_MODE
      Serial.println("envio al server apagar");
#endif
    }
    else if ( (estado == 1) && (estadoPlayload == false)) {  // orden apagar  y estado encendido
      client.publish(Topic, "1");              // apagamos
#ifdef EXC_DEBUG_MODE
      Serial.println("envio al server encender");
#endif
    }
    else {
      estadoEnviar = false;
    }
  }
}
