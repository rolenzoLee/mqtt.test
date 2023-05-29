import xtools, os, utime, urandom, umail, sender
from umqtt.simple import MQTTClient
xtools.connect_wifi_led()
packageIsDelivered = False
verifyCode = ""
packgePassword = ""
mqClient = MQTTClient (
        client_id = xtools.get_id(),
        server = "192.168.0.101",
        keepalive = 1000, # 保持連線 加上後就成功了
        user = 'rolenzo',
        password = 'rolenzo'
    )
def sub_cb(topic, msg):
    print("收到訊息: ", msg.decode())
    packgePassword = msg.decode()
    print(packgePassword)
    if (verifyCode == packgePassword):
        print("unlock")
        xtools.ledG()
    else:
        print("lock")
        xtools.ledR()
    
def init():
    mqClient.set_callback(sub_cb)
    mqClient.connect()
    mqClient.subscribe(b'password')
    xtools.ledOff()
init()
while True:
    
    if (xtools.buttonClicked()):
        verifyCode = sender.sendCAPTCHA() #傳送驗證碼
    mqClient.check_msg()
    utime.sleep(3)