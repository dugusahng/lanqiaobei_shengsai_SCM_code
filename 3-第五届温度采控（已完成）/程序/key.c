#include "key.h"

u8 down_count;
extern u8 trg,cont,Tmin,Tmax; 	
extern bit error_flag,set_flag;
u8 key_number[8];
extern u8 smg_duan[],down_table[];

void keyscanf()			 //setkbd��getkbd�궨������� iicͷ�ļ�����	��Ҫ������ͼ�⹦��
{
	u8 readdata;
	setkbd(0xf0);		  //����λ��0�а������� ����λ�����ñ�����
	readdata = getkbd;	   //��ȡ����λ�����
	setkbd(0x0f);			 //����λ��0�а������� ����λ�����ñ�����
	readdata = (readdata | getkbd) ^ 0xff;	   //���õ���ֵ�ϵ�һ�� ���Ϊ����1 ������0

	trg = readdata & (readdata ^ cont);		    //������һֱ����һ�ε���Ϊ��Чֵ ֮��Ϊ0
	cont = readdata;  						 //������һֱ��һֱΪ��Чֵ
}

void set_key_val(u8 i)
{
	down_count++;						 
	  						//��һ��������ʾ��ʱ0xbf����ֱ�Ӵ�1��ʼ
	down_table[down_count] = smg_duan[i];
		 					//��ÿһ��ֵ��Ӧ�������BCD�븳ֵ��down_table[]��Ӧ�ĵط�
	key_number[down_count] = i;			
							//����ǽ�ÿ�����ּ������ٴΰ�������ʱ �����������Сֵ
}

void keyproc()
{
	keyscanf();
	if(down_count == 2)down_count = 5;
			//���ֵ�������������Сֵ��Ӧ�������Ҳ���ǵ�6��7λ�������������Ϊ��������ʱ���Լ�1
	if(down_count >= 8)down_count = 8;
	 		//Ϊ�������Сֵ���������down_count��������

	if(trg == 0x48)	  //s8
	{
		if(set_flag == 0)set_flag = ~set_flag;
		if(down_count >= 7)		 //�����Сֵ�������
		{
			down_count = 0;			//�����´����ÿ��Խ�����������
			set_flag = ~set_flag;		   //��Ҫ����ת���������� 
			down_table[1] = 0xff; //��֤�޸Ĺ����ĸ������´��ٰ������ü������ȫ��	
			down_table[2] = 0xff;		
			down_table[6] = 0xff;
			down_table[7] = 0xff;
			Tmin = key_number[6]*10 + key_number[7];   //�������ֵ��Сֵ
			Tmax = key_number[1]*10 + key_number[2];
			if(Tmax < Tmin)					   //�ж�ֵ�Ƿ���Ч�����ֵ�Ƿ������Сֵ��
			{
				P2 = ((P2&0x1f)|0x80);P0 = 0xfd;P2 &= 0x1f;		  //L2��
				error_flag = 1;		//�̵�����־λ��Ϊ0ʱ�������̵���
			}
			else
			{
				P2 = ((P2&0x1f)|0x80);P0 = 0xff;P2 &= 0x1f;		 //Led��
				error_flag = 0;
			}
		}					
	}

	if(trg == 0x28)	  //s12			  ����� ǣ������ֵ��Ҫ��ʼ��
	{
		down_count = 0;			   //��������
		down_table[1] = 0xff;
		down_table[2] = 0xff;
		down_table[6] = 0xff;
		down_table[7] = 0xff; 	
	}
	if(set_flag)
	{					//0-9�İ���
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
