/*
Copyright 2013 Oleg Kostyuk <cub.uanic@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <util/delay.h>
#include "bootloader.h"
#include "keymap_common.h"

// Aliases for shifted symbols
// Names from QMK
#define KC_TILD KC_FN11
#define KC_EXLM KC_FN12
#define KC_AT   KC_FN13
#define KC_HASH KC_FN14
#define KC_DLR  KC_FN15
#define KC_PERC KC_FN16
#define KC_CIRC KC_FN17
#define KC_AMPR KC_FN18
#define KC_ASTR KC_FN19
#define KC_LPRN KC_FN20
#define KC_RPRN KC_FN21
#define KC_UNDS KC_FN22
#define KC_PLUS KC_FN23
#define KC_LCBR KC_FN24
#define KC_RCBR KC_FN25
#define KC_LABK KC_FN26
#define KC_RABK KC_FN27
#define KC_COLN KC_FN28
#define KC_PIPE KC_FN29
#define KC_QUES KC_FN30
#define KC_DQT  KC_FN31

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    KEYMAP(  // layer 0 : default
        // left hand
        ESC, 1,   2,   3,   4,   5,   FN0,
        TAB, Q,   W,   E,   R,   T,   NUBS,
        CAPS,A,   S,   D,   F,   G,
        LSFT,Z,   X,   C,   V,   B,   DEL,
        FN3, FN4, GRV, BSLS,LALT,
                                      LGUI,FN1,
                                           LSFT,
                                 BSPC,LCTL, FN6,
        // right hand
             EQL, 6,   7,   8,   9,   0,   MINS,
             RBRC,Y,   U,   I,   O,   P,   LBRC,
                  H,   J,   K,   L,   SCLN,QUOT,
             FN15,N,   M,   COMM,DOT, SLSH,RSFT,
                       RALT,LEFT,UP,  DOWN,RGHT,
        HOME,END,
        PGUP,
        PGDN,ENT, SPC
    ),

    KEYMAP(  // layer 1 : function and symbol keys
        // left hand
        FN5, TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,F1,  F2,  F3,  F4,  TRNS,TRNS,
        TRNS,F5,  F6,  F7,  F8  ,TRNS,
        TRNS,F9,  F10, F11, F12 ,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,
                                      TRNS,TRNS,
                                           TRNS,
                                 TRNS,TRNS,TRNS,
        // right hand
             MUTE,TRNS,TRNS,FN7, FN8, TRNS,TRNS,
             VOLU,TRNS,TRNS,FN9, FN10,TRNS,TRNS,
                  TRNS,TRNS,FN11,FN12,TRNS,TRNS,
             VOLD,TRNS,TRNS,FN13,FN14,TRNS,TRNS,
                       TRNS,TRNS,TRNS,TRNS,TRNS,
        MPRV,MNXT,
        MPLY,
        MPLY,TRNS,TRNS
    ),

    KEYMAP(  // layer 2 : keyboard functions
        // left hand
        FN5, TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,
                                      TRNS,TRNS,
                                           TRNS,
                                 TRNS,TRNS,TRNS,
        // right hand
             TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                  TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                       TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,
        TRNS,
        TRNS,TRNS,TRNS
    ),

    KEYMAP(  // layer 3: numpad
        // left hand
        FN5, TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,
                                      TRNS,TRNS,
                                           TRNS,
                                 TRNS,TRNS,TRNS,
        // right hand
             TRNS,NLCK,PSLS,PAST,PAST,PMNS,BSPC,
             TRNS,NO,  P7,  P8,  P9,  PMNS,BSPC,
                  NO,  P4,  P5,  P6,  PPLS,PENT,
             TRNS,NO,  P1,  P2,  P3,  PPLS,PENT,
                       P0,  PDOT,SLSH,PENT,PENT,
        TRNS,TRNS,
        TRNS,
        TRNS,TRNS,TRNS
    ),

};

/* id for user defined functions */
enum function_id {
    TEENSY_KEY,
};

/*
 * Fn action definition
 */
const action_t PROGMEM fn_actions[] = {
    ACTION_FUNCTION(TEENSY_KEY),                    // FN0 - Teensy key
    ACTION_LAYER_MOMENTARY(1),                      // FN1 - switch to Layer1
    ACTION_LAYER_MOMENTARY(2),                     // FN2 - set Layer2
    // ACTION_LAYER_SET(2, ON_PRESS),                  // FN2 - set Layer2
    ACTION_LAYER_MOMENTARY(3),                     // FN3 - switch Layer3 aka Numpad layer
    // ACTION_LAYER_TOGGLE(3),                         // FN3 - toggle Layer3 aka Numpad layer
    ACTION_LAYER_TOGGLE(3),                     // FN4 - toggle Layer3 aka Numpad layer
    // ACTION_LAYER_SET(0, ON_PRESS),                  // FN4 - set Layer0
    ACTION_LAYER_SET(0, ON_PRESS),                  // FN5 - set Layer0
    ACTION_MODS(MOD_LCTL | MOD_LALT),               // FN6 - CTRL + ALT
    ACTION_MODS_KEY(MOD_LSFT, KC_8),                   // FN7 - (
    ACTION_MODS_KEY(MOD_LSFT, KC_9),                   // FN8 - )
    ACTION_MODS_KEY(MOD_RALT, KC_LBRC),                // FN9 - [
    ACTION_MODS_KEY(MOD_RALT, KC_RBRC),                // FN10 - ]
    ACTION_MODS_KEY(MOD_RALT, KC_QUOT),                // FN11 - {
    ACTION_MODS_KEY(MOD_RALT, KC_BSLS),                // FN12 - } 
    ACTION_KEY(KC_NUBS),                               // FN13 - <
    ACTION_MODS_KEY(MOD_LSFT, KC_NUBS),                // FN14 - >
    ACTION_MODS_KEY(MOD_LSFT, KC_7),                   // FN15 - /
    // // Shifted keys
    // ACTION_MODS_KEY(MOD_LSFT, KC_GRV),              // FN11 - ~
    // ACTION_MODS_KEY(MOD_LSFT, KC_1),                // FN12 - !
    // ACTION_MODS_KEY(MOD_LSFT, KC_2),                // FN13 - @
    // ACTION_MODS_KEY(MOD_LSFT, KC_3),                // FN14 - #
    // ACTION_MODS_KEY(MOD_LSFT, KC_4),                // FN15 - $
    // ACTION_MODS_KEY(MOD_LSFT, KC_5),                // FN16 - %
    // ACTION_MODS_KEY(MOD_LSFT, KC_6),                // FN17 - ^
    // ACTION_MODS_KEY(MOD_LSFT, KC_7),                // FN18 - &
    // ACTION_MODS_KEY(MOD_LSFT, KC_8),                // FN19 - *
    // ACTION_MODS_KEY(MOD_LSFT, KC_9),                // FN20 - (
    // ACTION_MODS_KEY(MOD_LSFT, KC_0),                // FN21 - )
    // ACTION_MODS_KEY(MOD_LSFT, KC_MINS),             // FN22 - _
    // ACTION_MODS_KEY(MOD_LSFT, KC_EQL),              // FN23 - +
    // ACTION_MODS_KEY(MOD_LSFT, KC_LBRC),             // FN24 - {
    // ACTION_MODS_KEY(MOD_LSFT, KC_RBRC),             // FN25 - }
    // ACTION_MODS_KEY(MOD_LSFT, KC_COMM),             // FN26 - <
    // ACTION_MODS_KEY(MOD_LSFT, KC_DOT),              // FN27 - >
    // ACTION_MODS_KEY(MOD_LSFT, KC_SCLN),             // FN28 - :
    // ACTION_MODS_KEY(MOD_LSFT, KC_BSLS),             // FN29 - |
    // ACTION_MODS_KEY(MOD_LSFT, KC_SLSH),             // FN30 - ?
    // ACTION_MODS_KEY(MOD_LSFT , KC_QUOT),             // FN31 - "
};

void action_function(keyrecord_t *event, uint8_t id, uint8_t opt)
{
    if (id == TEENSY_KEY) {
        clear_keyboard();
        print("\n\nJump to bootloader... ");
        _delay_ms(250);
        bootloader_jump(); // should not return
        print("not supported.\n");
    }
}
