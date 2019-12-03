/*---------------------------------------------------
                MÓDULO SENSOR POMODRO 1
                AUTHOR: JOFFREY PEYRAC
                        v. 1.2.2
          Modifcado em: 19/10/2018 (dd/mm/YYYY)
  ----------------------------------------------------*/

//#include <PubSubClient.h>
//#include<Wire.h>
////teste
////Topico para publicar
//#define TOPICO_PUBLISH_UMIDADE "UmidadeSolo"
//#define TOPICO_SUBSCRIBE "Regra"
//
////id unico, em outros dispositos tem que ter outro nome
//#define ID_MQTT "NodeMCU"
//
////pino adc
//#define pinoSensor 0
////numero de amostras a serem coletadas
//#define amostras 5
//
////pino de leds
//#define pinoledMQTTerro 14 //(D5)
//#define pinoledWifiErro 12 //(D6)
//
////WIFI
//const char* ssid = "Rpi3-AP";
//const char* password = "raspberry";
//
////MQTT
////endereço do broker
//const char* BROKER_MQTT = "192.168.50.1";
//int BROKER_PORT = 1883;
//
////variaveis e objetos globais
//WiFiClient espClient;
////instancia o cliente mqtt passando o objeto espClient
//PubSubClient MQTT(espClient);
//
////PROTOTYPES
//void initSerial();
//void initWiFi();
//void initMQTT();
//void reconectWiFi();
//void mqtt_callback(char* topic, byte* payload, unsigned int length);
//void VerificaConexoesWiFIEMQTT(void);
//
//int sensor, valorSensor, sleepTime;
//int entradas[] = {0,1,2};
//bool ultimaMenssagem;
//
//void setup() {
//  Serial.begin(115200);
//  //inicializações
//  //sleepTime = 500;
//  sleepTime = 500000;
//  sensor = 0;
//  ultimaMenssagem = false;
//  /*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//  SLEEP TIME SETADO EM CODIGO, EM BREVE TRATAR ISSO
//  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//  salvar na eeprom ou pegar do mqtt(?)
//  ou função de primeira atividade para entao fazer o loop?
//  muitas duvidas...
//  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
//  pinMode(pinoledMQTTerro, OUTPUT);
//  pinMode(pinoledWifiErro, OUTPUT);
//  digitalWrite(pinoledMQTTerro, HIGH);
//  digitalWrite(pinoledWifiErro, HIGH);
//  Wire.begin(D1, D2);
//  //initSerial(); //init serial monitor, not necessary now
//  initWiFi();
//  initMQTT();
//
//}
////called on setup()
//void initWiFi() {
//  delay(10);
//  Serial.println("Conectando a rede");
//  reconectaWiFi();
//}
//
////reconecta ao wifi
//void reconectaWiFi() {
//  //se ja esta conectado, nada é feito
//  if (WiFi.status() == WL_CONNECTED) {
//    return;
//  }
//  digitalWrite(pinoledWifiErro, HIGH); //LIGA SE ESTIVER DESCONECTADO
//  WiFi.begin(ssid, password); //conecta ao wifi
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(100);
//    Serial.print(".");
//  }
//  digitalWrite(pinoledWifiErro, LOW); //DESLIGA SE CONECTAR
//  Serial.print("Conectou!");
//}
//
////called on setup()
////init parametros de conexao mqtt(endereço do broker, porta e seta função de callback)
//void initMQTT() {
//  //informa o endereco e a porta
//  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
//  /*atribui função de callback (função chamada quando qualquer
//    informação de um dos tópicos subescritos chega)*/
//  MQTT.setCallback(mqtt_callback); //NOT NECESARY RIGHT NOW
//}
//
//void mqtt_callback(char* topic, byte* payload, unsigned int length){
//  //Serial.println("lendo menssagem do topico inscrito");
//  //Serial.print("tamanho da menssagem: ");
//  Serial.println(topic);
//  if(length==2){
//    char p = (char)payload[0];
//    //Serial.println((p-'0')-1);
//    sensor = (p-'0')-1;
//    String b;
//    b += (char)payload[1];
//    if(b.equals("0")){
//      Serial.println("Bomba Desligada");
//      sleepTime = 7200000000;
//    }else{
//      Serial.println("Bomba Ligada");
//      sleepTime = 3600000000;
//    }
//  }
//  if(!ultimaMenssagem){
//    ultimaMenssagem = true;
//    sleepTime = 500;  
//  }
//}
//
///*função reconecta ao broker mqtt, caso ainda nao esteja conectado
//   ou em caso de a conexao ter caido, em caso de sucesso na
//   conexão ou reconexão, o subscribe dos topicos é refeito.*/
//void reconnectMQTT() {
//  while (!MQTT.connected()) {
//    //LIGA O LED SE A NÃO HOUVER CONEXÃO COM O MQTT
//    digitalWrite(pinoledMQTTerro, HIGH);
//    if (MQTT.connect(ID_MQTT, "JoffrMQTT", "mosquito")) {
//      Serial.println("Conectado com sucesso ao broker MQTT!");
//      //DESLIGA SE CONSEGUIR ESTABELECR CONEXAO COM O BROKER
//      digitalWrite(pinoledMQTTerro, LOW);
//      MQTT.subscribe(TOPICO_SUBSCRIBE);
//    } else {
//      digitalWrite(pinoledMQTTerro, HIGH);
//      delay(2000);
//    }
//  }
//}
//
//void VerificaConexoesWiFiEMQTT(void) {
//  if (!MQTT.connected()) {
//    reconnectMQTT(); // se nao ha conexao com broker a conexao e refeita
//  }
//  reconectaWiFi(); // se nao ha conexao com o wifi...
//}
//
////funcao que realiza comunicacao com arduino
//void solicitacao(int s){
//  Wire.beginTransmission(8);
//  Wire.write(entradas[s]);
//  Wire.endTransmission();
//
//  Wire.requestFrom(8,13);
//  if(Wire.available()>=2){
//    int iRXVal;
//    for (int i = 0; i < 2; i++){             // Receive and rebuild the 'int'.
//      iRXVal += Wire.read() << (i * 8);   //    "     "     "     "    "
//    }
//    valorSensor += iRXVal;
//  }else{
//    
//  }
//  delay(1000);
//}
//
////função envia ao broker o estado atual do output
//void EnviaEstadoOutput(void) {
//  //Serial.println();
//  //Serial.print("lendo o sesnor: ");
//  //Serial.println(sensor);
//  for(int l=0; l<amostras; l++){
//    solicitacao(sensor);  
//    //Serial.print(" .");
//  }
//  
//  valorSensor = valorSensor/amostras;
//
//  Serial.print("Media: ");
//  Serial.println(valorSensor);
//  /*int valor = map(sensorSolo, 300, 1024, 100, 0);
//  if (valor>100){
//    valor = 100;
//  }*/
//  char aux[8];
//  sprintf(aux, "%d", valorSensor);
//  char* valor = aux;
//  //=======================================
//  /*XXXXXXXXXXXXXXXXXXXXXXX CODIGO DE TESTE XXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//  se sleep time tiver curto significa que a bomba ta ligada
//  manda um numeor alto pra desligar*/
//  
//  /*
//  if(sleepTime == 1000){
//    MQTT.publish(TOPICO_PUBLISH_UMIDADE, "55");
//  }else{
//    //se o sleep time tiver alto significa que a bomba ta desligada
//    //manda um numero baixo pra ligar
//    MQTT.publish(TOPICO_PUBLISH_UMIDADE, "25");
//  }
//  */
//  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//  MQTT.publish(TOPICO_PUBLISH_UMIDADE, valor);
//  valorSensor=0;
//  //  Serial.println("Dado enviado ao broker!");
//  delay(3000);
//}
//
//void loop() {
//  //garante funcionamento das conecoes wifi e ao broker mqtt
//  VerificaConexoesWiFiEMQTT();
//    //Serial.println("oi");
//
//  if(ultimaMenssagem){
//    //envia a leitura do sensor para o broker no protocolo esperado
//    EnviaEstadoOutput();
//    ESP.deepSleep(sleepTime);
//  }
//  //keep-alive da comunicação com o broker mqtt
//  MQTT.loop();  
//  //Serial.println("tchau");
//  //aqui tem  segundo em microssegundos
//  //ESP.deepSleep(1 * 5000000);
//  //Serial.println(sleepTime);
//  //delay(sleepTime);
//  delay(1000);
//}
#include <WiFi.h>
#include <PubSubClient.h>

//wifi
const char *ssid =  "WiFi network";   // name of your WiFi network
const char *password =  "password"; // password of the WiFi network

//topico
const char *TOPIC = "helloWorld";  // Topic to subcribe to

const char *ID = "esp32";  // Name of our device, must be unique

//broker
const char *port = 1883 // port
const char *borkerIP = "465464" //mosquitto broker (change later)

WiFiClient wclient;
PubSubClient client(wclient); // Setup MQTT client

//prototypes
void initMQTT();
void setup_wifi();
void reconnect();

// Connect to WiFi network
void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to network

  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to client
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID)) {
      Serial.println("connected");
      Serial.print("Publishing to: ");
      Serial.println(TOPIC);
      Serial.println('\n');

    } else {
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//set address n' port
void initMqtt(){
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud
  delay(100);
  setup_wifi(); // Connect to network
  initMqtt();
}

void loop() {
   // Reconnect if connection is lost
  if (!client.connected()){
    reconnect();
  }
  
  client.loop();
  client.publish(TOPIC, "hello word");
  Serial.println((String)TOPIC);
  delay(2000)
}

//https://www.digikey.com/en/maker/projects/send-and-receive-messages-to-your-iot-devices-using-mqtt/39ed5690cc46473abe8904c8f960341f?utm_adgroup=General&utm_term=&slid=&gclid=CjwKCAiArJjvBRACEiwA-Wiqq_DcwDJ3EqaccZF2aFgkJBe3oi3cYWcfste0hW4MC4N6vyfVWMc2NxoCqDEQAvD_BwE&utm_campaign=Dynamic+Search_EN_Product&utm_medium=cpc&utm_source=google
