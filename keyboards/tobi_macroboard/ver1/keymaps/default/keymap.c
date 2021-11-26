#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_4x5(
        KC_TAB,   KC_9,   KC_8,   KC_7,   KC_BSPACE,
        KC_ESC,   KC_6,   KC_5,   KC_4,   KC_F3,
        S(KC_N),  KC_3,   KC_2,   KC_1,   KC_ENT,
        KC_LCTL,  KC_SLSH,   KC_COMMA,   KC_0,  MO(1)),
    [1] = LAYOUT_ortho_4x5(
        RESET,    KC_MUTE,  KC_UP,    KC_MPRV,    KC_MNXT,
        KC_DEL,   KC_LEFT,  KC_DOWN,  KC_RIGHT,   KC_MPLY,
        KC_VOLU,  C(KC_X),  C(KC_C),  C(KC_P),    KC_ENTER,
        KC_VOLD,  KC_RBRC,  S(KC_DOT),  ALGR(KC_Q),    MO(0))
};
