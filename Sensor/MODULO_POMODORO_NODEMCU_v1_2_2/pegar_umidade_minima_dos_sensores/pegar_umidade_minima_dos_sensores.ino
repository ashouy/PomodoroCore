#define sensor1 36
#define sensor2 39
#define sensor3 34

int a,b,c,indice;
int sensorValue1[7];
int sensorValue2[7];
int sensorValue3[7];
boolean trintaHoras;

void setup() {
  Serial.begin(115200);
  for(int i = 0; i< 7; i++){
    sensorValue1[i] = 5000;
    sensorValue2[i] = 5000;
    sensorValue3[i] = 5000;
  }
  trintaHoras = false;
  indice = 0;
}

void loop() {
  if (trintaHoras == false){
    Serial.println("contando 30 horas");
    delay(108000000);
    trintaHoras = true;
  }
  if(indice < 7){
    a = analogRead(sensor1);
    b = analogRead(sensor2);
    c = analogRead(sensor3);
    
    if(a<sensorValue1[indice]){
      sensorValue1[indice] = a;
    }
    if(b<sensorValue2[indice]){
      sensorValue2[indice] = b;
    }
    if(c<sensorValue3[indice]){
      sensorValue3[indice] = c;
    }
    indice ++;
    }else{
      Serial.println("sensor1");
      for(int j = 0 ; j < 7 ; j++){
        Serial.print(" " + (String)sensorValue1[j]);
      }
      Serial.println("sensor2");
      for(int j = 0 ; j < 7 ; j++){
        Serial.print(" " + (String)sensorValue2[j]);
      }
      Serial.println("sensor3");
      for(int j = 0 ; j < 7 ; j++){
        Serial.print(" " + (String)sensorValue3[j]);
      }
    }
  delay(600000);
}
