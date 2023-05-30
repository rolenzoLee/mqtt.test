 
/*-----------------------------------------------
  �W�١G��f�q�H
  ���e�G�s���n��f�Ϊ�usb���f�ܹq���A�U���ӵ{���A���}�q��
        ���}��f�����u��A�N��C�ǿ�t�v�]�m��9600�A�L�P��
        ����11.0592MHz�A�o�e�M�����ϥΪ��榡�ۦP�A�p���ϥ�
        �r�����榡�A�����w���ҵ{���A�i�H�ݨ챵���� UART test�A�޳N�׾¡Gwww.doflye.net �Цb�o�e�Ͽ�J���N�H
		�M��b�o�e�ϵo�e���N��T�A�����Ϫ�^�P�˸�T�A�����f���o�L�~
------------------------------------------------*/

#include<reg52.h> //�]�t���Y�ɡA�@�뱡�p���ݭn��ʡA���Y�ɥ]�t�S��\��H�s�����w�q                        
sbit esp32=P1^0;
#define MAX 20
unsigned char buf[MAX];
unsigned char head=0;
unsigned char tail=0;

/*------------------------------------------------
                   �禡�ŧi
------------------------------------------------*/
void SendStr(unsigned char *s);

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
void main (void)
{

unsigned long i;
unsigned char cmd;
InitUART();

//SendStr("UART test�A�޳N�׾¡Gwww.doflye.net �Цb�o�e�Ͽ�J���N��T");

ES    = 1;                  //���}��f���_
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

