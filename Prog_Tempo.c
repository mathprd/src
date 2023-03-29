/*
 * Prog_Tempo.c
 *
 *  Created on: 25 avr. 2022
 *      Author: mahout
 */



#include <Service_GPIO.h>
#include <Service_I2C.h>
#include <Prog_Tempo.h>
#include <Ecran_Lancer_Prog.h>

char Num_Prog_Courant;
Data_Prog_Typedef  Data_Prog;

extern char Prog_En_Marche[8];
extern int Etat ;
extern int Mode_Manuel ;


/******************************************************************/
/*   Incrémente la n° courant de programme pour la page d'édition */
/******************************************************************/
void Increment_Numeprog(void)
{
	Num_Prog_Courant = Num_Prog_Courant + 1;
	if  (Num_Prog_Courant > NumProgMax)
		Num_Prog_Courant = 1;
}

/******************************************************************/
/*   Decrémente la n° courant de programme pour la page d'édition */
/******************************************************************/
void Decrement_Numeprog(void)
{
	Num_Prog_Courant = Num_Prog_Courant - 1;
	if  (Num_Prog_Courant < 1)
		Num_Prog_Courant = NumProgMax;

}


/******************************************************************/
/* Initialisation des données des programmes                      */
/* Lecture de la RAM dee la DS1307                                */
/* Ram[0] = Actif                                                 */

/* Ram[1] = Jour[1}                                                 */
/* Ram[2] = H_start[1]                                            */
/* Ram[3] = M_start[1]                                            */
/* Ram[4] = H_Duree[1]                                            */
/* Ram[5] = M_Duree[1]                                            */
/* Ram[6] = H_start[2]                                            */
/*    .....                                                       */
/* Ram[32] = H_start[8]                                           */
/******************************************************************/
void Lire_Data_Prog(Data_Prog_Typedef *Data)
{

	int indice ;

	Data->Prog_Actif =  DS1307_Get_MemRTC_Data(0);

	for  (indice = 0  ; indice < NumProgMax ; indice++)
	{
			Data->Jour[indice]    =  DS1307_Get_MemRTC_Data((indice)*5+1);
			Data->H_Start[indice] =  DS1307_Get_MemRTC_Data((indice)*5+2);
			Data->M_Start[indice] =  DS1307_Get_MemRTC_Data((indice)*5+3);
			Data->H_Duree[indice] =  DS1307_Get_MemRTC_Data((indice)*5+4);
			Data->M_Duree[indice] =  DS1307_Get_MemRTC_Data((indice)*5+5);

	}

}

/******************************************************************/
/* Sauvegarde des données des programmes                      */
/* en RAM de la DS1307                                */
/* Ram[0] = Actif                                                 */
/* Ram[1] = H_start[1]                                            */
/* Ram[2] = M_start[1]                                            */
/* Ram[3] = H_Duree[1]                                            */
/* Ram[4] = M_Duree[1]                                            */
/* Ram[5] = H_start[2]                                            */
/*    .....                                                       */
/* Ram[32] = H_start[8]                                           */
/******************************************************************/
void Stocke_Data_Prog(Data_Prog_Typedef *Data)
{

	int indice ;
	char Tempo;

	Tempo = 0;
	for  (indice = 0  ; indice < NumProgMax ; indice++)
	{
			DS1307_Set_MemRTC_Data((indice)*5+1,Data->Jour[indice]);
			DS1307_Set_MemRTC_Data((indice)*5+2,Data->H_Start[indice]);
			DS1307_Set_MemRTC_Data((indice)*5+3,Data->M_Start[indice]);
			DS1307_Set_MemRTC_Data((indice)*5+4,Data->H_Duree[indice]);
			DS1307_Set_MemRTC_Data((indice)*5+5,Data->M_Duree[indice]);

			if ((Data->H_Duree[indice] + Data->M_Duree[indice] != 0) && (Data->Jour[indice] != 0))
			{
				 Tempo = Tempo | (0x01 << indice);  /* Si le programme contient une valeur, */
			}
	}
	Data->Prog_Actif = Tempo ;
	DS1307_Set_MemRTC_Data(0,Data->Prog_Actif);
}

void Verif_Programme()
{
		DS1307_Time_Typedef date;
		char Marche ;

		DS1307_GetTime(&date);

		CheckStop(&Data_Prog);

		Marche = Is_Start(&Data_Prog,date);

		if (Marche == 1){
			Allume_Pompe();
		} else {
			Eteint_Pompe();
		}

}


void CheckStop(Data_Prog_Typedef  *Data)
{
	int i;
	int JStop,MStop,HStop,retH,retM;

	for (i=0; i< NumProgMax; i++)
	{
		retM=0;
		retH = 0;

		MStop = Data->M_Start[i] + Data->M_Duree[i];
		if (MStop >=60)
		{
			MStop = MStop - 60;
			retM = 1;
		}
		HStop = Data->H_Start[i]+ Data->H_Duree[i]+retM;
		if (HStop >=24)
		{
			HStop = HStop - 24;
			retH = 1;
		}
		JStop = (Data->Jour[i] << retH) | (Data->Jour[i] >> (7-retH)) ;
		JStop &= ~(0x80) ;  //efface le bit numéro 7

		Data->Jour_Stop[i] = (char)(JStop);
		Data->H_Stop[i] = (char)(HStop);
		Data->M_Stop[i] = (char)(MStop);
	}
}


char  Is_Start(Data_Prog_Typedef  *Data, DS1307_Time_Typedef Top)
{
	int i;
	char Egal ;

	Egal = 0;

	for (i = 0 ; i < NumProgMax ; i ++)
	{
		if (Prog_En_Marche[i] == 1){
			if ((Data->Jour[i] == Data->Jour_Stop[i]) &&  ((Data->Jour[i] & Top.Day) != 0)) {  //même jour de fin et de début a mettre et jour actuel
				if ((Data->H_Start[i] == Data->H_Stop[i]) && (Data->H_Start[i] == Top.Hour)){ 	//même heure de début et de fin et heure actuelle
					if ((Data->M_Start[i] <= Top.Min) && (Data->M_Stop[i] > Top.Min)) {
						Egal = 1 ;
					}
				} else if (Data->H_Start[i] == Top.Hour){
					if (Data->M_Start[i] <= Top.Min){
						Egal = 1 ;
					}
				} else if (Data->H_Stop[i] == Top.Hour){
					if (Data->M_Stop[i] > Top.Min){
						Egal = 1 ;
					}
				} else if ((Data->H_Start[i] < Top.Hour) && (Data->H_Stop[i] > Top.Hour)){
					Egal = 1 ;
				}
			} else if ((Data->Jour[i] & Top.Day) != 0){   //Jour actuel = Jour de debut
				if ((Data->H_Start[i] = Top.Hour) && (Data->M_Start[i] <= Top.Min)){
					Egal = 1 ;
				} else if (Data->H_Start[i] < Top.Hour){
					Egal = 1 ;
				}
			} else if ((Data->Jour_Stop[i] & Top.Day) != 0){   //Jour actuel = Jour de fin
				if ((Data->H_Stop[i] = Top.Hour) && (Data->M_Stop[i] > Top.Min)){
					Egal = 1 ;
				} else if (Data->H_Stop[i] > Top.Hour){
					Egal = 1 ;
				}
			}
		}
	}
	return (Egal);
}


void Gestion_Priorites(void){

	if (Mode_Manuel != 1){
		if (Etat != 30){
			Verif_Programme();
		}
	}

}






