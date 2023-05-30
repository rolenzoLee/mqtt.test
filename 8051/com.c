/*-----------------------------------------------
  �W�١GIIC��ĳ EEPROM24c02 �s��Ū���ƾ�
  ���e�G���{���Ω��˴�EEPROM�ʯ�A���դ�k�p�U�G�g�J24c02�@�Ӹ�ơA�M��b�O���餤���ܳo�Ǹ�ơA
        ���q��D�O����N���h�o�Ǹ�T�A�M��q24c02���դJ�o�Ǹ�ơC�ݬO�_�P�g�J���ۦP�C�o�̥�8��LED�t��
		��ƬO�ĥγn�驵�ɪ���k����SCL�߽�,�T�ﰪ�����W�v�n�@ �@�w���ק�....(���ҬO1us����
		�g��,�Y�����W�v�n�p��12MHZ)
------------------------------------------------*/  
  
              
#include <reg52.h>          //���Y�ɪ��]�t
#include <intrins.h>

#define  _Nop()  _nop_()        //�w�q�ū��O

// �`,�ܼƩw�q��
unsigned char code dofly_DuanMa[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f, // 0 = 00111111
		                  	         0x77,0x7c,0x39,0x5e,0x79,0x71, 0x5c, 0x38, 0x40};// ��ܬq�X��0~F // o =01011100 // L = 00111000 // - = 01000000
unsigned char code dofly_WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//���O�����������ƽX���I�G,�Y��X
unsigned char TempData[10]; //�s�x��ܭȪ������ܼ�
#define DataPort P0 //�w�q��ư� �{�����J��DataPort �h��P0 ����
#define KeyPort  P1
sbit LATCH1=P2^2;//�w�q��s�ϯ�� �q��s
sbit LATCH2=P2^3;//                 ����s                                             
sbit SDA=P2^1;            //����I2C��ƶǰe�줸
sbit SCL=P2^0;            //����I2C��������줸
#define MAX 20
unsigned char buf[MAX];
unsigned char head=0;
unsigned char tail=0;
                          
bit ack;	              //�����лx�줸
   

void DelayUs2x(unsigned char t);//�禡�ŧi 
void DelayMs(unsigned char t);
void Display(unsigned char FirstBit,unsigned char Num);//�ƽX����ܨ��
unsigned char KeyScan(void);//��L���y
unsigned char KeyPro(void);
void Init_Timer0(void);//�p�ɾ���l��
/*------------------------------------------------
 ��ܨ�ơA�Ω�ʺA���y�ƽX��
 ��J�Ѽ� FirstBit ��ܻݭn��ܪ��Ĥ@��A�p���2��ܱq�ĤT�ӼƽX�޶}�l���
 �p��J0��ܱq�Ĥ@����ܡC
 Num��ܻݭn��ܪ��줸�ơA�p�ݭn���99��줸�ƭȫh�ӭȿ�J2
------------------------------------------------*/
void Display(unsigned char FirstBit,unsigned char Num)
{
  static unsigned char i=0;
	  

  DataPort=0;   //�M�Ÿ�ơA���������v
  LATCH1=1;     //�q��s
  LATCH1=0;

  DataPort=dofly_WeiMa[i+FirstBit]; //����X 
  LATCH2=1;     //����s
  LATCH2=0;

  DataPort=TempData[i]; //����ܸ�ơA�q�X
  LATCH1=1;     //�q��s
  LATCH1=0;
    
  i++;
  if(i==Num) i=0;
}
/*------------------------------------------------
                    �p�ɾ���l�ưƵ{��
------------------------------------------------*/
void Init_Timer0(void)
{
  TMOD |= 0x01;	  //�ϥμҦ�1�A16�줸�p�ɾ��A�ϥ�"|"�Ÿ��i�H�b�ϥΦh�ӭp�ɾ��ɤ����v�T		     
  //TH0=0x00;	      //���w���
  //TL0=0x00;
  EA=1;            //�`���_���}
  ET0=1;           //�p�ɾ����_���}
  TR0=1;           //�p�ɾ��}�����}
}
/*------------------------------------------------
                 �p�ɾ����_�Ƶ{��
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
  TH0=(65536-2000)/256;		  //���s��� 2ms
  TL0=(65536-2000)%256;

  Display(0,8);       // �եμƽX�ޱ��y

}

/*------------------------------------------------
        ���䱽�y��ơA��^���y���
------------------------------------------------*/
unsigned char KeyScan(void)  //��L���y��ơA�ϥΦ�C�v�ű��y�k
{
  unsigned char Val;
  KeyPort=0xf0;//���|��m���A�C�|��ԧC
  if(KeyPort!=0xf0) {//��ܦ�������U
    DelayMs(10);  //�h��
    if(KeyPort!=0xf0) {           //��ܦ�������U
      KeyPort=0xfe; //11111110�˴��Ĥ@��
      if(KeyPort!=0xfe) {
        Val=KeyPort&0xf0;
        Val+=0x0e;
        while(KeyPort!=0xfe);
        DelayMs(10); //�h��
        while(KeyPort!=0xfe);
        return Val;
      }
      KeyPort=0xfd; //�˴��ĤG��
      if(KeyPort!=0xfd) {
        Val=KeyPort&0xf0;
        Val+=0x0d;
        while(KeyPort!=0xfd);
        DelayMs(10); //�h��
        while(KeyPort!=0xfd);
        return Val;
      }
      KeyPort=0xfb; //�˴��ĤT��
      if(KeyPort!=0xfb) {
        Val=KeyPort&0xf0;
        Val+=0x0b;
        while(KeyPort!=0xfb);
        DelayMs(10); //�h��
        while(KeyPort!=0xfb);
        return Val;
      }
      KeyPort=0xf7; //�˴��ĥ|��
      if(KeyPort!=0xf7){
        Val=KeyPort&0xf0;
        Val+=0x07;
        while(KeyPort!=0xf7);
        DelayMs(10); //�h��
        while(KeyPort!=0xf7);
        return Val;
      }
    }
  }
  return 0xff;
}
/*------------------------------------------------
         ����ȳB�z��ơA��^�����
------------------------------------------------*/
unsigned char KeyPro(void)
{
  switch(KeyScan()) {						   //                        �e4bit: �ĴX��Q���U 0111��ܲĤ@��Q���U
                                   //                        ��4bit: �ĴX�C�Q���U
  case 0x7e:return 1;break;//0 ���U����������ܬ۹������X�� 01111110
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
 uS���ɨ�ơA�t����J�Ѽ� unsigned char t�A�L��^��
 unsigned char �O�w�q�L�Ÿ��r���ܼơA��Ȫ��d��O
 0~255 �o�̨ϥδ���12M�A��T���ɽШϥηJ�s,�j�P����
 ���צp�U T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
  while(--t);
}
/*------------------------------------------------
 mS���ɨ�ơA�t����J�Ѽ� unsigned char t�A�L��^��
 unsigned char �O�w�q�L�Ÿ��r���ܼơA��Ȫ��d��O
 0~255 �o�̨ϥδ���12M�A��T���ɽШϥηJ�s
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
  while(t--) {
      //�j�P����1mS
    DelayUs2x(245);
    DelayUs2x(245);
  }
}

// ���K�X
bit checkPassword(unsigned char *password, unsigned char *key){
	unsigned char i;
	for(i = 0; i < 4; ++i){
		if(password[i] != key[i]) return 0;
	}
	return 1;
}
/*------------------------------------------------
                    �o�e�@�Ӧ줸��
------------------------------------------------*/
void SendByte(unsigned char dat)
{
 SBUF = dat;
 while(!TI);
      TI = 0;
}
/*------------------------------------------------
                    �o�e�@�Ӧr��
------------------------------------------------*/
void SendStr(unsigned char *s)
{
 while(*s!='\0')// \0 ��ܦr�굲���лx�A�q�L�˴��O�_�r�꥽��
  {
  SendByte(*s);
  s++;
  }
}
/*------------------------------------------------
                     ��f���_�{��
------------------------------------------------*/
void UART_SER (void) interrupt 4 //��C���_�A�ȵ{��
{
    unsigned char Temp;          //�w�q�{���ܼ� 
   
   if(RI)                        //�P�_�O�������_����
     {
	  RI=0;                      //�лx�줸�M�s
	  Temp=SBUF;                 //Ū�J�w�İϪ���
      SBUF=Temp;                 //�Ⱶ���쪺�ȦA�o�^�q����
	  buf[head] = Temp;
	  head++;
	  if (head == MAX) head = 0;			
	 }

}

/*------------------------------------------------
                    ��f��l��
------------------------------------------------*/
void InitUART  (void)
{

    SCON  = 0x50;		        // SCON: �Ҧ� 1, 8-bit UART, �ϯ౵��  
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit ����
    TH1   = 0xFD;               // TH1:  ���˭� 9600 ��C�ǿ�t�v ���� 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 ���}                         
    EA    = 1;                  //���}�`���_
   // ES    = 1;                  //���}��f���_
}         
/*------------------------------------------------
                    �D���
------------------------------------------------*/
void main() {
  unsigned char password[6];       // �w�q�{���ܼ�
  unsigned char key[6];
  unsigned char i, j, num;
  unsigned char temp[8];
  bit isLock = 1;
  Init_Timer0();
 //InitUART();
    //for(i = 0; i < 4; ++i) temp[i]=dofly_DuanMa[key[i]]; // ��ܥX�K�X

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
      for(j=0;j<8;j++)//�M��
        TempData[j]=0;
    }
    if(num == 'e'){
      i = 0;
      for(j=0;j<8;j++) {//�M��
      	TempData[j]=0;
	  }
	  SendStr(password);
      if(isLock == 0) {
        
        DelayMs(20);
        
        TempData[7] = dofly_DuanMa[17]; //��ܤW��
        isLock = 1;
      }
      else if (checkPassword(&password, &key) == 1){
        TempData[7]=dofly_DuanMa[16]; //��ܶ}��
        isLock = 0;
      }else{
        TempData[7]=dofly_DuanMa[18]; //��ܱK�X���~
      }
    }

  }
}





