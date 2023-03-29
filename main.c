
#include <Ecran_Accueil.h>
#include <Ecran_Regle_Heure.h>
#include <Service_I2C.h>
#include <Service_GPIO.h>
#include <Service_Timer.h>
#include <Machine_Etat.h>
#include "stm32f7xx.h"
#include "lvgl/lvgl.h"
#include <Projet.h>
#include <Prog_Tempo.h>

#include "hal_stm_lvgl/tft/tft.h"
//#include "lvgl/src/hal/lv_hal.h"

//#include "stm32746g_discovery.h"
#include "stm32746g_discovery_ts.h"


#include "hal_stm_lvgl/tft/tft.h"
#include "hal_stm_lvgl/touchpad/touchpad.h"

extern char Transi_2to0;
extern int Etat;
extern Data_Prog_Typedef Data_Prog;


void Error_Handler(void);


static void SystemClock_Config(void);
extern DS1307_Time_Typedef UserTime;

GPIO_InitTypeDef MonGPIO_Init;
char state;


int main(void)
{

	HAL_Init();

	/* Configure the system clock to 216 MHz */
	SystemClock_Config();

	/* Enable I-Cache */
	SCB_EnableICache();

	/* Enable D-Cache */
	SCB_EnableDCache();
	Init_GPIO();

	MX_I2C1_Init();

	lv_init();
	tft_init();
	touchpad_init();

	/* on relit les donnes de programmation
	 * en méméoire de la RTC DS1307 */
	Lire_Data_Prog(&Data_Prog);

	Init_TIM2_Tempo();
	Stop_Tempo();

	Init_TIM3_Pompe_1sec();

	Transi_2to0 = 1;
    Etat = 2;
    // LCD_DisplayOff();


	while (1)
	{



		HAL_Delay(100);
        if (Change_Etat() == 1)
        	Modifie_Etat();
        else
        {
        	Refresh_Etat();
        	Gestion_Priorites();
        }
	}

}


static void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 10;
	RCC_OscInitStruct.PLL.PLLN = 210;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
}
/*------------------------------------ RTC configuration --------------------------------------*/
void RTC_Clock(void)

{

	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

}


void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

/*------------------------------------ TIM configuration --------------------------------------*/

