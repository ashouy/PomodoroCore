/*---------------------------------------------------
                MÓDULO SENSOR POMODRO 1
                AUTHOR: JOFFREY PEYRAC
                        v. 1.2.2
          Modifcado em: 19/10/2018 (dd/mm/YYYY)
  ----------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<Wire.h>

//Topico para publicar
#define TOPICO_PUBLISH_UMIDADE "UmidadeSolo"
#define TOPICO_SUBSCRIBE "Regra"

//id unico, em outros dispositos tem que ter outro nome
#define ID_MQTT "NodeMCU"

//pino adc
#define pinoSensor 0
//numero de amostras a serem coletadas
#define amostras 5

//pino de leds
#define pinoledMQTTerro 14 //(D5)
#define pinoledWifiErro 12 //(D6)

//WIFI
const char* ssid = "Rpi3-AP";
const char* password = "raspberry";

//MQTT
//endereço do broker
const char* BROKER_MQTT = "192.168.50.1";
int BROKER_PORT = 1883;

//variaveis e objetos globais
WiFiClient espClient;
//instancia o cliente mqtt passando o objeto espClient
PubSubClient MQTT(espClient);

//PROTOTYPES
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);

int sensor, valorSensor, sleepTime;
int entradas[] = {0,1,2};
bool ultimaMenssagem;

void setup() {
  Serial.begin(115200);
  //inicializações
  //sleepTime = 500;
  sleepTime = 500000;
  sensor = 0;
  ultimaMenssagem = false;
  /*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  SLEEP TIME SETADO EM CODIGO, EM BREVE TRATAR ISSO
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  salvar na eeprom ou pegar do mqtt(?)
  ou função de primeira atividade para entao fazer o loop?
  muitas duvidas...
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
  pinMode(pinoledMQTTerro, OUTPUT);
  pinMode(pinoledWifiErro, OUTPUT);
  digitalWrite(pinoledMQTTerro, HIGH);
  digitalWrite(pinoledWifiErro, HIGH);
  Wire.begin(D1, D2);
  //initSerial(); //init serial monitor, not necessary now
  initWiFi();
  initMQTT();

}
//called on setup()
void initWiFi() {
  delay(10);
  Serial.println("Conectando a rede");
  reconectaWiFi();
}

//reconecta ao wifi
void reconectaWiFi() {
  //se ja esta conectado, nada é feito
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  digitalWrite(pinoledWifiErro, HIGH); //LIGA SE ESTIVER DESCONECTADO
  WiFi.begin(ssid, password); //conecta ao wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  digitalWrite(pinoledWifiErro, LOW); //DESLIGA SE CONECTAR
  Serial.print("Conectou!");
}

//called on setup()
//init parametros de conexao mqtt(endereço do broker, porta e seta função de callback)
void initMQTT() {
  //informa o endereco e a porta
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  /*atribui função de callback (função chamada quando qualquer
    informação de um dos tópicos subescritos chega)*/
  MQTT.setCallback(mqtt_callback); //NOT NECESARY RIGHT NOW
}

void mqtt_callback(char* topic, byte* payload, unsigned int length){
  //Serial.println("lendo menssagem do topico inscrito");
  //Serial.print("tamanho da menssagem: ");
  Serial.println(topic);
  if(length==2){
    char p = (char)payload[0];
    //Serial.println((p-'0')-1);
    sensor = (p-'0')-1;
    String b;
    b += (char)payload[1];
    if(b.equals("0")){
      Serial.println("Bomba Desligada");
      sleepTime = 7200000000;
    }else{
      Serial.println("Bomba Ligada");
      sleepTime = 3600000000;
    }
  }
  if(!ultimaMenssagem){
    ultimaMenssagem = true;
    sleepTime = 500;  
  }
}

/*função reconecta ao broker mqtt, caso ainda nao esteja conectado
   ou em caso de a conexao ter caido, em caso de sucesso na
   conexão ou reconexão, o subscribe dos topicos é refeito.*/
void reconnectMQTT() {
  while (!MQTT.connected()) {
    //LIGA O LED SE A NÃO HOUVER CONEXÃO COM O MQTT
    digitalWrite(pinoledMQTTerro, HIGH);
    if (MQTT.connect(ID_MQTT, "JoffrMQTT", "mosquito")) {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      //DESLIGA SE CONSEGUIR ESTABELECR CONEXAO COM O BROKER
      digitalWrite(pinoledMQTTerro, LOW);
      MQTT.subscribe(TOPICO_SUBSCRIBE);
    } else {
      digitalWrite(pinoledMQTTerro, HIGH);
      delay(2000);
    }
  }
}

void VerificaConexoesWiFiEMQTT(void) {
  if (!MQTT.connected()) {
    reconnectMQTT(); // se nao ha conexao com broker a conexao e refeita
  }
  reconectaWiFi(); // se nao ha conexao com o wifi...
}

//funcao que realiza comunicacao com arduino
void solicitacao(int s){
  Wire.beginTransmission(8);
  Wire.write(entradas[s]);
  Wire.endTransmission();

  Wire.requestFrom(8,13);
  if(Wire.available()>=2){
    int iRXVal;
    for (int i = 0; i < 2; i++){             // Receive and rebuild the 'int'.
      iRXVal += Wire.read() << (i * 8);   //    "     "     "     "    "
    }
    valorSensor += iRXVal;
  }else{
    
  }
  delay(1000);
}

//função envia ao broker o estado atual do output
void EnviaEstadoOutput(void) {
  //Serial.println();
  //Serial.print("lendo o sesnor: ");
  //Serial.println(sensor);
  for(int l=0; l<amostras; l++){
    solicitacao(sensor);  
    //Serial.print(" .");
  }
  
  valorSensor = valorSensor/amostras;

  Serial.print("Media: ");
  Serial.println(valorSensor);
  /*int valor = map(sensorSolo, 300, 1024, 100, 0);
  if (valor>100){
    valor = 100;
  }*/
  char aux[8];
  sprintf(aux, "%d", valorSensor);
  char* valor = aux;
  //=======================================
  /*XXXXXXXXXXXXXXXXXXXXXXX CODIGO DE TESTE XXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  se sleep time tiver curto significa que a bomba ta ligada
  manda um numeor alto pra desligar*/
  
  /*
  if(sleepTime == 1000){
    MQTT.publish(TOPICO_PUBLISH_UMIDADE, "55");
  }else{
    //se o sleep time tiver alto significa que a bomba ta desligada
    //manda um numero baixo pra ligar
    MQTT.publish(TOPICO_PUBLISH_UMIDADE, "25");
  }
  */
  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  MQTT.publish(TOPICO_PUBLISH_UMIDADE, valor);
  valorSensor=0;
  //  Serial.println("Dado enviado ao broker!");
  delay(3000);
}

void loop() {
  //garante funcionamento das conecoes wifi e ao broker mqtt
  VerificaConexoesWiFiEMQTT();
    //Serial.println("oi");

  if(ultimaMenssagem){
    //envia a leitura do sensor para o broker no protocolo esperado
    EnviaEstadoOutput();
    ESP.deepSleep(sleepTime);
  }
  //keep-alive da comunicação com o broker mqtt
  MQTT.loop();  
  //Serial.println("tchau");
  //aqui tem  segundo em microssegundos
  //ESP.deepSleep(1 * 5000000);
  //Serial.println(sleepTime);
  //delay(sleepTime);
  delay(1000);
}
