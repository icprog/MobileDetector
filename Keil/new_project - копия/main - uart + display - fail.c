#include <LPC17xx.H>
#include <system_LPC17xx.h>
#include <lpc17xx_pinsel.h>
//#include <lpc17xx_gpio.h>
//#include <lpc17xx_systick.h>
//#include <lpc17xx_uart.h>
#include <lpc17xx_ssp.h>
//#include <lpc17xx_pwm.h>
//#include <stdbool.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <LPC17xx_timer.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define DISPLAY_DATA			0xFA00
#define DISPLAY_CMD				0xF800
#define DISPLAY_ROWS_COUNT		2
#define DISPLAY_ROW_LEN			40

#define DISPROW1_HDR			"Read ADC:"

static uint16_t m_DispRows[DISPLAY_ROWS_COUNT][DISPLAY_ROW_LEN];

void delay(uint32_t count);
uint8_t select_channel_adc(uint8_t channel);
#include <LPC17xx.H>
#include <system_LPC17xx.h>
#include <lpc17xx_pinsel.h>
//#include <lpc17xx_gpio.h>
//#include <lpc17xx_systick.h>
//#include <lpc17xx_uart.h>
#include <lpc17xx_ssp.h>
//#include <lpc17xx_pwm.h>
//#include <stdbool.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <LPC17xx_timer.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define DISPLAY_DATA			0xFA00
#define DISPLAY_CMD				0xF800
#define DISPLAY_ROWS_COUNT		2
#define DISPLAY_ROW_LEN			40
#define DISPROW1_HDR			"Read ADC:"
#define clear	   			0xF801
#define home				0xF802
#define disp_Success		0xFA53
#define disp_Fail			0xFA46
#define ADC_CS_ON			LPC_GPIO0 -> FIOCLR = (1 << 2);
#define ADC_CS_OFF			LPC_GPIO0 -> FIOSET = (1 << 2);	
#define DISP_CS_ON			LPC_GPIO0 -> FIOCLR = (1 << 3);
#define DISP_CS_OFF			LPC_GPIO0 -> FIOSET = (1 << 3);

static uint16_t m_DispRows[DISPLAY_ROWS_COUNT][DISPLAY_ROW_LEN];

void delay(uint32_t count);


void disp_show(char* lpStr, int strLen, int strNum, int column, bool isFullRow);
void disp_update( void );

uint16_t receivedata[20];
	SSP_CFG_Type SSPCFG;

uint16_t del;
uint8_t receivedata_first[20];
uint8_t receivedata_second[20];
uint8_t receivedata_third[20];


uint8_t k=0;
int tmp;

int main (){
	
	 
	
	


	
	memset( m_DispRows[0], 0, DISPLAY_ROW_LEN );
	memset( m_DispRows[1], 0, DISPLAY_ROW_LEN );
		

	LPC_PINCON -> PINSEL0 = 0xA8000;    

	LPC_GPIO0 -> FIODIR |= (1 << 3)|(1 << 2);


	
	
	ADC_CS_OFF;
	DISP_CS_OFF;
	delay(10000);
	
	

	
	SSPCFG.Databit = SSP_DATABIT_16;
	SSPCFG.CPHA = SSP_CPHA_FIRST;
	SSPCFG.CPOL = SSP_CPOL_HI;
	SSPCFG.Mode = SSP_MASTER_MODE;
	SSPCFG.FrameFormat = SSP_FRAME_SPI;
	SSPCFG.ClockRate = 50000;
	
	
	
	SSP_Init(LPC_SSP1, &SSPCFG);
	
	SSP_Cmd(LPC_SSP1, ENABLE);
	
	ADC_CS_OFF
	delay(100);
	DISP_CS_ON
	
	
	delay(100);
	SSP_SendData(LPC_SSP1, 0xF80C);		//Disp On
	delay(100);	
	
	
	SSP_SendData(LPC_SSP1, 0xF828);		//Func_set
	delay(100);
	
	SSP_SendData(LPC_SSP1, clear);
	delay(100);
	
	
	SSP_SendData(LPC_SSP1, home);
	delay(100);
	
	
	
	


	SysTick_Config(SystemCoreClock / 10);

  while(1){
	
	  
	 DISP_CS_ON 
	disp_show( DISPROW1_HDR, strlen(DISPROW1_HDR), 0, 0, true);
//	for(int j =0; j < 81; j++){
	disp_update();
//		delay(10000);
//	};
	delay(10000);
	

	
		

	


		
}
	
}

void SysTick_Handler(void) {
	
	
	
	DISP_CS_OFF
	 delay(100);
	ADC_CS_ON
	
	
	uint8_t channel = 6;
	uint8_t startbyte =	0x87 + (channel << 4);	
	
	  
	  
//	LPC_SSP1 -> CR0 = 0x7; 		//8 bit
	
	SSP_DeInit(LPC_SSP1);

	SSPCFG.Databit = SSP_DATABIT_8;
	SSPCFG.CPHA = SSP_CPHA_FIRST;
	SSPCFG.CPOL = SSP_CPOL_HI;
	SSPCFG.Mode = SSP_MASTER_MODE;
	SSPCFG.FrameFormat = SSP_FRAME_SPI;
	SSPCFG.ClockRate = 50000;
	
	
	
	
	
	SSP_Init(LPC_SSP1, &SSPCFG);
	
	
	
	del = SSP_ReceiveData(LPC_SSP1);

	
	
	
	SSP_SendData(LPC_SSP1, startbyte);
	
	delay(100);
//	while(SSP_GetStatus(LPC_SSP1, SSP_STAT_BUSY) == SET) {}		

	receivedata_first[k] = SSP_ReceiveData(LPC_SSP1);
	
	SSP_SendData(LPC_SSP1, 0x00);
	delay(100);
//	while(SSP_GetStatus(LPC_SSP1, SSP_STAT_BUSY) == SET) {}	
	receivedata_second[k] = SSP_ReceiveData(LPC_SSP1);
		
	SSP_SendData(LPC_SSP1, 0x00);
	delay(100);
//	while(SSP_GetStatus(LPC_SSP1, SSP_STAT_BUSY) == SET) {}	
	receivedata_third[k] = SSP_ReceiveData(LPC_SSP1);
	
		
	
	

	
	receivedata[k] = ((uint16_t)receivedata_first [k] << 9) + ((uint16_t)receivedata_second [k] << 1) + ((uint16_t)receivedata_third [k] >> 7);
	
	
	
	
	ADC_CS_OFF;	
//	LPC_SSP1 -> CR0 = 0xF; 		//16 bit
	
		
	SSP_DeInit(LPC_SSP1);
	SSPCFG.Databit = SSP_DATABIT_8;
	SSPCFG.CPHA = SSP_CPHA_FIRST;
	SSPCFG.CPOL = SSP_CPOL_HI;
	SSPCFG.Mode = SSP_MASTER_MODE;
	SSPCFG.FrameFormat = SSP_FRAME_SPI;
	SSPCFG.ClockRate = 50000;
	
	
	SSP_Init(LPC_SSP1, &SSPCFG);	
		
		
		
	delay(100);
		
	float u_val;
	float u_ref = 2.5;
	u_val = ((float)receivedata[k] * u_ref)/((1 << 16)-1);
		
		char tBuf[12];
	memset( tBuf, 0, 12 );
	int len = sprintf(tBuf, "%04f", u_val);
	tBuf[len] = '\0';
	disp_show( "  ", 1, 1, 0, false);
	

	disp_show( tBuf, len+1, 1, 2, false);
	delay(1000);
	
	
	float R1=1000;
	
	float Rt= (  R1 * u_val )/( u_ref - u_val);
	
	
//	float R2= 3300/2;
//	float Rt= ( R2*u_ref - R1*u_val )/( u_val - u_ref );
	
	
	float temperature;
	
	float A = 3.81E-3;
	
	temperature = (Rt - R1)/ A;
		
	sprintf(tBuf, "%04f", temperature);
	
	disp_show( tBuf, 18, 0, 11, true);
	
		
		
	
	
	k > 19 ? k=0 : k++;
	ADC_CS_OFF
	delay(100);
	DISP_CS_ON
	
	
	
	
	
	
};


void delay(uint32_t count) {
	volatile uint32_t tmp = count;
	while((--tmp) != 0) {
	};
}

void disp_show(char* lpStr, int strLen, int strNum, int column, bool isFullRow) {
	if((lpStr == (char*)NULL) || (strNum >= 2) || (strLen <= 0))
		return;
	
	for(int i = column  ; i < DISPLAY_ROW_LEN; i++) {
		if(i < strLen) {
			m_DispRows[strNum][i] = DISPLAY_DATA | (*lpStr);
			lpStr++;
		}
		else if(isFullRow){
			m_DispRows[strNum][i] = DISPLAY_DATA | ' ';
		}
		
	}
}

void disp_update( void ) {
	
	//while(SSP_GetStatus(LPC_SSP1, SSP_STAT_BUSY) == SET) {}
	ADC_CS_OFF;
	DISP_CS_ON;
	delay(1000);
	static int row = 0;
	static int col = 0;
	
	SSP_SendData(LPC_SSP1, m_DispRows[row][col]);
	
	col++;

	if(col >= DISPLAY_ROW_LEN) {
			col = 0;
			row++;
			if(row >= DISPLAY_ROWS_COUNT) {
				row = 0;
				SSP_SendData(LPC_SSP1, home);	
			}
		}
	
	//SSP_SendData(LPC_SSP1, m_DispRows[row][col]);
	
	
	
	//while(SSP_GetStatus(LPC_SSP1, SSP_STAT_BUSY) == SET) {}	
		
	delay(1000);
	DISP_CS_OFF;
	ADC_CS_ON;
	delay(1000);
	
}








