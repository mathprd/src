/*
 * Pilote_I2C.c
 *
 *  Created on: Jun 25, 2021
 *      Author: mahout
 */

#include <Service_I2C.h>
#include "stm32f7xx_hal.h"
#include <time.h>


I2C_HandleTypeDef hi2c1;
DS1307_Time_Typedef UserTimeSet;
DS1307_Time_Typedef UserTime;

unsigned char DS1307_TimeString[9]; // réservation premier byte pteur @
unsigned char DS1307_DataString[2]; // réservation premier byte pteur @




void MX_I2C1_Init(void)
{

	/* Enable I2Cx clock */
	__HAL_RCC_I2C1_CLK_ENABLE();

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x00A0A3F7;
	hi2c1.Init.OwnAddress1 = 0xFF;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure Analogue filter
	 */
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure Digital filter
	 */
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */



void DS1307_SetTime(DS1307_Time_Typedef * UserTime)
{
	/* Appel à mktime pour récupérer le jour de la semaine */
	struct tm myDate;
	myDate.tm_mday = UserTime->Date;
    myDate.tm_mon = UserTime->Month-1;
    myDate.tm_year = 2000+UserTime->Year-1900;   // Date == April 6, 2014
    myDate.tm_hour = 1;
    myDate.tm_min = 1;
    myDate.tm_sec = 0;     // Time == 12:30:15

    mktime(&myDate);

	// ptr adress à 0
	DS1307_TimeString[RTC_PtrAdr]=0;
	// Mise en forme des données BCD, voir documentation
	DS1307_TimeString[RTC_sec]=Conv_BCD(UserTime->Sec);
	DS1307_TimeString[RTC_minu]=Conv_BCD(UserTime->Min);
	DS1307_TimeString[RTC_day]= myDate.tm_wday;
	DS1307_TimeString[RTC_date]=Conv_BCD(UserTime->Date);
	DS1307_TimeString[RTC_month]=Conv_BCD(UserTime->Month);
	DS1307_TimeString[RTC_year]=Conv_BCD(UserTime->Year);


	// Insertion du bit de contrôle 12h/24h ds le chp Hour (bit6)
	if (UserTime->H_12_Not_24==0) //0-23h59
	{
		DS1307_TimeString[RTC_hour]=Conv_BCD(UserTime->Hour);
		(DS1307_TimeString[RTC_hour])&=~(1<<6);
	}
	else
	{
		DS1307_TimeString[RTC_hour]=Conv_BCD(UserTime->Hour);
		if (UserTime->PM_Not_AM==0) (DS1307_TimeString[RTC_hour])&=~(1<<5); // matin
		else (DS1307_TimeString[RTC_hour])|=(1<<5); // aprem
		DS1307_TimeString[RTC_hour]|=(1<<6);
	}

	// écriture du reg de contrôle, OUT=0 (b7), SQWE=0  (b4)
	//(pas de sortie d'horloge)
	(DS1307_TimeString[RTC_control])&=~(1<<4);
	(DS1307_TimeString[RTC_control])&=~(1<<7);


	// émission effective

	HAL_I2C_Master_Transmit(&hi2c1,DS1307_Slave7bitsAdr, DS1307_TimeString, 9, 1000000);

}

char DS1307_Get_MemRTC_Data(char Posi)
{
	char data;

    Posi = Posi+8;

	HAL_I2C_Master_Transmit(&hi2c1,DS1307_Slave7bitsAdr, &Posi, 1, 10000);

	HAL_I2C_Master_Receive(&hi2c1, DS1307_Slave7bitsAdr, &data, 1, 10000);

	return (data);

}






void DS1307_Set_MemRTC_Data(char Posi,char Data)
{
	DS1307_DataString[0]=Posi+8;
	DS1307_DataString[1]= Data;
	HAL_I2C_Master_Transmit(&hi2c1,DS1307_Slave7bitsAdr, DS1307_DataString, 2, 1000000);

}


char Conv_BCD(char Nbre)
{
	char u,d;
	d=Nbre/10;
	u=Nbre - 10*d;
	return ((d<<4)|u);
}




void DS1307_GetTime(DS1307_Time_Typedef * UserTime)
{
	unsigned char RecTab[7];
	unsigned char vide=0;



	HAL_I2C_Master_Transmit(&hi2c1,DS1307_Slave7bitsAdr, &vide, 1, 10000);


	HAL_I2C_Master_Receive(&hi2c1, DS1307_Slave7bitsAdr, RecTab, 7, 10000);


	// remplissage UserTime (supposition format 24h)
	UserTime->Sec=BCD_To_Dec(RecTab[0]);
	UserTime->Min=BCD_To_Dec(RecTab[1]);
	UserTime->Hour=BCD_To_Dec(RecTab[2]&0x3F);
	UserTime->Day=(RecTab[3])&0x07;
	UserTime->Date=BCD_To_Dec(RecTab[4]);
	UserTime->Month=BCD_To_Dec(RecTab[5]);
	UserTime->Year=BCD_To_Dec(RecTab[6]);
}







