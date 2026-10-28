# Zoom75 migration to QMK 0.34.7

## Baseline

- Upstream: `https://github.com/qmk/qmk_firmware.git`
- Revision: `180f394b489c238cb231c2bf8b080262155e03c0` (`0.34.7`).
- Source keymap: `zoom75-legacy` at `927a42fad8`.
- Upgrade branch: `zoom75`.
- Active worktree: `/home/yosef/dev/projects/qmk_firmware-zoom75-upgrade`.
- Legacy worktree: `/home/yosef/dev/projects/qmk_firmware`.

The five original keymap and Achordion source files were copied unchanged,
including their formatting, callbacks, and custom overlapping-tap handling.
The subsequent autocorrect fix adds a buffer setting to `config.h`; the other
four source files remain unchanged.
Chordal Hold, Flow Tap, and Speculative Hold are not enabled by this migration.
Replacing Achordion is a separate change.

## Core extension

`quantum/action_tapping.c` and `quantum/action_tapping.h` retain the existing
`get_hold_on_other_key_press_pair()` extension. With
`HOLD_ON_OTHER_KEY_PRESS_PER_KEY`, the callback receives the interrupting key.
The weak default delegates to the original per-key callback.

The keymap uses this to resolve K+C and K+V as Ctrl shortcuts without making
all other keys interrupt K immediately. Keep this extension when updating QMK,
unless the keymap policy is deliberately replaced with a native mechanism.

## Recovered autocorrect crash fix

The local stash `37752ae99f` (`new autocorrect and autocorrect fix crash fix`)
contained an uncommitted fix for long autocorrect replacements. Its private
files were not applied.

`config.h` now sets `AUTOCORRECT_CORRECTION_BUFFER_SIZE` to 48 bytes. This fits
an assembled corrected word of up to 47 characters plus its null terminator.
`quantum/process_keycode/process_autocorrect.c` checks the retained prefix and
replacement suffix before copying. An oversized correction is skipped, leaving
the typed text unchanged instead of overflowing the stack or truncating output.
The fallback buffer size for other keymaps remains unchanged.

## Build

From the upgrade worktree:

```sh
make meletrix/zoom75:yosef -j2
```

Validated with AVR GCC 16.2.0 and the existing QMK CLI 1.2.0 Python environment.
No system packages, Python package versions, or dependency pins were changed.
Submodules use the revisions selected by upstream QMK, not the old branch.

The initial clean build completed without warnings. The rebuild with the
recovered autocorrect fix also completed without warnings. Firmware size:
24,616 / 28,672 bytes (4,056 bytes free). Output: `meletrix_zoom75_yosef.hex`.

The existing private `autocorrect_data.h` was copied locally for compilation.
Both private files were subsequently restored from the verified backup at the
operator's request. The dictionary was not inspected. Neither private file
belongs in Git. Generated firmware also contains private data and
must not be published. A fresh clone needs the private generated header before
building this keymap with the same autocorrect behavior.

## Validation

All 303 tests across 19 tap-hold configurations passed, including the new
`hold_on_other_key_press_pair` regression suite. It checks immediate Ctrl+C/V,
an ordinary K+A roll without Ctrl, and fallback to the layer-tap per-key policy.
These tests exercise the core extension, not the complete Achordion/keymap
integration.

Run the pair-aware regression tests with:

```sh
make test:tap_hold_configurations/hold_on_other_key_press_pair -j2
```

The 6 existing autocorrect tests and 8 new long-replacement regression tests
also passed, both normally and under AddressSanitizer/UndefinedBehaviorSanitizer:

```sh
make test:autocorrect test:autocorrect_long_replacements -j2
make test:autocorrect test:autocorrect_long_replacements -j2 \
  BUILD_DIR=.build/autocorrect-sanitized \
  EXTRAFLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' \
  EXTRALDFLAGS='-fsanitize=address,undefined'
```

The new tests generate synthetic correction data under `.build/`; no dictionary
file is read. They cover 45-character replacements, exact-fit/null-terminator
boundaries, oversized replacements, retained prefixes, and word boundaries.

## Hardware checks still required

No firmware was flashed during migration. Before replacing the old firmware:

- Verify ordinary taps, held modifiers, and quick typing rolls.
- Verify K+C and K+V, including during a typing streak.
- Verify E/U/I overlapping Space, including both release orders and no stuck keys.
- Verify eager Shift/Ctrl/Alt with keyboard shortcuts and the mouse.
- Verify navigation/number layer-taps, per-key timing, and repeated taps.
- Verify encoder, Caps Word, dynamic macros, and private autocorrect behavior.

The original code is preserved on `zoom75-legacy` in its original worktree as
the rollback source. The initial upgraded draft was reported working on hardware
by the operator, but complete feature coverage and hardware verification of the
autocorrect fix remain pending. Keep a known-good old firmware image before
flashing the upgrade.
