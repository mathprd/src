/*
 * Ecran_Lancer_Prog.c
 *
 *  Created on: 23 mars 2023
 *      Author: mathi
 */

#include "lvgl/lvgl.h"
#include <Ecran_Lancer_Prog.h>
#include <stdlib.h>
#include <Service_I2C.h>
#include <Machine_Etat.h>

extern char Transi_0to3;
extern char Transi_3to0;


void Creer_Ecran_Lancer_Prog(void) {
	lv_obj_clean(lv_scr_act());
}














