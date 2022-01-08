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

int16_t sensor_x = 0;
int16_t sensor_y = 0;

uint16_t cursor_multiplier = 50;	// adjust cursor speed
int16_t cur_factor;

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
  cfg.cpi = 1600;
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
	cur_factor = cursor_multiplier; 

	mouse_report.x = CLAMP_HID( sensor_x * cur_factor / 100);
	mouse_report.y = CLAMP_HID(-sensor_y * cur_factor / 100);

	pointing_device_set_report(mouse_report);
	pointing_device_send();
}

void get_sensor_data(void) {
	if(!is_keyboard_master())
		return;
	report_pmw_t pmw_report = pmw_get_report();

	sensor_x = pmw_report.x;
	sensor_y = pmw_report.y;
}

void pointing_device_task(void) {
	get_sensor_data();
	handle_pointing_device_modes();
}
