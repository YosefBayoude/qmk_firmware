// SPDX-License-Identifier: GPL-2.0-or-later

#include <string>

#include "keycode.h"
#include "test_common.hpp"

namespace {
unsigned    correction_calls;
std::string corrected_word;
std::string replacement_suffix;
}

extern "C" bool apply_autocorrect(uint8_t backspaces, const char *changes, char *typo, char *correct) {
    ++correction_calls;
    corrected_word = correct;
    replacement_suffix = changes;
    return false;
}

class AutoCorrectLongReplacements : public TestFixture {
   public:
    void SetUp() override {
        autocorrect_disable();
        autocorrect_enable();
        correction_calls = 0;
        corrected_word.clear();
        replacement_suffix.clear();
    }

    bool TypeWord(uint16_t letter, bool word_boundary = false) {
        keyrecord_t record = {};
        record.event.pressed = true;
        bool result = true;
        for (unsigned i = 0; i < 4; ++i) {
            result = process_autocorrect(letter, &record);
        }
        if (word_boundary) {
            result = process_autocorrect(KC_SPC, &record);
        }
        return result;
    }
};

TEST_F(AutoCorrectLongReplacements, replacement_of_45_characters_reaches_callback_intact) {
    EXPECT_FALSE(TypeWord(KC_A));
    EXPECT_EQ(correction_calls, 1u);
    EXPECT_EQ(corrected_word, std::string(45, 'x'));
}

TEST_F(AutoCorrectLongReplacements, exact_fit_includes_null_terminator) {
    EXPECT_FALSE(TypeWord(KC_B));
    EXPECT_EQ(correction_calls, 1u);
    EXPECT_EQ(corrected_word, std::string(47, 'x'));
}

TEST_F(AutoCorrectLongReplacements, replacement_without_room_for_terminator_is_skipped) {
    EXPECT_TRUE(TypeWord(KC_C));
    EXPECT_EQ(correction_calls, 0u);
}

TEST_F(AutoCorrectLongReplacements, much_longer_replacement_is_skipped) {
    EXPECT_TRUE(TypeWord(KC_D));
    EXPECT_EQ(correction_calls, 0u);
}

TEST_F(AutoCorrectLongReplacements, retained_prefix_and_suffix_fit_exactly) {
    EXPECT_FALSE(TypeWord(KC_E));
    EXPECT_EQ(correction_calls, 1u);
    EXPECT_EQ(replacement_suffix, std::string(45, 'x'));
    EXPECT_EQ(corrected_word, "ee" + std::string(45, 'x'));
}

TEST_F(AutoCorrectLongReplacements, retained_prefix_counts_toward_buffer_limit) {
    EXPECT_TRUE(TypeWord(KC_F));
    EXPECT_EQ(correction_calls, 0u);
}

TEST_F(AutoCorrectLongReplacements, word_boundary_exact_fit_reaches_callback) {
    EXPECT_TRUE(TypeWord(KC_G, true));
    EXPECT_EQ(correction_calls, 1u);
    EXPECT_EQ(corrected_word, std::string(47, 'x'));
}

TEST_F(AutoCorrectLongReplacements, word_boundary_oversized_replacement_is_skipped) {
    EXPECT_TRUE(TypeWord(KC_H, true));
    EXPECT_EQ(correction_calls, 0u);
}
