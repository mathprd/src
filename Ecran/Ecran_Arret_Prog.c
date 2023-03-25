/*
 * Ecran_Arret_Prog.c
 *
 *  Created on: 24 mars 2023
 *      Author: mathi
 */


#include "lvgl/lvgl.h"
#include <Ecran_Arret_Prog.h>
#include <stdlib.h>
#include <Service_I2C.h>
#include <Machine_Etat.h>


extern char Transi_4to0;
extern char Prog_En_Marche ;


void event_handler_BoutonRetour_Arret_Prog(lv_event_t *e) ;
void event_handler_Bouton_Stopper_Prog(lv_event_t *e) ;

void Creer_Ecran_Arret_Prog(void){
	lv_obj_clean(lv_scr_act());
	Bouton_Retour_Arret_Prog();
	Afficher_Texte_Arret();
	Bouton_Arreter_Prog();
}



void Bouton_Retour_Arret_Prog(void) {
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

	lv_obj_t *Bouton_Retour_Arret_Prog = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Retour_Arret_Prog);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Retour_Arret_Prog, 40, 50);
	lv_obj_add_style(Bouton_Retour_Arret_Prog, &style, 0);

	lv_obj_add_style(Bouton_Retour_Arret_Prog, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Retour_Arret_Prog, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Retour_Arret_Prog, LV_ALIGN_CENTER, 00, 100);

	lv_obj_add_event_cb(Bouton_Retour_Arret_Prog,event_handler_BoutonRetour_Arret_Prog, LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Retour_Arret_Prog);
	lv_label_set_text(label, "Retour");
	lv_obj_center(label);

}

void event_handler_BoutonRetour_Arret_Prog(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_4to0 = 1;
	}
}

void Afficher_Texte_Arret(void){

	//static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, 0};
	static lv_obj_t * Texte;
	static lv_obj_t * Texte2;
	static lv_style_t style_txt;
	char Titre[20];

	Texte = lv_label_create(lv_scr_act());

	lv_label_set_recolor(Texte, true);
	lv_label_set_text(Texte, "Stopper le programme");
	//lv_obj_set_size(Texte, 400, 30);

	lv_style_set_text_color(&style_txt,lv_palette_main(LV_PALETTE_RED));
	lv_style_set_bg_color(&style_txt,lv_color_hex3(0x0b0) );
	lv_style_set_text_letter_space(&style_txt, 5);
	lv_style_set_text_line_space(&style_txt, 10);
	lv_style_set_text_font(&style_txt, &lv_font_montserrat_28);
	lv_obj_add_style(Texte, &style_txt, 0);

	lv_obj_align(Texte, LV_ALIGN_CENTER, 0, -80);

	//Affichage 2eme ligne

	Texte2 = lv_label_create(lv_scr_act());

	lv_label_set_recolor(Texte2, true);
	sprintf(Titre, "n° %d en cours ?", Prog_En_Marche);
	lv_label_set_text(Texte2, Titre);

	lv_obj_add_style(Texte2, &style_txt, 0);

	lv_obj_align(Texte2, LV_ALIGN_CENTER, 0, -45);
}


void Bouton_Arreter_Prog(void) {
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
	lv_style_set_text_font(&style, &lv_font_montserrat_14);

	/*Add a transition to the the outline*/
	static lv_style_transition_dsc_t trans;
	static lv_style_prop_t props[] = { LV_STYLE_OUTLINE_WIDTH,
			LV_STYLE_OUTLINE_OPA, 0 };
	lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0,
	NULL);

	lv_style_set_transition(&style_pr, &trans);

	lv_obj_t *Bouton_Stopper_Prog = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Stopper_Prog);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Stopper_Prog, 110, 90);
	lv_obj_add_style(Bouton_Stopper_Prog, &style, 0);

	lv_obj_add_style(Bouton_Stopper_Prog, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Stopper_Prog, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Stopper_Prog, LV_ALIGN_CENTER, 0, 20);

	lv_obj_add_event_cb(Bouton_Stopper_Prog, event_handler_Bouton_Stopper_Prog,
			LV_EVENT_ALL, NULL);

	lv_obj_t *label = lv_label_create(Bouton_Stopper_Prog);
	lv_label_set_text(label, "Stopper Progamme");
	lv_obj_center(label);

}

void event_handler_Bouton_Stopper_Prog(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);

	if (code == LV_EVENT_PRESSED) {
		Transi_4to0 = 1;
		Prog_En_Marche = 0 ;
	}
}





