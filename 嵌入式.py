from machine import Pin
from machine import UART
import utime



com = UART(1, 9600, tx=17, rx=16)
com.init(9600)


led = Pin(2, Pin.OUT, value=1)  # 設定LED接腳
print('MicroPython Ready...')  # 輸出訊息到終端機

while True:
    msg = com.readline()
    print(msg)
    utime.sleep(2)    
