/*
 * Ecran_Lancer_Capteur.c
 *
 *  Created on: 4 mai 2023
 *      Author: pierr
 */
#include "lvgl/lvgl.h"
#include <stdlib.h>
#include <Service_I2C.h>
#include <Machine_Etat.h>
#include <Ecran_Lancer_Capteur.h>

extern char Transi_4to0;
extern char Transi_4to4;

lv_obj_t * preload;
lv_obj_t * Texte_2;

int CapteurActivated;

void event_handler_BoutonRetour_Lancer_Capteur(lv_event_t *e);
void event_handler_Bouton_Appliquer_Capteur(lv_event_t * e);
void event_handler_Bouton_Stopper_Capteur(lv_event_t * e);


void Creer_Ecran_Lancer_Capteur(void) {
	lv_obj_clean(lv_scr_act());
	Bouton_Retour_Lancer_Capteur();
	Bouton_Appliquer_Capteur();
	Bouton_Stopper_Capteur();
	Afficher_Texte_Capteur();
	if (CapteurActivated==1){
		Afficher_Texte_Capteur_2();
		lv_ex_spinner_1();
	}
}



void Bouton_Retour_Lancer_Capteur(void) {
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

	lv_obj_t *Bouton_Retour_Lancer_Capteur = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Retour_Lancer_Capteur);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Retour_Lancer_Capteur, 40, 50);
	lv_obj_add_style(Bouton_Retour_Lancer_Capteur, &style, 0);

	lv_obj_add_style(Bouton_Retour_Lancer_Capteur, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Retour_Lancer_Capteur, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Retour_Lancer_Capteur, LV_ALIGN_CENTER, 00, 100);

	lv_obj_add_event_cb(Bouton_Retour_Lancer_Capteur,event_handler_BoutonRetour_Lancer_Capteur, LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Retour_Lancer_Capteur);
	lv_label_set_text(label, "Retour");
	lv_obj_center(label);

}

void event_handler_BoutonRetour_Lancer_Capteur(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_4to0 = 1;
	}
}

void Bouton_Appliquer_Capteur(void) {
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

	lv_obj_t *Bouton_Appliquer_Capteur = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Appliquer_Capteur);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Appliquer_Capteur, 80, 80);
	lv_obj_add_style(Bouton_Appliquer_Capteur, &style, 0);

	lv_obj_add_style(Bouton_Appliquer_Capteur, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Appliquer_Capteur, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Appliquer_Capteur, LV_ALIGN_CENTER, 120, -20);

	lv_obj_add_event_cb(Bouton_Appliquer_Capteur, event_handler_Bouton_Appliquer_Capteur,
			LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Appliquer_Capteur);
	lv_label_set_text(label, "Appliquer Capteur");
	lv_obj_center(label);

}

void event_handler_Bouton_Appliquer_Capteur(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	//if (Mode_Manuel==0){
		if (code == LV_EVENT_PRESSED) {
			Transi_4to4 = 1;
			CapteurActivated=1;
		}
	//}else{
	//	Afficher_Texte_Capteur_3();
	}


void Afficher_Texte_Capteur(void){
	//static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, 0};
	static lv_obj_t * Texte;
	static lv_style_t style_txt;


	Texte = lv_label_create(lv_scr_act());

	lv_label_set_recolor(Texte, true);
	lv_label_set_text(Texte, "MODE CAPTEUR");

	lv_color_t color = lv_palette_main(LV_PALETTE_BLUE) ;
	lv_style_set_text_color(&style_txt,color);	//lv_color_hex3(0xE27)
	lv_style_set_bg_color(&style_txt,lv_color_hex3(0x0b0) );
	lv_style_set_text_letter_space(&style_txt, 5);
	lv_style_set_text_line_space(&style_txt, 10);
	lv_style_set_text_font(&style_txt, &lv_font_montserrat_22);
	lv_obj_add_style(Texte, &style_txt, 0);

	lv_obj_align(Texte, LV_ALIGN_CENTER, 0, -70);

}

void Afficher_Texte_Capteur_2(void){
	//static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, 0};
	static lv_style_t style_txt;
	char Label[25] ;


	Texte_2 = lv_label_create(lv_scr_act());

	lv_label_set_recolor(Texte_2, true);
	sprintf(Label, "Mode Capteur en cours");
	lv_label_set_text(Texte_2, Label);

	lv_color_t color = lv_palette_main(LV_PALETTE_RED) ;
	lv_style_set_text_color(&style_txt,color);
	lv_style_set_bg_color(&style_txt,lv_color_hex3(0x0b0) );
	lv_style_set_text_letter_space(&style_txt, 5);
	lv_style_set_text_line_space(&style_txt, 10);
	lv_style_set_text_font(&style_txt, &lv_font_montserrat_10);
	lv_obj_add_style(Texte_2, &style_txt, 0);

	lv_obj_align(Texte_2, LV_ALIGN_CENTER, -120, 75);
}

void Afficher_Texte_Capteur_3(void){
	//static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, 0};
	static lv_style_t style_txt;
	char Label[25] ;


	Texte_2 = lv_label_create(lv_scr_act());

	lv_label_set_recolor(Texte_2, true);
	sprintf(Label, "Mode manuel en cours");
	lv_label_set_text(Texte_2, Label);

	lv_color_t color = lv_palette_main(LV_PALETTE_RED) ;
	lv_style_set_text_color(&style_txt,color);
	lv_style_set_bg_color(&style_txt,lv_color_hex3(0x0b0) );
	lv_style_set_text_letter_space(&style_txt, 5);
	lv_style_set_text_line_space(&style_txt, 10);
	lv_style_set_text_font(&style_txt, &lv_font_montserrat_10);
	lv_obj_add_style(Texte_2, &style_txt, 0);

	lv_obj_align(Texte_2, LV_ALIGN_CENTER, -120, 0);
}



void Bouton_Stopper_Capteur(void) {
	/*Init the style for the default state*/
	static lv_style_t style;
	lv_style_init(&style);

	lv_style_set_radius(&style, 3);

	lv_style_set_bg_opa(&style, LV_OPA_100);
	lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_RED));
	lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_RED, 2));
	lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

	lv_style_set_border_opa(&style, LV_OPA_40);
	lv_style_set_border_width(&style, 2);
	lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));

	lv_style_set_shadow_width(&style, 8);
	lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
	lv_style_set_shadow_ofs_y(&style, 8);

	lv_style_set_outline_opa(&style, LV_OPA_COVER);
	lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_RED));

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

	lv_obj_t *Bouton_Stopper_Capteur = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Stopper_Capteur);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Stopper_Capteur, 80, 80);
	lv_obj_add_style(Bouton_Stopper_Capteur, &style, 0);

	lv_obj_add_style(Bouton_Stopper_Capteur, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Stopper_Capteur, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Stopper_Capteur, LV_ALIGN_CENTER, 120, 40);

	lv_obj_add_event_cb(Bouton_Stopper_Capteur, event_handler_Bouton_Stopper_Capteur,
			LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Stopper_Capteur);
	lv_label_set_text(label, "Stopper Capteur");
	lv_obj_center(label);

}

void event_handler_Bouton_Stopper_Capteur(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_4to4 = 1;
		CapteurActivated=0;
	}
}

void lv_ex_spinner_1(void)
{
    //Create a Preloader object
    preload = lv_spinner_create(lv_scr_act(),2000,20);
    lv_obj_set_size(preload, 100, 100);
    lv_obj_align(preload,LV_ALIGN_CENTER, -125, 0);
}



