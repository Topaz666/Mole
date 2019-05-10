#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include "globals.h"



#include "stdio.h"
#include "string.h"
#include "pic.h"

volatile int tick;
int PS2_data,RVALID;
void get_ps2(void);
/* function prototypes */
void VGA_text (int, int, char *);//显示字符串
void VGA_box (int, int, int, int, short);//显示方框
void VGA_pic(int,int,int,int,int * pic_dat);//显示图片
void HEX_PS2(char, char, char);//PS2按键输入显示
void check_KEYs( int *, int *, int * );//板载按键输入
int game(int);//打地鼠游戏主体	传入参数l 难度级别0-2
void delay_ms(int);
int _getkey(void);
void _LedShow(void );
void UPDATE_HEX_DISPLAY(void);
/*******************************************************************************
 *本案例主要是做VGA显示做打地鼠游戏
 ******************************************************************************/
void _start1(void);
int cnt=0;
extern int _CNT;
int main(void)
{

//	{
//asm (".set		noat");					/* Instruct the assembler NOT to use reg at (r1) as 
										// * a temp register for performing optimizations */
//asm (".set		nobreak");				/* Suppresses a warning message that says that some
										// * debuggers corrupt regs bt (r25) and ba (r30) */
//asm ("movia		r2, main");				// Call the C language main program
//asm ("jmp		r2"); 
//}
	/* 声明端口映射的物理地址 */
  	volatile int * red_LED_ptr = (int *) LEDR_BASE;				// red LED address
	volatile int * PS2_ptr = (int *) PS2_BASE;						// PS/2 port address
	volatile int * KEY_ptr = (int *) KEY_BASE;						// KEY port address
	volatile int * interval_timer_ptr = (int *) TIMER_BASE;		// interal timer base address
	
	
	//_start1();
	
	/* 设置系统变量 */
	static int level=0;												//难度系数
	int KEY_value;													//板载按键值
	int PS2_data, RVALID;											/* 接收PS2按键的输入*/
	char byte1 = 0, byte2 = 0, byte3 = 0;
{//中断设置
	/* set the interval timer period for scrolling the HEX displays */
	int counter = 1000000;//5000000;				// 1/(100 MHz) x (5000000) = 50 msec
	*(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
	*(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;

	/* start interval timer, enable its interrupts */
	*(interval_timer_ptr + 1) = 0x7;	// STOP = 0, START = 1, CONT = 1, ITO = 1 
	
	/* set interrupt mask bits for levels 0 (interval timer) and level 1 (pushbuttons) */
	NIOS2_WRITE_IENABLE( 0x3 );
	NIOS2_WRITE_STATUS( 1 );			// enable Nios II interrupts
}
	/* 清除VGA显示器字符缓冲区*/
	int *p;
	for (p = (int*)FPGA_CHAR_BASE; p < (int*)FPGA_CHAR_END; ++p)
		*p = 0;
	/* VGA显示第一屏幕设置难度*/
	VGA_box (0, 0, 320, 240, 0);							// clear the screen
	VGA_pic(0,0,319,239,picdat);							//显示第一屏幕背景图片	
	VGA_box(47*4, 28*4, 51*4 - 1, 32*4 - 1,0xD3E5);			//显示选择方框

	/*PS/2键盘初始化*/
	*(PS2_ptr) = 0xFF;	
	cnt	=_CNT;
	while(1)
	{		
		{//板上按键处理			
			if(_getkey())//if(*KEY_ptr)
			{
				delay_ms(2);				
				//KEY_value=*KEY_ptr;			//获取板载按键值
				KEY_value =_getkey();
				//cnt++;
				//_CNT++;
				//_LedShow();
				//UPDATE_HEX_DISPLAY();//cnt++);
				switch(KEY_value)			//判断哪个按键KEY0、KEY1、KEY2、KEY3
				{
					case 1:					//如果按键KEY0，游戏难度为 easy
						*(red_LED_ptr) = 0x1;		 //LEDR0亮
						VGA_pic(0,0,319,239,picdat2);//显示游戏页面背景
						game(level);				//开始游戏，输入难度level=0;
						break;
					case 2:					//如果按键KEY1，难度增加1
						*(red_LED_ptr) = 0x2;//LEDR1亮
						if(++level >= 3)	
							level = 0;		//当难度大于3难度从difficult回到easy
						break;
					case 4:					//如果按键KEY2，难度减少1
						if(level-- == 0)
							level = 2;		//当难度小于等于0的时候，难度从easy回到difficult			
						break;
					case 8:					//按键KEY3，不处理
						break;
					default:
						continue;
						break;
				}
				VGA_pic(0,0,319,239,picdat);   //重新刷新背景图片
				if(level == 0)
				{
					VGA_box(47*4, 28*4, 51*4 - 1, 32*4 - 1,0xD3E5); //在easy位置显示选择筐
				}
				else if(level == 1)
				{
					VGA_box(47*4, 37*4, 51*4 - 1, 41*4 - 1,0xD3E5);//在medium位置显示选择筐
				}
				else if(level == 2)
				{
					VGA_box(47*4, 47*4, 51*4 - 1, 51*4 - 1,0xD3E5);//在difficult位置显示选择筐
				}
				while(*KEY_ptr)
				*(red_LED_ptr) = 0x0;//直到松开按键 关闭所有的LED灯				
			}
		}
				
		/* check for PS/2 data--display on HEX displays */
		PS2_data = *(PS2_ptr);								// 读取ps2端口的按键值
		RVALID = PS2_data & 0x8000;							// 获取有效的数据位
		if (RVALID)
		{
			/* shift the next data byte into the display */
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
			if(byte3==0x70 )//按ps/ 2 键盘数字键 0 进入游戏
			{
				game(level);
			}
			else
			{
				HEX_PS2 (byte1, byte2, byte3);
			}
			

			if ( (byte2 == (char) 0xAA) && (byte3 == (char) 0x00) )
				// mouse inserted; initialize sending of data
				*(PS2_ptr) = 0xF4;
		}
	}
}
void delay_ms(int dly)
{
	int delay,delay2;
	delay = tick;
	for(;dly>0;dly--)
	{
		while(tick == delay);
		delay = tick;
		//for(delay2=0;delay2<100000;delay2++);	
		//	for(delay=0;delay<100000;delay++);//delay 处理
	}
}
/*******************************************************************************
 * Subroutine to send a string of text to the VGA monitor 
 ******************************************************************************/
void VGA_text(int x, int y, char * text_ptr)
{
	int offset;
  	volatile char * character_buffer = (char *) FPGA_CHAR_BASE;	// VGA character buffer

	/* assume that the text string fits on one line */
	offset = (y << 7) + x;
	while ( *(text_ptr) )
	{
		*(character_buffer + offset) = *(text_ptr);				// write to the character buffer
		++text_ptr;
		++offset;
	}
}

/*******************************************************************************
 * Draw a filled rectangle on the VGA monitor 
*******************************************************************************/
void VGA_box(int x1, int y1, int x2, int y2, short pixel_color)
{
	int pixel_ptr, row, col;

	/* assume that the box coordinates are valid */
	for (row = y1; row <= y2; row++)
		for (col = x1; col <= x2; ++col)
		{
			pixel_ptr = FPGA_ONCHIP_BASE + (row << 10) + (col << 1);
			*(short *)pixel_ptr = pixel_color;		// set pixel color
		}
}

void VGA_pic(int x1, int y1, int x2, int y2, int * pic_dat)
{
	int pixel_ptr, row, col;
	int pbuff = 0;

	/* assume that the box coordinates are valid */
	for (row = y1; row < y2; row++)
		for (col = x1; col < x2; ++col)
		{
			pixel_ptr = FPGA_ONCHIP_BASE + (row << 10) + (col << 1);
			*(short *)pixel_ptr = pic_dat[pbuff];		// set pixel color
			pbuff++;
		}
}
void VGA_pic_c(int x1, int y1, int x2, int y2, unsigned char * pic_dat)
{
	int pixel_ptr, row, col;
	int pbuff = 0;
	int px;

	/* assume that the box coordinates are valid */
	for (row = y1; row < y2; row++)
		for (col = x1; col < x2; ++col)
		{
			pixel_ptr = FPGA_ONCHIP_BASE + (row << 10) + (col << 1);
			px = pic_dat[pbuff++];			
			px +=  (pic_dat[pbuff++] << 8);
			if(px != 0)
				*(short *)pixel_ptr = px;//pic_dat[pbuff];		// set pixel color
			//pbuff++;
		}
}

/*******************************************************************************
 * Subroutine to show a string of HEX data on the HEX displays
*******************************************************************************/
void HEX_PS2(char b1, char b2, char b3)
{
	volatile int * HEX3_HEX0_ptr = (int *) HEX3_HEX0_BASE;
	volatile int * HEX5_HEX4_ptr = (int *) HEX5_HEX4_BASE;
	/* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in 
	 * a single 7-seg display, for the hex digits 0 - F */
	unsigned char	seven_seg_decode_table[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 
		  										0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };
	unsigned char	hex_segs[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned int shift_buffer, nibble;
	unsigned char code;
	int i;

	shift_buffer = (b1 << 16) | (b2 << 8) | b3;
	for ( i = 0; i < 6; ++i )
	{
		nibble = shift_buffer & 0x0000000F;		// character is in rightmost nibble
		code = seven_seg_decode_table[nibble];
		hex_segs[i] = code;
		shift_buffer = shift_buffer >> 4;
	}
	/* drive the hex displays */
	*(HEX3_HEX0_ptr) = *(int *) (hex_segs);
	*(HEX5_HEX4_ptr) = *(int *) (hex_segs+4);
}

/*******************************************************************************
 * Subroutine to read KEYs
 ******************************************************************************/
void check_KEYs(int * KEY0, int * KEY1, int * counter)
{
	volatile int * KEY_ptr = (int *) KEY_BASE;
	volatile int * audio_ptr = (int *) AUDIO_BASE;
	int KEY_value;

	KEY_value = *(KEY_ptr); 				// read the pushbutton KEY values
	while (*KEY_ptr);						// wait for pushbutton KEY release

	if (KEY_value == 0x1)					// check KEY0
	{
		// reset counter to start recording
		*counter = 0;
		// clear audio-in FIFO
		*(audio_ptr) = 0x4;
		*(audio_ptr) = 0x0;

		*KEY0 = 1;
	}
	else if (KEY_value == 0x2)				// check KEY1
	{
		// reset counter to start playback
		*counter = 0;
		// clear audio-out FIFO
		*(audio_ptr) = 0x8;
		*(audio_ptr) = 0x0;

		*KEY1 = 1;
	}
}
#if(1)
int game(int l) {
	int key[]={0x6c,0x75,0x7d,0x6b,0x73,0x74,0x69,0x72,0x7a};
	int xx[]={75,150,230,65,153,230,60,150,240};
	int yy[]={95,145,200};
	int times = 0;/*游戏次数*/
	int mousey = 0;/*老鼠所在行号*/
	int mousex = 0;/*老鼠所在列号*/
	int posy = 0;/*锤子所在行号*/
	int posx = 0;/*锤子所在列号*/
	int hits = 0;/*打中次数*/
	int missed = 0;/*错过次数*/;
	int num = 0, row = 0, col = 0;
	int i,j;
	int level;
	
	volatile int * PS2_ptr = (int *) PS2_BASE;					// PS/2 port address
	//int  RVALID;
	//srand(time(0));
	//获得游戏次数
	//printf("请输入次数：");
	times = 100;//scanf("%d", &times);
	//打印地图
	//printf("***\n***\n***\n");
	//游戏过程	
	if(l == 1)
	{
		l = 100;
	}
	else if(l == 2)
	{
		l = 60;
	}
	else if(l == 0)
	{
		l = 120;
	}

	for (num = 1;num <= times;num++) {
		//获得老鼠和锤子的位置
        	mousey = rand() % 3;// + 1;
		mousex = rand() % 3;// + 1;
//打印地图
			VGA_pic(0,0,319,239,picdat2);
        	for (row = 0;row < 3;row++) {//for (row = 1;row <= 3;row++) {
			for (col = 0;col < 3;col++) {//for (col = 1;col <= 3;col++) {
			/*	if (row == posy && col == posx) {
					VGA_text (38-col, 34-row, "O");//printf("O");
				}
				else*/
 				if (row == mousey && col == mousex) {
					VGA_pic_c(xx[row*3+col]-25,yy[row]-50,xx[row*3+col]+25,yy[row],gImage_melo);
					//VGA_text (20+col*8, 25+row*8, "X");//printf("X");
				}
				else {
					//VGA_text (20+col*8, 25+row*8, "*");//printf("*");
				}
				}
			}
		level = l;
		while(--level)//等待时间,控制等待时间，决定游戏难易程度
		{
			if( *(int *) KEY_BASE == 8)
				return 0;
			get_ps2();//PS2_data = *(PS2_ptr);								// read the Data register in the PS/2 port
			//RVALID = PS2_data & 0x8000;							// extract the RVALID field
			if(RVALID)
			{
				delay_ms(2);//delay 处理
				get_ps2();//PS2_data = *(PS2_ptr);								// read the Data register in the PS/2 port
				//RVALID = PS2_data & 0x8000;							// extract the RVALID field
				if(!RVALID)
					continue;
			}
			if (RVALID)
			{
				posy = PS2_data & 0xFF;
				for(i=0;i<3;i++)
				{
					for(j=0;j<3;j++)
					{
						if(posy == key[i*3+j])//0x6c)//
						{
							posy = i;
							posx = j;
							i = j = 5;//	
						}
					}
				}
				if(i == 6)
				{
					break;
				}
			}
			delay_ms(1);//delay 处理
		}	
		
		//修改打中和错过的个数
		if (mousey == posy && mousex == posx) {
			hits++;
		}
		else {
			missed++;
		}
		//打印地图
			
        	for (row = 0;row < 3;row++) {//for (row = 1;row <= 3;row++) {
			for (col = 0;col < 3;col++) {//for (col = 1;col <= 3;col++) {
				if (row == posy && col == posx) {
					
					//VGA_text (20+col*8, 25+row*8, "O");//printf("O");
				}
/*				else if (row == mousey && col == mousex) {
					VGA_text (38-col, 34-row, "X");//printf("X");
				}*/
				else {
					//VGA_text (20+col*8, 25+row*8, "*");//printf("*");
				}
			}
			//printf("\n");
		}
		//提示是否打中
/*		if (mousey == posy && mousex == posx) {
			VGA_text (36, 39, "YES");//printf("打中了\n");
		}
		else {			
			VGA_text (36, 39, "NO ");//printf("没打中\n");
		}
*/
		PS2_data = *(PS2_ptr);								// read the Data register in the PS/2 port
		RVALID = PS2_data & 0x8000;							// extract the RVALID field
		while (RVALID)
		{
			PS2_data = *(PS2_ptr);								// read the Data register in the PS/2 port
			RVALID = PS2_data & 0x8000;							// extract the RVALID field
		}
	{//
		char text_top_row[40]={0};
		sprintf(text_top_row,"hits=%d,missed=%d!", hits, missed);
		_CNT = ((hits / 100) << 8) + (((hits % 100) / 10) << 4) +( (hits % 10));// / (hits + missed)
		
		UPDATE_HEX_DISPLAY();//cnt++);
//		VGA_text(30,48,text_top_row);
	}			
		//打印总成绩
		//printf("打中%d次，错过%d次\n", hits, missed);
	}
	return 0;
}
#endif
