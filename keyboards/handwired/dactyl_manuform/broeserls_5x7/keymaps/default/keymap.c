#include "keymap_german.h"
#include QMK_KEYBOARD_H


// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _DEFAULT 0
#define _SPECIAL_KEYS  1
#define _MOVEMENT 2

// Some basic macros
#define TASK   LCTL(LSFT(KC_ESC))
#define TAB_R  LCTL(KC_TAB)
#define TAB_L  LCTL(LSFT(KC_TAB))
#define TAB_RO LCTL(LSFT(KC_T))

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_DEFAULT] = LAYOUT_5x7(
        // left hand
        KC_ESC,       KC_1,    KC_2,    KC_3,   KC_4,   KC_5,   _______,
        KC_TAB,       KC_Q,    KC_W,    KC_E,   KC_R,   KC_T,   _______, 
        _______,      KC_A,    KC_S,    KC_D,   KC_F,   KC_G,   OSL(1),
        OSM(MOD_LSFT),KC_Z,    KC_X,    KC_C,   KC_V,   KC_B,
        KC_LCTL,      KC_NUBS, KC_NUHS, KC_LGUI,
                                    KC_BSPC, KC_DEL,
                                    _______, KC_LALT,
                                    TG(2),   _______,
        // right hand
                          _______, KC_6,    KC_7,    KC_8,     KC_9,     KC_0,     KC_MINS,
                          _______, KC_Y,    KC_U,    KC_I,     KC_O,     KC_P,     KC_LBRC,
                          OSL(1),  KC_H,    KC_J,    KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
                                   KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH,  OSM(MOD_RSFT),
                                                     KC_LEFT,  KC_DOWN,  KC_UP,    KC_RGHT,
             KC_ENT,  KC_SPC,
             KC_RALT, _______,
             KC_RCTL, _______
    ),

    [_SPECIAL_KEYS] = LAYOUT_5x7(
        // left hand
        KC_ESC,    KC_F1,     KC_F2,      KC_F3,    KC_F4,     KC_F5,    _______,
        S(DE_7),   S(DE_1),   DE_AT,     DE_LCBR,   DE_RCBR,   DE_PIPE,  _______,
        DE_BSLS,   DE_HASH,   S(DE_4),   S(DE_8),   S(DE_9),   DE_CIRC,  _______,
        _______,   DE_LABK,   DE_RABK,   DE_LBRC,   DE_RBRC,   DE_TILD,
        _______,   _______,   _______,   _______,
                                    _______, _______,
                                    _______, _______,
                                    _______, _______,
        // right hand
                          _______,   KC_F6,     KC_F7,     KC_F8,     KC_F9,     KC_F10,    KC_F11,
                          _______,   _______,   KC_7,      KC_8,      KC_9,      DE_ASTR,   KC_F12,
                          _______,   _______,   KC_4,      KC_5,      KC_6,      DE_PLUS,   _______,
                                     DE_AMPR,   KC_1,      KC_2,      KC_3,      DE_SLSH,   _______,
                                                           KC_0,      DE_COMM,   DE_EQL,    _______,
             _______, _______,
             _______, _______,
             _______, _______
    ),

    [_MOVEMENT] = LAYOUT_5x7(
        // left hand
        _______,   _______,   _______,   _______,   _______,   _______,  _______,
        _______,   _______,   _______,   KC_MS_U,   _______,   KC_WH_U,  KC_LGUI,
        _______,   _______,   KC_MS_L,   KC_MS_D,   KC_MS_R,   KC_WH_D,  KC_LSHIFT,
        _______,   _______,   _______,   _______,   _______,   KC_LCTL,
        _______,   _______,   _______,   _______,
                                    KC_BTN1, KC_BTN2,
                                    KC_BTN3, _______,
                                    TG(2)  , _______,
        // right hand
                          _______,   _______,   _______,   _______,   _______,   _______,   _______,
                          _______,   _______,   _______,   _______,   _______,   _______,   _______,
                          _______,   KC_LEFT,   KC_DOWN,   KC_UP,     KC_RIGHT,  _______,   _______,
                                     _______,   _______,   _______,   _______,   _______,   _______,
                                                           _______,   _______,   _______,   _______,
             KC_ENT,  KC_SPC,
             KC_PGUP, _______,
             KC_PGDOWN, _______
    ),
};
