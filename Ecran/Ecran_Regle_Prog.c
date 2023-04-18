/*
 * Ecran_Prog.c
 *
 *  Created on: 23 avr. 2022
 *      Author: mahout
 */
#include "lvgl/lvgl.h"
#include <Ecran_Regle_Prog.h>
#include <stdlib.h>
#include <Service_I2C.h>
#include <Machine_Etat.h>
#include <Prog_Tempo.h>
#include "lvgl/lvgl.h"

extern unsigned int Etat;
extern DS1307_Time_Typedef UserTimeSet;
extern DS1307_Time_Typedef UserTime;
extern char Transi_30to0, Transi_0to30, Transi_30to30;
extern char Num_Prog_Courant;
extern Data_Prog_Typedef Data_Prog;

lv_obj_t *LabelTitre;
lv_obj_t *Roller_H_Duree;
lv_obj_t *Roller_M_Duree;
lv_obj_t *Roller_H_Start;
lv_obj_t *Roller_M_Start;
lv_obj_t *BoxJour[7];


unsigned char Duree_Heure;
unsigned char Duree_Minute;
unsigned char Depart_Heure;
unsigned char Depart_Minute;

static void Roller_Depart_Heure_event_handler(lv_event_t *e);
static void Roller_Depart_Minute_event_handler(lv_event_t *e);
static void Roller_Duree_Heure_event_handler(lv_event_t *e);
static void Roller_Duree_Minute_event_handler(lv_event_t *e);

void event_handler_BoutonRetour_Regle_Prog(lv_event_t *e);
void event_handler_BoutonProg_Suivant(lv_event_t *e);
void event_handler_BoutonProg_Precedent(lv_event_t *e);

void Creer_Ecran_Regle_Prog(void) {
	if (Num_Prog_Courant == 0) {
		lv_obj_clean(lv_scr_act());
		Label_Num(Num_Prog_Courant);
		Increment_Numeprog();
		Bouton_Retour_Regle_Prog();
		Bouton_Prog_Suivant();
		Bouton_Prog_Precedent();
		CheckBox_Jour();
		Roller_Depart();
		Roller_Duree();
		lv_task_handler();
	}
	Label_Num(Num_Prog_Courant);

}



void Label_Num(char Num) {
	char Titre[20];
	int Boucle;
	static lv_point_t line_points[] = { { 0, 40 }, { LV_SIZE_CONTENT, 40 } };
	static lv_style_t style_line;
	static lv_style_t style_Lab;

	if (Num == 0) /* On vient de rentrer dans la page avec un n° de prog nul */
	{
		/*Creation style ligne */
		lv_style_init(&style_line);
		lv_style_set_line_width(&style_line, 2);
		lv_style_set_line_color(&style_line, lv_color_make(190, 0, 25));
		lv_style_set_line_rounded(&style_line, true);

		/*Creation ligne de séparation */
		lv_obj_t *line1;
		line1 = lv_line_create(lv_scr_act());
		lv_line_set_points(line1, line_points, 2);
		lv_obj_add_style(line1, &style_line, 0);

		/*Creation du label vide */
		lv_style_init(&style_Lab);
		lv_style_set_text_font(&style_Lab, &lv_font_montserrat_22);
		lv_style_set_text_color(&style_Lab, lv_color_make(190, 0, 25));
		LabelTitre = lv_label_create(lv_scr_act());
		lv_obj_add_style(LabelTitre, &style_Lab, 0);
		lv_obj_align(LabelTitre, LV_ALIGN_CENTER, 0, -115);
		lv_label_set_text(LabelTitre, " ");
	} else /* Modification du label avec le n° du programme courant */
	{
		sprintf(Titre, " Programme n° %d", Num);
		lv_label_set_text(LabelTitre, Titre);
		lv_roller_set_selected(Roller_H_Duree, Data_Prog.H_Duree[Num-1], LV_ANIM_ON);
		lv_roller_set_selected(Roller_M_Duree, Data_Prog.M_Duree[Num-1], LV_ANIM_ON);
		lv_roller_set_selected(Roller_H_Start, Data_Prog.H_Start[Num-1], LV_ANIM_ON);
		lv_roller_set_selected(Roller_M_Start, Data_Prog.M_Start[Num-1], LV_ANIM_ON);

		for (Boucle = 0; Boucle < NumProgMax-1 ; Boucle++ )
		{
			if ((Data_Prog.Jour[Num-1] & (0x01 << Boucle)) == (0x01 << Boucle))
				lv_obj_add_state(BoxJour[Boucle], LV_STATE_CHECKED);
			else
				lv_obj_clear_state(BoxJour[Boucle], LV_STATE_CHECKED) ;
		}
	}
}

void Roller_Depart(void) {

	int Boucle;

	char Chaine_Opt[200], Txt[5];

	static lv_style_t style;
	static lv_style_t style_sel;

	Chaine_Opt[0] = 0;
	for (Boucle = 0; Boucle < 23; Boucle++) {
		sprintf(Txt, "%d\n", Boucle);
		strcat(Chaine_Opt, Txt);
	}
	Boucle = 23;
	sprintf(Txt, "%d", Boucle);
	strcat(Chaine_Opt, Txt);

	/*A roller on the middle with center aligned text, and auto (default) width*/
	Roller_H_Start = lv_roller_create(lv_scr_act());
	lv_roller_set_options(Roller_H_Start, Chaine_Opt, LV_ROLLER_MODE_INFINITE);
	lv_roller_set_visible_row_count(Roller_H_Start, 4);
	lv_obj_add_style(Roller_H_Start, &style_sel, LV_PART_SELECTED);

	lv_obj_set_size(Roller_H_Start, 50, 100);
	lv_obj_align(Roller_H_Start, LV_ALIGN_CENTER, -20, 0);
	lv_obj_add_event_cb(Roller_H_Start, Roller_Depart_Heure_event_handler, LV_EVENT_ALL, NULL);

	lv_obj_t *Label2 = lv_label_create(lv_scr_act());

	lv_label_set_text(Label2, "Heure  Minute ");
	lv_obj_align(Label2, LV_ALIGN_CENTER, 15, -60);

	lv_obj_t *Label = lv_label_create(lv_scr_act());

	lv_label_set_text(Label, "Depart ");
	lv_obj_align(Label, LV_ALIGN_CENTER, 15, -80);
	lv_style_set_text_font(&style, &lv_font_montserrat_18);
	lv_style_set_text_color(&style, lv_color_make(125, 25, 12));
	lv_obj_add_style(Label, &style, 0);

	Chaine_Opt[0] = 0;
	for (Boucle = 0; Boucle < 59; Boucle++) {
		sprintf(Txt, "%d\n", Boucle);
		strcat(Chaine_Opt, Txt);
	}
	Boucle = 59;
	sprintf(Txt, "%d", Boucle);
	strcat(Chaine_Opt, Txt);
	/*A roller on the middle with center aligned text, and auto (default) width*/
	Roller_M_Start = lv_roller_create(lv_scr_act());
	lv_roller_set_options(Roller_M_Start, Chaine_Opt, LV_ROLLER_MODE_INFINITE);
	lv_roller_set_visible_row_count(Roller_M_Start, 3);
	lv_obj_add_style(Roller_M_Start, &style_sel, LV_PART_SELECTED);

	lv_obj_set_size(Roller_M_Start, 50, 100);
	lv_obj_align(Roller_M_Start, LV_ALIGN_CENTER, 30, 0);
	lv_obj_add_event_cb(Roller_M_Start, Roller_Depart_Minute_event_handler,LV_EVENT_ALL, NULL);
}

static void Roller_Depart_Heure_event_handler(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);
	if (code == LV_EVENT_VALUE_CHANGED) {
		char buf[32];
		lv_roller_get_selected_str(obj, buf, sizeof(buf));
		Depart_Heure = (unsigned char) (atoi(buf));
		Data_Prog.H_Start[Num_Prog_Courant - 1] = Depart_Heure;
	}
}

static void Roller_Depart_Minute_event_handler(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);
	if (code == LV_EVENT_VALUE_CHANGED) {
		char buf[32];
		lv_roller_get_selected_str(obj, buf, sizeof(buf));
		Depart_Minute = (unsigned char) (atoi(buf));
		Data_Prog.M_Start[Num_Prog_Courant - 1] = Depart_Minute;
	}
}

void Roller_Duree(void) {

	int Boucle;

	char Chaine_Opt[200], Txt[5];

	static lv_style_t style;
	static lv_style_t style_sel;

	Chaine_Opt[0] = 0;
	for (Boucle = 0; Boucle < 5; Boucle++) {
		sprintf(Txt, "%d\n", Boucle);
		strcat(Chaine_Opt, Txt);
	}
	Boucle = 3;
	sprintf(Txt, "%d", Boucle);
	strcat(Chaine_Opt, Txt);

	/*A roller on the middle with center aligned text, and auto (default) width*/
	Roller_H_Duree = lv_roller_create(lv_scr_act());
	lv_roller_set_options(Roller_H_Duree, Chaine_Opt, LV_ROLLER_MODE_INFINITE);
	lv_roller_set_visible_row_count(Roller_H_Duree, 4);
	lv_obj_add_style(Roller_H_Duree, &style_sel, LV_PART_SELECTED);

	lv_obj_set_size(Roller_H_Duree, 50, 100);
	lv_obj_align(Roller_H_Duree, LV_ALIGN_CENTER, 110, 0);
	lv_obj_add_event_cb(Roller_H_Duree, Roller_Duree_Heure_event_handler,LV_EVENT_ALL, NULL);

	lv_obj_t *Label2 = lv_label_create(lv_scr_act());

	lv_label_set_text(Label2, "Heures  Minutes ");
	lv_obj_align(Label2, LV_ALIGN_CENTER, 137, -60);

	lv_obj_t *Label = lv_label_create(lv_scr_act());

	lv_label_set_text(Label, "Duree ");
	lv_obj_align(Label, LV_ALIGN_CENTER, 137, -80);
	lv_style_set_text_font(&style, &lv_font_montserrat_18);
	lv_style_set_text_color(&style, lv_color_make(125, 25, 12));
	lv_obj_add_style(Label, &style, 0);

	Chaine_Opt[0] = 0;
	for (Boucle = 0; Boucle < 59; Boucle++) {
		sprintf(Txt, "%d\n", Boucle);
		strcat(Chaine_Opt, Txt);
	}
	Boucle = 59;
	sprintf(Txt, "%d", Boucle);
	strcat(Chaine_Opt, Txt);

	/*A roller on the middle w
	 * ith center aligned text, and auto (default) width*/
	Roller_M_Duree = lv_roller_create(lv_scr_act());
	lv_roller_set_options(Roller_M_Duree, Chaine_Opt, LV_ROLLER_MODE_INFINITE);
	lv_roller_set_visible_row_count(Roller_M_Duree, 3);
	lv_obj_add_style(Roller_M_Duree, &style_sel, LV_PART_SELECTED);

	lv_obj_set_size(Roller_M_Duree, 50, 100);
	lv_obj_align(Roller_M_Duree, LV_ALIGN_CENTER, 170, 0);
	lv_obj_add_event_cb(Roller_M_Duree, Roller_Duree_Minute_event_handler,LV_EVENT_ALL, NULL);


}

static void Roller_Duree_Heure_event_handler(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);
	if (code == LV_EVENT_VALUE_CHANGED) {
		char buf[32];
		lv_roller_get_selected_str(obj, buf, sizeof(buf));
		Duree_Heure = (unsigned char) (atoi(buf));
		Data_Prog.H_Duree[Num_Prog_Courant - 1] = Duree_Heure;

	}
}

static void Roller_Duree_Minute_event_handler(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);
	if (code == LV_EVENT_VALUE_CHANGED) {
		char buf[32];
		lv_roller_get_selected_str(obj, buf, sizeof(buf));
		Duree_Minute = (unsigned char) (atoi(buf));
		Data_Prog.M_Duree[Num_Prog_Courant - 1] = Duree_Minute;
	}
}

void Bouton_Retour_Regle_Prog(void) {
	/*Init the style for the default state*/
	static lv_style_t style;
	static lv_style_t style_pr;
	static lv_style_transition_dsc_t trans;
	static lv_style_prop_t props[] = { LV_STYLE_OUTLINE_WIDTH,
			LV_STYLE_OUTLINE_OPA, 0 };

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
	lv_style_set_bg_grad_color(&style_pr,
			lv_palette_darken(LV_PALETTE_BLUE, 4));
	lv_style_set_text_font(&style, &lv_font_montserrat_12);

	/*Add a transition to the the outline*/

	lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0,
			NULL);

	lv_style_set_transition(&style_pr, &trans);

	lv_obj_t *Bouton_Retour_Regle_Prog = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Retour_Regle_Prog);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Retour_Regle_Prog, 40, 50);
	lv_obj_add_style(Bouton_Retour_Regle_Prog, &style, 0);

	lv_obj_add_style(Bouton_Retour_Regle_Prog, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Retour_Regle_Prog, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Retour_Regle_Prog, LV_ALIGN_CENTER, 00, 100);

	lv_obj_add_event_cb(Bouton_Retour_Regle_Prog,event_handler_BoutonRetour_Regle_Prog, LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Retour_Regle_Prog);
	lv_label_set_text(label, "Retour");
	lv_obj_center(label);

}

void event_handler_BoutonRetour_Regle_Prog(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_30to0 = 1;
		Stocke_Data_Prog(&Data_Prog);
	}
}

void Bouton_Prog_Suivant(void) {
	/*Init the style for the default state*/
	static lv_style_t style;
	static lv_style_t style_pr;
	static lv_style_transition_dsc_t trans;
	static lv_style_prop_t props[] = { LV_STYLE_OUTLINE_WIDTH,
			LV_STYLE_OUTLINE_OPA, 0 };

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
	lv_style_set_bg_grad_color(&style_pr,
			lv_palette_darken(LV_PALETTE_BLUE, 4));
	lv_style_set_text_font(&style, &lv_font_montserrat_12);

	/*Add a transition to the the outline*/

	lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0,
			NULL);

	lv_style_set_transition(&style_pr, &trans);

	lv_obj_t *Bouton_Prog_Suivant = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Prog_Suivant);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Prog_Suivant, 40, 60);
	lv_obj_add_style(Bouton_Prog_Suivant, &style, 0);

	lv_obj_add_style(Bouton_Prog_Suivant, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Prog_Suivant, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Prog_Suivant, LV_ALIGN_CENTER, 150, 100);

	lv_obj_add_event_cb(Bouton_Prog_Suivant, event_handler_BoutonProg_Suivant,
			LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Prog_Suivant);
	lv_label_set_text(label, "Programme Suivant");
	lv_obj_center(label);

}

void event_handler_BoutonProg_Suivant(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Stocke_Data_Prog(&Data_Prog);
		Increment_Numeprog();
		Transi_30to30 = 1;
	}
}

void Bouton_Prog_Precedent(void) {
	/*Init the style for the default state*/
	static lv_style_t style;
	static lv_style_t style_pr;
	static lv_style_transition_dsc_t trans;
	static lv_style_prop_t props[] = { LV_STYLE_OUTLINE_WIDTH,
			LV_STYLE_OUTLINE_OPA, 0 };

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
	lv_style_set_bg_grad_color(&style_pr,
			lv_palette_darken(LV_PALETTE_BLUE, 4));
	lv_style_set_text_font(&style, &lv_font_montserrat_12);

	/*Add a transition to the the outline*/

	lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0,
			NULL);

	lv_style_set_transition(&style_pr, &trans);

	lv_obj_t *Bouton_Prog_Precedent = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Prog_Precedent);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Prog_Precedent, 40, 60);
	lv_obj_add_style(Bouton_Prog_Precedent, &style, 0);

	lv_obj_add_style(Bouton_Prog_Precedent, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Prog_Precedent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Prog_Precedent, LV_ALIGN_CENTER, -150, 100);

	lv_obj_add_event_cb(Bouton_Prog_Precedent,
			event_handler_BoutonProg_Precedent, LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Prog_Precedent);
	lv_label_set_text(label, "Programme Precedent");
	lv_obj_center(label);

}

void event_handler_BoutonProg_Precedent(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_30to30 = 1;
		Stocke_Data_Prog(&Data_Prog);
		Decrement_Numeprog();
	}
}

static void radio_event_handler(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);
	char txt[2];
	unsigned char Val;

	if (code == LV_EVENT_VALUE_CHANGED) {
		txt[0] = 0;
		strcat(txt, lv_checkbox_get_text(obj));
		if (txt[0] == 'L')
			Val = 0x01 << 0;
		if ((txt[0] == 'M') && (txt[1] == 'a'))
			Val = 0x01 << 1;
		if ((txt[0] == 'M') && (txt[1] == 'e'))
			Val = 0x01 << 2;
		if (txt[0] == 'J')
			Val = 0x01 << 3;
		if (txt[0] == 'V')
			Val = 0x01 << 4;
		if (txt[0] == 'S')
			Val = 0x01 << 5;
		if (txt[0] == 'D')
			Val = 0x01 << 6;

		if (lv_obj_get_state(obj) & LV_STATE_CHECKED) {
			Data_Prog.Jour[Num_Prog_Courant - 1] =
					Data_Prog.Jour[Num_Prog_Courant - 1] | Val;
		} else {
			Data_Prog.Jour[Num_Prog_Courant - 1] =
					Data_Prog.Jour[Num_Prog_Courant - 1] & ~Val;
		}
	}
}

void CheckBox_Jour(void) {

	//lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_COLUMN);
	//lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);


	BoxJour[0] = lv_checkbox_create(lv_scr_act());
	lv_checkbox_set_text(BoxJour[0], "Lu");
	//lv_obj_clear_state(BoxJour[0], LV_STATE_CHECKED);
	lv_obj_add_event_cb(BoxJour[0], radio_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(BoxJour[0], LV_ALIGN_CENTER, -200, -60);
	lv_obj_clear_state(BoxJour[0], LV_STATE_CHECKED);

	BoxJour[1] = lv_checkbox_create(lv_scr_act());
	lv_checkbox_set_text(BoxJour[1], "Ma");
	//lv_obj_clear_state(BoxJour[1], LV_STATE_CHECKED);
	lv_obj_add_event_cb(BoxJour[1], radio_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(BoxJour[1], LV_ALIGN_CENTER, -140, -60);
	lv_obj_clear_state(BoxJour[1], LV_STATE_CHECKED) ;

	BoxJour[2] = lv_checkbox_create(lv_scr_act());
	//lv_obj_clear_state(BoxJour[2], LV_STATE_CHECKED);
	lv_checkbox_set_text(BoxJour[2], "Me");
	lv_obj_add_event_cb(BoxJour[2], radio_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(BoxJour[2], LV_ALIGN_CENTER, -200, -20);
	lv_obj_clear_state(BoxJour[2], LV_STATE_CHECKED);

	BoxJour[3] = lv_checkbox_create(lv_scr_act());
	//lv_obj_clear_state(BoxJour[3], LV_STATE_CHECKED);
	lv_checkbox_set_text(BoxJour[3], "Je");
	lv_obj_add_event_cb(BoxJour[3], radio_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(BoxJour[3], LV_ALIGN_CENTER, -140, -20);
	lv_obj_clear_state(BoxJour[3], LV_STATE_CHECKED) ;

	BoxJour[4] = lv_checkbox_create(lv_scr_act());
	//lv_obj_clear_state(BoxJour[4], LV_STATE_CHECKED);
	lv_checkbox_set_text(BoxJour[4], "Ve");
	lv_obj_add_event_cb(BoxJour[4], radio_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(BoxJour[4], LV_ALIGN_CENTER, -200, 20);
	lv_obj_clear_state(BoxJour[4], LV_STATE_CHECKED);


	BoxJour[5] = lv_checkbox_create(lv_scr_act());
	//lv_obj_clear_state(BoxJour[5], LV_STATE_CHECKED);
	lv_checkbox_set_text(BoxJour[5], "Sa");
	lv_obj_add_event_cb(BoxJour[5], radio_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(BoxJour[5], LV_ALIGN_CENTER, -140, 20);
	lv_obj_clear_state(BoxJour[5], LV_STATE_CHECKED) ;

	BoxJour[6] = lv_checkbox_create(lv_scr_act());
	//lv_obj_clear_state(BoxJour[6], LV_STATE_CHECKED);
	lv_checkbox_set_text(BoxJour[6], "Di");
	lv_obj_add_event_cb(BoxJour[6], radio_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(BoxJour[6], LV_ALIGN_CENTER, -170, 60);
	lv_obj_clear_state(BoxJour[6], LV_STATE_CHECKED);

	//lv_obj_update_layout(BoxJour[6]);

}

