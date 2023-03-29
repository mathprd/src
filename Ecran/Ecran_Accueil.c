#include "lvgl/lvgl.h"
#include <Service_I2C.h>
#include <Ecran_Accueil.h>
#include <Machine_Etat.h>
#include <Prog_Tempo.h>

extern DS1307_Time_Typedef UserTime;

static lv_obj_t *label_H, *label_D;

void event_handler_Bouton_Regl_Heure(lv_event_t*);
void event_handler_Bouton_Marche(lv_event_t*);
void event_handler_Bouton_Regl_Prog(lv_event_t *e);
void event_handler_Bouton_Mode_Prog(lv_event_t *e) ;

void Reglage_Heure(void);

extern char Transi_0to1;
extern char Transi_0to2;
extern char Transi_0to3;
extern char Transi_0to4;
extern char Transi_0to30;
extern Data_Prog_Typedef Data_Prog;

extern char Prog_En_Marche[8] ;

void Creer_Ecran_Acceuil(void) {
	lv_obj_clean(lv_scr_act());
	Dessine_Horloge();
	Bouton_Marche();
	Bouton_Reglage_Heure();
	Bouton_Reglage_Prog();
	Bouton_Mode_Prog() ;
	DS1307_GetTime(&UserTime);
	Change_Heure(UserTime.Hour, UserTime.Min, UserTime.Sec);
	Change_Date(UserTime.Date, UserTime.Month, UserTime.Year);
	Creer_Label_ProgActif();
	lv_task_handler();

}

void Dessine_Horloge(void) {
	static lv_style_t style, styleD;
	lv_style_init(&style);

	/*Set a background color and a radius*/
	//lv_style_set_radius(&style, 20);
	//lv_style_set_bg_opa(&style, LV_OPA_COVER);
	lv_style_set_bg_color(&style, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2));

	/*Add border to the bottom+right*/
	lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_DEEP_PURPLE));
	lv_style_set_border_width(&style, 3);
	lv_style_set_border_opa(&style, LV_OPA_80);
	lv_style_set_border_side(&style, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT);

	/*Create an object with the new style*/
	lv_obj_t *Cadran = lv_obj_create(lv_scr_act());
	lv_obj_add_style(Cadran, &style, 0);
	lv_obj_center(Cadran);
	//lv_obj_set_width(Cadran, lv_pct(40))
	lv_obj_set_size(Cadran, 150, 60);

	label_H = lv_label_create(Cadran);
	lv_label_set_text(label_H, "HH:MM:SS");
	lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_DEEP_PURPLE));
	lv_style_set_text_letter_space(&style, 5);
	lv_style_set_text_line_space(&style, 10);
	lv_style_set_text_font(&style, &lv_font_montserrat_20);
	lv_obj_center(label_H);
	lv_obj_add_style(Cadran, &style, 0);
	lv_obj_align(Cadran, LV_ALIGN_CENTER, 0, -20);

	label_D = lv_label_create(lv_scr_act());
	lv_label_set_text(label_D, "JJ/MM/AA");

	lv_style_set_text_color(&styleD, lv_color_hex3(0xD02));
	lv_style_set_text_letter_space(&styleD, 5);
	lv_style_set_text_line_space(&styleD, 10);
	lv_style_set_text_font(&styleD, &lv_font_montserrat_12);

	lv_obj_center(label_D);
	lv_obj_align(label_D, LV_ALIGN_CENTER, 0, -60);

	lv_obj_add_style(label_D, &styleD, 0);

}

void Change_Heure(char Heu, char Min, char Sec) {
	char buf[40];

	lv_snprintf(buf, sizeof(buf), "%02d:%02d:%02d", Heu, Min, Sec);
	lv_label_set_text(label_H, buf);

}

void Print_Val(char Val) {
	char buf[40];

	lv_snprintf(buf, sizeof(buf), "Valeur : %2d", Val);
	lv_label_set_text(label_H, buf);

}

void Change_Date(char Jour, char Mois, char Annee) {
	char buf[40];

	lv_snprintf(buf, sizeof(buf), "%02d/%02d/%02d", Jour, Mois, Annee);
	lv_label_set_text(label_D, buf);

}

void Bouton_Reglage_Heure(void) {
	/*Init the style for the default state*/
	static lv_style_t style;
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

	lv_obj_t *Bouton_Regl_Heure = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Regl_Heure);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Regl_Heure, 50, 80);
	lv_obj_add_style(Bouton_Regl_Heure, &style, 0);

	lv_obj_add_style(Bouton_Regl_Heure, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Regl_Heure, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Regl_Heure, LV_ALIGN_CENTER, 0, 100);

	lv_obj_add_event_cb(Bouton_Regl_Heure, event_handler_Bouton_Regl_Heure,
			LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Regl_Heure);
	lv_label_set_text(label, "Reglage Heure/Date");
	lv_obj_center(label);

}

void event_handler_Bouton_Regl_Heure(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_0to1 = 1;
	}
}

void Bouton_Reglage_Prog(void) {
	/*Init the style for the default state*/
	static lv_style_t style;
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

	lv_obj_t *Bouton_Regl_Prog = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Regl_Prog);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Regl_Prog, 50, 80);
	lv_obj_add_style(Bouton_Regl_Prog, &style, 0);

	lv_obj_add_style(Bouton_Regl_Prog, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Regl_Prog, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Regl_Prog, LV_ALIGN_CENTER, 150, 100);

	lv_obj_add_event_cb(Bouton_Regl_Prog, event_handler_Bouton_Regl_Prog,
			LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Regl_Prog);
	lv_label_set_text(label, "Reglage Progamme");
	lv_obj_center(label);

}

void event_handler_Bouton_Regl_Prog(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_0to30 = 1;
	}
}

void Bouton_Marche(void) {
	/*Init the style for the default state*/
	static lv_style_t style;
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

	lv_obj_t *Bouton_Marche = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Marche);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Marche, 50, 100);
	lv_obj_add_style(Bouton_Marche, &style, 0);

	lv_obj_add_style(Bouton_Marche, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Marche, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Marche, LV_ALIGN_CENTER, 0 , 50);

	lv_obj_add_event_cb(Bouton_Marche, event_handler_Bouton_Marche,
			LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Marche);
	lv_label_set_text(label, "Mode Manuel");
	lv_obj_center(label);

}

void event_handler_Bouton_Marche(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_0to2 = 1;
	}
}

void Bouton_Mode_Prog(void) {
	/*Init the style for the default state*/
	static lv_style_t style;
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

	lv_obj_t *Bouton_Lancer_Prog = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Lancer_Prog);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Lancer_Prog, 50, 80);
	lv_obj_add_style(Bouton_Lancer_Prog, &style, 0);

	lv_obj_add_style(Bouton_Lancer_Prog, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Lancer_Prog, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Lancer_Prog, LV_ALIGN_CENTER, -150, 100);

	lv_obj_add_event_cb(Bouton_Lancer_Prog, event_handler_Bouton_Mode_Prog,
			LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Lancer_Prog);
	lv_label_set_text(label, "Mode Progamme");
	lv_obj_center(label);

}

void event_handler_Bouton_Mode_Prog(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_0to3 = 1 ;
	}
}



void Creer_Label_ProgActif(void) {
	int Boucle;
	char Texte[7];
	lv_style_t style_Txt;
	lv_obj_t *LabelProgActif[NumProgMax];

	lv_style_reset(&style_Txt);
	lv_style_set_text_font(&style_Txt, &lv_font_montserrat_12);
	lv_style_set_text_color(&style_Txt,	lv_color_make(190, 0, 25));

	for (Boucle = 0; Boucle < NumProgMax; Boucle++)
	{
		if (Prog_En_Marche[Boucle] == 1)
		{
			LabelProgActif[Boucle] = lv_label_create(lv_scr_act());
			sprintf(Texte, " P%d on", Boucle + 1);
			lv_label_set_text(LabelProgActif[Boucle], Texte);
			lv_obj_add_style(LabelProgActif[Boucle], &style_Txt, 0);
			lv_obj_align(LabelProgActif[Boucle], LV_ALIGN_CENTER, (-180 + 50 * Boucle), -120);

		}


	}

}

