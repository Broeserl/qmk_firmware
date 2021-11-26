#include "keymap_german.h"
#include QMK_KEYBOARD_H


// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _DEFAULT 0

// Some basic macros
#define TASK   LCTL(LSFT(KC_ESC))
#define TAB_R  LCTL(KC_TAB)
#define TAB_L  LCTL(LSFT(KC_TAB))
#define TAB_RO LCTL(LSFT(KC_T))

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_DEFAULT] = LAYOUT_5x7(
        // left hand
        KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,
        KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B, 
        KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,
        KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B, 
        KC_B,    KC_B,    KC_B,    KC_B,
                                KC_B,    KC_B,
                                KC_B,    KC_B,
                                KC_B,    KC_B,
        // right hand
                          KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,
                          KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,
                          KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,
                                   KC_B,    KC_B,    KC_B,    KC_B,    KC_B,    KC_B,
                                                     KC_B,    KC_B,    KC_B,    KC_B,
             KC_B, KC_B, 
             KC_B, KC_B,
             KC_B, KC_B 
    )
};
