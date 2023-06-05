import xtools, os, utime, urandom, umail, sender
import ubinascii
from umqtt.simple import MQTTClient
from machine import UART
xtools.connect_wifi_led()
packageIsDelivered = False
packgePassword = ""
MQTTServer = "192.168.0.101"
global mqClient
mqClient = MQTTClient (
        client_id = xtools.get_id(),
        server = MQTTServer, 
        keepalive = 1000, # 保持連線 加上後就成功了
        user = 'rolenzo',
        password = 'rolenzo'
    )
verifyCode = ""
def sub_cb(topic, msg):
    print("收到訊息: ", msg.decode())
    if (topic == b'sendVericyCode'):
        global verifyCode
        verifyCode = sender.sendCAPTCHA(msg.decode()) #傳送驗證碼
    if (topic == b'password'):
        packgePassword = msg.decode()
        print(packgePassword)
        if (verifyCode == packgePassword):
            print("unlock")
            mqClient.publish(b'echo', "unlock");
            xtools.ledG()
        else:
            mqClient.publish(b'echo', "incorrect");
            print("incorrect")
            xtools.ledOff()
            utime.sleep(0.3)
            xtools.ledR()
            utime.sleep(0.3)
            xtools.ledOff()
            utime.sleep(0.3)
            xtools.ledR()
    if(topic == b'control'):
        sender.sendWarning("ken")
        xtools.ledB()
        utime.sleep(1)
        xtools.ledOff()
        xtools.ledR()
        utime.sleep(1)
        xtools.ledOff()
        xtools.ledB()
        utime.sleep(1)
        xtools.ledOff()
        xtools.ledR()


def init():
    xtools.ledOff()
    #串口通訊
    global com
    com = UART(1, 9600, tx=17, rx=16)
    com.init(9600)
    #end 串口
    mqClient.set_callback(sub_cb)
    mqClient.connect()
    mqClient.subscribe(b'password')
    #用來判斷是否要傳送驗證碼
    mqClient.subscribe(b'sendVericyCode')
    mqClient.subscribe(b'control')
    xtools.ledR();
init()
def decode8051(msg) :
    print(verifyCode)
    if (bytes(verifyCode, 'utf-8') == msg):
        print("unlock")
        mqClient.publish(b'echo', "unlock");
        xtools.ledG()
    else:
        mqClient.publish(b'echo', "incorrect");
        print("incorrect")
        xtools.ledOff()
        utime.sleep(0.3)
        xtools.ledR()
        utime.sleep(0.3)
        xtools.ledOff()
        utime.sleep(0.3)
        xtools.ledR()
while True:
    mqClient.check_msg()
    msg = com.readline()
    if(msg != None):
        decode8051(msg)
    utime.sleep(3)