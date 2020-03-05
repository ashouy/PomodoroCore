#define sensor1 36
#define sensor2 39
#define sensor3 34

int a,b,c,indice;
int sensorValue1[100];
int sensorValue2[100];
int sensorValue3[100];

void setup() {
  Serial.begin(115200);
  for(int i = 0; i< 7; i++){
    sensorValue1[i] = 5000;
    sensorValue2[i] = 5000;
    sensorValue3[i] = 5000;
  }

  indice = 0;
}

void loop() {

  if(indice < 100){
    a = analogRead(sensor1);
    b = analogRead(sensor2);
    c = analogRead(sensor3);
    
    
    sensorValue1[indice] = a;

    sensorValue2[indice] = b;

    sensorValue3[indice] = c;
    
    indice ++;
    }
    else{
      Serial.println("sensor1");
      for(int j = 0 ; j < 100 ; j++){
        Serial.print(" " + (String)sensorValue1[j]);
      }
      Serial.println(" ");

      Serial.println("sensor2");
      for(int j = 0 ; j < 100 ; j++){
        Serial.print(" " + (String)sensorValue2[j]);
      }
      Serial.println(" ");

      Serial.println("sensor3");
      for(int j = 0 ; j < 100 ; j++){
        Serial.print(" " + (String)sensorValue3[j]);
      }
    }
  delay(30000);
}
