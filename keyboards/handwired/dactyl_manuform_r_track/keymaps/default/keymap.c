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

#define _DEFAULT 0
#define _SPECIAL_KEYS  1
#define _MOVEMENT 2

#include "keymap_german.h"

/***************************
 * Trackball related defines
 **************************/

#include "quantum.h"
#include "pointing_device.h"
#include "../../pmw3360/pmw3360.h"

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

uint16_t cursor_multiplier = 250;	// adjust cursor speed
uint16_t cursor_multiplier_inte = 20;
#define CPI_STEP 20

int16_t cur_factor;

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

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_DEFAULT] = LAYOUT_5x6(
KC_ESC,  KC_1,    KC_2,    KC_3,   KC_4,   KC_5,            KC_6,    KC_7,    KC_8,     KC_9,     KC_0,     KC_MINS,
KC_TAB,  KC_Q,    KC_W,    KC_E,   KC_R,   KC_T,            KC_Y,    KC_U,    KC_I,     KC_O,     KC_P,     KC_LBRC,
KC_NO,   KC_A,    KC_S,    KC_D,   KC_F,   KC_G,            KC_H,    KC_J,    KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
KC_LSFT, KC_Z,    KC_X,    KC_C,   KC_V,   KC_B,            KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,
KC_LCTL, KC_NUBS, KC_NUHS, KC_LGUI,                                           KC_LEFT,  KC_DOWN,  KC_UP,    KC_RGHT,
                                  KC_BSPC, KC_DEL,
                                  TG(2),   KC_LALT,        KC_SPC,  KC_ENT,
                                  OSL(1),  KC_LCTL,        OSL(1),  KC_RCTL
),

[_SPECIAL_KEYS] = LAYOUT_5x6(
KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,           KC_F6,    KC_F7,  KC_F8,   KC_F9,    KC_F10,   KC_F11,
S(DE_7),  S(DE_1),  DE_AT,    DE_LCBR,  DE_RCBR,  DE_PIPE,         KC_NO,    KC_7,   KC_8,    KC_9,     DE_ASTR,  KC_F12,
DE_BSLS,  DE_HASH,  S(DE_4),  S(DE_8),  S(DE_9),  DE_CIRC,         KC_NO,    KC_4,   KC_5,    KC_6,     DE_PLUS,  KC_NO,
DE_GRV,   DE_LABK,  DE_RABK,  DE_LBRC,  DE_RBRC,  DE_TILD,         DE_AMPR,  KC_1,   KC_2,    KC_3,     DE_SLSH,  KC_NO,
KC_NO,    KC_NO,    KC_NO,    KC_NO,                                                 KC_0,    DE_COMM,  DE_EQL,   KC_NO,
                                  KC_NO,   KC_NO,
                                  KC_NO,   KC_NO,                 KC_NO,  KC_NO,
                                  KC_NO,   KC_NO,                 RESET,  KC_NO
),
[_MOVEMENT] = LAYOUT_5x6(
KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,                 KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,  KC_NO,
KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_WH_U,               KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,  KC_NO,
KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_WH_D,               KC_NO,  KC_BTN1,  KC_BTN3,  KC_BTN2,  KC_NO,  KC_NO,
KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,                 KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,  KC_NO,
KC_NO,  KC_NO,  KC_NO,  KC_NO,                                                   KC_NO,    KC_NO,    KC_NO,  KC_NO,
                                  KC_PGDOWN,  KC_PGUP,
                                  TG(2),      KC_NO,       KC_NO,  KC_NO,
                                  KC_NO,      KC_NO,       KC_NO,  KC_RCTL

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
	} else {
		// accumulate movement until threshold reached
		cum_x += sensor_x;
		cum_y += sensor_y;
		if (track_mode == carret_mode) {
			if (integration_mode)
				cur_factor = carret_threshold_inte;
			else
				cur_factor = carret_threshold;
			//tap_tb(KC_RIGHT, KC_LEFT, KC_UP, KC_DOWN);

		} else if(track_mode == scroll_mode) {
				if (integration_mode)
					cur_factor = scroll_threshold_inte;
				else
					cur_factor = scroll_threshold;
				if(abs(cum_x) + abs(cum_y) >= cur_factor) {
					if(abs(cum_x) > abs(cum_y)) {
						mouse_report.h = sign(cum_x) * (abs(cum_x) + abs(cum_y)) / cur_factor;
					} else {
						mouse_report.v = sign(cum_y) * (abs(cum_x) + abs(cum_y)) / cur_factor;
					}
					cum_x = 0;
					cum_y = 0;
				}
		} else { // sound vol/brightness (3)
			cur_factor = carret_threshold;
			//tap_tb(KC_BRIGHTNESS_UP, KC_BRIGHTNESS_DOWN, KC_AUDIO_VOL_UP, KC_AUDIO_VOL_DOWN);
		}
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
	get_sensor_data();
	handle_pointing_device_modes();
}
