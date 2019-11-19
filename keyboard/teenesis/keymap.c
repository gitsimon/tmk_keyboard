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
#include <stdint.h>
#include <stdbool.h>
#include "bootloader.h"
#include "keycode.h"
#include "action.h"
#include "action_code.h"
#include "action_layer.h"
#include "action_macro.h"
#include "action_util.h"
#include "report.h"
#include "host.h"
#include "print.h"
#include "debug.h"
#include "keymap.h"

/*
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "bootloader.h"
*/


/* pinout
## columns
F0 - PL2/6    PL1/1
F1 - PL2/7    PL1/2
F4 - PL2/8    PL1/3
F5 - PL2/10   PL1/8
F6 - PL2/11
F7 - PL2/12
D0 - PR2/1    PR1/3
D1 - PR2/3    PR1/4
D2 - PR2/2    PR1/5
D3 - PR2/12   PR1/6
D4 - PR2/13
D5 - PR2/6
D6 - 
C6 - 
C7 - 

## rows
B0 - PL2/13   PR2/10
B1 - PL2/5    PR2/9
B2 - PL2/3    PR2/8
B3 - PL2/2    PR2/7
B4 - PL2/1    PR2/4

B5 - PL1/4    PR1/7
B6 - PL1/5    PR1/8
*/

/* Teenesis keymap definition macro */
#define KEYMAP(                                                                         \
    /* functional left */                                /* functional right */         \
    fl0,fl1,fl2,fl3,fl4,fl5,fl6,fl7,fl8,   fr8,fr7,fr6,fr5,fr4,fr3,fr2,fr1,fr0,         \
                                                                                        \
    /* left hand, spatial positions */      /* right hand, spatial positions */         \
    k00,k01,k02,k03,k04,k05,                           k06,k07,k08,k09,k0A,k0B,         \
    k10,k11,k12,k13,k14,k15,                           k16,k17,k18,k19,k1A,k1B,         \
    k20,k21,k22,k23,k24,k25,                           k26,k27,k28,k29,k2A,k2B,         \
    k30,k31,k32,k33,k34,k35,                           k36,k37,k38,k39,k3A,k3B,         \
        k41,k42,k43,k44,                                   k47,k48,k49,k4A,             \
                            k52,k62,           k59,k68,                                 \
                                k63,           k58,                                     \
                        k53,k50,k61,           k56,k67,k66 )                            \
                                                                                        \
                                                                                        \
   /* matrix positions, k40 & k4B unused for now, k45 & k46 are untraced on plate */    \
   /* pins */                                                                           \
   /* 0: B0    1: B1    2: B2    3: B3    4: B4      5: B5    6: B6                */   \
   {                                                                                    \
   /* Left hand                                      Left thumb                    */   \
    { KC_##k00,KC_##k10,KC_##k20,KC_##k30,KC_NO   ,  KC_##k50,KC_NO   },  /* 0: F0 */   \
    { KC_##k01,KC_##k11,KC_##k21,KC_##k31,KC_##k41,  KC_NO   ,KC_##k61},  /* 1: F1 */   \
    { KC_##k02,KC_##k12,KC_##k22,KC_##k32,KC_##k42,  KC_##k52,KC_##k62},  /* 2: F4 */   \
    { KC_##k03,KC_##k13,KC_##k23,KC_##k33,KC_##k43,  KC_##k53,KC_##k63},  /* 3: F5 */   \
    { KC_##k04,KC_##k14,KC_##k24,KC_##k34,KC_NO   ,  KC_NO   ,KC_NO   },  /* 4: F6 */   \
    { KC_##k05,KC_##k15,KC_##k25,KC_##k35,KC_##k44,  KC_NO   ,KC_NO   },  /* 5: F7 */   \
   /* Right hand                                     Right thumb                   */   \
    { KC_##k06,KC_##k16,KC_##k26,KC_##k36,KC_##k47,  KC_##k56,KC_##k66},  /* 0: D0 */   \
    { KC_##k07,KC_##k17,KC_##k27,KC_##k37,KC_NO   ,  KC_NO   ,KC_##k67},  /* 2: D1 */   \
    { KC_##k08,KC_##k18,KC_##k28,KC_##k38,KC_##k48,  KC_##k58,KC_##k68},  /* 1: D2 */   \
    { KC_##k09,KC_##k19,KC_##k29,KC_##k39,KC_##k49,  KC_##k59,KC_NO   },  /* 4: D3 */   \
    { KC_##k0A,KC_##k1A,KC_##k2A,KC_##k3A,KC_##k4A,  KC_NO   ,KC_NO   },  /* 5: D4 */   \
    { KC_##k0B,KC_##k1B,KC_##k2B,KC_##k3B,KC_NO   ,  KC_NO   ,KC_NO   },  /* 3: D5 */   \
   /* Left functional                     Right functional                         */   \
    { KC_##fl0,KC_##fl3,KC_##fl6,KC_NO   ,KC_##fr2,  KC_##fr8,KC_##fr5},  /* 0: D6 */   \
    { KC_##fl1,KC_##fl4,KC_##fl7,KC_NO   ,KC_##fr0,  KC_##fr7,KC_##fr4},  /* 1: C6 */   \
    { KC_##fl2,KC_##fl5,KC_##fl8,KC_NO   ,KC_##fr1,  KC_##fr6,KC_##fr3}   /* 2: C7 */   \
   }



static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    KEYMAP(  // Layer 0 : default
        ESC ,F1  ,F2  ,F3  ,F4  ,F5  ,F6  ,F7  ,F8  ,       F9  ,F10 ,F11 ,F12 ,PSCR,SLCK,PAUS,NLCK,EXEC,
    //                ;    !    #    {    }                      [    ]    *    (    )    =
                 GRV ,1   ,2   ,3   ,4   ,FN25,                  FN26,7   ,8   ,9   ,0   ,EQL ,
                 TAB ,Q   ,W   ,E   ,R   ,T   ,                  Y   ,U   ,I   ,O   ,P   ,LBRC,
                 FN10,FN21,FN22,FN23,FN24,G   ,                  H   ,J   ,K   ,L   ,SCLN,FN11,
                 FN9 ,Z   ,X   ,C   ,V   ,B   ,                  N   ,M   ,COMM,DOT ,SLSH,RBRC,
                      CAPS,SLCK,HOME,END ,                            LEFT,UP  ,DOWN,RGHT,
                                          NO  ,FN31,        PGUP,NO  ,
                                               FN12,        FN13,
                                     FN14,FN15,FN16,        FN17,FN18,FN19
    ),

    KEYMAP(  // Layer1: unconvenient keys on right hand
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,       TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,  // in Workman right hand will be:
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  FN0 ,FN1 ,FN2 ,9   ,0   ,EQL ,            //    | { } ( ) =
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  7   ,MINS,FN5 ,LBRC,RBRC,PPLS,            //    & - > [ ] +
                 NO  ,TRNS,NO  ,NO  ,NO  ,NO  ,                  QUOT,1   ,4   ,FN3 ,FN6 ,P   ,            //    ' ! $ " : ;
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  3   ,6   ,FN4 ,FN5 ,5   ,8   ,            //    # ^ < > % *
                 /*
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  FN1 ,FN2 ,9   ,8   ,0   ,PPLS,            //    { } ( * ) +
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  7   ,6   ,5   ,LBRC,RBRC,EQL ,            //    & ^ % [ ] =
                 NO  ,NO  ,NO  ,NO  ,TRNS,NO  ,                  QUOT,1   ,4   ,FN3 ,FN6 ,P   ,            //    ' ! $ " : ;
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  3   ,LBRC,FN4 ,FN5 ,RBRC,FN0 ,            //    # [ < > ] |
                 */
                      NO  ,NO  ,NO  ,NO  ,                            NO  ,NO  ,NO  ,NO  ,
                                          TRNS,TRNS,        TRNS,TRNS,
                                               TRNS,        TRNS,
                                     TRNS,TRNS,TRNS,        TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer2: F-keys + PgUp/PgDn on right hand
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,       TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                 FN30,NO  ,NO  ,NO  ,NO  ,NO  ,                  NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  NO  ,F1  ,F2  ,F3  ,F4  ,PGUP,
                 NO  ,NO  ,TRNS,NO  ,NO  ,NO  ,                  NO  ,F5  ,F6  ,F7  ,F8  ,PGDN,
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  NO  ,F9  ,F10 ,F11 ,F12 ,APP ,
                      NO  ,NO  ,NO  ,NO  ,                            HOME,END ,PGDN,PGUP,
                                          TRNS,TRNS,        TRNS,TRNS,
                                               TRNS,        TRNS,
                                     TRNS,TRNS,TRNS,        TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer3: mouse and navigation
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,       TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  MPLY,MPRV,MNXT,VOLD,VOLU,MUTE,
                 NO  ,NO  ,NO  ,NO  ,ACL0,NO  ,                  BTN2,WH_L,WH_U,WH_D,WH_R,PGUP,
                 NO  ,NO  ,NO  ,TRNS,ACL1,NO  ,                  BTN1,MS_L,MS_U,MS_D,MS_R,PGDN,
                 NO  ,NO  ,NO  ,NO  ,ACL2,NO  ,                  BTN3,HOME,END ,DEL ,INS ,NO  ,
                      NO  ,NO  ,NO  ,NO  ,                            NO  ,NO  ,NO  ,NO  ,
                                          TRNS,TRNS,        TRNS,TRNS,
                                               TRNS,        TRNS,
                                     TRNS,TRNS,TRNS,        TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer4: numpad
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,       TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  NLCK,PSLS,PAST,PAST,PMNS,BSPC,
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  NO  ,P7  ,P8  ,P9  ,PMNS,PGUP,
                 NO  ,NO  ,NO  ,NO  ,TRNS,NO  ,                  NO  ,P4  ,P5  ,P6  ,PPLS,PGDN,
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  NO  ,P1  ,P2  ,P3  ,PPLS,PENT,
                      NO  ,NO  ,NO  ,NO  ,                            P0  ,PDOT,SLSH,PENT,
                                          TRNS,TRNS,        TRNS,TRNS,
                                               TRNS,        TRNS,
                                     TRNS,TRNS,TRNS,        TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer5: Macros, media and full F-keys
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,       TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                 FN1 ,NO  ,NO  ,NO  ,NO  ,TRNS,                  TRNS,MUTE,VOLD,VOLU,NO  ,FN3 ,
                 FN2 ,F1  ,F2  ,F3  ,F4  ,NO  ,                  NO  ,F13 ,F14 ,F15 ,F16 ,FN4 ,
                 NO  ,F5  ,F6  ,F7  ,F8  ,NO  ,                  NO  ,F17 ,F18 ,F19 ,F20 ,NO  ,
                 NO  ,F9  ,F10 ,F11 ,F12 ,NO  ,                  NO  ,F21 ,F22 ,F23 ,F24 ,NO  ,
                      NO  ,NO  ,NO  ,NO  ,                            NO  ,NO  ,NO  ,NO  ,
                                          TRNS,TRNS,        TRNS,FN6 ,
                                               TRNS,        TRNS,
                                     TRNS,TRNS,TRNS,        SLEP,TRNS,FN30
    ),

    KEYMAP(  // Layer6: empty - reserved
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,       TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
                 NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,                  NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
                      NO  ,NO  ,NO  ,NO  ,                            NO  ,NO  ,NO  ,NO  ,
                                          TRNS,TRNS,        TRNS,TRNS,
                                               TRNS,        TRNS,
                                     TRNS,TRNS,TRNS,        TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer7 : games
        ESC ,F1  ,F2  ,F3  ,F4  ,F5  ,F6  ,F7  ,F8  ,       F9  ,F10 ,F11 ,F12 ,PSCR,SLCK,PAUS,NLCK,EXEC,
    //                ;    !    #    {    }                      [    ]    *    (    )    =
                 NO  ,1   ,2   ,3   ,4   ,5   ,                  6   ,7   ,8   ,9   ,0   ,EQL ,
                 TAB ,Q   ,W   ,E   ,R   ,T   ,                  Y   ,U   ,I   ,O   ,P   ,BSPC,
                 LSFT,A   ,S   ,D   ,F   ,G   ,                  H   ,J   ,K   ,L   ,SCLN,RSFT,
                 LSFT,Z   ,X   ,C   ,V   ,B   ,                  N   ,M   ,COMM,DOT ,SLSH,BSLS,
                      CAPS,SLCK,HOME,END ,                            LEFT,UP  ,DOWN,RGHT,
                                          NO  ,FN31,        PGUP,TRNS,
                                               LALT,        RALT,
                                     LCTL,DEL ,ESC ,        RGUI,ENT ,SPC
    ),

    /*

    KEYMAP(  // layer 0 : default
        ESC ,F1  ,F2  ,F3  ,F4  ,F5  ,F6  ,F7  ,F8  ,       F9  ,F10 ,F11 ,F12 ,PSCR,SLCK,PAUS,NLCK,EXEC,
    //                ;    !    #    {    }                      [    ]    *    (    )    =
                 BSLS,1   ,2   ,3   ,4   ,FN20,                  FN21,7   ,8   ,9   ,0   ,EQL ,
                 FN10,Q   ,W   ,E   ,R   ,T   ,                  Y   ,U   ,I   ,O   ,P   ,LBRC,
                 FN11,FN13,FN14,FN15,FN16,G   ,                  H   ,J   ,K   ,L   ,SCLN,FN12,
                 GRV ,Z   ,X   ,C   ,FN17,B   ,                  N   ,M   ,COMM,DOT ,SLSH,RBRC,
                      CAPS,FN1 ,HOME,END ,                            LEFT,UP  ,DOWN,RGHT,
                                          FN18,FN31,        FN0 ,FN19,
                                               FN23,        FN24,
                                     FN25,FN26,FN27,        FN28,FN29,FN30
    ),

    KEYMAP(  // Layer1: Workman layout
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,Q   ,D   ,R   ,W   ,B   ,               J   ,F   ,U   ,P   ,4   ,TRNS,
        TRNS,A   ,S   ,H   ,T   ,G   ,               Y   ,N   ,E   ,O   ,I   ,TRNS,
        TRNS,Z   ,X   ,M   ,C   ,V   ,               K   ,L   ,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,                         TRNS,TRNS,TRNS,TRNS,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer2: layer to lock other layers - TODO
        FN31,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,FN31,TRNS,TRNS,                         TRNS,TRNS,TRNS,TRNS,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer3: unconvenient keys on right hand
                                                    // in Workman right hand will be:
                                                    //      { } ( ) +
                                                    //    ? ^ % ( ) =
                                                    //    ' ! $ " ; \
                                                    //    # [ < > ] \
        FN31,NO  ,NO  ,NO  ,NO  ,NO  ,               NO  ,4   ,5   ,9   ,0   ,PPLS,
        NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,               FN5, MINS,FN6, 9   ,0   ,EQL ,
        NO  ,TRNS,NO  ,NO  ,NO  ,NO  ,               BSLS,2   ,P   ,FN1 ,1   ,FN2 ,
        NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,               3   ,6   ,FN3 ,FN4 ,7   ,FN2 ,
             NO  ,FN31,NO  ,NO  ,                         NO  ,NO  ,NO  ,NO  ,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer4: F-keys + PgUp/PgDn on right hand
        FN31,NO  ,NO  ,NO  ,NO  ,NO  ,               NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
        NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,               NO  ,F1  ,F2  ,F3  ,F4  ,PGUP,
        NO  ,NO  ,TRNS,NO  ,NO  ,NO  ,               NO  ,F5  ,F6  ,F7  ,F8  ,PGDN,
        NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,               NO  ,F9  ,F10 ,F11 ,F12 ,APP ,
             NO  ,FN31,NO  ,NO  ,                         NO  ,NO  ,NO  ,NO  ,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer5: mouse and navigation
        // left hand
        FN31,NO  ,NO  ,NO  ,NO  ,NO  ,               MPLY,MPRV,MNXT,VOLD,VOLU,MUTE,
        NO  ,NO  ,NO  ,NO  ,ACL0,NO  ,               BTN2,WH_L,WH_U,WH_D,WH_R,PGUP,
        NO  ,NO  ,NO  ,TRNS,ACL1,NO  ,               BTN1,MS_L,MS_U,MS_D,MS_R,PGDN,
        NO  ,NO  ,NO  ,NO  ,ACL2,NO  ,               BTN3,HOME,END ,DEL ,INS ,NO  ,
             NO  ,FN31,NO  ,NO  ,                         NO  ,NO  ,NO  ,NO  ,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer6: numpad
        FN31,NO  ,NO  ,NO  ,NO  ,NO  ,               NLCK,PSLS,PAST,PAST,PMNS,BSPC,
        NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,               3   ,P7  ,P8  ,P9  ,PMNS,PGUP,
        NO  ,NO  ,NO  ,NO  ,TRNS,NO  ,               P   ,P4  ,P5  ,P6  ,PPLS,PGDN,
        NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,               NO  ,P1  ,P2  ,P3  ,PPLS,PENT,
             NO  ,FN31,NO  ,NO  ,                         P0  ,PDOT,SLSH,PENT,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer7: application-specific shortcuts (mostly browser)
        // left hand
        FN31,NO  ,NO  ,NO  ,NO  ,NO  ,              NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
        NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,              NO  ,FN12,FN13,FN14,FN15,FN10,
        NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,              FN1 ,FN2 ,FN3 ,FN4 ,FN5 ,FN11,
        NO  ,NO  ,NO  ,NO  ,TRNS,NO  ,              NO  ,FN6 ,FN7 ,FN8 ,FN9 ,FN0 ,
             NO  ,FN31,NO  ,NO  ,                         NO  ,NO  ,NO  ,NO  ,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer8: Ctrl+Alt with F-keys
        TRNS,F1  ,F2  ,F3  ,F4  ,F5  ,               F6  ,F7  ,F8  ,F9  ,F10 ,TRNS,
        TRNS,P1  ,P2  ,P3  ,P4  ,P5  ,               P6  ,P7  ,P8  ,P9  ,P0  ,TRNS,
        TRNS,TRNS,TRNS,E   ,TRNS,TRNS,               TRNS,U   ,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,FN31,TRNS,TRNS,                         TRNS,TRNS,TRNS,TRNS,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer9 : Autocad (NumPad + immediate Ctrl/Shift/Alt/Win)
        ESC ,TRNS,TRNS,FN5 ,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        ESC ,NLCK,P7  ,P8  ,P9  ,PMNS,               Y   ,U   ,I   ,O   ,P   ,LBRC,
        TAB ,NO  ,P4  ,P5  ,P6  ,PPLS,               H   ,J   ,K   ,L   ,SCLN,TRNS,
        ENT ,P0  ,P1  ,P2  ,P3  ,PENT,               N   ,M   ,COMM,DOT ,SLSH,RBRC,
             TRNS,P0  ,SPC ,LALT,                         TRNS,TRNS,TRNS,TRNS,
                                 LGUI,TRNS,     TRNS,TRNS,
                                      ENT ,     TRNS,
                            LCTL,LSFT,ESC ,     TRNS,TRNS,TRNS
    ),


    KEYMAP(  // Layer10: fully transparent
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,                         TRNS,TRNS,TRNS,TRNS,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer11: fully transparent
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,                         TRNS,TRNS,TRNS,TRNS,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer12: fully transparent
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,                         TRNS,TRNS,TRNS,TRNS,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    KEYMAP(  // Layer13: Macros and F-keys + utils(Teensy, layers selector)
        FN1 ,NO  ,NO  ,FN5 ,NO  ,TRNS,               TRNS,NO  ,NO  ,NO  ,NO  ,FN0 ,
        FN2 ,F1  ,F2  ,F3  ,F4  ,NO  ,               NO  ,F13 ,F14 ,F15 ,F16 ,TRNS,
        TRNS,F5  ,F6  ,F7  ,F8  ,NO  ,               NO  ,F17 ,F18 ,F19 ,F20 ,TRNS,
        TRNS,F9  ,F10 ,F11 ,F12 ,NO  ,               NO  ,F21 ,F22 ,F23 ,F24 ,TRNS,
             TRNS,FN31,TRNS,TRNS,                         TRNS,TRNS,TRNS,TRNS,
                                 TRNS,TRNS,     TRNS,SLEP,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),

    */



/*
    // templates to copy from

    KEYMAP(  // LayerN: fully transparent
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,               TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
             TRNS,TRNS,TRNS,TRNS,                         TRNS,TRNS,TRNS,TRNS,
                                 TRNS,TRNS,     TRNS,TRNS,
                                      TRNS,     TRNS,
                            TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS
    ),


*/

};

/* id for user defined functions & macros */
enum function_id {
    TEENSY_KEY,
    INVERTED_GRV,
    L_CTRL_ALT_NO,
    R_CTRL_ALT_PGUP,
};

enum macro_id {
    PASSWORD0,
    PASSWORD1,
    PASSWORD2,
    PASSWORD3,
    PASSWORD4,
};

/*
 * Fn action definition
 */
const action_t fn_actions_0[] PROGMEM = {
    //[ 5] =  ACTION_LAYER_TAP_KEY(5, KC_V),                      // FN17 = momentary Layer7 on V key
    //[10] =  ACTION_MODS_KEY(MOD_LSFT, KC_MINS),                 // FN10 = Shifted Minus -- \ in Workman
    //[18] =  ACTION_FUNCTION_TAP(L_CTRL_ALT_NO),                 // FN18 = momentary Layer8+CTRL+ALT on NO
    //[19] =  ACTION_FUNCTION_TAP(R_CTRL_ALT_PGUP),               // FN19 = momentary Layer8+CTRL+ALT on PGUP

    // modified/shifted/inverted
    //[20] =  ACTION_MODS_KEY(MOD_LSFT, KC_SLCK),                 // FN22 = Shifted ScrollLock
//    [ 8] =  ACTION_MODS_KEY(MOD_LSFT,           KC_LBRC),       // FN8
    [ 9] =  ACTION_FUNCTION(INVERTED_GRV),                      // FN9  = Inverted `/~ pair

    // dual-role
    [10] =  ACTION_MODS_TAP_KEY(MOD_LSFT,       KC_TAB),        // FN10 = LShift with tap Tab
    [11] =  ACTION_MODS_TAP_KEY(MOD_RSFT,       KC_QUOT),       // FN11 = RShift with tap '/"
    // thumbs section
    [12] =  ACTION_MODS_TAP_KEY(MOD_LALT,       KC_NO),         // FN12 = LAlt   with tap Escape
    [13] =  ACTION_MODS_TAP_KEY(MOD_RALT,       KC_PGDN),       // FN13 = RAlt   with tap PgDn
    [14] =  ACTION_MODS_TAP_KEY(MOD_LCTL,       KC_BSPC),       // FN14 = LShift with tap BackSpace
    [15] =  ACTION_MODS_TAP_KEY(MOD_LSFT,       KC_DEL),        // FN15 = LCtrl  with tap Delete
    [16] =  ACTION_MODS_TAP_KEY(MOD_LGUI,       KC_ESC),        // FN16 = LGui   with tap Escape
    [17] =  ACTION_MODS_TAP_KEY(MOD_RGUI,       KC_INS),        // FN17 = RGui   with tap Ins
    [18] =  ACTION_MODS_TAP_KEY(MOD_RSFT,       KC_ENT),        // FN18 = RShift with tap Enter
    [19] =  ACTION_MODS_TAP_KEY(MOD_RCTL,       KC_SPC),        // FN19 = RCtrl  with tap Space

    // layers
    [21] =  ACTION_LAYER_TAP_KEY(1, KC_A),                      // FN21 = momentary Layer1 on F key
    [22] =  ACTION_LAYER_TAP_KEY(2, KC_S),                      // FN22 = momentary Layer2 on D key
    [23] =  ACTION_LAYER_TAP_KEY(3, KC_D),                      // FN23 = momentary Layer3 on S key
    [24] =  ACTION_LAYER_TAP_KEY(4, KC_F),                      // FN24 = momentary Layer4 on A key
    [25] =  ACTION_LAYER_TAP_KEY(5, KC_5),                      // FN25 = momentary Layer5 on 5 key
    [26] =  ACTION_LAYER_TAP_KEY(5, KC_6),                      // FN26 = momentary Layer5 on 6 key
    [27] =  ACTION_LAYER_TAP_KEY(7, KC_GRV),                    // FN27 = momentary Layer6 on ~ key - should be modified to set layer permanently

    // system hacks
    [30] =  ACTION_FUNCTION(TEENSY_KEY),                        // FN30 = Teensy key
    [31] =  ACTION_LAYER_SET(0, ON_PRESS),                      // FN31 = set Layer0
};

const action_t fn_actions_1[] PROGMEM = {
    [ 0] =  ACTION_MODS_KEY(MOD_LSFT,           KC_BSLS),       // FN0
    [ 1] =  ACTION_MODS_KEY(MOD_LSFT,           KC_LBRC),       // FN1
    [ 2] =  ACTION_MODS_KEY(MOD_LSFT,           KC_RBRC),       // FN2
    [ 3] =  ACTION_MODS_KEY(MOD_LSFT,           KC_QUOT),       // FN3
    [ 4] =  ACTION_MODS_KEY(MOD_LSFT,           KC_COMM),       // FN4
    [ 5] =  ACTION_MODS_KEY(MOD_LSFT,           KC_DOT),        // FN5
    [ 6] =  ACTION_MODS_KEY(MOD_LSFT,           KC_P),          // FN6
};

const action_t fn_actions_5[] PROGMEM = {
    [ 1] =  ACTION_MACRO(PASSWORD1),                            // FN1
    [ 2] =  ACTION_MACRO(PASSWORD2),                            // FN2
    [ 3] =  ACTION_MACRO(PASSWORD3),                            // FN3
    [ 4] =  ACTION_MACRO(PASSWORD4),                            // FN4
    [ 9] =  ACTION_LAYER_SET(6, ON_PRESS),                      // FN4
};

#define KEYMAPS_SIZE        (sizeof(keymaps)       / sizeof(keymaps[0]))
#define FN_ACTIONS_0_SIZE   (sizeof(fn_actions_0)  / sizeof(fn_actions_0[0]))
#define FN_ACTIONS_1_SIZE   (sizeof(fn_actions_1)  / sizeof(fn_actions_1[0]))
#define FN_ACTIONS_5_SIZE   (sizeof(fn_actions_5)  / sizeof(fn_actions_5[0]))

/*
 * translates Fn keycode to action
 * for some layers, use different translation table
 */
action_t keymap_fn_to_action(uint8_t keycode)
{
    uint8_t layer = biton32(layer_state);

    action_t action = (action_t)ACTION_NO;

    if (layer == 1 && FN_INDEX(keycode) < FN_ACTIONS_1_SIZE) {
        return (action_t)pgm_read_word(&fn_actions_1[FN_INDEX(keycode)]);
    } else if (layer == 5 && FN_INDEX(keycode) < FN_ACTIONS_5_SIZE) {
        return (action_t)pgm_read_word(&fn_actions_5[FN_INDEX(keycode)]);
    } else if (FN_INDEX(keycode) < FN_ACTIONS_0_SIZE) {
        // by default, use fn_actions from default layer 0
        // this is needed to get mapping for same key, that was used switch to some layer,
        // to have possibility to switch layers back
        // to get this working, FNxx to switch layers should be as high as possible and should not be re-defined on other layers
        return (action_t)pgm_read_word(&fn_actions_0[FN_INDEX(keycode)]);
    }

    return action;
}

uint8_t orig_mods, real_mods, shift_pressed;
void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    // print("action_function called\n");
    // print("id  = "); phex(id); print("\n");
    // print("opt = "); phex(opt); print("\n");

    if (id == TEENSY_KEY) {
        clear_keyboard();
        print("\n\nJump to bootloader... ");
        _delay_ms(50);


        cli();
        UDCON = 1;
        USBCON = (1<<FRZCLK);  // disable USB
        UCSR1B = 0;
        _delay_ms(5);
        EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
        TIMSK0 = 0; TIMSK1 = 0; TIMSK3 = 0; TIMSK4 = 0; UCSR1B = 0; TWCR = 0;
        DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; TWCR = 0;
        PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
        asm volatile("jmp 0x7E00");

        bootloader_jump(); // should not return
        print("not supported.\n");
    }

    if (id == INVERTED_GRV) {
        orig_mods = real_mods = get_mods();
        shift_pressed = real_mods && 0x22;              // both SHIFTs

        if (shift_pressed) {
            real_mods &= ~0x22;                         // remove bits for both SHIFTs
        } else {
            real_mods |= 0x02;                          // emulate LEFT SHIFT pressed
        }

        set_mods(real_mods);
        send_keyboard_report();

        if (record->event.pressed) {
            register_code(KC_GRV);
        } else {
            unregister_code(KC_GRV);
        }

        set_mods(orig_mods);
        send_keyboard_report();
    }

    if (   id == L_CTRL_ALT_NO
        || id == R_CTRL_ALT_PGUP
    ) {
        // debug
        /*
        if (record->event.pressed) dprint("P"); else dprint("R");
        dprintf("%d", record->tap.count);
        if (record->tap.interrupted) dprint("i");
        dprint("\n");
        */

        if (record->tap.count == 0 || record->tap.interrupted) {
            uint8_t weak_mods = 0;
            uint8_t layer = 0;

            if (id == L_CTRL_ALT_NO) {
                weak_mods = MOD_BIT(KC_LCTL) | MOD_BIT(KC_LALT);
                layer     = 8;
            } else if (id == R_CTRL_ALT_PGUP) {
                weak_mods = MOD_BIT(KC_RCTL) | MOD_BIT(KC_RALT);
                layer     = 8;
            } else {
                // do nothing - this should never happen
            }

            if (record->event.pressed) {
                layer_on(layer);
                add_weak_mods(weak_mods);
            } else {
                del_weak_mods(weak_mods);
                layer_off(layer);
            }
        } else {
            if (id == L_CTRL_ALT_NO) {
                if (record->event.pressed) {
                    add_key(KC_NO);
                } else {
                    del_key(KC_NO);
                }
            } else if (id == R_CTRL_ALT_PGUP) {
                if (record->event.pressed) {
                    add_key(KC_PGUP);
                } else {
                    del_key(KC_PGUP);
                }
            } else {
                // do nothing - this should never happen
            }

            send_keyboard_report();
        }
    }
}


/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    if (layer < KEYMAPS_SIZE) {
        return pgm_read_byte(&keymaps[(layer)][(key.row)][(key.col)]);
    } else {
        // fall back to layer 0
        return pgm_read_byte(&keymaps[0][(key.row)][(key.col)]);
    }
}

#define MACRO_PASSWORD0     MACRO( \
                                I(15), \
                                T(E), T(X), T(A), T(M), T(P), T(L), T(E), T(ENT), \
                            END)

#define MACRO_PASSWORD1     MACRO( \
                                I(15), \
                                T(C), T(U), T(B), T(I), T(C), \
                                T(R), T(U), T(B), T(I), T(C), \
                                T(ENT), \
                            END)

#define MACRO_PASSWORD2     MACRO( \
                                I(15), \
                                T(C), T(U), T(B), \
                                D(LSFT), T(MINS), U(LSFT), \
                                T(7), T(1), T(3), T(ENT), \
                            END)

#define MACRO_PASSWORD3     MACRO( \
                                I(15), \
                                T(V), T(I), T(T), T(SCLN), T(V), \
                                T(E), T(I), T(T), T(SCLN), T(V), \
                                T(ENT), \
                            END)

#define MACRO_PASSWORD4     MACRO( \
                                I(15), \
                                T(V), T(I), T(T), \
                                D(LSFT), T(MINS), \
                                T(7), T(1), T(3), \
                                U(LSFT), T(ENT), \
                            END)

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt) {
    if (record->event.pressed) {
        switch (id) {
            case PASSWORD1:     return (MACRO_PASSWORD1);
            case PASSWORD2:     return (MACRO_PASSWORD2);
            case PASSWORD3:     return (MACRO_PASSWORD3);
            case PASSWORD4:     return (MACRO_PASSWORD4);
        }
    }
    return MACRO_NONE;
}

