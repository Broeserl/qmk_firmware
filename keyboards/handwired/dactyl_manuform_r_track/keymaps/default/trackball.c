/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2020 Ploopy Corporation
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "trackball.h"

keyboard_config_t keyboard_config;
uint16_t          dpi_array[] = PLOOPY_DPI_OPTIONS;
#define DPI_OPTION_SIZE (sizeof(dpi_array) / sizeof(uint16_t))

__attribute__((weak)) void process_mouse_user(report_mouse_t* mouse_report, int16_t x, int16_t y) {
    mouse_report->x = x;
    mouse_report->y = y;
}

__attribute__((weak)) void process_mouse(report_mouse_t* mouse_report) {
    report_pmw_t data = pmw_read_burst();
    if (data.isOnSurface && data.isMotion) {
        // Reset timer if stopped moving

        // Wrap to HID size
        data.dx = constrain(data.dx, -127, 127);
        data.dy = constrain(data.dy, -127, 127);

        process_mouse_user(mouse_report, data.dx, -data.dy);
    }
}

// Hardware Setup
void keyboard_pre_init_kb(void) {
    debug_enable  = true;
    debug_mouse   = true;

    setPinInput(OPT_ENC1);
    setPinInput(OPT_ENC2);

    /* Ground all output pins connected to ground. This provides additional
     * pathways to ground. If you're messing with this, know this: driving ANY
     * of these pins high will cause a short. On the MCU. Ka-blooey.
     */
    keyboard_pre_init_user();
}

void pointing_device_init(void) {
    // initialize ball sensor
    pmw_spi_init();
    // initialize the scroll wheel's optical encoder
    opt_encoder_init();
}


void pointing_device_task(void) {
    report_mouse_t mouse_report = pointing_device_get_report();
    process_mouse(&mouse_report);
    pointing_device_set_report(mouse_report);
    pointing_device_send();
}

void eeconfig_init_kb(void) {
    keyboard_config.dpi_config = PLOOPY_DPI_DEFAULT;
    eeconfig_update_kb(keyboard_config.raw);
    eeconfig_init_user();
}

void matrix_init_kb(void) {
    // is safe to just read DPI setting since matrix init
    // comes before pointing device init.
    keyboard_config.raw = eeconfig_read_kb();
    if (keyboard_config.dpi_config > DPI_OPTION_SIZE) {
        eeconfig_init_kb();
    }
    matrix_init_user();
}

void keyboard_post_init_kb(void) {
    pmw_set_cpi(dpi_array[keyboard_config.dpi_config]);

    keyboard_post_init_user();
}
