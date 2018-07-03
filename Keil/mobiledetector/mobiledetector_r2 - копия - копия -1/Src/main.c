/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_hal.h"
#include "adc.h"
#include "i2c.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <mb_slave.h>
#include <modbus_config.h>
#include <math.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

mb_Regs Params;
unsigned * BUF1 = (unsigned *) &(Params.var_res_level);
unsigned * BUF2 = (unsigned *) ((uint32_t)0x08008000-FLASH_PAGE_SIZE);
mb_Regs * Flash_Params = (mb_Regs *) FLASH_LOCATION;
unsigned short * PARAMS_BUF = (unsigned short *) & (Params.var_res_level);
volatile uint32_t NbrOfPage = 0x00;
uint32_t EraseCounter = 0x00, Address = 0x00;
HAL_StatusTypeDef flash_status;


uint8_t init = 1;
bool adc_new = false;
bool i2c_new = false;
bool adc_flag = false;
float i2c_data = 0;
bool i2c_msg_flag = false;
bool flag_T = true;
bool i2c_is_busy = false;
uint8_t tick = 0;	
uint8_t var_res_level_old = 0;
uint32_t shnt_time = 0;
uint16_t tickcounter = 0;

	
bool isPassword = false;
uint16_t Password = 0x0101;
MbErrorCodes_t usart_init_msg;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void Flash_Store( void );
bool Flash_Restore( void );
void usart_int_enable( void );
void adc_getval( mb_Regs *Params, bool flag );
//void i2c_getval( mb_Regs *Params, uint8_t flag );
void get_sens_data(void);
void ResetFilter_data( mb_Regs *Params);
void Set_var_res( mb_Regs *Params );
void Set_var_res_null( mb_Regs *Prm );
void Check_alarm( mb_Regs *Params );

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  MX_IWDG_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN 2 */
	
	
	
	Set_var_res_null( &Params );
	
	bool read_flash = Flash_Restore();
	
	usart_init_msg = mb_slave_init( &m_mbParam );
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		if( Params.Flags_write & FLAG_WRITE )
		{
				Params.Flags_write &= ~FLAG_WRITE;
				Flash_Store();

		}
		
		if( Params.Flags_write & SHUNT_FLAG )
		{	
			//SHUNT ON
		}
		else
		{
				//SHUNT OFF
		}
		Set_var_res( &Params );
		if( tick >= 100 )
		{
		
			tick = 0;
			get_sens_data();
		
		}
	//Check_alarm( &Params );
		
	

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

void TIM4_IRQHandler(void)			// ������ ����������� ���������������� � stm32f1xx_it.c
{

		__HAL_TIM_CLEAR_IT( &htim4, TIM_IT_UPDATE);
		adc_flag = true;
		
	if( !i2c_is_busy )
		i2c_msg_flag = true;

};

int tyy = 0;
void TIM3_IRQHandler( void )				// ������ ����������� ���������������� � stm32f1xx_it.c
{
			__HAL_TIM_CLEAR_IT( &htim3, TIM_IT_UPDATE);
					mb_slave_timer_expired(&m_mbParam);

}



uint32_t m_hostTimeout = 0;
bool m_isPasswordValid = false;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{
		static bool isRidgeEn = false;
		
		if( huart->Instance == USART1)
		{
			
			uint8_t err = HAL_UART_GetError( &huart1 );
			
			/*
			#define HAL_UART_ERROR_NONE      ((uint32_t)0x00)    /*!< No error            
			#define HAL_UART_ERROR_PE        ((uint32_t)0x01)    /*!< Parity error        
			#define HAL_UART_ERROR_NE        ((uint32_t)0x02)    /*!< Noise error         
			#define HAL_UART_ERROR_FE        ((uint32_t)0x04)    /*!< frame error         
			#define HAL_UART_ERROR_ORE       ((uint32_t)0x08)    /*!< Overrun error       
			#define HAL_UART_ERROR_DMA       ((uint32_t)0x10)    /*!< DMA transfer error  
			*/
			
			
			
			uint8_t data = (uint8_t)huart->Instance->DR;
			
			MbErrorCodes_t err_msg = mb_slave_recive( &m_mbParam, data );
		//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
//			
//			uint8_t data11 = huart->Instance->DR;
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
//			HAL_UART_Transmit_IT(&huart1, &data11, 1);
//			while(HAL_UART_GetState( &huart1 ) == HAL_UART_STATE_BUSY)
//				{};
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		
		};

};


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if( huart->Instance == USART1)
	{
	 if( huart->Instance == USART1 )
			mb_tx(&m_mbParam);
	}
};





MbExceptionCodes_t mb_read_registers( uint8_t *lpDstBuf, uint16_t startAddr, uint16_t count ) {
	if(lpDstBuf == NULL)
		return MB_EXC_ILLEGAL_FUNCTION;
	
	isPassword = false;
	m_hostTimeout = 0;
	m_isPasswordValid = false;
	uint16_t endAddr = startAddr + count;
	
	int registersCount = sizeof(Params) / 2;
	
	if(startAddr < registersCount) {	
		// ?????? ???????????? ?????????
		if( endAddr > registersCount )
			return MB_EXC_ILLEGAL_DATA_ADDR;
		
		uint16_t *lpRegs = (uint16_t*)&Params;
		uint16_t reg;
		//
		for(uint32_t i = startAddr, j = 0; i < endAddr; i++, j += 2) {
			reg = lpRegs[i];
			lpDstBuf[j] = (uint8_t)(reg >> 8);
			lpDstBuf[j + 1] = (uint8_t)(reg);
		}
		//
		return MB_EXC_NONE;
	}
	
	//-------------------------------------------------------------------------
	uint16_t size = strlen( MODBUS_DEVICE_INFO );
	
	// ?????? ?????????? ???????? ? ?????????? ?? ??????????
	if(startAddr == InfoSizeRegNum) {
		
		if(count != 1) {
			return MB_EXC_ILLEGAL_DATA_VALUE;
		}
		//
		lpDstBuf[0] = (uint8_t)size >> 8;
		lpDstBuf[1] = (uint8_t)(size);
		//
		return MB_EXC_NONE;
	}
	
	// -----------------------------------------------------------------------
	// ?????? ?????????? ?? ??????????
	if(startAddr == InfoRegNum) {
		
		if(count != size) {
			return MB_EXC_ILLEGAL_DATA_VALUE;
		}
		
		char *ptr = MODBUS_DEVICE_INFO; // tBuf;
		uint16_t reg;
		
		for(uint32_t i = 0, j = 0; i < size; i++, j += 2) {
			reg = (uint16_t)ptr[i];
			lpDstBuf[j] = (uint8_t)(reg >> 8);
			lpDstBuf[j + 1] = (uint8_t)(reg);
		}
		//
		return MB_EXC_NONE;
	}
	
	return MB_EXC_ILLEGAL_FUNCTION;
};



MbExceptionCodes_t mb_write_registers( uint16_t startAddr, uint16_t count, uint8_t *lpSrcBuf ) {
	
//	if( !isPassword )
//	{
//	
//		if(((lpSrcBuf[0]<<8) + lpSrcBuf[1]) == Password	)
//			isPassword = true;
//			
//		return NULL;
//	}
	
	
	
	if(lpSrcBuf == NULL)
		return MB_EXC_ILLEGAL_FUNCTION;
	
	m_hostTimeout = 0;
	m_isPasswordValid = false;
	uint16_t endAddr = startAddr + count;
	uint16_t *tmp_ptr = (uint16_t *)&Params;
	
	uint16_t tmp_reg_val;
	
	
	//int i = startAddr;
	int j = startAddr;
	
	
	
	for( int i = startAddr; i < endAddr; i++)
	{
		tmp_reg_val = (lpSrcBuf[j - startAddr ] << 8) + lpSrcBuf[j - startAddr+1];
		
		//tmp_ptr[i+2] = lpSrcBuf[i - startAddr];
		tmp_ptr[i] = tmp_reg_val;
		j+=2;
//		tmp_ptr [1] = 0x77;
//		tmp_ptr [0] = 0x9988;
//	
	}
	
	
	
	

		//
		return MB_EXC_NONE;
	
	

	//return MB_EXC_ILLEGAL_FUNCTION;
	
};





uint16_t f_size1 = (((unsigned short *)&Params.crc - (unsigned short *)&Params.var_res_level)/2+1);
void  Flash_Store( void )
{

		EnterCritSection();
		
		uint8_t j=0;
		uint32_t end_addr = ((uint32_t)0x08008000);
		uint32_t flash_location = ((uint32_t)0x08008000-FLASH_PAGE_SIZE+4)	;
		
	
	
		for ( int i=0;i<f_size1;i++)
		{
			if (BUF1[i]!=BUF2[i])
				j=1;
		}
		
		if(!j) {
			ExitCritSection();
			return;
		}
	
		HAL_FLASH_Unlock();
		
		
		NbrOfPage = (end_addr - (flash_location-4)) / FLASH_PAGE_SIZE;
//		HAL_FLASH_Unlock();
		//�������
		for(EraseCounter = 0; (EraseCounter < NbrOfPage); EraseCounter++)
		{
			
			//FLASH_PageErase(flash_location - 4 + (FLASH_PAGE_SIZE * EraseCounter));			
			FLASH_PageErase(FLASH_LOCATION + (FLASH_PAGE_SIZE * EraseCounter));
			while(FLASH->SR & FLASH_SR_BSY )
			{}
			//FLASH_WaitForLastOperation(10);
		}
		CLEAR_BIT(FLASH->PECR,FLASH_PECR_ERASE);
		CLEAR_BIT(FLASH->PECR,FLASH_PECR_PROG);
		HAL_FLASH_Lock();

		Address = FLASH_LOCATION;
		int i=FLASH_SIZE;

		Params.crc = CalcCrc16((unsigned char *) & (Params.var_res_level), f_size1*4-4);
		
		HAL_FLASH_Unlock();

		for (i=0;i<f_size1;i++) {
			if (BUF1[i]!=BUF2[i])
			{
			//	FLASH->CR &= ~(FLASH_CR_PER);
			//	FLASH->CR |= FLASH_CR_PG;
				FLASH->SR |= FLASH_FLAG_PGAERR;
				FLASH->ACR &= ~(1<<2);
				flash_status = HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, flash_location-4+(i*4), BUF1[i]);		//BUF1[i]
				while(FLASH->SR & FLASH_SR_BSY )
				{}
			}
		}
		//flash_status = HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, 0x08007C96, 0x09);
			
		HAL_FLASH_Lock();
			
		ExitCritSection();
				
	
};







	uint16_t sdf;
uint32_t sdfgdf;

bool Flash_Restore( void )
{
	uint16_t crc;
	uint16_t f_size = (((unsigned short *)&Params.crc - (unsigned short *)&Params.var_res_level)/2+1);
	sdf = ((unsigned short *)&Params.crc - (unsigned short *)&Params.var_res_level);
	sdf = (((unsigned short *)&Params.crc - (unsigned short *)&Params.var_res_level)/2+1);
	sdf = (((unsigned short *)&Params.crc - (unsigned short *)&Params.var_res_level)/2+1) * 4;
	sdf = (((unsigned short *)&Params.crc - (unsigned short *)&Params.var_res_level)/2+1) * 4 - 4;
	sdfgdf = ((unsigned short *)&Params.crc - (unsigned short *) & Params.var_res_level);
	sdfgdf = FLASH_LOCATION + ((unsigned short *)&Params.crc - (unsigned short *) & Params.var_res_level)*2;
	
//	sdfgdf = *((uint16_t *)(FLASH_LOCATION + sdfgdf*2));
//	sdfgdf = FLASH_LOCATION;
//	sdfgdf = FLASH_PAGE_SIZE;
//	sdfgdf = ((uint32_t)0x08008000-FLASH_PAGE_SIZE);
	
//	sdf = FLASH_SIZE;
//	sdf = (FLASH_SIZE)*4;
//	sdf = FLASH_SIZE*4-4;
	
	crc = CalcCrc16((unsigned char *) Flash_Params, sdf);
	if (crc != *((uint16_t *)(sdfgdf)))		//(FLASH_LOCATION + sdfgdf*2)
	
	{
		for (int i = 0; i < f_size ; i++)
			PARAMS_BUF[i] = 0;
		return false;
		
	}
	else
	{
		for (int i = 0; i < f_size  ; i++)
			BUF1[i] = BUF2[i];
		return true;
		
	}
};



void get_sens_data(void)
{


		

//	
//		uint16_t tmp = HAL_ADC_GetValue(&hadc);
//		Params.Sensor_voltage = ((tmp * V_ref) / 4096.0f)*1000;
//		HAL_ADC_Start( &hadc );
		//	adc_getval( &Params, adc_flag );
	//	adc_new = true;
	//	Filter_data( &Params );
		float kA = -2327;
		float kA1 = -2.327;
		float kB = 241500;
		float kB1 = 241.5;
		uint16_t tmp = HAL_ADC_GetValue(&hadc);
		Params.Sensor_voltage = ((tmp * V_ref) / 4096.0f)*1000;
		
	
	
	
	
	
		
	
		float Sensor_voltage = ((tmp * V_ref) / 4096.0f)*1000;
		float s_v = (((float)tmp * V_ref) / 4096.0f);
		float v_r = (float)Params.ref_null/1000;
		float tst1 = (s_v - v_r)/(kA1 * (float)Params.var_res_level + kB1);
		float tst = (Sensor_voltage - (float)Params.ref_null)/(kA * (float)Params.var_res_level + kB);
		Params.test1 = (short)(tst * 1000);
		Params.test2 = (short)(tst1 * 1000);
		//Params.Sensor_voltage = (1000*( Sensor_voltage - (Params.ref_null/10) ))/( kA * Params.var_res_level + kB );
		HAL_ADC_Start( &hadc );
	
	
	
	
	
	
	
		if(i2c_msg_flag)
		{
			if(flag_T)
			{
				i2c_data = I2C_StartTransmission( T );
				i2c_is_busy = true;
				//if( i2c_data != Write_Error && i2c_data !=Writed_T && i2c_data !=Read_Error && i2c_data !=Crc8_Error && i2c_data != Conversion_Error )
				if( i2c_data != Write_Error && i2c_data !=Writed_T && i2c_data !=Read_Error && i2c_data !=Crc8_Error && i2c_data != Conversion_Error )
				{
					Params.Temperature = i2c_data;
					flag_T = false;
					i2c_is_busy = false;
					
				}
				
				if(i2c_data == Writed_T)
					i2c_is_busy = false;
				
				
				if( i2c_data == Write_Error || i2c_data ==Read_Error || i2c_data ==Crc8_Error || i2c_data == Conversion_Error)
				{
					flag_T = false;
					i2c_is_busy = false;
				}
			}
			else
			{
			
				i2c_data = I2C_StartTransmission( RH );
				i2c_is_busy = true;
				//if( i2c_data != Write_Error && i2c_data !=Writed_T && i2c_data !=Read_Error && i2c_data !=Crc8_Error && i2c_data != Conversion_Error )
				if( i2c_data != Write_Error && i2c_data !=Writed_T && i2c_data !=Read_Error && i2c_data !=Crc8_Error && i2c_data != Conversion_Error )
				{
					Params.Humidity = i2c_data;
					flag_T = true;
					i2c_is_busy = false;
				}
				if( i2c_data == Write_Error || i2c_data ==Read_Error || i2c_data ==Crc8_Error || i2c_data == Conversion_Error)
				{
					flag_T = true;
					i2c_is_busy = false;
				}
				if( i2c_data == Writed_T )
					i2c_is_busy = false;
			
			}
			i2c_msg_flag = false;
			i2c_new = true;
			if( init == 1 )
			{
				ResetFilter_data( &Params );
				init = 0;
			}
			else
				return;
//				Filter_data( &Params );
		}
		
		


};



void ResetFilter_data( mb_Regs *Params)
{

	i2c_new = false;
	adc_new = false;

	

};




void usart_int_enable( void )
{

				
						// ???????????????????????????????????????????????
			/* Enable the UART Parity Error Interrupt */
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_PE);

			/* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);

			/* Enable the UART Data Register not empty Interrupt */
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	
	

		//	__HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
			
			

};


void Set_var_res( mb_Regs *Params )
{

	if( Params->var_res_level != var_res_level_old)
	{

		
		
		uint8_t step_count = (abs(Params->var_res_level - var_res_level_old)) * 2;
		
		
		if( Params->var_res_level > var_res_level_old )
			VAR_RES_UD( UP );
		else if( Params->var_res_level < var_res_level_old )
			VAR_RES_UD( DOWN );
		
		
		VAR_RES_NCS( DOWN );		
		VAR_RES_SCLK( UP );
		
		for( int i = step_count; i >= 0; i--)
		{
				HAL_Delay( 1 );
				HAL_GPIO_TogglePin(VAR_RES_SCLK_GPIO_Port, VAR_RES_SCLK_Pin );
				
		}
		
		
		VAR_RES_NCS( UP );
		
		
		var_res_level_old = Params->var_res_level;
	}
	else
		return;
		
//		if( Params->var_res_level > var_res_level_old)
//		{
//		
//			VAR_RES_UD( UP );
//			uint8_t step_count = (Params->var_res_level - var_res_level_old) * 2;
//			
//			for( int i = step_count; i >= 0; i--)
//			{
//				
//				HAL_GPIO_TogglePin(VAR_RES_SCLK_GPIO_Port, VAR_RES_SCLK_Pin );
//				HAL_Delay( 1 );
//			
//			}
//		
//		}
//		else if( Params->var_res_level < var_res_level_old)
//		{
//		
//			VAR_RES_UD( DOWN );
//			uint8_t step_count = (var_res_level_old - Params->var_res_level) * 2;
//			
//			for( int i = step_count; i >= 0; i--)
//			{
//				
//				HAL_GPIO_TogglePin(VAR_RES_SCLK_GPIO_Port, VAR_RES_SCLK_Pin );
//				HAL_Delay( 1 );
//			
//			}
//		
//		}
		



};


void Set_var_res_null( mb_Regs *Prm )
{
			Prm->var_res_level = 64;
			Set_var_res(Prm);
	
			Prm->var_res_level = 0;
			Set_var_res(Prm);



};



void HAL_SYSTICK_Callback(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SYSTICK_Callback could be implemented in the user file
   */
	
	tick >= 250 ? tick = 0: tick++;
	
	
}



void Check_alarm( mb_Regs *Params )
{

		if( Params->Sensor_voltage > (Params->pos_tresh + Params->ref_null) )
			LED3(UP);
		else
			LED3(DOWN);
		
		if( Params->Sensor_voltage > (Params->pos_Tresh_Danger + Params->ref_null) )
			LED2(UP);
		else
			LED2(DOWN);
		
		
		if( Params->Sensor_voltage < (Params->ref_null - Params->neg_tresh ))		// ���� �����
				LED5(UP);
		else
			LED5(DOWN);
		
		if( Params->Sensor_voltage < (Params->ref_null - Params->neg_Tresh_Danger) )	//���� ������
			LED4(UP);
		else
			LED4(DOWN);
		
		
		if( /*(Params->Sensor_voltage > Params->shunt_tresh) || (Params->Sensor_voltage < (Params->ref_null - Params->shunt_tresh)) ||*/ Params->Flags_write & SHUNT_SET )
		{
			
			SHUNT_ON(UP);
			if( !Params->write_reg & SHUNT_FLAG )
			{
				shnt_time = HAL_GetTick();
			}
			
			Params->write_reg |= SHUNT_FLAG;
			
			
			
			
		}
		else
		{
			SHUNT_ON(DOWN);
			Params->write_reg &= ~SHUNT_FLAG;
		
		
		}
		
		
		

};







/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
