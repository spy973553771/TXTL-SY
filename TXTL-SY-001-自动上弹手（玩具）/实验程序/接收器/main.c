/***************************Infrared.c �ļ�����Դ����*****************************/
#include <reg52.h>
#include <intrins.h>
sbit IR_IN =P3^2;    //�����������
sbit SB_OUT =P1^1;	 //ʧ���ź����
sbit FH_IN  =P1^2;	 //���������ź�����

unsigned char ircode[4]; //���������ջ�����
bit flag_Ir=0; //������ձ�־���յ�һ֡��ȷ���ݺ��� 1

unsigned int err;     //��4��������ReadIr()��������
unsigned int tt;      //
unsigned char Time;   //
unsigned char j, k;   //
unsigned int q;		  //ѭ�����Ʊ���	

unsigned int sm;	  //����������
unsigned int sb;	  //ʧ��ָʾλ
 
void Ir_Init();       //�����ʼ�����ⲿ�ж�0�ͷ��ͽ��ն˿ڳ�ʼ����
//void FH_INT();        //����ר�ú���
void Delay7us();        //7us��ʱ @24.000MHz
void Delay19us();      //19us��ʱ@24.000MHz
void Delay100us();    //100us��ʱ@24.000MHz
void fengming();//��������������

/*������*/
void main()
{
	sm=0;
	SB_OUT=1;
	Ir_Init();                   //�����ʼ��
    while (1)
    {   
		if(flag_Ir)                //������յ��������ݣ���ִ���ж�����
        {
			flag_Ir=0;//������ս�����ɱ�־
			fengming();	
        }
    }
}


/*����ר�ú���*/
/*void FH_INT()
{
	if(FH_IN==0)
	{
		sm=3;
		SB_OUT=0;
	}
	else
	{
		sm=0;
	}
	
} */

/* ������ճ�ʼ��*/
void Ir_Init(void)
{
  IT0= 1;//�½��ش���
  EX0= 1;    //���ж�0����
  EA= 1;    //�����ж�
  IR_IN  = 1;    //��ʼ���˿�
  flag_Ir= 0;    //��ʼ��������ɱ�־λ
}

/*��ȡ�췢��ֵ�ⲿ�жϵķ������*/
void ReadIr() interrupt 0
{
    
    EA=0;                                                            //�ر����ж�
  Time=0;
  tt=80;                                                      //��ʱ8msȥ���ţ�������Ϊ9ms�ĵ͵�ƽ��4.5ms�ĸߵ�ƽ
    do 
	{
		Delay100us(); 
	} 
	while(--tt);
		if(IR_IN==0)                                                //ȷ���Ƿ���Ľ��յ���ȷ���ź�
		{
			err=500;                                                //��ʱ�ж�
			while((IR_IN==0)&&(err>0))                //�ȴ��ߵ�ƽ�����ȴ�50ms
				{
					Delay100us();
					err--;
				}
			if(IR_IN==1)                                            //�յ��ߵ�ƽ
			{
				err=500;                                            //�����ж�
				while((IR_IN==1)&&(err>0))            //�ȴ�4.5ms�ĸߵ�ƽ��ȥ
				{
					Delay100us();
					err--;
				}
			for(k=0; k<4; k++)                        //��ʼ�������ݣ�����4������
			{
				for(j=0; j<8; j++)                    //ÿ��8λ
				{
					err=60;
					while((IR_IN==0)&&(err>0))    //�ȴ��ź�ǰ���560us�͵�ƽ��ȥ
					{
						Delay100us();
						err--;
					}
						err=500;
					while((IR_IN==1)&&(err>0))    //����ߵ�ƽ��ʱ�䳤�ȡ�
					{
						Delay100us();
						Time++;
						err--;
					if(Time>50)                            //����5ms˵�����յ��ظ��롣�ظ��룺9ms�ߵ�ƽ+2.25ms�͵�ƽ+560us�ߵ�ƽ
                        {
                            EA=1;                                    //�����ж�
              return;                                //�˳�����
                        }
          }
          ircode[k]>>=1;                         //k��ʾ�ڼ�������
          if(Time>=8)                                //���ߵ�ƽ���ִ���0.8ms��Ϊ1��1.65msΪ1��0.56msΪ0������ȡ�м�ֵ��
            ircode[k]|=0x80;
          Time=0;                                        //����ʱ��Ҫ���¸�ֵ
        }
      }
    }
    if(ircode[0]=~ircode[1])            //�����жϵ�ַ��͵�ַ����Է���ͬ��
    {
    	if(ircode[2]=~ircode[3])	    //�����ж��ӵ����Ƿ�Ϸ���
		{
			if(ircode[0]==0x0A)				//����ͬ���жϵ�ַ���Ƿ���ȷ��
			{
				flag_Ir = 1;				//����ַ����ȷ���������ɱ�־��λ��	
			}
			else
			{
			flag_Ir = 0;				//����ַ���������λ��ɱ�־λ������ֹ������ʡ�	
			} 
		}
		else
		{
			flag_Ir = 0;				//���ӵ��벻�Ϸ�����ͬ������λ��ɱ�־λ������ֹ���߻𡱡�	
		}
		                               
    }
  }
    EA=1;                                                            //�����ж�
}

/* ��ʱ7us������38k��ʹ��*/
void Delay7us()        //@24.000MHz
{
	unsigned char a,b,c,d;
	for(d=1;d>0;d--)
		for(c=1;c>0;c--)
            for(b=1;b>0;b--)
                for(a=1;a>0;a--);
}
/**************************************************************************************************
* ��ʱ19us @ 24MHz STC-Y5
* ����38k��ʹ�ã�ռ�ձ���1/3ʱЧ����á�i��ȡֵ���ο���ʹ��ʱ�Ӳ��α��ֵ�����
*/
void Delay19us()        //@24.000MHz
{
	unsigned char a,b;
		for(b=7;b>0;b--)
			for(a=1;a>0;a--);
}
/**************************************************************************************************
* ��ʱ100us @ 24MHz STC-Y5
*/
void Delay100us()        //@24.000MHz
{
	unsigned char a,b;
    	for(b=1;b>0;b--)
        	for(a=97;a>0;a--);
}

/*��������������*/
void fengming()    //��������������t = 1ms ����T = 2ms Ƶ��f = 0.5khz ʵ�ʷ�����ʱ1ms��ʱ��Ч�����
{
        while(1)
		{
			SB_OUT=0;   //��P1.5���͵͵�ƽ
		}
		
}