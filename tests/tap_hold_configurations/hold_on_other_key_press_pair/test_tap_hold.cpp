// SPDX-License-Identifier: GPL-2.0-or-later

#include "keyboard_report_util.hpp"
#include "keycode.h"
#include "test_common.hpp"
#include "action_tapping.h"
#include "test_fixture.hpp"
#include "test_keymap_key.hpp"

using testing::_;
using testing::InSequence;

// Match the Zoom75 policy: layer-taps hold immediately, ordinary mod-taps do not.
extern "C" bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    return IS_QK_LAYER_TAP(keycode);
}

// Only copy and paste bypass K's normal tap-hold decision timing.
extern "C" bool get_hold_on_other_key_press_pair(uint16_t keycode, keyrecord_t *record, uint16_t other_keycode, keyrecord_t *other_record) {
    if (keycode == RCTL_T(KC_K)) {
        return other_keycode == KC_C || other_keycode == KC_V;
    }
    return get_hold_on_other_key_press(keycode, record);
}

class HoldOnOtherKeyPressPair : public TestFixture {};

TEST_F(HoldOnOtherKeyPressPair, copy_and_paste_hold_ctrl_before_tapping_term) {
    for (uint16_t shortcut : {KC_C, KC_V}) {
        TestDriver driver;
        InSequence s;
        auto       ctrl_k = KeymapKey(0, 1, 0, RCTL_T(KC_K));
        auto       other  = KeymapKey(0, 2, 0, shortcut);

        set_keymap({ctrl_k, other});

        // K alone remains undecided.
        EXPECT_NO_REPORT(driver);
        ctrl_k.press();
        run_one_scan_loop();
        VERIFY_AND_CLEAR(driver);

        // The shortcut press immediately resolves K as Ctrl.
        EXPECT_REPORT(driver, (KC_RCTL));
        EXPECT_REPORT(driver, (KC_RCTL, shortcut));
        other.press();
        run_one_scan_loop();
        VERIFY_AND_CLEAR(driver);

        EXPECT_REPORT(driver, (KC_RCTL));
        other.release();
        run_one_scan_loop();
        VERIFY_AND_CLEAR(driver);

        EXPECT_EMPTY_REPORT(driver);
        ctrl_k.release();
        run_one_scan_loop();
        VERIFY_AND_CLEAR(driver);
    }
}

TEST_F(HoldOnOtherKeyPressPair, ordinary_roll_does_not_hold_ctrl) {
    TestDriver driver;
    InSequence s;
    auto       ctrl_k = KeymapKey(0, 1, 0, RCTL_T(KC_K));
    auto       other  = KeymapKey(0, 2, 0, KC_A);

    set_keymap({ctrl_k, other});

    // Neither press should resolve K as Ctrl.
    EXPECT_NO_REPORT(driver);
    ctrl_k.press();
    run_one_scan_loop();
    other.press();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    // Releasing K first produces an unmodified K+A roll.
    EXPECT_REPORT(driver, (KC_K));
    EXPECT_REPORT(driver, (KC_K, KC_A));
    EXPECT_REPORT(driver, (KC_A));
    ctrl_k.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    EXPECT_EMPTY_REPORT(driver);
    other.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}

TEST_F(HoldOnOtherKeyPressPair, layer_tap_uses_existing_per_key_policy) {
    TestDriver driver;
    InSequence s;
    auto       layer_tap = KeymapKey(0, 1, 0, LT(1, KC_BSPC));
    auto       base_key  = KeymapKey(0, 2, 0, KC_A);
    auto       layer_key = KeymapKey(1, 2, 0, KC_B);

    set_keymap({layer_tap, base_key, layer_key});

    EXPECT_NO_REPORT(driver);
    layer_tap.press();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    // The interrupting key must resolve on the newly activated layer.
    EXPECT_REPORT(driver, (KC_B));
    base_key.press();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    EXPECT_EMPTY_REPORT(driver);
    base_key.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);

    EXPECT_NO_REPORT(driver);
    layer_tap.release();
    run_one_scan_loop();
    VERIFY_AND_CLEAR(driver);
}
