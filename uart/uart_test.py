from machine import Pin
from machine import UART
import utime

com = UART(2, 9600, tx=17, rx=16)
com.init(9600)

led = Pin(2, Pin.OUT, value=1)  # 設定LED接腳
esp32 = Pin(4, Pin.OUT, value=1)  # 設定LED接腳
print('MicroPython Ready...')  # 輸出訊息到終端機

while True:
    com.write('0')
    utime.sleep(0.1)
    choice = com.readline()

    if choice == b'LED_ON':
        led.value(1)
        print('LED_ON')
        esp32.value(1);
      #  com.write(b'LED is ON!')  # 回應訊息給電腦端的Python
    elif choice == b'LED_OFF':
        led.value(0)
        print('LED_OFF')
        esp32.value(0);
      #  com.write(b'LED is OFF!')
    utime.sleep(2)
    
    com.write('LED_OFF')
    utime.sleep(0.1)
    choice = com.readline()

    if choice == b'LED_ON':
        led.value(1)
        print('LED_ON')
        esp32.value(1);
      #  com.write(b'LED is ON!')  # 回應訊息給電腦端的Python
    elif choice == b'LED_OFF':
        led.value(0)
        print('LED_OFF')
        esp32.value(0);
      #  com.write(b'LED is OFF!')