# Inertia Player rewrite status

This directory contains the current handwritten C rewrite path for `IPLAY.EXE`.

## Current contract

- `iplay_rewrite.c` contains pure C behavior helpers.
- `iplay_player.c` contains the DOS player entry path and remains pure C.
- `rewrite_runner.c` contains the host/DOS test runner model and remains pure C.
- `iplay_abi_watcom.c` contains OpenWatcom public-symbol ABI glue.
- Register/inline-assembly ABI work is isolated to `iplay_abi_watcom.c`.
- Some hardware/no-device MIDI/SB/SB16 wrappers intentionally remain inline ABI shims because routing those hot startup paths through ordinary C calls broke DOS smoke tests.
- Adding more public ABI symbols to `iplay_abi_watcom.c` can change DOS player startup/link behavior even when standalone ABI runners pass. Add such wrappers only in small steps and keep the full smoke gate authoritative.
- The OpenWatcom small-model DOS link is close to the 64 KiB `_TEXT` limit; extra helper extraction can fail the link, and replacing inline byte stores with static data plus `memcpy` can pass link but break DOS smoke. Keep such changes small and smoke-tested.
- `iplay_rewrite.c` behavior helpers now stage register-visible outputs through central `apply_*` helpers; direct `r->... = ...` writes outside those helpers have been removed, leaving ordinary C locals for internal work.
- The active `sub_15577` channel mixer now has a register-free pointer-based C implementation. Original-binary parity compares mixed bytes and channel state across narrow and wide accumulation, self-modifying interpolation, non-looping sample end, and loop wrapping.
- Loaded and post-playback 80-column screens now use the original bordered live layout without the duplicate diagnostic panel. A valid MOD forced-HLT capture verifies original B800 coordinates and attributes for the `SMOKE MOD` title, filename, `462KB`, `M.K.`, 4 channels, 2/31 samples, and stable controls; the out-of-scope legacy driver header is intentionally replaced with `Sound Blaster 16 (44kHz)`. Post-playback redraw adds only the required changing output-level meter to the otherwise empty center. The 40-column modes retain their compact fallback because the original right-hand panel cannot fit.
- The preferred SDL terminal player now redraws the complete selected original-layout screen during playback at a bounded cadence instead of replacing it with a one-line level meter. The explicit `iplay_native --terminal-live` probe retains its compact machine-readable meter. Multi-frame tests prove repeated title, track, position, and SB16 panels; PTY coverage proves a live 80x25-to-80x50 resize switches subsequent frames to the resized mode.
- A second bounded original harness jumps over unsupported pre-draw hardware status callbacks and traps after the first `offs_draw`. It verifies `Current Track : 1/5`, `Track Position: 1/64`, four original channel prefixes, and all 30 meter glyph positions per row. The preferred SDL player carries libmodplug order, pattern, row, channel, note, instrument, volume/effect, speed, tempo, sample-name, and sample-count state through the audio bridge into that original 80-column layout while libmikmod supplies PCM for S3M and other non-MOD external formats. MOD/NST retains libmodplug PCM because its tested F9/F10 compatibility transformations patch libmodplug's decoded commands. Per-channel note, instrument/sample name, volume command, and effect state persist across sparse pattern rows so the display follows active tracker voices instead of blanking at every empty command. A deterministic MOD fixture verifies that retention after playback advances beyond the event row. MOD notes are normalized to the original octave convention and known effects are rendered with original-style names. Channel meters retain the original 30-cell geometry. For libmikmod-backed PCM, dynamically loaded `Voice_RealVolume` and `Voice_GetVolume` provide independent real mixer-voice levels; libopenmpt remains the command/pan telemetry source and supplies velocity-based VU only for the MOD/NST libmodplug path. No libopenmpt or libmikmod development package is required because both runtime APIs are loaded dynamically. If libopenmpt telemetry is unavailable, the renderer retains its tested stereo-peak fallback. The 40-column modes retain the compact `Output Levels` fallback.
- The original `f3_draw` meter envelope is now reproduced from the listing: raw channel magnitude clamps at 60, rises are applied immediately, lower values are peak-held, decay subtracts 3 every 32 draw calls, and the displayed width is the raw value shifted right once for an exact 0..30-cell range. This replaces the previous rounded 0..15 level followed by a two-cell multiplier, which could not render odd widths. Listing-contract and deterministic rise/hold/decay tests guard the constants and cadence. For libmikmod-backed playback, raw magnitude now derives from the active voice's 64-point peak-to-peak sample measurement, current mixer volume, the original 80-sample window, and the listing's integer `317/channel_count` divisor. Deterministic scale tests and real ARYX playback guard nonzero activity, clamping, and channel-count scaling. Exact sample-for-sample magnitude equality remains unproven because libmikmod exposes peak-to-peak voice measurements rather than the original private 80-byte signed mixer buffer.
- The current verified gate is `1333` passing tests plus `./iplay.sh --rebuild --check-playback samples/aryx.s3m`, which proves the rebuilt user-facing binary opens the SDL-compatible SB16 16-bit stereo path, continuously renders the selected original-layout terminal screen with the original meter envelope and live F1-F6/F8 behavior, routes the real S3M through libmikmod, supports pause/resume without advancing playback, and performs immediate original-shaped audio shutdown.
- The preferred terminal input path implements `q`, `Q`, and a lone Escape as stop controls. CSI/SS3 function-key sequences are parsed as complete sequences instead of treating their leading Escape as quit. A clean bridge-owned decoder-control object drives all displayed playback controls without globals: `-`/`+` master volume, `[`/`]` amplification in the original 50..2500 percent range, original arrow navigation (left/right by two rows and down/up by ten rows), original three-bank numeric mute for channels 1..30 (digits, Shift symbols, and terminal Alt-digits), `,`/`.` selected-channel navigation, `<`/`>` one-step pan, `{`/`}` eight-step pan, original `L`/`M`/`R` direct left/center/right presets, original `S` surround preset, ScrollLock/`C` current-pattern looping, End/`E` next-pattern seeking, Tab PAL/NTSC timing, F11 module restart/looping, F12 nearest/linear interpolation, F9 ProTracker vibrato depth, and F10 ignore-BPM handling. Tab preserves the fixed 44.1 kHz SDL/SB16 contract while changing decoder mixer stepping from PAL `44100` to NTSC `43698`, derived from the inverse 7.15909/7.09379 MHz clock ratio; lockstep libopenmpt telemetry uses the same effective rate, and the original bottom row switches between `(PAL)` and `(NTSC)`. Pattern looping is enforced before SDL submission: a block that crosses orders is discarded, the decoder seeks to the captured order's row zero, and replacement PCM is rendered. Numeric mute and pan are backed by libmikmod's real per-channel mixer controls, not post-mix approximations; mute application covers 32 mixer channels independently of the intentionally 10-row visible channel panel, and the original internal surround value `166` maps to libmikmod's surround constant. Effective gain is applied deterministically to signed 16-bit stereo PCM with saturation instead of relying on decoder-specific runtime master-volume behavior. Arrow and End navigation synchronize the PCM provider with libmodplug command state and dynamically loaded libopenmpt telemetry before the next audible block. F9/F10 behavior is derived from the original listing: ProTracker mode adds the original extra vibrato-depth shift for MOD effects `4xy`/`6xy`, while ignore-BPM converts MOD `Fxx > 20h` decoded tempo commands to speed commands without losing the original parameter. Mode changes reload the MOD decoder at the current order; unrelated control changes do not restart it. Tests prove PAL and NTSC produce different real PCM, arrow navigation changes decoded PCM at the listing-defined increments, pattern loop remains in its captured order through block limit, End advances to the next order, channel 11 mute changes real 12-channel S3M PCM, channels 21-30 retain correct masks on smaller modules, all four original pan presets change real PCM, and F11 preserves the full submitted-block limit across restart. The bottom/status fields are rendered after channel rows so channel text cannot overwrite their original right-hand values.
- `P` or Space now toggles the original pause state through the terminal callback. While paused, no decoder read or SDL submission occurs, the original-style `Paus` status is rendered, keyboard polling remains active, and resume continues from the same source position. A bounded `pause/resume/quit` test proves playback advances again after resume and that UI-only/view/pause actions do not increment the bridge audio-control generation.
- Original view keys now have decoder-independent terminal state. Both SS3 and CSI encodings are parsed atomically. F1 paints the original help heading and function descriptions through the notcurses-style cell plane during live playback; F2 renders an 80-sample waveform from the current decoded stereo PCM block; F3 restores the realtime VU screen; F4 renders the decoder's complete sample-name inventory and repeated F4 advances by the original `word_1DE6E - 1` page stride of 11 samples before wrapping; F5 renders a 32-band spectrum from an optimized 256-point radix-2 FFT; F6 reproduces the original undocumented per-channel L/M/R/S panning display using persistent pan state derived from each channel's live left/right telemetry ratio plus explicit original presets, with tracker-compatible defaults before activity; F8 displays the original shell heading, suspends in a host shell, and redraws/resumes playback after exit. Noninteractive F8 only launches when `IPLAY_SHELL_COMMAND` is explicitly supplied, preventing unattended hangs; an interactive terminal defaults to `$SHELL` or `/bin/sh`. Listing-contract tests pin repeated F4 paging, F6 `fs:[bx+3Ah]` panning semantics, and F8 COMSPEC execution plus working-directory restoration. Deterministic tests prove exact left/center/right/surround behavior, silent-state retention, valid live pan snapshots, nonzero waveform/spectrum data, every implemented screen, multi-page sample names, and shell resume before keyboard stop.
- Host SDL shutdown now follows the original `snd_offx` intent instead of draining stale queued audio: pause the device immediately, clear queued PCM, close the device, then quit the SDL audio subsystem. This removes the previous real-device queue-drain delay and prevents audio continuing after keyboard/block-limit exit. Diagnostics expose `paused=1 queue_cleared=1 closed=1`; listing-order tests pin original `snd_offx` before `snd_deinit`, and runtime tests prove the SDL sequence on both block-limit and keyboard exits.
- The remaining `r->...` uses in `iplay_rewrite.c` are restricted to local `abi_*` input accessors and central `apply_*` output staging helpers; inventory tests guard this boundary so rewritten behavior helpers do not read/write `IplayRegs` fields directly.
- Hex/decimal formatting, string/text-copy, message-frame, screen-stream, text recolor, mouse helper, interrupt-action, EMS config, EMS fallback, MOD delta/event packing, loader-result packing, stream-scan, memfree guard, zero-event packing, small DMA-fill, no-device timer/video helpers, INR sprintf/numeric formatting, no-device MIDI/SB16/DMA/DOS helper, interrupt-vector, deinit/RTC, loader/start, text/graph setup, spectrum, legacy command-line, environment, playsettings, volume/playstate, memclean, channel-count setup, event guard, device-message, timer-config, set-playsettings, effect-processing, MIDI, mix-setup, sample-fill, volume-prep, SB/no-device, DMA-fill, sndoff-fill, and sound-settings ABI wrappers now read register inputs through the local `abi_eax`/`abi_ebx`/`abi_ebp`/`abi_ecx`/`abi_edx`/`abi_esi`/`abi_edi` accessors before calling pure helpers. This keeps another rewritten-function batch off direct `IplayRegs` field reads while preserving the public original ABI glue.

## Current playback status

`IPLAYC.EXE` is not yet a full replacement player because tracker formats such as S3M/MOD are not yet decoded by a real library-backed renderer in the DOS player path. The current module path loads supported fixture headers, selects the intended loader boundary, initializes the VGA/text and SB16 wrapper seams, and runs the quiet continuous timer/keyboard playback policy. `IPLAYDIAG.EXE` is the bounded diagnostic player for unattended metadata and fallback-PCM checks.

For the immediate SDL/notcurses host-player goal, `.inr` playback is deferred by scope. The modern player still classifies `.inr` as `project-owned` and reports `project-decoder-unavailable`, so it is not confused with the libmodplug-backed external tracker path; this is intentional until `.inr` is brought back into scope.

Playback is represented by a `PlayerPlaybackLoop` policy object instead of being embedded directly in the source-drain loop. `IPLAYC.EXE` is linked against `iplay_player_cont_zm.obj`, compiled with `IPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS` and `IPLAY_PLAYER_ENABLE_DIAGNOSTICS=0`, so the production binary uses quiet continuous playback with real SB16 probing. `IPLAYTRY.EXE` is linked against `iplay_player_try_zm.obj`, compiled with the same quiet continuous policy but wrapper SB16 I/O, so it remains the explicit quiet kvikdos trial binary for exercising module loading, text UI, and playback without real SB16 hardware when requested with `--quiet`. `IPLAYDIAG.EXE` keeps the default bounded diagnostic policy (`mode=playback policy=bounded-trial cadence=immediate max_blocks=32 frames/block=512`) and is the bounded diagnostic wrapper-SB16 trial binary used by default from `try_player.sh`. `IPLAYCONT.EXE` is the diagnostic continuous-policy build for targeted continuous-loop evidence and is only selected explicitly with `--continuous-diagnostics` or `IPLAY_TRIAL_EXE=IPLAYCONT.EXE`. Continuous policy reports `cadence=timer`, creates a stateful `PlayerPlaybackTimer`, reads DOS BIOS tick state through the `DosHardwareIo` wrapper (`0040:006C`), primes the first block immediately, then waits for ticks between later blocks. Production continuous playback uses 1024-frame blocks, filling the 4096-byte SB16 DMA buffer for 16-bit stereo instead of underfeeding the hardware with 512-frame blocks. Keyboard polling for continuous playback also goes through `DosHardwareIo`, with `IPHWRUN.EXE playerkeyboardhw` proving the predicate seam and `IPHWRUN.EXE playerkeyboardstophw` proving a continuous loop can submit an SB16 block and exit with `stop=keyboard`.

`IPLAYHW.EXE` is a diagnostic real-SB16 variant linked against `iplay_player_hwdiag_zm.obj`. It keeps diagnostics enabled while using `IPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1`, so kvikdos can cover the real-hardware unavailable UI path (`Screen present: reason=audio-unavailable ...`) without making quiet production `IPLAYC.EXE` noisy.

The user-facing `IPLAYC.EXE` object is compiled with `IPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1`, so its SB16 backend uses real DSP/DMA port I/O instead of the `kvikdos` wrapper-counting path. Diagnostic/test binaries keep wrapper I/O because `kvikdos` does not emulate SB16 hardware.

The quiet real-hardware player now probes SB16 readiness before entering the playback pump. If the real DSP reset/probe fails, as it does under `kvikdos`, `IPLAYC.EXE` skips the continuous module-drain loop, renders `Playback disabled: SB16 not detected`, and exits with `IPLAY_PLAYER_EXIT_AUDIO_UNAVAILABLE` instead of relying on the outer subprocess timeout. Diagnostic binaries still use wrapper I/O and keep deterministic bounded playback coverage.

Player iteration no longer requires the full monolithic ABI/test-runner rebuild. `rewrite/build_player.sh` incrementally rebuilds only stale DOS player support objects and relinks only stale `IPLAYC.EXE`, `IPLAYTRY.EXE`, `IPLAYDIAG.EXE`, `IPLAYCONT.EXE`, and `IPLAYHW.EXE`, keeping the slow full `rewrite/build_rewrite.sh` path available for complete gates. Link files are only rewritten when their content changes, so an unchanged `.lnk` file does not force a redundant relink.

`rewrite/try_player.sh [--rebuild] [--modern|--native|--native-interactive|--native-source-end|--native-keyboard-after-one|--native-stdin-keyboard|--native-audio|--native-terminal|--native-live|--quiet|--diagnostics|--continuous-diagnostics|--hardware-diagnostics|--production] [--blocks=N] [--video-mode=MODE] <module-file|@file-list>` is the user-facing trial launcher. It fails before rebuilding if the host module path is missing, resolves original-shaped `@file-list` arguments by first finding the list file with DOS-style case-insensitive path matching and then selecting the first non-empty trimmed module path relative to the resolved list file, uses the selected module for requested-module and size evidence, passes the resolved absolute `@file-list` argument through to the selected native host executable (`iplay` for `--modern`, `iplay_native` for legacy native flags) so the native resolver is also tested, uses the player-only rebuild only when `--rebuild` is requested, the selected trial executable is missing, or the main player sources/shared header are newer than the selected executable, refuses to launch if the selected trial executable is still stale after that rebuild, copies the first host module path into `rewrite/.build`, skips that copy when the module is already in `rewrite/.build`, and runs `IPLAYDIAG.EXE --blocks=32` by default so module loading, text UI, diagnostic stdout, and the SB16 wrapper seam are exercised without waiting for a quiet or continuous-player timeout under headless `kvikdos`. The preferred SDL/notcurses host player path is `./iplay.sh samples/aryx.s3m`; it delegates to the implementation launcher `./rewrite/iplay.sh samples/aryx.s3m`, which rebuilds `rewrite/.build/iplay` when missing or stale, includes the host C++ headers in its stale-build dependency check, streams the terminal UI live, hides diagnostic evidence by default, maps normal keyboard/block-limit stops to success, preserves stderr for real errors, and selects the nearest supported text mode from the terminal size when `--video-mode` is omitted. Readiness checks are available through `./iplay.sh --check` or the implementation launcher `./rewrite/iplay.sh --check`, which rebuilds if needed, confirms `rewrite/.build/iplay` is executable, starts it in no-playback extension-list mode to catch SDL/libmodplug link failures, and exits before playback. The quick playback proof command is `./iplay.sh --check-playback samples/aryx.s3m`; it uses controlled 40x25 geometry, SDL dummy audio by default, and a one-block keyboard-stop run to prove SDL-compatible SB16 stereo output, notcurses-style terminal rendering, and `route=external-library provider=libmodplug` playback without printing raw diagnostics on success. The same check accepts original-shaped `@file-list` input and has explicit failure coverage for missing modules, missing selected file-list entries, corrupt known tracker data, unsupported probe-by-content files, project-owned decoder-unavailable files such as deferred `.inr`, and SDL audio open failure; the implementation launcher `./rewrite/iplay.sh --check-playback` has matching real-module and `@file-list` success coverage plus missing-module, missing file-list selection, corrupt known tracker, unsupported probe, project-owned decoder-unavailable, and SDL audio-open failure coverage. Supported external-library tracker extensions can be listed without playback through `./iplay.sh --list-extensions`. Decoder routing for a specific path can be checked without playback through `./iplay.sh --classify <path>`. Forced rebuilds are available through `./rewrite/iplay.sh --rebuild samples/aryx.s3m`. Raw evidence is available with `./rewrite/iplay.sh --diagnostics --video-mode=80x50 samples/aryx.s3m` or `IPLAY_LAUNCHER_DIAGNOSTICS=1`. The top-level diagnostics path `./iplay.sh --diagnostics --video-mode=80x50 <real-s3m>` is directly tested with SDL dummy audio and proves the preferred command reaches selected 80x50 geometry, terminal render evidence, opened SDL-compatible SB16 16-bit stereo output, libmodplug route/provider evidence, stdin-keyboard status, and nonzero/changing live audio levels. The lower-level diagnostic host binary remains `./rewrite/.build/iplay --video-mode=80x50 samples/aryx.s3m`, and the try-player equivalent is `./rewrite/try_player.sh --modern --video-mode=80x50 samples/aryx.s3m`, which launches `rewrite/.build/iplay`; the bounded legacy native proof path remains `./rewrite/try_player.sh --native --blocks=1 --video-mode=80x50 samples/aryx.s3m`, and the legacy audible/visible native alias remains `./rewrite/try_player.sh --native-interactive --video-mode=80x50 samples/aryx.s3m`, which opens a real SDL2 queued-audio sink before feeding the same SB16 16-bit stereo PCM path and paints the final notcurses-style text cells to the host terminal with ANSI 16-color output, and updates ANSI audio level meters from the playback callback while blocks are submitted, and stops immediately on q/Q/Escape through raw stdin when requested; `--modern` builds/runs `rewrite/.build/iplay` with normal player arguments (`--video-mode=MODE <module>` or `<module> <video-mode>`), while legacy native flags build/run `rewrite/.build/iplay_native` with explicit probe arguments such as `--source-end`, `--sdl-audio`, `--terminal-render`, `--terminal-live`, and `--stdin-keyboard`; both paths route libmodplug PCM through the SDL-compatible SB16 16-bit stereo bridge, renders through the notcurses-style text runtime, records selected text-mode geometry evidence, proves an 80x25-to-80x50 resize cycle on the same runtime, proves subwindow drawing over a live playback status runtime, proves all 16 foreground color nibbles, all 8 background color nibbles, and the blink bit survive a notcurses-style cell-buffer present through `trial_color_probe_valid=yes`, proves changing audio-level samples across a 16-block SB16 sequence, and writes `trial_result=native-sdl-notcurses-ok` on success. The native executable also accepts original-shaped `@file-list` module arguments, selecting the first non-empty trimmed module path relative to the list file before playback. The native launcher accepts DOS-style case-insensitive text-mode aliases directly in `iplay`/`iplay_native` as well as through `try_player.sh`. Direct `iplay` defaults to modern SDL/notcurses playback without `--modern`, while `iplay_native` keeps probe-style defaults; direct `iplay`/`iplay_native` also accepts `--blocks=N` as an alias for the bare max-block count, keeping direct host trials aligned with the wrapper bounded playback option. The native launcher also accepts `--native-source-end` to run until libmodplug reports natural source end, producing `trial_loop_policy=native-source-end` and `status=ok ... stop=source-end` evidence without entering `kvikdos`; `--native-keyboard-after-one` runs the same host path until the keyboard/interactive stop seam fires after one submitted block, producing `trial_loop_policy=native-keyboard-stop` and `status=keyboard` evidence. The script accepts `--quiet` before or after the module path to run `IPLAYTRY.EXE` for quiet continuous wrapper-SB16 playback, accepts `--diagnostics` before or after the module path to run `IPLAYDIAG.EXE` for visible bounded wrapper-SB16 diagnostic stdout, accepts `--continuous-diagnostics` before or after the module path to run `IPLAYCONT.EXE` for visible continuous-loop diagnostics, accepts `--hardware-diagnostics` before or after the module path to run `IPLAYHW.EXE` for real-SB16 probe/unavailable diagnostics, accepts `--production` before or after the module path to run `IPLAYC.EXE` as the quiet production real-SB16 DOS player, accepts `--blocks=N` before or after the module path for diagnostic/native bounded modes, and accepts DOS-style case-insensitive `--video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50` before or after the module path so existing text-mode constants can be exercised without rebuilding. Trial mode aliases `40x25mono`, `40x25`, `80x25mono`, `80x25`, and `80x50project` normalize to the corresponding canonical trial geometry, and explicit `80x25color` is accepted instead of being confused with the omitted-mode default. Diagnostic mode defaults to `--blocks=32` through `IPLAY_TRIAL_DIAGNOSTIC_BLOCKS` when the user does not supply a block limit, avoiding accidental long diagnostic playback. `IPLAY_TRIAL_VALIDATE_ONLY=1` runs the wrapper parser, selected trial executable, and video-mode normalization, prints `trial_exe=... diagnostics=... hardware_diagnostics=... production=... native=... native_source_end=... native_keyboard_after_one=...`, file-list evidence when applicable, `trial_video_mode=... cols=... rows=...` plus `dos_args=...`, and exits before rebuild/copy/emulator/native launch; this is a no-run guardrail and is not playback proof. Because `kvikdos` is headless and only supports memory dumps through the call-HLT harness, the DOS modes write a deterministic header with `host_module=... dos_module=...`, `trial_exe=... diagnostics=... hardware_diagnostics=... production=... rebuild=... needs_rebuild=...`, `trial_exe_path=...`, `audio_mode=wrapper-sb16-kvikdos-not-audible` for wrapper trial binaries or `audio_mode=real-sb16-hardware` for `IPLAYC.EXE`/`IPLAYHW.EXE`, `trial_loop_policy=...` identifying bounded diagnostics, continuous diagnostics, quiet wrapper playback, or real-SB16 playback, `trial_proof_scope=...` identifying playable wrapper proof, production real-SB16 proof, hardware-unavailable probe, or custom override, normalized expected `trial_video_mode=... cols=... rows=...`, `dos_args=...`, `trial_mode_note=quiet-player-no-diagnostic-stdout` for quiet runs, and `kvikdos_timeout_seconds=...` before stdout/stderr. It appends `kvikdos_timeout=yes seconds=...` when the emulator is killed by timeout, labels quiet timeouts as `quiet_trial_timeout=yes meaning=headless-run-ended-by-timeout-not-by-player-exit`, labels successful quiet exits as `quiet_trial_completed=yes meaning=player-exited-without-diagnostic-stdout`, and always appends `trial_exe=... exit_status=...` to `rewrite/.build/RES.TXT`. The production `IPLAYC.EXE` real-SB16 path remains unchanged.

Explicit unsupported trial video modes now fail before launching `kvikdos` with `try_player: unsupported video mode: ...`; only an omitted `--video-mode` defaults to 80x25 color. This prevents typoed mode names from silently producing misleading default-geometry evidence.
`tests/test_player_smoke.py::test_try_player_rejects_invalid_video_mode_before_kvikdos` covers this wrapper-level early-fail path with `--video-mode=bad`, proving no trial log is written and `kvikdos` is not launched.
`tests/test_player_smoke.py::test_try_player_rejects_invalid_video_mode_after_module_before_kvikdos` covers the same early-fail path when `--video-mode=bad` is supplied after the module path, preserving the wrapper's before-or-after option contract without entering `kvikdos`.
`tests/test_player_smoke.py::test_try_player_validate_only_normalizes_video_modes_without_kvikdos` covers canonical, uppercase, and alias trial video modes in no-emulator validate-only mode, with representative `--video-mode` options before and after the module path.
Direct DOS player invocations also reject unsupported explicit `--video-mode=...` values before loading a module, printing `Unsupported video mode. Use --video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50` and exiting with the normal command-error status instead of silently falling back to 80x25 color.
`tests/test_player_behavior.py::test_iplaydiag_rejects_invalid_video_mode_before_playback` covers that direct DOS rejection path by running `IPLAYDIAG.EXE --video-mode=bad SMOKE.S3M` and asserting the module is not loaded and playback does not start.
`tests/test_player_behavior.py::test_iplaydiag_valid_video_modes_render_playback_geometry` covers valid direct DOS video-mode playback geometry for `40x25bw`, `40X25BW`, `40x25mono`, `40x25color`, `80x25bw`, `80X25BW`, `80x25mono`, `80x25color`, `80x50`, `80X50`, and `80x50project`, asserting both playback-position and post-playback-status frames report the expected cols/rows/screen bytes with nonzero audio-frame drawing evidence, and that the same run emits a parsed one-block bounded SB16 stereo playback pump ending by source-end.
The Python fixture layer exposes `assert_text_screen_geometry(...)`, so DOS video-mode playback tests validate parsed `Screen present:` geometry through `text_mode_byte_count(cols, rows)` instead of relying only on raw `2000/4000/8000` literals. This keeps 40x25, 80x25, and 80x50 behavior tied to the text-mode constants needed for later resizable/notcurses-style wrappers.
The fixture layer also exposes `assert_screen_present_content(...)`; DOS screen-present tests use it to require the expected scope, nonzero checksum, nonblank text cells, `full=1` full-screen-copy evidence, `mode_ok=1`, and exact audio-frame evidence tied to the parsed playback pump for playback frames. `scope=status-only` means only the status rows were refreshed before the full active text screen was presented. This prevents blank, wrong-scope, or stale-audio-counter screen callbacks from satisfying the text-mode tests.
`tests/test_player_behavior.py::test_iplaydiag_help_advertises_supported_video_modes` covers direct DOS help discoverability for the supported `--video-mode` values and text backend.

After kvikdos exits, the trial script also appends `trial_module_loaded=yes/no`, `trial_loaded_module_name=...`, uppercase-normalized loaded/requested module keys, `trial_requested_module_loaded=yes/no`, `host_module_size=...`, `trial_module_size=...`, `trial_module_size_matches_host=yes/no`, `trial_module_loader_line=...`, `trial_module_loader=...`, `trial_module_type_tag=...`, `trial_module_title=...`, `trial_ok_loader_metadata=yes/no`, `trial_playback_pump=yes/no`, `trial_playback_valid=yes/no`, `trial_audio_backend=...`, `trial_audio_backend_valid=yes/no`, `trial_audio_levels_valid=yes/no`, `trial_audio_level_sequence_valid=yes/no`, `trial_color_probe_valid=yes/no`, `trial_resize_cycle_valid=yes/no`, `trial_subwindow_valid=yes/no`, `trial_screen_present=yes/no`, `trial_playback_position_present=yes/no`, `trial_playback_position_valid=yes/no`, `trial_playback_position_geometry_valid=yes/no`, `trial_post_playback_status_present=yes/no`, `trial_post_playback_status_valid=yes/no`, `trial_post_playback_status_geometry_valid=yes/no`, and `trial_audio_unavailable=yes/no`, `trial_audio_unavailable_source=screen|exit-code|none` by scanning the captured DOS output. The requested-module marker requires the DOS `Module:` line to match the requested DOS filename using a DOS-style case-insensitive comparison. The size markers compare the host file size with the DOS-reported size, while the loader/type-tag/title markers preserve the DOS loader choice, module tag, and optional song title before playback classification. `trial_ok_loader_metadata` summarizes whether that metadata is usable for an OK result, so a trial can distinguish wrong loader detection from later decoder, SB16, or screen failures. The valid playback marker requires nonzero blocks, frames, accepted bytes, and checksum; native OK also requires the normalized SDL-compatible SB16 backend marker, active/nonzero audio-level marker, changing 16-block audio-level sequence evidence, 16-color foreground attribute evidence, 80x25-to-80x50 runtime resize evidence, and subwindow draw/present evidence. The valid playback-position and post-playback-status markers require nonzero screen checksum, nonzero nonblank text cells, and nonzero audio frame count; the geometry markers additionally require each `Screen present:` line to match the selected `trial_video_mode` cols/rows. When present, it also records `trial_playback_line=...` with the final `Playback pump:` diagnostic, `trial_audio_status_line=...`, `trial_audio_levels=...`, `trial_audio_maxlevels=...`, `trial_audio_active=...`, `trial_screen_reasons=...` with the emitted screen-present reasons, decoder route fields such as `trial_decoder_route_line=...`, `trial_decoder_route_id=...`, `trial_decoder_route_name=...`, decoder handoff fields such as `trial_decoder_handoff_line=...` and `trial_decoder_handoff=...`, and PCM route fields such as `trial_pcm_source_line=...`, `trial_pcm_provider=...`, `trial_pcm_renderer=...`, `trial_pcm_route=...`, `trial_pcm_input=...`, `trial_pcm_truncated=...`, `trial_pcm_hook_provider=...`, and `trial_pcm_stream_start=...` from the `PCM source:` diagnostic. Missing or old-format size/loader/type-tag/title/decoder/PCM fields are normalized to `none`; a missing `hook_provider=` field is normalized to `missing`, so stale diagnostic formats cannot look like valid provider/route/hook-provider/stream-start evidence. This keeps `RES.TXT` useful for user trials such as `ARYX.S3M`: it directly says whether the requested module path was actually loaded, whether host and DOS module sizes matched, which loader/module tag/song title were selected, whether loader metadata was valid for OK classification, whether the playback pump and screen-present diagnostics were reached, whether the native SDL-compatible SB16 backend and audio level meters were active and changing, whether the notcurses-style cell buffer preserved all 16 foreground color nibbles, all 8 background color nibbles, and the blink bit, whether one runtime can resize and redraw the status screen, whether a child subwindow can draw over the active runtime, whether the module used `native-preview` versus `dos-fallback`, whether an external decoder hook was installed, which stream-start boundary was used, which decoder route id/name and handoff were selected, what pump result was observed, which screen frames were presented, whether selected text geometry was actually used for playback and final status drawing, or whether the run exited through the real-SB16 unavailable path, and whether that evidence came from a diagnostic screen or the quiet production exit code.

The same summary block now emits a single `trial_result=...` classification. `bounded-ui-playback-ok` means the default diagnostic path loaded the requested module, the DOS-reported size matched the host file size, reported non-`none` loader metadata plus a nonzero module type tag, reported decoder route id/name and handoff plus PCM source provider/route/input/stream-start evidence, reached a block-limited playback pump with nonzero block/frame/byte/checksum evidence, presented valid nonempty `reason=playback-position` and `reason=post-playback-status` screen frames after audio submission, and exited cleanly. `source-ended-ui-ok` means a continuous/source-ending diagnostic loaded the requested module, the DOS-reported size matched the host file size, and exited cleanly after non-`none` loader metadata plus nonzero module type tag, decoder route and handoff, PCM source provider/route/input/stream-start evidence, nonzero source-end playback, and a valid nonempty `reason=playback-position` screen frame after audio submission. `quiet-completed-no-diagnostics` means the explicit quiet wrapper player exited cleanly and wrote `quiet_trial_completed=yes`, so the DOS run completed but did not emit diagnostic playback/UI proof. `requested-module-not-loaded` means the process exited cleanly but the DOS `Module:` line did not match the requested filename. `module-size-mismatch` means the requested module loaded but the DOS-reported size differed from the host file size staged by the trial script. `loader-metadata-invalid` means the requested module loaded but its loader/type-tag diagnostics were missing, `none`, or `00000000`, so the run is not accepted as OK even if later playback/screen markers appear. `decoder-route-missing` means the requested module loaded but decoder route id/name evidence was missing. `decoder-handoff-missing` means the requested module loaded but the DOS diagnostic handoff line was missing, so route id/name alone is not accepted as OK evidence. `pcm-source-missing` means the requested module loaded but the PCM source/provider/input/stream-start summary was missing, so playback-source routing was not proven. `playback-pump-invalid` means the requested module loaded and emitted a pump line, but the parsed pump evidence did not prove nonzero blocks, frames, accepted bytes, and checksum. `screen-evidence-invalid` means playback pump evidence was valid but playback-position screen content or geometry evidence was missing or invalid. `post-screen-evidence-invalid` means bounded playback reached valid playback-position evidence but the final post-playback status screen content or geometry evidence was missing or invalid. `playback-without-screen` is a warning result for audio pump success without drawing evidence. `audio-unavailable` means the real-SB16 diagnostic path presented the unavailable screen or the quiet production real-SB16 path exited with the audio-unavailable status, `kvikdos-timeout` identifies emulator timeout kills, and `exited-without-playback-pump`/`failed` separate quiet/no-pump exits from hard failures. The user-facing summary writes literal result markers including `trial_result=bounded-ui-playback-ok`, `trial_result=quiet-completed-no-diagnostics`, `trial_result=playback-without-screen`, and `trial_result=audio-unavailable`.
The script also emits `trial_failure_reason=...`: `none` for OK results, `requested-module-not-loaded`, `module-size-mismatch`, `loader-metadata-invalid`, `decoder-route-missing`, `decoder-handoff-missing`, `pcm-source-missing`, `playback-pump-invalid`, `screen-evidence-invalid`, `post-screen-evidence-invalid`, `audio-pump-without-valid-screen-present`, `sb16-audio-unavailable`, `emulator-timeout`, `no-playback-pump-evidence`, or `player-process-failed` for non-OK classifications. This gives user trials a single first-line reason before inspecting the detailed markers.
`rewrite/check_rewrite.sh` source-guards the same trial result/failure vocabulary, including `loader-metadata-invalid`, the size/loader/type-tag/title/decoder summary markers such as `host_module_size=...`, `trial_module_size=...`, `trial_module_size_matches_host=...`, `trial_module_loader=...`, `trial_module_type_tag=...`, `trial_module_title=...`, `trial_decoder_handoff=...`, and `trial_ok_loader_metadata=...`, and the `Size:` / `Loader:` / `Module type tag:` / `Title:` / `Decoder handoff:` DOS-output probes that derive those markers. It also requires the trial script to use the final matching DOS diagnostic line, strip the printed prefix/suffix before writing the normalized marker, compare DOS-reported size against the host file size, and accept loader metadata only when the loader is present and the module type tag is an 8-digit nonzero hex value, so adding or removing trial classifications or metadata evidence requires updating the gate intentionally.
The same gate also guards representative failure predicates, including requested-module mismatch evidence for `requested-module-not-loaded`, host/DOS size mismatch evidence for `module-size-mismatch`, route-line/id/name absence for `decoder-route-missing`, PCM source/provider/input/stream-start absence for `pcm-source-missing`, present-but-invalid pump evidence for `playback-pump-invalid`, valid playback with invalid playback-position screen evidence for `screen-evidence-invalid`, and bounded playback with invalid post-playback screen evidence for `post-screen-evidence-invalid`, so these result labels cannot remain in the script while their evidence checks are weakened or removed.
The script exits with `trial_script_exit_status=4` when the DOS player exits `0` but the captured evidence only proves `requested-module-not-loaded`, `module-size-mismatch`, `loader-metadata-invalid`, `decoder-route-missing`, `decoder-handoff-missing`, `pcm-source-missing`, `playback-pump-invalid`, `screen-evidence-invalid`, `post-screen-evidence-invalid`, `playback-without-screen`, `audio-unavailable`, or `exited-without-playback-pump`. OK classifications and clean `quiet-completed-no-diagnostics` exits keep the player exit status, and hard failures/timeouts keep their real status. This makes `try_player.sh` a real pass/fail gate for user trials instead of only a log generator.
The real-SB16 `IPLAYC.EXE` and `IPLAYHW.EXE` paths are intentionally opt-in for the trial script through `IPLAY_TRIAL_EXE=...`; the default bounded diagnostic path and explicit quiet trial binary keep the wrapper-SB16 path for kvikdos evidence.
If `IPLAY_TRIAL_EXE` names an executable that the player-only build does not produce, the trial script now fails before launching kvikdos with `try_player: trial executable not found after player build: ...`, keeping misspelled override paths from looking like emulator/player hangs.
The trial script also supports `--help` before or after trial options without rebuilding, so users can inspect the supported kvikdos trial command shape before launching a DOS run.

Diagnostic DOS player builds now print `Screen present: reason=status scope=full-screen bytes=... screen_bytes=... screen_checksum=... screen_nonblank=... full=... cols=... rows=... mode_ok=... audio_frames=... levels=L/R` after the loaded-module status frame is copied to the DOS text presenter, `Screen present: reason=playback-position scope=full-screen bytes=... screen_bytes=... screen_checksum=... screen_nonblank=... full=... cols=... rows=... mode_ok=... audio_frames=... levels=L/R` as playback updates the bottom/status frame, and `Screen present: reason=audio-unavailable scope=full-screen bytes=... screen_bytes=... screen_checksum=... screen_nonblank=... full=... cols=... rows=... mode_ok=... audio_frames=... levels=L/R` on the real-SB16 unavailable path. The smoke script requires the status and playback-position screen-present markers with full-screen scope, screen byte count, nonzero rolling text-cell checksum, nonzero nonblank character-cell count, `full=1`, geometry, `mode_ok=1`, runtime audio frame counters, and frame-level audio meter values, giving headless kvikdos runs direct evidence that drawing reached the DOS text presenter, the full active text screen was copied, which text mode size was active, that the runtime accepted the requested text mode, which text-cell content was present, and which audio output state was current when the frame was presented.

The screen digest is exposed through reusable text/runtime facade helpers (`iplay_text_cells_checksum`, `iplay_text_cells_nonblank_count`, `iplay_text_screen_checksum`, `iplay_text_screen_nonblank_count`, `iplay_runtime_video_checksum`, and `iplay_runtime_video_nonblank_cells`) instead of living as player-local code. That keeps current DOS diagnostics and future C/C++ rewrite tests on the same B800-compatible cell ABI.

`IUIRUN.EXE textcelldigest` is the lightweight DOS test seam for this ABI. It fills a deterministic 40x25 B800-style cell buffer, then reports raw-cell and `IplayTextScreen` checksum/nonblank values side by side so tests can prove both views agree without entering full playback or SB16 code.

`IUIRUN.EXE runtimetextdigest` extends the same seam through `IplayRuntime`: it starts an 80x25 color runtime on the same B800-style buffer and reports raw-cell and runtime-video checksum/nonblank values side by side. That covers the exact facade used by player diagnostics without entering module playback.

`IUIRUN.EXE runtimepresentdigest` extends the seam through the present callback: it compares raw memory, runtime-video, and callback-presented checksum/nonblank values for the same 80x25 cell buffer. That proves the screen content handed to a DOS text presenter is the same content visible through the runtime facade.

The player hardware runner now records `text_checksum` and `text_nonblank` whenever the mocked DOS hardware layer receives a B800/B000 text-memory copy. Loaded-module probes (`plhw25`, `plhw40`, `plhw8b`, `plhw50`) and 80x50 runtime screen probes report those fields, so player-level tests prove the DOS text presenter received nonempty UI content in each supported text mode.

The same loaded-module hardware probes now parse their SB16 audio-copy line through `parse_player_hw_audio_digest`, asserting submitted copy count, total bytes, checksum, first sample word, and tail sample word as one structured hardware-copy diagnostic. This keeps the wrapper-level audio evidence aligned with the structured text-present and playback-pump parsers.
The continuous module-backed keyboard-stop hardware probe also emits and parses the same audio-copy digest fields after the first SB16 block, so the timer/keyboard exit path proves nonempty wrapper audio data was copied before stopping.
That same continuous keyboard-stop probe now emits and parses the hardware text-copy digest fields (`text_off`, byte count, checksum, and nonblank count), proving the DOS text presenter had nonempty B800 output on the timer/keyboard path as well.
Continuous split-runner playback tests now assert the 1024-frame/4096-byte SB16 stereo block contract for both synthetic keyboard-stop and module-backed keyboard-stop paths, while bounded diagnostic/level-proof tests keep the 512-frame/2048-byte block contract. This preserves the distinction between fast bounded diagnostics and production-style continuous DMA-sized playback blocks.
The Python behavior fixtures expose that contract as named constants (`SB16_STEREO_BYTES_PER_FRAME`, `SB16_BOUNDED_BLOCK_FRAMES`, `SB16_CONTINUOUS_BLOCK_FRAMES`, and derived byte counts) plus `sb16_stereo_byte_count(...)`, so future SDL-backed tests can share the same 16-bit stereo frame accounting instead of duplicating raw byte literals.
The same fixture layer also exposes `assert_sb16_stereo_frame_bytes(...)`, `assert_sb16_stereo_block_accounting(...)`, and `assert_playback_pump_sb16_stereo(...)`; parity tests use them to require playback `accepted` bytes and captured hardware-copy bytes to equal `frames * 4`, stay aligned to 16-bit stereo frames, and match the expected bounded/continuous frames-per-block contract. This keeps future SDL replacement work honest about frame accounting even when the hardware output implementation changes.
The playback-level refresh hardware probe (`playerplaybacklevelshw`) now emits and parses the same audio-copy and text-copy digest fields, so fast SB16 level redraw coverage proves both a nonempty submitted audio block and a nonempty B800 UI refresh.

The Python test fixtures now expose the same B800-style digest format through `text_cell_checksum`, `text_cell_nonblank_count`, and `text_cell_digest`. Future original-dump parity tests can compare original `B800:0000` bytes and rewritten presenter bytes with the same checksum/nonblank semantics used by the C runtime diagnostics.

Those fixtures also expose `text_memory_slice` and `text_memory_digest`, which extract a real-mode DOS text aperture from a full memory dump using `segment * 16 + offset`. That is the intended comparison primitive for future original `B800:0000` / `B000:0000` parity tests once the original screen capture path is available.

The fixtures also expose `parse_screen_present_digest`, which parses rewritten `Screen present: ... screen_checksum=... screen_nonblank=...` diagnostics into the same `{bytes, checksum, nonblank}` shape. Future original-vs-rewrite screen tests should compare `text_memory_digest(original_dump, 0xB800/0xB000, cols, rows)` against `parse_screen_present_digest(rewrite_output, reason)`.

The real DOS behavior tests now use `parse_screen_present_digest` plus shared `assert_text_screen_geometry(...)` and `assert_screen_present_content(...)` helpers on actual `IPLAYHW.EXE` audio-unavailable output and `IPLAYDIAG.EXE` bounded playback output, checking scope, geometry, byte count, audio frame counters, checksum, and nonblank text-cell counts from the emitted diagnostics. The post-playback redraw is asserted as `scope=status-only` while the initial, playback, and unavailable frames remain `scope=full-screen`.
The screen-present helper has direct Python coverage for accepting `scope=status-only` with `full=1` full-screen-copy evidence and for rejecting stale audio-frame counters when an exact expected frame count is supplied.
The text-geometry helper has direct Python coverage for accepting an 80x50/8000-byte screen digest and rejecting a digest whose cols/rows imply 8000 bytes but whose byte count only reports 4000.

The real DOS behavior tests also parse `Playback pump: ...` through `parse_playback_pump`, so the SB16-compatible playback evidence is checked as structured fields instead of only as substrings. The bounded `IPLAYDIAG.EXE` path asserts 32 submitted blocks, 16,384 frames, 65,536 accepted bytes, nonzero checksum, `limit=1`, `source_end=0`, and `stop=block-limit`; the continuous diagnostic source-end path asserts one 1024-frame block, 4096 accepted bytes, nonzero checksum, `limit=0`, `source_end=1`, and `stop=source-end`.
The fixture layer also parses `Playback loop: ...` through `parse_playback_loop` and `assert_playback_loop(...)`, so representative DOS behavior and split-runner tests assert loop mode, policy, cadence, max-block count, and frames-per-block as structured fields rather than raw diagnostic substrings.
The fixture layer now parses `Decoder progress: ...` through `parse_decoder_progress`, `assert_decoder_progress(...)`, and `assert_decoder_progress_block(...)`. Representative source-end, order-end, order-skip, pattern-break, and long-module block-counter tests assert block counters, order/pattern/row/channel, tick/speed/tempo, ended flag, and loop flag as structured fields instead of relying on substring matches.
The fixture layer now also parses `Decoder geometry: ...` through `parse_decoder_geometry(...)` and `assert_decoder_geometry(...)`. Representative S3M/MOD control-flow tests assert order count, rows/order, restart, speed, tempo, and channel count as structured fields, so future decoder rewrites cannot silently change module geometry while keeping vaguely similar diagnostic text.
Decoder event and voice diagnostics are also structured now: `parse_decoder_events(...)`, `assert_decoder_event(...)`, `parse_decoder_voices(...)`, and `assert_decoder_voice(...)` cover period/note/octave/instrument/volume/effect/param plus active voice, sample length/volume, loop start/length, and data offset. The behavior tests no longer rely on raw positive `Decoder event:` or `Decoder voice:` substring checks.
Loader metadata diagnostics are structured as well. `parse_module_loaded(...)` / `assert_module_loaded(...)` preserve exact module-name casing, and `parse_module_size(...)` / `assert_module_size(...)` preserve exact loaded byte counts. Positive behavior tests no longer rely on raw `Module:` or `Size:` substring checks; explicit absence checks still assert that rejected paths never load a module.
Loader-kind and decoder-handoff diagnostics now use the same structured approach. `parse_module_loader(...)` / `assert_module_loader(...)` preserve the exact loader description selected by the DOS path, while `parse_decoder_handoff(...)` / `assert_decoder_handoff(...)` preserve whether playback entered the external tracker SB16 seam or the project INR SB16 path. Positive behavior tests no longer rely on raw `Loader:` or `Decoder handoff:` substring checks; explicit non-handoff checks remain absence assertions.
Module metadata and loader-error diagnostics now have structured checks too. `parse_module_type_tag(...)`, `parse_module_title(...)`, and `parse_unsupported_module(...)` keep type tags, titles, and rejected filenames exact, with behavior tests using `assert_module_type_tag(...)`, `assert_module_title(...)`, and `assert_unsupported_module(...)` instead of raw positive substring checks.
Playback status, FFI marker, and order/channel summary diagnostics are also structured. `parse_playback_output(...)`, `parse_playback_disabled(...)`, `parse_ffi_marker(...)`, and `parse_orders_channels(...)` preserve SB16 output state, real-SB16 unavailable state, ABI marker output, and module order/channel summaries without raw positive substring checks.
Static help/capability diagnostics are centralized behind helper assertions as well: usage text, supported DOS formats, SB16-only audio scope, VGA text backend scope, and the SDL-compatible SB16 backend boundary are checked through fixture helpers instead of scattered raw positive substring assertions. The remaining raw behavior diagnostics are intentional absence checks for paths that must not load a module or emit the wrong decoder route/handoff.
Those absence checks are now centralized too: `assert_module_not_loaded(...)`, `assert_decoder_route_absent(...)`, and `assert_decoder_handoff_absent(...)` express negative loader/route expectations without raw diagnostic assertions in the behavior tests.
Those real DOS playback tests now also call the shared `assert_playback_pump_sb16_stereo(...)` helper, so both bounded `IPLAYDIAG.EXE` output and continuous `IPLAYCONT.EXE` source-end output must satisfy the same block count, frames-per-block, and `accepted == frames * 4` SB16 16-bit stereo accounting used by the split-runner hardware tests.
They also use `assert_playback_pump_stop_state(...)`, so bounded block-limit, one-block source-end, and continuous source-end paths all assert the expected `limit`, `source_end`, and `stop` fields through the same helper.
The behavior suite now also has shared `assert_bounded_sb16_playback(...)` and `assert_bounded_sb16_playback_blocks(...)` wrappers for diagnostic bounded paths. Loader, capped-header boundary, capped same-path, and stream-start boundary tests use them instead of raw `Playback pump:` substrings, so those module-loading paths assert SB16 16-bit stereo frame accounting, nonzero checksum, and `block-limit` stop state through one reusable assertion path.
Bounded source-end cases now use `assert_bounded_source_end_playback(...)`, including 63-block pattern-break drains, one-block immediate source end, and zero-block order-end cases. That keeps source-end stop state and bounded SB16 byte accounting structured without requiring a nonzero checksum for the zero-block no-audio path.
The stop-state helper has direct Python coverage for accepting a source-end pump and rejecting a mismatched stop reason.
The SB16 accounting helpers have direct Python coverage for accepting valid bounded block/frame/byte totals and parsed bounded playback-pump dictionaries, and for rejecting frame-count mismatches, block-count mismatches, unaligned 16-bit stereo byte counts, and accepted-byte mismatches on parsed playback-pump dictionaries.
Checksum-difference behavior tests now also read the checksum through `parse_playback_pump`, so all Python assertions that interpret the `Playback pump:` line share one diagnostic ABI.

`assert_text_memory_matches_screen_present` now packages that comparison into a single helper. It extracts and digests original text memory, parses the rewritten screen-present diagnostic, and compares byte count, checksum, nonblank count, columns, rows, and expected screen scope. The helper defaults to `full-screen` and accepts `expected_scope="status-only"` for post-playback status redraws.

The fixtures also expose `parse_player_hw_text_digest`, which parses `IPHWRUN` loaded-module/runtime hardware-copy output (`text_seg=... text_bytes=... text_checksum=... text_nonblank=...`) into the same digest shape. That gives player-level B800/B000 hardware-present probes the same structured comparison path as diagnostic `Screen present` lines.

`assert_text_memory_matches_player_hw_text` packages original-dump versus `IPHWRUN` hardware-copy comparison in the same style as `assert_text_memory_matches_screen_present`, including segment, offset, byte count, checksum, and nonblank count.

The existing loaded-module hardware text-mode tests (`plhw25`, `plhw40`, `plhw8b`, `plhw50`) now parse their real `IPHWRUN` output through `parse_player_hw_text_digest`, so the structured digest path is exercised by player-level DOS text-present probes rather than only by synthetic parser tests.

The 80x50 runtime hardware-present tests (`playerruntimehw80x50` and `playerruntimehw80x50levels`) also parse their `IPHWRUN` output through the same helper, covering static UI drawing and fast SB16 level rendering on the structured B800 digest path.
The smoke contract also checks frame ordering through `audio_frames`: the initial `reason=status` frame must still have `audio_frames=0`, and both `reason=playback-position` and `reason=post-playback-status` must match the bounded diagnostic run's completed `Playback prime` frame count of `16384`. This proves the final playback-position and status frames were presented after the playback pump completed, not merely after the first submitted audio block.
The second loaded-module status redraw now reports `Screen present: reason=post-playback-status ...`, so diagnostics distinguish the initial module/status frame from the final status frame drawn after playback has submitted audio blocks and updated runtime audio counters/levels.
Loaded-module status redraws call `iplay_runtime_draw_status_block(...)` before presenting, so the initial and post-playback status frames contain the original-style module filename, track/position, free memory, samples used, main volume, output levels, module type, and interpolation rows instead of stale module-only or developer diagnostic content. The post-playback frame reports `scope=status-only`, refreshes the status block in-place before presenting, and preserves the bottom playback-position area drawn during the playback pump instead of resetting it to default order/row values.
Playback-position refreshes now redraw the live audio level meter with `iplay_runtime_draw_audio_levels(...)` before presenting, so the per-block UI update carries both current order/row/speed state and current SB16-compatible level bars.
The real-SB16 unavailable screen also refreshes the audio backend/hardware/level rows with `iplay_runtime_draw_audio_status(...)` before writing `Playback disabled: SB16 not detected`, so the failure frame still carries current audio/status context.

Diagnostic playback prime output now includes `backend=... status=... dropped=... queued=... levels=L/R` in addition to readiness, hardware flag, written frames, and capacity. The backend/status text comes from the runtime audio facade, and the level values come from the runtime audio meter (`iplay_runtime_audio_levels(...)`) after playback has submitted blocks, so headless tests can see SB16-compatible audio state without relying on audible output.

Continuous playback now also has a bounded idle-poll fallback for DOS/emulator environments where BIOS timer ticks stop advancing. Real timer ticks still reset the idle counter and drive normal cadence, but a stuck tick source no longer leaves `IPLAYC.EXE` spinning forever before submitting the next SB16 block.

Behavior tests run `IPLAYC.EXE` against a naturally-ending synthetic S3M under `kvikdos` and assert it exits through the explicit no-SB16 code path with no module/playback diagnostics. The same fixture is run through `IPLAYCONT.EXE` to prove the continuous-policy route enters `policy=timer-keyboard` and exits through `stop=source-end` without relying on the bounded block limit or waiting for a second BIOS tick after the source has ended. Split-runner coverage also proves the timer/keyboard policy with a module-backed `PlayerDecoderContext` source.

The smoke script now runs `IPLAYTRY.EXE ENDCONT.S3M`, visible `IPLAYCONT.EXE ENDCONT.S3M`, and `IPLAYHW.EXE SMOKE.S3M` under `kvikdos` before the detailed `IPLAYDIAG.EXE` fixture matrix and reports `IPLAYTRY/IPLAYCONT/IPLAYHW/IPLAYDIAG/TEXTMODE smoke ok` only after all stages pass. `ENDCONT.S3M` is a shared source-end fixture, so the quiet wrapper-SB16 trial binary proves it is DOS-runnable without forcing a headless continuous-playback timeout on every smoke run, while the visible continuous diagnostic binary must show the timer-keyboard playback loop, source-end playback pump, and nonempty playback-position screen. Bounded diagnostic video-mode smoke probes also run `SMOKE.S3M` with `--video-mode=40x25bw`, `40x25color`, `80x25bw`, `80x25color`, and `80x50`, requiring playback-position and post-playback-status screens to report the expected cols/rows and screen byte counts. The diagnostic real-SB16 binary still reaches the expected `Playback disabled: SB16 not detected` full-screen unavailable frame with exit status `3`, and detailed loader/decoder/SB16 diagnostics stay on bounded `IPLAYDIAG.EXE`. Original/unsupported `IPLAY.EXE` kvikdos probes are capped by `IPLAY_ORIGINAL_KVIKDOS_MAX_TIMEOUT` (default `3`) even when an individual test requests a longer timeout, because those paths are blocker probes rather than full playback tests.

Module loading now reads module data into a named DOS module buffer instead of retaining only the old 8 KiB header window. The current fixed buffer is `IPLAY_PLAYER_MODULE_BUFFER_BYTES = 24576`, which is enough to cover sample data past the old header limit and the current 20,800-byte `aryx.s3m` trial case while avoiding unstable heap allocation under `kvikdos`. The buffer is declared as far data so it does not consume near DGROUP in the DOS large-model build. Oversized external tracker modules cross the loader boundary as capped file-backed modules with true file size preserved; oversized non-library/project-owned modules still report `Module too large` instead of the misleading `Module not found`.

Module file opens now retry lowercase and uppercase path variants before reporting failure. This preserves normal DOS-style case-insensitive use for host-backed emulators, so commands such as `IPLAYC.EXE ARYX.S3M` can load a local `aryx.s3m` fixture. The same helper is used by capped-header file streaming.

Module storage capacity is now carried by `PlayerModuleInfo` and read through `player_module_header_capacity(...)` instead of hard-coding `IPLAY_PLAYER_MODULE_BUFFER_BYTES` inside the file-read helper. The current capacity is still the safe fixed DOS buffer, but the loader boundary now has the metadata needed for a later far-buffer or streaming storage implementation.

Oversized external-library tracker modules can now cross the loader boundary with a capped DOS header view while preserving the real file size, matching the future decoder direction where libopenmpt/libxmp/libmodplug can read from the module path instead of near DOS storage. MOD, NST, S3M, MTM, STM, FAR, 669, PSM, and ULT have explicit DOS behavior coverage for this path. A near/static-buffer increase was proven unsafe under `kvikdos`, so this remains a bounded header-view path rather than full in-memory large-module support.

`PlayerModuleInfo` now records whether the loaded header view is truncated. Playback diagnostics report this as `truncated=0/1` on the `PCM source` line, so capped-header/library-boundary paths are visible under DOS and future C/C++ decoder selection can branch on explicit module state instead of inferring from size/capacity.

Decoder input mode is also explicit: complete module buffers report `input=memory`, while capped-header modules report `input=file-path`. The current DOS fallback avoids treating capped headers as full PCM/module bodies; those paths now use the deterministic fallback stream until the external library decoder reads the original module path.

PCM diagnostics also report the active renderer boundary with compact DOS-safe codes. External tracker formats report `renderer=e` and either `provider=native-preview` for complete in-memory modules or `provider=dos-fallback` for capped/header-only modules, INR/project-owned formats report `renderer=p` and `provider=native`, and only true fallback/default paths report `renderer=f`. This gives the later libmodplug/libxmp/libopenmpt replacement a stable behavior seam to switch from preview/fallback PCM to library-backed PCM without changing the loader contract.

Module PCM production now enters through `player_decoder_context_render_pcm(...)`, using a one-byte renderer kind stored in `PlayerDecoderContext`. The current dispatch routes complete in-memory external tracker formats to the DOS native preview mixer and capped/header-only external tracker formats to the bounded stream placeholder, keeping final effect-accurate playback on the external-library boundary. Project-owned and default fallback paths still use the native mixer branch.

External tracker renderer state now goes through `player_decoder_context_render_external_tracker_pcm(...)` before either native-preview or stream-placeholder PCM is emitted. That function is the intended insertion point for libmodplug/libxmp/libopenmpt-backed PCM without changing the playback source loop or project-owned renderer branch.

The DOS player now exposes a default-off pure-C external decoder hook: `iplay_player_set_external_decoder(...)` installs a `PlayerExternalDecoderRenderFn` that receives the opaque module and playback block for external tracker formats before the DOS native-preview/stream fallback path runs, and `iplay_player_clear_external_decoder(...)` restores the current DOS behavior. The public helper set exposes the module path, module size, truncated/input status, playback-block frame count, active byte count, and writable PCM pointer needed by a C/C++ adapter. When a hook is installed, PCM diagnostics report the active provider name such as `provider=libmodplug` while still reporting `hook_provider=libmodplug`, so future library-backed playback can be distinguished from the built-in DOS preview/fallback path without changing the trial parser. The DOS MZ build does not link libmodplug/C++ through this hook, but the future C/C++ rewrite can plug the existing libmodplug/SDL bridge behind the same playback loop without reworking SB16 block submission, screen updates, or provider diagnostics.

The host libmodplug bridge now includes that adapter shape directly: `IplayModplugExternalDecoder` owns a stateful `IplayModplugPcmSource`, `iplay_modplug_external_decoder_render(...)` reads the player module path through `iplay_player_module_path(...)`, obtains the active block through `iplay_player_playback_block_pcm(...)` / `iplay_player_playback_block_frames(...)`, and fills the same 16-bit stereo block consumed by the DOS/SB16 playback loop. This is still not linked into the DOS MZ player, but it proves the future modern C/C++ path can use the same per-block player decoder hook instead of a separate file-drain facade.

The same bridge now exposes one-call hook wiring behind `IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER`, defaulting to `0`: `iplay_modplug_external_decoder_install(...)` installs the adapter through `iplay_player_set_external_decoder(..., "libmodplug")`, and `iplay_modplug_external_decoder_uninstall(...)` clears the player hook. Keeping the adapter behind an explicit build flag prevents the always-linked modern host bridge from requiring `iplay_player.c` symbols, while still preserving the future build path that links the player and libmodplug together.

The external decoder hook now has explicit return codes: unavailable keeps the DOS native-preview/fallback path, rendered accepts the external PCM block, and source-ended marks the player decoder context ended without falling back to placeholder PCM. The libmodplug adapter returns `IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED` when its stateful source reaches natural end, keeping future library-backed playback aligned with the DOS playback pump's `source_end` stop reason.

The host hook probe also covers the installed-but-unavailable edge: a corrupt tracker file reports `provider=libmodplug` with `unavailable_status=0`, leaves the requested player block frame count intact for fallback rendering, and leaves the PCM checksum at zero before uninstalling the hook. The hook audio helper submits `iplay_player_playback_block_frames(block)` to the SB16/SDL frame writer, not the block's active byte count, so the future library-backed path preserves the same frame-count contract as the DOS playback loop and cannot overrun a player PCM block at the sink boundary. This protects the distinction between an installed decoder hook and the actual current PCM provider printed by DOS diagnostics.

The same hook audio helper accepts any nonzero submitted SB16 block when source-end is not required, so one-block bounded or callback-driven playback stops are valid. The success contract still requires frame-to-byte consistency, captured bytes, nonzero PCM checksum evidence, and live level-meter evidence.

The gate now includes a host-side `libmodplug` renderer proof. `tests/test_modplug_renderer.py` compiles `rewrite/modplug_renderer.cpp` plus `rewrite/modplug_probe.cpp`, loads the real `aryx.s3m` fixture through libmodplug, and asserts nonzero 44.1 kHz signed 16-bit stereo PCM frames/checksum/peak. The adapter exposes `iplay_modplug_render_file_pcm(...)`, which can fill caller-owned 16-bit stereo buffers, and the test covers a 512-frame block matching the player/SB16 block size. This does not link libmodplug into the DOS MZ player; it proves the external tracker library path that will later sit behind `player_decoder_context_render_external_tracker_pcm(...)`.

The host adapter also exposes a stateful `IplayModplugRenderer` open/read/close API. Tests now prove two consecutive 512-frame reads advance through the same libmodplug decoder instance, which is the behavior needed for a continuous SDL/audio callback path.

The libmodplug adapter sets `mLoopCount = 0` and exposes a bounded read-until-end helper. The helper reports failure if it hits its safety block limit without seeing source end. Tests prove a real S3M can be drained to natural source end in 512-frame blocks before the safety block limit, matching the player loop's source-end stop condition.

The host libmodplug layer now also exposes `IplayModplugPcmSource`, a read/ended/close wrapper with the same lifecycle as the DOS player's `PlayerPcmSource`. A probe test drains ARYX.S3M through this source loop in 512-frame blocks and requires the source to mark natural end before the safety limit. This is the runtime-facing seam for the later SDL-backed path; the DOS player still keeps SB16 hardware output separate.

Real libmodplug PCM is now also fed through the rewritten SDL-compatible/SB16-stereo audio output contract in a host probe. The probe drains ARYX.S3M through `IplayModplugPcmSource`, submits each 512-frame block with `iplay_sdl_audio_device_write_sb16_frames`, and asserts accepted bytes, written frames, captured bytes, and peak live level meters match the source. This proves the external decoder can drive the same 16-bit stereo audio wrapper intended to replace SB16 hardware with SDL.

That host audio path is no longer probe-only: `modplug_audio_bridge.cpp` exposes `iplay_modplug_audio_play_file_to_sdl_sb16(...)`, which accepts an `IplayAudioWriteFn` callback and drains libmodplug PCM into `IplaySdlAudioDevice`. The probe now only supplies a capture callback and prints bridge stats. This is the reusable host/SDL integration point; the DOS MZ build still stays pure C and does not link libmodplug or C++ sources.

`modern_player.cpp` now adds a host-side playback facade above the bridge: `iplay_modern_play_file_to_sdl_sb16(...)`. It routes external tracker content through libmodplug and the rewritten SDL-compatible/SB16 audio wrapper, while keeping `.inr` on the project-owned decoder-unavailable path. This gives the future C/C++ player a single modern entry point without changing the DOS MZ build.

The modern facade classifies supported paths explicitly while preserving original-style header precedence. MOD/NST/S3M/STM/669/MTM/PSM/FAR/ULT plus WOW/OKT/OCT/XM/IT/PTM/AMS/DBM/DMF/MDL/DSM/MED/IMF/J2B are known external tracker candidates for the library-backed path, INR is project-owned and currently reports `project-decoder-unavailable`, and unknown extensions may still succeed if libmodplug recognizes the file content. When such a probe-by-content file successfully decodes, the playback result is promoted to `route=external-library provider=libmodplug`, while failed unknown probes still report `unsupported-format`. The DOS diagnostic/player loader now also accepts WOW/OKT/OCT/XM/IT/PTM/AMS/DBM/DMF/MDL/DSM/MED/IMF/J2B through the generic `external_module` loader symbol, keeping them on `route=external-library` without adding handwritten decoders. The external tracker inventory is exported through `iplay_modern_external_tracker_extension_count(...)` / `iplay_modern_external_tracker_extension(...)` and can be probed with `iplay_modern_host --list-extensions`, so future decoder/library choices have a runtime-checkable boundary. If an unknown-extension file cannot be decoded, it reports `unsupported-format`.

The modern facade also exposes `IplayModernDecoderRoute`, `iplay_modern_decoder_route(...)`, `iplay_modern_path_is_external_tracker(...)`, `iplay_modern_path_is_project_owned(...)`, `iplay_modern_decoder_route_name(...)`, and `iplay_modern_decoder_route_uses_external_library(...)`, with `iplay_modern_host --classify <path>` covering S3M/XM as `route_id=0 external-library library=1`, INR as `route_id=1 project-owned library=0`, and unrelated extensions as `route_id=2 probe-by-content library=1`. This gives the future C/C++ player an explicit library-routing decision instead of duplicating extension logic outside the facade. Direct `iplay`/`iplay_native` module arguments, native `@file-list` paths, and native file-list entries now resolve filenames with DOS-style case-insensitive matching in their host directory before playback, so `ARYX.S3M` can find `aryx.s3m` and `@PLAYLIST.LST` can find `playlist.lst` on a case-sensitive host while preserving the selected real filename in module evidence. If the resolved direct module or selected file-list entry is still missing, `iplay`/`iplay_native` now reports `Module not found.` and returns exit code `2` before decoder routing, SB16 audio, or text-present diagnostics, matching the observed original missing-module behavior.

Modern playback results now carry the same route/provider evidence through playback, not only through classification. `IplayModernPlaybackResult` records `decoder_route` plus `decoder_provider`, the host probe prints `route_id=... route=... provider=...` on success and failure, and playback summaries include `route=...; provider=...;` before the status/stop fields. Valid external tracker playback, external decoder failures, probe-by-content failures, and project-owned INR failures therefore expose the same routing vocabulary used by DOS diagnostics.

The DOS player source documents the same route vocabulary in its loader boundary comment and routes DOS-recognized MOD/NST/S3M/STM/669/MTM/PSM/FAR/ULT plus generic external-library WOW/OKT/OCT/XM/IT/PTM/AMS/DBM/DMF/MDL/DSM/MED/IMF/J2B extensions through the `external-library`, `project-owned`, and `probe-by-content` route names.

The modern route tests also assert that every DOS-recognized external tracker extension (MOD/NST/S3M/STM/669/MTM/PSM/FAR/ULT/WOW/OKT/OCT/XM/IT/PTM/AMS/DBM/DMF/MDL/DSM/MED/IMF/J2B) is present in the modern external-library extension inventory. This keeps the DOS loader surface aligned with the future C/C++ library-backed decoder surface.
The inventory suite now also guards the DOS-side generic `external_module` loader table for WOW/OKT/OCT/XM/IT/PTM/AMS/DBM/DMF/MDL/DSM/MED/IMF/J2B, the shared `EXT ` module type tag for that generic external-library boundary, and rejects per-format handwritten decoder symbols such as `xm_module` or `it_module`. This keeps the expanded DOS loader surface as a routing boundary only, not a new translated-decoder implementation.
The generic external-library metadata path prints XM title metadata from the `Extended Module: ` header and IT title metadata from the `IMPM` header while still routing playback through `external_module` and `route=external-library`. This is diagnostic metadata only; it does not add handwritten XM or IT pattern/sample/effect decoders.

The same tests assert `.inr` is not in the modern external-library extension inventory, preserving INR as the project-owned route until a reliable external decoder is intentionally selected.

The route classifier is also exercised for each DOS-recognized external tracker extension using uppercase filenames, requiring `route_id=0 route=external-library library=1` for MOD/NST/S3M/STM/669/MTM/PSM/FAR/ULT/WOW/OKT/OCT/XM/IT/PTM/AMS/DBM/DMF/MDL/DSM/MED/IMF/J2B.

The classify probe also reports `backend="SDL-compatible SB16 16-bit stereo"`, tying decoder-route decisions to the intended modern audio backend that replaces SB16 output outside the DOS MZ build.

The expanded modern tracker classification is runtime-covered, not only source-inventoried. Corrupt known-library extensions across WOW/OKT/OCT/XM/IT/PTM/AMS/DBM/DMF/MDL/DSM/MED/IMF/J2B now report `external-decoder-failed` through the modern facade, while corrupt unknown extensions still report `unsupported-format`. A valid tracker copied under a newly classified `.XM` extension and a valid tracker hidden under an unknown `.XYZ` extension both reach `status=ok route=external-library provider=libmodplug` through libmodplug and the SDL-compatible SB16 audio bridge. This keeps future C/C++ rewrite routing on the external decoder boundary for reliable-library formats without adding DOS handwritten decoders.

The modern facade also exposes `iplay_modern_playback_summary(...)`, a status formatter using the same user-visible concepts as the DOS runtime status: audio backend, playback enabled/disabled, block/frame counters, accepted bytes, drops, and live/max audio levels. The host probe prints this summary after real libmodplug playback, giving the modern path a UI/status seam instead of audio-only evidence.

The modern facade now also renders playback status into the existing rewritten text/runtime facade through `iplay_modern_render_playback_status(...)`. It draws a title row plus module, block count, stop reason plus source-end state, audio backend, accepted byte and dropped frame counts, frame count plus source checksum, live/max levels, decode status, and playback state plus active flag with runtime status helpers, with each modern status row emitted exactly once. The host probe snapshots rendered text rows, so the modern libmodplug path now has screen-cell evidence in addition to audio counters.

Modern status labels and the audio-backend display string are centralized as `MODERN_STATUS_*` constants before crossing into the runtime draw calls. This keeps the host/libmodplug status surface aligned with the DOS runtime panel without duplicating literal labels across summary and screen rendering.

The modern status title is exposed through `iplay_modern_status_title()`, so callers and tests can use the same user-visible title as the runtime panel without depending on the private constant layout.

The modern audio backend display string is exposed through `iplay_modern_audio_backend_name()`, keeping the summary formatter and runtime panel on the same facade-level label.

Modern block-count display is formatted through `iplay_modern_format_blocks(...)` before it reaches the runtime panel, keeping counter text generation on the modern facade boundary.

Modern stop-row text is formatted through `iplay_modern_format_stop(...)`, so the stop reason, source-end flag, decoder route, and decoder provider are generated together on the facade boundary.

Modern accepted/drop and frames/checksum rows are formatted through `iplay_modern_format_accepted(...)` and `iplay_modern_format_frames(...)`, keeping the user-visible counter text in facade helpers instead of local render code.

Modern level-meter text is formatted through `iplay_modern_format_levels(...)`, so live/max channel level display also stays on the modern facade boundary.
Modern playback-state text is formatted through `iplay_modern_format_playback_state(...)`, so enabled/disabled wording and active audio state are generated together before the Playback row is drawn.

The modern facade's formatter helpers share one private `modern_finish_format(...)` truncation/error handler, so row-formatting failure behavior stays consistent across block, accepted/drop, frame/checksum, and level text.

The modern host probe now snapshots that rendered status in all supported text geometries: 80x25, 40x25, and 80x50. This exercises the same runtime/status drawing path across the project text modes, including natural clipping in the 40-column mode.

Those modern status snapshots are now also presented through the runtime video callback. The probe records one present event for each supported mode and checks the presented byte counts match the active text geometry: 40x25 = 2000 bytes, 80x25 = 4000 bytes, and 80x50 = 8000 bytes.

The modern libmodplug bridge now records explicit playback stop state. Successful drains set `source_ended=1` and `stop_reason="source-end"`, and the modern summary/probe output include `stop=source-end source_end=1`. This mirrors the DOS playback pump stop reason instead of representing a completed module as only `status=ok`.

The modern bridge also reports bounded playback stops. If the caller's block limit is reached before libmodplug reaches natural source end, stats carry `stop_reason="block-limit"`, `source_ended=0`, and the modern facade reports `status=block-limit`. The probe accepts an optional max-blocks argument so tests can force and verify this stop path.

The modern bridge now has a cancellable playback variant as well. A caller-provided stop callback can interrupt after a submitted block; the bridge reports `stop_reason="keyboard"` and the modern facade reports `status=keyboard`. The probe exposes `--keyboard-after-one` to force this path, matching the DOS loop's keyboard stop category.

Modern playback enabled/disabled wording now goes through `iplay_modern_playback_status_started(...)`. Natural source end, block-limit stops, and keyboard stops are all reported as playback-enabled states because audio was started and at least one block was submitted; project-decoder, unsupported-format, external-decoder, and invalid-argument failures remain playback-disabled states.

The concrete playback state label is exposed through `iplay_modern_playback_state_text(...)`, so summary output and the runtime panel share the same enabled/disabled wording.

Modern stop-reason display goes through `iplay_modern_playback_stop_text(...)`, centralizing the fallback from missing bridge stop text to `unknown` before summary and runtime panel rendering.

The modern runtime panel's Status row is selected through `iplay_modern_playback_panel_status_text(...)`: stop-driven playback states display their stop reason, while decoder/configuration failures display the playback status name.

Modern failure states now render through the same runtime text/present path as successful playback. The status row displays project/unsupported/decoder failures directly, while stop-driven paths still display `source-end`, `block-limit`, or `keyboard`. The probe emits the shared `iplay_modern_playback_summary(...)` string plus an 80x25 screen snapshot and present event before returning failure, so failed modern playback is user-readable through the same summary/status surface as successful playback.

The modern host playback path is now a concrete build artifact, `rewrite/.build/iplay_modern_host`, copied to `rewrite/.build/iplay` as the preferred user-facing SDL/notcurses binary and to `rewrite/.build/iplay_native` as the legacy native alias, built by `rewrite/build_rewrite.sh` from the libmodplug renderer, SDL-compatible/SB16 bridge, modern playback facade, and host probe. The rewrite gate checks that artifact exists separately from the DOS MZ binaries, keeping C++/libmodplug out of the DOS link while making the modern path directly runnable.

The earlier clean-C S3M/MOD voice/effect mixer experiment remains non-authoritative for reliable-library tracker formats. S3M, MOD/NST, STM, 669, MTM, PSM, FAR, ULT, and other reliable-library tracker formats still keep effect-level correctness on the external decoder boundary: DOS behavior tests preserve loader metadata, stream-start, stop-state, UI/audio wrapper, and SB16 plumbing evidence, while final effect-accurate PCM is expected to come from the host/modern libmodplug/libxmp/libopenmpt path. Complete in-memory tracker modules can use the existing DOS native preview mixer so the MZ player emits pattern/sample-driven SB16 PCM before the external library handoff exists; capped-header/file-path modules still use the bounded byte-stream placeholder.
effect-level PCM is expected to come from the host/modern libmodplug/libxmp/libopenmpt path.
The DOS trial proof requires PCM source provider/input/stream-start evidence before a playback result is accepted as working.
The modern status panel/probe reports block count, stop reason plus source-end state, audio backend, accepted byte and dropped frame counts, frame count plus source checksum, live/max levels, decode status, playback state plus active flag.

When a tracker format is still waiting for the future external-library decoder and cannot be fully held in the DOS module buffer, the SB16 PCM placeholder consumes bytes from the loaded module stream instead of generating output only from a fixed seed. This keeps current DOS playback bounded and testable while making real files affect emitted PCM data before the libmodplug/libxmp/libopenmpt handoff is wired in.

The file-byte fallback remains the capped-header external-tracker path, so oversized S3M/other external-library placeholders do not require a full in-memory MOD/S3M voice-mixer loop while waiting for the real external decoder boundary.
Complete external tracker diagnostics report `provider=native-preview`, while capped-header/file-path tracker diagnostics continue to report `provider=dos-fallback`. This makes trial output distinguish pattern/sample-driven DOS preview PCM from the bounded stream placeholder.

DOS diagnostics now also print `Decoder route: id=0 name=external-library`, `Decoder route: id=1 name=project-owned`, or `Decoder route: id=2 name=probe-by-content` before the handoff line. These ids mirror `IplayModernDecoderRoute` in the MZ diagnostic output without changing the existing handoff text.
The compact `PCM source:` line includes `route=<id>` next to renderer/provider state, so a single per-source diagnostic line identifies whether playback used the external-library, project-owned, or probe-by-content route.
Both the DOS constants and modern enum now carry comments requiring those route ids to stay synchronized.

Behavior tests assert external tracker fixtures report `Decoder route: id=0 name=external-library` and the INR fixture reports `Decoder route: id=1 name=project-owned`, so the DOS diagnostic route line is tied to runtime behavior rather than source inventory only.
The Python fixture layer now parses `Decoder route:` and `PCM source:` diagnostics into structured dictionaries. Representative DOS behavior tests use those parsers to require route id/name, PCM route id, renderer code, provider, and source module fields to agree instead of relying only on substring checks. This keeps the external-library/project-owned boundary usable for later C/C++ decoder replacement.
The fixture layer also exposes `assert_decoder_route(...)` and `assert_pcm_source_route(...)`, so representative behavior tests can require external-library/native-preview and project-owned/native route consistency through one shared assertion path. The PCM assertion helper now covers route id, renderer, provider, optional source, truncation flag, input kind, and stream-start boundary, with direct Python coverage for accepting expected route/source combinations and rejecting route/provider/input-kind mismatches.
The decoder route and PCM source parsers have direct Python coverage for successful field extraction and for rejecting missing diagnostics, so missing route/source evidence cannot silently pass future rewrite tests.

The fallback byte stream now starts at loader-relevant content instead of blindly starting at byte zero. Complete MOD streams start at pattern data, Complete S3M streams start at the first playable pattern table target, MTM streams skip the metadata/order-preview prefix, STM streams skip the title/tracker header, and FAR/669/PSM/ULT streams skip their currently printed metadata header prefixes. Capped-header/file-path modules now use the same loader-derived stream boundary when that offset is inside the real module size, instead of blindly starting at the arbitrary 24,576-byte DOS header-buffer edge. On wrap, both in-memory and file-backed playback return to that same stream-start boundary. This keeps the current DOS SB16 output more content-driven while preserving the future external-library decoder seam.

For complete in-memory modules, stream-start validation is bounded by the loaded module byte range (`player_module_header_len(...)`) rather than the recorded file size. For capped-header/file-path modules, stream-start validation is bounded by the recorded module size because playback reads from the original file path. If no loader-derived boundary is available, the fallback remains zero rather than pretending the buffer edge is meaningful tracker content.

S3M stream-start behavior now has same-path DOS coverage. A synthetic S3M fixture is overwritten in place with one byte changed before the pattern stream boundary and then with bytes changed inside the stream body; the checksum stays stable for the pre-stream-only edit and changes for the stream-body edit, proving the fallback PCM starts at the loader-selected pattern data rather than at byte zero.

MOD stream-start behavior now has matching same-path DOS coverage. A valid one-pattern MOD with zero sample lengths and inert pattern events is overwritten in place with one byte changed before offset `1084` and then with bytes changed inside the pattern stream; the checksum stays stable for the pre-pattern edit and changes for the pattern-body edit, proving MOD fallback PCM also starts at pattern data rather than accidental filename/header seeding.

MTM and STM stream-start behavior now have same-path DOS coverage as well. MTM changes before `0x42` and STM changes before `0x40` leave the fallback checksum stable, while changes after those stream starts alter the checksum. This protects the metadata-skipping starts for those external-library formats without adding handwritten MTM/STM decoders.

FAR, 669, PSM, and ULT stream-start behavior now have the same same-path coverage. Each fixture mutates the byte immediately before its metadata-prefix stream boundary and then mutates bytes after that boundary; checksums stay stable for the pre-boundary edit and change for the stream-body edit. This keeps all currently supported external-library fallback starts behavior-covered.

Playback diagnostics now include the selected fallback stream boundary on the `PCM source` line as `stream_start=...`. The stream-start behavior tests assert those offsets for S3M, MOD, MTM, STM, FAR, 669, PSM, and ULT, making boundary regressions visible without decoding full tracker content in the DOS MZ build.

Diagnostic reporting reaches that value through `player_module_diagnostic_stream_start_offset(...)`, which delegates to the same stream-start selector used by playback. This keeps the printed boundary tied to the actual fallback stream behavior while leaving a named diagnostics seam.

Capped-header/file-path diagnostics for S3M, MTM, STM, FAR, 669, PSM, ULT, and NST/MOD now assert route/provider/input/truncation state and loader-derived `stream_start=...` values through the shared structured PCM-source helper, proving the DOS fallback stream uses tracker-relevant file content rather than the near-buffer edge when the module body is read from the original file path. The same-path capped fallback tests assert those structured PCM fields for both before/after runs before comparing checksums, so checksum changes cannot hide a route/provider/input/stream-start regression.

The executable interface should stay original-shaped even while the internals move toward clean C. The DOS usage banner therefore follows `IPLAY [Switches] [FileName.Ext|@FileList.Ext]`; no-argument startup, `/0`, `-?`, and `--help` route to that usage path. `/i` plus `/I` report the in-scope SB16 16-bit stereo settings from the player hardware config. `@FileList.Ext` resolves the first non-empty list entry before module loading, keeping the original-shaped executable path covered without touching decoder state. `player_run_request(...)` and `player_run_path(...)` remain the non-CLI C entry points for the future rewrite.

The original-shaped help surface now has a direct patched-original comparison for the stable lines that both programs expose under `kvikdos`: usage and `/i`. Rewrite-specific help, supported-format, video-mode, SB16-only, text-backend, and SDL-compatible capability lines are intentionally not forced onto the original binary.

## Required gate

Run:

```sh
./rewrite/check_rewrite.sh
```

The gate verifies:

- `IPLAYC.EXE` and `IRUN.EXE` are built by `/home/xor/watcom/binl64/wcl` with `-bt=dos -3 -ms`.
- `iplay_rewrite.c` is compiled by `/home/xor/watcom/binl64/wcc` into `rewrite/.build/iplay_rewrite.obj`.
- `IPLAYC.EXE` is a DOS MZ executable and is not a PE/COFF executable.
- `IRUN.EXE` is a DOS MZ executable, is not a PE/COFF executable, and the DOS parity launcher exists.
- `IPHWRUN.EXE` is a DOS MZ executable, is not a PE/COFF executable, and links exactly the mocked player hardware runner source set.
- `IPLAYC.EXE` links the rewrite sources, not generated/fallback translated sources.
- `IRUN.EXE` links the rewrite sources, not generated/fallback translated sources.
- `IPHWRUN.EXE` links the rewrite/player hardware sources, not generated/fallback translated sources.
- `IPLAYC.EXE` and `IRUN.EXE` link exactly their intended C source sets, with no extra generated/helper sources.
- `IPLAYC.map` and `IRUN.map` do not reference generated/fallback translated sources.
- DOS player/runner links do not use C++ sources.
- DOS player/runner links do not use assembly sources or prebuilt object files.
- DOS player/runner links use exactly one ABI glue source: `iplay_abi_watcom.c`.
- The rewrite build and DOS artifacts do not depend on `libdosbox`/`dosbox`.
- The rewrite build and DOS artifacts do not depend on the original `IPLAY.EXE` binary.
- Pure C rewrite/player/runner files contain no `_asm` or `__WATCOMC__`.
- `iplay_rewrite.c` contains no direct `IplayRegs` output writes outside central `apply_*` helpers.
- Every public symbol in `iplay_abi_watcom.c` has an explicit `#pragma aux` ABI declaration.
- `#pragma aux` declarations in `iplay_rewrite.h` remain guarded by `__WATCOMC__` so normal C/C++ builds see only portable declarations.
- The generated/fallback monolithic ABI runner is not used.
- The DOS smoke player test passes.
- Function parity keeps `original/IPLAY.EXE` as the original reference executable.
- Function parity uses the DOS rewrite runner from `rewrite/.build/iplay_rewrite_dos_runner`.
- The original-vs-rewrite function parity tests pass.
- Full unit inventory coverage is enforced with `IPLAY_REQUIRE_FULL_UNIT_COVERAGE=1`, so SoundBlaster, VGA/text, and non-legacy required-proc inventory checks cannot silently skip in the gate.
- Each pytest stage in the gate is wrapped by `run_pytest_no_skip`, so skipped, xfailed, or xpassed tests fail the rewrite gate instead of being reported as acceptable green output.
- Player behavior tests are guarded against expected-failure markers; original blockers must be represented as passing assertions with explicit evidence.
- The DOS smoke script and Python behavior tests share `tests/player_behavior_fixtures.py` for module fixture generation, so the user-trial `IPLAYC.EXE` smoke path and pytest behavior matrix cannot silently drift to different tracker headers.
- The rewrite gate enforces that `rewrite/smoke_player.sh` imports the shared fixture generator and does not reintroduce inline tracker-header `bytearray(...)` construction.
- The shared fixture matrix now includes `SMOKE.NST`, exercising the advertised NoiseTracker/MOD loader path in both the DOS smoke script and Python behavior tests.
- Behavior tests parse the current DOS usage output and assert that the shared fixture generator covers every advertised format: `MOD`, `NST`, `S3M`, `STM`, `669`, `MTM`, `PSM`, `FAR`, `ULT`, and `INR`.
- DOS behavior metadata and external-decoder tests are driven from one shared `DOS_FIXTURE_METADATA_CASES` matrix, and that matrix is checked against the advertised format set.
- DOS behavior tests, smoke scripts, trial scripts, and generated DOS runners now use short bounded kvikdos timeouts. Normal `try_player.sh <module>` uses bounded diagnostic playback by default, quiet continuous playback is opt-in through `--quiet` or `IPLAY_TRIAL_DIAGNOSTICS=0`, and continuous/source-end hardware probes are capped at 3 seconds so a stuck `kvikdos` run cannot burn repeated 10-second waits.

Current validated architecture:

- Text output uses explicit `IplayTextMode` geometry for 40x25, 80x25, and project 80x50 modes.
- Bottom/status UI drawing is routed through notcurses-style `IplayNcPlane`, `IplayTerminal`, and `IplayNotcurses` wrappers.
- Text presentation, subwindow redraw, edge clipping, color attributes, and runtime subwindow behavior are covered by deterministic `IUIRUN.EXE` cell/present-event snapshots, giving current-only frame evidence until original whole-program text-memory capture is available.
- Video meter position preparation derives column offsets from the active text mode instead of assuming one fixed 80-column screen.
- Runtime UI exposes SB16 stereo output queue counters, audio levels, silence, and level reset through C facade functions.
- The DOS player entry path now initializes the pure-C `IplayRuntime` facade over a dedicated text backbuffer plus SB16-compatible SDL audio boundary, so runtime UI rendering does not overwrite translated-program metadata scratch memory.
- The DOS player SB16 hardware path starts/stops the runtime audio lifecycle through the SDL-compatible SB16 stereo callback boundary.
- The obsolete `IplayPlayerUi` facade has been removed; player-facing code now goes through the combined `IplayRuntime` facade rather than a parallel raw `IplayTextScreen` plus raw `IplayAudioOutput` wrapper.
- Runtime status text is drawn through `iplay_runtime_draw_status_line(...)`, so player UI text goes through the text-mode/runtime facade instead of direct VGA-memory writes.
- Labeled runtime status fields are drawn through `iplay_runtime_draw_status_field(...)`, with label/value attributes and active-width clipping handled by the runtime text facade.
- Runtime module/audio/video status now renders inside a framed status panel. The panel is drawn with the existing window/subwindow box primitive, and status text is routed through a one-column-inset content subwindow so borders remain visible across 40/80-column text modes.
- Numeric runtime status fields are drawn through `iplay_runtime_draw_status_u32(...)`, keeping decimal formatting and clipping inside the runtime text facade.
- Hex runtime status fields are drawn through `iplay_runtime_draw_status_hex32(...)`, so module type tags can be displayed through the runtime UI facade.
- Runtime presentation is routed through `iplay_runtime_present(...)`; the current memory-backed implementation reports the active screen byte span and can call an optional `IplayVideoPresentFn` callback, providing a future DOS/SDL flush boundary.
- Runtime audio initialization can use `iplay_runtime_init_vga_sdl_audio(...)`, making the future SDL audio sink callback explicit while still routing through the current SB16-compatible format.
- The player now initializes via `IplayRuntimeConfig` and `iplay_runtime_init_config(...)`, keeping backend callback wiring in one explicit configuration object.
- No-hardware and SDL-compatible runtime configurations remain available for tests/future host builds, while the DOS player uses the SB16 hardware configuration helper.
- Future SDL-style runtime configuration can be built by `iplay_runtime_config_sdl(...)`, centralizing both video-present and audio callback wiring.
- Runtime output spec setters and getters now route through raw output-spec helpers, isolating video backend, audio backend, and SB16 hardware flags from config construction.
- Runtime config video memory, present-callback, and video-backend setters/getters now route through raw config helpers, isolating text/notcurses setup fields from config construction.
- Runtime config audio sink, backend, and SB16 hardware setters/getters now route through raw config helpers, isolating SDL/SB16 audio setup fields from config construction.
- Runtime embedded notcurses and SDL-audio accessors now route through raw runtime helpers, isolating facade storage from runtime video/audio operations.
- Runtime video-mode status flag setters/getters now route through raw runtime helpers, isolating resize diagnostics from direct runtime storage access.
- Runtime config capabilities can be queried with `iplay_runtime_config_has_video_present(...)` and `iplay_runtime_config_has_audio_sink(...)`, avoiding backend decisions based on direct callback field inspection.
- Runtime config validity is centralized in `iplay_runtime_config_is_valid(...)`, requiring memory cells, a text mode, and an audio sink callback while keeping video present optional.
- Runtime config diagnostics are centralized in `iplay_runtime_config_error(...)`, returning stable missing-field codes for cells, mode, or audio sink.
- Human-readable runtime config diagnostics are exposed by `iplay_runtime_config_error_name(...)` for UI/log messages without duplicating switch logic.
- Text geometry now distinguishes fallback, default, supported, and maximum screen sizes with named constants; the player video buffer uses the maximum supported text screen size instead of assuming a single `80x25` mode.
- Text modes are queryable through pure-C helpers for supported-mode count, default/fallback modes, size lookup, row bytes, cells, and maximum screen bytes, so future C/C++ code can resize terminals without depending on raw `80x25` assumptions.
- DOS text presentation now preserves the video-mode selector through the player runtime output path and routes color modes to `B800:0000` and BW modes to `B000:0000`, instead of treating text geometry alone as enough to choose the hardware aperture.
- Text-mode public geometry accessors now route through raw mode helpers, keeping `IplayTextMode` layout isolated from resize and drawing code.
- Runtime/text-screen initialization is now capacity-aware. The player declares an `80x50`-sized text backbuffer, and invalid runtime configs reject too-small cell buffers before resize/present paths can overrun them.
- Video-mode changes have checked setter variants at the text-screen, terminal, notcurses, and runtime layers, so callers can distinguish successful resizes from capacity-rejected mode changes without relying on implicit mode inspection.
- Text-screen public setters and accessors now route through raw screen helpers, keeping VGA/notcurses screen storage layout isolated from resize and draw call sites.
- Bottom-layout fit checks now group row and column bounds through named raw layout helpers and raw layout accessors, keeping resize/layout predicates away from open-coded field checks.
- Bottom timing/mode row rendering now reads its layout positions through raw layout helpers, reducing direct layout-field coupling in text drawing.
- Bottom module/pattern row rendering now reads its layout positions through raw layout helpers, continuing the notcurses layout access boundary.
- Bottom flag-column rendering now reads module row and flag column through raw layout helpers.
- Bottom value/playstate rendering now reads layout positions and widths through raw layout helpers.
- SB16-compatible SDL audio devices now expose their active PCM format, bytes-per-frame, and SB16-compatible status through pure-C query helpers, keeping future SDL output code independent of nested sink/output structs.
- The runtime facade exposes the same audio format, bytes-per-frame, and SB16-compatible status directly, so player code can stay on the high-level runtime boundary instead of reaching through nested audio-device internals.
- Audio backend naming is now exposed through pure-C helpers and the player status line derives its audio label from the runtime backend instead of a hardcoded player macro.
- The runtime facade now exposes whether real audio hardware is enabled; the DOS player reports the SB16 hardware setting through the runtime API instead of a player-local macro.
- `IplayAudioOutput` write paths now route sink, level, source-format, and scratch access through pure-C facade helpers, keeping SB16 stereo output behavior covered while reducing nested-struct coupling for the future SDL replacement.
- Terminal present/draw paths now go through notcurses-style terminal accessors for screen cells, present callback, and text-screen routing, reducing raw VGA-backed struct coupling before host terminal backends are added.
- Terminal public setters and accessors now route through raw terminal helpers, isolating backend, present-callback, and embedded text-screen storage from notcurses call sites.
- Notcurses terminal access now routes through raw notcurses helpers, keeping the embedded terminal layout isolated from resize/render call sites.
- Ncplane geometry and backing-cell public helpers now route through raw plane helpers, keeping rows, columns, stride, origin, and cell storage isolated from draw call sites.
- Ncplane cursor and cell-byte public helpers now route through raw plane helpers, keeping cursor movement and text-cell mutation isolated from drawing logic.
- Window plane initialization and access now route through raw window helpers, keeping embedded plane storage isolated from notcurses-style window operations.
- Runtime video capacity and bottom-layout checks now use terminal facade queries instead of reaching through the runtime terminal's embedded text-screen storage.
- SDL-like audio device opening now stores its accepted config through a device facade setter, keeping SB16 stereo device configuration mutation behind the audio wrapper boundary.
- Audio sink writes now query the active state, format, capacity, write callback, and callback user through sink helpers before emitting PCM, tightening the SB16 stereo sink API without changing queued audio bytes.
- Audio sink queue accounting now updates written, dropped, underrun, and remaining-capacity counters through sink mutator helpers instead of open-coded counter changes in the write path.
- Audio sink start/stop now route active-state changes through a sink setter, keeping lifecycle mutation behind the SB16 sink facade.
- Audio sink initialization and counter reset now route format, callback, counter, capacity, and active-state setup through named sink setters/clearers instead of open-coded initialization fields.
- Audio sink public accessors and mutators now route through raw sink helpers, keeping SDL-compatible sink layout access isolated from call sites.
- Audio output initialization now sets source format and scratch storage through output facade setters, keeping mixer/SB16 output setup behind named C helpers.
- Audio output sink, source-format, and scratch public helpers now route through raw output helpers, keeping SDL-compatible output layout access isolated.
- Audio level reset and SB16 peak calculation now update meter state through `IplayAudioLevels` setter/clearer helpers instead of open-coded level-field writes.
- Audio level public setters now route through raw level helpers, keeping fast meter state layout isolated from SB16/text rendering paths.
- Audio level meter rendering now reads 16-step left/right values through level accessors, keeping text drawing independent of `IplayAudioLevels` layout.
- Audio format public accessors and setters now route through raw format helpers, keeping SB16/SDL format layout access isolated from call sites.
- SDL-like SB16 audio config construction now routes PCM format, callback/userdata, and backend/hardware fields through config setter helpers.
- SDL-like SB16 audio config sample-buffer size now uses a config setter, keeping the whole config construction path behind named helpers.
- SDL-like audio config public setters and getters now route through raw config helpers, isolating frequency, PCM format, callback, backend, and hardware fields from device setup code.
- Terminal VGA initialization now routes backend and present-callback setup through terminal setters, tightening the notcurses-style text facade.
- Terminal present-state checks now go through the present-callback accessor instead of inspecting the callback field directly.
- SDL-like audio device open now validates the SB16 16-bit stereo format through a single config predicate, keeping the SB16-only driver constraint explicit at the wrapper boundary.
- SDL-like audio device open now applies accepted config through a device helper, centralizing config/backend/hardware state synchronization.
- SDL-like audio device open now finishes accepted SB16 device setup through one helper that applies config and initializes the paused state.
- SDL-like audio device config and output public accessors now route through raw device helpers, keeping embedded config/output storage isolated from SB16 queue and lifecycle paths.
- SDL-like audio device backend, hardware-enabled, and paused state now stay behind named helpers; private static `_raw` function bodies are not used anywhere in the rewrite C source set.
- SDL-like audio queue and callback paths now share device helpers for queue readiness plus byte/frame conversion, centralizing SB16 stereo stream sizing for future SDL replacement.
- Playback status strings are now exposed by the runtime audio facade, so player UI text does not hardcode backend-specific playback state.
- SDL-compatible audio devices now expose hardware-enabled state and playback status text directly; runtime audio status delegates to that device-level contract.
- SB16 hardware runtime status text is characterized across initial/start/pause/resume/stop lifecycle states, preserving the current hardware-bound "Playback enabled" UI contract for the future SDL replacement.
- Player runtime status rows and audio-level meter position/width are named constants instead of raw coordinates, keeping text-mode layout assumptions centralized for later resize work.
- Player runtime status colors are named constants rather than raw VGA attribute literals, keeping text/notcurses presentation choices centralized.
- Audio backend label, hardware flag, level meter, and playback status rendering are grouped behind `iplay_runtime_draw_audio_status(...)`, keeping player code on the runtime facade.
- Module title/path/size/loader and tag rendering are grouped behind runtime helpers, so the player no longer calls low-level status-line drawing functions directly.
- Runtime module-status helpers are characterized directly by the parity runner, preserving title/module/size/loader/tag text-cell output while player code stays on higher-level runtime calls.
- Runtime audio-status rendering is characterized directly by the parity runner, preserving audio backend, hardware flag, and playback status output.
- Module status is now represented by `IplayModuleStatus`, with runtime drawing helpers that consume the struct so future C module-loader code has a typed UI handoff instead of long status argument lists.
- `IplayModuleStatus` now has pure-C accessors and a type setter, so player/runtime code no longer needs direct field mutation for the loaded module tag.
- `IplayModuleStatus` public setters/getters now route through raw status helpers, isolating loader/UI handoff storage from runtime drawing code.
- `IplayModuleStatus` accessors/setter are characterized directly by the parity runner, covering the typed module-loader-to-UI handoff independent of rendered text cells.
- Module tag hex formatting is exposed through `iplay_module_status_type_hex(...)`, giving pure-C loader/status code the same uppercase 8-digit representation used by runtime UI.
- Struct-based runtime module-tag rendering now uses `iplay_module_status_type_hex(...)` internally, keeping module tag formatting centralized.
- `IplayModuleStatus` now exposes an explicit type-clear helper for the not-loaded/no-tag state, covered by the module-status API characterization.
- `iplay_runtime_draw_status_block(...)` renders module status, audio status, and module tag from one typed `IplayModuleStatus`, giving player code a single high-level status-block call after module load.
- The DOS player now uses `iplay_runtime_draw_status_block(...)` for both pre-load/no-tag and post-load/tagged status rendering, avoiding direct module/audio status helper calls in player code.
- Runtime config startup is grouped in `iplay_runtime_start_config(...)`, which initializes config, starts audio, and applies the requested video mode so player setup order is centralized.
- `iplay_runtime_start_config_checked(...)` exposes video-mode resize success/failure during startup, while still centralizing config init and audio start.
- The DOS player uses the checked runtime startup helper, preserving current default-mode behavior while making future buffer/mode startup failures observable.
- `IplayRuntime` now records whether the last video-mode request succeeded, with `iplay_runtime_video_mode_ok(...)` exposing that state for player/UI diagnostics.
- Runtime status-block rendering now includes a video-mode success field, making rejected terminal mode changes visible in the text UI path.
- Runtime video status rendering is characterized directly by the parity runner, including rejected mode changes caused by insufficient text-buffer capacity.
- Runtime video diagnostics now include `iplay_runtime_video_status_text(...)` for readable accepted/rejected mode status.
- Runtime video diagnostics also expose parser-friendly accepted/rejected tokens for stable parity runner output.
- Invalid runtime configs passed to `iplay_runtime_init_config(...)` fall back to an inert 40x25 memory-backed runtime instead of leaving partially initialized state.
- The DOS player renders the selected loader symbol through the runtime status facade, so loader decisions are visible in the memory-backed UI path as well as stdout.
- Module startup now routes CLI-derived paths through a typed `PlayerModuleRequest`, isolating command-line parsing as an edge adapter before the future non-CLI C API replaces it.
- `PlayerModuleRequest` now has a path-based initializer used by the CLI adapter, giving future non-command-line C startup code a direct request construction path.
- Player execution now has a request-based `player_run_request(...)` core, leaving `player_run(argc, argv, ...)` as a thin command-line adapter around the clean C startup path.
- Usage handling now stays in the CLI adapter, while `player_run_request(...)` only loads and runs the supplied module request for the future non-command-line C API.
- Player startup now also has `player_run_path(...)`, giving future clean C callers a direct module-path entry without constructing command-line arguments.
- The command-line startup adapter is now named `player_run_cli(...)`, leaving the player core represented by request/path-based entry points instead of an `argc`/`argv` API.
- The DOS player displays the selected audio boundary and SB16 hardware flag through runtime status fields.
- The DOS player renders runtime audio level meters through `iplay_runtime_draw_audio_levels(...)`, so the SDL/SB16 level path is represented in the text UI.
- The DOS player renders the current SB16 playback-enabled state through the runtime text facade, not just stdout.
- Runtime lifecycle teardown is exposed through `iplay_runtime_shutdown(...)`, which stops the audio facade and clears runtime-visible audio levels. The DOS player trial exit keeps its own localized shutdown policy so the process can return after bounded playback without blocking in the runtime audio facade.
- Runtime facade accessors now cover common text geometry, bottom-layout fit, audio spec/backend, audio active state, and audio counters; checked runner paths are guarded against reaching through `IplayRuntime` into nested notcurses, terminal, screen, SDL-device, output, or sink internals.

## Hardware driver scope

Only SB16 16-bit stereo is in scope for hardware audio. The current target keeps the legacy no-device/SB-stub ABI behavior tested where needed, while the DOS player routes playback through the SB16 stereo hardware wrapper and the higher-level SDL-compatible audio boundary.

The DOS player hardware-specific port IO, far-memory DMA address calculation, far-memory copies, and text-video memory lookup are confined to the `DosHardwareIo` wrapper table. The runnable player path is inventory-guarded against introducing GUS/PAS/WSS/AdLib/Tandy/PC-speaker/Sound-Source/Covox hardware driver paths.

`DosHardwareIo` callback lookup now crosses raw table accessors before reaching the public port, far-memory, copy, timer, keyboard, and text-memory helpers, keeping wrapper-table layout out of the hardware call sites.

DOS text-memory lookup is split into color and mono callbacks in `DosHardwareIo`; the text presenter chooses between them through the active video-mode selector, keeping future terminal-size changes independent from color/BW hardware segment selection.

The player text presenter is executable under the same mocked DOS hardware IO table through `IPHWRUN.EXE`: the test captures 80x25 color-mode presentation to `B800:0000`, 40x25 BW-mode presentation to `B000:0000`, 80x50 project-mode presentation to `B800:0000`, and byte-count clamping to the active text mode instead of the caller-provided maximum buffer size.

`IPHWRUN.EXE` also exercises a loaded-module player path with a synthetic MOD header, proving the player-level path reaches both DOS hardware seams in one run: SB16 DMA/audio submission and VGA text presentation through the runtime status UI. That route now captures deterministic SB16 PCM byte count, checksum, first sample word, tail sample word, final speaker-off DSP write, and presented text markers for the module path, `sb16-stereo` audio backend, and playback-enabled status across the hardware copy/shutdown seams.

The same loaded-module hardware path is now characterized across multiple DOS text modes. `IPHWRUN.EXE` captures the default 80x25 color frame, 40x25 BW frame at `B000:0000`, 80x25 BW frame at `B000:0000`, and 80x50 project frame at `B800:0000`, with byte counts matching each active geometry while SB16 playback still runs through the same module path.

`DosHardwareIo` field reads now sit behind macro field boundaries and public hardware IO helpers, keeping call sites off direct callback-table layout access.

The DOS player is also inventory-guarded against direct `IplayAudioOutput`, `IplaySdlAudioDevice`, `iplay_audio_output_*`, `iplay_sdl_audio_device_*`, or `iplay_runtime_audio(&runtime)` usage. Player audio setup must stay at the runtime configuration boundary plus the SB16 hardware callback seam.

SB16 hardware config reads for base port, IRQ, DMA16 channel, and sample rate now cross player-facing helpers over macro field boundaries, keeping the hardware state layout behind a smaller SB16 boundary.

The player SB16 hardware wrapper is executable under a mocked DOS hardware IO table through `IPHWRUN.EXE`: the test captures reset, DMA16 mask/mode/address/count, DSP rate/start/sample-count writes, aligned PCM copy length, and shutdown speaker-off sequencing without touching real hardware.

The remaining DOS player translated-memory audio defaults are named through `IPLAY_PLAYER_MEM_*` and `IPLAY_PLAYER_DEFAULT_*` constants and accessed through small player helpers instead of raw `mem[0x....]` literals.

The DOS player static and bottom/status UI seed values are also named as `IPLAY_PLAYER_DEFAULT_*` constants before crossing into `iplay_runtime_render_static(...)` and `iplay_runtime_render_bottom(...)`, avoiding unexplained positional literals in the runnable player path.

The DOS player delegates module-header metadata printing through `print_loader_metadata(...)`; metadata selection dispatches on compact `LoaderInfo.kind` values instead of loader-symbol string comparisons.

DOS behavior tests assert tracker formats report the external tracker decoder handoff and INR reports the project decoder handoff, keeping the future C/C++ rewrite boundary aligned with library-backed tracker decoding.

Original whole-program comparison remains blocked under plain `kvikdos`, but the loader blocker is now executable evidence: tests parse `original/IPLAY.EXE` and assert its `SS:SP = 2451:1000` stack lies beyond the minalloc image, matching the deterministic `DOS .exe stack pointer after end of program memory` failure. A temporary minalloc-patched copy now reaches original program logic, reports the expected missing-config message without `IPLAY.CFG`, prints the original `/?` help with a valid minimal config fixture, prints original `/i` sound settings for an AdLib config, and reports `Module not found.` with exit code `2` for a missing module under that valid config. SB16-style `/i` configs currently hit kvikdos `offset overflow in print` after the original `callsubx` path; the underlying SB16 no-device `callsubx` state is already covered by original-vs-rewrite function parity, while whole-program SB16 `/i` still needs a harness workaround.

The plain-original whole-program blocker is now represented by passing tests only; the previous expected-failure test was converted into an explicit assertion of the deterministic kvikdos stack-layout failure.

The DOS rewrite missing-module path now matches the observed original patched-harness behavior by reporting `Module not found.` and returning exit code `2`.

An existing corrupt `.MOD` under the original patched harness reaches the UI/module path, exits `0`, and emits no console text. The rewrite rejects too-short MOD/NST input before stdout module reporting or playback diagnostics, so corrupt input is not mistaken for a library-backed playable tracker module. A passing behavior test also proves that adding kvikdos `--hlt-dump` to this normal whole-program UI path does not create a memory dump, because the program exits normally instead of through the call-HLT harness. Original `B800:0000` UI/module screen comparison therefore still needs a different capture path.

The coverage inventory now keeps that original UI parity gap explicit. It asserts that `tests/COVERAGE.md` still lists missing original `B800:0000`/text-memory comparisons for screen rendering, subwindows, and fast audio levels, and that the behavior suite still proves the current `--hlt-dump` attempt cannot capture the normally exiting original UI path. This prevents current-only screen tests from being mistaken for full original UI parity.

Loader module-type tags are derived from one-byte `LoaderInfo.kind` values via `loader_module_type_tag(...)`, so `main` consumes typed loader data instead of running a symbol-string-to-tag dispatch helper while keeping the DOS small-model table compact.

Module-header signature detection also resolves loaders through `find_loader_by_kind(...)`, keeping loader symbols as display/status strings rather than internal dispatch keys.

Loader metadata field access now routes through loader accessors for extension, symbol, name, and kind, keeping module detection, metadata dispatch, summaries, and status initialization off direct `LoaderInfo` fields.

Loader metadata field reads now sit behind macro field boundaries and public loader helpers, keeping call sites off direct loader-table layout access.

DOS player usage-option parsing is localized in `player_requested_usage(...)`, keeping raw option string checks out of the main startup flow.

DOS player translated-program startup is localized in `player_start_translated_program(...)`, preserving `iplay_start_bounded(...)` plus translated audio defaults behind one named boundary.

DOS player process initialization is localized in `player_init_process(...)`, grouping player memory zeroing, text backbuffer zeroing, register zeroing, and DOS hardware IO reset.

Module loader selection policy is localized in `detect_loader_for_module(...)`, which applies header-signature detection before extension fallback instead of spelling that fallback sequence directly in `main`.

DOS player stdout module reporting is localized in `print_module_summary(...)`; `main` delegates summary and metadata reporting instead of owning the raw `printf` sequence.

DOS player module open/type failure messages are localized through `player_report_open_failed(...)` and `player_report_unsupported_module(...)`, leaving `main` responsible only for control flow and exit codes.

DOS player playback backend reporting is localized through `player_report_playback_output(...)`, keeping SB16/discard reporting details out of the main startup flow.

DOS player stdout report flushing is localized through `player_flush_reports(...)`, keeping output synchronization out of the main startup flow.

DOS player loaded-module reporting is grouped behind `player_report_loaded_module(...)`, so `main` no longer owns the stdout summary/metadata/playback reporting sequence.

DOS player module file-info reading, loader detection, and module type-tag derivation are grouped behind `player_load_module_info(...)`, giving future C/C++ loader replacement one tested player seam.

Loaded module state now crosses player helpers as `PlayerModuleInfo` instead of parallel path/size/header/loader/tag arguments, making the future C/C++ loader boundary typed and test-guarded.

Player module-info state access now sits behind macro field boundaries and typed helpers, keeping future C/C++ loader handoff helpers off direct `PlayerModuleInfo` field reads and writes.

Module path access for player reporting is centralized through `player_module_path(...)`, keeping report helpers on the typed module boundary instead of directly spelling struct field access at call sites.

Module loader access for player reporting/status handoff is centralized through `player_module_loader(...)`, further keeping `PlayerModuleInfo` field access behind typed helper boundaries.

Module size access for reporting and runtime-status handoff is centralized through `player_module_size(...)`, reducing direct `PlayerModuleInfo` field reads in player output paths.

Module type-tag access for reporting and runtime-status handoff is centralized through `player_module_type_tag(...)`, keeping loaded-module tag reads behind the typed module boundary.

Module setup and loading are grouped behind `player_prepare_module(...)`, so session code performs one typed prepare step before either reporting a load failure or running the loaded module.

Module load outcomes are named as `IPLAY_PLAYER_MODULE_*` status constants, so player startup no longer depends on raw `0`/negative/positive status meanings for loader control flow.

Module load success is checked through `player_module_load_ok(...)`, keeping session flow independent of the exact OK status comparison.

Module load failure reporting and exit-code mapping are localized in `player_report_module_load_failure(...)`, leaving `main` with one named non-OK load-status branch.

Module-load status to process-exit mapping is named through `player_module_load_exit_code(...)`, keeping reporting and exit-code policy separated inside the player boundary.

Player process exit codes are named as `IPLAY_PLAYER_EXIT_*` constants, so session and load-failure control flow no longer relies on raw success/error return values.

Usage reporting and its success exit code are localized in `player_report_usage(...)`, keeping the session control flow on named report helpers for both usage and module-load failure paths.

DOS player runtime configuration construction is localized in `player_configure_runtime(...)`; `main` passes the resulting `IplayRuntimeConfig` to the runtime startup facade.

DOS player runtime startup is localized in `player_start_runtime(...)`, preserving checked startup with `IPLAY_TEXT_DEFAULT_VIDEO_MODE` while keeping startup policy out of `main`.

DOS player runtime status rendering is localized in `player_render_runtime_status(...)`, grouping static screen rendering, bottom/status fields, status block drawing, and presentation behind one player-level runtime facade call.

DOS player runtime teardown is localized in `player_shutdown_runtime(...)`, preserving SB16 hardware shutdown while keeping teardown policy out of `main`.

DOS player module-status construction is localized in `player_init_module_status(...)`, keeping loader-to-runtime-status handoff behind a typed player helper.

DOS player runtime UI lifecycle is grouped behind `player_run_runtime_ui(...)`, so `main` crosses one text/audio runtime boundary instead of spelling out configure/start/render/shutdown sequencing.

DOS player loaded-module execution is grouped behind `player_run_loaded_module(...)`, so `main` no longer owns translated-program start, module-status setup, stdout reporting, or runtime UI sequencing after a successful module load.

DOS player prepared-module dispatch is grouped behind `player_run_prepared_module(...)`, keeping `player_run(...)` on a single named loaded-module execution call across text-UI and non-UI builds.

Prepared-module execution now returns the session success exit code, so `player_run(...)` returns directly from the loaded-module boundary instead of carrying a separate trailing success return.

Module header access for metadata reporting is centralized through `player_module_header(...)` and `player_module_header_len(...)`, keeping the report path on typed module accessors.

Loaded module loader and type-tag assignment are centralized through `player_module_set_loader(...)` and `player_module_set_type_tag(...)`, keeping module-load writes behind typed helper boundaries.

Module file-info reading is localized through `player_read_module_file_info(...)`, naming the file-size/header load boundary while preserving the existing static header storage.

Module loader detection now consumes `PlayerModuleInfo` through path/header accessors, avoiding direct field reads in the loader detection call site.

Module source binding is localized through `player_module_set_source(...)`, naming the path/header initialization handoff while keeping the existing module storage layout.

Loaded module reset/default state is localized through `player_module_clear_loaded_state(...)`, separating source binding from loader/header-size/type reset during module initialization.

Module file-info output targets are centralized through `player_module_size_out(...)`, `player_module_header_data(...)`, and `player_module_header_len_out(...)`, so file reads no longer take direct field addresses in the load path.

DOS hardware calls now cross named wrapper helpers (`dos_hw_io_read_port(...)`, `dos_hw_io_write_port(...)`, `dos_hw_io_far_physical(...)`, `dos_hw_io_copy_to_far(...)`, and `dos_hw_io_text_color_memory(...)`) instead of dereferencing the hardware vtable directly inside SB16/text rendering logic.

The DOS player now reaches the SB16 hardware singleton through `player_sb16_hardware(...)` for runtime configuration, reporting, and shutdown, reducing direct player call-site coupling to the current hardware instance.

Runtime output selection is localized in `player_configure_runtime_output(...)`, and SB16 teardown is localized in `player_shutdown_audio_hardware(...)`, keeping the player runtime lifecycle on named SDL/notcurses-compatible boundary helpers.

The static translated memory and text backbuffer are now reached through `player_memory(...)` and `player_video_memory(...)` at player call sites, preserving static DOS storage while reducing direct global-buffer coupling.

The module header scratch buffer is now localized behind `player_module_header_buffer(...)`, so `player_run(...)` no longer owns raw header storage while preserving the existing static buffer.

Module path argument selection is localized through `player_module_arg(...)`, so usage checks and module preparation no longer reach into `argv[1]` directly.

Requested-module preparation is localized through `player_prepare_requested_module(...)`, grouping command argument selection and header scratch-buffer selection behind one session-level load boundary.

Process initialization is split into `player_clear_translated_memory(...)`, `player_clear_video_memory(...)`, `player_init_registers(...)`, and `player_init_hardware_io(...)`, keeping DOS storage reset and hardware-wrapper reset as replaceable startup steps.

Runtime UI activation is localized in `player_activate_runtime_ui(...)`, grouping configuration and startup before the separate render/shutdown phases.

Loaded-module UI preparation is localized in `player_prepare_loaded_module_ui(...)`, grouping translated startup and module-status setup before reporting and runtime rendering.

Loaded-module presentation is localized in `player_present_loaded_module(...)`, grouping stdout module reporting with notcurses/runtime UI presentation after startup.

The no-text-UI loaded-module path mirrors the same phase split through `player_prepare_loaded_module_no_ui(...)` and `player_present_loaded_module_no_ui(...)`.

Module loader detection and module type-tag application are localized in `player_detect_module_loader(...)` and `player_apply_module_type_tag(...)`, keeping `player_load_module_info(...)` on named load phases.

Module load-status decisions now use `player_module_load_open_failed(...)` and `player_module_load_unsupported(...)`, so reporting and exit-code mapping no longer branch on raw status constants directly.

Module load readiness checks are named through `player_module_file_info_loaded(...)` and `player_module_loader_available(...)`, keeping file-read and loader-availability failures explicit in `player_load_module_info(...)`.

Module load status values are now returned through `player_module_open_failed_status(...)`, `player_module_unsupported_status(...)`, and `player_module_ok_status(...)`, preserving the ABI values while removing raw status constants from the load flow.

Process exit status values are now returned through `player_exit_open_failed_status(...)`, `player_exit_unsupported_status(...)`, and `player_exit_ok_status(...)`, keeping module-load to process-exit mapping on named helpers.

Prepared-module execution now returns success through `player_exit_ok_status(...)`, keeping the post-run success path on the same named process-exit boundary.

Usage reporting now returns success through `player_exit_ok_status(...)`, keeping usage and post-run success on the same named process-exit boundary.

The unused DOS player `audio_backend_prepare(...)` hook was removed, eliminating a dead call path to translated `iplay_snd_on_parnt_bounded(...)` from the runnable player source while keeping SB16 runtime output unchanged.

Translated-memory byte reads/writes in the player path now go through `player_set_translated_memory_byte(...)` and `player_translated_memory_byte(...)`, so audio defaults and master-volume reads no longer index translated memory directly at call sites.

SB16 playback reporting now reads hardware configuration through `player_sb16_base_port(...)`, `player_sb16_irq(...)`, `player_sb16_dma16(...)`, and `player_sb16_sample_rate(...)` instead of dereferencing the hardware struct in the reporting call site.
The DOS player now reads the standard `BLASTER` environment variable before `/i` reporting or module playback. `A` overrides the SB base port, `I` overrides IRQ, and `H` overrides the 16-bit DMA channel; defaults remain 220h/IRQ5/DMA5 when tokens are missing. A `D` token is accepted only when it names a valid 16-bit DMA channel (`5..7`) and no `H` token was present.

The SB16 SDL-style audio callback now maps callback user data through `sb16_audio_user_hardware(...)`, so the callback body no longer performs the raw `void *` to hardware-struct cast directly.

SB16 callback DMA byte-count clipping/alignment is localized in `sb16_audio_dma_copy_count(...)`, keeping the SDL-style audio callback body on named hardware-write phases.

SB16 callback reset/detection readiness is now behind `sb16_audio_ensure_ready(...)`, so the callback body stays organized around named SDL-style audio write phases.

SB16 callback PCM-to-DMA copying is now isolated in `sb16_audio_copy_to_dma(...)`, creating a named replacement point between SDL-style audio input and DOS SB16 DMA output.

SB16 callback DMA playback start is now isolated in `sb16_audio_start_dma_block(...)`, leaving the callback as named readiness, sizing, copy, and start phases.

SB16 DSP block sample-count calculation is now named as `sb16_block_sample_count(...)`, making the 16-bit stereo block-start command sequence less translated and easier to cover directly.

SB16 DSP command bytes for output-rate, speaker-enable, 16-bit output, and stereo signed mode are now named `IPLAY_SB16_DSP_*` constants instead of inline literals in the hardware start path.

SB16 DSP reset/read/write port offsets, ready masks, and reset-ack byte are now named constants, removing raw DSP-port literals from the wrapper path.

SB16 16-bit DMA controller ports and mode/mask bits are now named constants in `sb16_program_dma16(...)`, keeping DMA programming explicit without raw controller literals.

SB16 16-bit DMA word-address and word-count conversions are now named as `sb16_dma16_word_address(...)` and `sb16_dma16_word_count(...)`, reducing inline hardware math in DMA programming.

SB16 16-bit DMA channel-index and terminal-count conversions are now named as `sb16_dma16_channel_index(...)` and `sb16_dma16_terminal_count(...)`, leaving `sb16_program_dma16(...)` focused on ordered controller writes.

SB16 16-bit DMA mask, playback-mode, and unmask writes are now named as `sb16_dma16_mask_channel(...)`, `sb16_dma16_set_playback_mode(...)`, and `sb16_dma16_unmask_channel(...)`, reducing raw controller expressions in the DMA setup body.

SB16 16-bit DMA address/page and terminal-count port writes are now isolated in `sb16_dma16_write_address(...)` and `sb16_dma16_write_count(...)`, leaving `sb16_program_dma16(...)` as an ordered setup pipeline. The address/count/page ports are selected from the configured 16-bit DMA channel, so BLASTER `H5`, `H6`, and `H7` program C4/C6/8B, C8/CA/89, and CC/CE/8A respectively instead of always using the H5 ports.

SB16 16-bit DMA flip-flop clearing is now isolated in `sb16_dma16_clear_flipflop(...)`, so `sb16_program_dma16(...)` uses named helper phases for all controller writes.

SB16 16-bit stereo DSP playback command writes are now isolated in `sb16_start_16bit_stereo_dsp(...)`, leaving block start as DMA setup, rate setup, DSP start, and state update.

SB16 block-start state accounting is now isolated in `sb16_commit_started_block(...)`, so the block start path has named phases for setup, DSP start, and commit.

SB16 active-state and playback-counter field writes now cross raw hardware-state mutators before reaching semantic playback helpers, keeping block-start bookkeeping independent of struct layout.

SB16 block-start payload validation and 4-byte alignment are now named as `sb16_block_has_payload(...)` and `sb16_block_aligned_byte_count(...)`, removing inline normalization from the hardware start path.

SB16 block-start preparation now produces a `Sb16PreparedBlock` through `sb16_prepare_16bit_stereo_block(...)`, grouping the aligned byte count and DSP sample count before DMA/rate/DSP start phases.

SB16 prepared-block byte-count and sample fields now cross raw struct accessors before reaching the public prepared-block helpers.

SB16 prepared-block reads now go through `sb16_prepared_block_byte_count(...)` and `sb16_prepared_block_samples(...)`, so the block start path no longer reads prepared fields directly.

DOS text presentation now clips requested cell bytes through `dos_text_present_byte_count(...)`, separating mode-capacity handling from the video-memory copy phase.

DOS text presentation now obtains video memory through `dos_text_present_video_memory(...)` and copies cells through `dos_text_present_cells(...)`, leaving the callback as named clipping, destination, and present phases.

DOS text presentation now prepares a `DosTextPresentFrame` through `dos_text_prepare_present(...)` and reads it through frame accessors, making the callback a prepare-and-present wrapper.

DOS text presentation now commits prepared frames through `dos_text_present_frame(...)`, so the callback delegates both frame preparation and final cell presentation.

DOS text presentation callback user data now passes through `dos_text_present_user(...)`, leaving a named backend-state hook instead of an inline ignored parameter.

DOS text present frame construction now uses `dos_text_present_frame_set_byte_count(...)` and `dos_text_present_frame_set_video(...)`, isolating frame field writes from preparation logic.

DOS text present frame construction now also goes through `dos_text_present_frame_init(...)`, grouping destination and clipped-byte-count assignment behind one frame initialization step.

DOS text present-frame state now sits behind macro field boundaries and presentation helpers, keeping notcurses-style presentation code off direct frame layout access.

DOS text present preparation now computes destination and clipped byte count through `dos_text_prepare_present_video(...)` and `dos_text_prepare_present_byte_count(...)` before frame initialization.

SB16 hardware shutdown now emits the speaker-off DSP command through the named `IPLAY_SB16_DSP_SPEAKER_OFF` constant and `sb16_stop_16bit_stereo_dsp(...)` helper instead of an inline raw command byte. Shutdown uses a direct hardware-wrapper port write for that final command so the bounded DOS trial does not spin indefinitely waiting for DSP write-ready after playback output has already completed.

SB16 DSP reset assert/release values and settle-loop count are now named as `IPLAY_SB16_DSP_RESET_ASSERT`, `IPLAY_SB16_DSP_RESET_RELEASE`, and `IPLAY_SB16_RESET_SETTLE_READS`, keeping reset sequencing explicit in the hardware wrapper.

SB16 DSP read/write polling now uses the named `IPLAY_SB16_DSP_IO_SPIN_LIMIT` constant instead of inline timeout literals in the hardware wrapper wait loops.

SB16 DSP word argument byte splitting now goes through `sb16_dsp_word_hi(...)` and `sb16_dsp_word_lo(...)`, keeping sample-rate and block-length command writes on named wrapper helpers.

SB16 DSP port-address arithmetic now goes through `sb16_dsp_write_data_port(...)`, `sb16_dsp_read_status_port(...)`, `sb16_dsp_read_data_port(...)`, and `sb16_dsp_reset_port(...)`, keeping low-level port offsets behind the hardware wrapper.

SB16 DMA flip-flop clearing now uses the named `IPLAY_SB16_DMA_CLEAR_FLIPFLOP` controller value instead of a raw zero write.

SB16 DMA address/page/count byte extraction now goes through `sb16_dma16_byte_lo(...)`, `sb16_dma16_byte_hi(...)`, and `sb16_dma16_page_byte(...)`, keeping controller write byte selection behind named helpers.

SB16 DMA mask command construction now goes through `sb16_dma16_disable_mask_value(...)` and `sb16_dma16_enable_mask_value(...)`, keeping channel mask bytes explicit before controller writes.

SB16 DMA playback-mode command construction now goes through `sb16_dma16_playback_mode_value(...)`, keeping the DMA mode byte explicit before the controller write.

SB16 shutdown state clearing now goes through `sb16_mark_inactive(...)` and `sb16_mark_dma_idle(...)`, keeping device-state mutation behind named hardware-wrapper helpers.

SB16 DMA programming state now goes through `sb16_mark_dma_programmed(...)`, keeping `dma_programmed` and `last_block_bytes` updates behind one hardware-wrapper state helper.

SB16 DMA programmed-state and last-block byte fields now cross raw hardware-state mutators before reaching the semantic DMA bookkeeping helpers.

SB16 playback-start commit state now goes through `sb16_mark_active(...)`, `sb16_count_started_block(...)`, and `sb16_count_written_bytes(...)`, keeping active/counter mutation behind named device-state helpers.

SB16 reset detection now goes through `sb16_detected_from_reset_ack(...)`, `sb16_mark_detected(...)`, and `sb16_is_detected(...)`, keeping probe-result state behind named hardware-wrapper helpers.

SB16 detected-state field access now crosses raw detected-flag helpers before reaching the public detection helpers, keeping reset/probe logic independent of the hardware struct layout.

SB16 ready and shutdown paths now read detection state through `sb16_is_detected(...)` instead of direct `hw->detected` checks.

SB16 reset, rate, DMA-channel, DSP-start, and DSP-stop paths now read hardware configuration through `player_sb16_*` accessors instead of direct `DosSb16Hardware` field reads.

DOS text present frames now carry source cells as well as destination and byte count, so the callback prepares one complete flush object before presenting it.

DOS text present frames now also carry the present callback user pointer, preserving backend context in the prepared flush object for future notcurses/SDL-style presenters.

DOS text presentation now registers a `DosTextPresenter` backend object as the callback user and resolves video memory through that presenter instead of treating present user data as unused.

DOS text presentation now routes the frame copy through the `DosTextPresenter` backend object as well as video-memory lookup, making destination and flush behavior replaceable together.

DOS text present frame presentation now resolves its backend through `dos_text_present_frame_presenter(...)`, keeping presenter lookup behind the prepared-frame accessor boundary.

DOS text presenter construction now goes through `dos_text_presenter_init(...)` during player process initialization instead of relying on a static backend table initializer.

DOS text presenter default backend selection now goes through `dos_text_presenter_init_vga_text(...)`, keeping concrete VGA text wiring behind one backend-specific initializer.

DOS text presenter backend fields now cross raw presenter accessors before reaching public presenter setup and invocation helpers, keeping the notcurses-style backend object layout isolated.

DOS player SB16 audio callback wiring now goes through a local `PlayerAudioBackend` config object, so runtime configuration consumes an explicit backend write/user pair instead of hardcoded callback arguments while avoiding extra persistent DOS memory use.

DOS player SB16 audio backend selection now goes through `player_audio_backend_init_sb16(...)`, keeping concrete SB16 callback/user wiring behind one backend-specific initializer.

DOS player SB16 runtime output configuration is now isolated in `player_configure_runtime_sb16_output(...)`, keeping the high-level runtime output dispatcher free of callback wiring details.

The SDL-compatible fallback runtime branch now also uses a local `PlayerAudioBackend` initialized by `player_audio_backend_init_discard(...)`, avoiding raw discard-callback/NULL-user wiring while keeping non-SB legacy drivers out of scope.

The SDL-compatible fallback runtime output configuration is now isolated in `player_configure_runtime_sdl_output(...)`, making the high-level runtime output dispatcher symmetric across SB16 and fallback builds.

DOS player video-present callback wiring now goes through a local `PlayerVideoBackend` initialized by `player_video_backend_init_text(...)`, so runtime configuration consumes explicit video present/user pairs instead of hardcoded text callback arguments.

Player audio/video backend state now sits behind macro field boundaries and public SDL/notcurses bridge helpers, keeping call sites off direct backend-field reads and writes.

DOS player runtime video cells, capacity, and default mode now go through `player_runtime_video_cells(...)`, `player_runtime_video_capacity(...)`, and `player_runtime_video_mode(...)` before runtime configuration.

DOS player runtime video configuration is now grouped in a local `PlayerVideoConfig`, so runtime output helpers consume one video config object before passing cells, capacity, and mode to the runtime facade.

Player video-config state now sits behind macro field boundaries and runtime video configuration helpers, keeping call sites off direct config-field reads and writes.

DOS player runtime output setup now groups video config, video backend, and audio backend in a local `PlayerRuntimeOutput` aggregate before applying SB16 or fallback runtime configuration.

Player runtime-output aggregate access now sits behind macro field boundaries and SDL/notcurses bridge setup helpers, keeping call sites off direct output-field reads.

DOS player session control flow is grouped behind `player_run(...)`; `main` now performs process initialization and delegates argument/module/runtime sequencing to a named C entry helper.

Runtime output configuration now has one `iplay_runtime_config_output_capacity(...)` facade for video backend, SB16/SDL-compatible audio backend, and hardware-enabled selection; the public SB16, SDL, and no-hardware helpers delegate to that boundary.

Runtime output selection now uses `IplayRuntimeOutputSpec` plus SDL-compatible and SB16-hardware preset initializers, so callers pass one C output object instead of raw backend enums and hardware flags.

Runtime output configuration now reads `IplayRuntimeOutputSpec` through named accessors for video backend, audio backend, and hardware-enabled state, keeping spec layout out of runtime config wiring.

Runtime initialization now uses `iplay_runtime_config_uses_sb16_hardware(...)` and runtime-config audio accessors for SB16-vs-SDL audio selection instead of checking audio backend fields inline.

Runtime initialization now reads video cells, capacity, text mode, present callback, and audio callback state through `IplayRuntimeConfig` accessors before initializing the notcurses-style terminal and SDL/SB16 audio device.

Runtime config validation now also uses the same `IplayRuntimeConfig` accessors for cells, mode, capacity, present callback, and audio callback checks instead of direct field reads.

Runtime config present-callback validation now uses `iplay_runtime_config_video_present_enabled(...)`, keeping the video-present enable flag behind the config accessor boundary.

Runtime config video backend selection is now exposed through `iplay_runtime_config_video_backend(...)`, so future notcurses/SDL bridge code does not need direct `IplayRuntimeConfig` field reads.

Runtime terminal access now goes through `iplay_runtime_terminal(...)`, reducing direct `runtime->nc` terminal lookups in init, draw, and present paths.

Runtime audio facade calls now use `iplay_runtime_audio(...)` and `iplay_runtime_audio_const(...)`, and audio-level drawing obtains the SDL-compatible output through `iplay_sdl_audio_device_output(...)` instead of reaching into `runtime->audio.output`.

Runtime video spec and mode queries now use const notcurses/terminal runtime helpers plus `iplay_terminal_has_present(...)`, reducing direct read-only `runtime->nc.terminal` field access.

Runtime video capacity and bottom-layout fit queries now also use `iplay_runtime_terminal_const(...)`, avoiding direct `runtime->nc.terminal.screen` reads in those video facade helpers.

Runtime resize, set-video-mode, and render helpers now call notcurses through `iplay_runtime_notcurses(...)` instead of passing `&runtime->nc` directly.

Runtime status drawing now initializes root windows through `iplay_runtime_stdplane(...)`, and that helper resolves the plane through `iplay_runtime_notcurses(...)` instead of direct `runtime->nc` access.

Runtime video-mode success state now goes through `iplay_runtime_set_video_mode_ok(...)` and `iplay_runtime_video_mode_ok(...)`, avoiding scattered direct writes and checked-return reads of `runtime->video_mode_ok`.

Runtime initialization helpers now initialize notcurses and SDL-compatible audio through `iplay_runtime_notcurses(...)` and `iplay_runtime_audio(...)`, removing direct `&runtime->nc` and `&runtime->audio` wiring from those init paths.

SDL-compatible audio device opening now validates and copies `IplaySdlAudioDeviceConfig` through named config accessors, keeping SB16 stereo format and hardware/backend selection behind the SDL-like wrapper boundary.

SDL-compatible audio device backend, hardware-enabled, and paused state transitions now go through named setters, so open/start/stop/pause paths no longer write those device fields directly.

SDL-compatible audio device read-only spec, backend-name, and SB16-compatibility queries now use the device backend and hardware accessors instead of reading those fields directly.

SDL-compatible audio device output access now has a const accessor, and device methods route format, active/counter, start/stop, and SB16 frame writes through output accessors instead of direct `device->output` references.

SDL-compatible audio silence, levels, and reset-levels methods now also use the audio-device output accessors, completing that wrapper pass for those tail methods.

Audio output now exposes sink accessors, and the basic output facade methods route sink init, start/stop, counters, capacity, active, and format queries through those helpers.
- SDL-like SB16 audio config now exposes sample count, callback, and userdata through pure-C accessors, giving future SDL queue/open code a typed config boundary instead of depending on raw config fields.
- SDL-like SB16 audio config validation now materializes an `IplayAudioFormat` through a config facade helper and uses shared audio-format equality, removing duplicated field-by-field SB16 checks from the device predicate.
- Notcurses-style plane scrolling now uses `iplay_ncplane_cell_offset(...)` for text-cell addressing, centralizing VGA cell offset arithmetic before future terminal backends replace raw memory writes.
- Notcurses-style single-cell writes now use `iplay_ncplane_cell_offset(...)`, so normal character drawing and scrolling share the same centralized text-cell address calculation.
- Notcurses-style single-cell drawing now writes character/attribute pairs through `iplay_ncplane_put_cell_offset(...)`, separating plane cell addressing from cell mutation for future terminal backends.
- Notcurses-style scrolling now copies character/attribute cells through `iplay_ncplane_copy_cell_offset(...)`, keeping scroll movement on named plane operations instead of direct VGA byte-copy statements.
- Notcurses-style cell copying now reads source character/attribute pairs through `iplay_ncplane_cell_ch(...)` and `iplay_ncplane_cell_attr(...)`, keeping scroll copies behind named plane accessors.
- Notcurses-style plane initialization now derives origin cell pointers through `iplay_ncplane_cells_at(...)`, keeping subwindow/root setup from spelling VGA cell pointer arithmetic inline.
- SDL-like SB16 audio devices now expose accepted sample count, callback, and userdata through device-level accessors, keeping future host audio code off the embedded config struct.
- SDL-like audio device spec construction now uses the device-level format accessor, keeping accepted device reporting on the SDL wrapper boundary instead of reaching through the output sink.
- SDL-like SB16 compatibility checks now use the device-level format accessor plus backend query, avoiding direct reach-through to the nested audio-output compatibility predicate.
- Terminal cell access now goes through `iplay_text_screen_cells(...)`, keeping notcurses-style terminal code from reaching directly into the embedded text-screen cell pointer.
- Terminal presentation now uses const text-screen/terminal cell accessors, keeping read-only notcurses flush paths separate from mutable drawing access.
- Terminal presentation now obtains the active screen byte count through `iplay_text_screen_bytes(...)`, keeping flush sizing on the text-screen facade instead of recalculating from terminal mode locally.
- Text-screen resize and bottom-layout predicates now use text-screen accessor helpers for capacity and mode, reducing direct field coupling inside the notcurses-style screen facade.
- Text-screen initialization and resize now update cells, capacity, and mode through text-screen setter helpers, keeping setup mutation behind the notcurses-style screen facade.
- Text-screen root-plane setup now goes through `iplay_text_screen_reinit_root(...)`, so init and resize share the same notcurses-style screen reinitialization boundary.
- Text-screen drawing helpers now use `iplay_text_screen_root(...)` for title, bottom/status, and audio-level rendering, keeping draw paths behind the notcurses-style screen root accessor.
- Terminal VGA-memory initialization now reaches the embedded text screen through `iplay_terminal_screen(...)`, keeping terminal setup on the notcurses-style facade boundary.
- Terminal present-callback setup now routes callback and user mutation through `iplay_terminal_set_present_fn(...)` and `iplay_terminal_set_present_user(...)`, keeping present state mutation behind named terminal facade setters.
- SB16 audio output frame sizing now uses `iplay_audio_output_bytes_for_frames(...)`, keeping byte-count calculation on the audio-output facade instead of hardcoding the global SB16 format in the write path.
- Mixer audio output accepted-frame sizing now uses `iplay_audio_output_frames_for_bytes(...)`, keeping byte-to-frame conversion on the audio-output facade.
- Mixer and SB16 audio output writes now share `iplay_audio_output_accepted_frames(...)`, centralizing inactive-output and capacity clamping on the audio-output facade.
- Mixer and SB16 audio output writes share `iplay_audio_output_accepted_frames(...)` for level/capacity acceptance while preserving requested byte counts for sink write/drop accounting.
- Runtime const terminal access now goes through `iplay_notcurses_terminal_const(...)`, removing direct reach-through into the embedded notcurses terminal from runtime query paths.
- Notcurses terminal operations now route through `iplay_notcurses_terminal(...)` / const accessor instead of passing `&nc->terminal` through init, resize, render, and mode/stdplane helpers.
- Notcurses plane cursor, stride, empty-state, clipping, and single-cell put paths now use named `iplay_ncplane_*` helpers instead of open-coding those field reads at call sites.
- Notcurses plane erase and status-line drawing now size through `iplay_ncplane_rows(...)` / `iplay_ncplane_cols(...)` instead of reading plane dimensions directly at those call sites.
- SB16 audio sink capacity add/consume paths now go through `iplay_audio_sink_capacity(...)` and `iplay_audio_sink_set_capacity(...)`, keeping capacity mutation behind the SDL-like sink facade.
- Status-field drawing now obtains plane width through `iplay_ncplane_cols(...)`, removing direct dimension reads from that notcurses-style window helper.
- Bottom-layout fit checks now use `iplay_text_mode_rows(...)` and `iplay_text_mode_cols(...)`, keeping multi-text-mode sizing behind the text-mode facade.
- Text-mode lookup by size now compares through `iplay_text_mode_cols(...)` and `iplay_text_mode_rows(...)`, keeping supported-mode matching on the text-mode facade.
- Text-mode row-byte, cell-count, and equality helpers now compose through `iplay_text_mode_cols(...)` / `iplay_text_mode_rows(...)` instead of duplicating raw field access.
- Notcurses plane initialization now composes through named plane setup helpers for cells, size, stride, origin, and cursor reset instead of open-coding the init field writes.
- Notcurses plane resize now updates size through `iplay_ncplane_set_size(...)` and clamps cursor state through cursor helpers instead of open-coding resize field mutation.
- Top-title drawing now obtains the plane backing cells through `iplay_ncplane_cells(...)` instead of reaching directly into `plane->cells` at that draw call site.
- Notcurses subplane setup now derives parent size, cells, stride, and origin through plane helpers, then sets child origin through `iplay_ncplane_set_origin_yx(...)` instead of direct parent/child field access.
- Notcurses plane initialization from a text mode now uses `iplay_text_mode_rows(...)` and `iplay_text_mode_cols(...)` instead of reading text-mode fields directly.
- Message and recolor text offset calculations now use text-mode row-byte/column helpers instead of reading `mode->cols` at those call sites.
- Video meter positioning now obtains active text columns through `iplay_text_mode_cols(...)` instead of reading the text-mode struct directly.
- SB16 audio sink counter clear/add paths now route through explicit counter setters and counter accessors instead of open-coding counter mutation.
- SB16 audio output initialization now delegates to the generic audio-output initializer with the SB16 stereo format and no scratch buffer, keeping output setup in one SDL-like path.
- Audio format equality, naming, rate checks, SB16 predicates, and conversion now use read-only audio-format accessors instead of reaching through format fields at those call sites.
- Audio byte-per-frame calculation and SDL-compatible config format copying now use audio-format accessors instead of reading format fields directly.
- Runtime configuration construction now routes video and audio setup through `iplay_runtime_config_set_video(...)` and `iplay_runtime_config_set_audio(...)`, keeping output-spec mapping behind config setters.
- Audio source-format construction now uses `iplay_audio_format_set(...)`, centralizing format field mutation behind the audio-format facade.
- Audio output source-format and scratch setup now route through dedicated output helpers, leaving only helper-owned field writes.
- SDL-like audio device config copying now goes field-by-field through config helpers instead of copying the embedded config struct directly.
- Runtime video-mode status now has explicit flag accessor helpers, keeping mode-switch code from writing the runtime field directly.
- Notcurses-style window operations now route through window plane accessors instead of passing `&window->plane` at each operation.
- Runtime config construction now uses smaller semantic setters for video memory, presentation, video backend, audio sink, and audio backend setup.
- Module status initialization now composes named setters for title, path, size, loader symbol, and type instead of bulk field assignment.
- SDL-like audio device backend, hardware-enabled, and paused state now route through raw state helpers, leaving public setters to normalize and delegate.
- SB16 prepared-block setup now uses named setters for aligned byte count and sample count instead of writing the block fields inline.
- SB16 DMA bookkeeping now uses named setters for programmed state and last block size, including idle transition cleanup.
- SB16 playback active state and write/start counters now route through named state/counter helpers instead of direct assignments in lifecycle helpers.
- SB16 detection state now routes through detected-flag helpers instead of direct normalized field read/write in reset logic.
- DOS text presenter invocation now goes through callback accessors, keeping notcurses-style presentation calls off direct presenter fields.
- DOS text present-frame setters/getters now delegate through raw frame field helpers, isolating frame layout access from presentation flow.
- Player audio/video backend setup now delegates through raw backend accessors, isolating local SDL/notcurses bridge layout access.
- Player video-config setup now delegates through raw config field helpers, keeping the DOS text runtime bridge off bulk struct assignment.
- Player runtime-output embedded video/audio objects now route through macro field boundaries before semantic output accessors expose them.
- Player module-info setup and access now route through raw field helpers before semantic module APIs expose path, header, loader, size, and type.
- DOS hardware I/O function-pointer calls now route through callback accessors before port, far-memory, and text-memory operations invoke them.
- Player startup now separates reusable `player_init_core_state(...)` memory/register reset from `player_init_dos_process(...)` hardware and text-presenter setup.
- Player runtime output setup now names the text/video half explicitly with `player_runtime_output_init_text_video(...)`, leaving SB16/SDL-compatible audio backend selection as a separate step.
- Runtime output now applies SB16 and SDL-compatible config through `PlayerRuntimeOutput` mapping helpers, keeping backend selection separate from runtime-config field plumbing.
- `PlayerRuntimeOutput` now has SB16 and SDL-compatible initializers, so configure functions build the requested output object before applying it to runtime config.
- Runtime output apply helpers now consume output-level video/audio query helpers instead of unpacking embedded video and audio backend structs locally.
- DOS `main(...)` now delegates to `player_run_dos_cli_process(...)`, keeping process initialization and CLI dispatch behind a named adapter.
- SB16 DMA writes now access the DMA storage through `sb16_dma_buffer_memory(...)`, keeping the audio write path off the global buffer symbol.
- SB16 audio copy sizing now queries `sb16_dma_buffer_capacity(...)` instead of reading the DMA buffer size macro in the write path.
- SB16 audio copy sizing now aligns through `sb16_dma_align_16bit_stereo_bytes(...)`, making the 16-bit stereo frame requirement explicit.
- SB16 audio callback submission now funnels copy plus DMA start through `sb16_audio_submit_dma_block(...)`, leaving the write callback with one submit operation.
- SB16 audio callbacks now delegate copy sizing and empty-block filtering to `sb16_audio_submit_pcm(...)`, keeping callback glue separate from DMA packet preparation.
- SB16 block start now separates block preparation from prepared-block execution through `sb16_start_prepared_16bit_stereo_block(...)`.
- Prepared SB16 block execution now starts rate/DSP output through `sb16_start_prepared_16bit_stereo_dsp(...)`, localizing the sample-count query.
- SB16 DSP start now uses named speaker, 16-bit stereo mode, and sample-count command helpers instead of open-coded command writes.
- SB16 sample-rate setup now delegates the DSP output-rate command sequence through `sb16_dsp_set_output_rate(...)`.
- SB16 reset now uses named assert, settle, and release helpers around the DSP reset port sequence.
- SB16 reset ACK reading now goes through `sb16_dsp_read_reset_detected(...)`, separating reset sequencing from detection interpretation.
- SB16 reset pulse sequencing is grouped behind `sb16_dsp_pulse_reset(...)`, keeping `sb16_reset(...)` focused on pulse, ACK read, and state update.
- DOS text presentation now clamps copy byte counts through `dos_text_clamp_present_byte_count(...)`, keeping present-frame sizing behind a named helper.
- DOS text presenter default storage now routes through `dos_text_default_presenter_state(...)`, keeping init/default-user paths off direct global presenter access.
- DOS text present preparation now routes frame initialization through `dos_text_prepare_present_frame_init(...)`, separating presenter/user resolution from frame field assembly.
- DOS text present-frame execution now delegates backend copying through `dos_text_present_frame_copy_to_video(...)`, leaving frame presentation as a named operation.
- Text video backend initialization now queries `player_text_video_present_fn(...)` and `player_text_video_present_user(...)`, separating backend wiring from the DOS text presenter symbols.
- Text video config initialization now uses `player_text_video_cells(...)`, `player_text_video_capacity(...)`, and `player_text_video_mode(...)`, naming the text-mode surface separately from generic runtime output plumbing.
- Player runtime text startup now uses a settable video-mode id and a max-sized text backbuffer; `IPHWRUN.EXE playerruntimehw80x50` proves the player runtime can render and present an 80x50 frame through the DOS `B800:0000` hardware wrapper, and `IPHWRUN.EXE playerruntimehw80x50levels` proves SB16 level updates can be drawn on that 80x50 DOS hardware-presented frame.
- Runtime text video output initialization now separates surface config and presenter backend setup through `player_runtime_output_init_text_config(...)` and `player_runtime_output_init_text_backend(...)`.
- Runtime config application now snapshots video and audio callbacks through `PlayerRuntimeVideoOutput` and `PlayerRuntimeAudioOutput` views before calling the SB16/SDL-compatible runtime config entrypoints.
- Runtime video/audio output views now use raw setter/getter helpers, keeping direct field writes contained behind the same layered accessor style as the rest of the rewrite.
- Runtime config application now routes the final SB16/SDL-compatible config calls through `player_apply_runtime_sb16_output_views(...)` and `player_apply_runtime_sdl_output_views(...)`.
- Runtime config application now builds `PlayerRuntimeOutputViews` once per output, sharing the video/audio view preparation path across SB16 and SDL-compatible apply code.
- Runtime output view bundles now expose const video/audio accessors for final SB16/SDL-compatible config application.
- Runtime config application now hands whole output view bundles to `player_apply_runtime_sb16_output_view_bundle(...)` and `player_apply_runtime_sdl_output_view_bundle(...)`.
- Runtime output initialization now separates SB16 and SDL-compatible audio backend setup through `player_runtime_output_init_sb16_audio(...)` and `player_runtime_output_init_sdl_audio(...)`.
- Runtime output initialization now shares text-video-plus-audio composition through `player_runtime_output_init_with_audio(...)`, leaving SB16/SDL-compatible initializers to select only the audio backend initializer.
- Runtime output configuration now shares the output init/apply sequence through `player_configure_runtime_output_with(...)`, leaving SB16/SDL-compatible configure functions to select only their init and apply callbacks.
