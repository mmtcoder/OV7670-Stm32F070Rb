/*
 * ov7670.c
 *
 *  Created on: 6 Haz 2021
 *      Author: H3RK3S
 */
/*
 *  ov7670 camera initializing codes are taken by Engin KAVAK
 *
 */
#include "ov7670.h"
#include "stm32f0xx_hal.h"
#include "main.h"
#include  "ILI9341_GFX.h"

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

//This settings includes RGB565 format,QQVGA format and color optimization settings.
//You can find "OV7670 Implementation " document as pdf by OmniVision Company

const uint8_t OV7670_reg[][2] = {
 {REG_COM7, 0x80},
 {REG_CLKRC, 0x91},
 {REG_COM11, 0x0A},
 {REG_TSLB, 0x04},
 {REG_TSLB, 0x04},
 {REG_COM7, 0x04},

 {REG_RGB444, 0x00},
 {REG_COM15, 0xD0},

 {REG_HSTART, 0x16},
 {REG_HSTOP, 0x04},
 {REG_HREF, 0x24},
 {REG_VSTART, 0x02},
 {REG_VSTOP, 0x7a},
 {REG_VREF, 0x0a},
 {REG_COM10, 0x02},
 {REG_COM3, 0x04},
 {REG_MVFP, 0x3f},
 // 3 consecutive lines of code are QQVGA format settings.
 {REG_COM14, 0x1a},
 {0x72, 0x22},
 {0x73, 0xf2},

 {0x4f, 0x80},
 {0x50, 0x80},
 {0x51, 0x00},
 {0x52, 0x22},
 {0x53, 0x5e},
 {0x54, 0x80},
 {0x56, 0x40},
 {0x58, 0x9e},
 {0x59, 0x88},
 {0x5a, 0x88},
 {0x5b, 0x44},
 {0x5c, 0x67},
 {0x5d, 0x49},
 {0x5e, 0x0e},
 {0x69, 0x00},
 {0x6a, 0x40},
 {0x6b, 0x0a},
 {0x6c, 0x0a},
 {0x6d, 0x55},
 {0x6e, 0x11},
 {0x6f, 0x9f},

 {0xb0, 0x84},

 {0xFF, 0xFF},

};

const uint8_t needForHalfMicroDelay =0;
const uint16_t hrefPeriod= 9406; //Micro Second
const uint16_t pclkHighPixSynTime = 6; //Micro Second
const uint16_t pclkLowPixSynTime = 4;  //Micro Second
const uint16_t vsyncLowTime = 7052; //Micro Second

uint8_t horizArray[320];

uint8_t vsync =0;



char ov7670_init(void)
{
	  ResetRegisterForOvCam();
	  HAL_Delay(30);

	  uint8_t buffer[4];
	ReadOperationOVCam(REG_VER, buffer);
	  if ( buffer[0] != 0x73)
	  {
		  return 0;
	  }
	  else
	  {
		  ov7670_config();
	  }

	  return 1;
}

char ov7670_config()
{
	ResetRegisterForOvCam();
    HAL_Delay(30);

  for(int i = 0; OV7670_reg[i][0]!=0xFF; i++) {
    WriteOperationOVCam(OV7670_reg[i][0], OV7670_reg[i][1]);
    HAL_Delay(1);
  }
  return 0;
}

void GetFramesFromOvCam()
{
	/*
	 * PB0 = D0	    PB1 = D1		PB2 = D2
	 * PB3 = D3	    PB4 = D4		PB5 = D5
	 * PB6 = D6	    PB7 = D7
	 *
	 */

	uint16_t microCounter =0;
	uint16_t microCountertwo =0;
	//This definition also needed for pixel sync or you can define
	//unused value instead this value...
	uint8_t currentRowHorizantalData =0;


	//RGB565 format is 16bit format and OV7670 is supported it.
	//In order to obtain 160 horizantel pixel for LCD you need to catch 320 pixel
	//(8 bit = 1 pixel for Camera)
	const uint16_t horizantelPixel = 320;
	const uint8_t verticalPixel = 120;


	if(needForHalfMicroDelay == 0)
	{


	if(vsync == 1)
	{
		//In order to catch the second Interrupt at the right time
		//I added this delay(It is measured with Logic Analyzer with cheap one :)
		delayUsec(vsyncLowTime);
		while(vsync == 2)
		{

			if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) )

			{
				for(uint16_t verticalCount =0; verticalCount < verticalPixel; verticalCount++)
				{

					__HAL_TIM_SET_COUNTER(&htim6,0);
					__HAL_TIM_SET_COUNTER(&htim7,0);

					for(uint16_t horizCount =0; horizCount < horizantelPixel ; horizCount++)
					{

						if(HAL_GPIO_ReadPin(GPIOA, pclk_Pin) == 0 )
						{
							/*
							 * This part when PCLK is
							 */
							currentRowHorizantalData = ((0x00FF) & (GPIOB->IDR));
							horizArray[horizCount] = currentRowHorizantalData;
							microCountertwo = pclkHighPixSynTime -  __HAL_TIM_GET_COUNTER(&htim7) ;
							delayUsecForTimSeven(microCountertwo);


						}else
						{
							currentRowHorizantalData = ((0x00FF) & (GPIOB->IDR));
							horizArray[horizCount] = currentRowHorizantalData;
							//This unused variable needed for very little pixel spikes
							//If you try delete this value, you can understand :)
							uint8_t forNanoSecDelay =0;
							microCountertwo = pclkLowPixSynTime -  __HAL_TIM_GET_COUNTER(&htim7) ;
							delayUsecForTimSeven(microCountertwo);

						}



						__HAL_TIM_SET_COUNTER(&htim7,0);


					}
					//Elapsed Time for Data sending to led is 1900 - 2800  microSecond
					sendImageBufferToLed(&hspi1, horizArray, verticalCount,horizantelPixel);
					microCounter = hrefPeriod -__HAL_TIM_GET_COUNTER(&htim6);

					//It need to wait to be HIGH of the HREF
					delayUsec(microCounter);



				}

			}
		}
	}

}
	//delayUsec(38155);
}
void ConfigurePWDNandRESETpins()
{
	//for pwdn configure as Reset state to PC3
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, RESET);

	//for RESET configure as Set state to PC2
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, SET);
}

void ResetRegisterForOvCam()
{
	uint8_t pData[] = {0x80};

	//You need to set of Bit[7] for 0x12 Register Address
	if( HAL_I2C_Mem_Write(&hi2c1, writeAddressSCCB, REG_COM7, I2C_MEMADD_SIZE_8BIT, pData, 1, 10) != HAL_OK)
	{

		return;
	}
}

void WriteOperationOVCam(uint16_t memADdress, uint8_t  pData)
{
	if( HAL_I2C_Mem_Write(&hi2c1, writeAddressSCCB, memADdress, I2C_MEMADD_SIZE_8BIT, &pData, 1, 10) != HAL_OK)
	{

		return;
	}
}

void ReadOperationOVCam(uint16_t memAddress, uint8_t* buffer)
{

	if(HAL_I2C_Master_Transmit(&hi2c1, writeAddressSCCB, (uint8_t*)&memAddress, 1, 10) != HAL_OK)
	{
		return ;
	}
   if(HAL_I2C_Master_Receive(&hi2c1, readAddressSCCB, buffer, 1, 10) != HAL_OK)
   {
	   return;
   }


}

//I defined timers separately for not changing the time sync
void delayUsec(uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim6,0);
	while( __HAL_TIM_GET_COUNTER(&htim6) < time);
}

void delayUsecForTimSeven(uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim7,0);
	while( __HAL_TIM_GET_COUNTER(&htim7) < time);
}
