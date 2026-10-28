# SPDX-License-Identifier: GPL-2.0-or-later

AUTOCORRECT_ENABLE = yes

# Keep generated autocorrect data out of version control, even for test fixtures.
AUTOCORRECT_FIXTURE_DIR := $(BUILD_DIR)/autocorrect_long_replacements
AUTOCORRECT_FIXTURE := $(AUTOCORRECT_FIXTURE_DIR)/autocorrect_data.h
VPATH += $(AUTOCORRECT_FIXTURE_DIR)

$(AUTOCORRECT_FIXTURE): $(TEST_PATH)/generate_fixture.py
	mkdir -p $(dir $@)
	python3 $< $@

$(TEST_OBJ)/$(TEST_OUTPUT)/quantum/process_keycode/process_autocorrect.o: $(AUTOCORRECT_FIXTURE)
