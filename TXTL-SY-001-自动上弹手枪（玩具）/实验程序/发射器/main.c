/*
===================

===================
*/
#include 	"intrins.h"
#include    "reg52.h"

sbit IROUT=P1^0;                                    //���ⷢ��˿�
sbit k1=P2^0;		                                //1��
sbit k2=P2^1;		                                //2��
sbit wd=P1^1;										//�޵�ҩ�ź�
sbit kq=P1^2;                                       //��ǹ��Ч����
sbit st=P1^3;          								//������Ч����

bit wds; 											//�޵�ҩ��־λ
bit irflag,keyflag;									//���ⷢ�ͱ�־λ,������־λ                                  			
unsigned char irkey,ircode;                         //���͵ĺ�������
unsigned short int zd;								//���У�10���� 

void Ir_Init(void);                                 //�����ʼ�����ⲿ�ж�0�ͷ��ͽ��ն˿ڳ�ʼ����
void Ir_Out(unsigned char u, unsigned char x);      //�������ݣ����������ֱ�Ϊ�û���ͼ�ֵ
void Ir_Out_Frame(unsigned char y);                 //����һ֡���ݣ��� Ir_Out() ��������
void Delay7us();                                    //7us��ʱ @24.000MHz_STC-Y5
void Delay19us();                                   //19us��ʱ@24.000MHz_STC-Y5
void Delay100us();                                  //100us��ʱ@24.000MHz_STC-Y5
void keyscan();
void delay(unsigned int a);
void QBL_OUT();										//���䴥��
void QBL_ST();										//���Ŵ���

/*������*/

void main() 
	{ 
		wds=1;
		wd=1;
		zd=0;										   //��ʼ������
		Ir_Init();                                     //�����ʼ��
		irflag=0;                                      //���ⷢ���־����0
		while(1) 
			{ 
				keyscan();                             //��ֵɨ�� 
				if(keyflag)                            //���а�������
					{ 
						if(irkey==0x01)
							{
								delay(10); 
								QBL_OUT(); 
								delay(500); 
								keyflag=0;            //������־����
								kq=1; 
								delay(100);
							}
						if(irkey==0x02)
							{
								delay(10);
								QBL_ST();
								delay(500); 
								keyflag=0;           //������־���� 
								delay(100);
							}
						
						
					} 
			} 
	}
/*�������*/
void QBL_OUT()
	{
		if(wds==0)
		{
			if(zd>0)
			{		
				Ir_Out(0x0A,0x0A);    //���ⷢ��,����Ϊ�û��롢��ֵ
				wd=0;
			    kq=0;
				zd--;
			}
			else 
			{
				wds=1;
				zd=0;
			}
		}
		else 
			{
				zd=0;
				wd=1;
			}	
	}	
	 
/*���ų���*/
void QBL_ST()
	{
		if(0<=zd<10)
			{
				st=0;
				delay(20000);
				zd=10;
				wd=0;
				wds=0;
				st=1;
			}
		else
		{
			wd=0;
		}
	}

/*����ȥ����ʱ*/
void delay(unsigned int a)				//��ʱԼ1ms
	{ 
		unsigned char i;
		while(--a!=0)
		for(i=300;i>0;i--);             //�������12M����i=125������ǹ���ֵ����������12M�����źŵ��Ʋ��ȶ���
	} 
/*����ɨ��*/
void keyscan()                           
	{ 
		if(k1==0)				  
			{					  
				keyflag=1;		        //1�����£��������ܡ����䡱
				irkey=0x01;	  
			}                     

		if(k2==0)				  
			{					  
				keyflag=1;		        // 2�����£��������ܡ����š�
				irkey=0x02;	  
			}					  
	} 
/*���ⷢ���ʼ��*/
void Ir_Init(void)
{
	IT0=1;                              //�½��ش���
	EX0=1;                              //���ж�0����
	EA= 1;                              //�����ж�
	IROUT=0;	                        //��ʼ�������
}

/*��ʱ7us @ 24MHz����������38k��������Ʋ���*/
void Delay7us()                         //@24.000MHz
{
	unsigned char a,b,c,d;
	for(d=1;d>0;d--)
		for(c=1;c>0;c--)
            for(b=1;b>0;b--)
                for(a=1;a>0;a--);
}
/*��ʱ19us @ 24MHz����������38k��������Ʋ�*/
void Delay19us()                       //@24.000MHz
{
	unsigned char a,b;
		for(b=7;b>0;b--)
			for(a=1;a>0;a--);
}
/*��ʱ100us @ 24MHz STC-Y5*/
void Delay100us()                     //@24.000MHz
{
	unsigned char a,b;
    	for(b=1;b>0;b--)
        	for(a=97;a>0;a--);
}

/*���ⷢ�����*/
void Ir_Out(unsigned char u, unsigned char x)
{
	int tt;
	EA=0;                            //�ر������ж�
    
/*����������*/
	tt=275;                          //���9ms 1�����ź������ʾ 1��
	do 
  		{
			IROUT=1;
			Delay19us();
			IROUT=0;
			Delay7us();
		} 
	while(--tt);
	tt=43;                           //���4.5ms 0(���ź������ʾ0)
    do
	{
		IROUT=0;
		Delay100us();
	}
	while(--tt);
	/*�����û�����û�����*/
	Ir_Out_Frame(u);
	Ir_Out_Frame(~u);
	/*���ͼ�ֵ�ͼ�ֵ����*/
	Ir_Out_Frame(x);
	Ir_Out_Frame(~x);
	/*���ͽ�����*/
	tt=45;                          //���0.65ms  1
  	do
	{
		IROUT=1;
		Delay19us();
		IROUT=0;
		Delay7us();
	} 
	while(--tt);
	tt=45;                           //���4.5ms 0(���ź������ʾ0)
    do
	{
		IROUT=0;
		Delay100us();
	}
	while(--tt);
  	EA=1;                          //���ж�

}
/*���ⵥ֡�������*/
void Ir_Out_Frame(unsigned char y)
{
	char num;
	int    tt;
	for(num=0; num<8; num++)      //ѭ��8����λ
		{
    		tt=18;                //���0.56ms  1                                    
    		do
			{
				IROUT=1;
				Delay19us();
				IROUT=0;
				Delay7us();
			}
			while(--tt);
    		if(y&0x01)            //ifΪ1
   		 	{
      			tt=16;            //���1.65ms 1
      			do
				{
					IROUT=0;
					Delay100us();
				}
				while(--tt);
    		}
    		else                 //����
    		{
      			tt=5;            //���0.56ms 0
      			do 
				{
					IROUT=0;
					Delay100us();
				}
				while(--tt);
    		}
    		y >>= 1;             //����һλ
  		}
}