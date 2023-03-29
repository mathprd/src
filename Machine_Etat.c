/*
 * Machine_Etat.c
 *
 *  Created on: 18 juil. 2021
 *      Author: mahout
 */
#include <Machine_Etat.h>
#include <Ecran_Accueil.h>
#include <Ecran_Marche.h>
#include <Ecran_Marche.h>
#include <Ecran_Regle_Prog.h>
#include <Ecran_Lancer_Prog.h>
#include <Ecran_Regle_Heure.h>
#include <Service_Timer.h>
#include <Service_I2C.h>
#include <Service_GPIO.h>

#include "stm32746g_discovery_ts.h"
#include "hal_stm_lvgl/tft/tft.h"
#include "hal_stm_lvgl/touchpad/touchpad.h"
#include "stm32f7xx.h"
#include "lvgl/lvgl.h"
#include <Projet.h>

int Etat;

extern void LCD_DisplayOn(void);
extern void LCD_DisplayOff(void);

char Change;
char Transi_0to1;
char Transi_0to3;
char Transi_1to0;
char Transi_2to0;
char Transi_0to2;
char Transi_3to0 ;
char Transi_3to3 ;
char Transi_30to0,Transi_0to30,Transi_30to30;

extern char Prog_Selected ;
extern char Prog_En_Marche[8] ;

extern lv_obj_t * spinner ;
extern lv_obj_t * Texte_Marche;
extern int A_Effacer ;
extern int TempoMini ;
extern int Mode_Manuel ;


char Poussoir_Start_Appui;
char Fin_Tempo;
char Minute60Sec ;
int Compteur_Marche_Pompe = 20 ;

TS_StateTypeDef  TS_State;

extern DS1307_Time_Typedef UserTime;
extern char Num_Prog_Courant;


char  Change_Etat(void)
{
	Change = 0 ;

	if(Etat == 0)
	{
		if(Transi_0to1 == 1)
		{
			Etat = 1;
			Change = 1;
			Transi_0to1 = 0;
			Stop_Tempo();
		}
		if (Fin_Tempo == 1)
		{
			Etat = -1;
			Change = 1;
			Fin_Tempo = 0;
		}
		if (Transi_0to2 == 1 || Poussoir_Start_Appui == 1 )
		{
			Etat = 2;
			Change = 1;
			Transi_0to2 = 0;
			Poussoir_Start_Appui = 0;
			Stop_Tempo();
		}
		if (Transi_0to30 == 1 )
		{
			Etat = 30;
			Change = 1;
			Transi_0to30 = 0;
			Stop_Tempo();
		}
		if (Transi_0to3 ==1){
			Etat = 3;
			Change = 1;
			Transi_0to3 = 0 ;
			Stop_Tempo();
		}
	}

	if(Etat == 1 && Transi_1to0 == 1)
	{
		Etat = 0;
		Change = 1;
		Transi_1to0 = 0;
	}

	if(Etat == 3)
	{
		if (Transi_3to0 == 1){
			Etat = 0;
			Change = 1;
			Transi_3to0 = 0;
		}
		if (Transi_3to3 == 1){
			Etat = 3;
			Change = 1;
			Transi_3to3 = 0;
		}
	}

	if(Etat == 30)
	{
		if (Transi_30to30 == 1){
			Etat = 30;
			Change = 1;
			Transi_30to30 = 0;
		}
		if (Transi_30to0 == 1){
			Etat = 0;
			Change = 1;
			Transi_30to0 = 0;
		}
	}

	if(Etat == -1 )
	{
		BSP_TS_GetState(&TS_State);
		if(TS_State.touchDetected || Poussoir_Start_Appui==1)
		{
			LCD_DisplayOn();
			Etat = 0;
			Change = 1;
			Poussoir_Start_Appui=0;
		}
	}
	if(Etat == 2)
	{
		if (Transi_2to0 == 1 || Poussoir_Start_Appui==1)
		{
			Etat = 0;
			Change = 1;
			Transi_2to0 = 0;
			Poussoir_Start_Appui=0;
		}
		if (Compteur_Marche_Pompe < 0){
			lv_obj_del(Texte_Marche) ;
			lv_obj_del(spinner) ;
			A_Effacer = 0 ;
			Eteint_Pompe();
			Stop_Pompe_1sec();;
			Compteur_Marche_Pompe = TempoMini ;
			Mode_Manuel = 0 ;
			Minute60Sec = 0 ;
		}
	}

	return (Change);

}

void Modifie_Etat(void)
{

	if ( Etat == 0 )
	{

		Creer_Ecran_Acceuil();
		Run_Tempo();
		Num_Prog_Courant = 0;
		Prog_Selected = 0;

	}

	if (Etat == 1)
	{
		Creer_Ecran_Regle_Heure();
	}
	if (Etat == -1)
	{
		LCD_DisplayOff();
	}
	if (Etat == 2)
	{
		Transi_0to2 = 0;
		Creer_Ecran_Marche();

		Refresh_Slider(Compteur_Marche_Pompe);

	}

	if (Etat == 30)
	{
		Transi_30to30 = 0;
		Creer_Ecran_Regle_Prog();
	}
	if (Etat ==3){
		Creer_Ecran_Lancer_Prog() ;
	}

}

void Refresh_Etat(void)
{
	if ( Etat == 0 )
	{
		DS1307_GetTime(&UserTime);
		Change_Heure(UserTime.Hour,UserTime.Min,UserTime.Sec);
		Change_Date(UserTime.Date,UserTime.Month,UserTime.Year);
		lv_task_handler();
	}
	if ( Etat == 2 )
	{
		Refresh_Slider(Compteur_Marche_Pompe);

	}
}



