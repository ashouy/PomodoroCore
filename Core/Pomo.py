'''---------------------------------------------------
                    POMODORO CORE
                AUTHOR: JOFFREY PEYRAC
                        v. 1.0
        Ultima revisão em: 16/06/2019 (dd/mm/YYYY)
  ----------------------------------------------------'''
import paho.mqtt.client as mqtt
import sys
import RPi.GPIO as GPIO
#!/usr/bin/python
import MySQLdb
import time as t

rele = 40
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)
GPIO.cleanup()
t.sleep(1)
GPIO.setup(rele, GPIO.OUT)
GPIO.output(rele, GPIO.LOW)

#variaveis globais
umidadeMinima = [600, 650, 700]
umidadeMaxima = [337, 459, 566]
bombaligada = 0
mudouTemporada = 1
temporada = 1
diaspassados = 0

#definicoes Broker
Broker = "192.168.50.1"
PortaBroker = 1883
KeepAliveBroker = 60

#Topicos queo sistema assinara
TopicoSubscriber = "UmidadeSolo"
TopicoTemporada = "Temporada"
TopicoDia = "Dia"

#Callback conexao ao broker
def on_connect(client, userdata, flags, rc):
    client.subscribe(TopicoSubscriber)
    client.subscribe(TopicoTemporada)
    client.subscribe(TopicoDia)

#Callback menssagem recebida
def on_message(client, userdata, msg):
    
    global mudouTemporada
    global bombaligada
    global temporada
    global diaspassados
    
    MenssagemRecebida = str(msg.payload)
    #print("[MSG RECEBIDA] Topico: "+msg.topic+" / Menssagem: "+MenssagemRecebida)
    #se a menssagem que chegou for do topico UmidadeSolo
    if msg.topic == TopicoSubscriber:
        save_database(MenssagemRecebida) #salva no banco
        numflut = float(MenssagemRecebida) #pega a mensagem e converte para float
        #se a umidade do solo for maior que o valor minimo a bomba deve ligar ex. 700 >= 600, liga a bomba
        if(numflut >= umidadeMinima[temporada-1]):
            GPIO.output(rele,GPIO.HIGH)
            #se a bomba estava desligada ou foi a primeiramenssagem do algoritmo
            if bombaligada == 0 or mudouTemporada == 1:
                #envia para o topico procedimento uma chave para o microcontrolador saber periodo que tem que fazer as leituras
                client.publish("Regra", str(temporada)+"1", qos=0, retain=True)
                mudouTemporada = 0
                bombaligada = 1
        #se a umidade for menor que o valor maximo a bomba deve desligar ex. 299 <= 337, desliga a bomba        
        elif (numflut <= umidadeMaxima[temporada-1]):
            GPIO.output(rele,GPIO.LOW)
            #se a bomba estava ligada ou foi a primeira menssagem do algoritmo
            if bombaligada == 1 or mudouTemporada == 1:
                #envia para o topico procedimento uma chave para o microcontrolado saber o periodo de leitura
                client.publish("Regra", str(temporada)+"0", qos=0, retain=True)
                mudouTemporada = 0
                bombaligada = 0
    #se a menssagem que chegou for do topico Temporada            
    elif msg.topic == TopicoTemporada:
        temporada = int(MenssagemRecebida)
        diaspassados = 0
        mudouTemporada = 1
    #se a menssagem que chegou for do topico dia
    elif msg.topic == TopicoDia:
        #verifica se a mensagem diz novo e incrmenta a contagem
        if MenssagemRecebida == "NOVO":
            diaspassados+=1
        #se for outra mensagem sera um numero para alterar o valor do incremento
        else:
            diaspassados = int(MenssagemRecebida)
        NovaTemporada()
        
#metodo para saber se passa para proxima temoprada
def NovaTemporada():
    global temporada
    global diaspassados
    global mudouTemporada
    
    mudouTemporada = 1
    
    if temporada == 1 and diaspassados >= 20:
        temporada = 2
        diaspassados = 0
    elif temporada > 1 and diaspassados > 15:
        temporada+=1
        diaspassados = 0
        if temporada > 3:
            temporada = 1
    else:
        mudouTemporada = 0
        
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
    #print("[STATUS] Inicializando MQTT...")
    #Inicializa MQTT:
    client = mqtt.Client()
    client.username_pw_set("JoffrMQTT", "mosquito")
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(Broker, PortaBroker, KeepAliveBroker)
    client.loop_forever()
except KeyboardInterrupt:
    print("\nCtrl+C pressionado, encerrado aplicacao")
    GPIO.cleanup()
    sys.exit(0)
