#include "key.h"

u8 down_count;
extern u8 trg,cont,Tmin,Tmax; 	
extern bit error_flag,set_flag;
u8 key_number[8];
extern u8 smg_duan[],down_table[];

void keyscanf()			 //setkbd和getkbd宏定义放在了 iic头文件里了	主要是输入和检测功能
{
	u8 readdata;
	setkbd(0xf0);		  //高四位置0有按键按下 低四位会有置被拉低
	readdata = getkbd;	   //获取低四位的情况
	setkbd(0x0f);			 //低四位置0有按键按下 高四位会有置被拉低
	readdata = (readdata | getkbd) ^ 0xff;	   //将得到的值合到一块 异或为了置1 其余置0

	trg = readdata & (readdata ^ cont);		    //若按键一直按第一次调用为有效值 之后为0
	cont = readdata;  						 //若按键一直按一直为有效值
}

void set_key_val(u8 i)
{
	down_count++;						 
	  						//第一个数组显示的时0xbf所以直接从1开始
	down_table[down_count] = smg_duan[i];
		 					//将每一个值对应的数码管BCD码赋值给down_table[]对应的地方
	key_number[down_count] = i;			
							//这个是将每个数字记起来再次按下设置时 来设置最大最小值
}

void keyproc()
{
	keyscanf();
	if(down_count == 2)down_count = 5;
			//最大值设置完后跳到最小值对应的数码管也就是第6、7位这里等于五是因为按键按下时会自加1
	if(down_count >= 8)down_count = 8;
	 		//为了最大最小值输入完后不让down_count接着增加

	if(trg == 0x48)	  //s8
	{
		if(set_flag == 0)set_flag = ~set_flag;
		if(down_count >= 7)		 //最大最小值输入完后
		{
			down_count = 0;			//归零下次设置可以接着重新设置
			set_flag = ~set_flag;		   //不要忘了转回正常界面 
			down_table[1] = 0xff; //保证修改过的四个数组下次再按下设置键数码管全灭	
			down_table[2] = 0xff;		
			down_table[6] = 0xff;
			down_table[7] = 0xff;
			Tmin = key_number[6]*10 + key_number[7];   //计算最大值最小值
			Tmax = key_number[1]*10 + key_number[2];
			if(Tmax < Tmin)					   //判断值是否有效（最大值是否大于最小值）
			{
				P2 = ((P2&0x1f)|0x80);P0 = 0xfd;P2 &= 0x1f;		  //L2亮
				error_flag = 1;		//继电器标志位，为0时，开启继电器
			}
			else
			{
				P2 = ((P2&0x1f)|0x80);P0 = 0xff;P2 &= 0x1f;		 //Led灭
				error_flag = 0;
			}
		}					
	}

	if(trg == 0x28)	  //s12			  清除键 牵扯到的值都要初始化
	{
		down_count = 0;			   //计数清零
		down_table[1] = 0xff;
		down_table[2] = 0xff;
		down_table[6] = 0xff;
		down_table[7] = 0xff; 	
	}
	if(set_flag)
	{					//0-9的按键
		if(trg == 0x81){set_key_val(0);} //s7
		if(trg == 0x41){set_key_val(1);} //s11
		if(trg == 0x21){set_key_val(2);} //s15	                      
		if(trg == 0x82){set_key_val(3);} //s6	                 
		if(trg == 0x42){set_key_val(4);} //s10	              
		if(trg == 0x22){set_key_val(5);} //s14	                
		if(trg == 0x84){set_key_val(6);} //s5	                  
		if(trg == 0x44){set_key_val(7);} //s9	                      
		if(trg == 0x24){set_key_val(8);} //s13	                     
		if(trg == 0x88){set_key_val(9);} //s4 
	}                            
}
