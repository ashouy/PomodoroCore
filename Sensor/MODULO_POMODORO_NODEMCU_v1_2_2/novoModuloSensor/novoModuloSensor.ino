
#include <WiFi.h>
#include <PubSubClient.h>

//wifi
const char *ssid =  "JC";   // name of your WiFi network
const char *password =  "password"; // password of the WiFi network

//topic
#define *topicToPublish = "UmidadedeSolo"  // Topic to publish
#define *topicToSubscribe ="Regra" // Topic to subscribe

// Name of our device, must be unique
#define ID "esp32"

//broker
int port = 1883; // port
const char *borkerIP = "test.mosquitto.org"; //mosquitto broker (change later)

WiFiClient wclient;
PubSubClient client(wclient); // Setup MQTT client

//sensores
#define sensor1 36
#define sensro2 39
#define sensor3 34
int selectedSensor, sensorValue, sleepTime;
int inputs[] = {0,1,2};
bool lastMessage;

//leds
#define ledPinMQTTerro 32
#define ledpinWiFierro 33

// quantity of samples
#define numberOfSamples 5

//prototypes
void initMQTT();
void setup_wifi();
void reconnect();
void mqtt_callback(char* topicSubscribed, byte* payload, unsigned int length);
void reconnectMQTT();

//set address n' port
void initMqtt(){
  client.setServer(borkerIP,port);
}

// Connect to WiFi network
void setup_wifi() {
  if (WiFi.status() == WL_CONNECTED){
    return;
  }
  digitalWrite(ledpinWiFierro,HIGH)// on if is not connected
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); // Connect to network
  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
  }
  digitalWrite(ledpinWiFierro,LOW)// off if is not connected
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
    digitalWrite(ledPinMQTTerro,HIGH); // on if is not connected
    if (client.connect(ID)) {
      digitalWrite(ledPinMQTTerro,LOW); // off if is not connected
      Serial.println("connected");
      Serial.print("Publishing to: ");
      Serial.println(TOPIC);
      Serial.println('\n');
      client.subscribe(topicToSubscribe);
    } else {
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
      digitalWrite(ledPinMQTTerro,HIGH); // on if is not connected
    }
  }
}

void verifyConnections(void){
  if(!client.connected()){
    reconnect();
  }
  setup_wifi();
}

void mqtt_callback(char* topicSubscribed, byte* payload, un =signed int length){
  Serial.println(topicSubscribed);
  if(length ==2){
    char p = (char)payload[0];
    sensor = (p-'0')-1;
    String b;
    b += (char)payload[1];
    if(b.equals("0")){
      Serial.println("Bomb Off");
      sleepTime = 7200000000;
    }else{
      Serial.println("Bomb On");
      sleepTime = 3600000000;
    }
    if(!lastMessage){
    lastMesage = true;
    sleepTime = 500;  
  }
  }
}

void request(int selectedSensor){
  sensorValue += analogRead(selectedSensor)
}

void sendOutputState(void){
  for(int i = 0; i<numberOfSamples; i++){
    request(selectedSensor);
  }
  sensorValue = sensorValue/numberOfSamples;
  Serial.print("Average: ");
  Serial.println(sensorValue);

  char sensorValueFormatted[10];
  sprintf(aux, "%d", sensorValue);
  char* pointerValueFormatted = sensorValueFormatted;

  client.publish(topicToPublish,pointerValueFormatted);
  sensorValue =0;

  delay(3000);
}

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud
  sensor = 0;
  lastMessage = false;
  sleepTime = 500000;
  pinMode(ledPinMQTTerro,OUTPUT);
  pinMode(ledpinWiFierro,OUTPUT);
  digitalWrite(ledPinMQTTerro,HIGH);
  digitalWrite(ledpinWiFierro,HIGH);
  setup_wifi(); // Connect to network
  initMqtt();
  
}

void loop() {
   // Reconnect if connection is lost
  verifyConnections();
  
  if(lastMessage){
    sendOutputState();

    //deep sleep here
  }
  delay(2000);
}

//https://www.digikey.com/en/maker/projects/send-and-receive-messages-to-your-iot-devices-using-mqtt/39ed5690cc46473abe8904c8f960341f?utm_adgroup=General&utm_term=&slid=&gclid=CjwKCAiArJjvBRACEiwA-Wiqq_DcwDJ3EqaccZF2aFgkJBe3oi3cYWcfste0hW4MC4N6vyfVWMc2NxoCqDEQAvD_BwE&utm_campaign=Dynamic+Search_EN_Product&utm_medium=cpc&utm_source=google
