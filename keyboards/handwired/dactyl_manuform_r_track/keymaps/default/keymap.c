/* Copyright 2020 Qurn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.	If not, see <http://www.gnu.org/licenses/>.
 */

/***************************
 * Usual defines
 **************************/

#include QMK_KEYBOARD_H

#define _COLEMAKDHM 0

/***************************
 * Trackball related defines
 **************************/

#include "pointing_device.h"
#include "../../pmw3360/pmw3360.h"
#include "print.h"

uint8_t track_mode = 0; // 0 Mousecursor; 1 arrowkeys/carret; 2 scrollwheel; 3 sound/brightness
#define cursor_mode 0
#define carret_mode 1
#define scroll_mode 2
#define sound_brightness_mode 3
uint8_t prev_track_mode = 0;
bool integration_mode = false;
int16_t cum_x = 0;
int16_t cum_y = 0;
int16_t sensor_x = 0;
int16_t sensor_y = 0;

// Thresholds help to move only horizontal or vertical. When accumulated distance reaches threshold, only move one discrete value in direction with bigger delta.
uint8_t	carret_threshold = 24;		 // higher means slower
uint16_t carret_threshold_inte = 340; // in integration mode higher threshold

#define regular_smoothscroll_factor 8
bool smooth_scroll = true;
uint8_t	scroll_threshold = 8 / regular_smoothscroll_factor;	// divide if started smooth
uint16_t scroll_threshold_inte = 1200 / regular_smoothscroll_factor;

uint16_t cursor_multiplier = 50;	// adjust cursor speed
uint16_t cursor_multiplier_inte = 20;
#define CPI_STEP 20

int16_t cur_factor;

/***************************
 * Sticky key related defines
 **************************/

//identify keycombinations
//bool is_alt_active = false;
//bool is_sft_active = false;
//bool is_ctl_active = false;
//bool is_gui_active = false;
//bool is_low_active = false;
//bool is_rai_active = false;
//
//bool activate_alt = false;
//bool activate_sft = false;
//bool activate_ctl = false;
//bool activate_gui = false;
//bool activate_low = false;
//bool activate_rai = false;

bool sticky_key = false;
uint32_t sticky_timer = 0;
#define STICKY_TERM 500

/***************************
 * Tap dance 1 of 2
 **************************/

// tapdance keycodes
enum td_keycodes {
	ALT_TM,
	SFT_TM,
	CTL_TM,
	GUI_TM,
	RAI_TM,
	LOW_TM
};

// define a type containing as many tapdance states as you need
//typedef enum {
//	SINGLE_TAP,
//	SINGLE_HOLD
//} td_state_t;

// create a global instance of the tapdance state type
//static td_state_t td_state;

// function to determine the current tapdance state
//int cur_dance (qk_tap_dance_state_t *state);

//// `finished` and `reset` functions for each tapdance keycode
//void alttm_finished (qk_tap_dance_state_t *state, void *user_data);
//void alttm_reset (qk_tap_dance_state_t *state, void *user_data);
//void sfttm_finished (qk_tap_dance_state_t *state, void *user_data);
//void sfttm_reset (qk_tap_dance_state_t *state, void *user_data);
//void ctltm_finished (qk_tap_dance_state_t *state, void *user_data);
//void ctltm_reset (qk_tap_dance_state_t *state, void *user_data);
//void guitm_finished (qk_tap_dance_state_t *state, void *user_data);
//void guitm_reset (qk_tap_dance_state_t *state, void *user_data);
//void raisetm_finished (qk_tap_dance_state_t *state, void *user_data);
//void raisetm_reset (qk_tap_dance_state_t *state, void *user_data);
//void lowertm_finished (qk_tap_dance_state_t *state, void *user_data);
//void lowertm_reset (qk_tap_dance_state_t *state, void *user_data);

/***************************
 * Mouse pressed
 **************************/

void on_mouse_button(uint8_t mouse_button, bool pressed) {
	report_mouse_t report = pointing_device_get_report();

	if(pressed)
		report.buttons |= mouse_button;
	else
		report.buttons &= ~mouse_button;
	pointing_device_set_report(report);
	pointing_device_send();
}

/***************************
 * Custom Keycodes and Layers
 **************************/

enum custom_keycodes {
	KC_INTE = SAFE_RANGE,
	KC_RAISE,
	KC_LOWER,
	KC_SCLN_INV,
	KC_QUOT_MY, //not US Int style
	KC_TILD_MY, //not US Int style
	KC_BSPC_LCTL,
	KC_CPI_DOWN,
	KC_CPI_STD,
	KC_CPI_UP,
	KC_SMO_SC
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

//[_COLEMAK] = LAYOUT_5x6(
//DM_REC1,    DM_REC1, DM_PLY1, DM_REC2 , DM_PLY2 , DM_RSTP,               KC_CPI_DOWN, KC_CPI_STD , KC_CPI_UP , KC_SMO_SC , KC_0        ,KC_QUOT_MY,
//KC_TAB,     KC_Q ,   KC_W   , KC_F    , KC_P    , KC_G   ,               KC_J    ,    KC_L       , KC_U      , KC_Y      , KC_SCLN_INV ,KC_QUOT_MY,
//KC_ESC,     KC_A ,   KC_R   , KC_S    , KC_T    , KC_D   ,               KC_H    ,    KC_N       , KC_E      , KC_I      , KC_O        ,KC_MINS,
//KC_TILD_MY, KC_Z ,   KC_X   , KC_C    , KC_V    , KC_B   ,               KC_K    ,    KC_M       , KC_COMM   , KC_DOT    , KC_SLSH     ,KC_BSLASH,
//                           _______, _______,                                         _______, KC_LGUI,
//                                       TD(SFT_TM),    TD(RAI_TM),          _______, KC_SPC,
//                                       TD(CTL_TM),    TD(GUI_TM),          _______, KC_ENT,
//                                       TD(ALT_TM),    TD(LOW_TM),          KC_BSPC, KC_DEL
//),
[_COLEMAKDHM] = LAYOUT_5x6(
DM_REC1,    DM_REC1, DM_PLY1, DM_REC2 , DM_PLY2 , DM_RSTP,               KC_6,    KC_7,    KC_8,     KC_9,     KC_0,     KC_MINS,
KC_TAB,     KC_Q ,   KC_W   , KC_F    , KC_P    , KC_B   ,               KC_Y,    KC_U,    KC_I,     KC_O,     KC_P,     KC_LBRC,
KC_ESC,     KC_A ,   KC_R   , KC_S    , KC_T    , KC_G   ,               KC_H,    KC_J,    KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
KC_TILD_MY, KC_Z ,   KC_X   , KC_C    , KC_D    , KC_V   ,               KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH,  OSM(MOD_RSFT),
                                        KC_A    , KC_B   ,                                 KC_M,  KC_N,  KC_LEFT,    KC_DOWN,
                                       KC_UP,   KC_RGHT,                     KC_I,  KC_J,
                                       KC_RALT,   KC_RCTL,               KC_ENT, KC_SPC,
                                       KC_G,   KC_H
                                                
                                                
),
};

/***************************
 * Trackball handling
 **************************/

void pointing_device_init(void){
  config_pmw_t cfg;
  cfg.cpi = 200;
	if(!is_keyboard_master())
		return;
	pmw_init();
  pmw_set_config(cfg);
}

int max(int num1, int num2) { return (num1 > num2 ) ? num1 : num2; }
int min(int num1, int num2) { return (num1 > num2 ) ? num2 : num1; }

int8_t sign(int x) { return (x > 0) - (x < 0); }
int8_t CLAMP_HID(int value) { return value < -127 ? -127 : value > 127 ? 127 : value; }

//void tap_code_fast(uint8_t code) {
//	register_code(code);
//	// Dont do this:
//	// if (code == KC_CAPS) {
//	//	 wait_ms(TAP_HOLD_CAPS_DELAY);
//	// } else {
//	//	 wait_ms(TAP_CODE_DELAY);
//	// }
//	unregister_code(code);
//}
//
//void tap_tb(uint8_t keycode0, uint8_t keycode1, uint8_t keycode2, uint8_t keycode3) {
//	if(abs(cum_x) + abs(cum_y) >= cur_factor){
//		if(abs(cum_x) > abs(cum_y)) {
//			if(cum_x > 0) {
//				for (int8_t i = 0; i <= (abs(cum_x) + abs(cum_y)) / cur_factor; i++) {
//					tap_code_fast(keycode0);
//					cum_x = max(cum_x - cur_factor, 0);
//				}
//				cum_y = 0;
//			} else {
//				for (int8_t i = 0; i <= (abs(cum_x) + abs(cum_y)) / cur_factor; i++) {
//					tap_code_fast(keycode1);
//					cum_x = min(cum_x + cur_factor, 0);
//				}
//				cum_y = 0;
//			}
//		} else {
//			if(cum_y > 0) {
//				for (int8_t i = 0; i <= (abs(cum_x) + abs(cum_y)) / cur_factor; i++) {
//					tap_code_fast(keycode2);
//					cum_y = max(cum_y - cur_factor, 0);
//					}
//				cum_x = 0;
//			} else {
//				for (int8_t i = 0; i <= (abs(cum_x) + abs(cum_y)) / cur_factor; i++) {
//					tap_code_fast(keycode3);
//					cum_y = min(cum_y + cur_factor, 0);
//				}
//				cum_x = 0;
//			}
//		}
//	}
//}

void handle_pointing_device_modes(void){
	report_mouse_t mouse_report = pointing_device_get_report();
	if (track_mode == cursor_mode) {
		if (integration_mode) {
			cur_factor = cursor_multiplier_inte;
    } else {
			cur_factor = cursor_multiplier; 
    }

		mouse_report.x = CLAMP_HID( sensor_x * cur_factor / 100);
		mouse_report.y = CLAMP_HID(-sensor_y * cur_factor / 100);

      uprintf("sensor_x: %d - ", sensor_x);
      uprintf("sensor_y: %d\n", sensor_y); 
      uprintf("mouse_report.x: %d - ", mouse_report.x);
      uprintf("mouse_report.y: %d\n", mouse_report.y);
	} 
	
	pointing_device_set_report(mouse_report);
	pointing_device_send();
}

void get_sensor_data(void) {
	if(!is_keyboard_master())
		return;
	report_pmw_t pmw_report = pmw_get_report();

	if (integration_mode) {
		sensor_x += pmw_report.x;
		sensor_y += pmw_report.y;
	} else {
		sensor_x = pmw_report.x;
		sensor_y = pmw_report.y;
	}
}

void pointing_device_task(void) {
#ifndef POLLING
	if ( is_keyboard_master() && integration_mode )
		handle_pointing_device_modes();
#else
	get_sensor_data();
	handle_pointing_device_modes();
#endif
//	if(sticky_key && timer_elapsed(sticky_timer) > STICKY_TERM) {
//		//unstick_keys();
//		activate_alt = false;
//		activate_sft = false;
//		activate_ctl = false;
//		activate_gui = false;
//		activate_rai = false;
//		activate_low = false;
//		sticky_key = false;
//	}
}

#ifndef POLLING
	ISR(INT2_vect) {
		get_sensor_data();
		handle_pointing_device_modes();
	}
#endif

///***************************
// * process_record_kb
// **************************/
//
//bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
//	if(!process_record_user(keycode, record)) {
//		return false;
//	}
//
//	switch (keycode) {
//
//		// no repetitive ::: with holding
//		case KC_SCLN_INV:
//			if (record->event.pressed) {
//				if (is_sft_active) {
//					unregister_mods(MOD_BIT(KC_LSFT));
//					tap_code(KC_SCLN);
//					register_mods(MOD_BIT(KC_LSFT));
//				} else {
//					register_mods(MOD_BIT(KC_LSFT));
//					tap_code(KC_SCLN);
//					unregister_mods(MOD_BIT(KC_LSFT));
//				}
//			}
//			return false;
//
//		// ; and : gets confused in some corner cases
//		//case KC_SCLN_INV:
//		//	if (record->event.pressed) {
//		//	if (is_sft_active) {
//		//		unregister_mods(MOD_BIT(KC_LSFT));
//		//		register_code(KC_SCLN);
//		//	} else {
//		//		register_mods(MOD_BIT(KC_LSFT));
//		//		register_code(KC_SCLN);
//		//	}
//		//	} else {
//		//	if (is_sft_active) {
//		//		unregister_code(KC_SCLN);
//		//		register_mods(MOD_BIT(KC_LSFT));
//		//	} else {
//		//		unregister_code(KC_SCLN);
//		//		unregister_mods(MOD_BIT(KC_LSFT));
//		//	}
//		//	}
//		//	return false;
//
//		case KC_TILD_MY:
//			if (record->event.pressed) {
//				tap_code16(KC_TILD);
//				tap_code(KC_SPC);
//			}
//			return false;
//
//		case KC_QUOT_MY:
//			if (record->event.pressed) {
//				tap_code(KC_QUOT);
//				tap_code(KC_SPC);
//			}
//			return false;
//
//		// handle mouse
//		case KC_BTN1:
//			on_mouse_button(MOUSE_BTN1, record->event.pressed);
//			return false;
//
//		case KC_BTN2:
//			on_mouse_button(MOUSE_BTN2, record->event.pressed);
//			return false;
//
//		case KC_BTN3:
//			on_mouse_button(MOUSE_BTN3, record->event.pressed);
//			return false;
//
//		case KC_BTN4:
//			on_mouse_button(MOUSE_BTN4, record->event.pressed);
//			return false;
//
//		case KC_BTN5:
//			on_mouse_button(MOUSE_BTN5, record->event.pressed);
//			return false;
//
//		case KC_CPI_DOWN:
//			if (cursor_multiplier > CPI_STEP)
//				cursor_multiplier = cursor_multiplier - CPI_STEP;
//			return false;
//
//		case KC_CPI_STD:
//			cursor_multiplier = 250;
//			return false;
//
//		case KC_CPI_UP:
//			cursor_multiplier = cursor_multiplier + CPI_STEP;
//			return false;
//
//		case KC_SMO_SC:
//			if (record->event.pressed) {
//				if (smooth_scroll) {
//				scroll_threshold = scroll_threshold * regular_smoothscroll_factor;
//				scroll_threshold_inte = scroll_threshold_inte * regular_smoothscroll_factor;
//				smooth_scroll = false;
//				} else {
//				scroll_threshold = scroll_threshold / regular_smoothscroll_factor;
//				scroll_threshold_inte = scroll_threshold_inte / regular_smoothscroll_factor;
//				smooth_scroll = true;
//				}
//			}
//
//		default:
//			return true;
//	}
//}

// debug
//		char snum[5];
//		itoa(variable, snum, 10);
//		SEND_STRING(" ");
//		send_string(snum);
