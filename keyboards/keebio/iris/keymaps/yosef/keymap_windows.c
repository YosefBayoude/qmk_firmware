#include QMK_KEYBOARD_H

#ifdef ACHORDION_ENABLE
#include "features/achordion.h"
#endif // ACHORDION_ENABLE
#ifdef RGB_MATRIX_ENABLE
#include "features/rgb_matrix.h"
#endif // RGB_MATRIX_ENABLE
#include "print.h"

void matrix_scan_user(void) {
#ifdef ACHORDION_ENABLE
  achordion_task();
#endif // ACHORDION_ENABLE
}

// per key process
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    dprintf(
        "KL: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, "
        "count: %u\n",
        keycode,
        record->event.key.col,
        record->event.key.row,
        record->event.pressed,
        record->event.time,
        record->tap.interrupted,
        record->tap.count
    );
  }
#ifdef ACHORDION_ENABLE
  if (!process_achordion(keycode, record)) {
    return false;
  }
#endif // ACHORDION_ENABLE
  switch (keycode) {
    case LT(_NAVIGATION, PB_1): // delete previous word
      if (record->tap.count > 0) {
        if (record->event.pressed) {
          register_code(KC_LCTL);
          tap_code(KC_BSPC);
          unregister_code(KC_LCTL);
        }
        return false;
      }
  }
  return true;
}

void keyboard_post_init_user(void) {
  // enable debugging
  // debug_enable = true; // to attach run: qmk console

  // rgb_matrix_sethsv(HSV_OFF);
  // rgb_matrix_mode(RGB_MATRIX_NONE);

#ifdef RGB_MATRIX_ENABLE
  rgb_matrix_enable();

  // custom effect to avoid rgb_matrix frames to override any led control
  rgb_matrix_sethsv(HSV_OFF);
  rgb_matrix_mode(RGB_MATRIX_CUSTOM_empty_effect);

  // does NOT need to only run on slave
  transaction_register_rpc(SET_RGB_SYNC, set_rgb_sync_slave_handler);
  transaction_register_rpc(SET_RGB_ALL_SYNC, set_rgb_all_sync_slave_handler);
#endif // RGB_MATRIX_ENABLE
}

// minimum time before a tap is condisered a hold
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  uint8_t mod = mod_config(QK_MOD_TAP_GET_MODS(keycode));
  switch (mod) {
    case MOD_LGUI:
    case MOD_RGUI:
      return TAPPING_TERM + 400;
    default:
      return TAPPING_TERM;
  }
}

// consider this a hold if any other key pressed while this one is not yet
// released
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
  uint8_t layer = QK_LAYER_TAP_GET_LAYER(keycode);
  switch (layer) {
    case _NAVIGATION:
    case _NUMBERS:
      return true;
    default:
      return false;
  }
}

// time before a double press is considered as tap function
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
  uint8_t layer = QK_LAYER_TAP_GET_LAYER(keycode);
  switch (layer) {
    case _NAVIGATION:
    case _NUMBERS:
      return 0;
    default:
      return QUICK_TAP_TERM;
  }
}

// caps word
bool caps_word_press_user(uint16_t keycode) {
  switch (keycode) {
    // continue caps word, with shift applied
    case KC_A ... KC_Z:
      add_weak_mods(MOD_BIT(KC_LSFT)); // apply shift to next key
      return true;

    // continue caps word, without shifting
    case KC_1 ... KC_9:
    case KC_DEL:
    case KC_UNDS:
    case KC_SCLN:
    case KC_MINS:
    case KC_BSPC:
    case KC_LEFT:
    case KC_RIGHT:
      return true;

    // deactivate caps word
    default:
      return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Achordion
///////////////////////////////////////////////////////////////////////////////
#ifdef ACHORDION_ENABLE
bool achordion_chord(
    uint16_t tap_hold_keycode,
    keyrecord_t *tap_hold_record,
    uint16_t other_keycode,
    keyrecord_t *other_record
) {
  if (IS_QK_LAYER_TAP(tap_hold_keycode)) {
    return true; // Disable streak detection on layer-tap keys.
  }
  switch (tap_hold_keycode) {}
  switch (other_keycode) {
    case KC_SPC:
      return true;
      break;
  }
  return achordion_opposite_hands(tap_hold_record, other_record);
}

bool achordion_eager_mod(uint8_t mod) {
  switch (mod) {
    case MOD_LSFT:
    case MOD_RSFT:
    case MOD_LCTL:
    case MOD_RCTL:
    case MOD_LALT:
    case MOD_RALT:
      return true;

    default:
      return false;
  }
}
#ifdef ACHORDION_STREAK
uint16_t achordion_streak_chord_timeout(
    uint16_t tap_hold_keycode,
    uint16_t next_keycode
) {
  if (IS_QK_LAYER_TAP(tap_hold_keycode)) {
    return 0; // Disable streak detection on layer-tap keys.
  }
  // Otherwise, tap_hold_keycode is a mod-tap key.
  uint8_t mod = mod_config(QK_MOD_TAP_GET_MODS(tap_hold_keycode));
  if ((mod & MOD_LSFT) != 0) {
    return 0; // a shorter streak timeout for shift mod-tap keys
  }
  return 200; // default of 100 ms
}
#endif // ACHORDION_STREAK
#endif // ACHORDION_ENABLE

///////////////////////////////////////////////////////////////////////////////
// RGB
///////////////////////////////////////////////////////////////////////////////

#define EXPAND_RGB(color) (color).r, (color).g, (color).b

#ifdef RGB_MATRIX_ENABLE

void rgb_matrix_set_color_list(
    uint8_t list[],
    size_t size,
    uint8_t r,
    uint8_t g,
    uint8_t b
) {
  for (size_t i = 0; i < size; i++) {
    rgb_matrix_set_color_iris_rev8(list[i], r, g, b);
  }
}

uint8_t workspace = -1;
void raw_hid_receive(uint8_t *data, uint8_t length) {
  rgb_matrix_set_color_all_iris_rev8(0, 0, 0);
  workspace = data[0];
  uint8_t list[] = {workspace};
  rgb_matrix_set_color_list(list, ARRAY_SIZE(list), EXPAND_RGB(_RGB_CYAN));
}
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
  uint8_t list[] = {workspace};
  rgb_matrix_set_color_list(list, ARRAY_SIZE(list), EXPAND_RGB(_RGB_CYAN));
  return true;
}

void caps_word_set_user(bool active) {
  if (active) {
    RGB main_color = _RGB_ORANGE;
    rgb_matrix_set_color_all_iris_rev8(
        main_color.r,
        main_color.g,
        main_color.b
    );
  } else {
    rgb_matrix_set_color_all_iris_rev8(0, 0, 0);
  }
}

layer_state_t layer_state_set_user(layer_state_t state) {
  switch (get_highest_layer(state)) {
    case _SYMBOLS: {
      uint8_t list[] = {
          13, 12, 11, 10, 9,  16, 17, 18, 19, 20, 26, 25, 23, 22, 21,
          43, 44, 45, 46, 47, 54, 53, 52, 51, 50, 55, 56, 57, 59, 60,
      };
      rgb_matrix_set_color_list(list, ARRAY_SIZE(list), EXPAND_RGB(_RGB_CYAN));
    } break;
    case _NAVIGATION: {
      {
        uint8_t list[] =
            {43, 44, 45, 46, 47, 51, 52, 53, 54, 50, 55, 56, 59, 60};
        rgb_matrix_set_color_list(
            list,
            ARRAY_SIZE(list),
            EXPAND_RGB(_RGB_PURPLE)
        );
      }
      {
        uint8_t list[] = {13};
        rgb_matrix_set_color_list(list, ARRAY_SIZE(list), EXPAND_RGB(_RGB_RED));
      }
      {
        uint8_t list[] = {37, 39, 40};
        rgb_matrix_set_color_list(
            list,
            ARRAY_SIZE(list),
            EXPAND_RGB(_RGB_GREEN)
        );
      }
    } break;

    default:
      rgb_matrix_set_color_all_iris_rev8(0, 0, 0);
      break;
  }

  return state;
}

void oneshot_mods_changed_user(uint8_t mods) {
  if (mods & MOD_MASK_SHIFT) {
    uint8_t list[] = {19, 32, 53};
    rgb_matrix_set_color_list(list, ARRAY_SIZE(list), EXPAND_RGB(_RGB_ORANGE));
  }
  if (mods & MOD_MASK_CTRL) {
  }
  if (mods & MOD_MASK_ALT) {
  }
  if (mods & MOD_MASK_GUI) {
  }
  if (!mods) {
    rgb_matrix_set_color_all_iris_rev8(0, 0, 0);
  }
}
#endif // RGB_MATRIX_ENABLE

///////////////////////////////////////////////////////////////////////////////
// Keymap
///////////////////////////////////////////////////////////////////////////////
// NOTE : for shifted us keys to work, keyboard needs to be set as ANSI in mac
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//    ┌───────────────┬───────────┬───────────┬───────────┬───────────────────────┬────────────────────┐                         ┌──────┬───────────────┬───────────┬───────────┬───────────┬───────────────┐
//    │     A(c)      │   A(1)    │   A(2)    │   A(3)    │         A(4)          │        A(5)        │                         │ A(6) │     A(7)      │   A(8)    │   A(9)    │   A(0)    │  system_wake  │
//    ├───────────────┼───────────┼───────────┼───────────┼───────────────────────┼────────────────────┤                         ├──────┼───────────────┼───────────┼───────────┼───────────┼───────────────┤
//    │      tab      │     q     │     w     │     e     │           r           │         t          │                         │  y   │       u       │     i     │     o     │     p     │     S(;)      │
//    ├───────────────┼───────────┼───────────┼───────────┼───────────────────────┼────────────────────┤                         ├──────┼───────────────┼───────────┼───────────┼───────────┼───────────────┤
//    │      esc      │ LCTL_T(a) │ LALT_T(s) │ LGUI_T(d) │       LSFT_T(f)       │         g          │                         │  h   │   RSFT_T(j)   │ RGUI_T(k) │ RALT_T(l) │ RCTL_T(m) │       ;       │
//    ├───────────────┼───────────┼───────────┼───────────┼───────────────────────┼────────────────────┼───────────────┬─────────┼──────┼───────────────┼───────────┼───────────┼───────────┼───────────────┤
//    │ OSM(MOD_LSFT) │     z     │     x     │     c     │           v           │         b          │               │ QK_BOOT │  n   │       /       │     ,     │     .     │     =     │ OSM(MOD_RSFT) │
//    └───────────────┴───────────┴───────────┴───────────┼───────────────────────┼────────────────────┼───────────────┼─────────┼──────┼───────────────┼───────────┴───────────┴───────────┴───────────────┘
//                                                        │ LT(_NAVIGATION, PB_1) │ LT(_NUMBERS, bspc) │ OSM(MOD_LSFT) │   ent   │ spc  │ OSL(_SYMBOLS) │
//                                                        └───────────────────────┴────────────────────┴───────────────┴─────────┴──────┴───────────────┘
[_BASE] = LAYOUT(
  A(KC_C)       , A(KC_1)      , A(KC_2)      , A(KC_3)      , A(KC_4)               , A(KC_5)               ,                           A(KC_6) , A(KC_7)       , A(KC_8)      , A(KC_9)      , A(KC_0)      , KC_SYSTEM_WAKE,
  KC_TAB        , KC_Q         , KC_W         , KC_E         , KC_R                  , KC_T                  ,                           KC_Y    , KC_U          , KC_I         , KC_O         , KC_P         , S(KC_SCLN)    ,
  KC_ESC        , LGUI_T(KC_A) , LALT_T(KC_S) , LCTL_T(KC_D) , LSFT_T(KC_F)          , KC_G                  ,                           KC_H    , RSFT_T(KC_J)  , RCTL_T(KC_K) , RALT_T(KC_L) , RGUI_T(KC_M) , KC_SCLN       ,
  OSM(MOD_LSFT) , KC_Z         , KC_X         , KC_C         , KC_V                  , KC_B                  , _______       , QK_BOOT , KC_N    , KC_SLSH       , KC_COMM      , KC_DOT       , KC_EQL       , OSM(MOD_RSFT) ,
                                                               LT(_NAVIGATION, PB_1) , LT(_NUMBERS, KC_BSPC) , OSM(MOD_LSFT) , KC_ENT  , KC_SPC  , OSL(_SYMBOLS)
),

//    ┌─────────┬──────┬──────┬──────┬──────┬─────┐           ┌─────────┬─────────┬──────┬─────────┬─────────┬─────┐
//    │ QK_MAKE │      │      │      │      │     │           │         │  mprv   │ mply │  mnxt   │         │     │
//    ├─────────┼──────┼──────┼──────┼──────┼─────┤           ├─────────┼─────────┼──────┼─────────┼─────────┼─────┤
//    │         │ A(q) │ A(z) │ A(e) │ A(r) │     │           │         │ A(left) │  up  │ A(rght) │         │     │
//    ├─────────┼──────┼──────┼──────┼──────┼─────┤           ├─────────┼─────────┼──────┼─────────┼─────────┼─────┤
//    │         │ A(a) │ A(s) │ A(d) │ A(f) │     │           │ G(left) │  left   │ down │  rght   │ G(rght) │     │
//    ├─────────┼──────┼──────┼──────┼──────┼─────┼─────┬─────┼─────────┼─────────┼──────┼─────────┼─────────┼─────┤
//    │ AC_TOGG │ A(w) │ A(x) │ A(c) │ A(v) │     │     │     │ A(bspc) │  bspc   │      │   del   │ G(del)  │     │
//    └─────────┴──────┴──────┴──────┼──────┼─────┼─────┼─────┼─────────┼─────────┼──────┴─────────┴─────────┴─────┘
//                                   │      │     │     │     │         │ G(spc)  │
//                                   └──────┴─────┴─────┴─────┴─────────┴─────────┘
[_NAVIGATION] = LAYOUT(
  QK_MAKE , _______ , _______ , _______ , _______ , _______ ,                     _______    , KC_MPRV    , KC_MPLY , KC_MNXT    , _______    , _______,
  _______ , A(KC_Q) , A(KC_Z) , A(KC_E) , A(KC_R) , _______ ,                     _______    , C(KC_LEFT) , KC_UP   , C(KC_RGHT) , _______    , _______,
  _______ , A(KC_A) , A(KC_S) , A(KC_D) , A(KC_F) , _______ ,                     KC_HOME , KC_LEFT    , KC_DOWN , KC_RGHT    , KC_END , _______,
  AC_TOGG , A(KC_W) , A(KC_X) , A(KC_C) , A(KC_V) , _______ , _______ , _______ , A(KC_BSPC) , KC_BSPC    , _______ , KC_DEL     , G(KC_DEL)  , _______,
                                          _______ , _______ , _______ , _______ , _______    , G(KC_SPC)
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐           ┌──────────┬──────┬──────┬──────┬─────┬─────┐
//    │ f12 │ f1  │ f2  │ f3  │ f4  │ f5  │           │    f6    │  f7  │  f8  │  f9  │ f10 │ f11 │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤           ├──────────┼──────┼──────┼──────┼─────┼─────┤
//    │     │     │     │     │     │     │           │          │ kp_7 │ kp_8 │ kp_9 │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤           ├──────────┼──────┼──────┼──────┼─────┼─────┤
//    │     │     │     │     │     │     │           │ nUM_LOCK │ kp_4 │ kp_5 │ kp_6 │  ,  │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┬─────┼──────────┼──────┼──────┼──────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │          │ kp_1 │ kp_2 │ kp_3 │  .  │     │
//    └─────┴─────┴─────┴─────┼─────┼─────┼─────┼─────┼──────────┼──────┼──────┴──────┴─────┴─────┘
//                            │     │     │     │     │   kp_0   │ kp_0 │
//                            └─────┴─────┴─────┴─────┴──────────┴──────┘
[_NUMBERS] = LAYOUT(
  KC_F12  , KC_F1   , KC_F2   , KC_F3   , KC_F4   , KC_F5   ,                     KC_F6       , KC_F7 , KC_F8 , KC_F9 , KC_F10  , KC_F11 ,
  _______ , _______ , _______ , _______ , _______ , _______ ,                     _______     , KC_P7 , KC_P8 , KC_P9 , _______ , _______,
  _______ , _______ , _______ , _______ , _______ , _______ ,                     KC_NUM_LOCK , KC_P4 , KC_P5 , KC_P6 , KC_COMM , _______,
  _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______     , KC_P1 , KC_P2 , KC_P3 , KC_DOT  , _______,
                                          _______ , _______ , _______ , _______ , KC_P0       , KC_P0
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐           ┌─────┬─────┬─────┬─────┬─────┬─────┐
//    │     │     │     │     │     │     │           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │  `  │  '  │  "  │  {  │  }  │           │  [  │  ]  │  #  │  |  │  ~  │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │  $  │  +  │  -  │  (  │  )  │           │  <  │  >  │  @  │  ?  │  !  │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┬─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │  ^  │  *  │  _  │  %  │  &  │     │     │     │  \  │     │     │     │     │
//    └─────┴─────┴─────┴─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┴─────┴─────┴─────┘
//                            │     │     │     │     │     │     │
//                            └─────┴─────┴─────┴─────┴─────┴─────┘
[_SYMBOLS] = LAYOUT(
  _______ , _______ , _______ , _______ , _______ , _______ ,                     _______ , _______ , _______ , _______ , _______ , _______,
  _______ , KC_GRV  , KC_QUOT , KC_DQT  , KC_LCBR , KC_RCBR ,                     KC_LBRC , KC_RBRC , KC_HASH , KC_PIPE , KC_TILD , _______,
  _______ , KC_DLR  , KC_PLUS , KC_MINS , KC_LPRN , KC_RPRN ,                     KC_LABK , KC_RABK , KC_AT   , KC_QUES , KC_EXLM , _______,
  _______ , KC_CIRC , KC_ASTR , KC_UNDS , KC_PERC , KC_AMPR , _______ , _______ , _______ , KC_BSLS , _______ , _______ , _______ , _______,
                                          _______ , _______ , _______ , _______ , _______ , _______
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐           ┌─────┬─────┬─────┬─────┬─────┬─────┐
//    │     │     │     │     │     │     │           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┬─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │     │     │     │     │     │     │
//    └─────┴─────┴─────┴─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┴─────┴─────┴─────┘
//                            │     │     │     │     │     │     │
//                            └─────┴─────┴─────┴─────┴─────┴─────┘
[_TEMPLATE] = LAYOUT(
  _______ , _______ , _______ , _______ , _______ , _______ ,                     _______ , _______ , _______ , _______ , _______ , _______,
  _______ , _______ , _______ , _______ , _______ , _______ ,                     _______ , _______ , _______ , _______ , _______ , _______,
  _______ , _______ , _______ , _______ , _______ , _______ ,                     _______ , _______ , _______ , _______ , _______ , _______,
  _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______,
                                          _______ , _______ , _______ , _______ , _______ , _______
)
};

// clang-format on
