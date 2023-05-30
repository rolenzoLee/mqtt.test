import xtools, os, utime, urandom, umail, sender
from umqtt.simple import MQTTClient
xtools.connect_wifi_led()
packageIsDelivered = False
packgePassword = ""
global mqClient
mqClient = MQTTClient (
        client_id = xtools.get_id(),
        server = "192.168.3.189",
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
    
def init():
    mqClient.set_callback(sub_cb)
    mqClient.connect()
    mqClient.subscribe(b'password')
    #用來判斷是否要傳送驗證碼
    mqClient.subscribe(b'sendVericyCode')
    xtools.ledR();
init()
while True:
    mqClient.check_msg()
    utime.sleep(3)