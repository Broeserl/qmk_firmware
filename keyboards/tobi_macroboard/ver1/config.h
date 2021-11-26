#pragma once

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID       0x6666
#define PRODUCT_ID      0x6667
#define DEVICE_VER      0x0001
#define MANUFACTURER    Tobi
#define PRODUCT         Macroboard ver1

/* key matrix size */
#define MATRIX_ROWS 4
#define MATRIX_COLS 5

/* pin-out */
#define MATRIX_ROW_PINS { B1, B3, B2, B6 }
#define MATRIX_COL_PINS { C6, D7, E6, B4, B5 }
#define UNUSED_PINS

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION COL2ROW
