'''---------------------------------------------------
                    POMODORO CORE
                AUTHOR: JOFFREY PEYRAC
                        v. 1.0
        Ultima revisão em: 26/11/2019 (dd/mm/YYYY)
  ----------------------------------------------------'''
import paho.mqtt.client as mqtt
import sys
import RPi.GPIO as GPIO
#!/usr/bin/python
import MySQLdb
import time as t

rele = 40
faseled = [38,36,35]
#38 36 35

#tolerancia dos 3sensores
minimo = [60,60,60]
ideal = [80,80,80]

GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)
GPIO.cleanup()
t.sleep(1)
GPIO.setup(rele, GPIO.OUT)
GPIO.output(rele, GPIO.LOW)


GPIO.setup(faseled[0], GPIO.OUT)
GPIO.setup(faseled[1], GPIO.OUT)
GPIO.setup(faseled[2], GPIO.OUT)

GPIO.output(faseled[0], GPIO.HIGH)
GPIO.output(faseled[1], GPIO.LOW)
GPIO.output(faseled[2], GPIO.LOW)

#variaveis globais
#umidadeMaxima = [337, 459, 566]
temporada = 1
diaspassados = 0
horaspassados = 0
tempoInt = 21
tempoIrr = 3
isAuto = False
isSensor = False
bomba = False

#definicoes Broker
Broker = "192.168.50.1"
PortaBroker = 1883
KeepAliveBroker = 60

#Topicos queo sistema assinara
TopicoSubscriber = "UmidadeSolo"
TopicoTemporada = "Temporada"
TopicoDia = "Dia"
TopicoVerificaDia = "VerificaDia"
TopicoManual = "Manual"
TopicoBomba = "Bomba"
TopicoIdeal = "Ideal"
TopicoMinimo = "Minimo"
TopicoSensor = "Sensor"
TopicoIrrigacao = "TempoIrrigacao"
TopicoIntervalo = "TempoIntervalo"
TopicoHora = "Hora"

#Callback conexao ao broker
def on_connect(client, userdata, flags, rc):
    client.subscribe(TopicoSubscriber)
    client.subscribe(TopicoTemporada)
    client.subscribe(TopicoDia)
#    client.subscribe(TopicoVerificaDia)
    client.subscribe(TopicoManual)
    client.subscribe(TopicoBomba)
    client.subscribe(TopicoIdeal)
    client.subscribe(TopicoMinimo)
    client.subscribe(TopicoSensor)
    client.subscribe(TopicoIrrigacao)
    client.subscribe(TopicoIntervalo)
    client.subscribe(TopicoHora)

#Callback menssagem recebida
def on_message(client, userdata, msg):
    
    global temporada
    global diaspassados
    global isAuto
    global isSensor
    global horaspassados
    global bomba
    global tempoIrr
    global tempoInt
    
    MenssagemRecebida = str(msg.payload)
    
    if msg.topic == TopicoSubscriber:
        #save_database(MenssagemRecebida) #salva no banco
        #print("salvou no banco")
        numflut = float(MenssagemRecebida)
        tolerancia = Tolerancia(temporada, numflut)
        save_database(tolerancia)
        if(isAuto):
            if(isSensor):
                if(tolerancia <= ideal[temporada-1]):
                    client.publish("Regra", str(temporada)+"1", qos=0, retain=True)
                    bomba = True
                    EstadoBomba(bomba)
            
                elif (tolerancia >= minimo[temporada-1]):
                    client.publish("Regra", str(temporada)+"0", qos=0, retain=True)
                    bomba = False
                    EstadoBomba(bomba)
        else:
            client.publish("Regra", str(temporada)+"0", qos=0, retain=True)
                
    elif msg.topic == TopicoTemporada:
        temporada = int(MenssagemRecebida);
        #print("topico temporada")
        if temporada == 2:
            GPIO.output(faseled[0], GPIO.LOW)
            GPIO.output(faseled[1], GPIO.HIGH)
            GPIO.output(faseled[2], GPIO.LOW)

        elif temporada == 3:
            GPIO.output(faseled[0], GPIO.LOW)
            GPIO.output(faseled[1], GPIO.LOW)
            GPIO.output(faseled[2], GPIO.HIGH)
        else:
            temporada = 1
            GPIO.output(faseled[0], GPIO.HIGH)
            GPIO.output(faseled[1], GPIO.LOW)
            GPIO.output(faseled[2], GPIO.LOW)
            
        diaspassados = 1
        client.publish(TopicoVerificaDia, diaspassados, qos=0, retain=True)
        mudouTemporada = 1
        print("passou")

    elif msg.topic == TopicoDia:
        #print("topico dia")
        if MenssagemRecebida == "NOVO":
            diaspassados+=1
        else:
            diaspassados = int(MenssagemRecebida)
        client.publish(TopicoVerificaDia, diaspassados, qos=0, retain=True)
        NovaTemporada()
        
    elif msg.topic == TopicoManual:
        if MenssagemRecebida == "1":
            isAuto = False
        elif MenssagemRecebida == "0":
            isAuto = True
            
    elif msg.topic == TopicoBomba:
        if MenssagemRecebida == "1":
            GPIO.output(rele,GPIO.HIGH)
            bomba = True
        elif MenssagemRecebida == "0":
            GPIO.output(rele,GPIO.LOW)
            bomba = False
        horaspassados = 0
        
    elif msg.topic == TopicoIdeal:
        num = float(MenssagemRecebida[1:])
        i = int(MenssagemRecebida[0])
        ideal[i-1] = num

    elif msg.topic == TopicoMinimo:
        num = float(MenssagemRecebida[1:])
        i = int(MenssagemRecebida[0])
        minimo[i-1] = num

    elif msg.topic == TopicoSensor:
        if MenssagemRecebida == "1":
            isSensor = True
            #print("Sensor Ligado")
        elif MenssagemRecebida == "0":
            isSensor = False
            #print("Sensor Desligado")

    elif msg.topic == TopicoIrrigacao:
        tempoIrr = int(MenssagemRecebida)

    elif msg.topic == TopicoIntervalo:
        tempoInt = int(MenssagemRecebida)

    elif msg.topic == TopicoHora:
        horaspassados+=1
        if isAuto and not isSensor:
            if bomba:
                if horaspassados >= tempoIrr:
                    bomba = False
                    EstadoBomba(bomba)
                    horaspassados = 0
                else:
                    bomba = True
                    EstadoBomba(bomba)
            else:
                if horaspassados >= tempoInt:
                    bomba = True
                    EstadoBomba(bomba)
                    horaspassados = 0
                else:
                    bomba = False
                    EstadoBomba(bomba)

#metodo para saber se passa para proxima temoprada
def NovaTemporada():
    global temporada
    global diaspassados
    #print("nova temporada")
    
    if temporada == 1 and diaspassados > 20:
        temporada = 2
        client.publish("Temporada", str(temporada), qos=0, retain=True)
    elif temporada > 1 and diaspassados > 15:
        temporada+=1
        if temporada > 3:
            temporada = 1
        client.publish("Temporada", str(temporada), qos=0, retain=True)

def Tolerancia(t, v):
    if t == 1:
        return ((-0.1458*v)+149.1534)
    elif t == 2:
        return ((-0.1773*v)+181.3779)
    elif t == 3:
        return ((-0.2188*v)+223.8324)
    
def EstadoBomba(b):
    global temporada
    if b:
        GPIO.output(rele,GPIO.HIGH)
        client.publish("Bomba", "1", qos=0, retain=True)
    else:
        GPIO.output(rele,GPIO.LOW)
        client.publish("Bomba", "0", qos=0, retain=True)

#metodo para salvar no banco
def save_database(temp):
    db = MySQLdb.connect(host="localhost",
                         user="root",
                         passwd="root",
                         db="estufa")
    cur = db.cursor()
    try:
        cur.execute("""INSERT INTO umidade (valor, hora_registro, data_registro) values(%s, CURRENT_TIME, CURRENT_DATE)""",(temp))
        db.commit()
    except:
        db.rollback()

    db.close()

#Programa principal
try:
    print("[STATUS] Inicializando MQTT...")
    #Inicializa MQTT:
    client = mqtt.Client()
    client.username_pw_set("JoffrMQTT", "mosquito")
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(Broker, PortaBroker, KeepAliveBroker)
    #O sistema inicializa no dia 1 temporada 1
    client.publish(TopicoTemporada, str(temporada), qos=0, retain=True)
    client.publish(TopicoDia, "1", qos=0, retain=True)
    client.publish("Regra", str(temporada)+"0", qos=0, retain=True)
    #O sistema inicializa no automatico
    client.publish(TopicoTemporada, "0", qos=0, retain=True)
    #O sistema inicializa com a bomba desligada
    client.publish(TopicoBomba, "0", qos=0, retain=True)
    client.publish(TopicoVerificaDia, str(diaspassados), qos=0, retain=True)
    client.publish(TopicoSensor, "0", qos=0, retain=True)
    client.loop_forever()
except KeyboardInterrupt:
    print("\nCtrl+C pressionado, encerrado aplicacao")
    GPIO.cleanup()
    sys.exit(0)
