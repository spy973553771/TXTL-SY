/***************************Infrared.c 文件程序源代码*****************************/
#include <reg52.h>
#include <intrins.h>
sbit IR_IN =P3^2;    //红外接收引脚
sbit SB_OUT =P1^1;	 //失败信号输出
sbit FH_IN  =P1^2;	 //复活允许信号输入

unsigned char ircode[4]; //红外代码接收缓冲区
bit flag_Ir=0; //红外接收标志，收到一帧正确数据后置 1

unsigned int err;     //这4个变量由ReadIr()函数调用
unsigned int tt;      //
unsigned char Time;   //
unsigned char j, k;   //
unsigned int q;		  //循环控制变量	

unsigned int sm;	  //生命计数器
unsigned int sb;	  //失败指示位
 
void Ir_Init();       //红外初始化（外部中断0和发送接收端口初始化）
//void FH_INT();        //复活专用函数
void Delay7us();        //7us延时 @24.000MHz
void Delay19us();      //19us延时@24.000MHz
void Delay100us();    //100us延时@24.000MHz
void fengming();//蜂鸣器驱动函数

/*主函数*/
void main()
{
	sm=0;
	SB_OUT=1;
	Ir_Init();                   //红外初始化
    while (1)
    {   
		if(flag_Ir)                //如果接收到完整数据，则执行判定程序。
        {
			flag_Ir=0;//必须清空接收完成标志
			fengming();	
        }
    }
}


/*复活专用函数*/
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

/* 红外接收初始化*/
void Ir_Init(void)
{
  IT0= 1;//下降沿触发
  EX0= 1;    //打开中断0允许
  EA= 1;    //打开总中断
  IR_IN  = 1;    //初始化端口
  flag_Ir= 0;    //初始化接收完成标志位
}

/*读取红发外值外部中断的服务程序*/
void ReadIr() interrupt 0
{
    
    EA=0;                                                            //关闭总中断
  Time=0;
  tt=80;                                                      //延时8ms去干扰，引导码为9ms的低电平和4.5ms的高电平
    do 
	{
		Delay100us(); 
	} 
	while(--tt);
		if(IR_IN==0)                                                //确认是否真的接收到正确的信号
		{
			err=500;                                                //超时判断
			while((IR_IN==0)&&(err>0))                //等待高电平，最多等待50ms
				{
					Delay100us();
					err--;
				}
			if(IR_IN==1)                                            //收到高电平
			{
				err=500;                                            //超市判断
				while((IR_IN==1)&&(err>0))            //等待4.5ms的高电平过去
				{
					Delay100us();
					err--;
				}
			for(k=0; k<4; k++)                        //开始接收数据，共有4个数据
			{
				for(j=0; j<8; j++)                    //每个8位
				{
					err=60;
					while((IR_IN==0)&&(err>0))    //等待信号前面的560us低电平过去
					{
						Delay100us();
						err--;
					}
						err=500;
					while((IR_IN==1)&&(err>0))    //计算高电平的时间长度。
					{
						Delay100us();
						Time++;
						err--;
					if(Time>50)                            //超过5ms说明接收到重复码。重复码：9ms高电平+2.25ms低电平+560us高电平
                        {
                            EA=1;                                    //打开总中断
              return;                                //退出函数
                        }
          }
          ircode[k]>>=1;                         //k表示第几组数据
          if(Time>=8)                                //若高电平出现大于0.8ms，为1（1.65ms为1，0.56ms为0，这里取中间值）
            ircode[k]|=0x80;
          Time=0;                                        //用完时间要重新赋值
        }
      }
    }
    if(ircode[0]=~ircode[1])            //这里判断地址码和地址反码对否相同。
    {
    	if(ircode[2]=~ircode[3])	    //这里判断子弹码是否合法。
		{
			if(ircode[0]==0x0A)				//若相同则判断地址码是否正确。
			{
				flag_Ir = 1;				//若地址码正确，则接收完成标志置位。	
			}
			else
			{
			flag_Ir = 0;				//若地址码错误，则不置位完成标志位，以阻止错误访问。	
			} 
		}
		else
		{
			flag_Ir = 0;				//若子弹码不合法，则同样不置位完成标志位，以阻止“走火”。	
		}
		                               
    }
  }
    EA=1;                                                            //打开总中断
}

/* 延时7us，生成38k波使用*/
void Delay7us()        //@24.000MHz
{
	unsigned char a,b,c,d;
	for(d=1;d>0;d--)
		for(c=1;c>0;c--)
            for(b=1;b>0;b--)
                for(a=1;a>0;a--);
}
/**************************************************************************************************
* 延时19us @ 24MHz STC-Y5
* 生成38k波使用，占空比在1/3时效果最好。i的取值供参考，使用时视波形表现调整。
*/
void Delay19us()        //@24.000MHz
{
	unsigned char a,b;
		for(b=7;b>0;b--)
			for(a=1;a>0;a--);
}
/**************************************************************************************************
* 延时100us @ 24MHz STC-Y5
*/
void Delay100us()        //@24.000MHz
{
	unsigned char a,b;
    	for(b=1;b>0;b--)
        	for(a=97;a>0;a--);
}

/*蜂鸣器驱动程序*/
void fengming()    //蜂鸣函数，脉宽t = 1ms 周期T = 2ms 频率f = 0.5khz 实际发现延时1ms的时候效果最好
{
        while(1)
		{
			SB_OUT=0;   //给P1.5口送低电平
		}
		
}