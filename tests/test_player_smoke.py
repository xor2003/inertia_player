import os
import fcntl
import pty
import re
import select
import signal
import struct
import subprocess
import sys
import termios
import time
from pathlib import Path

import pytest

from player_behavior_fixtures import write_endcont_module, write_smoke_modules


ROOT = Path(__file__).resolve().parents[1]
REAL_ARYX_MODULE = ROOT / "samples" / "aryx.s3m"


def test_try_player_help_explains_kvikdos_default_and_real_sb16_production() -> None:
    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--help"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "runs bounded IPLAYDIAG.EXE diagnostics under kvikdos by default" in result.stderr
    assert "default mode is the safe kvikdos proof path" in result.stderr
    assert "use --production only on real SB16-capable DOS or when checking the SB16-unavailable exit" in result.stderr
    assert "--native runs rewrite/.build/iplay_native directly on the host" in result.stderr
    assert "--modern runs rewrite/.build/iplay, the preferred SDL/notcurses host player" in result.stderr
    assert "direct SDL/notcurses player example: ./rewrite/iplay.sh <module-file>" in result.stderr
    assert "./rewrite/iplay.sh --diagnostics --video-mode=80x50 <module-file> for raw evidence" in result.stderr
    assert "--native-source-end runs the native host path until libmikmod reports natural source end" in result.stderr
    assert "--native-keyboard-after-one runs the native host path until the keyboard/interactive stop seam fires after one block" in result.stderr
    assert "--production runs IPLAYC.EXE, the quiet production real-SB16 DOS player" in result.stderr
    assert "kvikdos_timeout" not in result.stdout + result.stderr


def test_iplayc_dos_player_smoke() -> None:
    result = subprocess.run(
        [str(ROOT / "rewrite" / "smoke_player.sh")],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=int(os.environ.get("IPLAY_SMOKE_TEST_TIMEOUT", "45")),
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "IPLAYTRY/IPLAYCONT/IPLAYHW/IPLAYDIAG/TEXTMODE smoke ok" in result.stdout


def test_try_player_rejects_invalid_video_mode_before_kvikdos(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--video-mode=bad", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=5,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "try_player: unsupported video mode: bad" in result.stderr
    assert "kvikdos" not in result.stdout + result.stderr
    assert not trial_log.exists()


def test_try_player_rejects_invalid_video_mode_after_module_before_kvikdos(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), str(module), "--video-mode=bad"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=5,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "try_player: unsupported video mode: bad" in result.stderr
    assert "kvikdos" not in result.stdout + result.stderr
    assert not trial_log.exists()


@pytest.mark.parametrize(
    ("before_module", "after_module"),
    [
        (["--production", "--blocks=1"], []),
        (["--quiet", "--blocks=1"], []),
        (["--blocks=1", "--production"], []),
        (["--blocks=1", "--quiet"], []),
        ([], ["--production", "--blocks=1"]),
        ([], ["--quiet", "--blocks=1"]),
        ([], ["--blocks=1", "--production"]),
        ([], ["--blocks=1", "--quiet"]),
    ],
)
def test_try_player_rejects_blocks_for_non_diagnostic_modes_before_kvikdos(
    tmp_path: Path,
    before_module: list[str],
    after_module: list[str],
) -> None:
    trial_log = tmp_path / "RES.TXT"
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), *before_module, str(module), *after_module],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log), "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "try_player: --blocks=N is only supported with diagnostic trial modes" in result.stderr
    assert "kvikdos" not in result.stdout + result.stderr
    assert not trial_log.exists()


def test_try_player_resolves_module_path_case_like_dos_before_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    original = tmp_path / "SMOKE.S3M"
    lowercase = tmp_path / "smoke.s3m"
    original.rename(lowercase)
    requested = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), str(requested)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert f"try_player: resolved DOS-style case-insensitive module path: {requested} -> {lowercase}" in result.stderr
    assert "trial_exe=IPLAYDIAG.EXE diagnostics=1 hardware_diagnostics=0 production=0" in result.stdout
    assert "dos_args=--blocks=32 smoke.s3m" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_selects_production_real_sb16_player_without_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--production", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=IPLAYC.EXE diagnostics=0 hardware_diagnostics=0 production=1" in result.stdout
    assert "dos_args=SMOKE.S3M" in result.stdout
    assert "--blocks=32" not in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_selects_native_sdl_notcurses_without_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", "--video-mode=80x50", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1 native_source_end=0 native_keyboard_after_one=0" in result.stdout
    assert "dos_args=--blocks=1 --video-mode=80x50 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_filelist_selects_first_trimmed_entry_without_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"
    filelist = tmp_path / "PLAYLIST.LST"
    filelist.write_text("\n\t SMOKE.S3M \r\nignored.s3m\n")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", "@" + str(filelist)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert f"trial_filelist_arg=@{filelist}" in result.stdout
    assert f"trial_filelist_path={filelist}" in result.stdout
    assert "trial_filelist_selected=SMOKE.S3M" in result.stdout
    assert f"trial_filelist_selected_host={module}" in result.stdout
    assert "dos_args=--blocks=1 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_resolves_filelist_path_case_like_dos_without_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"
    filelist = tmp_path / "playlist.lst"
    requested = tmp_path / "PLAYLIST.LST"
    filelist.write_text("SMOKE.S3M\n")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", "@" + str(requested)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert f"try_player: resolved DOS-style case-insensitive file-list path: {requested} -> {filelist}" in result.stderr
    assert f"trial_filelist_arg=@{requested}" in result.stdout
    assert f"trial_filelist_path={filelist}" in result.stdout
    assert "trial_filelist_selected=SMOKE.S3M" in result.stdout
    assert f"trial_filelist_selected_host={module}" in result.stdout
    assert "dos_args=--blocks=1 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_selects_native_keyboard_stop_without_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-keyboard-after-one", "--video-mode=40x25", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1 native_source_end=0 native_keyboard_after_one=1" in result.stdout
    assert "trial_video_mode=40x25color cols=40 rows=25" in result.stdout
    assert "dos_args=--video-mode=40x25 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_selects_native_source_end_without_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-source-end", "--video-mode=80x25", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1 native_source_end=1 native_keyboard_after_one=0" in result.stdout
    assert "trial_video_mode=80x25color cols=80 rows=25" in result.stdout
    assert "dos_args=--video-mode=80x25 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_selects_native_sdl_audio_without_kvikdos(tmp_path: Path) -> None:
    module = tmp_path / "SMOKE.S3M"
    module.write_bytes(b"SCRM" + b"\0" * 96)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-audio", "--blocks=1", "--video-mode=80x25", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1" in result.stdout
    assert "trial_video_mode=80x25color cols=80 rows=25" in result.stdout
    assert "dos_args=--blocks=1 --video-mode=80x25 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_selects_native_interactive_without_kvikdos(tmp_path: Path) -> None:
    module = tmp_path / "SMOKE.S3M"
    module.write_bytes(b"SCRM" + b"\0" * 96)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-interactive", "--video-mode=80x50", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1 native_source_end=1 native_keyboard_after_one=0 native_stdin_keyboard=1 native_audio=1 native_terminal=1 native_live=1 native_modern=0" in result.stdout
    assert "trial_video_mode=80x50 cols=80 rows=50" in result.stdout
    assert "dos_args=--video-mode=80x50 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_selects_modern_alias_after_module_without_kvikdos(tmp_path: Path) -> None:
    module = tmp_path / "SMOKE.S3M"
    module.write_bytes(b"SCRM" + b"\0" * 96)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), str(module), "--modern", "--video-mode=80x50"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=iplay diagnostics=0 hardware_diagnostics=0 production=0 native=1 native_source_end=1 native_keyboard_after_one=0 native_stdin_keyboard=1 native_audio=1 native_terminal=1 native_live=1 native_modern=1" in result.stdout
    assert "trial_video_mode=80x50 cols=80 rows=50" in result.stdout
    assert "dos_args=--video-mode=80x50 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_native_interactive_defaults_to_terminal_size_without_kvikdos(tmp_path: Path) -> None:
    module = tmp_path / "SMOKE.S3M"
    module.write_bytes(b"SCRM" + b"\0" * 96)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-interactive", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1", "COLUMNS": "80", "LINES": "50"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1 native_source_end=1 native_keyboard_after_one=0 native_stdin_keyboard=1 native_audio=1 native_terminal=1 native_live=1" in result.stdout
    assert "trial_video_mode=80x50 cols=80 rows=50" in result.stdout
    assert "dos_args=SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_native_interactive_alias_combines_audio_terminal_live_keyboard(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-interactive", "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log), "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "native_stdin_keyboard=1 native_audio=1 native_terminal=1 native_live=1" in log
    assert "trial_loop_policy=native-interactive-source-end-keyboard-stop" in log
    assert "try_player native interactive: module=" in log
    assert "try_player native interactive: module=" in result.stdout
    assert "mode=40x25color cols=40 rows=25" in result.stdout
    assert "stop_keys=q,Q,Esc" in result.stdout
    assert "native_args=" in log and " --source-end " in log
    assert " --sdl-audio --terminal-render --terminal-live --stdin-keyboard" in log
    assert "SDL audio sink: requested=1 opened=1" in log
    assert "Terminal live summary: requested=1" in log
    assert "Terminal live summary: requested=1" in result.stdout
    assert result.stdout.count("Terminal live summary: requested=1") == 1
    assert "Stdin keyboard: requested=1 stopped=1" in log
    assert "Stdin keyboard: requested=1 stopped=1" in result.stdout
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" in log
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" in result.stdout
    assert "trial_result=native-sdl-notcurses-ok" in log
    assert "trial_result=native-sdl-notcurses-ok" in result.stdout
    assert result.stdout.count("trial_result=native-sdl-notcurses-ok") == 1


def test_try_player_modern_alias_uses_iplay_default_player_args(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--modern", "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log), "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "trial_exe=iplay diagnostics=0 hardware_diagnostics=0 production=0 native=1" in log
    assert "native_modern=1" in log
    assert "try_player modern: module=" in log
    assert "try_player modern: module=" in result.stdout
    assert "try_player native interactive: module=" not in log
    assert f"native_args={module} 40x25color" in log
    assert "--source-end" not in log
    assert "--sdl-audio" not in log
    assert "--terminal-render" not in log
    assert "--terminal-live" not in log
    assert "--stdin-keyboard" not in log
    assert "SDL audio sink: requested=1 opened=1" in log
    assert "Terminal live summary: requested=1" in log
    assert "Stdin keyboard: requested=1 stopped=1" in log
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" in log
    assert "trial_result=native-sdl-notcurses-ok" in log


def test_top_level_iplay_diagnostics_proves_sdl_notcurses_sb16_path(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--diagnostics", "--video-mode=80x50", str(module)],
        cwd=ROOT,
        input="",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Module: aryx.s3m" in result.stdout
    assert "Selected text mode: 80x50 cols=80 rows=50" in result.stdout
    assert "Terminal render: requested=1 cols=80 rows=50 bytes=8000" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout
    assert "Terminal live summary: requested=1" in result.stdout
    assert "Stdin keyboard: requested=1 stopped=0" in result.stdout
    assert "Playback output: SDL-compatible SB16 16-bit stereo native." in result.stdout
    assert "Inertia Player V1.22" in result.stdout
    assert "Current Track" in result.stdout
    assert "Track Position" in result.stdout
    assert "Sound Blaster 16" in result.stdout
    assert "Terminal live: block=" not in result.stdout
    assert "Audio backend: SDL-compatible SB16 16-bit stereo" in result.stdout
    assert "Decoder route: id=0 name=external-library" in result.stdout
    assert "PCM source: external_module seed=1 truncated=0 input=file-path renderer=e route=0 provider=libmikmod hook_provider=libmikmod stream_start=0" in result.stdout
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in result.stdout
    live = re.search(r"Terminal live summary: requested=1 samples=(\d+) nonzero=(\d+) changed=(\d+)", result.stdout)
    assert live, result.stdout + result.stderr
    samples, nonzero, changed = (int(group) for group in live.groups())
    assert samples > 1
    assert nonzero > 0
    assert changed == 1


@pytest.mark.parametrize("extension", [".MOD", ".XM", ".IT", ".XYZ"])
def test_top_level_iplay_diagnostics_plays_valid_tracker_under_library_route(tmp_path: Path, extension: str) -> None:
    module = tmp_path / ("aryx" + extension)
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--diagnostics", "--video-mode=80x25", str(module)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert f"Module: aryx{extension}" in result.stdout
    assert "Selected text mode: 80x25 cols=80 rows=25" in result.stdout
    if extension == ".XYZ":
        assert "Loader: probe_by_content (libmodplug content probe)" in result.stdout
    assert "Decoder route: id=0 name=external-library" in result.stdout
    provider = "libmodplug" if extension == ".MOD" else "libmikmod"
    assert f"PCM source: external_module seed=1 truncated=0 input=file-path renderer=e route=0 provider={provider} hook_provider={provider} stream_start=0" in result.stdout
    assert "Playback output: SDL-compatible SB16 16-bit stereo native." in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout
    assert f"status=keyboard route_id=0 route=external-library provider={provider} stop=keyboard" in result.stdout


def test_top_level_iplay_environment_diagnostics_preserves_sdl_notcurses_evidence(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "IPLAY_LAUNCHER_DIAGNOSTICS": "1"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Module: aryx.s3m" in result.stdout
    assert "Selected text mode: 40x25color cols=40 rows=25" in result.stdout
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout
    assert "Stdin keyboard: requested=1 stopped=1" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod stop=keyboard" in result.stdout


def test_top_level_iplay_diagnostics_escape_key_stops_playback(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--diagnostics", "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        input="\x1b",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Selected text mode: 40x25color cols=40 rows=25" in result.stdout
    assert "Stdin keyboard: requested=1 stopped=1" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod stop=keyboard" in result.stdout
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout


def test_top_level_iplay_diagnostics_lowercase_q_stops_playback(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--diagnostics", "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        input="q",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Selected text mode: 40x25color cols=40 rows=25" in result.stdout
    assert "Stdin keyboard: requested=1 stopped=1" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod stop=keyboard" in result.stdout
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout


def test_top_level_iplay_diagnostics_uppercase_q_stops_playback(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--diagnostics", "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        input="Q",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Selected text mode: 40x25color cols=40 rows=25" in result.stdout
    assert "Stdin keyboard: requested=1 stopped=1" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod stop=keyboard" in result.stdout
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout


def test_try_player_native_interactive_without_key_runs_to_source_end(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-interactive", "--video-mode=80x25", str(module)],
        cwd=ROOT,
        input="",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log), "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "trial_loop_policy=native-interactive-source-end-keyboard-stop" in log
    assert "native_args=" in log and " --source-end " in log
    assert "Stdin keyboard: requested=1 stopped=0" in log
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in log
    assert "Terminal live summary: requested=1" in log
    assert re.search(r"Terminal live summary: requested=1 samples=\d+ nonzero=\d+ changed=\d+ printed=\d+ suppressed=[1-9]\d*", log)
    assert "Terminal render: requested=1 cols=80 rows=25 bytes=4000" in log
    assert "trial_result=native-sdl-notcurses-ok" in log


def test_try_player_validate_only_selects_native_terminal_without_kvikdos(tmp_path: Path) -> None:
    module = tmp_path / "SMOKE.S3M"
    module.write_bytes(b"SCRM" + b"\0" * 96)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-terminal", "--blocks=1", "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1" in result.stdout
    assert "trial_video_mode=40x25color cols=40 rows=25" in result.stdout
    assert "dos_args=--blocks=1 --video-mode=40x25color SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_selects_native_live_without_kvikdos(tmp_path: Path) -> None:
    module = tmp_path / "SMOKE.S3M"
    module.write_bytes(b"SCRM" + b"\0" * 96)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-live", "--blocks=2", "--video-mode=80x25", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1" in result.stdout
    assert "trial_video_mode=80x25color cols=80 rows=25" in result.stdout
    assert "dos_args=--blocks=2 --video-mode=80x25 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_selects_native_stdin_keyboard_without_kvikdos(tmp_path: Path) -> None:
    module = tmp_path / "SMOKE.S3M"
    module.write_bytes(b"SCRM" + b"\0" * 96)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-stdin-keyboard", "--blocks=32", "--video-mode=80x25", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1" in result.stdout
    assert "trial_video_mode=80x25color cols=80 rows=25" in result.stdout
    assert "dos_args=--blocks=32 --video-mode=80x25 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_native_stdin_keyboard_stops_on_q(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-stdin-keyboard", "--blocks=32", "--video-mode=80x25", str(module)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "native_stdin_keyboard=1" in log
    assert "trial_loop_policy=native-stdin-keyboard-stop" in log
    assert " --stdin-keyboard" in log
    assert "Stdin keyboard: requested=1 stopped=1" in log
    assert "Stdin keyboard mode: requested=1 raw=0 restored=0" in log
    assert "stop=keyboard" in log
    assert "trial_result=native-sdl-notcurses-ok" in log


def test_try_player_native_live_reports_changing_audio_levels(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-live", "--blocks=2", "--video-mode=80x25", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "native_live=1" in log
    assert " --terminal-live" in log
    assert "\x1b[HTerminal live: block=1 frames=512 accepted=2048 levels=" in log
    assert "\x1b[HTerminal live: block=2 frames=1024 accepted=4096 levels=" in log
    assert "\x1b[92m" in log
    assert "\x1b[96m" in log
    assert "\x1b[90m" in log
    assert "Terminal live summary: requested=1 samples=2 nonzero=2" in log
    assert "trial_result=native-sdl-notcurses-ok" in log


def test_direct_iplay_tracks_sigwinch_terminal_resize_during_live_playback() -> None:
    subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=True,
        timeout=30,
    )
    master_fd, slave_fd = pty.openpty()
    fcntl.ioctl(master_fd, termios.TIOCSWINSZ, struct.pack("HHHH", 25, 80, 0, 0))
    proc = subprocess.Popen(
        [str(ROOT / "rewrite" / ".build" / "iplay"), "--video-mode=auto", str(REAL_ARYX_MODULE), "--blocks=512"],
        cwd=ROOT,
        stdin=subprocess.PIPE,
        stdout=slave_fd,
        stderr=subprocess.PIPE,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "", "LINES": ""},
        preexec_fn=os.setsid,
    )
    os.close(slave_fd)
    output = bytearray()
    resized = False
    deadline = time.monotonic() + 30.0
    try:
        while time.monotonic() < deadline:
            readable, _, _ = select.select([master_fd], [], [], 0.05)
            if readable:
                try:
                    chunk = os.read(master_fd, 8192)
                except OSError:
                    chunk = b""
                if chunk:
                    output.extend(chunk)
            if not resized and b"Inertia Player V1.22" in output:
                fcntl.ioctl(master_fd, termios.TIOCSWINSZ, struct.pack("HHHH", 50, 80, 0, 0))
                os.killpg(proc.pid, signal.SIGWINCH)
                resized = True
            if proc.poll() is not None:
                while True:
                    try:
                        chunk = os.read(master_fd, 8192)
                    except OSError:
                        break
                    if not chunk:
                        break
                    output.extend(chunk)
                break
        if proc.poll() is None:
            os.killpg(proc.pid, signal.SIGTERM)
            proc.wait(timeout=5)
    finally:
        os.close(master_fd)
        if proc.stdin:
            proc.stdin.close()
    stderr = proc.stderr.read().decode(errors="ignore") if proc.stderr else ""
    stdout = output.decode(errors="ignore")

    assert proc.returncode == 3, stdout + stderr
    assert "Selected text mode: auto cols=80 rows=25" in stdout
    resize = re.search(r"Terminal resize: requested=1 signals=(\d+) changes=(\d+) initial=80x25 current=80x50", stdout)
    assert resize, stdout + stderr
    signals, changes = (int(group) for group in resize.groups())
    assert signals >= 1
    assert changes >= 1
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod" in stdout


def test_pty_runner_passes_columns_lines_to_child_stdout_tty(tmp_path: Path) -> None:
    log = tmp_path / "pty.log"
    code = (
        "import fcntl,struct,sys,termios;"
        "rows,cols,_,_=struct.unpack('HHHH', fcntl.ioctl(sys.stdout.fileno(), termios.TIOCGWINSZ, b'\\0'*8));"
        "print(f'{cols}x{rows}')"
    )

    result = subprocess.run(
        [sys.executable, str(ROOT / "rewrite" / "pty_run.py"), str(log), sys.executable, "-c", code],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "COLUMNS": "80", "LINES": "50"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "80x50" in result.stdout
    assert "80x50" in log.read_text(errors="ignore")


def test_pty_runner_forwards_stdin_through_child_tty(tmp_path: Path) -> None:
    log = tmp_path / "pty-input.log"
    code = "import os,sys; data=os.read(sys.stdin.fileno(), 4); print(data.decode().strip())"
    result = subprocess.run(
        [sys.executable, str(ROOT / "rewrite" / "pty_run.py"), str(log), sys.executable, "-c", code],
        cwd=ROOT,
        input="abc\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "COLUMNS": "80", "LINES": "25"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "abc" in result.stdout
    assert "abc" in log.read_text(errors="ignore")


def test_pty_runner_uses_stdin_tty_size_when_stdout_is_piped(tmp_path: Path) -> None:
    log = tmp_path / "pty-stdin-size.log"
    code = (
        "import fcntl,struct,sys,termios;"
        "rows,cols,_,_=struct.unpack('HHHH', fcntl.ioctl(sys.stdout.fileno(), termios.TIOCGWINSZ, b'\\0'*8));"
        "print(f'{cols}x{rows}')"
    )
    master_fd, slave_fd = pty.openpty()
    fcntl.ioctl(slave_fd, termios.TIOCSWINSZ, struct.pack("HHHH", 35, 100, 0, 0))
    try:
        result = subprocess.run(
            [sys.executable, str(ROOT / "rewrite" / "pty_run.py"), str(log), sys.executable, "-c", code],
            cwd=ROOT,
            stdin=slave_fd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
            env={**os.environ, "COLUMNS": "", "LINES": ""},
            timeout=5,
        )
    finally:
        os.close(master_fd)
        os.close(slave_fd)

    assert result.returncode == 0, result.stdout + result.stderr
    assert "100x35" in result.stdout
    assert "100x35" in log.read_text(errors="ignore")


def test_pty_runner_forwards_sigwinch_size_to_child_stdout_tty(tmp_path: Path) -> None:
    log = tmp_path / "pty-resize.log"
    code = (
        "import fcntl,signal,struct,sys,termios,time\n"
        "def size():\n"
        " r,c,_,_=struct.unpack('HHHH', fcntl.ioctl(sys.stdout.fileno(), termios.TIOCGWINSZ, b'\\0'*8))\n"
        " return f'{c}x{r}'\n"
        "print('initial='+size(), flush=True)\n"
        "def h(signum, frame):\n"
        " print('resize='+size(), flush=True)\n"
        " sys.exit(0)\n"
        "signal.signal(signal.SIGWINCH, h)\n"
        "time.sleep(10); sys.exit(1)"
    )
    master_fd, slave_fd = pty.openpty()
    fcntl.ioctl(master_fd, termios.TIOCSWINSZ, struct.pack("HHHH", 25, 80, 0, 0))
    proc = subprocess.Popen(
        [sys.executable, str(ROOT / "rewrite" / "pty_run.py"), str(log), sys.executable, "-c", code],
        cwd=ROOT,
        stdin=subprocess.DEVNULL,
        stdout=slave_fd,
        stderr=subprocess.PIPE,
        env={**os.environ, "COLUMNS": "", "LINES": ""},
        preexec_fn=os.setsid,
    )
    os.close(slave_fd)
    output = bytearray()
    resized = False
    deadline = time.monotonic() + 10.0
    try:
        while time.monotonic() < deadline:
            readable, _, _ = select.select([master_fd], [], [], 0.05)
            if readable:
                try:
                    chunk = os.read(master_fd, 8192)
                except OSError:
                    chunk = b""
                if chunk:
                    output.extend(chunk)
            if not resized and b"initial=80x25" in output:
                fcntl.ioctl(master_fd, termios.TIOCSWINSZ, struct.pack("HHHH", 50, 80, 0, 0))
                os.killpg(proc.pid, signal.SIGWINCH)
                resized = True
            if proc.poll() is not None:
                while True:
                    try:
                        chunk = os.read(master_fd, 8192)
                    except OSError:
                        break
                    if not chunk:
                        break
                    output.extend(chunk)
                break
        if proc.poll() is None:
            os.killpg(proc.pid, signal.SIGTERM)
            proc.wait(timeout=5)
    finally:
        os.close(master_fd)
    stderr = proc.stderr.read().decode(errors="ignore") if proc.stderr else ""
    stdout = output.decode(errors="ignore")

    assert proc.returncode == 0, stdout + stderr
    assert "initial=80x25" in stdout
    assert "resize=80x50" in stdout
    assert "resize=80x50" in log.read_text(errors="ignore")


def test_pty_runner_forwards_termination_to_child_process_group(tmp_path: Path) -> None:
    log = tmp_path / "pty-term.log"
    code = (
        "import signal,sys,time\n"
        "def h(signum, frame):\n"
        " print('child-term', flush=True)\n"
        " sys.exit(0)\n"
        "signal.signal(signal.SIGTERM, h)\n"
        "print('child-ready', flush=True)\n"
        "time.sleep(10)\n"
        "sys.exit(1)\n"
    )
    proc = subprocess.Popen(
        [sys.executable, str(ROOT / "rewrite" / "pty_run.py"), str(log), sys.executable, "-c", code],
        cwd=ROOT,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        env={**os.environ, "COLUMNS": "80", "LINES": "25"},
    )
    output = ""
    deadline = time.monotonic() + 10.0
    while time.monotonic() < deadline and "child-ready" not in output:
        if proc.stdout:
            readable, _, _ = select.select([proc.stdout], [], [], 0.05)
            if readable:
                chunk = os.read(proc.stdout.fileno(), 1024).decode(errors="ignore")
                if chunk:
                    output += chunk
        if proc.poll() is not None:
            break
    proc.send_signal(signal.SIGTERM)
    stdout, stderr = proc.communicate(timeout=5)
    output += stdout

    assert proc.returncode == 0, output + stderr
    assert "child-ready" in output
    assert "child-term" in output
    assert "child-term" in log.read_text(errors="ignore")


def test_try_player_native_terminal_reports_rendered_screen(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-terminal", "--blocks=1", "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "native_terminal=1" in log
    assert "native_args=" in log
    assert " --terminal-render" in log
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" in log
    assert "\x1b[2J\x1b[H" in log
    assert "Inertia Player V1.22" in log
    assert "Terminal render end" in log
    assert "trial_result=native-sdl-notcurses-ok" in log


def test_try_player_native_sdl_notcurses_reports_playback_ok(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", "--video-mode=80x50", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1" in log
    assert "trial_video_mode=80x50 cols=80 rows=50" in log
    assert "native_args=" in log
    assert " 1 80x50" in log
    assert "audio_mode=sdl-compatible-sb16-native" in log
    assert "trial_proof_scope=native-sdl-notcurses" in log
    assert "Module: aryx.s3m" in log
    assert "trial_module_loaded=yes" in log
    assert "trial_loaded_module_name=aryx.s3m" in log
    assert "trial_loaded_module_key=ARYX.S3M" in log
    assert "trial_requested_module_key=ARYX.S3M" in log
    assert "trial_requested_module_loaded=yes" in log
    assert "trial_module_size=20800" in log
    assert "trial_module_size_matches_host=yes" in log
    assert "Loader: s3m_module (Scream Tracker 3)" in log
    assert "Module type tag: 204D3353" in log
    assert "Title: aryx" in log
    assert "trial_module_loader=s3m_module (Scream Tracker 3)" in log
    assert "trial_module_type_tag=204D3353" in log
    assert "trial_module_title=aryx" in log
    assert "trial_ok_loader_metadata=yes" in log
    assert "Decoder route: id=0 name=external-library" in log
    assert "trial_decoder_route_id=0" in log
    assert "trial_decoder_route_name=external-library" in log
    assert "Decoder handoff: external tracker -> SB16 PCM seam." in log
    assert "trial_decoder_handoff=external tracker -> SB16 PCM seam." in log
    assert "Playback output: SDL-compatible SB16 16-bit stereo native." in log
    assert "trial_audio_backend=SDL-compatible SB16 16-bit stereo" in log
    assert "trial_audio_backend_valid=yes" in log
    assert "trial_audio_levels_valid=yes" in log
    assert "PCM source: external_module seed=1 truncated=0 input=file-path renderer=e route=0 provider=libmikmod hook_provider=libmikmod stream_start=0" in log
    assert "trial_pcm_provider=libmikmod" in log
    assert "trial_pcm_renderer=e" in log
    assert "trial_pcm_route=0" in log
    assert "trial_pcm_input=file-path" in log
    assert "trial_pcm_truncated=0" in log
    assert "trial_pcm_hook_provider=libmikmod" in log
    assert "trial_pcm_stream_start=0" in log
    assert "Playback pump: blocks=1 frames=512 accepted=2048" in log
    assert "trial_playback_pump=yes" in log
    assert "trial_playback_valid=yes" in log
    assert "limit=1 source_end=0 stop=block-limit" in log
    assert "Screen present: reason=post-playback-status scope=status-only bytes=4000 screen_bytes=4000" in log
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod stop=block-limit source_end=0 blocks=1 source_frames=512" in log
    assert "Audio backend: SDL-compatible SB16 16-bit stereo" in log
    assert "Selected text mode: 80x50 cols=80 rows=50" in log
    assert "selected_present=calls:1 bytes:8000 cols:80 rows:50" in log
    assert "trial_screen_present=yes" in log
    assert "trial_selected_screen_geometry_valid=yes" in log
    assert "trial_playback_position_present=yes" in log
    assert "trial_playback_position_valid=yes" in log
    assert "trial_playback_position_geometry_valid=yes" in log
    assert "trial_post_playback_status_present=yes" in log
    assert "trial_post_playback_status_valid=yes" in log
    assert "trial_post_playback_status_geometry_valid=yes" in log
    assert "screen_present=calls:1 bytes:4000 cols:80 rows:25" in log
    assert "screen40_present=calls:1 bytes:2000 cols:40 rows:25" in log
    assert "screen80x50_present=calls:1 bytes:8000 cols:80 rows:50" in log
    assert "Resize present: phase=before bytes=4000 screen_bytes=4000" in log
    assert "Resize present: phase=after bytes=8000 screen_bytes=8000" in log
    assert "resize_before_present=calls:1 bytes:4000 cols:80 rows:25 resize_ok:1" in log
    assert "resize_after_present=calls:2 bytes:12000 cols:80 rows:50 resize_ok:1" in log
    assert "trial_resize_cycle_valid=yes" in log
    assert "Subwindow present: origin=3,5 rows=5 cols=34 screen_bytes=4000" in log
    assert "subwindow_title=" in log
    assert "SUBWINDOW" in log
    assert "trial_subwindow_valid=yes" in log
    assert "Color probe: target_fg=16 target_bg=8 cells=24 fg_matches=16 bg_matches=8 blink_matches=8 fg_mask=ffff bg_mask=ff blink_mask=aa present_calls=1 bytes=4000 cols=80 rows=25" in log
    assert "trial_color_probe_valid=yes" in log
    assert "Level sequence: target=16 samples=16" in log
    assert "trial_audio_level_sequence_valid=yes" in log
    assert "trial_result=native-sdl-notcurses-ok" in log
    assert "trial_failure_reason=none" in log


def test_try_player_native_filelist_reports_playback_ok(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())
    filelist = tmp_path / "PLAYLIST.LST"
    filelist.write_text("\n\t aryx.s3m \r\nignored.s3m\n")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", "--video-mode=80x25", "@" + str(filelist)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert f"trial_filelist_arg=@{filelist}" in log
    assert f"trial_filelist_path={filelist}" in log
    assert "trial_filelist_selected=aryx.s3m" in log
    assert f"trial_filelist_selected_host={module}" in log
    assert f"native_args=@{filelist} 1 80x25color" in log
    assert f"File list: @{filelist} selected={module}" in log
    assert "Module: aryx.s3m" in log
    assert "trial_requested_module_loaded=yes" in log
    assert "trial_module_size_matches_host=yes" in log
    assert "trial_audio_backend_valid=yes" in log
    assert "trial_audio_levels_valid=yes" in log
    assert "trial_audio_level_sequence_valid=yes" in log
    assert "trial_resize_cycle_valid=yes" in log
    assert "trial_subwindow_valid=yes" in log
    assert "trial_color_probe_valid=yes" in log
    assert "trial_selected_screen_geometry_valid=yes" in log
    assert "trial_result=native-sdl-notcurses-ok" in log
    assert "trial_failure_reason=none" in log


def test_try_player_native_resolves_filelist_path_case_like_dos(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())
    filelist = tmp_path / "playlist.lst"
    requested = tmp_path / "PLAYLIST.LST"
    filelist.write_text("aryx.s3m\n")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", "--video-mode=80x25", "@" + str(requested)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert f"try_player: resolved DOS-style case-insensitive file-list path: {requested} -> {filelist}" in result.stderr
    assert f"trial_filelist_arg=@{requested}" in log
    assert f"trial_filelist_path={filelist}" in log
    assert f"native_args=@{filelist} 1 80x25color" in log
    assert f"File list: @{filelist} selected={module}" in log
    assert "Module: aryx.s3m" in log
    assert "trial_requested_module_loaded=yes" in log
    assert "trial_module_size_matches_host=yes" in log
    assert "trial_color_probe_valid=yes" in log
    assert "trial_result=native-sdl-notcurses-ok" in log
    assert "trial_failure_reason=none" in log


def test_try_player_native_keyboard_stop_reports_interactive_stop_ok(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-keyboard-after-one", "--video-mode=40x25", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "trial_exe=iplay_native diagnostics=0 hardware_diagnostics=0 production=0 native=1" in log
    assert "trial_loop_policy=native-keyboard-stop" in log
    assert "trial_requested_module_loaded=yes" in log
    assert "trial_module_size_matches_host=yes" in log
    assert "Loader: s3m_module (Scream Tracker 3)" in log
    assert "Module type tag: 204D3353" in log
    assert "Title: aryx" in log
    assert "trial_module_loader=s3m_module (Scream Tracker 3)" in log
    assert "trial_module_type_tag=204D3353" in log
    assert "trial_module_title=aryx" in log
    assert "trial_ok_loader_metadata=yes" in log
    assert "trial_playback_valid=yes" in log
    assert "trial_audio_backend=SDL-compatible SB16 16-bit stereo" in log
    assert "trial_audio_backend_valid=yes" in log
    assert "trial_audio_levels_valid=yes" in log
    assert "trial_pcm_provider=libmikmod" in log
    assert "trial_decoder_route_name=external-library" in log
    assert "trial_decoder_handoff=external tracker -> SB16 PCM seam." in log
    assert "native_args=" in log
    assert " --keyboard-after-one 40x25color" in log
    assert "Selected text mode: 40x25color cols=40 rows=25" in log
    assert "selected_present=calls:1 bytes:2000 cols:40 rows:25" in log
    assert "trial_selected_screen_geometry_valid=yes" in log
    assert "trial_playback_position_present=yes" in log
    assert "trial_playback_position_valid=yes" in log
    assert "trial_playback_position_geometry_valid=yes" in log
    assert "trial_post_playback_status_present=yes" in log
    assert "trial_post_playback_status_valid=yes" in log
    assert "trial_post_playback_status_geometry_valid=yes" in log
    assert "trial_resize_cycle_valid=yes" in log
    assert "trial_subwindow_valid=yes" in log
    assert "trial_color_probe_valid=yes" in log
    assert "trial_audio_level_sequence_valid=yes" in log
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod stop=keyboard source_end=0 blocks=1 source_frames=512" in log
    assert "trial_result=native-sdl-notcurses-ok" in log
    assert "trial_failure_reason=none" in log


def test_try_player_native_source_end_reports_source_end_ok(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native-source-end", "--video-mode=80x25", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "trial_loop_policy=native-source-end" in log
    assert "native_args=" in log
    assert " --source-end 80x25color" in log
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in log
    assert "trial_requested_module_loaded=yes" in log
    assert "trial_module_size_matches_host=yes" in log
    assert "trial_playback_valid=yes" in log
    assert "trial_audio_backend=SDL-compatible SB16 16-bit stereo" in log
    assert "trial_audio_backend_valid=yes" in log
    assert "trial_audio_levels_valid=yes" in log
    assert "trial_pcm_provider=libmikmod" in log
    assert "trial_decoder_route_name=external-library" in log
    assert "trial_decoder_handoff=external tracker -> SB16 PCM seam." in log
    assert "trial_post_playback_status_valid=yes" in log
    assert "trial_post_playback_status_geometry_valid=yes" in log
    assert "trial_playback_position_valid=yes" in log
    assert "trial_playback_position_geometry_valid=yes" in log
    assert "trial_resize_cycle_valid=yes" in log
    assert "trial_subwindow_valid=yes" in log
    assert "trial_color_probe_valid=yes" in log
    assert "trial_audio_level_sequence_valid=yes" in log
    assert "trial_result=native-sdl-notcurses-ok" in log
    assert "trial_failure_reason=none" in log


def test_try_player_native_project_owned_reports_project_decoder_unavailable(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "SMOKE.INR"
    module.write_bytes(b"INR\x00SMOKE INR")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 4, result.stdout + result.stderr + log
    assert "status=project-decoder-unavailable route_id=1 route=project-owned provider=native" in log
    assert "trial_decoder_route_name=project-owned" in log
    assert "trial_result=project-decoder-unavailable" in log
    assert "trial_failure_reason=project-decoder-unavailable" in log
    assert "trial_script_exit_status=4" in log


def test_try_player_native_unknown_file_reports_unsupported_format(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "BAD.XYZ"
    module.write_bytes(b"bad")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 4, result.stdout + result.stderr + log
    assert "status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod" in log
    assert "trial_decoder_route_name=probe-by-content" in log
    assert "trial_result=unsupported-format" in log
    assert "trial_failure_reason=unsupported-format" in log
    assert "trial_script_exit_status=4" in log


def test_try_player_native_corrupt_known_tracker_reports_external_decoder_failed(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "BAD.S3M"
    module.write_bytes(b"bad")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=30,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 4, result.stdout + result.stderr + log
    assert "status=external-decoder-failed route_id=0 route=external-library provider=libmikmod" in log
    assert "trial_decoder_route_name=external-library" in log
    assert "trial_result=external-decoder-failed" in log
    assert "trial_failure_reason=external-decoder-failed" in log
    assert "trial_script_exit_status=4" in log


def test_try_player_validate_only_selects_production_after_module_without_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), str(module), "--production"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=IPLAYC.EXE diagnostics=0 hardware_diagnostics=0 production=1" in result.stdout
    assert "dos_args=SMOKE.S3M" in result.stdout
    assert "--blocks=32" not in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_production_classifies_real_sb16_unavailable_under_kvikdos(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--production", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=10,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 3, result.stdout + result.stderr + log
    assert "trial_exe=IPLAYC.EXE diagnostics=0 hardware_diagnostics=0" in log
    assert "audio_mode=real-sb16-hardware" in log
    assert "trial_proof_scope=production-real-sb16" in log
    assert "trial_audio_unavailable=yes" in log
    assert "trial_audio_unavailable_source=exit-code" in log
    assert "trial_result=audio-unavailable" in log
    assert "trial_failure_reason=sb16-audio-unavailable" in log
    assert "trial_script_exit_status=3" in log


def test_try_player_hardware_diagnostics_classifies_real_sb16_unavailable_screen_under_kvikdos(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--hardware-diagnostics", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=10,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 3, result.stdout + result.stderr + log
    assert "trial_exe=IPLAYHW.EXE diagnostics=1 hardware_diagnostics=1" in log
    assert "audio_mode=real-sb16-hardware" in log
    assert "trial_proof_scope=hardware-unavailable-probe" in log
    assert "Screen present: reason=audio-unavailable " in log
    assert "trial_audio_unavailable=yes" in log
    assert "trial_audio_unavailable_source=screen" in log
    assert "trial_result=audio-unavailable" in log
    assert "trial_failure_reason=sb16-audio-unavailable" in log


def test_try_player_validate_only_selects_hardware_diagnostics_after_module_without_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), str(module), "--hardware-diagnostics"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_exe=IPLAYHW.EXE diagnostics=1 hardware_diagnostics=1 production=0" in result.stdout
    assert "dos_args=--blocks=32 SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


@pytest.mark.parametrize(
    ("mode", "expected_header", "expected_args", "forbidden_arg"),
    [
        ("--quiet", "trial_exe=IPLAYTRY.EXE diagnostics=0 hardware_diagnostics=0 production=0", "dos_args=SMOKE.S3M", "--blocks=32"),
        ("--diagnostics", "trial_exe=IPLAYDIAG.EXE diagnostics=1 hardware_diagnostics=0 production=0", "dos_args=--blocks=32 SMOKE.S3M", None),
        ("--continuous-diagnostics", "trial_exe=IPLAYCONT.EXE diagnostics=1 hardware_diagnostics=0 production=0", "dos_args=--blocks=32 SMOKE.S3M", None),
    ],
)
def test_try_player_validate_only_selects_remaining_modes_after_module_without_kvikdos(
    tmp_path: Path,
    mode: str,
    expected_header: str,
    expected_args: str,
    forbidden_arg: str | None,
) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), str(module), mode],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert expected_header in result.stdout
    assert expected_args in result.stdout
    if forbidden_arg is not None:
        assert forbidden_arg not in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


@pytest.mark.parametrize(
    ("mode", "expected_header"),
    [
        ("--production", "trial_exe=IPLAYC.EXE diagnostics=0 hardware_diagnostics=0 production=1"),
        ("--hardware-diagnostics", "trial_exe=IPLAYHW.EXE diagnostics=1 hardware_diagnostics=1 production=0"),
    ],
)
def test_try_player_validate_only_selects_real_sb16_modes_and_video_mode_after_module_without_kvikdos(
    tmp_path: Path,
    mode: str,
    expected_header: str,
) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), str(module), mode, "--video-mode=80x50"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert expected_header in result.stdout
    assert "trial_video_mode=80x50 cols=80 rows=50" in result.stdout
    assert "dos_args=--video-mode=80x50" in result.stdout
    assert "SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_reports_parsed_pcm_stream_start_from_dos_output(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--blocks=1", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=10,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "trial_pcm_source_line=PCM source:" in log
    assert "trial_pcm_provider=native-preview" in log
    assert "trial_pcm_hook_provider=none" in log
    assert "trial_pcm_input=memory" in log
    assert "trial_pcm_stream_start=" in log
    assert "trial_pcm_stream_start=none" not in log
    assert "trial_result=bounded-ui-playback-ok" in log


def test_try_player_real_aryx_s3m_reports_bounded_ui_playback_ok(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "aryx.s3m"
    requested = tmp_path / "ARYX.S3M"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--blocks=1", str(requested)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=10,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "try_player: resolved DOS-style case-insensitive module path:" in result.stderr
    assert "trial_loaded_module_key=ARYX.S3M" in log
    assert "trial_requested_module_key=ARYX.S3M" in log
    assert "trial_requested_module_loaded=yes" in log
    assert "trial_module_size_matches_host=yes" in log
    assert "trial_module_loader=s3m_module" in log
    assert "trial_module_type_tag=204D3353" in log
    assert "trial_decoder_route_id=0" in log
    assert "trial_decoder_route_name=external-library" in log
    assert "trial_decoder_handoff=external tracker -> SB16 PCM seam." in log
    assert "trial_pcm_provider=native-preview" in log
    assert "trial_pcm_renderer=e" in log
    assert "trial_pcm_input=memory" in log
    assert "trial_pcm_hook_provider=none" in log
    assert "trial_pcm_stream_start=" in log
    assert "trial_pcm_stream_start=none" not in log
    assert "trial_playback_valid=yes" in log
    assert "trial_playback_position_geometry_valid=yes" in log
    assert "trial_post_playback_status_geometry_valid=yes" in log
    assert "trial_result=bounded-ui-playback-ok" in log
    assert "trial_failure_reason=none" in log


def test_try_player_continuous_diagnostics_reports_source_ended_ui_ok(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    write_endcont_module(tmp_path)
    module = tmp_path / "ENDCONT.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--continuous-diagnostics", "--video-mode=80x50", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=10,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "trial_exe=IPLAYCONT.EXE diagnostics=1 hardware_diagnostics=0 production=0" in log
    assert "trial_loop_policy=continuous-diagnostics" in log
    assert "trial_proof_scope=playable-wrapper-continuous" in log
    assert "trial_video_mode=80x50 cols=80 rows=50" in log
    assert "trial_requested_module_loaded=yes" in log
    assert "trial_module_size_matches_host=yes" in log
    assert "trial_decoder_route_name=external-library" in log
    assert "trial_decoder_handoff=external tracker -> SB16 PCM seam." in log
    assert "trial_pcm_provider=native-preview" in log
    assert "trial_pcm_hook_provider=none" in log
    assert "trial_playback_valid=yes" in log
    assert "stop=source-end" in log
    assert "trial_playback_position_geometry_valid=yes" in log
    assert "trial_post_playback_status_present=yes" in log
    assert "trial_post_playback_status_valid=yes" in log
    assert "trial_post_playback_status_geometry_valid=yes" in log
    assert "trial_result=source-ended-ui-ok" in log
    assert "trial_failure_reason=none" in log


def test_try_player_quiet_source_end_reports_quiet_completed_without_diagnostics(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    write_endcont_module(tmp_path)
    module = tmp_path / "ENDCONT.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--quiet", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=10,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "trial_exe=IPLAYTRY.EXE diagnostics=0 hardware_diagnostics=0 production=0" in log
    assert "trial_loop_policy=continuous-quiet-wrapper" in log
    assert "trial_mode_note=quiet-player-no-diagnostic-stdout" in log
    assert "quiet_trial_completed=yes meaning=player-exited-without-diagnostic-stdout" in log
    assert "trial_playback_pump=no" in log
    assert "trial_result=quiet-completed-no-diagnostics" in log
    assert "trial_failure_reason=none" in log
    assert "trial_script_exit_status=0" in log


def test_try_player_reports_capped_file_path_pcm_stream_start(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    module = tmp_path / "CAPTRIAL.S3M"
    s3m = bytearray(28672)
    s3m[:8] = b"CAPTRIAL"
    s3m[0x20:0x22] = (1).to_bytes(2, "little")
    s3m[0x22:0x24] = (0).to_bytes(2, "little")
    s3m[0x24:0x26] = (1).to_bytes(2, "little")
    s3m[0x2C:0x30] = b"SCRM"
    s3m[0x31] = 1
    s3m[0x32] = 125
    s3m[0x40] = 0
    s3m[0x41:0x60] = bytes([0xFF]) * 31
    s3m[0x60] = 0
    s3m[0x61:0x63] = (0x20).to_bytes(2, "little")
    s3m[0x200:0x202] = (2).to_bytes(2, "little")
    s3m[0x220:] = bytes([0x55]) * (len(s3m) - 0x220)
    module.write_bytes(s3m)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--blocks=1", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log)},
        timeout=10,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 0, result.stdout + result.stderr + log
    assert "trial_module_size=28672" in log
    assert "trial_module_size_matches_host=yes" in log
    assert "trial_pcm_provider=dos-fallback" in log
    assert "trial_pcm_input=file-path" in log
    assert "trial_pcm_truncated=1" in log
    assert "trial_pcm_stream_start=512" in log
    assert "trial_result=bounded-ui-playback-ok" in log


def test_try_player_rejects_old_pcm_source_without_stream_start(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    fake_kvikdos = tmp_path / "fake-kvikdos.sh"
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"
    fake_kvikdos.write_text(
        "#!/bin/sh\n"
        "cat <<'EOF'\n"
        "Module: SMOKE.S3M\n"
        f"Size: {module.stat().st_size} bytes\n"
        "Loader: s3m_module (Scream Tracker 3)\n"
        "Module type tag: 204D3353\n"
        "Title: SMOKE\n"
        "Decoder route: id=0 name=external-library\n"
        "Decoder handoff: external tracker -> SB16 PCM seam.\n"
        "PCM source: s3m_module seed=1 truncated=0 input=memory renderer=e route=0 provider=native-preview\n"
        "Playback pump: blocks=1 frames=512 accepted=2048 checksum=1234 limit=1 source_end=0 stop=block-limit\n"
        "Screen present: reason=playback-position scope=full-screen bytes=4000 screen_bytes=4000 screen_checksum=123 screen_nonblank=10 full=1 cols=80 rows=25 mode_ok=1 audio_frames=512 levels=1/1\n"
        "Screen present: reason=post-playback-status scope=status-only bytes=4000 screen_bytes=4000 screen_checksum=124 screen_nonblank=10 full=1 cols=80 rows=25 mode_ok=1 audio_frames=512 levels=1/1\n"
        "EOF\n"
    )
    fake_kvikdos.chmod(0o755)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--blocks=1", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log), "KVIKDOS": str(fake_kvikdos)},
        timeout=10,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 4, result.stdout + result.stderr + log
    assert "trial_pcm_source_line=PCM source:" in log
    assert "trial_pcm_hook_provider=missing" in log
    assert "trial_pcm_stream_start=none" in log
    assert "trial_result=pcm-source-missing" in log
    assert "trial_failure_reason=pcm-source-missing" in log


def test_try_player_rejects_pcm_source_without_hook_provider(tmp_path: Path) -> None:
    trial_log = tmp_path / "RES.TXT"
    fake_kvikdos = tmp_path / "fake-kvikdos.sh"
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"
    fake_kvikdos.write_text(
        "#!/bin/sh\n"
        "cat <<'EOF'\n"
        "Module: SMOKE.S3M\n"
        f"Size: {module.stat().st_size} bytes\n"
        "Loader: s3m_module (Scream Tracker 3)\n"
        "Module type tag: 204D3353\n"
        "Title: SMOKE\n"
        "Decoder route: id=0 name=external-library\n"
        "Decoder handoff: external tracker -> SB16 PCM seam.\n"
        "PCM source: s3m_module seed=1 truncated=0 input=memory renderer=e route=0 provider=native-preview stream_start=512\n"
        "Playback pump: blocks=1 frames=512 accepted=2048 checksum=1234 limit=1 source_end=0 stop=block-limit\n"
        "Screen present: reason=playback-position scope=full-screen bytes=4000 screen_bytes=4000 screen_checksum=123 screen_nonblank=10 full=1 cols=80 rows=25 mode_ok=1 audio_frames=512 levels=1/1\n"
        "Screen present: reason=post-playback-status scope=status-only bytes=4000 screen_bytes=4000 screen_checksum=124 screen_nonblank=10 full=1 cols=80 rows=25 mode_ok=1 audio_frames=512 levels=1/1\n"
        "EOF\n"
    )
    fake_kvikdos.chmod(0o755)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--blocks=1", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_LOG": str(trial_log), "KVIKDOS": str(fake_kvikdos)},
        timeout=10,
    )

    log = trial_log.read_text(errors="ignore") if trial_log.exists() else ""
    assert result.returncode == 4, result.stdout + result.stderr + log
    assert "trial_pcm_hook_provider=missing" in log
    assert "trial_pcm_stream_start=512" in log
    assert "trial_result=pcm-source-missing" in log
    assert "trial_failure_reason=pcm-source-missing" in log


@pytest.mark.parametrize(
    ("mode", "expected", "option_after_module"),
    [
        ("40x25bw", "trial_video_mode=40x25bw cols=40 rows=25", False),
        ("40X25BW", "trial_video_mode=40x25bw cols=40 rows=25", False),
        ("40x25mono", "trial_video_mode=40x25bw cols=40 rows=25", True),
        ("40x25", "trial_video_mode=40x25color cols=40 rows=25", True),
        ("80x25bw", "trial_video_mode=80x25bw cols=80 rows=25", False),
        ("80X25BW", "trial_video_mode=80x25bw cols=80 rows=25", False),
        ("80x25mono", "trial_video_mode=80x25bw cols=80 rows=25", True),
        ("80x25color", "trial_video_mode=80x25color cols=80 rows=25", False),
        ("80x25", "trial_video_mode=80x25color cols=80 rows=25", True),
        ("80x50", "trial_video_mode=80x50 cols=80 rows=50", False),
        ("80X50", "trial_video_mode=80x50 cols=80 rows=50", False),
        ("80x50project", "trial_video_mode=80x50 cols=80 rows=50", True),
    ],
)
def test_try_player_validate_only_normalizes_video_modes_without_kvikdos(
    tmp_path: Path,
    mode: str,
    expected: str,
    option_after_module: bool,
) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"
    trial_args = [str(module), f"--video-mode={mode}"] if option_after_module else [f"--video-mode={mode}", str(module)]

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), *trial_args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert expected in result.stdout
    assert "dos_args=" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_terminal_video_mode_uses_columns_lines_without_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", "--video-mode=terminal", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_TRIAL_VALIDATE_ONLY": "1", "COLUMNS": "80", "LINES": "50"},
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_video_mode=80x50 cols=80 rows=50" in result.stdout
    assert "dos_args=--blocks=1 --video-mode=terminal SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_try_player_validate_only_terminal_video_mode_uses_stty_size_without_kvikdos(tmp_path: Path) -> None:
    write_smoke_modules(tmp_path)
    module = tmp_path / "SMOKE.S3M"
    bin_dir = tmp_path / "bin"
    fake_stty = bin_dir / "stty"
    bin_dir.mkdir()
    fake_stty.write_text("#!/bin/sh\nprintf '50 80\\n'\n")
    fake_stty.chmod(0o755)

    result = subprocess.run(
        [str(ROOT / "rewrite" / "try_player.sh"), "--native", "--blocks=1", "--video-mode=auto", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={
            **os.environ,
            "IPLAY_TRIAL_VALIDATE_ONLY": "1",
            "COLUMNS": "",
            "LINES": "",
            "PATH": f"{bin_dir}:{os.environ.get('PATH', '')}",
        },
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "trial_video_mode=80x50 cols=80 rows=50" in result.stdout
    assert "dos_args=--blocks=1 --video-mode=auto SMOKE.S3M" in result.stdout
    assert "kvikdos" not in result.stdout + result.stderr


def test_iplay_launcher_help_describes_rebuild_and_direct_player() -> None:
    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--help"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "usage: ./rewrite/iplay.sh [--check] [--rebuild] [--diagnostics] [iplay-options] [module-file|@file-list]" in result.stderr
    assert "builds rewrite/.build/iplay when missing or stale" in result.stderr
    assert "--check verifies the native SDL/notcurses host player is built and executable without starting playback" in result.stderr
    assert "--rebuild forces the native SDL/notcurses host player rebuild before launch" in result.stderr
    assert "streams player output live while keeping status evidence for the wrapper exit decision" in result.stderr
    assert "--diagnostics or IPLAY_LAUNCHER_DIAGNOSTICS=1 shows raw player evidence" in result.stderr
    assert "--list-extensions is passed through to list external tracker formats handled by the library-backed path" in result.stderr
    assert "--classify <path> is passed through to show decoder route selection without playback" in result.stderr
    assert "when --video-mode is omitted, iplay selects the nearest supported text mode from the terminal size" in result.stderr
    assert "press q, Q, or Escape to stop playback" in result.stderr
    assert "keyboard and block-limit player stops from the diagnostic host status code are returned as success" in result.stderr
    assert "example: ./rewrite/iplay.sh --video-mode=80x50 samples/aryx.s3m" in result.stderr


def test_iplay_launcher_help_after_diagnostics_stays_launcher_help() -> None:
    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--diagnostics", "--help"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "usage: ./rewrite/iplay.sh [--check] [--rebuild] [--diagnostics] [iplay-options] [module-file|@file-list]" in result.stderr
    assert "usage: iplay" not in result.stdout + result.stderr


def test_iplay_launcher_help_after_rebuild_stays_launcher_help() -> None:
    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--rebuild", "--help"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "usage: ./rewrite/iplay.sh [--check] [--rebuild] [--diagnostics] [iplay-options] [module-file|@file-list]" in result.stderr
    assert "--rebuild forces the native SDL/notcurses host player rebuild before launch" in result.stderr
    assert "usage: iplay" not in result.stdout + result.stderr


def test_iplay_launcher_check_verifies_binary_without_playback() -> None:
    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "iplay.sh: ready: exe=rewrite/.build/iplay rebuilt=" in result.stdout
    assert "Module:" not in result.stdout
    assert "status=" not in result.stdout
    assert "extensions=" not in result.stdout


def test_iplay_launcher_runs_direct_sdl_notcurses_player(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Inertia Player V1.22" in result.stdout
    assert "Selected text mode: 40x25color cols=40 rows=25" not in result.stdout
    assert "SDL audio sink: requested=1 opened=1" not in result.stdout
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" not in result.stdout
    assert "Stdin keyboard: requested=1 stopped=1" not in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod" not in result.stdout


def test_iplay_launcher_diagnostics_mode_preserves_direct_player_evidence(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "IPLAY_LAUNCHER_DIAGNOSTICS": "1"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Module: aryx.s3m" in result.stdout
    assert "Selected text mode: 40x25color cols=40 rows=25" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" in result.stdout
    assert result.stdout.count("Terminal render: requested=1 cols=40 rows=25 bytes=2000") == 1
    assert "Stdin keyboard: requested=1 stopped=1" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_iplay_launcher_diagnostics_option_preserves_direct_player_evidence(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--diagnostics", "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Module: aryx.s3m" in result.stdout
    assert "Selected text mode: 40x25color cols=40 rows=25" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_iplay_launcher_does_not_hide_decoder_failures(tmp_path: Path) -> None:
    module = tmp_path / "bad.s3m"
    module.write_bytes(b"not a tracker module")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "status=external-decoder-failed route_id=0 route=external-library provider=libmikmod" not in result.stdout
    assert "iplay.sh: playback failed: status=external-decoder-failed route_id=0 route=external-library provider=libmikmod" in result.stderr
    assert "Selected text mode: 40x25color cols=40 rows=25" not in result.stdout


def test_iplay_launcher_summarizes_early_failures_without_status_line(tmp_path: Path) -> None:
    missing = tmp_path / "missing.s3m"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--video-mode=40x25color", str(missing)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "Module not found." in result.stderr
    assert "iplay.sh: player exited with status 2" in result.stderr
    assert "status=" not in result.stdout


def test_iplay_launcher_without_video_mode_uses_terminal_auto_size(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--diagnostics", str(module)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Selected text mode: auto cols=80 rows=50" in result.stdout
    assert "Terminal render: requested=1 cols=80 rows=50 bytes=8000" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_iplay_launcher_filelist_selects_first_entry_through_direct_player(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())
    filelist = tmp_path / "PLAYLIST.LST"
    filelist.write_text("\n  aryx.s3m\r\nignored.s3m\n")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--diagnostics", "--video-mode=40x25color", "@" + str(filelist)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert f"File list: @{filelist} selected={module}" in result.stdout
    assert "Module: aryx.s3m" in result.stdout
    assert "Selected text mode: 40x25color cols=40 rows=25" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_iplay_launcher_resolves_module_path_case_like_dos(tmp_path: Path) -> None:
    lowercase = tmp_path / "aryx.s3m"
    lowercase.write_bytes(REAL_ARYX_MODULE.read_bytes())
    requested = tmp_path / "ARYX.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--diagnostics", "--video-mode=40x25color", str(requested)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Module: aryx.s3m" in result.stdout
    assert "Module not found." not in result.stderr
    assert "Selected text mode: 40x25color cols=40 rows=25" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_iplay_launcher_resolves_filelist_path_case_like_dos(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())
    lowercase_filelist = tmp_path / "playlist.lst"
    requested_filelist = tmp_path / "PLAYLIST.LST"
    lowercase_filelist.write_text("aryx.s3m\n")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--diagnostics", "--video-mode=40x25color", "@" + str(requested_filelist)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert f"File list: @{requested_filelist} selected={module}" in result.stdout
    assert "Module: aryx.s3m" in result.stdout
    assert "Module not found." not in result.stderr
    assert "Selected text mode: 40x25color cols=40 rows=25" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_iplay_launcher_rebuild_check_forces_rebuild_without_playback() -> None:
    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--rebuild", "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "iplay.sh: ready: exe=rewrite/.build/iplay rebuilt=1" in result.stdout
    assert "Module:" not in result.stdout
    assert "status=" not in result.stdout
    assert "extensions=" not in result.stdout


def test_iplay_launcher_lists_external_tracker_extensions_without_playback() -> None:
    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--list-extensions"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert result.stdout.startswith("extensions=")
    for ext in [".mod", ".s3m", ".xm", ".it", ".far", ".mtm"]:
        assert ext in result.stdout
    assert "Module:" not in result.stdout
    assert "status=" not in result.stdout


def test_iplay_launcher_classifies_decoder_routes_without_playback(tmp_path: Path) -> None:
    s3m = tmp_path / "song.s3m"
    inr = tmp_path / "song.inr"
    unknown = tmp_path / "song.bin"
    s3m.write_bytes(b"")
    inr.write_bytes(b"")
    unknown.write_bytes(b"")

    cases = [
        (s3m, "route_id=0 route=external-library library=1"),
        (inr, "route_id=1 route=project-owned library=0"),
        (unknown, "route_id=2 route=probe-by-content library=1"),
    ]
    for path, expected in cases:
        result = subprocess.run(
            [str(ROOT / "rewrite" / "iplay.sh"), "--classify", str(path)],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
            timeout=30,
        )

        assert result.returncode == 0, result.stdout + result.stderr
        assert expected in result.stdout
        assert "backend=\"SDL-compatible SB16 16-bit stereo\"" in result.stdout
        assert "Module:" not in result.stdout
        assert "status=" not in result.stdout


def test_iplay_launcher_reports_project_owned_decoder_unavailable_without_raw_status(tmp_path: Path) -> None:
    module = tmp_path / "song.inr"
    module.write_bytes(b"INR placeholder")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 3, result.stdout + result.stderr
    assert "status=project-decoder-unavailable" not in result.stdout
    assert "iplay.sh: playback failed: status=project-decoder-unavailable route_id=1 route=project-owned provider=native" in result.stderr
    assert "Selected text mode: 40x25color cols=40 rows=25" not in result.stdout


def test_iplay_launcher_playback_check_project_owned_reports_decoder_unavailable(tmp_path: Path) -> None:
    module = tmp_path / "SONG.INR"
    module.write_bytes(b"INR placeholder")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--check-playback", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "iplay.sh: playback readiness check failed: status=project-decoder-unavailable route_id=1 route=project-owned provider=native" in result.stderr
    assert "iplay.sh: playback readiness check failed with status 3" in result.stderr
    assert "iplay.sh: playback-ready:" not in result.stderr
    assert "Module:" not in result.stderr
    assert "Terminal render:" not in result.stderr
    assert "Playback pump:" not in result.stderr


def test_iplay_launcher_playback_check_runs_short_sdl_notcurses_probe() -> None:
    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--check-playback", str(REAL_ARYX_MODULE)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert (
        "iplay.sh: playback-ready: exe=rewrite/.build/iplay video=40x25 "
        "audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod"
    ) in result.stdout
    assert result.stderr == ""
    assert "Module:" not in result.stdout
    assert "File list:" not in result.stdout
    assert "Terminal render:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=keyboard" not in result.stdout


def test_iplay_launcher_playback_check_accepts_filelist_probe(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())
    filelist = tmp_path / "PLAYLIST.LST"
    filelist.write_text("\n\t aryx.s3m \r\nignored.s3m\n")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--check-playback", "@" + str(filelist)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert (
        "iplay.sh: playback-ready: exe=rewrite/.build/iplay video=40x25 "
        "audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod"
    ) in result.stdout
    assert result.stderr == ""
    assert "Module:" not in result.stdout
    assert "File list:" not in result.stdout
    assert "Terminal render:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=keyboard" not in result.stdout


def test_iplay_launcher_playback_check_corrupt_known_tracker_reports_decoder_failure(tmp_path: Path) -> None:
    module = tmp_path / "BAD.S3M"
    module.write_bytes(b"not a tracker module")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--check-playback", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "iplay.sh: playback readiness check failed: status=external-decoder-failed route_id=0 route=external-library provider=libmikmod" in result.stderr
    assert "iplay.sh: playback readiness check failed with status 3" in result.stderr
    assert "iplay.sh: playback-ready:" not in result.stderr
    assert "Module:" not in result.stderr
    assert "Terminal render:" not in result.stderr
    assert "Playback pump:" not in result.stderr


def test_iplay_launcher_playback_check_unknown_probe_reports_unsupported_format(tmp_path: Path) -> None:
    module = tmp_path / "BAD.BIN"
    module.write_bytes(b"not a tracker module")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--check-playback", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "iplay.sh: playback readiness check failed: status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod" in result.stderr
    assert "iplay.sh: playback readiness check failed with status 3" in result.stderr
    assert "iplay.sh: playback-ready:" not in result.stderr
    assert "Module:" not in result.stderr
    assert "Terminal render:" not in result.stderr
    assert "Playback pump:" not in result.stderr


def test_iplay_launcher_playback_check_missing_module_reports_clean_not_found(tmp_path: Path) -> None:
    missing = tmp_path / "MISSING.S3M"

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--check-playback", str(missing)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "Module not found." in result.stderr
    assert "could not open SDL2 SB16 stereo audio sink" not in result.stderr
    assert "iplay.sh: playback readiness check failed with status 2" in result.stderr
    assert "Playback pump:" not in result.stderr
    assert "Terminal render:" not in result.stderr
    assert "status=" not in result.stderr


def test_iplay_launcher_playback_check_filelist_missing_selected_module_stays_clean(tmp_path: Path) -> None:
    filelist = tmp_path / "PLAYLIST.LST"
    filelist.write_text("missing.s3m\n")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--check-playback", "@" + str(filelist)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "Module not found." in result.stderr
    assert "File list:" not in result.stderr
    assert "could not open SDL2 SB16 stereo audio sink" not in result.stderr
    assert "iplay.sh: playback readiness check failed with status 2" in result.stderr
    assert "Playback pump:" not in result.stderr
    assert "Terminal render:" not in result.stderr
    assert "status=" not in result.stderr


def test_iplay_launcher_playback_check_sdl_audio_failure_is_not_ready(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--check-playback", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "could not open SDL2 SB16 stereo audio sink requested freq=44100 format=0x8010 channels=2 samples=1024:" in result.stderr
    assert "iplay.sh: playback readiness check failed with status 2" in result.stderr
    assert "Module not found." not in result.stderr
    assert "iplay.sh: playback-ready:" not in result.stderr
    assert "Playback pump:" not in result.stderr
    assert "status=" not in result.stderr


def test_iplay_launcher_reports_unsupported_probe_format_without_raw_status(tmp_path: Path) -> None:
    module = tmp_path / "unknown.bin"
    module.write_bytes(b"not a tracker module")

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 3, result.stdout + result.stderr
    assert "status=unsupported-format" not in result.stdout
    assert "iplay.sh: playback failed: status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod" in result.stderr
    assert "Selected text mode: 40x25color cols=40 rows=25" not in result.stdout


def test_iplay_launcher_reports_sdl_audio_open_failure_without_hiding_stderr(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), "--video-mode=40x25color", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "iplay: could not open SDL2 SB16 stereo audio sink requested freq=44100 format=0x8010 channels=2 samples=1024:" in result.stderr
    assert "iplay.sh: player exited with status 2" in result.stderr
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout


def test_iplay_launcher_documented_normal_command_runs_filtered_auto_mode() -> None:
    result = subprocess.run(
        [str(ROOT / "rewrite" / "iplay.sh"), str(REAL_ARYX_MODULE)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Inertia Player V1.22" in result.stdout
    assert "Module: aryx.s3m" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert "iplay.sh: playback failed" not in result.stderr


def test_top_level_iplay_normal_command_runs_filtered_sdl_notcurses_player() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), str(REAL_ARYX_MODULE)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Inertia Player V1.22" in result.stdout
    assert "Module: aryx.s3m" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "Terminal render: requested=1" not in result.stdout
    assert "Terminal live summary:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "Stdin keyboard:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert "iplay.sh: playback failed" not in result.stderr


def test_top_level_iplay_normal_source_end_keeps_full_live_layout_without_diagnostics() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), str(REAL_ARYX_MODULE)],
        cwd=ROOT,
        input="",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "25"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert result.stdout.count("Inertia Player V1.22") > 1
    assert "\x1b[?25l" in result.stdout
    assert "Current Track" in result.stdout
    assert "Track Position" in result.stdout
    assert "Sound Blaster 16" in result.stdout
    assert "Terminal live: block=" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "Terminal live summary:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "status=ok" not in result.stdout
    assert "iplay.sh: playback failed" not in result.stderr


@pytest.mark.parametrize(
    ("columns", "lines", "expected_mode", "expected_render"),
    [
        ("40", "25", "auto cols=40 rows=25", "Terminal render: requested=1 cols=40 rows=25 bytes=2000"),
        ("80", "25", "auto cols=80 rows=25", "Terminal render: requested=1 cols=80 rows=25 bytes=4000"),
    ],
)
def test_top_level_iplay_diagnostics_auto_size_supports_smaller_terminals(
    columns: str,
    lines: str,
    expected_mode: str,
    expected_render: str,
) -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--diagnostics", str(REAL_ARYX_MODULE)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": columns, "LINES": lines},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert f"Selected text mode: {expected_mode}" in result.stdout
    assert expected_render in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout


def test_top_level_iplay_normal_explicit_80x50_runs_filtered_sdl_notcurses_player() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--video-mode=80x50", str(REAL_ARYX_MODULE)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Inertia Player V1.22" in result.stdout
    assert "Module: aryx.s3m" not in result.stdout
    assert "Selected text mode: 80x50" not in result.stdout
    assert "Terminal render: requested=1 cols=80 rows=50 bytes=8000" not in result.stdout
    assert "Terminal live summary:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "Stdin keyboard:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert "iplay.sh: playback failed" not in result.stderr


@pytest.mark.parametrize("mode", ["40x25color", "40x25bw", "40x25", "80x25color", "80x25bw", "80x25", "80x50project"])
def test_top_level_iplay_normal_supported_text_modes_run_filtered_sdl_notcurses_player(mode: str) -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), f"--video-mode={mode}", str(REAL_ARYX_MODULE)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Inertia Player V1.22" in result.stdout
    assert "Module: aryx.s3m" not in result.stdout
    assert f"Selected text mode: {mode}" not in result.stdout
    assert "Terminal render: requested=1" not in result.stdout
    assert "Terminal live summary:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "Stdin keyboard:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert "iplay.sh: playback failed" not in result.stderr


def test_top_level_iplay_normal_accepts_video_mode_after_module() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), str(REAL_ARYX_MODULE), "--video-mode=80x50"],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Inertia Player V1.22" in result.stdout
    assert "Module: aryx.s3m" not in result.stdout
    assert "Selected text mode: 80x50" not in result.stdout
    assert "Terminal render: requested=1 cols=80 rows=50 bytes=8000" not in result.stdout
    assert "Terminal live summary:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "Stdin keyboard:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert "iplay.sh: playback failed" not in result.stderr


def test_top_level_iplay_invalid_video_mode_fails_before_playback() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--video-mode=bad", str(REAL_ARYX_MODULE)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "iplay: unsupported text mode: bad" in result.stderr
    assert "iplay.sh: player exited with status 2" in result.stderr
    assert "Inertia Player V1.22" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout
    assert "SDL audio sink:" not in result.stdout


def test_top_level_iplay_normal_filelist_runs_filtered_sdl_notcurses_player(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())
    filelist = tmp_path / "PLAYLIST.LST"
    filelist.write_text("\n\t aryx.s3m \r\nignored.s3m\n")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "@" + str(filelist)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Inertia Player V1.22" in result.stdout
    assert f"File list: @{filelist}" not in result.stdout
    assert "Module: aryx.s3m" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "Terminal render: requested=1" not in result.stdout
    assert "Terminal live summary:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "Stdin keyboard:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert "iplay.sh: playback failed" not in result.stderr


def test_top_level_iplay_normal_resolves_module_path_case_like_dos(tmp_path: Path) -> None:
    lowercase = tmp_path / "aryx.s3m"
    lowercase.write_bytes(REAL_ARYX_MODULE.read_bytes())
    requested = tmp_path / "ARYX.S3M"

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), str(requested)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Inertia Player V1.22" in result.stdout
    assert "Module not found." not in result.stderr
    assert "Module: aryx.s3m" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert "iplay.sh: playback failed" not in result.stderr


def test_top_level_iplay_normal_resolves_filelist_path_case_like_dos(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())
    lowercase_filelist = tmp_path / "playlist.lst"
    requested_filelist = tmp_path / "PLAYLIST.LST"
    lowercase_filelist.write_text("aryx.s3m\n")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "@" + str(requested_filelist)],
        cwd=ROOT,
        input="q\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Inertia Player V1.22" in result.stdout
    assert "Module not found." not in result.stderr
    assert f"File list: @{requested_filelist}" not in result.stdout
    assert "Module: aryx.s3m" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert "iplay.sh: playback failed" not in result.stderr


def test_top_level_iplay_missing_module_reports_original_style_not_found(tmp_path: Path) -> None:
    missing = tmp_path / "MISSING.S3M"

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), str(missing)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "Module not found." in result.stderr
    assert "iplay.sh: player exited with status 2" in result.stderr
    assert "Inertia Player V1.22" not in result.stdout
    assert "Module:" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "Terminal render: requested=1" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout


def test_top_level_iplay_missing_module_reports_not_found_before_sdl_audio_failure(tmp_path: Path) -> None:
    missing = tmp_path / "MISSING.S3M"

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), str(missing)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "Module not found." in result.stderr
    assert "could not open SDL2 SB16 stereo audio sink" not in result.stderr
    assert "iplay.sh: player exited with status 2" in result.stderr
    assert "SDL audio sink:" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout


def test_top_level_iplay_missing_filelist_reports_clear_no_playback_error(tmp_path: Path) -> None:
    missing_filelist = tmp_path / "MISSING.LST"

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "@" + str(missing_filelist)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert f"iplay: could not resolve file list: @{missing_filelist}" in result.stderr
    assert "iplay.sh: player exited with status 2" in result.stderr
    assert "Inertia Player V1.22" not in result.stdout
    assert "File list:" not in result.stdout
    assert "Module:" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout


def test_top_level_iplay_empty_filelist_reports_clear_no_playback_error(tmp_path: Path) -> None:
    empty_filelist = tmp_path / "EMPTY.LST"
    empty_filelist.write_text("\n\t \r\n")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "@" + str(empty_filelist)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert f"iplay: could not resolve file list: @{empty_filelist}" in result.stderr
    assert "iplay.sh: player exited with status 2" in result.stderr
    assert "Inertia Player V1.22" not in result.stdout
    assert "File list:" not in result.stdout
    assert "Module:" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout


def test_top_level_iplay_filelist_missing_selected_module_reports_not_found(tmp_path: Path) -> None:
    filelist = tmp_path / "PLAYLIST.LST"
    filelist.write_text("missing.s3m\n")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "@" + str(filelist)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "Module not found." in result.stderr
    assert "iplay.sh: player exited with status 2" in result.stderr
    assert "Inertia Player V1.22" not in result.stdout
    assert "File list:" not in result.stdout
    assert "Module:" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout


def test_top_level_iplay_filelist_missing_selected_module_reports_not_found_before_sdl_audio_failure(tmp_path: Path) -> None:
    filelist = tmp_path / "PLAYLIST.LST"
    filelist.write_text("missing.s3m\n")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "@" + str(filelist)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "Module not found." in result.stderr
    assert "could not open SDL2 SB16 stereo audio sink" not in result.stderr
    assert "iplay.sh: player exited with status 2" in result.stderr
    assert "File list:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout


def test_top_level_iplay_corrupt_known_tracker_reports_filtered_decoder_failure(tmp_path: Path) -> None:
    module = tmp_path / "BAD.S3M"
    module.write_bytes(b"not a tracker module")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 3, result.stdout + result.stderr
    assert "iplay.sh: playback failed: status=external-decoder-failed route_id=0 route=external-library provider=libmikmod" in result.stderr
    assert "status=external-decoder-failed" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "Playback pump:" not in result.stdout


def test_top_level_iplay_project_owned_inr_reports_filtered_unavailable(tmp_path: Path) -> None:
    module = tmp_path / "SONG.INR"
    module.write_bytes(b"INR placeholder")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 3, result.stdout + result.stderr
    assert "iplay.sh: playback failed: status=project-decoder-unavailable route_id=1 route=project-owned provider=native" in result.stderr
    assert "status=project-decoder-unavailable" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "Playback pump:" not in result.stdout


def test_top_level_iplay_unknown_probe_reports_filtered_unsupported_format(tmp_path: Path) -> None:
    module = tmp_path / "BAD.BIN"
    module.write_bytes(b"not a tracker module")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 3, result.stdout + result.stderr
    assert "iplay.sh: playback failed: status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod" in result.stderr
    assert "status=unsupported-format" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "Playback pump:" not in result.stdout


def test_top_level_iplay_sdl_audio_open_failure_preserves_stderr(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "iplay: could not open SDL2 SB16 stereo audio sink requested freq=44100 format=0x8010 channels=2 samples=1024:" in result.stderr
    assert "iplay.sh: player exited with status 2" in result.stderr
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout
    assert "SDL audio sink: requested=1 opened=1" not in result.stdout


def test_top_level_iplay_lists_external_tracker_extensions_without_playback() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--list-extensions"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert result.stdout.startswith("extensions=")
    for ext in [".mod", ".s3m", ".xm", ".it", ".far", ".mtm"]:
        assert ext in result.stdout
    assert "Module:" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout


def test_top_level_iplay_classifies_decoder_routes_without_playback(tmp_path: Path) -> None:
    s3m = tmp_path / "song.s3m"
    inr = tmp_path / "song.inr"
    unknown = tmp_path / "song.bin"
    s3m.write_bytes(b"")
    inr.write_bytes(b"")
    unknown.write_bytes(b"")

    cases = [
        (s3m, "route_id=0 route=external-library library=1"),
        (inr, "route_id=1 route=project-owned library=0"),
        (unknown, "route_id=2 route=probe-by-content library=1"),
    ]
    for path, expected in cases:
        result = subprocess.run(
            [str(ROOT / "iplay.sh"), "--classify", str(path)],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
            timeout=30,
        )

        assert result.returncode == 0, result.stdout + result.stderr
        assert expected in result.stdout
        assert "backend=\"SDL-compatible SB16 16-bit stereo\"" in result.stdout
        assert "Module:" not in result.stdout
        assert "Playback pump:" not in result.stdout
        assert "status=" not in result.stdout


def test_top_level_iplay_launcher_help_delegates_to_preferred_launcher() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--help"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "usage: ./iplay.sh [--check] [--rebuild] [--diagnostics] [iplay-options] [module-file|@file-list]" in result.stderr
    assert "example: ./iplay.sh --video-mode=80x50 samples/aryx.s3m" in result.stderr
    assert "usage: ./rewrite/iplay.sh" not in result.stderr
    assert "press q, Q, or Escape to stop playback" in result.stderr


@pytest.mark.parametrize("prefix", [["--diagnostics"], ["--rebuild"]])
def test_top_level_iplay_help_after_wrapper_flags_stays_launcher_help(prefix: list[str]) -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), *prefix, "--help"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=5,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "usage: ./iplay.sh [--check] [--rebuild] [--diagnostics] [iplay-options] [module-file|@file-list]" in result.stderr
    assert "usage: ./rewrite/iplay.sh" not in result.stderr
    assert "usage: iplay" not in result.stdout + result.stderr
    assert "Module:" not in result.stdout
    assert "status=" not in result.stdout


def test_top_level_iplay_without_arguments_opens_module_selector() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh")],
        cwd=ROOT,
        input="q",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "IPLAY_LAUNCHER_DIAGNOSTICS": "1"},
        timeout=5,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert "usage: ./iplay.sh" not in result.stderr
    assert "usage: ./rewrite/iplay.sh" not in result.stderr
    assert "Module:" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout


def test_native_selector_accepts_enter_for_selected_module(tmp_path: Path) -> None:
    module = tmp_path / "SELECT.S3M"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())
    result = subprocess.run(
        [str(ROOT / "rewrite" / ".build" / "iplay")],
        cwd=tmp_path,
        input="\x0e\nq",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=10,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "Module: SELECT.S3M" in result.stdout


def test_top_level_iplay_launcher_check_delegates_to_preferred_launcher() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "iplay.sh: ready: exe=rewrite/.build/iplay rebuilt=" in result.stdout
    assert "Module:" not in result.stdout
    assert "status=" not in result.stdout


def test_top_level_iplay_playback_check_runs_short_sdl_notcurses_probe() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--check-playback", str(REAL_ARYX_MODULE)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "iplay.sh: playback-ready: exe=rewrite/.build/iplay video=40x25 audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod" in result.stdout
    assert "Module: aryx.s3m" not in result.stdout
    assert "Terminal render:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert result.stderr == ""


def test_top_level_iplay_playback_check_accepts_filelist_probe(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())
    filelist = tmp_path / "PLAYLIST.LST"
    filelist.write_text("\n\t aryx.s3m \r\nignored.s3m\n")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--check-playback", "@" + str(filelist)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "iplay.sh: playback-ready: exe=rewrite/.build/iplay video=40x25 audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod" in result.stdout
    assert "File list:" not in result.stdout
    assert "Module:" not in result.stdout
    assert "Terminal render:" not in result.stdout
    assert "SDL audio sink:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert result.stderr == ""


@pytest.mark.parametrize(
    ("video_mode", "reported_mode"),
    [
        ("40x25color", "40x25"),
        ("80x25color", "80x25"),
        ("80x50", "80x50"),
    ],
)
def test_top_level_iplay_playback_check_honors_explicit_video_mode(
    video_mode: str,
    reported_mode: str,
) -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--check-playback", str(REAL_ARYX_MODULE), f"--video-mode={video_mode}"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert f"iplay.sh: playback-ready: exe=rewrite/.build/iplay video={reported_mode} audio=SDL-compatible-SB16-stereo route=external-library provider=libmikmod" in result.stdout
    assert "Terminal render:" not in result.stdout
    assert "Selected text mode:" not in result.stdout
    assert "status=keyboard" not in result.stdout
    assert result.stderr == ""


def test_top_level_iplay_playback_check_missing_module_reports_clean_not_found(tmp_path: Path) -> None:
    missing = tmp_path / "MISSING.S3M"

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--check-playback", str(missing)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "Module not found." in result.stderr
    assert "could not open SDL2 SB16 stereo audio sink" not in result.stderr
    assert "iplay.sh: playback readiness check failed with status 2" in result.stderr
    assert "Playback pump:" not in result.stderr
    assert "Terminal render:" not in result.stderr
    assert "status=" not in result.stderr


def test_top_level_iplay_playback_check_filelist_missing_selected_module_stays_clean(tmp_path: Path) -> None:
    filelist = tmp_path / "PLAYLIST.LST"
    filelist.write_text("missing.s3m\n")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--check-playback", "@" + str(filelist)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "Module not found." in result.stderr
    assert "File list:" not in result.stderr
    assert "could not open SDL2 SB16 stereo audio sink" not in result.stderr
    assert "iplay.sh: playback readiness check failed with status 2" in result.stderr
    assert "Playback pump:" not in result.stderr
    assert "Terminal render:" not in result.stderr
    assert "status=" not in result.stderr


def test_top_level_iplay_playback_check_corrupt_known_tracker_reports_decoder_failure(tmp_path: Path) -> None:
    module = tmp_path / "BAD.S3M"
    module.write_bytes(b"not a tracker module")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--check-playback", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "status=external-decoder-failed route_id=0 route=external-library provider=libmikmod" in result.stderr
    assert "iplay.sh: playback readiness check failed with status 3" in result.stderr
    assert "iplay.sh: playback-ready:" not in result.stderr
    assert "Module:" not in result.stderr
    assert "Terminal render:" not in result.stderr
    assert "Playback pump:" not in result.stderr


def test_top_level_iplay_playback_check_unknown_probe_reports_unsupported_format(tmp_path: Path) -> None:
    module = tmp_path / "BAD.BIN"
    module.write_bytes(b"not a tracker module")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--check-playback", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "status=unsupported-format route_id=2 route=probe-by-content provider=libmikmod" in result.stderr
    assert "iplay.sh: playback readiness check failed with status 3" in result.stderr
    assert "iplay.sh: playback-ready:" not in result.stderr
    assert "Module:" not in result.stderr
    assert "Terminal render:" not in result.stderr
    assert "Playback pump:" not in result.stderr


def test_top_level_iplay_playback_check_project_owned_reports_decoder_unavailable(tmp_path: Path) -> None:
    module = tmp_path / "SONG.INR"
    module.write_bytes(b"INR placeholder")

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--check-playback", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "status=project-decoder-unavailable route_id=1 route=project-owned provider=native" in result.stderr
    assert "iplay.sh: playback readiness check failed with status 3" in result.stderr
    assert "iplay.sh: playback-ready:" not in result.stderr
    assert "Module:" not in result.stderr
    assert "Terminal render:" not in result.stderr
    assert "Playback pump:" not in result.stderr


def test_top_level_iplay_playback_check_sdl_audio_failure_is_not_ready(tmp_path: Path) -> None:
    module = tmp_path / "aryx.s3m"
    module.write_bytes(REAL_ARYX_MODULE.read_bytes())

    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--check-playback", str(module)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver"},
        timeout=30,
    )

    assert result.returncode == 2, result.stdout + result.stderr
    assert result.stdout == ""
    assert "could not open SDL2 SB16 stereo audio sink requested freq=44100 format=0x8010 channels=2 samples=1024:" in result.stderr
    assert "iplay.sh: playback readiness check failed with status 2" in result.stderr
    assert "Module not found." not in result.stderr
    assert "iplay.sh: playback-ready:" not in result.stderr
    assert "Playback pump:" not in result.stderr
    assert "status=" not in result.stderr


def test_top_level_iplay_rebuild_check_delegates_to_preferred_launcher_without_playback() -> None:
    result = subprocess.run(
        [str(ROOT / "iplay.sh"), "--rebuild", "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=30,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "iplay.sh: ready: exe=rewrite/.build/iplay rebuilt=1" in result.stdout
    assert "Module:" not in result.stdout
    assert "Playback pump:" not in result.stdout
    assert "status=" not in result.stdout
