/*-----------------------------------------------
  名稱：IIC協議 EEPROM24c02 存數讀取數據
  內容：此程式用於檢測EEPROM性能，測試方法如下：寫入24c02一個資料，然後在記憶體中改變這些資料，
        掉電後主記憶體將失去這些資訊，然後從24c02中調入這些資料。看是否與寫入的相同。這裡用8個LED演示
		函數是採用軟體延時的方法產生SCL脈衝,固對高晶振頻率要作 一定的修改....(本例是1us機器
		週期,即晶振頻率要小於12MHZ)
------------------------------------------------*/  
  
              
#include <reg52.h>          //標頭檔的包含
#include <intrins.h>

#define  _Nop()  _nop_()        //定義空指令

// 常,變數定義區
unsigned char code dofly_DuanMa[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f, // 0 = 00111111
		                  	         0x77,0x7c,0x39,0x5e,0x79,0x71, 0x5c, 0x38, 0x40};// 顯示段碼值0~F // o =01011100 // L = 00111000 // - = 01000000
unsigned char code dofly_WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//分別對應相應的數碼管點亮,即位碼
unsigned char TempData[10]; //存儲顯示值的全域變數
#define DataPort P0 //定義資料埠 程式中遇到DataPort 則用P0 替換
#define KeyPort  P1
sbit LATCH1=P2^2;//定義鎖存使能埠 段鎖存
sbit LATCH2=P2^3;//                 位鎖存                                             
sbit SDA=P2^1;            //類比I2C資料傳送位元
sbit SCL=P2^0;            //類比I2C時鐘控制位元
#define MAX 20
unsigned char buf[MAX];
unsigned char head=0;
unsigned char tail=0;
                          
bit ack;	              //應答標誌位元
   

void DelayUs2x(unsigned char t);//函式宣告 
void DelayMs(unsigned char t);
void Display(unsigned char FirstBit,unsigned char Num);//數碼管顯示函數
unsigned char KeyScan(void);//鍵盤掃描
unsigned char KeyPro(void);
void Init_Timer0(void);//計時器初始化
/*------------------------------------------------
 顯示函數，用於動態掃描數碼管
 輸入參數 FirstBit 表示需要顯示的第一位，如賦值2表示從第三個數碼管開始顯示
 如輸入0表示從第一個顯示。
 Num表示需要顯示的位元數，如需要顯示99兩位元數值則該值輸入2
------------------------------------------------*/
void Display(unsigned char FirstBit,unsigned char Num)
{
  static unsigned char i=0;
	  

  DataPort=0;   //清空資料，防止有交替重影
  LATCH1=1;     //段鎖存
  LATCH1=0;

  DataPort=dofly_WeiMa[i+FirstBit]; //取位碼 
  LATCH2=1;     //位鎖存
  LATCH2=0;

  DataPort=TempData[i]; //取顯示資料，段碼
  LATCH1=1;     //段鎖存
  LATCH1=0;
    
  i++;
  if(i==Num) i=0;
}
/*------------------------------------------------
                    計時器初始化副程式
------------------------------------------------*/
void Init_Timer0(void)
{
  TMOD |= 0x01;	  //使用模式1，16位元計時器，使用"|"符號可以在使用多個計時器時不受影響		     
  //TH0=0x00;	      //給定初值
  //TL0=0x00;
  EA=1;            //總中斷打開
  ET0=1;           //計時器中斷打開
  TR0=1;           //計時器開關打開
}
/*------------------------------------------------
                 計時器中斷副程式
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
  TH0=(65536-2000)/256;		  //重新賦值 2ms
  TL0=(65536-2000)%256;

  Display(0,8);       // 調用數碼管掃描

}

/*------------------------------------------------
        按鍵掃描函數，返回掃描鍵值
------------------------------------------------*/
unsigned char KeyScan(void)  //鍵盤掃描函數，使用行列逐級掃描法
{
  unsigned char Val;
  KeyPort=0xf0;//高四位置高，低四位拉低
  if(KeyPort!=0xf0) {//表示有按鍵按下
    DelayMs(10);  //去抖
    if(KeyPort!=0xf0) {           //表示有按鍵按下
      KeyPort=0xfe; //11111110檢測第一行
      if(KeyPort!=0xfe) {
        Val=KeyPort&0xf0;
        Val+=0x0e;
        while(KeyPort!=0xfe);
        DelayMs(10); //去抖
        while(KeyPort!=0xfe);
        return Val;
      }
      KeyPort=0xfd; //檢測第二行
      if(KeyPort!=0xfd) {
        Val=KeyPort&0xf0;
        Val+=0x0d;
        while(KeyPort!=0xfd);
        DelayMs(10); //去抖
        while(KeyPort!=0xfd);
        return Val;
      }
      KeyPort=0xfb; //檢測第三行
      if(KeyPort!=0xfb) {
        Val=KeyPort&0xf0;
        Val+=0x0b;
        while(KeyPort!=0xfb);
        DelayMs(10); //去抖
        while(KeyPort!=0xfb);
        return Val;
      }
      KeyPort=0xf7; //檢測第四行
      if(KeyPort!=0xf7){
        Val=KeyPort&0xf0;
        Val+=0x07;
        while(KeyPort!=0xf7);
        DelayMs(10); //去抖
        while(KeyPort!=0xf7);
        return Val;
      }
    }
  }
  return 0xff;
}
/*------------------------------------------------
         按鍵值處理函數，返回掃鍵值
------------------------------------------------*/
unsigned char KeyPro(void)
{
  switch(KeyScan()) {						   //                        前4bit: 第幾行被按下 0111表示第一行被按下
                                   //                        後4bit: 第幾列被按下
  case 0x7e:return 1;break;//0 按下相應的鍵顯示相對應的碼值 01111110
  case 0xbe:return 2;break;//1 10111110
  case 0xde:return 3;break;//2 11011110
  case 0xee:return 'c';break;//3
  case 0x7d:return 4;break;//4
  case 0xbd:return 5;break;//5
  case 0xdd:return 6;break;//6
  case 0xed:return 'e';break;//7
  case 0x7b:return 7;break;//8
  case 0xbb:return 8;break;//9
  case 0xdb:return 9;break;//a
  case 0xeb:return 0xff;break;//b
  case 0x77:return 0xff;break;//c
  case 0xb7:return 0;break;//d
  case 0xd7:return 0xff;break;//e
  case 0xe7:return 0xff;break;//f
  default:return 0xff;break;
  }
}
/*------------------------------------------------
 uS延時函數，含有輸入參數 unsigned char t，無返回值
 unsigned char 是定義無符號字元變數，其值的範圍是
 0~255 這裡使用晶振12M，精確延時請使用彙編,大致延時
 長度如下 T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
  while(--t);
}
/*------------------------------------------------
 mS延時函數，含有輸入參數 unsigned char t，無返回值
 unsigned char 是定義無符號字元變數，其值的範圍是
 0~255 這裡使用晶振12M，精確延時請使用彙編
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
  while(t--) {
      //大致延時1mS
    DelayUs2x(245);
    DelayUs2x(245);
  }
}

// 比對密碼
bit checkPassword(unsigned char *password, unsigned char *key){
	unsigned char i;
	for(i = 0; i < 4; ++i){
		if(password[i] != key[i]) return 0;
	}
	return 1;
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
void main() {
  unsigned char password[6];       // 定義臨時變數
  unsigned char key[6];
  unsigned char i, j, num;
  unsigned char temp[8];
  bit isLock = 1;
  Init_Timer0();
 //InitUART();
    //for(i = 0; i < 4; ++i) temp[i]=dofly_DuanMa[key[i]]; // 顯示出密碼

  for(j=0;j<6;j++) TempData[j]=temp[j]; 
  i = 0;
  while(1) {
SendStr("1");  
    num = keyPro();
    if(num!=0xff){
      if(i<6) {
          password[i] = num;
          temp[i]=dofly_DuanMa[num];
          for(j=0;j<=i;j++) TempData[7-i+j]=temp[j];
      }
      i++;
    }
    if(num == 'c'){
      i = 0;
      for(j=0;j<8;j++)//清屏
        TempData[j]=0;
    }
    if(num == 'e'){
      i = 0;
      for(j=0;j<8;j++) {//清屏
      	TempData[j]=0;
	  }
	  SendStr(password);
      if(isLock == 0) {
        
        DelayMs(20);
        
        TempData[7] = dofly_DuanMa[17]; //顯示上鎖
        isLock = 1;
      }
      else if (checkPassword(&password, &key) == 1){
        TempData[7]=dofly_DuanMa[16]; //顯示開鎖
        isLock = 0;
      }else{
        TempData[7]=dofly_DuanMa[18]; //顯示密碼錯誤
      }
    }

  }
}





