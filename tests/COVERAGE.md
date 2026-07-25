# IPLAY rewrite test coverage

The parity harness in `test_function_parity.py` is intended to run the same cases against:

- the original DOS `IPLAY.EXE` through `kvikdos`;
- a translated or rewritten C/C++ runner supplied with `IPLAY_TRANSLATED_RUNNER`.

Current direct-call coverage is limited to routines that behave as stable near functions when called from a wrapper:

- hexadecimal formatting: `u16tox`, `u8tox`, `u4tox`;
- decimal formatting: `my_u8toa_10`, `my_u16toa_10`, `my_u32toa10_0`, `my_i8toa10_0`, `my_i16toa10_0`, `my_i32toa10_0`;
- byte/string helpers: `my_putdigit`, `mystrlen_0`, `strcpy_count_0`, `copy_printable`.
- interpolation patching: the `loc_157F2` / `lc_perfrm_interpol` entry is entered with a synthetic saved-channel stack frame and `buffer_size2 = 0` to verify all currently known self-modified interpolation bytes without mixing sample data.

The 24-bit interpolation/mixer path must not be tested by jumping into one unrolled chunk directly.
`IPLAY.lst` shows the original code self-modifies bytes in `CS` before dispatching through `offs_interpol`; for example `sub_15577` writes `AL` into `cs:byte_158B4`, `cs:byte_158E3`, and many later embedded bytes used by the interpolation chunks.

The active mixer is now covered through `sub_15577`, so the original setup entry performs its own self-modification before observable state is compared with the clean C mixer:

- deterministic source sample bytes and interpolation fractions;
- narrow 16-bit and wide 32-bit volume-table accumulation;
- interpolated mixed output buffer bytes;
- updated channel position/flags, non-looping voice stop, and loop wrapping.

This is branch-complete deterministic parity for the currently rewritten `sub_15577` behavior, not exhaustive fuzzing over every period, volume ramp, overflow, and loop boundary.

Do not use standalone direct-call tests for labels inside the patched unrolled chunks unless the wrapper also initializes the same self-modified bytes first.

## First milestone: DOS-working behavior coverage

The immediate goal is not more C cleanup. The first milestone is:

- `rewrite/.build/IPLAYC.EXE` runs under DOS through `kvikdos`.
- Current smoke and parity gates pass.
- Behavior tests compare original observable behavior against the rewritten DOS player where practical.
- Missing original behavior coverage is tracked as inventory, not guessed as a percentage.

Current measured original-proc inventory:

- total `proc` symbols parsed from `IPLAY.lst`: `383`;
- `proc` symbols present in the tested inventory: `341` (`89.03%` of all original procs);
- untested `proc` symbols: `42`;
- all `42` untested procs are currently in the explicitly excluded non-SB legacy audio-driver scope;
- required original-proc inventory excluding non-SB legacy drivers: `341 / 341`.
- `./rewrite/check_rewrite.sh` now runs this inventory with `IPLAY_REQUIRE_FULL_UNIT_COVERAGE=1`, so SoundBlaster, VGA/text, and non-legacy required-proc inventory checks are enforced instead of skipped.
- `./rewrite/check_rewrite.sh` also rejects pytest summaries containing skipped, xfailed, or xpassed tests, so the reported green gate means the active test stages actually passed.

This is function/proc inventory accounting only. It does not prove full whole-program/UI/audio behavior coverage.

Use this gate before any further rewrite batch:

```sh
./rewrite/check_rewrite.sh
```

The existing gate currently proves:

- the rewrite builds DOS MZ binaries;
- `IPLAYC.EXE` does not link generated fallback translated sources;
- the DOS smoke player runs under `kvikdos`;
- direct ABI/function parity tests pass for the cases listed in `test_function_parity.py`;
- inventory tests enforce important wrapper and clean-C structure invariants.

This does not prove full old-player behavior coverage.

## Original-vs-current behavior test matrix

Add tests in this order.

| Area | Original evidence | Current evidence | Test type | Status |
| --- | --- | --- | --- | --- |
| DOS startup/no module | `IPLAY.EXE` under `kvikdos` | `IPLAYC.EXE` under `kvikdos` | stdout/exit/status snapshot | current-only covered; original whole-program harness blocked |
| CLI/module argument path | `IPLAY.EXE` with fixture module names | `IPLAYC.EXE` with same fixtures | stdout/exit/status snapshot | current-only covered; DOS `@FileList.Ext` coverage loads the first non-empty entry and trims surrounding spaces/tabs before module load, proving later entries are not accidentally selected |
| Module loader detection | `IPLAY.EXE` + fixture headers | `IPLAYC.EXE` + same fixture headers | loader line/type tag/title comparison | current-only covered for fixture headers and header-over-extension detection; original patched-harness existing corrupt MOD reaches UI/module path with no console error; rewrite rejects too-short MOD/NST before playback |
| Unsupported/missing module | original failure paths | rewritten failure paths | exit/status/error text comparison | original patched-harness missing `.MOD` behavior is directly compared with the rewrite for `Module not found.`/exit `2`; rewrite missing-file coverage also proves all supported text modes exit before screen-present, PCM source, decoder route/handoff, or playback pump evidence; unsupported type current-only covered |
| Text video mode setup | original video mode writes/state | rewritten text wrapper events/state | wrapper/event or memory snapshot comparison | partial ABI/current tests; DOS presenter now has mocked hardware event coverage for color/BW segment routing and mode-size clamping |
| Text screen rendering | original B800 text memory | rewritten text cells/present event | cell/attribute snapshot comparison | exact 4000-byte original-vs-rewrite parity exists for the corrupt-MOD cleanup/error frame; valid `SMOKE.MOD` captures exist both at completed `text_init2` and immediately after the first `offs_draw` callback, bypassing unsupported hardware status calls; the rewrite matches original coordinates and attributes for the title, filename, `462KB`, `M.K.`, 4-channel, 2/31-sample, module-title, two-column controls, `1/5` track count, `1/64` position, four channel prefixes, and all 30 meter glyphs per channel; the driver header is intentionally adapted to SB16-only scope; the duplicate diagnostic panel is removed from 80-column frames; the preferred SDL player propagates libmodplug order/pattern/row and stateful per-channel note, sample-name, and effect state into the original 80-column rows, including MOD octave normalization and named effects; loaded-module coverage spans 40x25 BW/color, 80x25 BW/color, and 80x50; bounded multi-frame host tests prove continuous full-layout redraw and live 80x25-to-80x50 resize behavior; original `f3_draw` clamp, peak hold, decay cadence, and exact 0..30-cell scaling are reproduced and listing-guarded; libmikmod-backed formats use real mixer-voice measurements while exact raw magnitude equality remains incomplete because libmikmod exposes a 64-point peak-to-peak value instead of the original private 80-byte signed mixer-buffer average |
| Multiple text modes | original mode changes | rewritten `IplayTextMode`/terminal resize behavior | mode/capacity/layout tests | partial current-only tests; resize, capacity, BW/color, and 80x50 paths covered; DOS presenter now covers 40x25, 80x25, and 80x50 byte-count/aperture behavior; player runtime startup now has an `IPHWRUN.EXE` 80x50 DOS hardware-present proof |
| UI/subwindows | original text memory snapshots | rewritten screen snapshots | deterministic frame snapshot tests | current-only subwindow/redraw/edge-clipping snapshots covered; original corrupt-module cleanup frame now has B800 geometry/nonblank comparison against the rewrite unsupported-module frame; F4 renders the full decoder sample-name inventory and repeated F4 uses the original 11-sample page advance with multi-page runtime coverage; exact original text-memory comparison missing |
| Fast audio levels display | original level rendering | rewritten level rendering | frame/cell snapshot over sample inputs | original first-draw capture now compares four channel-row prefixes and all 30 original meter glyph positions; current runtime/SB16 snapshots prove SDL stereo peaks change meter attributes, and the 80x50 DOS hardware-present path plus `playerplaybacklevelshw` retain audio-copy/B800 digest evidence; S3M and other libmikmod-backed formats map `Voice_RealVolume` plus current mixer volume through the original integer channel divisor and envelope into the original 30-cell rows; deterministic scale tests require zero silence, increasing partial/full values, clamp at 60, and nonzero real ARYX channel activity; MOD/NST retains lockstep libopenmpt velocity telemetry and the tested no-libopenmpt fallback |
| SB16 detect/reset | original port I/O sequence | rewritten hardware wrapper events | port-event sequence comparison | partial ABI tests; `callsubx` SB16 no-device failure preserves requested settings and message pointer under original-vs-rewrite function parity; DOS `IPLAYHW.EXE` unavailable-hardware coverage proves module load, external-library handoff, SB16 16-bit stereo hardware wrapper selection, `base=220h irq=5 dma16=5 rate=44100` config reporting, no PCM source, no playback pump, and zero presented audio frames across supported text modes |
| SB16 16-bit stereo start | original DSP/DMA programming | rewritten SB16 wrapper events | port/DMA command sequence comparison | partial current-only tests; hardware status text is characterized across lifecycle states; player SB16 hardware wrapper now has mocked DOS port/DMA event coverage |
| SB16 playback write path | original DMA buffer/content behavior | rewritten wrapper/DMA buffer behavior | buffer/event snapshot comparison | partial current-only tests; loaded-module player path now proves SB16 wrapper is reached from player playback and parses deterministic PCM copy count, byte count, checksum, first sample word, and tail sample word at the hardware copy seam; continuous module-backed keyboard-stop coverage now proves nonempty SB16 wrapper audio is copied before the timer/keyboard path exits; real DOS behavior tests parse `Playback pump: ...` as structured fields for bounded SB16 playback, continuous source-end playback, and checksum-difference comparisons |
| Continuous real module playback | original timed player loop and decoded PCM | rewritten DOS player with real decoder/library PCM | audible/PCM/event-loop comparison | incomplete; current default DOS path selects through `IPLAY_PLAYER_DEFAULT_LOOP_POLICY` and is explicitly tested as bounded `Playback loop: mode=playback policy=bounded-trial cadence=immediate max_blocks=32 frames/block=512`; `IPLAYCONT.EXE` is separately built with the continuous policy, reports `cadence=timer`, uses nonzero `IPLAY_PLAYER_CONTINUOUS_TIMER_INTERVAL_TICKS`, creates a stateful `PlayerPlaybackTimer` that reads BIOS ticks through `DosHardwareIo`, primes the first block, routes keyboard exit polling through `DosHardwareIo`, and is tested to exit via natural `stop=source-end` while presenting both `playback-position` and `post-playback-status` text frames after audio; `try_player.sh --continuous-diagnostics --video-mode=80x50` now requires valid 80x50 playback and final status screen geometry before classifying `source-ended-ui-ok`; split-runner coverage now proves both dummy PCM and module-backed `PlayerDecoderContext` sources submit a 512-frame SB16 block and exit through the keyboard seam, but full original-vs-rewrite BIOS/PIT-paced continuous playback with real decoded PCM/library output is not complete |
| Executable interface shape | original `IPLAY [Switches] [FileName.Ext|@FileList.Ext]` usage | rewritten DOS executable usage adapter | stdout/exit snapshot | partial; usage banner restored to original-shaped interface while clean C request/path entry points remain internal; patched-original and rewrite help output now directly compare the stable usage and `/i` lines while allowing rewrite-specific help/capability lines |
| Interactive playback controls | original keyboard controls | preferred SDL terminal input | input sequence, PCM checksum, screen-state, and playback-state tests | covered for stop, pause, audio controls, original arrow navigation, channel mute/pan, and original F1-F6/F8 behaviors; `q`, `Q`, and lone Escape stop playback, P/Space pauses without decoder reads or SDL submission and resumes at the same source position, while CSI/SS3 sequences are consumed atomically; a bridge-owned control object applies `-`/`+` master volume, `[`/`]` amplification clamped to the original 50..2500 percent range, left/right two-row and down/up ten-row seeks, numeric channel mute for channels 1..10, `,`/`.` selected-channel navigation, one-step `k`/`m` and eight-step `K`/`M` pan, F11 restart/looping, F12 nearest/linear interpolation, F9's original extra MOD vibrato-depth shift, and F10's original routing of MOD `Fxx > 20h` from tempo to speed; libmikmod supplies real per-channel mute/pan and signed 16-bit stereo PCM for non-MOD external formats, while MOD/NST retains libmodplug PCM for tested compatibility-command patching; amplification is applied as deterministic saturated signed-16-bit PCM gain rather than relying on decoder-specific runtime volume behavior; seeking clamps at row zero and synchronizes PCM, libmodplug command state, and dynamically loaded libopenmpt telemetry before the next submitted block; independent terminal view state renders original-shaped F1 help, a live F2 80-sample stereo-PCM scope, F3 realtime VU, an F4 sample-name/active-channel view, an F5 32-band spectrum generated by a 256-point radix-2 FFT, and the original F6 per-channel L/M/R panning display from persistent `0..128` state; F8 presents the original shell heading, invokes an interactive host shell or explicit test command, then redraws and resumes playback; tests prove pause rendering/resume/control-generation isolation, all audio-affecting toggles, row navigation, channel mute, and channel pan change PCM/source-end behavior, loop mode preserves the full submitted-block limit, and the original panel's final overlay shows the applied volume and amplification values without channel-row overwrite; all views are emitted during bounded playback, F2/F5 consume nonzero PCM visualization data, F6 ratios/default retention and visible pan are valid, and F8 resumes to keyboard stop |
| Non-SB drivers | original may contain legacy paths | rewritten player path | inventory absence check | covered as excluded scope for player |
| Module playback core | original ABI/function behavior | rewritten ABI/function behavior | function parity/state comparison plus external decoder probes | tracker-format effect decoding is intentionally on the external-library boundary for MOD/NST, S3M, STM, 669, MTM, PSM, FAR, ULT, WOW, OKT, OCT, XM, IT, PTM, AMS, DBM, DMF, MDL, DSM, MED, IMF, J2B, and related reliable-library formats; DOS behavior tests cover loader metadata, external-handoff diagnostics, stream-start/body sensitivity, representative generic external-module routing with the shared `EXT ` tag plus XM/IT title metadata, source-end/block-limit stop state, SB16 wrapper plumbing, and runtime UI/status seams, while host libmodplug probes cover real 16-bit stereo PCM generation through the SDL-compatible/SB16 bridge plus route/provider playback evidence and installed-but-unavailable fallback behavior; original whole-program comparison remains missing |
| User trial diagnostics | original interactive playback | rewritten `try_player.sh` / `RES.TXT` | structured trial-result markers | current-only covered; `RES.TXT` reports module-loaded, loaded module name, requested-module match, host module size, DOS module size, host/DOS size match, loader line, module type tag, optional module title, loader-metadata OK predicate, decoder route id/name, decoder handoff, PCM provider/route/input/stream-start, playback-pump, valid nonzero playback-pump evidence, final playback-pump line, PCM provider/renderer/route/input/truncation/hook-provider/stream-start fields, screen-present, playback-position present, valid playback-position digest, post-playback-status present, valid post-playback-status digest, screen-present reasons, audio-unavailable, classified trial result, timeout, selected EXE, audio mode, and exit status markers so user trials can distinguish successful requested-module bounded UI playback with explicit size/loader/type-tag/title, decoder route/handoff, and PCM route evidence, nonzero audio, and nonempty post-audio `playback-position` and final status frames, requested-module source-end UI playback, requested-module mismatch/missing-load evidence, module size mismatches, loader metadata failures, missing decoder route, missing decoder handoff, missing PCM source evidence, invalid playback-pump evidence, invalid playback-position screen evidence, invalid post-playback status screen evidence, audio-without-screen regressions, native-preview versus DOS fallback PCM, installed external decoder hooks versus no hook, missing module/playback/UI, wrong-module loads, timeout, and real-SB16 unavailable exits; OK classifications require non-`none` loader metadata, reject `00000000` module tags, and require parsed PCM hook-provider and stream-start evidence |
| Preferred SDL/notcurses launcher | original interactive playback | rewritten `./iplay.sh` diagnostics and `--check-playback` | top-level command stdout/status evidence | current-only covered; `./iplay.sh --diagnostics --video-mode=80x50` with a real S3M proves the preferred user command reaches libmikmod route/provider evidence, SDL-compatible SB16 16-bit stereo output, opened SDL dummy audio, selected 80x50 text geometry, terminal render evidence, stdin keyboard status, and nonzero/changing live audio level summary through the top-level launcher rather than only through `rewrite/.build/iplay`; original numeric modifier banks address channels 1-30 through real libmikmod mute controls, with channel 11 producing a tested PCM change and the third bank retaining exact masks on smaller modules; original `L/M/R/S` pan presets are listing-pinned and tested to produce distinct real libmikmod PCM, including surround; ScrollLock pattern loop is tested across an order boundary without leaking the next order to SDL, End advances the real decoder to the next order, and Tab switches real mixer stepping plus the original `(PAL)`/`(NTSC)` display while SDL remains fixed at SB16-compatible 44.1 kHz; `./iplay.sh --check-playback samples/aryx.s3m` is the quick proof command for a controlled 40x25 SDL/notcurses/libmikmod readiness run, including `@file-list` input and clean failures for missing modules, corrupt known tracker data, unsupported probe files, deferred project-owned `.inr`, implementation-launcher real-module/`@file-list` success plus missing/corrupt/unknown/`.inr`/SDL-failure quick-check coverage, and SDL audio open failure |
| Module data availability | original loaded module memory | rewritten module buffer | sample-data offset behavior | current-only covered for MOD sample data starting past the old 8 KiB header window; external-library tracker files just beyond the current 24,576-byte DOS buffer now load through a capped-header path for the future library decoder boundary while preserving true file size, with MOD/NST/S3M/MTM/STM/FAR/669/PSM/ULT behavior tests plus MOD/NST/S3M/STM/MTM/PSM/FAR/669/ULT same-path stream coverage; generic external-library extensions WOW/OKT/OCT/XM/IT/PTM/AMS/DBM/DMF/MDL/DSM/MED/IMF/J2B are source-guarded through the DOS `external_module` loader table and covered by representative `.XM` DOS diagnostics; fallback stream-start behavior is same-path covered for S3M first-pattern data, MOD pattern data, MTM post-metadata data, STM post-header data, and FAR/669/PSM/ULT post-metadata data so pre-stream byte changes do not perturb PCM while stream-body changes do; DOS diagnostics also report `stream_start=...` for complete and capped-header file-path modules, capped/file-path playback now uses the loader-derived stream boundary when it is inside the real module size instead of the arbitrary 24,576-byte buffer edge, and capped S3M file-path playback is covered across the 496-byte DOS file-stream refill boundary; full original/library-equivalent large-module decoding still needs the modern external decoder integration path |

Original `B800:0000` corrupt-module UI/module screen comparison has exact 4000-byte equality. Stable valid-MOD capture points exist at original `text_init2` completion and after the first `offs_draw`; they prove real metadata, module title, controls, dynamic track geometry, channel prefixes, and meter-glyph placement. Libmodplug now supplies pattern-command note, sample-name, and effect state to the preferred SDL player, and a deterministic sparse MOD verifies that active channel state persists after playback advances beyond the command row. Libmikmod-backed formats now supply real per-voice mixer measurements to the original meter geometry, while MOD/NST retains dynamically loaded libopenmpt telemetry and a tested stereo-peak fallback. Exact whole-frame equality still needs active-voice timing reconciliation and calibration of libmikmod's peak-to-peak measurement against the original private 80-byte signed mixer buffer.

fallback stream-start behavior is same-path covered for S3M first-pattern data and MOD pattern data.
| 24-bit interpolation | original self-modified setup entry | clean pointer-based C mixer | setup-entry output/state parity, not raw chunk calls | active deterministic parity covered: original `loc_157F2` patching is compared for all known self-modified byte sites; `sub_154F4` compares interpolation bytes plus `BX/CX/SI`; active `sub_15577` compares strided mixed-buffer bytes and channel state for narrow accumulation, wide accumulation, interpolation, non-looping voice stop, and loop wrapping; exhaustive period/volume/overflow fuzzing remains future hardening |
| Shutdown/cleanup | original exit/deinit behavior | rewritten exit/deinit behavior | event/status comparison | host SDL path is covered against the original ordering contract: listing tests prove `snd_offx` precedes `snd_deinit`, while the native sink pauses immediately, clears queued PCM, closes the device, and then quits SDL; block-limit and keyboard-exit tests require `paused=1 queue_cleared=1 closed=1`, eliminating stale-audio drain delays; loaded-module hardware path now asserts final SB16 speaker-off DSP write and idle SB16 state after 16 submitted hardware blocks across 40x25 color/BW, 80x25 BW, and 80x50 text modes; exact original hardware event comparison remains missing |

## Test harness work items

1. Add an original/current DOS player comparison helper.

   It should run:

   ```sh
   /home/xor/kvikdos/kvikdos original/IPLAY.EXE <fixture>
   /home/xor/kvikdos/kvikdos rewrite/.build/IPLAYC.EXE <fixture>
   ```

   Normalize unstable output such as paths, timing, and memory addresses before comparing.

Current blocker: raw whole-program execution of `original/IPLAY.EXE` under plain `kvikdos` fails before program start with an EXE stack/memory layout error. `tests/test_player_behavior.py` now parses the original MZ header and proves the exact condition: `SS:SP = 2451:1000` places the stack beyond the plain minalloc program image (`stack_end > min_program_end`). The same file also asserts the deterministic plain-kvikdos fatal message as a passing blocker test, not an expected failure. A temporary test copy with `min_extra_paragraphs` raised to cover the declared stack reaches original program logic, reports `Config file not found. Run ISETUP first` without config, prints the original `/?` help when a valid 16-byte `IPLAY.CFG` fixture is present, prints original `/i` sound settings for an AdLib config, reports `Module not found.` with exit code `2` for a missing module under that valid config, and reaches a UI/module path with exit `0` and no console error for an existing corrupt `.MOD`. The rewrite does not treat that corrupt `.MOD` as playable, but it now presents an `unsupported-module` DOS text frame before exit, so `test_original_iplay_forced_hlt_dump_captures_b800_text_aperture` compares original B800 cleanup-frame geometry/nonblank/shared visible title, module identity, `Filename      : BAD.MOD`, and `Module Type   : N.T.` text with rewrite normal-exit B800 text-present evidence for the same corrupt `.MOD` case, including exact 4000-byte original-vs-rewrite cell/attribute equality. SB16-style `/i` configs currently hit kvikdos `offset overflow in print` after the original `callsubx` path; the underlying SB16 no-device `callsubx` state is covered by original-vs-rewrite function parity, but whole-program SB16 `/i` still needs a harness workaround. Adding kvikdos `--hlt-dump` to the unmodified patched-original UI path produces no dump file because the program exits normally instead of through the call-HLT harness. Forcing a test-only `HLT` at the original cleanup exit now produces a dump that includes `B800:0000`, and normal-exit `KVIKDOS_MEM_DUMP` captures the rewrite B800 frame; live playable-module UI frame equality still needs stable original/rewrite capture point pairs, but the emulator-level VGA dump blocker is removed.

2. Split current-only smoke from original-parity smoke.

   Keep `tests/test_player_smoke.py` as the runnable DOS sanity check.

   Add a new original-parity smoke test for behavior lines that both binaries can expose deterministically.

3. Add fixture modules once and reuse them.

   `tests/player_behavior_fixtures.py` is now shared by the DOS smoke script and Python behavior tests, so the user-trial binary path and behavior matrix use the same tracker headers. The rewrite gate rejects reintroducing inline smoke-only tracker fixture construction. The shared fixture set includes `SMOKE.NST`, so the advertised NoiseTracker/MOD loader path is covered in the same DOS paths as the other supported tracker formats. Behavior tests parse the current DOS usage line and assert that all advertised formats have shared fixtures and metadata behavior cases.

4. Add wrapper-event comparison where stdout is too weak.

   For SB16 and text presentation, compare event logs or memory/cell snapshots instead of relying only on printed text.

5. Do not add tests for excluded legacy audio drivers.

   Player audio scope is SB16 16-bit stereo only. Non-SB legacy routines may remain only as ABI compatibility stubs if needed by old code coverage.

6. Do not hand-rewrite tracker/module decoders that reliable libraries already cover.

   Keep `MOD`/`NST`, `S3M`, `STM`, `669`, `MTM`, `PSM`, `FAR`, `ULT`, `WOW`, `OKT`/`OCT`, `XM`, `IT`, `PTM`, `AMS`, `DBM`, `DMF`, `MDL`, `DSM`, `MED`, `IMF`, and `J2B` on an external decoder boundary such as libopenmpt/libxmp/libmodplug. Keep only project-specific glue such as UI state, runtime state, and SB16/text wrappers in the immediate C rewrite scope unless a format has no reliable library path. `.inr` playback is deferred for the current SDL/notcurses goal; current tests keep it classified as project-owned/unavailable instead of treating it as a missing libmodplug-backed format. Current DOS behavior tests assert tracker fixtures report the external tracker handoff while INR reports the project decoder handoff, and the modern SDL/libmodplug facade classifies the broader tracker list as external-library candidates.

## Rewrite rule after this milestone

Before rewriting a behavior area, there must be at least one test in the matrix proving that area against either:

- original `IPLAY.EXE`, or
- a documented original ABI/function parity fixture where whole-program comparison is not practical.

After each rewrite batch, run:

```sh
./rewrite/check_rewrite.sh
```

Do not report total completion percent unless it is computed from this inventory.
