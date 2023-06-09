 
/*-----------------------------------------------
  名稱：串口通信
  內容：連接好串口或者usb轉串口至電腦，下載該程式，打開電源
        打開串口偵錯工具，將串列傳輸速率設置為9600，無同位
        晶振11.0592MHz，發送和接收使用的格式相同，如都使用
        字元型格式，按重定重啟程式，可以看到接收到 UART test，技術論壇：www.doflye.net 請在發送區輸入任意信
		然後在發送區發送任意資訊，接收區返回同樣資訊，表明串口收發無誤
------------------------------------------------*/

#include<reg52.h> //包含標頭檔，一般情況不需要改動，標頭檔包含特殊功能寄存器的定義                        
sbit esp32=P1^0;
#define MAX 20
unsigned char buf[MAX];
unsigned char head=0;
unsigned char tail=0;

/*------------------------------------------------
                   函式宣告
------------------------------------------------*/
void SendStr(unsigned char *s);

/*------------------------------------------------
                    串口初始化
------------------------------------------------*/
void InitUART  (void)
{

    SCON  = 0x50;		        // SCON: 模式 1, 8-bit UART, 使能接收  
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit 重裝
    TH1   = 0xFD;               // TH1:  重裝值 9600 串列傳輸速率 晶振 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 打開                         
    EA    = 1;                  //打開總中斷
   // ES    = 1;                  //打開串口中斷
}                            
/*------------------------------------------------
                    主函數
------------------------------------------------*/
void main (void)
{

unsigned long i;
unsigned char cmd;
InitUART();

//SendStr("UART test，技術論壇：www.doflye.net 請在發送區輸入任意資訊");

ES    = 1;                  //打開串口中斷
while (1)                       
    {

		if (head != tail)
		{
			cmd = buf[tail];
			tail++;
			if (tail == MAX)
            	tail = 0;
			
			P0 = cmd;
		}

    }
}

/*------------------------------------------------
                    發送一個位元組
------------------------------------------------*/
void SendByte(unsigned char dat)
{
 SBUF = dat;
 while(!TI);
      TI = 0;
}
/*------------------------------------------------
                    發送一個字串
------------------------------------------------*/
void SendStr(unsigned char *s)
{
 while(*s!='\0')// \0 表示字串結束標誌，通過檢測是否字串末尾
  {
  SendByte(*s);
  s++;
  }
}
/*------------------------------------------------
                     串口中斷程式
------------------------------------------------*/
void UART_SER (void) interrupt 4 //串列中斷服務程式
{
    unsigned char Temp;          //定義臨時變數 
   
   if(RI)                        //判斷是接收中斷產生
     {
	  RI=0;                      //標誌位元清零
	  Temp=SBUF;                 //讀入緩衝區的值
      SBUF=Temp;                 //把接收到的值再發回電腦端
	  buf[head] = Temp;
	  head++;
	  if (head == MAX) head = 0;			
	 }

} 

