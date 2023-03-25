#include <Ecran_Regle_Heure.h>
#include <stdlib.h>
#include <Service_I2C.h>
#include <Machine_Etat.h>


#include "lvgl/lvgl.h"

extern unsigned int Etat;
extern DS1307_Time_Typedef UserTimeSet;
extern DS1307_Time_Typedef UserTime;


unsigned char Heure;
unsigned char Minute;
unsigned char Jour;
unsigned char Mois;
unsigned char Annee;

extern char Transi_1to0;
extern char Transi_2to0;


lv_obj_t  * calendar;
lv_calendar_date_t highlighted_days[1];

void Creer_Ecran_Regle_Heure(void)
{
	lv_obj_clean(lv_scr_act());
	Regle_Heure();
	Bouton_Retour_Regle_Heure();
	Bouton_Appliquer();
	lv_task_handler();
}



static void Roller_Heure_event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target(e);
	if(code == LV_EVENT_VALUE_CHANGED) {
		char buf[32];
		lv_roller_get_selected_str(obj, buf, sizeof(buf));
		Heure = (unsigned char) (atoi(buf));
	}
}

static void Roller_Minute_event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target(e);
	if(code == LV_EVENT_VALUE_CHANGED) {
		char buf[32];
		lv_roller_get_selected_str(obj, buf, sizeof(buf));
		Minute = (unsigned char) (atoi(buf));
	}
}

void Regle_Heure(void)
{
	int Boucle;

	char Chaine_Opt[200],Txt[5];

	/*A style to make the selected option larger*/
	static lv_style_t style_sel;


	lv_style_init(&style_sel);
	lv_style_set_text_font(&style_sel, &lv_font_montserrat_22);
	lv_style_set_text_color(&style_sel, lv_color_make(100,5,2) );

	DS1307_GetTime(&UserTime);
	Heure = UserTime.Hour;
	Minute = UserTime.Min;
	Mois = UserTime.Month;
	Jour = UserTime.Date;
	Annee = UserTime.Year;



	lv_obj_t *Roller_H;
	lv_obj_t *Roller_M;

	Chaine_Opt[0] = 0;
	for (Boucle= 0; Boucle <23; Boucle++)
	{
		sprintf(Txt,"%d\n",Boucle);
		strcat(Chaine_Opt,Txt);
	}
	strcat(Chaine_Opt,"23");

	/*A roller on the middle with center aligned text, and auto (default) width*/
	Roller_H = lv_roller_create(lv_scr_act());
	lv_roller_set_options(Roller_H, Chaine_Opt, LV_ROLLER_MODE_INFINITE);
	lv_roller_set_visible_row_count(Roller_H, 4);
	lv_obj_add_style(Roller_H, &style_sel, LV_PART_SELECTED);

	lv_obj_set_size(Roller_H, 50, 100);
	lv_obj_align(Roller_H, LV_ALIGN_CENTER, 110, -30);
	lv_obj_add_event_cb(Roller_H, Roller_Heure_event_handler, LV_EVENT_ALL, NULL);
	lv_roller_set_selected(Roller_H, Heure, LV_ANIM_ON);

	lv_obj_t * Label = lv_label_create(lv_scr_act());
	lv_label_set_text(Label, "Heure ");
	lv_obj_align(Label, LV_ALIGN_CENTER, 110, -100);


	Chaine_Opt[0] = 0;
	for (Boucle = 0; Boucle < 59; Boucle++)
	{
		sprintf(Txt,"%d\n",Boucle);
		strcat(Chaine_Opt,Txt);
	}
	strcat(Chaine_Opt,"59");

	/*A roller on the middle with center aligned text, and auto (default) width*/
	Roller_M = lv_roller_create(lv_scr_act());
	lv_roller_set_options(Roller_M, Chaine_Opt, LV_ROLLER_MODE_INFINITE);
	lv_roller_set_visible_row_count(Roller_M, 3);
	lv_obj_add_style(Roller_M, &style_sel, LV_PART_SELECTED);

	lv_obj_set_size(Roller_M, 50, 100);
	lv_obj_align(Roller_M, LV_ALIGN_CENTER, 170, -30);
	lv_obj_add_event_cb(Roller_M, Roller_Minute_event_handler, LV_EVENT_ALL, NULL);
	lv_roller_set_selected(Roller_M, Minute, LV_ANIM_ON);

	Label = lv_label_create(lv_scr_act());
	lv_label_set_text(Label, "Minute ");
	lv_obj_align(Label, LV_ALIGN_CENTER, 170, -100);

	Calendrier();

}

void Bouton_Retour_Regle_Heure(void)
{
	/*Init the style for the default state*/
	static lv_style_t style;
	static lv_style_t style_pr;
	static lv_style_transition_dsc_t trans;
	static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};


	lv_style_init(&style);

	lv_style_set_radius(&style, 3);

	lv_style_set_bg_opa(&style, LV_OPA_100);
	lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE));
	lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 2));
	lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

	lv_style_set_border_opa(&style, LV_OPA_40);
	lv_style_set_border_width(&style, 2);
	lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));

	lv_style_set_shadow_width(&style, 8);
	lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
	lv_style_set_shadow_ofs_y(&style, 8);

	lv_style_set_outline_opa(&style, LV_OPA_COVER);
	lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));

	lv_style_set_text_color(&style, lv_color_white());
	lv_style_set_pad_all(&style, 10);

	/*Init the pressed style*/
	lv_style_init(&style_pr);

	/*Ad a large outline when pressed*/
	lv_style_set_outline_width(&style_pr, 25);
	lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

	lv_style_set_translate_y(&style_pr, 5);
	lv_style_set_shadow_ofs_y(&style_pr, 3);
	lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 2));
	lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 4));
	lv_style_set_text_font(&style, &lv_font_montserrat_12);

	/*Add a transition to the the outline*/

	lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

	lv_style_set_transition(&style_pr, &trans);

	lv_obj_t * Bouton_Retour_Regle_Heure = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Retour_Regle_Heure);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Retour_Regle_Heure, 40, 50);
	lv_obj_add_style(Bouton_Retour_Regle_Heure, &style, 0);

	lv_obj_add_style(Bouton_Retour_Regle_Heure, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Retour_Regle_Heure, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Retour_Regle_Heure, LV_ALIGN_CENTER, 140, 100);

	lv_obj_add_event_cb(Bouton_Retour_Regle_Heure, event_handler_BoutonRetour_Regle_Heure, LV_EVENT_ALL, NULL);

	lv_obj_t * label = lv_label_create(Bouton_Retour_Regle_Heure);
	lv_label_set_text(label, "Retour");
	lv_obj_center(label);

}


void event_handler_BoutonRetour_Regle_Heure(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_PRESSED) {
		Transi_1to0 = 1;
	}
}


static void event_handler_Calendrier(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target(e);

	if(code == LV_EVENT_VALUE_CHANGED) {
		lv_calendar_date_t date;
		if(lv_calendar_get_pressed_date(obj, &date)) {
			Jour = date.day;
			Mois = date.month;
			Annee = date.year-2000;

			lv_calendar_set_today_date(calendar, date.year, date.month, date.day);


		}
	}
}

void Calendrier()
{
	const char * days[7] = {"Di","Lu", "Ma", "Me","Je","Ve","Sa"};
	uint16_t Y2000;


	Y2000 = (uint16_t)(Annee) + 2000;

	calendar = lv_calendar_create(lv_scr_act());
	lv_obj_align(calendar, LV_ALIGN_CENTER, -70,20);
	lv_obj_add_event_cb(calendar, event_handler_Calendrier, LV_EVENT_ALL, NULL);

	lv_calendar_set_today_date(calendar,Y2000, (int8_t) Mois, (int8_t) Jour);
	lv_calendar_set_showed_date(calendar,Y2000, (int8_t) Mois);
	lv_calendar_set_day_names(calendar, days);


	//lv_calendar_header_dropdown_create(lv_scr_act(), calendar);
	lv_calendar_header_arrow_create(lv_scr_act(), calendar,25);
	lv_obj_set_size(calendar, 250,200);
}

void event_handler_Bouton_Appliquer(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_1to0 = 1;
		UserTimeSet.Min=Minute;
		UserTimeSet.Hour=Heure;
		UserTimeSet.Year=Annee;
		UserTimeSet.Date=Jour;
		UserTimeSet.Month=Mois;
		DS1307_SetTime(&UserTimeSet);
	}
}

void Bouton_Appliquer(void) {
	/*Init the style for the default state*/
	static lv_style_t style;
	lv_style_init(&style);

	lv_style_set_radius(&style, 3);

	lv_style_set_bg_opa(&style, LV_OPA_100);
	lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_ORANGE));
	lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_DEEP_ORANGE, 2));
	lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

	lv_style_set_border_opa(&style, LV_OPA_40);
	lv_style_set_border_width(&style, 2);
	lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));

	lv_style_set_shadow_width(&style, 8);
	lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
	lv_style_set_shadow_ofs_y(&style, 8);

	lv_style_set_outline_opa(&style, LV_OPA_COVER);
	lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));

	lv_style_set_text_color(&style, lv_color_white());
	lv_style_set_pad_all(&style, 10);

	/*Init the pressed style*/
	static lv_style_t style_pr;
	lv_style_init(&style_pr);

	/*Ad a large outline when pressed*/
	lv_style_set_outline_width(&style_pr, 25);
	lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

	lv_style_set_translate_y(&style_pr, 5);
	lv_style_set_shadow_ofs_y(&style_pr, 3);
	lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 2));
	lv_style_set_bg_grad_color(&style_pr,
			lv_palette_darken(LV_PALETTE_BLUE, 4));
	lv_style_set_text_font(&style, &lv_font_montserrat_12);

	/*Add a transition to the the outline*/
	static lv_style_transition_dsc_t trans;
	static lv_style_prop_t props[] = { LV_STYLE_OUTLINE_WIDTH,
			LV_STYLE_OUTLINE_OPA, 0 };
	lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0,
	NULL);

	lv_style_set_transition(&style_pr, &trans);

	lv_obj_t *Bouton_Appliquer = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Appliquer);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Appliquer, 50, 50);
	lv_obj_add_style(Bouton_Appliquer, &style, 0);

	lv_obj_add_style(Bouton_Appliquer, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Appliquer, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Appliquer, LV_ALIGN_CENTER, 140, 50);

	lv_obj_add_event_cb(Bouton_Appliquer, event_handler_Bouton_Appliquer,
			LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Appliquer);
	lv_label_set_text(label, "Appliquer");
	lv_obj_center(label);

}


