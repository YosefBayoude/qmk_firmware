#include QMK_KEYBOARD_H
#ifdef ACHORDION_ENABLE
#include "features/achordion.h"
#endif // ACHORDION_ENABLE
#include "print.h"

enum custom_keycodes {
  DEL_WORD = SAFE_RANGE,
};

void matrix_scan_user(void) {
#ifdef ACHORDION_ENABLE
  achordion_task();
#endif // ACHORDION_ENABLE
}

uint16_t previous_keycode = -1;

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
    case LT(_NAV, DEL_WORD): // delete previous word
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
  debug_enable = true; // to attach run: qmk console
}

// minimum time before a tap is condisered a hold
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  uint8_t mod = mod_config(QK_MOD_TAP_GET_MODS(keycode));
  switch (mod) {
    case MOD_LGUI:
    case MOD_RGUI:
      return TAPPING_TERM + 200;
    default:
      return TAPPING_TERM;
  }
}

// consider this a hold if any other key pressed and released while this one is not yet released
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
  uint8_t mod = mod_config(QK_MOD_TAP_GET_MODS(keycode));
  if ((mod & MOD_LGUI) != 0 || (mod & MOD_RGUI) != 0) {
    return false;
  }
  return true;
}

// consider this a hold if any other key pressed while this one is not yet released
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
  uint8_t layer = QK_LAYER_TAP_GET_LAYER(keycode);
  switch (layer) {
    case _NAV:
    case _NBR:
      return true;
    default:
      return false;
  }
}

// time before a double press is considered as tap function
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
  uint8_t layer = QK_LAYER_TAP_GET_LAYER(keycode);
  switch (layer) {
    case _NAV:
    case _NBR:
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

int findElement(const uint16_t arr[], int n, int key) {
  for (int i = 0; i < n; i++)
    if (arr[i] == key)
      return i; // return index if element found

  return -1; // return -1 if element not found
}

const uint16_t exclude_from_left[] = {
    KC_Y,
    KC_U,
    KC_I,
    KC_O,
    KC_P,
    KC_H,
    KC_J,
    KC_K,
    KC_L,
    KC_M,
    KC_SCLN,
    KC_N,
    KC_COLN,
    KC_SLSH,
    KC_COMM,
    KC_DOT,
    KC_EQL,
    KC_RSFT
};
const uint16_t exclude_from_right[] = {
    KC_TAB,
    KC_Q,
    KC_W,
    KC_E,
    KC_R,
    KC_T,
    KC_ESC,
    KC_A,
    KC_S,
    KC_D,
    KC_F,
    KC_G,
    KC_LSFT,
    KC_Z,
    KC_X,
    KC_C,
    KC_V,
    KC_B
};
bool achordion_chord(
    uint16_t tap_hold_keycode,
    keyrecord_t *tap_hold_record,
    uint16_t other_keycode,
    keyrecord_t *other_record
) {
  if (IS_QK_LAYER_TAP(tap_hold_keycode)) {
    return true; // Disable streak detection on layer-tap keys.
  }

  int r = sizeof(exclude_from_left) / sizeof(exclude_from_left[0]);
  int l = sizeof(exclude_from_right) / sizeof(exclude_from_right[0]);
  switch (mod_config(QK_MOD_TAP_GET_MODS(tap_hold_keycode))) {
    case MOD_LSFT:
    case MOD_LCTL:
    case MOD_LALT:
    case MOD_LGUI:
      return findElement(
                 exclude_from_right,
                 l,
                 QK_MOD_TAP_GET_TAP_KEYCODE(other_keycode)
             ) == -1;
    case MOD_RSFT:
    case MOD_RCTL:
    case MOD_RALT:
    case MOD_RGUI:
      return findElement(
                 exclude_from_left,
                 r,
                 QK_MOD_TAP_GET_TAP_KEYCODE(other_keycode)
             ) == -1;
    default:
      return true;
  }
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
  return 150; // default of 100 ms
}

#endif // ACHORDION_STREAK
#endif // ACHORDION_ENABLE

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//    ┌───────────────┬───────────┬────────────────┬───────────┬───────────┬────────────────┬──────┬───────────┬───────────┬───────────┬───────────┬─────┬───────────────┬──────────┬──────┐
//    │      esc      │    f1     │       f2       │    f3     │    f4     │       f5       │  f6  │    f7     │    f8     │    f9     │    f10    │ f11 │      f12      │ TG(_DFT) │ G(l) │
//    ├───────────────┼───────────┼────────────────┼───────────┼───────────┼────────────────┼──────┼───────────┼───────────┼───────────┼───────────┼─────┼───────────────┼──────────┼──────┼──────┐
//    │     A(`)      │   A(1)    │      A(2)      │   A(3)    │   A(4)    │      A(5)      │ A(6) │   A(7)    │   A(8)    │   A(9)    │     0     │  -  │       =       │   bspc   │ bspc │ del  │
//    ├───────────────┼───────────┼────────────────┼───────────┼───────────┼────────────────┼──────┼───────────┼───────────┼───────────┼───────────┼─────┼───────────────┼──────────┼──────┼──────┤
//    │      tab      │ LGUI_T(q) │       w        │     e     │     r     │       t        │  y   │     u     │     i     │     o     │     p     │  :  │       ]       │    \     │      │ pgup │
//    ├───────────────┼───────────┼────────────────┼───────────┼───────────┼────────────────┼──────┼───────────┼───────────┼───────────┼───────────┼─────┼───────────────┼──────────┘      ├──────┤
//    │      esc      │     a     │   LALT_T(s)    │ LCTL_T(d) │ LSFT_T(f) │       g        │  h   │ RSFT_T(j) │ RCTL_T(k) │ RALT_T(l) │ RGUI_T(m) │  ;  │      ent      │                 │ pgdn │
//    ├───────────────┼───────────┼────────────────┼───────────┼───────────┼────────────────┼──────┼───────────┼───────────┼───────────┼───────────┼─────┼───────────────┼──────────┐      └──────┘
//    │ OSM(MOD_LSFT) │     z     │       x        │     c     │     v     │       b        │      │     n     │     /     │     ,     │     .     │  =  │ OSM(MOD_RSFT) │    up    │
//    ├───────────────┼───────────┼────────────────┼───────────┴───────────┼────────────────┼──────┼───────────┼───────────┼───────────┼───────────┼─────┼───────────────┼──────────┼──────┐
//    │     lctl      │   lgui    │ LT(_NAV, PB_1) │                       │ LT(_NBR, bspc) │ ent  │    spc    │           │ OSL(_SBL) │ MO(_NAV)  │     │     left      │   down   │ rght │
//    └───────────────┴───────────┴────────────────┘                       └────────────────┴──────┴───────────┘           └───────────┴───────────┘     └───────────────┴──────────┴──────┘
[_BASE] = LAYOUT(
  KC_ESC        , KC_F1        , KC_F2          , KC_F3        , KC_F4        , KC_F5             , KC_F6   , KC_F7        , KC_F8        , KC_F9        , KC_F10       , KC_F11  , KC_F12        , TG(_DFT) , G(KC_L)           ,
  A(KC_GRV)     , A(KC_1)      , A(KC_2)        , A(KC_3)      , A(KC_4)      , A(KC_5)           , A(KC_6) , A(KC_7)      , A(KC_8)      , A(KC_9)      , KC_0         , KC_MINS , KC_EQL        , KC_BSPC  , KC_BSPC  , KC_DEL ,
  KC_TAB        , LGUI_T(KC_Q) , KC_W           , KC_E         , KC_R         , KC_T              , KC_Y    , KC_U         , KC_I         , KC_O         , KC_P         , KC_COLN , KC_RBRC       , KC_BSLS  ,            KC_PGUP,
  KC_ESC        , KC_A         , LALT_T(KC_S)   , LCTL_T(KC_D) , LSFT_T(KC_F) , KC_G              , KC_H    , RSFT_T(KC_J) , RCTL_T(KC_K) , RALT_T(KC_L) , RGUI_T(KC_M) , KC_SCLN , KC_ENT        ,                       KC_PGDN,
  OSM(MOD_LSFT) , KC_Z         , KC_X           , KC_C         , KC_V         , KC_B              , _______ , KC_N         , KC_SLSH      , KC_COMM      , KC_DOT       , KC_EQL  , OSM(MOD_RSFT) , KC_UP                        ,
  C(KC_C)       , C(KC_V)      , LT(_NAV, PB_1) ,                               LT(_NBR, KC_BSPC) , KC_ENT  , KC_SPC       ,                OSL(_SBL)    , MO(_NAV)     ,           KC_LEFT       , KC_DOWN  , KC_RIGHT
),

//    ┌─────┬──────┬──────┬──────┬──────┬─────┬──────┬─────────┬──────┬─────────┬─────┬────────┬─────┬─────┬─────┐
//    │     │      │      │      │      │     │      │         │      │         │     │        │     │     │     │
//    ├─────┼──────┼──────┼──────┼──────┼─────┼──────┼─────────┼──────┼─────────┼─────┼────────┼─────┼─────┼─────┼─────────┐
//    │     │      │      │      │      │     │      │         │      │         │     │        │     │     │     │ QK_BOOT │
//    ├─────┼──────┼──────┼──────┼──────┼─────┼──────┼─────────┼──────┼─────────┼─────┼────────┼─────┼─────┼─────┼─────────┤
//    │     │ A(q) │ A(w) │ A(e) │ A(r) │     │      │ C(left) │  up  │ C(rght) │     │        │     │     │     │         │
//    ├─────┼──────┼──────┼──────┼──────┼─────┼──────┼─────────┼──────┼─────────┼─────┼────────┼─────┼─────┘     ├─────────┤
//    │     │ A(a) │ A(s) │ A(d) │ A(f) │     │ home │  left   │ down │  rght   │ end │        │     │           │         │
//    ├─────┼──────┼──────┼──────┼──────┼─────┼──────┼─────────┼──────┼─────────┼─────┼────────┼─────┼─────┐     └─────────┘
//    │     │ A(z) │ A(x) │ A(c) │ A(v) │     │      │ C(bspc) │ bspc │         │ del │ C(del) │     │     │
//    ├─────┼──────┼──────┼──────┴──────┼─────┼──────┼─────────┼──────┼─────────┼─────┼────────┼─────┼─────┼─────┐
//    │     │      │      │             │     │      │         │      │ A(spc)  │     │        │     │     │     │
//    └─────┴──────┴──────┘             └─────┴──────┴─────────┘      └─────────┴─────┘        └─────┴─────┴─────┘
[_NAV] = LAYOUT(
  _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______    , _______ , _______    , _______ , _______   , _______ , _______ , _______          ,
  _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______    , _______ , _______    , _______ , _______   , _______ , _______ , _______ , QK_BOOT,
  _______ , A(KC_Q) , A(KC_W) , A(KC_E) , A(KC_R) , _______ , _______ , C(KC_LEFT) , KC_UP   , C(KC_RGHT) , _______ , _______   , _______ , _______ ,           _______,
  _______ , A(KC_A) , A(KC_S) , A(KC_D) , A(KC_F) , _______ , KC_HOME , KC_LEFT    , KC_DOWN , KC_RGHT    , KC_END  , _______   , _______ ,                     _______,
  _______ , A(KC_Z) , A(KC_X) , A(KC_C) , A(KC_V) , _______ , _______ , C(KC_BSPC) , KC_BSPC , _______    , KC_DEL  , C(KC_DEL) , _______ , _______                    ,
  _______ , _______ , _______ ,                     _______ , _______ , _______    ,           A(KC_SPC)  , _______ ,             _______ , _______ , _______
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬──────┬──────┬──────┬──────┬─────┬─────┬─────┬─────┐
//    │     │     │     │     │     │     │     │      │      │      │      │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼──────┼──────┼──────┼──────┼─────┼─────┼─────┼─────┼─────┐
//    │     │     │     │     │     │     │     │      │      │      │      │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼──────┼──────┼──────┼──────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │ kp_7 │ kp_8 │ kp_9 │      │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼──────┼──────┼──────┼──────┼─────┼─────┼─────┘     ├─────┤
//    │     │     │     │     │     │     │ nUM │ kp_4 │ kp_5 │ kp_6 │      │     │     │           │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼──────┼──────┼──────┼──────┼─────┼─────┼─────┐     └─────┘
//    │     │     │     │     │     │     │     │      │ kp_1 │ kp_2 │ kp_3 │     │     │     │
//    ├─────┼─────┼─────┼─────┴─────┼─────┼─────┼──────┼──────┼──────┼──────┼─────┼─────┼─────┼─────┐
//    │     │     │     │           │     │     │  0   │      │      │      │     │     │     │     │
//    └─────┴─────┴─────┘           └─────┴─────┴──────┘      └──────┴──────┘     └─────┴─────┴─────┘
[_NBR] = LAYOUT(
  _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______          ,
  _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______,
  _______ , _______ , _______ , _______ , _______ , _______ , _______ , KC_P7   , KC_P8   , KC_P9   , _______ , _______ , _______ , _______ ,           _______,
  _______ , _______ , _______ , _______ , _______ , _______ , KC_NUM  , KC_P4   , KC_P5   , KC_P6   , _______ , _______ , _______ ,                     _______,
  _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , KC_P1   , KC_P2   , KC_P3   , _______ , _______ , _______                    ,
  _______ , _______ , _______ ,                     _______ , _______ , KC_0    ,           _______ , _______ ,           _______ , _______ , _______
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
//    │     │     │     │     │     │     │     │     │     │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┐
//    │     │     │     │     │     │     │     │     │     │     │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │  `  │  '  │  "  │  {  │  }  │  [  │  ]  │  #  │  |  │  ~  │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┘     ├─────┤
//    │     │  $  │  +  │  -  │  (  │  )  │  <  │  >  │  @  │  ?  │  !  │     │     │           │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┐     └─────┘
//    │     │  ^  │  *  │  _  │  %  │  &  │     │     │  \  │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┴─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┐
//    │     │     │     │           │     │     │     │     │     │     │     │     │     │     │
//    └─────┴─────┴─────┘           └─────┴─────┴─────┘     └─────┴─────┘     └─────┴─────┴─────┘
[_SBL] = LAYOUT(
  _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______          ,
  _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______ , _______,
  _______ , KC_GRV  , KC_QUOT , KC_DQT  , KC_LCBR , KC_RCBR , KC_LBRC , KC_RBRC , KC_HASH , KC_PIPE , KC_TILD , _______ , _______ , _______ ,           _______,
  _______ , KC_DLR  , KC_PLUS , KC_MINS , KC_LPRN , KC_RPRN , KC_LABK , KC_RABK , KC_AT   , KC_QUES , KC_EXLM , _______ , _______ ,                     _______,
  _______ , KC_CIRC , KC_ASTR , KC_UNDS , KC_PERC , KC_AMPR , _______ , _______ , KC_BSLS , _______ , _______ , _______ , _______ , _______                    ,
  _______ , _______ , _______ ,                     _______ , _______ , _______ ,           _______ , _______ ,           _______ , _______ , _______
),

//    ┌──────┬──────┬──────┬────┬────┬─────┬─────┬─────┬────┬──────┬─────┬─────┬──────┬──────────┬──────┐
//    │ esc  │  f1  │  f2  │ f3 │ f4 │ f5  │ f6  │ f7  │ f8 │  f9  │ f10 │ f11 │ f12  │ TG(_DFT) │ mute │
//    ├──────┼──────┼──────┼────┼────┼─────┼─────┼─────┼────┼──────┼─────┼─────┼──────┼──────────┼──────┼─────────┐
//    │  `   │  1   │  2   │ 3  │ 4  │  5  │  6  │  7  │ 8  │  9   │  0  │  -  │  =   │   bspc   │ bspc │   del   │
//    ├──────┼──────┼──────┼────┼────┼─────┼─────┼─────┼────┼──────┼─────┼─────┼──────┼──────────┼──────┼─────────┤
//    │ tab  │  q   │  w   │ e  │ r  │  t  │  y  │  u  │ i  │  o   │  p  │  [  │  ]   │    \     │      │ QK_BOOT │
//    ├──────┼──────┼──────┼────┼────┼─────┼─────┼─────┼────┼──────┼─────┼─────┼──────┼──────────┘      ├─────────┤
//    │ caps │  a   │  s   │ d  │ f  │  g  │  h  │  j  │ k  │  l   │  ;  │  '  │ ent  │                 │  pgdn   │
//    ├──────┼──────┼──────┼────┼────┼─────┼─────┼─────┼────┼──────┼─────┼─────┼──────┼──────────┐      └─────────┘
//    │ lsft │ lsft │  z   │ x  │ c  │  v  │  b  │  n  │ m  │  ,   │  .  │  /  │ rsft │    up    │
//    ├──────┼──────┼──────┼────┴────┼─────┼─────┼─────┼────┼──────┼─────┼─────┼──────┼──────────┼──────┐
//    │ lctl │ lgui │ lalt │         │ spc │ spc │ spc │    │ ralt │     │     │ left │   down   │ rght │
//    └──────┴──────┴──────┘         └─────┴─────┴─────┘    └──────┴─────┘     └──────┴──────────┴──────┘
[_DFT] = LAYOUT(
  KC_ESC  , KC_F1   , KC_F2   , KC_F3 , KC_F4 , KC_F5  , KC_F6  , KC_F7  , KC_F8 , KC_F9   , KC_F10  , KC_F11  , KC_F12  , TG(_DFT) , KC_MUTE           ,
  KC_GRV  , KC_1    , KC_2    , KC_3  , KC_4  , KC_5   , KC_6   , KC_7   , KC_8  , KC_9    , KC_0    , KC_MINS , KC_EQL  , KC_BSPC  , KC_BSPC  , KC_DEL ,
  KC_TAB  , KC_Q    , KC_W    , KC_E  , KC_R  , KC_T   , KC_Y   , KC_U   , KC_I  , KC_O    , KC_P    , KC_LBRC , KC_RBRC , KC_BSLS  ,            QK_BOOT,
  KC_CAPS , KC_A    , KC_S    , KC_D  , KC_F  , KC_G   , KC_H   , KC_J   , KC_K  , KC_L    , KC_SCLN , KC_QUOT , KC_ENT  ,                       KC_PGDN,
  KC_LSFT , KC_LSFT , KC_Z    , KC_X  , KC_C  , KC_V   , KC_B   , KC_N   , KC_M  , KC_COMM , KC_DOT  , KC_SLSH , KC_RSFT , KC_UP                        ,
  KC_LCTL , KC_LGUI , KC_LALT ,                 KC_SPC , KC_SPC , KC_SPC ,         KC_RALT , _______ ,           KC_LEFT , KC_DOWN  , KC_RIGHT
)
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE] =  { ENCODER_CCW_CW(C(A(KC_F2)), C(A(KC_F1))) },
    [_NAV] =  { ENCODER_CCW_CW(_______, _______), },
    [_NBR] =  { ENCODER_CCW_CW(_______, _______), },
    [_SBL] =  { ENCODER_CCW_CW(_______, _______), },
    [_DFT] =  { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), },
};
#endif
// clang-format on
