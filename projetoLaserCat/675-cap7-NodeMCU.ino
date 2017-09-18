#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define POS_INICIAL 90
#define X_MINIMO    30 // Angulo de X (30 a 150)
#define Y_MINIMO    50 // Angulo de y (50 a 90)
#define X_INTERVALO 12 // (X maximo - X minimo)/10 = 12 
#define Y_INTERVALO 4 // (Y maximo - Y minimo)/10 = 4
#define LASER_TEMPO 60 // Tempo em segundos que o laser ficará acionado
#define LASER       D1
#define PIR         D2

Servo servoX;
Servo servoY;

//Setup WIfi e Mqtt
const char* ssid = "caelum_sala2";
const char* password = "5987cb800f";
const char* mqtt_server = "m10.cloudmqtt.com";
const int mqtt_port = 11480;
const char* mqtt_topic = "monitoracao/lasercat/status";
const char* mqtt_user = "sjoeirvo";
const char* mqtt_password = "xfhLPQ9-eO27";

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  servoX.attach(D3);
  servoY.attach(D4);

  servoX.write(POS_INICIAL);
  servoY.write(POS_INICIAL);

  pinMode(LASER, OUTPUT);
  digitalWrite(LASER, LOW);

  pinMode(PIR, INPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  
  Serial.begin(9600);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  int trigger = digitalRead(PIR);
  client.publish(mqtt_topic, String(trigger).c_str());
  if(trigger == HIGH){
    liga_laser();
    //Serial.println("Detectado");
  }else{
    desliga_laser();
   //Serial.println("NAO Detectado");
  }
 delay(1000);
  
}

//FUNCOES AUXILIARES

void posiciona_servos(){
  int posicaoX = (random(0,(X_INTERVALO))* 10 + (X_MINIMO));
  int posicaoY = (random(0,(Y_INTERVALO))* 10 + (Y_MINIMO));
  servoX.write(posicaoX);
  servoY.write(posicaoY);
  //Serial.println(posicaoX);
  //Serial.print("  ,  ");
  //Serial.println(posicaoY);

}

void liga_laser(){
  for(int i = 0; i<=LASER_TEMPO/2; i++){
   digitalWrite(LASER, HIGH);
   posiciona_servos();
   delay(2000);
    
  }
}

void desliga_laser(){
  digitalWrite(LASER, LOW);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}


