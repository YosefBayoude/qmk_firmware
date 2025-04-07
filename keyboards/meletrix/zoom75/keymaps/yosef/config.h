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

// start layers at a higher number, this is so that QK_LAYER_TAP_GET_LAYER can work,
// starting high avoids clashes layer values may have with other mods / keycodes may use
// example : QK_LAYER_TAP_GET_LAYER(RCTL_T(m)) returns 1
#define _BASE 0
#define _NAV 12
#define _NBR 13
#define _SBL 14
#define _DFT 15
