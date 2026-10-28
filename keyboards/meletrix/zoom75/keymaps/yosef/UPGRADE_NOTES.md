# What changed from QMK 0.28.3 to 0.34.7

The old custom branch was based just after QMK 0.28.3 (April 2025). The new
baseline is upstream 0.34.7 (October 2026). The tag-to-tag range contains 1,160
commits, including hardware additions and changes irrelevant to this keyboard.
This summary focuses on major feature groups and practical gains for Zoom75.

## Relevant now

- **Current AVR toolchain compatibility.** Upstream added GCC 15 compilation
  fixes and an AVR-libc 2.3.0 compatibility fix. The latter supplies the AVR
  register definitions missing in the old checkout's `tap_random_base64()`;
  this explains why the old checkout failed on `TCNT0`/`TCNT1`/`TCNT3`/`TCNT4`.
  The upgrade builds on the installed AVR GCC 16.2.0 without dependency changes.
- **USB/host behavior.** Wake-up keypress suppression was added, and the System
  Control HID usage range was narrowed to avoid some hosts identifying QMK
  keyboards as gamepads. Zoom75 enables Extra Keys, so the descriptor change
  applies here; it does not imply a previously observed gamepad issue was fixed.
- **Shared core fixes.** Examples include Caps Word/Repeat Key ignoring Layer
  Lock, Repeat Key working with Key Overrides, retro-tapping modifier cleanup,
  and an infinite-recursion fix in Repeat Key. Each matters only when the
  affected features are used together; not all are enabled in this keymap.
- **Our separate autocorrect fix.** The recovered 48-byte correction buffer and
  bounds checks are local changes, not a benefit supplied by upstream 0.34.7.
  The unsafe upstream copy still existed in that baseline. Synthetic tests now
  cover long replacements and overflow boundaries without private data.

## New options for the later Achordion migration

- **Flow Tap** disables holds during rapid typing and can send taps immediately.
  Later fixes improve distinct taps and rolls across multiple tap-hold keys.
  This is the new native option most relevant to typing-streak behavior.
- **Speculative Hold** applies a mod-tap's modifier immediately while the tap/hold
  decision is pending, cancelling it if the key resolves as a tap. This targets
  actions such as Shift+click and Ctrl+scroll with an external mouse. Later
  options constrain it to one key or require a preceding idle period.
- **Chordal Hold already existed in 0.28.3.** It is not new in this upgrade.
  Combined with the newer options, it gives us a more complete native toolbox
  for replacing Achordion. Behavioral equivalence still needs testing.

None of these native options is enabled by this migration. The customized
Achordion implementation, including overlapping E/U/I taps with Space and eager
modifiers, remains in use.

## Other major additions and changes

- **Dynamic macros:** `DYNAMIC_MACRO_KEEP_ORIGINAL_LAYER_STATE` can preserve the
  recording layer state and use it during playback. Dynamic macros are enabled
  here, but this new option is not.
- **Community modules:** more pointing/layer/lighting callbacks, custom split
  data synchronization, and persistent per-module data. This can reduce the
  need for copied feature files in future projects; it does not automatically
  convert the current Achordion code into a module.
- **Persistent storage and drivers:** nonvolatile-data infrastructure changes,
  wear-leveling fixes, ChibiOS/ChibiOS-Contrib updates, new sensors, and expanded
  keyboard support. ARM/split improvements are more relevant to a future Iris
  migration than to this AVR Zoom75.
- **VIA:** matrix-test keypress reporting was restricted to mitigate keylogging,
  VIA v13 support was added, and VIA integration began transitioning to a
  community module. VIA is not enabled in this keymap, so these are not active
  benefits here.
- **Steno and lighting:** Plover HID/steno rework, extra LED/RGB Matrix support,
  and animation fixes. These are available upstream but not enabled here.
- **Tooling:** better keymap/alias discovery, diagnostics and validation;
  `qmk compile --compiledb` replaces the old compilation-database command;
  explicit output selection for `generate-autocorrect-data` was fixed.

## Compatibility costs

The range also removes old GPIO aliases, `isLeftHand`, `FORCE_NKRO`, legacy
converter settings, some old RGB/mouse keycodes, and `DEFAULT_FOLDER` handling.
Tap Dance custom state access changed. These may require work when migrating
other keymaps; successful Zoom75 compilation does not validate the Iris branches.

No general typing-speed or latency improvement was measured. New native
features are opt-in. The immediate gain is a tested, toolchain-compatible
baseline that preserves the custom keymap and offers better native options for
the next stage.

## Sources

The release notes and commit history shipped with this checkout were used:

- [May 2025](../../../../../docs/ChangeLog/20250525.md): Flow Tap, module hooks, AVR compiler fixes.
- [August 2025](../../../../../docs/ChangeLog/20250831.md): VIA mitigation and core cleanup.
- [November 2025](../../../../../docs/ChangeLog/20251130.md): Speculative Hold and dynamic macro layer state.
- [February 2026](../../../../../docs/ChangeLog/20260222.md): storage fixes and tooling changes.
- [May 2026](../../../../../docs/ChangeLog/20260531.md): Speculative Hold controls, host/driver fixes.
- [August 2026](../../../../../docs/ChangeLog/20260830.md): persistent modules, steno/HID, Extra Keys descriptor.
- Additional stable-branch commits through 0.34.7, notably `a0b15d08bc` (AVR-libc compatibility).
