#include "qy_printf.h"
#include "usart.h"
#include <stdint.h>

void U_Putchar(uint8_t _char)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)&_char,sizeof(char),1000);
}

/*  下方是U_Printf实现部分，耦合性很低（没有），可移植  */

/**@brief  发送一段文本
  *@param  words 要发送的文本
  *@retval void
  */
void U_SendWords(const char* words)
{
	for(uint16_t i=0;words[i]!='\0';i++)
	{
		U_Putchar(words[i]);
	}
}
/**@brief  发送一个整数
  *@param  num 要发送的数字
  *@retval void
  */
void U_SendNum(uint32_t num)
{
	if(num==0)
	{
		U_Putchar('0');
		return;
	}
	uint32_t num_length = 1;
	//测量数字位数
	for(;num_length<=num;num_length*=10);
	//从高位开始发送
	for(num_length/=10;num_length>=1;num_length/=10)
	{
		U_Putchar( num/num_length +'0');
		//减去最高位
		num -= (num - (num%num_length));
	}
}
/**@brief  把hex转成字符
  *@param  hex 要转的十六进制
  *@retval 字符
  *@add    内容太敷衍了 但是这个函数又不能没有...
  */
char HexToChar(uint8_t hex)
{
	if(hex<10)
	{
		return hex+'0';
	}
	else 
	{
		return hex+'A'-10;
	}
}
/**@brief  发送一个十六进制
  *@param  hex 要发送的十六进制
  *@retval void
  */
void U_SendHex(uint32_t hex)
{
	U_SendWords("0x");
	if(hex==0)
	{
		U_Putchar('0');
		return;
	}
	//测量位数
	uint32_t hex_length = 0x01;
	int length_num = 0;
	for(;hex_length<=hex;hex_length = hex_length<<1)
	{
		length_num++;
	}
	while(length_num%4!=0)
	{
		length_num++;
		hex_length = hex_length<<1;
	}
	length_num /= 4;
	length_num--;
	//从高位发送数字
	for(;length_num>=0;length_num--)
	{
		U_Putchar(HexToChar( (( hex>>(length_num*4) )&0xF) ));
	}
}
/**@brief  发送二进制
  *@param  binary 要发送的二进制
  *@retval void
  */
void U_SendBinary(uint32_t binary)
{
	U_SendWords("0b");
	if(binary==0)
	{
		U_Putchar('0');
		return;
	}
	//测量长度
	uint32_t length = 0x1;
	uint32_t length_num=0;
	for(;length<=binary;length=length<<1)
	{
		length_num++;
	}
	while(length_num%4!=0)
	{
		length_num++;
		length=length<<1;
	}
	length=length>>1;
	for(;length>=0x01;length=length>>1)
	{
		if(length_num%4==0)
		{
			U_Putchar(' ');
		}
		if( (binary&length) == 0 )
		{
			U_Putchar('0');
		}
		else 
		{
			U_Putchar('1');
		}
		length_num--;
	}
}
/**@brief  printf
  *@param  ...
  *@retval void
  */
#include "stdarg.h"
void QY_Printf(const char* words,...)
{
	va_list ap;
	va_start(ap,words);
	for(int i=0;words[i]!='\0';i++)
	{
		if(words[i]=='%')
		{
			switch(words[++i])
			{
			//整数
			case 'D':case 'd':U_SendNum(va_arg(ap,uint32_t));break;
			//字符串
			case 'S':case 's':U_SendWords(va_arg(ap,char*));break;
			//单个字符
			case 'C':case 'c':U_Putchar(va_arg(ap,int));break;
			//十六进制，建议使用标准库X
			case 'H':case 'h':case 'X':case 'x':U_SendHex(va_arg(ap,uint32_t));break;
			//二进制
			case 'B':case 'b':U_SendBinary(va_arg(ap,uint32_t));break;
			}
			continue;
		}
		U_Putchar(words[i]);
	}
	va_end(ap);
}
