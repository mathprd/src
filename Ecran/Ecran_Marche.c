/*
 * Ecran_Marche.c
 *
 *  Created on: 19 juil. 2021
 *      Author: mahout
 */

#include <Ecran_Marche.h>
#include <Machine_Etat.h>


#include "lvgl/lvgl.h"

static lv_obj_t * slider_label;
lv_obj_t * Slider_Marche;

void event_handler_Bouton_Retour_Marche(lv_event_t *);
static void slider_event_cb(lv_event_t * );


//extern char Transi_0to1;
extern char Transi_1to0;
extern char Transi_2to0;
//char Transi_0to2;

extern char Poussoir_Start_Appui;
//char Fin_Tempo;
extern int Compteur_Marche_Pompe;
extern char Minute60Sec ;



void Creer_Ecran_Marche(void)
{
	lv_obj_clean(lv_scr_act());
	Marche();
	Bouton_Retour_Marche();
	lv_task_handler();
}


void Marche(void)
{
	//static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, 0};
	static lv_obj_t * Texte_Pompe;
	static lv_style_t style_label;
	static lv_style_t style_txt;
	static lv_style_t style_main;
	static lv_style_t style_indicator;
	static lv_style_t style_knob;
	static lv_style_t style_pressed_color;


	Texte_Pompe = lv_label_create(lv_scr_act());

	lv_label_set_recolor(Texte_Pompe, true);
	lv_label_set_text(Texte_Pompe, "Pompe en !");
	//lv_obj_set_size(Texte_Pompe, 400, 30);

	lv_style_set_text_color(&style_txt,lv_color_hex3(0xE27));
	lv_style_set_bg_color(&style_txt,lv_color_hex3(0x0b0) );
	lv_style_set_text_letter_space(&style_txt, 5);
	lv_style_set_text_line_space(&style_txt, 10);
	lv_style_set_text_font(&style_txt, &lv_font_montserrat_28);
	lv_obj_add_style(Texte_Pompe, &style_txt, 0);

	lv_obj_align(Texte_Pompe, LV_ALIGN_CENTER, -40, -70);


	lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 500, 45);
	lv_obj_set_size(spinner, 70, 70);
	lv_obj_center(spinner);
	lv_obj_align_to(spinner,Texte_Pompe, LV_ALIGN_CENTER, 230, 0);





	lv_style_init(&style_main);
	lv_style_set_bg_opa(&style_main, LV_OPA_COVER);
	lv_style_set_bg_color(&style_main, lv_color_hex3(0xbbb));
	lv_style_set_radius(&style_main, LV_RADIUS_CIRCLE);
	lv_style_set_pad_ver(&style_main, -2); /*Makes the indicator larger*/

	lv_style_init(&style_indicator);
	lv_style_set_bg_opa(&style_indicator, LV_OPA_COVER);
	lv_style_set_bg_color(&style_indicator, lv_palette_main(LV_PALETTE_CYAN));
	lv_style_set_radius(&style_indicator, LV_RADIUS_CIRCLE);


	lv_style_init(&style_knob);
	lv_style_set_bg_opa(&style_knob, LV_OPA_COVER);
	lv_style_set_bg_color(&style_knob, lv_palette_main(LV_PALETTE_RED));
	lv_style_set_border_color(&style_knob, lv_palette_darken(LV_PALETTE_CYAN, 3));
	lv_style_set_border_width(&style_knob, 3);
	lv_style_set_radius(&style_knob, LV_RADIUS_CIRCLE);
	lv_style_set_pad_all(&style_knob, 10); /*Makes the knob larger*/


	lv_style_init(&style_pressed_color);
	lv_style_set_bg_color(&style_pressed_color, lv_palette_darken(LV_PALETTE_PURPLE,1));

	/*Create a slider and add the style*/
	Slider_Marche = lv_slider_create(lv_scr_act());
	lv_slider_set_value(Slider_Marche, 20, LV_ANIM_ON);
	lv_slider_set_range(Slider_Marche,5, 120);
	lv_obj_remove_style_all(Slider_Marche);        /*Remove the styles coming from the theme*/

	lv_obj_add_style(Slider_Marche, &style_main, LV_PART_MAIN);
	lv_obj_add_style(Slider_Marche, &style_indicator, LV_PART_INDICATOR);
	lv_obj_add_style(Slider_Marche, &style_pressed_color, LV_PART_INDICATOR | LV_STATE_PRESSED);
	lv_obj_add_style(Slider_Marche, &style_knob, LV_PART_KNOB);
	lv_obj_add_style(Slider_Marche, &style_pressed_color, LV_PART_KNOB | LV_STATE_PRESSED);
	lv_obj_align(Slider_Marche, LV_ALIGN_CENTER, 0, 10);
	lv_obj_add_event_cb(Slider_Marche, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

	lv_obj_add_style(spinner, &style_knob, LV_PART_MAIN);



	slider_label = lv_label_create(lv_scr_act());
	lv_label_set_text(slider_label, "Il reste 20 minutes");

	//lv_obj_set_size(slider_label, 400, 30);


	lv_style_set_text_color(&style_label, lv_palette_main(LV_PALETTE_DEEP_PURPLE));
	lv_style_set_text_letter_space(&style_label, 5);
	lv_style_set_text_line_space(&style_label, 10);
	lv_style_set_text_font(&style_label, &lv_font_montserrat_18);
	lv_obj_add_style(slider_label, &style_label, 0);
	lv_obj_align_to(slider_label, Slider_Marche, LV_ALIGN_CENTER, 0, 30);


}

static void slider_event_cb(lv_event_t * e)
{
	lv_obj_t * slider = lv_event_get_target(e);
	char buf[25];
	lv_snprintf(buf, sizeof(buf), "Il reste %d minutes", lv_slider_get_value(slider));
	lv_label_set_text(slider_label, buf);
	Compteur_Marche_Pompe = lv_slider_get_value(slider);

	//lv_obj_align_to(slider_label, slider, LV_ALIGN_CENTER, 0, 50);
}

void Refresh_Slider(int Val)
{

	char buf[25];

	if (Val > 5)
	{
		lv_slider_set_value(Slider_Marche, Val, LV_ANIM_ON);
		lv_snprintf(buf, sizeof(buf), "Il reste %d minutes", Compteur_Marche_Pompe);
		lv_label_set_text(slider_label, buf);
	}

	else
	{
		lv_snprintf(buf, sizeof(buf), "Il reste %d mn et %d sec", Compteur_Marche_Pompe,Minute60Sec);
		lv_label_set_text(slider_label, buf);
	}




}

void Bouton_Retour_Marche(void)
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

	lv_obj_t * Bouton_Retour_Marche = lv_btn_create(lv_scr_act());
	lv_obj_remove_style_all(Bouton_Retour_Marche);
	/*Remove the style coming from the theme*/
	lv_obj_set_size(Bouton_Retour_Marche, 40, 50);
	lv_obj_add_style(Bouton_Retour_Marche, &style, 0);

	lv_obj_add_style(Bouton_Retour_Marche, &style_pr, LV_STATE_PRESSED);

	lv_obj_set_size(Bouton_Retour_Marche, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(Bouton_Retour_Marche, LV_ALIGN_CENTER, 0, 100);

	lv_obj_add_event_cb(Bouton_Retour_Marche, event_handler_Bouton_Retour_Marche, LV_EVENT_ALL, NULL);

	lv_obj_t * label = lv_label_create(Bouton_Retour_Marche);
	lv_label_set_text(label, "Retour");
	lv_obj_center(label);

}


void event_handler_Bouton_Retour_Marche(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_PRESSED) {
		Transi_2to0 = 1;
	}
}
