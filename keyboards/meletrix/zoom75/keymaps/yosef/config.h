#pragma once

#define ENCODER_MAP_ENABLE

//  key behaviour
// https://docs.qmk.fm/tap_hold#tapping-term
#define TAPPING_TERM 200 // NOTE : cannot be shorter than achordion_streak
#define TAPPING_TERM_PER_KEY

// https://docs.qmk.fm/tap_hold#tap-or-hold-decision-modes
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY

// https://docs.qmk.fm/tap_hold#quick-tap-term
#define QUICK_TAP_TERM_PER_KEY

// https://docs.qmk.fm/tap_hold#permissive-hold
#define PERMISSIVE_HOLD_PER_KEY

// https://getreuer.info/posts/keyboards/achordion/index.html
// https://github.com/getreuer/qmk-keymap/tree/main/features
#define ACHORDION_ENABLE
// https://getreuer.info/posts/keyboards/achordion/index.html#typing-streaks
#define ACHORDION_STREAK

// https://docs.qmk.fm/features/caps_word
#define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD

// WARNING: QK_LAYER_TAP_GET_LAYER only works correctly on layer-tap keys.
// When used on mod-taps, it returns the modifier bits instead of a layer number.
// Always guard with IS_QK_LAYER_TAP(keycode) before calling QK_LAYER_TAP_GET_LAYER.
// Example: QK_LAYER_TAP_GET_LAYER(RCTL_T(KC_M)) returns 1 (LCTL bit), not a layer.
#define _BASE 0
#define _NAV 12
#define _NBR 13
#define _SBL 14
#define _DFT 15
