from machine import Pin
from machine import UART
import utime



com1 = UART(2, 9600, tx=17, rx=16)
com1.init(9600)
com2 = UART(1, 9600, tx=26, rx=27)
com2.init(9600)

led = Pin(2, Pin.OUT, value=1)  # 設定LED接腳
#esp32 = Pin(4, Pin.OUT, value=1)  # 設定LED接腳
print('MicroPython Ready...')  # 輸出訊息到終端機

while True:
    com1.write('ON')
    utime.sleep(1)
    led.value(1)
#    esp32.value(1);
    b = com2.any()
    print(b)
    if b > 0:
       a = com2.readline()
       print(a)
       
    com1.write('OFF')
    utime.sleep(1)
    led.value(0)
#    esp32.value(0);
    b = com2.any()
    print(b)
    if b > 0:
       a = com2.readline()
       print(a)    