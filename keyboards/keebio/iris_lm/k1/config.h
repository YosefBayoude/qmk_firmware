// Copyright 2024 Danny Nguyen (@nooges)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// RGB
#define SPLIT_TRANSACTION_IDS_USER SET_RGB_ALL_SYNC, SET_RGB_SYNC

/* Defines for the split keyboard setup */
#define SERIAL_USART_DRIVER SD3 // USART 3
#define SERIAL_USART_TX_PIN B10
#define SERIAL_USART_RX_PIN B11
#define SERIAL_USART_TX_PAL_MODE 7
#define SERIAL_USART_RX_PAL_MODE 7
#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_PIN_SWAP

#define USB_VBUS_PIN C6

/* Defines for the RGB matrix */
#ifdef RGB_MATRIX_ENABLE
#define WS2812_PWM_DRIVER PWMD3
#define WS2812_PWM_CHANNEL 4
#define WS2812_PWM_PAL_MODE 10
#define WS2812_DMA_STREAM STM32_DMA1_STREAM2
#define WS2812_DMA_CHANNEL 2
#define WS2812_DMAMUX_ID STM32_DMAMUX1_TIM3_UP
#endif

//  key behaviour
// https://docs.qmk.fm/tap_hold#tapping-term
#define TAPPING_TERM 200 // NOTE : cannot be shorter than achordion_streak
#define TAPPING_TERM_PER_KEY

// https://docs.qmk.fm/tap_hold#tap-or-hold-decision-modes
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY

// https://docs.qmk.fm/tap_hold#quick-tap-term
#define QUICK_TAP_TERM_PER_KEY

// https://docs.qmk.fm/tap_hold#permissive-hold
#define PERMISSIVE_HOLD

// https://getreuer.info/posts/keyboards/achordion/index.html
// https://github.com/getreuer/qmk-keymap/tree/main/features
#define ACHORDION_ENABLE
// https://getreuer.info/posts/keyboards/achordion/index.html#typing-streaks
#define ACHORDION_STREAK

// https://docs.qmk.fm/features/caps_word
#define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD

// start layers at a higher number, this is so that QK_LAYER_TAP_GET_LAYER can work,
// starting high avoids clashes layer values may have with other mods / keycodes may use
// example : QK_LAYER_TAP_GET_LAYER(RCTL_T(m)) returns 1
#define _BASE 0
#define _NAVIGATION 12
#define _SYMBOLS 13
#define _NUMBERS 14
#define _GAMING 11
#define _TEMPLATE 15

#define _RGB_AZURE ((RGB){.r = 0x99, .g = 0xF5, .b = 0xFF})
#define _RGB_BLACK ((RGB){.r = 0x00, .g = 0x00, .b = 0x00})
#define _RGB_BLUE ((RGB){.r = 0x00, .g = 0x00, .b = 0xFF})
#define _RGB_CHARTREUSE ((RGB){.r = 0x80, .g = 0xFF, .b = 0x00})
#define _RGB_CORAL ((RGB){.r = 0xFF, .g = 0x7C, .b = 0x4D})
#define _RGB_CYAN ((RGB){.r = 0x00, .g = 0xFF, .b = 0xFF})
#define _RGB_DARK_CYAN ((RGB){.r = 0x00, .g = 0x20, .b = 0x20})
#define _RGB_GOLD ((RGB){.r = 0xFF, .g = 0xD9, .b = 0x00})
#define _RGB_GOLDENROD ((RGB){.r = 0xD9, .g = 0xA5, .b = 0x21})
#define _RGB_GREEN ((RGB){.r = 0x00, .g = 0xFF, .b = 0x00})
#define _RGB_MAGENTA ((RGB){.r = 0xFF, .g = 0x00, .b = 0xFF})
#define _RGB_ORANGE ((RGB){.r = 0xFF, .g = 0x80, .b = 0x00})
#define _RGB_PINK ((RGB){.r = 0xFF, .g = 0x80, .b = 0xBF})
#define _RGB_PURPLE ((RGB){.r = 0x7A, .g = 0x00, .b = 0xFF})
#define _RGB_RED ((RGB){.r = 0xFF, .g = 0x00, .b = 0x00})
#define _RGB_SPRINGGREEN ((RGB){.r = 0x00, .g = 0xFF, .b = 0x80})
#define _RGB_TEAL ((RGB){.r = 0x00, .g = 0x80, .b = 0x80})
#define _RGB_TURQUOISE ((RGB){.r = 0x47, .g = 0x6E, .b = 0x6A})
#define _RGB_WHITE ((RGB){.r = 0xFF, .g = 0xFF, .b = 0xFF})
#define _RGB_YELLOW ((RGB){.r = 0xFF, .g = 0xFF, .b = 0x00})
#define _RGB_OFF _RGB_BLACK // _RGB_OFF mapped to _RGB_BLACK
