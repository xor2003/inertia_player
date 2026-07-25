from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
ORIGINAL_LST = ROOT / "IPLAY.lst"


def test_preferred_iplay_launcher_is_guarded_by_smoke_tests() -> None:
    top_launcher = (ROOT / "iplay.sh").read_text()
    launcher = (ROOT / "rewrite" / "iplay.sh").read_text()
    smoke = (ROOT / "tests" / "test_player_smoke.py").read_text()
    gate = (ROOT / "rewrite" / "check_rewrite.sh").read_text()
    coverage = (ROOT / "tests" / "COVERAGE.md").read_text()

    assert 'IPLAY_LAUNCHER_DISPLAY=./iplay.sh exec ./rewrite/iplay.sh "$@"' in top_launcher
    assert "launcher_display=${IPLAY_LAUNCHER_DISPLAY:-./rewrite/iplay.sh}" in launcher
    assert 'usage: $launcher_display [--check] [--rebuild] [--diagnostics] [iplay-options] <module-file|@file-list>' in launcher
    assert "builds rewrite/.build/iplay when missing or stale" in launcher
    assert "--check verifies the native SDL/notcurses host player is built and executable without starting playback" in launcher
    assert "--check-playback <module-file|@file-list> runs a short SDL/notcurses playback readiness check with dummy SDL audio by default" in launcher
    assert "`./iplay.sh --check-playback samples/aryx.s3m` is the quick proof command" in coverage
    assert "including `@file-list` input and clean failures for missing modules, corrupt known tracker data, unsupported probe files, deferred project-owned `.inr`, implementation-launcher real-module/`@file-list` success plus missing/corrupt/unknown/`.inr`/SDL-failure quick-check coverage, and SDL audio open failure" in coverage
    assert "--rebuild forces the native SDL/notcurses host player rebuild before launch" in launcher
    assert "streams player output live while keeping status evidence for the wrapper exit decision" in launcher
    assert "--diagnostics or IPLAY_LAUNCHER_DIAGNOSTICS=1 shows raw player evidence" in launcher
    assert "--list-extensions is passed through to list external tracker formats handled by the library-backed path" in launcher
    assert "--classify <path> is passed through to show decoder route selection without playback" in launcher
    assert "when --video-mode is omitted, iplay selects the nearest supported text mode from the terminal size" in launcher
    assert "supported --video-mode values: 40x25bw, 40x25color, 40x25mono, 80x25bw, 80x25color, 80x25mono, 80x50, 80x50project, auto, terminal" in launcher
    assert "a bare text-mode token may be supplied before or after <module-file|@file-list>" in launcher
    assert "press q, Q, or Escape to stop playback" in launcher
    assert "keyboard and block-limit player stops from the diagnostic host status code are returned as success" in launcher
    assert "cleanup_run_log()" in launcher
    assert "exit_after_signal()" in launcher
    assert "trap cleanup_run_log EXIT" in launcher
    assert "trap 'exit_after_signal 129' HUP" in launcher
    assert "trap 'exit_after_signal 130' INT" in launcher
    assert "trap 'exit_after_signal 143' TERM" in launcher
    assert "native_rebuild_deps=\"rewrite/iplay_rewrite.c rewrite/iplay_rewrite.h rewrite/modplug_renderer.cpp rewrite/modplug_renderer.hpp rewrite/modplug_audio_bridge.cpp rewrite/modplug_audio_bridge.hpp rewrite/modern_player.cpp rewrite/modern_player.hpp rewrite/modplug_audio_probe.cpp rewrite/build_native_player.sh\"" in launcher
    assert "./rewrite/build_native_player.sh" in launcher
    assert './rewrite/.build/iplay "${player_args[@]}"' in launcher
    assert 'player_args=()' in launcher
    assert 'launcher_diagnostics=1' in launcher
    assert 'launcher_rebuild=1' in launcher
    assert 'launcher_check=1' in launcher
    assert 'launcher_check_playback=1' in launcher
    assert 'native_rebuilt=0' in launcher
    assert 'native_rebuilt=1' in launcher
    assert './rewrite/.build/iplay --list-extensions >/dev/null' in launcher
    assert 'playback_args+=(--video-mode=40x25color)' in launcher
    assert './rewrite/.build/iplay "${playback_args[@]}"' in launcher
    assert "check_playback_video_mode_supported()" in launcher
    assert "is_bare_video_mode_token()" in launcher
    assert "normalize_leading_bare_video_mode_arg()" in launcher
    assert "normalize_leading_bare_video_mode_arg" in launcher
    assert 'video_mode=${player_arg#--video-mode=}' in launcher
    assert "video_mode_normalized=${video_mode,,}" in launcher
    assert "40x25|40x25bw|40x25color|40x25mono|80x25|80x25bw|80x25color|80x25mono|80x50|80x50project|auto|terminal)" in launcher
    assert 'echo "iplay: unsupported text mode: $video_mode" >&2' in launcher
    assert 'check_playback_video_mode_supported || {' in launcher
    assert 'iplay.sh: native executable failed no-playback startup check: rewrite/.build/iplay' in launcher
    assert 'iplay.sh: ready: exe=rewrite/.build/iplay rebuilt=$native_rebuilt' in launcher
    assert 'iplay.sh: playback-ready: exe=rewrite/.build/iplay video=${playback_cols}x${playback_rows} audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod' in launcher
    assert 'failure_status=$(grep -E \'^status=\' "$run_log" | tail -n 1 || true)' in launcher
    assert 'iplay.sh: playback readiness check failed: $failure_status' in launcher
    assert "^Terminal render: requested=1 cols=[0-9]+ rows=[0-9]+ bytes=[0-9]+" in launcher
    assert 'playback_bytes" -ne $((playback_cols * playback_rows * 2))' in launcher
    assert '^status=keyboard route_id=0 route=external-library provider=libmikmod' in launcher
    assert 'native_needs_rebuild=$launcher_rebuild' in launcher
    assert '    -h|--help)' in launcher
    assert '| tee "$run_log"' in launcher
    assert '2>&1 | tee "$run_log"' not in launcher
    assert 'python3 ./rewrite/pty_run.py "$run_log" ./rewrite/.build/iplay "${player_args[@]}"' in launcher
    pty_runner = (ROOT / "rewrite" / "pty_run.py").read_text()
    assert "master_fd, slave_fd = pty.openpty()" in pty_runner
    assert "def parse_env_size() -> tuple[int, int]:" in pty_runner
    assert "def request_resize(signum: int, frame: object) -> None:" in pty_runner
    assert "def forward_termination(signum: int, frame: object) -> None:" in pty_runner
    assert "signal.signal(signal.SIGWINCH, request_resize)" in pty_runner
    assert "signal.signal(signal.SIGTERM, forward_termination)" in pty_runner
    assert "os.killpg(child_process.pid, signum)" in pty_runner
    assert "os.killpg(proc.pid, signal.SIGTERM)" in pty_runner
    assert "proc.send_signal(signal.SIGWINCH)" in pty_runner
    assert "fcntl.ioctl(fd, termios.TIOCSWINSZ, struct.pack(\"HHHH\", rows, cols, 0, 0))" in pty_runner
    assert "set_pty_size(slave_fd)" in pty_runner
    assert "proc = subprocess.Popen(argv, stdin=slave_fd, stdout=slave_fd, stderr=None, close_fds=True, start_new_session=True)" in pty_runner
    assert "os.write(master_fd, input_data)" in pty_runner
    assert "os.write(sys.stdout.fileno(), data)" in pty_runner
    assert "log.write(data)" in pty_runner
    assert 'IPLAY_LAUNCHER_DIAGNOSTICS' in launcher
    assert '| awk' not in launcher
    assert 'status=${PIPESTATUS[0]}' in launcher
    assert 'grep -Eq \'^status=(keyboard|block-limit) \' "$run_log"' in launcher
    assert 'cleanup_run_log\n  exit 0' in launcher
    assert 'iplay.sh: playback failed: $failure_status' in launcher
    assert 'iplay.sh: player exited with status $status' in launcher
    assert "test_iplay_launcher_help_describes_rebuild_and_direct_player" in smoke
    assert "test_iplay_launcher_help_after_diagnostics_stays_launcher_help" in smoke
    assert "test_iplay_launcher_help_after_rebuild_stays_launcher_help" in smoke
    assert "test_iplay_launcher_check_verifies_binary_without_playback" in smoke
    assert "test_iplay_launcher_rebuild_check_forces_rebuild_without_playback" in smoke
    assert "test_iplay_launcher_lists_external_tracker_extensions_without_playback" in smoke
    assert "test_iplay_launcher_classifies_decoder_routes_without_playback" in smoke
    assert "test_iplay_launcher_runs_direct_sdl_notcurses_player" in smoke
    assert "test_iplay_launcher_diagnostics_mode_preserves_direct_player_evidence" in smoke
    assert "test_iplay_launcher_diagnostics_option_preserves_direct_player_evidence" in smoke
    assert "test_iplay_launcher_does_not_hide_decoder_failures" in smoke
    assert "test_iplay_launcher_reports_project_owned_decoder_unavailable_without_raw_status" in smoke
    assert "test_iplay_launcher_playback_check_project_owned_reports_decoder_unavailable" in smoke
    assert "test_iplay_launcher_playback_check_runs_short_sdl_notcurses_probe" in smoke
    assert "test_iplay_launcher_playback_check_accepts_filelist_probe" in smoke
    assert "test_iplay_launcher_playback_check_corrupt_known_tracker_reports_decoder_failure" in smoke
    assert "test_iplay_launcher_playback_check_unknown_probe_reports_unsupported_format" in smoke
    assert "test_iplay_launcher_playback_check_missing_module_reports_clean_not_found" in smoke
    assert "test_iplay_launcher_playback_check_filelist_missing_selected_module_stays_clean" in smoke
    assert "test_iplay_launcher_playback_check_sdl_audio_failure_is_not_ready" in smoke
    assert "test_iplay_launcher_reports_unsupported_probe_format_without_raw_status" in smoke
    assert "test_iplay_launcher_reports_sdl_audio_open_failure_without_hiding_stderr" in smoke
    assert "test_iplay_launcher_summarizes_early_failures_without_status_line" in smoke
    assert "test_iplay_launcher_without_video_mode_uses_terminal_auto_size" in smoke
    assert "test_iplay_launcher_filelist_selects_first_entry_through_direct_player" in smoke
    assert "test_iplay_launcher_resolves_module_path_case_like_dos" in smoke
    assert "test_iplay_launcher_resolves_filelist_path_case_like_dos" in smoke
    assert "test_iplay_launcher_documented_normal_command_runs_filtered_auto_mode" in smoke
    assert "test_top_level_iplay_normal_command_runs_filtered_sdl_notcurses_player" in smoke
    assert "test_top_level_iplay_normal_source_end_keeps_full_live_layout_without_diagnostics" in smoke
    assert "test_pty_runner_passes_columns_lines_to_child_stdout_tty" in smoke
    assert "test_pty_runner_forwards_stdin_through_child_tty" in smoke
    assert "test_pty_runner_uses_stdin_tty_size_when_stdout_is_piped" in smoke
    assert "test_pty_runner_forwards_sigwinch_size_to_child_stdout_tty" in smoke
    assert "test_pty_runner_forwards_termination_to_child_process_group" in smoke
    assert 'result.stdout.count("Inertia Player V1.22") > 1' in smoke
    assert '"Current Track" in result.stdout' in smoke
    assert '"Track Position" in result.stdout' in smoke
    assert '"Sound Blaster 16" in result.stdout' in smoke
    assert '"Terminal live: block=" not in result.stdout' in smoke
    assert "test_top_level_iplay_diagnostics_auto_size_supports_smaller_terminals" in smoke
    assert '("40", "25", "auto cols=40 rows=25", "Terminal render: requested=1 cols=40 rows=25 bytes=2000")' in smoke
    assert '("80", "25", "auto cols=80 rows=25", "Terminal render: requested=1 cols=80 rows=25 bytes=4000")' in smoke
    assert "test_top_level_iplay_normal_explicit_80x50_runs_filtered_sdl_notcurses_player" in smoke
    assert "test_top_level_iplay_normal_supported_text_modes_run_filtered_sdl_notcurses_player" in smoke
    assert "test_top_level_iplay_normal_accepts_video_mode_after_module" in smoke
    assert "test_top_level_iplay_invalid_video_mode_fails_before_playback" in smoke
    assert "test_top_level_iplay_normal_filelist_runs_filtered_sdl_notcurses_player" in smoke
    assert "test_top_level_iplay_normal_resolves_module_path_case_like_dos" in smoke
    assert "test_top_level_iplay_normal_resolves_filelist_path_case_like_dos" in smoke
    assert "test_top_level_iplay_missing_module_reports_original_style_not_found" in smoke
    assert "test_top_level_iplay_missing_module_reports_not_found_before_sdl_audio_failure" in smoke
    assert '"could not open SDL2 SB16 stereo audio sink" not in result.stderr' in smoke
    assert "test_top_level_iplay_missing_filelist_reports_clear_no_playback_error" in smoke
    assert "test_top_level_iplay_empty_filelist_reports_clear_no_playback_error" in smoke
    assert "test_top_level_iplay_filelist_missing_selected_module_reports_not_found" in smoke
    assert "test_top_level_iplay_filelist_missing_selected_module_reports_not_found_before_sdl_audio_failure" in smoke
    assert '"File list:" not in result.stdout' in smoke
    assert "test_top_level_iplay_corrupt_known_tracker_reports_filtered_decoder_failure" in smoke
    assert "test_top_level_iplay_project_owned_inr_reports_filtered_unavailable" in smoke
    assert "test_top_level_iplay_unknown_probe_reports_filtered_unsupported_format" in smoke
    assert "test_top_level_iplay_sdl_audio_open_failure_preserves_stderr" in smoke
    assert "test_top_level_iplay_lists_external_tracker_extensions_without_playback" in smoke
    assert "test_top_level_iplay_classifies_decoder_routes_without_playback" in smoke
    assert "test_top_level_iplay_launcher_help_delegates_to_preferred_launcher" in smoke
    assert "test_top_level_iplay_help_after_wrapper_flags_stays_launcher_help" in smoke
    assert "test_top_level_iplay_without_arguments_prints_usage_without_playback" in smoke
    assert "usage: ./iplay.sh [--check] [--rebuild] [--diagnostics] [iplay-options] <module-file|@file-list>" in smoke
    assert "example: ./iplay.sh --video-mode=80x50 samples/aryx.s3m" in smoke
    assert "test_top_level_iplay_launcher_check_delegates_to_preferred_launcher" in smoke
    assert "test_top_level_iplay_playback_check_runs_short_sdl_notcurses_probe" in smoke
    assert "test_top_level_iplay_playback_check_accepts_filelist_probe" in smoke
    assert "test_top_level_iplay_playback_check_honors_explicit_video_mode" in smoke
    assert "test_top_level_iplay_playback_check_missing_module_reports_clean_not_found" in smoke
    assert "test_top_level_iplay_playback_check_filelist_missing_selected_module_stays_clean" in smoke
    assert "test_top_level_iplay_playback_check_corrupt_known_tracker_reports_decoder_failure" in smoke
    assert "test_top_level_iplay_playback_check_unknown_probe_reports_unsupported_format" in smoke
    assert "test_top_level_iplay_playback_check_project_owned_reports_decoder_unavailable" in smoke
    assert "test_top_level_iplay_playback_check_sdl_audio_failure_is_not_ready" in smoke
    assert '"iplay.sh: playback readiness check failed with status 2" in result.stderr' in smoke
    assert '"iplay.sh: playback readiness check failed with status 3" in result.stderr' in smoke
    assert 'assert "Module:" not in result.stderr' in smoke
    assert 'assert "Terminal render:" not in result.stderr' in smoke
    assert "test_top_level_iplay_rebuild_check_delegates_to_preferred_launcher_without_playback" in smoke
    assert "test_top_level_iplay_diagnostics_proves_sdl_notcurses_sb16_path" in smoke
    assert "test_top_level_iplay_diagnostics_plays_valid_tracker_under_library_route" in smoke
    assert '@pytest.mark.parametrize("extension", [".MOD", ".XM", ".IT", ".XYZ"])' in smoke
    assert "test_top_level_iplay_environment_diagnostics_preserves_sdl_notcurses_evidence" in smoke
    assert "test_top_level_iplay_diagnostics_escape_key_stops_playback" in smoke
    assert "test_top_level_iplay_diagnostics_lowercase_q_stops_playback" in smoke
    assert "test_top_level_iplay_diagnostics_uppercase_q_stops_playback" in smoke
    assert "test -x iplay.sh" in gate
    assert "./iplay.sh --check >/dev/null" in gate
    assert "run_clean_normal_playback()" in gate
    assert 'printf \'q\\n\' | SDL_AUDIODRIVER=dummy COLUMNS=80 LINES=50 "$@" >"$out" 2>"$err"' in gate
    assert 'if [ "$rc" -ne 0 ]; then' in gate
    assert 'b"\\x1b[?25l"' in gate
    assert 'b"\\x1b[?7l"' in gate
    assert 'b"\\x1b[38;2;255;255;255;48;2;170;170;170m"' in gate
    assert 'b"\\x1b[38;2;255;255;85;48;2;"' in gate
    assert 'b"\\x1b[38;2;85;85;85;48;2;170;170;170m"' in gate
    assert 'b"Filename      : "' in gate
    assert 'b"aryx.s3m"' in gate
    assert 'b"Current Track"' in gate
    assert 'b"Track Position"' in gate
    assert 'b"Output Levels : " not in data and b"Sound Blaster 16" not in data' in gate
    assert 'b"Main Volume"' in gate
    assert 'b"Module Type   : "' in gate
    assert 'b"S3M"' in gate
    assert 'b"24bit Interpolation"' in gate
    assert 'b"F-12"' in gate
    assert "normal playback did not emit required notcurses-style status UI" in gate
    assert "Terminal render end|" in gate
    assert "normal playback leaked diagnostic output" in gate
    assert "normal playback wrote stderr" in gate
    assert "run_missing_module_failure()" in gate
    assert "SDL_AUDIODRIVER=not-a-real-driver" in gate
    assert 'if [ "$rc" -ne 2 ]; then' in gate
    assert "Module not found." in gate
    assert "missing module command tried to open SDL audio before failing" in gate
    assert "missing module command leaked playback output" in gate
    assert "run_invalid_video_mode_failure()" in gate
    assert "iplay: unsupported text mode: bad" in gate
    assert "invalid video-mode command did not report wrapper failure status" in gate
    assert "invalid video-mode command tried to open SDL audio before failing" in gate
    assert "invalid video-mode command leaked playback output" in gate
    assert "run_missing_filelist_failure()" in gate
    assert "iplay: could not resolve file list: $filelist_arg" in gate
    assert "missing file-list command did not report wrapper failure status" in gate
    assert "missing file-list command tried to open SDL audio before failing" in gate
    assert "missing file-list command leaked playback output" in gate
    assert "run_check_playback_missing_filelist_failure()" in gate
    assert "missing file-list readiness command wrote stdout" in gate
    assert "missing file-list readiness command did not report readiness failure status" in gate
    assert "missing file-list readiness command leaked wrong readiness state" in gate
    assert "run_check_playback_invalid_video_mode_failure()" in gate
    assert "invalid video-mode readiness command wrote stdout" in gate
    assert "invalid video-mode readiness command did not report readiness failure status" in gate
    assert "invalid video-mode readiness command leaked wrong readiness state" in gate
    assert "run_corrupt_external_tracker_failure()" in gate
    assert 'if [ "$rc" -ne 3 ]; then' in gate
    assert "status=external-decoder-failed route_id=0 route=external-library provider=libmikmod" in gate
    assert "corrupt external tracker command leaked raw diagnostic output" in gate
    assert "run_project_owned_decoder_unavailable()" in gate
    assert "status=project-decoder-unavailable route_id=1 route=project-owned provider=native" in gate
    assert "project-owned decoder command leaked raw diagnostic output" in gate
    assert "run_unsupported_probe_failure()" in gate
    assert "status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod" in gate
    assert "unsupported probe command leaked raw diagnostic output" in gate
    assert "run_check_playback_decoder_failure()" in gate
    assert "decoder readiness failure command wrote stdout" in gate
    assert "iplay.sh: playback readiness check failed: $expected_status" in gate
    assert "iplay.sh: playback readiness check failed with status 3" in gate
    assert "decoder readiness failure command leaked playback-ready or raw diagnostics" in gate
    assert "run_sdl_audio_open_failure()" in gate
    assert "could not open SDL2 SB16 stereo audio sink requested freq=44100 format=0x8010 channels=2 samples=1024:" in gate
    assert "SDL audio open failure command leaked successful playback output" in gate
    assert "SDL audio sink: requested=1 opened=1" in gate
    assert "run_check_playback_sdl_audio_open_failure()" in gate
    assert "SDL audio readiness failure command wrote stdout" in gate
    assert "iplay.sh: playback readiness check failed with status 2" in gate
    assert "SDL audio readiness failure command leaked wrong readiness state" in gate
    assert "run_clean_normal_playback ./iplay.sh samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./iplay.sh --video-mode=40x25 samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./iplay.sh --video-mode=40x25bw samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./iplay.sh --video-mode=40x25color samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./iplay.sh --video-mode=80x25 samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./iplay.sh --video-mode=80x25bw samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./iplay.sh --video-mode=80x25color samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./iplay.sh --video-mode=80x50 samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./iplay.sh samples/aryx.s3m --video-mode=80x50" in gate
    assert "run_clean_normal_playback ./iplay.sh --video-mode=80x50project samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./iplay.sh --video-mode=auto samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./iplay.sh --video-mode=terminal samples/aryx.s3m" in gate
    assert "run_diagnostics_playback_evidence()" in gate
    assert "run_diagnostics_playback_evidence ./iplay.sh --diagnostics --video-mode=80x50 samples/aryx.s3m" in gate
    assert "run_auto_size_diagnostics_evidence()" in gate
    assert "printf 'q\\n' | SDL_AUDIODRIVER=dummy COLUMNS=$columns LINES=$lines" in gate
    assert "auto-size diagnostics missing evidence" in gate
    assert "auto-size diagnostics wrote stderr" in gate
    assert "run_auto_size_diagnostics_evidence 40 25 'auto cols=40 rows=25' 'Terminal render: requested=1 cols=40 rows=25 bytes=2000' ./iplay.sh --diagnostics samples/aryx.s3m" in gate
    assert "run_auto_size_diagnostics_evidence 80 25 'auto cols=80 rows=25' 'Terminal render: requested=1 cols=80 rows=25 bytes=4000' ./iplay.sh --diagnostics samples/aryx.s3m" in gate
    assert "run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./iplay.sh --diagnostics samples/aryx.s3m" in gate
    assert "run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./iplay.sh --diagnostics --video-mode=auto samples/aryx.s3m" in gate
    assert "run_auto_size_diagnostics_evidence 80 50 'terminal cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./iplay.sh --diagnostics --video-mode=terminal samples/aryx.s3m" in gate
    assert "'Module: aryx.s3m'" in gate
    assert "'Selected text mode: 80x50 cols=80 rows=50'" in gate
    assert "'Terminal render: requested=1 cols=80 rows=50 bytes=8000'" in gate
    assert "'SDL audio sink: requested=1 opened=1'" in gate
    assert "'Terminal live summary: requested=1'" in gate
    assert "'Playback output: SDL-compatible SB16 16-bit stereo native.'" in gate
    assert "'Decoder route: id=0 name=external-library'" in gate
    assert "'PCM source: external_module seed=1 truncated=0 input=file-path renderer=e route=0 provider=libmikmod hook_provider=libmikmod stream_start=0'" in gate
    assert "'status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1'" in gate
    assert 'case "$arg" in' in gate
    assert '@*)' in gate
    assert "^File list: @.*[[:alnum:]_-]+\\.LST selected=.*aryx\\.s3m$" in gate
    assert "diagnostics playback missing file-list selection evidence" in gate
    assert "diagnostics playback wrote stderr" in gate
    assert "./iplay.sh --check-playback samples/aryx.s3m" in gate
    assert "run_invalid_video_mode_failure ./iplay.sh --video-mode=bad samples/aryx.s3m" in gate
    assert "run_invalid_video_mode_failure ./iplay.sh samples/aryx.s3m --video-mode=bad" in gate
    assert "run_check_playback_invalid_video_mode_failure ./iplay.sh --check-playback --video-mode=bad samples/aryx.s3m" in gate
    assert "run_check_playback_invalid_video_mode_failure ./iplay.sh --check-playback samples/aryx.s3m --video-mode=bad" in gate
    assert 'printf \'not a tracker module\\n\' > "$playback_check_tmp/BAD.S3M"' in gate
    assert 'printf \'INR placeholder\\n\' > "$playback_check_tmp/SONG.INR"' in gate
    assert 'printf \'not a tracker module\\n\' > "$playback_check_tmp/BAD.BIN"' in gate
    assert "printf '\\n\\t aryx.s3m \\r\\nignored.s3m\\n' > \"$playback_check_tmp/PLAYLIST.LST\"" in gate
    assert 'printf \'aryx.s3m\\n\' > "$playback_check_tmp/caseplay.lst"' in gate
    assert "printf '\\n\\t \\r\\n' > \"$playback_check_tmp/EMPTY.LST\"" in gate
    assert 'run_missing_module_failure ./iplay.sh "$playback_check_tmp/MISSING.S3M"' in gate
    assert 'run_missing_filelist_failure ./iplay.sh "@$playback_check_tmp/MISSING.LST"' in gate
    assert 'run_missing_filelist_failure ./iplay.sh "@$playback_check_tmp/EMPTY.LST"' in gate
    assert 'run_check_playback_missing_filelist_failure ./iplay.sh --check-playback "@$playback_check_tmp/MISSING.LST"' in gate
    assert 'run_check_playback_missing_filelist_failure ./iplay.sh --check-playback "@$playback_check_tmp/EMPTY.LST"' in gate
    assert 'run_corrupt_external_tracker_failure ./iplay.sh "$playback_check_tmp/BAD.S3M"' in gate
    assert 'run_project_owned_decoder_unavailable ./iplay.sh "$playback_check_tmp/SONG.INR"' in gate
    assert 'run_unsupported_probe_failure ./iplay.sh "$playback_check_tmp/BAD.BIN"' in gate
    assert "run_check_playback_decoder_failure 'status=external-decoder-failed route_id=0 route=external-library provider=libmikmod' ./iplay.sh --check-playback \"$playback_check_tmp/BAD.S3M\"" in gate
    assert "run_check_playback_decoder_failure 'status=project-decoder-unavailable route_id=1 route=project-owned provider=native' ./iplay.sh --check-playback \"$playback_check_tmp/SONG.INR\"" in gate
    assert "run_check_playback_decoder_failure 'status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod' ./iplay.sh --check-playback \"$playback_check_tmp/BAD.BIN\"" in gate
    assert 'run_sdl_audio_open_failure ./iplay.sh "$playback_check_tmp/aryx.s3m"' in gate
    assert 'run_check_playback_sdl_audio_open_failure ./iplay.sh --check-playback "$playback_check_tmp/aryx.s3m"' in gate
    assert 'run_check_playback_sdl_audio_open_failure ./iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_check_playback_sdl_audio_open_failure ./iplay.sh --check-playback "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_invalid_video_mode_failure ./iplay.sh "@$playback_check_tmp/PLAYLIST.LST" --video-mode=bad' in gate
    assert 'run_check_playback_invalid_video_mode_failure ./iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST" --video-mode=bad' in gate
    assert 'run_clean_normal_playback ./iplay.sh "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_clean_normal_playback ./iplay.sh "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_clean_normal_playback ./iplay.sh --video-mode=auto "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_clean_normal_playback ./iplay.sh --video-mode=terminal "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_clean_normal_playback ./iplay.sh --video-mode=auto "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_clean_normal_playback ./iplay.sh --video-mode=terminal "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_diagnostics_playback_evidence ./iplay.sh --diagnostics --video-mode=80x50 "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_diagnostics_playback_evidence ./iplay.sh --diagnostics --video-mode=80x50 "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_auto_size_diagnostics_evidence 80 50 \'auto cols=80 rows=50\' \'Terminal render: requested=1 cols=80 rows=50 bytes=8000\' ./iplay.sh --diagnostics --video-mode=auto "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_auto_size_diagnostics_evidence 80 50 \'terminal cols=80 rows=50\' \'Terminal render: requested=1 cols=80 rows=50 bytes=8000\' ./iplay.sh --diagnostics --video-mode=terminal "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_auto_size_diagnostics_evidence 80 50 \'auto cols=80 rows=50\' \'Terminal render: requested=1 cols=80 rows=50 bytes=8000\' ./iplay.sh --diagnostics --video-mode=auto "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_auto_size_diagnostics_evidence 80 50 \'terminal cols=80 rows=50\' \'Terminal render: requested=1 cols=80 rows=50 bytes=8000\' ./iplay.sh --diagnostics --video-mode=terminal "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert './iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert './iplay.sh --check-playback "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert "check_external_tracker_extensions()" in gate
    assert "external tracker extension list must start with extensions=" in gate
    assert "external tracker extension list contains malformed entries" in gate
    assert "external tracker extension list contains duplicates" in gate
    assert "for extension in .mod .nst .s3m .stm .669 .mtm .psm .far .ult .wow .okt .oct .xm .it .ptm .ams .dbm .dmf .mdl .dsm .med .imf .j2b; do" in gate
    assert "external tracker extension missing from --list-extensions" in gate
    assert "project-owned .inr must not be advertised as an external tracker extension" in gate
    assert "check_external_tracker_classification()" in gate
    assert "for extension in mod nst s3m stm 669 mtm psm far ult wow okt oct xm it ptm ams dbm dmf mdl dsm med imf j2b; do" in gate
    assert '"$launcher" --classify "example.$extension" | grep \'route_id=0 route=external-library library=1\' >/dev/null' in gate
    assert "check_decoder_route_classification()" in gate
    assert "decoder route classification mismatch for $path" in gate
    assert "external=1 project=0 route_id=0 route=external-library library=1 backend=\"SDL-compatible SB16 16-bit stereo\"" in gate
    assert "external=0 project=1 route_id=1 route=project-owned library=0 backend=\"SDL-compatible SB16 16-bit stereo\"" in gate
    assert "external=0 project=0 route_id=2 route=probe-by-content library=1 backend=\"SDL-compatible SB16 16-bit stereo\"" in gate
    assert "run_check_playback_ready()" in gate
    assert "playback readiness command did not report ready state" in gate
    assert "playback readiness command leaked raw playback diagnostics" in gate
    assert "playback readiness command wrote stderr" in gate
    assert "run_check_playback_ready_modes()" in gate
    assert "for mode in 40x25 40x25bw 40x25color 80x25 80x25bw 80x25color 80x50 80x50project auto terminal; do" in gate
    assert 'run_check_playback_ready "$launcher" --check-playback "$module" --video-mode="$mode"' in gate
    assert "run_check_playback_ready_modes ./iplay.sh samples/aryx.s3m" in gate
    assert 'run_check_playback_ready_modes ./iplay.sh "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_check_playback_ready_modes ./iplay.sh "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert "check_external_tracker_extensions ./iplay.sh" in gate
    assert "check_external_tracker_classification ./iplay.sh" in gate
    assert "check_decoder_route_classification ./iplay.sh example.s3m" in gate
    assert "check_decoder_route_classification ./iplay.sh example.inr" in gate
    assert "check_decoder_route_classification ./iplay.sh example.bin" in gate
    assert "./iplay.sh --classify example.inr | grep 'route_id=1 route=project-owned library=0' >/dev/null" in gate
    assert "./iplay.sh --classify example.bin | grep 'route_id=2 route=probe-by-content library=1' >/dev/null" in gate
    assert "test -x rewrite/iplay.sh" in gate
    assert "./rewrite/iplay.sh --check >/dev/null" in gate
    assert "run_invalid_video_mode_failure ./rewrite/iplay.sh --video-mode=bad samples/aryx.s3m" in gate
    assert "run_invalid_video_mode_failure ./rewrite/iplay.sh samples/aryx.s3m --video-mode=bad" in gate
    assert "run_check_playback_invalid_video_mode_failure ./rewrite/iplay.sh --check-playback --video-mode=bad samples/aryx.s3m" in gate
    assert "run_check_playback_invalid_video_mode_failure ./rewrite/iplay.sh --check-playback samples/aryx.s3m --video-mode=bad" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh --video-mode=40x25 samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh --video-mode=40x25bw samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh --video-mode=40x25color samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh --video-mode=80x25 samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh --video-mode=80x25bw samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh --video-mode=80x25color samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh --video-mode=80x50 samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh samples/aryx.s3m --video-mode=80x50" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh --video-mode=80x50project samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh --video-mode=auto samples/aryx.s3m" in gate
    assert "run_clean_normal_playback ./rewrite/iplay.sh --video-mode=terminal samples/aryx.s3m" in gate
    assert "run_diagnostics_playback_evidence ./rewrite/iplay.sh --diagnostics --video-mode=80x50 samples/aryx.s3m" in gate
    assert "run_auto_size_diagnostics_evidence 40 25 'auto cols=40 rows=25' 'Terminal render: requested=1 cols=40 rows=25 bytes=2000' ./rewrite/iplay.sh --diagnostics samples/aryx.s3m" in gate
    assert "run_auto_size_diagnostics_evidence 80 25 'auto cols=80 rows=25' 'Terminal render: requested=1 cols=80 rows=25 bytes=4000' ./rewrite/iplay.sh --diagnostics samples/aryx.s3m" in gate
    assert "run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./rewrite/iplay.sh --diagnostics samples/aryx.s3m" in gate
    assert "run_auto_size_diagnostics_evidence 80 50 'auto cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./rewrite/iplay.sh --diagnostics --video-mode=auto samples/aryx.s3m" in gate
    assert "run_auto_size_diagnostics_evidence 80 50 'terminal cols=80 rows=50' 'Terminal render: requested=1 cols=80 rows=50 bytes=8000' ./rewrite/iplay.sh --diagnostics --video-mode=terminal samples/aryx.s3m" in gate
    assert 'run_missing_module_failure ./rewrite/iplay.sh "$playback_check_tmp/MISSING.S3M"' in gate
    assert 'run_missing_filelist_failure ./rewrite/iplay.sh "@$playback_check_tmp/MISSING.LST"' in gate
    assert 'run_missing_filelist_failure ./rewrite/iplay.sh "@$playback_check_tmp/EMPTY.LST"' in gate
    assert 'run_check_playback_missing_filelist_failure ./rewrite/iplay.sh --check-playback "@$playback_check_tmp/MISSING.LST"' in gate
    assert 'run_check_playback_missing_filelist_failure ./rewrite/iplay.sh --check-playback "@$playback_check_tmp/EMPTY.LST"' in gate
    assert 'run_corrupt_external_tracker_failure ./rewrite/iplay.sh "$playback_check_tmp/BAD.S3M"' in gate
    assert 'run_project_owned_decoder_unavailable ./rewrite/iplay.sh "$playback_check_tmp/SONG.INR"' in gate
    assert 'run_unsupported_probe_failure ./rewrite/iplay.sh "$playback_check_tmp/BAD.BIN"' in gate
    assert "run_check_playback_decoder_failure 'status=external-decoder-failed route_id=0 route=external-library provider=libmikmod' ./rewrite/iplay.sh --check-playback \"$playback_check_tmp/BAD.S3M\"" in gate
    assert "run_check_playback_decoder_failure 'status=project-decoder-unavailable route_id=1 route=project-owned provider=native' ./rewrite/iplay.sh --check-playback \"$playback_check_tmp/SONG.INR\"" in gate
    assert "run_check_playback_decoder_failure 'status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod' ./rewrite/iplay.sh --check-playback \"$playback_check_tmp/BAD.BIN\"" in gate
    assert 'run_sdl_audio_open_failure ./rewrite/iplay.sh "$playback_check_tmp/aryx.s3m"' in gate
    assert 'run_check_playback_sdl_audio_open_failure ./rewrite/iplay.sh --check-playback "$playback_check_tmp/aryx.s3m"' in gate
    assert 'run_check_playback_sdl_audio_open_failure ./rewrite/iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_check_playback_sdl_audio_open_failure ./rewrite/iplay.sh --check-playback "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_invalid_video_mode_failure ./rewrite/iplay.sh "@$playback_check_tmp/PLAYLIST.LST" --video-mode=bad' in gate
    assert 'run_check_playback_invalid_video_mode_failure ./rewrite/iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST" --video-mode=bad' in gate
    assert 'run_clean_normal_playback ./rewrite/iplay.sh "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_clean_normal_playback ./rewrite/iplay.sh "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_clean_normal_playback ./rewrite/iplay.sh --video-mode=auto "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_clean_normal_playback ./rewrite/iplay.sh --video-mode=terminal "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_clean_normal_playback ./rewrite/iplay.sh --video-mode=auto "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_clean_normal_playback ./rewrite/iplay.sh --video-mode=terminal "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_diagnostics_playback_evidence ./rewrite/iplay.sh --diagnostics --video-mode=80x50 "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_diagnostics_playback_evidence ./rewrite/iplay.sh --diagnostics --video-mode=80x50 "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_auto_size_diagnostics_evidence 80 50 \'auto cols=80 rows=50\' \'Terminal render: requested=1 cols=80 rows=50 bytes=8000\' ./rewrite/iplay.sh --diagnostics --video-mode=auto "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_auto_size_diagnostics_evidence 80 50 \'terminal cols=80 rows=50\' \'Terminal render: requested=1 cols=80 rows=50 bytes=8000\' ./rewrite/iplay.sh --diagnostics --video-mode=terminal "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_auto_size_diagnostics_evidence 80 50 \'auto cols=80 rows=50\' \'Terminal render: requested=1 cols=80 rows=50 bytes=8000\' ./rewrite/iplay.sh --diagnostics --video-mode=auto "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_auto_size_diagnostics_evidence 80 50 \'terminal cols=80 rows=50\' \'Terminal render: requested=1 cols=80 rows=50 bytes=8000\' ./rewrite/iplay.sh --diagnostics --video-mode=terminal "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert "./rewrite/iplay.sh --check-playback samples/aryx.s3m" in gate
    assert "run_check_playback_ready_modes ./rewrite/iplay.sh samples/aryx.s3m" in gate
    assert './rewrite/iplay.sh --check-playback "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert './rewrite/iplay.sh --check-playback "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert 'run_check_playback_ready_modes ./rewrite/iplay.sh "@$playback_check_tmp/PLAYLIST.LST"' in gate
    assert 'run_check_playback_ready_modes ./rewrite/iplay.sh "@$playback_check_tmp/CASEPLAY.LST"' in gate
    assert "./rewrite/iplay.sh --rebuild --check | grep 'iplay\\.sh: ready: exe=rewrite/\\.build/iplay rebuilt=1' >/dev/null" in gate
    assert "check_external_tracker_extensions ./rewrite/iplay.sh" in gate
    assert "check_external_tracker_classification ./rewrite/iplay.sh" in gate
    assert "check_decoder_route_classification ./rewrite/iplay.sh example.s3m" in gate
    assert "check_decoder_route_classification ./rewrite/iplay.sh example.inr" in gate
    assert "check_decoder_route_classification ./rewrite/iplay.sh example.bin" in gate
    assert "./rewrite/iplay.sh --classify example.inr | grep 'route_id=1 route=project-owned library=0' >/dev/null" in gate
    assert "./rewrite/iplay.sh --classify example.bin | grep 'route_id=2 route=probe-by-content library=1' >/dev/null" in gate


TESTED_ORIGINAL_ENTRIES = {
    "adlib_18389",
    "adlib_18395",
    "adlib_clean",
    "adlib_init",
    "alloc_dma_buf",
    "_2stm_module",
    "CheckSB",
    "adlib_sndoff",
    "configure_timer",
    "copy_printable",
    "covox_deinit",
    "covox_init",
    "covox_sndoff",
    "cpy_printable",
    "draw_frame",
    "deinit_125B9",
    "doschdir",
    "dosexec",
    "dosfindnext",
    "dosfread",
    "dosgetcurdir",
    "dosseek",
    "ems_deinit",
    "ems_init",
    "ems_mapmem",
    "ems_mapmem2",
    "ems_mapmemx",
    "ems_mapmemy",
    "ems_realloc",
    "ems_realloc2",
    "ems_release",
    "ems_save_mapctx",
    "ems_restore_mapctx",
    "eff_1387F",
    "eff_13886",
    "eff_1389D",
    "eff_138A4",
    "eff_138D2",
    "eff_1392F",
    "eff_139AC",
    "eff_139B2",
    "eff_139B9",
    "eff_13A43",
    "eff_13A94",
    "eff_13AD7",
    "eff_13B06",
    "eff_13B78",
    "eff_13B88",
    "eff_13BA3",
    "eff_13BB2",
    "eff_13BC0",
    "eff_13BC8",
    "eff_13C02",
    "eff_13C34",
    "eff_13C3F",
    "eff_13C64",
    "eff_13C88",
    "eff_13C95",
    "eff_13CA2",
    "eff_13CB3",
    "eff_13CC9",
    "eff_13CDD",
    "eff_13CE8",
    "eff_13DE5",
    "eff_13DEF",
    "eff_13E1E",
    "eff_13E2D",
    "eff_13E32",
    "eff_13E7F",
    "eff_13E84",
    "eff_13E8C",
    "eff_13F05",
    "eff_13F3B",
    "eff_13FBE",
    "eff_14020",
    "eff_14030",
    "eff_14067",
    "eff_nullsub",
    "f1_help",
    "f2_draw_waves",
    "f2_draw_waves2",
    "f2_waves",
    "far_module",
    "f3_textmetter",
    "f4_patternnae",
    "f5_graphspectr",
    "f5_draw_spectr",
    "f6_undoc",
    "calc_14043",
    "change_amplif",
    "change_volume",
    "callsubx",
    "get_12F7C",
    "fill_dma",
    "fill_dmabuf16stereo",
    "fill_dmabuf8",
    "fill_dmabuf8stereo",
    "filelist_198B8",
    "find_mods",
    "get_comspec",
    "getint_vect",
    "get_keybsw",
    "get_playsettings",
    "graph_1C070",
    "clean_11C43",
    "clean_int8_mem_timr",
    "clean_timer",
    "getexename",
    "hex_1BE39",
    "inr_read_118B0",
    "inr_read_119B7",
    "inr_module",
    "int1a_timer",
    "int2f_checkmyself",
    "int9_keyb",
    "init_f5_spectr",
    "init_vga_waves",
    "initclockfromrtc",
    "keyb_19EFD",
    "set_playsettings",
    "set_dmachn_mask",
    "set_egasequencer",
    "set_timer",
    "set_timer_int",
    "getmemallocstrat",
    "setmemalloc1",
    "setmemalloc2",
    "setmemallocstrat",
    "setint_vect",
    "set_keybsw",
    "setsnd_handler",
    "snd_deinit",
    "snd_initialze",
    "snd_off",
    "snd_offx",
    "snd_on",
    "snd_on_parnt",
    "stereo_sndoff",
    "stereo_deinit",
    "stereo_init",
    "stereo_timer_int",
    "start",
    "someplaymode",
    "setvideomode",
    "sb_clean",
    "sb16_deinit",
    "sb16_detect_port",
    "sb16_handler_int",
    "sb16_18540",
    "sb16_init",
    "sb16_off",
    "sb16_on",
    "sb16_sound_off",
    "sb16_sound_on",
    "sb_detect_irq",
    "sb_handler_int",
    "sb_init",
    "sb_on",
    "sb_sndoff",
    "sb_test_interrupt",
    "sbpro_clean",
    "sbpro_init",
    "sbpro_sndoff",
    "s3m_module",
    "spectr_1B084",
    "spectr_1BBC1",
    "spectr_1B406",
    "spectr_1BC2D",
    "spectr_1BCE9",
    "sub_1AB8C",
    "sub_1279A",
    "sub_13429",
    "sub_135CA",
    "sub_137D5",
    "sub_13813",
    "sub_13826",
    "sub_13CF6",
    "sub_13D95",
    "sub_1609F",
    "sub_182DB",
    "timer_int_end",
    "txt_1ABAE",
    "txt_draw_top_title",
    "txt_draw_bottom",
    "video_prp_mtr_positn",
    "getset_playstate",
    "int24",
    "loc_157F2",
    "loadcfg",
    "memclean",
    "memfree_125DA",
    "memfree",
    "memalloc",
    "memalloc12k",
    "memfree_18A28",
    "memfill8080",
    "mem_reallocx",
    "memrealloc",
    "mod_1021E",
    "mod_1024A",
    "mod_102F5",
    "moduleread",
    "mod_n_t_module",
    "mod_read_10311",
    "mod_readfile_11F4E",
    "mod_readfile_12247",
    "mod_sub_delta",
    "modules_search",
    "mtm_module",
    "midi_clean",
    "midi_sndoff",
    "midi_153C0",
    "midi_153D6",
    "midi_153F1",
    "midi_15413",
    "midi_15442",
    "midi_1544D",
    "midi_15466",
    "midi_154DA",
    "midi_154DE",
    "midi_154AC",
    "midi_set",
    "mouse_getpos",
    "mouse_deinit",
    "mouse_hide",
    "mouse_hide2",
    "mouse_init",
    "mouse_1C7A9",
    "mouse_1C7CF",
    "mouse_show",
    "mouse_showcur",
    "my_i8toa10_0",
    "my_i8toa10",
    "my_i16toa10_0",
    "my_i16toa10",
    "my_i32toa10_0",
    "my_i32toa10",
    "my_pnt_u32toa_fill",
    "my_putdigit",
    "my_u16toa10",
    "my_u8toa_10",
    "my_u16toa_10",
    "my_u16tox",
    "my_u32toa",
    "my_u32toa_0",
    "my_u32toa_fill",
    "my_u32toa10",
    "my_u32toa10_0",
    "my_u32tox",
    "my_u4tox",
    "my_u8toa10",
    "my_u8tox",
    "myputdigit",
    "message_1BE77",
    "myasmsprintf",
    "mystrlen",
    "nullsub_2",
    "nullsub_4",
    "nullsub_5",
    "nullsub_3",
    "e669_module",
    "nongravis_dma",
    "parse_cmdline",
    "pcspeaker_clean",
    "pcspeaker_init",
    "pcspeaker_sndoff",
    "program_dma",
    "psm_module",
    "put_message",
    "put_message2",
    "read2buffer",
    "readallmoules",
    "read_module",
    "ReadMixerSB",
    "ReadSB",
    "read_sndsettings",
    "restore_intvector",
    "rereadrtc_settmr",
    "recolortxt",
    "sub_13177",
    "sub_131DA",
    "sub_131EF",
    "sub_13E9B",
    "sub_14087",
    "sub_140B6",
    "sub_1415E",
    "sub_154F4",
    "mystrlen",
    "mystrlen_0",
    "strcpy_count",
    "strcpy_count_0",
    "spectr_1C4F8",
    "sub_11BA6",
    "sub_11C0C",
    "sub_1265D",
    "sub_126A9",
    "sub_1281A",
    "sub_12AFD",
    "sub_12DA8",
    "sub_12B18",
    "sub_12B83",
    "sub_12CAD",
    "sub_12D05",
    "sub_12D35",
    "sub_12F56",
    "sub_13017",
    "sub_13044",
    "sub_13623",
    "sub_15577",
    "sub_19050",
    "sub_197F2",
    "text_1BF69",
    "text_init",
    "text_init2",
    "txt_blinkingoff",
    "write_scr",
    "txt_enableblink",
    "ult_module",
    "useless_12D61",
    "useless_11787",
    "useless_doswrite",
    "useless_doswrite2",
    "useless_strange",
    "useless_unset_egaseq",
    "useless_writeinr",
    "useless_writeinr_118",
    "useless_mysprintf",
    "useless_sprint_10",
    "useless_sprint_11",
    "useless_sprint_12",
    "useless_sprint_6",
    "useless_sprint_7",
    "useless_sprint_8",
    "useless_sprint_9",
    "u16tox",
    "u32tox",
    "u8tox",
    "u4tox",
    "ult_1150B",
    "ult_read",
    "vlm_141DF",
    "volume_prep",
    "volume_prepare_waves",
    "volume_12A66",
    "WriteMixerSB",
    "WriteSB",
}

SOUNDBLASTER_UNIT_SCOPE_ENTRIES = {
    # Sound Blaster helpers and SB16 paths exposed by the current translated
    # dispatch table and covered by original-binary parity tests. Older SB/SBPro
    # init/on/IRQ-detect labels exist in IPLAY.lst but are not currently exposed
    # as translated dispatch entries, so they need an integration harness or
    # future C/C++ rewrite hooks rather than this function-level runner.
    "CheckSB",
    "ReadMixerSB",
    "ReadSB",
    "WriteMixerSB",
    "WriteSB",
    "sb_clean",
    "sb16_18540",
    "sb16_deinit",
    "sb16_detect_port",
    "sb16_handler_int",
    "sb16_init",
    "sb16_off",
    "sb16_sound_off",
    "sb16_sound_on",
    "sb_sndoff",
    "sbpro_clean",
    "sbpro_sndoff",
}

DISPLAY_UNIT_SCOPE_ENTRIES = {
    # VGA/text routines exposed by the current translated dispatch table and
    # covered by original-binary parity tests. Full live draw loops that depend
    # on timer/audio/video integration need a higher-level harness or future
    # rewrite hooks; this scope is the unit-testable display surface today.
    "draw_frame",
    "f1_help",
    "f3_textmetter",
    "f4_patternnae",
    "f5_graphspectr",
    "f6_undoc",
    "filelist_198B8",
    "graph_1C070",
    "hex_1BE39",
    "init_f5_spectr",
    "message_1BE77",
    "put_message",
    "put_message2",
    "recolortxt",
    "set_egasequencer",
    "setvideomode",
    "spectr_1B406",
    "spectr_1BBC1",
    "spectr_1BC2D",
    "spectr_1BCE9",
    "spectr_1C4F8",
    "text_1BF69",
    "text_init",
    "text_init2",
    "txt_1ABAE",
    "txt_blinkingoff",
    "txt_draw_bottom",
    "txt_draw_top_title",
    "txt_enableblink",
    "video_prp_mtr_positn",
    "write_scr",
}

NON_SB_AUDIO_DRIVER_ENTRIES = {
    # User scope: audio-driver parity is required for Sound Blaster only.
    # Other output-device families are intentionally excluded from the full
    # required-coverage gate, even if some already have opportunistic tests.
    "adlib_set",
    "covox_on",
    "gravis_13215",
    "gravis_13272",
    "gravis_132A9",
    "gravis_13363",
    "gravis_134A2",
    "gravis_13A6A",
    "gravis_17DC6",
    "gravis_17DE8",
    "gravis_17E0E",
    "gravis_17E49",
    "gravis_17E86",
    "gravis_17F30",
    "gravis_17F7D",
    "gravis_18062",
    "gravis_18079",
    "gravis_18201",
    "gravis_18216",
    "gravis_clean",
    "gravis_init",
    "gravis_int",
    "gravis_set",
    "gravis_sndoff",
    "pcspeaker_set",
    "proaud_14700",
    "proaud_clean",
    "proaud_init",
    "proaud_set",
    "proaud_sndoff",
    "useless_17EEC",
    "proaud_spectr_14",
    "stereo_on",
    "wss_1495F",
    "wss_1498A",
    "wss_ReadMixer",
    "wss_WriteMixer",
    "wss_clean",
    "wss_init",
    "wss_set",
    "wss_sndoff",
    "wss_test",
}

TESTED_NON_PROC_ENTRIES = {
    # Covered label/chunk entrypoints that are intentionally tracked with the
    # parity inventory but are not declared as "proc" symbols in IPLAY.lst.
    "loc_157F2",
    "my_i16toa10",
    "my_i32toa10",
    "nullsub_3",
}


def original_procs() -> list[str]:
    pattern = re.compile(r"^seg[0-9A-Fa-f]+:[0-9A-Fa-f]{4}\s+([A-Za-z_][\w@$?]*)\s+proc\s+(near|far)")
    procs: list[str] = []
    for line in ORIGINAL_LST.read_text(errors="replace").splitlines():
        match = pattern.match(line)
        if match:
            procs.append(match.group(1))
    return procs


def test_original_proc_inventory_is_parseable() -> None:
    procs = original_procs()
    assert len(procs) == 383
    assert "moduleread" in procs
    assert "get_playsettings" in procs
    assert "u16tox" in procs


def test_required_original_proc_inventory_is_accounted_for() -> None:
    procs = set(original_procs())
    missing = procs - TESTED_ORIGINAL_ENTRIES
    unexpected = sorted(missing - NON_SB_AUDIO_DRIVER_ENTRIES)
    assert not unexpected, (
        f"{len(unexpected)} required original procs are missing from the tested inventory: "
        + ", ".join(unexpected)
    )
    assert len(procs) == 383
    assert len(procs & TESTED_ORIGINAL_ENTRIES) == 341
    assert len(missing) == 42


def test_soundblaster_unit_scope_has_parity_tests() -> None:
    procs = set(original_procs())
    unknown = sorted(SOUNDBLASTER_UNIT_SCOPE_ENTRIES - procs)
    assert not unknown, f"Sound Blaster inventory entries not found in IPLAY.lst: {', '.join(unknown)}"
    missing = sorted(SOUNDBLASTER_UNIT_SCOPE_ENTRIES - TESTED_ORIGINAL_ENTRIES)
    assert not missing, f"{len(missing)} Sound Blaster unit-scope procs still need parity tests: {', '.join(missing)}"


def test_display_unit_scope_has_parity_tests() -> None:
    procs = set(original_procs())
    unknown = sorted(DISPLAY_UNIT_SCOPE_ENTRIES - procs)
    assert not unknown, f"VGA/text inventory entries not found in IPLAY.lst: {', '.join(unknown)}"
    missing = sorted(DISPLAY_UNIT_SCOPE_ENTRIES - TESTED_ORIGINAL_ENTRIES)
    assert not missing, f"{len(missing)} VGA/text unit-scope procs still need parity tests: {', '.join(missing)}"


def test_private_raw_setters_are_macro_boundaries_and_public_raw_setters_are_explicit_abi() -> None:
    rewrite = (ROOT / "rewrite" / "iplay_rewrite.c").read_text()
    rewrite_sources = "\n".join(path.read_text() for path in sorted((ROOT / "rewrite").glob("*.c")))
    header = (ROOT / "rewrite" / "iplay_rewrite.h").read_text()
    private_raw_setter_bodies = sorted(
        set(re.findall(r"^static\s+void\s+([A-Za-z0-9_]*set[A-Za-z0-9_]*_raw)\s*\(", rewrite_sources, re.MULTILINE))
    )
    assert not private_raw_setter_bodies
    public_raw_setters = [
        "iplay_ncplane_set_cursor_yx_raw",
        "iplay_sdl_audio_device_set_backend_raw",
    ]
    rewrite_public_raw_setters = sorted(
        set(re.findall(r"^void\s+(iplay_[A-Za-z0-9_]*set[A-Za-z0-9_]*_raw)\s*\(", rewrite, re.MULTILINE))
    )
    header_public_raw_setters = sorted(
        set(re.findall(r"^void\s+(iplay_[A-Za-z0-9_]*set[A-Za-z0-9_]*_raw)\s*\(", header, re.MULTILINE))
    )
    assert rewrite_public_raw_setters == public_raw_setters
    assert header_public_raw_setters == public_raw_setters


def test_private_raw_getters_are_macro_boundaries() -> None:
    rewrite = (ROOT / "rewrite" / "iplay_rewrite.c").read_text()
    rewrite_sources = "\n".join(path.read_text() for path in sorted((ROOT / "rewrite").glob("*.c")))
    header = (ROOT / "rewrite" / "iplay_rewrite.h").read_text()
    private_raw_functions = sorted(
        set(re.findall(r"^static\b[^\n]*\b([A-Za-z_][A-Za-z0-9_]*_raw)\s*\(", rewrite_sources, re.MULTILINE))
    )
    assert private_raw_functions == []
    public_raw_functions = sorted(
        set(re.findall(r"^(?!static\b)[A-Za-z_][A-Za-z0-9_ *]*\s+(iplay_[A-Za-z0-9_]*_raw)\s*\(", rewrite, re.MULTILINE))
    )
    header_raw_functions = sorted(
        set(re.findall(r"^[A-Za-z_][A-Za-z0-9_ *]*\s+(iplay_[A-Za-z0-9_]*_raw)\s*\(", header, re.MULTILINE))
    )
    assert public_raw_functions == [
        "iplay_ncplane_set_cursor_yx_raw",
        "iplay_sdl_audio_device_backend_raw",
        "iplay_sdl_audio_device_set_backend_raw",
    ]
    assert header_raw_functions == public_raw_functions


def test_formatting_string_display_and_guard_abi_wrappers_read_registers_through_local_accessors() -> None:
    rewrite = (ROOT / "rewrite" / "iplay_rewrite.c").read_text()
    assert "static dd abi_eax(const IplayRegs *r) { return r->eax; }" in rewrite
    assert "static dd abi_ebx(const IplayRegs *r) { return r->ebx; }" in rewrite
    assert "static dd abi_ebp(const IplayRegs *r) { return r->ebp; }" in rewrite
    assert "static dd abi_ecx(const IplayRegs *r) { return r->ecx; }" in rewrite
    assert "static dd abi_edx(const IplayRegs *r) { return r->edx; }" in rewrite
    assert "static dd abi_esi(const IplayRegs *r) { return r->esi; }" in rewrite
    assert "static dd abi_edi(const IplayRegs *r) { return r->edi; }" in rewrite
    assert "dd eax = abi_eax(r);" in rewrite
    assert "dd ecx = abi_ecx(r);" in rewrite
    assert "dd edx = abi_edx(r);" in rewrite
    assert "dd esi = abi_esi(r);" in rewrite
    assert "dd edi = abi_edi(r);" in rewrite
    assert "IplayDecimalResult result = iplay_u32_base_to_buffer(mem, &si, value, base, (dw)abi_ecx(r));" in rewrite
    assert "int32_t value = (int32_t)eax;" in rewrite
    assert "IplayAsmSprintfResult result = iplay_myasmsprintf_to_buffer(mem, (dw)esi, (dw)edi, eax, ecx, edx);" in rewrite
    assert "IplayStringCopyResult result = iplay_strcpy_count_to_buffer(src_mem, dst_mem, (dw)esi, (dw)edi);" in rewrite
    assert "result = iplay_copy_printable_to_buffer(src_mem, dst_mem, (dw)abi_esi(r), (dw)abi_edi(r), (dw)abi_ecx(r));" in rewrite
    assert "IplayAttributedTextResult result = iplay_copy_attributed_fixed_to_buffer(src_mem, dst_mem, (dw)esi, (dw)edi, 0x16u, 0x7b);" in rewrite
    assert "apply_full_regs6(r, eax, abi_ebx(r), cx, (edx & 0xffffff00UL) | digit, si, abi_edi(r));" in rewrite
    assert "iplay_u32_decimal_fill_to_buffer(mem, &di, eax, count, with_pointer_prefix);" in rewrite
    assert "IplayAttributedTextResult result = iplay_message_1be77_to_buffer(mem, video_base, (dw)esi, (db)eax, (db)(eax >> 8));" in rewrite
    assert "IplayScreenStreamResult result = iplay_write_screen_stream_to_buffer(src_mem, dst_mem, (dw)esi, bp);" in rewrite
    assert "dd ebp = abi_ebp(r);" in rewrite
    assert "IplayRecolorResult result = iplay_recolor_text_row(mem, mode, (dw)eax, (db)ebx);" in rewrite
    assert "int outside = mouse_rect_hit(&ax, &bp, &cx, &dx, &si, &di);" in rewrite
    assert "int miss = mouse_table_lookup(mem, &bx, &ax, &bp, &cx, &dx, &si, &di, &id);" in rewrite
    assert "dd old_eax = abi_eax(r);" in rewrite
    assert "dd old_ebp = abi_ebp(r);" in rewrite
    assert "dd eax = (abi_eax(r) & 0xffff0000UL) | 1u;" in rewrite
    assert "apply_full_regs6(r, eax, abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));" in rewrite
    assert "dd eax = (old_eax & 0xffff0000UL) | 8u;" in rewrite
    assert "al = apply_delta_decode(mem, &si, &cx, al);" in rewrite
    assert "ch = write_packed_mod_event(mem, &di, &dx, bx, cl, ch, current_max);" in rewrite
    assert "pack_sub_126a9_regs(word_245fa, size1, channels, realloc_count, module_type, &eax, &bx, &cx, &si, &di);" in rewrite
    assert "pack_sub_1265d_regs(volume, sndcard, byte_24666, byte_24667, sndflags, byte_24628, stereo, byte_24671, word_245f6, word_245f0, &ax, &bx, &cx, &dx, &bp, &si, &di);" in rewrite
    assert "scan_sub_11c0c_stream(mem, &al, &bl, &si);" in rewrite
    assert "apply_ecx_esi_regs(r, old_ecx, si);" in rewrite
    assert "apply_eax_reg(r, (abi_eax(r) & 0xffff0000UL) | 0x156au);" in rewrite
    assert "pack_zero_event_regs(&eax, &ebx, &ecx, &edx, &esi);" in rewrite
    assert "apply_full_regs6(r, eax, ebx, ecx, edx, esi, old_edi);" in rewrite
    assert "IplayRegs6Result result = iplay_fill_dma_small_result(" in rewrite
    assert "abi_edi(r));" in rewrite
    assert "apply_full_regs6(r, eax, old_ebx, ecx, edx, old_esi, old_edi);" in rewrite
    assert "apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));" in rewrite
    assert "apply_full_regs6(r, 0x1000, ebx, 0x9abc, 0xdef0, abi_esi(r), abi_edi(r));" in rewrite
    assert "dd old_esi = abi_esi(r);" in rewrite
    assert "dd old_edi = abi_edi(r);" in rewrite
    assert "(old_esi & 0xffff0000UL) | si" in rewrite
    assert "(old_edi & 0xffff0000UL) | di" in rewrite
    assert "dw si = (dw)abi_esi(r);" in rewrite
    assert "dw di = (dw)abi_edi(r);" in rewrite
    assert "static void apply_fixed4_keep_index_regs(IplayRegs *r, dd eax, dd ebx, dd ecx, dd edx) {\n    apply_full_regs6(r, eax, ebx, ecx, edx, abi_esi(r), abi_edi(r));\n}" in rewrite
    assert "dd eax = ((abi_eax(r) & 0xff00u) == 0xff00u) ? 0xff00u : 0x1200u;" in rewrite
    assert "(old_ebx & 0xffff0000UL) | 0x9000u" in rewrite
    assert "apply_full_regs6(r, init_result.eax, init_result.ebx, init_result.ecx, init_result.edx, abi_esi(r), abi_edi(r));" in rewrite
    assert "apply_full_regs6(r, result.eax, result.ebx, result.ecx, result.edx, abi_esi(r), abi_edi(r));" in rewrite
    assert "apply_full_regs6(r, 0xfffc, 0, 0, 0xbf68, abi_esi(r), abi_edi(r));" in rewrite
    assert "apply_full_regs6(r, 0x156a, 1, 0, 0, abi_esi(r), abi_edi(r));" in rewrite
    assert "apply_full_regs6(r, 0x2345, 0x3040, abi_ecx(r), abi_edx(r), abi_esi(r), 0);" in rewrite
    assert "eax = (abi_eax(r) & 0xffff0000UL) | 0x0003u;" in rewrite
    assert "(old_eax & 0xffff0000UL) | 0x7f00u" in rewrite
    assert "(old_esi & 0xffff0000UL) | 0x1681u" in rewrite
    assert "db index = (db)((channel[0x35] & 0x0fu) + ((db)abi_ecx(r)));" in rewrite
    assert "dd ebx = (old_ebx & 0xffff0000UL) | (dw)(((dw)old_ebx) + 99u);" in rewrite
    assert "(old_edi & 0xffff0000UL) | (dw)(di + 1u)" in rewrite
    assert "(old_eax & 0xffff0000UL) | 0x0d8fu" in rewrite
    assert "apply_edi_reg(r, (abi_edi(r) & 0xffff0000UL) | di);" in rewrite
    assert "if ((dw)old_eax == 0x60ffu && (dw)old_ebx == 0x5344u && (dw)old_ecx == 0x4d50u)" in rewrite
    assert "apply_eax_reg(r, iplay_get_playsettings_eax(abi_eax(r), flag_playsettings));" in rewrite
    assert "IplaySb16RegsResult result = iplay_volume_12a66_result(channel_count, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));" in rewrite
    assert "dd eax = iplay_getset_playstate_eax(abi_eax(r), play_state);" in rewrite
    assert "IplayRegs6Result result = iplay_memclean_result(mem, (dw)old_edi, size, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), old_edi);" in rewrite
    assert "iplay_sub_12b83_state(globals, channels, channel_stride, types, (db)abi_eax(r));" in rewrite
    assert "IplayRegs6Result result = iplay_sub_13623_guard_result(channel_count, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));" in rewrite
    assert "*code_byte = iplay_sub_12d35_disable_code(abi_eax(r));" in rewrite
    assert "iplay_sub_12da8_guard_state(globals, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r));" in rewrite
    assert "iplay_eff_13a43_state(channel, (db)abi_eax(r), sndflags);" in rewrite
    assert "IplayRegs6Result result = iplay_eff_13ba3_result(channel, (db)abi_eax(r), abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));" in rewrite
    assert "apply_eax_reg(r, iplay_eff_13886_eax(channel, (db)abi_eax(r), abi_eax(r)));" in rewrite
    assert "apply_eax_reg(r, (abi_eax(r) & 0xffff0000UL) | iplay_calc_14043_ax(byte_2467b, byte_2467c));" in rewrite
    assert "IplayRegs3Result result = iplay_sub_14087_result(channel, (db)abi_eax(r), byte_24668, abi_eax(r), abi_ecx(r), abi_edx(r));" in rewrite
    assert "IplayRegs3Result result = iplay_eff_14067_result(globals, (db)abi_eax(r), byte_2467b, byte_2467c, freq, buffer_size, abi_eax(r), abi_ecx(r), abi_edx(r));" in rewrite
    assert "IplayRegs6Result result = iplay_eff_13bc8_result(channel, (db)abi_eax(r), (dw)abi_edx(r), byte_2461a, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));" in rewrite
    assert "dd eax = iplay_eff_13c02_eax(channel, globals, (db)old_eax, word_245f6, old_eax);" in rewrite
    assert "apply_eax_reg(r, iplay_eff_13ca2_eax((db)abi_eax(r), byte_24668, abi_eax(r)));" in rewrite
    assert "IplayRegs3Result result = iplay_sub_13d95_result(globals, abi_eax(r), abi_ecx(r), abi_edx(r));" in rewrite
    assert "db high = iplay_sub_13e9b((db)old_eax, &ax);" in rewrite
    assert "IplayRegs6Result result = iplay_sub_13826_result(channel, (db)abi_eax(r), byte_2461a, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));" in rewrite
    assert "apply_eax_reg(r, iplay_eff_13e1e_eax(channel, (db)abi_eax(r), abi_eax(r)));" in rewrite
    assert "IplayRegs3Result result = iplay_eff_139ac_result(channel, (db)abi_eax(r), max_volume, abi_eax(r), abi_ecx(r), abi_edx(r));" in rewrite
    assert "IplayRegs6Result result = iplay_eff_13fbe_result(channel, (db)abi_eax(r), byte_24668, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));" in rewrite
    assert "apply_eax_reg(r, iplay_change_amplif_eax(globals, sound_mode, abi_eax(r)));" in rewrite
    assert "apply_eax_reg(r, (abi_eax(r) & 0xffff00ffUL) | ((dw)channel[0x18] << 8));" in rewrite
    assert "ebp = (abi_ebp(r) & 0xffff0000UL) | (period >> 8);" in rewrite
    assert "IplayRegs6Result result = iplay_sub_1609f_disabled_result(dst, buffer_size, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));" in rewrite
    assert "IplaySb16RegsResult result = iplay_sb_helper_no_device_result(symbol, base_port, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));" in rewrite
    assert "IplayRegs6Result result = iplay_sb_legacy_init_no_device_result(globals, sbpro_mode, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));" in rewrite
    assert "result = iplay_sub_19050_bounded_result(abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r));" in rewrite
    assert "IplayRegs6Result result = iplay_memfill8080_result(dma, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));" in rewrite
    assert "IplayRegs6Result result = iplay_sndoff_fill_result(dma, symbol, abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_esi(r), abi_edi(r));" in rewrite
    assert "abi_eax(r), abi_ebx(r), abi_ecx(r), abi_edx(r), abi_ebp(r), abi_esi(r)," in rewrite


def test_iplay_rewrite_register_field_access_is_limited_to_abi_boundary_helpers() -> None:
    rewrite = (ROOT / "rewrite" / "iplay_rewrite.c").read_text()
    direct_access_lines = [
        line.strip()
        for line in rewrite.splitlines()
        if "r->" in line
    ]
    assert direct_access_lines == [
        "static dd abi_eax(const IplayRegs *r) { return r->eax; }",
        "static dd abi_ebx(const IplayRegs *r) { return r->ebx; }",
        "static dd abi_ebp(const IplayRegs *r) { return r->ebp; }",
        "static dd abi_ecx(const IplayRegs *r) { return r->ecx; }",
        "static dd abi_edx(const IplayRegs *r) { return r->edx; }",
        "static dd abi_esi(const IplayRegs *r) { return r->esi; }",
        "static dd abi_edi(const IplayRegs *r) { return r->edi; }",
        "r->eax = eax;",
        "r->ebx = ebx;",
        "r->ecx = ecx;",
        "r->edx = edx;",
        "r->esi = esi;",
        "r->edi = edi;",
        "r->eax = eax;",
        "r->ebp = ebp;",
        "r->esi = esi;",
        "r->edi = edi;",
        "r->eax = eax;",
        "r->edi = edi;",
        "r->eax = eax;",
        "r->esi = esi;",
        "r->ecx = ecx;",
        "r->esi = esi;",
        "r->eax = eax;",
        "r->edx = edx;",
        "r->eax = eax;",
        "r->ecx = ecx;",
        "r->edx = edx;",
        "r->ebx = ebx;",
        "r->ebp = ebp;",
        "r->ecx = ecx;",
        "r->esi = esi;",
        "r->eax = eax;",
        "r->ebx = ebx;",
        "r->ecx = ecx;",
        "r->edx = edx;",
        "r->ebp = ebp;",
        "r->esi = esi;",
    ]


def test_private_raw_macros_are_not_used_as_internal_field_accessors() -> None:
    rewrite_sources = "\n".join(path.read_text() for path in sorted((ROOT / "rewrite").glob("*.c")))
    private_raw_macros = sorted(
        set(re.findall(r"^#define\s+([A-Za-z_][A-Za-z0-9_]*_raw)\b", rewrite_sources, re.MULTILINE))
    )
    assert private_raw_macros == []


def test_raw_identifiers_are_public_abi_only() -> None:
    rewrite_sources = "\n".join(path.read_text() for path in sorted((ROOT / "rewrite").glob("*.c")))
    header = (ROOT / "rewrite" / "iplay_rewrite.h").read_text()
    raw_identifiers = sorted(
        set(re.findall(r"\b([A-Za-z_][A-Za-z0-9_]*_raw)\b", rewrite_sources + "\n" + header))
    )
    assert raw_identifiers == [
        "iplay_ncplane_set_cursor_yx_raw",
        "iplay_sdl_audio_device_backend_raw",
        "iplay_sdl_audio_device_set_backend_raw",
    ]


def test_core_rewrite_sources_do_not_use_inline_assembly() -> None:
    core_sources = "\n".join(
        (ROOT / "rewrite" / name).read_text()
        for name in ("iplay_rewrite.c", "iplay_player.c", "iplay_rewrite.h")
    )
    assert not re.findall(r"\b(?:_asm|__asm|asm\s*\()\b", core_sources)


def test_player_runtime_does_not_use_register_abi_surface() -> None:
    player = (ROOT / "rewrite" / "iplay_player.c").read_text()
    assert "IplayRegs" not in player
    assert "IplayRegs6Result" not in player
    assert not re.findall(r"\bapply_[A-Za-z0-9_]*regs?\b", player)
    assert not re.findall(r"\b(?:eax|ebx|ecx|edx|esi|edi|ax|bx|cx|dx|si|di)\b", player)


def test_player_direct_hardware_primitives_stay_inside_dos_io_adapter() -> None:
    player = (ROOT / "rewrite" / "iplay_player.c").read_text()
    direct_hardware_snippets = [
        "return (db)inp((unsigned)port);",
        "outp((unsigned)port, value);",
        "return ((unsigned long)FP_SEG(ptr) << 4) + FP_OFF(ptr);",
        "_fmemcpy(dst, src, byte_count);",
        "return (db far *)MK_FP(IPLAY_DOS_TEXT_COLOR_SEG, 0);",
    ]
    for snippet in direct_hardware_snippets:
        assert player.count(snippet) == 1
    assert "static db dos_hw_port_read(dw port)" in player
    assert "static void dos_hw_port_write(dw port, db value)" in player
    assert "static unsigned long dos_hw_far_physical(const void far *ptr)" in player
    assert "static void dos_hw_copy_to_far(void far *dst, const void *src, dw byte_count)" in player
    assert "static unsigned long dos_hw_timer_ticks(void)" in player
    assert "MK_FP(0x0040u, 0x006cu)" in player
    assert "static db far *dos_hw_text_color_memory(void)" in player
    assert "dos_hw_io_read_port(sb16_dsp_write_data_port(base_port))" in player
    assert "dos_hw_io_write_port(sb16_dsp_write_data_port(base_port), value);" in player
    assert "dos_hw_io_copy_to_far(sb16_dma_buffer_memory(), pcm, byte_count);" in player
    assert "dos_hw_io_copy_to_far(video, cells, byte_count);" in player
    assert "inp((unsigned)sb16" not in player
    assert "outp((unsigned)sb16" not in player
    assert "MK_FP(IPLAY_DOS_TEXT_COLOR_SEG" in player


def test_sdl_compatible_audio_boundary_keeps_callback_config_and_spec_api() -> None:
    rewrite = (ROOT / "rewrite" / "iplay_rewrite.c").read_text()
    header = (ROOT / "rewrite" / "iplay_rewrite.h").read_text()
    required_header_api = [
        "IplaySdlAudioCallback",
        "void iplay_sdl_audio_device_config_set_callback(IplaySdlAudioDeviceConfig *config, IplaySdlAudioCallback callback, void *userdata);",
        "IplaySdlAudioCallback iplay_sdl_audio_device_config_callback(const IplaySdlAudioDeviceConfig *config);",
        "void *iplay_sdl_audio_device_config_userdata(const IplaySdlAudioDeviceConfig *config);",
        "int iplay_sdl_audio_device_open(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config, IplayAudioWriteFn write, void *write_user);",
        "IplaySdlAudioSpec iplay_sdl_audio_device_spec(const IplaySdlAudioDevice *device);",
        "int iplay_sdl_audio_device_is_sdl_compatible(const IplaySdlAudioDevice *device);",
        "dw iplay_sdl_audio_device_callback(void *user, db *stream, dw byte_count);",
    ]
    for api in required_header_api:
        assert api in header
    assert "iplay_sdl_audio_device_config_set_callback_field(config, callback);" in rewrite
    assert "iplay_sdl_audio_device_config_set_userdata_field(config, userdata);" in rewrite
    assert "return iplay_sdl_audio_device_config_callback_field(config);" in rewrite
    assert "return iplay_sdl_audio_device_config_userdata_field(config);" in rewrite
    assert "iplay_sdl_audio_device_config_sb16_stereo(&config, device, IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE, 0);" in rewrite
    assert "return !iplay_sdl_audio_spec_hardware_enabled(spec)" in rewrite
    assert "&& iplay_audio_backend_is_sdl_compatible(iplay_sdl_audio_spec_backend(spec))" in rewrite


def test_sb16_audio_boundary_is_exactly_signed_16bit_stereo() -> None:
    rewrite = (ROOT / "rewrite" / "iplay_rewrite.c").read_text()
    header = (ROOT / "rewrite" / "iplay_rewrite.h").read_text()
    assert "extern const IplayAudioFormat IPLAY_AUDIO_SB16_STEREO_16;" in header
    assert "const IplayAudioFormat IPLAY_AUDIO_SB16_STEREO_16 = { 44100u, 16u, 2u, 1u };" in rewrite
    assert "int iplay_audio_format_is_sb16_stereo_16(const IplayAudioFormat *format)" in rewrite
    assert "iplay_audio_format_bits_per_sample(format) == 16u" in rewrite
    assert "iplay_audio_format_channels(format) == 2u" in rewrite
    assert "iplay_audio_format_signed_samples(format) != 0" in rewrite
    assert "iplay_audio_sink_init(iplay_audio_output_sink(output), &IPLAY_AUDIO_SB16_STEREO_16, write, user);" in rewrite
    assert "iplay_sdl_audio_device_config_set_format(config, &IPLAY_AUDIO_SB16_STEREO_16);" in rewrite
    assert "&& iplay_audio_format_equals(iplay_sdl_audio_device_format(device), &IPLAY_AUDIO_SB16_STEREO_16);" in rewrite


def test_player_runtime_uses_c_owned_memory_naming() -> None:
    player = (ROOT / "rewrite" / "iplay_player.c").read_text()
    assert "translated" not in player.lower()
    assert "static void player_set_memory_byte(db *player_mem, dw offset, db value)" in player
    assert "static db player_memory_byte(const db *player_mem, dw offset)" in player
    assert "static void player_init_audio_defaults(db *player_mem)" in player
    assert "static void player_start_program_memory(db *player_mem)" in player
    assert "static void player_clear_player_memory(void)" in player


def test_text_mode_geometry_uses_named_constants() -> None:
    rewrite = (ROOT / "rewrite" / "iplay_rewrite.c").read_text()
    header = (ROOT / "rewrite" / "iplay_rewrite.h").read_text()
    player = (ROOT / "rewrite" / "iplay_player.c").read_text()
    runner = (ROOT / "rewrite" / "rewrite_runner.c").read_text()
    status = (ROOT / "rewrite" / "STATUS.md").read_text()
    stale_raw_status_phrases = [
        "raw accessors now explicitly own",
        "raw accessors now own",
        "raw helper accessors",
        "raw helper",
        "raw helpers",
        "through raw accessors before",
    ]
    assert not [phrase for phrase in stale_raw_status_phrases if phrase in status]
    assert "IPLAYC.EXE` is not yet a full replacement player because tracker formats" in status
    assert "typedef int (*PlayerExternalDecoderRenderFn)(void *user, const PlayerModuleInfo *module, PlayerPlaybackBlock *block);" in header
    assert "#define IPLAY_PLAYER_EXTERNAL_DECODER_UNAVAILABLE 0" in header
    assert "#define IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED 1" in header
    assert "#define IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED 2" in header
    assert "void iplay_player_set_external_decoder(PlayerExternalDecoderRenderFn render, void *user, const char *provider);" in header
    assert "void iplay_player_clear_external_decoder(void);" in header
    assert "const char *iplay_player_module_path(const PlayerModuleInfo *module);" in header
    assert "unsigned long iplay_player_module_size(const PlayerModuleInfo *module);" in header
    assert "int iplay_player_module_header_truncated(const PlayerModuleInfo *module);" in header
    assert "const char *iplay_player_module_decoder_input_name(const PlayerModuleInfo *module);" in header
    assert "db *iplay_player_playback_block_pcm(PlayerPlaybackBlock *block);" in header
    assert "dw iplay_player_playback_block_frames(const PlayerPlaybackBlock *block);" in header
    assert "dw iplay_player_playback_block_active_bytes(const PlayerPlaybackBlock *block);" in header
    assert "static PlayerExternalDecoder player_external_decoder" in player
    assert "player_external_decoder_render(context->module, block)" in player
    assert "external_status == IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED" in player
    assert "external_status == IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED" in player
    assert "player_decoder_context_mark_ended(context);" in player
    assert "if (player_decoder_context_ended(context)) return player_playback_block_frames(block);" in player
    assert "if (!player_external_decoder_available()) return \"none\";" in player
    assert "return player_external_decoder_provider_name();" in player
    assert "const char *iplay_player_module_path(const PlayerModuleInfo *module)" in player
    assert "db *iplay_player_playback_block_pcm(PlayerPlaybackBlock *block)" in player
    bridge_h = (ROOT / "rewrite" / "modplug_audio_bridge.hpp").read_text()
    bridge = (ROOT / "rewrite" / "modplug_audio_bridge.cpp").read_text()
    assert "#define IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER 0" in bridge_h
    assert "#if IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER" in bridge_h
    assert "#if IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER" in bridge
    assert "IplayModplugExternalDecoder *iplay_modplug_external_decoder_create(void);" in bridge_h
    assert "void iplay_modplug_external_decoder_destroy(IplayModplugExternalDecoder *decoder);" in bridge_h
    assert "int iplay_modplug_external_decoder_render(void *user, const PlayerModuleInfo *module, PlayerPlaybackBlock *block);" in bridge_h
    assert "void iplay_modplug_external_decoder_install(IplayModplugExternalDecoder *decoder);" in bridge_h
    assert "void iplay_modplug_external_decoder_uninstall(void);" in bridge_h
    assert "struct IplayModplugExternalDecoder" in bridge
    assert 'iplay_player_set_external_decoder(iplay_modplug_external_decoder_render, decoder, "libmikmod");' in bridge
    assert "iplay_player_clear_external_decoder();" in bridge
    assert "iplay_player_module_path(module)" in bridge
    assert "iplay_player_playback_block_pcm(block)" in bridge
    assert "iplay_player_playback_block_frames(block)" in bridge
    assert "iplay_modplug_pcm_source_read(decoder->source, reinterpret_cast<std::int16_t *>(pcm), (int)frames, &stats)" in bridge
    assert "IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED" in bridge
    assert "IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED" in bridge
    hook_probe = (ROOT / "rewrite" / "modplug_player_hook_probe.cpp").read_text()
    assert 'std::strcmp(argv[2], "--unavailable") == 0' in hook_probe
    assert "unavailable_status = installed_render(installed_user, &module, &first);" in hook_probe
    assert "unavailable_checksum = checksum_block(&first);" in hook_probe
    modplug_tests = (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_libmodplug_external_decoder_hook_reports_installed_provider_when_unavailable" in modplug_tests
    assert "provider=libmikmod unavailable_status=" in modplug_tests
    assert "IPLAYC.EXE` is linked against `iplay_player_cont_zm.obj`" in status
    assert "IPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS" in status
    assert "IPLAY_PLAYER_ENABLE_DIAGNOSTICS=0" in status
    assert "IPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1" in status
    assert "Diagnostic/test binaries keep wrapper I/O because `kvikdos` does not emulate SB16 hardware." in status
    assert "IPLAYDIAG.EXE` keeps the default bounded diagnostic policy" in status
    assert "IPLAYC.EXE` is not yet a full replacement player. The current module path" not in status
    assert "currently defaults to `mode=playback policy=bounded-trial" not in status
    assert "This is not yet the default user-trial binary" not in status
    assert "private static `_raw` function bodies are not used anywhere in the rewrite C source set" in status
    assert "macro field boundaries" in status
    forbidden = [
        r"\*\s*80u\b",
        r"\b80u\s*\*",
        r"\*\s*160u\b",
        r"\b160u\s*\*",
        r"\bIPLAY_TEXT_ROWS_25\b",
        r"\bIPLAY_TEXT_COLS_80\b",
    ]
    offenders = []
    for pattern in forbidden:
        offenders.extend(
            f"{lineno}: {line.strip()}"
            for lineno, line in enumerate(rewrite.splitlines(), 1)
            if re.search(pattern, line) and "IPLAY_TEXT_MODE_" not in line
        )
    assert not offenders, (
        "text display code must not assume one 80x25 mode directly; use "
        "IPLAY_TEXT_DEFAULT_* or IPLAY_TEXT_OFFSET/ROW_BYTES: " + "; ".join(offenders)
    )
    default_col_uses = [
        f"{lineno}: {line.strip()}"
        for lineno, line in enumerate(rewrite.splitlines(), 1)
        if "IPLAY_TEXT_DEFAULT_COLS" in line and "IPLAY_TEXT_DEFAULT_MODE" not in line
    ]
    assert not default_col_uses, (
        "drawing code must use the active text mode, not default columns: "
        + "; ".join(default_col_uses)
    )
    assert "put_cell_run" not in rewrite
    assert "draw_frame_border_row" not in rewrite
    assert "iplay_ncplane_vline_yx(plane, (dw)(y + 1u), x" in rewrite
    assert "put_cell_yx" not in rewrite
    assert "put_text_at" not in rewrite
    assert "put_text_yx" not in rewrite
    assert "put_textn_yx" not in rewrite
    assert "IplayBottomLayout" in header
    assert "IplayTextScreen" in header
    assert "IplayTerminal" in header
    assert "IplayTerminalBackend" in header
    assert "IplayVideoSpec" in header
    assert "IplayNotcurses" in header
    assert "IplayVideoPresentFn" in header
    assert "IPLAY_TERMINAL_BACKEND_VGA_MEMORY" in header
    assert "IplayNcPlane root" in header
    assert "IplayWindow" in header
    assert "origin_y" in header
    assert "origin_x" in header
    assert "IplayTextScreen screen" in header
    assert "iplay_draw_frame_plane" in header
    assert "iplay_txt_draw_top_title_plane" in header
    assert "iplay_txt_draw_bottom_plane" in header
    assert "cursor_y" in header
    assert "cursor_x" in header
    assert "module_width" in header
    assert "playstate_width" in header
    assert "IPLAY_VIDEO_MODE_40X25_BW" in header
    assert "IPLAY_VIDEO_MODE_40X25_COLOR" in header
    assert "IPLAY_VIDEO_MODE_80X25_BW" in header
    assert "IPLAY_VIDEO_MODE_80X25_COLOR" in header
    assert "IPLAY_VIDEO_MODE_80X28_PROJECT" in header
    assert "IPLAY_VIDEO_MODE_80X50_PROJECT" in header
    assert "extern const IplayTextMode IPLAY_TEXT_MODE_40X25;" in header
    assert "extern const IplayTextMode IPLAY_TEXT_MODE_80X25;" in header
    assert "extern const IplayTextMode IPLAY_TEXT_MODE_80X28;" in header
    assert "extern const IplayTextMode IPLAY_TEXT_MODE_80X50;" in header
    assert "const IplayTextMode IPLAY_TEXT_MODE_40X25 = { IPLAY_TEXT_COLS_40, IPLAY_TEXT_ROWS_25 };" in rewrite
    assert "const IplayTextMode IPLAY_TEXT_MODE_80X25 = { IPLAY_TEXT_COLS_80, IPLAY_TEXT_ROWS_25 };" in rewrite
    assert "const IplayTextMode IPLAY_TEXT_MODE_80X28 = { IPLAY_TEXT_COLS_80, IPLAY_TEXT_ROWS_28 };" in rewrite
    assert "const IplayTextMode IPLAY_TEXT_MODE_80X50 = { IPLAY_TEXT_COLS_80, IPLAY_TEXT_ROWS_50 };" in rewrite
    assert "&IPLAY_TEXT_MODE_40X25" in rewrite
    assert "&IPLAY_TEXT_MODE_80X25" in rewrite
    assert "&IPLAY_TEXT_MODE_80X28" in rewrite
    assert "&IPLAY_TEXT_MODE_80X50" in rewrite
    assert "#define IPLAY_TEXT_ROWS_28 28u" in header
    assert "#define IPLAY_TEXT_SUPPORTED_MODE_COUNT 4u" in header
    assert "static const IplayTextMode *const iplay_supported_text_modes[IPLAY_TEXT_SUPPORTED_MODE_COUNT]" in rewrite
    assert "return IPLAY_TEXT_SUPPORTED_MODE_COUNT;" in rewrite
    assert "IPLAY_VIDEO_METER_LEFT_X" in header
    assert "IPLAY_VIDEO_METER_CENTER_DIVISOR" in header
    assert "IPLAY_VIDEO_METER_RIGHT_DIVISOR" in header
    assert "IPLAY_VIDEO_METER_RIGHT_PAD" in header
    assert "IPLAY_RUNTIME_STATUS_TITLE_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_MODULE_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_SIZE_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_LOADER_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_AUDIO_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_HARDWARE_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_VIDEO_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_LEVELS_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_TAG_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_PLAYBACK_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_PANEL_ROW" in header
    assert "IPLAY_RUNTIME_STATUS_PANEL_HEIGHT" in header
    assert "IPLAY_RUNTIME_STATUS_PANEL_ATTR" in header
    assert "IPLAY_RUNTIME_STATUS_PANEL_FILL_ATTR" in header
    assert "IPLAY_RUNTIME_STATUS_LEVELS_X" in header
    assert "IPLAY_RUNTIME_STATUS_LEVELS_WIDTH" in header
    assert "IPLAY_RUNTIME_STATUS_TITLE_ATTR" in header
    assert "IPLAY_RUNTIME_STATUS_LABEL_ATTR" in header
    assert "IPLAY_RUNTIME_STATUS_VALUE_ATTR" in header
    assert "IPLAY_RUNTIME_STATUS_PLAYBACK_ATTR" in header
    assert "IPLAY_RUNTIME_CONFIG_OK" in header
    assert "IPLAY_RUNTIME_CONFIG_MISSING_CELLS" in header
    assert "IPLAY_RUNTIME_CONFIG_MISSING_MODE" in header
    assert "IPLAY_RUNTIME_CONFIG_MISSING_AUDIO" in header
    assert "IPLAY_RUNTIME_CONFIG_SMALL_CELLS" in header
    assert "IPLAY_TEXT_SUPPORTED_MODE_COUNT" in header
    assert "IPLAY_TEXT_FALLBACK_COLS" in header
    assert "IPLAY_TEXT_FALLBACK_ROWS" in header
    assert "IPLAY_TEXT_MAX_COLS" in header
    assert "IPLAY_TEXT_MAX_ROWS" in header
    assert "IPLAY_TEXT_DEFAULT_VIDEO_MODE" in header
    assert "IPLAY_TEXT_DEFAULT_SCREEN_BYTES" in header
    assert "IPLAY_TEXT_FALLBACK_SCREEN_BYTES" in header
    assert "IPLAY_TEXT_MAX_SCREEN_BYTES" in header
    assert "IPLAY_TEXT_MAX_SCREEN_BYTES" in player
    assert "IPLAY_TEXT_DEFAULT_VIDEO_MODE" in player
    assert "const IplayBottomLayout *iplay_bottom_layout(void);" in header
    assert "const IplayBottomLayout *iplay_bottom_layout_for_mode(const IplayTextMode *mode);" in header
    assert "int iplay_bottom_layout_fits(const IplayBottomLayout *layout, const IplayTextMode *mode);" in header
    assert "const IplayTextMode *iplay_text_mode_for_size(dw cols, dw rows);" in header
    assert "const IplayTextMode *iplay_text_default_mode(void);" in header
    assert "const IplayTextMode *iplay_set_current_text_video_mode(db video_mode);" in header
    assert "const IplayTextMode *iplay_text_fallback_mode(void);" in header
    assert "const IplayTextMode *iplay_text_supported_mode(dw index);" in header
    assert "dw iplay_text_supported_mode_count(void);" in header
    assert "int iplay_text_size_is_supported(dw cols, dw rows);" in header
    assert "int iplay_text_mode_is_supported(const IplayTextMode *mode);" in header
    assert "dw iplay_text_mode_cols(const IplayTextMode *mode);" in header
    assert "dw iplay_text_mode_rows(const IplayTextMode *mode);" in header
    assert "dw iplay_text_max_screen_bytes(void);" in header
    assert "if (iplay_text_mode_cols(mode) == cols && iplay_text_mode_rows(mode) == rows) return mode;" in rewrite
    assert "#define iplay_text_mode_cols_field(state) ((state)->cols)" in rewrite
    assert "#define iplay_text_mode_rows_field(state) ((state)->rows)" in rewrite
    assert "static dw iplay_text_mode_cols_field(const IplayTextMode *state)" not in rewrite
    assert "static dw iplay_text_mode_rows_field(const IplayTextMode *state)" not in rewrite
    assert "return iplay_text_mode_cols_field(mode);" in rewrite
    assert "return iplay_text_mode_rows_field(mode);" in rewrite
    assert "iplay_current_text_mode = iplay_text_mode_for_video_mode(video_mode);" in rewrite
    assert "return IPLAY_TEXT_ROW_BYTES(iplay_text_mode_cols(mode));" in rewrite
    assert "return (dw)(iplay_text_mode_cols(mode) * iplay_text_mode_rows(mode));" in rewrite
    assert "return iplay_text_mode_cols(a) == iplay_text_mode_cols(b) && iplay_text_mode_rows(a) == iplay_text_mode_rows(b);" in rewrite
    assert "msg_off = (dw)((dw)(y - 1u) * iplay_text_mode_row_bytes(mode) + ((dw)cl & 0xfffeu) + 0x00a4u);" in rewrite
    assert "dw di = (dw)((row * iplay_text_mode_row_bytes(mode)) + IPLAY_TEXT_OFFSET(iplay_text_mode_cols(mode), 10u, 8u) + 1u);" in rewrite
    assert "dw cols = iplay_text_mode_cols(mode);" in rewrite
    assert "if (mode->cols == cols && mode->rows == rows) return mode;" not in rewrite
    assert "return IPLAY_TEXT_ROW_BYTES(mode->cols);" not in rewrite
    assert "return (dw)(mode->cols * mode->rows);" not in rewrite
    assert "return mode->cols;" not in rewrite
    assert "return mode->rows;" not in rewrite
    assert "return a->cols == b->cols && a->rows == b->rows;" not in rewrite
    assert "msg_off = (dw)((dw)(y - 1u) * IPLAY_TEXT_ROW_BYTES(mode->cols)" not in rewrite
    assert "IPLAY_TEXT_OFFSET(mode->cols, 10u, 8u)" not in rewrite
    assert "dw cols = mode->cols;" not in rewrite
    assert "void iplay_text_screen_init(IplayTextScreen *screen, db *cells, const IplayTextMode *mode);" in header
    assert "void iplay_text_screen_init_capacity(IplayTextScreen *screen, db *cells, dw capacity_bytes, const IplayTextMode *mode);" in header
    assert "void iplay_text_screen_set_cells(IplayTextScreen *screen, db *cells);" in header
    assert "void iplay_text_screen_set_capacity(IplayTextScreen *screen, dw capacity_bytes);" in header
    assert "void iplay_text_screen_set_mode(IplayTextScreen *screen, const IplayTextMode *mode);" in header
    assert "void iplay_text_screen_reinit_root(IplayTextScreen *screen);" in header
    assert "void iplay_text_screen_resize(IplayTextScreen *screen, const IplayTextMode *mode);" in header
    assert "int iplay_text_screen_resize_checked(IplayTextScreen *screen, const IplayTextMode *mode);" in header
    assert "void iplay_text_screen_resize_to_size(IplayTextScreen *screen, dw cols, dw rows);" in header
    assert "int iplay_text_screen_resize_to_size_checked(IplayTextScreen *screen, dw cols, dw rows);" in header
    assert "int iplay_text_screen_can_resize(const IplayTextScreen *screen, const IplayTextMode *mode);" in header
    assert "dw iplay_text_screen_capacity(const IplayTextScreen *screen);" in header
    assert "db *iplay_text_screen_cells(IplayTextScreen *screen);" in header
    assert "const db *iplay_text_screen_cells_const(const IplayTextScreen *screen);" in header
    assert "dw iplay_text_screen_bytes(const IplayTextScreen *screen);" in header
    assert "int iplay_text_screen_set_video_mode_checked(IplayTextScreen *screen, db video_mode);" in header
    assert "iplay_text_screen_set_video_mode" in header
    assert "IplayNcPlane *iplay_text_screen_root(IplayTextScreen *screen);" in header
    assert "const IplayTextMode *iplay_text_screen_mode(const IplayTextScreen *screen);" in header
    assert "iplay_text_screen_bottom_layout" in header
    assert "iplay_text_screen_bottom_layout_fits" in header
    assert "iplay_text_screen_draw_top_title" in header
    assert "iplay_text_screen_draw_bottom" in header
    assert "iplay_text_screen_draw_audio_output_levels" in header
    assert "iplay_txt_draw_top_title_plane(iplay_text_screen_root(screen));" in rewrite
    assert "iplay_txt_draw_bottom_plane(iplay_text_screen_root(screen), iplay_text_screen_bottom_layout(screen)" in rewrite
    assert "iplay_audio_output_draw_levels_yx(iplay_text_screen_root(screen), y, x, output" in rewrite
    assert "iplay_txt_draw_top_title_plane(&screen->root);" not in rewrite
    assert "iplay_txt_draw_bottom_plane(&screen->root" not in rewrite
    assert "iplay_audio_output_draw_levels_yx(&screen->root" not in rewrite
    assert "void iplay_text_screen_set_cells(IplayTextScreen *screen, db *cells)" in rewrite
    assert "void iplay_text_screen_set_capacity(IplayTextScreen *screen, dw capacity_bytes)" in rewrite
    assert "void iplay_text_screen_set_mode(IplayTextScreen *screen, const IplayTextMode *mode)" in rewrite
    assert "void iplay_text_screen_reinit_root(IplayTextScreen *screen)" in rewrite
    assert "iplay_ncplane_init_mode(iplay_text_screen_root(screen), iplay_text_screen_cells(screen), iplay_text_screen_mode(screen));" in rewrite
    assert "iplay_text_screen_set_cells(screen, cells);" in rewrite
    assert "iplay_text_screen_set_capacity(screen, capacity_bytes);" in rewrite
    assert rewrite.count("iplay_text_screen_set_mode(screen, mode);") == 2
    assert rewrite.count("iplay_text_screen_reinit_root(screen);") == 2
    assert "#define iplay_text_screen_set_cells_field(state, value) ((state)->cells = (value))" in rewrite
    assert "#define iplay_text_screen_set_capacity_field(state, value) ((state)->capacity_bytes = (value))" in rewrite
    assert "#define iplay_text_screen_set_mode_field(state, value) ((state)->mode = *(value))" in rewrite
    assert "#define iplay_text_screen_capacity_field(state) ((state)->capacity_bytes)" in rewrite
    assert "#define iplay_text_screen_cells_field(state) ((state)->cells)" in rewrite
    assert "#define iplay_text_screen_cells_const_field(state) ((state)->cells)" in rewrite
    assert "#define iplay_text_screen_root_field(state) (&(state)->root)" in rewrite
    assert "#define iplay_text_screen_mode_field(state) (&(state)->mode)" in rewrite
    assert "static void iplay_text_screen_set_cells_field(IplayTextScreen *state, db *cells)" not in rewrite
    assert "static void iplay_text_screen_set_capacity_field(IplayTextScreen *state, dw capacity_bytes)" not in rewrite
    assert "static void iplay_text_screen_set_mode_field(IplayTextScreen *state, const IplayTextMode *mode)" not in rewrite
    assert "static dw iplay_text_screen_capacity_field(const IplayTextScreen *state)" not in rewrite
    assert "static db *iplay_text_screen_cells_field(IplayTextScreen *state)" not in rewrite
    assert "static const db *iplay_text_screen_cells_const_field(const IplayTextScreen *state)" not in rewrite
    assert "static IplayNcPlane *iplay_text_screen_root_field(IplayTextScreen *state)" not in rewrite
    assert "static const IplayTextMode *iplay_text_screen_mode_field(const IplayTextScreen *state)" not in rewrite
    assert "iplay_text_screen_set_cells_field(screen, cells);" in rewrite
    assert "iplay_text_screen_set_capacity_field(screen, capacity_bytes);" in rewrite
    assert "iplay_text_screen_set_mode_field(screen, mode);" in rewrite
    assert "return iplay_text_screen_capacity_field(screen);" in rewrite
    assert "return iplay_text_screen_cells_field(screen);" in rewrite
    assert "return iplay_text_screen_cells_const_field(screen);" in rewrite
    assert "return iplay_text_screen_root_field(screen);" in rewrite
    assert "return iplay_text_screen_mode_field(screen);" in rewrite
    assert "screen->cells = cells;" not in rewrite
    assert "screen->capacity_bytes = capacity_bytes;" not in rewrite
    assert "screen->mode = *mode;" not in rewrite
    assert "return screen->capacity_bytes;" not in rewrite
    assert "return screen->cells;" not in rewrite
    assert "return &screen->root;" not in rewrite
    assert "return &screen->mode;" not in rewrite
    assert "iplay_ncplane_init_mode(&screen->root, cells, &screen->mode);" not in rewrite
    assert "iplay_ncplane_init_mode(&screen->root, screen->cells, &screen->mode);" not in rewrite
    assert "db *iplay_text_screen_cells(IplayTextScreen *screen)" in rewrite
    assert "const db *iplay_text_screen_cells_const(const IplayTextScreen *screen)" in rewrite
    assert "dw iplay_text_screen_bytes(const IplayTextScreen *screen)" in rewrite
    assert "return iplay_text_mode_screen_bytes(iplay_text_screen_mode(screen));" in rewrite
    assert "int iplay_text_mode_fits_capacity(const IplayTextMode *mode, dw capacity_bytes)" in rewrite
    assert "int iplay_text_size_is_supported(dw cols, dw rows)" in rewrite
    assert "return iplay_text_mode_for_size(cols, rows) != 0;" in rewrite
    assert "int iplay_text_mode_is_supported(const IplayTextMode *mode)" in rewrite
    assert "void iplay_text_screen_resize_to_size(IplayTextScreen *screen, dw cols, dw rows)" in rewrite
    assert "(void)iplay_text_screen_resize_to_size_checked(screen, cols, rows);" in rewrite
    assert "int iplay_text_screen_resize_to_size_checked(IplayTextScreen *screen, dw cols, dw rows)" in rewrite
    assert "return iplay_text_screen_resize_checked(screen, mode);" in rewrite
    assert "rewrite/text_wrapper_runner.c" in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert 'streq(argv[1], "textcelldigest")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimetextdigest")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimepresentdigest")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "capture->checksum = iplay_text_cells_checksum(cells, byte_count);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "capture->nonblank = iplay_text_cells_nonblank_count(cells, byte_count);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_text_cells_checksum(mem, bytes)" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_text_screen_checksum(&screen)" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_text_cells_nonblank_count(mem, bytes)" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_text_screen_nonblank_count(&screen)" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_runtime_video_checksum(&runtime)" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_runtime_video_nonblank_cells(&runtime)" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "cb_checksum=%lu" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "cb_nonblank=%u" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'translated("textcelldigest")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'translated("runtimetextdigest")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'translated("runtimepresentdigest")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "from typing import Optional, Union" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "TextCells = Union[bytes, bytearray]" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "MODULE_LOADED_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "MODULE_SIZE_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "MODULE_LOADER_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "DECODER_HANDOFF_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "MODULE_TYPE_TAG_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "MODULE_TITLE_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "UNSUPPORTED_MODULE_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "PLAYBACK_OUTPUT_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "PLAYBACK_DISABLED_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "FFI_MARKER_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "ORDERS_CHANNELS_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'HELP_USAGE_TEXT = "Usage: IPLAY [Switches] [FileName.Ext|@FileList.Ext]"' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def comparable_help_lines(output: str) -> list[str]:" in (ROOT / "tests" / "test_player_behavior.py").read_text()
    assert "def test_iplayc_dos_help_preserves_original_comparable_usage_lines(tmp_path: Path) -> None:" in (ROOT / "tests" / "test_player_behavior.py").read_text()
    assert "assert comparable_help_lines(original_out) == comparable_help_lines(rewrite_out)" in (ROOT / "tests" / "test_player_behavior.py").read_text()
    assert 'SUPPORTED_DOS_FORMATS_TEXT = "Supported by this DOS hardware build: MOD NST S3M STM 669 MTM PSM FAR ULT WOW OKT OCT XM IT PTM AMS DBM DMF MDL DSM MED IMF J2B"' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'DEFERRED_PROJECT_OWNED_FORMATS = {"INR"}' in (ROOT / "tests" / "test_player_behavior.py").read_text()
    assert 'SB16_AUDIO_SCOPE_TEXT = "Audio driver scope: SB16 16-bit stereo only."' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'TEXT_BACKEND_MEMORY_TEXT = "Text backend: VGA color/BW text memory at B800:0000/B000:0000."' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'SDL_COMPAT_AUDIO_BACKEND_TEXT = "Audio backend: SB16 16-bit stereo hardware wrapper, SDL-compatible callback boundary."' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def text_cell_checksum(cells: TextCells, byte_count: Optional[int] = None) -> int:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def text_cell_nonblank_count(cells: TextCells, byte_count: Optional[int] = None) -> int:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def text_cell_digest(cells: TextCells, byte_count: Optional[int] = None) -> dict[str, int]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def text_memory_slice(memory: bytes, segment: int, cols: int, rows: int, offset: int = VGA_TEXT_OFFSET) -> bytes:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def text_memory_digest(memory: bytes, segment: int, cols: int, rows: int, offset: int = VGA_TEXT_OFFSET) -> dict[str, int]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "SCREEN_PRESENT_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "PLAYER_HW_TEXT_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "PLAYER_HW_AUDIO_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "PLAYBACK_PUMP_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "PLAYBACK_LOOP_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "DECODER_PROGRESS_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "DECODER_GEOMETRY_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "DECODER_EVENT_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "DECODER_VOICE_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "DECODER_ROUTE_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "PCM_SOURCE_RE = re.compile(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_screen_present_digest(output: str, reason: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"scope": match.group("scope")' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_module_loaded(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'return {"name": match.group("name")}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_module_size(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'return {"size": int(match.group("size"))}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_module_loader(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'return {"loader": match.group("loader")}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_decoder_handoff(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'return {"handoff": match.group("handoff")}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_module_type_tag(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'return {"tag": match.group("tag")}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_module_title(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'return {"title": match.group("title")}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_unsupported_module(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'return {"name": match.group("name")}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_playback_output(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'return {"output": match.group("output")}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_playback_disabled(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'return {"reason": match.group("reason")}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_ffi_marker(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'return {"marker": match.group("marker")}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_orders_channels(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"channels": int(match.group("channels"))' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_player_hw_text_digest(output: str) -> dict[str, int]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"segment": int(match.group("segment"), 16)' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_player_hw_audio_digest(output: str) -> dict[str, int]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"first": int(match.group("first"), 16)' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_playback_pump(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"stop": match.group("stop")' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_playback_loop(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"frames_per_block": int(match.group("frames_per_block"))' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_decoder_progress(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"total_blocks": int(match.group("total_blocks"))' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_decoder_geometry(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"rows_per_order": int(match.group("rows_per_order"))' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_decoder_events(output: str) -> list[dict[str, object]]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"effect": int(match.group("effect"))' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_decoder_voices(output: str) -> list[dict[str, object]]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"loop_len": int(match.group("loop_len"))' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_decoder_route(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"id": int(match.group("id"))' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def parse_pcm_source(output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"renderer": match.group("renderer")' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_decoder_route(output: str, route_id: int, name: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert route == {"id": route_id, "name": name}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_decoder_route_absent(output: str, route_id: int, name: str) -> None:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert f"Decoder route: id={route_id} name={name}" not in output' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_module_loaded(output: str, name: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert module == {"name": name}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_module_not_loaded(output: str, name: str) -> None:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert f"Module: {name}" not in output' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_module_size(output: str, size: int) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert module_size == {"size": size}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_module_loader(output: str, loader: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert module_loader == {"loader": loader}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_decoder_handoff(output: str, handoff: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert decoder_handoff == {"handoff": handoff}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_decoder_handoff_absent(output: str, handoff: str) -> None:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert f"Decoder handoff: {handoff}" not in output' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_module_type_tag(output: str, tag: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert module_type_tag == {"tag": tag}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_module_title(output: str, title: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert module_title == {"title": title}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_unsupported_module(output: str, name: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert unsupported == {"name": name}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_playback_output(output: str, expected_output: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert playback_output == {"output": expected_output}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_playback_disabled(output: str, reason: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert playback_disabled == {"reason": reason}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_ffi_marker(output: str, marker: str) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert ffi_marker == {"marker": marker}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_orders_channels(output: str, orders: int, channels: int) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert summary == {"orders": orders, "channels": channels}' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_help_usage(output: str) -> None:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_supported_dos_formats(output: str) -> None:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_sb16_audio_scope(output: str) -> None:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_text_backend(output: str) -> None:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_text_backend_memory(output: str) -> None:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_sdl_compatible_audio_backend(output: str) -> None:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_playback_loop(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"frames_per_block": frames_per_block' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_decoder_progress(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"total_blocks": total_blocks' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_decoder_progress_block(output: str, block: int, total_blocks: int) -> dict[str, object]:" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert progress["total_blocks"] == total_blocks' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_decoder_geometry(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"channels": channels' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_decoder_event(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "for event in parse_decoder_events(output):" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_decoder_voice(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "for voice in parse_decoder_voices(output):" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    fixtures = (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_pcm_source_route(" in fixtures
    assert "truncated: Optional[int] = None" in fixtures
    assert "input_kind: Optional[str] = None" in fixtures
    assert "hook_provider: Optional[str] = None" in fixtures
    assert 'assert pcm["hook_provider"] == hook_provider' in fixtures
    assert "stream_start: Optional[int] = None" in fixtures
    assert 'assert pcm["route"] == route_id' in fixtures
    assert 'assert pcm["truncated"] == truncated' in fixtures
    assert 'assert pcm["input"] == input_kind' in fixtures
    assert 'assert pcm["stream_start"] == stream_start' in fixtures
    assert "def assert_text_memory_matches_screen_present(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'expected_scope: str = "full-screen"' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"scope": expected_scope' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "def assert_text_memory_matches_player_hw_text(" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "rewrite = parse_player_hw_text_digest(output)" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert 'assert actual == expected' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"checksum": int(match.group("checksum"))' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert '"nonblank": int(match.group("nonblank"))' in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "return segment * 16 + offset" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "VGA_COLOR_TEXT_SEG = 0xB800" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "VGA_MONO_TEXT_SEG = 0xB000" in (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "text_memory_digest," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_decoder_route," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_decoder_route_absent," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_decoder_handoff_absent," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_module_not_loaded," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_pcm_source_route," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_decoder_route," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_decoder_geometry," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_decoder_events," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_decoder_voices," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_decoder_progress," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_pcm_source," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_module_loaded," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_module_loader," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_module_size," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_module_type_tag," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_module_title," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_unsupported_module," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_playback_output," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_playback_disabled," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_ffi_marker," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_orders_channels," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_decoder_handoff," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_screen_present_digest," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_playback_pump_sb16_stereo," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_playback_pump_stop_state," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_screen_present_content," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_module_loaded," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_module_loader," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_module_size," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_module_type_tag," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_module_title," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_unsupported_module," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_playback_output," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_playback_disabled," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_ffi_marker," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_orders_channels," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_help_usage," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_supported_dos_formats," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_sb16_audio_scope," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_text_backend," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_text_backend_memory," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_sdl_compatible_audio_backend," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_decoder_handoff," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_text_screen_geometry," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_player_hw_audio_digest," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_player_hw_text_digest," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_playback_loop," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "parse_playback_pump," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_playback_loop," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_decoder_progress," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_decoder_progress_block," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_decoder_geometry," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_decoder_event," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_decoder_voice," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_text_memory_matches_screen_present," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "assert_text_memory_matches_player_hw_text," in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_text_cell_digest_matches_b800_cell_semantics' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_text_memory_digest_extracts_real_mode_vga_aperture' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_screen_present_digest_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_screen_present_content_helper_accepts_status_only_full_screen_copy' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert_screen_present_content(digest, "status-only", expected_audio_frames=16384)' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_screen_present_content_helper_rejects_stale_audio_frame_counter' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("stale audio-frame counter was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_text_screen_geometry_helper_accepts_expected_mode_bytes' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert_text_screen_geometry(digest, 80, 50)' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_text_screen_geometry_helper_rejects_byte_count_mismatch' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("text screen byte-count mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_pump_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_route_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert parse_decoder_route(output) == {"id": 0, "name": "external-library"}' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_route_parser_rejects_missing_diagnostic' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("missing decoder route diagnostic was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_pcm_source_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert '"provider": "native-preview"' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_pcm_source_parser_rejects_missing_diagnostic' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("missing PCM source diagnostic was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_route_assertion_helper_accepts_expected_route' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert assert_decoder_route(output, 1, "project-owned") == {"id": 1, "name": "project-owned"}' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_route_assertion_helper_rejects_wrong_route_id' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("decoder route assertion mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_not_loaded_helper_rejects_present_module' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("module-not-loaded helper did not reject present module")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_route_absent_helper_rejects_present_route' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("decoder-route-absent helper did not reject present route")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_handoff_absent_helper_rejects_present_handoff' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("decoder-handoff-absent helper did not reject present handoff")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_pcm_source_route_assertion_helper_accepts_expected_source' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'pcm = assert_pcm_source_route(output, 1, "p", "native", source="inr_module", truncated=0, input_kind="memory", hook_provider="none", stream_start=0)' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_pcm_source_route_assertion_helper_rejects_wrong_provider' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_pcm_source_route_assertion_helper_rejects_wrong_input_kind' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_pcm_source_route_assertion_helper_rejects_wrong_hook_provider' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("PCM source hook-provider mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("PCM source input-kind mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("PCM source route assertion mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_sb16_stereo_block_accounting_helper_accepts_expected_bytes' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert_sb16_stereo_block_accounting(32, 32 * SB16_BOUNDED_BLOCK_FRAMES, 32 * SB16_BOUNDED_BLOCK_BYTES, SB16_BOUNDED_BLOCK_FRAMES)' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_sb16_stereo_block_accounting_helper_rejects_frame_count_mismatch' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("SB16 block frame-count mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_sb16_stereo_frame_byte_helper_rejects_unaligned_byte_count' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert_sb16_stereo_frame_bytes(1, 2)' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("SB16 unaligned stereo byte count was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_pump_sb16_helper_rejects_accepted_byte_mismatch' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("SB16 playback-pump accepted-byte mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_pump_sb16_helper_accepts_parsed_bounded_pump' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert_playback_pump_sb16_stereo(pump, 32, SB16_BOUNDED_BLOCK_FRAMES)' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_pump_sb16_helper_rejects_block_count_mismatch' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("SB16 playback-pump block-count mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_pump_stop_state_helper_accepts_source_end' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert_playback_pump_stop_state(pump, 0, 1, "source-end")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_pump_stop_state_helper_rejects_wrong_stop_reason' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("playback-pump stop reason mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_loop_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_loop_assertion_helper_rejects_wrong_policy' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("playback loop policy mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_progress_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_progress_assertion_helper_rejects_wrong_row' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("decoder progress row mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_progress_block_helper_accepts_prefix_check' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_loaded_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_loaded_assertion_helper_rejects_wrong_case' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("module-loaded case mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_size_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_size_assertion_helper_rejects_wrong_size' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("module-size mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_loader_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_loader_assertion_helper_rejects_wrong_loader' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("module-loader mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_handoff_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_handoff_assertion_helper_rejects_wrong_handoff' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("decoder-handoff mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_type_tag_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_type_tag_assertion_helper_rejects_wrong_tag' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("module-type-tag mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_title_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_module_title_assertion_helper_rejects_wrong_title' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("module-title mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_unsupported_module_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_unsupported_module_assertion_helper_rejects_wrong_name' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("unsupported-module name mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_output_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_output_assertion_helper_rejects_wrong_output' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("playback-output mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_disabled_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_playback_disabled_assertion_helper_rejects_wrong_reason' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("playback-disabled reason mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_ffi_marker_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_ffi_marker_assertion_helper_rejects_wrong_marker' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("FFI marker mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_orders_channels_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_orders_channels_assertion_helper_rejects_wrong_channel_count' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("orders/channels mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_help_and_capability_helpers_accept_expected_text' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_help_usage_helper_rejects_missing_usage' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("missing help usage text was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_geometry_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_geometry_assertion_helper_rejects_wrong_channel_count' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("decoder geometry channel-count mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_event_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_event_assertion_helper_rejects_wrong_effect' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("decoder event effect mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_voice_parser_matches_rewrite_diagnostic_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_decoder_voice_assertion_helper_rejects_wrong_loop_length' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("decoder voice loop-length mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_text_memory_to_screen_present_comparison_helper_matches_digest_fields' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'expected_scope="status-only"' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_text_memory_to_screen_present_comparison_helper_rejects_digest_mismatch' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("digest mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_text_memory_to_screen_present_comparison_helper_rejects_scope_mismatch' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("screen-present scope mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_player_hw_text_digest_parser_matches_runner_output_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_player_hw_audio_digest_parser_matches_runner_output_shape' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_text_memory_to_player_hw_text_comparison_helper_matches_digest_fields' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'test_python_text_memory_to_player_hw_text_comparison_helper_rejects_digest_mismatch' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'raise AssertionError("player hardware text digest mismatch was not rejected")' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'text_digest = parse_player_hw_text_digest(got)' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'audio_digest = parse_player_hw_audio_digest(got)' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert audio_digest["checksum"] == 7456' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert text_digest["segment"] == VGA_COLOR_TEXT_SEG' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert text_digest["segment"] == VGA_MONO_TEXT_SEG' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert (ROOT / "tests" / "test_function_parity.py").read_text().count('text_digest = parse_player_hw_text_digest(got)') >= 6
    assert '[ "$1" = "textcelldigest" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimetextdigest" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimepresentdigest" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert 'streq(argv[1], "textscreenresizebad")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_text_screen_resize_to_size_checked(&screen, 132, 43)" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textscreenresizecapacity")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_text_screen_init_capacity(&screen, mem, iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X25), &IPLAY_TEXT_MODE_40X25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_text_screen_can_resize(&screen, &IPLAY_TEXT_MODE_80X50)" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textscreenresize80x25")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_text_screen_resize_to_size_checked(&screen, 80, 25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textscreenresize80x50")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_text_screen_resize_to_size_checked(&screen, 80, 50);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textscreenresizecycle")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "narrow_ok = iplay_text_screen_resize_to_size_checked(&screen, 40, 25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textsubplaneclip")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_ncplane_subplane(&child, &root, 23, 38, 4, 5);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textsubplanezeroedge")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_ncplane_subplane(&child, &root, 25, 40, 3, 4);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textsubplane80x50zeroedge")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_ncplane_subplane(&child, root, 50, 80, 3, 4);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textpresent80x50")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_notcurses_set_present_callback(&nc, capture_text_present, &capture);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X50_PROJECT);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textpresent80x25bw")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X25_BW);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textresize80x25present")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_notcurses_resize_to_size_checked(&nc, 80, 25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textmodecyclepresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "wide_ok = iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X25_COLOR);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "narrow_ok = iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_40X25_COLOR);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textpresentclear")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "no_cb_has = iplay_notcurses_has_present(&nc);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_notcurses_clear_present_callback(&nc);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textpresentreplace")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_notcurses_set_present_callback(&nc, capture_text_present, &second);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textbadmodepresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_notcurses_set_video_mode_checked(&nc, 0x99);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textresizecapacitypresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_notcurses_init_vga_memory_capacity(&nc, mem, iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X25), &IPLAY_TEXT_MODE_40X25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_notcurses_resize_to_size_checked(&nc, 80, 50);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "terminalresizecapacitypresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_terminal_init_vga_memory_capacity(&terminal, mem, iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X25), &IPLAY_TEXT_MODE_40X25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_terminal_resize_to_size_checked(&terminal, 80, 50);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "terminalresize80x25present")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_terminal_init_vga_memory_capacity(&terminal, mem, sizeof(mem), &IPLAY_TEXT_MODE_40X25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_terminal_resize_to_size_checked(&terminal, 80, 25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "terminalpresent80x25bw")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_terminal_set_video_mode_checked(&terminal, IPLAY_VIDEO_MODE_80X25_BW);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "terminalpresent80x25color")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_terminal_set_video_mode_checked(&terminal, IPLAY_VIDEO_MODE_80X25_COLOR);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "terminalbadmodepresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_terminal_set_video_mode_checked(&terminal, 0x99);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "terminalpresentclear")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_terminal_clear_present_callback(&terminal);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "terminalpresentreplace")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_terminal_set_present_callback(&terminal, capture_text_present, &second);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "terminalresize80x50present")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_terminal_resize_to_size_checked(&terminal, 80, 50);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "terminalresizecyclepresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "narrow_ok = iplay_terminal_resize_to_size_checked(&terminal, 40, 25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textsubwindowpresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_init_subwindow(&child, &root, 3, 5, 4, 16);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_draw_status_field(&child, 1, \"Song\", \"DEMO\", 0x2a, 0x4c);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textsubwindowredraw")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_init_subwindow(&child, &root, 6, 7, 3, 16);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_erase(&child, 0x03);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_draw_status_line(&child, 0, \"NEW\", 0x5a);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textsubwindowzeroedge")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_init_subwindow(&child, &root, 25, 40, 3, 4);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textsubwindow80x50clip")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_notcurses_set_video_mode_checked(&nc, IPLAY_VIDEO_MODE_80X50_PROJECT);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_init_subwindow(&child, &root, 48, 78, 4, 5);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textsubwindow80x50zeroedge")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_init_subwindow(&child, &root, 50, 80, 3, 4);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textcursorresize")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_ncplane_resize(&child, 2, 3);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textcursorresizezero")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_ncplane_resize(&child, 0, 0);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textcursor80x50resizezero")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_ncplane_subplane(&child, root, 48, 78, 2, 2);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "textcolorattrs16")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_ncplane_putc_yx(root, 2, i, (db)('A' + i), (db)((i << 4) | i));" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "print_bytes(mem + IPLAY_TEXT_OFFSET(IPLAY_TEXT_COLS_40, 2, 0), 32);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimepresentclear")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_runtime_video_set_present_callback(&runtime, capture_text_present, &second);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_runtime_video_clear_present_callback(&runtime);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimebadmodepresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_runtime_set_video_mode_checked(&runtime, 0x99);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_runtime_video_status_token(&runtime)" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeresizebadpresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_runtime_resize_to_size_checked(&runtime, 132, 43);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeresizecapacitypresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_runtime_config_sdl_capacity(&config, mem, iplay_text_mode_screen_bytes(&IPLAY_TEXT_MODE_80X25), &IPLAY_TEXT_MODE_40X25, capture_text_present, &capture, 0, 0);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_runtime_video_capacity(&runtime)" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimepresent80x25bw")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_runtime_set_video_mode_checked(&runtime, IPLAY_VIDEO_MODE_80X25_BW);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimepresent80x25color")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_runtime_set_video_mode_checked(&runtime, IPLAY_VIDEO_MODE_80X25_COLOR);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeresize80x50present")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 50);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeresize80x25present")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "ok = iplay_runtime_resize_to_size_checked(&runtime, 80, 25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeresizecyclepresent")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "narrow_ok = iplay_runtime_resize_to_size_checked(&runtime, 40, 25);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimecursor80x50resizezero")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_ncplane_putc_yx(root, 49, 79, 'M', 0x6d);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimesubwindow80x50clip")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_init_subwindow(&child, &root_window, 48, 78, 4, 5);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimesubwindowresizecycleclip")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_init_subwindow(&child, &root_window, 23, 38, 4, 5);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimesubwindowzeroedge")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_init_subwindow(&child, &root_window, 25, 40, 3, 4);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimesubwindow80x50zeroedge")' in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert "iplay_window_init_subwindow(&child, &root_window, 50, 80, 3, 4);" in (ROOT / "rewrite" / "text_wrapper_runner.c").read_text()
    assert 'streq(op, "textscreenresizebad")' not in runner
    assert 'streq(op, "textscreenresizecapacity")' not in runner
    assert 'streq(op, "textscreenresize80x25")' not in runner
    assert 'streq(op, "textscreenresize80x50")' not in runner
    assert 'streq(op, "textscreenresizecycle")' not in runner
    assert 'streq(op, "textsubplaneclip")' not in runner
    assert 'streq(op, "textsubplanezeroedge")' not in runner
    assert 'streq(op, "textsubplane80x50zeroedge")' not in runner
    assert 'streq(op, "textpresent80x50")' not in runner
    assert 'streq(op, "textpresent80x25bw")' not in runner
    assert 'streq(op, "textresize80x25present")' not in runner
    assert 'streq(op, "textmodecyclepresent")' not in runner
    assert 'streq(op, "textpresentclear")' not in runner
    assert 'streq(op, "textpresentreplace")' not in runner
    assert 'streq(op, "textbadmodepresent")' not in runner
    assert 'streq(op, "textresizecapacitypresent")' not in runner
    assert 'streq(op, "terminalresizecapacitypresent")' not in runner
    assert 'streq(op, "terminalresize80x25present")' not in runner
    assert 'streq(op, "terminalpresent80x25bw")' not in runner
    assert 'streq(op, "terminalpresent80x25color")' not in runner
    assert 'streq(op, "terminalbadmodepresent")' not in runner
    assert 'streq(op, "terminalpresentclear")' not in runner
    assert 'streq(op, "terminalpresentreplace")' not in runner
    assert 'streq(op, "terminalresize80x50present")' not in runner
    assert 'streq(op, "terminalresizecyclepresent")' not in runner
    assert 'streq(op, "textsubwindowpresent")' not in runner
    assert 'streq(op, "textsubwindowredraw")' not in runner
    assert 'streq(op, "textsubwindowzeroedge")' not in runner
    assert 'streq(op, "textsubwindow80x50clip")' not in runner
    assert 'streq(op, "textsubwindow80x50zeroedge")' not in runner
    assert 'streq(op, "textcursorresize")' not in runner
    assert 'streq(op, "textcursor80x50resizezero")' not in runner
    assert 'streq(op, "textcolorattrs16")' not in runner
    assert 'streq(op, "runtimepresentclear")' not in runner
    assert 'streq(op, "runtimebadmodepresent")' not in runner
    assert 'streq(op, "runtimeresizebadpresent")' not in runner
    assert 'streq(op, "runtimeresizecapacitypresent")' not in runner
    assert 'streq(op, "runtimepresent80x25bw")' not in runner
    assert 'streq(op, "runtimepresent80x25color")' not in runner
    assert 'streq(op, "runtimeresize80x50present")' not in runner
    assert 'streq(op, "runtimeresize80x25present")' not in runner
    assert 'streq(op, "runtimeresizecyclepresent")' not in runner
    assert 'streq(op, "runtimecursor80x50resizezero")' not in runner
    assert 'streq(op, "runtimesubwindow80x50clip")' not in runner
    assert 'streq(op, "runtimesubwindowresizecycleclip")' not in runner
    assert 'streq(op, "runtimesubwindowzeroedge")' not in runner
    assert 'streq(op, "runtimesubwindow80x50zeroedge")' not in runner
    assert "return mode != 0 && iplay_text_size_is_supported(iplay_text_mode_cols(mode), iplay_text_mode_rows(mode));" in rewrite
    assert "return mode != 0 && iplay_text_mode_for_size(iplay_text_mode_cols(mode), iplay_text_mode_rows(mode)) != 0;" not in rewrite
    assert "return iplay_text_mode_is_supported(mode) && iplay_text_mode_screen_bytes(mode) <= capacity_bytes;" in rewrite
    assert "return mode != 0 && iplay_text_mode_screen_bytes(mode) <= capacity_bytes;" not in rewrite
    assert "return iplay_text_mode_fits_capacity(mode, iplay_text_screen_capacity(screen));" in rewrite
    assert "return iplay_text_mode_screen_bytes(mode) <= iplay_text_screen_capacity(screen);" not in rewrite
    assert "return iplay_bottom_layout_for_mode(iplay_text_screen_mode(screen));" in rewrite
    assert "return iplay_bottom_layout_fits(iplay_text_screen_bottom_layout(screen), iplay_text_screen_mode(screen));" in rewrite
    assert "static int iplay_bottom_layout_rows_fit(const IplayBottomLayout *layout, dw rows)" in rewrite
    assert "static int iplay_bottom_layout_cols_fit(const IplayBottomLayout *layout, dw cols)" in rewrite
    assert "static int iplay_bottom_layout_rows_fit_raw(const IplayBottomLayout *layout, dw rows)" not in rewrite
    assert "static int iplay_bottom_layout_cols_fit_raw(const IplayBottomLayout *layout, dw cols)" not in rewrite
    assert "dw rows = iplay_text_mode_rows(mode);" in rewrite
    assert "dw cols = iplay_text_mode_cols(mode);" in rewrite
    assert "if (iplay_bottom_layout_module_y_field(layout) >= rows) return 0;" in rewrite
    assert "if (iplay_bottom_layout_flag_x_field(layout) >= cols) return 0;" in rewrite
    assert "if ((dw)(iplay_bottom_layout_left_x_field(layout) + iplay_bottom_layout_module_width_field(layout)) > cols) return 0;" in rewrite
    assert "return iplay_bottom_layout_rows_fit(layout, rows) && iplay_bottom_layout_cols_fit(layout, cols);" in rewrite
    assert "return iplay_text_screen_cells(iplay_terminal_screen(terminal));" in rewrite
    assert "return iplay_text_screen_cells_const(iplay_terminal_screen_const(terminal));" in rewrite
    assert "return iplay_terminal_screen(terminal)->cells;" not in rewrite
    assert "return iplay_text_mode_screen_bytes(mode) <= screen->capacity_bytes;" not in rewrite
    assert "return iplay_bottom_layout_for_mode(&screen->mode);" not in rewrite
    assert "return iplay_bottom_layout_fits(iplay_text_screen_bottom_layout(screen), &screen->mode);" not in rewrite
    assert "if (layout->module_y >= mode->rows) return 0;" not in rewrite
    assert "if (layout->flag_x >= mode->cols) return 0;" not in rewrite
    assert "if ((dw)(layout->left_x + layout->module_width) > mode->cols) return 0;" not in rewrite
    assert "if (layout->module_y >= rows) return 0;" not in rewrite
    assert "if (layout->flag_x >= cols) return 0;" not in rewrite
    assert "if ((dw)(layout->left_x + layout->module_width) > cols) return 0;" not in rewrite
    assert "iplay_terminal_init_vga_memory" in header
    assert "iplay_terminal_init_vga_memory_capacity" in header
    assert "iplay_text_screen_init_capacity(iplay_terminal_screen(terminal), cells, capacity_bytes, mode);" in rewrite
    assert "iplay_text_screen_init_capacity(&terminal->screen, cells, capacity_bytes, mode);" not in rewrite
    assert "iplay_terminal_set_backend" in header
    assert "iplay_terminal_set_present_fn" in header
    assert "iplay_terminal_set_present_user" in header
    assert "iplay_terminal_set_present_callback" in header
    assert "iplay_terminal_clear_present_callback" in header
    assert "iplay_terminal_backend" in header
    assert "iplay_terminal_has_present" in header
    assert "iplay_terminal_screen" in header
    assert "iplay_terminal_screen_const" in header
    assert "iplay_terminal_cells" in header
    assert "iplay_terminal_cells_const" in header
    assert "iplay_terminal_present_callback" in header
    assert "iplay_terminal_present_user" in header
    assert "iplay_terminal_capacity" in header
    assert "iplay_terminal_bottom_layout_fits" in header
    assert "iplay_terminal_root" in header
    assert "iplay_terminal_mode" in header
    assert "iplay_terminal_resize" in header
    assert "iplay_terminal_resize_checked" in header
    assert "iplay_terminal_resize_to_size" in header
    assert "iplay_terminal_resize_to_size_checked" in header
    assert "iplay_terminal_set_video_mode" in header
    assert "iplay_terminal_set_video_mode_checked" in header
    assert "iplay_terminal_present" in header
    assert "#define iplay_terminal_set_backend_field(state, value) ((state)->backend = (value))" in rewrite
    assert "#define iplay_terminal_set_present_field(state, value) ((state)->present = (value))" in rewrite
    assert "#define iplay_terminal_set_present_user_field(state, value) ((state)->present_user = (value))" in rewrite
    assert "#define iplay_terminal_backend_field(state) ((state)->backend)" in rewrite
    assert "#define iplay_terminal_screen_field(state) (&(state)->screen)" in rewrite
    assert "#define iplay_terminal_screen_const_field(state) (&(state)->screen)" in rewrite
    assert "#define iplay_terminal_present_field(state) ((state)->present)" in rewrite
    assert "#define iplay_terminal_present_user_field(state) ((state)->present_user)" in rewrite
    assert "static void iplay_terminal_set_backend_field(IplayTerminal *state, IplayTerminalBackend backend)" not in rewrite
    assert "static void iplay_terminal_set_present_field(IplayTerminal *state, IplayVideoPresentFn present)" not in rewrite
    assert "static void iplay_terminal_set_present_user_field(IplayTerminal *state, void *user)" not in rewrite
    assert "static IplayTerminalBackend iplay_terminal_backend_field(const IplayTerminal *state)" not in rewrite
    assert "static IplayTextScreen *iplay_terminal_screen_field(IplayTerminal *state)" not in rewrite
    assert "static const IplayTextScreen *iplay_terminal_screen_const_field(const IplayTerminal *state)" not in rewrite
    assert "static IplayVideoPresentFn iplay_terminal_present_field(const IplayTerminal *state)" not in rewrite
    assert "static void *iplay_terminal_present_user_field(const IplayTerminal *state)" not in rewrite
    assert "iplay_terminal_set_backend_field(terminal, backend);" in rewrite
    assert "iplay_terminal_set_present_field(terminal, present);" in rewrite
    assert "iplay_terminal_set_present_user_field(terminal, user);" in rewrite
    assert "return iplay_terminal_backend_field(terminal);" in rewrite
    assert "return iplay_terminal_screen_field(terminal);" in rewrite
    assert "return iplay_terminal_screen_const_field(terminal);" in rewrite
    assert "return iplay_terminal_present_field(terminal);" in rewrite
    assert "return iplay_terminal_present_user_field(terminal);" in rewrite
    assert "const IplayTextMode *iplay_terminal_resize_to_size(IplayTerminal *terminal, dw cols, dw rows)" in rewrite
    assert "(void)iplay_terminal_resize_to_size_checked(terminal, cols, rows);" in rewrite
    assert "int iplay_terminal_resize_to_size_checked(IplayTerminal *terminal, dw cols, dw rows)" in rewrite
    assert "return iplay_terminal_resize_checked(terminal, mode);" in rewrite
    assert "terminal->backend = backend;" not in rewrite
    assert "terminal->present = present;" not in rewrite
    assert "terminal->present_user = user;" not in rewrite
    assert "return terminal->backend;" not in rewrite
    assert "return &terminal->screen;" not in rewrite
    assert "return terminal->present;" not in rewrite
    assert "return terminal->present_user;" not in rewrite
    assert "dw bytes = iplay_text_screen_bytes(iplay_terminal_screen_const(terminal));" in rewrite
    assert "present(iplay_terminal_present_user(terminal), iplay_terminal_cells_const(terminal), mode, bytes);" in rewrite
    assert "present(iplay_terminal_present_user(terminal), iplay_terminal_cells(terminal), mode, bytes);" not in rewrite
    assert "dw bytes = iplay_text_mode_screen_bytes(mode);" not in rewrite
    assert "iplay_terminal_erase" in header
    assert "iplay_terminal_draw_top_title" in header
    assert "iplay_terminal_draw_bottom" in header
    assert "iplay_terminal_draw_audio_output_levels" in header
    assert "iplay_video_spec_backend" in header
    assert "iplay_video_spec_mode" in header
    assert "iplay_video_spec_cols" in header
    assert "iplay_video_spec_rows" in header
    assert "iplay_video_spec_present_enabled" in header
    assert "iplay_notcurses_init_vga_memory" in header
    assert "iplay_notcurses_terminal" in header
    assert "iplay_notcurses_stdplane" in header
    assert "iplay_notcurses_mode" in header
    assert "iplay_notcurses_capacity" in header
    assert "iplay_notcurses_cols" in header
    assert "iplay_notcurses_rows" in header
    assert "iplay_notcurses_row_bytes" in header
    assert "iplay_notcurses_screen_bytes" in header
    assert "iplay_notcurses_bottom_layout_fits" in header
    assert "iplay_notcurses_video_spec" in header
    assert "iplay_notcurses_backend" in header
    assert "iplay_notcurses_present_enabled" in header
    assert "iplay_notcurses_has_present" in header
    assert "iplay_notcurses_present_callback" in header
    assert "iplay_notcurses_present_user" in header
    assert "iplay_notcurses_set_present_fn" in header
    assert "iplay_notcurses_set_present_user" in header
    assert "iplay_notcurses_set_present_callback" in header
    assert "iplay_notcurses_clear_present_callback" in header
    assert "iplay_notcurses_resize" in header
    assert "iplay_notcurses_resize_checked" in header
    assert "iplay_notcurses_resize_to_size" in header
    assert "iplay_notcurses_resize_to_size_checked" in header
    assert "iplay_notcurses_set_video_mode" in header
    assert "iplay_notcurses_set_video_mode_checked" in header
    assert "iplay_notcurses_render_static" in header
    assert "iplay_notcurses_render_bottom" in header
    assert "iplay_notcurses_draw_audio_output_levels" in header
    assert "iplay_notcurses_present" in header
    assert "iplay_terminal_init_vga_memory(iplay_notcurses_terminal(nc), cells, mode);" in rewrite
    assert "iplay_terminal_init_vga_memory_capacity(iplay_notcurses_terminal(nc), cells, capacity_bytes, mode);" in rewrite
    assert "return iplay_terminal_root(iplay_notcurses_terminal(nc));" in rewrite
    assert "return iplay_terminal_mode(iplay_notcurses_terminal_const(nc));" in rewrite
    assert "dw iplay_notcurses_capacity(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_terminal_capacity(iplay_notcurses_terminal_const(nc));" in rewrite
    assert "dw iplay_notcurses_cols(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_text_mode_cols(iplay_notcurses_mode(nc));" in rewrite
    assert "dw iplay_notcurses_rows(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_text_mode_rows(iplay_notcurses_mode(nc));" in rewrite
    assert "dw iplay_notcurses_row_bytes(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_text_mode_row_bytes(iplay_notcurses_mode(nc));" in rewrite
    assert "dw iplay_notcurses_screen_bytes(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_text_mode_screen_bytes(iplay_notcurses_mode(nc));" in rewrite
    assert "int iplay_notcurses_bottom_layout_fits(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_terminal_bottom_layout_fits(iplay_notcurses_terminal_const(nc));" in rewrite
    assert "IplayVideoSpec iplay_notcurses_video_spec(const IplayNotcurses *nc)" in rewrite
    assert "#define iplay_video_spec_backend_field(state) ((state)->backend)" in rewrite
    assert "#define iplay_video_spec_set_backend_field(state, value) ((state)->backend = (value))" in rewrite
    assert "#define iplay_video_spec_mode_field(state) (&(state)->mode)" in rewrite
    assert "#define iplay_video_spec_set_mode_field(state, value) ((state)->mode = *(value))" in rewrite
    assert "#define iplay_video_spec_present_enabled_field(state) ((state)->present_enabled)" in rewrite
    assert "#define iplay_video_spec_set_present_enabled_field(state, value) ((state)->present_enabled = (value))" in rewrite
    assert "static IplayTerminalBackend iplay_video_spec_backend_field(const IplayVideoSpec *state)" not in rewrite
    assert "static const IplayTextMode *iplay_video_spec_mode_field(const IplayVideoSpec *state)" not in rewrite
    assert "static db iplay_video_spec_present_enabled_field(const IplayVideoSpec *state)" not in rewrite
    assert "IplayTerminalBackend iplay_video_spec_backend(const IplayVideoSpec *spec)" in rewrite
    assert "return iplay_video_spec_backend_field(spec);" in rewrite
    assert "const IplayTextMode *iplay_video_spec_mode(const IplayVideoSpec *spec)" in rewrite
    assert "return iplay_video_spec_mode_field(spec);" in rewrite
    assert "dw iplay_video_spec_cols(const IplayVideoSpec *spec)" in rewrite
    assert "return iplay_text_mode_cols(iplay_video_spec_mode(spec));" in rewrite
    assert "dw iplay_video_spec_rows(const IplayVideoSpec *spec)" in rewrite
    assert "return iplay_text_mode_rows(iplay_video_spec_mode(spec));" in rewrite
    assert "int iplay_video_spec_present_enabled(const IplayVideoSpec *spec)" in rewrite
    assert "return iplay_video_spec_present_enabled_field(spec) != 0;" in rewrite
    assert "const IplayTerminal *terminal = iplay_notcurses_terminal_const(nc);" in rewrite
    assert "iplay_video_spec_set_backend_field(&spec, iplay_terminal_backend(terminal));" in rewrite
    assert "iplay_video_spec_set_mode_field(&spec, iplay_terminal_mode(terminal));" in rewrite
    assert "iplay_video_spec_set_present_enabled_field(&spec, (db)iplay_terminal_has_present(terminal));" in rewrite
    assert "IplayTerminalBackend iplay_notcurses_backend(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_video_spec_backend(&spec);" in rewrite
    assert "int iplay_notcurses_present_enabled(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_video_spec_present_enabled(&spec);" in rewrite
    assert "int iplay_notcurses_has_present(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_terminal_has_present(iplay_notcurses_terminal_const(nc));" in rewrite
    assert "IplayVideoPresentFn iplay_notcurses_present_callback(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_terminal_present_callback(iplay_notcurses_terminal_const(nc));" in rewrite
    assert "void *iplay_notcurses_present_user(const IplayNotcurses *nc)" in rewrite
    assert "return iplay_terminal_present_user(iplay_notcurses_terminal_const(nc));" in rewrite
    assert "void iplay_notcurses_set_present_fn(IplayNotcurses *nc, IplayVideoPresentFn present)" in rewrite
    assert "iplay_terminal_set_present_fn(iplay_notcurses_terminal(nc), present);" in rewrite
    assert "void iplay_notcurses_set_present_user(IplayNotcurses *nc, void *user)" in rewrite
    assert "iplay_terminal_set_present_user(iplay_notcurses_terminal(nc), user);" in rewrite
    assert "void iplay_notcurses_set_present_callback(IplayNotcurses *nc, IplayVideoPresentFn present, void *user)" in rewrite
    assert "iplay_notcurses_set_present_fn(nc, present);" in rewrite
    assert "iplay_notcurses_set_present_user(nc, user);" in rewrite
    assert "void iplay_notcurses_clear_present_callback(IplayNotcurses *nc)" in rewrite
    assert "iplay_notcurses_set_present_callback(nc, 0, 0);" in rewrite
    assert "return iplay_terminal_resize_checked(iplay_notcurses_terminal(nc), mode);" in rewrite
    assert "const IplayTextMode *iplay_notcurses_resize_to_size(IplayNotcurses *nc, dw cols, dw rows)" in rewrite
    assert "(void)iplay_notcurses_resize_to_size_checked(nc, cols, rows);" in rewrite
    assert "int iplay_notcurses_resize_to_size_checked(IplayNotcurses *nc, dw cols, dw rows)" in rewrite
    assert "return iplay_notcurses_resize_checked(nc, mode);" in rewrite
    assert "return iplay_terminal_set_video_mode(iplay_notcurses_terminal(nc), video_mode);" in rewrite
    assert "return iplay_terminal_set_video_mode_checked(iplay_notcurses_terminal(nc), video_mode);" in rewrite
    assert "iplay_terminal_erase(iplay_notcurses_terminal(nc), erase_attr);" in rewrite
    assert "iplay_terminal_draw_top_title(iplay_notcurses_terminal(nc));" in rewrite
    assert "iplay_terminal_draw_bottom(iplay_notcurses_terminal(nc), byte_1de72" in rewrite
    assert "void iplay_notcurses_draw_audio_output_levels(IplayNotcurses *nc, dw y, dw x, const IplayAudioOutput *output, dw width, db fill_ch, db empty_ch, db left_attr, db right_attr, db empty_attr)" in rewrite
    assert "iplay_terminal_draw_audio_output_levels(iplay_notcurses_terminal(nc), y, x, output, width, fill_ch, empty_ch, left_attr, right_attr, empty_attr);" in rewrite
    assert "dw iplay_notcurses_present(IplayNotcurses *nc)" in rewrite
    assert "return iplay_terminal_present(iplay_notcurses_terminal(nc));" in rewrite
    assert "iplay_terminal_init_vga_memory(&nc->terminal" not in rewrite
    assert "iplay_terminal_init_vga_memory_capacity(&nc->terminal" not in rewrite
    assert "iplay_terminal_root(&nc->terminal)" not in rewrite
    assert "iplay_terminal_mode(&nc->terminal)" not in rewrite
    assert "iplay_terminal_resize_checked(&nc->terminal" not in rewrite
    assert "iplay_terminal_set_video_mode(&nc->terminal" not in rewrite
    assert "iplay_terminal_set_video_mode_checked(&nc->terminal" not in rewrite
    assert "iplay_terminal_erase(&nc->terminal" not in rewrite
    assert "iplay_terminal_draw_top_title(&nc->terminal" not in rewrite
    assert "iplay_terminal_draw_bottom(&nc->terminal" not in rewrite
    assert "#define iplay_notcurses_terminal_field(state) (&(state)->terminal)" in rewrite
    assert "#define iplay_notcurses_terminal_const_field(state) (&(state)->terminal)" in rewrite
    assert "#define iplay_notcurses_terminal_raw(state) (&(state)->terminal)" not in rewrite
    assert "#define iplay_notcurses_terminal_const_raw(state) (&(state)->terminal)" not in rewrite
    assert "static IplayTerminal *iplay_notcurses_terminal_raw(IplayNotcurses *state)" not in rewrite
    assert "static const IplayTerminal *iplay_notcurses_terminal_const_raw(const IplayNotcurses *state)" not in rewrite
    assert "return iplay_notcurses_terminal_field(nc);" in rewrite
    assert "return iplay_notcurses_terminal_const_field(nc);" in rewrite
    assert "return &nc->terminal;" not in rewrite
    assert "dw iplay_text_mode_row_bytes(const IplayTextMode *mode);" in header
    assert "dw iplay_text_mode_cells(const IplayTextMode *mode);" in header
    assert "dw iplay_text_mode_screen_bytes(const IplayTextMode *mode);" in header
    assert "int iplay_text_mode_fits_capacity(const IplayTextMode *mode, dw capacity_bytes);" in header
    assert "int iplay_text_mode_equals(const IplayTextMode *a, const IplayTextMode *b);" in header
    assert "int iplay_ncplane_visible_region" in header
    assert "db *iplay_ncplane_cells_at(db *cells, dw stride_cols, dw origin_y, dw origin_x);" in header
    assert "void iplay_ncplane_origin_yx(const IplayNcPlane *plane, dw *y, dw *x);" in header
    assert "dw iplay_ncplane_rows(const IplayNcPlane *plane);" in header
    assert "dw iplay_ncplane_cols(const IplayNcPlane *plane);" in header
    assert "dw iplay_ncplane_stride_cols(const IplayNcPlane *plane);" in header
    assert "db *iplay_ncplane_cells(const IplayNcPlane *plane);" in header
    assert "void iplay_ncplane_set_cells(IplayNcPlane *plane, db *cells);" in header
    assert "void iplay_ncplane_set_size(IplayNcPlane *plane, dw rows, dw cols);" in header
    assert "void iplay_ncplane_set_stride_cols(IplayNcPlane *plane, dw stride_cols);" in header
    assert "void iplay_ncplane_set_origin_yx(IplayNcPlane *plane, dw y, dw x);" in header
    assert "int iplay_ncplane_is_empty(const IplayNcPlane *plane);" in header
    assert "dw iplay_ncplane_cursor_y(const IplayNcPlane *plane);" in header
    assert "dw iplay_ncplane_cursor_x(const IplayNcPlane *plane);" in header
    assert "void iplay_ncplane_set_cursor_yx_raw(IplayNcPlane *plane, dw y, dw x);" in header
    assert "void iplay_ncplane_advance_cursor_x(IplayNcPlane *plane);" in header
    assert "dw iplay_ncplane_cell_offset(const IplayNcPlane *plane, dw y, dw x);" in header
    assert "db iplay_ncplane_cell_ch(const IplayNcPlane *plane, dw offset);" in header
    assert "db iplay_ncplane_cell_attr(const IplayNcPlane *plane, dw offset);" in header
    assert "dw iplay_ncplane_stride_cols(const IplayNcPlane *plane)" in rewrite
    assert "db *iplay_ncplane_cells(const IplayNcPlane *plane)" in rewrite
    assert "void iplay_ncplane_set_cells(IplayNcPlane *plane, db *cells)" in rewrite
    assert "void iplay_ncplane_set_size(IplayNcPlane *plane, dw rows, dw cols)" in rewrite
    assert "void iplay_ncplane_set_stride_cols(IplayNcPlane *plane, dw stride_cols)" in rewrite
    assert "void iplay_ncplane_set_origin_yx(IplayNcPlane *plane, dw y, dw x)" in rewrite
    assert "int iplay_ncplane_is_empty(const IplayNcPlane *plane)" in rewrite
    assert "dw iplay_ncplane_cursor_y(const IplayNcPlane *plane)" in rewrite
    assert "dw iplay_ncplane_cursor_x(const IplayNcPlane *plane)" in rewrite
    assert "void iplay_ncplane_set_cursor_yx_raw(IplayNcPlane *plane, dw y, dw x)" in rewrite
    assert "void iplay_ncplane_advance_cursor_x(IplayNcPlane *plane)" in rewrite
    assert "iplay_ncplane_set_cells(plane, iplay_ncplane_cells_at(cells, stride_cols, origin_y, origin_x));" in rewrite
    assert "iplay_ncplane_init(plane, cells, iplay_text_mode_rows(mode), iplay_text_mode_cols(mode));" in rewrite
    assert "if (y < iplay_ncplane_rows(parent)) max_rows = (dw)(iplay_ncplane_rows(parent) - y);" in rewrite
    assert "if (x < iplay_ncplane_cols(parent)) max_cols = (dw)(iplay_ncplane_cols(parent) - x);" in rewrite
    assert "iplay_ncplane_init_at(child, iplay_ncplane_cells(parent), rows, cols, y, x, iplay_ncplane_stride_cols(parent));" in rewrite
    assert "iplay_ncplane_origin_yx(parent, &parent_y, &parent_x);" in rewrite
    assert "iplay_ncplane_set_origin_yx(child, (dw)(parent_y + y), (dw)(parent_x + x));" in rewrite
    assert "iplay_ncplane_set_size(plane, rows, cols);" in rewrite
    assert "iplay_ncplane_set_stride_cols(plane, stride_cols);" in rewrite
    assert "iplay_ncplane_set_origin_yx(plane, origin_y, origin_x);" in rewrite
    assert "#define iplay_ncplane_origin_y_field(state) ((state)->origin_y)" in rewrite
    assert "#define iplay_ncplane_origin_x_field(state) ((state)->origin_x)" in rewrite
    assert "#define iplay_ncplane_rows_field(state) ((state)->rows)" in rewrite
    assert "#define iplay_ncplane_cols_field(state) ((state)->cols)" in rewrite
    assert "#define iplay_ncplane_stride_cols_field(state) ((state)->stride_cols)" in rewrite
    assert "#define iplay_ncplane_cells_field(state) ((state)->cells)" in rewrite
    assert "#define iplay_ncplane_set_cells_field(state, value) ((state)->cells = (value))" in rewrite
    assert "#define iplay_ncplane_set_size_field(state, value_rows, value_cols) ((state)->rows = (value_rows), (state)->cols = (value_cols))" in rewrite
    assert "#define iplay_ncplane_set_stride_cols_field(state, value) ((state)->stride_cols = (value))" in rewrite
    assert "#define iplay_ncplane_set_origin_yx_field(state, value_y, value_x) ((state)->origin_y = (value_y), (state)->origin_x = (value_x))" in rewrite
    assert "static dw iplay_ncplane_origin_y_field(const IplayNcPlane *state)" not in rewrite
    assert "static dw iplay_ncplane_origin_x_field(const IplayNcPlane *state)" not in rewrite
    assert "static dw iplay_ncplane_rows_field(const IplayNcPlane *state)" not in rewrite
    assert "static dw iplay_ncplane_cols_field(const IplayNcPlane *state)" not in rewrite
    assert "static dw iplay_ncplane_stride_cols_field(const IplayNcPlane *state)" not in rewrite
    assert "static db *iplay_ncplane_cells_field(const IplayNcPlane *state)" not in rewrite
    assert "static void iplay_ncplane_set_cells_field(IplayNcPlane *state, db *cells)" not in rewrite
    assert "static void iplay_ncplane_set_size_field(IplayNcPlane *state, dw rows, dw cols)" not in rewrite
    assert "static void iplay_ncplane_set_stride_cols_field(IplayNcPlane *state, dw stride_cols)" not in rewrite
    assert "static void iplay_ncplane_set_origin_yx_field(IplayNcPlane *state, dw y, dw x)" not in rewrite
    assert "*y = iplay_ncplane_origin_y_field(plane);" in rewrite
    assert "*x = iplay_ncplane_origin_x_field(plane);" in rewrite
    assert "return iplay_ncplane_rows_field(plane);" in rewrite
    assert "return iplay_ncplane_cols_field(plane);" in rewrite
    assert "return iplay_ncplane_stride_cols_field(plane);" in rewrite
    assert "return iplay_ncplane_cells_field(plane);" in rewrite
    assert "iplay_ncplane_set_cells_field(plane, cells);" in rewrite
    assert "iplay_ncplane_set_size_field(plane, rows, cols);" in rewrite
    assert "iplay_ncplane_set_stride_cols_field(plane, stride_cols);" in rewrite
    assert "iplay_ncplane_set_origin_yx_field(plane, y, x);" in rewrite
    assert "iplay_ncplane_set_cursor_yx_raw(plane, 0, 0);" in rewrite
    assert "dw cursor_y = iplay_ncplane_cursor_y(plane);" in rewrite
    assert "dw cursor_x = iplay_ncplane_cursor_x(plane);" in rewrite
    assert "if (cursor_y >= rows) cursor_y = rows ? (dw)(rows - 1u) : 0;" in rewrite
    assert "if (cursor_x >= cols) cursor_x = cols ? (dw)(cols - 1u) : 0;" in rewrite
    assert "iplay_ncplane_set_cursor_yx_raw(plane, cursor_y, cursor_x);" in rewrite
    assert "#define iplay_ncplane_cursor_y_field(state) ((state)->cursor_y)" in rewrite
    assert "#define iplay_ncplane_cursor_x_field(state) ((state)->cursor_x)" in rewrite
    assert "#define iplay_ncplane_set_cursor_y_field(state, value) ((state)->cursor_y = (value))" in rewrite
    assert "#define iplay_ncplane_set_cursor_x_field(state, value) ((state)->cursor_x = (value))" in rewrite
    assert "#define iplay_ncplane_advance_cursor_x_field(state) (++(state)->cursor_x)" in rewrite
    assert "#define iplay_ncplane_cell_byte_field(state, offset_value) ((state)->cells[(offset_value)])" in rewrite
    assert "#define iplay_ncplane_set_cell_byte_field(state, offset_value, value) ((state)->cells[(offset_value)] = (value))" in rewrite
    assert "static dw iplay_ncplane_cursor_y_field(const IplayNcPlane *state)" not in rewrite
    assert "static dw iplay_ncplane_cursor_x_field(const IplayNcPlane *state)" not in rewrite
    assert "static void iplay_ncplane_set_cursor_y_field(IplayNcPlane *state, dw y)" not in rewrite
    assert "static void iplay_ncplane_set_cursor_x_field(IplayNcPlane *state, dw x)" not in rewrite
    assert "static void iplay_ncplane_advance_cursor_x_field(IplayNcPlane *state)" not in rewrite
    assert "static db iplay_ncplane_cell_byte_field(const IplayNcPlane *state, dw offset)" not in rewrite
    assert "static void iplay_ncplane_set_cell_byte_field(IplayNcPlane *state, dw offset, db value)" not in rewrite
    assert "return iplay_ncplane_cursor_y_field(plane);" in rewrite
    assert "return iplay_ncplane_cursor_x_field(plane);" in rewrite
    assert "iplay_ncplane_set_cursor_y_field(plane, y);" in rewrite
    assert "iplay_ncplane_set_cursor_x_field(plane, x);" in rewrite
    assert "iplay_ncplane_advance_cursor_x_field(plane);" in rewrite
    assert "return iplay_ncplane_cell_byte_field(plane, offset);" in rewrite
    assert "return iplay_ncplane_cell_byte_field(plane, (dw)(offset + 1u));" in rewrite
    assert "iplay_ncplane_set_cell_byte_field(plane, offset, ch);" in rewrite
    assert "iplay_ncplane_set_cell_byte_field(plane, (dw)(offset + 1u), attr);" in rewrite
    assert "return (dw)((((dw)y * iplay_ncplane_stride_cols(plane)) + x) * IPLAY_TEXT_CELL_BYTES);" in rewrite
    assert "*y = iplay_ncplane_cursor_y(plane);" in rewrite
    assert "*x = iplay_ncplane_cursor_x(plane);" in rewrite
    assert "if (iplay_ncplane_is_empty(plane))" in rewrite
    assert "iplay_ncplane_set_cursor_yx_raw(plane, 0, 0);" in rewrite
    assert "iplay_ncplane_set_cursor_yx_raw(plane, y, x);" in rewrite
    assert "dw plane_rows = iplay_ncplane_rows(plane);" in rewrite
    assert "dw plane_cols = iplay_ncplane_cols(plane);" in rewrite
    assert "if (y >= iplay_ncplane_rows(plane) || x >= iplay_ncplane_cols(plane)) return;" in rewrite
    assert "iplay_ncplane_putc_yx(plane, iplay_ncplane_cursor_y(plane), iplay_ncplane_cursor_x(plane), ch, attr);" in rewrite
    assert "iplay_ncplane_advance_cursor_x(plane);" in rewrite
    assert "iplay_ncplane_fill_yx(plane, 0, 0, iplay_ncplane_rows(plane), iplay_ncplane_cols(plane), ' ', attr);" in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, y, 0, text, attr, iplay_ncplane_cols(plane));" in rewrite
    assert "put_screen_stream(title, iplay_ncplane_cells(plane), &si, 0, &di);" in rewrite
    assert "dw cols = iplay_ncplane_cols(plane);" in rewrite
    assert "while (*label && x < cols)" in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, y, x, value, value_attr, (dw)(cols - x));" in rewrite
    assert "return (dw)((((dw)y * plane->stride_cols) + x) * IPLAY_TEXT_CELL_BYTES);" not in rewrite
    assert "*y = plane->cursor_y;" not in rewrite
    assert "*x = plane->cursor_x;" not in rewrite
    assert "if (plane->rows == 0 || plane->cols == 0)" not in rewrite
    assert "if (y >= plane->rows || x >= plane->cols) return;" not in rewrite
    assert "iplay_ncplane_putc_yx(plane, plane->cursor_y, plane->cursor_x, ch, attr);" not in rewrite
    assert "iplay_ncplane_fill_yx(plane, 0, 0, plane->rows, plane->cols" not in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, y, 0, text, attr, plane->cols);" not in rewrite
    assert "while (*label && x < plane->cols)" not in rewrite
    assert "if (x < plane->cols)" not in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, y, x, value, value_attr, (dw)(plane->cols - x));" not in rewrite
    assert "put_screen_stream(title, plane->cells, &si, 0, &di);" not in rewrite
    assert "iplay_ncplane_init(plane, cells, mode->rows, mode->cols);" not in rewrite
    assert "plane->cells = iplay_ncplane_cells_at(cells, stride_cols, origin_y, origin_x);" not in rewrite
    assert "plane->rows = rows;\n    plane->cols = cols;\n    plane->stride_cols = stride_cols;" not in rewrite
    assert "plane->rows = rows;\n    plane->cols = cols;\n    if (plane->cursor_y >= rows)" not in rewrite
    assert "if (plane->cursor_y >= rows) plane->cursor_y = rows ? (dw)(rows - 1u) : 0;" not in rewrite
    assert "if (plane->cursor_x >= cols) plane->cursor_x = cols ? (dw)(cols - 1u) : 0;" not in rewrite
    assert "plane->origin_y = origin_y;" not in rewrite
    assert "plane->origin_x = origin_x;" not in rewrite
    assert "*y = plane->origin_y;" not in rewrite
    assert "*x = plane->origin_x;" not in rewrite
    assert "return plane->rows;" not in rewrite
    assert "return plane->cols;" not in rewrite
    assert "return plane->stride_cols;" not in rewrite
    assert "return plane->cells;" not in rewrite
    assert "plane->cells = cells;" not in rewrite
    assert "plane->rows = rows;" not in rewrite
    assert "plane->cols = cols;" not in rewrite
    assert "plane->stride_cols = stride_cols;" not in rewrite
    assert "plane->origin_y = y;" not in rewrite
    assert "plane->origin_x = x;" not in rewrite
    assert "plane->cursor_y = 0;" not in rewrite
    assert "plane->cursor_x = 0;" not in rewrite
    assert "return plane->cursor_y;" not in rewrite
    assert "return plane->cursor_x;" not in rewrite
    assert "plane->cursor_y = y;" not in rewrite
    assert "plane->cursor_x = x;" not in rewrite
    assert "++plane->cursor_x;" not in rewrite
    assert "return plane->cells[offset];" not in rewrite
    assert "return plane->cells[(dw)(offset + 1u)];" not in rewrite
    assert "plane->cells[offset] = ch;" not in rewrite
    assert "plane->cells[(dw)(offset + 1u)] = attr;" not in rewrite
    assert "if (y < parent->rows) max_rows = (dw)(parent->rows - y);" not in rewrite
    assert "if (x < parent->cols) max_cols = (dw)(parent->cols - x);" not in rewrite
    assert "iplay_ncplane_init_at(child, parent->cells, rows, cols, y, x, parent->stride_cols);" not in rewrite
    assert "child->origin_y = (dw)(parent->origin_y + y);" not in rewrite
    assert "child->origin_x = (dw)(parent->origin_x + x);" not in rewrite
    assert "void iplay_ncplane_put_cell_offset(IplayNcPlane *plane, dw offset, db ch, db attr);" in header
    assert "void iplay_ncplane_copy_cell_offset(IplayNcPlane *plane, dw dst_offset, dw src_offset);" in header
    assert "void iplay_ncplane_cursor_yx(const IplayNcPlane *plane, dw *y, dw *x);" in header
    assert "void iplay_ncplane_cursor_move_yx(IplayNcPlane *plane, dw y, dw x);" in header
    assert "void iplay_ncplane_putc(IplayNcPlane *plane, db ch, db attr);" in header
    assert "void iplay_ncplane_putstr(IplayNcPlane *plane, const char *text, db attr);" in header
    assert "void iplay_ncplane_putnstr(IplayNcPlane *plane, const char *text, db attr, dw width);" in header
    assert "void iplay_ncplane_putnstr_fill(IplayNcPlane *plane, const char *text, db attr, dw width);" in header
    assert "iplay_ncplane_meter16_yx" in header
    assert "db *iplay_ncplane_cells_at(db *cells, dw stride_cols, dw origin_y, dw origin_x)" in rewrite
    assert "iplay_ncplane_set_cells(plane, iplay_ncplane_cells_at(cells, stride_cols, origin_y, origin_x));" in rewrite
    assert "dw iplay_ncplane_cell_offset(const IplayNcPlane *plane, dw y, dw x)" in rewrite
    assert "db iplay_ncplane_cell_ch(const IplayNcPlane *plane, dw offset)" in rewrite
    assert "db iplay_ncplane_cell_attr(const IplayNcPlane *plane, dw offset)" in rewrite
    assert "void iplay_ncplane_put_cell_offset(IplayNcPlane *plane, dw offset, db ch, db attr)" in rewrite
    assert "void iplay_ncplane_copy_cell_offset(IplayNcPlane *plane, dw dst_offset, dw src_offset)" in rewrite
    assert "iplay_ncplane_put_cell_offset(plane," in rewrite
    assert "iplay_ncplane_cell_ch(plane, src_offset)" in rewrite
    assert "iplay_ncplane_cell_attr(plane, src_offset)" in rewrite
    assert "off = iplay_ncplane_cell_offset(plane, y, x);" in rewrite
    assert "iplay_ncplane_put_cell_offset(plane, off, ch, attr);" in rewrite
    assert "dw dst = iplay_ncplane_cell_offset(plane, (dw)(top + row), (dw)(left + col));" in rewrite
    assert "dw src = iplay_ncplane_cell_offset(plane, (dw)(top + row + count), (dw)(left + col));" in rewrite
    assert "dw dst = iplay_ncplane_cell_offset(plane, (dw)(top + row + count), (dw)(left + col));" in rewrite
    assert "dw src = iplay_ncplane_cell_offset(plane, (dw)(top + row), (dw)(left + col));" in rewrite
    assert rewrite.count("iplay_ncplane_copy_cell_offset(plane, dst, src);") == 2
    assert "((((dw)(top + row) * plane->stride_cols) + left + col) * 2u)" not in rewrite
    assert "((((dw)(top + row + count) * plane->stride_cols) + left + col) * 2u)" not in rewrite
    assert "off = (dw)(((dw)(y * plane->stride_cols) + x) * 2u);" not in rewrite
    assert "plane->cells[off] = ch;" not in rewrite
    assert "plane->cells[(dw)(off + 1u)] = attr;" not in rewrite
    assert "plane->cells[dst] = plane->cells[src];" not in rewrite
    assert "plane->cells[(dw)(dst + 1u)] = plane->cells[(dw)(src + 1u)];" not in rewrite
    assert "plane->cells[src_offset]" not in rewrite
    assert "plane->cells[(dw)(src_offset + 1u)]" not in rewrite
    assert "plane->cells = cells + ((((dw)(origin_y * stride_cols)) + origin_x) * IPLAY_TEXT_CELL_BYTES);" not in rewrite
    assert "iplay_window_init_root" in header
    assert "iplay_window_init_subwindow" in header
    assert "iplay_window_plane" in header
    assert "iplay_window_plane_const" in header
    assert "iplay_window_resize" in header
    assert "iplay_window_origin_yx" in header
    assert "iplay_window_rows" in header
    assert "iplay_window_cols" in header
    assert "iplay_window_erase" in header
    assert "iplay_window_fill_yx" in header
    assert "iplay_window_box_yx" in header
    assert "iplay_window_cursor_yx" in header
    assert "iplay_window_cursor_move_yx" in header
    assert "iplay_window_putc" in header
    assert "iplay_window_putstr" in header
    assert "iplay_window_putnstr" in header
    assert "iplay_window_putnstr_fill_yx" in header
    assert "iplay_window_scroll_up" in header
    assert "iplay_window_scroll_down" in header
    assert "iplay_window_draw_audio_levels" in header
    assert "const IplayNcPlane *iplay_window_plane_const(const IplayWindow *window)" in rewrite
    assert "#define iplay_window_set_plane_field(state, plane_value) ((state)->plane = *(plane_value))" in rewrite
    assert "static void iplay_window_set_plane_field(IplayWindow *state, const IplayNcPlane *plane)" not in rewrite
    assert "#define iplay_window_plane_field(state) (&(state)->plane)" in rewrite
    assert "#define iplay_window_plane_const_field(state) (&(state)->plane)" in rewrite
    assert "static IplayNcPlane *iplay_window_plane_field(IplayWindow *state)" not in rewrite
    assert "static const IplayNcPlane *iplay_window_plane_const_field(const IplayWindow *state)" not in rewrite
    assert "iplay_window_set_plane_field(window, root);" in rewrite
    assert "return iplay_window_plane_field(window);" in rewrite
    assert "return iplay_window_plane_const_field(window);" in rewrite
    assert "iplay_ncplane_subplane(iplay_window_plane(window), iplay_window_plane_const(parent), y, x, rows, cols);" in rewrite
    assert "iplay_ncplane_resize(iplay_window_plane(window), rows, cols);" in rewrite
    assert "iplay_ncplane_origin_yx(iplay_window_plane_const(window), y, x);" in rewrite
    assert "return iplay_ncplane_rows(iplay_window_plane_const(window));" in rewrite
    assert "return iplay_ncplane_cols(iplay_window_plane_const(window));" in rewrite
    assert "iplay_ncplane_erase(iplay_window_plane(window), attr);" in rewrite
    assert "iplay_ncplane_fill_yx(iplay_window_plane(window), y, x, rows, cols, ch, attr);" in rewrite
    assert "iplay_ncplane_box_yx(iplay_window_plane(window), y, x, rows, cols, attr, fill_attr);" in rewrite
    assert "iplay_ncplane_cursor_yx(iplay_window_plane_const(window), y, x);" in rewrite
    assert "iplay_ncplane_cursor_move_yx(iplay_window_plane(window), y, x);" in rewrite
    assert "iplay_ncplane_putc(iplay_window_plane(window), ch, attr);" in rewrite
    assert "iplay_ncplane_putstr(iplay_window_plane(window), text, attr);" in rewrite
    assert "iplay_ncplane_putnstr(iplay_window_plane(window), text, attr, width);" in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(iplay_window_plane(window), y, x, text, attr, width);" in rewrite
    assert "iplay_ncplane_scroll_up(iplay_window_plane(window), top, left, rows, cols, count, fill_attr);" in rewrite
    assert "iplay_ncplane_scroll_down(iplay_window_plane(window), top, left, rows, cols, count, fill_attr);" in rewrite
    assert "iplay_audio_levels_draw_yx(iplay_window_plane(window), y, x, levels, width, fill_ch, empty_ch, left_attr, right_attr, empty_attr);" in rewrite
    assert "window->plane = *root;" not in rewrite
    assert "return &window->plane;" not in rewrite
    assert "iplay_ncplane_subplane(&window->plane" not in rewrite
    assert "&parent->plane" not in rewrite
    assert "iplay_ncplane_resize(&window->plane" not in rewrite
    assert "iplay_ncplane_origin_yx(&window->plane" not in rewrite
    assert "iplay_ncplane_rows(&window->plane" not in rewrite
    assert "iplay_ncplane_cols(&window->plane" not in rewrite
    assert "iplay_ncplane_erase(&window->plane" not in rewrite
    assert "iplay_ncplane_fill_yx(&window->plane" not in rewrite
    assert "iplay_ncplane_box_yx(&window->plane" not in rewrite
    assert "iplay_ncplane_cursor_yx(&window->plane" not in rewrite
    assert "iplay_ncplane_cursor_move_yx(&window->plane" not in rewrite
    assert "iplay_ncplane_putc(&window->plane" not in rewrite
    assert "iplay_ncplane_putstr(&window->plane" not in rewrite
    assert "iplay_ncplane_putnstr(&window->plane" not in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(&window->plane" not in rewrite
    assert "iplay_ncplane_scroll_up(&window->plane" not in rewrite
    assert "iplay_ncplane_scroll_down(&window->plane" not in rewrite
    assert "iplay_audio_levels_draw_yx(&window->plane" not in rewrite
    assert "iplay_audio_levels_draw_yx" in header
    assert "IplayAudioSink" in header
    assert "IplayAudioOutput" in header
    assert "IplayAudioLevels" in header
    assert "IplaySdlAudioCallback" in header
    assert "IplaySdlAudioDeviceConfig" in header
    assert "IplayAudioBackend" in header
    assert "IplaySdlAudioSpec" in header
    assert "IplaySdlAudioDevice" in header
    assert "IplaySdlAudioDeviceConfig config" in header
    assert "hardware_enabled" in header
    assert "paused" in header
    assert "IplayRuntime" in header
    assert "IplayRuntimeOutputSpec" in header
    assert "IplayTerminalBackend video_backend;" in header
    assert "IplayAudioBackend audio_backend;" in header
    assert "db audio_hardware_enabled;" in header
    assert "video_mode_ok" in header
    assert "IplayRuntimeConfig" in header
    assert "video_backend" in header
    assert "video_present_enabled" in header
    assert "audio_backend" in header
    assert "audio_hardware_enabled" in header
    assert "IplayModuleStatus" in header
    assert "IPLAY_AUDIO_BACKEND_SB16_STEREO" in header
    assert "IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE" in header
    assert header.count("IPLAY_AUDIO_BACKEND_") == 2
    assert "IPLAY_AUDIO_BACKEND_ADLIB" not in header
    assert "IPLAY_AUDIO_BACKEND_GUS" not in header
    assert "IPLAY_AUDIO_BACKEND_GRAVIS" not in header
    assert "IPLAY_AUDIO_BACKEND_PC_SPEAKER" not in header
    assert "IPLAY_AUDIO_BACKEND_SPEAKER" not in header
    assert "int iplay_audio_backend_is_sb16_scope(IplayAudioBackend backend)" in header
    assert "int iplay_audio_backend_is_sb16_hardware(IplayAudioBackend backend)" in header
    assert "int iplay_audio_backend_is_sdl_compatible(IplayAudioBackend backend)" in header
    assert "iplay_sdl_audio_spec_backend" in header
    assert "iplay_sdl_audio_spec_backend_name" in header
    assert "iplay_sdl_audio_spec_format" in header
    assert "iplay_sdl_audio_spec_sample_rate" in header
    assert "iplay_sdl_audio_spec_bits_per_sample" in header
    assert "iplay_sdl_audio_spec_channels" in header
    assert "iplay_sdl_audio_spec_signed_samples" in header
    assert "iplay_sdl_audio_spec_hardware_enabled" in header
    assert "iplay_sdl_audio_spec_is_sb16_compatible" in header
    assert "iplay_sdl_audio_spec_is_sb16_hardware" in header
    assert "iplay_sdl_audio_spec_is_sdl_compatible" in header
    assert "IplayTextScreen screen" in header
    assert "IplayAudioOutput output" in header
    assert "left_peak" in header
    assert "right_peak" in header
    assert "left_16" in header
    assert "right_16" in header
    assert "IplayAudioLevels levels" in header
    assert "source_format" in header
    assert "scratch_bytes" in header
    assert "underrun_frames" in header
    assert "dropped_frames" in header
    assert "capacity_frames" in header
    assert "IPLAY_AUDIO_SB16_STEREO_16" in header
    assert "IPLAY_AUDIO_U8_MONO" in header
    assert "IPLAY_AUDIO_U8_STEREO" in header
    assert "IPLAY_AUDIO_S16_MONO" in header
    assert "IPLAY_AUDIO_S16_STEREO" in header
    assert "iplay_audio_frames_for_bytes" in header
    assert "iplay_audio_format_sample_rate" in header
    assert "iplay_audio_format_bits_per_sample" in header
    assert "iplay_audio_format_channels" in header
    assert "iplay_audio_format_signed_samples" in header
    assert "iplay_audio_format_set" in header
    assert "iplay_audio_format_equals" in header
    assert "iplay_audio_format_name" in header
    assert "iplay_audio_backend_name" in header
    assert "iplay_audio_source_format" in header
    assert "iplay_audio_make_source_format" in header
    assert "iplay_audio_rates_match" in header
    assert "iplay_audio_sink_set_format" in header
    assert "iplay_audio_sink_set_write_callback" in header
    assert "iplay_audio_sink_start" in header
    assert "iplay_audio_sink_stop" in header
    assert "iplay_audio_sink_set_active" in header
    assert "iplay_audio_sink_reset_counters" in header
    assert "iplay_audio_sink_set_capacity" in header
    assert "iplay_audio_sink_add_capacity" in header
    assert "iplay_audio_sink_format" in header
    assert "iplay_audio_sink_bytes_per_frame" in header
    assert "iplay_audio_sink_capacity" in header
    assert "iplay_audio_sink_frames_written" in header
    assert "iplay_audio_sink_underrun_frames" in header
    assert "iplay_audio_sink_dropped_frames" in header
    assert "iplay_audio_sink_is_active" in header
    assert "iplay_audio_sink_write_callback" in header
    assert "iplay_audio_sink_write_user" in header
    assert "iplay_audio_sink_set_frames_written" in header
    assert "iplay_audio_sink_set_underrun_frames" in header
    assert "iplay_audio_sink_set_dropped_frames" in header
    assert "iplay_audio_sink_clear_frames_written" in header
    assert "iplay_audio_sink_clear_underrun_frames" in header
    assert "iplay_audio_sink_clear_dropped_frames" in header
    assert "iplay_audio_sink_add_frames_written" in header
    assert "iplay_audio_sink_add_underrun_frames" in header
    assert "iplay_audio_sink_add_dropped_frames" in header
    assert "iplay_audio_sink_consume_capacity" in header
    assert "iplay_audio_sink_write" in header
    assert "iplay_audio_sink_write_silence" in header
    assert "iplay_audio_u8_to_s16_stereo" in header
    assert "iplay_audio_s16_to_s16_stereo" in header
    assert "iplay_audio_convert_to_sink_format" in header
    assert "iplay_audio_sink_write_converted" in header
    assert "#define iplay_audio_format_sample_rate_field(state) ((state)->sample_rate)" in rewrite
    assert "#define iplay_audio_format_bits_per_sample_field(state) ((state)->bits_per_sample)" in rewrite
    assert "#define iplay_audio_format_channels_field(state) ((state)->channels)" in rewrite
    assert "#define iplay_audio_format_signed_samples_field(state) ((state)->signed_samples)" in rewrite
    assert "#define iplay_audio_format_set_sample_rate_field(state, value) ((state)->sample_rate = (value))" in rewrite
    assert "#define iplay_audio_format_set_bits_per_sample_field(state, value) ((state)->bits_per_sample = (value))" in rewrite
    assert "#define iplay_audio_format_set_channels_field(state, value) ((state)->channels = (value))" in rewrite
    assert "#define iplay_audio_format_set_signed_samples_field(state, value) ((state)->signed_samples = (value))" in rewrite
    assert "static dw iplay_audio_format_sample_rate_field(const IplayAudioFormat *state)" not in rewrite
    assert "static db iplay_audio_format_bits_per_sample_field(const IplayAudioFormat *state)" not in rewrite
    assert "static db iplay_audio_format_channels_field(const IplayAudioFormat *state)" not in rewrite
    assert "static db iplay_audio_format_signed_samples_field(const IplayAudioFormat *state)" not in rewrite
    assert "static void iplay_audio_format_set_sample_rate_field(IplayAudioFormat *state, dw sample_rate)" not in rewrite
    assert "static void iplay_audio_format_set_bits_per_sample_field(IplayAudioFormat *state, db bits_per_sample)" not in rewrite
    assert "static void iplay_audio_format_set_channels_field(IplayAudioFormat *state, db channels)" not in rewrite
    assert "static void iplay_audio_format_set_signed_samples_field(IplayAudioFormat *state, db signed_samples)" not in rewrite
    assert "dw iplay_audio_format_sample_rate(const IplayAudioFormat *format)" in rewrite
    assert "return iplay_audio_format_sample_rate_field(format);" in rewrite
    assert "db iplay_audio_format_bits_per_sample(const IplayAudioFormat *format)" in rewrite
    assert "return iplay_audio_format_bits_per_sample_field(format);" in rewrite
    assert "db iplay_audio_format_channels(const IplayAudioFormat *format)" in rewrite
    assert "return iplay_audio_format_channels_field(format);" in rewrite
    assert "db iplay_audio_format_signed_samples(const IplayAudioFormat *format)" in rewrite
    assert "return iplay_audio_format_signed_samples_field(format);" in rewrite
    assert "void iplay_audio_format_set(IplayAudioFormat *format, dw sample_rate, db bits_per_sample, db channels, db signed_samples)" in rewrite
    assert "iplay_audio_format_set_sample_rate_field(format, sample_rate);" in rewrite
    assert "iplay_audio_format_set_bits_per_sample_field(format, bits_per_sample);" in rewrite
    assert "iplay_audio_format_set_channels_field(format, channels);" in rewrite
    assert "iplay_audio_format_set_signed_samples_field(format, signed_samples);" in rewrite
    assert "iplay_audio_format_set(format, sample_rate, bits_per_sample, channels, signed_samples ? 1u : 0u);" in rewrite
    assert "return (dw)((iplay_audio_format_bits_per_sample(format) / 8u) * iplay_audio_format_channels(format));" in rewrite
    assert "return iplay_audio_format_sample_rate(a) == iplay_audio_format_sample_rate(b)" in rewrite
    assert "iplay_audio_format_bits_per_sample(format) == 16u && iplay_audio_format_channels(format) == 2u" in rewrite
    assert "return iplay_audio_format_sample_rate(src_format) == iplay_audio_format_sample_rate(dst_format);" in rewrite
    assert "iplay_audio_u8_to_s16_stereo(src, src_frames, iplay_audio_format_channels(src_format), dst, dst_bytes);" in rewrite
    assert "iplay_audio_s16_to_s16_stereo(src, src_frames, iplay_audio_format_channels(src_format), dst, dst_bytes);" in rewrite
    assert "return a->sample_rate == b->sample_rate" not in rewrite
    assert "return format->bits_per_sample == 16u && format->channels == 2u && format->signed_samples != 0;" not in rewrite
    assert "return src_format->sample_rate == dst_format->sample_rate;" not in rewrite
    assert "return (dw)((format->bits_per_sample / 8u) * format->channels);" not in rewrite
    assert "return format->sample_rate;" not in rewrite
    assert "return format->bits_per_sample;" not in rewrite
    assert "return format->channels;" not in rewrite
    assert "return format->signed_samples;" not in rewrite
    assert "format->sample_rate = sample_rate;" not in rewrite
    assert "format->bits_per_sample = bits_per_sample;" not in rewrite
    assert "format->channels = channels;" not in rewrite
    assert "format->signed_samples = signed_samples;" not in rewrite
    assert (
        "format->sample_rate = sample_rate;\n"
        "    format->bits_per_sample = bits_per_sample;\n"
        "    format->channels = channels;\n"
        "    format->signed_samples = signed_samples ? 1u : 0u;"
    ) not in rewrite
    assert "src_format->bits_per_sample == 8u && src_format->signed_samples == 0" not in rewrite
    assert "src_format->bits_per_sample == 16u && src_format->signed_samples != 0" not in rewrite
    assert "src_format->channels, dst, dst_bytes" not in rewrite
    assert "#define iplay_audio_sink_set_format_field(state, value) ((state)->format = *(value))" in rewrite
    assert "#define iplay_audio_sink_set_write_field(state, value) ((state)->write = (value))" in rewrite
    assert "#define iplay_audio_sink_set_user_field(state, value) ((state)->user = (value))" in rewrite
    assert "#define iplay_audio_sink_set_active_field(state, value) ((state)->active = (value) ? 1u : 0u)" in rewrite
    assert "#define iplay_audio_sink_set_capacity_field(state, value) ((state)->capacity_frames = (value))" in rewrite
    assert "#define iplay_audio_sink_format_field(state) (&(state)->format)" in rewrite
    assert "#define iplay_audio_sink_capacity_field(state) ((state)->capacity_frames)" in rewrite
    assert "#define iplay_audio_sink_frames_written_field(state) ((state)->frames_written)" in rewrite
    assert "#define iplay_audio_sink_underrun_frames_field(state) ((state)->underrun_frames)" in rewrite
    assert "#define iplay_audio_sink_dropped_frames_field(state) ((state)->dropped_frames)" in rewrite
    assert "#define iplay_audio_sink_is_active_field(state) ((state)->active != 0)" in rewrite
    assert "#define iplay_audio_sink_write_callback_field(state) ((state)->write)" in rewrite
    assert "#define iplay_audio_sink_write_user_field(state) ((state)->user)" in rewrite
    assert "#define iplay_audio_sink_set_frames_written_field(state, value) ((state)->frames_written = (value))" in rewrite
    assert "#define iplay_audio_sink_set_underrun_frames_field(state, value) ((state)->underrun_frames = (value))" in rewrite
    assert "#define iplay_audio_sink_set_dropped_frames_field(state, value) ((state)->dropped_frames = (value))" in rewrite
    assert "static void iplay_audio_sink_set_format_field(IplayAudioSink *state, const IplayAudioFormat *format)" not in rewrite
    assert "static void iplay_audio_sink_set_write_field(IplayAudioSink *state, IplayAudioWriteFn write)" not in rewrite
    assert "static void iplay_audio_sink_set_user_field(IplayAudioSink *state, void *user)" not in rewrite
    assert "static void iplay_audio_sink_set_active_field(IplayAudioSink *state, int active)" not in rewrite
    assert "static void iplay_audio_sink_set_capacity_field(IplayAudioSink *state, dd capacity_frames)" not in rewrite
    assert "static void iplay_audio_sink_set_frames_written_field(IplayAudioSink *state, dd frames)" not in rewrite
    assert "static void iplay_audio_sink_set_underrun_frames_field(IplayAudioSink *state, dd frames)" not in rewrite
    assert "static void iplay_audio_sink_set_dropped_frames_field(IplayAudioSink *state, dd frames)" not in rewrite
    assert "static const IplayAudioFormat *iplay_audio_sink_format_field(const IplayAudioSink *state)" not in rewrite
    assert "static dd iplay_audio_sink_capacity_field(const IplayAudioSink *state)" not in rewrite
    assert "static dd iplay_audio_sink_frames_written_field(const IplayAudioSink *state)" not in rewrite
    assert "static dd iplay_audio_sink_underrun_frames_field(const IplayAudioSink *state)" not in rewrite
    assert "static dd iplay_audio_sink_dropped_frames_field(const IplayAudioSink *state)" not in rewrite
    assert "static int iplay_audio_sink_is_active_field(const IplayAudioSink *state)" not in rewrite
    assert "static IplayAudioWriteFn iplay_audio_sink_write_callback_field(const IplayAudioSink *state)" not in rewrite
    assert "static void *iplay_audio_sink_write_user_field(const IplayAudioSink *state)" not in rewrite
    assert "void iplay_audio_sink_set_format(IplayAudioSink *sink, const IplayAudioFormat *format)" in rewrite
    assert "iplay_audio_sink_set_format_field(sink, format);" in rewrite
    assert "void iplay_audio_sink_set_write_callback(IplayAudioSink *sink, IplayAudioWriteFn write, void *user)" in rewrite
    assert "iplay_audio_sink_set_write_field(sink, write);" in rewrite
    assert "iplay_audio_sink_set_user_field(sink, user);" in rewrite
    assert "iplay_audio_sink_set_format(sink, format);" in rewrite
    assert "iplay_audio_sink_set_write_callback(sink, write, user);" in rewrite
    assert "iplay_audio_sink_reset_counters(sink);" in rewrite
    assert "iplay_audio_sink_set_capacity(sink, 0xffffffffUL);" in rewrite
    assert "void iplay_audio_sink_set_active(IplayAudioSink *sink, int active)" in rewrite
    assert "iplay_audio_sink_set_active_field(sink, active);" in rewrite
    assert "iplay_audio_sink_set_active(sink, 1);" in rewrite
    assert "iplay_audio_sink_set_active(sink, 0);" in rewrite
    assert "IplayAudioWriteFn iplay_audio_sink_write_callback(const IplayAudioSink *sink)" in rewrite
    assert "return iplay_audio_sink_write_callback_field(sink);" in rewrite
    assert "void *iplay_audio_sink_write_user(const IplayAudioSink *sink)" in rewrite
    assert "return iplay_audio_sink_write_user_field(sink);" in rewrite
    assert "void iplay_audio_sink_set_frames_written(IplayAudioSink *sink, dd frames)" in rewrite
    assert "iplay_audio_sink_set_frames_written_field(sink, frames);" in rewrite
    assert "void iplay_audio_sink_set_underrun_frames(IplayAudioSink *sink, dd frames)" in rewrite
    assert "iplay_audio_sink_set_underrun_frames_field(sink, frames);" in rewrite
    assert "void iplay_audio_sink_set_dropped_frames(IplayAudioSink *sink, dd frames)" in rewrite
    assert "iplay_audio_sink_set_dropped_frames_field(sink, frames);" in rewrite
    assert "void iplay_audio_sink_clear_frames_written(IplayAudioSink *sink)" in rewrite
    assert "void iplay_audio_sink_clear_underrun_frames(IplayAudioSink *sink)" in rewrite
    assert "void iplay_audio_sink_clear_dropped_frames(IplayAudioSink *sink)" in rewrite
    assert "iplay_audio_sink_clear_frames_written(sink);" in rewrite
    assert "iplay_audio_sink_clear_underrun_frames(sink);" in rewrite
    assert "iplay_audio_sink_clear_dropped_frames(sink);" in rewrite
    assert "iplay_audio_sink_set_frames_written(sink, 0);" in rewrite
    assert "iplay_audio_sink_set_underrun_frames(sink, 0);" in rewrite
    assert "iplay_audio_sink_set_dropped_frames(sink, 0);" in rewrite
    assert "void iplay_audio_sink_add_frames_written(IplayAudioSink *sink, dd frames)" in rewrite
    assert "void iplay_audio_sink_add_underrun_frames(IplayAudioSink *sink, dd frames)" in rewrite
    assert "void iplay_audio_sink_add_dropped_frames(IplayAudioSink *sink, dd frames)" in rewrite
    assert "iplay_audio_sink_set_frames_written(sink, iplay_audio_sink_frames_written(sink) + frames);" in rewrite
    assert "iplay_audio_sink_set_underrun_frames(sink, iplay_audio_sink_underrun_frames(sink) + frames);" in rewrite
    assert "iplay_audio_sink_set_dropped_frames(sink, iplay_audio_sink_dropped_frames(sink) + frames);" in rewrite
    assert "void iplay_audio_sink_consume_capacity(IplayAudioSink *sink, dd frames)" in rewrite
    assert "dd old = iplay_audio_sink_capacity(sink);" in rewrite
    assert "iplay_audio_sink_set_capacity(sink, updated);" in rewrite
    assert "iplay_audio_sink_set_capacity(sink, iplay_audio_sink_capacity(sink) - frames);" in rewrite
    assert "IplayAudioWriteFn write = iplay_audio_sink_write_callback(sink);" in rewrite
    assert "const IplayAudioFormat *format = iplay_audio_sink_format(sink);" in rewrite
    assert "if (!iplay_audio_sink_is_active(sink)) return;" in rewrite
    assert "if (write != 0) write(iplay_audio_sink_write_user(sink), pcm, byte_count);" in rewrite
    assert "iplay_audio_sink_add_dropped_frames(sink, frames - iplay_audio_sink_capacity(sink));" in rewrite
    assert "iplay_audio_sink_add_frames_written(sink, frames);" in rewrite
    assert "iplay_audio_sink_consume_capacity(sink, frames);" in rewrite
    assert "iplay_audio_sink_add_underrun_frames(sink, frame_count);" in rewrite
    assert "iplay_audio_convert_to_sink_format(src_format, src, src_frames, iplay_audio_sink_format(sink), scratch, scratch_bytes);" in rewrite
    assert "void iplay_audio_sink_start(IplayAudioSink *sink) {\n    sink->active = 1;\n}" not in rewrite
    assert "void iplay_audio_sink_stop(IplayAudioSink *sink) {\n    sink->active = 0;\n}" not in rewrite
    assert "sink->format = *format;" not in rewrite
    assert "sink->write = write;" not in rewrite
    assert "sink->user = user;" not in rewrite
    assert "sink->active = active ? 1u : 0u;" not in rewrite
    assert "sink->capacity_frames = capacity_frames;" not in rewrite
    assert "return &sink->format;" not in rewrite
    assert "return sink->capacity_frames;" not in rewrite
    assert "return sink->frames_written;" not in rewrite
    assert "return sink->underrun_frames;" not in rewrite
    assert "return sink->dropped_frames;" not in rewrite
    assert "return sink->active != 0;" not in rewrite
    assert "return sink->write;" not in rewrite
    assert "return sink->user;" not in rewrite
    assert "sink->frames_written = 0;" not in rewrite
    assert "sink->underrun_frames = 0;" not in rewrite
    assert "sink->dropped_frames = 0;" not in rewrite
    assert "sink->frames_written = frames;" not in rewrite
    assert "sink->underrun_frames = frames;" not in rewrite
    assert "sink->dropped_frames = frames;" not in rewrite
    assert "sink->frames_written += frames;" not in rewrite
    assert "sink->underrun_frames += frames;" not in rewrite
    assert "sink->dropped_frames += frames;" not in rewrite
    assert "dd old = sink->capacity_frames;" not in rewrite
    assert "sink->capacity_frames += capacity_frames;" not in rewrite
    assert "if (sink->capacity_frames < old) sink->capacity_frames = 0xffffffffUL;" not in rewrite
    assert "sink->capacity_frames -= frames;" not in rewrite
    assert "iplay_audio_output_sink" in header
    assert "iplay_audio_output_sink_const" in header
    assert "iplay_audio_output_set_source_format" in header
    assert "iplay_audio_output_set_scratch" in header
    assert "iplay_audio_output_init" in header
    assert "iplay_audio_output_init_sb16_stereo" in header
    assert "iplay_audio_output_start" in header
    assert "iplay_audio_output_stop" in header
    assert "iplay_audio_output_is_active" in header
    assert "iplay_audio_output_reset_counters" in header
    assert "iplay_audio_output_set_capacity" in header
    assert "iplay_audio_output_add_capacity" in header
    assert "iplay_audio_output_capacity" in header
    assert "iplay_audio_output_accepted_frames" in header
    assert "iplay_audio_output_frames_written" in header
    assert "iplay_audio_output_underrun_frames" in header
    assert "iplay_audio_output_dropped_frames" in header
    assert "iplay_audio_output_source_format" in header
    assert "iplay_audio_output_sink_format" in header
    assert "iplay_audio_output_bytes_per_frame" in header
    assert "iplay_audio_output_frames_for_bytes" in header
    assert "iplay_audio_output_bytes_for_frames" in header
    assert "iplay_audio_output_is_sb16_stereo" in header
    assert "iplay_audio_output_levels" in header
    assert "iplay_audio_output_levels_mut" in header
    assert "iplay_audio_output_scratch" in header
    assert "iplay_audio_output_scratch_bytes" in header
    assert "iplay_audio_levels_set" in header
    assert "iplay_audio_levels_clear" in header
    assert "db iplay_audio_levels_left_16(const IplayAudioLevels *levels);" in header
    assert "db iplay_audio_levels_right_16(const IplayAudioLevels *levels);" in header
    assert "iplay_audio_output_reset_levels" in header
    assert "iplay_audio_output_write_mixer_frames" in header
    assert "iplay_audio_output_write_sb16_frames" in header
    assert "iplay_audio_output_write_silence" in header
    assert "iplay_audio_level_to_16" in header
    assert "iplay_audio_sb16_stereo_levels" in header
    assert "iplay_audio_output_draw_levels_yx" in header
    assert "#define iplay_audio_output_sink_field(state) (&(state)->sink)" in rewrite
    assert "#define iplay_audio_output_sink_const_field(state) (&(state)->sink)" in rewrite
    assert "#define iplay_audio_output_source_format_mut_field(state) (&(state)->source_format)" in rewrite
    assert "#define iplay_audio_output_source_format_field(state) (&(state)->source_format)" in rewrite
    assert "#define iplay_audio_output_set_scratch_buffer_field(state, value) ((state)->scratch = (value))" in rewrite
    assert "#define iplay_audio_output_set_scratch_bytes_field(state, value) ((state)->scratch_bytes = (value))" in rewrite
    assert "#define iplay_audio_output_levels_field(state) (&(state)->levels)" in rewrite
    assert "#define iplay_audio_output_levels_mut_field(state) (&(state)->levels)" in rewrite
    assert "#define iplay_audio_output_scratch_field(state) ((state)->scratch)" in rewrite
    assert "#define iplay_audio_output_scratch_bytes_field(state) ((state)->scratch_bytes)" in rewrite
    assert "static IplayAudioSink *iplay_audio_output_sink_field(IplayAudioOutput *state)" not in rewrite
    assert "static const IplayAudioSink *iplay_audio_output_sink_const_field(const IplayAudioOutput *state)" not in rewrite
    assert "static IplayAudioFormat *iplay_audio_output_source_format_mut_field(IplayAudioOutput *state)" not in rewrite
    assert "static const IplayAudioFormat *iplay_audio_output_source_format_field(const IplayAudioOutput *state)" not in rewrite
    assert "static void iplay_audio_output_set_scratch_buffer_field(IplayAudioOutput *state, db *scratch)" not in rewrite
    assert "static void iplay_audio_output_set_scratch_bytes_field(IplayAudioOutput *state, dw scratch_bytes)" not in rewrite
    assert "static const IplayAudioLevels *iplay_audio_output_levels_field(const IplayAudioOutput *state)" not in rewrite
    assert "static IplayAudioLevels *iplay_audio_output_levels_mut_field(IplayAudioOutput *state)" not in rewrite
    assert "static db *iplay_audio_output_scratch_field(IplayAudioOutput *state)" not in rewrite
    assert "static dw iplay_audio_output_scratch_bytes_field(const IplayAudioOutput *state)" not in rewrite
    assert "IplayAudioSink *iplay_audio_output_sink(IplayAudioOutput *output)" in rewrite
    assert "return iplay_audio_output_sink_field(output);" in rewrite
    assert "const IplayAudioSink *iplay_audio_output_sink_const(const IplayAudioOutput *output)" in rewrite
    assert "return iplay_audio_output_sink_const_field(output);" in rewrite
    assert "void iplay_audio_output_set_source_format(IplayAudioOutput *output, const IplayAudioFormat *source_format)" in rewrite
    assert "void iplay_audio_output_set_scratch(IplayAudioOutput *output, db *scratch, dw scratch_bytes)" in rewrite
    assert "iplay_audio_output_set_source_format(output, source_format);" in rewrite
    assert "iplay_audio_output_set_scratch(output, scratch, scratch_bytes);" in rewrite
    assert "iplay_audio_output_init(output, &IPLAY_AUDIO_SB16_STEREO_16, write, user, 0, 0);" in rewrite
    assert "iplay_audio_sink_init(iplay_audio_output_sink(output), &IPLAY_AUDIO_SB16_STEREO_16, write, user);" in rewrite
    assert "iplay_audio_output_set_source_format(output, &IPLAY_AUDIO_SB16_STEREO_16);" not in rewrite
    assert "iplay_audio_output_set_scratch(output, 0, 0);" not in rewrite
    assert "iplay_audio_sink_start(iplay_audio_output_sink(output));" in rewrite
    assert "iplay_audio_sink_stop(iplay_audio_output_sink(output));" in rewrite
    assert "return iplay_audio_sink_is_active(iplay_audio_output_sink_const(output));" in rewrite
    assert "return iplay_audio_sink_capacity(iplay_audio_output_sink_const(output));" in rewrite
    assert "dw iplay_audio_output_accepted_frames(const IplayAudioOutput *output, dw frame_count)" in rewrite
    assert "if (!iplay_audio_output_is_active(output)) return 0;" in rewrite
    assert "if (frame_count > iplay_audio_output_capacity(output)) return (dw)iplay_audio_output_capacity(output);" in rewrite
    assert rewrite.count("accepted_frames = iplay_audio_output_accepted_frames(output, accepted_frames);") == 2
    assert "if (!iplay_audio_output_is_active(output)) accepted_frames = 0;" not in rewrite
    assert "if (accepted_frames > iplay_audio_output_capacity(output)) accepted_frames = (dw)iplay_audio_output_capacity(output);" not in rewrite
    assert "return iplay_audio_sink_format(iplay_audio_output_sink_const(output));" in rewrite
    assert "IplayAudioFormat *iplay_audio_output_source_format_mut(IplayAudioOutput *output)" in header
    assert "IplayAudioFormat *iplay_audio_output_source_format_mut(IplayAudioOutput *output)" in rewrite
    assert "return iplay_audio_output_source_format_mut_field(output);" in rewrite
    assert "const IplayAudioFormat *iplay_audio_output_source_format(const IplayAudioOutput *output)" in rewrite
    assert "return iplay_audio_output_source_format_field(output);" in rewrite
    assert "iplay_audio_format_set(\n        iplay_audio_output_source_format_mut(output)," in rewrite
    assert "void iplay_audio_output_set_scratch_buffer(IplayAudioOutput *output, db *scratch)" in header
    assert "void iplay_audio_output_set_scratch_bytes(IplayAudioOutput *output, dw scratch_bytes)" in header
    assert "void iplay_audio_output_set_scratch_buffer(IplayAudioOutput *output, db *scratch)" in rewrite
    assert "iplay_audio_output_set_scratch_buffer_field(output, scratch);" in rewrite
    assert "void iplay_audio_output_set_scratch_bytes(IplayAudioOutput *output, dw scratch_bytes)" in rewrite
    assert "iplay_audio_output_set_scratch_bytes_field(output, scratch_bytes);" in rewrite
    assert "iplay_audio_output_set_scratch_buffer(output, scratch);" in rewrite
    assert "iplay_audio_output_set_scratch_bytes(output, scratch_bytes);" in rewrite
    assert "IplayAudioLevels *iplay_audio_output_levels_mut(IplayAudioOutput *output)" in rewrite
    assert "return iplay_audio_output_levels_mut_field(output);" in rewrite
    assert "db *iplay_audio_output_scratch(IplayAudioOutput *output)" in rewrite
    assert "return iplay_audio_output_scratch_field(output);" in rewrite
    assert "dw iplay_audio_output_scratch_bytes(const IplayAudioOutput *output)" in rewrite
    assert "return iplay_audio_output_scratch_bytes_field(output);" in rewrite
    assert "dw iplay_audio_output_frames_for_bytes(const IplayAudioOutput *output, dw byte_count)" in rewrite
    assert "return iplay_audio_frames_for_bytes(iplay_audio_output_sink_format(output), byte_count);" in rewrite
    assert "dw iplay_audio_output_bytes_for_frames(const IplayAudioOutput *output, dw frame_count)" in rewrite
    assert "return (dw)(frame_count * iplay_audio_output_bytes_per_frame(output));" in rewrite
    assert "accepted_frames = iplay_audio_output_frames_for_bytes(output, bytes);" in rewrite
    assert "accepted_frames = iplay_audio_frames_for_bytes(sink_format, bytes);" not in rewrite
    assert "accepted_bytes = iplay_audio_output_bytes_for_frames(output, accepted_frames);" in rewrite
    assert "return accepted_bytes;" in rewrite
    assert "if (src == 0) return 0;" in rewrite
    assert "if (pcm == 0) return 0;" in rewrite
    assert "if (stream == 0) return 0;" in rewrite
    assert "rewrite/audio_wrapper_runner.c" in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert 'streq(argv[1], "sdlaudioinitformat")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_sdl_audio_device_init_sb16_compatible(&device, capture_audio_write, &capture);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_sdl_audio_device_frames_for_bytes(&device, 10)" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudiowriteaccepted")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "accepted = iplay_sdl_audio_device_write_sb16_frames(&device, pcm, 3);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudiowritenull")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "accepted = iplay_sdl_audio_device_write_sb16_frames(&device, 0, 2);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudiowritesignedlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "0x00,0xc0, 0x00,0xf8" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudiocallbackaccepted")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "accepted = iplay_sdl_audio_device_callback(&device, stream, sizeof(stream));" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudiocallbackpartial")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "frames = iplay_sdl_audio_device_frames_for_bytes(&device, sizeof(stream));" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudiocallbacksignedlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "0x00,0xc0, 0x00,0xf8" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudiocallbackpaused")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_sdl_audio_device_pause(&device, 1);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudiocallbacknull")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "accepted = iplay_sdl_audio_device_callback(0, stream, sizeof(stream));" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudiocallbacknullstream")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "accepted = iplay_sdl_audio_device_callback(&device, 0, 8);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudioopenrejectnonsb16")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_sdl_audio_device_config_set_format(&config, &IPLAY_AUDIO_U8_MONO);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_sdl_audio_device_config_set_format(&config, &IPLAY_AUDIO_S16_MONO);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_sdl_audio_device_config_set_format(&config, &IPLAY_AUDIO_SB16_STEREO_16);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "sdlaudioopenpreservesactive")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "bad_reopen = iplay_sdl_audio_device_open(&device, &config, capture_audio_write, &capture);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeinitformat")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_bytes_for_frames(&runtime, 3)" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeinitcounters")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_queued_bytes(&runtime)" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeaudiostartclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_start(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeaudiostopclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_stop(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeaudiopauseclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_pause(&runtime, 1);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeaudiopauseresumeclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_pause(&runtime, 0);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeaudiocapacityclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_set_capacity(&runtime, 3);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeaudioaddcapacityclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_add_capacity(&runtime, 3);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeaudioclearqueuedclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_clear_queued(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeaudioresetcountersclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_reset_counters(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimewriteaccepted")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 3);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert '[ "$1" = "runtimeinitformat" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimeinitcounters" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimeaudiostartclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimeaudiostopclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimeaudiopauseclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimeaudiopauseresumeclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimeaudiocapacityclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimeaudioaddcapacityclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimeaudioclearqueuedclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimeaudioresetcountersclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimehwinitformat" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimehwinitcounters" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimehwaudiopauseclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimehwaudiopauseresumeclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimehwaudiocapacityclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimehwaudioaddcapacityclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimehwaudioclearqueuedclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimehwaudioresetcountersclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimehwaudiostartclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '[ "$1" = "runtimehwaudiostopclean" ]' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert 'streq(argv[1], "runtimewritenull")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "accepted = iplay_runtime_write_sb16_frames(&runtime, 0, 2);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeaudioopenpreservesactive")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "bad_reopen = iplay_sdl_audio_device_open(iplay_runtime_audio(&runtime), &bad_config, capture_audio_write, &capture);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimesignedlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "0x00,0x80, 0x00,0x10" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimequeuepartial")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "frames = iplay_runtime_audio_frames_for_bytes(&runtime, sizeof(stream));" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "queued = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimequeuesignedlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "0x00,0xc0, 0x00,0xf8" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimequeuepaused")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_pause(&runtime, 1);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_pause(&runtime, 0);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimepausepreserveslevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "sdl=%u hw=%u first=%u before=%u,%u before_l=" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimepauseresumelevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "paused_levels=%u,%u paused_l=" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimequeuestopstart")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_stop(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_start(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimestoppreserveslevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "sdl=%u hw=%u first=%u before=%u,%u before_l=" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimestopstartlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "stopped_levels=%u,%u stopped_l=" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeclearqueuedlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "sdl=%u hw=%u queued=%u before=%lu,%lu levels_before=%u,%u before_l=" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeresetcounterslevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "sdl=%u hw=%u accepted=%u before=%lu,%lu,%lu,%u,%u before_l=" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimeresetunderrunlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "sdl=%u hw=%u before=%lu,%lu,%lu,%u,%u before_l=" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimelevelsdisplay80x50")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_is_sdl_compatible(&runtime)" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimelevelsreset80x50")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_reset_levels(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwinitformat")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_is_sb16_hardware(&runtime)" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_bytes_for_frames(&runtime, 3)" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwinitcounters")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_queued_frames(&runtime)" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwaudiopauseclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwaudiopauseresumeclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwaudiocapacityclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwaudioaddcapacityclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwaudioclearqueuedclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwaudioresetcountersclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwaudiostartclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwaudiostopclean")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwwriteaccepted")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwwritenull")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwaudioopenpreservesactive")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_is_sb16_hardware(&runtime)" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwqueuepartial")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "frames = iplay_runtime_audio_frames_for_bytes(&runtime, sizeof(stream));" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "queued = iplay_runtime_audio_queue(&runtime, stream, sizeof(stream));" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwqueuesignedlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "0x00,0xc0, 0x00,0xf8" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwqueuepaused")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_pause(&runtime, 1);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_pause(&runtime, 0);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwwritepaused")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "accepted = iplay_runtime_write_sb16_frames(&runtime, pcm, 2);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwpausepreserveslevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "backend=%u hw=%u first=%u before=%u,%u before_l=" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwpauseresumelevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwwritestopped")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwstoppreserveslevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwstopstartlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwwritepauseresume")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "paused_accepted = iplay_runtime_write_sb16_frames(&runtime, paused_pcm, 1);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "live_accepted = iplay_runtime_write_sb16_frames(&runtime, live_pcm, 1);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwqueuestopstart")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwshutdown")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_shutdown(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwlevelsdisplay")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "accepted_low = iplay_runtime_write_sb16_frames(&runtime, low_pcm, 1);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "accepted_high = iplay_runtime_write_sb16_frames(&runtime, high_pcm, 1);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_draw_audio_levels(&runtime, 10, 4, 16);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwlevelsdisplay80x50")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_draw_audio_levels(&runtime, 48, 62, 16);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "playerruntimehw80x50levels")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "iplay_runtime_draw_audio_levels(&runtime, 48, 62, 16);" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "text_level_left[32]" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "text_level_right[32]" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "level_l=" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "level_r=" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert '"playerruntimehw80x50levels"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert 'streq(argv[1], "runtimehwresetlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_reset_levels(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwlevelsreset80x50")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_backend(&runtime)" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwsignedlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "0x00,0x80, 0x00,0x10" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwwritesilence")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_write_silence(&runtime, 3);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_underrun_frames(&runtime)" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwstoppedsilence")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_stop(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwpausedsilence")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_pause(&runtime, 1);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_config_sb16_hardware_capacity(&config, cells, sizeof(cells), &IPLAY_TEXT_MODE_40X25, 0, 0, capture_audio_write, &capture);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwcapacityrefill")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "first = iplay_runtime_write_sb16_frames(&runtime, pcm1, 3);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_add_capacity(&runtime, 1);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "second = iplay_runtime_write_sb16_frames(&runtime, pcm2, 2);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwclearqueued")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_clear_queued(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwclearqueuedlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "levels_clear=%u,%u" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwresetcounters")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_audio_reset_counters(&runtime);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwresetcounterslevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "backend=%u hw=%u accepted=%u before=%lu,%lu,%lu,%u,%u before_l=" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwresetunderrun")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(argv[1], "runtimehwresetunderrunlevels")' in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert "iplay_runtime_write_silence(&runtime, 2);" in (ROOT / "rewrite" / "audio_wrapper_runner.c").read_text()
    assert 'streq(op, "sdlaudioinitformat")' not in runner
    assert 'streq(op, "sdlaudiowriteaccepted")' not in runner
    assert 'streq(op, "sdlaudiowritenull")' not in runner
    assert 'streq(op, "sdlaudiowritesignedlevels")' not in runner
    assert 'streq(op, "sdlaudiocallbackaccepted")' not in runner
    assert 'streq(op, "sdlaudiocallbackpartial")' not in runner
    assert 'streq(op, "sdlaudiocallbacksignedlevels")' not in runner
    assert 'streq(op, "sdlaudiocallbackpaused")' not in runner
    assert 'streq(op, "sdlaudiocallbacknull")' not in runner
    assert 'streq(op, "sdlaudiocallbacknullstream")' not in runner
    assert 'streq(op, "sdlaudioopenrejectnonsb16")' not in runner
    assert 'streq(op, "sdlaudioopenpreservesactive")' not in runner
    assert 'streq(op, "runtimeinitformat")' not in runner
    assert 'streq(op, "runtimeinitcounters")' not in runner
    assert 'streq(op, "runtimeaudiostartclean")' not in runner
    assert 'streq(op, "runtimeaudiostopclean")' not in runner
    assert 'streq(op, "runtimeaudiopauseclean")' not in runner
    assert 'streq(op, "runtimeaudiopauseresumeclean")' not in runner
    assert 'streq(op, "runtimeaudiocapacityclean")' not in runner
    assert 'streq(op, "runtimeaudioaddcapacityclean")' not in runner
    assert 'streq(op, "runtimeaudioclearqueuedclean")' not in runner
    assert 'streq(op, "runtimeaudioresetcountersclean")' not in runner
    assert 'streq(op, "runtimewriteaccepted")' not in runner
    assert 'streq(op, "runtimewritenull")' not in runner
    assert 'streq(op, "runtimeaudioopenpreservesactive")' not in runner
    assert 'streq(op, "runtimesignedlevels")' not in runner
    assert 'streq(op, "runtimequeuepartial")' not in runner
    assert 'streq(op, "runtimequeuesignedlevels")' not in runner
    assert 'streq(op, "runtimequeuepaused")' not in runner
    assert 'streq(op, "runtimepausepreserveslevels")' not in runner
    assert 'streq(op, "runtimepauseresumelevels")' not in runner
    assert 'streq(op, "runtimequeuestopstart")' not in runner
    assert 'streq(op, "runtimestoppreserveslevels")' not in runner
    assert 'streq(op, "runtimestopstartlevels")' not in runner
    assert 'streq(op, "runtimeclearqueuedlevels")' not in runner
    assert 'streq(op, "runtimeresetcounterslevels")' not in runner
    assert 'streq(op, "runtimeresetunderrunlevels")' not in runner
    assert 'streq(op, "runtimehwinitformat")' not in runner
    assert 'streq(op, "runtimehwaudiopauseclean")' not in runner
    assert 'streq(op, "runtimehwaudiopauseresumeclean")' not in runner
    assert 'streq(op, "runtimehwaudiocapacityclean")' not in runner
    assert 'streq(op, "runtimehwaudioaddcapacityclean")' not in runner
    assert 'streq(op, "runtimehwaudioclearqueuedclean")' not in runner
    assert 'streq(op, "runtimehwaudioresetcountersclean")' not in runner
    assert 'streq(op, "runtimehwinitcounters")' not in runner
    assert 'streq(op, "runtimehwaudiostartclean")' not in runner
    assert 'streq(op, "runtimehwaudiostopclean")' not in runner
    assert 'streq(op, "runtimehwwriteaccepted")' not in runner
    assert 'streq(op, "runtimehwwritenull")' not in runner
    assert 'streq(op, "runtimehwaudioopenpreservesactive")' not in runner
    assert 'streq(op, "runtimehwqueuepartial")' not in runner
    assert 'streq(op, "runtimehwqueuesignedlevels")' not in runner
    assert 'streq(op, "runtimehwqueuepaused")' not in runner
    assert 'streq(op, "runtimehwwritepaused")' not in runner
    assert 'streq(op, "runtimehwpausepreserveslevels")' not in runner
    assert 'streq(op, "runtimehwpauseresumelevels")' not in runner
    assert 'streq(op, "runtimehwwritestopped")' not in runner
    assert 'streq(op, "runtimehwstoppreserveslevels")' not in runner
    assert 'streq(op, "runtimehwstopstartlevels")' not in runner
    assert 'streq(op, "runtimehwwritepauseresume")' not in runner
    assert 'streq(op, "runtimehwqueuestopstart")' not in runner
    assert 'streq(op, "runtimehwshutdown")' not in runner
    assert 'streq(op, "runtimehwlevelsdisplay")' not in runner
    assert 'streq(op, "runtimehwresetlevels")' not in runner
    assert 'streq(op, "runtimehwsignedlevels")' not in runner
    assert 'streq(op, "runtimehwwritesilence")' not in runner
    assert 'streq(op, "runtimehwstoppedsilence")' not in runner
    assert 'streq(op, "runtimehwpausedsilence")' not in runner
    assert 'streq(op, "runtimehwcapacityrefill")' not in runner
    assert 'streq(op, "runtimehwclearqueued")' not in runner
    assert 'streq(op, "runtimehwclearqueuedlevels")' not in runner
    assert 'streq(op, "runtimehwresetcounters")' not in runner
    assert 'streq(op, "runtimehwresetcounterslevels")' not in runner
    assert 'streq(op, "runtimehwresetunderrun")' not in runner
    assert 'streq(op, "runtimehwresetunderrunlevels")' not in runner
    assert "return &output->sink;" not in rewrite
    assert "return &output->source_format;" not in rewrite
    assert "return &output->levels;" not in rewrite
    assert "return output->scratch;" not in rewrite
    assert "return output->scratch_bytes;" not in rewrite
    assert "output->scratch = scratch;" not in rewrite
    assert "output->scratch_bytes = scratch_bytes;" not in rewrite
    assert "byte_count = iplay_audio_output_bytes_for_frames(output, frame_count);" in rewrite
    assert "byte_count = (dw)(frame_count * iplay_audio_bytes_per_frame(&IPLAY_AUDIO_SB16_STEREO_16));" not in rewrite
    assert "void iplay_audio_levels_set(IplayAudioLevels *levels, dw left_peak, dw right_peak)" in rewrite
    assert "void iplay_audio_levels_clear(IplayAudioLevels *levels)" in rewrite
    assert "iplay_audio_levels_clear(iplay_audio_output_levels_mut(output));" in rewrite
    assert "iplay_audio_levels_set(levels, left_peak, right_peak);" in rewrite
    assert "#define iplay_audio_levels_left_16_field(state) ((state)->left_16)" in rewrite
    assert "#define iplay_audio_levels_right_16_field(state) ((state)->right_16)" in rewrite
    assert "static db iplay_audio_levels_left_16_field(const IplayAudioLevels *state)" not in rewrite
    assert "static db iplay_audio_levels_right_16_field(const IplayAudioLevels *state)" not in rewrite
    assert "db iplay_audio_levels_left_16(const IplayAudioLevels *levels)" in rewrite
    assert "db iplay_audio_levels_right_16(const IplayAudioLevels *levels)" in rewrite
    assert "return iplay_audio_levels_left_16_field(levels);" in rewrite
    assert "return iplay_audio_levels_right_16_field(levels);" in rewrite
    assert "iplay_ncplane_meter16_yx(plane, y, x, iplay_audio_levels_left_16(levels), width, fill_ch, empty_ch, left_attr, empty_attr);" in rewrite
    assert "iplay_ncplane_meter16_yx(plane, (dw)(y + 1u), x, iplay_audio_levels_right_16(levels), width, fill_ch, empty_ch, right_attr, empty_attr);" in rewrite
    assert "levels->left_16, width" not in rewrite
    assert "levels->right_16, width" not in rewrite
    assert "iplay_audio_convert_to_sink_format(iplay_audio_output_source_format(output), src, src_frames, sink_format, scratch, iplay_audio_output_scratch_bytes(output));" in rewrite
    assert "iplay_audio_sb16_stereo_levels(iplay_audio_output_levels_mut(output), pcm, accepted_frames);" in rewrite
    assert "iplay_audio_sink_start(&output->sink)" not in rewrite
    assert "iplay_audio_sink_stop(&output->sink)" not in rewrite
    assert "iplay_audio_sink_reset_counters(&output->sink)" not in rewrite
    assert "iplay_audio_sink_set_capacity(&output->sink" not in rewrite
    assert "iplay_audio_sink_add_capacity(&output->sink" not in rewrite
    assert "iplay_audio_sink_is_active(&output->sink)" not in rewrite
    assert "iplay_audio_sink_write(&output->sink" not in rewrite
    assert "iplay_audio_sink_write_silence(&output->sink" not in rewrite
    assert "&output->sink.format" not in rewrite
    assert "output->sink.capacity_frames" not in rewrite
    assert "output->source_format = *source_format;" not in rewrite
    assert "output->source_format = IPLAY_AUDIO_SB16_STEREO_16;" not in rewrite
    assert "output->scratch = scratch;" not in rewrite
    assert "output->scratch_bytes = scratch_bytes;" not in rewrite
    assert "output->scratch = 0;" not in rewrite
    assert "#define iplay_audio_levels_set_left_peak_field(state, value) ((state)->left_peak = (value))" in rewrite
    assert "#define iplay_audio_levels_set_right_peak_field(state, value) ((state)->right_peak = (value))" in rewrite
    assert "#define iplay_audio_levels_set_left_16_field(state, value) ((state)->left_16 = (value))" in rewrite
    assert "#define iplay_audio_levels_set_right_16_field(state, value) ((state)->right_16 = (value))" in rewrite
    assert "static void iplay_audio_levels_set_left_peak_field(IplayAudioLevels *state, dw left_peak)" not in rewrite
    assert "static void iplay_audio_levels_set_right_peak_field(IplayAudioLevels *state, dw right_peak)" not in rewrite
    assert "static void iplay_audio_levels_set_left_16_field(IplayAudioLevels *state, dw left_16)" not in rewrite
    assert "static void iplay_audio_levels_set_right_16_field(IplayAudioLevels *state, dw right_16)" not in rewrite
    assert "iplay_audio_levels_set_left_peak_field(levels, left_peak);" in rewrite
    assert "iplay_audio_levels_set_right_peak_field(levels, right_peak);" in rewrite
    assert "iplay_audio_levels_set_left_16_field(levels, iplay_audio_level_to_16(left_peak));" in rewrite
    assert "iplay_audio_levels_set_right_16_field(levels, iplay_audio_level_to_16(right_peak));" in rewrite
    assert "levels->left_peak = left_peak;" not in rewrite
    assert "levels->right_peak = right_peak;" not in rewrite
    assert "levels->left_16 = iplay_audio_level_to_16(left_peak);" not in rewrite
    assert "levels->right_16 = iplay_audio_level_to_16(right_peak);" not in rewrite
    assert "levels->left_peak = 0;" not in rewrite
    assert "levels->right_peak = 0;" not in rewrite
    assert "levels->left_16 = 0;" not in rewrite
    assert "levels->right_16 = 0;" not in rewrite
    assert "iplay_sdl_audio_device_config_sb16_stereo" in header
    assert "iplay_sdl_audio_device_config_set_format" in header
    assert "iplay_sdl_audio_device_config_set_samples" in header
    assert "iplay_sdl_audio_device_config_set_callback" in header
    assert "iplay_sdl_audio_device_config_set_backend" in header
    assert "iplay_sdl_audio_device_config_format" in header
    assert "iplay_sdl_audio_device_config_frequency" in header
    assert "iplay_sdl_audio_device_config_bits_per_sample" in header
    assert "iplay_sdl_audio_device_config_channels" in header
    assert "iplay_sdl_audio_device_config_signed_samples" in header
    assert "iplay_sdl_audio_device_config_samples" in header
    assert "iplay_sdl_audio_device_config_callback" in header
    assert "iplay_sdl_audio_device_config_userdata" in header
    assert "iplay_sdl_audio_device_config_backend" in header
    assert "iplay_sdl_audio_device_config_hardware_enabled" in header
    assert "iplay_sdl_audio_device_config_is_sb16_stereo" in header
    assert "iplay_sdl_audio_device_open" in header
    assert "iplay_sdl_audio_device_config" in header
    assert "iplay_sdl_audio_device_set_config" in header
    assert "iplay_sdl_audio_device_apply_config" in header
    assert "iplay_sdl_audio_device_finish_open" in header
    assert "iplay_sdl_audio_device_init_sb16_compatible" in header
    assert "iplay_sdl_audio_device_init_sb16_hardware" in header
    assert "iplay_sdl_audio_device_spec" in header
    assert "iplay_sdl_audio_device_backend" in header
    assert "iplay_sdl_audio_device_set_backend" in header
    assert "iplay_sdl_audio_device_backend_name" in header
    assert "iplay_sdl_audio_device_output" in header
    assert "iplay_sdl_audio_device_output_const" in header
    assert "iplay_sdl_audio_device_format" in header
    assert "iplay_sdl_audio_device_sample_rate" in header
    assert "iplay_sdl_audio_device_bits_per_sample" in header
    assert "iplay_sdl_audio_device_channels" in header
    assert "iplay_sdl_audio_device_signed_samples" in header
    assert "iplay_sdl_audio_device_bytes_per_frame" in header
    assert "iplay_sdl_audio_device_samples" in header
    assert "iplay_sdl_audio_device_audio_callback" in header
    assert "iplay_sdl_audio_device_audio_userdata" in header
    assert "iplay_sdl_audio_device_is_sb16_compatible" in header
    assert "iplay_sdl_audio_device_is_sb16_hardware" in header
    assert "iplay_sdl_audio_device_is_sdl_compatible" in header
    assert "iplay_sdl_audio_device_hardware_enabled" in header
    assert "iplay_sdl_audio_device_set_hardware_enabled" in header
    assert "iplay_sdl_audio_device_status_text" in header
    assert "iplay_sdl_audio_device_start" in header
    assert "iplay_sdl_audio_device_stop" in header
    assert "iplay_sdl_audio_device_active" in header
    assert "iplay_sdl_audio_device_pause" in header
    assert "iplay_sdl_audio_device_paused" in header
    assert "iplay_sdl_audio_device_set_paused" in header
    assert "iplay_sdl_audio_device_reset_counters" in header
    assert "iplay_sdl_audio_device_set_capacity" in header
    assert "iplay_sdl_audio_device_add_capacity" in header
    assert "iplay_sdl_audio_device_clear_queued" in header
    assert "iplay_sdl_audio_device_capacity" in header
    assert "iplay_sdl_audio_device_frames_written" in header
    assert "iplay_sdl_audio_device_underrun_frames" in header
    assert "iplay_sdl_audio_device_dropped_frames" in header
    assert "iplay_sdl_audio_device_queued_frames" in header
    assert "iplay_sdl_audio_device_queued_bytes" in header
    assert "iplay_sdl_audio_device_write_sb16_frames" in header
    assert "iplay_sdl_audio_device_can_queue" in header
    assert "iplay_sdl_audio_device_frames_for_bytes" in header
    assert "iplay_sdl_audio_device_bytes_for_frames" in header
    assert "iplay_sdl_audio_device_callback" in header
    assert "iplay_sdl_audio_device_queue" in header
    assert "iplay_sdl_audio_device_queue_frames" in header
    assert "iplay_sdl_audio_device_write_silence" in header
    assert "iplay_sdl_audio_device_levels" in header
    assert "iplay_sdl_audio_device_reset_levels" in header
    assert "iplay_sdl_audio_device_config_sb16_stereo(&config, device, IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE, 0);" in rewrite
    assert "iplay_sdl_audio_device_config_sb16_stereo(&config, device, IPLAY_AUDIO_BACKEND_SB16_STEREO, 1);" in rewrite
    assert "iplay_sdl_audio_device_open(device, &config, write, user)" in rewrite
    assert "void iplay_sdl_audio_device_config_set_format(IplaySdlAudioDeviceConfig *config, const IplayAudioFormat *format)" in rewrite
    assert "#define iplay_sdl_audio_device_config_set_frequency_field(state, value) ((state)->frequency = (value))" in rewrite
    assert "#define iplay_sdl_audio_device_config_set_bits_per_sample_field(state, value) ((state)->bits_per_sample = (value))" in rewrite
    assert "#define iplay_sdl_audio_device_config_set_channels_field(state, value) ((state)->channels = (value))" in rewrite
    assert "#define iplay_sdl_audio_device_config_set_signed_samples_field(state, value) ((state)->signed_samples = (value))" in rewrite
    assert "#define iplay_sdl_audio_device_config_set_samples_field(state, value) ((state)->samples = (value))" in rewrite
    assert "#define iplay_sdl_audio_device_config_set_callback_field(state, value) ((state)->callback = (value))" in rewrite
    assert "#define iplay_sdl_audio_device_config_set_userdata_field(state, value) ((state)->userdata = (value))" in rewrite
    assert "#define iplay_sdl_audio_device_config_set_backend_field(state, value) ((state)->backend = (value))" in rewrite
    assert "#define iplay_sdl_audio_device_config_set_hardware_enabled_field(state, value) ((state)->hardware_enabled = (value))" in rewrite
    assert "#define iplay_sdl_audio_device_config_frequency_field(state) ((state)->frequency)" in rewrite
    assert "#define iplay_sdl_audio_device_config_bits_per_sample_field(state) ((state)->bits_per_sample)" in rewrite
    assert "#define iplay_sdl_audio_device_config_channels_field(state) ((state)->channels)" in rewrite
    assert "#define iplay_sdl_audio_device_config_signed_samples_field(state) ((state)->signed_samples)" in rewrite
    assert "#define iplay_sdl_audio_device_config_samples_field(state) ((state)->samples)" in rewrite
    assert "#define iplay_sdl_audio_device_config_callback_field(state) ((state)->callback)" in rewrite
    assert "#define iplay_sdl_audio_device_config_userdata_field(state) ((state)->userdata)" in rewrite
    assert "#define iplay_sdl_audio_device_config_backend_field(state) ((state)->backend)" in rewrite
    assert "#define iplay_sdl_audio_device_config_hardware_enabled_field(state) ((state)->hardware_enabled)" in rewrite
    assert "static void iplay_sdl_audio_device_config_set_frequency_field(IplaySdlAudioDeviceConfig *state, dw frequency)" not in rewrite
    assert "static void iplay_sdl_audio_device_config_set_bits_per_sample_field(IplaySdlAudioDeviceConfig *state, db bits_per_sample)" not in rewrite
    assert "static void iplay_sdl_audio_device_config_set_channels_field(IplaySdlAudioDeviceConfig *state, db channels)" not in rewrite
    assert "static void iplay_sdl_audio_device_config_set_signed_samples_field(IplaySdlAudioDeviceConfig *state, db signed_samples)" not in rewrite
    assert "static void iplay_sdl_audio_device_config_set_samples_field(IplaySdlAudioDeviceConfig *state, dw samples)" not in rewrite
    assert "static void iplay_sdl_audio_device_config_set_callback_field(IplaySdlAudioDeviceConfig *state, IplaySdlAudioCallback callback)" not in rewrite
    assert "static void iplay_sdl_audio_device_config_set_userdata_field(IplaySdlAudioDeviceConfig *state, void *userdata)" not in rewrite
    assert "static void iplay_sdl_audio_device_config_set_backend_field(IplaySdlAudioDeviceConfig *state, IplayAudioBackend backend)" not in rewrite
    assert "static void iplay_sdl_audio_device_config_set_hardware_enabled_field(IplaySdlAudioDeviceConfig *state, db hardware_enabled)" not in rewrite
    assert "static dw iplay_sdl_audio_device_config_frequency_field(const IplaySdlAudioDeviceConfig *state)" not in rewrite
    assert "static db iplay_sdl_audio_device_config_bits_per_sample_field(const IplaySdlAudioDeviceConfig *state)" not in rewrite
    assert "static db iplay_sdl_audio_device_config_channels_field(const IplaySdlAudioDeviceConfig *state)" not in rewrite
    assert "static db iplay_sdl_audio_device_config_signed_samples_field(const IplaySdlAudioDeviceConfig *state)" not in rewrite
    assert "static dw iplay_sdl_audio_device_config_samples_field(const IplaySdlAudioDeviceConfig *state)" not in rewrite
    assert "static IplaySdlAudioCallback iplay_sdl_audio_device_config_callback_field(const IplaySdlAudioDeviceConfig *state)" not in rewrite
    assert "static void *iplay_sdl_audio_device_config_userdata_field(const IplaySdlAudioDeviceConfig *state)" not in rewrite
    assert "static IplayAudioBackend iplay_sdl_audio_device_config_backend_field(const IplaySdlAudioDeviceConfig *state)" not in rewrite
    assert "static db iplay_sdl_audio_device_config_hardware_enabled_field(const IplaySdlAudioDeviceConfig *state)" not in rewrite
    assert "iplay_sdl_audio_device_config_set_frequency_field(config, iplay_audio_format_sample_rate(format));" in rewrite
    assert "iplay_sdl_audio_device_config_set_bits_per_sample_field(config, iplay_audio_format_bits_per_sample(format));" in rewrite
    assert "iplay_sdl_audio_device_config_set_channels_field(config, iplay_audio_format_channels(format));" in rewrite
    assert "iplay_sdl_audio_device_config_set_signed_samples_field(config, iplay_audio_format_signed_samples(format));" in rewrite
    assert "iplay_sdl_audio_device_config_set_samples_field(config, samples);" in rewrite
    assert "iplay_sdl_audio_device_config_set_callback_field(config, callback);" in rewrite
    assert "iplay_sdl_audio_device_config_set_userdata_field(config, userdata);" in rewrite
    assert "iplay_sdl_audio_device_config_set_backend_field(config, backend);" in rewrite
    assert "iplay_sdl_audio_device_config_set_hardware_enabled_field(config, hardware_enabled);" in rewrite
    assert "config->frequency = format->sample_rate;" not in rewrite
    assert "config->bits_per_sample = format->bits_per_sample;" not in rewrite
    assert "config->channels = format->channels;" not in rewrite
    assert "config->signed_samples = format->signed_samples;" not in rewrite
    assert "config->frequency = iplay_audio_format_sample_rate(format);" not in rewrite
    assert "config->bits_per_sample = iplay_audio_format_bits_per_sample(format);" not in rewrite
    assert "config->channels = iplay_audio_format_channels(format);" not in rewrite
    assert "config->signed_samples = iplay_audio_format_signed_samples(format);" not in rewrite
    assert "void iplay_sdl_audio_device_config_set_samples(IplaySdlAudioDeviceConfig *config, dw samples)" in rewrite
    assert "void iplay_sdl_audio_device_config_set_callback(IplaySdlAudioDeviceConfig *config, IplaySdlAudioCallback callback, void *userdata)" in rewrite
    assert "void iplay_sdl_audio_device_config_set_backend(IplaySdlAudioDeviceConfig *config, IplayAudioBackend backend, db hardware_enabled)" in rewrite
    assert "int iplay_sdl_audio_device_config_format(const IplaySdlAudioDeviceConfig *config, IplayAudioFormat *format)" in rewrite
    assert "return iplay_audio_make_source_format(format," in rewrite
    assert "iplay_sdl_audio_device_config_set_format(config, &IPLAY_AUDIO_SB16_STEREO_16);" in rewrite
    assert "iplay_sdl_audio_device_config_set_samples(config, 1024u);" in rewrite
    assert "iplay_sdl_audio_device_config_set_callback(config, iplay_sdl_audio_device_callback, userdata);" in rewrite
    assert "iplay_sdl_audio_device_config_set_backend(config, backend, hardware_enabled);" in rewrite
    assert "config->frequency = IPLAY_AUDIO_SB16_STEREO_16.sample_rate;" not in rewrite
    assert "config->bits_per_sample = IPLAY_AUDIO_SB16_STEREO_16.bits_per_sample;" not in rewrite
    assert "config->channels = IPLAY_AUDIO_SB16_STEREO_16.channels;" not in rewrite
    assert "config->signed_samples = IPLAY_AUDIO_SB16_STEREO_16.signed_samples;" not in rewrite
    assert "config->samples = 1024u;" not in rewrite
    assert "config->callback = iplay_sdl_audio_device_callback;" not in rewrite
    assert "dw iplay_sdl_audio_device_config_frequency(const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "return iplay_sdl_audio_device_config_frequency_field(config);" in rewrite
    assert "db iplay_sdl_audio_device_config_bits_per_sample(const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "return iplay_sdl_audio_device_config_bits_per_sample_field(config);" in rewrite
    assert "db iplay_sdl_audio_device_config_channels(const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "return iplay_sdl_audio_device_config_channels_field(config);" in rewrite
    assert "db iplay_sdl_audio_device_config_signed_samples(const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "return iplay_sdl_audio_device_config_signed_samples_field(config);" in rewrite
    assert "dw iplay_sdl_audio_device_config_samples(const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "return iplay_sdl_audio_device_config_samples_field(config);" in rewrite
    assert "IplaySdlAudioCallback iplay_sdl_audio_device_config_callback(const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "return iplay_sdl_audio_device_config_callback_field(config);" in rewrite
    assert "void *iplay_sdl_audio_device_config_userdata(const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "return iplay_sdl_audio_device_config_userdata_field(config);" in rewrite
    assert "IplayAudioBackend iplay_sdl_audio_device_config_backend(const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "return iplay_sdl_audio_device_config_backend_field(config);" in rewrite
    assert "int iplay_sdl_audio_device_config_hardware_enabled(const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "return iplay_sdl_audio_device_config_hardware_enabled_field(config) != 0;" in rewrite
    assert "int iplay_sdl_audio_device_config_is_sb16_stereo(const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "if (!iplay_sdl_audio_device_config_format(config, &format)) return 0;" in rewrite
    assert "return iplay_audio_format_equals(&format, &IPLAY_AUDIO_SB16_STEREO_16);" in rewrite
    assert "if (!iplay_sdl_audio_device_config_is_sb16_stereo(config)) return 0;" in rewrite
    assert "if (iplay_sdl_audio_device_config_frequency(config) != IPLAY_AUDIO_SB16_STEREO_16.sample_rate) return 0;" not in rewrite
    assert "if (iplay_sdl_audio_device_config_bits_per_sample(config) != IPLAY_AUDIO_SB16_STEREO_16.bits_per_sample) return 0;" not in rewrite
    assert "&& iplay_sdl_audio_device_config_channels(config) == IPLAY_AUDIO_SB16_STEREO_16.channels" not in rewrite
    assert "&& iplay_sdl_audio_device_config_signed_samples(config) == IPLAY_AUDIO_SB16_STEREO_16.signed_samples" not in rewrite
    assert "IplayAudioBackend iplay_sdl_audio_device_backend_raw(const IplaySdlAudioDevice *state)" in header
    assert "void iplay_sdl_audio_device_set_backend_raw(IplaySdlAudioDevice *state, IplayAudioBackend backend)" in header
    assert "IplayAudioBackend iplay_sdl_audio_device_backend_raw(const IplaySdlAudioDevice *state)" in rewrite
    assert "void iplay_sdl_audio_device_set_backend_raw(IplaySdlAudioDevice *state, IplayAudioBackend backend)" in rewrite
    assert "return state->backend;" in rewrite
    assert "state->backend = backend;" in rewrite
    assert "return iplay_sdl_audio_device_backend_raw(device);" in rewrite
    assert "iplay_sdl_audio_device_set_backend_raw(device, backend);" in rewrite
    assert "void iplay_sdl_audio_device_set_hardware_enabled(IplaySdlAudioDevice *device, int enabled)" in rewrite
    assert "db iplay_sdl_audio_device_hardware_enabled_flag(const IplaySdlAudioDevice *state)" in header
    assert "void iplay_sdl_audio_device_set_hardware_enabled_flag(IplaySdlAudioDevice *state, db enabled)" in header
    assert "db iplay_sdl_audio_device_hardware_enabled_flag(const IplaySdlAudioDevice *state)" in rewrite
    assert "void iplay_sdl_audio_device_set_hardware_enabled_flag(IplaySdlAudioDevice *state, db enabled)" in rewrite
    assert "return state->hardware_enabled;" in rewrite
    assert "state->hardware_enabled = enabled;" in rewrite
    assert "return iplay_sdl_audio_device_hardware_enabled_flag(device) != 0;" in rewrite
    assert "iplay_sdl_audio_device_set_hardware_enabled_flag(device, enabled ? 1u : 0u);" in rewrite
    assert "void iplay_sdl_audio_device_set_paused(IplaySdlAudioDevice *device, int paused)" in rewrite
    assert "db iplay_sdl_audio_device_paused_flag(const IplaySdlAudioDevice *state)" in header
    assert "void iplay_sdl_audio_device_set_paused_flag(IplaySdlAudioDevice *state, db paused)" in header
    assert "db iplay_sdl_audio_device_paused_flag(const IplaySdlAudioDevice *state)" in rewrite
    assert "void iplay_sdl_audio_device_set_paused_flag(IplaySdlAudioDevice *state, db paused)" in rewrite
    assert "return state->paused;" in rewrite
    assert "state->paused = paused;" in rewrite
    assert "return iplay_sdl_audio_device_paused_flag(device) != 0;" in rewrite
    assert "iplay_sdl_audio_device_set_paused_flag(device, paused ? 1u : 0u);" in rewrite
    assert "return device->backend;" not in rewrite
    assert "device->backend = backend;" not in rewrite
    assert "return device->hardware_enabled;" not in rewrite
    assert "device->hardware_enabled = enabled;" not in rewrite
    assert "return device->paused;" not in rewrite
    assert "device->paused = paused;" not in rewrite
    assert "device->hardware_enabled = enabled ? 1u : 0u;" not in rewrite
    assert "device->paused = paused ? 1u : 0u;" not in rewrite
    assert "iplay_sdl_audio_device_finish_open(device, config);" in rewrite
    assert "void iplay_sdl_audio_device_apply_config(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "void iplay_sdl_audio_device_finish_open(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "iplay_sdl_audio_device_apply_config(device, config);" in rewrite
    assert "void iplay_sdl_audio_device_set_config(IplaySdlAudioDevice *device, const IplaySdlAudioDeviceConfig *config)" in rewrite
    assert "iplay_sdl_audio_device_set_config(device, config);" in rewrite
    assert "iplay_sdl_audio_device_set_backend(device, iplay_sdl_audio_device_config_backend(config));" in rewrite
    assert "iplay_sdl_audio_device_set_hardware_enabled(device, iplay_sdl_audio_device_config_hardware_enabled(config));" in rewrite
    assert "iplay_sdl_audio_device_set_paused(device, 1);" in rewrite
    assert "iplay_sdl_audio_device_set_paused(device, 0);" in rewrite
    assert "iplay_sdl_audio_spec_set_backend_field(&spec, iplay_sdl_audio_device_backend(device));" in rewrite
    assert "iplay_sdl_audio_spec_set_format_field(&spec, iplay_sdl_audio_device_format(device));" in rewrite
    assert "iplay_sdl_audio_spec_set_hardware_enabled_field(&spec, (db)iplay_sdl_audio_device_hardware_enabled(device));" in rewrite
    assert "return iplay_audio_backend_name(iplay_sdl_audio_device_backend(device));" in rewrite
    assert "int iplay_audio_backend_is_sb16_scope(IplayAudioBackend backend)" in rewrite
    assert "return backend == IPLAY_AUDIO_BACKEND_SB16_STEREO || backend == IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE;" in rewrite
    assert "int iplay_audio_backend_is_sb16_hardware(IplayAudioBackend backend)" in rewrite
    assert "return backend == IPLAY_AUDIO_BACKEND_SB16_STEREO;" in rewrite
    assert "int iplay_audio_backend_is_sdl_compatible(IplayAudioBackend backend)" in rewrite
    assert "return backend == IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE;" in rewrite
    assert "#define iplay_sdl_audio_spec_backend_field(state) ((state)->backend)" in rewrite
    assert "#define iplay_sdl_audio_spec_set_backend_field(state, value) ((state)->backend = (value))" in rewrite
    assert "#define iplay_sdl_audio_spec_format_field(state) (&(state)->format)" in rewrite
    assert "#define iplay_sdl_audio_spec_set_format_field(state, value) ((state)->format = *(value))" in rewrite
    assert "#define iplay_sdl_audio_spec_hardware_enabled_field(state) ((state)->hardware_enabled)" in rewrite
    assert "#define iplay_sdl_audio_spec_set_hardware_enabled_field(state, value) ((state)->hardware_enabled = (value))" in rewrite
    assert "static IplayAudioBackend iplay_sdl_audio_spec_backend_field(const IplaySdlAudioSpec *state)" not in rewrite
    assert "static const IplayAudioFormat *iplay_sdl_audio_spec_format_field(const IplaySdlAudioSpec *state)" not in rewrite
    assert "static db iplay_sdl_audio_spec_hardware_enabled_field(const IplaySdlAudioSpec *state)" not in rewrite
    assert "IplayAudioBackend iplay_sdl_audio_spec_backend(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return iplay_sdl_audio_spec_backend_field(spec);" in rewrite
    assert "const char *iplay_sdl_audio_spec_backend_name(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return iplay_audio_backend_name(iplay_sdl_audio_spec_backend(spec));" in rewrite
    assert "const IplayAudioFormat *iplay_sdl_audio_spec_format(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return iplay_sdl_audio_spec_format_field(spec);" in rewrite
    assert "dw iplay_sdl_audio_spec_sample_rate(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return iplay_audio_format_sample_rate(iplay_sdl_audio_spec_format(spec));" in rewrite
    assert "db iplay_sdl_audio_spec_bits_per_sample(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return iplay_audio_format_bits_per_sample(iplay_sdl_audio_spec_format(spec));" in rewrite
    assert "db iplay_sdl_audio_spec_channels(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return iplay_audio_format_channels(iplay_sdl_audio_spec_format(spec));" in rewrite
    assert "db iplay_sdl_audio_spec_signed_samples(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return iplay_audio_format_signed_samples(iplay_sdl_audio_spec_format(spec));" in rewrite
    assert "int iplay_sdl_audio_spec_hardware_enabled(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return iplay_sdl_audio_spec_hardware_enabled_field(spec) != 0;" in rewrite
    assert "int iplay_sdl_audio_spec_is_sb16_compatible(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return iplay_audio_backend_is_sb16_scope(iplay_sdl_audio_spec_backend(spec))" in rewrite
    assert "int iplay_sdl_audio_spec_is_sb16_hardware(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return iplay_sdl_audio_spec_hardware_enabled(spec)" in rewrite
    assert "&& iplay_audio_backend_is_sb16_hardware(iplay_sdl_audio_spec_backend(spec))" in rewrite
    assert "int iplay_sdl_audio_spec_is_sdl_compatible(const IplaySdlAudioSpec *spec)" in rewrite
    assert "return !iplay_sdl_audio_spec_hardware_enabled(spec)" in rewrite
    assert "&& iplay_audio_backend_is_sdl_compatible(iplay_sdl_audio_spec_backend(spec))" in rewrite
    assert "return spec->backend;" not in rewrite
    assert "return &spec->format;" not in rewrite
    assert "return spec->hardware_enabled != 0;" not in rewrite
    assert "return iplay_audio_backend_is_sb16_scope(iplay_sdl_audio_device_backend(device))" in rewrite
    assert "IplayAudioBackend backend = iplay_sdl_audio_device_backend(device);" not in rewrite
    assert "return (backend == IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE || backend == IPLAY_AUDIO_BACKEND_SB16_STEREO)" not in rewrite
    assert "&& iplay_audio_format_equals(iplay_sdl_audio_device_format(device), &IPLAY_AUDIO_SB16_STEREO_16);" in rewrite
    assert "int iplay_sdl_audio_device_is_sb16_hardware(const IplaySdlAudioDevice *device)" in rewrite
    assert "return iplay_sdl_audio_device_hardware_enabled(device)" in rewrite
    assert "&& iplay_audio_backend_is_sb16_hardware(iplay_sdl_audio_device_backend(device))" in rewrite
    assert "int iplay_sdl_audio_device_is_sdl_compatible(const IplaySdlAudioDevice *device)" in rewrite
    assert "return !iplay_sdl_audio_device_hardware_enabled(device)" in rewrite
    assert "&& iplay_audio_backend_is_sdl_compatible(iplay_sdl_audio_device_backend(device))" in rewrite
    assert "const IplayAudioOutput *iplay_sdl_audio_device_output_const(const IplaySdlAudioDevice *device)" in rewrite
    assert "#define iplay_sdl_audio_device_config_mut_field(state) (&(state)->config)" in rewrite
    assert "#define iplay_sdl_audio_device_config_field(state) (&(state)->config)" in rewrite
    assert "#define iplay_sdl_audio_device_output_field(state) (&(state)->output)" in rewrite
    assert "#define iplay_sdl_audio_device_output_const_field(state) (&(state)->output)" in rewrite
    assert "#define iplay_sdl_audio_device_config_mut_raw(state) (&(state)->config)" not in rewrite
    assert "#define iplay_sdl_audio_device_config_raw(state) (&(state)->config)" not in rewrite
    assert "#define iplay_sdl_audio_device_output_raw(state) (&(state)->output)" not in rewrite
    assert "#define iplay_sdl_audio_device_output_const_raw(state) (&(state)->output)" not in rewrite
    assert "static IplaySdlAudioDeviceConfig *iplay_sdl_audio_device_config_mut_raw(IplaySdlAudioDevice *state)" not in rewrite
    assert "static const IplaySdlAudioDeviceConfig *iplay_sdl_audio_device_config_raw(const IplaySdlAudioDevice *state)" not in rewrite
    assert "static IplayAudioOutput *iplay_sdl_audio_device_output_raw(IplaySdlAudioDevice *state)" not in rewrite
    assert "static const IplayAudioOutput *iplay_sdl_audio_device_output_const_raw(const IplaySdlAudioDevice *state)" not in rewrite
    assert "return iplay_sdl_audio_device_config_mut_field(device);" in rewrite
    assert "return iplay_sdl_audio_device_config_field(device);" in rewrite
    assert "return iplay_sdl_audio_device_output_field(device);" in rewrite
    assert "return iplay_sdl_audio_device_output_const_field(device);" in rewrite
    assert "iplay_audio_output_init_sb16_stereo(iplay_sdl_audio_device_output(device), write, write_user);" in rewrite
    assert "iplay_audio_output_sink_format(iplay_sdl_audio_device_output_const(device))" in rewrite
    assert "dw iplay_sdl_audio_device_sample_rate(const IplaySdlAudioDevice *device)" in rewrite
    assert "return iplay_audio_format_sample_rate(iplay_sdl_audio_device_format(device));" in rewrite
    assert "db iplay_sdl_audio_device_bits_per_sample(const IplaySdlAudioDevice *device)" in rewrite
    assert "return iplay_audio_format_bits_per_sample(iplay_sdl_audio_device_format(device));" in rewrite
    assert "db iplay_sdl_audio_device_channels(const IplaySdlAudioDevice *device)" in rewrite
    assert "return iplay_audio_format_channels(iplay_sdl_audio_device_format(device));" in rewrite
    assert "db iplay_sdl_audio_device_signed_samples(const IplaySdlAudioDevice *device)" in rewrite
    assert "return iplay_audio_format_signed_samples(iplay_sdl_audio_device_format(device));" in rewrite
    assert "IplaySdlAudioDeviceConfig *iplay_sdl_audio_device_config_mut(IplaySdlAudioDevice *device)" in header
    assert "IplaySdlAudioDeviceConfig *iplay_sdl_audio_device_config_mut(IplaySdlAudioDevice *device)" in rewrite
    assert "IplaySdlAudioDeviceConfig *dst = iplay_sdl_audio_device_config_mut(device);" in rewrite
    assert "(void)iplay_sdl_audio_device_config_format(config, &format);" in rewrite
    assert "iplay_sdl_audio_device_config_set_format(dst, &format);" in rewrite
    assert "iplay_sdl_audio_device_config_set_samples(dst, iplay_sdl_audio_device_config_samples(config));" in rewrite
    assert "iplay_sdl_audio_device_config_set_callback(dst, iplay_sdl_audio_device_config_callback(config), iplay_sdl_audio_device_config_userdata(config));" in rewrite
    assert "iplay_sdl_audio_device_config_set_backend(dst, iplay_sdl_audio_device_config_backend(config), (db)iplay_sdl_audio_device_config_hardware_enabled(config));" in rewrite
    assert "dw iplay_sdl_audio_device_samples(const IplaySdlAudioDevice *device)" in rewrite
    assert "return iplay_sdl_audio_device_config_samples(iplay_sdl_audio_device_config(device));" in rewrite
    assert "IplaySdlAudioCallback iplay_sdl_audio_device_audio_callback(const IplaySdlAudioDevice *device)" in rewrite
    assert "return iplay_sdl_audio_device_config_callback(iplay_sdl_audio_device_config(device));" in rewrite
    assert "void *iplay_sdl_audio_device_audio_userdata(const IplaySdlAudioDevice *device)" in rewrite
    assert "return iplay_sdl_audio_device_config_userdata(iplay_sdl_audio_device_config(device));" in rewrite
    assert "iplay_audio_output_start(iplay_sdl_audio_device_output(device));" in rewrite
    assert "iplay_audio_output_stop(iplay_sdl_audio_device_output(device));" in rewrite
    assert "return iplay_audio_output_is_active(iplay_sdl_audio_device_output_const(device));" in rewrite
    assert "return iplay_audio_output_write_sb16_frames(iplay_sdl_audio_device_output(device), pcm, frame_count);" in rewrite
    assert "void iplay_sdl_audio_device_clear_queued(IplaySdlAudioDevice *device)" in rewrite
    assert "iplay_sdl_audio_device_set_capacity(device, 0);" in rewrite
    assert "dd iplay_sdl_audio_device_queued_frames(const IplaySdlAudioDevice *device)" in rewrite
    assert "return iplay_sdl_audio_device_capacity(device);" in rewrite
    assert "dd iplay_sdl_audio_device_queued_bytes(const IplaySdlAudioDevice *device)" in rewrite
    assert "return iplay_sdl_audio_device_queued_frames(device) * iplay_sdl_audio_device_bytes_per_frame(device);" in rewrite
    assert "int iplay_sdl_audio_device_can_queue(const IplaySdlAudioDevice *device)" in rewrite
    assert "dw iplay_sdl_audio_device_frames_for_bytes(const IplaySdlAudioDevice *device, dw byte_count)" in rewrite
    assert "dw iplay_sdl_audio_device_bytes_for_frames(const IplaySdlAudioDevice *device, dw frame_count)" in rewrite
    assert "if (device == 0) return 0;" in rewrite
    assert "if (!iplay_sdl_audio_device_can_queue(device)) return 0;" in rewrite
    assert "frames = iplay_sdl_audio_device_frames_for_bytes(device, byte_count);" in rewrite
    assert "return iplay_sdl_audio_device_queue_frames(device, stream, frames);" in rewrite
    assert "dw iplay_sdl_audio_device_queue_frames(IplaySdlAudioDevice *device, const db *stream, dw frame_count)" in rewrite
    assert "iplay_sdl_audio_device_add_capacity(device, frame_count);" in rewrite
    assert "iplay_sdl_audio_device_bytes_for_frames(device, frame_count)" in rewrite
    assert "iplay_audio_output_write_silence(iplay_sdl_audio_device_output(device), frame_count);" in rewrite
    assert "return iplay_audio_output_levels(iplay_sdl_audio_device_output_const(device));" in rewrite
    assert "iplay_audio_output_reset_levels(iplay_sdl_audio_device_output(device));" in rewrite
    assert "iplay_audio_output_init_sb16_stereo(&device->output" not in rewrite
    assert "iplay_audio_output_start(&device->output)" not in rewrite
    assert "iplay_audio_output_stop(&device->output)" not in rewrite
    assert "iplay_audio_output_write_silence(&device->output" not in rewrite
    assert "return iplay_audio_output_levels(&device->output);" not in rewrite
    assert "iplay_audio_output_reset_levels(&device->output)" not in rewrite
    assert "return &device->config;" not in rewrite
    assert "return &device->output;" not in rewrite
    assert "spec.format = *iplay_audio_output_sink_format(iplay_sdl_audio_device_output_const(device));" not in rewrite
    assert "spec.backend = iplay_sdl_audio_device_backend(device);" not in rewrite
    assert "spec.format = *iplay_sdl_audio_device_format(device);" not in rewrite
    assert "spec.hardware_enabled = (db)iplay_sdl_audio_device_hardware_enabled(device);" not in rewrite
    assert "iplay_audio_output_is_sb16_stereo(iplay_sdl_audio_device_output_const(device))" not in rewrite
    assert rewrite.count("frame_bytes = iplay_sdl_audio_device_bytes_per_frame(device);") == 1
    assert "frames = (dw)(byte_count / frame_bytes);" not in rewrite
    assert rewrite.count("(dw)(frames * frame_bytes)") == 1
    assert "spec.backend = device->backend;" not in rewrite
    assert "spec.hardware_enabled = device->hardware_enabled;" not in rewrite
    assert "return iplay_audio_backend_name(device->backend);" not in rewrite
    assert "device->backend == IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE" not in rewrite
    assert "if (config->frequency != IPLAY_AUDIO_SB16_STEREO_16.sample_rate) return 0;" not in rewrite
    assert "device->config = *config;" not in rewrite
    assert "device->backend = config->backend;" not in rewrite
    assert "device->hardware_enabled = config->hardware_enabled;" not in rewrite
    assert rewrite.count("iplay_sdl_audio_device_set_config(device, config);") == 1
    assert rewrite.count("iplay_sdl_audio_device_set_backend(device, iplay_sdl_audio_device_config_backend(config));") == 1
    assert rewrite.count("iplay_sdl_audio_device_set_paused(device, 1);") == 2
    assert "device->paused = 1;" not in rewrite
    assert "device->paused = 0;" not in rewrite
    assert "iplay_runtime_init_vga_sb16" in header
    assert "iplay_runtime_init_vga_sdl_audio" in header
    assert "iplay_runtime_init_vga_sb16_present" in header
    assert "iplay_runtime_init_callbacks" in header
    assert "iplay_runtime_init_callbacks_capacity" in header
    assert "iplay_runtime_init_config" in header
    assert "iplay_runtime_start_config" in header
    assert "iplay_runtime_start_config_checked" in header
    assert "iplay_runtime_output_spec_init" in header
    assert "iplay_runtime_output_spec_video_backend" in header
    assert "iplay_runtime_output_spec_audio_backend" in header
    assert "iplay_runtime_output_spec_audio_hardware_enabled" in header
    assert "iplay_runtime_output_spec_sdl" in header
    assert "iplay_runtime_output_spec_sb16_hardware" in header
    assert "iplay_runtime_config_output_capacity" in header
    assert "iplay_runtime_config_set_video_memory" in header
    assert "iplay_runtime_config_set_video_present" in header
    assert "iplay_runtime_config_set_video_backend" in header
    assert "iplay_runtime_config_set_video" in header
    assert "iplay_runtime_config_set_audio_sink" in header
    assert "iplay_runtime_config_set_audio_backend" in header
    assert "iplay_runtime_config_set_audio" in header
    assert "iplay_runtime_config_no_hardware" in header
    assert "iplay_runtime_config_no_hardware_capacity" in header
    assert "iplay_runtime_config_sb16_hardware" in header
    assert "iplay_runtime_config_sb16_hardware_capacity" in header
    assert "iplay_runtime_config_sdl" in header
    assert "iplay_runtime_config_sdl_capacity" in header
    assert "void iplay_runtime_output_spec_init(IplayRuntimeOutputSpec *spec" in rewrite
    assert "#define iplay_runtime_output_spec_set_video_backend_field(state, value) ((state)->video_backend = (value))" in rewrite
    assert "#define iplay_runtime_output_spec_set_audio_backend_field(state, value) ((state)->audio_backend = (value))" in rewrite
    assert "#define iplay_runtime_output_spec_set_audio_hardware_enabled_field(state, value) ((state)->audio_hardware_enabled = (value))" in rewrite
    assert "#define iplay_runtime_output_spec_video_backend_field(state) ((state)->video_backend)" in rewrite
    assert "#define iplay_runtime_output_spec_audio_backend_field(state) ((state)->audio_backend)" in rewrite
    assert "#define iplay_runtime_output_spec_audio_hardware_enabled_field(state) ((state)->audio_hardware_enabled)" in rewrite
    assert "state->video_backend = video_backend;" not in rewrite
    assert "state->audio_backend = audio_backend;" not in rewrite
    assert "state->audio_hardware_enabled = audio_hardware_enabled;" not in rewrite
    assert "static IplayTerminalBackend iplay_runtime_output_spec_video_backend_field(const IplayRuntimeOutputSpec *state)" not in rewrite
    assert "static IplayAudioBackend iplay_runtime_output_spec_audio_backend_field(const IplayRuntimeOutputSpec *state)" not in rewrite
    assert "static db iplay_runtime_output_spec_audio_hardware_enabled_field(const IplayRuntimeOutputSpec *state)" not in rewrite
    assert "iplay_runtime_output_spec_set_video_backend_field(spec, video_backend);" in rewrite
    assert "iplay_runtime_output_spec_set_audio_backend_field(spec, audio_backend);" in rewrite
    assert "iplay_runtime_output_spec_set_audio_hardware_enabled_field(spec, audio_hardware_enabled);" in rewrite
    assert "IplayTerminalBackend iplay_runtime_output_spec_video_backend(const IplayRuntimeOutputSpec *spec)" in rewrite
    assert "return iplay_runtime_output_spec_video_backend_field(spec);" in rewrite
    assert "IplayAudioBackend iplay_runtime_output_spec_audio_backend(const IplayRuntimeOutputSpec *spec)" in rewrite
    assert "return iplay_runtime_output_spec_audio_backend_field(spec);" in rewrite
    assert "int iplay_runtime_output_spec_audio_hardware_enabled(const IplayRuntimeOutputSpec *spec)" in rewrite
    assert "return iplay_runtime_output_spec_audio_hardware_enabled_field(spec) != 0;" in rewrite
    assert "spec->video_backend = video_backend;" not in rewrite
    assert "spec->audio_backend = audio_backend;" not in rewrite
    assert "spec->audio_hardware_enabled = audio_hardware_enabled;" not in rewrite
    assert "return spec->video_backend;" not in rewrite
    assert "return spec->audio_backend;" not in rewrite
    assert "return spec->audio_hardware_enabled != 0;" not in rewrite
    assert "iplay_runtime_output_spec_init(spec, IPLAY_TERMINAL_BACKEND_VGA_MEMORY, IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE, 0);" in rewrite
    assert "iplay_runtime_output_spec_init(spec, IPLAY_TERMINAL_BACKEND_VGA_MEMORY, IPLAY_AUDIO_BACKEND_SB16_STEREO, 1);" in rewrite
    assert "void iplay_runtime_config_output_capacity(IplayRuntimeConfig *config" in rewrite
    assert "const IplayRuntimeOutputSpec *output" in rewrite
    assert "void iplay_runtime_config_set_video_memory(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode)" in rewrite
    assert "void iplay_runtime_config_set_video_present(IplayRuntimeConfig *config, IplayVideoPresentFn present, void *present_user)" in rewrite
    assert "void iplay_runtime_config_set_video_backend(IplayRuntimeConfig *config, IplayTerminalBackend backend)" in rewrite
    assert "void iplay_runtime_config_set_video(IplayRuntimeConfig *config, db *cells, dw cell_capacity_bytes, const IplayTextMode *mode, IplayTerminalBackend backend, IplayVideoPresentFn present, void *present_user)" in rewrite
    assert "void iplay_runtime_config_set_audio_sink(IplayRuntimeConfig *config, IplayAudioWriteFn audio_write, void *audio_user)" in rewrite
    assert "void iplay_runtime_config_set_audio_backend(IplayRuntimeConfig *config, IplayAudioBackend backend, db hardware_enabled)" in rewrite
    assert "void iplay_runtime_config_set_audio(IplayRuntimeConfig *config, IplayAudioWriteFn audio_write, void *audio_user, IplayAudioBackend backend, db hardware_enabled)" in rewrite
    assert "iplay_runtime_config_set_video(config, cells, cell_capacity_bytes, mode, iplay_runtime_output_spec_video_backend(output), present, present_user);" in rewrite
    assert "iplay_runtime_config_set_audio(config, audio_write, audio_user, iplay_runtime_output_spec_audio_backend(output), (db)iplay_runtime_output_spec_audio_hardware_enabled(output));" in rewrite
    assert "iplay_runtime_config_set_video_memory(config, cells, cell_capacity_bytes, mode);" in rewrite
    assert "iplay_runtime_config_set_video_present(config, present, present_user);" in rewrite
    assert "iplay_runtime_config_set_video_backend(config, backend);" in rewrite
    assert "iplay_runtime_config_set_audio_sink(config, audio_write, audio_user);" in rewrite
    assert "iplay_runtime_config_set_audio_backend(config, backend, hardware_enabled);" in rewrite
    assert "#define iplay_runtime_config_set_cells_field(state, value) ((state)->cells = (value))" in rewrite
    assert "#define iplay_runtime_config_set_cell_capacity_field(state, value) ((state)->cell_capacity_bytes = (value))" in rewrite
    assert "#define iplay_runtime_config_set_mode_field(state, value) ((state)->mode = (value))" in rewrite
    assert "#define iplay_runtime_config_set_present_field(state, value) ((state)->present = (value))" in rewrite
    assert "#define iplay_runtime_config_set_present_user_field(state, value) ((state)->present_user = (value))" in rewrite
    assert "#define iplay_runtime_config_set_video_present_enabled_field(state, value) ((state)->video_present_enabled = (value))" in rewrite
    assert "#define iplay_runtime_config_set_video_backend_field(state, value) ((state)->video_backend = (value))" in rewrite
    assert "#define iplay_runtime_config_cells_field(state) ((state)->cells)" in rewrite
    assert "#define iplay_runtime_config_cell_capacity_field(state) ((state)->cell_capacity_bytes)" in rewrite
    assert "#define iplay_runtime_config_mode_field(state) ((state)->mode)" in rewrite
    assert "#define iplay_runtime_config_video_backend_field(state) ((state)->video_backend)" in rewrite
    assert "#define iplay_runtime_config_present_field(state) ((state)->present)" in rewrite
    assert "#define iplay_runtime_config_present_user_field(state) ((state)->present_user)" in rewrite
    assert "#define iplay_runtime_config_video_present_enabled_field(state) ((state)->video_present_enabled)" in rewrite
    assert "static void iplay_runtime_config_set_cells_field(IplayRuntimeConfig *state, db *cells)" not in rewrite
    assert "static void iplay_runtime_config_set_cell_capacity_field(IplayRuntimeConfig *state, dw cell_capacity_bytes)" not in rewrite
    assert "static void iplay_runtime_config_set_mode_field(IplayRuntimeConfig *state, const IplayTextMode *mode)" not in rewrite
    assert "static void iplay_runtime_config_set_present_field(IplayRuntimeConfig *state, IplayVideoPresentFn present)" not in rewrite
    assert "static void iplay_runtime_config_set_present_user_field(IplayRuntimeConfig *state, void *present_user)" not in rewrite
    assert "static void iplay_runtime_config_set_video_present_enabled_field(IplayRuntimeConfig *state, db enabled)" not in rewrite
    assert "state->video_backend = backend;" not in rewrite
    assert "static db *iplay_runtime_config_cells_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "static dw iplay_runtime_config_cell_capacity_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "static const IplayTextMode *iplay_runtime_config_mode_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "static IplayTerminalBackend iplay_runtime_config_video_backend_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "static IplayVideoPresentFn iplay_runtime_config_present_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "static void *iplay_runtime_config_present_user_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "static db iplay_runtime_config_video_present_enabled_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "iplay_runtime_config_set_cells_field(config, cells);" in rewrite
    assert "iplay_runtime_config_set_cell_capacity_field(config, cell_capacity_bytes);" in rewrite
    assert "iplay_runtime_config_set_mode_field(config, mode);" in rewrite
    assert "iplay_runtime_config_set_present_field(config, present);" in rewrite
    assert "iplay_runtime_config_set_present_user_field(config, present_user);" in rewrite
    assert "iplay_runtime_config_set_video_present_enabled_field(config, present != 0);" in rewrite
    assert "iplay_runtime_config_set_video_backend_field(config, backend);" in rewrite
    assert "#define iplay_runtime_config_set_audio_write_field(state, value) ((state)->audio_write = (value))" in rewrite
    assert "#define iplay_runtime_config_set_audio_user_field(state, value) ((state)->audio_user = (value))" in rewrite
    assert "#define iplay_runtime_config_set_audio_backend_field(state, value) ((state)->audio_backend = (value))" in rewrite
    assert "#define iplay_runtime_config_set_audio_hardware_enabled_field(state, value) ((state)->audio_hardware_enabled = (value))" in rewrite
    assert "#define iplay_runtime_config_audio_write_field(state) ((state)->audio_write)" in rewrite
    assert "#define iplay_runtime_config_audio_user_field(state) ((state)->audio_user)" in rewrite
    assert "#define iplay_runtime_config_audio_backend_field(state) ((state)->audio_backend)" in rewrite
    assert "#define iplay_runtime_config_audio_hardware_enabled_field(state) ((state)->audio_hardware_enabled)" in rewrite
    assert "static void iplay_runtime_config_set_audio_write_field(IplayRuntimeConfig *state, IplayAudioWriteFn audio_write)" not in rewrite
    assert "static void iplay_runtime_config_set_audio_user_field(IplayRuntimeConfig *state, void *audio_user)" not in rewrite
    assert "state->audio_backend = backend;" not in rewrite
    assert "state->audio_hardware_enabled = hardware_enabled;" not in rewrite
    assert "static IplayAudioWriteFn iplay_runtime_config_audio_write_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "static void *iplay_runtime_config_audio_user_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "static IplayAudioBackend iplay_runtime_config_audio_backend_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "static db iplay_runtime_config_audio_hardware_enabled_field(const IplayRuntimeConfig *state)" not in rewrite
    assert "iplay_runtime_config_set_audio_write_field(config, audio_write);" in rewrite
    assert "iplay_runtime_config_set_audio_user_field(config, audio_user);" in rewrite
    assert "iplay_runtime_config_set_audio_backend_field(config, backend);" in rewrite
    assert "iplay_runtime_config_set_audio_hardware_enabled_field(config, hardware_enabled);" in rewrite
    assert (
        "config->cells = cells;\n"
        "    config->cell_capacity_bytes = cell_capacity_bytes;\n"
        "    config->mode = mode;\n"
        "    config->present = present;\n"
        "    config->present_user = present_user;\n"
        "    config->video_backend = backend;\n"
        "    config->video_present_enabled = present != 0;"
    ) not in rewrite
    assert (
        "config->audio_write = audio_write;\n"
        "    config->audio_user = audio_user;\n"
        "    config->audio_backend = backend;\n"
        "    config->audio_hardware_enabled = hardware_enabled;"
    ) not in rewrite
    assert "config->video_backend = iplay_runtime_output_spec_video_backend(output);" not in rewrite
    assert "config->audio_backend = iplay_runtime_output_spec_audio_backend(output);" not in rewrite
    assert "config->audio_hardware_enabled = (db)iplay_runtime_output_spec_audio_hardware_enabled(output);" not in rewrite
    assert "config->video_backend = output->video_backend;" not in rewrite
    assert "config->audio_backend = output->audio_backend;" not in rewrite
    assert "config->audio_hardware_enabled = output->audio_hardware_enabled;" not in rewrite
    assert "config->cells = cells;" not in rewrite
    assert "config->cell_capacity_bytes = cell_capacity_bytes;" not in rewrite
    assert "config->mode = mode;" not in rewrite
    assert "config->present = present;" not in rewrite
    assert "config->present_user = present_user;" not in rewrite
    assert "config->video_present_enabled = present != 0;" not in rewrite
    assert "config->video_backend = backend;" not in rewrite
    assert "config->audio_write = audio_write;" not in rewrite
    assert "config->audio_user = audio_user;" not in rewrite
    assert "config->audio_backend = backend;" not in rewrite
    assert "config->audio_hardware_enabled = hardware_enabled;" not in rewrite
    assert "IplayRuntimeOutputSpec output;" in rewrite
    assert "iplay_runtime_output_spec_sdl(&output);" in rewrite
    assert "iplay_runtime_output_spec_sb16_hardware(&output);" in rewrite
    assert "iplay_runtime_config_output_capacity(config, cells, cell_capacity_bytes, mode, 0, 0, audio_write, audio_user, &output);" in rewrite
    assert "iplay_runtime_config_output_capacity(config, cells, cell_capacity_bytes, mode, present, present_user, audio_write, audio_user, &output);" in rewrite
    assert "iplay_runtime_config_output_capacity(config, cells, cell_capacity_bytes, mode, present, present_user, audio_write, audio_user, IPLAY_TERMINAL_BACKEND_VGA_MEMORY" not in rewrite
    assert "iplay_runtime_config_has_video_present" in header
    assert "iplay_runtime_config_has_audio_sink" in header
    assert "iplay_runtime_config_has_cell_capacity" in header
    assert "iplay_runtime_config_cells" in header
    assert "iplay_runtime_config_cell_capacity" in header
    assert "iplay_runtime_config_mode" in header
    assert "iplay_runtime_config_video_backend" in header
    assert "iplay_runtime_config_present" in header
    assert "iplay_runtime_config_present_user" in header
    assert "iplay_runtime_config_video_present_enabled" in header
    assert "iplay_runtime_config_audio_write" in header
    assert "iplay_runtime_config_audio_user" in header
    assert "iplay_runtime_config_audio_backend" in header
    assert "iplay_runtime_config_audio_hardware_enabled" in header
    assert "iplay_runtime_config_uses_sb16_hardware" in header
    assert "IplayAudioBackend iplay_runtime_config_audio_backend(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_audio_backend_field(config);" in rewrite
    assert "int iplay_runtime_config_audio_hardware_enabled(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_audio_hardware_enabled_field(config) != 0;" in rewrite
    assert "int iplay_runtime_config_uses_sb16_hardware(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_audio_hardware_enabled(config) || iplay_audio_backend_is_sb16_hardware(iplay_runtime_config_audio_backend(config));" in rewrite
    assert "return iplay_runtime_config_audio_hardware_enabled(config) || iplay_runtime_config_audio_backend(config) == IPLAY_AUDIO_BACKEND_SB16_STEREO;" not in rewrite
    assert "if (iplay_runtime_config_uses_sb16_hardware(config))" in rewrite
    assert "if (config->audio_hardware_enabled != 0 || config->audio_backend == IPLAY_AUDIO_BACKEND_SB16_STEREO)" not in rewrite
    assert "db *iplay_runtime_config_cells(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_cells_field(config);" in rewrite
    assert "dw iplay_runtime_config_cell_capacity(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_cell_capacity_field(config);" in rewrite
    assert "const IplayTextMode *iplay_runtime_config_mode(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_mode_field(config);" in rewrite
    assert "IplayTerminalBackend iplay_runtime_config_video_backend(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_video_backend_field(config);" in rewrite
    assert "IplayVideoPresentFn iplay_runtime_config_present(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_present_field(config);" in rewrite
    assert "void *iplay_runtime_config_present_user(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_present_user_field(config);" in rewrite
    assert "int iplay_runtime_config_video_present_enabled(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_video_present_enabled_field(config) != 0;" in rewrite
    assert "IplayAudioWriteFn iplay_runtime_config_audio_write(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_audio_write_field(config);" in rewrite
    assert "void *iplay_runtime_config_audio_user(const IplayRuntimeConfig *config)" in rewrite
    assert "return iplay_runtime_config_audio_user_field(config);" in rewrite
    assert "return iplay_runtime_config_video_present_enabled(config) && iplay_runtime_config_present(config) != 0;" in rewrite
    assert "return config->video_present_enabled != 0 && iplay_runtime_config_present(config) != 0;" not in rewrite
    assert "return iplay_runtime_config_audio_write(config) != 0;" in rewrite
    assert "return iplay_text_mode_fits_capacity(iplay_runtime_config_mode(config), iplay_runtime_config_cell_capacity(config));" in rewrite
    assert "return iplay_runtime_config_cell_capacity(config) >= iplay_text_mode_screen_bytes(mode);" not in rewrite
    assert "if (iplay_runtime_config_cells(config) == 0) return IPLAY_RUNTIME_CONFIG_MISSING_CELLS;" in rewrite
    assert "if (iplay_runtime_config_mode(config) == 0) return IPLAY_RUNTIME_CONFIG_MISSING_MODE;" in rewrite
    assert "return config->video_present_enabled != 0 && config->present != 0;" not in rewrite
    assert "return config->audio_write != 0;" not in rewrite
    assert "if (config->mode == 0) return 0;" not in rewrite
    assert "return config->cell_capacity_bytes >= iplay_text_mode_screen_bytes(config->mode);" not in rewrite
    assert "if (config->cells == 0) return IPLAY_RUNTIME_CONFIG_MISSING_CELLS;" not in rewrite
    assert "if (config->mode == 0) return IPLAY_RUNTIME_CONFIG_MISSING_MODE;" not in rewrite
    assert "iplay_notcurses_init_vga_memory_capacity(iplay_runtime_notcurses(runtime), iplay_runtime_config_cells(config), iplay_runtime_config_cell_capacity(config), iplay_runtime_config_mode(config));" in rewrite
    assert "iplay_sdl_audio_device_init_sb16_hardware(iplay_runtime_audio(runtime), iplay_runtime_config_audio_write(config), iplay_runtime_config_audio_user(config));" in rewrite
    assert "iplay_sdl_audio_device_init_sb16_compatible(iplay_runtime_audio(runtime), iplay_runtime_config_audio_write(config), iplay_runtime_config_audio_user(config));" in rewrite
    assert "iplay_notcurses_init_vga_memory_capacity(iplay_runtime_notcurses(runtime), cells, iplay_text_mode_screen_bytes(mode), mode);" in rewrite
    assert "iplay_notcurses_init_vga_memory_capacity(iplay_runtime_notcurses(runtime), cells, cell_capacity_bytes, mode);" in rewrite
    assert "iplay_sdl_audio_device_init_sb16_hardware(iplay_runtime_audio(runtime), write, user);" in rewrite
    assert "iplay_sdl_audio_device_init_sb16_compatible(iplay_runtime_audio(runtime), audio_write, audio_user);" in rewrite
    assert "iplay_notcurses_init_vga_memory_capacity(&runtime->nc, cells" not in rewrite
    assert "iplay_sdl_audio_device_init_sb16_hardware(&runtime->audio" not in rewrite
    assert "iplay_sdl_audio_device_init_sb16_compatible(&runtime->audio" not in rewrite
    assert "iplay_notcurses_set_present_callback(iplay_runtime_notcurses(runtime), present, present_user);" in rewrite
    assert "iplay_notcurses_set_present_callback(iplay_runtime_notcurses(runtime), iplay_runtime_config_present(config), iplay_runtime_config_present_user(config));" in rewrite
    assert "iplay_terminal_set_present_callback(iplay_runtime_terminal(runtime)" not in rewrite
    assert "iplay_notcurses_init_vga_memory_capacity(&runtime->nc, config->cells, config->cell_capacity_bytes, config->mode);" not in rewrite
    assert "iplay_sdl_audio_device_init_sb16_hardware(&runtime->audio, config->audio_write, config->audio_user);" not in rewrite
    assert "iplay_sdl_audio_device_init_sb16_compatible(&runtime->audio, config->audio_write, config->audio_user);" not in rewrite
    assert "iplay_terminal_set_present_callback(iplay_notcurses_terminal(&runtime->nc), config->present, config->present_user);" not in rewrite
    assert "iplay_runtime_config_error" in header
    assert "iplay_runtime_config_error_name" in header
    assert "iplay_runtime_config_is_valid" in header
    assert "iplay_runtime_shutdown" in header
    assert "iplay_runtime_notcurses" in header
    assert "iplay_runtime_notcurses_const" in header
    assert "iplay_runtime_terminal" in header
    assert "iplay_runtime_terminal_const" in header
    assert "iplay_notcurses_terminal_const" in header
    assert "const IplayTerminal *iplay_notcurses_terminal_const(const IplayNotcurses *nc)" in rewrite
    assert "IplayTerminal *iplay_runtime_terminal(IplayRuntime *runtime)" in rewrite
    assert "const IplayNotcurses *iplay_runtime_notcurses_const(const IplayRuntime *runtime)" in rewrite
    assert "const IplayTerminal *iplay_runtime_terminal_const(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_notcurses_terminal(iplay_runtime_notcurses(runtime));" in rewrite
    assert "return iplay_notcurses_terminal_const(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "return &iplay_runtime_notcurses_const(runtime)->terminal;" not in rewrite
    assert "iplay_runtime_audio" in header
    assert "iplay_runtime_audio_const" in header
    assert "IplaySdlAudioDevice *iplay_runtime_audio(IplayRuntime *runtime)" in rewrite
    assert "const IplaySdlAudioDevice *iplay_runtime_audio_const(const IplayRuntime *runtime)" in rewrite
    assert "#define iplay_runtime_notcurses_field(state) (&(state)->nc)" in rewrite
    assert "#define iplay_runtime_notcurses_const_field(state) (&(state)->nc)" in rewrite
    assert "#define iplay_runtime_audio_field(state) (&(state)->audio)" in rewrite
    assert "#define iplay_runtime_audio_const_field(state) (&(state)->audio)" in rewrite
    assert "#define iplay_runtime_notcurses_raw(state) (&(state)->nc)" not in rewrite
    assert "#define iplay_runtime_notcurses_const_raw(state) (&(state)->nc)" not in rewrite
    assert "#define iplay_runtime_audio_raw(state) (&(state)->audio)" not in rewrite
    assert "#define iplay_runtime_audio_const_raw(state) (&(state)->audio)" not in rewrite
    assert "static IplayNotcurses *iplay_runtime_notcurses_raw(IplayRuntime *state)" not in rewrite
    assert "static const IplayNotcurses *iplay_runtime_notcurses_const_raw(const IplayRuntime *state)" not in rewrite
    assert "static IplaySdlAudioDevice *iplay_runtime_audio_raw(IplayRuntime *state)" not in rewrite
    assert "static const IplaySdlAudioDevice *iplay_runtime_audio_const_raw(const IplayRuntime *state)" not in rewrite
    assert "return iplay_runtime_notcurses_field(runtime);" in rewrite
    assert "return iplay_runtime_notcurses_const_field(runtime);" in rewrite
    assert "return iplay_runtime_audio_field(runtime);" in rewrite
    assert "return iplay_runtime_audio_const_field(runtime);" in rewrite
    assert "return &runtime->nc;" not in rewrite
    assert "return &runtime->audio;" not in rewrite
    assert "iplay_runtime_stdplane" in header
    assert "return iplay_notcurses_stdplane(iplay_runtime_notcurses(runtime));" in rewrite
    assert "iplay_window_init_root(&window, iplay_runtime_stdplane(runtime));" in rewrite
    assert "iplay_window_init_root(&window, iplay_notcurses_stdplane(&runtime->nc));" not in rewrite
    assert "iplay_runtime_video_spec" in header
    assert "iplay_runtime_video_backend" in header
    assert "iplay_runtime_video_present_enabled" in header
    assert "iplay_runtime_video_has_present" in header
    assert "iplay_runtime_video_present_callback" in header
    assert "iplay_runtime_video_present_user" in header
    assert "iplay_runtime_video_set_present_fn" in header
    assert "iplay_runtime_video_set_present_user" in header
    assert "iplay_runtime_video_set_present_callback" in header
    assert "iplay_runtime_video_clear_present_callback" in header
    assert "iplay_runtime_video_mode" in header
    assert "iplay_runtime_video_cells_const" in header
    assert "iplay_runtime_video_checksum" in header
    assert "iplay_runtime_video_nonblank_cells" in header
    assert "iplay_runtime_video_capacity" in header
    assert "iplay_runtime_video_cols" in header
    assert "iplay_runtime_video_rows" in header
    assert "iplay_runtime_video_row_bytes" in header
    assert "iplay_runtime_video_screen_bytes" in header
    assert "iplay_runtime_bottom_layout_fits" in header
    assert "int iplay_terminal_has_present(const IplayTerminal *terminal)" in rewrite
    assert "void iplay_terminal_set_backend(IplayTerminal *terminal, IplayTerminalBackend backend)" in rewrite
    assert "void iplay_terminal_set_present_fn(IplayTerminal *terminal, IplayVideoPresentFn present)" in rewrite
    assert "void iplay_terminal_set_present_user(IplayTerminal *terminal, void *user)" in rewrite
    assert "iplay_terminal_set_present_fn(terminal, present);" in rewrite
    assert "iplay_terminal_set_present_user(terminal, user);" in rewrite
    assert "void iplay_terminal_clear_present_callback(IplayTerminal *terminal)" in rewrite
    assert "iplay_terminal_set_backend(terminal, IPLAY_TERMINAL_BACKEND_VGA_MEMORY);" in rewrite
    assert "iplay_terminal_clear_present_callback(terminal);" in rewrite
    assert "iplay_terminal_set_present_callback(terminal, 0, 0);" in rewrite
    assert "return iplay_terminal_present_callback(terminal) != 0;" in rewrite
    assert "const IplayTextScreen *iplay_terminal_screen_const(const IplayTerminal *terminal)" in rewrite
    assert "db *iplay_terminal_cells(IplayTerminal *terminal)" in rewrite
    assert "IplayVideoPresentFn iplay_terminal_present_callback(const IplayTerminal *terminal)" in rewrite
    assert "void *iplay_terminal_present_user(const IplayTerminal *terminal)" in rewrite
    assert "dw iplay_terminal_capacity(const IplayTerminal *terminal)" in rewrite
    assert "int iplay_terminal_bottom_layout_fits(const IplayTerminal *terminal)" in rewrite
    assert "present(iplay_terminal_present_user(terminal), iplay_terminal_cells_const(terminal), mode, bytes);" in rewrite
    assert "iplay_text_screen_draw_top_title(iplay_terminal_screen(terminal));" in rewrite
    assert "return iplay_notcurses_video_spec(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "IplayTerminalBackend iplay_runtime_video_backend(const IplayRuntime *runtime)" in rewrite
    assert "IplayVideoSpec spec = iplay_runtime_video_spec(runtime);" in rewrite
    assert "return iplay_video_spec_backend(&spec);" in rewrite
    assert "int iplay_runtime_video_present_enabled(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_video_spec_present_enabled(&spec);" in rewrite
    assert "int iplay_runtime_video_has_present(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_notcurses_has_present(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "IplayVideoPresentFn iplay_runtime_video_present_callback(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_notcurses_present_callback(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "void *iplay_runtime_video_present_user(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_notcurses_present_user(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "void iplay_runtime_video_set_present_fn(IplayRuntime *runtime, IplayVideoPresentFn present)" in rewrite
    assert "iplay_notcurses_set_present_fn(iplay_runtime_notcurses(runtime), present);" in rewrite
    assert "void iplay_runtime_video_set_present_user(IplayRuntime *runtime, void *user)" in rewrite
    assert "iplay_notcurses_set_present_user(iplay_runtime_notcurses(runtime), user);" in rewrite
    assert "void iplay_runtime_video_set_present_callback(IplayRuntime *runtime, IplayVideoPresentFn present, void *user)" in rewrite
    assert "iplay_runtime_video_set_present_fn(runtime, present);" in rewrite
    assert "iplay_runtime_video_set_present_user(runtime, user);" in rewrite
    assert "void iplay_runtime_video_clear_present_callback(IplayRuntime *runtime)" in rewrite
    assert "iplay_runtime_video_set_present_callback(runtime, 0, 0);" in rewrite
    assert "const IplayTerminal *terminal = iplay_runtime_terminal_const(runtime);" not in rewrite
    assert "return iplay_notcurses_mode(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "const db *iplay_runtime_video_cells_const(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_terminal_cells_const(iplay_runtime_terminal_const(runtime));" in rewrite
    assert "dd iplay_runtime_video_checksum(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_text_cells_checksum(iplay_runtime_video_cells_const(runtime), iplay_runtime_video_screen_bytes(runtime));" in rewrite
    assert "dw iplay_runtime_video_nonblank_cells(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_text_cells_nonblank_count(iplay_runtime_video_cells_const(runtime), iplay_runtime_video_screen_bytes(runtime));" in rewrite
    assert "return iplay_notcurses_capacity(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "return iplay_notcurses_cols(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "return iplay_notcurses_rows(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "return iplay_notcurses_row_bytes(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "return iplay_notcurses_screen_bytes(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "return iplay_text_mode_cols(iplay_runtime_video_mode(runtime));" not in rewrite
    assert "return iplay_text_mode_rows(iplay_runtime_video_mode(runtime));" not in rewrite
    assert "return iplay_text_mode_row_bytes(iplay_runtime_video_mode(runtime));" not in rewrite
    assert "return iplay_text_mode_screen_bytes(iplay_runtime_video_mode(runtime));" not in rewrite
    assert "return iplay_notcurses_bottom_layout_fits(iplay_runtime_notcurses_const(runtime));" in rewrite
    assert "return iplay_terminal_capacity(iplay_runtime_terminal_const(runtime));" not in rewrite
    assert "return iplay_terminal_bottom_layout_fits(iplay_runtime_terminal_const(runtime));" not in rewrite
    assert "iplay_notcurses_resize_checked(iplay_runtime_notcurses(runtime), mode)" in rewrite
    assert "iplay_notcurses_set_video_mode_checked(iplay_runtime_notcurses(runtime), video_mode)" in rewrite
    assert "iplay_notcurses_render_static(iplay_runtime_notcurses(runtime), erase_attr);" in rewrite
    assert "iplay_notcurses_render_bottom(iplay_runtime_notcurses(runtime), byte_1de72" in rewrite
    assert "return iplay_notcurses_present(iplay_runtime_notcurses(runtime));" in rewrite
    assert "return iplay_terminal_present(iplay_runtime_terminal(runtime));" not in rewrite
    assert "const IplayTerminal *terminal = &runtime->nc.terminal;" not in rewrite
    assert "spec.backend = iplay_terminal_backend(terminal);" not in rewrite
    assert "spec.mode = *iplay_terminal_mode(terminal);" not in rewrite
    assert "spec.present_enabled = (db)iplay_terminal_has_present(terminal);" not in rewrite
    assert "spec.backend = terminal->backend;" not in rewrite
    assert "spec.present_enabled = terminal->present != 0;" not in rewrite
    assert "return terminal->present != 0;" not in rewrite
    assert "terminal->backend = IPLAY_TERMINAL_BACKEND_VGA_MEMORY;" not in rewrite
    assert "terminal->present = 0;" not in rewrite
    assert "terminal->present_user = 0;" not in rewrite
    assert "iplay_text_screen_draw_top_title(&terminal->screen);" not in rewrite
    assert "iplay_text_screen_draw_bottom(&terminal->screen" not in rewrite
    assert "iplay_text_screen_draw_audio_output_levels(&terminal->screen" not in rewrite
    assert "return iplay_text_screen_capacity(&iplay_runtime_terminal_const(runtime)->screen);" not in rewrite
    assert "return iplay_text_screen_bottom_layout_fits(&iplay_runtime_terminal_const(runtime)->screen);" not in rewrite
    assert "return iplay_text_screen_capacity(&runtime->nc.terminal.screen);" not in rewrite
    assert "return iplay_text_screen_bottom_layout_fits(&runtime->nc.terminal.screen);" not in rewrite
    assert "iplay_notcurses_resize_checked(&runtime->nc, mode)" not in rewrite
    assert "iplay_notcurses_set_video_mode_checked(&runtime->nc, video_mode)" not in rewrite
    assert "iplay_notcurses_render_static(&runtime->nc, erase_attr);" not in rewrite
    assert "iplay_notcurses_render_bottom(&runtime->nc, byte_1de72" not in rewrite
    assert "iplay_runtime_audio_spec" in header
    assert "iplay_runtime_audio_backend" in header
    assert "iplay_runtime_audio_format" in header
    assert "iplay_runtime_audio_sample_rate" in header
    assert "iplay_runtime_audio_bits_per_sample" in header
    assert "iplay_runtime_audio_channels" in header
    assert "iplay_runtime_audio_signed_samples" in header
    assert "iplay_runtime_audio_samples" in header
    assert "iplay_runtime_audio_backend_name" in header
    assert "iplay_runtime_audio_hardware_enabled" in header
    assert "iplay_runtime_audio_status_text" in header
    assert "iplay_runtime_audio_bytes_per_frame" in header
    assert "iplay_runtime_audio_is_sb16_compatible" in header
    assert "iplay_runtime_audio_is_sb16_hardware" in header
    assert "iplay_runtime_audio_is_sdl_compatible" in header
    assert "iplay_runtime_resize" in header
    assert "iplay_runtime_resize_checked" in header
    assert "iplay_runtime_resize_to_size" in header
    assert "iplay_runtime_resize_to_size_checked" in header
    assert "iplay_runtime_set_video_mode" in header
    assert "iplay_runtime_set_video_mode_checked" in header
    assert "iplay_runtime_set_video_mode_ok_flag" in header
    assert "iplay_runtime_set_video_mode_ok" in header
    assert "iplay_runtime_video_mode_ok_flag" in header
    assert "iplay_runtime_video_mode_ok" in header
    assert "dw iplay_runtime_audio_sample_rate(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_sdl_audio_device_sample_rate(iplay_runtime_audio_const(runtime));" in rewrite
    assert "db iplay_runtime_audio_bits_per_sample(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_sdl_audio_device_bits_per_sample(iplay_runtime_audio_const(runtime));" in rewrite
    assert "db iplay_runtime_audio_channels(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_sdl_audio_device_channels(iplay_runtime_audio_const(runtime));" in rewrite
    assert "db iplay_runtime_audio_signed_samples(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_sdl_audio_device_signed_samples(iplay_runtime_audio_const(runtime));" in rewrite
    assert "dw iplay_runtime_audio_samples(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_sdl_audio_device_samples(iplay_runtime_audio_const(runtime));" in rewrite
    assert "void iplay_runtime_set_video_mode_ok_flag(IplayRuntime *runtime, db ok)" in rewrite
    assert "#define iplay_runtime_set_video_mode_ok_field(state, value) ((state)->video_mode_ok = (value))" in rewrite
    assert "#define iplay_runtime_video_mode_ok_field(state) ((state)->video_mode_ok)" in rewrite
    assert "static void iplay_runtime_set_video_mode_ok_field(IplayRuntime *state, db ok)" not in rewrite
    assert "static db iplay_runtime_video_mode_ok_field(const IplayRuntime *state)" not in rewrite
    assert "iplay_runtime_set_video_mode_ok_field(runtime, ok);" in rewrite
    assert "db iplay_runtime_video_mode_ok_flag(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_runtime_video_mode_ok_field(runtime);" in rewrite
    assert "void iplay_runtime_set_video_mode_ok(IplayRuntime *runtime, int ok)" in rewrite
    assert "iplay_runtime_set_video_mode_ok_flag(runtime, ok ? 1u : 0u);" in rewrite
    assert "return iplay_runtime_video_mode_ok_flag(runtime) != 0;" in rewrite
    assert "return iplay_runtime_video_mode_ok(runtime);" in rewrite
    assert "const IplayTextMode *iplay_runtime_resize_to_size(IplayRuntime *runtime, dw cols, dw rows)" in rewrite
    assert "(void)iplay_runtime_resize_to_size_checked(runtime, cols, rows);" in rewrite
    assert "int iplay_runtime_resize_to_size_checked(IplayRuntime *runtime, dw cols, dw rows)" in rewrite
    assert "const IplayTextMode *mode = iplay_text_mode_for_size(cols, rows);" in rewrite
    assert "return iplay_runtime_resize_checked(runtime, mode);" in rewrite
    assert "iplay_runtime_set_video_mode_ok(runtime, iplay_notcurses_resize_checked(iplay_runtime_notcurses(runtime), mode));" in rewrite
    assert "iplay_runtime_set_video_mode_ok(runtime, iplay_notcurses_set_video_mode_checked(iplay_runtime_notcurses(runtime), video_mode));" in rewrite
    assert "runtime->video_mode_ok = ok ? 1u : 0u;" not in rewrite
    assert "runtime->video_mode_ok = ok;" not in rewrite
    assert "return runtime->video_mode_ok;" not in rewrite
    assert "runtime->video_mode_ok = 1u;" not in rewrite
    assert "runtime->video_mode_ok = (db)iplay_notcurses_resize_checked" not in rewrite
    assert "runtime->video_mode_ok = (db)iplay_notcurses_set_video_mode_checked" not in rewrite
    assert "iplay_runtime_video_status_text" in header
    assert "iplay_runtime_video_status_token" in header
    assert "iplay_runtime_render_static" in header
    assert "iplay_runtime_render_bottom" in header
    assert "iplay_runtime_audio_start" in header
    assert "iplay_runtime_audio_stop" in header
    assert "iplay_runtime_audio_active" in header
    assert "iplay_runtime_audio_pause" in header
    assert "iplay_runtime_audio_paused" in header
    assert "iplay_runtime_audio_reset_counters" in header
    assert "iplay_runtime_audio_set_capacity" in header
    assert "iplay_runtime_audio_add_capacity" in header
    assert "iplay_runtime_audio_clear_queued" in header
    assert "iplay_runtime_audio_capacity" in header
    assert "iplay_runtime_audio_frames_written" in header
    assert "iplay_runtime_audio_underrun_frames" in header
    assert "iplay_runtime_audio_dropped_frames" in header
    assert "iplay_runtime_audio_queued_frames" in header
    assert "iplay_runtime_audio_queued_bytes" in header
    assert "iplay_runtime_audio_can_queue" in header
    assert "iplay_runtime_audio_frames_for_bytes" in header
    assert "iplay_runtime_audio_bytes_for_frames" in header
    assert "iplay_runtime_audio_queue" in header
    assert "iplay_runtime_audio_queue_frames" in header
    assert "iplay_runtime_write_sb16_frames" in header
    assert "return iplay_sdl_audio_device_spec(iplay_runtime_audio_const(runtime));" in rewrite
    assert "return iplay_sdl_audio_device_backend(iplay_runtime_audio_const(runtime));" in rewrite
    assert "iplay_sdl_audio_device_start(iplay_runtime_audio(runtime));" in rewrite
    assert "iplay_sdl_audio_device_stop(iplay_runtime_audio(runtime));" in rewrite
    assert "void iplay_runtime_audio_clear_queued(IplayRuntime *runtime)" in rewrite
    assert "iplay_sdl_audio_device_clear_queued(iplay_runtime_audio(runtime));" in rewrite
    assert "dd iplay_runtime_audio_queued_frames(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_sdl_audio_device_queued_frames(iplay_runtime_audio_const(runtime));" in rewrite
    assert "dd iplay_runtime_audio_queued_bytes(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_sdl_audio_device_queued_bytes(iplay_runtime_audio_const(runtime));" in rewrite
    assert "int iplay_runtime_audio_can_queue(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_sdl_audio_device_can_queue(iplay_runtime_audio_const(runtime));" in rewrite
    assert "dw iplay_runtime_audio_frames_for_bytes(const IplayRuntime *runtime, dw byte_count)" in rewrite
    assert "return iplay_sdl_audio_device_frames_for_bytes(iplay_runtime_audio_const(runtime), byte_count);" in rewrite
    assert "dw iplay_runtime_audio_bytes_for_frames(const IplayRuntime *runtime, dw frame_count)" in rewrite
    assert "return iplay_sdl_audio_device_bytes_for_frames(iplay_runtime_audio_const(runtime), frame_count);" in rewrite
    assert "return iplay_sdl_audio_device_queue(iplay_runtime_audio(runtime), pcm, byte_count);" in rewrite
    assert "dw iplay_runtime_audio_queue_frames(IplayRuntime *runtime, const db *pcm, dw frame_count)" in rewrite
    assert "return iplay_sdl_audio_device_queue_frames(iplay_runtime_audio(runtime), pcm, frame_count);" in rewrite
    assert "int iplay_runtime_audio_is_sb16_hardware(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_sdl_audio_device_is_sb16_hardware(iplay_runtime_audio_const(runtime));" in rewrite
    assert "int iplay_runtime_audio_is_sdl_compatible(const IplayRuntime *runtime)" in rewrite
    assert "return iplay_sdl_audio_device_is_sdl_compatible(iplay_runtime_audio_const(runtime));" in rewrite
    assert "return iplay_sdl_audio_device_write_sb16_frames(iplay_runtime_audio(runtime), pcm, frame_count);" in rewrite
    assert "iplay_notcurses_draw_audio_output_levels(iplay_runtime_notcurses(runtime), y, x, iplay_sdl_audio_device_output(iplay_runtime_audio(runtime)), width" in rewrite
    assert "iplay_terminal_draw_audio_output_levels(iplay_runtime_terminal(runtime), y, x, iplay_sdl_audio_device_output(iplay_runtime_audio(runtime)), width" not in rewrite
    assert "iplay_terminal_draw_audio_output_levels(iplay_runtime_terminal(runtime), y, x, &runtime->audio.output" not in rewrite
    assert "iplay_runtime_write_silence" in header
    assert "iplay_runtime_audio_levels" in header
    assert "iplay_runtime_audio_reset_levels" in header
    assert "iplay_runtime_draw_audio_levels" in header
    assert "iplay_runtime_draw_audio_status" in header
    assert "iplay_runtime_refresh_audio_status" in header
    assert "iplay_runtime_draw_video_status" in header
    assert "iplay_module_status_init" in header
    assert "iplay_module_status_set_title" in header
    assert "iplay_module_status_set_path" in header
    assert "iplay_module_status_set_size" in header
    assert "iplay_module_status_set_loader" in header
    assert "iplay_module_status_title" in header
    assert "iplay_module_status_path" in header
    assert "iplay_module_status_size" in header
    assert "iplay_module_status_loader" in header
    assert "iplay_module_status_type" in header
    assert "iplay_module_status_set_type" in header
    assert "iplay_module_status_clear_type" in header
    assert "iplay_module_status_type_hex" in header
    assert "iplay_runtime_draw_module_status_struct" in header
    assert "iplay_runtime_draw_module_tag_struct" in header
    assert "iplay_runtime_draw_status_block" in header
    assert "iplay_runtime_draw_module_status" in header
    assert "iplay_runtime_draw_module_tag" in header
    assert "iplay_runtime_draw_status_line" in header
    assert "iplay_runtime_draw_status_field" in header
    assert "iplay_runtime_draw_status_u32" in header
    assert "iplay_runtime_draw_status_hex32" in header
    assert "iplay_window_draw_status_line" in header
    assert "iplay_window_draw_status_field" in header
    assert "iplay_window_draw_status_u32" in header
    assert "iplay_window_draw_status_hex32" in header
    assert "iplay_runtime_present" in header
    assert "void iplay_module_status_set_title(IplayModuleStatus *status, const char *title)" in rewrite
    assert "void iplay_module_status_set_path(IplayModuleStatus *status, const char *module_path)" in rewrite
    assert "void iplay_module_status_set_size(IplayModuleStatus *status, dd module_size)" in rewrite
    assert "void iplay_module_status_set_loader(IplayModuleStatus *status, const char *loader_symbol)" in rewrite
    assert "#define iplay_module_status_set_title_field(state, value) ((state)->title = (value))" in rewrite
    assert "#define iplay_module_status_set_path_field(state, value) ((state)->module_path = (value))" in rewrite
    assert "#define iplay_module_status_set_size_field(state, value) ((state)->module_size = (value))" in rewrite
    assert "#define iplay_module_status_set_loader_field(state, value) ((state)->loader_symbol = (value))" in rewrite
    assert "#define iplay_module_status_set_type_field(state, value) ((state)->module_type = (value))" in rewrite
    assert "#define iplay_module_status_title_field(state) ((state)->title)" in rewrite
    assert "#define iplay_module_status_path_field(state) ((state)->module_path)" in rewrite
    assert "#define iplay_module_status_size_field(state) ((state)->module_size)" in rewrite
    assert "#define iplay_module_status_loader_field(state) ((state)->loader_symbol)" in rewrite
    assert "#define iplay_module_status_type_field(state) ((state)->module_type)" in rewrite
    assert "static void iplay_module_status_set_title_field(IplayModuleStatus *state, const char *title)" not in rewrite
    assert "static void iplay_module_status_set_path_field(IplayModuleStatus *state, const char *module_path)" not in rewrite
    assert "static void iplay_module_status_set_size_field(IplayModuleStatus *state, dd module_size)" not in rewrite
    assert "static void iplay_module_status_set_loader_field(IplayModuleStatus *state, const char *loader_symbol)" not in rewrite
    assert "static void iplay_module_status_set_type_field(IplayModuleStatus *state, dd module_type)" not in rewrite
    assert "static const char *iplay_module_status_title_field(const IplayModuleStatus *state)" not in rewrite
    assert "static const char *iplay_module_status_path_field(const IplayModuleStatus *state)" not in rewrite
    assert "static dd iplay_module_status_size_field(const IplayModuleStatus *state)" not in rewrite
    assert "static const char *iplay_module_status_loader_field(const IplayModuleStatus *state)" not in rewrite
    assert "static dd iplay_module_status_type_field(const IplayModuleStatus *state)" not in rewrite
    assert "iplay_module_status_set_title(status, title);" in rewrite
    assert "iplay_module_status_set_path(status, module_path);" in rewrite
    assert "iplay_module_status_set_size(status, module_size);" in rewrite
    assert "iplay_module_status_set_loader(status, loader_symbol);" in rewrite
    assert "iplay_module_status_set_type(status, module_type);" in rewrite
    assert "iplay_module_status_set_title_field(status, title);" in rewrite
    assert "iplay_module_status_set_path_field(status, module_path);" in rewrite
    assert "iplay_module_status_set_size_field(status, module_size);" in rewrite
    assert "iplay_module_status_set_loader_field(status, loader_symbol);" in rewrite
    assert "iplay_module_status_set_type_field(status, module_type);" in rewrite
    assert "return iplay_module_status_title_field(status);" in rewrite
    assert "return iplay_module_status_path_field(status);" in rewrite
    assert "return iplay_module_status_size_field(status);" in rewrite
    assert "return iplay_module_status_loader_field(status);" in rewrite
    assert "return iplay_module_status_type_field(status);" in rewrite
    assert "status->title = title;" not in rewrite
    assert "status->module_path = module_path;" not in rewrite
    assert "status->module_size = module_size;" not in rewrite
    assert "status->loader_symbol = loader_symbol;" not in rewrite
    assert "status->module_type = module_type;" not in rewrite
    assert "return status->title;" not in rewrite
    assert "return status->module_path;" not in rewrite
    assert "return status->module_size;" not in rewrite
    assert "return status->loader_symbol;" not in rewrite
    assert "return status->module_type;" not in rewrite
    assert (
        "status->title = title;\n"
        "    status->module_path = module_path;\n"
        "    status->module_size = module_size;\n"
        "    status->loader_symbol = loader_symbol;\n"
        "    status->module_type = module_type;"
    ) not in rewrite
    assert "IplayPlayerUi" not in header
    assert "iplay_player_ui_" not in header
    assert "extern const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_ORIGINAL;" in header
    assert "extern const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_40COL;" in header
    assert "extern const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_80COL;" in header
    assert "extern const IplayBottomLayout IPLAY_BOTTOM_LAYOUT_80X50;" in header
    assert "IPLAY_BOTTOM_LAYOUT_ORIGINAL" in rewrite
    assert "IPLAY_BOTTOM_LAYOUT_40COL" in rewrite
    assert "IPLAY_BOTTOM_LAYOUT_80COL" in rewrite
    assert "IPLAY_BOTTOM_LAYOUT_80X50" in rewrite
    assert "iplay_bottom_layout_for_mode" in rewrite
    assert "iplay_bottom_layout" in rewrite
    assert "#define iplay_bottom_layout_timing_y_field(state) ((state)->timing_y)" in rewrite
    assert "#define iplay_bottom_layout_module_y_field(state) ((state)->module_y)" in rewrite
    assert "#define iplay_bottom_layout_pattern_y_field(state) ((state)->pattern_y)" in rewrite
    assert "#define iplay_bottom_layout_playstate_y_field(state) ((state)->playstate_y)" in rewrite
    assert "#define iplay_bottom_layout_left_x_field(state) ((state)->left_x)" in rewrite
    assert "#define iplay_bottom_layout_value_x_field(state) ((state)->value_x)" in rewrite
    assert "#define iplay_bottom_layout_playstate_x_field(state) ((state)->playstate_x)" in rewrite
    assert "#define iplay_bottom_layout_flag_x_field(state) ((state)->flag_x)" in rewrite
    assert "#define iplay_bottom_layout_timing_width_field(state) ((state)->timing_width)" in rewrite
    assert "#define iplay_bottom_layout_module_width_field(state) ((state)->module_width)" in rewrite
    assert "#define iplay_bottom_layout_pattern_width_field(state) ((state)->pattern_width)" in rewrite
    assert "#define iplay_bottom_layout_value_width_field(state) ((state)->value_width)" in rewrite
    assert "#define iplay_bottom_layout_playstate_width_field(state) ((state)->playstate_width)" in rewrite
    assert "#define iplay_bottom_layout_mode_x_field(state) ((state)->mode_x)" in rewrite
    assert "#define iplay_bottom_layout_mode_width_field(state) ((state)->mode_width)" in rewrite
    assert "static dw iplay_bottom_layout_timing_y_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_module_y_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_pattern_y_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_playstate_y_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_left_x_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_value_x_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_playstate_x_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_flag_x_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_timing_width_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_module_width_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_pattern_width_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_value_width_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_playstate_width_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_mode_x_field(const IplayBottomLayout *state)" not in rewrite
    assert "static dw iplay_bottom_layout_mode_width_field(const IplayBottomLayout *state)" not in rewrite
    assert "if (iplay_bottom_layout_module_y_field(layout) >= rows) return 0;" in rewrite
    assert "if (iplay_bottom_layout_pattern_y_field(layout) >= rows) return 0;" in rewrite
    assert "if (iplay_bottom_layout_timing_y_field(layout) >= rows) return 0;" in rewrite
    assert "if (iplay_bottom_layout_playstate_y_field(layout) >= rows) return 0;" in rewrite
    assert "if (iplay_bottom_layout_flag_x_field(layout) >= cols) return 0;" in rewrite
    assert "if ((dw)(iplay_bottom_layout_left_x_field(layout) + iplay_bottom_layout_module_width_field(layout)) > cols) return 0;" in rewrite
    assert "if ((dw)(iplay_bottom_layout_left_x_field(layout) + iplay_bottom_layout_pattern_width_field(layout)) > cols) return 0;" in rewrite
    assert "if ((dw)(iplay_bottom_layout_left_x_field(layout) + iplay_bottom_layout_timing_width_field(layout)) > cols) return 0;" in rewrite
    assert "if ((dw)(iplay_bottom_layout_mode_x_field(layout) + iplay_bottom_layout_mode_width_field(layout)) > cols) return 0;" in rewrite
    assert "if ((dw)(iplay_bottom_layout_value_x_field(layout) + iplay_bottom_layout_value_width_field(layout)) > cols) return 0;" in rewrite
    assert "if ((dw)(iplay_bottom_layout_playstate_x_field(layout) + iplay_bottom_layout_playstate_width_field(layout)) > cols) return 0;" in rewrite
    assert "layout->module_y >= rows" not in rewrite
    assert "layout->pattern_y >= rows" not in rewrite
    assert "layout->timing_y >= rows" not in rewrite
    assert "layout->playstate_y >= rows" not in rewrite
    assert "layout->flag_x >= cols" not in rewrite
    assert "layout->left_x + layout->module_width" not in rewrite
    assert "layout->left_x + layout->pattern_width" not in rewrite
    assert "layout->left_x + layout->timing_width" not in rewrite
    assert "layout->mode_x + layout->mode_width" not in rewrite
    assert "layout->value_x + layout->value_width" not in rewrite
    assert "layout->playstate_x + layout->playstate_width" not in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_timing_y_field(layout), iplay_bottom_layout_left_x_field(layout), buf, 0x7f, iplay_bottom_layout_timing_width_field(layout));" in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_timing_y_field(layout), iplay_bottom_layout_mode_x_field(layout), (flags & 8u) ? \"(PAL) \" : \"(NTSC)\", 0x7e, iplay_bottom_layout_mode_width_field(layout));" in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_module_y_field(layout), iplay_bottom_layout_left_x_field(layout), buf, 0x7f, iplay_bottom_layout_module_width_field(layout));" in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_pattern_y_field(layout), iplay_bottom_layout_left_x_field(layout), buf, 0x7f, iplay_bottom_layout_pattern_width_field(layout));" in rewrite
    assert "iplay_ncplane_putc_yx(plane, (dw)(iplay_bottom_layout_module_y_field(layout) - 3u), iplay_bottom_layout_flag_x_field(layout), 0xfe, attr);" in rewrite
    assert "iplay_ncplane_putc_yx(plane, (dw)(iplay_bottom_layout_module_y_field(layout) - 2u), iplay_bottom_layout_flag_x_field(layout), 0xfe, attr);" in rewrite
    assert "iplay_ncplane_putc_yx(plane, (dw)(iplay_bottom_layout_module_y_field(layout) - 1u), iplay_bottom_layout_flag_x_field(layout), 0xfe, attr);" in rewrite
    assert "iplay_ncplane_putc_yx(plane, iplay_bottom_layout_module_y_field(layout), iplay_bottom_layout_flag_x_field(layout), 0xfe, attr);" in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_pattern_y_field(layout), iplay_bottom_layout_value_x_field(layout), buf, 0x7f, iplay_bottom_layout_value_width_field(layout));" in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_timing_y_field(layout), iplay_bottom_layout_value_x_field(layout), buf, 0x7f, iplay_bottom_layout_value_width_field(layout));" in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, iplay_bottom_layout_playstate_y_field(layout), iplay_bottom_layout_playstate_x_field(layout), \"Play\", 0x7e, iplay_bottom_layout_playstate_width_field(layout));" in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, layout->timing_y, layout->left_x, buf, 0x7f, layout->timing_width);" not in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, layout->timing_y, layout->mode_x, (flags & 8u) ? \"(PAL) \" : \"(NTSC)\", 0x7e, layout->mode_width);" not in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, layout->module_y, layout->left_x, buf, 0x7f, layout->module_width);" not in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, layout->pattern_y, layout->left_x, buf, 0x7f, layout->pattern_width);" not in rewrite
    assert "iplay_ncplane_putc_yx(plane, (dw)(layout->module_y - 3u), layout->flag_x, 0xfe, attr);" not in rewrite
    assert "iplay_ncplane_putc_yx(plane, (dw)(layout->module_y - 2u), layout->flag_x, 0xfe, attr);" not in rewrite
    assert "iplay_ncplane_putc_yx(plane, (dw)(layout->module_y - 1u), layout->flag_x, 0xfe, attr);" not in rewrite
    assert "iplay_ncplane_putc_yx(plane, layout->module_y, layout->flag_x, 0xfe, attr);" not in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, layout->pattern_y, layout->value_x, buf, 0x7f, layout->value_width);" not in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, layout->timing_y, layout->value_x, buf, 0x7f, layout->value_width);" not in rewrite
    assert "iplay_ncplane_putnstr_fill_yx(plane, layout->playstate_y, layout->playstate_x, \"Play\", 0x7e, layout->playstate_width);" not in rewrite
    assert "static void put_cell(" not in rewrite
    assert "static void put_cell_yx(" not in rewrite
    assert "static void put_text_yx(" not in rewrite
    assert "static void put_textn_yx(" not in rewrite
    assert "IPLAY_ENABLE_TEXT_SUBPLANES" not in rewrite
    assert "static db video_mem[PLAYER_VIDEO_SIZE];" in player
    assert "static db mem[PLAYER_MEM_SIZE];" in player
    assert "#define PLAYER_MEM_SIZE 0xa000u" in player
    assert "#define PLAYER_VIDEO_SIZE IPLAY_TEXT_MAX_SCREEN_BYTES" in player
    assert "static db video_mem[PLAYER_MEM_SIZE];" not in player
    assert "#define PLAYER_VIDEO_SIZE 0xa000u" not in player
    assert "PLAYER_VIDEO_SIZE 0xA000u" not in player
    assert "static db *player_memory(void)" in player
    assert "static db *player_video_memory(void)" in player
    assert "return mem;" in player
    assert "return video_mem;" in player
    assert "video_mem = (db *)calloc(" not in player
    assert "mem = (db *)calloc(PLAYER_MEM_SIZE" not in player
    assert "static void player_clear_player_memory(void)" in player
    assert "static void player_clear_video_memory(void)" in player
    assert "static void player_init_hardware_io(void)" in player
    assert "static void player_init_core_state(void)" in player
    assert "static void player_init_dos_process(void)" in player
    assert "static int player_run_dos_cli_process(int argc, char **argv)" in player
    assert "player_init_dos_process();" in player
    assert "return player_run_cli(argc, argv);" in player
    assert "return player_run_dos_cli_process(argc, argv);" in player
    assert "memset(player_memory(), 0, PLAYER_MEM_SIZE);" in player
    assert "memset(player_video_memory(), 0, PLAYER_VIDEO_SIZE);" in player
    assert "player_clear_player_memory();" in player
    assert "player_clear_video_memory();" in player
    assert "player_init_registers(" not in player
    assert "player_init_core_state();" in player
    assert "player_init_hardware_io();" in player
    assert "player_init_text_presenter();" in player
    assert "static void player_init_process(IplayRegs *r)" not in player
    assert "player_init_process(&r);" not in player
    assert "player_init_sb16_audio_backend();" not in player
    assert "memset(mem, 0, PLAYER_MEM_SIZE);" not in player
    assert "memset(video_mem, 0, PLAYER_VIDEO_SIZE);" not in player
    assert "IplayRegs" not in player
    assert "memset(&r, 0, sizeof(r));" not in player
    assert "sizeof(video_mem)" not in player
    assert "sizeof(mem)" not in player
    assert "#define IPLAY_PLAYER_MEM_SB_BASE_PORT_LO 0x0137u" in player
    assert "#define IPLAY_PLAYER_MEM_SOUND_DRIVER 0x00d9u" in player
    assert "#define IPLAY_PLAYER_MEM_MASTER_VOLUME 0x00dau" in player
    assert "#define IPLAY_PLAYER_DEFAULT_SB_BASE_PORT_LO 0x22u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_SOUND_DRIVER 6u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_MASTER_VOLUME 125u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_PATTERN 1u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_ORDER 1u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_ROW 0u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_SPEED 0u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_FLAGS 0u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_VOLUME 0x80u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_AMPLIFICATION 100u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_ERASE_ATTR 0x07u" in player
    assert "static void player_set_memory_byte(db *player_mem, dw offset, db value)" in player
    assert "static db player_memory_byte(const db *player_mem, dw offset)" in player
    assert "player_mem[offset] = value;" in player
    assert "return player_mem[offset];" in player
    assert "static void player_init_audio_defaults(db *player_mem)" in player
    assert "static void player_start_program_memory(db *player_mem)" in player
    assert "iplay_start_player_memory(player_mem);" in player
    assert "iplay_start_bounded(" not in player
    assert "void iplay_start_player_memory(db *mem);" in header
    assert "void iplay_start_player_memory(db *mem)" in rewrite
    assert "static db player_master_volume(const db *player_mem)" in player
    assert "static const char *player_module_arg(int argc, char **argv)" in player
    assert "#define player_module_request_path_field(state) ((state)->path)" in player
    assert "#define player_module_request_set_path_field(state, value) ((state)->path = (value))" in player
    assert "#define player_module_request_path_raw(state) ((state)->path)" not in player
    assert "#define player_module_request_set_path_raw(state, value) ((state)->path = (value))" not in player
    assert "static const char *player_module_request_path_raw(const PlayerModuleRequest *state)" not in player
    assert "static void player_module_request_set_path_raw(PlayerModuleRequest *state, const char *path)" not in player
    assert "static void player_module_request_init_path_blocks(PlayerModuleRequest *request, const char *path, dd trial_block_limit)" in player
    assert "static void player_module_request_init_path(PlayerModuleRequest *request, const char *path)" in player
    assert "static void player_module_request_init_cli(PlayerModuleRequest *request, int argc, char **argv)" in player
    assert "static const char *player_module_request_path(const PlayerModuleRequest *request)" in player
    assert "static dd player_module_request_trial_block_limit(const PlayerModuleRequest *request)" in player
    assert "static int player_module_request_is_usage(const PlayerModuleRequest *request)" in player
    assert "static int player_requested_usage(const PlayerModuleRequest *request)" in player
    assert "#define IPLAY_PLAYER_FILE_LIST_PATH_BYTES 80u" in player
    assert "static int player_path_is_file_list(const char *path)" in player
    assert "static int player_file_list_space(char ch)" in player
    assert "static void player_trim_file_list_path(char *path)" in player
    assert "static int player_read_file_list_first_path(const char *list_arg, char *path, unsigned capacity)" in player
    assert "static const char *player_resolve_requested_module_path(const PlayerModuleRequest *request, char *file_list_path, unsigned capacity)" in player
    assert "static char upper_ascii(char ch)" in player
    assert "static void copy_path_case_variant(char *dst, const char *src, unsigned capacity, int upper)" in player
    assert "static int player_open_read_binary(const char *path)" in player
    assert "static int player_exit_ok_status(void);" in player
    assert "player_module_request_set_path_field(request, path);" in player
    assert "request->trial_block_limit = trial_block_limit;" in player
    assert "player_module_request_init_path_blocks(request, path, IPLAY_PLAYER_TRIAL_PLAYBACK_BLOCK_LIMIT);" in player
    assert "player_module_request_init_path_blocks(request, player_module_arg(argc, argv), trial_block_limit);" in player
    assert "return player_module_request_path_field(request);" in player
    assert "return request->trial_block_limit;" in player
    assert "const char *path = player_module_request_path(request);" in player
    assert "return player_module_request_is_usage(request);" in player
    assert "return path && path[0] == '@' && path[1] != 0;" in player
    assert "fd = open(list_path, O_RDONLY | O_BINARY);" in player
    assert "player_trim_file_list_path(path);" in player
    assert "fd = player_open_read_binary(path);" in player
    assert "path = player_resolve_requested_module_path(request, file_list_path, IPLAY_PLAYER_FILE_LIST_PATH_BYTES);" in player
    assert "PlayerModuleRequest request;" in player
    assert "player_module_request_init_cli(&request, argc, argv);" in player
    assert "static int player_run_request(const PlayerModuleRequest *request)" in player
    assert "if (player_requested_usage(&request)) return player_report_usage();" in player
    assert "static int player_run_path(const char *path)" in player
    assert "player_module_request_init_path(&request, path);" in player
    coverage = (ROOT / "tests" / "COVERAGE.md").read_text()
    behavior = (ROOT / "tests" / "test_player_behavior.py").read_text()
    assert "DOS `@FileList.Ext` coverage loads the first non-empty entry and trims surrounding spaces/tabs before module load" in coverage
    assert "test_iplayc_dos_file_list_argument_trims_whitespace_around_first_module" in behavior
    assert "return player_run_request(&request);" in player
    assert "static int player_run_cli(int argc, char **argv)" in player
    assert "return player_run_cli(argc, argv);" in player
    assert "static int player_run(int argc, char **argv, IplayRegs *r)" not in player
    assert "return player_run(argc, argv, &r);" not in player
    assert "static int player_run_request(const PlayerModuleRequest *request, IplayRegs *r)" not in player
    assert "static int player_run_path(const char *path, IplayRegs *r)" not in player
    assert "static int player_run_cli(int argc, char **argv, IplayRegs *r)" not in player
    assert "return player_run_request(&request, r);" not in player
    assert "return player_run_cli(argc, argv, &r);" not in player
    assert "return argc < 2 ? NULL : argv[1];" in player
    assert "const char *path = player_module_request_path(request);" in player
    assert 'strcmp(path, "/?")' in player
    assert 'strcmp(path, "-?")' in player
    assert "static int player_requested_usage(int argc, char **argv)" not in player
    assert "if (player_requested_usage(argc, argv))" not in player
    assert "if (player_requested_usage(request))" not in player
    assert "const char *path = player_module_arg(argc, argv);" not in player
    assert 'strcmp(argv[1], "/?")' not in player
    assert 'strcmp(argv[1], "-?")' not in player
    assert 'if (argc < 2 || strcmp(argv[1], "/?")' not in player
    assert "static void audio_backend_prepare(IplayRegs *r)" not in player
    assert "iplay_snd_on_parnt_bounded(player_memory());" not in player
    assert "player_start_program_memory(player_memory());" in player
    assert "player_init_audio_defaults(mem);" not in player
    assert "iplay_start_bounded(&r, mem);" not in player
    assert "player_master_volume(player_memory())" in player
    assert "player_set_memory_byte(player_mem, IPLAY_PLAYER_MEM_SB_BASE_PORT_LO, IPLAY_PLAYER_DEFAULT_SB_BASE_PORT_LO);" in player
    assert "player_set_memory_byte(player_mem, IPLAY_PLAYER_MEM_SOUND_DRIVER, IPLAY_PLAYER_DEFAULT_SOUND_DRIVER);" in player
    assert "player_set_memory_byte(player_mem, IPLAY_PLAYER_MEM_MASTER_VOLUME, IPLAY_PLAYER_DEFAULT_MASTER_VOLUME);" in player
    assert "return player_memory_byte(player_mem, IPLAY_PLAYER_MEM_MASTER_VOLUME);" in player
    assert "player_mem[IPLAY_PLAYER_MEM_SB_BASE_PORT_LO]" not in player
    assert "player_mem[IPLAY_PLAYER_MEM_SOUND_DRIVER]" not in player
    assert "player_mem[IPLAY_PLAYER_MEM_MASTER_VOLUME]" not in player
    assert "mem[0x0137u]" not in player
    assert "mem[0x00d9u]" not in player
    assert "mem[0x00dau]" not in player
    assert "IplayRuntimeConfig runtime_config" in player
    assert "IplayModuleStatus module_status" in player
    assert "typedef struct PlayerModuleRequest" in player
    assert "db kind;" in player
    assert "typedef struct PlayerModuleInfo" in player
    assert "const LoaderInfo *loader;" in player
    assert "dd module_type;" in player
    assert "typedef struct PlayerAudioBackend" in player
    assert "IplayAudioWriteFn write;" in player
    assert "void *user;" in player
    assert "typedef struct PlayerVideoBackend" in player
    assert "IplayVideoPresentFn present;" in player
    assert "typedef struct PlayerVideoConfig" in player
    assert "db *cells;" in player
    assert "dw capacity;" in player
    assert "const IplayTextMode *mode;" in player
    assert "typedef struct PlayerRuntimeOutput" in player
    assert "PlayerVideoConfig video_config;" in player
    assert "PlayerVideoBackend video_backend;" in player
    assert "PlayerAudioBackend audio_backend;" in player
    assert "#define IPLAY_PLAYER_TAG4(a, b, c, d)" in player
    assert "#define IPLAY_LOADER_KIND_MOD 1u" in player
    assert "#define IPLAY_DECODER_BACKEND_EXTERNAL 1u" in player
    assert "#define IPLAY_DECODER_LIBRARY_TRACKER 1u" in player
    assert "#define IPLAY_DECODER_LIBRARY_INR 2u" in player
    assert "Decoder ownership boundary for the later modern C/C++ rewrite" in player
    assert "MOD/NST/S3M/STM/669/MTM/PSM/FAR/ULT are external-library formats" in player
    assert "libopenmpt/libxmp/libmodplug" in player
    assert "WOW/OKT/OCT and XM/IT/PTM/AMS/DBM/DMF/MDL/DSM/MED/IMF/J2B stay on the" in player
    assert "external-library side through the generic external_module loader boundary." in player
    assert "#define IPLAY_LOADER_KIND_EXTERNAL_LIBRARY 10u" in player
    assert '{" .xm"' not in player
    assert '{".xm", "external_module", "FastTracker XM", IPLAY_LOADER_KIND_EXTERNAL_LIBRARY}' in player
    assert "case IPLAY_LOADER_KIND_EXTERNAL_LIBRARY: return IPLAY_PLAYER_TAG4('E', 'X', 'T', ' ');" in player
    assert "static void print_external_library_metadata(const db *header, size_t header_len)" in player
    assert 'has_sig(header, header_len, 0u, "Extended Module: ")' in player
    assert "copy_trimmed_text(title, sizeof(title), header + 17u, 20);" in player
    assert 'has_sig(header, header_len, 0u, "IMPM")' in player
    assert "copy_trimmed_text(title, sizeof(title), header + 4u, 26);" in player
    assert "case IPLAY_LOADER_KIND_EXTERNAL_LIBRARY:\n        print_external_library_metadata(header, header_len);" in player
    assert "INR remains a" in player
    assert "These names intentionally match the modern facade routes:" in player
    assert "external-library, project-owned, and probe-by-content." in player
    assert "must not grow handwritten pattern/sample/effect decoders" in player
    assert "Keep extra explanatory route prose out of compiled DOS data" in player
    modern_tests = (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "dos_external = list(expected)" in modern_tests
    assert "assert all(ext in expected for ext in dos_external)" in modern_tests
    assert 'for ext in dos_external:' in modern_tests
    assert '"DOSFMT" + ext.upper()' in modern_tests
    assert 'backend = \' backend="SDL-compatible SB16 16-bit stereo"\'' in modern_tests
    assert 'assert ".inr" not in expected' in modern_tests
    assert 'assert \'".inr"\' not in modern.split("static const char *const modern_external_tracker_extensions[]", 1)[1].split("};", 1)[0]' in modern_tests
    generic_external_extensions = [
        ".wow",
        ".okt",
        ".oct",
        ".xm",
        ".it",
        ".ptm",
        ".ams",
        ".dbm",
        ".dmf",
        ".mdl",
        ".dsm",
        ".med",
        ".imf",
        ".j2b",
    ]
    for ext in generic_external_extensions:
        assert f'{{"{ext}", "external_module"' in player
        assert f'{{"{ext}", "external_module"' in player.split("{\".inr\"", 1)[0]
    library_only_symbols = [
        "wow_module",
        "xm_module",
        "it_module",
        "okt_module",
        "oct_module",
        "ptm_module",
        "ams_module",
        "dbm_module",
        "dmf_module",
        "mdl_module",
        "dsm_module",
        "med_module",
        "imf_module",
        "j2b_module",
    ]
    for symbol in library_only_symbols:
        assert f'"{symbol}"' not in player
        assert f"static void {symbol}(" not in player
        assert f"static int {symbol}(" not in player
        assert f"void {symbol}(" not in player
        assert f"int {symbol}(" not in player
        assert "#define IPLAY_PLAYER_MODULE_OPEN_FAILED 0" in player
        assert "#define IPLAY_PLAYER_MODULE_UNSUPPORTED -1" in player
        assert "#define IPLAY_PLAYER_MODULE_TOO_LARGE -2" in player
        assert "#define IPLAY_PLAYER_MODULE_OK 1" in player
        assert "#define IPLAY_PLAYER_MODULE_HEADER_TRUNCATED 2" in player
        assert "#define IPLAY_PLAYER_EXIT_OK 0" in player
        assert "#define IPLAY_PLAYER_EXIT_OPEN_FAILED 2" in player
        assert "#define IPLAY_PLAYER_EXIT_UNSUPPORTED 2" in player
        assert "#define IPLAY_PLAYER_EXIT_TOO_LARGE 2" in player
    assert "module_type_tag_for_loader" not in player
    assert "find_loader_by_symbol" not in player
    assert "static const LoaderInfo *find_loader_by_kind(db kind)" in player
    assert "return find_loader_by_kind(IPLAY_LOADER_KIND_S3M);" in player
    assert "static const LoaderInfo *detect_loader_for_module(const char *path, const db *header, size_t header_len)" in player
    assert "player_module_set_loader(module, detect_loader_for_module(player_module_path(module), player_module_header(module), player_module_header_len(module)));" in player
    assert "if (!loader) loader = detect_loader(argv[1]);" not in player
    assert "static dd loader_module_type_tag(const LoaderInfo *loader)" in player
    assert "player_module_set_type_tag(module, loader_module_type_tag(player_module_loader(module)));" in player
    assert "module_type = module_type_tag_for_loader" not in player
    assert "static void print_loader_metadata(const LoaderInfo *loader, const db *header, size_t header_len)" in player
    assert "#define loader_ext_field(state) ((state)->ext)" in player
    assert "#define loader_symbol_field(state) ((state)->symbol)" in player
    assert "#define loader_name_field(state) ((state)->name)" in player
    assert "#define loader_kind_field(state) ((state)->kind)" in player
    assert "#define loader_ext_raw(state) ((state)->ext)" not in player
    assert "#define loader_symbol_raw(state) ((state)->symbol)" not in player
    assert "#define loader_name_raw(state) ((state)->name)" not in player
    assert "#define loader_kind_raw(state) ((state)->kind)" not in player
    assert "static const char *loader_ext_raw(const LoaderInfo *state)" not in player
    assert "static const char *loader_symbol_raw(const LoaderInfo *state)" not in player
    assert "static const char *loader_name_raw(const LoaderInfo *state)" not in player
    assert "static db loader_kind_raw(const LoaderInfo *state)" not in player
    assert "static const char *loader_ext(const LoaderInfo *loader)" in player
    assert "static const char *loader_symbol(const LoaderInfo *loader)" in player
    assert "static const char *loader_name(const LoaderInfo *loader)" in player
    assert "static db loader_kind(const LoaderInfo *loader)" in player
    assert "static db loader_decoder_backend(const LoaderInfo *loader)" in player
    assert "static db loader_decoder_library(const LoaderInfo *loader)" in player
    assert "static int loader_uses_external_decoder_library(const LoaderInfo *loader)" in player
    assert "static int loader_uses_project_decoder(const LoaderInfo *loader)" in player
    assert "static int loader_decoder_available(const LoaderInfo *loader)" in player
    assert "switch (loader_kind(loader))" in player
    assert "str_eq_nocase(dot, loader_ext(&loaders[i]))" in player
    assert "loader_kind(&loaders[i]) == kind" in player
    assert "return IPLAY_DECODER_BACKEND_PROJECT;" in player
    assert "return IPLAY_DECODER_BACKEND_EXTERNAL;" in player
    assert "return IPLAY_DECODER_LIBRARY_INR;" in player
    assert "return IPLAY_DECODER_LIBRARY_TRACKER;" in player
    assert "loader_decoder_backend(loader) == IPLAY_DECODER_BACKEND_EXTERNAL && loader_decoder_library(loader) != 0" in player
    assert "loader_decoder_backend(loader) == IPLAY_DECODER_BACKEND_PROJECT" in player
    assert "return loader_uses_external_decoder_library(loader) || loader_uses_project_decoder(loader);" in player
    assert "loader->kind" not in player
    assert "loader->decoder_backend" not in player
    assert "loader->decoder_library" not in player
    assert "loader->name" not in player
    assert "loader->symbol" not in player
    assert "info->ext" not in player
    assert "info->symbol" not in player
    assert "info->name" not in player
    assert "info->kind" not in player
    assert "loaders[i].ext" not in player
    assert "loaders[i].kind" not in player
    assert "static void print_module_summary(const char *path, unsigned long size, const LoaderInfo *loader, dd module_type)" in player
    assert "static void player_report_open_failed(const char *path)" in player
    assert "static void player_report_unsupported_module(const char *path)" in player
    assert "static void player_report_module_too_large(const char *path, unsigned long size)" in player
    assert "static int player_report_usage(void)" in player
    assert "static void player_report_playback_output(void)" in player
    assert "static void player_report_decoder_handoff(const LoaderInfo *loader)" in player
    assert "Decoder handoff: external tracker -> SB16 PCM seam." in player
    assert "Decoder handoff: project INR -> SB16 PCM." in player
    assert "static const char *loader_decoder_route_name(const LoaderInfo *loader)" in player
    assert "Keep route ids synchronized with IplayModernDecoderRoute." in player
    assert "Keep route ids synchronized with the DOS IPLAY_DECODER_ROUTE_* constants." in (ROOT / "rewrite" / "modern_player.hpp").read_text()
    assert "#define IPLAY_DECODER_ROUTE_EXTERNAL_LIBRARY 0u" in player
    assert "#define IPLAY_DECODER_ROUTE_PROJECT_OWNED 1u" in player
    assert "#define IPLAY_DECODER_ROUTE_PROBE_BY_CONTENT 2u" in player
    assert "static db loader_decoder_route_id(const LoaderInfo *loader)" in player
    assert "Decoder route: id=%u name=%s" in player
    assert "Decoder route: id=" in (ROOT / "rewrite" / "smoke_player.sh").read_text()
    behavior = (ROOT / "tests" / "test_player_behavior.py").read_text()
    assert "assert_decoder_route" in behavior
    assert "assert_pcm_source_route" in behavior
    assert 'route = assert_decoder_route(output, 0, "external-library")' in behavior
    assert 'assert_pcm_source_route(output, route["id"], "e", "native-preview", truncated=0, input_kind="memory", hook_provider="none")' in behavior
    assert 'assert_pcm_source_route(out, route["id"], "e", "native-preview", truncated=0, input_kind="memory", hook_provider="none")' in behavior
    assert 'route = assert_decoder_route(out, 1, "project-owned")' in behavior
    assert 'assert_pcm_source_route(out, route["id"], "p", "native", source="inr_module", truncated=0, input_kind="memory", hook_provider="none", stream_start=0)' in behavior
    assert "Decoder route: id=0 name=external-library" in behavior
    assert "Decoder route: id=1 name=project-owned" in behavior
    assert "static void player_flush_reports(void)" in player
    assert "static db *player_module_header_buffer(void)" not in player
    assert "static db header[HEADER_READ_SIZE];" not in player
    assert "#define IPLAY_PLAYER_MODULE_BUFFER_BYTES 24576u" in player
    assert "static db *player_module_storage_buffer(void)" in player
    assert "static db far module_data[IPLAY_PLAYER_MODULE_BUFFER_BYTES];" in player
    assert "Oversized external tracker modules cross the loader boundary as capped file-backed modules with true file size preserved" in status
    assert "oversized non-library/project-owned modules still report `Module too large`" in status
    assert "Larger real modules now report `Module too large`" not in status
    assert "return header;" not in player
    assert "#define player_module_set_path_field(state, value) ((state)->path = (value))" in player
    assert "#define player_module_set_header_field(state, value) ((state)->header = (value))" in player
    assert "#define player_module_set_loader_field(state, value) ((state)->loader = (value))" in player
    assert "#define player_module_set_header_len_field(state, value) ((state)->header_len = (value))" in player
    assert "#define player_module_set_header_truncated_field(state, value) ((state)->header_truncated = (value))" in player
    assert "#define player_module_set_size_field(state, value) ((state)->size = (value))" in player
    assert "#define player_module_set_type_tag_field(state, value) ((state)->module_type = (value))" in player
    assert "#define player_module_path_field(state) ((state)->path)" in player
    assert "#define player_module_loader_field(state) ((state)->loader)" in player
    assert "#define player_module_size_field(state) ((state)->size)" in player
    assert "#define player_module_type_tag_field(state) ((state)->module_type)" in player
    assert "#define player_module_header_field(state) ((state)->header)" in player
    assert "#define player_module_header_data_field(state) ((state)->header)" in player
    assert "#define player_module_header_len_field(state) ((state)->header_len)" in player
    assert "#define player_module_header_truncated_field(state) ((state)->header_truncated)" in player
    assert "#define player_module_size_out_field(state) (&(state)->size)" in player
    assert "#define player_module_header_len_out_field(state) (&(state)->header_len)" in player
    assert "static void player_module_set_path_field(PlayerModuleInfo *state, const char *source_path)" not in player
    assert "static void player_module_set_header_field(PlayerModuleInfo *state, db *header_buffer)" not in player
    assert "static void player_module_set_loader_field(PlayerModuleInfo *state, const LoaderInfo *selected_loader)" not in player
    assert "static void player_module_set_header_len_field(PlayerModuleInfo *state, size_t header_len)" not in player
    assert "static void player_module_set_size_field(PlayerModuleInfo *state, unsigned long size)" not in player
    assert "static void player_module_set_type_tag_field(PlayerModuleInfo *state, dd type_tag)" not in player
    assert "static const char *player_module_path_field(const PlayerModuleInfo *state)" not in player
    assert "static const LoaderInfo *player_module_loader_field(const PlayerModuleInfo *state)" not in player
    assert "static unsigned long player_module_size_field(const PlayerModuleInfo *state)" not in player
    assert "static dd player_module_type_tag_field(const PlayerModuleInfo *state)" not in player
    assert "static const db *player_module_header_field(const PlayerModuleInfo *state)" not in player
    assert "static db *player_module_header_data_field(PlayerModuleInfo *state)" not in player
    assert "static size_t player_module_header_len_field(const PlayerModuleInfo *state)" not in player
    assert "static unsigned long *player_module_size_out_field(PlayerModuleInfo *state)" not in player
    assert "static size_t *player_module_header_len_out_field(PlayerModuleInfo *state)" not in player
    assert "static void player_module_set_source(PlayerModuleInfo *module, const char *path, db *header, size_t capacity)" in player
    assert "static void player_module_clear_loaded_state(PlayerModuleInfo *module)" in player
    assert "static void player_init_module_info(PlayerModuleInfo *module, const char *path, db *header, size_t capacity)" in player
    assert "static const char *player_module_path(const PlayerModuleInfo *module)" in player
    assert "static const LoaderInfo *player_module_loader(const PlayerModuleInfo *module)" in player
    assert "static unsigned long player_module_size(const PlayerModuleInfo *module)" in player
    assert "static dd player_module_type_tag(const PlayerModuleInfo *module)" in player
    assert "static const db *player_module_header(const PlayerModuleInfo *module)" in player
    assert "static db *player_module_header_data(PlayerModuleInfo *module)" in player
    assert "static size_t player_module_header_len(const PlayerModuleInfo *module)" in player
    assert "static int player_module_header_truncated(const PlayerModuleInfo *module)" in player
    assert "static int player_module_data_complete(const PlayerModuleInfo *module)" in player
    assert "return module && !player_module_header_truncated(module);" in player
    assert "static const char *player_module_decoder_input_name(const PlayerModuleInfo *module)" in player
    assert "static dd player_module_u32_le_at(const PlayerModuleInfo *module, dd offset)" in player
    assert "((dd)player_module_byte_at(module, offset + 1u) << 8) |\n           ((dd)player_module_byte_at(module, offset + 2u) << 16)" in player
    assert player.count("((dd)player_module_byte_at(module, offset + 1u) << 8)") == 1
    assert "static unsigned long *player_module_size_out(PlayerModuleInfo *module)" in player
    assert "static size_t *player_module_header_len_out(PlayerModuleInfo *module)" in player
    assert "static void player_module_release(PlayerModuleInfo *module)" in player
    assert "static void player_module_set_loader(PlayerModuleInfo *module, const LoaderInfo *loader)" in player
    assert "static void player_module_set_type_tag(PlayerModuleInfo *module, dd module_type)" in player
    assert "static void player_detect_module_loader(PlayerModuleInfo *module)" in player
    assert "static void player_apply_module_type_tag(PlayerModuleInfo *module)" in player
    assert "static int player_read_module_file_info(PlayerModuleInfo *module)" in player
    assert "static int player_module_file_info_loaded(PlayerModuleInfo *module)" in player
    assert "static int player_module_loader_available(const PlayerModuleInfo *module)" in player
    assert "static int player_module_open_failed_status(void)" in player
    assert "static int player_module_unsupported_status(void)" in player
    assert "static int player_module_too_large_status(void)" in player
    assert "static int player_module_ok_status(void)" in player
    assert "static void player_report_loaded_module(const PlayerModuleInfo *module)" in player
    assert "static int player_load_module_info(PlayerModuleInfo *module)" in player
    assert "static int player_prepare_module(PlayerModuleInfo *module, const char *path, db *data, size_t capacity)" in player
    assert "static int player_prepare_requested_module(PlayerModuleInfo *module, const PlayerModuleRequest *request)" in player
    assert "static int player_module_load_ok(int load_status)" in player
    assert "static int player_module_load_open_failed(int load_status)" in player
    assert "static int player_module_load_unsupported(int load_status)" in player
    assert "static int player_module_load_too_large(int load_status)" in player
    assert "static int player_exit_open_failed_status(void)" in player
    assert "static int player_exit_unsupported_status(void)" in player
    assert "static int player_exit_too_large_status(void)" in player
    assert "static int player_exit_ok_status(void)" in player
    assert "static int player_module_load_exit_code(int load_status)" in player
    assert "static int player_report_module_load_failure(int load_status, const PlayerModuleInfo *module)" in player
    assert "return player_module_path_field(module);" in player
    assert "return player_module_loader_field(module);" in player
    assert "return player_module_size_field(module);" in player
    assert "return player_module_type_tag_field(module);" in player
    assert "return player_module_header_field(module);" in player
    assert "return player_module_header_data_field(module);" in player
    assert "return player_module_header_len_field(module);" in player
    assert "return player_module_header_truncated_field(module) != 0;" in player
    assert "return !player_module_header_truncated(module);" in player
    assert 'return player_module_data_complete(module) ? "memory" : "file-path";' in player
    assert "return player_module_size_out_field(module);" in player
    assert "return player_module_header_len_out_field(module);" in player
    assert "player_module_set_path_field(module, path);" in player
    assert "player_module_set_header_field(module, header);" in player
    assert "player_module_set_header_truncated_field(module, 0);" in player
    assert "player_module_set_loader_field(module, NULL);" in player
    assert "player_module_set_header_len_field(module, 0);" in player
    assert "player_module_set_size_field(module, 0);" in player
    assert "player_module_set_type_tag_field(module, 0);" in player
    assert "player_module_set_loader_field(module, loader);" in player
    assert "player_module_set_type_tag_field(module, module_type);" in player
    assert (
        "module->loader = NULL;\n"
        "    module->header_len = 0;\n"
        "    module->size = 0;\n"
        "    module->module_type = 0;"
    ) not in player
    assert "module->path = path;" not in player
    assert "module->header = header;" not in player
    assert "module->loader = loader;" not in player
    assert "module->module_type = module_type;" not in player
    assert "module->path = source_path;" not in player
    assert "module->header = header_buffer;" not in player
    assert "module->loader = selected_loader;" not in player
    assert "module->header_len = header_len;" not in player
    assert "module->size = size;" not in player
    assert "module->module_type = type_tag;" not in player
    assert "return module->path;" not in player
    assert "return module->loader;" not in player
    assert "return module->size;" not in player
    assert "return module->module_type;" not in player
    assert "return module->header;" not in player
    assert "return module->header_len;" not in player
    assert "return &module->size;" not in player
    assert "return &module->header_len;" not in player
    assert "player_detect_module_loader(module);" in player
    assert "player_apply_module_type_tag(module);" in player
    assert "static int read_file_info(const char *path, unsigned long *size_out, db *data, size_t data_capacity, size_t *data_len)" in player
    assert "static int player_module_storage_can_hold_size(unsigned long wanted, size_t capacity)" in player
    assert "return wanted <= (unsigned long)capacity;" in player
    assert "wanted = player_module_storage_can_hold_size(file_size, data_capacity) ? (size_t)file_size : data_capacity;" in player
    assert "if (!player_module_storage_can_hold_size(file_size, data_capacity)) return IPLAY_PLAYER_MODULE_HEADER_TRUNCATED;" in player
    assert "while (read_count < wanted)" in player
    assert "malloc(" not in player
    assert "free(" not in player
    assert "return read_file_info(player_module_path(module), player_module_size_out(module), player_module_header_data(module)," in player
    assert "static size_t player_module_header_capacity(const PlayerModuleInfo *module)" in player
    assert "return player_module_header_capacity_field(module);" in player
    assert "player_module_set_header_capacity_field(module, capacity);" in player
    assert "player_module_set_header_capacity_field(module, 0);" in player
    assert "player_module_header_capacity(module), player_module_header_len_out(module));" in player
    assert "return player_read_module_file_info(module);" in player
    assert "return loader_decoder_available(player_module_loader(module));" in player
    assert "static int player_module_accepts_capped_header(const PlayerModuleInfo *module)" in player
    assert "return loader_uses_external_decoder_library(loader);" in player
    assert "return player_module_loader(module) != NULL;" not in player
    assert "return player_module_open_failed_status();" in player
    assert "return player_module_unsupported_status();" in player
    assert "return player_module_too_large_status();" in player
    assert "return player_module_ok_status();" in player
    assert "return player_exit_open_failed_status();" in player
    assert "return player_exit_unsupported_status();" in player
    assert "return player_exit_too_large_status();" in player
    assert "return player_exit_ok_status();" in player
    assert "player_report_open_failed(player_module_path(module));" in player
    assert "player_report_unsupported_module(player_module_path(module));" in player
    assert "player_report_module_too_large(player_module_path(module), player_module_size(module));" in player
    assert "return player_report_usage();" in player
    assert "static int player_report_usage(void)" in player
    assert "print_usage();" in player
    assert "player_report_decoder_handoff(player_module_loader(module));" in player
    assert "player_report_playback_output();" in player
    assert "player_flush_reports();" in player
    assert "player_module_set_source(module, path, header, capacity);" in player
    assert "player_module_clear_loaded_state(module);" in player
    assert "player_init_module_info(module, path, data, capacity);" in player
    assert "player_report_loaded_module(module);" in player
    assert "return player_load_module_info(module);" in player
    assert "return player_prepare_module(module, player_module_request_path(request), player_module_storage_buffer(), IPLAY_PLAYER_MODULE_BUFFER_BYTES);" in player
    assert "player_module_header_buffer()" not in player
    assert "int read_status = player_module_file_info_loaded(module);" in player
    assert "int capped_header = read_status == IPLAY_PLAYER_MODULE_HEADER_TRUNCATED;" in player
    assert "player_module_set_header_truncated_field(module, capped_header ? 1u : 0u);" in player
    assert "if (read_status == IPLAY_PLAYER_MODULE_TOO_LARGE) return player_module_too_large_status();" in player
    assert "if (read_status != IPLAY_PLAYER_MODULE_OK && !capped_header) return player_module_open_failed_status();" in player
    assert "if (capped_header && !player_module_accepts_capped_header(module)) return player_module_too_large_status();" in player
    assert "static int player_module_loader_header_valid(const PlayerModuleInfo *module)" in player
    assert "if (!player_module_loader_header_valid(module)) return player_module_unsupported_status();" in player
    assert "#define IPLAY_MOD_MIN_HEADER_BYTES 1084u" in player
    assert "if (!player_module_loader_available(module)) return player_module_unsupported_status();" in player
    assert "if (!player_module_file_info_loaded(module)) return IPLAY_PLAYER_MODULE_OPEN_FAILED;" not in player
    assert "if (!player_module_loader_available(module)) return IPLAY_PLAYER_MODULE_UNSUPPORTED;" not in player
    assert "if (!player_read_module_file_info(module)) return IPLAY_PLAYER_MODULE_OPEN_FAILED;" not in player
    assert "if (!player_module_loader(module)) return IPLAY_PLAYER_MODULE_UNSUPPORTED;" not in player
    assert "player_module_set_type_tag(module, loader_module_type_tag(player_module_loader(module)));" in player
    assert "load_status = player_prepare_module(&module, path, player_module_storage_buffer(), IPLAY_PLAYER_MODULE_BUFFER_BYTES);" in player
    assert "static int player_prepare_requested_module(PlayerModuleInfo *module, int argc, char **argv)" not in player
    assert "return player_prepare_module(module, player_module_arg(argc, argv), player_module_header_buffer());" not in player
    assert "load_status = player_prepare_module(&module, player_module_arg(argc, argv), player_module_header_buffer());" not in player
    assert "load_status = player_prepare_module(&module, argv[1], player_module_header_buffer());" not in player
    assert "load_status = player_prepare_module(&module, argv[1], header);" not in player
    assert "return IPLAY_PLAYER_MODULE_OPEN_FAILED;" in player
    assert "return IPLAY_PLAYER_MODULE_UNSUPPORTED;" in player
    assert "return IPLAY_PLAYER_MODULE_OK;" in player
    assert "return IPLAY_PLAYER_EXIT_OPEN_FAILED;" in player
    assert "return IPLAY_PLAYER_EXIT_UNSUPPORTED;" in player
    assert "return IPLAY_PLAYER_EXIT_OK;" in player
    assert "print_usage();\n    return player_exit_ok_status();" in player
    assert "print_usage();\n    return IPLAY_PLAYER_EXIT_OK;" not in player
    assert "if (player_module_load_open_failed(load_status)) return player_exit_open_failed_status();" in player
    assert "if (player_module_load_unsupported(load_status)) return player_exit_unsupported_status();" in player
    assert "if (player_module_load_open_failed(load_status)) return IPLAY_PLAYER_EXIT_OPEN_FAILED;" not in player
    assert "if (player_module_load_unsupported(load_status)) return IPLAY_PLAYER_EXIT_UNSUPPORTED;" not in player
    assert "return player_module_load_exit_code(load_status);" in player
    assert "return load_status == IPLAY_PLAYER_MODULE_OPEN_FAILED;" in player
    assert "return load_status == IPLAY_PLAYER_MODULE_UNSUPPORTED;" in player
    assert "if (player_module_load_open_failed(load_status))" in player
    assert "if (player_module_load_unsupported(load_status))" in player
    assert "if (load_status == IPLAY_PLAYER_MODULE_OPEN_FAILED)" not in player
    assert "if (load_status == IPLAY_PLAYER_MODULE_UNSUPPORTED)" not in player
    assert "return load_status == IPLAY_PLAYER_MODULE_OK;" in player
    assert "if (!player_module_load_ok(load_status)) {" in player
    assert "exit_status = player_report_module_load_failure(load_status, &module);" in player
    assert "return exit_status;" in player
    assert "print_module_summary(player_module_path(module), player_module_size(module), player_module_loader(module), player_module_type_tag(module));" in player
    assert "print_loader_metadata(player_module_loader(module), player_module_header(module), player_module_header_len(module));" in player
    assert 'printf("Cannot open module: %s\\n", argv[1]);' not in player
    assert 'printf("Unsupported module type: %s\\n", argv[1]);' not in player
    assert 'printf("Module: %s\\n", argv[1]);' not in player
    assert 'printf("Module type tag: %08lX\\n", (unsigned long)module_type);' in player
    assert "strcmp(loader->symbol," not in player
    assert "strcmp(symbol," not in player
    assert "#define IPLAY_PLAYER_ENABLE_TEXT_UI 1" in player
    assert "#define IPLAY_DOS_TEXT_COLOR_SEG 0xb800u" in player
    assert "#define IPLAY_DOS_TEXT_MONO_SEG 0xb000u" in player
    assert "db far *(*text_color_memory)(void);" in player
    assert "db far *(*text_mono_memory)(void);" in player
    assert "#include <stdlib.h>" in player
    assert "static db far *dos_hw_text_color_memory(void)" in player
    assert "static db far *dos_hw_text_mono_memory(void)" in player
    assert "return (db far *)MK_FP(IPLAY_DOS_TEXT_COLOR_SEG, 0);" in player
    assert "return (db far *)MK_FP(IPLAY_DOS_TEXT_MONO_SEG, 0);" in player
    assert "#define IPLAY_PLAYER_ENABLE_SB16_HW 1" in player
    assert "#define IPLAY_SB16_DEFAULT_BASE 0x220u" in player
    assert "#define IPLAY_SB16_DEFAULT_IRQ 5u" in player
    assert "#define IPLAY_SB16_DEFAULT_DMA16 5u" in player
    assert "#define IPLAY_SB16_DSP_SET_OUTPUT_RATE 0x41u" in player
    assert "#define IPLAY_SB16_DSP_SPEAKER_ON 0xd1u" in player
    assert "#define IPLAY_SB16_DSP_OUTPUT_16BIT 0xb0u" in player
    assert "#define IPLAY_SB16_DSP_MODE_STEREO_SIGNED 0x30u" in player
    assert "#define IPLAY_SB16_PORT_DSP_RESET 0x06u" in player
    assert "#define IPLAY_SB16_PORT_DSP_READ_DATA 0x0au" in player
    assert "#define IPLAY_SB16_PORT_DSP_WRITE_DATA 0x0cu" in player
    assert "#define IPLAY_SB16_PORT_DSP_READ_STATUS 0x0eu" in player
    assert "#define IPLAY_SB16_DSP_WRITE_READY_MASK 0x80u" in player
    assert "#define IPLAY_SB16_DSP_READ_READY_MASK 0x80u" in player
    assert "#define IPLAY_SB16_DSP_RESET_ASSERT 1u" in player
    assert "#define IPLAY_SB16_DSP_RESET_RELEASE 0u" in player
    assert "#define IPLAY_SB16_RESET_SETTLE_READS 256u" in player
    assert "#define IPLAY_SB16_DSP_IO_SPIN_LIMIT 0xffffu" in player
    assert "#define IPLAY_SB16_DSP_RESET_ACK 0xaau" in player
    assert "#define IPLAY_SB16_DMA16_CHANNEL_BASE 4u" in player
    assert "#define IPLAY_SB16_DMA16_PORT_MASK 0xd4u" in player
    assert "#define IPLAY_SB16_DMA16_PORT_CLEAR_FLIPFLOP 0xd8u" in player
    assert "#define IPLAY_SB16_DMA16_PORT_MODE 0xd6u" in player
    assert "#define IPLAY_SB16_DMA16_PORT_ADDRESS_BASE 0xc0u" in player
    assert "#define IPLAY_SB16_DMA16_PORT_COUNT_BASE 0xc2u" in player
    assert "#define IPLAY_SB16_DMA16_PORT_STRIDE 4u" in player
    assert "#define IPLAY_SB16_DMA16_PORT_PAGE_CH5 0x8bu" in player
    assert "#define IPLAY_SB16_DMA16_PORT_PAGE_CH6 0x89u" in player
    assert "#define IPLAY_SB16_DMA16_PORT_PAGE_CH7 0x8au" in player
    assert "#define IPLAY_SB16_DMA_MASK_DISABLE 0x04u" in player
    assert "#define IPLAY_SB16_DMA_MODE_PLAYBACK 0x48u" in player
    assert "#define IPLAY_SB16_DMA_CLEAR_FLIPFLOP 0u" in player
    non_sb_player_driver_terms = [
        "gravis",
        "gus",
        "pro audio spectrum",
        "windows sound system",
        "adlib",
        "tandy",
        "pc speaker",
        "sound source",
        "covox",
    ]
    player_lower = player.lower()
    assert not [
        term for term in non_sb_player_driver_terms if term in player_lower
    ], "DOS player path must stay SB16-only; legacy non-SB audio drivers belong only in tested ABI compatibility stubs"
    assert "typedef struct DosHardwareIo" in player
    player_hw_runner = (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "static const DosHardwareIo dos_hw_default_io" in player
    assert "static const DosHardwareIo *dos_hw_io = &dos_hw_default_io;" in player
    assert "static void dos_hw_use_io(const DosHardwareIo *io)" in player
    assert "dos_hw_io = io ? io : &dos_hw_default_io;" in player
    assert "dos_hw_use_io(NULL);" in player
    assert "static db dos_hw_port_read(dw port)" in player
    assert "static void dos_hw_port_write(dw port, db value)" in player
    assert "#define dos_hw_io_port_read_field(state) ((state)->port_read)" in player
    assert "#define dos_hw_io_port_write_field(state) ((state)->port_write)" in player
    assert "#define dos_hw_io_far_physical_field(state) ((state)->far_physical)" in player
    assert "#define dos_hw_io_copy_to_far_field(state) ((state)->copy_to_far)" in player
    assert "#define dos_hw_io_timer_ticks_field(state) ((state)->timer_ticks)" in player
    assert "#define dos_hw_io_text_color_memory_field(state) ((state)->text_color_memory)" in player
    assert "#define dos_hw_io_port_read_raw(state) ((state)->port_read)" not in player
    assert "#define dos_hw_io_port_write_raw(state) ((state)->port_write)" not in player
    assert "#define dos_hw_io_far_physical_raw(state) ((state)->far_physical)" not in player
    assert "#define dos_hw_io_copy_to_far_raw(state) ((state)->copy_to_far)" not in player
    assert "#define dos_hw_io_text_color_memory_raw(state) ((state)->text_color_memory)" not in player
    assert "static db (*dos_hw_io_port_read_raw(const DosHardwareIo *state))(dw port)" not in player
    assert "static void (*dos_hw_io_port_write_raw(const DosHardwareIo *state))(dw port, db value)" not in player
    assert "static unsigned long (*dos_hw_io_far_physical_raw(const DosHardwareIo *state))(const void far *ptr)" not in player
    assert "static void (*dos_hw_io_copy_to_far_raw(const DosHardwareIo *state))(void far *dst, const void *src, dw byte_count)" not in player
    assert "static db far *(*dos_hw_io_text_color_memory_raw(const DosHardwareIo *state))(void)" not in player
    assert "return io->port_read;" not in player
    assert "return io->port_write;" not in player
    assert "return io->far_physical;" not in player
    assert "return io->copy_to_far;" not in player
    assert "return io->text_color_memory;" not in player
    assert "static db (*dos_hw_io_port_read_fn(void))(dw port)" in player
    assert "return dos_hw_io_port_read_field(dos_hw_io);" in player
    assert "static void (*dos_hw_io_port_write_fn(void))(dw port, db value)" in player
    assert "return dos_hw_io_port_write_field(dos_hw_io);" in player
    assert "static unsigned long (*dos_hw_io_far_physical_fn(void))(const void far *ptr)" in player
    assert "return dos_hw_io_far_physical_field(dos_hw_io);" in player
    assert "static void (*dos_hw_io_copy_to_far_fn(void))(void far *dst, const void *src, dw byte_count)" in player
    assert "return dos_hw_io_copy_to_far_field(dos_hw_io);" in player
    assert "static unsigned long (*dos_hw_io_timer_ticks_fn(void))(void)" in player
    assert "return dos_hw_io_timer_ticks_field(dos_hw_io);" in player
    assert "static db far *(*dos_hw_io_text_color_memory_fn(void))(void)" in player
    assert "return dos_hw_io_text_color_memory_field(dos_hw_io);" in player
    assert "static db dos_hw_io_read_port(dw port)" in player
    assert "return dos_hw_io_port_read_fn()(port);" in player
    assert "static void dos_hw_io_write_port(dw port, db value)" in player
    assert "dos_hw_io_port_write_fn()(port, value);" in player
    assert "static unsigned long dos_hw_io_far_physical(const void far *ptr)" in player
    assert "return dos_hw_io_far_physical_fn()(ptr);" in player
    assert "static void dos_hw_io_copy_to_far(void far *dst, const void *src, dw byte_count)" in player
    assert "dos_hw_io_copy_to_far_fn()(dst, src, byte_count);" in player
    assert "static unsigned long dos_hw_io_timer_ticks(void)" in player
    assert "return dos_hw_io_timer_ticks_fn()();" in player
    assert "static db far *dos_hw_io_text_color_memory(void)" in player
    assert "return dos_hw_io_text_color_memory_fn()();" in player
    assert "static dw sb16_dsp_write_data_port(dw base_port)" in player
    assert "return (dw)(base_port + IPLAY_SB16_PORT_DSP_WRITE_DATA);" in player
    assert "static dw sb16_dsp_read_status_port(dw base_port)" in player
    assert "return (dw)(base_port + IPLAY_SB16_PORT_DSP_READ_STATUS);" in player
    assert "static dw sb16_dsp_read_data_port(dw base_port)" in player
    assert "return (dw)(base_port + IPLAY_SB16_PORT_DSP_READ_DATA);" in player
    assert "static dw sb16_dsp_reset_port(dw base_port)" in player
    assert "return (dw)(base_port + IPLAY_SB16_PORT_DSP_RESET);" in player
    assert "spin < IPLAY_SB16_DSP_IO_SPIN_LIMIT" in player
    assert "spin < 0xffffu" not in player
    assert "dos_hw_io_read_port(sb16_dsp_write_data_port(base_port))" in player
    assert "dos_hw_io_read_port(sb16_dsp_read_status_port(base_port))" in player
    assert "dos_hw_io_write_port(sb16_dsp_write_data_port(base_port), value);" in player
    assert "dos_hw_io_read_port(sb16_dsp_read_data_port(base_port));" in player
    assert "static db sb16_dsp_word_hi(dw value)" in player
    assert "return (db)(value >> 8);" in player
    assert "static db sb16_dsp_word_lo(dw value)" in player
    assert "return (db)value;" in player
    assert "static db sb16_detected_from_reset_ack(db value)" in player
    assert "return value == IPLAY_SB16_DSP_RESET_ACK;" in player
    assert "#define sb16_set_detected_flag_field(state, value) ((state)->detected = (value))" in player
    assert "#define sb16_detected_flag_field(state) ((state)->detected)" in player
    assert "#define sb16_set_detected_flag_raw(state, value) ((state)->detected = (value))" not in player
    assert "#define sb16_detected_flag_raw(state) ((state)->detected)" not in player
    assert "static void sb16_set_detected_flag_raw(DosSb16Hardware *state, db detected)" not in player
    assert "static db sb16_detected_flag_raw(const DosSb16Hardware *state)" not in player
    assert "state->detected = detected;" not in player
    assert "return state->detected;" not in player
    assert "static void sb16_set_detected_flag(DosSb16Hardware *hw, db detected)" in player
    assert "sb16_set_detected_flag_field(hw, detected);" in player
    assert "static db sb16_detected_flag(const DosSb16Hardware *hw)" in player
    assert "return sb16_detected_flag_field(hw);" in player
    assert "static void sb16_mark_detected(DosSb16Hardware *hw, db detected)" in player
    assert "sb16_set_detected_flag(hw, detected);" in player
    assert "static int sb16_is_detected(const DosSb16Hardware *hw)" in player
    assert "return sb16_detected_flag(hw) != 0;" in player
    assert "dw base_port = player_sb16_base_port(hw);" in player
    assert "static void sb16_dsp_assert_reset(dw base_port)" in player
    assert "static void sb16_dsp_release_reset(dw base_port)" in player
    assert "static void sb16_dsp_settle_reset(dw base_port)" in player
    assert "dos_hw_io_write_port(sb16_dsp_reset_port(base_port), IPLAY_SB16_DSP_RESET_ASSERT);" in player
    assert "delay < IPLAY_SB16_RESET_SETTLE_READS" in player
    assert "dos_hw_io_read_port(sb16_dsp_read_status_port(base_port))" in player
    assert "dos_hw_io_write_port(sb16_dsp_reset_port(base_port), IPLAY_SB16_DSP_RESET_RELEASE);" in player
    assert "static void sb16_dsp_pulse_reset(dw base_port)" in player
    assert "sb16_dsp_assert_reset(base_port);" in player
    assert "sb16_dsp_settle_reset(base_port);" in player
    assert "sb16_dsp_release_reset(base_port);" in player
    assert "sb16_dsp_pulse_reset(base_port);" in player
    assert "static int sb16_dsp_read_reset_detected(dw base_port, db *detected)" in player
    assert "if (!sb16_dsp_read(base_port, &value)) return 0;" in player
    assert "*detected = sb16_detected_from_reset_ack(value);" in player
    assert "if (!sb16_dsp_read_reset_detected(base_port, &detected)) return 0;" in player
    assert "sb16_mark_detected(hw, detected);" in player
    assert "sb16_mark_detected(hw, sb16_detected_from_reset_ack(value));" not in player
    assert "return sb16_is_detected(hw);" in player
    assert "hw->detected = value == IPLAY_SB16_DSP_RESET_ACK;" not in player
    assert "hw->detected = detected;" not in player
    assert "return hw->detected;" not in player
    assert "return hw->detected != 0;" not in player
    assert "dos_hw_io_read_port((dw)(base_port + IPLAY_SB16_PORT_DSP_WRITE_DATA))" not in player
    assert "dos_hw_io_read_port((dw)(base_port + IPLAY_SB16_PORT_DSP_READ_STATUS))" not in player
    assert "dos_hw_io_write_port((dw)(base_port + IPLAY_SB16_PORT_DSP_WRITE_DATA), value);" not in player
    assert "dos_hw_io_read_port((dw)(base_port + IPLAY_SB16_PORT_DSP_READ_DATA));" not in player
    assert "dos_hw_io_write_port((dw)(hw->base_port + IPLAY_SB16_PORT_DSP_RESET), 1);" not in player
    assert "delay < 256u" not in player
    assert "dos_hw_io_write_port((dw)(hw->base_port + IPLAY_SB16_PORT_DSP_RESET), 0);" not in player
    assert "base_port + 0x0cu" not in player
    assert "base_port + 0x0eu" not in player
    assert "base_port + 0x0au" not in player
    assert "hw->base_port + 0x06u" not in player
    assert "value == 0xaau" not in player
    assert "unsigned long phys = dos_hw_io_far_physical(buffer);" in player
    assert "dos_hw_io_copy_to_far(sb16_dma_buffer_memory(), pcm, byte_count);" in player
    assert "typedef struct DosTextPresenter" in player
    assert "db far *(*video_memory)(void);" in player
    assert "void (*copy_to_video)(db far *video, const db *cells, dw byte_count);" in player
    assert "static DosTextPresenter dos_text_default_presenter;" in player
    assert "dos_hw_io_text_color_memory" in player
    assert "dos_text_present_cells" in player
    assert "typedef struct DosTextPresentFrame" in player
    assert "void *user;" in player
    assert "db far *video;" in player
    assert "const db *cells;" in player
    assert "dw byte_count;" in player
    assert "static dw dos_text_clamp_present_byte_count(dw byte_count, dw max_bytes)" in player
    assert "return byte_count > max_bytes ? max_bytes : byte_count;" in player
    assert "static dw dos_text_present_byte_count(const IplayTextMode *mode, dw byte_count)" in player
    assert "dw max_bytes = iplay_text_mode_screen_bytes(mode);" in player
    assert "return dos_text_clamp_present_byte_count(byte_count, max_bytes);" in player
    assert "static db far *dos_text_present_video_memory(void)" in player
    assert "return dos_hw_io_text_color_memory();" in player
    assert "static void dos_text_present_cells(db far *video, const db *cells, dw byte_count)" in player
    assert "dos_hw_io_copy_to_far(video, cells, byte_count);" in player
    assert "#define dos_text_presenter_set_video_memory_field(state, value) ((state)->video_memory = (value))" in player
    assert "#define dos_text_presenter_set_copy_to_video_field(state, value) ((state)->copy_to_video = (value))" in player
    assert "#define dos_text_presenter_video_memory_fn_field(state) ((state)->video_memory)" in player
    assert "#define dos_text_presenter_copy_to_video_fn_field(state) ((state)->copy_to_video)" in player
    assert "static void dos_text_presenter_set_video_memory_field(DosTextPresenter *state, db far *(*video_memory)(void))" not in player
    assert "static void dos_text_presenter_set_copy_to_video_field(DosTextPresenter *state, void (*copy_to_video)(db far *video, const db *cells, dw byte_count))" not in player
    assert "static db far *(*dos_text_presenter_video_memory_fn_field(const DosTextPresenter *state))(void)" not in player
    assert "static void (*dos_text_presenter_copy_to_video_fn_field(const DosTextPresenter *state))(db far *video, const db *cells, dw byte_count)" not in player
    assert "static void dos_text_presenter_set_video_memory(DosTextPresenter *presenter, db far *(*video_memory)(void))" in player
    assert "dos_text_presenter_set_video_memory_field(presenter, video_memory);" in player
    assert "static void dos_text_presenter_set_copy_to_video(DosTextPresenter *presenter, void (*copy_to_video)(db far *video, const db *cells, dw byte_count))" in player
    assert "dos_text_presenter_set_copy_to_video_field(presenter, copy_to_video);" in player
    assert "static void dos_text_presenter_init(DosTextPresenter *presenter, db video_mode, void (*copy_to_video)(db far *video, const db *cells, dw byte_count))" in player
    assert "dos_text_presenter_set_video_memory(presenter, dos_text_video_memory_for_mode(video_mode));" in player
    assert "dos_text_presenter_set_copy_to_video(presenter, copy_to_video);" in player
    assert "static void dos_text_presenter_init_vga_text(DosTextPresenter *presenter)" in player
    assert "dos_text_presenter_init_vga_text_mode(presenter, IPLAY_TEXT_DEFAULT_VIDEO_MODE);" in player
    assert "static void player_init_text_presenter(void)" in player
    assert "dos_text_presenter_init_vga_text(dos_text_default_presenter_state());" in player
    assert "dos_text_presenter_init(&dos_text_default_presenter, dos_hw_io_text_color_memory, dos_text_present_cells);" not in player
    assert "static void *dos_text_default_present_user(void)" in player
    assert "return (void *)dos_text_default_presenter_state();" in player
    assert "static void *dos_text_present_user(void *user)" in player
    assert "return user ? user : dos_text_default_present_user();" in player
    assert "static const DosTextPresenter *dos_text_presenter_from_user(void *user)" in player
    assert "return (const DosTextPresenter *)dos_text_present_user(user);" in player
    assert "static db far *(*dos_text_presenter_video_memory_fn(const DosTextPresenter *presenter))(void)" in player
    assert "return dos_text_presenter_video_memory_fn_field(presenter);" in player
    assert "static void (*dos_text_presenter_copy_to_video_fn(const DosTextPresenter *presenter))(db far *video, const db *cells, dw byte_count)" in player
    assert "return dos_text_presenter_copy_to_video_fn_field(presenter);" in player
    assert "static db far *dos_text_presenter_video_memory(const DosTextPresenter *presenter)" in player
    assert "return dos_text_presenter_video_memory_fn(presenter)();" in player
    assert "static void dos_text_presenter_copy_to_video(const DosTextPresenter *presenter, db far *video, const db *cells, dw byte_count)" in player
    assert "dos_text_presenter_copy_to_video_fn(presenter)(video, cells, byte_count);" in player
    assert "return presenter->video_memory();" not in player
    assert "presenter->copy_to_video(video, cells, byte_count);" not in player
    assert "presenter->video_memory = video_memory;" not in player
    assert "presenter->copy_to_video = copy_to_video;" not in player
    assert "return presenter->video_memory;" not in player
    assert "return presenter->copy_to_video;" not in player
    assert "#define dos_text_present_frame_set_byte_count_field(state, value) ((state)->byte_count = (value))" in player
    assert "#define dos_text_present_frame_set_user_field(state, value) ((state)->user = (value))" in player
    assert "#define dos_text_present_frame_set_video_field(state, value) ((state)->video = (value))" in player
    assert "#define dos_text_present_frame_set_cells_field(state, value) ((state)->cells = (value))" in player
    assert "static void dos_text_present_frame_set_byte_count_field(DosTextPresentFrame *state, dw byte_count)" not in player
    assert "static void dos_text_present_frame_set_user_field(DosTextPresentFrame *state, void *user)" not in player
    assert "static void dos_text_present_frame_set_video_field(DosTextPresentFrame *state, db far *video)" not in player
    assert "static void dos_text_present_frame_set_cells_field(DosTextPresentFrame *state, const db *cells)" not in player
    assert "static void dos_text_present_frame_set_byte_count(DosTextPresentFrame *frame, dw byte_count)" in player
    assert "dos_text_present_frame_set_byte_count_field(frame, byte_count);" in player
    assert "static void dos_text_present_frame_set_user(DosTextPresentFrame *frame, void *user)" in player
    assert "dos_text_present_frame_set_user_field(frame, user);" in player
    assert "static void dos_text_present_frame_set_video(DosTextPresentFrame *frame, db far *video)" in player
    assert "dos_text_present_frame_set_video_field(frame, video);" in player
    assert "static void dos_text_present_frame_set_cells(DosTextPresentFrame *frame, const db *cells)" in player
    assert "dos_text_present_frame_set_cells_field(frame, cells);" in player
    assert "static void dos_text_present_frame_init(DosTextPresentFrame *frame, void *user, db far *video, const db *cells, dw byte_count)" in player
    assert "dos_text_present_frame_set_user(frame, user);" in player
    assert "dos_text_present_frame_set_byte_count(frame, byte_count);" in player
    assert "dos_text_present_frame_set_video(frame, video);" in player
    assert "dos_text_present_frame_set_cells(frame, cells);" in player
    assert "static db far *dos_text_prepare_present_video(void *user)" in player
    assert "return dos_text_presenter_video_memory(dos_text_presenter_from_user(user));" in player
    assert "static dw dos_text_prepare_present_byte_count(const IplayTextMode *mode, dw byte_count)" in player
    assert "return dos_text_present_byte_count(mode, byte_count);" in player
    assert "static void dos_text_prepare_present_frame_init(DosTextPresentFrame *frame, void *present_user, const db *cells, const IplayTextMode *mode, dw byte_count)" in player
    assert "dos_text_present_frame_init(frame, present_user, dos_text_prepare_present_video(present_user), cells, dos_text_prepare_present_byte_count(mode, byte_count));" in player
    assert "static void dos_text_prepare_present(DosTextPresentFrame *frame, void *user, const db *cells, const IplayTextMode *mode, dw byte_count)" in player
    assert "void *present_user = dos_text_present_user(user);" in player
    assert "dos_text_prepare_present_frame_init(frame, present_user, cells, mode, byte_count);" in player
    assert "dos_text_present_frame_init(frame, dos_text_present_video_memory(), dos_text_present_byte_count(mode, byte_count));" not in player
    assert "dos_text_present_frame_set_byte_count(frame, dos_text_present_byte_count(mode, byte_count));" not in player
    assert "dos_text_present_frame_set_video(frame, dos_text_present_video_memory());" not in player
    assert "frame->byte_count = dos_text_present_byte_count(mode, byte_count);" not in player
    assert "frame->video = dos_text_present_video_memory();" not in player
    assert "#define dos_text_present_frame_user_field(state) ((state)->user)" in player
    assert "static void *dos_text_present_frame_user_field(const DosTextPresentFrame *state)" not in player
    assert "static void *dos_text_present_frame_user(const DosTextPresentFrame *frame)" in player
    assert "return dos_text_present_frame_user_field(frame);" in player
    assert "static const DosTextPresenter *dos_text_present_frame_presenter(const DosTextPresentFrame *frame)" in player
    assert "return dos_text_presenter_from_user(dos_text_present_frame_user(frame));" in player
    assert "#define dos_text_present_frame_video_field(state) ((state)->video)" in player
    assert "static db far *dos_text_present_frame_video_field(const DosTextPresentFrame *state)" not in player
    assert "static db far *dos_text_present_frame_video(const DosTextPresentFrame *frame)" in player
    assert "return dos_text_present_frame_video_field(frame);" in player
    assert "#define dos_text_present_frame_byte_count_field(state) ((state)->byte_count)" in player
    assert "static dw dos_text_present_frame_byte_count_field(const DosTextPresentFrame *state)" not in player
    assert "static dw dos_text_present_frame_byte_count(const DosTextPresentFrame *frame)" in player
    assert "return dos_text_present_frame_byte_count_field(frame);" in player
    assert "#define dos_text_present_frame_cells_field(state) ((state)->cells)" in player
    assert "static const db *dos_text_present_frame_cells_field(const DosTextPresentFrame *state)" not in player
    assert "static const db *dos_text_present_frame_cells(const DosTextPresentFrame *frame)" in player
    assert "return dos_text_present_frame_cells_field(frame);" in player
    assert "static void dos_text_present_frame_copy_to_video(const DosTextPresentFrame *frame)" in player
    assert "dos_text_presenter_copy_to_video(dos_text_present_frame_presenter(frame), dos_text_present_frame_video(frame), dos_text_present_frame_cells(frame), dos_text_present_frame_byte_count(frame));" in player
    assert "static void dos_text_present_frame(const DosTextPresentFrame *frame)" in player
    assert "dos_text_present_frame_copy_to_video(frame);" in player
    assert "dos_text_presenter_copy_to_video(dos_text_presenter_from_user(dos_text_present_frame_user(frame))" not in player
    assert "(void)dos_text_present_frame_user(frame);" not in player
    assert "dos_text_present_cells(dos_text_present_frame_video(frame), dos_text_present_frame_cells(frame), dos_text_present_frame_byte_count(frame));" not in player
    assert "frame->byte_count = byte_count;" not in player
    assert "frame->user = user;" not in player
    assert "frame->video = video;" not in player
    assert "frame->cells = cells;" not in player
    assert "return frame->user;" not in player
    assert "return frame->video;" not in player
    assert "return frame->byte_count;" not in player
    assert "return frame->cells;" not in player
    assert "DosTextPresentFrame frame;" in player
    assert "(void)dos_text_present_user(user);" not in player
    assert "dos_text_prepare_present(&frame, user, cells, mode, byte_count);" in player
    assert "dos_text_present_frame(&frame);" in player
    assert (
        "DosTextPresentFrame frame;\n"
        "    (void)user;"
    ) not in player
    assert "dos_text_present_cells(dos_text_present_frame_video(&frame), cells, dos_text_present_frame_byte_count(&frame));" not in player
    assert "dos_text_present_frame(&frame, cells);" not in player
    assert "if (byte_count > max_bytes) byte_count = max_bytes;" not in player
    assert "\n    byte_count = dos_text_present_byte_count(mode, byte_count);" not in player
    assert "video = dos_hw_io_text_color_memory();" not in player
    assert "\n    video = dos_text_present_video_memory();" not in player
    assert "dos_text_present_cells(video, cells, byte_count);" not in player
    assert "dos_hw_io->port_read((dw)(base_port" not in player
    assert "dos_hw_io->port_write((dw)(base_port" not in player
    assert "dos_hw_io->far_physical(buffer)" not in player
    assert "dos_hw_io->copy_to_far(sb16_dma_buffer" not in player
    assert "video = dos_hw_io->text_color_memory();" not in player
    assert "static unsigned long dos_hw_far_physical(const void far *ptr)" in player
    assert "static void dos_hw_copy_to_far(void far *dst, const void *src, dw byte_count)" in player
    assert "static unsigned long hw_mock_timer_ticks(void)" in player_hw_runner
    assert "hw_mock_timer_ticks," in player_hw_runner
    assert "return (db)inp((unsigned)port);" in player
    assert "outp((unsigned)port, value);" in player
    assert "return ((unsigned long)FP_SEG(ptr) << 4) + FP_OFF(ptr);" in player
    assert "_fmemcpy(dst, src, byte_count);" in player
    assert player.count("inp((unsigned)") == 1
    assert player.count("outp((unsigned)") == 1
    assert player.count("FP_SEG(") == 1
    assert player.count("FP_OFF(") == 1
    assert player.count("_fmemcpy(") == 1
    assert "#define dos_hw_io_port_read_field(state) ((state)->port_read)" in player
    assert "#define dos_hw_io_port_write_field(state) ((state)->port_write)" in player
    assert "#define dos_hw_io_far_physical_field(state) ((state)->far_physical)" in player
    assert "#define dos_hw_io_copy_to_far_field(state) ((state)->copy_to_far)" in player
    assert "#define dos_hw_io_text_color_memory_field(state) ((state)->text_color_memory)" in player
    assert "#define dos_hw_io_port_read_raw(state) ((state)->port_read)" not in player
    assert "#define dos_hw_io_port_write_raw(state) ((state)->port_write)" not in player
    assert "#define dos_hw_io_far_physical_raw(state) ((state)->far_physical)" not in player
    assert "#define dos_hw_io_copy_to_far_raw(state) ((state)->copy_to_far)" not in player
    assert "#define dos_hw_io_text_color_memory_raw(state) ((state)->text_color_memory)" not in player
    assert "static db (*dos_hw_io_port_read_raw(const DosHardwareIo *state))(dw port)" not in player
    assert "static void (*dos_hw_io_port_write_raw(const DosHardwareIo *state))(dw port, db value)" not in player
    assert "static unsigned long (*dos_hw_io_far_physical_raw(const DosHardwareIo *state))(const void far *ptr)" not in player
    assert "static void (*dos_hw_io_copy_to_far_raw(const DosHardwareIo *state))(void far *dst, const void *src, dw byte_count)" not in player
    assert "static db far *(*dos_hw_io_text_color_memory_raw(const DosHardwareIo *state))(void)" not in player
    assert "return io->port_read;" not in player
    assert "return io->port_write;" not in player
    assert "return io->far_physical;" not in player
    assert "return io->copy_to_far;" not in player
    assert "return io->text_color_memory;" not in player
    assert "return dos_hw_io_port_read_field(dos_hw_io);" in player
    assert "return dos_hw_io_port_write_field(dos_hw_io);" in player
    assert "return dos_hw_io_far_physical_field(dos_hw_io);" in player
    assert "return dos_hw_io_copy_to_far_field(dos_hw_io);" in player
    assert "return dos_hw_io_text_color_memory_field(dos_hw_io);" in player
    assert "return dos_hw_io->port_read;" not in player
    assert "return dos_hw_io->port_write;" not in player
    assert "return dos_hw_io->far_physical;" not in player
    assert "return dos_hw_io->copy_to_far;" not in player
    assert "return dos_hw_io->text_color_memory;" not in player
    assert "dos_hw_io->port_read(port)" not in player
    assert "dos_hw_io->port_write(port, value)" not in player
    assert "dos_hw_io->far_physical(ptr)" not in player
    assert "dos_hw_io->copy_to_far(dst, src, byte_count)" not in player
    assert "dos_hw_io->text_color_memory()" not in player
    assert "typedef struct DosSb16Hardware" in player
    assert "typedef struct Sb16PreparedBlock" in player
    assert "dw byte_count;" in player
    assert "dw samples;" in player
    assert "static db far sb16_dma_buffer[IPLAY_SB16_DMA_BUFFER_BYTES];" in player
    assert "static DosSb16Hardware *player_sb16_hardware(void)" in player
    assert "#define player_sb16_base_port_field(state) ((state)->base_port)" in player
    assert "#define player_sb16_irq_field(state) ((state)->irq)" in player
    assert "#define player_sb16_dma16_field(state) ((state)->dma16)" in player
    assert "#define player_sb16_sample_rate_field(state) ((state)->sample_rate)" in player
    assert "#define player_sb16_base_port_raw(state) ((state)->base_port)" not in player
    assert "#define player_sb16_irq_raw(state) ((state)->irq)" not in player
    assert "#define player_sb16_dma16_raw(state) ((state)->dma16)" not in player
    assert "#define player_sb16_sample_rate_raw(state) ((state)->sample_rate)" not in player
    assert "static dw player_sb16_base_port(const DosSb16Hardware *hw)" in player
    assert "static db player_sb16_irq(const DosSb16Hardware *hw)" in player
    assert "static db player_sb16_dma16(const DosSb16Hardware *hw)" in player
    assert "static dw player_sb16_sample_rate(const DosSb16Hardware *hw)" in player
    assert "static void player_sb16_set_base_port(DosSb16Hardware *hw, dw base_port)" in player
    assert "static void player_sb16_set_irq(DosSb16Hardware *hw, db irq)" in player
    assert "static void player_sb16_set_dma16(DosSb16Hardware *hw, db dma16)" in player
    assert "static void player_configure_sb16_from_blaster(void)" in player
    assert 'const char *p = getenv("BLASTER");' in player
    assert "case 'A':" in player
    assert "case 'I':" in player
    assert "case 'H':" in player
    assert "case 'D':" in player
    assert "if (sb16_blaster_dma16_valid(value))" in player
    assert "return value >= 5ul && value <= 7ul;" in player
    assert "player_configure_sb16_from_blaster();" in player
    assert "return &sb16_hw;" in player
    assert "static dw player_sb16_base_port_raw(const DosSb16Hardware *state)" not in player
    assert "static db player_sb16_irq_raw(const DosSb16Hardware *state)" not in player
    assert "static db player_sb16_dma16_raw(const DosSb16Hardware *state)" not in player
    assert "static dw player_sb16_sample_rate_raw(const DosSb16Hardware *state)" not in player
    assert "return player_sb16_base_port_field(hw);" in player
    assert "return player_sb16_irq_field(hw);" in player
    assert "return player_sb16_dma16_field(hw);" in player
    assert "return player_sb16_sample_rate_field(hw);" in player
    assert "player_sb16_base_port_field(hw) = base_port;" in player
    assert "player_sb16_irq_field(hw) = irq;" in player
    assert "player_sb16_dma16_field(hw) = dma16;" in player
    assert "return hw->base_port;" not in player
    assert "return hw->irq;" not in player
    assert "return hw->dma16;" not in player
    assert "return hw->sample_rate;" not in player
    assert "dma_programmed" in player
    assert "last_block_bytes" in player
    assert "blocks_started" in player
    assert "static void sb16_mark_dma_programmed(DosSb16Hardware *hw, dw byte_count)" in player
    assert "#define sb16_set_dma_programmed_field(state, value) ((state)->dma_programmed = (value))" in player
    assert "#define sb16_set_last_block_bytes_field(state, value) ((state)->last_block_bytes = (value))" in player
    assert "#define sb16_set_dma_programmed_raw(state, value) ((state)->dma_programmed = (value))" not in player
    assert "#define sb16_set_last_block_bytes_raw(state, value) ((state)->last_block_bytes = (value))" not in player
    assert "static void sb16_set_dma_programmed_raw(DosSb16Hardware *state, db programmed)" not in player
    assert "static void sb16_set_last_block_bytes_raw(DosSb16Hardware *state, dw byte_count)" not in player
    assert "state->dma_programmed = programmed;" not in player
    assert "state->last_block_bytes = byte_count;" not in player
    assert "static void sb16_set_dma_programmed(DosSb16Hardware *hw, db programmed)" in player
    assert "static void sb16_set_last_block_bytes(DosSb16Hardware *hw, dw byte_count)" in player
    assert "sb16_set_dma_programmed_field(hw, programmed);" in player
    assert "sb16_set_last_block_bytes_field(hw, byte_count);" in player
    assert "sb16_set_dma_programmed(hw, 1);" in player
    assert "sb16_set_last_block_bytes(hw, byte_count);" in player
    assert "hw->dma_programmed = programmed;" not in player
    assert "hw->last_block_bytes = byte_count;" not in player
    assert "static void sb16_count_started_block(DosSb16Hardware *hw)" in player
    assert "sb16_add_started_blocks(hw, 1u);" in player
    assert "static int sb16_reset" in player
    assert "static unsigned long sb16_dma16_word_address(unsigned long phys)" in player
    assert "return phys >> 1;" in player
    assert "static dw sb16_dma16_word_count(dw byte_count)" in player
    assert "return (dw)(byte_count >> 1);" in player
    assert "static db sb16_dma16_channel_index(db dma16)" in player
    assert "return (db)(dma16 - IPLAY_SB16_DMA16_CHANNEL_BASE);" in player
    assert "static dw sb16_dma16_address_port(db chan)" in player
    assert "return (dw)(IPLAY_SB16_DMA16_PORT_ADDRESS_BASE + (dw)chan * IPLAY_SB16_DMA16_PORT_STRIDE);" in player
    assert "static dw sb16_dma16_count_port(db chan)" in player
    assert "return (dw)(IPLAY_SB16_DMA16_PORT_COUNT_BASE + (dw)chan * IPLAY_SB16_DMA16_PORT_STRIDE);" in player
    assert "static dw sb16_dma16_page_port(db chan)" in player
    assert "return IPLAY_SB16_DMA16_PORT_PAGE_CH6;" in player
    assert "return IPLAY_SB16_DMA16_PORT_PAGE_CH7;" in player
    assert "return IPLAY_SB16_DMA16_PORT_PAGE_CH5;" in player
    assert "static dw sb16_dma16_terminal_count(dw word_count)" in player
    assert "return (dw)(word_count - 1u);" in player
    assert "static db sb16_dma16_byte_lo(unsigned long value)" in player
    assert "return (db)value;" in player
    assert "static db sb16_dma16_byte_hi(unsigned long value)" in player
    assert "return (db)(value >> 8);" in player
    assert "static db sb16_dma16_page_byte(unsigned long phys)" in player
    assert "return (db)(phys >> 16);" in player
    assert "static db sb16_dma16_disable_mask_value(db chan)" in player
    assert "return (db)(IPLAY_SB16_DMA_MASK_DISABLE | chan);" in player
    assert "static db sb16_dma16_enable_mask_value(db chan)" in player
    assert "return chan;" in player
    assert "static db sb16_dma16_playback_mode_value(db chan)" in player
    assert "return (db)(IPLAY_SB16_DMA_MODE_PLAYBACK | chan);" in player
    assert "static void sb16_dma16_mask_channel(db chan)" in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_MASK, sb16_dma16_disable_mask_value(chan));" in player
    assert "static void sb16_dma16_clear_flipflop(void)" in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_CLEAR_FLIPFLOP, IPLAY_SB16_DMA_CLEAR_FLIPFLOP);" in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_CLEAR_FLIPFLOP, 0);" not in player
    assert "static void sb16_dma16_set_playback_mode(db chan)" in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_MODE, sb16_dma16_playback_mode_value(chan));" in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_MODE, (db)(IPLAY_SB16_DMA_MODE_PLAYBACK | chan));" not in player
    assert "static void sb16_dma16_unmask_channel(db chan)" in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_MASK, sb16_dma16_enable_mask_value(chan));" in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_MASK, (db)(IPLAY_SB16_DMA_MASK_DISABLE | chan));" not in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_MASK, chan);" not in player
    assert "static void sb16_dma16_write_address(db chan, unsigned long word_addr, unsigned long phys)" in player
    assert "dos_hw_io_write_port(sb16_dma16_address_port(chan), sb16_dma16_byte_lo(word_addr));" in player
    assert "dos_hw_io_write_port(sb16_dma16_address_port(chan), sb16_dma16_byte_hi(word_addr));" in player
    assert "dos_hw_io_write_port(sb16_dma16_page_port(chan), sb16_dma16_page_byte(phys));" in player
    assert "static void sb16_dma16_write_count(db chan, dw terminal_count)" in player
    assert "dos_hw_io_write_port(sb16_dma16_count_port(chan), sb16_dma16_byte_lo(terminal_count));" in player
    assert "dos_hw_io_write_port(sb16_dma16_count_port(chan), sb16_dma16_byte_hi(terminal_count));" in player
    assert "IPLAY_SB16_DMA16_PORT_ADDRESS 0xc4u" not in player
    assert "IPLAY_SB16_DMA16_PORT_COUNT 0xc6u" not in player
    assert "IPLAY_SB16_DMA16_PORT_PAGE 0x8bu" not in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_ADDRESS, (db)word_addr);" not in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_ADDRESS, (db)(word_addr >> 8));" not in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_PAGE, (db)(phys >> 16));" not in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_COUNT, (db)terminal_count);" not in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_COUNT, (db)(terminal_count >> 8));" not in player
    assert "static void sb16_program_dma16" in player
    assert "unsigned long word_addr = sb16_dma16_word_address(phys);" in player
    assert "dw word_count = sb16_dma16_word_count(byte_count);" in player
    assert "dw terminal_count = sb16_dma16_terminal_count(word_count);" in player
    assert "db chan = sb16_dma16_channel_index(player_sb16_dma16(hw));" in player
    assert "sb16_dma16_mask_channel(chan);" in player
    assert "sb16_dma16_clear_flipflop();" in player
    assert "sb16_dma16_set_playback_mode(chan);" in player
    assert "sb16_dma16_write_address(chan, word_addr, phys);" in player
    assert "sb16_dma16_write_count(chan, terminal_count);" in player
    assert "sb16_dma16_unmask_channel(chan);" in player
    assert "sb16_mark_dma_programmed(hw, byte_count);" in player
    assert (
        "sb16_dma16_unmask_channel(chan);\n"
        "    hw->dma_programmed = 1;"
    ) not in player
    assert (
        "hw->dma_programmed = 1;\n"
        "    hw->last_block_bytes = byte_count;"
    ) not in player
    assert "hw->dma16 - 4u" not in player
    assert "hw->dma16 - IPLAY_SB16_DMA16_CHANNEL_BASE" not in player
    assert "dos_hw_io_write_port(0xd4u" not in player
    assert "dos_hw_io_write_port(0xd8u" not in player
    assert "dos_hw_io_write_port(0xd6u" not in player
    assert "dos_hw_io_write_port(0xc4u" not in player
    assert "dos_hw_io_write_port(0x8bu" not in player
    assert "dos_hw_io_write_port(0xc6u" not in player
    assert "0x04u | chan" not in player
    assert "0x48u | chan" not in player
    assert (
        "db chan = sb16_dma16_channel_index(player_sb16_dma16(hw));\n"
        "    dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_MASK, (db)(IPLAY_SB16_DMA_MASK_DISABLE | chan));"
    ) not in player
    assert (
        "sb16_dma16_mask_channel(chan);\n"
        "    dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_CLEAR_FLIPFLOP, 0);"
    ) not in player
    assert (
        "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_CLEAR_FLIPFLOP, 0);\n"
        "    sb16_dma16_set_playback_mode(chan);"
    ) not in player
    assert (
        "sb16_dma16_set_playback_mode(chan);\n"
        "    dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_ADDRESS, (db)word_addr);"
    ) not in player
    assert (
        "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_PAGE, (db)(phys >> 16));\n"
        "    dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_COUNT, (db)terminal_count);"
    ) not in player
    assert "unsigned long word_addr = phys >> 1;" not in player
    assert "dw word_count = (dw)(byte_count >> 1);" not in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_COUNT, (db)(word_count - 1u));" not in player
    assert "dos_hw_io_write_port(IPLAY_SB16_DMA16_PORT_COUNT, (db)((word_count - 1u) >> 8));" not in player
    assert "dw sample_rate = player_sb16_sample_rate(hw);" in player
    assert "static int sb16_dsp_set_output_rate(dw base_port, dw sample_rate)" in player
    assert "sb16_dsp_write(base_port, IPLAY_SB16_DSP_SET_OUTPUT_RATE)" in player
    assert "sb16_dsp_write(base_port, sb16_dsp_word_hi(sample_rate))" in player
    assert "sb16_dsp_write(base_port, sb16_dsp_word_lo(sample_rate))" in player
    assert "return sb16_dsp_set_output_rate(base_port, sample_rate);" in player
    assert "sb16_dsp_write(hw->base_port, IPLAY_SB16_DSP_SET_OUTPUT_RATE)" not in player
    assert "sb16_dsp_write(hw->base_port, sb16_dsp_word_hi(hw->sample_rate))" not in player
    assert "sb16_dsp_write(hw->base_port, sb16_dsp_word_lo(hw->sample_rate))" not in player
    assert "sb16_dsp_write(hw->base_port, (db)(hw->sample_rate >> 8))" not in player
    assert "sb16_dsp_write(hw->base_port, (db)hw->sample_rate)" not in player
    assert "static dw sb16_block_sample_count(dw byte_count)" in player
    assert "return (dw)((byte_count >> 1) - 1u);" in player
    assert "static int sb16_block_has_payload(dw byte_count)" in player
    assert "return byte_count >= 4u;" in player
    assert "static dw sb16_block_aligned_byte_count(dw byte_count)" in player
    assert "return (dw)(byte_count & (dw)~3u);" in player
    assert "#define sb16_prepared_block_set_byte_count_field(state, value) ((state)->byte_count = (value))" in player
    assert "#define sb16_prepared_block_set_samples_field(state, value) ((state)->samples = (value))" in player
    assert "#define sb16_prepared_block_byte_count_field(state) ((state)->byte_count)" in player
    assert "#define sb16_prepared_block_samples_field(state) ((state)->samples)" in player
    assert "#define sb16_prepared_block_set_byte_count_raw(state, value) ((state)->byte_count = (value))" not in player
    assert "#define sb16_prepared_block_set_samples_raw(state, value) ((state)->samples = (value))" not in player
    assert "#define sb16_prepared_block_byte_count_raw(state) ((state)->byte_count)" not in player
    assert "#define sb16_prepared_block_samples_raw(state) ((state)->samples)" not in player
    assert "static void sb16_prepared_block_set_byte_count_raw(Sb16PreparedBlock *state, dw byte_count)" not in player
    assert "static void sb16_prepared_block_set_samples_raw(Sb16PreparedBlock *state, dw samples)" not in player
    assert "static dw sb16_prepared_block_byte_count_raw(const Sb16PreparedBlock *state)" not in player
    assert "static dw sb16_prepared_block_samples_raw(const Sb16PreparedBlock *state)" not in player
    assert "static void sb16_prepared_block_set_byte_count(Sb16PreparedBlock *block, dw byte_count)" in player
    assert "static void sb16_prepared_block_set_samples(Sb16PreparedBlock *block, dw samples)" in player
    assert "sb16_prepared_block_set_byte_count_field(block, byte_count);" in player
    assert "sb16_prepared_block_set_samples_field(block, samples);" in player
    assert "static int sb16_prepare_16bit_stereo_block(dw byte_count, Sb16PreparedBlock *block)" in player
    assert "dw aligned_byte_count;" in player
    assert "aligned_byte_count = sb16_block_aligned_byte_count(byte_count);" in player
    assert "sb16_prepared_block_set_byte_count(block, aligned_byte_count);" in player
    assert "sb16_prepared_block_set_samples(block, sb16_block_sample_count(aligned_byte_count));" in player
    assert "static dw sb16_prepared_block_byte_count(const Sb16PreparedBlock *block)" in player
    assert "return sb16_prepared_block_byte_count_field(block);" in player
    assert "static dw sb16_prepared_block_samples(const Sb16PreparedBlock *block)" in player
    assert "return sb16_prepared_block_samples_field(block);" in player
    assert "block->byte_count = byte_count;" not in player
    assert "block->samples = samples;" not in player
    assert "return block->byte_count;" not in player
    assert "return block->samples;" not in player
    assert "block->byte_count = sb16_block_aligned_byte_count(byte_count);" not in player
    assert "block->samples = sb16_block_sample_count(block->byte_count);" not in player
    assert "static int sb16_start_16bit_stereo_dsp(DosSb16Hardware *hw, dw samples)" in player
    assert "static int sb16_dsp_speaker_on(dw base_port)" in player
    assert "return sb16_dsp_write(base_port, IPLAY_SB16_DSP_SPEAKER_ON);" in player
    assert "static int sb16_dsp_start_16bit_stereo_output(dw base_port)" in player
    assert "if (!sb16_dsp_write(base_port, IPLAY_SB16_DSP_OUTPUT_16BIT)) return 0;" in player
    assert "return sb16_dsp_write(base_port, IPLAY_SB16_DSP_MODE_STEREO_SIGNED);" in player
    assert "static int sb16_dsp_write_sample_count(dw base_port, dw samples)" in player
    assert "if (!sb16_dsp_write(base_port, sb16_dsp_word_lo(samples))) return 0;" in player
    assert "return sb16_dsp_write(base_port, sb16_dsp_word_hi(samples));" in player
    assert "static int sb16_is_active(const DosSb16Hardware *hw)" in player
    assert "if (!sb16_is_active(hw) && !sb16_dsp_speaker_on(base_port)) return 0;" in player
    assert "if (!sb16_dsp_start_16bit_stereo_output(base_port)) return 0;" in player
    assert "if (!sb16_dsp_write_sample_count(base_port, samples)) return 0;" in player
    assert "if (!sb16_dsp_write(base_port, IPLAY_SB16_DSP_SPEAKER_ON)) return 0;" not in player
    assert "if (!sb16_dsp_write(base_port, IPLAY_SB16_DSP_MODE_STEREO_SIGNED)) return 0;" not in player
    assert "if (!sb16_dsp_write(base_port, sb16_dsp_word_hi(samples))) return 0;" not in player
    assert "if (!sb16_dsp_write(hw->base_port, (db)samples)) return 0;" not in player
    assert "if (!sb16_dsp_write(hw->base_port, (db)(samples >> 8))) return 0;" not in player
    assert "static void sb16_mark_active(DosSb16Hardware *hw)" in player
    assert "#define sb16_set_active_field(state, value) ((state)->active = (value))" in player
    assert "#define sb16_add_started_blocks_field(state, value) ((state)->blocks_started += (value))" in player
    assert "#define sb16_add_written_bytes_field(state, value) ((state)->bytes_written += (value))" in player
    assert "#define sb16_set_active_raw(state, value) ((state)->active = (value))" not in player
    assert "#define sb16_add_started_blocks_raw(state, value) ((state)->blocks_started += (value))" not in player
    assert "#define sb16_add_written_bytes_raw(state, value) ((state)->bytes_written += (value))" not in player
    assert "static void sb16_set_active_raw(DosSb16Hardware *state, db active)" not in player
    assert "static void sb16_add_started_blocks_raw(DosSb16Hardware *state, dd blocks)" not in player
    assert "static void sb16_add_written_bytes_raw(DosSb16Hardware *state, dw bytes)" not in player
    assert "state->active = active;" not in player
    assert "state->blocks_started += blocks;" not in player
    assert "state->bytes_written += bytes;" not in player
    assert "static void sb16_set_active(DosSb16Hardware *hw, db active)" in player
    assert "static void sb16_add_started_blocks(DosSb16Hardware *hw, dd blocks)" in player
    assert "static void sb16_add_written_bytes(DosSb16Hardware *hw, dw bytes)" in player
    assert "sb16_set_active_field(hw, active);" in player
    assert "sb16_add_started_blocks_field(hw, blocks);" in player
    assert "sb16_add_written_bytes_field(hw, bytes);" in player
    assert "sb16_set_active(hw, 1);" in player
    assert "sb16_add_started_blocks(hw, 1u);" in player
    assert "sb16_add_written_bytes(hw, byte_count);" in player
    assert "static void sb16_count_written_bytes(DosSb16Hardware *hw, dw byte_count)" in player
    assert "hw->active = active;" not in player
    assert "hw->blocks_started += blocks;" not in player
    assert "hw->bytes_written += bytes;" not in player
    assert "hw->active = 1;" not in player
    assert "hw->blocks_started += 1u;" not in player
    assert "hw->bytes_written += byte_count;" not in player
    assert "static void sb16_commit_started_block(DosSb16Hardware *hw, dw byte_count)" in player
    assert "sb16_mark_active(hw);" in player
    assert "sb16_count_started_block(hw);" in player
    assert "sb16_count_written_bytes(hw, byte_count);" in player
    assert "static int sb16_start_prepared_16bit_stereo_block(DosSb16Hardware *hw, const db far *buffer, const Sb16PreparedBlock *block)" in player
    assert "static int sb16_start_prepared_16bit_stereo_dsp(DosSb16Hardware *hw, const Sb16PreparedBlock *block)" in player
    assert "if (!sb16_is_active(hw) && !sb16_set_rate(hw)) return 0;" in player
    assert "return sb16_start_16bit_stereo_dsp(hw, sb16_prepared_block_samples(block));" in player
    assert "sb16_program_dma16(hw, buffer, sb16_prepared_block_byte_count(block));" in player
    assert "if (!sb16_start_prepared_16bit_stereo_dsp(hw, block)) return 0;" in player
    assert "if (!sb16_start_16bit_stereo_dsp(hw, sb16_prepared_block_samples(block))) return 0;" not in player
    assert "sb16_commit_started_block(hw, sb16_prepared_block_byte_count(block));" in player
    assert "static int sb16_start_16bit_stereo_block" in player
    assert "Sb16PreparedBlock block;" in player
    assert "if (!sb16_prepare_16bit_stereo_block(byte_count, &block)) return 0;" in player
    assert "return sb16_start_prepared_16bit_stereo_block(hw, buffer, &block);" in player
    assert "sb16_program_dma16(hw, buffer, sb16_prepared_block_byte_count(&block));" not in player
    assert "if (!sb16_start_16bit_stereo_dsp(hw, sb16_prepared_block_samples(&block))) return 0;" not in player
    assert "sb16_commit_started_block(hw, sb16_prepared_block_byte_count(&block));" not in player
    assert "sb16_program_dma16(hw, buffer, block.byte_count);" not in player
    assert "if (!sb16_start_16bit_stereo_dsp(hw, block.samples)) return 0;" not in player
    assert "sb16_commit_started_block(hw, block.byte_count);" not in player
    assert "if (!sb16_start_16bit_stereo_dsp(hw, samples)) return 0;" not in player
    assert "\n    byte_count = sb16_block_aligned_byte_count(byte_count);" not in player
    assert "samples = sb16_block_sample_count(byte_count);" not in player
    assert "sb16_commit_started_block(hw, byte_count);" not in player
    assert "samples = (dw)((byte_count >> 1) - 1u);" not in player
    assert (
        "if (byte_count < 4u) return 0;\n"
        "    byte_count &= (dw)~3u;"
    ) not in player
    assert (
        "if (!sb16_set_rate(hw)) return 0;\n"
        "    if (!sb16_dsp_write(hw->base_port, IPLAY_SB16_DSP_SPEAKER_ON)) return 0;"
    ) not in player
    assert (
        "if (!sb16_start_16bit_stereo_dsp(hw, samples)) return 0;\n"
        "    hw->active = 1;"
    ) not in player
    assert (
        "static void sb16_commit_started_block(DosSb16Hardware *hw, dw byte_count) {\n"
        "    hw->active = 1;"
    ) not in player
    assert (
        "hw->active = 1;\n"
        "    hw->blocks_started += 1u;"
    ) not in player
    assert "sb16_dsp_write(hw->base_port, 0x41u)" not in player
    assert "sb16_dsp_write(hw->base_port, 0xd1u)" not in player
    assert "dos_hw_io_write_port(sb16_dsp_write_data_port(player_sb16_base_port(hw)), IPLAY_SB16_DSP_SPEAKER_OFF);" in player
    assert "(void)sb16_dsp_write(player_sb16_base_port(hw), IPLAY_SB16_DSP_SPEAKER_OFF);" not in player
    assert 'streq(argv[1], "playersb16hwtwoblocks")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert '"playersb16hwtwoblocks"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert "static void run_sb16_hw_dma6(void)" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'streq(argv[1], "playersb16hwdma6")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert '"playersb16hwdma6"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert "static void run_sb16_hw_dma7(void)" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'streq(argv[1], "playersb16hwdma7")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert '"playersb16hwdma7"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert "static dw hw_mock_sb_base = IPLAY_SB16_DEFAULT_BASE;" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "static void run_sb16_hw_base240(void)" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'streq(argv[1], "playersb16hwbase240")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert '"playersb16hwbase240"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    function_parity = (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "test_translated_player_sb16_hardware_programs_configured_dma6_ports" in function_parity
    assert '"0c8:a0,0c8:91,089:01,0ca:03,0ca:00"' in function_parity
    assert "test_translated_player_sb16_hardware_programs_configured_dma7_ports" in function_parity
    assert '"0cc:a0,0cc:91,08a:01,0ce:03,0ce:00"' in function_parity
    assert "test_translated_player_sb16_hardware_programs_configured_base_port" in function_parity
    assert '"24c:41,24c:ac,24c:44,24c:d1,24c:b0,24c:30,24c:03,24c:00,24c:d5,246:01,246:00"' in function_parity
    assert "IPLAY_SB16_DSP_SPEAKER_OFF 0xd5u" in player
    assert "static void sb16_stop_16bit_stereo_dsp(DosSb16Hardware *hw)" in player
    assert "dos_hw_io_write_port(sb16_dsp_write_data_port(player_sb16_base_port(hw)), IPLAY_SB16_DSP_SPEAKER_OFF);" in player
    assert "(void)sb16_dsp_write(player_sb16_base_port(hw), IPLAY_SB16_DSP_SPEAKER_OFF);" not in player
    assert "sb16_stop_16bit_stereo_dsp(hw);" in player
    assert "(void)sb16_reset(hw);" in player
    assert "sb16_dsp_write(hw->base_port, 0xd5u)" not in player
    assert "sb16_dsp_write(hw->base_port, 0xb0u)" not in player
    assert "sb16_dsp_write(hw->base_port, 0x30u)" not in player
    assert "static void sb16_mark_inactive(DosSb16Hardware *hw)" in player
    assert "sb16_set_active(hw, 0);" in player
    assert "static void sb16_mark_dma_idle(DosSb16Hardware *hw)" in player
    assert "sb16_set_dma_programmed(hw, 0);" in player
    assert "static DosSb16Hardware *sb16_audio_user_hardware(void *user)" in player
    assert "return (DosSb16Hardware *)user;" in player
    assert "static int sb16_audio_ensure_ready(DosSb16Hardware *hw)" in player
    assert "return sb16_is_detected(hw) || sb16_reset(hw);" in player
    assert "static dw sb16_audio_dma_copy_count(dw byte_count)" in player
    assert "static dw sb16_dma_buffer_capacity(void)" in player
    assert "return IPLAY_SB16_DMA_BUFFER_BYTES;" in player
    assert "dw capacity = sb16_dma_buffer_capacity();" in player
    assert "dw copy_count = byte_count > capacity ? capacity : byte_count;" in player
    assert "byte_count > IPLAY_SB16_DMA_BUFFER_BYTES ? IPLAY_SB16_DMA_BUFFER_BYTES : byte_count" not in player
    assert "static dw sb16_dma_align_16bit_stereo_bytes(dw byte_count)" in player
    assert "return (dw)(byte_count & (dw)~3u);" in player
    assert "return sb16_dma_align_16bit_stereo_bytes(copy_count);" in player
    assert "return (dw)(copy_count & (dw)~3u);" not in player
    assert "static db far *sb16_dma_buffer_memory(void)" in player
    assert "return sb16_dma_buffer;" in player
    assert "static void sb16_audio_copy_to_dma(const db *pcm, dw byte_count)" in player
    assert "dos_hw_io_copy_to_far(sb16_dma_buffer_memory(), pcm, byte_count);" in player
    assert "static void sb16_audio_start_dma_block(DosSb16Hardware *hw, dw byte_count)" in player
    assert "(void)sb16_start_16bit_stereo_block(hw, sb16_dma_buffer_memory(), byte_count);" in player
    assert "static void sb16_audio_submit_dma_block(DosSb16Hardware *hw, const db *pcm, dw byte_count)" in player
    assert "sb16_audio_copy_to_dma(pcm, byte_count);" in player
    assert "sb16_audio_start_dma_block(hw, byte_count);" in player
    assert "static void sb16_audio_submit_pcm(DosSb16Hardware *hw, const db *pcm, dw byte_count)" in player
    assert "dw copy_count = sb16_audio_dma_copy_count(byte_count);" in player
    assert "if (copy_count == 0) return;" in player
    assert "sb16_audio_submit_dma_block(hw, pcm, copy_count);" in player
    assert "static void sb16_audio_write" in player
    assert "DosSb16Hardware *hw = sb16_audio_user_hardware(user);" in player
    assert "if (!sb16_audio_ensure_ready(hw)) return;" in player
    assert "sb16_audio_submit_pcm(hw, pcm, byte_count);" in player
    assert "    copy_count = sb16_audio_dma_copy_count(byte_count);" not in player
    assert "sb16_audio_copy_to_dma(pcm, copy_count);" not in player
    assert "sb16_audio_start_dma_block(hw, copy_count);" not in player
    assert "#define player_audio_backend_set_write_field(state, value) ((state)->write = (value))" in player
    assert "#define player_audio_backend_set_user_field(state, value) ((state)->user = (value))" in player
    assert "#define player_audio_backend_write_field(state) ((state)->write)" in player
    assert "#define player_audio_backend_user_field(state) ((state)->user)" in player
    assert "#define player_audio_backend_set_write_raw(state, value) ((state)->write = (value))" not in player
    assert "#define player_audio_backend_set_user_raw(state, value) ((state)->user = (value))" not in player
    assert "#define player_audio_backend_write_raw(state) ((state)->write)" not in player
    assert "#define player_audio_backend_user_raw(state) ((state)->user)" not in player
    assert "static void player_audio_backend_set_write_raw(PlayerAudioBackend *state, IplayAudioWriteFn fn)" not in player
    assert "static void player_audio_backend_set_user_raw(PlayerAudioBackend *state, void *ptr)" not in player
    assert "static IplayAudioWriteFn player_audio_backend_write_raw(const PlayerAudioBackend *state)" not in player
    assert "static void *player_audio_backend_user_raw(const PlayerAudioBackend *state)" not in player
    assert "static void player_audio_backend_set_write(PlayerAudioBackend *backend, IplayAudioWriteFn write)" in player
    assert "player_audio_backend_set_write_field(backend, write);" in player
    assert "static void player_audio_backend_set_user(PlayerAudioBackend *backend, void *user)" in player
    assert "player_audio_backend_set_user_field(backend, user);" in player
    assert "static void player_audio_backend_init(PlayerAudioBackend *backend, IplayAudioWriteFn write, void *user)" in player
    assert "player_audio_backend_set_write(backend, write);" in player
    assert "player_audio_backend_set_user(backend, user);" in player
    assert "static void player_audio_backend_init_discard(PlayerAudioBackend *backend)" in player
    assert "player_audio_backend_init(backend, player_audio_discard, NULL);" in player
    assert "static IplayAudioWriteFn player_audio_backend_write(const PlayerAudioBackend *backend)" in player
    assert "return player_audio_backend_write_field(backend);" in player
    assert "static void *player_audio_backend_user(const PlayerAudioBackend *backend)" in player
    assert "return player_audio_backend_user_field(backend);" in player
    assert "#define player_video_backend_set_present_field(state, value) ((state)->present = (value))" in player
    assert "#define player_video_backend_set_user_field(state, value) ((state)->user = (value))" in player
    assert "#define player_video_backend_present_field(state) ((state)->present)" in player
    assert "#define player_video_backend_user_field(state) ((state)->user)" in player
    assert "#define player_video_backend_set_present_raw(state, value) ((state)->present = (value))" not in player
    assert "#define player_video_backend_set_user_raw(state, value) ((state)->user = (value))" not in player
    assert "#define player_video_backend_present_raw(state) ((state)->present)" not in player
    assert "#define player_video_backend_user_raw(state) ((state)->user)" not in player
    assert "static void player_video_backend_set_present_raw(PlayerVideoBackend *state, IplayVideoPresentFn fn)" not in player
    assert "static void player_video_backend_set_user_raw(PlayerVideoBackend *state, void *ptr)" not in player
    assert "static IplayVideoPresentFn player_video_backend_present_raw(const PlayerVideoBackend *state)" not in player
    assert "static void *player_video_backend_user_raw(const PlayerVideoBackend *state)" not in player
    assert "static void player_video_backend_set_present(PlayerVideoBackend *backend, IplayVideoPresentFn present)" in player
    assert "player_video_backend_set_present_field(backend, present);" in player
    assert "static void player_video_backend_set_user(PlayerVideoBackend *backend, void *user)" in player
    assert "player_video_backend_set_user_field(backend, user);" in player
    assert "static void player_video_backend_init(PlayerVideoBackend *backend, IplayVideoPresentFn present, void *user)" in player
    assert "player_video_backend_set_present(backend, present);" in player
    assert "player_video_backend_set_user(backend, user);" in player
    assert "static IplayVideoPresentFn player_video_backend_present(const PlayerVideoBackend *backend)" in player
    assert "return player_video_backend_present_field(backend);" in player
    assert "static void *player_video_backend_user(const PlayerVideoBackend *backend)" in player
    assert "return player_video_backend_user_field(backend);" in player
    assert "backend->write = write;" not in player
    assert "backend->user = user;" not in player
    assert "backend->present = present;" not in player
    assert "backend->write = fn;" not in player
    assert "backend->user = ptr;" not in player
    assert "backend->present = fn;" not in player
    assert "return backend->write;" not in player
    assert "return backend->user;" not in player
    assert "return backend->present;" not in player
    assert "static PlayerAudioBackend player_sb16_audio_backend;" not in player
    assert "static PlayerAudioBackend *player_sb16_runtime_audio_backend(void)" not in player
    assert "static void player_init_sb16_audio_backend(void)" not in player
    assert "static void player_audio_backend_init_sb16(PlayerAudioBackend *backend)" in player
    assert "player_audio_backend_init(backend, sb16_audio_write, player_sb16_hardware());" in player
    assert "DosSb16Hardware *hw = (DosSb16Hardware *)user;" not in player
    assert "if (!hw->detected && !sb16_reset(hw)) return;" not in player
    assert "dos_hw_io_copy_to_far(sb16_dma_buffer, pcm, copy_count);" not in player
    assert "dos_hw_io_copy_to_far(sb16_dma_buffer, pcm, byte_count);" not in player
    assert "(void)sb16_start_16bit_stereo_block(hw, sb16_dma_buffer, byte_count);" not in player
    assert "(void)sb16_start_16bit_stereo_block(hw, sb16_dma_buffer, copy_count);" not in player
    assert (
        "copy_count = byte_count > IPLAY_SB16_DMA_BUFFER_BYTES ? IPLAY_SB16_DMA_BUFFER_BYTES : byte_count;\n"
        "    copy_count &= (dw)~3u;"
    ) not in player
    assert "sb16_audio_write, player_sb16_hardware()" in player
    assert "sb16_shutdown(player_sb16_hardware());" in player
    assert "if (!sb16_is_detected(hw)) return;" in player
    assert "sb16_mark_inactive(hw);" in player
    assert "sb16_mark_dma_idle(hw);" in player
    assert "DosSb16Hardware *hw = player_sb16_hardware();" in player
    assert "sb16_audio_write, &sb16_hw" not in player
    assert "sb16_shutdown(&sb16_hw);" not in player
    assert "return hw->detected || sb16_reset(hw);" not in player
    assert "if (!hw->detected) return;" not in player
    assert (
        "sb16_stop_16bit_stereo_dsp(hw);\n"
        "    hw->active = 0;"
    ) not in player
    assert (
        "hw->active = 0;\n"
        "    hw->dma_programmed = 0;"
    ) not in player
    assert "Playback output: SB16 16-bit stereo hardware wrapper enabled." in player
    assert "SB16 config: base=%03Xh irq=%u dma16=%u rate=%u" in player
    assert "(unsigned)player_sb16_base_port(hw)" in player
    assert "(unsigned)player_sb16_irq(hw)" in player
    assert "(unsigned)player_sb16_dma16(hw)" in player
    assert "(unsigned)player_sb16_sample_rate(hw)" in player
    assert "(unsigned)hw->base_port" not in player
    assert "(unsigned)hw->irq" not in player
    assert "(unsigned)hw->dma16" not in player
    assert "(unsigned)hw->sample_rate" not in player
    assert "SB16 16-bit stereo hardware DMA is still pending" not in player
    assert "static void player_configure_runtime(IplayRuntimeConfig *runtime_config)" in player
    assert "static void player_configure_runtime_output(IplayRuntimeConfig *runtime_config)" in player
    assert "player_configure_runtime_output(runtime_config);" in player
    assert "player_configure_runtime(runtime_config);" in player
    assert "iplay_runtime_config_sb16_hardware_capacity" in player
    assert "static void dos_text_present" in player
    assert "static db far *dos_hw_text_color_memory(void)" in player
    assert "static db far *dos_hw_text_mono_memory(void)" in player
    assert "MK_FP(IPLAY_DOS_TEXT_COLOR_SEG, 0)" in player
    assert "MK_FP(IPLAY_DOS_TEXT_MONO_SEG, 0)" in player
    assert player.count("MK_FP(") == 3
    assert "dos_hw_io_copy_to_far(video, cells, byte_count);" in player
    assert "iplay_runtime_config_sdl_capacity(runtime_config" in player
    assert "dos_text_present" in player
    assert "iplay_runtime_config_no_hardware_capacity(&runtime_config" not in player
    assert "PLAYER_VIDEO_SIZE" in player
    assert "#define PLAYER_VIDEO_SIZE IPLAY_TEXT_MAX_SCREEN_BYTES" in player
    assert "#define IPLAY_PLAYER_DEFAULT_VIDEO_MODE IPLAY_TEXT_DEFAULT_VIDEO_MODE" in player
    assert "static void player_start_runtime(IplayRuntime *runtime, const IplayRuntimeConfig *runtime_config)" in player
    assert "player_start_runtime(runtime, runtime_config);" in player
    assert "static void player_shutdown_audio_hardware(void)" in player
    assert "player_shutdown_audio_hardware();" in player
    assert "iplay_runtime_start_config_checked(runtime, runtime_config, player_text_video_mode_id())" in player
    assert "iplay_runtime_start_config_checked(runtime, runtime_config, IPLAY_TEXT_DEFAULT_VIDEO_MODE)" not in player
    assert "iplay_runtime_start_config_checked(&runtime, &runtime_config" not in player
    assert "iplay_runtime_start_config(&runtime, &runtime_config" not in player
    assert "iplay_runtime_init_config(&runtime, &runtime_config)" not in player
    assert "runtime_config.cells =" not in player
    assert "static IplayVideoPresentFn player_text_video_present_fn(void)" in player
    assert "return dos_text_present;" in player
    assert "static void *player_text_video_present_user(void)" in player
    assert "return dos_text_default_present_user();" in player
    assert "static void player_video_backend_init_text(PlayerVideoBackend *backend)" in player
    assert "player_video_backend_init(backend, player_text_video_present_fn(), player_text_video_present_user());" in player
    assert "player_video_backend_init(backend, dos_text_present, dos_text_default_present_user());" not in player
    assert "db video_mode;" in player
    assert "static db far *dos_text_present_mono_video_memory(void)" in player
    assert "static int dos_text_video_mode_is_mono(db video_mode)" in player
    assert "case IPLAY_VIDEO_MODE_40X25_BW:" in player
    assert "case IPLAY_VIDEO_MODE_80X25_BW:" in player
    assert "static db far *(*dos_text_video_memory_for_mode(db video_mode))(void)" in player
    assert "return dos_text_video_mode_is_mono(video_mode) ? dos_text_present_mono_video_memory : dos_text_present_video_memory;" in player
    assert "#define dos_text_presenter_set_video_mode_field(state, value) ((state)->video_mode = (value))" in player
    assert "#define dos_text_presenter_video_mode_field(state) ((state)->video_mode)" in player
    assert "static void dos_text_presenter_set_video_mode(DosTextPresenter *presenter, db video_mode)" in player
    assert "static db dos_text_presenter_video_mode(const DosTextPresenter *presenter)" in player
    assert "static void dos_text_presenter_init(DosTextPresenter *presenter, db video_mode, void (*copy_to_video)(db far *video, const db *cells, dw byte_count))" in player
    assert "dos_text_presenter_set_video_memory(presenter, dos_text_video_memory_for_mode(video_mode));" in player
    assert "dos_text_presenter_set_video_mode(presenter, video_mode);" in player
    assert "static void dos_text_presenter_init_vga_text_mode(DosTextPresenter *presenter, db video_mode)" in player
    assert "dos_text_presenter_init_vga_text_mode(presenter, IPLAY_TEXT_DEFAULT_VIDEO_MODE);" in player
    assert "static DosTextPresenter *dos_text_default_presenter_state(void)" in player
    assert "return &dos_text_default_presenter;" in player
    assert "dos_text_presenter_init_vga_text(dos_text_default_presenter_state());" in player
    assert "return (void *)dos_text_default_presenter_state();" in player
    assert "dos_text_presenter_init_vga_text(&dos_text_default_presenter);" not in player
    assert "return (void *)&dos_text_default_presenter;" not in player
    assert "static void player_configure_runtime_sb16_output(IplayRuntimeConfig *runtime_config)" in player
    assert "static void player_configure_runtime_sdl_output(IplayRuntimeConfig *runtime_config)" in player
    assert "PlayerRuntimeOutput output;" in player
    assert "typedef struct PlayerRuntimeVideoOutput" in player
    assert "typedef struct PlayerRuntimeAudioOutput" in player
    assert "typedef struct PlayerRuntimeOutputViews" in player
    assert "typedef void (*PlayerRuntimeOutputAudioInitFn)(PlayerRuntimeOutput *output);" in player
    assert "typedef void (*PlayerRuntimeOutputInitFn)(PlayerRuntimeOutput *output);" in player
    assert "typedef void (*PlayerRuntimeOutputApplyFn)(IplayRuntimeConfig *runtime_config, PlayerRuntimeOutput *output);" in player
    assert "IplayVideoPresentFn present;" in player
    assert "db video_mode;" in player
    assert "IplayAudioWriteFn write;" in player
    assert "static db *player_text_video_cells(void)" in player
    assert "return player_video_memory();" in player
    assert "static dw player_text_video_capacity(void)" in player
    assert "return PLAYER_VIDEO_SIZE;" in player
    assert "static db player_text_current_video_mode = IPLAY_PLAYER_DEFAULT_VIDEO_MODE;" in player
    assert "static void player_set_text_video_mode_id(db video_mode)" in player
    assert "player_text_current_video_mode = video_mode;" in player
    assert "static const IplayTextMode *player_text_video_mode(void)" in player
    assert "static db player_text_video_mode_id(void)" in player
    assert "return iplay_text_mode_for_video_mode(player_text_video_mode_id());" in player
    assert "return player_text_current_video_mode;" in player
    assert "return &IPLAY_TEXT_DEFAULT_MODE;" not in player
    assert "return IPLAY_TEXT_DEFAULT_VIDEO_MODE;" not in player
    assert "static db *player_runtime_video_cells(void)" not in player
    assert "static dw player_runtime_video_capacity(void)" not in player
    assert "static const IplayTextMode *player_runtime_video_mode(void)" not in player
    assert "#define player_video_config_set_cells_field(state, value) ((state)->cells = (value))" in player
    assert "#define player_video_config_set_capacity_field(state, value) ((state)->capacity = (value))" in player
    assert "#define player_video_config_set_mode_field(state, value) ((state)->mode = (value))" in player
    assert "#define player_video_config_set_video_mode_field(state, value) ((state)->video_mode = (value))" in player
    assert "#define player_video_config_cells_field(state) ((state)->cells)" in player
    assert "#define player_video_config_capacity_field(state) ((state)->capacity)" in player
    assert "#define player_video_config_mode_field(state) ((state)->mode)" in player
    assert "#define player_video_config_video_mode_field(state) ((state)->video_mode)" in player
    assert "static void player_video_config_set_cells_field(PlayerVideoConfig *state, db *cells)" not in player
    assert "static void player_video_config_set_capacity_field(PlayerVideoConfig *state, dw capacity)" not in player
    assert "static void player_video_config_set_mode_field(PlayerVideoConfig *state, const IplayTextMode *mode)" not in player
    assert "static db *player_video_config_cells_field(const PlayerVideoConfig *state)" not in player
    assert "static dw player_video_config_capacity_field(const PlayerVideoConfig *state)" not in player
    assert "static const IplayTextMode *player_video_config_mode_field(const PlayerVideoConfig *state)" not in player
    assert "static void player_video_config_init(PlayerVideoConfig *config, db *cells, dw capacity, const IplayTextMode *mode, db video_mode)" in player
    assert "player_video_config_set_cells_field(config, cells);" in player
    assert "player_video_config_set_capacity_field(config, capacity);" in player
    assert "player_video_config_set_mode_field(config, mode);" in player
    assert "player_video_config_set_video_mode_field(config, video_mode);" in player
    assert "static void player_video_config_init_runtime(PlayerVideoConfig *config)" in player
    assert "player_video_config_init(config, player_text_video_cells(), player_text_video_capacity(), player_text_video_mode(), player_text_video_mode_id());" in player
    assert "player_video_config_init(config, player_runtime_video_cells(), player_runtime_video_capacity(), player_runtime_video_mode());" not in player
    assert "static db *player_video_config_cells(const PlayerVideoConfig *config)" in player
    assert "return player_video_config_cells_field(config);" in player
    assert "static dw player_video_config_capacity(const PlayerVideoConfig *config)" in player
    assert "return player_video_config_capacity_field(config);" in player
    assert "static const IplayTextMode *player_video_config_mode(const PlayerVideoConfig *config)" in player
    assert "return player_video_config_mode_field(config);" in player
    assert "static db player_video_config_video_mode(const PlayerVideoConfig *config)" in player
    assert "return player_video_config_video_mode_field(config);" in player
    assert (
        "config->cells = cells;\n"
        "    config->capacity = capacity;\n"
        "    config->mode = mode;"
    ) not in player
    assert "config->cells = cells;" not in player
    assert "config->capacity = capacity;" not in player
    assert "config->mode = mode;" not in player
    assert "return config->cells;" not in player
    assert "return config->capacity;" not in player
    assert "return config->mode;" not in player
    assert "#define player_runtime_output_video_config_field(state) (&(state)->video_config)" in player
    assert "#define player_runtime_output_video_backend_field(state) (&(state)->video_backend)" in player
    assert "#define player_runtime_output_audio_backend_field(state) (&(state)->audio_backend)" in player
    assert "static PlayerVideoConfig *player_runtime_output_video_config_field(PlayerRuntimeOutput *state)" not in player
    assert "static PlayerVideoBackend *player_runtime_output_video_backend_field(PlayerRuntimeOutput *state)" not in player
    assert "static PlayerAudioBackend *player_runtime_output_audio_backend_field(PlayerRuntimeOutput *state)" not in player
    assert "return &output->video_config;" not in player
    assert "return &output->video_backend;" not in player
    assert "return &output->audio_backend;" not in player
    assert "static PlayerVideoConfig *player_runtime_output_video_config(PlayerRuntimeOutput *output)" in player
    assert "return player_runtime_output_video_config_field(output);" in player
    assert "static PlayerVideoBackend *player_runtime_output_video_backend(PlayerRuntimeOutput *output)" in player
    assert "return player_runtime_output_video_backend_field(output);" in player
    assert "static PlayerAudioBackend *player_runtime_output_audio_backend(PlayerRuntimeOutput *output)" in player
    assert "return player_runtime_output_audio_backend_field(output);" in player
    assert "static void player_runtime_output_init_text_config(PlayerRuntimeOutput *output)" in player
    assert "player_video_config_init_runtime(player_runtime_output_video_config(output));" in player
    assert "static void player_runtime_output_init_text_backend(PlayerRuntimeOutput *output)" in player
    assert "player_video_backend_init_text(player_runtime_output_video_backend(output));" in player
    assert "static void player_runtime_output_init_text_video(PlayerRuntimeOutput *output)" in player
    assert "player_runtime_output_init_text_config(output);" in player
    assert "player_runtime_output_init_text_backend(output);" in player
    assert "static db *player_runtime_output_video_cells(PlayerRuntimeOutput *output)" in player
    assert "static dw player_runtime_output_video_capacity(PlayerRuntimeOutput *output)" in player
    assert "static const IplayTextMode *player_runtime_output_video_mode(PlayerRuntimeOutput *output)" in player
    assert "static db player_runtime_output_video_mode_id(PlayerRuntimeOutput *output)" in player
    assert "static IplayVideoPresentFn player_runtime_output_video_present(PlayerRuntimeOutput *output)" in player
    assert "static void *player_runtime_output_video_user(PlayerRuntimeOutput *output)" in player
    assert "static IplayAudioWriteFn player_runtime_output_audio_write(PlayerRuntimeOutput *output)" in player
    assert "static void *player_runtime_output_audio_user(PlayerRuntimeOutput *output)" in player
    assert "return player_video_config_cells(player_runtime_output_video_config(output));" in player
    assert "return player_video_config_capacity(player_runtime_output_video_config(output));" in player
    assert "return player_video_config_mode(player_runtime_output_video_config(output));" in player
    assert "return player_video_config_video_mode(player_runtime_output_video_config(output));" in player
    assert "return player_video_backend_present(player_runtime_output_video_backend(output));" in player
    assert "return player_video_backend_user(player_runtime_output_video_backend(output));" in player
    assert "return player_audio_backend_write(player_runtime_output_audio_backend(output));" in player
    assert "return player_audio_backend_user(player_runtime_output_audio_backend(output));" in player
    assert "#define player_runtime_video_output_set_cells_field(state, value) ((state)->cells = (value))" in player
    assert "#define player_runtime_video_output_set_capacity_field(state, value) ((state)->capacity = (value))" in player
    assert "#define player_runtime_video_output_set_mode_field(state, value) ((state)->mode = (value))" in player
    assert "#define player_runtime_video_output_set_video_mode_field(state, value) ((state)->video_mode = (value))" in player
    assert "#define player_runtime_video_output_set_present_field(state, value) ((state)->present = (value))" in player
    assert "#define player_runtime_video_output_set_user_field(state, value) ((state)->user = (value))" in player
    assert "#define player_runtime_audio_output_set_write_field(state, value) ((state)->write = (value))" in player
    assert "#define player_runtime_audio_output_set_user_field(state, value) ((state)->user = (value))" in player
    assert "#define player_runtime_video_output_cells_field(state) ((state)->cells)" in player
    assert "#define player_runtime_video_output_capacity_field(state) ((state)->capacity)" in player
    assert "#define player_runtime_video_output_mode_field(state) ((state)->mode)" in player
    assert "#define player_runtime_video_output_video_mode_field(state) ((state)->video_mode)" in player
    assert "#define player_runtime_video_output_present_field(state) ((state)->present)" in player
    assert "#define player_runtime_video_output_user_field(state) ((state)->user)" in player
    assert "#define player_runtime_audio_output_write_field(state) ((state)->write)" in player
    assert "#define player_runtime_audio_output_user_field(state) ((state)->user)" in player
    assert "static void player_runtime_video_output_set_cells_field(PlayerRuntimeVideoOutput *video, db *cells)" not in player
    assert "static void player_runtime_video_output_set_capacity_field(PlayerRuntimeVideoOutput *video, dw capacity)" not in player
    assert "static void player_runtime_video_output_set_mode_field(PlayerRuntimeVideoOutput *video, const IplayTextMode *mode)" not in player
    assert "static void player_runtime_video_output_set_present_field(PlayerRuntimeVideoOutput *video, IplayVideoPresentFn present)" not in player
    assert "static void player_runtime_video_output_set_user_field(PlayerRuntimeVideoOutput *video, void *user)" not in player
    assert "static void player_runtime_audio_output_set_write_field(PlayerRuntimeAudioOutput *audio, IplayAudioWriteFn write)" not in player
    assert "static void player_runtime_audio_output_set_user_field(PlayerRuntimeAudioOutput *audio, void *user)" not in player
    assert "static db *player_runtime_video_output_cells_field(const PlayerRuntimeVideoOutput *video)" not in player
    assert "static dw player_runtime_video_output_capacity_field(const PlayerRuntimeVideoOutput *video)" not in player
    assert "static const IplayTextMode *player_runtime_video_output_mode_field(const PlayerRuntimeVideoOutput *video)" not in player
    assert "static IplayVideoPresentFn player_runtime_video_output_present_field(const PlayerRuntimeVideoOutput *video)" not in player
    assert "static void *player_runtime_video_output_user_field(const PlayerRuntimeVideoOutput *video)" not in player
    assert "static IplayAudioWriteFn player_runtime_audio_output_write_field(const PlayerRuntimeAudioOutput *audio)" not in player
    assert "static void *player_runtime_audio_output_user_field(const PlayerRuntimeAudioOutput *audio)" not in player
    assert "static void player_runtime_video_output_set_cells(PlayerRuntimeVideoOutput *video, db *cells)" in player
    assert "static void player_runtime_video_output_set_capacity(PlayerRuntimeVideoOutput *video, dw capacity)" in player
    assert "static void player_runtime_video_output_set_mode(PlayerRuntimeVideoOutput *video, const IplayTextMode *mode)" in player
    assert "static void player_runtime_video_output_set_video_mode(PlayerRuntimeVideoOutput *video, db video_mode)" in player
    assert "static void player_runtime_video_output_set_present(PlayerRuntimeVideoOutput *video, IplayVideoPresentFn present)" in player
    assert "static void player_runtime_video_output_set_user(PlayerRuntimeVideoOutput *video, void *user)" in player
    assert "static void player_runtime_audio_output_set_write(PlayerRuntimeAudioOutput *audio, IplayAudioWriteFn write)" in player
    assert "static void player_runtime_audio_output_set_user(PlayerRuntimeAudioOutput *audio, void *user)" in player
    assert "static void player_runtime_video_output_prepare_presenter(PlayerRuntimeVideoOutput *video)" in player
    assert "dos_text_presenter_init_vga_text_mode((DosTextPresenter *)dos_text_present_user(player_runtime_video_output_user_field(video)), player_runtime_video_output_video_mode_field(video));" in player
    assert "static void player_runtime_video_output_init(PlayerRuntimeVideoOutput *video, PlayerRuntimeOutput *output)" in player
    assert "player_runtime_video_output_set_cells(video, player_runtime_output_video_cells(output));" in player
    assert "player_runtime_video_output_set_capacity(video, player_runtime_output_video_capacity(output));" in player
    assert "player_runtime_video_output_set_mode(video, player_runtime_output_video_mode(output));" in player
    assert "player_runtime_video_output_set_video_mode(video, player_runtime_output_video_mode_id(output));" in player
    assert "player_runtime_video_output_set_present(video, player_runtime_output_video_present(output));" in player
    assert "player_runtime_video_output_set_user(video, player_runtime_output_video_user(output));" in player
    assert "player_runtime_video_output_prepare_presenter(video);" in player
    assert "static void player_runtime_audio_output_init(PlayerRuntimeAudioOutput *audio, PlayerRuntimeOutput *output)" in player
    assert "player_runtime_audio_output_set_write(audio, player_runtime_output_audio_write(output));" in player
    assert "player_runtime_audio_output_set_user(audio, player_runtime_output_audio_user(output));" in player
    assert "static db *player_runtime_video_output_cells(const PlayerRuntimeVideoOutput *video)" in player
    assert "static dw player_runtime_video_output_capacity(const PlayerRuntimeVideoOutput *video)" in player
    assert "static const IplayTextMode *player_runtime_video_output_mode(const PlayerRuntimeVideoOutput *video)" in player
    assert "static db player_runtime_video_output_video_mode(const PlayerRuntimeVideoOutput *video)" in player
    assert "static IplayVideoPresentFn player_runtime_video_output_present(const PlayerRuntimeVideoOutput *video)" in player
    assert "static void *player_runtime_video_output_user(const PlayerRuntimeVideoOutput *video)" in player
    assert "static IplayAudioWriteFn player_runtime_audio_output_write(const PlayerRuntimeAudioOutput *audio)" in player
    assert "static void *player_runtime_audio_output_user(const PlayerRuntimeAudioOutput *audio)" in player
    assert "return player_runtime_video_output_cells_field(video);" in player
    assert "return player_runtime_video_output_capacity_field(video);" in player
    assert "return player_runtime_video_output_mode_field(video);" in player
    assert "return player_runtime_video_output_video_mode_field(video);" in player
    assert "return player_runtime_video_output_present_field(video);" in player
    assert "return player_runtime_video_output_user_field(video);" in player
    assert "return player_runtime_audio_output_write_field(audio);" in player
    assert "return player_runtime_audio_output_user_field(audio);" in player
    assert "video->cells = player_runtime_output_video_cells(output);" not in player
    assert "video->capacity = player_runtime_output_video_capacity(output);" not in player
    assert "video->mode = player_runtime_output_video_mode(output);" not in player
    assert "video->present = player_runtime_output_video_present(output);" not in player
    assert "video->user = player_runtime_output_video_user(output);" not in player
    assert "audio->write = player_runtime_output_audio_write(output);" not in player
    assert "audio->user = player_runtime_output_audio_user(output);" not in player
    assert "#define player_runtime_output_views_video_field(state) (&(state)->video)" in player
    assert "#define player_runtime_output_views_audio_field(state) (&(state)->audio)" in player
    assert "#define player_runtime_output_views_video_const_field(state) (&(state)->video)" in player
    assert "#define player_runtime_output_views_audio_const_field(state) (&(state)->audio)" in player
    assert "static PlayerRuntimeVideoOutput *player_runtime_output_views_video_field(PlayerRuntimeOutputViews *views)" not in player
    assert "static PlayerRuntimeAudioOutput *player_runtime_output_views_audio_field(PlayerRuntimeOutputViews *views)" not in player
    assert "static const PlayerRuntimeVideoOutput *player_runtime_output_views_video_const_field(const PlayerRuntimeOutputViews *views)" not in player
    assert "static const PlayerRuntimeAudioOutput *player_runtime_output_views_audio_const_field(const PlayerRuntimeOutputViews *views)" not in player
    assert "static PlayerRuntimeVideoOutput *player_runtime_output_views_video(PlayerRuntimeOutputViews *views)" in player
    assert "return player_runtime_output_views_video_field(views);" in player
    assert "static PlayerRuntimeAudioOutput *player_runtime_output_views_audio(PlayerRuntimeOutputViews *views)" in player
    assert "return player_runtime_output_views_audio_field(views);" in player
    assert "static const PlayerRuntimeVideoOutput *player_runtime_output_views_video_const(const PlayerRuntimeOutputViews *views)" in player
    assert "return player_runtime_output_views_video_const_field(views);" in player
    assert "static const PlayerRuntimeAudioOutput *player_runtime_output_views_audio_const(const PlayerRuntimeOutputViews *views)" in player
    assert "return player_runtime_output_views_audio_const_field(views);" in player
    assert "static void player_runtime_output_views_init(PlayerRuntimeOutputViews *views, PlayerRuntimeOutput *output)" in player
    assert "player_runtime_video_output_init(player_runtime_output_views_video(views), output);" in player
    assert "player_runtime_audio_output_init(player_runtime_output_views_audio(views), output);" in player
    assert "static void player_runtime_output_init_with_audio(PlayerRuntimeOutput *output, PlayerRuntimeOutputAudioInitFn init_audio)" in player
    assert "player_runtime_output_init_text_video(output);" in player
    assert "init_audio(output);" in player
    assert "static void player_configure_runtime_output_with(IplayRuntimeConfig *runtime_config, PlayerRuntimeOutputInitFn init_output, PlayerRuntimeOutputApplyFn apply_output)" in player
    assert "init_output(&output);" in player
    assert "apply_output(runtime_config, &output);" in player
    assert "static void player_runtime_output_init_sb16(PlayerRuntimeOutput *output)" in player
    assert "static void player_runtime_output_init_sdl(PlayerRuntimeOutput *output)" in player
    assert "static void player_runtime_output_init_sb16_audio(PlayerRuntimeOutput *output)" in player
    assert "static void player_runtime_output_init_sdl_audio(PlayerRuntimeOutput *output)" in player
    assert "player_audio_backend_init_sb16(player_runtime_output_audio_backend(output));" in player
    assert "player_audio_backend_init_discard(player_runtime_output_audio_backend(output));" in player
    assert "player_runtime_output_init_with_audio(output, player_runtime_output_init_sb16_audio);" in player
    assert "player_runtime_output_init_with_audio(output, player_runtime_output_init_sdl_audio);" in player
    assert "player_runtime_output_init_sb16_audio(output);" not in player
    assert "player_runtime_output_init_sdl_audio(output);" not in player
    assert (
        "static void player_runtime_output_init_sb16(PlayerRuntimeOutput *output) {\n"
        "    player_runtime_output_init_text_video(output);\n"
        "    player_audio_backend_init_sb16(player_runtime_output_audio_backend(output));"
    ) not in player
    assert (
        "static void player_runtime_output_init_sdl(PlayerRuntimeOutput *output) {\n"
        "    player_runtime_output_init_text_video(output);\n"
        "    player_audio_backend_init_discard(player_runtime_output_audio_backend(output));"
    ) not in player
    assert "static void player_apply_runtime_sb16_output_config(IplayRuntimeConfig *runtime_config, PlayerRuntimeOutput *output)" in player
    assert "static void player_apply_runtime_sdl_output_config(IplayRuntimeConfig *runtime_config, PlayerRuntimeOutput *output)" in player
    assert "PlayerRuntimeOutputViews views;" in player
    assert "player_runtime_output_views_init(&views, output);" in player
    assert (
        "static void player_apply_runtime_sb16_output_config(IplayRuntimeConfig *runtime_config, PlayerRuntimeOutput *output) {\n"
        "PlayerRuntimeVideoOutput video;\n"
        "    PlayerRuntimeAudioOutput audio;"
    ) not in player
    assert (
        "static void player_apply_runtime_sdl_output_config(IplayRuntimeConfig *runtime_config, PlayerRuntimeOutput *output) {\n"
        "PlayerRuntimeVideoOutput video;\n"
        "    PlayerRuntimeAudioOutput audio;"
    ) not in player
    assert "player_runtime_video_output_init(&video, output);" not in player
    assert "player_runtime_audio_output_init(&audio, output);" not in player
    assert "PlayerVideoConfig *video_config = player_runtime_output_video_config(output);" not in player
    assert "PlayerVideoBackend *video_backend = player_runtime_output_video_backend(output);" not in player
    assert "PlayerAudioBackend *audio_backend = player_runtime_output_audio_backend(output);" not in player
    assert "player_configure_runtime_output_with(runtime_config, player_runtime_output_init_sb16, player_apply_runtime_sb16_output_config);" in player
    assert "player_configure_runtime_output_with(runtime_config, player_runtime_output_init_sdl, player_apply_runtime_sdl_output_config);" in player
    assert "typedef struct PlayerRuntimeOutputBackend" not in player
    assert "PlayerRuntimeOutputInitFn init;" not in player
    assert "PlayerRuntimeOutputApplyFn apply;" not in player
    assert "player_configure_runtime_output_with(runtime_config, &backend);" not in player
    assert "player_runtime_output_init_sb16(&output);" not in player
    assert "player_runtime_output_init_sdl(&output);" not in player
    assert "PlayerAudioBackend *audio_backend = player_runtime_output_audio_backend(&output);" not in player
    assert "player_runtime_output_init_text_video(&output);" not in player
    assert "player_runtime_output_init_text(&output);" not in player
    assert "player_audio_backend_init_sb16(audio_backend);" not in player
    assert "player_audio_backend_init_discard(audio_backend);" not in player
    assert "static void player_apply_runtime_sb16_output_views(IplayRuntimeConfig *runtime_config, const PlayerRuntimeVideoOutput *video, const PlayerRuntimeAudioOutput *audio)" in player
    assert "static void player_apply_runtime_sdl_output_views(IplayRuntimeConfig *runtime_config, const PlayerRuntimeVideoOutput *video, const PlayerRuntimeAudioOutput *audio)" in player
    forbidden_runtime_config_mutations = [
        "runtime_config->cells",
        "runtime_config->cell_capacity_bytes",
        "runtime_config->mode",
        "runtime_config->present",
        "runtime_config->present_user",
        "runtime_config->video_backend",
        "runtime_config->audio_write",
        "runtime_config->audio_user",
        "runtime_config->audio_backend",
        "runtime_config->audio_hardware_enabled",
    ]
    assert not [field for field in forbidden_runtime_config_mutations if field in player]
    assert "static void player_apply_runtime_sb16_output_view_bundle(IplayRuntimeConfig *runtime_config, const PlayerRuntimeOutputViews *views)" in player
    assert "static void player_apply_runtime_sdl_output_view_bundle(IplayRuntimeConfig *runtime_config, const PlayerRuntimeOutputViews *views)" in player
    assert "player_apply_runtime_sb16_output_views(runtime_config, player_runtime_output_views_video_const(views), player_runtime_output_views_audio_const(views));" in player
    assert "player_apply_runtime_sdl_output_views(runtime_config, player_runtime_output_views_video_const(views), player_runtime_output_views_audio_const(views));" in player
    assert "player_apply_runtime_sb16_output_view_bundle(runtime_config, &views);" in player
    assert "player_apply_runtime_sdl_output_view_bundle(runtime_config, &views);" in player
    assert "player_apply_runtime_sb16_output_views(runtime_config, player_runtime_output_views_video_const(&views), player_runtime_output_views_audio_const(&views));" not in player
    assert "player_apply_runtime_sdl_output_views(runtime_config, player_runtime_output_views_video_const(&views), player_runtime_output_views_audio_const(&views));" not in player
    assert "player_apply_runtime_sb16_output_views(runtime_config, player_runtime_output_views_video(&views), player_runtime_output_views_audio(&views));" not in player
    assert "player_apply_runtime_sdl_output_views(runtime_config, player_runtime_output_views_video(&views), player_runtime_output_views_audio(&views));" not in player
    assert "player_apply_runtime_sb16_output_views(runtime_config, &video, &audio);" not in player
    assert "player_apply_runtime_sdl_output_views(runtime_config, &video, &audio);" not in player
    assert "player_apply_runtime_sb16_output_config(runtime_config, &output);" not in player
    assert "player_apply_runtime_sdl_output_config(runtime_config, &output);" not in player
    assert "player_audio_backend_init(&audio_backend, sb16_audio_write, player_sb16_hardware());" not in player
    assert "player_runtime_audio_output_write(audio), player_runtime_audio_output_user(audio)" in player
    assert "player_configure_runtime_sb16_output(runtime_config);" in player
    assert "player_configure_runtime_sdl_output(runtime_config);" in player
    assert "iplay_runtime_config_sb16_hardware_capacity(runtime_config, player_runtime_video_output_cells(video), player_runtime_video_output_capacity(video), player_runtime_video_output_mode(video), player_runtime_video_output_present(video), player_runtime_video_output_user(video)" in player
    assert "iplay_runtime_config_sdl_capacity(runtime_config, player_runtime_video_output_cells(video), player_runtime_video_output_capacity(video), player_runtime_video_output_mode(video), player_runtime_video_output_present(video), player_runtime_video_output_user(video)" in player
    assert "iplay_runtime_config_sb16_hardware_capacity(runtime_config, player_runtime_video_output_cells(&video), player_runtime_video_output_capacity(&video), player_runtime_video_output_mode(&video), player_runtime_video_output_present(&video), player_runtime_video_output_user(&video)" not in player
    assert "iplay_runtime_config_sdl_capacity(runtime_config, player_runtime_video_output_cells(&video), player_runtime_video_output_capacity(&video), player_runtime_video_output_mode(&video), player_runtime_video_output_present(&video), player_runtime_video_output_user(&video)" not in player
    assert "iplay_runtime_config_sb16_hardware_capacity(runtime_config, player_runtime_output_video_cells(output), player_runtime_output_video_capacity(output), player_runtime_output_video_mode(output), player_runtime_output_video_present(output), player_runtime_output_video_user(output)" not in player
    assert "iplay_runtime_config_sdl_capacity(runtime_config, player_runtime_output_video_cells(output), player_runtime_output_video_capacity(output), player_runtime_output_video_mode(output), player_runtime_output_video_present(output), player_runtime_output_video_user(output)" not in player
    assert "iplay_runtime_config_sb16_hardware_capacity(runtime_config, player_video_config_cells(&video_config), player_video_config_capacity(&video_config), player_video_config_mode(&video_config)" not in player
    assert "iplay_runtime_config_sdl_capacity(runtime_config, player_video_config_cells(&video_config), player_video_config_capacity(&video_config), player_video_config_mode(&video_config)" not in player
    assert "iplay_runtime_config_sb16_hardware_capacity(runtime_config, player_runtime_video_cells(), player_runtime_video_capacity(), player_runtime_video_mode()" not in player
    assert "iplay_runtime_config_sdl_capacity(runtime_config, player_runtime_video_cells(), player_runtime_video_capacity(), player_runtime_video_mode()" not in player
    assert "iplay_runtime_config_sb16_hardware_capacity(runtime_config, player_video_memory(), PLAYER_VIDEO_SIZE, &IPLAY_TEXT_DEFAULT_MODE" not in player
    assert "iplay_runtime_config_sdl_capacity(runtime_config, player_video_memory(), PLAYER_VIDEO_SIZE, &IPLAY_TEXT_DEFAULT_MODE" not in player
    assert "iplay_runtime_config_sb16_hardware_capacity(runtime_config, player_video_memory(), PLAYER_VIDEO_SIZE, &IPLAY_TEXT_DEFAULT_MODE, dos_text_present, dos_text_default_present_user()" not in player
    assert "iplay_runtime_config_sdl_capacity(runtime_config, player_video_memory(), PLAYER_VIDEO_SIZE, &IPLAY_TEXT_DEFAULT_MODE, dos_text_present, dos_text_default_present_user()" not in player
    assert "dos_text_present, NULL" not in player
    assert (
        "iplay_runtime_config_sdl_capacity(runtime_config, player_video_memory(), PLAYER_VIDEO_SIZE, &IPLAY_TEXT_DEFAULT_MODE, dos_text_present, dos_text_default_present_user(),\n"
        "                                      player_audio_discard, NULL);"
    ) not in player
    assert (
        "iplay_runtime_config_sb16_hardware_capacity(runtime_config, player_video_memory(), PLAYER_VIDEO_SIZE, &IPLAY_TEXT_DEFAULT_MODE, dos_text_present, dos_text_default_present_user(),\n"
        "                                                sb16_audio_write, player_sb16_hardware());"
    ) not in player
    assert (
        "static void player_configure_runtime_output(IplayRuntimeConfig *runtime_config) {\n"
        "#if IPLAY_PLAYER_ENABLE_SB16_HW\n"
        "    PlayerAudioBackend audio_backend;"
    ) not in player
    assert (
        "static void player_configure_runtime_output(IplayRuntimeConfig *runtime_config) {\n"
        "#if IPLAY_PLAYER_ENABLE_SB16_HW\n"
        "    player_configure_runtime_sb16_output(runtime_config);\n"
        "#else\n"
        "    PlayerAudioBackend audio_backend;"
    ) not in player
    assert "iplay_runtime_init_callbacks(&runtime, video_mem" not in player
    assert "iplay_runtime_init_vga_sdl_audio(&runtime, video_mem" not in player
    assert "iplay_runtime_init_vga_sb16(&runtime, video_mem" not in player
    assert "iplay_runtime_init_vga_sb16(&runtime, mem" not in player
    assert "static void player_render_runtime_status(IplayRuntime *runtime, const PlayerModuleInfo *module)" in player
    assert "player_render_runtime_status(runtime, module);" in player
    assert "iplay_runtime_render_static(runtime, IPLAY_PLAYER_DEFAULT_ERASE_ATTR)" in player
    assert "iplay_runtime_render_static(&runtime, IPLAY_PLAYER_DEFAULT_ERASE_ATTR)" not in player
    assert "iplay_runtime_render_static(&runtime, 0x07)" not in player
    assert "iplay_runtime_render_bottom(runtime" in player
    assert "IPLAY_PLAYER_DEFAULT_PATTERN" in player
    assert "IPLAY_PLAYER_DEFAULT_ORDER" in player
    assert "IPLAY_PLAYER_DEFAULT_ROW" in player
    assert "IPLAY_PLAYER_DEFAULT_SPEED" in player
    assert "IPLAY_PLAYER_DEFAULT_FLAGS" in player
    assert "IPLAY_PLAYER_DEFAULT_VOLUME" in player
    assert "IPLAY_PLAYER_DEFAULT_AMPLIFICATION" in player
    assert "iplay_runtime_render_bottom(&runtime, 1, 1, 0, 0" not in player
    assert "0, 0x80, 100)" not in player
    assert "iplay_runtime_audio_start(&runtime" not in player
    assert "iplay_runtime_audio_stop(&runtime" not in player
    assert "static int player_runtime_playback_ready(const IplayRuntime *runtime)" in player
    assert "return iplay_runtime_audio_active(runtime) && iplay_runtime_audio_is_sb16_compatible(runtime);" in player
    assert "typedef struct PlayerPlaybackBlock" in player
    assert "#define IPLAY_PLAYER_SB16_BLOCK_FRAMES 512u" in player
    assert "#define IPLAY_PLAYER_CONTINUOUS_BLOCK_FRAMES 1024u" in player
    assert "#define IPLAY_PLAYER_MAX_BLOCK_FRAMES IPLAY_PLAYER_CONTINUOUS_BLOCK_FRAMES" in player
    assert "db pcm[IPLAY_PLAYER_MAX_BLOCK_FRAMES * 4u];" in player
    assert "typedef struct PlayerPlayback" in player
    assert "db limit_reached;" in player
    assert "db source_ended;" in player
    assert "typedef struct PlayerDecoderContext" in player
    assert "typedef struct PlayerPcmSource PlayerPcmSource;" in player
    assert "typedef dw (*PlayerPcmSourceReadFn)(PlayerPcmSource *source, PlayerPlaybackBlock *block);" in player
    assert "dw frames_per_block;" in player
    assert "static void player_playback_init(PlayerPlayback *playback)" in player
    assert "static void player_pcm_source_set_frames_per_block(PlayerPcmSource *source, dw frames_per_block)" in player
    assert "static dw player_pcm_source_frames_per_block(const PlayerPcmSource *source)" in player
    assert "static void player_pcm_source_init(PlayerPcmSource *source, PlayerPcmSourceReadFn read, PlayerPcmSourceEndedFn ended, void *user)" in player
    assert "static dw player_pcm_source_read(PlayerPcmSource *source, PlayerPlaybackBlock *block)" in player
    assert "static dw player_prime_pcm_source_read(PlayerPcmSource *source, PlayerPlaybackBlock *block)" in player
    assert "static dw player_module_pcm_source_read(PlayerPcmSource *source, PlayerPlaybackBlock *block)" in player
    assert "return 0;\n        return 0;" not in player
    assert "static void player_module_pcm_source_init(PlayerPcmSource *source, PlayerDecoderContext *context)" in player
    assert "static dw player_playback_block_capacity_frames(void)" in player
    assert "static dw player_playback_block_active_bytes(const PlayerPlaybackBlock *block)" in player
    assert "static void player_playback_prepare_block_frames(PlayerPlaybackBlock *block, dw frames)" in player
    assert "static void player_playback_prepare_sb16_block(PlayerPlaybackBlock *block)" in player
    assert "static void player_playback_fill_seed_pcm(PlayerPlaybackBlock *block, db seed)" in player
    assert "static db player_voice_state_pcm_seed(const PlayerVoiceState *voice, db fallback)" in player
    assert "static dd player_voice_state_step_for_period(dw period)" in player
    assert "static db player_voice_state_next_sample_byte(PlayerVoiceState *voice, const PlayerModuleInfo *module)" in player
    assert "static int player_mix_clamp_s16(long sample)" in player
    assert "static int player_sample_byte_to_s8(db sample_byte)" in player
    assert "static void player_mix_put_s16le(db *pcm, unsigned offset, int sample)" in player
    assert "static void player_playback_fill_voice_pcm(PlayerPlaybackBlock *block, PlayerVoiceState *voice, const PlayerModuleInfo *module, db fallback)" in player
    assert "static void player_playback_mix_voices_pcm(PlayerPlaybackBlock *block, PlayerDecoderContext *context, db fallback)" in player
    assert "#define IPLAY_PLAYER_FILE_STREAM_BUFFER_BYTES 496u" in player
    assert "dd file_stream_base;" in player
    assert "int file_stream_fd;" in player
    assert "db file_stream_open;" in player
    assert "dw file_stream_len;" in player
    assert "dw file_stream_index;" in player
    assert "static db *player_module_file_stream_buffer(void)" in player
    assert "context->file_stream_fd = -1;" in player
    assert "context->file_stream_open = 0;" in player
    assert "static void player_decoder_context_close_file_stream(PlayerDecoderContext *context)" in player
    assert "static int player_decoder_context_open_file_stream(PlayerDecoderContext *context)" in player
    assert "fd = player_open_read_binary(player_module_path(context->module));" in player
    assert "static int player_decoder_context_refill_file_stream(PlayerDecoderContext *context)" in player
    assert "static db player_decoder_context_next_file_stream_byte(PlayerDecoderContext *context, db fallback)" in player
    assert "return player_decoder_context_next_file_stream_byte(context, fallback);" in player
    assert "static dd player_playback_block_checksum(const PlayerPlaybackBlock *block)" in player
    assert "typedef struct PlayerPatternCell" in player
    assert "dw period;" in player
    assert "db note;" in player
    assert "db octave;" in player
    assert "db instrument;" in player
    assert "db volume;" in player
    assert "db volume_set;" in player
    assert "db effect;" in player
    assert "db param;" in player
    assert "typedef struct PlayerSampleInfo" in player
    assert "dd length;" in player
    assert "dd loop_start;" in player
    assert "dd loop_length;" in player
    assert "dd data_offset;" in player
    assert "#define IPLAY_PLAYER_SAMPLE_DATA_OFFSET_16BIT 0x80000000ul" in player
    assert "typedef struct PlayerVoiceState" in player
    assert "db active;" in player
    assert "dd sample_position;" in player
    assert "dd sample_phase;" in player
    assert "dd sample_step;" in player
    assert "static db player_module_header_seed(const PlayerModuleInfo *module)" in player
    assert "static db player_module_pcm_seed(const PlayerModuleInfo *module)" in player
    assert "static dw player_module_order_count(const PlayerModuleInfo *module)" in player
    assert "static dw player_module_mod_order_value(db raw_order)" in player
    assert "static dw player_module_order_value(const PlayerModuleInfo *module, dw order)" in player
    assert "static dw player_module_pattern_count(const PlayerModuleInfo *module)" in player
    assert "static dw player_module_s3m_channel_count(const db *header, size_t len)" in player
    assert "static dw player_module_s3m_physical_channel(const PlayerModuleInfo *module, dw logical_channel)" in player
    assert "if (count == logical_channel) return i;" in player
    assert "physical_channel = player_module_s3m_physical_channel(module, channel);" in player
    assert "packed_channel == (db)physical_channel" in player
    assert "static dw player_module_channel_count(const PlayerModuleInfo *module)" in player
    assert "static void player_pattern_cell_clear(PlayerPatternCell *cell)" in player
    assert "static void player_sample_info_clear(PlayerSampleInfo *sample)" in player
    assert "static void player_voice_state_clear(PlayerVoiceState *voice)" in player
    assert "static int player_voice_state_playable(const PlayerVoiceState *voice)" in player
    assert "static void player_module_mod_sample_info(const PlayerModuleInfo *module, db instrument, PlayerSampleInfo *sample)" in player
    assert "static void player_module_sample_info(const PlayerModuleInfo *module, db instrument, PlayerSampleInfo *sample)" in player
    assert "static dd player_module_mod_pattern_stream_offset(const PlayerModuleInfo *module)" in player
    assert "(void)module;\n    return 1084ul;" in player
    assert "static dd player_module_mtm_stream_offset(const PlayerModuleInfo *module)" in player
    assert "return 0x42ul;" in player
    assert "static dd player_module_stm_stream_offset(const PlayerModuleInfo *module)" in player
    assert "return 0x40ul;" in player
    assert "static dd player_module_far_stream_offset(const PlayerModuleInfo *module)" in player
    assert "return 0x80ul;" in player
    assert "static dd player_module_669_stream_offset(const PlayerModuleInfo *module)" in player
    assert "return 0x71ul;" in player
    assert "static dd player_module_psm_stream_offset(const PlayerModuleInfo *module)" in player
    assert "static dd player_module_ult_stream_offset(const PlayerModuleInfo *module)" in player
    assert "return 0x60ul;" in player
    assert "static dd player_module_s3m_pattern_stream_offset(const PlayerModuleInfo *module)" in player
    assert "static dd player_module_stream_start_offset(const PlayerModuleInfo *module)" in player
    assert "static dd player_module_diagnostic_stream_start_offset(const PlayerModuleInfo *module)" in player
    assert "return player_module_stream_start_offset(module);" in player
    assert "dd available;" in player
    assert "available = (dd)player_module_header_len(module);" in player
    assert "dd module_size;" in player
    assert "module_size = (dd)player_module_size(module);" in player
    assert "if (offset != 0 && module_size != 0 && offset < module_size) return offset;" in player
    assert "if (available == 0 || offset >= available) return 0;" in player
    assert "if (available == 0 || offset >= available) return 0;" in player
    assert "if (offset >= player_module_size(module)) return 0;" not in player
    assert "context->pcm_stream_offset = player_module_stream_start_offset(module);" in player
    assert "case IPLAY_LOADER_KIND_MTM:" in player
    assert "offset = player_module_mtm_stream_offset(module);" in player
    assert "case IPLAY_LOADER_KIND_STM:" in player
    assert "offset = player_module_stm_stream_offset(module);" in player
    assert "case IPLAY_LOADER_KIND_FAR:" in player
    assert "offset = player_module_far_stream_offset(module);" in player
    assert "case IPLAY_LOADER_KIND_669:" in player
    assert "offset = player_module_669_stream_offset(module);" in player
    assert "case IPLAY_LOADER_KIND_PSM:" in player
    assert "offset = player_module_psm_stream_offset(module);" in player
    assert "case IPLAY_LOADER_KIND_ULT:" in player
    assert "offset = player_module_ult_stream_offset(module);" in player
    assert "context->pcm_stream_offset = 0;" not in player
    assert "if (context->pcm_stream_offset >= module_size) context->pcm_stream_offset = player_module_stream_start_offset(context->module);" in player
    assert "if (context->pcm_stream_offset >= (dd)player_module_header_len(context->module)) context->pcm_stream_offset = player_module_stream_start_offset(context->module);" in player
    assert "static db player_module_sample_byte(const PlayerModuleInfo *module, const PlayerSampleInfo *sample, dd index)" in player
    assert "static int player_module_s3m_samples_unsigned(const PlayerModuleInfo *module)" in player
    assert "return player_module_u16_le_at(module, 0x2au) == 2u;" in player
    assert "value = player_module_byte_at(module, offset);" in player
    assert "if (loader_kind(player_module_loader(module)) == IPLAY_LOADER_KIND_S3M && player_module_s3m_samples_unsigned(module)) value ^= 0x80u;" in player
    assert "#define IPLAY_PLAYER_S3M_DEFAULT_C2SPD 8363ul" in player
    assert "static dd player_module_s3m_sample_c2spd(const PlayerModuleInfo *module, db instrument)" in player
    assert "c2spd = player_module_u32_le_at(module, offset + 32u);" in player
    assert "static dd player_module_sample_c2spd(const PlayerModuleInfo *module, db instrument)" in player
    assert "static int player_module_mod_sample_finetune(const PlayerModuleInfo *module, db instrument)" in player
    assert "raw = (db)(player_module_byte_at(module, offset + 24u) & 0x0fu);" in player
    assert "return raw >= 8u ? (int)raw - 16 : (int)raw;" in player
    assert "static void player_voice_state_apply_sample_c2spd(PlayerVoiceState *voice, const PlayerModuleInfo *module)" in player
    assert "if (loader_kind(loader) == IPLAY_LOADER_KIND_MOD)" in player
    assert "int finetune = player_module_mod_sample_finetune(module, voice->instrument);" in player
    assert "long tuned = (long)voice->sample_step * (128L + (long)finetune);" in player
    assert "static void player_voice_state_apply_cell(PlayerVoiceState *voice, const PlayerPatternCell *cell, const PlayerModuleInfo *module)" in player
    assert "static int player_sample_info_loop_enabled(const PlayerSampleInfo *sample)" in player
    assert "static dd player_sample_info_loop_end(const PlayerSampleInfo *sample)" in player
    assert "static dd player_sample_info_loop_start(const PlayerSampleInfo *sample)" in player
    assert "static void player_mod_period_to_note(dw period, db *note, db *octave)" in player
    assert "static void player_module_mod_pattern_cell(const PlayerModuleInfo *module, dw pattern, dw row, dw channel, PlayerPatternCell *cell)" in player
    assert "static void player_module_pattern_cell(const PlayerModuleInfo *module, dw pattern, dw row, dw channel, PlayerPatternCell *cell)" in player
    assert "else if (volume >= 128u && volume <= 192u)" in player
    assert "cell->volume = volume == 192u ? 255u : (db)((volume - 128u) * 4u);" in player
    assert "cell->volume_set = 2;" in player
    assert "static dw player_module_rows_per_order(const PlayerModuleInfo *module)" in player
    assert "static dw player_module_restart_order(const PlayerModuleInfo *module)" in player
    assert "static dw player_module_initial_speed(const PlayerModuleInfo *module)" in player
    assert "static dw player_module_initial_tempo(const PlayerModuleInfo *module)" in player
    assert "static dw player_module_initial_global_volume(const PlayerModuleInfo *module)" in player
    assert "if (len > 0x30u)" in player
    assert "volume = header[0x30u];" in player
    assert "master_volume = (db)(header[0x33u] & 0x7fu);" in player
    assert "if (master_volume > 64u) master_volume = 64u;" in player
    assert "scaled_volume = ((dd)volume * (dd)master_volume) / 64ul;" in player
    assert "static int player_module_initial_channel_pan(const PlayerModuleInfo *module, dw channel, db *pan)" in player
    assert "static int player_module_s3m_stereo_enabled(const PlayerModuleInfo *module)" in player
    assert "return len > 0x33u && (header[0x33u] & 0x80u) != 0;" in player
    assert "if (!player_module_s3m_stereo_enabled(module)) return 0;" in player
    assert "if (physical_channel < 32u && len > 0x40u + physical_channel)" in player
    assert "setting = header[0x40u + physical_channel];" in player
    assert "*pan = setting < 8u ? 48u : 208u;" in player
    assert "if (len > 0x35u && header[0x35u] == 0xfcu)" in player
    assert "pan_table_offset = player_module_s3m_table_offset(module) + (dd)instrument_count * 2ul + (dd)pattern_count * 2ul;" in player
    assert "pan_entry = player_module_byte_at(module, pan_table_offset + physical_channel);" in player
    assert "if (pan_entry & 0x20u) *pan = (db)((pan_entry & 0x0fu) * 17u);" in player
    assert "static void player_decoder_context_init(PlayerDecoderContext *context, const PlayerModuleInfo *module)" in player
    assert "static const LoaderInfo *player_decoder_context_loader(const PlayerDecoderContext *context)" in player
    assert "static db player_decoder_context_seed(const PlayerDecoderContext *context)" in player
    assert "static int player_decoder_context_has_block(const PlayerDecoderContext *context)" in player
    assert "static void player_decoder_context_mark_ended(PlayerDecoderContext *context)" in player
    assert "static db player_decoder_context_block_seed(PlayerDecoderContext *context)" in player
    assert "static db player_decoder_context_next_module_stream_byte(PlayerDecoderContext *context, db fallback)" in player
    assert "static void player_decoder_context_advance(PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_estimated_max_blocks(dw orders, dw rows, dw speed, dw channels)" in player
    assert "static dw player_decoder_context_block_index(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_max_blocks(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_order_count(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_rows_per_order(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_restart_order(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_initial_speed(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_initial_tempo(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_current_speed(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_current_tempo(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_current_tick(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_channel_count(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_current_order_value(const PlayerDecoderContext *context)" in player
    assert "static const PlayerPatternCell *player_decoder_context_current_cell(const PlayerDecoderContext *context)" in player
    assert "static const PlayerVoiceState *player_decoder_context_current_voice(const PlayerDecoderContext *context)" in player
    assert "static PlayerVoiceState *player_decoder_context_voice(PlayerDecoderContext *context, dw channel)" in player
    assert "static const PlayerVoiceState *player_decoder_context_voice_const(const PlayerDecoderContext *context, dw channel)" in player
    assert "static void player_decoder_context_refresh_current_cell(PlayerDecoderContext *context)" in player
    assert "static void player_decoder_context_load_row_events(PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_order(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_row(const PlayerDecoderContext *context)" in player
    assert "static dw player_decoder_context_channel(const PlayerDecoderContext *context)" in player
    assert "static db player_decoder_context_ended(const PlayerDecoderContext *context)" in player
    assert "static db player_decoder_context_loop_enabled(const PlayerDecoderContext *context)" in player
    assert "static int player_decoder_context_has_active_voice(PlayerDecoderContext *context, dw channel_limit)" in player
    assert "static void player_playback_fill_stream_pcm(PlayerPlaybackBlock *block, PlayerDecoderContext *context, db fallback)" in player
    assert "db pattern_break_pending;" in player
    assert "db position_jump_pending;" in player
    assert "db pattern_loop_active;" in player
    assert "db pattern_loop_remaining;" in player
    assert "db pattern_loop_completed;" in player
    assert "db pattern_loop_jump_pending;" in player
    assert "dw pattern_break_row;" in player
    assert "dw position_jump_order;" in player
    assert "dw pattern_loop_row;" in player
    assert "context->ended = 0;" in player
    assert "context->loop_enabled = 0;" in player
    assert "context->pattern_break_pending = 0;" in player
    assert "context->position_jump_pending = 0;" in player
    assert "context->pattern_loop_active = 0;" in player
    assert "context->pattern_loop_remaining = 0;" in player
    assert "context->pattern_loop_completed = 0;" in player
    assert "context->pattern_loop_jump_pending = 0;" in player
    assert "#define IPLAY_PLAYER_PUMP_BLOCK_LIMIT 16u" in player
    assert "blocks = (dd)orders * (dd)rows * (dd)speed * (dd)channels;" in player
    assert "return blocks > 0xfffful ? 0xffffu : (dw)blocks;" in player
    assert "context->max_blocks = player_decoder_context_estimated_max_blocks(context->order_count, context->rows_per_order, context->current_speed, context->channel_count);" in player
    assert "context->max_blocks = IPLAY_PLAYER_PRIME_BLOCKS;" not in player
    assert "context->order_count = player_module_order_count(module);" in player
    assert "context->rows_per_order = player_module_rows_per_order(module);" in player
    assert "context->restart_order = player_module_restart_order(module);" in player
    assert "context->initial_speed = player_module_initial_speed(module);" in player
    assert "context->initial_tempo = player_module_initial_tempo(module);" in player
    assert "context->current_speed = context->initial_speed;" in player
    assert "context->current_tempo = context->initial_tempo;" in player
    assert "db global_volume;" in player
    assert "context->global_volume = player_module_initial_global_volume(module);" in player
    assert "context->current_tick = 0;" in player
    assert "context->pcm_stream_offset = player_module_stream_start_offset(module);" in player
    assert "context->pattern_break_row = 0;" in player
    assert "context->position_jump_order = 0;" in player
    assert "context->pattern_loop_row = 0;" in player
    assert "context->channel_count = player_module_channel_count(module);" in player
    assert "context->current_order_value = 0;" in player
    assert "context->order = 0;" in player
    assert "context->row = 0;" in player
    assert "context->channel = 0;" in player
    assert "PlayerVoiceState voices[IPLAY_PLAYER_MAX_CHANNELS];" in player
    assert "if (player_module_initial_channel_pan(module, i, &pan))" in player
    assert "context->voices[i].pan_set = 1;" in player
    assert "context->voices[i].pan = pan;" in player
    assert "if (player_module_order_value_is_skip(value))" in player
    assert "if (player_module_order_value_is_end(value))" in player
    assert "if (!context->ended) player_decoder_context_load_row_events(context);" in player
    assert "player_decoder_context_refresh_current_cell(context);" in player
    assert "channel_limit = context->channel_count;" in player
    assert "if (channel_limit > IPLAY_PLAYER_MAX_CHANNELS) channel_limit = IPLAY_PLAYER_MAX_CHANNELS;" in player
    assert "for (channel = 0; channel < channel_limit; ++channel)" in player
    assert "player_module_pattern_cell(context->module, context->current_order_value, context->row, channel, &cell);" in player
    assert "PlayerVoiceState *voice;" in player
    assert "if (player_pattern_cell_has_note_delay(&cell)) continue;" in player
    assert "voice = player_decoder_context_voice(context, channel);" in player
    assert "player_voice_state_apply_cell(voice, &cell, context->module);" in player
    assert "#define IPLAY_PLAYER_DEFAULT_ROWS_PER_ORDER 64u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_INITIAL_SPEED 6u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_INITIAL_TEMPO 125u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_CHANNELS 4u" in player
    assert "#define IPLAY_PLAYER_MAX_CHANNELS 32u" in player
    assert "header = player_module_header(module);" in player
    assert "len = player_module_header_len(module);" in player
    assert "static int player_module_has_range(const PlayerModuleInfo *module, dd offset, dd byte_count)" in player
    assert "static db player_module_byte_at(const PlayerModuleInfo *module, dd offset)" in player
    assert "static dw player_module_u16_be_at(const PlayerModuleInfo *module, dd offset)" in player
    assert "return (db)(loader_kind(player_module_loader(module)) + player_module_header_seed(module));" in player
    assert "return context && !context->ended && context->block_index < context->max_blocks;" in player
    assert "return context && context->block_index < context->max_blocks;" not in player
    assert "if (!player_decoder_context_has_block(context)) {" in player
    assert "if (context) player_decoder_context_mark_ended(context);" in player
    assert "seed = player_decoder_context_block_seed(context);" in player
    assert "player_playback_mix_voices_pcm(block, context, seed);" in player
    assert "player_decoder_context_next_module_stream_byte(context, fallback)" in player
    assert "player_decoder_context_advance(context);" in player
    assert "if (!player_decoder_context_has_active_voice(context, channel_limit))" in player
    assert "player_playback_fill_stream_pcm(block, context, fallback);" in player
    assert "if (voice->pan_set)" in player
    assert "left += (sample * (long)(255u - voice->pan)) / 255L" in player
    assert "right += (sample * (long)voice->pan) / 255L;" in player
    assert "sample = (sample * (long)context->global_volume) / 64L;" in player
    assert "context->channel += 1u;" in player
    assert "if (context->channel >= context->channel_count)" in player
    assert "context->channel = 0;" in player
    assert "context->current_tick += 1u;" in player
    assert "if (context->current_speed == 0 || context->current_tick >= context->current_speed)" in player
    assert "context->current_tick = 0;" in player
    assert "if (context->pattern_break_pending || context->position_jump_pending)" in player
    assert "context->row = context->pattern_break_pending ? context->pattern_break_row : 0;" in player
    assert "context->order = context->position_jump_order;" in player
    assert "context->pattern_break_pending = 0;" in player
    assert "context->position_jump_pending = 0;" in player
    assert "context->row += 1u;" in player
    assert "return IPLAY_PLAYER_DEFAULT_ROWS_PER_ORDER;" in player
    assert "if (len > 0x70u) return header[0x70u];" in player
    assert "if (len > 0x31u && header[0x31u] != 0) return header[0x31u];" in player
    assert "if (len > 0x32u && header[0x32u] != 0) return header[0x32u];" in player
    assert "if (len > 0x60u + order) return header[0x60u + order];" in player
    assert "raw_count = header[950];" in player
    assert "return raw_count > 128u ? 128u : raw_count;" in player
    assert "static int player_module_order_value_is_skip(dw value)" in player
    assert "return value == 0xfeu;" in player
    assert "static int player_module_order_value_is_end(dw value)" in player
    assert "return value == 0xffu;" in player
    assert "if (len > 950u && header[950] != 0) return header[950];" not in player
    assert "return raw_order >= 0xfeu ? 0u : raw_order;" in player
    assert "if (len > 952u + order) return player_module_mod_order_value(header[952u + order]);" in player
    assert "if (len > 952u + order) return header[952u + order];" not in player
    assert "if (len > 0x22u + order) return header[0x22u + order];" in player
    assert "if (header[0x40u + i] < 16u) count += 1u;" in player
    assert "return count ? count : IPLAY_PLAYER_DEFAULT_CHANNELS;" in player
    assert "if (len >= 1084u) return (dw)mod_channels_from_sig(header + 1080u);" in player
    assert "if (len > 0x20u && header[0x20u] != 0) return header[0x20u];" in player
    assert "return 8u;" in player
    assert "return 16u;" in player
    assert "offset = 1084ul + ((dd)pattern * (dd)IPLAY_PLAYER_DEFAULT_ROWS_PER_ORDER * (dd)channels * 4ul)" in player
    assert "856u, 808u, 762u, 720u, 678u, 640u, 604u, 570u, 538u, 508u, 480u, 453u" in player
    assert "dw best_delta = 0xffffu;" in player
    assert "if (period == 0) return;" in player
    assert "if (note_byte == 0xfeu)" in player
    assert "cell->effect = 0x0eu;" in player
    assert "cell->param = 0xc0u;" in player
    assert "dw delta = periods[i] > period ? (dw)(periods[i] - period) : (dw)(period - periods[i]);" in player
    assert "if (delta < best_delta)" in player
    assert "best = i;" in player
    assert "*note = (db)((best % 12u) + 1u);" in player
    assert "*octave = (db)((best / 12u) + 1u);" in player
    assert "cell->period = (dw)(((dw)(b0 & 0x0fu) << 8) | b1);" in player
    assert "player_mod_period_to_note(cell->period, &cell->note, &cell->octave);" in player
    assert "cell->instrument = (db)((b0 & 0xf0u) | ((b2 >> 4) & 0x0fu));" in player
    assert "cell->effect = (db)(b2 & 0x0fu);" in player
    assert "cell->param = b3;" in player
    assert "cell->volume_set = 0;" in player
    assert "if (cell->effect == 0x0cu)" in player
    assert "cell->volume = cell->param > 64u ? 64u : cell->param;" in player
    assert "cell->volume_set = 1;" in player
    assert "static void player_voice_state_apply_volume_slide(PlayerVoiceState *voice, db param)" in player
    assert "up = (db)((param >> 4) & 0x0fu);" in player
    assert "down = (db)(param & 0x0fu);" in player
    assert "voice->volume = (db)(voice->volume + up > 64u ? 64u : voice->volume + up);" in player
    assert "voice->volume = (db)(voice->volume > down ? voice->volume - down : 0);" in player
    assert "db channel_volume;" in player
    assert "voice->channel_volume = 64u;" in player
    assert "static void player_voice_state_apply_channel_volume(PlayerVoiceState *voice, db param)" in player
    assert "voice->channel_volume = param > 64u ? 64u : param;" in player
    assert "static void player_voice_state_apply_channel_volume_slide(PlayerVoiceState *voice, db param)" in player
    assert "voice->channel_volume = (db)(voice->channel_volume + up > 64u ? 64u : voice->channel_volume + up);" in player
    assert "voice->channel_volume = (db)(voice->channel_volume > down ? voice->channel_volume - down : 0);" in player
    assert "static void player_voice_state_apply_extended_volume_slide(PlayerVoiceState *voice, db param)" in player
    assert "if (kind == 0x0au)" in player
    assert "voice->volume = (db)(voice->volume + amount > 64u ? 64u : voice->volume + amount);" in player
    assert "else if (kind == 0x0bu)" in player
    assert "static void player_voice_state_apply_sample_offset(PlayerVoiceState *voice, db param)" in player
    assert "offset = (dd)param << 8;" in player
    assert "voice->sample_position = offset;" in player
    assert "voice->sample_phase = offset << 8;" in player
    assert "static void player_voice_state_apply_pitch_slide(PlayerVoiceState *voice, db effect, db param)" in player
    assert "voice->period = voice->period > amount ? (dw)(voice->period - amount) : 1u;" in player
    assert "voice->period = (dw)(voice->period + amount < voice->period ? 0xffffu : voice->period + amount);" in player
    assert "voice->sample_step = player_voice_state_step_for_period(voice->period);" in player
    assert "static void player_voice_state_apply_extended_pitch_slide(PlayerVoiceState *voice, db param)" in player
    assert "kind = (db)((param >> 4) & 0x0fu);" in player
    assert "amount = (db)(param & 0x0fu);" in player
    assert "player_voice_state_apply_pitch_slide(voice, 0x01u, amount);" in player
    assert "player_voice_state_apply_pitch_slide(voice, 0x02u, amount);" in player
    assert "dw target_period;" in player
    assert "voice->target_period = 0;" in player
    assert "db pan_set;" in player
    assert "db pan;" in player
    assert "voice->pan_set = 0;" in player
    assert "voice->pan = 128;" in player
    assert "static void player_voice_state_apply_tone_portamento(PlayerVoiceState *voice, db param)" in player
    assert "if (!voice || !voice->active || param == 0 || voice->target_period == 0 || voice->period == 0) return;" in player
    assert "voice->period = (dw)(voice->period - voice->target_period > amount ? voice->period - amount : voice->target_period);" in player
    assert "voice->period = (dw)(voice->target_period - voice->period > amount ? voice->period + amount : voice->target_period);" in player
    assert "static void player_voice_state_apply_arpeggio(PlayerVoiceState *voice, db param)" in player
    assert "semitone = (db)((param >> 4) & 0x0fu);" in player
    assert "if (semitone == 0) semitone = (db)(param & 0x0fu);" in player
    assert "amount = (dw)(((dd)voice->period * (dd)semitone) / 32ul);" in player
    assert "static void player_voice_state_apply_retrigger(PlayerVoiceState *voice, db param)" in player
    assert "voice->sample_position = 0;" in player
    assert "voice->sample_phase = 0;" in player
    assert "static void player_voice_state_apply_extended_retrigger(PlayerVoiceState *voice, db param)" in player
    assert "if (((param >> 4) & 0x0fu) != 0x09u) return;" in player
    assert "player_voice_state_apply_retrigger(voice, (db)(param & 0x0fu));" in player
    assert "static void player_voice_state_apply_vibrato(PlayerVoiceState *voice, db param)" in player
    assert "depth = (db)(param & 0x0fu);" in player
    assert "if (depth == 0) depth = (db)((param >> 4) & 0x0fu);" in player
    assert "period = voice->period > amount ? (dw)(voice->period - amount) : 1u;" in player
    assert "static void player_voice_state_apply_tremolo(PlayerVoiceState *voice, db param)" in player
    assert "voice->volume = voice->volume > depth ? (db)(voice->volume - depth) : 0;" in player
    assert "static void player_voice_state_apply_tremor(PlayerVoiceState *voice, db param)" in player
    assert "on_ticks = (db)((param >> 4) & 0x0fu);" in player
    assert "off_ticks = (db)(param & 0x0fu);" in player
    assert "if (off_ticks != 0 && (on_ticks == 0 || off_ticks >= on_ticks)) voice->volume = 0;" in player
    assert "static void player_voice_state_apply_panning(PlayerVoiceState *voice, db param)" in player
    assert "voice->pan_set = 1;" in player
    assert "voice->pan = param;" in player
    assert "static void player_voice_state_apply_panning_slide(PlayerVoiceState *voice, db param)" in player
    assert "voice->pan = 128;" in player
    assert "voice->pan = (db)(voice->pan + right > 255u ? 255u : voice->pan + right);" in player
    assert "voice->pan = (db)(voice->pan > left ? voice->pan - left : 0);" in player
    assert "static void player_voice_state_apply_panbrello(PlayerVoiceState *voice, db param)" in player
    assert "amount = (dw)depth * 8u;" in player
    assert "voice->pan = (db)(128u + amount > 255u ? 255u : 128u + amount);" in player
    assert "static void player_voice_state_apply_extended_panning(PlayerVoiceState *voice, db param)" in player
    assert "if (((param >> 4) & 0x0fu) != 0x08u) return;" in player
    assert "player_voice_state_apply_panning(voice, (db)((param & 0x0fu) * 17u));" in player
    assert "static void player_voice_state_apply_note_cut(PlayerVoiceState *voice, db param)" in player
    assert "if (((param >> 4) & 0x0fu) != 0x0cu) return;" in player
    assert "voice->active = 0;" in player
    assert "static int player_pattern_cell_has_note_delay(const PlayerPatternCell *cell)" in player
    assert "cell->effect == 0x0eu && ((cell->param >> 4) & 0x0fu) == 0x0du && (cell->param & 0x0fu) != 0" in player
    assert "static int player_pattern_cell_note_delay_tick(const PlayerPatternCell *cell, dw tick)" in player
    assert "return player_pattern_cell_has_note_delay(cell) && (dw)(cell->param & 0x0fu) == tick;" in player
    assert "static void player_decoder_context_load_delayed_row_events(PlayerDecoderContext *context)" in player
    assert "if (!player_pattern_cell_note_delay_tick(&cell, context->current_tick)) continue;" in player
    assert "player_decoder_context_load_delayed_row_events(context);" in player
    assert "static void player_decoder_context_apply_timing_effect(PlayerDecoderContext *context, const PlayerPatternCell *cell)" in player
    assert "if (cell->effect == 0x0fu && cell->param != 0)" in player
    assert "if (cell->param <= 0x20u)" in player
    assert "context->current_speed = cell->param;" in player
    assert "context->current_tempo = cell->param;" in player
    assert "if (cell->effect == 0x0du)" in player
    assert "dw row = (dw)(((cell->param >> 4) & 0x0fu) * 10u + (cell->param & 0x0fu));" in player
    assert "context->pattern_break_row = row;" in player
    assert "context->pattern_break_pending = 1;" in player
    assert "if (cell->effect == 0x0bu)" in player
    assert "context->position_jump_order = cell->param;" in player
    assert "context->position_jump_pending = 1;" in player
    assert "if (cell->effect == 0x16u)" in player
    assert "context->global_volume = cell->param > 64u ? 64u : cell->param;" in player
    assert "if (cell->effect == 0x17u && cell->param != 0)" in player
    assert "context->global_volume = (db)(context->global_volume + up > 64u ? 64u : context->global_volume + up);" in player
    assert "context->global_volume = (db)(context->global_volume > down ? context->global_volume - down : 0);" in player
    assert "if (cell->effect == 0x0eu && ((cell->param >> 4) & 0x0fu) == 0x06u)" in player
    assert "case 4u:" in player
    assert "cell->effect = 0x0au;" in player
    assert "case 5u:" in player
    assert "cell->effect = 0x02u;" in player
    assert "case 6u:" in player
    assert "cell->effect = 0x01u;" in player
    assert "case 7u:" in player
    assert "cell->effect = 0x03u;" in player
    assert "case 8u:" in player
    assert "cell->effect = 0x04u;" in player
    assert "case 9u:" in player
    assert "cell->effect = 0x14u;" in player
    assert "case 18u:" in player
    assert "cell->effect = 0x07u;" in player
    assert "case 19u:" in player
    assert "cell->effect = 0x0eu;" in player
    assert "case 21u:" in player
    assert "cell->effect = 0x04u;" in player
    assert "case 22u:" in player
    assert "cell->effect = 0x16u;" in player
    assert "case 23u:" in player
    assert "cell->effect = 0x17u;" in player
    assert "case 10u:" in player
    assert "cell->effect = 0x00u;" in player
    assert "case 11u:" in player
    assert "cell->effect = 0x12u;" in player
    assert "case 12u:" in player
    assert "cell->effect = 0x13u;" in player
    assert "case 13u:" in player
    assert "cell->effect = 0x19u;" in player
    assert "case 14u:" in player
    assert "cell->effect = 0x1au;" in player
    assert "case 24u:" in player
    assert "cell->effect = 0x15u;" in player
    assert "case 25u:" in player
    assert "cell->effect = 0x1bu;" in player
    assert "if (cell.effect == 0x00u && cell.param != 0) player_voice_state_apply_arpeggio(voice, cell.param);" in player
    assert "if (cell.effect == 0x04u) player_voice_state_apply_vibrato(voice, cell.param);" in player
    assert "if (cell.effect == 0x07u) player_voice_state_apply_tremolo(voice, cell.param);" in player
    assert "if (cell.effect == 0x14u) player_voice_state_apply_tremor(voice, cell.param);" in player
    assert "if (cell.effect == 0x19u) player_voice_state_apply_channel_volume(voice, cell.param);" in player
    assert "if (cell.effect == 0x1au) player_voice_state_apply_channel_volume_slide(voice, cell.param);" in player
    assert "if (cell.effect == 0x12u) {" in player
    assert "player_voice_state_apply_vibrato(voice, cell.param);" in player
    assert "player_voice_state_apply_volume_slide(voice, cell.param);" in player
    assert "if (cell.effect == 0x13u) {" in player
    assert "player_voice_state_apply_tone_portamento(voice, cell.param);" in player
    assert "if (cell.effect == 0x01u || cell.effect == 0x02u) player_voice_state_apply_pitch_slide(voice, cell.effect, cell.param);" in player
    assert "if (cell.effect == 0x03u) player_voice_state_apply_tone_portamento(voice, cell.param);" in player
    assert "if (cell.effect == 0x0eu) player_voice_state_apply_extended_pitch_slide(voice, cell.param);" in player
    assert "if (cell.effect == 0x08u) player_voice_state_apply_panning(voice, cell.param);" in player
    assert "case 15u:" in player
    assert "cell->effect = 0x09u;" in player
    assert "case 16u:" in player
    assert "cell->effect = 0x18u;" in player
    assert "case 17u:" in player
    assert "cell->effect = 0x11u;" in player
    assert "if (cell.effect == 0x11u) player_voice_state_apply_retrigger(voice, cell.param);" in player
    assert "if (cell.effect == 0x0eu) player_voice_state_apply_extended_retrigger(voice, cell.param);" in player
    assert "if (cell.effect == 0x15u) player_voice_state_apply_panning(voice, cell.param);" in player
    assert "if (cell.effect == 0x18u) player_voice_state_apply_panning_slide(voice, cell.param);" in player
    assert "if (cell.effect == 0x1bu) player_voice_state_apply_panbrello(voice, cell.param);" in player
    assert "if (cell.effect == 0x0eu) player_voice_state_apply_extended_panning(voice, cell.param);" in player
    assert "if (cell.effect == 0x0eu) player_voice_state_apply_note_cut(voice, cell.param);" in player
    assert "if (cell.effect == 0x09u) player_voice_state_apply_sample_offset(voice, cell.param);" in player
    assert "if (cell.effect == 0x0au) player_voice_state_apply_volume_slide(voice, cell.param);" in player
    assert "if (cell.effect == 0x0eu) player_voice_state_apply_extended_volume_slide(voice, cell.param);" in player
    assert "sample = (sample * (long)voice->channel_volume) / 64L;" in player
    assert "context->pattern_loop_row = context->row;" in player
    assert "context->pattern_loop_active = 1;" in player
    assert "context->pattern_loop_remaining = 0;" in player
    assert "context->pattern_loop_completed = 0;" in player
    assert "if (context->pattern_loop_completed) return;" in player
    assert "if (context->pattern_loop_remaining == 0) context->pattern_loop_remaining = loop_count;" in player
    assert "context->pattern_loop_remaining -= 1u;" in player
    assert "if (context->pattern_loop_remaining != 0)" in player
    assert "context->pattern_break_row = context->pattern_loop_row;" in player
    assert "context->pattern_break_pending = 1;" in player
    assert "context->pattern_loop_jump_pending = 1;" in player
    assert "context->pattern_loop_completed = 1;" in player
    assert "else if (!context->pattern_loop_jump_pending)" in player
    assert "static dw load_u16_be(const db *p)" in player
    assert "sample->length = (dd)player_module_u16_be_at(module, offset + 22u) * 2u;" in player
    assert "volume = player_module_byte_at(module, offset + 25u);" in player
    assert "sample->volume = volume > 64u ? 64u : volume;" in player
    assert "sample->loop_start = (dd)player_module_u16_be_at(module, offset + 26u) * 2u;" in player
    assert "sample->loop_length = (dd)player_module_u16_be_at(module, offset + 28u) * 2u;" in player
    assert "sample->length = (dd)load_u16_be(header + offset + 22u) * 2u;" not in player
    assert "sample->volume = header[offset + 25u] > 64u ? 64u : header[offset + 25u];" not in player
    assert "sample->loop_start = (dd)load_u16_be(header + offset + 26u) * 2u;" not in player
    assert "sample->loop_length = (dd)load_u16_be(header + offset + 28u) * 2u;" not in player
    assert "flags = player_module_byte_at(module, offset + 31u);" in player
    assert "sample->loop_length = ((flags & 1u) && loop_end > sample->loop_start) ? loop_end - sample->loop_start : 0;" in player
    assert "if (flags & 4u) sample->data_offset |= IPLAY_PLAYER_SAMPLE_DATA_OFFSET_16BIT;" in player
    assert "data_offset = 1084ul + (dd)player_module_pattern_count(module) * (dd)IPLAY_PLAYER_DEFAULT_ROWS_PER_ORDER * (dd)player_module_channel_count(module) * 4ul;" in player
    assert "sample->data_offset = data_offset;" in player
    assert "if (index >= sample->length) return 0;" in player
    assert "static int player_sample_info_is_16bit(const PlayerSampleInfo *sample)" in player
    assert "static dd player_sample_info_data_offset(const PlayerSampleInfo *sample)" in player
    assert "offset = player_sample_info_is_16bit(sample) ? player_sample_info_data_offset(sample) + index * 2ul + 1ul : player_sample_info_data_offset(sample) + index;" in player
    assert "player_sample_info_data_offset(&voice->sample)" in player
    assert "offset = sample->data_offset + (index % sample->length);" not in player
    assert "value = player_module_byte_at(module, offset);" in player
    assert "return value;" in player
    assert "return header[(size_t)offset];" not in player
    assert "if (cell->period != 0) {" in player
    assert "voice->period = cell->period;" in player
    assert "if (cell->note != 0) voice->note = cell->note;" in player
    assert "if (cell->octave != 0) voice->octave = cell->octave;" in player
    assert "player_module_sample_info(module, voice->instrument, &voice->sample);" in player
    assert "voice->sample_position = 0;" in player
    assert "voice->sample_phase = 0;" in player
    assert "if (voice->sample.volume != 0) voice->volume = voice->sample.volume;" in player
    assert "db apply_c2spd = 0;" in player
    assert "apply_c2spd = 1;" in player
    assert "if (apply_c2spd) player_voice_state_apply_sample_c2spd(voice, module);" in player
    assert "if (cell->volume_set == 1u)" in player
    assert "voice->volume = cell->volume;" in player
    assert "else if (cell->volume_set == 2u)" in player
    assert "player_voice_state_apply_panning(voice, cell->volume);" in player
    assert "return voice && voice->period != 0 && voice->sample.length != 0;" in player
    assert "voice->active = player_voice_state_playable(voice) ? 1 : 0;" in player
    assert "if (voice->period != 0 || voice->instrument != 0) voice->active = 1;" not in player
    assert "sample_byte = player_module_sample_byte(module, &voice->sample, voice->sample_position);" in player
    assert "return (dd)(((dd)856u * 0x0100ul) / period);" in player
    assert "voice->sample_step = player_voice_state_step_for_period(voice->period);" in player
    assert "voice->sample_phase += voice->sample_step ? voice->sample_step : 0x0100ul;" in player
    assert "voice->sample_position = voice->sample_phase >> 8;" in player
    assert "return sample && sample->loop_length > 2ul;" in player
    assert "loop_end = sample->loop_start + sample->loop_length;" in player
    assert "return loop_end > sample->length ? sample->length : loop_end;" in player
    assert "if (sample->loop_start >= sample->length) return sample->length ? sample->length - 1ul : 0;" in player
    assert "return sample->loop_start;" in player
    assert "if (player_sample_info_loop_enabled(&voice->sample))" in player
    assert "loop_end = player_sample_info_loop_end(&voice->sample);" in player
    assert "loop_start = player_sample_info_loop_start(&voice->sample);" in player
    assert "voice->sample_phase = loop_start << 8;" in player
    assert "voice->sample_position = loop_start;" in player
    assert "return (db)(fallback + (db)voice->note + (db)(voice->octave << 4) + (db)voice->instrument + (db)voice->volume + (db)voice->period);" in player
    assert "pcm[i + 2u] = (db)(seed + sample_byte + (db)voice->instrument);" in player
    assert "if (channel_limit > IPLAY_PLAYER_MAX_CHANNELS) channel_limit = IPLAY_PLAYER_MAX_CHANNELS;" in player
    assert "for (channel = 0; channel < channel_limit; ++channel)" in player
    assert "long left = ((long)(int)fallback - 128L) << 8;" not in player
    assert "long left = ((long)(int)file_byte - 128L) << 8;" in player
    assert "long right = ((long)(int)file_byte - 128L) << 8;" in player
    assert "return sample_byte < 128u ? (int)sample_byte : (int)sample_byte - 256;" in player
    assert "long sample = (long)player_voice_state_next_sample_s16(voice, context->module);" in player
    assert "static void player_voice_state_advance_sample(PlayerVoiceState *voice)" in player
    assert "static int player_voice_state_sample_s16_at(const PlayerVoiceState *voice, const PlayerModuleInfo *module, dd index)" in player
    assert "static dd player_voice_state_next_sample_index(const PlayerVoiceState *voice, dd index)" in player
    assert "fraction = voice->sample_phase & 0xfful;" in player
    assert "next_index = player_voice_state_next_sample_index(voice, index);" in player
    assert "current = player_voice_state_sample_s16_at(voice, module, index);" in player
    assert "next = player_voice_state_sample_s16_at(voice, module, next_index);" in player
    assert "interpolated = (long)current + (((long)next - (long)current) * (long)fraction) / 256L;" in player
    assert "player_voice_state_advance_sample(voice);" in player
    assert "return player_mix_clamp_s16(interpolated);" in player
    rewrite = (ROOT / "rewrite" / "iplay_rewrite.c").read_text()
    runner = (ROOT / "rewrite" / "rewrite_runner.c").read_text()
    behavior = (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "0x58b4, 0x58e3, 0x5912, 0x5941, 0x5970, 0x599f, 0x59ce, 0x59fd" in rewrite
    assert "0x5d15, 0x5d42, 0x5d6f, 0x5d9c, 0x5dc9, 0x5df6, 0x5e23" in rewrite
    assert "0x58b4, 0x58e3, 0x5912, 0x5941, 0x5970, 0x599f, 0x59ce, 0x59fd" in runner
    assert "0x5d15, 0x5d42, 0x5d6f, 0x5d9c, 0x5dc9, 0x5df6, 0x5e23" in runner
    assert "0x58B4, 0x58E3, 0x5912, 0x5941, 0x5970, 0x599F, 0x59CE, 0x59FD" in behavior
    assert "0x5D15, 0x5D42, 0x5D6F, 0x5D9C, 0x5DC9, 0x5DF6, 0x5E23" in behavior
    assert "offset = player_sample_info_data_offset(&voice->sample) + index * 2ul;" in player
    assert "lo = player_module_byte_at(module, offset);" in player
    assert "hi = player_module_byte_at(module, offset + 1ul);" in player
    assert "lo = player_module_sample_byte(module, &voice->sample, index * 2ul);" not in player
    assert "hi = player_module_sample_byte(module, &voice->sample, index * 2ul + 1ul);" not in player
    assert "if (loader_kind(player_module_loader(module)) == IPLAY_LOADER_KIND_S3M && player_module_s3m_samples_unsigned(module)) hi ^= 0x80u;" in player
    assert "sample = (sample * (long)voice->volume) / 64L;" in player
    assert "if ((channel & 1u) == 0)" in player
    assert "player_mix_put_s16le(pcm, (unsigned)(frame * 4u), player_mix_clamp_s16(left));" in player
    assert "player_mix_put_s16le(pcm, (unsigned)(frame * 4u + 2u), player_mix_clamp_s16(right));" in player
    assert "playback->pcm_checksum += player_playback_block_checksum(block);" in player
    assert "if (context->row >= context->rows_per_order)" in player
    assert "context->order += 1u;" in player
    assert "if (context->order >= context->order_count)" in player
    assert "player_decoder_context_mark_ended(context);" in player
    assert "while (context->order < context->order_count)" in player
    assert "value = player_module_order_value(context->module, context->order);" in player
    assert "context->current_order_value = value;" in player
    assert "player_decoder_context_refresh_current_cell(context);" in player
    assert "player_module_pattern_cell(context->module, context->current_order_value, context->row, channel, &cell);" in player
    assert "voice = player_decoder_context_voice(context, channel);" in player
    assert "player_voice_state_apply_cell(voice, &cell, context->module);" in player
    assert "static void player_report_pcm_source(const PlayerPcmSource *source)" in player
    assert "#define IPLAY_PLAYER_RENDERER_NONE 0u" in player
    assert "#define IPLAY_PLAYER_RENDERER_DOS_FALLBACK 1u" in player
    assert "#define IPLAY_PLAYER_RENDERER_PROJECT 2u" in player
    assert "#define IPLAY_PLAYER_RENDERER_EXTERNAL 3u" in player
    assert "db renderer;" in player
    assert "static db loader_decoder_renderer_kind(const LoaderInfo *loader)" in player
    assert "return IPLAY_PLAYER_RENDERER_EXTERNAL;" in player
    assert "return IPLAY_PLAYER_RENDERER_PROJECT;" in player
    assert "context->renderer = loader_decoder_renderer_kind(context->loader);" in player
    assert "static db player_decoder_context_renderer(const PlayerDecoderContext *context)" in player
    assert "static char player_renderer_code(db renderer)" in player
    assert "static const char *player_decoder_context_provider_name(const PlayerDecoderContext *context)" in player
    assert "static const char *player_decoder_context_hook_provider_name(const PlayerDecoderContext *context)" in player
    assert 'if (player_external_decoder_available()) return player_external_decoder_provider_name();' in player
    assert "case IPLAY_PLAYER_RENDERER_DOS_FALLBACK:" in player
    assert "case IPLAY_PLAYER_RENDERER_PROJECT:" in player
    assert "case IPLAY_PLAYER_RENDERER_EXTERNAL:" in player
    assert "PCM source: %s seed=%u truncated=%u input=%s renderer=%c route=%u provider=%s hook_provider=%s stream_start=%lu" in player
    assert 'printf("PCM source: %s seed=%u truncated=%u input=%s renderer=%c route=%u provider=%s hook_provider=%s stream_start=%lu\\n",' in player
    assert "(unsigned)loader_decoder_route_id(loader)" in player
    assert "player_decoder_context_hook_provider_name(context)" in player
    assert "stream_start=" in player
    assert player.count("stream_start=%lu") == 1
    assert 'if (player_external_decoder_available()) return player_external_decoder_provider_name();\n    return player_module_data_complete(context->module) ? "native-preview" : "dos-fallback";' in player
    assert 'return player_module_data_complete(context->module) ? "native-preview" : "dos-fallback";' in player
    assert "player_decoder_context_provider_name(context)" in player
    assert "static void player_decoder_context_render_dos_fallback_pcm(PlayerDecoderContext *context, PlayerPlaybackBlock *block, db seed)" in player
    assert "static void player_decoder_context_render_external_tracker_pcm(PlayerDecoderContext *context, PlayerPlaybackBlock *block, db seed)" in player
    assert "static void player_decoder_context_render_project_pcm(PlayerDecoderContext *context, PlayerPlaybackBlock *block, db seed)" in player
    assert "player_playback_mix_voices_pcm(block, context, seed);" in player
    assert "player_playback_fill_stream_pcm(block, context, seed);" in player
    assert "if (context && player_module_data_complete(context->module))" in player
    assert "player_playback_mix_voices_pcm(block, context, seed);\n        return;" in player
    assert "player_playback_fill_stream_pcm(block, context, seed);" in player
    assert "static void player_decoder_context_render_external_tracker_pcm(PlayerDecoderContext *context, PlayerPlaybackBlock *block, db seed) {\n    player_decoder_context_render_dos_fallback_pcm(context, block, seed);\n}" not in player
    assert "static void player_decoder_context_render_pcm(PlayerDecoderContext *context, PlayerPlaybackBlock *block, db seed)" in player
    assert "switch (player_decoder_context_renderer(context))" in player
    assert "case IPLAY_PLAYER_RENDERER_PROJECT:" in player
    assert "player_decoder_context_render_project_pcm(context, block, seed);" in player
    assert "case IPLAY_PLAYER_RENDERER_EXTERNAL:" in player
    assert "case IPLAY_PLAYER_RENDERER_DOS_FALLBACK:" in player
    assert "player_decoder_context_render_external_tracker_pcm(context, block, seed);" in player
    assert "player_decoder_context_render_dos_fallback_pcm(context, block, seed);" in player
    assert "player_decoder_context_render_pcm(context, block, seed);" in player
    assert "player_playback_mix_voices_pcm(block, context, seed);\n    player_decoder_context_advance(context);" not in player
    assert "if (!player_module_data_complete(context->module)) return player_decoder_context_next_file_stream_byte(context, fallback);" in player
    assert "static void player_report_decoder_geometry(const PlayerDecoderContext *context)" in player
    assert "Decoder geometry: orders=%u rows/order=%u restart=%u speed=%u tempo=%u channels=%u" in player
    assert "static void player_report_decoder_event(const PlayerDecoderContext *context)" in player
    assert "Decoder event: period=%u note=%u octave=%u instrument=%u volume=%u effect=%u param=%u" in player
    assert "static void player_report_decoder_voice(const PlayerDecoderContext *context)" in player
    assert "Decoder voice: active=%u period=%u note=%u octave=%u instrument=%u volume=%u sample_len=%lu sample_vol=%u loop=%lu/%lu data=%lu" in player
    assert "static void player_report_decoder_progress(const PlayerDecoderContext *context)" in player
    assert "Decoder progress: block=%u/%u order=%u pattern=%u row=%u channel=%u tick=%u/%u speed=%u tempo=%u ended=%u loop=%u" in player
    assert "static int player_playback_fill_next_block(PlayerPlayback *playback, PlayerPcmSource *source)" in player
    assert "static void player_pump_playback_once(IplayRuntime *runtime, PlayerPlayback *playback)" in player
    assert "typedef int (*PlayerPcmSourceEndedFn)(const PlayerPcmSource *source);" in player
    assert "PlayerPcmSourceEndedFn ended;" in player
    assert "static int player_pcm_source_ended(const PlayerPcmSource *source)" in player
    assert "static int player_module_pcm_source_ended(const PlayerPcmSource *source)" in player
    assert "return player_decoder_context_ended((const PlayerDecoderContext *)source->user) != 0;" in player
    assert "player_pcm_source_init(source, player_module_pcm_source_read, player_module_pcm_source_ended, context);" in player
    assert "if (player_pcm_source_ended(source))" in player
    assert "typedef struct PlayerPlaybackLoop" in player
    assert "typedef struct PlayerPlaybackTimer" in player
    assert "dw frames_per_block;" in player
    assert "dw timer_interval_ticks;" in player
    assert "const PlayerPlaybackLoop *loop;" in player
    assert "unsigned long last_ticks;" in player
    assert "dw interval_ticks;" in player
    assert "dw elapsed_ticks;" in player
    assert "dd ready_count;" in player
    assert "db policy;" in player
    assert "db stop_reason;" in player
    assert "#define IPLAY_PLAYER_CONTINUOUS_TIMER_INTERVAL_TICKS 1u" in player
    assert "#define IPLAY_PLAYER_TIMER_IDLE_POLL_LIMIT 4096u" in player
    assert "#define IPLAY_PLAYER_LOOP_POLICY_BOUNDED 1u" in player
    assert "#define IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS 2u" in player
    assert "#define IPLAY_PLAYER_DEFAULT_LOOP_POLICY IPLAY_PLAYER_LOOP_POLICY_BOUNDED" in player
    assert "#define IPLAY_PLAYER_STOP_BLOCK_LIMIT 1u" in player
    assert "#define IPLAY_PLAYER_STOP_SOURCE_END 2u" in player
    assert "#define IPLAY_PLAYER_STOP_KEYBOARD 3u" in player
    assert "static void player_playback_loop_init(PlayerPlaybackLoop *loop, const char *name, db policy, dd max_blocks, dw frames_per_block)" in player
    assert "static void player_playback_loop_init_diagnostic(PlayerPlaybackLoop *loop)" in player
    assert "static void player_playback_loop_init_trial(PlayerPlaybackLoop *loop, dd block_limit)" in player
    assert "static void player_playback_loop_init_continuous(PlayerPlaybackLoop *loop)" in player
    assert "static void player_playback_loop_init_for_policy(PlayerPlaybackLoop *loop, db policy, dd trial_block_limit)" in player
    assert "static void player_playback_loop_init_default(PlayerPlaybackLoop *loop, dd trial_block_limit)" in player
    assert "static dd player_playback_loop_max_blocks(const PlayerPlaybackLoop *loop)" in player
    assert "static dw player_playback_loop_frames_per_block(const PlayerPlaybackLoop *loop)" in player
    assert "static dw player_playback_loop_timer_interval_ticks(const PlayerPlaybackLoop *loop)" in player
    assert "static db player_playback_loop_policy(const PlayerPlaybackLoop *loop)" in player
    assert "static int player_playback_loop_is_bounded(const PlayerPlaybackLoop *loop)" in player
    assert "static const char *player_playback_loop_policy_name(const PlayerPlaybackLoop *loop)" in player
    assert "static int player_playback_loop_uses_timer(const PlayerPlaybackLoop *loop)" in player
    assert "static const char *player_playback_loop_cadence_name(const PlayerPlaybackLoop *loop)" in player
    assert "static void player_playback_timer_init(PlayerPlaybackTimer *timer, const PlayerPlaybackLoop *loop)" in player
    assert "static int player_playback_timer_uses_timer(const PlayerPlaybackTimer *timer)" in player
    assert "static void player_playback_timer_count_ready(PlayerPlaybackTimer *timer)" in player
    assert "static void player_playback_timer_poll_tick(PlayerPlaybackTimer *timer)" in player
    assert "static int player_playback_timer_idle_fallback_ready(PlayerPlaybackTimer *timer)" in player
    assert "static int player_playback_timer_ready(PlayerPlaybackTimer *timer)" in player
    assert "timer->last_ticks = timer->interval_ticks ? dos_hw_io_timer_ticks() : 0;" in player
    assert "unsigned long ticks = dos_hw_io_timer_ticks();" in player
    assert "unsigned long delta = ticks - timer->last_ticks;" in player
    assert "if (delta > 0xfffful) delta = 0xfffful;" in player
    assert "if (delta != 0) timer->ready_count = 0;" in player
    assert "if (timer->ready_count < IPLAY_PLAYER_TIMER_IDLE_POLL_LIMIT)" in player
    assert "timer->ready_count += 1u;" in player
    assert "if (timer->elapsed_ticks < timer->interval_ticks && !player_playback_timer_idle_fallback_ready(timer)) return 0;" in player
    assert "static int player_playback_loop_should_continue(const PlayerPlayback *playback, const PlayerPlaybackLoop *loop)" in player
    assert "static const char *player_playback_stop_reason_name(db stop_reason)" in player
    assert "static void player_playback_mark_block_limit(PlayerPlayback *playback)" in player
    assert "static void player_playback_mark_source_end(PlayerPlayback *playback)" in player
    assert "static void player_playback_mark_keyboard(PlayerPlayback *playback)" in player
    assert "static int player_playback_loop_keyboard_requested(const PlayerPlaybackLoop *loop)" in player
    assert "if (player_playback_loop_policy(loop) != IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS) return 0;" in player
    assert "int (*keyboard_hit)(void);" in player
    assert "static int dos_hw_keyboard_hit(void)" in player
    assert "return kbhit() != 0;" in player
    assert "#define dos_hw_io_keyboard_hit_field(state) ((state)->keyboard_hit)" in player
    assert "static int (*dos_hw_io_keyboard_hit_fn(void))(void)" in player
    assert "static int dos_hw_io_keyboard_hit(void)" in player
    assert "return dos_hw_io_keyboard_hit() != 0;" in player
    assert 'streq(argv[1], "playerkeyboardhw")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert '"playerkeyboardhw"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert 'streq(argv[1], "playerkeyboardstophw")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "static void run_playback_keyboard_stop_hw(void)" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'audio_checksum=%lu audio_first=' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    fixtures = (ROOT / "tests" / "player_behavior_fixtures.py").read_text()
    assert "SB16_STEREO_BYTES_PER_FRAME = 4" in fixtures
    assert "SB16_BOUNDED_BLOCK_FRAMES = 512" in fixtures
    assert "SB16_CONTINUOUS_BLOCK_FRAMES = 1024" in fixtures
    assert "SB16_BOUNDED_BLOCK_BYTES = SB16_BOUNDED_BLOCK_FRAMES * SB16_STEREO_BYTES_PER_FRAME" in fixtures
    assert "SB16_CONTINUOUS_BLOCK_BYTES = SB16_CONTINUOUS_BLOCK_FRAMES * SB16_STEREO_BYTES_PER_FRAME" in fixtures
    assert "def sb16_stereo_byte_count(frames: int) -> int:" in fixtures
    assert "return frames * SB16_STEREO_BYTES_PER_FRAME" in fixtures
    assert "def assert_text_screen_geometry(digest: dict[str, object], cols: int, rows: int) -> None:" in fixtures
    assert 'assert digest["bytes"] == text_mode_byte_count(cols, rows)' in fixtures
    assert 'assert digest["presented"] == text_mode_byte_count(cols, rows)' in fixtures
    assert "def assert_screen_present_content(" in fixtures
    assert "expected_audio_frames: Optional[int] = None" in fixtures
    assert 'assert digest["scope"] == scope' in fixtures
    assert 'assert digest["checksum"] != 0' in fixtures
    assert 'assert digest["nonblank"] > 0' in fixtures
    assert 'assert digest["full"] == 1' in fixtures
    assert 'assert digest["mode_ok"] == 1' in fixtures
    assert 'if require_audio_frames:' in fixtures
    assert 'assert digest["audio_frames"] > 0' in fixtures
    assert "if expected_audio_frames is not None:" in fixtures
    assert 'assert digest["audio_frames"] == expected_audio_frames' in fixtures
    assert "def assert_sb16_stereo_frame_bytes(frames: int, byte_count: int) -> None:" in fixtures
    assert "assert byte_count == sb16_stereo_byte_count(frames)" in fixtures
    assert "assert byte_count % SB16_STEREO_BYTES_PER_FRAME == 0" in fixtures
    assert "def assert_sb16_stereo_block_accounting(blocks: int, frames: int, byte_count: int, frames_per_block: int) -> None:" in fixtures
    assert "assert frames == blocks * frames_per_block" in fixtures
    assert "def assert_playback_pump_sb16_stereo(pump: dict[str, object], blocks: int, frames_per_block: int) -> None:" in fixtures
    assert 'assert pump["blocks"] == blocks' in fixtures
    assert "def assert_playback_pump_stop_state(pump: dict[str, object], limit: int, source_end: int, stop: str) -> None:" in fixtures
    assert 'assert pump["limit"] == limit' in fixtures
    assert 'assert pump["source_end"] == source_end' in fixtures
    assert 'assert pump["stop"] == stop' in fixtures
    function_parity = (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "SB16_BOUNDED_BLOCK_BYTES" in function_parity
    assert "SB16_BOUNDED_BLOCK_FRAMES" in function_parity
    assert "SB16_CONTINUOUS_BLOCK_BYTES" in function_parity
    assert "SB16_CONTINUOUS_BLOCK_FRAMES" in function_parity
    assert "assert_sb16_stereo_block_accounting" in function_parity
    assert "assert_sb16_stereo_frame_bytes" in function_parity
    assert "sb16_stereo_byte_count" in function_parity
    assert 'audio_digest = parse_player_hw_audio_digest(got)' in function_parity
    assert 'assert audio_digest["bytes"] == sb16_stereo_byte_count(SB16_CONTINUOUS_BLOCK_FRAMES)' in function_parity
    assert 'assert audio_digest["first"] != audio_digest["tail"]' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'text_off=%04x text_bytes=%u text_checksum=%lu text_nonblank=%u' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'text_digest = parse_player_hw_text_digest(got)' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert text_digest["bytes"] == 4000' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert text_digest["nonblank"] > 0' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "hw_capture.keyboard_after_audio_copies = 1u;" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert '"playerkeyboardstophw"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert "static void player_pump_playback_loop(IplayRuntime *runtime, PlayerPlayback *playback, PlayerPcmSource *source, const PlayerPlaybackLoop *loop)" in player
    assert "static void player_pump_playback(IplayRuntime *runtime, PlayerPlayback *playback, PlayerPcmSource *source, dd trial_block_limit)" in player
    assert "static void player_refresh_module_playback_position(IplayRuntime *runtime, const PlayerPcmSource *source)" in player
    assert "source->read != player_module_pcm_source_read" in player
    assert "iplay_runtime_render_bottom(runtime," in player
    assert "player_refresh_module_playback_position(runtime, source);" in player
    assert 'levelproof blocks=%u frames=%u accepted=%u audio_copies=%u audio_bytes=%u audio_checksum=%lu audio_first=' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'text_off=%04x text_bytes=%u text_checksum=%lu text_nonblank=%u ui_module=%u ui_sb16=%u ui_playback=%u stopcode=%u' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'test_translated_player_playback_submission_refreshes_dos_text_audio_levels' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert 'assert audio_digest["bytes"] == sb16_stereo_byte_count(SB16_BOUNDED_BLOCK_FRAMES)' in function_parity
    assert 'assert text_digest["checksum"] != 0' in (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "player_playback_loop_init(loop, \"diagnostic\", IPLAY_PLAYER_LOOP_POLICY_BOUNDED, IPLAY_PLAYER_PUMP_BLOCK_LIMIT, IPLAY_PLAYER_PRIME_FRAMES);" in player
    assert "player_playback_loop_init(loop, \"playback\", IPLAY_PLAYER_LOOP_POLICY_BOUNDED, block_limit, IPLAY_PLAYER_SB16_BLOCK_FRAMES);" in player
    assert "player_playback_loop_init(loop, \"playback\", IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS, 0, IPLAY_PLAYER_CONTINUOUS_BLOCK_FRAMES);" in player
    assert 'assert int(field(got, "frames")) == SB16_CONTINUOUS_BLOCK_FRAMES' in function_parity
    assert 'assert int(field(got, "accepted")) == SB16_CONTINUOUS_BLOCK_BYTES' in function_parity
    assert 'assert int(field(got, "audio_bytes")) == sb16_stereo_byte_count(SB16_CONTINUOUS_BLOCK_FRAMES)' in function_parity
    assert 'assert_sb16_stereo_block_accounting(1, int(field(got, "frames")), int(field(got, "accepted")), SB16_CONTINUOUS_BLOCK_FRAMES)' in function_parity
    assert 'assert_sb16_stereo_block_accounting(1, int(field(got, "frames")), int(field(got, "accepted")), SB16_BOUNDED_BLOCK_FRAMES)' in function_parity
    assert 'assert_sb16_stereo_frame_bytes(int(field(got, "frames")), int(field(got, "audio_bytes")))' in function_parity
    assert 'assert_sb16_stereo_frame_bytes(int(field(got, "frames")), audio_digest["bytes"])' in function_parity
    assert "frames/block={SB16_CONTINUOUS_BLOCK_FRAMES}" in function_parity
    assert "loop->timer_interval_ticks = IPLAY_PLAYER_CONTINUOUS_TIMER_INTERVAL_TICKS;" in player
    assert 'streq(argv[1], "playercontinuousloophw")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert '"playercontinuousloophw"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert "player_playback_loop_init_for_policy(loop, IPLAY_PLAYER_DEFAULT_LOOP_POLICY, trial_block_limit);" in player
    assert "player_playback_loop_init_default(&loop, trial_block_limit);" in player
    assert "player_playback_loop_init_trial(&loop);" not in player
    build_script = (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    gate_script = (ROOT / "rewrite" / "check_rewrite.sh").read_text()
    assert "#define IPLAY_PLAYER_ENABLE_DIAGNOSTICS 1" in player
    assert "-fo=rewrite/.build/iplay_player_diag_zm.obj rewrite/iplay_player.c" in build_script
    assert "-DIPLAY_PLAYER_DEFAULT_LOOP_POLICY=IPLAY_PLAYER_LOOP_POLICY_CONTINUOUS" in build_script
    assert "-DIPLAY_PLAYER_ENABLE_DIAGNOSTICS=0" in build_script
    assert "-DIPLAY_PLAYER_ENABLE_DIAGNOSTICS=0 -DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1" in build_script
    assert "-fo=rewrite/.build/iplay_player_cont_zm.obj rewrite/iplay_player.c" in build_script
    assert "-DIPLAY_PLAYER_ENABLE_DIAGNOSTICS=0 -Irewrite -fo=rewrite/.build/iplay_player_try_zm.obj rewrite/iplay_player.c" in build_script
    assert "-DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1 -Irewrite -fo=rewrite/.build/iplay_player_try_zm.obj" not in build_script
    assert "-fo=rewrite/.build/iplay_player_contdiag_zm.obj rewrite/iplay_player.c" in build_script
    assert "-DIPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1 -Irewrite -fo=rewrite/.build/iplay_player_hwdiag_zm.obj rewrite/iplay_player.c" in build_script
    assert "name rewrite/.build/IPLAYC.EXE" in build_script
    assert "file rewrite/.build/iplay_player_cont_zm.obj" in build_script
    assert "name rewrite/.build/IPLAYTRY.EXE" in build_script
    assert "file rewrite/.build/iplay_player_try_zm.obj" in build_script
    assert "name rewrite/.build/IPLAYCONT.EXE" in build_script
    assert "file rewrite/.build/iplay_player_contdiag_zm.obj" in build_script
    assert "name rewrite/.build/IPLAYDIAG.EXE" in build_script
    assert "file rewrite/.build/iplay_player_diag_zm.obj" in build_script
    assert "name rewrite/.build/IPLAYHW.EXE" in build_script
    assert "file rewrite/.build/iplay_player_hwdiag_zm.obj" in build_script
    assert "test -s rewrite/.build/IPLAYCONT.EXE" in gate_script
    assert "test -s rewrite/.build/IPLAYCONT.map" in gate_script
    assert "test -s rewrite/.build/IPLAYTRY.EXE" in gate_script
    assert "test -s rewrite/.build/IPLAYTRY.map" in gate_script
    assert "test -s rewrite/.build/IPLAYDIAG.EXE" in gate_script
    assert "test -s rewrite/.build/IPLAYDIAG.map" in gate_script
    assert "test -s rewrite/.build/IPLAYHW.EXE" in gate_script
    assert "test -s rewrite/.build/IPLAYHW.map" in gate_script
    assert 'for exe_name in ["IPLAYC", "IPLAYTRY", "IPLAYCONT", "IPLAYDIAG", "IPLAYHW"]:' in gate_script
    assert "could not find {exe_name} AUTO code segments" in gate_script
    assert "{exe_name} AUTO code is too close to the 64 KiB DOS limit" in gate_script
    assert '"IPLAYCONT.EXE"' in gate_script
    assert '"IPLAYTRY.EXE"' in gate_script
    assert '"IPLAYDIAG.EXE"' in gate_script
    assert '"IPLAYHW.EXE"' in gate_script
    assert "rewrite/.build/IPLAYCONT.map" in gate_script
    assert "rewrite/.build/IPLAYTRY.map" in gate_script
    assert "rewrite/.build/IPLAYDIAG.map" in gate_script
    assert "rewrite/.build/IPLAYHW.map" in gate_script
    assert "rewrite/.build/iplay_modern_host" in build_script
    assert "rewrite/.build/iplay_native" in build_script
    assert "rewrite/.build/iplay" in build_script
    assert "pkg-config --cflags libmodplug sdl2" in build_script
    assert "pkg-config --libs libmodplug sdl2" in build_script
    assert "rewrite/modplug_renderer.cpp" in build_script
    assert "rewrite/modplug_audio_bridge.cpp" in build_script
    assert "rewrite/modern_player.cpp" in build_script
    assert "rewrite/modplug_audio_probe.cpp" in build_script
    assert "test -x rewrite/.build/iplay_modern_host" in gate_script
    assert "test -x rewrite/.build/iplay_native" in gate_script
    assert "test -x rewrite/.build/iplay" in gate_script
    smoke = (ROOT / "tests" / "test_player_smoke.py").read_text()
    trial_script = (ROOT / "rewrite" / "try_player.sh").read_text()
    native_build_script = (ROOT / "rewrite" / "build_native_player.sh").read_text()
    assert "--native runs rewrite/.build/iplay_native directly on the host" in trial_script
    assert "--modern runs rewrite/.build/iplay, the preferred SDL/notcurses host player" in trial_script
    assert "--native-interactive enables native source-end playback, SDL2 audio, terminal render, live meters, and raw stdin keyboard stop" in trial_script
    assert "--modern)" in trial_script
    assert "--native-interactive)" in trial_script
    assert "trial_modern=1" in trial_script
    assert 'validate_trial_exe=${IPLAY_TRIAL_EXE:-iplay}' in trial_script
    assert 'validate_trial_exe=${IPLAY_TRIAL_EXE:-iplay_native}' in trial_script
    assert 'IPLAY_NATIVE_EXE=${IPLAY_TRIAL_EXE:-iplay}' in trial_script
    assert 'IPLAY_NATIVE_EXE=${IPLAY_TRIAL_EXE:-iplay_native}' in trial_script
    assert "native_use_default_player_args=1" in trial_script
    assert "native_use_default_player_args=0" in trial_script
    assert 'if [ "$native_use_default_player_args" = "1" ]; then' in trial_script
    assert 'set -- "$native_module_arg" "$trial_video_mode"' in trial_script
    assert "native_modern=%s" in trial_script
    assert "trial_native_source_end=1" in trial_script
    assert "--native-stdin-keyboard stops native playback when q, Q, or Escape is read from stdin" in trial_script
    assert "--native-audio opens a real SDL2 queued-audio device in native mode" in trial_script
    assert "--native-terminal renders the final notcurses-style text cells to the host terminal with ANSI 16-color output" in trial_script
    assert "--native-live updates ANSI audio level meters from the native playback callback while blocks are submitted" in trial_script
    assert 'set -- "$native_module_arg" "$native_play_arg" "$trial_video_mode"' in trial_script
    assert "native-interactive-source-end-keyboard-stop" in trial_script
    assert 'set -- "$@" --sdl-audio' in trial_script
    assert 'set -- "$@" --terminal-render' in trial_script
    assert 'set -- "$@" --terminal-live' in trial_script
    assert 'set -- "$@" --stdin-keyboard' in trial_script
    assert '"rewrite/.build/$IPLAY_NATIVE_EXE" "$@"' in trial_script
    assert "native_passthrough=1" in trial_script
    assert "native_passthrough_label=modern" in trial_script
    assert 'native_passthrough_label="native interactive"' in trial_script
    assert "try_player %s: module=%s mode=%s cols=%s rows=%s stop_keys=q,Q,Esc log=%s" in trial_script
    assert "native_rc_file=$(mktemp" in trial_script
    assert "trap 'rm -f \"$native_rc_file\"' EXIT HUP INT TERM" in trial_script
    assert "trap - EXIT HUP INT TERM" in trial_script
    assert 'tee -a "$IPLAY_TRIAL_LOG"' in trial_script
    assert 'tail -n 4 "$IPLAY_TRIAL_LOG"' not in trial_script
    assert "printf 'trial_result=%s\\n' \"$trial_result\"" in trial_script
    assert "printf 'trial_script_exit_status=%s\\n' \"$trial_script_exit_status\"" in trial_script
    assert "./rewrite/build_native_player.sh" in trial_script
    assert "rewrite/modplug_audio_probe.cpp" in native_build_script
    assert "pkg-config --cflags libmodplug sdl2" in native_build_script
    assert "pkg-config --libs libmodplug sdl2" in native_build_script
    assert "trial_proof_scope=native-sdl-notcurses" in trial_script
    assert "trial_result=native-sdl-notcurses-ok" in trial_script
    assert 'summary="Audio backend: .*; Playback enabled;' in trial_script
    assert "test_try_player_native_sdl_notcurses_reports_playback_ok" in smoke
    assert "test_user_facing_iplay_alias_runs_modern_sdl_notcurses_player" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_try_player_validate_only_selects_native_interactive_without_kvikdos" in smoke
    assert "test_try_player_validate_only_selects_modern_alias_after_module_without_kvikdos" in smoke
    assert "test_try_player_validate_only_native_interactive_defaults_to_terminal_size_without_kvikdos" in smoke
    assert "test_try_player_native_interactive_alias_combines_audio_terminal_live_keyboard" in smoke
    assert "test_try_player_modern_alias_uses_iplay_default_player_args" in smoke
    assert "test_try_player_native_interactive_without_key_runs_to_source_end" in smoke
    assert "test_try_player_validate_only_selects_native_sdl_audio_without_kvikdos" in smoke
    assert "test_try_player_validate_only_selects_native_terminal_without_kvikdos" in smoke
    assert "test_try_player_native_terminal_reports_rendered_screen" in smoke
    assert "test_try_player_validate_only_selects_native_live_without_kvikdos" in smoke
    assert "test_try_player_native_live_reports_changing_audio_levels" in smoke
    assert "test_try_player_validate_only_selects_native_stdin_keyboard_without_kvikdos" in smoke
    assert "test_try_player_native_stdin_keyboard_stops_on_q" in smoke
    modern = (ROOT / "rewrite" / "modern_player.cpp").read_text()
    modern_probe = (ROOT / "rewrite" / "modplug_audio_probe.cpp").read_text()
    status = (ROOT / "rewrite" / "STATUS.md").read_text()
    assert "static const char *const MODERN_STATUS_TITLE = \"Inertia Player V1.22\";" in modern
    assert "static const char *const MODERN_STATUS_AUDIO_BACKEND = \"SDL-compatible SB16 16-bit stereo\";" in modern
    assert "static const char *const MODERN_STATUS_INTERPOLATION = \"24bit Interpolation      F-12\";" in modern
    assert "static const char *const MODERN_STATUS_CURRENT_TRACK = \"1/0\";" in modern
    assert "static const char *const MODERN_STATUS_TRACK_POSITION = \"1/64\";" in modern
    assert "static const char *const MODERN_STATUS_FREE_MEMORY = \"482KB\";" in modern
    assert "static const char *const MODERN_STATUS_SAMPLES_USED = \"0/15\";" in modern
    assert "static const char *const MODERN_STATUS_MAIN_VOLUME = \" 100%      - +\";" in modern
    assert "#include <SDL.h>" in modern_probe
    assert "static int native_audio_sink_open_sdl(NativeAudioSink *sink)" in modern_probe
    assert "SDL_OpenAudioDevice(0, 0, &want, &have, SDL_AUDIO_ALLOW_SAMPLES_CHANGE)" in modern_probe
    assert "if (have.freq != want.freq || have.format != want.format || have.channels != want.channels)" in modern_probe
    assert "sink->samples = have.samples;" in modern_probe
    assert "native_audio_sink_register_close(sink);" in modern_probe
    assert "static void native_audio_sink_close_at_exit(void)" in modern_probe
    assert "static void native_restore_process_state_on_signal(int signum)" in modern_probe
    assert "std::atexit(native_audio_sink_close_at_exit);" in modern_probe
    assert "native_audio_sink_close(native_audio_sink_to_close);" in modern_probe
    assert "could not open SDL2 SB16 stereo audio sink requested freq=%d format=0x%04x channels=%u samples=%u" in modern_probe
    assert "sdl_queue_limit_bytes" in modern_probe
    assert "SDL_GetQueuedAudioSize(sink->device) > sink->sdl_queue_limit_bytes" in modern_probe
    assert "queue_limit_bytes=%lu queue_waits=%lu driver=%s paused=%d queue_cleared=%d closed=%d" in modern_probe
    assert "char sdl_driver[32];" in modern_probe
    assert "SDL_GetCurrentAudioDriver() ? SDL_GetCurrentAudioDriver() : \"none\"" in modern_probe
    assert "int sdl_closed;" in modern_probe
    assert "SDL_PauseAudioDevice(sink->device, 1);" in modern_probe
    assert "SDL_ClearQueuedAudio(sink->device);" in modern_probe
    assert "if (!sink || sink->sdl_closed || (!sink->sdl_requested && !sink->sdl_opened)) return;" in modern_probe
    assert "sink->sdl_closed = 1;" in modern_probe
    assert "SDL_QueueAudio(sink->device, pcm, byte_count)" in modern_probe
    assert "SDL audio sink: requested=%d opened=%d bytes=%lu queue_failures=%lu freq=%d format=0x%04x channels=%u samples=%u" in modern_probe
    assert "static void native_terminal_render_playback(const IplayTextMode *mode, const char *path, const IplayModernPlaybackResult *result)" in modern_probe
    assert "native_terminal_render_cells(cells, mode, 1);" in modern_probe
    assert "native_terminal_render_cells(cells, mode, 0);" in modern_probe
    presenter = (ROOT / "rewrite" / "notcurses_presenter.cpp").read_text()
    assert "iplay_notcurses_present_cells(cells, mode)" in modern_probe
    assert "notcurses_init(&options, stdout)" in presenter
    assert "ncplane_putegc_yx" in presenter
    assert "draw_80x25_closing_row(plane)" in presenter
    assert "Terminal render: requested=1 cols=%u rows=%u bytes=%u screen_checksum=%lu screen_nonblank=%u present_calls=%lu present_bytes=%lu" in modern_probe
    assert "static bool native_playback_progress(void *user, const IplayModplugAudioBridgeStats *stats)" in modern_probe
    assert "IPLAY_NATIVE_LIVE_INITIAL_PRINT_BLOCKS" in modern_probe
    assert "IPLAY_NATIVE_LIVE_PRINT_CADENCE_BLOCKS" in modern_probe
    assert "control->live_suppressed += 1ul;" in modern_probe
    assert "terminal_live_cursor_hidden" in modern_probe
    assert "static void native_terminal_live_finish(NativeRunControl *control)" in modern_probe
    assert 'std::printf("\\033[?25l");' in modern_probe
    assert 'std::printf("\\033[0m\\033[?25h\\n");' in modern_probe
    assert "Terminal live: block=%lu frames=%lu accepted=%lu levels=%u/%u L[" in modern_probe
    assert "Terminal live summary: requested=1 samples=%lu nonzero=%lu changed=%u printed=%lu suppressed=%lu" in modern_probe
    assert "Terminal resize: requested=1 signals=%u changes=%u initial=%ux%u current=%ux%u" in modern_probe
    assert "native_terminal_resize_signal_count" in modern_probe
    assert "std::signal(SIGWINCH, native_terminal_resize_signal);" in modern_probe
    assert "test_direct_iplay_tracks_sigwinch_terminal_resize_during_live_playback" in (ROOT / "tests" / "test_player_smoke.py").read_text()
    assert "enum NativeKeyboardAction" in modern_probe
    assert "static NativeKeyboardAction native_stdin_keyboard_action(void)" in modern_probe
    assert "IplayModplugPlaybackControls *controls;" in modern_probe
    assert "struct NativeKeyboardMode" in modern_probe
    assert "static void native_keyboard_mode_enable(NativeKeyboardMode *mode, int requested)" in modern_probe
    assert "static void native_keyboard_mode_restore(NativeKeyboardMode *mode)" in modern_probe
    assert "static void native_keyboard_mode_restore_at_exit(void)" in modern_probe
    assert "std::atexit(native_keyboard_mode_restore_at_exit);" in modern_probe
    assert "std::signal(SIGINT, native_restore_process_state_on_signal);" in modern_probe
    assert "std::signal(SIGTERM, native_restore_process_state_on_signal);" in modern_probe
    assert "std::signal(SIGHUP, native_restore_process_state_on_signal);" in modern_probe
    assert "raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);" in modern_probe
    assert "native_keyboard_mode_register_restore(mode);" in modern_probe
    assert "native_keyboard_mode_restore(&keyboard_mode);" in modern_probe
    assert "select(STDIN_FILENO + 1, &fds, 0, 0, &tv)" in modern_probe
    assert "Stdin keyboard: requested=1 stopped=%d" in modern_probe
    assert "Stdin keyboard mode: requested=1 raw=%d restored=%d" in modern_probe
    assert "int iplay_modern_path_is_external_tracker(const char *path)" in modern
    assert "int iplay_modern_path_is_project_owned(const char *path)" in modern
    assert "enum IplayModernDecoderRoute" in (ROOT / "rewrite" / "modern_player.hpp").read_text()
    assert "IplayModernDecoderRoute decoder_route;" in (ROOT / "rewrite" / "modern_player.hpp").read_text()
    assert "const char *decoder_provider;" in (ROOT / "rewrite" / "modern_player.hpp").read_text()
    assert "IplayModernDecoderRoute iplay_modern_decoder_route(const char *path)" in modern
    assert "const char *iplay_modern_decoder_route_name(const char *path)" in modern
    assert "int iplay_modern_decoder_route_uses_external_library(const char *path)" in modern
    assert "const char *iplay_modern_playback_decoder_route_name(const IplayModernPlaybackResult *result)" in modern
    assert "const char *iplay_modern_playback_decoder_provider_name(const IplayModernPlaybackResult *result)" in modern
    assert "result->decoder_route = iplay_modern_decoder_route(path);" in modern
    assert '? ((modern_extension_equals(modern_path_extension(path), ".mod") ||' in modern
    assert 'modern_extension_equals(modern_path_extension(path), ".nst")) ? "libmodplug" : "libmikmod")' in modern
    assert 'return "external-library";' in modern
    assert 'return "project-owned";' in modern
    assert 'return "probe-by-content";' in modern
    assert "--classify" in modern_probe
    assert "static bool modern_finish_format(char *dst, size_t dst_size, int written)" in modern
    assert "return modern_finish_format(dst, dst_size, written);" in modern
    assert "bool iplay_modern_playback_status_started(IplayModernPlaybackStatus status)" in modern
    assert "status == IPLAY_MODERN_PLAYBACK_BLOCK_LIMIT" in modern
    assert "status == IPLAY_MODERN_PLAYBACK_KEYBOARD" in modern
    assert "iplay_modern_playback_status_started(result->status)" in modern
    assert "const char *iplay_modern_playback_state_text(IplayModernPlaybackStatus status)" in modern
    assert "return iplay_modern_playback_status_started(status) ? \"Playback enabled\" : \"Playback disabled\";" in modern
    assert "iplay_modern_playback_state_text(result->status)" in modern
    assert "iplay_modern_playback_decoder_route_name(result)" in modern
    assert "iplay_modern_playback_decoder_provider_name(result)" in modern
    assert "route=%s; provider=%s; status=%s" in modern
    assert "route_id=%d route=%s provider=%s" in modern_probe
    assert "const char *iplay_modern_playback_stop_text(const IplayModernPlaybackResult *result)" in modern
    assert "return result->audio.stop_reason;" in modern
    assert "iplay_modern_playback_stop_text(result)" in modern
    assert "const char *iplay_modern_playback_panel_status_text(const IplayModernPlaybackResult *result)" in modern
    assert "if (iplay_modern_playback_status_started(result->status)) return iplay_modern_playback_stop_text(result);" in modern
    assert "return iplay_modern_playback_status_name(result->status);" in modern
    assert "static const char *modern_status_module_display_name(const char *path)" in modern
    assert "if (*cursor == '/' || *cursor == '\\\\' || *cursor == ':') base = cursor + 1;" in modern
    assert "module_display_name = modern_status_module_display_name(module_path);" in modern
    assert "module_type_text = modern_status_module_type_text(module_path);" in modern
    assert "const char *iplay_modern_status_title(void)" in modern
    assert "return MODERN_STATUS_TITLE;" in modern
    assert "const char *iplay_modern_audio_backend_name(void)" in modern
    assert "return MODERN_STATUS_AUDIO_BACKEND;" in modern
    assert "static const char *modern_status_module_type_text(const char *path)" in modern
    assert 'return "S3M";' in modern
    assert 'return "N.T.";' in modern
    assert 'return "INR";' in modern
    assert "iplay_modern_audio_backend_name()" in modern
    assert "bool iplay_modern_format_blocks(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert 'std::snprintf(dst, dst_size, "%lu", result->audio.blocks)' in modern
    assert "iplay_modern_format_blocks(result, blocks, sizeof(blocks))" in modern
    assert "bool iplay_modern_format_stop(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert '"%s end=%u via %s/%s"' in modern
    assert "iplay_modern_playback_decoder_route_name(result)" in modern
    assert "iplay_modern_playback_decoder_provider_name(result)" in modern
    assert "iplay_modern_format_stop(result, stop, sizeof(stop))" in modern
    assert "bool iplay_modern_format_accepted(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert 'std::snprintf(dst, dst_size, "%lu drop %lu", result->audio.accepted_bytes, result->audio.dropped_frames)' in modern
    assert "iplay_modern_format_accepted(result, accepted, sizeof(accepted))" in modern
    assert "bool iplay_modern_format_frames(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert 'std::snprintf(dst, dst_size, "%lu sum %lu", result->audio.source_frames, result->audio.source_checksum)' in modern
    assert "iplay_modern_format_frames(result, frames, sizeof(frames))" in modern
    assert "bool iplay_modern_format_levels(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert "bool iplay_modern_format_playback_state(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert '"L[%.*s%.*s] R[%.*s%.*s]"' in modern
    assert '"########"' in modern
    assert '"--------"' in modern
    assert "result->audio.last_left_level" in modern
    assert "result->audio.last_right_level" in modern
    assert "maxlevels=%u,%u" in modern
    assert "iplay_modern_format_levels(result, levels, sizeof(levels))" in modern
    assert "iplay_modern_format_playback_state(result, playback_state, sizeof(playback_state))" in modern
    assert "iplay_runtime_draw_status_line(runtime, IPLAY_RUNTIME_STATUS_TITLE_ROW, iplay_modern_status_title()" in modern
    for row in [
        "IPLAY_RUNTIME_STATUS_MODULE_ROW",
        "IPLAY_RUNTIME_STATUS_SIZE_ROW",
        "IPLAY_RUNTIME_STATUS_LOADER_ROW",
        "IPLAY_RUNTIME_STATUS_AUDIO_ROW",
        "IPLAY_RUNTIME_STATUS_HARDWARE_ROW",
        "IPLAY_RUNTIME_STATUS_VIDEO_ROW",
        "IPLAY_RUNTIME_STATUS_LEVELS_ROW",
        "IPLAY_RUNTIME_STATUS_TAG_ROW",
    ]:
        assert f"iplay_runtime_draw_status_field(runtime, {row}" in modern
    for label in ["MODULE", "BLOCKS", "STOP", "AUDIO", "ACCEPTED", "FRAMES", "LEVELS", "STATUS", "PLAYBACK"]:
        assert f"MODERN_STATUS_LABEL_{label}" in modern
    assert 'MODERN_STATUS_LABEL_MODULE = "Filename      "' in modern
    assert 'MODERN_STATUS_LABEL_STOP = "Track Position"' in modern
    assert 'MODERN_STATUS_LABEL_AUDIO = "Playing in Stereo, Free"' in modern
    assert 'MODERN_STATUS_LABEL_STATUS = "Module Type   "' in modern
    assert "IPLAY_RUNTIME_STATUS_SIZE_ROW, MODERN_STATUS_LABEL_BLOCKS, MODERN_STATUS_CURRENT_TRACK" in modern
    assert "IPLAY_RUNTIME_STATUS_LOADER_ROW, MODERN_STATUS_LABEL_STOP, MODERN_STATUS_TRACK_POSITION" in modern
    assert "IPLAY_RUNTIME_STATUS_AUDIO_ROW, MODERN_STATUS_LABEL_AUDIO, MODERN_STATUS_FREE_MEMORY" in modern
    assert "IPLAY_RUNTIME_STATUS_HARDWARE_ROW, MODERN_STATUS_LABEL_ACCEPTED, MODERN_STATUS_SAMPLES_USED" in modern
    assert "IPLAY_RUNTIME_STATUS_VIDEO_ROW, MODERN_STATUS_LABEL_FRAMES, MODERN_STATUS_MAIN_VOLUME" in modern
    assert "IPLAY_RUNTIME_STATUS_MODULE_ROW, MODERN_STATUS_LABEL_MODULE, module_display_name" in modern
    assert "IPLAY_RUNTIME_STATUS_TAG_ROW, MODERN_STATUS_LABEL_STATUS, module_type_text" in modern
    assert "iplay_runtime_draw_status_line(runtime, IPLAY_RUNTIME_STATUS_PLAYBACK_ROW, MODERN_STATUS_INTERPOLATION" in modern
    assert 'screen40_{row_name}="' in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert 'screen80x50_{row_name}="' in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert '("levels", r"Output Levels.*L\\[.*\\] R\\[")' in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "result->audio.accepted_bytes" in modern
    assert "result->audio.dropped_frames" in modern
    assert "result->audio.source_checksum" in modern
    assert "result->audio.active" in modern
    assert 'std::snprintf(accepted, sizeof(accepted), "%lu drop %lu", result->audio.accepted_bytes, result->audio.dropped_frames);' in modern
    assert 'std::snprintf(frames, sizeof(frames), "%lu sum %lu", result->audio.source_frames, result->audio.source_checksum);' in modern
    assert 'std::snprintf(dst, dst_size, "%s active=%u", iplay_modern_playback_state_text(result->status), result->audio.active);' in modern
    assert 'std::snprintf(stop, sizeof(stop), "%s end=%u via %s/%s", stop_text, result->audio.source_ended, route_text, provider_text);' in modern
    assert 'iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_LOADER_ROW, "Stop", stop' in modern
    for suffix in ["blocks", "stop", "accepted", "frames", "levels"]:
        assert f'%s_{suffix}=\\\"' in modern_probe
    assert '%s_title=\\\"' in modern_probe
    assert "block count, stop reason plus source-end state, audio backend, accepted byte and dropped frame counts, frame count plus source checksum, live/max levels, decode status, playback state plus active flag" in status
    assert "player_pump_playback_loop(runtime, playback, source, &loop);" in player
    assert "PlayerPlaybackTimer timer;" in player
    assert "player_playback_timer_init(&timer, loop);" in player
    assert "player_pcm_source_set_frames_per_block(source, player_playback_loop_frames_per_block(loop));" in player
    assert "while (player_playback_loop_should_continue(playback, loop))" in player
    assert "if (!player_playback_timer_ready(&timer)) continue;" in player
    assert "if (player_playback_loop_keyboard_requested(loop))" in player
    assert "player_playback_mark_keyboard(playback);" in player
    assert "if (!player_playback_fill_next_block(playback, source))" in player
    assert "player_playback_mark_source_end(playback);" in player
    assert "if (player_playback_loop_reached_limit(playback, loop)) player_playback_mark_block_limit(playback);" in player
    assert "Playback loop: mode=%s policy=%s cadence=%s max_blocks=%lu frames/block=%u" in player
    assert "Playback pump: blocks=%lu frames=%lu accepted=%lu checksum=%lu limit=%u source_end=%u stop=%s" in player
    assert "return \"bounded-trial\";" in player
    assert "return \"timer-keyboard\";" in player
    assert "return player_playback_loop_uses_timer(loop) ? \"timer\" : \"immediate\";" in player
    assert "return \"block-limit\";" in player
    assert "return \"source-end\";" in player
    assert "return \"keyboard\";" in player
    assert "while (playback->blocks_submitted < IPLAY_PLAYER_PUMP_BLOCK_LIMIT && player_playback_fill_next_block(playback, source)) player_submit_playback_block(runtime, playback);" not in player
    assert "while (player_playback_fill_next_block(playback, source)) player_submit_playback_block(runtime, playback);" not in player
    assert "player_report_decoder_progress(&decoder);" in player
    assert "static void player_submit_runtime_prime_pcm(IplayRuntime *runtime, const PlayerModuleInfo *module, dd trial_block_limit)" in player
    assert "player_decoder_context_close_file_stream(&decoder);" in player
    assert "player_decoder_context_init(&decoder, module);" in player
    assert "player_module_pcm_source_init(&source, &decoder);" in player
    assert "player_report_pcm_source(&source);" in player
    assert "player_report_decoder_geometry(&decoder);" in player
    assert "iplay_runtime_audio_set_capacity(runtime, frames);" in player
    assert "accepted = iplay_runtime_write_sb16_frames(runtime, player_playback_block_pcm_const(block), frames);" in player
    assert "playback->limit_reached = 0;" in player
    assert "playback->source_ended = 0;" in player
    assert "Playback pump: blocks=%lu frames=%lu accepted=%lu checksum=%lu limit=%u source_end=%u" in player
    assert "static void player_prime_runtime_playback(IplayRuntime *runtime, const PlayerModuleInfo *module, dd trial_block_limit)" in player
    assert "const IplayAudioLevels *levels = iplay_runtime_audio_levels(runtime);" in player
    assert "Playback prime: ready=%u hw=%u backend=%s status=%s frames=%lu capacity=%lu dropped=%lu queued=%lu levels=%u/%u" in player
    assert "iplay_runtime_audio_backend_name(runtime)" in player
    assert "iplay_runtime_audio_status_text(runtime)" in player
    assert "(unsigned long)iplay_runtime_audio_dropped_frames(runtime)" in player
    assert "(unsigned long)iplay_runtime_audio_queued_frames(runtime)" in player
    assert "iplay_audio_levels_left_16(levels)" in player
    assert "iplay_audio_levels_right_16(levels)" in player
    assert "Playback prime: ready=1 hw=1 backend=.* status=.* frames=16384 capacity=0 dropped=0 queued=0 levels=" in (ROOT / "rewrite" / "smoke_player.sh").read_text()
    assert "static void player_shutdown_runtime(IplayRuntime *runtime)" in player
    assert "player_shutdown_runtime(runtime);" in player
    assert "(void)runtime;" in player
    assert "iplay_runtime_shutdown(runtime)" not in player
    assert "iplay_runtime_shutdown(&runtime" not in player
    assert "iplay_runtime_draw_status_line(&runtime" not in player
    assert "iplay_runtime_draw_status_field(&runtime" not in player
    assert "iplay_runtime_draw_status_u32(&runtime" not in player
    assert "iplay_runtime_draw_status_hex32(&runtime" not in player
    assert "iplay_runtime_draw_module_status(&runtime" not in player
    assert "iplay_runtime_draw_module_tag(&runtime" not in player
    assert "static void player_init_module_status(IplayModuleStatus *module_status, const PlayerModuleInfo *module)" in player
    assert "player_init_module_status(module_status, module);" in player
    assert "const LoaderInfo *loader = player_module_loader(module);" in player
    assert "iplay_module_status_init(module_status, loader_name(loader), player_module_path(module), (dd)player_module_size(module), loader_symbol(loader), 0);" in player
    assert "static void player_activate_runtime_ui(IplayRuntime *runtime, IplayRuntimeConfig *runtime_config)" in player
    assert "player_activate_runtime_ui(runtime, runtime_config);" in player
    assert "static void player_render_runtime_status_reason(IplayRuntime *runtime, const PlayerModuleInfo *module, const char *reason)" in player
    assert 'player_render_runtime_status_reason(runtime, module, "status");' in player
    assert "static void player_render_runtime_audio_status_frame(IplayRuntime *runtime, const PlayerModuleInfo *module, const char *reason)" in player
    assert "iplay_runtime_draw_status_block(runtime, &compact_status);" in player
    assert "player_draw_original_live_module_info(runtime, module);" in player
    assert "player_present_runtime_frame_scope(runtime, reason, \"status-only\");" in player
    assert 'player_render_runtime_audio_status_frame(runtime, module, "post-playback-status");' in player
    assert 'player_render_runtime_status_reason(runtime, module_status, "post-playback-status");' not in player
    assert "iplay_runtime_draw_audio_status(runtime);" in player
    assert "static void player_render_runtime_audio_unavailable(IplayRuntime *runtime) {\n    iplay_runtime_draw_audio_status(runtime);" in player
    assert "iplay_runtime_draw_audio_levels(runtime,\n                                    IPLAY_RUNTIME_STATUS_LEVELS_ROW,\n                                    IPLAY_RUNTIME_STATUS_LEVELS_X,\n                                    IPLAY_RUNTIME_STATUS_LEVELS_WIDTH);" in player
    assert "static dw player_present_runtime_frame_scope(IplayRuntime *runtime, const char *reason, const char *scope)" in player
    assert "static dw player_present_runtime_frame(IplayRuntime *runtime, const char *reason)" in player
    assert "dd iplay_text_cells_checksum(const db *cells, dw byte_count)" in header
    assert "dw iplay_text_cells_nonblank_count(const db *cells, dw byte_count)" in header
    assert "dd iplay_text_screen_checksum(const IplayTextScreen *screen)" in header
    assert "dw iplay_text_screen_nonblank_count(const IplayTextScreen *screen)" in header
    assert "dd iplay_text_cells_checksum(const db *cells, dw byte_count)" in rewrite
    assert "dw iplay_text_cells_nonblank_count(const db *cells, dw byte_count)" in rewrite
    assert "checksum *= 16777619ul;" in rewrite
    assert "if (cells[i] != 0 && cells[i] != ' ') ++count;" in rewrite
    assert "return iplay_text_cells_checksum(iplay_text_screen_cells_const(screen), iplay_text_screen_bytes(screen));" in rewrite
    assert "return iplay_text_cells_nonblank_count(iplay_text_screen_cells_const(screen), iplay_text_screen_bytes(screen));" in rewrite
    assert "static dd player_screen_cell_checksum" not in player
    assert "static dw player_screen_nonblank_cells" not in player
    assert "dw byte_count;\n#if IPLAY_PLAYER_ENABLE_DIAGNOSTICS\n    dw screen_bytes;\n    dd screen_checksum;\n    dw screen_nonblank;\n    const IplayAudioLevels *levels;" in player
    assert "byte_count = iplay_runtime_present(runtime);" in player
    assert "screen_bytes = iplay_runtime_video_screen_bytes(runtime);" in player
    assert "screen_checksum = iplay_runtime_video_checksum(runtime);" in player
    assert "screen_nonblank = iplay_runtime_video_nonblank_cells(runtime);" in player
    assert "levels = iplay_runtime_audio_levels(runtime);" in player
    assert 'printf("Screen present: reason=%s scope=%s bytes=%u screen_bytes=%u screen_checksum=%lu screen_nonblank=%u full=%u cols=%u rows=%u mode_ok=%u audio_frames=%lu levels=%u/%u\\n",' in player
    assert 'return player_present_runtime_frame_scope(runtime, reason, "full-screen");' in player
    assert 'player_present_runtime_frame_scope(runtime, reason, "status-only");' in player
    assert "(unsigned)screen_bytes" in player
    assert "(unsigned long)screen_checksum" in player
    assert "(unsigned)screen_nonblank" in player
    assert "(unsigned)(byte_count == screen_bytes)" in player
    assert "(unsigned)iplay_runtime_video_cols(runtime)" in player
    assert "(unsigned)iplay_runtime_video_rows(runtime)" in player
    assert "(unsigned)iplay_runtime_video_mode_ok(runtime)" in player
    assert "(unsigned long)iplay_runtime_audio_frames_written(runtime)" in player
    assert "levels ? (unsigned)iplay_audio_levels_left_16(levels) : 0u" in player
    assert "levels ? (unsigned)iplay_audio_levels_right_16(levels) : 0u" in player
    assert "(void)scope;" in player
    assert 'player_present_runtime_frame(runtime, "status");' in player
    assert 'player_present_runtime_frame(runtime, "playback-position");' in player
    assert 'player_present_runtime_frame(runtime, "audio-unavailable");' in player
    assert "player_prime_runtime_playback(runtime, module, trial_block_limit);" in player
    assert "static void player_run_runtime_ui(IplayRuntime *runtime, IplayRuntimeConfig *runtime_config, const PlayerModuleInfo *module, const IplayModuleStatus *module_status, dd trial_block_limit)" in player
    assert "player_run_runtime_ui(runtime, runtime_config, module, module_status, trial_block_limit);" in player
    assert "static void player_prepare_loaded_module_ui(const PlayerModuleInfo *module, IplayModuleStatus *module_status)" in player
    assert "player_prepare_loaded_module_ui(module, module_status);" in player
    assert "static void player_present_loaded_module(IplayRuntime *runtime, IplayRuntimeConfig *runtime_config, const PlayerModuleInfo *module," in player
    assert "player_present_loaded_module(runtime, runtime_config, module, module_status, trial_block_limit);" in player
    assert "static void player_prepare_loaded_module_no_ui(void)" in player
    assert "static void player_present_loaded_module_no_ui(const PlayerModuleInfo *module)" in player
    assert "player_prepare_loaded_module_no_ui();" in player
    assert "player_present_loaded_module_no_ui(module);" in player
    assert "static void player_run_loaded_module(const PlayerModuleInfo *module, IplayRuntime *runtime, IplayRuntimeConfig *runtime_config," in player
    assert "static int player_run_prepared_module(const PlayerModuleInfo *module, IplayRuntime *runtime, IplayRuntimeConfig *runtime_config," in player
    assert "player_run_loaded_module(module, runtime, runtime_config, module_status, trial_block_limit);" in player
    assert "static void player_prepare_loaded_module_ui(IplayRegs *r" not in player
    assert "static void player_prepare_loaded_module_no_ui(IplayRegs *r)" not in player
    assert "static void player_run_loaded_module(IplayRegs *r" not in player
    assert "static int player_run_prepared_module(IplayRegs *r" not in player
    assert player.count("return player_exit_ok_status();") >= 3
    assert "return IPLAY_PLAYER_EXIT_OK;\n}\n#else\nstatic int player_run_prepared_module" not in player
    assert "exit_status = player_run_prepared_module(&module, &runtime, &runtime_config, &module_status, player_module_request_trial_block_limit(request));" in player
    assert "iplay_module_status_init(module_status" in player
    assert "iplay_module_status_set_type(module_status, player_module_type_tag(module));" in player
    assert "iplay_module_status_init(&module_status" not in player
    assert "iplay_module_status_set_type(&module_status" not in player
    assert "iplay_runtime_draw_module_status_struct(&runtime, &module_status)" not in player
    assert "iplay_runtime_draw_module_tag_struct(&runtime, &module_status)" not in player
    assert "iplay_runtime_draw_status_block(runtime, module_status)" in player
    assert "module_status.module_type =" not in player
    assert "iplay_runtime_present(runtime" in player
    smoke_script = (ROOT / "rewrite" / "smoke_player.sh").read_text()
    assert "Screen present: reason=status scope=full-screen bytes=.* screen_bytes=.* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* full=1 cols=.* rows=.* mode_ok=1 audio_frames=0 levels=" in smoke_script
    assert "Screen present: reason=playback-position scope=full-screen bytes=.* screen_bytes=.* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* full=1 cols=.* rows=.* mode_ok=1 audio_frames=16384 levels=" in smoke_script
    assert "Screen present: reason=post-playback-status scope=status-only bytes=.* screen_bytes=.* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* full=1 cols=.* rows=.* mode_ok=1 audio_frames=16384 levels=" in smoke_script
    assert "iplay_runtime_draw_audio_levels(&runtime" not in player
    assert "iplay_runtime_draw_audio_status(&runtime)" not in player
    assert "iplay_runtime_draw_audio_status(runtime);" in player
    assert 'streq(argv[1], "playerplaybacklevelshw")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "unsigned long text_checksum;" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "unsigned text_nonblank;" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "hw_capture.text_checksum = iplay_text_cells_checksum(bytes, byte_count);" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "hw_capture.text_nonblank = iplay_text_cells_nonblank_count(bytes, byte_count);" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "text_checksum=%lu text_nonblank=%u" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'streq(argv[1], "playermodulekeyboardstophw")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'streq(argv[1], "plhw25")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'streq(argv[1], "plhw40")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'streq(argv[1], "plhw8b")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert 'streq(argv[1], "plhw50")' in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "run_loaded_module_hw_path_mode(IPLAY_TEXT_DEFAULT_VIDEO_MODE);" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "run_loaded_module_hw_path_mode(IPLAY_VIDEO_MODE_40X25_BW);" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "run_loaded_module_hw_path_mode(IPLAY_VIDEO_MODE_80X25_BW);" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert "run_loaded_module_hw_path_mode(IPLAY_VIDEO_MODE_80X50_PROJECT);" in (ROOT / "rewrite" / "player_hw_runner.c").read_text()
    assert '"playerplaybacklevelshw"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '"playermodulekeyboardstophw"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '"plhw25"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '"plhw40"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '"plhw8b"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert '"plhw50"' in (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    assert "iplay_runtime_audio(&runtime)" not in player
    assert "iplay_audio_output_" not in player
    assert "iplay_sdl_audio_device_" not in player
    assert "IplayAudioOutput" not in player
    assert "IplaySdlAudioDevice" not in player
    assert "IPLAY_RUNTIME_STATUS_TITLE_ROW" not in player
    assert "IPLAY_RUNTIME_STATUS_MODULE_ROW" not in player
    assert "IPLAY_RUNTIME_STATUS_SIZE_ROW" not in player
    assert "IPLAY_RUNTIME_STATUS_LOADER_ROW" not in player
    assert "IPLAY_RUNTIME_STATUS_AUDIO_ROW" not in player
    assert "IPLAY_RUNTIME_STATUS_HARDWARE_ROW" not in player
    assert "IPLAY_RUNTIME_STATUS_VIDEO_ROW" not in player
    assert "IPLAY_RUNTIME_STATUS_TAG_ROW" not in player
    assert "IPLAY_RUNTIME_STATUS_TITLE_ATTR" not in player
    assert "IPLAY_RUNTIME_STATUS_LABEL_ATTR" not in player
    assert "IPLAY_RUNTIME_STATUS_VALUE_ATTR" not in player
    assert "IPLAY_RUNTIME_STATUS_TITLE_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_MODULE_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_SIZE_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_LOADER_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_AUDIO_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_HARDWARE_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_VIDEO_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_LEVELS_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_TAG_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_PLAYBACK_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_PANEL_ROW" in rewrite
    assert "IPLAY_RUNTIME_STATUS_PANEL_HEIGHT" in rewrite
    assert "IPLAY_RUNTIME_STATUS_PANEL_ATTR" in rewrite
    assert "IPLAY_RUNTIME_STATUS_PANEL_FILL_ATTR" in rewrite
    assert "static void iplay_runtime_draw_status_panel(IplayRuntime *runtime)" in rewrite
    assert "iplay_window_box_yx(&root," in rewrite
    assert "static void iplay_runtime_status_content_window(IplayRuntime *runtime, IplayWindow *window)" in rewrite
    assert "iplay_window_init_subwindow(window," in rewrite
    assert "iplay_runtime_draw_status_panel(runtime);" in rewrite
    assert "IPLAY_RUNTIME_STATUS_LEVELS_X" in rewrite
    assert "IPLAY_RUNTIME_STATUS_LEVELS_WIDTH" in rewrite
    assert '"Playing in Stereo, Free", "482KB"' in rewrite
    assert '"Samples Used  ", "0/15"' in rewrite
    assert '"Output Levels ", ""' in rewrite
    assert '"Main Volume   ", " 100%      - +"' in rewrite
    assert '"24bit Interpolation      F-12"' in rewrite
    assert "static const char *iplay_runtime_module_type_text(dd module_type)" in rewrite
    assert "IPLAY_RUNTIME_TAG4('S', '3', 'M', ' '): return \"S3M\";" in rewrite
    assert '"Module Type   ", iplay_runtime_module_type_text(iplay_module_status_type(status))' in rewrite
    assert '"Filename      ", module_path' in rewrite
    assert '"Current Track ", "1/0"' in rewrite
    assert '"Track Position", "1/64"' in rewrite
    assert "IPLAY_RUNTIME_STATUS_TITLE_ATTR" in rewrite
    assert "IPLAY_RUNTIME_STATUS_LABEL_ATTR" in rewrite
    assert "IPLAY_RUNTIME_STATUS_VALUE_ATTR" in rewrite
    assert "IPLAY_RUNTIME_STATUS_PLAYBACK_ATTR" in rewrite
    assert '"Loader", loader->symbol' not in player
    assert "iplay_runtime_audio_status_text(&runtime)" not in player
    assert '"Audio", iplay_runtime_audio_backend_name(&runtime)' not in player
    assert '"Hardware", (dd)iplay_runtime_audio_hardware_enabled(&runtime)' not in player
    assert "IplayPlayerUi" not in player
    assert "iplay_player_ui_" not in player
    assert "IplayTextScreen" not in player
    assert "IplayAudioOutput" not in player
    assert "iplay_runtime_audio(&runtime)->" not in runner
    assert "iplay_runtime_notcurses(&runtime)" not in runner
    assert "iplay_notcurses_mode(iplay_runtime_notcurses" not in runner
    assert "iplay_notcurses_stdplane(iplay_runtime_notcurses" not in runner
    assert "iplay_terminal_screen(iplay_notcurses_terminal(iplay_runtime_notcurses" not in runner
    assert "iplay_sdl_audio_device_backend(iplay_runtime_audio" not in runner
    assert "iplay_sdl_audio_device_frames_written(iplay_runtime_audio" not in runner
    assert "(unsigned)spec.hardware_enabled" not in runner
    assert "audio_spec.backend" not in runner
    assert "audio_spec.hardware_enabled" not in runner
    assert "audio_spec.format" not in runner
    assert "iplay_sdl_audio_spec_backend(&spec)" in runner
    assert "iplay_sdl_audio_spec_hardware_enabled(&spec)" in runner
    assert "iplay_sdl_audio_spec_sample_rate(&spec)" in runner
    assert "iplay_sdl_audio_spec_backend(&audio_spec)" in runner
    assert "iplay_sdl_audio_spec_hardware_enabled(&audio_spec)" in runner
    assert "iplay_sdl_audio_spec_format(&audio_spec)" in runner
    assert "iplay_sdl_audio_spec_sample_rate(&audio_spec)" in runner
    assert "iplay_hex4_to_buffer(mem, &off" in runner
    assert "iplay_hex8_to_buffer(mem, &off" in runner
    assert "iplay_hex16_to_buffer(mem, &off" in runner
    assert "iplay_hex32_to_buffer(mem, &off" in runner
    assert runner.count("iplay_hex4_to_buffer(mem, &off") >= 2
    assert runner.count("iplay_hex8_to_buffer(mem, &off") >= 2
    assert runner.count("iplay_hex16_to_buffer(mem, &off") >= 2
    assert runner.count("iplay_hex32_to_buffer(mem, &off") >= 2
    assert "iplay_u8_decimal_to_buffer(mem, &off" in runner
    assert "iplay_u16_decimal_to_buffer(mem, &off" in runner
    assert "iplay_u32_decimal_to_buffer(mem, &off" in runner
    assert "iplay_u32_base_to_buffer(mem, &off" in runner
    assert "iplay_u32_decimal_fill_to_buffer(mem, &off" in runner
    assert "iplay_i8_decimal_to_buffer(mem, &off" in runner
    assert "iplay_i16_decimal_to_buffer(mem, &off" in runner
    assert "iplay_i32_decimal_to_buffer(mem, &off" in runner
    assert runner.count("iplay_u8_decimal_to_buffer(mem, &off") >= 2
    assert runner.count("iplay_u16_decimal_to_buffer(mem, &off") >= 2
    assert runner.count("iplay_u32_decimal_to_buffer(mem, &off") >= 2
    assert runner.count("iplay_i8_decimal_to_buffer(mem, &off") >= 2
    assert "iplay_my_u32toa(&r, mem, base)" not in runner
    assert "iplay_my_u32toa_fill(&r, mem" not in runner
    assert "iplay_put_counted_char_to_buffer(mem, &off" in runner
    assert runner.count("iplay_put_counted_char_to_buffer(mem, &off") >= 2
    assert "iplay_string_length_at(mem, BUF_OFF)" in runner
    assert "iplay_string_length_at(mem, DSEG_SCRATCH)" in runner
    assert "iplay_strcpy_count_to_buffer(mem, mem, BUF_OFF, DST_OFF)" in runner
    assert "iplay_strcpy_count_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40)" in runner
    assert "iplay_copy_printable_to_buffer(mem, mem, BUF_OFF, DST_OFF" in runner
    assert "iplay_copy_printable_padded_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40" in runner
    assert "iplay_copy_attributed_fixed_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40" in runner
    assert "iplay_put_attributed_message_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40" in runner
    assert "iplay_put_controlled_attributed_text_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40" in runner
    assert "iplay_message_1be77_to_buffer(mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x500" in runner
    assert "iplay_myasmsprintf_to_buffer(mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40" in runner
    assert "iplay_write_screen_stream_to_buffer(mem, mem, DSEG_SCRATCH, DSEG_SCRATCH + 0x40" in runner
    assert "iplay_recolor_text_row(mem, &IPLAY_TEXT_MODE_80X25" in runner
    assert "iplay_sb16_probe_no_device_to_buffer(mem + DSEG_SCRATCH" in runner
    assert "iplay_sb16_init_fail_to_buffer(mem + DSEG_SCRATCH" in runner
    assert "IplaySb16RegsResult init_result = iplay_sb16_init_fail_to_buffer(scratch);" in rewrite
    assert "IplaySb16RegsResult result = iplay_sb16_dma_fail_to_buffer(scratch);" in rewrite
    assert "IplaySb16RegsResult result = iplay_sb16_off_no_device_to_buffer(scratch, symbol);" in rewrite
    assert "apply_full_regs6(r, 0, 0x5678, 0, 0x0ff6" not in rewrite
    assert "iplay_sb16_int_ack_to_buffer(mem + DSEG_SCRATCH" in runner
    assert "iplay_sb16_dma_fail_to_buffer(mem + DSEG_SCRATCH" in runner
    assert "iplay_sb16_off_no_device_to_buffer(mem + DSEG_SCRATCH" in runner
    assert "iplay_sb_helper_no_device_result(symbol, base_port" in runner
    assert "iplay_set_dmachn_mask_no_device_result(channel" in runner
    assert "iplay_adlib_delay_no_device_result(symbol" in runner
    assert "iplay_sb_legacy_init_no_device_result(globals" in runner
    assert "iplay_sb_detect_irq_no_device_result(0x1234" in runner
    assert "iplay_sb_test_interrupt_no_device_result(&counter" in runner
    assert runner.count("iplay_set_current_text_video_mode(video_mode)") >= 3
    assert "iplay_setvideomode_no_hw(&r, mem);" not in runner
    assert "iplay_mystrlen(&r, mem)" not in runner
    assert "iplay_strcpy_count(&r, mem, mem)" not in runner
    assert "iplay_copy_printable(&r, mem, mem)" not in runner
    assert "iplay_seg1_copy_printable(&r, mem, mem)" not in runner
    assert "iplay_txt_1abae(&r, mem, mem)" not in runner
    assert "iplay_put_message(&r, mem, mem" not in runner
    assert "iplay_text_1bf69(&r, mem, mem)" not in runner
    assert "iplay_message_1be77(&r, mem, DSEG_SCRATCH)" not in runner
    assert "iplay_myasmsprintf(&r, mem)" not in runner
    assert "iplay_write_scr(&r, mem, mem)" not in runner
    assert "iplay_recolor_txt(&r, mem)" not in runner
    assert "iplay_sb16_probe_no_device(&r, mem + DSEG_SCRATCH" not in runner
    assert "iplay_sb16_init_fail(&r, mem + DSEG_SCRATCH)" not in runner
    assert "iplay_sb16_int_ack(&r, mem + DSEG_SCRATCH)" not in runner
    assert "iplay_sb16_dma_fail(&r, mem + DSEG_SCRATCH)" not in runner
    assert "iplay_sb16_off_no_device(&r, mem + DSEG_SCRATCH" not in runner
    assert "iplay_sb_helper_no_device(&r, symbol, base_port)" not in runner
    assert "iplay_set_dmachn_mask_no_device(&r, channel)" not in runner
    assert "iplay_adlib_delay_no_device(&r, symbol)" not in runner
    assert "iplay_sb_legacy_init_no_device(&r, globals" not in runner
    assert "iplay_sb_detect_irq_no_device(&r)" not in runner
    assert "iplay_sb_test_interrupt_no_device(&r, &counter)" not in runner
    assert "iplay_sub_19050_bounded(&r, globals)" not in runner
    assert "iplay_memfill8080(&r, dma)" not in runner
    assert "iplay_sndoff_fill(&r, dma, symbol)" not in runner
    assert "iplay_get_playsettings(&r" not in runner
    assert "iplay_getset_playstate(&r" not in runner
    assert "iplay_get_12f7c(&r" not in runner
    assert "iplay_read_sndsettings(" not in runner
    assert "iplay_snd_guard(&r, globals, snd_op)" not in runner
    assert "iplay_set_playsettings(&r, globals" not in runner
    assert "iplay_volume_12a66(&r" not in runner
    assert "iplay_vlm_141df(&r, globals" not in runner
    assert "iplay_change_volume(&r, globals" not in runner
    assert "iplay_sub_12b83(&r, globals" not in runner
    assert "iplay_sub_12d05(&r, dst" not in runner
    assert "iplay_sub_12d35_disable(&r, &code_byte)" not in runner
    assert "iplay_sub_12da8_guard(&r, globals)" not in runner
    assert "iplay_sub_13623_guard(&r" not in runner
    assert "iplay_sub_12cad_guard(&r, event_store" not in runner
    assert "iplay_sub_1281a_small(&r, dst" not in runner
    assert "iplay_volume_prep_inactive(&r, globals" not in runner
    assert "iplay_sub_1609f_disabled(&r, dst" not in runner
    behavior = (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "def wrapper_sub_154f4_direct(" in behavior
    assert 'assert field(got, "bx") == field(out, "bx")' in behavior
    assert 'assert field(got, "cx") == field(out, "cx")' in behavior
    assert 'assert field(got, "si") == field(out, "si")' in behavior
    assert "(0x0FF0, 0x10, 0x00000F80, 0x1201, 0x7F, 0x4000, 0x3434)" in behavior
    assert "(0x0000, 0x00, 0x00FFF123, 0x00F0, 0x00, 0x5000, 0x7856)" in behavior
    assert 'printf("bx=%04x cx=%04x si=%04x data="' in runner
    assert "iplay_fill_dma_inactive_mono(&r, mem" not in runner
    assert "iplay_configure_timer_bounded(&r, globals" not in runner
    assert "iplay_fill_dma_small(&r, mem" not in runner
    assert "iplay_memclean(&r, mem" not in runner
    assert "no-hardware playback status" not in status
    assert "no-hardware playback-disabled" not in status
    assert "Runtime facade accessors now cover common text geometry" in status
    for symbol in [
        "IPLAY_TEXT_MODE_40X25",
        "IPLAY_TEXT_MODE_80X25",
        "IPLAY_TEXT_MODE_80X50",
        "iplay_text_mode_for_size",
        "iplay_text_default_mode",
        "iplay_text_fallback_mode",
        "iplay_text_supported_mode_count",
        "iplay_text_size_is_supported",
        "iplay_text_mode_is_supported",
        "iplay_text_mode_cols",
        "iplay_text_mode_rows",
        "iplay_text_max_screen_bytes",
        "iplay_text_screen_init",
        "iplay_text_screen_init_capacity",
        "iplay_text_screen_resize",
        "iplay_text_screen_resize_checked",
        "iplay_text_screen_resize_to_size",
        "iplay_text_screen_resize_to_size_checked",
        "iplay_text_screen_can_resize",
        "iplay_text_screen_capacity",
        "iplay_text_screen_set_video_mode",
        "iplay_text_screen_set_video_mode_checked",
        "iplay_text_screen_root",
        "iplay_text_screen_mode",
        "iplay_text_screen_bottom_layout",
        "iplay_text_screen_bottom_layout_fits",
        "iplay_text_screen_draw_top_title",
        "iplay_text_screen_draw_bottom",
        "iplay_text_screen_draw_audio_output_levels",
        "IplayTerminal",
        "IplayTerminalBackend",
        "IplayNotcurses",
        "IplayVideoPresentFn",
        "IPLAY_TERMINAL_BACKEND_VGA_MEMORY",
        "iplay_terminal_init_vga_memory",
        "iplay_terminal_set_present_callback",
        "iplay_terminal_backend",
        "iplay_terminal_screen",
        "iplay_terminal_root",
        "iplay_terminal_mode",
        "iplay_terminal_resize_to_size",
        "iplay_terminal_resize_to_size_checked",
        "iplay_terminal_set_video_mode",
        "iplay_terminal_present",
        "iplay_terminal_erase",
        "iplay_terminal_draw_top_title",
        "iplay_terminal_draw_bottom",
        "iplay_terminal_draw_audio_output_levels",
        "iplay_video_spec_backend",
        "iplay_video_spec_mode",
        "iplay_video_spec_cols",
        "iplay_video_spec_rows",
        "iplay_video_spec_present_enabled",
        "iplay_notcurses_init_vga_memory",
        "iplay_notcurses_terminal",
        "iplay_notcurses_stdplane",
        "iplay_notcurses_mode",
        "iplay_notcurses_capacity",
        "iplay_notcurses_cols",
        "iplay_notcurses_rows",
        "iplay_notcurses_row_bytes",
        "iplay_notcurses_screen_bytes",
        "iplay_notcurses_bottom_layout_fits",
        "iplay_notcurses_video_spec",
        "iplay_notcurses_backend",
        "iplay_notcurses_present_enabled",
        "iplay_notcurses_has_present",
        "iplay_notcurses_present_callback",
        "iplay_notcurses_present_user",
        "iplay_notcurses_set_present_fn",
        "iplay_notcurses_set_present_user",
        "iplay_notcurses_set_present_callback",
        "iplay_notcurses_clear_present_callback",
        "iplay_notcurses_resize",
        "iplay_notcurses_resize_to_size",
        "iplay_notcurses_resize_to_size_checked",
        "iplay_notcurses_set_video_mode",
        "iplay_notcurses_render_static",
        "iplay_notcurses_render_bottom",
        "iplay_notcurses_draw_audio_output_levels",
        "iplay_notcurses_present",
        "iplay_text_supported_mode",
        "iplay_text_mode_row_bytes",
        "iplay_text_mode_cells",
        "iplay_text_mode_screen_bytes",
        "iplay_text_mode_equals",
        "iplay_start_player_memory",
        "iplay_hex4_to_buffer",
        "iplay_hex8_to_buffer",
        "iplay_hex16_to_buffer",
        "iplay_hex32_to_buffer",
        "IplayDecimalResult",
        "iplay_u8_decimal_to_buffer",
        "iplay_u16_decimal_to_buffer",
        "iplay_u32_decimal_to_buffer",
        "iplay_u32_base_to_buffer",
        "iplay_u32_decimal_fill_to_buffer",
        "iplay_i8_decimal_to_buffer",
        "iplay_i16_decimal_to_buffer",
        "iplay_i32_decimal_to_buffer",
        "iplay_put_counted_char_to_buffer",
        "IplayStringCopyResult",
        "iplay_string_length_at",
        "iplay_strcpy_count_to_buffer",
        "iplay_copy_printable_to_buffer",
        "iplay_copy_printable_padded_to_buffer",
        "IplayAttributedTextResult",
        "iplay_copy_attributed_fixed_to_buffer",
        "iplay_put_attributed_message_to_buffer",
        "iplay_put_controlled_attributed_text_to_buffer",
        "iplay_message_1be77_to_buffer",
        "IplayAsmSprintfResult",
        "iplay_myasmsprintf_to_buffer",
        "IplayScreenStreamResult",
        "iplay_write_screen_stream_to_buffer",
        "IplayRecolorResult",
        "iplay_recolor_text_row",
        "IplaySb16ProbeResult",
        "iplay_sb16_probe_no_device_to_buffer",
        "IplaySb16RegsResult",
        "iplay_sb16_init_fail_to_buffer",
        "iplay_sb16_int_ack_to_buffer",
        "iplay_sb16_dma_fail_to_buffer",
        "iplay_sb16_off_no_device_to_buffer",
        "iplay_sb_helper_no_device_result",
        "iplay_sb_write_no_device_state",
        "IplayRegs6Result",
        "iplay_set_dmachn_mask_no_device_result",
        "iplay_adlib_delay_no_device_result",
        "iplay_sb_legacy_init_no_device_result",
        "iplay_sb_detect_irq_no_device_result",
        "iplay_sb_test_interrupt_no_device_result",
        "iplay_sb_handler_int_bounded_state",
        "iplay_sub_19050_bounded_result",
        "iplay_memfill8080_result",
        "iplay_sndoff_fill_result",
        "IplaySndSettingsResult",
        "iplay_get_playsettings_eax",
        "iplay_getset_playstate_eax",
        "iplay_get_12f7c_result",
        "iplay_read_sndsettings_result",
        "iplay_snd_guard_state",
        "iplay_set_playsettings_result",
        "iplay_volume_12a66_result",
        "iplay_vlm_141df_result",
        "iplay_change_volume_result",
        "iplay_sub_12b83_state",
        "iplay_sub_12d05_to_buffer",
        "iplay_sub_12d35_disable_code",
        "iplay_sub_12da8_guard_state",
        "iplay_sub_12cad_guard_result",
        "iplay_sub_13623_guard_result",
        "iplay_sub_1281a_small_result",
        "iplay_volume_prep_inactive_result",
        "iplay_sub_1609f_disabled_result",
        "iplay_fill_dma_inactive_mono_result",
        "iplay_configure_timer_bounded_result",
        "iplay_fill_dma_small_result",
        "iplay_memclean_result",
        "iplay_calc_14043_ax",
        "iplay_sub_13d95_result",
        "iplay_sub_13cf6_result",
        "iplay_eff_13ce8_state",
        "iplay_eff_14030_result",
        "iplay_eff_14067_result",
        "iplay_eff_13a43_state",
        "iplay_eff_13b78_al",
        "iplay_eff_13b88_result",
        "iplay_eff_13ca2_eax",
        "iplay_eff_13cc9_eax",
        "iplay_eff_13cdd_state",
        "iplay_eff_13ad7_result",
        "iplay_eff_13b06_ax",
        "iplay_eff_13bb2_state",
        "iplay_eff_13ba3_result",
        "iplay_eff_13bc8_result",
        "iplay_eff_13c02_eax",
        "iplay_eff_13c3f_result",
        "iplay_eff_13c64_result",
        "iplay_eff_13c88_result",
        "iplay_eff_13c95_result",
        "iplay_eff_13cb3_state",
        "iplay_sub_14087_result",
        "iplay_eff_13de5_result",
        "iplay_eff_13def_result",
        "iplay_eff_13e32_result",
        "iplay_eff_slide_step_eax",
        "iplay_eff_13e1e_eax",
        "iplay_vibrato_eax",
        "iplay_eff_13e2d_eax",
        "iplay_eff_13e7f_result",
        "iplay_eff_13e84_result",
        "iplay_eff_13e8c_result",
        "iplay_eff_13f05_eax",
        "iplay_eff_13f3b_result",
        "iplay_change_amplif_eax",
        "iplay_eff_14020_eax",
        "iplay_eff_13886_eax",
        "iplay_eff_138a4_eax",
        "iplay_eff_1387f_eax",
        "iplay_eff_1389d_eax",
        "iplay_sub_13826_result",
        "iplay_eff_13fbe_result",
        "iplay_eff_138d2_eax",
        "iplay_eff_1392f_eax",
        "iplay_eff_139ac_result",
        "iplay_eff_139b2_result",
        "iplay_eff_139b9_eax",
        "iplay_ncplane_init_mode",
        "iplay_text_mode_for_video_mode",
        "iplay_text_current_mode",
        "iplay_set_current_text_video_mode",
        "IplayTextColor",
        "iplay_text_attr",
        "iplay_text_attr_fg",
        "iplay_text_attr_bg",
        "iplay_text_attr_blink",
        "iplay_ncplane_subplane",
        "iplay_ncplane_resize",
        "iplay_ncplane_origin_yx",
        "iplay_ncplane_rows",
        "iplay_ncplane_cols",
        "iplay_ncplane_cursor_yx",
        "iplay_ncplane_cursor_move_yx",
        "iplay_ncplane_putc",
        "iplay_ncplane_vline_yx",
        "iplay_ncplane_meter16_yx",
        "iplay_audio_levels_draw_yx",
        "iplay_window_init_root",
        "iplay_window_init_subwindow",
        "iplay_window_plane",
        "iplay_window_resize",
        "iplay_window_origin_yx",
        "iplay_window_rows",
        "iplay_window_cols",
        "iplay_window_erase",
        "iplay_window_fill_yx",
        "iplay_window_box_yx",
        "iplay_window_cursor_yx",
        "iplay_window_cursor_move_yx",
        "iplay_window_putc",
        "iplay_window_putstr",
        "iplay_window_putnstr",
        "iplay_window_putnstr_fill_yx",
        "iplay_window_scroll_up",
        "iplay_window_scroll_down",
        "iplay_window_draw_audio_levels",
        "iplay_window_draw_status_line",
        "iplay_window_draw_status_field",
        "iplay_window_draw_status_u32",
        "iplay_window_draw_status_hex32",
        "iplay_ncplane_fill_yx",
        "iplay_ncplane_erase",
        "iplay_ncplane_box_yx",
        "iplay_ncplane_scroll_up",
        "iplay_ncplane_scroll_down",
        "iplay_ncplane_putstr",
        "iplay_ncplane_putnstr",
        "iplay_ncplane_putnstr_yx",
        "iplay_ncplane_putnstr_fill_yx",
        "iplay_ncplane_putnstr_fill",
        "iplay_notcurses_set_video_mode_checked",
        "IplayAudioOutput",
        "iplay_audio_output_init",
        "iplay_audio_output_init_sb16_stereo",
        "iplay_audio_sink_format",
        "iplay_audio_sink_bytes_per_frame",
        "iplay_audio_output_start",
        "iplay_audio_output_stop",
        "iplay_audio_output_is_active",
        "iplay_audio_output_reset_counters",
        "iplay_audio_output_set_capacity",
        "iplay_audio_output_add_capacity",
        "iplay_audio_output_capacity",
        "iplay_audio_output_frames_written",
        "iplay_audio_output_underrun_frames",
        "iplay_audio_output_dropped_frames",
        "iplay_audio_output_is_sb16_stereo",
        "iplay_audio_output_levels",
        "iplay_audio_output_reset_levels",
        "iplay_audio_output_write_mixer_frames",
        "iplay_audio_output_write_sb16_frames",
        "iplay_audio_output_write_silence",
        "iplay_audio_output_source_format",
        "iplay_audio_output_sink_format",
        "iplay_audio_output_bytes_per_frame",
        "IplayAudioLevels",
        "IplayAudioBackend",
        "IplaySdlAudioDevice",
        "IplayRuntime",
        "IplayRuntimeConfig",
        "IPLAY_AUDIO_BACKEND_SB16_STEREO",
        "IPLAY_AUDIO_BACKEND_SDL_COMPATIBLE",
        "iplay_audio_level_to_16",
        "iplay_audio_sb16_stereo_levels",
        "iplay_audio_backend_name",
        "iplay_sdl_audio_spec_backend",
        "iplay_sdl_audio_spec_backend_name",
        "iplay_sdl_audio_spec_format",
        "iplay_sdl_audio_spec_sample_rate",
        "iplay_sdl_audio_spec_bits_per_sample",
        "iplay_sdl_audio_spec_channels",
        "iplay_sdl_audio_spec_signed_samples",
        "iplay_sdl_audio_spec_hardware_enabled",
        "iplay_sdl_audio_spec_is_sb16_compatible",
        "iplay_sdl_audio_spec_is_sb16_hardware",
        "iplay_sdl_audio_spec_is_sdl_compatible",
        "iplay_audio_output_draw_levels_yx",
        "iplay_sdl_audio_device_init_sb16_compatible",
        "iplay_sdl_audio_device_backend",
        "iplay_sdl_audio_device_backend_name",
        "iplay_sdl_audio_device_output",
        "iplay_sdl_audio_device_format",
        "iplay_sdl_audio_device_sample_rate",
        "iplay_sdl_audio_device_bits_per_sample",
        "iplay_sdl_audio_device_channels",
        "iplay_sdl_audio_device_signed_samples",
        "iplay_sdl_audio_device_bytes_per_frame",
        "iplay_sdl_audio_device_is_sb16_compatible",
        "iplay_sdl_audio_device_is_sb16_hardware",
        "iplay_sdl_audio_device_is_sdl_compatible",
        "iplay_sdl_audio_device_hardware_enabled",
        "iplay_sdl_audio_device_status_text",
        "iplay_sdl_audio_device_start",
        "iplay_sdl_audio_device_stop",
        "iplay_sdl_audio_device_active",
        "iplay_sdl_audio_device_pause",
        "iplay_sdl_audio_device_paused",
        "iplay_sdl_audio_device_reset_counters",
        "iplay_sdl_audio_device_set_capacity",
        "iplay_sdl_audio_device_add_capacity",
        "iplay_sdl_audio_device_clear_queued",
        "iplay_sdl_audio_device_capacity",
        "iplay_sdl_audio_device_frames_written",
        "iplay_sdl_audio_device_underrun_frames",
        "iplay_sdl_audio_device_dropped_frames",
        "iplay_sdl_audio_device_queued_frames",
        "iplay_sdl_audio_device_queued_bytes",
        "iplay_sdl_audio_device_callback",
        "iplay_sdl_audio_device_queue",
        "iplay_sdl_audio_device_queue_frames",
        "iplay_sdl_audio_device_write_sb16_frames",
        "iplay_sdl_audio_device_write_silence",
        "iplay_sdl_audio_device_levels",
        "iplay_sdl_audio_device_reset_levels",
        "iplay_runtime_init_vga_sb16",
        "iplay_runtime_init_vga_sdl_audio",
        "iplay_runtime_init_vga_sb16_present",
        "iplay_runtime_init_callbacks",
        "iplay_runtime_init_callbacks_capacity",
        "iplay_runtime_init_config",
        "iplay_runtime_start_config",
        "iplay_runtime_start_config_checked",
        "iplay_runtime_config_no_hardware",
        "iplay_runtime_config_no_hardware_capacity",
        "iplay_runtime_config_sdl",
        "iplay_runtime_config_sdl_capacity",
        "iplay_runtime_config_has_video_present",
        "iplay_runtime_config_has_audio_sink",
        "iplay_runtime_config_has_cell_capacity",
        "iplay_runtime_config_error",
        "iplay_runtime_config_error_name",
        "iplay_runtime_config_is_valid",
        "iplay_runtime_shutdown",
        "iplay_runtime_notcurses",
        "iplay_runtime_stdplane",
        "iplay_runtime_video_spec",
        "iplay_runtime_video_backend",
        "iplay_runtime_video_present_enabled",
        "iplay_runtime_video_has_present",
        "iplay_runtime_video_present_callback",
        "iplay_runtime_video_present_user",
        "iplay_runtime_video_set_present_fn",
        "iplay_runtime_video_set_present_user",
        "iplay_runtime_video_set_present_callback",
        "iplay_runtime_video_clear_present_callback",
        "iplay_runtime_video_mode",
        "iplay_runtime_video_cells_const",
        "iplay_runtime_video_checksum",
        "iplay_runtime_video_nonblank_cells",
        "iplay_runtime_video_capacity",
        "iplay_runtime_video_cols",
        "iplay_runtime_video_rows",
        "iplay_runtime_video_row_bytes",
        "iplay_runtime_video_screen_bytes",
        "iplay_runtime_bottom_layout_fits",
        "iplay_runtime_audio",
        "iplay_runtime_audio_spec",
        "iplay_runtime_audio_backend",
        "iplay_runtime_audio_format",
        "iplay_runtime_audio_sample_rate",
        "iplay_runtime_audio_bits_per_sample",
        "iplay_runtime_audio_channels",
        "iplay_runtime_audio_signed_samples",
        "iplay_runtime_audio_samples",
        "iplay_runtime_audio_backend_name",
        "iplay_runtime_audio_active",
        "iplay_runtime_audio_hardware_enabled",
        "iplay_runtime_audio_status_text",
        "iplay_runtime_audio_bytes_per_frame",
        "iplay_runtime_audio_is_sb16_compatible",
        "iplay_runtime_audio_is_sb16_hardware",
        "iplay_runtime_audio_is_sdl_compatible",
        "iplay_runtime_resize",
        "iplay_runtime_resize_checked",
        "iplay_runtime_resize_to_size",
        "iplay_runtime_resize_to_size_checked",
        "iplay_runtime_set_video_mode",
        "iplay_runtime_set_video_mode_checked",
        "iplay_runtime_video_mode_ok",
        "iplay_runtime_video_status_text",
        "iplay_runtime_video_status_token",
        "iplay_runtime_render_static",
        "iplay_runtime_render_bottom",
        "iplay_runtime_audio_start",
        "iplay_runtime_audio_stop",
        "iplay_runtime_audio_pause",
        "iplay_runtime_audio_paused",
        "iplay_runtime_audio_reset_counters",
        "iplay_runtime_audio_set_capacity",
        "iplay_runtime_audio_add_capacity",
        "iplay_runtime_audio_clear_queued",
        "iplay_runtime_audio_capacity",
        "iplay_runtime_audio_frames_written",
        "iplay_runtime_audio_underrun_frames",
        "iplay_runtime_audio_dropped_frames",
        "iplay_runtime_audio_queued_frames",
        "iplay_runtime_audio_queued_bytes",
        "iplay_runtime_audio_can_queue",
        "iplay_runtime_audio_frames_for_bytes",
        "iplay_runtime_audio_bytes_for_frames",
        "iplay_runtime_audio_queue",
        "iplay_runtime_audio_queue_frames",
        "iplay_runtime_write_sb16_frames",
        "iplay_runtime_write_silence",
        "iplay_runtime_audio_levels",
        "iplay_runtime_audio_reset_levels",
        "iplay_runtime_draw_audio_levels",
        "iplay_runtime_draw_audio_status",
        "iplay_runtime_draw_video_status",
        "iplay_module_status_init",
        "iplay_module_status_title",
        "iplay_module_status_path",
        "iplay_module_status_size",
        "iplay_module_status_loader",
        "iplay_module_status_type",
        "iplay_module_status_set_type",
        "iplay_module_status_clear_type",
        "iplay_module_status_type_hex",
        "iplay_runtime_draw_module_status_struct",
        "iplay_runtime_draw_module_tag_struct",
        "iplay_runtime_draw_status_block",
        "iplay_runtime_draw_module_status",
        "iplay_runtime_draw_module_tag",
        "iplay_runtime_draw_status_line",
        "iplay_runtime_draw_status_field",
        "iplay_runtime_draw_status_u32",
        "iplay_runtime_draw_status_hex32",
        "iplay_runtime_present",
        "iplay_draw_frame_plane",
        "iplay_txt_draw_top_title_plane",
        "iplay_txt_draw_bottom_plane",
    ]:
        assert symbol in header
        assert symbol in rewrite


def test_required_original_scope_has_parity_tests() -> None:
    procs = set(original_procs())
    unknown_tested = sorted(TESTED_ORIGINAL_ENTRIES - procs - TESTED_NON_PROC_ENTRIES)
    assert not unknown_tested, f"tested inventory entries not found in IPLAY.lst: {', '.join(unknown_tested)}"
    unknown_excluded = sorted(NON_SB_AUDIO_DRIVER_ENTRIES - procs)
    assert not unknown_excluded, f"non-SB audio exclusions not found in IPLAY.lst: {', '.join(unknown_excluded)}"
    required = procs - NON_SB_AUDIO_DRIVER_ENTRIES
    missing = sorted(required - TESTED_ORIGINAL_ENTRIES)
    assert not missing, (
        f"{len(missing)} required original procs still need original/translated parity tests "
        f"(non-SB audio drivers excluded): {', '.join(missing)}"
    )


def test_original_ui_parity_gap_stays_explicit_until_capture_harness_exists() -> None:
    coverage = (ROOT / "tests" / "COVERAGE.md").read_text()
    behavior = (ROOT / "tests" / "test_player_behavior.py").read_text()
    function_parity = (ROOT / "tests" / "test_function_parity.py").read_text()
    assert "| Executable interface shape | original `IPLAY [Switches] [FileName.Ext|@FileList.Ext]` usage | rewritten DOS executable usage adapter |" in coverage
    assert "patched-original and rewrite help output now directly compare the stable usage and `/i` lines" in coverage
    assert "| Text screen rendering | original B800 text memory | rewritten text cells/present event |" in coverage
    assert "rewrite missing-file coverage also proves all supported text modes exit before screen-present, PCM source, decoder route/handoff, or playback pump evidence" in coverage
    assert "exact 4000-byte original-vs-rewrite parity exists for the corrupt-MOD cleanup/error frame" in coverage
    assert "valid `SMOKE.MOD` captures exist both at completed `text_init2` and immediately after the first `offs_draw` callback" in coverage
    assert "matches original coordinates and attributes for the title, filename" in coverage
    assert "preferred SDL player propagates libmodplug order/pattern/row and stateful per-channel note" in coverage
    assert "libmikmod-backed formats use real mixer-voice measurements" in coverage
    assert "exact raw magnitude equality remains incomplete" in coverage
    assert "`462KB`, `M.K.`, 4-channel, 2/31-sample, module-title" in coverage
    assert "driver header is intentionally adapted to SB16-only scope" in coverage
    assert "duplicate diagnostic panel is removed from 80-column frames" in coverage
    assert "40x25 BW/color, 80x25 BW/color, and 80x50" in coverage
    assert "DOS `IPLAYHW.EXE` unavailable-hardware coverage proves module load, external-library handoff, SB16 16-bit stereo hardware wrapper selection, `base=220h irq=5 dma16=5 rate=44100` config reporting, no PCM source, no playback pump, and zero presented audio frames across supported text modes" in coverage
    assert "assert_text_cell_span_at_original_location_equal" in behavior
    assert "test_iplaydiag_post_playback_status_screen_present_matches_b800_dump" in behavior
    assert "test_iplaydiag_post_playback_status_screen_present_matches_text_memory_dump_for_supported_modes" in behavior
    assert "test_iplayhw_audio_unavailable_screen_present_matches_text_memory_dump_for_supported_modes" in behavior
    assert "test_iplaydiag_unsupported_module_screen_present_matches_text_memory_dump_for_supported_modes" in behavior
    assert "test_iplaydiag_missing_module_does_not_present_stale_screen_or_playback_for_supported_modes" in behavior
    assert 'assert_playback_output(out, "SB16 16-bit stereo hardware wrapper enabled.")' in behavior
    assert 'assert "SB16 config: base=220h irq=5 dma16=5 rate=44100" in out' in behavior
    assert '("40x25bw", VGA_MONO_TEXT_SEG, 40, 25)' in behavior
    assert '("40x25color", VGA_COLOR_TEXT_SEG, 40, 25)' in behavior
    assert '("80x25bw", VGA_MONO_TEXT_SEG, 80, 25)' in behavior
    assert '("80x25color", VGA_COLOR_TEXT_SEG, 80, 25)' in behavior
    assert '("80x50", VGA_COLOR_TEXT_SEG, 80, 50)' in behavior
    assert 'assert "Playback pump:" not in out' in behavior
    assert 'assert "PCM source:" not in out' in behavior
    assert 'assert_unsupported_module(out, "BADMODE.MOD")' in behavior
    assert 'assert audio_unavailable_screen["audio_frames"] == 0' in behavior
    assert 'assert unsupported_screen["audio_frames"] == 0' in behavior
    assert 'assert_text_memory_matches_screen_present(' in behavior
    assert "assert rewrite_memory == original_cells" in behavior
    assert 'KVIKDOS_MEM_DUMP_START' in behavior
    assert "| UI/subwindows | original text memory snapshots | rewritten screen snapshots |" in coverage
    assert "original corrupt-module cleanup frame now has B800 geometry/nonblank comparison against the rewrite unsupported-module frame" in coverage
    assert "| Fast audio levels display | original level rendering | rewritten level rendering |" in coverage
    assert "Forcing a test-only `HLT` at the original cleanup exit now produces a dump that includes `B800:0000`" in coverage
    assert "Original `B800:0000` corrupt-module UI/module screen comparison has exact 4000-byte equality." in coverage
    assert "including exact 4000-byte original-vs-rewrite cell/attribute equality" in coverage
    assert "Stable valid-MOD capture points exist at original `text_init2` completion and after the first `offs_draw`" in coverage
    assert "Libmodplug now supplies pattern-command note, sample-name, and effect state to the preferred SDL player" in coverage
    assert "deterministic sparse MOD verifies that active channel state persists after playback advances beyond the command row" in coverage
    assert "Libmikmod-backed formats now supply real per-voice mixer measurements" in coverage
    assert "Exact whole-frame equality still needs active-voice timing reconciliation and calibration of libmikmod's peak-to-peak measurement" in coverage
    assert "loaded-module hardware path now asserts final SB16 speaker-off DSP write and idle SB16 state after 16 submitted hardware blocks across 40x25 color/BW, 80x25 BW, and 80x50 text modes" in coverage
    assert 'assert field(got, "last_write") == "22c:d5"' in function_parity
    assert 'assert field(got, "sb_blocks") == "16"' in function_parity
    assert "the emulator-level VGA dump blocker is removed" in coverage
    assert "--hlt-dump" in behavior
    assert "assert not dump.exists()" in behavior
    assert "test_original_iplay_forced_hlt_dump_captures_b800_text_aperture" in behavior
    assert "test_original_and_rewrite_live_mod_player_frame_share_text_cells_and_attributes" in behavior
    assert "ORIGINAL_PLAYBACK_UI_HLT_TRAP_FILE_OFFSET" in behavior
    assert 'rewrite_screen = parse_screen_present_digest(rewrite_out, "unsupported-module")' in behavior
    assert 'assert rewrite_screen["bytes"] == digest["bytes"] == 4000' in behavior
    assert 'rewrite_env["KVIKDOS_MEM_DUMP"] = str(rewrite_dump)' in behavior
    assert 'assert rewrite_digest["checksum"] == rewrite_screen["checksum"]' in behavior
    assert 'assert "Inertia Player V1.22" in original_text' in behavior
    assert 'assert "Inertia Player V1.22" in rewrite_text' in behavior
    assert 'assert "BAD.MOD" in original_text' in behavior
    assert 'assert "BAD.MOD" in rewrite_text' in behavior
    assert 'assert "Filename      : BAD.MOD" in original_text' in behavior
    assert 'assert "Filename      : BAD.MOD" in rewrite_text' in behavior
    assert 'assert "Module Type   : N.T." in rewrite_text' in behavior
    assert 'assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Filename      : BAD.MOD")' in behavior
    assert 'assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Module Type   : N.T.")' in behavior
    assert 'assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "CD Edition")' in behavior
    assert 'assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Playing in Stereo, Free: 482KB")' in behavior
    assert 'assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Main Volume   :  100%      - +")' in behavior
    assert 'assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Hope you liked using the Inertia Player")' in behavior
    assert 'assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Internet : sdanes@marvels.hacktic.nl")' in behavior
    assert 'assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Send email to listserver@oliver.sun.ac.za to subscribe to one or both of")' in behavior
    assert "assert_original_visible_row_cells_equal(original_cells, rewrite_memory, 80, 6)" in behavior
    assert "for row in range(8, 11):" in behavior
    assert "for row in range(13, 17):" in behavior
    assert "for row in range(25):" in behavior
    assert "assert_original_visible_row_cells_equal(original_cells, rewrite_memory, 80, row)" in behavior
    assert "assert rewrite_memory == original_cells" in behavior
    assert "assert_no_extra_rewrite_visible_text_on_original_blank_cells(original_cells, rewrite_memory, 80, 25)" in behavior
    assert "player_render_runtime_unsupported_module" in (ROOT / "rewrite" / "iplay_player.c").read_text()
    assert "player_original_module_type_label" in (ROOT / "rewrite" / "iplay_player.c").read_text()
    assert "player_original_filename_label" in (ROOT / "rewrite" / "iplay_player.c").read_text()
    assert "ORIGINAL_CLEANUP_EXIT_HLT_TRAP_FILE_OFFSET" in behavior
    assert 'dump.stat().st_size >= vga_text_linear + digest["bytes"]' in behavior
    kvikdos = Path("/home/xor/kvikdos/kvikdos.c").read_text()
    assert "#define GUEST_MEM_LIMIT 0xc0000" in kvikdos
    assert "region.memory_size = GUEST_MEM_LIMIT - GUEST_MEM_MODULE_START;" in kvikdos
    assert "write(fd, mem, GUEST_MEM_LIMIT)" in kvikdos
    assert "maybe_dump_guest_mem(mem, GUEST_MEM_LIMIT);" in kvikdos


def test_fallback_stream_start_coverage_stays_visible() -> None:
    coverage = (ROOT / "tests" / "COVERAGE.md").read_text()
    behavior = (ROOT / "tests" / "test_player_behavior.py").read_text()
    status = (ROOT / "rewrite" / "STATUS.md").read_text()
    assert "fallback stream-start behavior is same-path covered for S3M first-pattern data and MOD pattern data" in coverage
    assert "DOS diagnostics also report `stream_start=...` for complete and capped-header file-path modules" in coverage
    assert "capped S3M file-path playback is covered across the 496-byte DOS file-stream refill boundary" in coverage
    assert "test_iplayc_dos_s3m_placeholder_pcm_starts_at_pattern_stream_same_path" in behavior
    assert "test_iplayc_dos_capped_s3m_file_pcm_crosses_file_stream_refill_boundary_same_path" in behavior
    assert "test_iplayc_dos_mod_placeholder_pcm_starts_at_pattern_stream_same_path" in behavior
    assert "test_iplayc_dos_mtm_placeholder_pcm_starts_after_metadata_same_path" in behavior
    assert "test_iplayc_dos_stm_placeholder_pcm_starts_after_header_same_path" in behavior
    assert "test_iplayc_dos_far_placeholder_pcm_starts_after_metadata_same_path" in behavior
    assert "test_iplayc_dos_669_placeholder_pcm_starts_after_metadata_same_path" in behavior
    assert "test_iplayc_dos_psm_placeholder_pcm_starts_after_metadata_same_path" in behavior
    assert "test_iplayc_dos_ult_placeholder_pcm_starts_after_metadata_same_path" in behavior
    assert behavior.count('run_dos(IPLAYDIAG_EXE, "--blocks=32", "STRSTART.S3M")') == 3
    assert behavior.count('run_dos(IPLAYDIAG_EXE, "--blocks=32", "MODSTRM.MOD")') == 3
    assert behavior.count('run_dos(IPLAYDIAG_EXE, "--blocks=32", "MTMSTRM.MTM")') == 3
    assert behavior.count('run_dos(IPLAYDIAG_EXE, "--blocks=32", "STMSTRM.STM")') == 3
    assert behavior.count('run_dos(IPLAYDIAG_EXE, "--blocks=32", "FARSTRM.FAR")') == 3
    assert behavior.count('run_dos(IPLAYDIAG_EXE, "--blocks=32", "S669STRM.669")') == 3
    assert behavior.count('run_dos(IPLAYDIAG_EXE, "--blocks=32", "PSMSTRM.PSM")') == 3
    assert behavior.count('run_dos(IPLAYDIAG_EXE, "--blocks=32", "ULTSTRM.ULT")') == 3
    for expected_stream_assertion in (
        'assert_external_pcm_source(base, "s3m_module", "native-preview", 0, "memory", 512)',
        'assert_external_pcm_source(base, "mod_n_t_module", "native-preview", 0, "memory", 1084)',
        'assert_external_pcm_source(base, "mtm_module", "native-preview", 0, "memory", 66)',
        'assert_external_pcm_source(base, "_2stm_module", "native-preview", 0, "memory", 64)',
        'assert_external_pcm_source(base, "far_module", "native-preview", 0, "memory", 128)',
        'assert_external_pcm_source(base, "e669_module", "native-preview", 0, "memory", 113)',
        'assert_external_pcm_source(base, "psm_module", "native-preview", 0, "memory", 128)',
        'assert_external_pcm_source(base, "ult_module", "native-preview", 0, "memory", 96)',
        'assert_external_pcm_source(out, "s3m_module", "dos-fallback", 1, "file-path", 107)',
        'assert_external_pcm_source(out, "mtm_module", "dos-fallback", 1, "file-path", 66)',
        'assert_external_pcm_source(out, "_2stm_module", "dos-fallback", 1, "file-path", 64)',
        'assert_external_pcm_source(out, "far_module", "dos-fallback", 1, "file-path", 128)',
        'assert_external_pcm_source(out, "e669_module", "dos-fallback", 1, "file-path", 113)',
        'assert_external_pcm_source(out, "psm_module", "dos-fallback", 1, "file-path", 128)',
        'assert_external_pcm_source(out, "ult_module", "dos-fallback", 1, "file-path", 96)',
        'assert_external_pcm_source(out, "mod_n_t_module", "dos-fallback", 1, "file-path", 1084)',
        'assert_external_pcm_source(first, "mod_n_t_module", "dos-fallback", 1, "file-path", 1084)',
        'assert_external_pcm_source(second, "mod_n_t_module", "dos-fallback", 1, "file-path", 1084)',
        'assert_external_pcm_source(first, "psm_module", "dos-fallback", 1, "file-path", 128)',
        'assert_external_pcm_source(second, "psm_module", "dos-fallback", 1, "file-path", 128)',
        'assert_external_pcm_source(first, "far_module", "dos-fallback", 1, "file-path", 128)',
        'assert_external_pcm_source(second, "far_module", "dos-fallback", 1, "file-path", 128)',
        'assert_external_pcm_source(first, "e669_module", "dos-fallback", 1, "file-path", 113)',
        'assert_external_pcm_source(second, "e669_module", "dos-fallback", 1, "file-path", 113)',
        'assert_external_pcm_source(first, "ult_module", "dos-fallback", 1, "file-path", 96)',
        'assert_external_pcm_source(second, "ult_module", "dos-fallback", 1, "file-path", 96)',
        'assert_external_pcm_source(first, "mtm_module", "dos-fallback", 1, "file-path", 66)',
        'assert_external_pcm_source(second, "mtm_module", "dos-fallback", 1, "file-path", 66)',
        'assert_external_pcm_source(first, "_2stm_module", "dos-fallback", 1, "file-path", 64)',
        'assert_external_pcm_source(second, "_2stm_module", "dos-fallback", 1, "file-path", 64)',
        'assert_external_pcm_source(first, "s3m_module", "dos-fallback", 1, "file-path", stream_start)',
        'assert_external_pcm_source(second, "s3m_module", "dos-fallback", 1, "file-path", stream_start)',
    ):
        assert expected_stream_assertion in behavior
    assert "refill_boundary = stream_start + 496" in behavior
    assert 'assert_bounded_sb16_playback_blocks(first, 1)' in behavior
    assert 'assert_bounded_sb16_playback_blocks(second, 1)' in behavior
    assert "playback_checksum(base) == playback_checksum(before_only)" in behavior
    assert "playback_checksum(before_only) != playback_checksum(body_changed)" in behavior
    assert "Complete MOD streams start at pattern data" in status
    assert "Complete S3M streams start at the first playable pattern table target" in status
    assert "MTM streams skip the metadata/order-preview prefix" in status
    assert "STM streams skip the title/tracker header" in status
    assert "FAR/669/PSM/ULT streams skip their currently printed metadata header prefixes" in status


def test_real_sb16_player_skips_playback_when_hardware_probe_fails() -> None:
    player = (ROOT / "rewrite" / "iplay_player.c").read_text()

    assert "#define IPLAY_PLAYER_EXIT_AUDIO_UNAVAILABLE 3" in player
    assert "static int player_exit_audio_unavailable_status(void)" in player
    assert "static int player_audio_hardware_ready(void)" in player
    assert "#if IPLAY_PLAYER_ENABLE_SB16_HW && IPLAY_PLAYER_SB16_REAL_HARDWARE_IO" in player
    assert "return sb16_audio_ensure_ready(player_sb16_hardware());" in player
    assert "static void player_render_runtime_audio_unavailable(IplayRuntime *runtime)" in player
    assert "Playback disabled: SB16 not detected" in player
    assert "exit_status = player_exit_audio_unavailable_status();" in player
    assert "static int player_run_runtime_ui" in player
    assert "return player_run_loaded_module(module, runtime, runtime_config, module_status, trial_block_limit);" in player
    assert "if (player_audio_hardware_ready())" in player
    assert "player_prime_runtime_playback(runtime, module, trial_block_limit);" in player
    behavior = (ROOT / "tests" / "test_player_behavior.py").read_text()
    assert 'result = run_dos(IPLAYC_EXE, "ENDCONT.S3M", timeout=3)' in behavior
    assert 'IPLAYHW_EXE = BUILD_DIR / "IPLAYHW.EXE"' in behavior
    assert 'result = run_dos(IPLAYHW_EXE, "ENDCONT.S3M", timeout=3)' in behavior
    assert "SB16_BOUNDED_BLOCK_BYTES" in behavior
    assert "SB16_BOUNDED_BLOCK_FRAMES" in behavior
    assert "SB16_CONTINUOUS_BLOCK_BYTES" in behavior
    assert "SB16_CONTINUOUS_BLOCK_FRAMES" in behavior
    assert "assert_sb16_stereo_frame_bytes" in behavior
    assert "assert_playback_pump_sb16_stereo" in behavior
    assert "assert_playback_pump_stop_state" in behavior
    assert "assert_playback_loop" in behavior
    assert "assert_decoder_progress" in behavior
    assert "assert_decoder_progress_block" in behavior
    assert "assert_decoder_geometry" in behavior
    assert "assert_decoder_event" in behavior
    assert "assert_decoder_voice" in behavior
    assert "assert_module_loaded" in behavior
    assert "assert_module_not_loaded" in behavior
    assert "assert_module_loader" in behavior
    assert "assert_module_size" in behavior
    assert "assert_module_type_tag" in behavior
    assert "assert_module_title" in behavior
    assert "assert_unsupported_module" in behavior
    assert "assert_playback_output" in behavior
    assert "assert_playback_disabled" in behavior
    assert "assert_ffi_marker" in behavior
    assert "assert_orders_channels" in behavior
    assert "assert_help_usage" in behavior
    assert "assert_supported_dos_formats" in behavior
    assert "assert_sb16_audio_scope" in behavior
    assert "assert_text_backend" in behavior
    assert "assert_text_backend_memory" in behavior
    assert "assert_sdl_compatible_audio_backend" in behavior
    assert "assert_decoder_handoff" in behavior
    assert "assert_decoder_handoff_absent" in behavior
    assert "assert_decoder_route_absent" in behavior
    assert 'assert_playback_loop(out, "playback", "bounded-trial", "immediate", 32, SB16_BOUNDED_BLOCK_FRAMES)' in behavior
    assert 'assert_playback_loop(out, "playback", "timer-keyboard", "timer", 0, SB16_CONTINUOUS_BLOCK_FRAMES)' in behavior
    assert 'assert_decoder_progress(broken, 63, 128, 2, 1, 0, 0, 0, 1, 125, 1, 0)' in behavior
    assert 'assert_decoder_progress(out, 0, 64, 0, 0, 0, 0, 0, 1, 125, 1, 0)' in behavior
    assert 'assert_decoder_progress_block(out, 32, 7680)' in behavior
    assert 'assert_decoder_geometry(base, 2, 64, 0, 1, 125, 1)' in behavior
    assert 'assert_decoder_geometry(base, 3, 64, 0, 6, 125, 4)' in behavior
    assert 'assert_decoder_geometry(out, 128, 64, 0, 6, 125, 4)' in behavior
    assert 'assert_decoder_event(out, 855, 1, 1, 1, 64, 12, 127)' in behavior
    assert 'assert_decoder_event(faster, 214, 1, 4, 1, 48, 15, 180)' in behavior
    assert 'assert_decoder_voice(out, 1, 855, 1, 1, 1, 64, 4, 64, 0, 2, 6204)' in behavior
    assert 'assert_decoder_voice(portamento, 0, 800, 2, 1, 1, 48, 1024, 48, 0, 0, 2108)' in behavior
    assert 'assert_module_loaded(out, "aryx.s3m")' in behavior
    assert 'assert_module_loaded(out, "BIG.MOD")' in behavior
    assert 'assert_module_not_loaded(out, "ENDCONT.S3M")' in behavior
    assert 'assert_module_size(out, 24577)' in behavior
    assert 'assert_module_size(out, 20800)' in behavior
    assert 'assert_module_loader(out, "s3m_module (Scream Tracker 3)")' in behavior
    assert 'assert_module_loader(out, "mod_n_t_module (ProTracker/NoiseTracker MOD)")' in behavior
    assert 'assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")' in behavior
    assert 'assert_decoder_handoff(out, "project INR -> SB16 PCM.")' in behavior
    assert 'assert_decoder_route_absent(out, 1, "project-owned")' in behavior
    assert 'assert_decoder_handoff_absent(out, "project INR -> SB16 PCM.")' in behavior
    assert 'assert_module_type_tag(out, "204D3353")' in behavior
    assert 'assert_module_title(out, "SMOKE S3M")' in behavior
    assert 'assert_unsupported_module(out, "BAD.XYZ")' in behavior
    assert 'assert_unsupported_module(out, "BAD.MOD")' in behavior
    assert 'assert_playback_output(out, "SB16 16-bit stereo hardware wrapper enabled.")' in behavior
    assert 'assert_playback_disabled(out, "SB16 not detected")' in behavior
    assert 'assert_ffi_marker(signed, "0001")' in behavior
    assert 'assert_ffi_marker(unsigned, "0002")' in behavior
    assert 'assert_orders_channels(out, 129, 4)' in behavior
    assert 'assert_help_usage(out)' in behavior
    assert 'assert_supported_dos_formats(out)' in behavior
    assert 'assert_sb16_audio_scope(out)' in behavior
    assert 'assert_text_backend(out)' in behavior
    assert 'assert_text_backend_memory(out)' in behavior
    assert 'assert_sdl_compatible_audio_backend(out)' in behavior
    assert "def assert_bounded_sb16_playback_blocks(output: str, blocks: int) -> dict[str, object]:" in behavior
    assert "def assert_bounded_sb16_playback(output: str) -> dict[str, object]:" in behavior
    assert "def assert_bounded_source_end_playback(output: str, blocks: int) -> dict[str, object]:" in behavior
    assert "assert_bounded_sb16_playback_blocks(first, 64)" in behavior
    assert "assert_bounded_sb16_playback_blocks(second, 64)" in behavior
    assert "assert_bounded_source_end_playback(broken, 63)" in behavior
    assert "assert_bounded_source_end_playback(out, 0)" in behavior
    assert "assert_bounded_sb16_playback(out)" in behavior
    assert "assert_bounded_sb16_playback(base)" in behavior
    assert "assert_bounded_sb16_playback(sharp)" in behavior
    assert "assert_screen_present_content" in behavior
    assert "parse_playback_pump" in behavior
    assert "parse_screen_present_digest" in behavior
    assert 'audio_unavailable_screen = parse_screen_present_digest(out, "audio-unavailable")' in behavior
    assert 'assert_screen_present_content(audio_unavailable_screen, "full-screen")' in behavior
    assert 'assert audio_unavailable_screen["mode_ok"] == 1' in behavior
    assert 'status_screen = parse_screen_present_digest(out, "status")' in behavior
    assert 'playback_screen = parse_screen_present_digest(out, "playback-position")' in behavior
    assert 'post_status_screen = parse_screen_present_digest(out, "post-playback-status")' in behavior
    assert "assert_text_screen_geometry" in behavior
    assert "assert_text_screen_geometry(status_screen, 80, 25)" in behavior
    assert "assert_text_screen_geometry(playback_screen, cols, rows)" in behavior
    assert "assert_text_screen_geometry(post_status_screen, cols, rows)" in behavior
    assert 'assert_text_screen_geometry(playback_screen, 80, 25)' in behavior
    assert 'assert_text_screen_geometry(post_status_screen, 80, 25)' in behavior
    assert 'assert_screen_present_content(status_screen, "full-screen")' in behavior
    assert 'assert_screen_present_content(playback_screen, "full-screen", expected_audio_frames=bounded_pump["frames"])' in behavior
    assert 'assert_screen_present_content(post_status_screen, "status-only", expected_audio_frames=bounded_pump["frames"])' in behavior
    assert 'assert status_screen["audio_frames"] == 0' in behavior
    assert 'assert playback_screen["audio_frames"] == 16384' in behavior
    assert 'continuous_pump = parse_playback_pump(out)' in behavior
    assert 'left_right_pump = parse_playback_pump(left_right)' in behavior
    assert 'right_left_pump = parse_playback_pump(right_left)' in behavior
    assert 'mono_pump = parse_playback_pump(mono)' in behavior
    assert 'stereo_pump = parse_playback_pump(stereo)' in behavior
    assert 'assert_playback_pump_sb16_stereo(left_right_pump, 32, SB16_BOUNDED_BLOCK_FRAMES)' in behavior
    assert 'assert left_right_pump["accepted"] == 32 * SB16_BOUNDED_BLOCK_BYTES' in behavior
    assert 'assert_playback_pump_sb16_stereo(mono_pump, 32, SB16_BOUNDED_BLOCK_FRAMES)' in behavior
    assert 'assert mono_pump["accepted"] == 32 * SB16_BOUNDED_BLOCK_BYTES' in behavior
    assert 'assert_playback_pump_sb16_stereo(continuous_pump, 1, SB16_CONTINUOUS_BLOCK_FRAMES)' in behavior
    assert 'assert_playback_pump_sb16_stereo(bounded_pump, 32, SB16_BOUNDED_BLOCK_FRAMES)' in behavior
    assert 'assert continuous_pump["accepted"] == SB16_CONTINUOUS_BLOCK_BYTES' in behavior
    assert 'assert_playback_pump_stop_state(continuous_pump, 0, 1, "source-end")' in behavior
    assert 'playback_screen = parse_screen_present_digest(out, "playback-position")' in behavior
    assert 'post_status_screen = parse_screen_present_digest(out, "post-playback-status")' in behavior
    assert 'assert_screen_present_content(post_status_screen, "status-only", expected_audio_frames=continuous_pump["frames"])' in behavior
    assert '"Filename      : SMOKE.S3M"' in behavior
    assert '"Output Levels :"' in behavior
    assert '"Module Type   : S3M"' in behavior
    assert '"24bit Interpolation      F-12"' in behavior
    assert behavior.count('"Filename      : SMOKE.S3M"') >= 2
    assert "def test_iplaydiag_help_advertises_supported_video_modes() -> None:" in behavior
    assert 'result = run_dos(IPLAYDIAG_EXE, "/?")' in behavior
    assert "Text backend: VGA color/BW text memory" in behavior
    assert "def test_iplaydiag_rejects_invalid_video_mode_before_playback() -> None:" in behavior
    assert 'result = run_dos(IPLAYDIAG_EXE, "--video-mode=bad", "SMOKE.S3M")' in behavior
    assert "Unsupported video mode. Use --video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50" in behavior
    assert 'assert "Module: SMOKE.S3M" not in out' in behavior
    assert 'assert "Playback pump:" not in out' in behavior
    assert "def test_iplaydiag_valid_video_modes_render_playback_geometry(mode: str, cols: int, rows: int, screen_bytes: int) -> None:" in behavior
    assert '("40x25bw", 40, 25, 2000)' in behavior
    assert '("40X25BW", 40, 25, 2000)' in behavior
    assert '("40x25mono", 40, 25, 2000)' in behavior
    assert '("40x25color", 40, 25, 2000)' in behavior
    assert '("80x25bw", 80, 25, 4000)' in behavior
    assert '("80X25BW", 80, 25, 4000)' in behavior
    assert '("80x25mono", 80, 25, 4000)' in behavior
    assert '("80x25color", 80, 25, 4000)' in behavior
    assert '("80x50", 80, 50, 8000)' in behavior
    assert '("80X50", 80, 50, 8000)' in behavior
    assert '("80x50project", 80, 50, 8000)' in behavior
    assert 'result = run_dos(IPLAYDIAG_EXE, "--blocks=1", f"--video-mode={mode}", "SMOKE.S3M")' in behavior
    assert 'playback_screen = parse_screen_present_digest(out, "playback-position")' in behavior
    assert 'post_status_screen = parse_screen_present_digest(out, "post-playback-status")' in behavior
    assert 'assert playback_screen["bytes"] == screen_bytes' in behavior
    assert 'assert post_status_screen["bytes"] == screen_bytes' in behavior
    assert 'bounded_pump = parse_playback_pump(out)' in behavior
    assert 'assert_playback_pump_sb16_stereo(bounded_pump, 1, SB16_BOUNDED_BLOCK_FRAMES)' in behavior
    assert 'assert bounded_pump["accepted"] == SB16_BOUNDED_BLOCK_BYTES' in behavior
    assert 'assert_playback_pump_stop_state(bounded_pump, 0, 1, "source-end")' in behavior
    assert 'assert bounded_pump["accepted"] == 65536' in behavior
    assert 'assert_playback_pump_stop_state(bounded_pump, 1, 0, "block-limit")' in behavior
    assert 'def playback_checksum(output: str) -> int:\n    return int(parse_playback_pump(output)["checksum"])' in behavior
    assert 're.search(r"^Playback pump: .* checksum=' not in behavior
    assert "assert result.returncode == 3, out" in behavior
    assert "assert result.returncode == 0, out\n    assert \"Module: ENDCONT.S3M\" not in out" not in behavior
    assert "exits with `IPLAY_PLAYER_EXIT_AUDIO_UNAVAILABLE`" in (ROOT / "rewrite" / "STATUS.md").read_text()


def test_player_only_rebuild_script_keeps_iteration_off_full_runner_build() -> None:
    script = (ROOT / "rewrite" / "build_player.sh").read_text()
    behavior = (ROOT / "tests" / "test_player_behavior.py").read_text()
    smoke_test = (ROOT / "tests" / "test_player_smoke.py").read_text()
    status = (ROOT / "rewrite" / "STATUS.md").read_text()
    player = (ROOT / "rewrite" / "iplay_player.c").read_text()
    gate_script = (ROOT / "rewrite" / "check_rewrite.sh").read_text()

    assert "rewrite/.build/IPLAYC.EXE" in script
    assert "rewrite/.build/IPLAYTRY.EXE" in script
    assert "rewrite/.build/IPLAYDIAG.EXE" in script
    assert "rewrite/.build/IPLAYCONT.EXE" in script
    assert "rewrite/.build/IPLAYHW.EXE" in script
    assert "iplay_player_cont_zm.obj" in script
    assert "iplay_player_try_zm.obj" in script
    assert "iplay_player_diag_zm.obj" in script
    assert "iplay_player_contdiag_zm.obj" in script
    assert "iplay_player_hwdiag_zm.obj" in script
    assert "IPLAY_PLAYER_SB16_REAL_HARDWARE_IO=1" in script
    assert "--video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50" in player
    assert "static int player_streq_ci(const char *left, const char *right)" in player
    assert "static int player_arg_is_video_mode_override(const char *arg)" in player
    assert "static db player_parse_cli_video_mode(int argc, char **argv)" in player
    assert "static int player_parse_cli_video_mode_valid(int argc, char **argv)" in player
    assert "static int player_video_mode_value_supported(const char *value)" in player
    assert "static int player_report_invalid_video_mode(void)" in player
    assert "Unsupported video mode. Use --video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50" in player
    assert "if (!player_module_request_video_mode_valid(&request)) return player_report_invalid_video_mode();" in player
    assert "player_set_text_video_mode_id(player_module_request_video_mode(request));" in player
    assert "IPLAY_VIDEO_MODE_40X25_BW" in player
    assert "IPLAY_VIDEO_MODE_40X25_COLOR" in player
    assert "IPLAY_VIDEO_MODE_80X25_BW" in player
    assert "IPLAY_VIDEO_MODE_80X25_COLOR" in player
    assert "IPLAY_VIDEO_MODE_80X50_PROJECT" in player
    assert 'player_streq_ci(value, "40x25mono")' in player
    assert 'player_streq_ci(value, "80x25mono")' in player
    assert 'player_streq_ci(value, "80x50project")' in player
    assert 'player_streq_ci(value, "80x50")' in player
    assert "needs_rebuild()" in script
    assert "compile_obj()" in script
    assert "link_exe()" in script
    assert "cmp -s \"$tmp\" \"$lnk\"" in script
    assert 'if needs_rebuild "$obj" "$src" rewrite/iplay_rewrite.h rewrite/build_player.sh; then' in script
    assert 'if needs_rebuild "$exe" "$lnk" "$@"; then' in script
    assert "abi_nullsub3_runner" not in script
    assert "IABI_" not in script
    assert 'subprocess.run([str(ROOT / "rewrite" / "build_player.sh")], cwd=ROOT, check=True, timeout=30)' in behavior
    assert 'subprocess.run([str(ROOT / "rewrite" / "build_rewrite.sh")], cwd=ROOT, check=True)' not in behavior
    smoke_script = (ROOT / "rewrite" / "smoke_player.sh").read_text()
    assert "./rewrite/build_player.sh" in smoke_script
    assert "./rewrite/build_rewrite.sh" not in smoke_script
    assert "run_iplaytry ENDCONT.S3M" in smoke_script
    assert "run_iplaycont ENDCONT.S3M" in smoke_script
    assert "write_endcont_module(Path(\".\"))" in smoke_script
    assert "run_iplayhw_unavailable SMOKE.S3M" in smoke_script
    assert "IPLAYTRY.EXE" in smoke_script
    assert "IPLAYCONT.EXE" in smoke_script
    assert "IPLAYHW.EXE" in smoke_script
    assert "Playback loop: mode=playback policy=timer-keyboard cadence=timer max_blocks=0 frames/block=1024" in smoke_script
    assert "limit=0 source_end=1 stop=source-end" in smoke_script
    assert "Screen present: reason=post-playback-status scope=status-only" in smoke_script
    assert "run_video_mode_smoke SMOKE.S3M 40x25bw 40 25 2000" in smoke_script
    assert "run_video_mode_smoke SMOKE.S3M 40x25color 40 25 2000" in smoke_script
    assert "run_video_mode_smoke SMOKE.S3M 80x25bw 80 25 4000" in smoke_script
    assert "run_video_mode_smoke SMOKE.S3M 80x25color 80 25 4000" in smoke_script
    assert "run_video_mode_smoke SMOKE.S3M 80x50 80 50 8000" in smoke_script
    assert "IPLAYDIAG video-mode smoke failed:" in smoke_script
    assert '"--video-mode=$mode"' in smoke_script
    assert "expected audio-unavailable status 3" in smoke_script
    assert "Screen present: reason=audio-unavailable scope=full-screen" in smoke_script
    assert "unexpectedly used real SB16 unavailable path" in smoke_script
    assert "IPLAYTRY/IPLAYCONT/IPLAYHW/IPLAYDIAG/TEXTMODE smoke ok" in smoke_script
    assert "IPLAYTRY/IPLAYCONT/IPLAYHW/IPLAYDIAG/TEXTMODE smoke ok" in smoke_test
    trial_script = (ROOT / "rewrite" / "try_player.sh").read_text()
    assert "./rewrite/build_player.sh" in trial_script
    assert "./rewrite/build_rewrite.sh" not in trial_script
    assert "IPLAY_TRIAL_LOG=${IPLAY_TRIAL_LOG:-RES.TXT}" in trial_script
    assert "usage: ./rewrite/try_player.sh [--rebuild] [--modern|--native|--native-interactive|--native-source-end|--native-keyboard-after-one|--native-stdin-keyboard|--native-audio|--native-terminal|--native-live|--quiet|--diagnostics|--continuous-diagnostics|--hardware-diagnostics|--production] [--blocks=N] [--video-mode=MODE] <module-file|@file-list>" in trial_script
    assert "direct SDL/notcurses player example: ./rewrite/iplay.sh <module-file>" in trial_script
    assert "./rewrite/iplay.sh --diagnostics --video-mode=80x50 <module-file> for raw evidence" in trial_script
    assert trial_script.count("-h|--help)") >= 3
    assert "default mode is the safe kvikdos proof path; use --production only on real SB16-capable DOS or when checking the SB16-unavailable exit" in trial_script
    assert "def test_try_player_help_explains_kvikdos_default_and_real_sb16_production() -> None:" in smoke_test
    assert 'assert "use --production only on real SB16-capable DOS or when checking the SB16-unavailable exit" in result.stderr' in smoke_test
    assert "--diagnostics runs IPLAYDIAG.EXE for visible bounded diagnostic stdout" in trial_script
    assert "--continuous-diagnostics runs IPLAYCONT.EXE for visible continuous-loop diagnostic stdout" in trial_script
    assert "--native-source-end runs the native host path until libmikmod reports natural source end" in trial_script
    assert "--native-keyboard-after-one runs the native host path until the keyboard/interactive stop seam fires after one block" in trial_script
    assert "--quiet runs IPLAYTRY.EXE continuous quiet playback; in headless kvikdos this can end by timeout" in trial_script
    assert "--hardware-diagnostics runs IPLAYHW.EXE for real-SB16 probe/unavailable diagnostics" in trial_script
    assert "--production runs IPLAYC.EXE, the quiet production real-SB16 DOS player" in trial_script
    assert "--rebuild forces a player-only rebuild before launching kvikdos" in trial_script
    assert "--blocks=N is consumed by IPLAYDIAG.EXE when bounded diagnostics are enabled" in trial_script
    assert "--video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50|terminal|auto selects the text mode for the trial" in trial_script
    assert "terminal/auto selects the nearest supported size from COLUMNS/LINES or stty size" in trial_script
    assert "stty size" in trial_script
    assert "trial_terminal_rows_from_stty" in trial_script
    assert "test_try_player_validate_only_terminal_video_mode_uses_stty_size_without_kvikdos" in smoke_test
    assert "try_player: unsupported video mode:" in trial_script
    assert "try_player: --blocks=N is only supported with diagnostic trial modes" in trial_script
    assert "try_player: missing module file after trial options" in trial_script
    assert "try_player: module file not found:" in trial_script
    assert "try_player: file list not found:" in trial_script
    assert "try_player: file list has no module entries:" in trial_script
    assert "resolve_case_insensitive_file()" in trial_script
    assert "try_player: resolved DOS-style case-insensitive file-list path:" in trial_script
    assert "trial_filelist_arg=%s" in trial_script
    assert "trial_filelist_path=%s" in trial_script
    assert "trial_filelist_selected=%s" in trial_script
    assert "trial_filelist_selected_host=%s" in trial_script
    assert "native_module_arg=@$trial_filelist_abs" in trial_script
    assert 'set -- "$native_module_arg" "$native_play_arg" "$trial_video_mode"' in trial_script
    assert '"rewrite/.build/$IPLAY_NATIVE_EXE" "$@"' in trial_script
    assert "def test_try_player_validate_only_filelist_selects_first_trimmed_entry_without_kvikdos(tmp_path: Path) -> None:" in smoke_test
    assert "def test_try_player_validate_only_resolves_filelist_path_case_like_dos_without_kvikdos(tmp_path: Path) -> None:" in smoke_test
    assert "def test_try_player_native_filelist_reports_playback_ok(tmp_path: Path) -> None:" in smoke_test
    assert "def test_try_player_native_resolves_filelist_path_case_like_dos(tmp_path: Path) -> None:" in smoke_test
    assert 'if [ "$src_dir/$name" != "$dst_dir/$name" ]; then' in trial_script
    assert 'cp "$src_dir/$name" "$dst_dir/$name"' in trial_script
    assert trial_script.count('--diagnostics)') >= 2
    assert "trial_diagnostics=1" in trial_script
    assert trial_script.count('--continuous-diagnostics)') >= 2
    assert trial_script.count('--quiet)') >= 2
    assert "trial_diagnostics=0" in trial_script
    assert trial_script.count('--hardware-diagnostics)') >= 2
    assert trial_script.count('--production)') >= 2
    assert "trial_hardware_diagnostics=${IPLAY_TRIAL_HARDWARE_DIAGNOSTICS:-0}" in trial_script
    assert "trial_hardware_diagnostics=1" in trial_script
    assert "trial_production=1" in trial_script
    assert "trial_production=0" in trial_script
    assert trial_script.count('--rebuild)') >= 2
    assert "trial_rebuild=${IPLAY_TRIAL_REBUILD:-auto}" in trial_script
    assert "trial_diagnostics=${IPLAY_TRIAL_DIAGNOSTICS:-1}" in trial_script
    assert "trial_native_source_end=0" in trial_script
    assert "native_loop_policy=native-source-end" in trial_script
    assert "native_expected_status=ok" in trial_script
    assert "trial_native_keyboard_after_one=0" in trial_script
    assert "native_loop_policy=native-keyboard-stop" in trial_script
    assert "native_play_arg=--keyboard-after-one" in trial_script
    assert "native_play_arg=--source-end" in trial_script
    assert "native_trial_loaded_module_name=$(grep '^Module: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | tr -d '\\r' | sed 's/^Module: //')" in trial_script
    assert "trial_requested_module_loaded=yes" in trial_script
    assert "native_trial_module_size=$(grep '^Size: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | tr -d '\\r' | sed 's/^Size: //; s/ bytes$//')" in trial_script
    assert "trial_module_size_matches_host=yes" in trial_script
    assert "native_trial_module_loader_line=$(grep '^Loader: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | tr -d '\\r')" in trial_script
    assert "trial_module_loader=%s" in trial_script
    assert "native_trial_module_type_tag=$(grep '^Module type tag: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | tr -d '\\r' | sed 's/^Module type tag: //')" in trial_script
    assert "native_trial_module_title=$(grep '^Title: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | tr -d '\\r' | sed 's/^Title: //')" in trial_script
    assert "grep '^trial_ok_loader_metadata=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "native_trial_pcm_source_line=$(grep '^PCM source: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1)" in trial_script
    assert "trial_pcm_provider=%s" in trial_script
    assert "native_trial_decoder_route_line=$(grep '^Decoder route: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1)" in trial_script
    assert "trial_decoder_route_name=%s" in trial_script
    assert "native_trial_decoder_handoff_line=$(grep '^Decoder handoff: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1)" in trial_script
    assert "trial_selected_screen_geometry_valid=yes" in trial_script
    assert "trial_playback_position_present=yes" in trial_script
    assert "trial_playback_position_valid=yes" in trial_script
    assert "trial_playback_position_geometry_valid=yes" in trial_script
    assert "trial_post_playback_status_present=yes" in trial_script
    assert "trial_post_playback_status_valid=yes" in trial_script
    assert "trial_post_playback_status_geometry_valid=yes" in trial_script
    assert "trial_color_probe_valid=yes" in trial_script
    assert "grep '^trial_color_probe_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^trial_requested_module_loaded=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^trial_pcm_provider=libmikmod$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^trial_decoder_route_name=external-library$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^trial_post_playback_status_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^trial_playback_position_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "trial_result=project-decoder-unavailable" in trial_script
    assert "trial_failure_reason=project-decoder-unavailable" in trial_script
    assert "trial_result=unsupported-format" in trial_script
    assert "trial_failure_reason=unsupported-format" in trial_script
    assert "trial_result=external-decoder-failed" in trial_script
    assert "trial_failure_reason=external-decoder-failed" in trial_script
    assert "def test_try_player_native_corrupt_known_tracker_reports_external_decoder_failed(tmp_path: Path) -> None:" in smoke_test
    native_probe = (ROOT / "rewrite" / "modplug_audio_probe.cpp").read_text()
    assert "static const char *native_program_name(const char *path)" in native_probe
    assert "std::strrchr(path, '/')" in native_probe
    assert "const char *display_name = native_program_name(program_name);" in native_probe
    assert "usage: %s [--modern] [--video-mode=MODE] <module-file|@file-list> [--modern|max-blocks|--blocks=N|--source-end|--keyboard-after-one]" in native_probe
    assert "--video-mode=MODE selects the same text mode as a positional mode argument" in native_probe
    assert "print_native_usage(argv[0]);" in native_probe
    assert "static int native_parse_max_blocks_arg(const char *arg)" in native_probe
    assert 'std::strncmp(arg, "--blocks=", 9) == 0' in native_probe
    assert "--blocks=N bounds native playback to N external-decoder blocks" in native_probe
    assert "modern SDL/notcurses player mode is the default for iplay" in native_probe
    assert "--modern enables the preferred direct SDL/notcurses player mode" in native_probe
    assert "static void native_enable_modern_mode(int *modern, int *max_blocks, int *sdl_audio, int *terminal_render, int *terminal_live, int *stdin_keyboard)" in native_probe
    assert 'std::strcmp(native_program_name(argv[0]), "iplay") == 0' in native_probe
    assert "native_enable_modern_mode(&native_modern_requested, &max_blocks, &sdl_audio_requested, &terminal_render_requested, &terminal_live_requested, &stdin_keyboard_requested);" in native_probe
    assert "const char *module_arg = 0;" in native_probe
    assert "int video_mode_explicit = 0;" in native_probe
    assert "static const char *native_video_mode_option_value(const char *arg)" in native_probe
    assert 'std::strncmp(arg, "--video-mode=", 13) == 0' in native_probe
    assert "i = 1;" in native_probe
    assert "module_arg = argv[i];" in native_probe
    assert "if (!module_arg) return 2;" in native_probe
    assert 'std::strcmp(argv[i], "--modern") == 0' in native_probe
    assert "static int native_arg_is_text_mode(const char *arg)" in native_probe
    assert "static int native_arg_is_block_limit(const char *arg)" in native_probe
    assert 'std::strncmp(arg, "--blocks=", 9) == 0' in native_probe
    assert "native_positionals" in native_probe
    assert "native_modern_requested && native_arg_is_text_mode(native_positionals[0])" in native_probe
    assert "native_modern_requested && !native_arg_is_block_limit(native_positionals[0])" in native_probe
    assert "video_mode_explicit = 1;" in native_probe
    assert 'if (native_modern_requested && !video_mode_explicit && native_positional_count == 0)' in native_probe
    assert 'video_mode_arg = "auto";' in native_probe
    assert "--sdl-audio opens a real SDL2 queued-audio device for audible native playback" in native_probe
    assert "--terminal-render paints the final notcurses-style text cells to the host terminal with ANSI 16-color output" in native_probe
    assert "--terminal-live updates ANSI audio level meters from the native playback callback while blocks are submitted" in native_probe
    assert "--stdin-keyboard stops native playback when q, Q, or Escape is read from stdin" in native_probe
    assert "static void native_audio_sink_write(void *user, const db *pcm, dw byte_count)" in native_probe
    assert "native_audio_sink_write, &audio_sink" in native_probe
    assert "native_resolve_filelist_argument" in native_probe
    assert "native_resolve_case_insensitive_path" in native_probe
    assert "native_streq_ci" in native_probe
    assert "resolved_list_path" in native_probe
    assert 'std::fprintf(stderr, "%s: unsupported text mode: %s\\n", native_program_name(argv[0]), video_mode_arg);' in native_probe
    assert 'std::fprintf(stderr, "%s: could open SDL2 SB16 stereo audio sink requested freq=%d format=0x%04x channels=%u samples=%u: %s\\n",' not in native_probe
    assert 'std::fprintf(stderr, "%s: could not open SDL2 SB16 stereo audio sink requested freq=%d format=0x%04x channels=%u samples=%u: %s\\n",' in native_probe
    assert 'if (module_arg[0] == \'@\') std::printf("File list: %s selected=%s\\n", module_arg, module_path);' in native_probe
    assert 'std::fprintf(stderr, "Module not found.\\n");' in native_probe
    assert "@file-list selects the first non-empty trimmed module path relative to the list file" in native_probe
    assert "module filenames are resolved with DOS-style case-insensitive matching in their host directory" in native_probe
    assert "test_native_binary_modern_alias_accepts_flag_before_module" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_user_facing_iplay_alias_help_uses_alias_name" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_user_facing_iplay_alias_defaults_to_modern_sdl_notcurses_player" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_loads_first_trimmed_filelist_entry_relative_to_list" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_resolves_module_path_case_like_dos" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_resolves_filelist_entry_case_like_dos" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_resolves_filelist_path_case_like_dos" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_missing_module_reports_original_style_not_found" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_missing_filelist_entry_reports_original_style_not_found" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_user_facing_iplay_alias_errors_use_alias_name" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_user_facing_iplay_alias_filelist_echo_uses_module_argument" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "--source-end plays until libmikmod reports natural source end" in native_probe
    assert "plays external tracker modules through libmodplug into the SDL-compatible SB16 16-bit stereo bridge" in native_probe
    assert "renders status through the notcurses-style text runtime and supports 40x25, 80x25, and 80x50 text geometry; terminal/auto selects the nearest supported size from COLUMNS/LINES or TIOCGWINSZ" in native_probe
    assert "print_color_probe_evidence()" in native_probe
    assert "Color probe: target_fg=16 target_bg=8 cells=24 fg_matches=%u bg_matches=%u blink_matches=%u fg_mask=%04x bg_mask=%02x blink_mask=%02x" in native_probe
    assert "iplay_ncplane_putc_yx(plane, 0u, (dw)i, (db)('A' + i), iplay_text_attr((IplayTextColor)i, (IplayTextColor)0, 0));" in native_probe
    assert "%s: unsupported text mode:" in native_probe
    assert "static const char *native_loader_name(const char *path)" in native_probe
    assert "static const char *native_module_type_tag(const char *path)" in native_probe
    assert "static void native_module_title(const char *path, char *dst, size_t dst_size)" in native_probe
    assert 'native_streq_ci(arg, "80x50project")' in native_probe
    assert "static const IplayTextMode *native_terminal_text_mode(void)" in native_probe
    assert 'native_streq_ci(arg, "terminal") || native_streq_ci(arg, "auto")' in native_probe
    assert 'native_env_unsigned("COLUMNS")' in native_probe
    assert 'native_env_unsigned("LINES")' in native_probe
    assert "TIOCGWINSZ" in native_probe
    assert "test_native_binary_accepts_text_mode_case_like_dos" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_accepts_video_mode_option_before_or_after_module" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_user_facing_iplay_alias_accepts_video_mode_option_before_module" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_terminal_text_mode_uses_columns_lines" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_blocks_option_reports_block_limit" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_sdl_audio_option_queues_exact_sb16_sink" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_modern_alias_enables_direct_sdl_notcurses_player_mode" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_modern_alias_accepts_text_mode_before_or_after_flag" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_terminal_render_option_paints_selected_text_mode" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_terminal_live_option_updates_audio_levels_per_block" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert "test_native_binary_stdin_keyboard_option_stops_on_q" in (ROOT / "tests" / "test_modplug_renderer.py").read_text()
    assert 'render_and_print_rows("playback", selected_mode, module_path, &result, "playback-position", "full-screen")' in native_probe
    assert 'std::printf("Loader: %s (%s)\\n", native_loader_name(path), native_loader_description(path));' in native_probe
    assert 'std::printf("Module type tag: %s\\n", native_module_type_tag(path));' in native_probe
    assert 'std::printf("Title: %s\\n", title[0] ? title : "none");' in native_probe
    assert 'player_rebuild_deps="rewrite/iplay_player.c rewrite/iplay_rewrite.c rewrite/iplay_rewrite.h rewrite/iplay_abi_watcom.c rewrite/build_player.sh"' in trial_script
    assert "try_player: selected trial executable is stale after player build:" in trial_script
    assert "trial_binary_fresh=%s" in trial_script
    assert "trial_exe_path=%s" in trial_script
    assert 'if [ "$needs_rebuild" = "1" ]; then' in trial_script
    assert "rewrite/iplay_rewrite.h" in trial_script
    assert trial_script.count('--blocks=*)') >= 2
    assert trial_script.count('--video-mode=*)') >= 2
    assert "trial_video_mode_arg=" in trial_script
    assert "trial_video_mode_default=80x25color" in trial_script
    assert "trial_video_mode_default=auto" in trial_script
    assert "trial_video_mode_key=$(printf '%s' \"${trial_video_mode_arg:-$trial_video_mode_default}\" | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz')" in trial_script
    assert 'if [ -n "${trial_video_mode_arg:-}" ]; then' in trial_script
    assert "trial_video_mode=40x25bw" in trial_script
    assert "trial_video_mode=40x25color" in trial_script
    assert "trial_video_mode=80x25bw" in trial_script
    assert "trial_video_mode=80x25color" in trial_script
    assert "trial_video_mode=80x50" in trial_script
    assert "40x25bw|40x25mono)" in trial_script
    assert "40x25color|40x25)" in trial_script
    assert "80x25bw|80x25mono)" in trial_script
    assert "80x25color|80x25)" in trial_script
    assert "80x50|80x50project)" in trial_script
    assert 'if [ "${IPLAY_TRIAL_VALIDATE_ONLY:-0}" = "1" ]; then' in trial_script
    assert "trial_blocks_set=0" in trial_script
    assert "trial_blocks_set=1" in trial_script
    assert "IPLAY_TRIAL_DIAGNOSTIC_BLOCKS:-32" in trial_script
    assert 'set -- $player_args "$name" "$@"' in trial_script
    assert "IPLAY_TRIAL_EXE=" in trial_script
    assert "IPLAY_TRIAL_EXE=" in trial_script
    assert "IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYDIAG.EXE}" in trial_script
    assert "IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYCONT.EXE}" in trial_script
    assert "IPLAY_TRIAL_EXE=${IPLAY_TRIAL_EXE:-IPLAYHW.EXE}" in trial_script
    assert "try_player: trial executable not found after player build:" in trial_script
    assert 'if [ "$trial_diagnostics" = "1" ]; then' in trial_script
    assert 'IPLAYC.EXE|IPLAYHW.EXE)' in trial_script
    assert "trial_audio_mode=real-sb16-hardware" in trial_script
    assert "trial_audio_mode=wrapper-sb16-kvikdos-not-audible" in trial_script
    assert "trial_loop_policy=bounded-diagnostics" in trial_script
    assert "trial_loop_policy=continuous-diagnostics" in trial_script
    assert "trial_loop_policy=continuous-quiet-wrapper" in trial_script
    assert "trial_loop_policy=continuous-real-sb16" in trial_script
    assert "trial_loop_policy=continuous-real-sb16-diagnostics" in trial_script
    assert "trial_loop_policy=custom" in trial_script
    assert "trial_proof_scope=playable-wrapper-diagnostic" in trial_script
    assert "trial_proof_scope=playable-wrapper-continuous" in trial_script
    assert "trial_proof_scope=production-real-sb16" in trial_script
    assert "trial_proof_scope=hardware-unavailable-probe" in trial_script
    assert "trial_proof_scope=custom" in trial_script
    assert "printf 'trial_exe=%s diagnostics=%s hardware_diagnostics=%s production=%s rebuild=%s needs_rebuild=%s\\n' \"$IPLAY_TRIAL_EXE\" \"$trial_diagnostics\" \"$trial_hardware_diagnostics\" \"$trial_production\" \"$trial_rebuild\" \"$needs_rebuild\"" in trial_script
    assert "printf 'audio_mode=%s\\n' \"$trial_audio_mode\"" in trial_script
    assert "printf 'trial_loop_policy=%s\\n' \"$trial_loop_policy\"" in trial_script
    assert "printf 'trial_proof_scope=%s\\n' \"$trial_proof_scope\"" in trial_script
    assert "printf 'dos_args='" in trial_script
    assert 'sep=\n  for arg in "$@"; do' in trial_script
    assert 'for arg in "$@"; do' in trial_script
    assert "trial_mode_note=quiet-player-no-diagnostic-stdout" in trial_script
    assert "quiet_trial_timeout=yes meaning=headless-run-ended-by-timeout-not-by-player-exit" in trial_script
    assert "quiet_trial_completed=yes meaning=player-exited-without-diagnostic-stdout" in trial_script
    assert "trial_result=quiet-completed-no-diagnostics" in trial_script
    assert "trial_audio_unavailable=yes" in trial_script
    assert "trial_audio_unavailable_source=screen" in trial_script
    assert "trial_audio_unavailable_source=exit-code" in trial_script
    assert "trial_audio_unavailable_source=none" in trial_script
    assert "trial_result=audio-unavailable" in trial_script
    assert "trial_failure_reason=sb16-audio-unavailable" in trial_script
    assert "trial_module_loaded=yes" in trial_script
    assert "trial_loaded_module_name=%s" in trial_script
    assert "trial_loaded_module_key=%s" in trial_script
    assert "trial_requested_module_key=%s" in trial_script
    assert "trial_requested_module_loaded=yes" in trial_script
    assert "trial_playback_pump=yes" in trial_script
    assert "trial_playback_valid=yes" in trial_script
    assert "trial_audio_backend=SDL-compatible SB16 16-bit stereo" in trial_script
    assert "trial_audio_backend_valid=yes" in trial_script
    assert "trial_audio_levels_valid=yes" in trial_script
    assert "grep '^trial_audio_backend_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_audio_levels_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "trial_audio_level_sequence_valid=yes" in trial_script
    assert "grep '^trial_audio_level_sequence_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "trial_color_probe_valid=yes" in trial_script
    assert "grep '^trial_color_probe_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "trial_resize_cycle_valid=yes" in trial_script
    assert "grep '^trial_resize_cycle_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "trial_subwindow_valid=yes" in trial_script
    assert "grep '^trial_subwindow_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "trial_screen_present=yes" in trial_script
    assert "trial_playback_position_present=yes" in trial_script
    assert "trial_playback_position_valid=yes" in trial_script
    assert "trial_playback_position_geometry_valid=yes" in trial_script
    assert "trial_post_playback_status_present=yes" in trial_script
    assert "trial_post_playback_status_valid=yes" in trial_script
    assert "trial_post_playback_status_geometry_valid=yes" in trial_script
    assert "trial_audio_unavailable=yes" in trial_script
    assert "trial_audio_unavailable_source=screen" in trial_script
    assert "trial_audio_unavailable_source=exit-code" in trial_script
    assert "trial_audio_unavailable_source=none" in trial_script
    assert "trial_playback_line=%s" in trial_script
    assert "trial_screen_reasons=%s" in trial_script
    assert "trial_pcm_source_line=%s" in trial_script
    assert "trial_pcm_source_line=none" in trial_script
    assert "trial_pcm_provider=%s" in trial_script
    assert "trial_pcm_renderer=%s" in trial_script
    assert "trial_pcm_route=%s" in trial_script
    assert "trial_pcm_input=%s" in trial_script
    assert "trial_pcm_truncated=%s" in trial_script
    assert "trial_pcm_hook_provider=%s" in trial_script
    assert "trial_pcm_stream_start=%s" in trial_script
    assert "trial_decoder_route_line=%s" in trial_script
    assert "trial_decoder_route_line=none" in trial_script
    assert "trial_decoder_route_id=%s" in trial_script
    assert "trial_decoder_route_name=%s" in trial_script
    assert "trial_pcm_provider=none" in trial_script
    assert "trial_pcm_renderer=none" in trial_script
    assert "trial_pcm_route=none" in trial_script
    assert "trial_pcm_input=none" in trial_script
    assert "trial_pcm_truncated=none" in trial_script
    assert "trial_pcm_hook_provider=missing" in trial_script
    assert "trial_pcm_stream_start=none" in trial_script
    assert "trial_decoder_route_id=none" in trial_script
    assert "trial_decoder_route_name=none" in trial_script
    assert "trial_module_loaded=no" in trial_script
    assert "trial_loaded_module_name=none" in trial_script
    assert "trial_requested_module_loaded=no" in trial_script
    assert "trial_playback_pump=no" in trial_script
    assert "trial_playback_valid=no" in trial_script
    assert "trial_screen_present=no" in trial_script
    assert "trial_playback_position_present=no" in trial_script
    assert "trial_playback_position_valid=no" in trial_script
    assert "trial_playback_position_geometry_valid=no" in trial_script
    assert "trial_post_playback_status_present=no" in trial_script
    assert "trial_post_playback_status_valid=no" in trial_script
    assert "trial_post_playback_status_geometry_valid=no" in trial_script
    assert "trial_audio_unavailable=no" in trial_script
    assert "grep '^Module: ' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "grep '^Playback pump: .* stop=' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "grep '^Playback pump: blocks=[1-9][0-9]* frames=[1-9][0-9]* accepted=[1-9][0-9]* checksum=[1-9][0-9]* .* stop=' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "grep '^PCM source: ' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "grep '^Decoder route: ' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "grep '^Screen present: ' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "grep '^Screen present: reason=playback-position ' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "grep '^Screen present: reason=playback-position .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "grep '^Screen present: reason=post-playback-status .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "trial_playback_line=$(grep '^Playback pump: .* stop=' \"$IPLAY_TRIAL_LOG\" | tail -n 1)" in trial_script
    assert "trial_screen_reasons=$(grep '^Screen present: ' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "trial_result=bounded-ui-playback-ok" in trial_script
    assert "trial_result=source-ended-ui-ok" in trial_script
    assert "trial_result=module-size-mismatch" in trial_script
    assert "trial_result=loader-metadata-invalid" in trial_script
    assert "trial_result=decoder-route-missing" in trial_script
    assert "trial_result=decoder-handoff-missing" in trial_script
    assert "trial_result=pcm-source-missing" in trial_script
    assert "trial_result=playback-pump-invalid" in trial_script
    assert "trial_result=screen-evidence-invalid" in trial_script
    assert "trial_result=post-screen-evidence-invalid" in trial_script
    assert "trial_result=requested-module-not-loaded" in trial_script
    assert "trial_result=playback-without-screen" in trial_script
    assert "trial_result=audio-unavailable" in trial_script
    assert "trial_result=quiet-completed-no-diagnostics" in trial_script
    assert "trial_result=kvikdos-timeout" in trial_script
    assert "trial_result=exited-without-playback-pump" in trial_script
    assert "trial_result=failed" in trial_script
    assert "trial_failure_reason=none" in trial_script
    assert "trial_failure_reason=audio-pump-without-valid-screen-present" in trial_script
    assert "trial_failure_reason=module-size-mismatch" in trial_script
    assert "trial_failure_reason=loader-metadata-invalid" in trial_script
    assert "trial_failure_reason=decoder-route-missing" in trial_script
    assert "trial_failure_reason=decoder-handoff-missing" in trial_script
    assert "trial_failure_reason=pcm-source-missing" in trial_script
    assert "trial_failure_reason=playback-pump-invalid" in trial_script
    assert "trial_failure_reason=screen-evidence-invalid" in trial_script
    assert "trial_failure_reason=post-screen-evidence-invalid" in trial_script
    assert "trial_failure_reason=requested-module-not-loaded" in trial_script
    assert "trial_failure_reason=sb16-audio-unavailable" in trial_script
    assert "trial_failure_reason=emulator-timeout" in trial_script
    assert "trial_failure_reason=no-playback-pump-evidence" in trial_script
    assert "trial_failure_reason=player-process-failed" in trial_script
    assert "trial_failure_reason=unknown" in trial_script
    assert "trial_result=bounded-ui-playback-ok" in gate_script
    assert "trial_loaded_module_name=%s" in gate_script
    assert "trial_loaded_module_key=%s" in gate_script
    assert "trial_requested_module_key=%s" in gate_script
    assert "trial_requested_module_loaded=yes" in gate_script
    assert "trial_result=source-ended-ui-ok" in gate_script
    assert "trial_result=module-size-mismatch" in gate_script
    assert "trial_result=loader-metadata-invalid" in gate_script
    assert "trial_result=decoder-route-missing" in gate_script
    assert "trial_result=decoder-handoff-missing" in gate_script
    assert "trial_result=pcm-source-missing" in gate_script
    assert "trial_result=playback-pump-invalid" in gate_script
    assert "trial_result=screen-evidence-invalid" in gate_script
    assert "trial_result=post-screen-evidence-invalid" in gate_script
    assert "trial_result=requested-module-not-loaded" in gate_script
    assert "trial_result=playback-without-screen" in gate_script
    assert "trial_result=audio-unavailable" in gate_script
    assert "trial_result=quiet-completed-no-diagnostics" in gate_script
    assert "trial_result=kvikdos-timeout" in gate_script
    assert "trial_result=exited-without-playback-pump" in gate_script
    assert "trial_result=failed" in gate_script
    assert "trial_pcm_source_line=%s" in gate_script
    assert "trial_pcm_provider=%s" in gate_script
    assert "trial_pcm_renderer=%s" in gate_script
    assert "trial_pcm_route=%s" in gate_script
    assert "trial_pcm_input=%s" in gate_script
    assert "trial_pcm_truncated=%s" in gate_script
    assert "trial_pcm_hook_provider=%s" in gate_script
    assert "trial_pcm_stream_start=%s" in gate_script
    assert "trial_decoder_route_line=%s" in gate_script
    assert "trial_decoder_route_id=%s" in gate_script
    assert "trial_decoder_route_name=%s" in gate_script
    assert "trial_decoder_handoff_line=%s" in gate_script
    assert "trial_decoder_handoff_line=none" in gate_script
    assert "trial_decoder_handoff=%s" in gate_script
    assert "trial_decoder_handoff=none" in gate_script
    assert "host_module_size=$(wc -c < \"$host_module\" | tr -d ' ')" in gate_script
    assert "printf 'host_module_size=%s\\n' \"$host_module_size\"" in gate_script
    assert "trial_module_size=%s" in gate_script
    assert "trial_module_size=none" in gate_script
    assert "trial_module_size_matches_host=yes" in gate_script
    assert "trial_module_size_matches_host=no" in gate_script
    assert "trial_module_loader_line=%s" in gate_script
    assert "trial_module_loader_line=none" in gate_script
    assert "trial_module_loader=%s" in gate_script
    assert "trial_module_loader=none" in gate_script
    assert "trial_module_type_tag=%s" in gate_script
    assert "trial_module_type_tag=none" in gate_script
    assert "trial_module_title=%s" in gate_script
    assert "trial_module_title=none" in gate_script
    assert "trial_ok_loader_metadata=yes" in gate_script
    assert "trial_ok_loader_metadata=no" in gate_script
    assert "trial_failure_reason=loader-metadata-invalid" in gate_script
    assert "trial_failure_reason=module-size-mismatch" in gate_script
    assert "trial_failure_reason=decoder-route-missing" in gate_script
    assert "trial_failure_reason=decoder-handoff-missing" in gate_script
    assert "trial_failure_reason=pcm-source-missing" in gate_script
    assert "trial_failure_reason=playback-pump-invalid" in gate_script
    assert "trial_failure_reason=screen-evidence-invalid" in gate_script
    assert "trial_failure_reason=post-screen-evidence-invalid" in gate_script
    assert "trial_failure_reason=requested-module-not-loaded" in gate_script
    assert "rewrite/try_player.sh must record the final DOS loader line from trial output" in gate_script
    assert "rewrite/try_player.sh must record the final DOS module size without its prefix/suffix" in gate_script
    assert "rewrite/try_player.sh must compare DOS-reported module size against the host file size" in gate_script
    assert "rewrite/try_player.sh module-size-mismatch must require explicit host/DOS size mismatch evidence" in gate_script
    assert "rewrite/try_player.sh OK classifications must require DOS module size to match host file size" in gate_script
    assert "rewrite/try_player.sh must fail the command when a loaded module size differs from the host file" in gate_script
    assert "rewrite/try_player.sh must strip the Loader prefix before writing trial_module_loader" in gate_script
    assert "rewrite/try_player.sh must record the final DOS module type tag without its prefix" in gate_script
    assert "rewrite/try_player.sh must record the final DOS module title without its prefix when present" in gate_script
    assert "rewrite/try_player.sh must accept loader metadata only when loader is present and module type tag is nonzero 8-digit hex" in gate_script
    assert "rewrite/try_player.sh must fail the command when a loaded module has invalid loader metadata" in gate_script
    assert "rewrite/try_player.sh must fail the command when a loaded module lacks decoder route evidence" in gate_script
    assert "rewrite/try_player.sh decoder-route-missing must require missing route line/id/name evidence" in gate_script
    assert "rewrite/try_player.sh must fail the command when a loaded module lacks decoder handoff evidence" in gate_script
    assert "rewrite/try_player.sh must fail the command when a loaded module lacks PCM source evidence" in gate_script
    assert "rewrite/try_player.sh pcm-source-missing must require missing PCM source/provider/input/hook-provider/stream-start evidence" in gate_script
    assert "rewrite/try_player.sh must fail the command when a loaded module has invalid playback-pump evidence" in gate_script
    assert "rewrite/try_player.sh playback-pump-invalid must require a present but invalid playback-pump summary" in gate_script
    assert "rewrite/try_player.sh must fail the command when a loaded module has invalid playback screen evidence" in gate_script
    assert "rewrite/try_player.sh screen-evidence-invalid must require valid playback with invalid playback-position screen evidence" in gate_script
    assert "rewrite/try_player.sh must fail the command when bounded playback has invalid post-playback screen evidence" in gate_script
    assert "rewrite/try_player.sh must fail the command when the requested module was not loaded" in gate_script
    assert "rewrite/try_player.sh requested-module-not-loaded must require requested-module mismatch evidence" in gate_script
    assert "rewrite/try_player.sh post-screen-evidence-invalid must require bounded playback with invalid post-playback screen evidence" in gate_script
    assert "trial_result=loader-metadata-invalid\n  trial_script_exit_status=4" in gate_script
    assert "trial_module_size=$(grep '^Size: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | sed 's/^Size: //; s/ bytes$//')" in gate_script
    assert "[ \"$trial_module_size\" = \"$host_module_size\" ]" in gate_script
    assert "grep '^trial_module_size_matches_host=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in gate_script
    assert "trial_result=module-size-mismatch\n  trial_script_exit_status=4" in gate_script
    assert "grep '^trial_module_size_matches_host=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in gate_script
    assert "trial_result=decoder-route-missing\n  trial_script_exit_status=4" in gate_script
    assert "grep '^trial_decoder_route_line=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_decoder_route_id=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_decoder_route_name=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in gate_script
    assert "trial_result=decoder-handoff-missing\n  trial_script_exit_status=4" in gate_script
    assert "trial_result=pcm-source-missing\n  trial_script_exit_status=4" in gate_script
    assert "grep '^trial_pcm_source_line=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_pcm_provider=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_pcm_input=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_pcm_hook_provider=missing$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_pcm_stream_start=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in gate_script
    assert "trial_result=playback-pump-invalid\n  trial_script_exit_status=4" in gate_script
    assert "grep '^trial_playback_pump=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_playback_valid=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in gate_script
    assert "trial_result=screen-evidence-invalid\n  trial_script_exit_status=4" in gate_script
    assert "grep '^trial_playback_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && { grep '^trial_playback_position_valid=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_playback_position_geometry_valid=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1; }" in gate_script
    assert "trial_result=post-screen-evidence-invalid\n  trial_script_exit_status=4" in gate_script
    assert "trial_result=requested-module-not-loaded\n  trial_script_exit_status=4" in gate_script
    assert "grep '^trial_requested_module_loaded=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in gate_script
    assert "grep '^trial_playback_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^Playback pump: .* stop=block-limit' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && { grep '^trial_post_playback_status_valid=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 || grep '^trial_post_playback_status_geometry_valid=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1; }" in gate_script
    assert "trial_module_loader_line=$(grep '^Loader: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1)" in gate_script
    assert "trial_module_loader=$(printf '%s\\n' \"$trial_module_loader_line\" | sed 's/^Loader: //')" in gate_script
    assert "trial_module_type_tag=$(grep '^Module type tag: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | sed 's/^Module type tag: //')" in gate_script
    assert "trial_module_title=$(grep '^Title: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1 | sed 's/^Title: //')" in gate_script
    assert "grep '^trial_module_type_tag=[0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F]$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in gate_script
    assert "! grep '^trial_module_type_tag=00000000$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in gate_script
    assert "trial_playback_position_present=yes" in gate_script
    assert "trial_playback_valid=yes" in gate_script
    assert "trial_playback_position_valid=yes" in gate_script
    assert "trial_post_playback_status_present=yes" in gate_script
    assert "trial_post_playback_status_valid=yes" in gate_script
    assert "grep '^Size: ' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "grep '^Loader: ' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "grep '^Module type tag: ' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "grep '^Title: ' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "grep '^PCM source: ' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "grep '^Decoder route: ' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "grep '^Decoder handoff: ' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "grep '^Playback pump: blocks=[1-9][0-9]* frames=[1-9][0-9]* accepted=[1-9][0-9]* checksum=[1-9][0-9]* .* stop=' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "grep '^Screen present: reason=playback-position ' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "grep '^Screen present: reason=playback-position .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "grep '^Screen present: reason=post-playback-status .* screen_checksum=[1-9][0-9]* screen_nonblank=[1-9][0-9]* .* audio_frames=[1-9][0-9]* ' \"$IPLAY_TRIAL_LOG\"" in gate_script
    assert "rewrite/try_player.sh must classify trial result" in gate_script
    assert "rewrite/try_player.sh must write trial summary marker" in gate_script
    assert "rewrite/try_player.sh must derive trial summary from DOS output" in gate_script
    assert "rewrite/try_player.sh OK results must require loaded-module evidence before playback/UI evidence" in gate_script
    assert 'grep \'^trial_requested_module_loaded=yes$\' \\"$IPLAY_TRIAL_LOG\\" >/dev/null 2>&1 && grep \'^trial_playback_valid=yes$\' \\"$IPLAY_TRIAL_LOG\\" >/dev/null 2>&1' in gate_script
    assert "rewrite/try_player.sh must record the loaded module name from DOS output" in gate_script
    assert "rewrite/try_player.sh must normalize loaded module name for DOS-style comparison" in gate_script
    assert "rewrite/try_player.sh must normalize requested module name for DOS-style comparison" in gate_script
    assert "rewrite/try_player.sh OK results must require normalized requested module name to match loaded module" in gate_script
    assert "rewrite/try_player.sh OK classifications must use requested-module match marker" in gate_script
    assert "trial_module_loader_line=%s" in trial_script
    assert "trial_module_loader=none" in trial_script
    assert "trial_module_type_tag=%s" in trial_script
    assert "trial_module_type_tag=none" in trial_script
    assert "trial_ok_loader_metadata=0" in trial_script
    assert "trial_ok_loader_metadata=1" in trial_script
    assert "trial_ok_loader_metadata=yes" in trial_script
    assert "trial_ok_loader_metadata=no" in trial_script
    assert '[ "$trial_ok_loader_metadata" = "1" ]' in trial_script
    assert "grep '^trial_ok_loader_metadata=no$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "! grep '^trial_module_type_tag=00000000$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "rewrite/try_player.sh OK classifications must require selected video geometry on playback-position screen" in gate_script
    assert "rewrite/try_player.sh bounded OK classification must require selected video geometry on post-playback-status screen" in gate_script
    assert "rewrite/try_player.sh must derive playback-position geometry validity from selected trial video mode" in gate_script
    assert "rewrite/try_player.sh must derive post-playback-status geometry validity from selected trial video mode" in gate_script
    assert 'trial_loaded_module_name=$(grep \'^Module: \' "$IPLAY_TRIAL_LOG" | tail -n 1 | sed \'s/^Module: //\')' in trial_script
    assert "trial_loaded_module_key=$(printf '%s' \"$trial_loaded_module_name\" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" in trial_script
    assert "trial_requested_module_key=$(printf '%s' \"$name\" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" in trial_script
    assert '[ "$trial_loaded_module_key" = "$trial_requested_module_key" ]' in trial_script
    assert "grep '^trial_requested_module_loaded=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^trial_playback_position_geometry_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^trial_post_playback_status_geometry_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert 'grep "^Screen present: reason=playback-position .* cols=$trial_video_cols rows=$trial_video_rows " "$IPLAY_TRIAL_LOG" >/dev/null 2>&1' in trial_script
    assert 'grep "^Screen present: reason=post-playback-status .* cols=$trial_video_cols rows=$trial_video_rows " "$IPLAY_TRIAL_LOG" >/dev/null 2>&1' in trial_script
    assert "rewrite/try_player.sh OK results must require PCM source provider/route/input/stream-start evidence" in gate_script
    assert "rewrite/try_player.sh OK results must require decoder route id/name evidence" in gate_script
    assert 'if "! grep \'^trial_pcm_provider=none$\' \\"$IPLAY_TRIAL_LOG\\" >/dev/null 2>&1 && ! grep \'^trial_pcm_input=none$\' \\"$IPLAY_TRIAL_LOG\\" >/dev/null 2>&1 && ! grep \'^trial_pcm_hook_provider=missing$\' \\"$IPLAY_TRIAL_LOG\\" >/dev/null 2>&1 && ! grep \'^trial_pcm_stream_start=none$\' \\"$IPLAY_TRIAL_LOG\\" >/dev/null 2>&1" not in trial:' in gate_script
    assert 'if "grep \'^trial_decoder_route_id=[0-9][0-9]*$\' \\"$IPLAY_TRIAL_LOG\\" >/dev/null 2>&1 && ! grep \'^trial_decoder_route_name=none$\' \\"$IPLAY_TRIAL_LOG\\" >/dev/null 2>&1" not in trial:' in gate_script
    assert "rewrite/try_player.sh OK results must require decoder handoff evidence" in gate_script
    assert "rewrite/try_player.sh must record the final DOS decoder handoff line" in gate_script
    assert "rewrite/try_player.sh must strip the Decoder handoff prefix before writing trial_decoder_handoff" in gate_script
    assert "grep '^trial_decoder_handoff=[^ ]' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_decoder_handoff=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in gate_script
    assert "trial_decoder_handoff_line=$(grep '^Decoder handoff: ' \"$IPLAY_TRIAL_LOG\" | tail -n 1)" in gate_script
    assert "trial_decoder_handoff=$(printf '%s\\n' \"$trial_decoder_handoff_line\" | sed 's/^Decoder handoff: //')" in gate_script
    assert "rewrite/try_player.sh source-ended OK result must require valid playback-position screen evidence after audio" in gate_script
    assert "rewrite/try_player.sh source-ended OK result must require valid post-playback status screen evidence after audio" in gate_script
    assert "rewrite/try_player.sh OK results must reject missing PCM input evidence" in gate_script
    assert "rewrite/try_player.sh OK results must reject missing PCM provider evidence" in gate_script
    assert "grep '^trial_decoder_route_id=[0-9][0-9]*$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "! grep '^trial_decoder_route_name=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^trial_playback_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_playback_position_geometry_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^trial_post_playback_status_geometry_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^trial_decoder_route_id=[0-9][0-9]*$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "! grep '^trial_pcm_provider=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_pcm_input=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_pcm_hook_provider=missing$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && ! grep '^trial_pcm_stream_start=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^Playback pump: .* stop=source-end' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "! grep '^trial_pcm_input=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "! grep '^trial_pcm_provider=none$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^Playback pump: .* stop=block-limit' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "grep '^Playback pump: .* stop=source-end' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "grep '^trial_playback_valid=yes$' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1 && grep '^Playback pump: .* stop=block-limit' \"$IPLAY_TRIAL_LOG\" >/dev/null 2>&1" in trial_script
    assert "grep '^Playback pump: .* stop=source-end' \"$IPLAY_TRIAL_LOG\"" in trial_script
    assert "KVIKDOS_SECONDS=${KVIKDOS_SECONDS:-3}" in trial_script
    assert '"$KVIKDOS_TIMEOUT" -k 1 "$KVIKDOS_SECONDS" "$KVIKDOS" "$IPLAY_TRIAL_EXE" "$@" >> "$IPLAY_TRIAL_LOG" 2>&1' in trial_script
    assert "host_module=%s dos_module=%s" in trial_script
    assert "kvikdos_timeout_seconds=%s" in trial_script
    assert "kvikdos_timeout=yes seconds=%s" in trial_script
    assert "trial_exe=%s exit_status=%s" in trial_script
    assert "trial_failure_reason=%s" in trial_script
    assert 'elif [ "$rc" -eq 0 ] && [ "$trial_loop_policy" = "continuous-quiet-wrapper" ] && grep \'^quiet_trial_completed=yes \' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then\n  trial_result=quiet-completed-no-diagnostics' in trial_script
    assert "trial_loop_policy=%s" in trial_script
    assert "trial_proof_scope=%s" in trial_script
    assert "trial_video_mode=%s cols=%s rows=%s" in trial_script
    assert "trial_script_exit_status=%s" in trial_script
    assert "trial_script_exit_status=4" in trial_script
    assert 'exit "$trial_script_exit_status"' in trial_script
    assert 'elif grep \'^trial_audio_unavailable=yes$\' "$IPLAY_TRIAL_LOG" >/dev/null 2>&1; then\n  trial_result=audio-unavailable\n  if [ "$rc" -eq 0 ]; then\n    trial_script_exit_status=4' in trial_script
    for field in ["provider", "renderer", "route", "input", "truncated", "hook_provider"]:
        assert f'case "$trial_pcm_source_line" in *" {field}="*)' in trial_script
    for field in ["id", "name"]:
        assert f'case "$trial_decoder_route_line" in *" {field}="*)' in trial_script
    assert "IPLAYTRY_EXE = BUILD_DIR / \"IPLAYTRY.EXE\"" in behavior
    assert 'timeout = 3 if exe == ORIGINAL_EXE else int(os.environ.get("IPLAY_KVIKDOS_TEST_TIMEOUT", "3"))' in behavior
    assert 'IPLAY_ORIGINAL_KVIKDOS_MAX_TIMEOUT' in behavior
    assert 'result = run_dos(IPLAYTRY_EXE, "ENDCONT.S3M", timeout=3)' in behavior
    assert 'timeout=int(os.environ.get("IPLAY_SMOKE_TEST_TIMEOUT", "45"))' in smoke_test
    assert "timeout=90" not in smoke_test
    assert "def test_try_player_rejects_invalid_video_mode_before_kvikdos(tmp_path: Path) -> None:" in smoke_test
    assert "def test_try_player_rejects_invalid_video_mode_after_module_before_kvikdos(tmp_path: Path) -> None:" in smoke_test
    assert "def test_try_player_rejects_blocks_for_non_diagnostic_modes_before_kvikdos(" in smoke_test
    assert "before_module: list[str]" in smoke_test
    assert "after_module: list[str]" in smoke_test
    assert "def test_try_player_validate_only_normalizes_video_modes_without_kvikdos(" in smoke_test
    assert "def test_try_player_validate_only_selects_production_real_sb16_player_without_kvikdos(tmp_path: Path) -> None:" in smoke_test
    assert "def test_try_player_validate_only_selects_production_after_module_without_kvikdos(tmp_path: Path) -> None:" in smoke_test
    assert "REAL_ARYX_MODULE = ROOT / \"samples\" / \"aryx.s3m\"" in smoke_test
    assert "def test_try_player_real_aryx_s3m_reports_bounded_ui_playback_ok(tmp_path: Path) -> None:" in smoke_test
    assert "def test_try_player_continuous_diagnostics_reports_source_ended_ui_ok(tmp_path: Path) -> None:" in smoke_test
    assert "def test_try_player_quiet_source_end_reports_quiet_completed_without_diagnostics(tmp_path: Path) -> None:" in smoke_test
    assert '[str(ROOT / "rewrite" / "try_player.sh"), "--continuous-diagnostics", "--video-mode=80x50", str(module)]' in smoke_test
    assert 'assert "trial_video_mode=80x50 cols=80 rows=50" in log' in smoke_test
    assert 'assert "trial_loaded_module_key=ARYX.S3M" in log' in smoke_test
    assert 'assert "trial_result=bounded-ui-playback-ok" in log' in smoke_test
    assert 'assert "trial_result=source-ended-ui-ok" in log' in smoke_test
    assert 'assert "trial_post_playback_status_geometry_valid=yes" in log' in smoke_test
    assert 'assert "trial_result=quiet-completed-no-diagnostics" in log' in smoke_test
    assert "option_after_module: bool" in smoke_test
    assert "from player_behavior_fixtures import write_endcont_module, write_smoke_modules" in smoke_test
    assert "import pytest" in smoke_test
    assert "write_smoke_modules(tmp_path)" in smoke_test
    assert "write_endcont_module(tmp_path)" in smoke_test
    assert 'module = tmp_path / "SMOKE.S3M"' in smoke_test
    assert 'module = tmp_path / "ENDCONT.S3M"' in smoke_test
    assert '[str(ROOT / "rewrite" / "try_player.sh"), str(module), "--video-mode=bad"]' in smoke_test
    assert '"IPLAY_TRIAL_LOG": str(trial_log)' in smoke_test
    assert '"IPLAY_TRIAL_VALIDATE_ONLY": "1"' in smoke_test
    assert 'assert result.returncode == 2' in smoke_test
    assert 'assert "try_player: unsupported video mode: bad" in result.stderr' in smoke_test
    assert '("80x25color", "trial_video_mode=80x25color cols=80 rows=25", False)' in smoke_test
    assert '("80x25", "trial_video_mode=80x25color cols=80 rows=25", True)' in smoke_test
    assert '("80x50project", "trial_video_mode=80x50 cols=80 rows=50", True)' in smoke_test
    assert 'trial_args = [str(module), f"--video-mode={mode}"] if option_after_module else [f"--video-mode={mode}", str(module)]' in smoke_test
    assert '[str(ROOT / "rewrite" / "try_player.sh"), *trial_args]' in smoke_test
    assert 'assert expected in result.stdout' in smoke_test
    assert 'assert "kvikdos" not in result.stdout + result.stderr' in smoke_test
    assert "assert not trial_log.exists()" in smoke_test
    assert "Player iteration no longer requires the full monolithic ABI/test-runner rebuild." in status
    assert "`rewrite/try_player.sh [--rebuild] [--modern|--native|--native-interactive|--native-source-end|--native-keyboard-after-one|--native-stdin-keyboard|--native-audio|--native-terminal|--native-live|--quiet|--diagnostics|--continuous-diagnostics|--hardware-diagnostics|--production] [--blocks=N] [--video-mode=MODE] <module-file|@file-list>` is the user-facing trial launcher." in status
    assert "The preferred SDL/notcurses host player path is `./iplay.sh samples/aryx.s3m`" in status
    assert "delegates to the implementation launcher `./rewrite/iplay.sh samples/aryx.s3m`" in status
    assert "Readiness checks are available through `./iplay.sh --check` or the implementation launcher `./rewrite/iplay.sh --check`" in status
    assert "starts it in no-playback extension-list mode to catch SDL/libmodplug link failures" in status
    assert "The quick playback proof command is `./iplay.sh --check-playback samples/aryx.s3m`" in status
    assert "controlled 40x25 geometry, SDL dummy audio by default, and a one-block keyboard-stop run" in status
    assert "includes the host C++ headers in its stale-build dependency check" in status
    assert "project-owned decoder-unavailable files such as deferred `.inr`" in status
    assert "the implementation launcher `./rewrite/iplay.sh --check-playback` has matching real-module and `@file-list` success coverage plus missing-module, missing file-list selection, corrupt known tracker, unsupported probe, project-owned decoder-unavailable, and SDL audio-open failure coverage" in status
    assert "Loaded-module status redraws call `iplay_runtime_draw_status_block(...)` before presenting" in status
    assert "refreshes the status block in-place before presenting" in status
    assert "Supported external-library tracker extensions can be listed without playback through `./iplay.sh --list-extensions`" in status
    assert "Decoder routing for a specific path can be checked without playback through `./iplay.sh --classify <path>`" in status
    assert "Forced rebuilds are available through `./rewrite/iplay.sh --rebuild samples/aryx.s3m`" in status
    assert "Raw evidence is available with `./rewrite/iplay.sh --diagnostics --video-mode=80x50 samples/aryx.s3m`" in status
    assert "The top-level diagnostics path `./iplay.sh --diagnostics --video-mode=80x50 <real-s3m>` is directly tested with SDL dummy audio" in status
    assert "proves the preferred command reaches selected 80x50 geometry, terminal render evidence, opened SDL-compatible SB16 16-bit stereo output, libmodplug route/provider evidence, stdin-keyboard status, and nonzero/changing live audio levels" in status
    assert "The lower-level diagnostic host binary remains `./rewrite/.build/iplay --video-mode=80x50 samples/aryx.s3m`" in status
    assert "passes the resolved absolute `@file-list` argument through to the selected native host executable (`iplay` for `--modern`, `iplay_native` for legacy native flags) so the native resolver is also tested" in status
    assert "finding the list file with DOS-style case-insensitive path matching" in status
    assert "./rewrite/.build/iplay --video-mode=80x50 samples/aryx.s3m" in status
    assert "./rewrite/try_player.sh --modern --video-mode=80x50 samples/aryx.s3m" in status
    assert "launches `rewrite/.build/iplay`" in status
    assert "Direct `iplay` defaults to modern SDL/notcurses playback without `--modern`, while `iplay_native` keeps probe-style defaults" in status
    assert "`--modern` builds/runs `rewrite/.build/iplay` with normal player arguments (`--video-mode=MODE <module>` or `<module> <video-mode>`)" in status
    assert "legacy native flags build/run `rewrite/.build/iplay_native` with explicit probe arguments such as `--source-end`, `--sdl-audio`, `--terminal-render`, `--terminal-live`, and `--stdin-keyboard`" in status
    assert "test_try_player_modern_alias_uses_iplay_default_player_args" in smoke_test
    assert "./rewrite/try_player.sh --native --blocks=1 --video-mode=80x50 samples/aryx.s3m" in status
    assert "trial_loop_policy=native-source-end" in status
    assert "trial_loop_policy=native-keyboard-stop" in status
    assert "trial_exe=... diagnostics=... hardware_diagnostics=... production=... rebuild=... needs_rebuild=..." in status
    assert "trial_exe=... diagnostics=... hardware_diagnostics=... rebuild=... needs_rebuild=..." not in status
    assert "trial_module_loaded=yes/no" in status
    assert "trial_requested_module_loaded=yes/no" in status
    assert "trial_playback_pump=yes/no" in status
    assert "trial_playback_valid=yes/no" in status
    assert "trial_audio_backend=..." in status
    assert "trial_audio_backend_valid=yes/no" in status
    assert "trial_audio_levels_valid=yes/no" in status
    assert "trial_audio_level_sequence_valid=yes/no" in status
    assert "trial_color_probe_valid=yes/no" in status
    assert "trial_resize_cycle_valid=yes/no" in status
    assert "trial_subwindow_valid=yes/no" in status
    assert "trial_screen_present=yes/no" in status
    assert "trial_playback_position_present=yes/no" in status
    assert "trial_playback_position_valid=yes/no" in status
    assert "trial_post_playback_status_present=yes/no" in status
    assert "trial_post_playback_status_valid=yes/no" in status
    assert "trial_playback_line=..." in status
    assert "trial_screen_reasons=..." in status
    assert "trial_pcm_provider=..." in status
    assert "trial_pcm_route=..." in status
    assert "trial_pcm_stream_start=..." in status
    assert "trial_decoder_route_name=..." in status
    assert "PCM source provider/route/input/stream-start evidence" in status
    assert "PCM source/provider/input/stream-start summary was missing" in status
    assert "PCM source/provider/input/stream-start absence for `pcm-source-missing`" in status
    assert "trial_audio_unavailable=yes/no" in status
    assert "trial_audio_unavailable_source=screen|exit-code|none" in status
    assert "trial_result=bounded-ui-playback-ok" in status
    assert "trial_result=playback-without-screen" in status
    assert "trial_result=audio-unavailable" in status
    assert "trial_result=quiet-completed-no-diagnostics" in status


def test_external_tracker_behavior_tests_do_not_pin_obsolete_voice_mixer_checksum() -> None:
    behavior = (ROOT / "tests" / "test_player_behavior.py").read_text()
    status = (ROOT / "rewrite" / "STATUS.md").read_text()

    assert "def assert_external_native_preview(output: str) -> None:" in behavior
    assert "test_iplayc_dos_real_aryx_s3m_reaches_external_tracker_native_preview" in behavior
    assert "test_iplayc_dos_mod_finetune_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_mod_finetune_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_sample_loop_reports_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_sample_loop_same_path_reports_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_16bit_sample_flag_reports_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_16bit_sample_flag_same_path_reports_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_c2spd_reports_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_c2spd_same_path_reports_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_fractional_interpolation_same_path_reports_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_volume_slide_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_volume_slide_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_extended_volume_slide_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_extended_volume_slide_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_global_volume_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_global_volume_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_header_global_volume_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_header_global_volume_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_master_volume_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_master_volume_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_global_volume_slide_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_global_volume_slide_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_channel_volume_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_channel_volume_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_channel_volume_slide_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_channel_volume_slide_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_header_channel_panning_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_default_pan_table_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_stereo_flag_reports_header_panning_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_volume_column_panning_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_volume_column_panning_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_volume_column_panning_command_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_note_byte_fe_reports_cut_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_note_byte_fe_same_path_reports_cut_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_sample_offset_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_sample_offset_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_pitch_slide_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_pitch_slide_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_extended_pitch_slide_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_extended_pitch_slide_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_tone_portamento_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_tone_portamento_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_tone_portamento_volume_slide_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_tone_portamento_volume_slide_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_arpeggio_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_arpeggio_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_retrigger_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_retrigger_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_extended_retrigger_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_extended_retrigger_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_vibrato_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_vibrato_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_fine_vibrato_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_fine_vibrato_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_vibrato_volume_slide_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_vibrato_volume_slide_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_tremolo_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_tremolo_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_tremor_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_tremor_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_note_cut_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_note_cut_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_note_delay_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_note_delay_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_unsigned_sample_format_reports_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_unsigned_sample_format_same_path_reports_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_panning_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_panning_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_panning_slide_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_panning_slide_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_panbrello_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_panbrello_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_extended_panning_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_extended_panning_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_pattern_break_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_pattern_break_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_position_jump_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_position_jump_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_pattern_loop_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_speed_command_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_speed_command_same_path_stays_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_tempo_command_reports_runtime_tempo_metadata_on_external_stream_placeholder" in behavior
    assert "test_iplayc_dos_s3m_tempo_command_same_path_reports_runtime_tempo_metadata_on_external_stream_placeholder" in behavior
    for name in [
        "test_iplayc_dos_mod_panning_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_pattern_break_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_position_jump_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_pattern_break_same_path_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_position_jump_same_path_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_tempo_command_reports_runtime_tempo_metadata_on_external_stream_placeholder",
        "test_iplayc_dos_mod_speed_command_same_path_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_tempo_command_same_path_reports_runtime_tempo_metadata_on_external_stream_placeholder",
        "test_iplayc_dos_mod_sample_loop_reports_metadata_on_external_stream_placeholder",
        "test_iplayc_dos_mod_sample_loop_same_path_reports_metadata_on_external_stream_placeholder",
        "test_iplayc_dos_mod_volume_command_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_volume_command_same_path_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_sample_offset_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_sample_offset_same_path_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_arpeggio_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_arpeggio_same_path_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_pitch_slide_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_pitch_slide_down_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_pitch_slide_same_path_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_tone_portamento_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_vibrato_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_tremolo_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_volume_slide_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_volume_slide_same_path_up_down_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_tone_portamento_volume_slide_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_vibrato_volume_slide_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_note_cut_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_extended_retrigger_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_extended_fine_pitch_slide_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_extended_finetune_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_extended_fine_volume_slide_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_note_delay_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_extended_panning_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_pattern_loop_stays_on_external_stream_placeholder",
        "test_iplayc_dos_mod_panning_same_path_stays_on_external_stream_placeholder",
    ]:
        assert name in behavior
    assert "def assert_external_native_preview(output: str) -> None:" in behavior
    assert "assert_external_native_preview(out)" in behavior
    assert 'route = assert_decoder_route(output, 0, "external-library")' in behavior
    assert 'route = assert_decoder_route(out, 0, "external-library")' in behavior
    assert 'assert_external_pcm_source(out, "s3m_module", "dos-fallback", 1, "file-path", 107)' in behavior
    assert 'assert_external_pcm_source(first, "mod_n_t_module", "dos-fallback", 1, "file-path", 1084)' in behavior
    assert "checksum=17638693" not in behavior
    assert "test_iplayc_dos_mod_finetune_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_mod_finetune_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_sample_loop_requires_loop_flag" not in behavior
    assert "test_iplayc_dos_s3m_sample_loop_same_path_requires_loop_flag" not in behavior
    assert "test_iplayc_dos_s3m_16bit_sample_flag_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_16bit_sample_flag_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_c2spd_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_c2spd_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_fractional_interpolation_same_path_uses_adjacent_sample" not in behavior
    assert "test_iplayc_dos_s3m_volume_slide_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_volume_slide_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_extended_volume_slide_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_extended_volume_slide_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_global_volume_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_global_volume_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_header_global_volume_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_header_global_volume_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_master_volume_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_master_volume_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_global_volume_slide_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_global_volume_slide_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_channel_volume_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_channel_volume_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_channel_volume_slide_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_channel_volume_slide_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_header_channel_panning_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_default_pan_table_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_stereo_flag_enables_header_panning" not in behavior
    assert "test_iplayc_dos_s3m_volume_column_panning_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_volume_column_panning_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_volume_column_panning_coexists_with_command" not in behavior
    assert "test_iplayc_dos_s3m_note_byte_fe_cuts_active_voice" not in behavior
    assert "test_iplayc_dos_s3m_note_byte_fe_same_path_cuts_active_voice" not in behavior
    assert "test_iplayc_dos_s3m_sample_offset_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_sample_offset_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_pitch_slide_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_pitch_slide_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_extended_pitch_slide_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_extended_pitch_slide_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_tone_portamento_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_tone_portamento_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_tone_portamento_volume_slide_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_tone_portamento_volume_slide_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_arpeggio_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_arpeggio_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_retrigger_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_retrigger_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_extended_retrigger_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_extended_retrigger_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_vibrato_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_vibrato_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_fine_vibrato_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_fine_vibrato_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_vibrato_volume_slide_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_vibrato_volume_slide_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_tremolo_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_tremolo_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_tremor_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_tremor_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_note_cut_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_note_cut_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_note_delay_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_note_delay_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_unsigned_sample_format_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_unsigned_sample_format_same_path_changes_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_panning_changes_stereo_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_panning_same_path_changes_stereo_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_panning_slide_changes_stereo_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_panning_slide_same_path_changes_stereo_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_panbrello_changes_stereo_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_panbrello_same_path_changes_stereo_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_extended_panning_changes_stereo_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_extended_panning_same_path_changes_stereo_pcm_output" not in behavior
    assert "test_iplayc_dos_s3m_pattern_break_changes_playback_flow" not in behavior
    assert "test_iplayc_dos_s3m_pattern_break_same_path_changes_playback_flow" not in behavior
    assert "test_iplayc_dos_s3m_position_jump_changes_playback_flow" not in behavior
    assert "test_iplayc_dos_s3m_position_jump_same_path_changes_playback_flow" not in behavior
    assert "test_iplayc_dos_s3m_pattern_loop_same_path_changes_playback_flow" not in behavior
    assert "test_iplayc_dos_s3m_speed_command_changes_row_timing" not in behavior
    assert "test_iplayc_dos_s3m_speed_command_same_path_changes_row_timing" not in behavior
    assert "test_iplayc_dos_s3m_tempo_command_updates_runtime_tempo_state" not in behavior
    assert "test_iplayc_dos_s3m_tempo_command_same_path_updates_runtime_tempo_state" not in behavior
    for name in [
        "test_iplayc_dos_mod_panning_changes_stereo_pcm_output",
        "test_iplayc_dos_mod_pattern_break_changes_playback_flow",
        "test_iplayc_dos_mod_position_jump_changes_playback_flow",
        "test_iplayc_dos_mod_pattern_break_same_path_changes_playback_flow",
        "test_iplayc_dos_mod_position_jump_same_path_changes_playback_flow",
        "test_iplayc_dos_mod_tempo_command_updates_runtime_tempo_state",
        "test_iplayc_dos_mod_speed_command_same_path_changes_row_timing",
        "test_iplayc_dos_mod_tempo_command_same_path_updates_runtime_tempo_state",
        "test_iplayc_dos_mod_sample_loop_changes_pcm_output",
        "test_iplayc_dos_mod_sample_loop_same_path_changes_pcm_output",
        "test_iplayc_dos_mod_volume_command_changes_pcm_output",
        "test_iplayc_dos_mod_volume_command_same_path_changes_pcm_output",
        "test_iplayc_dos_mod_sample_offset_changes_pcm_output",
        "test_iplayc_dos_mod_sample_offset_same_path_changes_pcm_output",
        "test_iplayc_dos_mod_arpeggio_changes_pcm_output",
        "test_iplayc_dos_mod_arpeggio_same_path_changes_pcm_output",
        "test_iplayc_dos_mod_pitch_slide_changes_pcm_output",
        "test_iplayc_dos_mod_pitch_slide_down_changes_pcm_output",
        "test_iplayc_dos_mod_pitch_slide_same_path_changes_pcm_output",
        "test_iplayc_dos_mod_tone_portamento_changes_pcm_output",
        "test_iplayc_dos_mod_vibrato_changes_pcm_output",
        "test_iplayc_dos_mod_tremolo_changes_pcm_output",
        "test_iplayc_dos_mod_volume_slide_changes_pcm_output",
        "test_iplayc_dos_mod_volume_slide_same_path_up_down_changes_pcm_output",
        "test_iplayc_dos_mod_tone_portamento_volume_slide_changes_pcm_output",
        "test_iplayc_dos_mod_vibrato_volume_slide_changes_pcm_output",
        "test_iplayc_dos_mod_note_cut_changes_pcm_output",
        "test_iplayc_dos_mod_extended_retrigger_changes_pcm_output",
        "test_iplayc_dos_mod_extended_fine_pitch_slide_changes_pcm_output",
        "test_iplayc_dos_mod_extended_finetune_changes_pcm_output",
        "test_iplayc_dos_mod_extended_fine_volume_slide_changes_pcm_output",
        "test_iplayc_dos_mod_note_delay_changes_pcm_output",
        "test_iplayc_dos_mod_extended_panning_changes_pcm_output",
        "test_iplayc_dos_mod_pattern_loop_changes_playback_flow",
        "test_iplayc_dos_mod_panning_same_path_changes_pcm_output",
    ]:
        assert name not in behavior
    assert "Complete in-memory tracker modules can use the existing DOS native preview mixer" in status
    assert "capped-header/file-path modules still use the bounded byte-stream placeholder" in status
    assert "Complete external tracker diagnostics report `provider=native-preview`" in status
    assert "effect-level PCM is expected to come from the host/modern libmodplug/libxmp/libopenmpt path" in status
    assert "The DOS S3M/MOD mixer now applies" not in status
    assert "The DOS S3M mixer now applies" not in status
    assert "The DOS S3M mixer now also" not in status
    assert "MOD sample loop points now have direct DOS behavior coverage" not in status
    assert "MOD volume commands now have direct DOS behavior coverage" not in status
    coverage = (ROOT / "tests" / "COVERAGE.md").read_text()
    assert "tracker-format effect decoding is intentionally on the external-library boundary" in coverage
    assert "route/provider playback evidence" in coverage
    assert "installed-but-unavailable fallback behavior" in coverage
    assert "./iplay.sh --diagnostics --video-mode=80x50" in coverage
    assert "nonzero/changing live audio level summary through the top-level launcher" in coverage
    assert "partial current-only MOD/S3M playback coverage: voice state, PCM checksum" not in coverage
