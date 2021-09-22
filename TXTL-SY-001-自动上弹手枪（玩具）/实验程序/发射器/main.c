/*
===================

===================
*/
#include 	"intrins.h"
#include    "reg52.h"

sbit IROUT=P1^0;                                    //红外发射端口
sbit k1=P2^0;		                                //1键
sbit k2=P2^1;		                                //2键
sbit wd=P1^1;										//无弹药信号
sbit kq=P1^2;                                       //开枪音效触发
sbit st=P1^3;          								//上膛音效触发

bit wds; 											//无弹药标志位
bit irflag,keyflag;									//红外发送标志位,按键标志位                                  			
unsigned char irkey,ircode;                         //发送的红外数据
unsigned short int zd;								//弹夹（10发） 

void Ir_Init(void);                                 //红外初始化（外部中断0和发送接收端口初始化）
void Ir_Out(unsigned char u, unsigned char x);      //发送数据，两个参数分别为用户码和键值
void Ir_Out_Frame(unsigned char y);                 //发送一帧数据，由 Ir_Out() 函数调用
void Delay7us();                                    //7us延时 @24.000MHz_STC-Y5
void Delay19us();                                   //19us延时@24.000MHz_STC-Y5
void Delay100us();                                  //100us延时@24.000MHz_STC-Y5
void keyscan();
void delay(unsigned int a);
void QBL_OUT();										//镭射触发
void QBL_ST();										//上膛触发

/*主函数*/

void main() 
	{ 
		wds=1;
		wd=1;
		zd=0;										   //初始化弹夹
		Ir_Init();                                     //红外初始化
		irflag=0;                                      //红外发射标志先置0
		while(1) 
			{ 
				keyscan();                             //键值扫描 
				if(keyflag)                            //若有按键按下
					{ 
						if(irkey==0x01)
							{
								delay(10); 
								QBL_OUT(); 
								delay(500); 
								keyflag=0;            //按键标志清零
								kq=1; 
								delay(100);
							}
						if(irkey==0x02)
							{
								delay(10);
								QBL_ST();
								delay(500); 
								keyflag=0;           //按键标志清零 
								delay(100);
							}
						
						
					} 
			} 
	}
/*发射程序*/
void QBL_OUT()
	{
		if(wds==0)
		{
			if(zd>0)
			{		
				Ir_Out(0x0A,0x0A);    //红外发送,参数为用户码、键值
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
	 
/*上膛程序*/
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

/*按键去抖延时*/
void delay(unsigned int a)				//延时约1ms
	{ 
		unsigned char i;
		while(--a!=0)
		for(i=300;i>0;i--);             //如果是用12M晶振i=125（这个是估计值，不建议用12M晶振，信号调制不稳定）
	} 
/*按键扫描*/
void keyscan()                           
	{ 
		if(k1==0)				  
			{					  
				keyflag=1;		        //1键按下，启动功能“发射”
				irkey=0x01;	  
			}                     

		if(k2==0)				  
			{					  
				keyflag=1;		        // 2键按下，启动功能“上膛”
				irkey=0x02;	  
			}					  
	} 
/*红外发射初始化*/
void Ir_Init(void)
{
	IT0=1;                              //下降沿触发
	EX0=1;                              //打开中断0允许
	EA= 1;                              //打开总中断
	IROUT=0;	                        //初始化发射管
}

/*延时7us @ 24MHz，用来生成38k近红外调制波。*/
void Delay7us()                         //@24.000MHz
{
	unsigned char a,b,c,d;
	for(d=1;d>0;d--)
		for(c=1;c>0;c--)
            for(b=1;b>0;b--)
                for(a=1;a>0;a--);
}
/*延时19us @ 24MHz，用来生成38k近红外调制波*/
void Delay19us()                       //@24.000MHz
{
	unsigned char a,b;
		for(b=7;b>0;b--)
			for(a=1;a>0;a--);
}
/*延时100us @ 24MHz STC-Y5*/
void Delay100us()                     //@24.000MHz
{
	unsigned char a,b;
    	for(b=1;b>0;b--)
        	for(a=97;a>0;a--);
}

/*红外发射程序*/
void Ir_Out(unsigned char u, unsigned char x)
{
	int tt;
	EA=0;                            //关闭所有中断
    
/*发送引导码*/
	tt=275;                          //输出9ms 1（有信号输出表示 1）
	do 
  		{
			IROUT=1;
			Delay19us();
			IROUT=0;
			Delay7us();
		} 
	while(--tt);
	tt=43;                           //输出4.5ms 0(无信号输出表示0)
    do
	{
		IROUT=0;
		Delay100us();
	}
	while(--tt);
	/*发送用户码和用户补码*/
	Ir_Out_Frame(u);
	Ir_Out_Frame(~u);
	/*发送键值和键值补码*/
	Ir_Out_Frame(x);
	Ir_Out_Frame(~x);
	/*发送结束码*/
	tt=45;                          //输出0.65ms  1
  	do
	{
		IROUT=1;
		Delay19us();
		IROUT=0;
		Delay7us();
	} 
	while(--tt);
	tt=45;                           //输出4.5ms 0(无信号输出表示0)
    do
	{
		IROUT=0;
		Delay100us();
	}
	while(--tt);
  	EA=1;                          //打开中断

}
/*红外单帧发射程序*/
void Ir_Out_Frame(unsigned char y)
{
	char num;
	int    tt;
	for(num=0; num<8; num++)      //循环8次移位
		{
    		tt=18;                //输出0.56ms  1                                    
    		do
			{
				IROUT=1;
				Delay19us();
				IROUT=0;
				Delay7us();
			}
			while(--tt);
    		if(y&0x01)            //if为1
   		 	{
      			tt=16;            //输出1.65ms 1
      			do
				{
					IROUT=0;
					Delay100us();
				}
				while(--tt);
    		}
    		else                 //否则
    		{
      			tt=5;            //输出0.56ms 0
      			do 
				{
					IROUT=0;
					Delay100us();
				}
				while(--tt);
    		}
    		y >>= 1;             //右移一位
  		}
}