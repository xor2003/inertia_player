from __future__ import annotations

import os
import re
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BUILD_DIR = ROOT / "rewrite" / ".build"
PROBE = BUILD_DIR / "modplug_probe"
AUDIO_PROBE = BUILD_DIR / "iplay_native"
PLAYER_ALIAS = BUILD_DIR / "iplay"
HOOK_PROBE = BUILD_DIR / "modplug_player_hook_probe"
MIKMOD_PROBE = BUILD_DIR / "mikmod_channel_probe"
PROBE_COMPILED = False
AUDIO_PROBE_COMPILED = False
HOOK_PROBE_COMPILED = False
MIKMOD_PROBE_COMPILED = False


def run(cmd: list[str], check: bool = True, timeout: int | None = None, env: dict[str, str] | None = None, input_text: str | None = None) -> subprocess.CompletedProcess[str]:
    if timeout is None:
        timeout = int(os.environ.get("IPLAY_HOST_TEST_TIMEOUT", "30"))
    return subprocess.run(cmd,
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=check,
        timeout=timeout,
        env=env,
        input=input_text,
    )


def aryx_s3m_path() -> Path:
    for source in (
        ROOT.parent / "old" / "aryx.s3m",
        ROOT.parent / "masm" / "BIN" / "aryx.s3m",
        ROOT.parent / "libdosbox-0.5x" / "i" / "aryx.s3m",
        BUILD_DIR / "aryx.s3m",
    ):
        if source.exists():
            return source
    raise AssertionError("missing aryx.s3m fixture")


def hacker4_s3m_path() -> Path:
    for source in (
        ROOT / "HACKER4.S3M",
        BUILD_DIR / "HACKER4.S3M",
    ):
        if source.exists():
            return source
    raise AssertionError("missing HACKER4.S3M fixture")


def write_sparse_active_channel_mod(path: Path, include_compatibility_events: bool = False) -> Path:
    module = bytearray(1084 + 1024 + 64)
    module[0:10] = b"ACTIVE MOD"
    module[20:33] = b"ACTIVE SAMPLE"
    module[42:44] = (32).to_bytes(2, "big")
    module[45] = 64
    module[48:50] = (1).to_bytes(2, "big")
    module[950] = 1
    module[1080:1084] = b"M.K."
    period = 428
    module[1084] = (period >> 8) & 0x0F
    module[1085] = period & 0xFF
    module[1086] = 0x1F
    module[1087] = 0x06
    if include_compatibility_events:
        module[1102] = 0x04
        module[1103] = 0x0F
        module[1118] = 0x0F
        module[1119] = 0x40
    for index in range(64):
        module[2108 + index] = (index * 8) & 0xFF
    path.write_bytes(module)
    return path


def compile_probe() -> None:
    global PROBE_COMPILED
    if PROBE_COMPILED:
        return
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    cflags = run(["pkg-config", "--cflags", "libmodplug"]).stdout.split()
    libs = run(["pkg-config", "--libs", "libmodplug"]).stdout.split()
    run([
        "c++",
        "-std=c++17",
        "-O0",
        "-Wall",
        "-Wextra",
        *cflags,
        "-Irewrite",
        str(ROOT / "rewrite" / "modplug_renderer.cpp"),
        str(ROOT / "rewrite" / "modplug_probe.cpp"),
        *libs,
        "-o",
        str(PROBE),
    ])
    PROBE_COMPILED = True


def compile_audio_probe() -> None:
    global AUDIO_PROBE_COMPILED
    if AUDIO_PROBE_COMPILED:
        return
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    cflags = run(["pkg-config", "--cflags", "libmodplug", "sdl2", "SDL2_image", "notcurses"]).stdout.split()
    libs = run(["pkg-config", "--libs", "libmodplug", "sdl2", "SDL2_image", "notcurses"]).stdout.split()
    rewrite_obj = BUILD_DIR / "iplay_rewrite_for_modplug_audio.o"
    run([
        "cc",
        "-std=c99",
        "-O0",
        "-Wall",
        "-Wextra",
        "-Irewrite",
        "-c",
        str(ROOT / "rewrite" / "iplay_rewrite.c"),
        "-o",
        str(rewrite_obj),
    ])
    run([
        "c++",
        "-std=c++17",
        "-O0",
        "-Wall",
        "-Wextra",
        *cflags,
        "-Irewrite",
        str(ROOT / "rewrite" / "modplug_renderer.cpp"),
        str(ROOT / "rewrite" / "modplug_audio_bridge.cpp"),
        str(ROOT / "rewrite" / "modern_player.cpp"),
        str(ROOT / "rewrite" / "notcurses_presenter.cpp"),
        str(ROOT / "rewrite" / "sdl_visualizer.cpp"),
        str(ROOT / "rewrite" / "modplug_audio_probe.cpp"),
        str(rewrite_obj),
        *libs,
        "-o",
        str(AUDIO_PROBE),
    ])
    player_alias_tmp = PLAYER_ALIAS.with_name(f"{PLAYER_ALIAS.name}.{os.getpid()}.tmp")
    try:
        player_alias_tmp.write_bytes(AUDIO_PROBE.read_bytes())
        player_alias_tmp.chmod(0o755)
        os.replace(player_alias_tmp, PLAYER_ALIAS)
    finally:
        player_alias_tmp.unlink(missing_ok=True)
    AUDIO_PROBE_COMPILED = True


def compile_hook_probe() -> None:
    global HOOK_PROBE_COMPILED
    if HOOK_PROBE_COMPILED:
        return
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    cflags = run(["pkg-config", "--cflags", "libmodplug"]).stdout.split()
    libs = run(["pkg-config", "--libs", "libmodplug"]).stdout.split()
    rewrite_obj = BUILD_DIR / "iplay_rewrite_for_modplug_hook.o"
    run([
        "cc",
        "-std=c99",
        "-O0",
        "-Wall",
        "-Wextra",
        "-Irewrite",
        "-c",
        str(ROOT / "rewrite" / "iplay_rewrite.c"),
        "-o",
        str(rewrite_obj),
    ])
    run([
        "c++",
        "-std=c++17",
        "-O0",
        "-Wall",
        "-Wextra",
        "-DIPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER=1",
        *cflags,
        "-Irewrite",
        str(ROOT / "rewrite" / "modplug_renderer.cpp"),
        str(ROOT / "rewrite" / "modplug_audio_bridge.cpp"),
        str(ROOT / "rewrite" / "modplug_player_hook_probe.cpp"),
        str(rewrite_obj),
        *libs,
        "-o",
        str(HOOK_PROBE),
    ])
    HOOK_PROBE_COMPILED = True


def compile_mikmod_probe() -> None:
    global MIKMOD_PROBE_COMPILED
    if MIKMOD_PROBE_COMPILED:
        return
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    cflags = run(["pkg-config", "--cflags", "libmodplug"]).stdout.split()
    libs = run(["pkg-config", "--libs", "libmodplug"]).stdout.split()
    run([
        "c++",
        "-std=c++17",
        "-O0",
        "-Wall",
        "-Wextra",
        *cflags,
        "-Irewrite",
        str(ROOT / "rewrite" / "modplug_renderer.cpp"),
        str(ROOT / "rewrite" / "mikmod_channel_probe.cpp"),
        *libs,
        "-o",
        str(MIKMOD_PROBE),
    ])
    MIKMOD_PROBE_COMPILED = True


def test_libmikmod_provider_applies_real_channel_mute_and_pan_to_pcm() -> None:
    compile_mikmod_probe()
    result = run([str(MIKMOD_PROBE), str(aryx_s3m_path())])
    match = re.search(
        r"baseline=(\d+) muted=(\d+) left_pan=(\d+) right_pan=(\d+) "
        r"left_energy=(\d+) right_energy=(\d+) baseline_level=(\d+) "
        r"scale_zero=(\d+) scale_full_one=(\d+) scale_half_four=(\d+) scale_full_four=(\d+) "
        r"muted_state=(\d+) order=(\d+) row=(\d+)",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    (
        baseline,
        muted,
        left_pan,
        right_pan,
        left_energy,
        right_energy,
        baseline_level,
        scale_zero,
        scale_full_one,
        scale_half_four,
        scale_full_four,
        muted_state,
        order,
        row,
    ) = (
        int(group) for group in match.groups()
    )
    assert baseline != 0
    assert muted != baseline
    assert left_pan != right_pan
    assert left_energy != 0
    assert right_energy != 0
    assert 0 < baseline_level <= 60
    assert scale_zero == 0
    assert 0 < scale_full_one < scale_full_four
    assert 0 < scale_half_four < scale_full_four
    assert scale_full_four == 60
    assert muted_state == 0
    assert order >= 0
    assert row >= 0


def test_preferred_player_channel_mute_and_pan_controls_change_real_pcm() -> None:
    compile_audio_probe()

    def playback(input_text: str) -> tuple[int, str]:
        result = run(
            [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
            check=False,
            input_text=input_text,
            env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        )
        assert result.returncode == 3
        checksum = re.search(r"Playback pump:.*checksum=(\d+)", result.stdout)
        assert checksum
        return int(checksum.group(1)), result.stdout

    baseline, _ = playback("xxq")
    muted, mute_output = playback("1xq")
    _, first_ten_muted_output = playback("1234567890xq")
    _, first_ten_unmuted_output = playback("12345678901234567890xq")
    panned, pan_output = playback("mxq")
    left, _ = playback("lxq")
    right, _ = playback("rxq")
    surround, _ = playback("sxq")
    assert muted != baseline
    assert panned != baseline
    assert left != right
    assert surround not in (left, panned, right)
    assert "provider=libmikmod" in mute_output
    assert "selected_channel=0 muted_mask=1 channel_generation=1" in mute_output
    assert "selected_channel=0 muted_mask=1023 channel_generation=10" in first_ten_muted_output
    assert "selected_channel=0 muted_mask=0 channel_generation=20" in first_ten_unmuted_output
    assert "selected_channel=0 muted_mask=0 channel_generation=1" in pan_output
    listing = (ROOT / "IPLAY.lst").read_text(errors="ignore")
    keyboard = listing[listing.index("seg001:0EAD keyb_19EFD"):listing.index("seg001:0FEF keyb_19EFD      endp")]
    assert "cmp     al, 4Dh ; 'M'" in keyboard
    assert "cmp     al, 4Bh ; 'K'" in keyboard
    assert "cmp     al, 26h ; '&'" in keyboard
    assert "cmp     al, 32h ; '2'" in keyboard
    assert "cmp     al, 13h" in keyboard
    assert "cmp     al, 1Fh" in keyboard
    assert "cmp     al, 0Bh" in keyboard


def test_preferred_player_modified_digits_mute_original_channel_banks() -> None:
    compile_audio_probe()

    def playback(input_text: str) -> tuple[int, str]:
        result = run(
            [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=512", "80x25color"],
            check=False,
            input_text=input_text,
            env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        )
        assert result.returncode == 3
        checksum = re.search(r"Playback pump:.*checksum=(\d+)", result.stdout)
        assert checksum
        return int(checksum.group(1)), result.stdout

    baseline, _ = playback("x")
    channel_11, shifted_output = playback("!")
    channel_21, alt_output = playback("\x1b1")
    assert channel_11 != baseline
    assert channel_21 == baseline
    assert "muted_mask=1024" in shifted_output
    assert "muted_mask=1048576" in alt_output
    listing = (ROOT / "IPLAY.lst").read_text(errors="ignore")
    keyboard = listing[listing.index("seg001:12EE loc_1A33E"):listing.index("seg001:132F")]
    assert "add     al, 10" in keyboard
    assert "add     al, 20" in keyboard


def test_preferred_player_pattern_loop_and_end_pattern_follow_original_dispatch() -> None:
    compile_audio_probe()
    looped = run(
        [str(PLAYER_ALIAS), str(hacker4_s3m_path()), "--blocks=1000", "80x25color"],
        check=False,
        input_text="\x1b[Fxc",
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
    )
    ended = run(
        [str(PLAYER_ALIAS), str(hacker4_s3m_path()), "--blocks=32", "80x25color"],
        check=False,
        input_text="\x1b[Fxq",
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
    )
    assert looped.returncode == 3
    assert ended.returncode == 3
    assert "pattern_loop=1:1" in looped.stdout
    assert "stop=block-limit source_end=0" in looped.stdout
    assert "Current Track : 3/" not in looped.stdout
    assert "seek=1:0 seek_generation=1" in ended.stdout
    listing = (ROOT / "IPLAY.lst").read_text(errors="ignore")
    keyboard = listing[listing.index("seg001:1247 loc_1A297"):listing.index("seg001:132F")]
    assert "xor     al, 1000b" in keyboard
    assert "mov     al, 2" in keyboard
    assert "mov     dx, 0Dh" in keyboard


def test_preferred_player_tab_toggles_original_pal_ntsc_real_pcm_timing() -> None:
    compile_audio_probe()

    def playback(input_text: str) -> tuple[int, str]:
        result = run(
            [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
            check=False,
            input_text=input_text,
            env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        )
        assert result.returncode == 3
        checksum = re.search(r"Playback pump:.*checksum=(\d+)", result.stdout)
        assert checksum
        return int(checksum.group(1)), result.stdout

    pal, pal_output = playback("xq")
    ntsc, ntsc_output = playback("\txq")
    assert pal != ntsc
    assert "pal=1" in pal_output
    assert "pal=0" in ntsc_output
    assert "(NTSC)" in ntsc_output
    listing = (ROOT / "IPLAY.lst").read_text(errors="ignore")
    keyboard = listing[listing.index("seg001:1247 loc_1A297"):listing.index("seg001:1271")]
    assert "xor     al, 1000b" in keyboard
    assert "Toggle PAL/NTSC" in listing


def test_libmodplug_decodes_real_s3m_to_signed_stereo_pcm() -> None:
    compile_probe()
    result = run([str(PROBE), str(aryx_s3m_path())])
    match = re.search(r"frames=(\d+) samples=(\d+) bytes=(\d+) checksum=(\d+) peak=(\d+)", result.stdout)
    assert match, result.stdout + result.stderr
    frames, samples, byte_count, checksum, peak = (int(group) for group in match.groups())
    assert frames > 0
    assert samples == frames * 2
    assert byte_count == samples * 2
    assert checksum != 0
    assert peak > 0


def test_original_channel_meter_envelope_rises_holds_and_decays_at_original_cadence() -> None:
    compile_probe()
    result = run([str(PROBE), "--meter-envelope"])
    assert result.stdout == "rise=60 hold31=60 decay32=57 decay64=54 rerise=60 cells=30\n"


def test_channel_pan_uses_live_stereo_vu_ratio_and_preserves_silent_fallback() -> None:
    compile_probe()
    result = run([str(PROBE), "--pan-ratio"])
    assert result.stdout == "left=0 center=64 right=128 silent=37\n"


def test_original_listing_defines_channel_meter_clamp_peak_hold_decay_and_cell_scale() -> None:
    listing = (ROOT / "IPLAY.lst").read_text(errors="ignore")
    start = listing.index("seg001:18A8 f3_draw:")
    end = listing.index("seg001:1A7B loc_1AACB:", start)
    meter = listing[start:end]
    assert "cmp     al, 60" in meter
    assert "mov     al, 60" in meter
    assert "cmp     al, fs:[bx+1Ah]" in meter
    assert "and     byte_1DE71, 1Fh" in meter
    assert "sub     fs:[bx+1Ah], al" in meter
    assert "movzx   cx, byte ptr fs:[bx+1Ah]" in meter
    assert "shr     cx, 1" in meter
    assert "mov     dx, 30" in meter


def test_libmodplug_renders_player_sized_512_frame_stereo_block() -> None:
    compile_probe()
    result = run([str(PROBE), str(aryx_s3m_path()), "512"])
    match = re.search(r"frames=(\d+) samples=(\d+) bytes=(\d+) checksum=(\d+) peak=(\d+)", result.stdout)
    assert match, result.stdout + result.stderr
    frames, samples, byte_count, checksum, peak = (int(group) for group in match.groups())
    assert frames == 512
    assert samples == 1024
    assert byte_count == 2048
    assert checksum != 0
    assert peak > 0


def test_libmodplug_ui_retains_active_channel_state_across_sparse_rows(tmp_path: Path) -> None:
    compile_probe()
    module = write_sparse_active_channel_mod(tmp_path / "ACTIVE.MOD")
    result = run([str(PROBE), str(module), "--ui-blocks=48"])
    snapshots = [
        (int(row), int(note), int(instrument), int(effect), int(parameter), sample)
        for row, note, instrument, effect, parameter, sample in re.findall(
            r'row=(\d+) note=(\d+) instrument=(\d+) effect=(\d+) parameter=(\d+) sample="([^"]*)"',
            result.stdout,
        )
    ]
    assert snapshots
    later = [snapshot for snapshot in snapshots if snapshot[0] > 0]
    assert later
    assert all(note != 0 for _, note, _, _, _, _ in later)
    assert all(instrument == 1 for _, _, instrument, _, _, _ in later)
    assert all(effect != 0 and parameter == 6 for _, _, _, effect, parameter, _ in later)
    assert all(sample == "ACTIVE SAMPLE" for _, _, _, _, _, sample in later)
    telemetry = [
        (int(available), int(level))
        for available, level in re.findall(r"vu_available=(\d+) level=(\d+)", result.stdout)
    ]
    assert telemetry
    assert all(available == 1 for available, _ in telemetry)
    assert any(level > 0 for _, level in telemetry)
    assert all(0 <= level <= 30 for _, level in telemetry)
    pans = [
        (int(pan), int(valid))
        for pan, valid in re.findall(r"pan=(\d+) pan_valid=(\d+)", result.stdout)
    ]
    assert pans
    assert all(0 <= pan <= 128 and valid == 1 for pan, valid in pans)


def test_libmodplug_ui_falls_back_when_openmpt_telemetry_is_disabled(tmp_path: Path) -> None:
    compile_probe()
    module = write_sparse_active_channel_mod(tmp_path / "ACTIVE.MOD")
    result = run(
        [str(PROBE), str(module), "--ui-blocks=8"],
        env={**os.environ, "IPLAY_DISABLE_OPENMPT_TELEMETRY": "1"},
    )
    telemetry = [
        (int(available), int(level))
        for available, level in re.findall(r"vu_available=(\d+) level=(\d+)", result.stdout)
    ]
    assert telemetry
    assert all(available == 0 and level == 0 for available, level in telemetry)


def test_libmodplug_decodes_large_real_s3m_to_player_sized_stereo_block() -> None:
    compile_probe()
    result = run([str(PROBE), str(hacker4_s3m_path()), "--until-end"])
    match = re.search(r"frames=(\d+) samples=(\d+) bytes=(\d+) checksum=(\d+) peak=(\d+)", result.stdout)
    assert match, result.stdout + result.stderr
    frames, samples, byte_count, checksum, peak = (int(group) for group in match.groups())
    assert frames > 512
    assert samples == frames * 2
    assert byte_count == samples * 2
    assert checksum != 0
    assert peak > 0


def test_libmodplug_stateful_renderer_reads_consecutive_512_frame_blocks() -> None:
    compile_probe()
    result = run([str(PROBE), str(aryx_s3m_path()), "--twoblocks"])
    match = re.search(
        r"first_frames=(\d+) samples=(\d+) bytes=(\d+) checksum=(\d+) peak=(\d+) "
        r"second_frames=(\d+) second_samples=(\d+) second_bytes=(\d+) second_checksum=(\d+) second_peak=(\d+)",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    (
        first_frames,
        first_samples,
        first_bytes,
        first_checksum,
        first_peak,
        second_frames,
        second_samples,
        second_bytes,
        second_checksum,
        second_peak,
    ) = (int(group) for group in match.groups())
    assert first_frames == second_frames == 512
    assert first_samples == second_samples == 1024
    assert first_bytes == second_bytes == 2048
    assert first_checksum != 0
    assert second_checksum != 0
    assert first_peak > 0
    assert second_peak > 0
    assert first_checksum != second_checksum


def test_libmodplug_stateful_renderer_can_read_until_natural_source_end() -> None:
    compile_probe()
    result = run([str(PROBE), str(aryx_s3m_path()), "--until-end"])
    match = re.search(r"frames=(\d+) samples=(\d+) bytes=(\d+) checksum=(\d+) peak=(\d+)", result.stdout)
    assert match, result.stdout + result.stderr
    frames, samples, byte_count, checksum, peak = (int(group) for group in match.groups())
    assert frames > 512
    assert frames < 16384 * 512
    assert samples == frames * 2
    assert byte_count == samples * 2
    assert checksum != 0
    assert peak > 0


def test_libmodplug_pcm_source_matches_player_read_until_ended_contract() -> None:
    compile_probe()
    result = run([str(PROBE), str(aryx_s3m_path()), "--source-loop"])
    match = re.search(
        r"blocks=(\d+) ended=(\d+) frames=(\d+) samples=(\d+) bytes=(\d+) checksum=(\d+) peak=(\d+)",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    blocks, ended, frames, samples, byte_count, checksum, peak = (int(group) for group in match.groups())
    assert blocks > 1
    assert ended == 1
    assert frames > 512
    assert frames < 16384 * 512
    assert samples == frames * 2
    assert byte_count == samples * 2
    assert checksum != 0
    assert peak > 0


def test_libmodplug_pcm_source_feeds_rewrite_sdl_sb16_audio_contract() -> None:
    compile_audio_probe()
    result = run([str(AUDIO_PROBE), str(aryx_s3m_path())])
    match = re.search(
        r"status=(\w+) route_id=(\d+) route=([\w-]+) provider=([\w-]+) stop=([\w-]+) source_end=(\d+) blocks=(\d+) source_frames=(\d+) accepted_bytes=(\d+) frames_written=(\d+) dropped=(\d+) "
        r"capture_calls=(\d+) capture_bytes=(\d+) capture_checksum=(\d+) source_checksum=(\d+) "
        r"levels=(\d+),(\d+) maxlevels=(\d+),(\d+) active=(\d+) summary=\"([^\"]+)\"",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    (
        status,
        route_id,
        route,
        provider,
        stop_reason,
        source_end,
        blocks,
        source_frames,
        accepted_bytes,
        frames_written,
        dropped,
        capture_calls,
        capture_bytes,
        capture_checksum,
        source_checksum,
        left_level,
        right_level,
        max_left_level,
        max_right_level,
        active,
        summary,
    ) = match.groups()
    route_id = int(route_id)
    source_end = int(source_end)
    blocks = int(blocks)
    source_frames = int(source_frames)
    accepted_bytes = int(accepted_bytes)
    frames_written = int(frames_written)
    dropped = int(dropped)
    capture_calls = int(capture_calls)
    capture_bytes = int(capture_bytes)
    capture_checksum = int(capture_checksum)
    source_checksum = int(source_checksum)
    left_level = int(left_level)
    right_level = int(right_level)
    max_left_level = int(max_left_level)
    max_right_level = int(max_right_level)
    active = int(active)
    assert status == "ok"
    assert route_id == 0
    assert route == "external-library"
    assert provider == "libmikmod"
    assert stop_reason == "source-end"
    assert source_end == 1
    assert blocks > 1
    assert source_frames > 512
    assert accepted_bytes == source_frames * 4
    assert frames_written == source_frames
    assert dropped == 0
    assert capture_calls == blocks
    assert capture_bytes == source_frames * 4
    assert capture_checksum != 0
    assert source_checksum != 0
    assert left_level >= 0
    assert right_level >= 0
    assert max_left_level > 0 or max_right_level > 0
    assert active == 1
    assert "Module: aryx.s3m" in result.stdout
    assert "Size: 20800 bytes" in result.stdout
    assert "Loader: s3m_module (Scream Tracker 3)" in result.stdout
    assert "Module type tag: 204D3353" in result.stdout
    assert "Title: aryx" in result.stdout
    assert "Decoder route: id=0 name=external-library" in result.stdout
    assert "Decoder handoff: external tracker -> SB16 PCM seam." in result.stdout
    assert "Playback output: SDL-compatible SB16 16-bit stereo native." in result.stdout
    assert "PCM source: external_module seed=1 truncated=0 input=file-path renderer=e route=0 provider=libmikmod hook_provider=libmikmod stream_start=0" in result.stdout
    assert "Playback pump: blocks=" in result.stdout
    assert "source_end=1 stop=source-end" in result.stdout
    assert "Screen present: reason=post-playback-status scope=status-only bytes=4000 screen_bytes=4000" in result.stdout
    assert "Audio backend: SDL-compatible SB16 16-bit stereo" in summary
    assert "Playback enabled" in summary
    assert "route=external-library" in summary
    assert "provider=libmikmod" in summary
    assert "status=ok" in summary
    assert "stop=source-end" in summary
    assert "source_end=1" in summary
    assert f"blocks={blocks}" in summary
    assert f"frames={source_frames}" in summary
    assert 'screen_title="' in result.stdout
    assert "Inertia Player V1.22" in result.stdout
    assert 'screen_module="' in result.stdout
    assert re.search(r'screen40_module=".*Filename.*aryx\.s3m', result.stdout)
    assert 'screen_module="Filename      /' not in result.stdout
    assert 'Module' in result.stdout
    assert "Filename      ::" not in result.stdout
    assert "Current Track ::" not in result.stdout
    assert "Track Position::" not in result.stdout
    assert "Playing in Stereo, Free::" not in result.stdout
    assert "Samples Used  ::" not in result.stdout
    assert "Main Volume   ::" not in result.stdout
    assert "Output Levels ::" not in result.stdout
    assert "Module Type   ::" not in result.stdout
    assert 'screen_blocks="' in result.stdout
    assert "Current Track  1/0" in result.stdout
    assert f"blocks={blocks}" in summary
    assert 'screen_stop="' in result.stdout
    assert "Track Position 1/64" in result.stdout
    assert "stop=source-end" in summary
    assert 'screen_audio="' in result.stdout
    assert "Playing in Stereo, Free 482KB" in result.stdout
    assert 'SDL-compatible SB16 16-bit stereo' in result.stdout
    assert 'screen_accepted="' in result.stdout
    assert "Samples Used   0/15" in result.stdout
    assert f"accepted_bytes={accepted_bytes}" in summary
    assert f"dropped={dropped}" in summary
    assert 'screen_frames="' in result.stdout
    assert "Main Volume     100%      - +" in result.stdout
    assert f"frames={source_frames}" in summary
    assert str(source_checksum) in result.stdout
    assert 'screen_levels="' in result.stdout
    assert "Output Levels  L[" in result.stdout
    assert "] R[" in result.stdout
    assert "maxlevels=" in summary
    assert 'screen_playback="' in result.stdout
    assert '24bit Interpolation      F-12' in result.stdout
    assert "Playback enabled" in summary
    assert 'screen_status="' in result.stdout
    assert "Module Type    S3M" in result.stdout
    assert "status=ok" in summary
    assert 'screen40_title="' in result.stdout
    assert 'screen40_module="' in result.stdout
    assert 'screen40_blocks="' in result.stdout
    assert 'screen40_stop="' in result.stdout
    assert 'screen40_audio="' in result.stdout
    assert 'screen40_accepted="' in result.stdout
    assert 'screen40_frames="' in result.stdout
    assert 'screen40_levels="' in result.stdout
    assert 'screen40_playback="' in result.stdout
    assert 'screen40_status="' in result.stdout
    for row_name, row_text in [
        ("module", r"Filename.*aryx\.s3m"),
        ("blocks", r"Current Track.*1/0"),
        ("stop", r"Track Position.*1/64"),
        ("audio", r"Playing in Stereo, Free.*482KB"),
        ("accepted", r"Samples Used.*0/15"),
        ("frames", r"Main Volume.*100%      - \+"),
        ("levels", r"Output Levels.*L\[.*\] R\["),
        ("playback", r"24bit Interpolation      F-12"),
        ("status", r"Module Type.*S3M"),
    ]:
        assert re.search(rf'screen40_{row_name}=".*{row_text}', result.stdout)
    assert "screen40_present=calls:1 bytes:2000 cols:40 rows:25" in result.stdout
    assert 'screen80x50_title="' in result.stdout
    assert 'screen80x50_module="' in result.stdout
    assert 'screen80x50_blocks="' in result.stdout
    assert 'screen80x50_stop="' in result.stdout
    assert 'screen80x50_audio="' in result.stdout
    assert 'screen80x50_accepted="' in result.stdout
    assert 'screen80x50_frames="' in result.stdout
    assert 'screen80x50_levels="' in result.stdout
    assert 'screen80x50_playback="' in result.stdout
    assert 'screen80x50_status="' in result.stdout
    for row_name, row_text in [
        ("module", r"Filename.*aryx\.s3m"),
        ("blocks", r"Current Track.*\d+/\d+"),
        ("stop", r"Track Position.*\d+/\d+"),
        ("audio", r"Playing in Stereo, Free.*\d+KB"),
        ("accepted", r"Samples Used.*\d+/\d+"),
        ("frames", r"Main Volume.*100%      - \+"),
        ("levels", r"\s+1\s+"),
        ("playback", r"24bit Interpolation      F-12"),
        ("status", r"Module Type.*S3M"),
    ]:
        assert re.search(rf'screen80x50_{row_name}=".*{row_text}', result.stdout)
    assert "screen80x50_present=calls:1 bytes:8000 cols:80 rows:50" in result.stdout
    assert "screen_present=calls:1 bytes:4000 cols:80 rows:25" in result.stdout


def test_modern_player_facade_routes_probe_to_reusable_audio_bridge() -> None:
    bridge = (ROOT / "rewrite" / "modplug_audio_bridge.cpp").read_text()
    bridge_h = (ROOT / "rewrite" / "modplug_audio_bridge.hpp").read_text()
    modern = (ROOT / "rewrite" / "modern_player.cpp").read_text()
    probe = (ROOT / "rewrite" / "modplug_audio_probe.cpp").read_text()
    assert "bool iplay_modplug_audio_play_file_to_sdl_sb16(" in bridge
    assert "iplay_sdl_audio_device_write_sb16_frames" in bridge
    assert "#define IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER 0" in bridge_h
    assert "#if IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER" in bridge_h
    assert "#if IPLAY_MODPLUG_ENABLE_PLAYER_HOOK_ADAPTER" in bridge
    assert "IplayModplugExternalDecoder *iplay_modplug_external_decoder_create(void);" in bridge_h
    assert "struct IplayModplugHookAudioStats" in bridge_h
    assert "int block_limit;" in bridge_h
    assert "stats->block_limit = 0;" in bridge
    assert "stats->block_limit = 1;" in bridge
    assert "bool iplay_modplug_external_decoder_play_module_to_sdl_sb16(" in bridge_h
    assert "int iplay_modplug_external_decoder_render(void *user, const PlayerModuleInfo *module, PlayerPlaybackBlock *block);" in bridge_h
    assert "void iplay_modplug_external_decoder_install(IplayModplugExternalDecoder *decoder);" in bridge_h
    assert "void iplay_modplug_external_decoder_uninstall(void);" in bridge_h
    assert 'iplay_player_set_external_decoder(iplay_modplug_external_decoder_render, decoder, "libmikmod");' in bridge
    assert "iplay_player_clear_external_decoder();" in bridge
    assert "const PlayerModuleInfo *module;" in bridge
    assert "decoder->module = 0;" in bridge
    assert "decoder->source && decoder->module == module && decoder->path == path" in bridge
    assert "iplay_player_module_path(module)" in bridge
    assert "iplay_player_playback_block_pcm(block)" in bridge
    assert "iplay_player_playback_block_frames(block)" in bridge
    assert "iplay_player_playback_block_set_frames(block, (dw)stats.frames);" in bridge
    assert "iplay_modplug_pcm_source_read(decoder->source, reinterpret_cast<std::int16_t *>(pcm), (int)frames, &stats)" in bridge
    assert "if (iplay_modplug_pcm_source_ended(decoder->source)) return IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED;" in bridge
    assert "IPLAY_PLAYER_EXTERNAL_DECODER_RENDERED" in bridge
    assert "IPLAY_PLAYER_EXTERNAL_DECODER_SOURCE_ENDED" in bridge
    assert "bool iplay_modplug_external_decoder_play_module_to_sdl_sb16(IplayModplugExternalDecoder *decoder," in bridge
    assert "if (stats) modplug_hook_audio_clear_stats(stats);" in bridge
    assert "static void modplug_hook_audio_write_capture(void *user, const db *pcm, dw byte_count)" in bridge
    assert "capture->stats->capture_calls += 1ul;" in bridge
    assert "capture->stats->capture_bytes += (unsigned long)byte_count;" in bridge
    assert "capture->stats->capture_checksum += (unsigned long)pcm[i];" in bridge
    assert "if (capture->write) capture->write(capture->user, pcm, byte_count);" in bridge
    assert "iplay_sdl_audio_device_init_sb16_compatible(&device, modplug_hook_audio_write_capture, &capture);" in bridge
    assert "stats->accepted_bytes += (unsigned long)iplay_sdl_audio_device_write_sb16_frames(&device, iplay_player_playback_block_pcm(block), iplay_player_playback_block_frames(block));" in bridge
    assert "iplay_player_playback_block_active_bytes(block));" not in bridge
    assert "dw active_bytes;" not in bridge
    assert "stats->accepted_bytes == stats->frames * 4ul" in bridge
    assert "stats->capture_calls == stats->blocks" in bridge
    assert "stats->capture_bytes == stats->accepted_bytes" in bridge
    assert "stats->capture_checksum != 0ul" in bridge
    assert "(stats->max_left_level != 0u || stats->max_right_level != 0u)" in bridge
    assert "iplay_modplug_audio_play_file_to_sdl_sb16_controlled(path, write, write_user, frames_per_block, max_blocks, stop, stop_user, controls, &result->audio)" in modern
    assert "iplay_modern_play_file_to_sdl_sb16_controlled(module_path, native_audio_sink_write, &audio_sink, 512, max_blocks, (run_control.after_blocks || run_control.terminal_live || run_control.stdin_keyboard) ? native_playback_progress : 0, &run_control, &playback_controls, &result)" in probe
    assert "iplay_modern_playback_summary(&result, summary, sizeof(summary))" in probe
    assert "iplay_sdl_audio_device_write_sb16_frames" not in probe
    build = (ROOT / "rewrite" / "build_rewrite.sh").read_text()
    native_build = (ROOT / "rewrite" / "build_native_player.sh").read_text()
    gate = (ROOT / "rewrite" / "check_rewrite.sh").read_text()
    for player_build in (build, native_build):
        assert "install_player_atomically()" in player_build
        assert 'tmp="${dst}.$$"' in player_build
        assert 'cp "$src" "$tmp"' in player_build
        assert 'chmod +x "$tmp"' in player_build
        assert 'mv -f "$tmp" "$dst"' in player_build
        assert "install_player_atomically rewrite/.build/iplay_modern_host rewrite/.build/iplay_native" in player_build
        assert "install_player_atomically rewrite/.build/iplay_modern_host rewrite/.build/iplay" in player_build
    assert "test -x rewrite/.build/iplay_native" in gate
    assert "test -x rewrite/.build/iplay" in gate


def test_libmodplug_external_decoder_hook_renders_player_sized_blocks() -> None:
    compile_hook_probe()
    result = run([str(HOOK_PROBE), str(aryx_s3m_path())])
    match = re.search(
        r"provider=libmikmod first_status=(\d+) second_status=(\d+) "
        r"first_frames=(\d+) second_frames=(\d+) first_checksum=(\d+) second_checksum=(\d+) "
        r"after_uninstall_render=none after_uninstall_provider=none",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    first_status, second_status, first_frames, second_frames, first_checksum, second_checksum = (
        int(group) for group in match.groups()
    )
    assert first_status == 1
    assert second_status == 1
    assert first_frames == 512
    assert second_frames == 512
    assert first_checksum != 0
    assert second_checksum != 0
    assert first_checksum != second_checksum


def test_libmodplug_external_decoder_hook_reports_source_end_and_clears_install() -> None:
    compile_hook_probe()
    result = run([str(HOOK_PROBE), str(aryx_s3m_path()), "--until-end"])
    match = re.search(
        r"provider=libmodplug ended=(\d+) blocks=(\d+) frames=(\d+) active_bytes=(\d+) checksum=(\d+) partial_blocks=(\d+) last_status=(\d+) "
        r"after_uninstall_render=none after_uninstall_provider=none",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    ended, blocks, frames, active_bytes, checksum, partial_blocks, last_status = (int(group) for group in match.groups())
    assert ended == 1
    assert blocks > 1
    assert frames > 512
    assert active_bytes == frames * 4
    assert checksum != 0
    assert partial_blocks >= 0
    assert last_status == 2


def test_libmodplug_external_decoder_hook_reopens_when_module_path_changes() -> None:
    compile_hook_probe()
    result = run([str(HOOK_PROBE), str(aryx_s3m_path()), "--switch-path", str(hacker4_s3m_path())])
    match = re.search(
        r"provider=libmikmod switched=(\d+) first_status=(\d+) second_status=(\d+) "
        r"first_checksum=(\d+) second_checksum=(\d+) after_uninstall_render=none after_uninstall_provider=none",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    switched, first_status, second_status, first_checksum, second_checksum = (
        int(group) for group in match.groups()
    )
    assert switched == 1
    assert first_status == 1
    assert second_status == 1
    assert first_checksum != 0
    assert second_checksum != 0
    assert first_checksum != second_checksum


def test_libmodplug_external_decoder_hook_replays_same_path_for_new_module_object() -> None:
    compile_hook_probe()
    result = run([str(HOOK_PROBE), str(aryx_s3m_path()), "--replay-same-path"])
    match = re.search(
        r"provider=libmikmod ended=(\d+) end_status=(\d+) replay_status=(\d+) "
        r"replay_blocks=(\d+) replay_frames=(\d+) replay_active_bytes=(\d+) replay_checksum=(\d+) replay_partial_blocks=(\d+) replay_first_checksum=(\d+) "
        r"after_uninstall_render=none after_uninstall_provider=none",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    ended, end_status, replay_status, replay_blocks, replay_frames, replay_active_bytes, replay_checksum, replay_partial_blocks, replay_first_checksum = (
        int(group) for group in match.groups()
    )
    assert ended == 1
    assert end_status == 2
    assert replay_status == 1
    assert replay_blocks > 1
    assert replay_frames > 512
    assert replay_active_bytes == replay_frames * 4
    assert replay_checksum != 0
    assert replay_partial_blocks >= 0
    assert replay_first_checksum != 0


def test_libmodplug_external_decoder_hook_recovers_after_bad_module(tmp_path: Path) -> None:
    compile_hook_probe()
    bad = tmp_path / "BAD.S3M"
    bad.write_bytes(b"bad")
    result = run([str(HOOK_PROBE), str(bad), "--bad-then-good", str(aryx_s3m_path())])
    match = re.search(
        r"provider=libmikmod recovered=(\d+) bad_status=(\d+) good_status=(\d+) good_checksum=(\d+) "
        r"after_uninstall_render=none after_uninstall_provider=none",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    recovered, bad_status, good_status, good_checksum = (int(group) for group in match.groups())
    assert recovered == 1
    assert bad_status == 0
    assert good_status == 1
    assert good_checksum != 0


def test_libmodplug_external_decoder_hook_reports_installed_provider_when_unavailable(tmp_path: Path) -> None:
    compile_hook_probe()
    player = (ROOT / "rewrite" / "iplay_player.c").read_text()
    assert 'if (player_external_decoder_available()) return player_external_decoder_provider_name();' in player
    assert "player_decoder_context_provider_name(context)" in player
    assert "player_decoder_context_hook_provider_name(context)" in player
    bad = tmp_path / "BAD.S3M"
    bad.write_bytes(b"bad")
    result = run([str(HOOK_PROBE), str(bad), "--unavailable"])
    match = re.search(
        r"provider=libmikmod unavailable_status=(\d+) frames=(\d+) active_bytes=(\d+) checksum=(\d+) "
        r"after_uninstall_render=none after_uninstall_provider=none",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    status, frames, active_bytes, checksum = (int(group) for group in match.groups())
    assert status == 0
    assert frames == 512
    assert active_bytes == 2048
    assert checksum == 0


def test_player_hook_playback_block_frame_count_controls_active_bytes() -> None:
    compile_hook_probe()
    result = run([str(HOOK_PROBE), "--frame-contract"])
    header = (ROOT / "rewrite" / "iplay_rewrite.h").read_text()
    player = (ROOT / "rewrite" / "iplay_player.c").read_text()
    assert "dw iplay_player_playback_block_capacity_frames(void);" in header
    assert "dw iplay_player_playback_block_capacity_frames(void)" in player
    assert "return player_playback_block_capacity_frames();" in player
    assert "void iplay_player_playback_block_set_frames(PlayerPlaybackBlock *block, dw frames)" in player
    assert "if (frames > player_playback_block_capacity_frames()) frames = player_playback_block_capacity_frames();" in player
    assert result.stdout.strip() == "frames=123 active_bytes=492 capacity=1024 over_frames=1024 over_active_bytes=4096"


def test_hook_audio_helper_clears_stats_on_invalid_arguments() -> None:
    compile_hook_probe()
    result = run([str(HOOK_PROBE), "--invalid-helper-stats"])
    assert result.stdout.strip() == (
        "ok=0 blocks=0 frames=0 accepted=0 capture_calls=0 capture_bytes=0 capture_checksum=0 "
        "levels=0,0 maxlevels=0,0 source_end=0 block_limit=0 last_status=0"
    )


def test_libmodplug_external_decoder_hook_feeds_sdl_sb16_audio_contract() -> None:
    compile_hook_probe()
    probe = (ROOT / "rewrite" / "modplug_player_hook_probe.cpp").read_text()
    assert "iplay_modplug_external_decoder_play_module_to_sdl_sb16(decoder," in probe
    result = run([str(HOOK_PROBE), str(aryx_s3m_path()), "--hook-audio"])
    match = re.search(
        r"provider=libmikmod audio_ok=(\d+) blocks=(\d+) frames=(\d+) accepted=(\d+) "
        r"capture_calls=(\d+) capture_bytes=(\d+) capture_checksum=(\d+) levels=(\d+),(\d+) "
        r"maxlevels=(\d+),(\d+) "
        r"source_end=(\d+) block_limit=(\d+) last_status=(\d+) "
        r"after_uninstall_render=none after_uninstall_provider=none",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    audio_ok, blocks, frames, accepted, capture_calls, capture_bytes, capture_checksum, left, right, max_left, max_right, source_end, block_limit, last_status = (
        int(group) for group in match.groups()
    )
    assert audio_ok == 1
    assert blocks > 1
    assert frames > 512
    assert accepted == frames * 4
    assert capture_calls == blocks
    assert capture_bytes == accepted
    assert capture_checksum != 0
    assert left > 0 or right > 0
    assert max_left > 0 or max_right > 0
    assert source_end == 0
    assert block_limit == 1
    assert last_status == 1


def test_libmodplug_external_decoder_hook_accepts_single_submitted_block() -> None:
    compile_hook_probe()
    bridge = (ROOT / "rewrite" / "modplug_audio_bridge.cpp").read_text()
    probe = (ROOT / "rewrite" / "modplug_player_hook_probe.cpp").read_text()
    assert "stats->blocks != 0ul" in bridge
    assert "stats->frames != 0ul" in bridge
    assert "stats->blocks > 1ul" not in bridge
    assert '"--hook-audio-one-block"' in probe
    result = run([str(HOOK_PROBE), str(aryx_s3m_path()), "--hook-audio-one-block"])
    match = re.search(
        r"provider=libmikmod audio_ok=(\d+) blocks=(\d+) frames=(\d+) accepted=(\d+) "
        r"capture_calls=(\d+) capture_bytes=(\d+) capture_checksum=(\d+) levels=(\d+),(\d+) "
        r"maxlevels=(\d+),(\d+) source_end=(\d+) block_limit=(\d+) last_status=(\d+) after_uninstall_render=none after_uninstall_provider=none",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    audio_ok, blocks, frames, accepted, capture_calls, capture_bytes, capture_checksum, left, right, max_left, max_right, source_end, block_limit, last_status = (
        int(group) for group in match.groups()
    )
    assert audio_ok == 1
    assert blocks == 1
    assert frames == 512
    assert accepted == 2048
    assert capture_calls == 1
    assert capture_bytes == 2048
    assert capture_checksum != 0
    assert left > 0 or right > 0
    assert max_left > 0 or max_right > 0
    assert source_end == 0
    assert block_limit == 1
    assert last_status == 1


def test_libmodplug_external_decoder_hook_feeds_sdl_sb16_audio_until_source_end() -> None:
    compile_hook_probe()
    result = run([str(HOOK_PROBE), str(aryx_s3m_path()), "--hook-audio-until-end"])
    match = re.search(
        r"provider=libmikmod audio_ok=(\d+) blocks=(\d+) frames=(\d+) accepted=(\d+) "
        r"capture_calls=(\d+) capture_bytes=(\d+) capture_checksum=(\d+) levels=(\d+),(\d+) "
        r"maxlevels=(\d+),(\d+) source_end=(\d+) block_limit=(\d+) last_status=(\d+) after_uninstall_render=none after_uninstall_provider=none",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    audio_ok, blocks, frames, accepted, capture_calls, capture_bytes, capture_checksum, left, right, max_left, max_right, source_end, block_limit, last_status = (
        int(group) for group in match.groups()
    )
    assert audio_ok == 1
    assert blocks > 1
    assert frames > 512
    assert accepted == frames * 4
    assert capture_calls == blocks
    assert capture_bytes == accepted
    assert capture_checksum != 0
    assert left >= 0
    assert right >= 0
    assert max_left > 0 or max_right > 0
    assert source_end == 1
    assert block_limit == 0
    assert last_status == 2


def test_libmodplug_external_decoder_hook_audio_helper_captures_without_external_writer() -> None:
    compile_hook_probe()
    result = run([str(HOOK_PROBE), str(aryx_s3m_path()), "--hook-audio-null-writer"])
    match = re.search(
        r"provider=libmikmod audio_ok=(\d+) blocks=(\d+) frames=(\d+) accepted=(\d+) "
        r"capture_calls=(\d+) capture_bytes=(\d+) capture_checksum=(\d+) levels=(\d+),(\d+) "
        r"maxlevels=(\d+),(\d+) source_end=(\d+) block_limit=(\d+) last_status=(\d+) after_uninstall_render=none after_uninstall_provider=none",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    audio_ok, blocks, frames, accepted, capture_calls, capture_bytes, capture_checksum, left, right, max_left, max_right, source_end, block_limit, last_status = (
        int(group) for group in match.groups()
    )
    assert audio_ok == 1
    assert blocks > 1
    assert frames > 512
    assert accepted == frames * 4
    assert capture_calls == blocks
    assert capture_bytes == accepted
    assert capture_checksum != 0
    assert max_left > 0 or max_right > 0
    assert source_end == 0
    assert block_limit == 1
    assert last_status == 1


def test_modern_player_facade_exposes_runtime_status_summary() -> None:
    modern_h = (ROOT / "rewrite" / "modern_player.hpp").read_text()
    modern = (ROOT / "rewrite" / "modern_player.cpp").read_text()
    assert "bool iplay_modern_playback_summary(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);" in modern_h
    assert "IplayModernDecoderRoute decoder_route;" in modern_h
    assert "const char *decoder_provider;" in modern_h
    assert "bool iplay_modern_playback_status_started(IplayModernPlaybackStatus status);" in modern_h
    assert "const char *iplay_modern_playback_state_text(IplayModernPlaybackStatus status);" in modern_h
    assert "const char *iplay_modern_playback_stop_text(const IplayModernPlaybackResult *result);" in modern_h
    assert "const char *iplay_modern_playback_panel_status_text(const IplayModernPlaybackResult *result);" in modern_h
    assert "const char *iplay_modern_playback_decoder_route_name(const IplayModernPlaybackResult *result);" in modern_h
    assert "const char *iplay_modern_playback_decoder_provider_name(const IplayModernPlaybackResult *result);" in modern_h
    assert "bool iplay_modern_format_blocks(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);" in modern_h
    assert "bool iplay_modern_format_stop(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);" in modern_h
    assert "bool iplay_modern_format_accepted(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);" in modern_h
    assert "bool iplay_modern_format_frames(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);" in modern_h
    assert "bool iplay_modern_format_levels(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);" in modern_h
    assert "bool iplay_modern_format_playback_state(const IplayModernPlaybackResult *result, char *dst, size_t dst_size);" in modern_h
    assert "bool iplay_modern_playback_status_started(IplayModernPlaybackStatus status)" in modern
    assert "const char *iplay_modern_playback_state_text(IplayModernPlaybackStatus status)" in modern
    assert "const char *iplay_modern_playback_stop_text(const IplayModernPlaybackResult *result)" in modern
    assert "const char *iplay_modern_playback_panel_status_text(const IplayModernPlaybackResult *result)" in modern
    assert "const char *iplay_modern_playback_decoder_route_name(const IplayModernPlaybackResult *result)" in modern
    assert "const char *iplay_modern_playback_decoder_provider_name(const IplayModernPlaybackResult *result)" in modern
    assert '? ((modern_extension_equals(modern_path_extension(path), ".mod") ||' in modern
    assert 'modern_extension_equals(modern_path_extension(path), ".nst")) ? "libmodplug" : "libmikmod")' in modern
    assert "bool iplay_modern_format_blocks(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert "bool iplay_modern_format_stop(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert "bool iplay_modern_format_accepted(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert "bool iplay_modern_format_frames(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert "bool iplay_modern_format_levels(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert "bool iplay_modern_format_playback_state(const IplayModernPlaybackResult *result, char *dst, size_t dst_size)" in modern
    assert "const char *iplay_modern_audio_backend_name(void);" in modern_h
    assert "const char *iplay_modern_audio_backend_name(void)" in modern
    assert "MODERN_STATUS_AUDIO_BACKEND" in modern
    assert "Audio backend: %s" in modern
    assert "iplay_modern_audio_backend_name()" in modern
    assert "iplay_modern_playback_status_started(result->status)" in modern
    assert "iplay_modern_playback_state_text(result->status)" in modern
    assert "iplay_modern_playback_decoder_route_name(result)" in modern
    assert "iplay_modern_playback_decoder_provider_name(result)" in modern
    assert "iplay_modern_playback_stop_text(result)" in modern
    assert "static const char *modern_status_module_type_text(const char *path)" in modern
    assert "module_type_text = modern_status_module_type_text(module_path);" in modern
    assert "iplay_modern_format_blocks(result, blocks, sizeof(blocks))" in modern
    assert "iplay_modern_format_stop(result, stop, sizeof(stop))" in modern
    assert "iplay_modern_format_accepted(result, accepted, sizeof(accepted))" in modern
    assert "iplay_modern_format_frames(result, frames, sizeof(frames))" in modern
    assert "iplay_modern_format_levels(result, levels, sizeof(levels))" in modern
    assert "iplay_modern_format_playback_state(result, playback_state, sizeof(playback_state))" in modern
    assert "Playback enabled" in modern
    assert "Playback disabled" in modern


def test_modern_player_facade_renders_status_to_runtime_text_cells() -> None:
    modern_h = (ROOT / "rewrite" / "modern_player.hpp").read_text()
    modern = (ROOT / "rewrite" / "modern_player.cpp").read_text()
    probe = (ROOT / "rewrite" / "modplug_audio_probe.cpp").read_text()
    assert "bool iplay_modern_render_playback_status(IplayRuntime *runtime, const char *module_path, const IplayModernPlaybackResult *result);" in modern_h
    assert "const char *iplay_modern_status_title(void);" in modern_h
    assert "const char *iplay_modern_status_title(void)" in modern
    assert "iplay_runtime_draw_status_line(runtime, IPLAY_RUNTIME_STATUS_TITLE_ROW, iplay_modern_status_title()" in modern
    assert "Inertia Player V1.22" in modern
    assert "iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_MODULE_ROW" in modern
    assert "iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_SIZE_ROW" in modern
    assert modern.count("    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_SIZE_ROW") == 1
    assert "iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_LOADER_ROW" in modern
    assert modern.count("    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_LOADER_ROW") == 1
    assert "iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_AUDIO_ROW" in modern
    assert modern.count("    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_AUDIO_ROW") == 1
    assert "iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_HARDWARE_ROW" in modern
    assert modern.count("    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_HARDWARE_ROW") == 1
    assert "iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_VIDEO_ROW" in modern
    assert modern.count("    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_VIDEO_ROW") == 1
    assert "iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_LEVELS_ROW" in modern
    assert modern.count("    iplay_runtime_draw_status_field(runtime, IPLAY_RUNTIME_STATUS_LEVELS_ROW") == 1
    assert "iplay_runtime_draw_status_line(runtime, IPLAY_RUNTIME_STATUS_PLAYBACK_ROW, MODERN_STATUS_INTERPOLATION" in modern
    assert "status_text = iplay_modern_playback_status_name(result->status);" in modern
    assert "iplay_modern_render_playback_status(&runtime, path, result)" in probe
    assert "iplay_runtime_init_callbacks(&runtime, cells, mode, capture_video_present, &video, capture_audio_write, 0)" in probe
    assert "iplay_runtime_init_callbacks_capacity(&runtime, cells, IPLAY_TEXT_MAX_SCREEN_BYTES, &IPLAY_TEXT_MODE_80X25" in probe
    assert "(void)iplay_runtime_present(&runtime);" in probe
    assert "static const IplayTextMode *native_text_mode_from_arg(const char *arg)" in probe
    assert 'render_and_print_rows("playback", selected_mode, module_path, &result, "playback-position", "full-screen")' in probe
    assert 'render_and_print_rows("selected", selected_mode, module_path, &result, "post-playback-status", "status-only")' in probe
    assert 'render_and_print_rows("screen", &IPLAY_TEXT_MODE_80X25, module_path, &result, "post-playback-status", "status-only")' in probe
    assert 'render_and_print_rows("screen40", &IPLAY_TEXT_MODE_40X25, module_path, &result, "post-playback-status", "status-only")' in probe
    assert 'render_and_print_rows("screen80x50", &IPLAY_TEXT_MODE_80X50, module_path, &result, "post-playback-status", "status-only")' in probe
    assert "render_resize_cycle_and_print_rows(module_path, &result)" in probe
    assert "iplay_runtime_resize_to_size_checked(&runtime, 80, 50)" in probe
    assert "render_subwindow_and_print_rows(module_path, &result)" in probe
    assert "iplay_window_init_subwindow(&child, &root, 3, 5, 5, 34)" in probe
    assert "print_level_sequence_evidence(module_path)" in probe
    assert "capture_level_sequence" in probe


def test_modern_status_probe_renders_all_supported_text_modes() -> None:
    compile_audio_probe()
    result = run([str(AUDIO_PROBE), str(aryx_s3m_path()), "16384", "40x25bw"])
    assert "Selected text mode: 40x25bw cols=40 rows=25" in result.stdout
    assert "selected_present=calls:1 bytes:2000 cols:40 rows:25" in result.stdout
    for prefix in ["screen", "screen40", "screen80x50"]:
        assert f'{prefix}_title="' in result.stdout
        assert f'{prefix}_module="' in result.stdout
        assert f'{prefix}_blocks="' in result.stdout
        assert f'{prefix}_stop="' in result.stdout
        assert f'{prefix}_audio="' in result.stdout
        assert f'{prefix}_accepted="' in result.stdout
        assert f'{prefix}_frames="' in result.stdout
        assert f'{prefix}_levels="' in result.stdout
        assert f'{prefix}_playback="' in result.stdout
        assert f'{prefix}_status="' in result.stdout
    assert "Playback enabled" in result.stdout
    assert "active=1" in result.stdout
    assert "source-end" in result.stdout
    assert "screen_present=calls:1 bytes:4000 cols:80 rows:25" in result.stdout
    assert "screen40_present=calls:1 bytes:2000 cols:40 rows:25" in result.stdout
    assert "screen80x50_present=calls:1 bytes:8000 cols:80 rows:50" in result.stdout
    assert "Resize present: phase=before bytes=4000 screen_bytes=4000" in result.stdout
    assert "cols=80 rows=25 resize_ok=1" in result.stdout
    assert "Resize present: phase=after bytes=8000 screen_bytes=8000" in result.stdout
    assert "cols=80 rows=50 resize_ok=1" in result.stdout
    assert "resize_before_present=calls:1 bytes:4000 cols:80 rows:25 resize_ok:1" in result.stdout
    assert "resize_after_present=calls:2 bytes:12000 cols:80 rows:50 resize_ok:1" in result.stdout
    assert "Subwindow present: origin=3,5 rows=5 cols=34 screen_bytes=4000" in result.stdout
    assert "calls=1 bytes=4000 present_cols=80 present_rows=25" in result.stdout
    assert 'subwindow_title="' in result.stdout
    assert "SUBWINDOW" in result.stdout
    assert 'subwindow_audio="' in result.stdout
    assert "SDL-compatible SB16 16-bit stereo" in result.stdout
    assert "Color probe: target_fg=16 target_bg=8 cells=24 fg_matches=16 bg_matches=8 blink_matches=8 fg_mask=ffff bg_mask=ff blink_mask=aa present_calls=1 bytes=4000 cols=80 rows=25" in result.stdout
    assert "Level sequence: target=16 samples=16" in result.stdout
    assert "changed=1" in result.stdout
    assert "status=keyboard stop=keyboard" in result.stdout


def test_native_binary_help_describes_sdl_sb16_notcurses_contract() -> None:
    compile_audio_probe()
    result = run([str(AUDIO_PROBE), "--help"])
    assert result.returncode == 0
    assert "usage: iplay_native [--modern] [--video-mode=MODE] <module-file|@file-list> [--modern|max-blocks|--blocks=N|--source-end|--keyboard-after-one]" in result.stdout
    assert "@file-list selects the first non-empty trimmed module path relative to the list file" in result.stdout
    assert "SDL-compatible SB16 16-bit stereo bridge" in result.stdout
    assert "notcurses-style text runtime" in result.stdout
    assert "40x25, 80x25, and 80x50 text geometry" in result.stdout
    assert "--modern enables the preferred direct SDL/notcurses player mode" in result.stdout
    assert "--keyboard-after-one stops through the keyboard/interactive seam" in result.stdout
    assert "--blocks=N bounds native playback to N external-decoder blocks" in result.stdout
    assert "--sdl-audio opens a real SDL2 queued-audio device for audible native playback" in result.stdout
    assert "--terminal-render paints the final notcurses-style text cells to the host terminal with ANSI 16-color output" in result.stdout
    assert "--terminal-live updates ANSI audio level meters from the native playback callback while blocks are submitted" in result.stdout
    assert "--stdin-keyboard stops native playback when q, Q, or Escape is read from stdin" in result.stdout
    assert "--source-end plays until libmikmod reports natural source end" in result.stdout
    assert "--video-mode=MODE selects the same text mode as a positional mode argument" in result.stdout
    assert "module filenames are resolved with DOS-style case-insensitive matching in their host directory" in result.stdout


def test_user_facing_iplay_alias_help_uses_alias_name() -> None:
    compile_audio_probe()
    result = run([str(PLAYER_ALIAS), "--help"])
    assert result.returncode == 0
    assert "usage: iplay [--modern] [--video-mode=MODE] <module-file|@file-list> [--modern|max-blocks|--blocks=N|--source-end|--keyboard-after-one]" in result.stdout
    assert "usage: iplay_native" not in result.stdout
    assert "modern SDL/notcurses player mode is the default for iplay" in result.stdout
    assert "--video-mode=MODE selects the same text mode as a positional mode argument" in result.stdout
    assert "--modern enables the preferred direct SDL/notcurses player mode" not in result.stdout


def test_native_binary_source_end_argument_reports_source_end() -> None:
    compile_audio_probe()
    result = run([str(AUDIO_PROBE), str(aryx_s3m_path()), "--source-end", "80x25color"])
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in result.stdout
    assert "Selected text mode: 80x25color cols=80 rows=25" in result.stdout
    assert "playback_present=calls:1 bytes:4000 cols:80 rows:25" in result.stdout


def test_native_binary_rejects_unsupported_selected_text_mode() -> None:
    compile_audio_probe()
    result = run([str(AUDIO_PROBE), str(aryx_s3m_path()), "1", "badmode"], check=False)
    assert result.returncode == 2
    assert "iplay_native: unsupported text mode: badmode" in result.stderr
    assert "usage: iplay_native [--modern] [--video-mode=MODE] <module-file|@file-list> [--modern|max-blocks|--blocks=N|--source-end|--keyboard-after-one]" in result.stdout


def test_user_facing_iplay_alias_errors_use_alias_name() -> None:
    compile_audio_probe()
    result = run([str(PLAYER_ALIAS), str(aryx_s3m_path()), "badmode"], check=False)
    assert result.returncode == 2
    assert "iplay: unsupported text mode: badmode" in result.stderr
    assert "iplay_native: unsupported text mode" not in result.stderr
    assert "usage: iplay [--modern] [--video-mode=MODE] <module-file|@file-list> [--modern|max-blocks|--blocks=N|--source-end|--keyboard-after-one]" in result.stdout


def test_native_binary_accepts_text_mode_case_like_dos() -> None:
    compile_audio_probe()
    result = run([str(AUDIO_PROBE), str(aryx_s3m_path()), "1", "80X50PROJECT"], check=False)
    assert result.returncode == 3
    assert "Selected text mode: 80X50PROJECT cols=80 rows=50" in result.stdout
    assert "selected_present=calls:1 bytes:8000 cols:80 rows:50" in result.stdout
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_native_binary_accepts_video_mode_option_before_or_after_module() -> None:
    compile_audio_probe()
    for args in (
        [str(AUDIO_PROBE), "--video-mode=80x50", str(aryx_s3m_path()), "--blocks=1"],
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=1", "--video-mode=80x50"],
    ):
        result = run(args, check=False)
        assert result.returncode == 3, result.stdout + result.stderr
        assert "Selected text mode: 80x50 cols=80 rows=50" in result.stdout
        assert "selected_present=calls:1 bytes:8000 cols:80 rows:50" in result.stdout
        assert "status=block-limit route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_user_facing_iplay_alias_accepts_video_mode_option_before_module() -> None:
    compile_audio_probe()
    result = run(
        [str(PLAYER_ALIAS), "--video-mode=80x50", str(aryx_s3m_path())],
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Selected text mode: 80x50 cols=80 rows=50" in result.stdout
    assert "Terminal render: requested=1 cols=80 rows=50 bytes=8000" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in result.stdout


def test_native_binary_terminal_text_mode_uses_columns_lines() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "1", "terminal"],
        check=False,
        env={**os.environ, "COLUMNS": "80", "LINES": "50"},
    )
    assert result.returncode == 3
    assert "Selected text mode: terminal cols=80 rows=50" in result.stdout
    assert "selected_present=calls:1 bytes:8000 cols:80 rows:50" in result.stdout
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_native_binary_loads_first_trimmed_filelist_entry_relative_to_list(tmp_path: Path) -> None:
    compile_audio_probe()
    module = tmp_path / "aryx.s3m"
    filelist = tmp_path / "PLAYLIST.LST"
    module.write_bytes(aryx_s3m_path().read_bytes())
    filelist.write_text("\n\t aryx.s3m \r\nignored.s3m\n")
    result = run([str(AUDIO_PROBE), "@" + str(filelist), "1", "80x25color"], check=False)
    assert result.returncode == 3
    assert f"File list: @{filelist} selected={module}" in result.stdout
    assert "Module: aryx.s3m" in result.stdout
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod" in result.stdout
    assert "Level sequence: target=16 samples=16" in result.stdout


def test_native_binary_resolves_module_path_case_like_dos(tmp_path: Path) -> None:
    compile_audio_probe()
    module = tmp_path / "aryx.s3m"
    requested = tmp_path / "ARYX.S3M"
    module.write_bytes(aryx_s3m_path().read_bytes())
    result = run([str(AUDIO_PROBE), str(requested), "1", "80x25color"], check=False)
    assert result.returncode == 3
    assert "Module: aryx.s3m" in result.stdout
    assert "Size: 20800 bytes" in result.stdout
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_native_binary_resolves_filelist_entry_case_like_dos(tmp_path: Path) -> None:
    compile_audio_probe()
    module = tmp_path / "aryx.s3m"
    filelist = tmp_path / "PLAYLIST.LST"
    module.write_bytes(aryx_s3m_path().read_bytes())
    filelist.write_text("ARYX.S3M\n")
    result = run([str(AUDIO_PROBE), "@" + str(filelist), "1", "80x25color"], check=False)
    assert result.returncode == 3
    assert f"File list: @{filelist} selected={module}" in result.stdout
    assert "Module: aryx.s3m" in result.stdout
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_native_binary_resolves_filelist_path_case_like_dos(tmp_path: Path) -> None:
    compile_audio_probe()
    module = tmp_path / "aryx.s3m"
    filelist = tmp_path / "playlist.lst"
    requested = tmp_path / "PLAYLIST.LST"
    module.write_bytes(aryx_s3m_path().read_bytes())
    filelist.write_text("aryx.s3m\n")
    result = run([str(AUDIO_PROBE), "@" + str(requested), "1", "80x25color"], check=False)
    assert result.returncode == 3
    assert f"File list: @{requested} selected={module}" in result.stdout
    assert "Module: aryx.s3m" in result.stdout
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_native_binary_missing_module_reports_original_style_not_found(tmp_path: Path) -> None:
    compile_audio_probe()
    missing = tmp_path / "MISSING.S3M"
    result = run([str(AUDIO_PROBE), str(missing), "1", "80x25color"], check=False)
    assert result.returncode == 2
    assert "Module not found." in result.stderr
    assert "Module:" not in result.stdout
    assert "Decoder route:" not in result.stdout
    assert "Playback pump:" not in result.stdout


def test_native_binary_missing_filelist_entry_reports_original_style_not_found(tmp_path: Path) -> None:
    compile_audio_probe()
    filelist = tmp_path / "PLAYLIST.LST"
    missing = tmp_path / "MISSING.S3M"
    filelist.write_text("MISSING.S3M\n")
    result = run([str(AUDIO_PROBE), "@" + str(filelist), "1", "80x25color"], check=False)
    assert result.returncode == 2
    assert f"File list: @{filelist} selected={missing}" in result.stdout
    assert "Module not found." in result.stderr
    assert "Decoder route:" not in result.stdout
    assert "Playback pump:" not in result.stdout


def test_user_facing_iplay_alias_filelist_echo_uses_module_argument(tmp_path: Path) -> None:
    compile_audio_probe()
    module = tmp_path / "aryx.s3m"
    filelist = tmp_path / "PLAYLIST.LST"
    module.write_bytes(aryx_s3m_path().read_bytes())
    filelist.write_text("aryx.s3m\n")
    result = run(
        [str(PLAYER_ALIAS), "--modern", "@" + str(filelist), "80x50"],
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert f"File list: @{filelist} selected={module}" in result.stdout
    assert "File list: --modern" not in result.stdout
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in result.stdout


def test_modern_playback_result_reports_source_end_stop_reason() -> None:
    compile_audio_probe()
    result = run([str(AUDIO_PROBE), str(aryx_s3m_path())])
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in result.stdout
    assert "summary=\"Audio backend: SDL-compatible SB16 16-bit stereo; Playback enabled; route=external-library; provider=libmikmod; status=ok; stop=source-end; source_end=1;" in result.stdout
    assert "stop=source-end" in result.stdout


def test_modern_playback_result_reports_block_limit_stop_reason() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "1"],
        check=False,
    )
    assert result.returncode == 3
    assert "Module: aryx.s3m" in result.stdout
    assert "Decoder route: id=0 name=external-library" in result.stdout
    assert "Loader: s3m_module (Scream Tracker 3)" in result.stdout
    assert "Module type tag: 204D3353" in result.stdout
    assert "Title: aryx" in result.stdout
    assert "PCM source: external_module seed=1 truncated=0 input=file-path renderer=e route=0 provider=libmikmod hook_provider=libmikmod stream_start=0" in result.stdout
    assert "Playback pump: blocks=1 frames=512 accepted=2048" in result.stdout
    assert "limit=1 source_end=0 stop=block-limit" in result.stdout
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod stop=block-limit source_end=0 blocks=1 source_frames=512" in result.stdout
    assert "stop=block-limit" in result.stdout
    assert "Playback enabled" in result.stdout
    assert "Screen present: reason=post-playback-status scope=status-only bytes=4000 screen_bytes=4000" in result.stdout
    assert "screen_present=calls:1 bytes:4000 cols:80 rows:25" in result.stdout
    assert "screen40_present=calls:1 bytes:2000 cols:40 rows:25" in result.stdout
    assert "screen80x50_present=calls:1 bytes:8000 cols:80 rows:50" in result.stdout
    assert "resize_after_present=calls:2 bytes:12000 cols:80 rows:50 resize_ok:1" in result.stdout
    assert "Subwindow present: origin=3,5 rows=5 cols=34 screen_bytes=4000" in result.stdout
    assert "Color probe: target_fg=16 target_bg=8 cells=24 fg_matches=16 bg_matches=8 blink_matches=8 fg_mask=ffff bg_mask=ff blink_mask=aa present_calls=1 bytes=4000 cols=80 rows=25" in result.stdout
    assert "Level sequence: target=16 samples=16" in result.stdout


def test_native_binary_blocks_option_reports_block_limit() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=1", "80x25color"],
        check=False,
    )
    assert result.returncode == 3
    assert "Playback pump: blocks=1 frames=512 accepted=2048" in result.stdout
    assert "limit=1 source_end=0 stop=block-limit" in result.stdout
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod" in result.stdout
    assert "Selected text mode: 80x25color cols=80 rows=25" in result.stdout


def test_native_binary_sdl_audio_option_queues_exact_sb16_sink() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=1", "80x25color", "--sdl-audio"],
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
    )
    assert result.returncode == 3
    assert "Playback pump: blocks=1 frames=512 accepted=2048" in result.stdout
    assert "SDL audio sink: requested=1 opened=1 bytes=2048 queue_failures=0 freq=44100 format=0x8010 channels=2 samples=1024" in result.stdout
    assert "queue_limit_bytes=16384 queue_waits=0 driver=dummy paused=1 queue_cleared=1 closed=1" in result.stdout
    assert "status=block-limit route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_native_binary_modern_alias_enables_direct_sdl_notcurses_player_mode() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--modern"],
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "50"},
        timeout=30,
    )
    assert result.returncode == 0
    assert "SDL audio sink: requested=1 opened=1" in result.stdout
    assert "driver=dummy paused=1 queue_cleared=1 closed=1" in result.stdout
    assert "Terminal live summary: requested=1" in result.stdout
    assert "Selected text mode: auto cols=80 rows=50" in result.stdout
    assert "Terminal render: requested=1 cols=80 rows=50 bytes=8000" in result.stdout
    assert "Stdin keyboard: requested=1 stopped=0" in result.stdout
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in result.stdout


def test_native_binary_modern_alias_accepts_text_mode_before_or_after_flag() -> None:
    compile_audio_probe()
    for args in (
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--modern", "80x50"],
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "80x50", "--modern"],
    ):
        result = run(
            args,
            check=False,
            env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
            timeout=30,
        )
        assert result.returncode == 0, result.stdout + result.stderr
        assert "Selected text mode: 80x50 cols=80 rows=50" in result.stdout
        assert "Terminal render: requested=1 cols=80 rows=50 bytes=8000" in result.stdout
        assert "SDL audio sink: requested=1 opened=1" in result.stdout
        assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in result.stdout


def test_native_binary_modern_alias_accepts_flag_before_module() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), "--modern", str(aryx_s3m_path()), "80x50"],
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Module: aryx.s3m" in result.stdout
    assert "Selected text mode: 80x50 cols=80 rows=50" in result.stdout
    assert "Terminal render: requested=1 cols=80 rows=50 bytes=8000" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in result.stdout


def test_user_facing_iplay_alias_runs_modern_sdl_notcurses_player() -> None:
    compile_audio_probe()
    result = run(
        [str(PLAYER_ALIAS), "--modern", str(aryx_s3m_path()), "80x50"],
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Module: aryx.s3m" in result.stdout
    assert "Selected text mode: 80x50 cols=80 rows=50" in result.stdout
    assert "Terminal render: requested=1 cols=80 rows=50 bytes=8000" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in result.stdout


def test_user_facing_iplay_alias_defaults_to_modern_sdl_notcurses_player() -> None:
    compile_audio_probe()
    result = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "80x50"],
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert "Module: aryx.s3m" in result.stdout
    assert "Selected text mode: 80x50 cols=80 rows=50" in result.stdout
    assert "Terminal render: requested=1 cols=80 rows=50 bytes=8000" in result.stdout
    assert "Terminal live summary: requested=1" in result.stdout
    assert "Stdin keyboard: requested=1 stopped=0" in result.stdout
    assert "SDL audio sink: requested=1 opened=1" in result.stdout
    assert "status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1" in result.stdout


def test_native_binary_sdl_audio_open_failure_reports_requested_format() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=1", "80x25color", "--sdl-audio"],
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "not-a-real-driver"},
    )
    assert result.returncode == 2
    assert "iplay_native: could not open SDL2 SB16 stereo audio sink requested freq=44100 format=0x8010 channels=2 samples=1024:" in result.stderr
    assert "Playback pump:" not in result.stdout


def test_native_binary_terminal_render_option_paints_selected_text_mode() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=1", "40x25color", "--terminal-render"],
        check=False,
    )
    assert result.returncode == 3
    assert "Terminal render: requested=1 cols=40 rows=25 bytes=2000" in result.stdout
    assert "screen_nonblank=" in result.stdout
    probe_source = (ROOT / "rewrite" / "modplug_audio_probe.cpp").read_text()
    presenter_source = (ROOT / "rewrite" / "notcurses_presenter.cpp").read_text()
    assert "iplay_notcurses_present_cells(cells, mode)" in probe_source
    assert '"\\033[?25l\\033[?7l\\033[2J"' not in probe_source
    assert "notcurses_init(&options, stdout)" in presenter_source
    assert "notcurses_render(presenter)" in presenter_source
    assert "Inertia Player V1.22" in result.stdout
    assert "Terminal render end" in result.stdout
    assert "Selected text mode: 40x25color cols=40 rows=25" in result.stdout


def test_native_binary_terminal_live_option_updates_audio_levels_per_block() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=2", "80x25color", "--terminal-live"],
        check=False,
    )
    assert result.returncode == 3
    assert "\x1b[?25l\x1b[HTerminal live: block=1 frames=512 accepted=2048 levels=" in result.stdout
    assert "\x1b[HTerminal live: block=2 frames=1024 accepted=4096 levels=" in result.stdout
    assert "\x1b[0m\x1b[?25h\nTerminal live summary: requested=1" in result.stdout
    assert "L[" in result.stdout
    assert "] R[" in result.stdout
    assert "Terminal live summary: requested=1 samples=2 nonzero=2" in result.stdout
    assert "printed=2 suppressed=0" in result.stdout
    assert "Playback pump: blocks=2 frames=1024 accepted=4096" in result.stdout


def test_preferred_player_redraws_full_original_layout_during_live_playback() -> None:
    compile_audio_probe()
    result = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=8", "80x25color"],
        check=False,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        timeout=30,
    )
    assert result.returncode == 3
    probe_source = (ROOT / "rewrite" / "modplug_audio_probe.cpp").read_text()
    presenter_source = (ROOT / "rewrite" / "notcurses_presenter.cpp").read_text()
    assert "iplay_notcurses_present_cells(cells, mode)" in probe_source
    assert "ncplane_putegc_yx" in presenter_source
    assert result.stdout.count("Inertia Player V1.22") >= 1
    modern_source = (ROOT / "rewrite" / "modern_player.cpp").read_text()
    assert '"Sound Blaster 16 (44kHz)"' in modern_source
    assert 'std::snprintf(track, sizeof(track), "Current Track : %u/%u"' in modern_source
    assert 'std::snprintf(position, sizeof(position), "Track Position: %u/%u"' in modern_source
    assert "Terminal live: block=" not in result.stdout
    assert "Terminal live summary: requested=1 samples=8" in result.stdout
    assert "printed=5 suppressed=3" in result.stdout
    assert "Playback pump: blocks=8 frames=4096 accepted=16384" in result.stdout


def test_native_binary_stdin_keyboard_option_stops_on_q() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=32", "80x25color", "--stdin-keyboard"],
        check=False,
        input_text="q\n",
    )
    assert result.returncode == 3
    assert "Stdin keyboard: requested=1 stopped=1" in result.stdout
    assert "Stdin keyboard mode: requested=1 raw=0 restored=0" in result.stdout
    assert "stop=keyboard" in result.stdout
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod" in result.stdout


def test_native_binary_function_key_escape_sequence_does_not_stop_as_escape() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=32", "80x25color", "--stdin-keyboard"],
        check=False,
        input_text="\x1b[20~q",
    )
    assert result.returncode == 3
    match = re.search(r"Playback pump: blocks=(\d+)", result.stdout)
    assert match
    assert int(match.group(1)) >= 2
    assert "Stdin keyboard: requested=1 stopped=1" in result.stdout
    assert "stop=keyboard" in result.stdout


def test_preferred_player_f1_help_f3_vu_and_f4_sample_views_are_live() -> None:
    compile_audio_probe()
    result = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
        check=False,
        input_text="\x1bOP\x1b[13~\x1b[14~q",
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
    )
    assert result.returncode == 3
    assert "view_mask=13" in result.stdout
    player = (ROOT / "rewrite" / "modern_player.cpp").read_text()
    assert "unsigned display_rows = iplay_notcurses_presenter_rows();" in player
    assert "display_rows - 19u" in player
    assert "iplay_ncplane_fill_yx(plane, 6u, 2u" in player
    assert "iplay_ncplane_fill_yx(plane, 6u, 2u, 10u" in player
    assert "(dw)(rows - 28u)" in player


def test_preferred_player_repeated_f4_advances_original_sample_name_page() -> None:
    compile_audio_probe()
    result = run(
        [str(PLAYER_ALIAS), str(hacker4_s3m_path()), "--blocks=32", "80x25color"],
        check=False,
        input_text="\x1b[14~x\x1b[14~q",
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
    )
    assert result.returncode == 3
    assert "view_mask=8" in result.stdout
    probe = (ROOT / "rewrite" / "modplug_audio_probe.cpp").read_text()
    assert "display_rows = iplay_notcurses_presenter_rows();" in probe
    assert "display_rows > 19u ? display_rows - 19u : 9u" in probe
    listing = (ROOT / "IPLAY.lst").read_text(errors="ignore")
    keyboard = listing[listing.index("seg001:11B0 loc_1A200"):listing.index("seg001:11D5")]
    assert "dec     ax" in keyboard
    assert "add     current_patterns, ax" in keyboard


def test_preferred_player_f2_scope_and_f5_fft_views_use_live_pcm() -> None:
    compile_audio_probe()
    result = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
        check=False,
        input_text="\x1bOQ\x1b[15~q",
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "SDL_VIDEODRIVER": "dummy"},
    )
    assert result.returncode == 3
    assert "view_mask=18" in result.stdout
    visualizer = (ROOT / "rewrite" / "sdl_visualizer.cpp").read_text()
    probe = (ROOT / "rewrite" / "modplug_audio_probe.cpp").read_text()
    assert '"Inertia Player - F2 Graphical Scopes"' in visualizer
    assert "load_f2_background" in visualizer
    assert "if (visualizer.f2_lane_count != requested_lane_count)" in visualizer
    assert "stats->ui.channel_count," in visualizer
    renderer = (ROOT / "rewrite" / "modplug_renderer.cpp").read_text()
    assert "channel_count = ModPlug_NumChannels(renderer->file);" in renderer
    assert "ui->channel_count = s3m_channel_count(renderer->module, channel_count);" in renderer
    assert "channel < visualizer.f2_lane_count" in visualizer
    assert "visualizer.f2_lane_x[channel]" in visualizer
    assert "visualizer.f2_lane_center[channel]" in visualizer
    assert "SDL_GetDesktopDisplayMode(0, &desktop)" in visualizer
    assert "visualizer.f2_background = scale_pixels(" in visualizer
    assert "left_count = (visualizer.f2_lane_count + 1u) / 2u;" in visualizer
    assert "const bool left = (channel & 1u) == 0u;" in visualizer
    assert "fit_aspect(" in visualizer
    assert "visualizer.f2_view_x + original_x * visualizer.f2_view_width / 640" in visualizer
    assert "296 * visualizer.f2_view_width / 640" in visualizer
    assert "unsigned fraction = position % (unsigned)trace_width;" in visualizer
    assert "sample0 * (int)((unsigned)trace_width - fraction)" in visualizer
    assert 'ensure_visualizer(320, 200, "Inertia Player - F5 Frequency Analysis")' in visualizer
    assert "for (int x = 4; x < 12; ++x)" in visualizer
    assert "(y & 1u) ? rgb(0u, 0u, 0u) : rgb(0u, 96u, 224u)" in visualizer
    assert "int x0 = 22 + (int)bin * 3;" in visualizer
    assert "iplay_sdl_visualizer_present_f2(stats)" in probe
    assert "iplay_sdl_visualizer_present_f5(stats)" in probe
    assert "iplay_notcurses_poll_key()" in probe
    assert "native_select_module" in probe
    assert "Inertia Player - Select Module" in probe
    presenter = (ROOT / "rewrite" / "notcurses_presenter.cpp").read_text()
    assert "const int lower_top = (int)terminal_rows - 11;" in presenter
    assert "const int center = (int)terminal_cols / 2;" in presenter
    assert "center_source_title_row" in presenter
    assert "middle_bottom, center - 2, 'P'" not in presenter
    assert "46u * terminal_cols / 80u" not in presenter
    assert 'paused ? "Pausing"' in probe
    assert 'stats->loop_enabled ? "Looping" : "Playing"' in probe
    assert "put_dos_cell(plane, lower_top, 0, 0xb3u, 0x7fu);" in presenter
    assert "draw_hline(plane, (int)terminal_rows - 1, 1, (int)terminal_cols - 2, 0x78u);" in presenter
    assert "target_y = middle_bottom" in presenter
    assert "target_y = lower_top + (int)row - 17" in presenter
    assert "col >= 4u && col <= 36u" in presenter
    assert "col >= 43u && col <= 75u" in presenter
    assert "if (col < 2u || col > 77u) continue;" in presenter
    assert "put_dos_cell(plane, y, 1, 0xb3u, 0x78u);" in presenter
    assert "put_dos_cell(plane, y, (int)terminal_cols - 2, 0xb3u, 0x7fu);" in presenter
    assert "draw_hline(plane, 0, 1, (int)terminal_cols - 2, 0x7fu);" in presenter
    assert "draw_hline(plane, 1, 4, (int)terminal_cols - 5, 0x7fu);" in presenter
    assert "draw_hline(plane, 4, 4, (int)terminal_cols - 5, 0x78u);" in presenter
    assert "is_channel_meter_row" in presenter
    assert "int effect_x = (int)terminal_cols - 17;" in presenter
    assert "target_y = 16 + (int)row - 28;" in presenter
    assert "overflow_source_first_row" in presenter
    assert "channel_label_column" in presenter
    player = (ROOT / "rewrite" / "modern_player.cpp").read_text()
    rewrite = (ROOT / "rewrite" / "iplay_rewrite.c").read_text()
    assert "modern_format_effect(state, effect);" in player
    assert '"Fine Vol Slide"' in player
    assert "low == 0x0fu && high" in player
    assert "high == 0x0fu && low" in player
    assert "28u + channel - 10u" in rewrite
    assert "modern_declared_channel_count(module_path, ui->channel_count)" in player
    assert "std::fseek(file, 0x40, SEEK_SET)" in player
    assert "channels[channel] < 16u" in player
    assert "display_rows - 19u" in player
    assert "page_row < 9u" in player
    assert "modern_format_sample_details" in player
    assert "is_sample_view" in presenter
    assert "int details_x = (int)terminal_cols - 44;" in presenter
    renderer = (ROOT / "rewrite" / "modplug_renderer.cpp").read_text()
    assert "ui->channels[channel].row_effect = note.Effect;" in renderer
    assert "ui->channels[channel].row_parameter = note.Parameter;" in renderer
    assert "capture_s3m_sample_info(renderer, ui);" in renderer
    assert "s3m_channel_pan(renderer->module, channel, fallback)" in renderer
    assert "s3m_channel_count(renderer->module, channel_count)" in renderer
    assert "renderer->active_channels[channel].pan = iplay_modplug_pan_from_stereo_vu" not in renderer
    assert "header + 0x10u" in renderer
    assert "header + 0x20u" in renderer
    listing = (ROOT / "IPLAY.lst").read_text(errors="ignore")
    original_scope = listing[listing.index("seg001:1E6A f2_draw_waves"):listing.index("seg001:1F12 f2_draw_waves", listing.index("seg001:1E6A f2_draw_waves"))]
    assert "mov     cx, amount_of_x" in original_scope
    assert "mov     ah, 37" in original_scope
    assert "add     si, 8" in original_scope
    original_spectrum = listing[listing.index("seg001:1F7F init_f5_spectr"):listing.index("seg001:2D18 f6_draw:")]
    assert "mov     ax, 13h" in original_spectrum
    assert "mov     cx, 200h" in original_spectrum
    assert "mov     cx, 99" in original_spectrum
    assert original_spectrum.count("cmp     al, 90") >= 2
    bridge_header = (ROOT / "rewrite" / "modplug_audio_bridge.hpp").read_text()
    assert "#define IPLAY_MODPLUG_FFT_SAMPLES 512u" in bridge_header
    assert "#define IPLAY_MODPLUG_SPECTRUM_BANDS 100u" in bridge_header
    assert "#define IPLAY_MODPLUG_SPECTRUM_MAX_LEVEL 90u" in bridge_header
    assert "Playback pump: blocks=" in result.stdout


def test_preferred_player_combined_original_control_and_view_trace() -> None:
    compile_audio_probe()
    keys = (
        "\x1bOPx"
        "\x1bOQx"
        "\x1b[13~x"
        "\x1b[14~x"
        "\x1b[15~x"
        "\x1b[17~x"
        "\x1b[19~x"
        "\x1b[20~x"
        "\x1b[21~x"
        "\x1b[23~x"
        "\x1b[24~x"
        "-+[]"
        "\x1b[C\x1b[D\x1b[A\x1b[B"
        "1,.kKmMppq"
    )
    result = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=128", "80x25color"],
        check=False,
        input_text=keys,
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "IPLAY_SHELL_COMMAND": "true"},
    )
    assert result.returncode == 3
    for expected in (
        "Playback controls:",
        "pause_toggles=2 paused=0",
        "view_mask=63",
        "shell_invocations=0",
        "selected_channel=1",
        "muted_mask=1",
        "seek_generation=4",
        "stop=keyboard",
    ):
        assert expected in result.stdout
    controls = re.search(r"Playback controls:.*", result.stdout)
    assert controls
    assert "volume=100" in controls.group(0)
    assert "amplification=100" in controls.group(0)
    assert "loop=1" in controls.group(0)
    assert "interpolation=0" in controls.group(0)
    assert "protracker=0" in controls.group(0)
    assert "ignore_bpm=1" in controls.group(0)
    assert "channel_generation=4" in controls.group(0)


def test_original_listing_defines_f6_pan_view_and_f8_shell_restore_contract() -> None:
    listing = (ROOT / "IPLAY.lst").read_text(errors="ignore")
    f6 = listing[listing.index("seg001:2D18 f6_draw:"):listing.index("seg001:2DE0                 retn", listing.index("seg001:2D18 f6_draw:"))]
    shell = listing[listing.index("seg001:3169 dosexec"):listing.index("seg001:3217 dosexec", listing.index("seg001:3169 dosexec"))]
    assert "mov     al, fs:[bx+3Ah]" in f6
    assert "shr     al, 3" in f6
    player = (ROOT / "rewrite" / "modern_player.cpp").read_text()
    assert "static unsigned base_pan[IPLAY_MODPLUG_UI_MAX_CHANNELS]" in player
    assert "result->audio.channel_pan_valid_mask & (1u << channel)" in player
    assert ": base_pan[channel]" in player
    assert "mov     al, 4Dh ; 'M'" in f6
    assert "mov     al, 4Ch ; 'L'" in f6
    assert "mov     al, 52h ; 'R'" in f6
    assert "call    get_comspec" in shell
    assert "int     21h             ; DOS - 2+ - LOAD OR EXECUTE (EXEC)" in shell
    assert "call    doschdir" in shell


def test_preferred_player_f6_pan_view_and_f8_shell_resume_playback() -> None:
    compile_audio_probe()
    result = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
        check=False,
        input_text="\x1b[17~\x1b[19~q",
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "IPLAY_SHELL_COMMAND": "printf IPLAYSHELL"},
    )
    assert result.returncode == 3
    assert "DOS Shell (Type EXIT to return)" not in result.stdout
    assert "IPLAYSHELL" not in result.stdout
    assert "view_mask=32" in result.stdout
    assert "shell_invocations=0" in result.stdout
    assert "stop=keyboard" in result.stdout


def test_preferred_player_pause_resume_preserves_position_and_control_generation() -> None:
    compile_audio_probe()
    result = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
        check=False,
        input_text="ppq",
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
    )
    assert result.returncode == 3
    assert "Paus - press P or Space to resume" in result.stdout
    match = re.search(r"Playback pump: blocks=(\d+)", result.stdout)
    assert match and int(match.group(1)) >= 2
    assert "Playback controls: volume=100 loop=0 interpolation=1 protracker=1 ignore_bpm=0 generation=0" in result.stdout
    assert "stop=keyboard" in result.stdout


def test_preferred_player_brackets_change_original_amplification_and_pcm() -> None:
    compile_audio_probe()

    def playback(input_text: str) -> tuple[int, str]:
        result = run(
            [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
            check=False,
            input_text=input_text,
            env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        )
        assert result.returncode == 3
        checksum = re.search(r"Playback pump:.*checksum=(\d+)", result.stdout)
        assert checksum
        return int(checksum.group(1)), result.stdout

    baseline, _ = playback("xq")
    amplified, output = playback("]q")
    reduced, reduced_output = playback("[q")
    assert amplified != baseline
    assert reduced != baseline
    assert "generation=1 amplification=110" in output
    assert "generation=1 amplification=90" in reduced_output
    assert "110%" in output


def test_preferred_player_arrow_keys_seek_original_rows_and_change_pcm() -> None:
    compile_audio_probe()

    def playback(input_text: str) -> tuple[int, str]:
        result = run(
            [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
            check=False,
            input_text=input_text,
            env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
        )
        assert result.returncode == 3
        checksum = re.search(r"Playback pump:.*checksum=(\d+)", result.stdout)
        assert checksum
        return int(checksum.group(1)), result.stdout

    baseline, _ = playback("xxq")
    forward, output = playback("\x1b[Cxq")
    forward_ten, forward_ten_output = playback("\x1b[Axq")
    back, back_output = playback("\x1b[Dxq")
    back_ten, back_ten_output = playback("\x1b[Bxq")
    assert forward != baseline
    assert forward_ten != baseline
    assert "generation=1 amplification=100 seek=0:2 seek_generation=1" in output
    assert "generation=1 amplification=100 seek=0:10 seek_generation=1" in forward_ten_output
    assert "generation=1 amplification=100 seek=0:0 seek_generation=1" in back_output
    assert "generation=1 amplification=100 seek=0:0 seek_generation=1" in back_ten_output
    listing = (ROOT / "IPLAY.lst").read_text(errors="ignore")
    keyboard = listing[listing.index("seg001:0EAD keyb_19EFD"):listing.index("seg001:0FEF keyb_19EFD      endp")]
    assert "mov     cx, 2" in keyboard
    assert "cmp     ax, 0E04Dh      ; gr_right" in keyboard
    assert "cmp     ax, 0E04Bh      ; gr_left" in keyboard
    presenter = (ROOT / "rewrite" / "notcurses_presenter.cpp").read_text()
    visualizer = (ROOT / "rewrite" / "sdl_visualizer.cpp").read_text()
    probe = (ROOT / "rewrite" / "modplug_audio_probe.cpp").read_text()
    assert "case NCKEY_LEFT: return IPLAY_NOTCURSES_KEY_LEFT;" in presenter
    assert "case NCKEY_RIGHT: return IPLAY_NOTCURSES_KEY_RIGHT;" in presenter
    assert "case NCKEY_UP: return IPLAY_NOTCURSES_KEY_UP;" in presenter
    assert "case NCKEY_DOWN: return IPLAY_NOTCURSES_KEY_DOWN;" in presenter
    assert "SDLK_LEFT) return 13;" in visualizer
    assert "SDLK_RIGHT) return 14;" in visualizer
    assert "SDLK_UP) return 15;" in visualizer
    assert "SDLK_DOWN) return 16;" in visualizer
    assert "SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP" in visualizer
    assert "event.key.keysym.sym == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT)" in visualizer
    assert "SDL_GetWindowFlags(visualizer.window)" in visualizer
    assert "SDL_SetWindowFullscreen(visualizer.window, mode)" in visualizer
    assert "IPLAY_NOTCURSES_KEY_LEFT) return NATIVE_KEY_SEEK_BACK_2;" in probe
    assert "IPLAY_NOTCURSES_KEY_RIGHT) return NATIVE_KEY_SEEK_FORWARD_2;" in probe
    assert "IPLAY_NOTCURSES_KEY_UP) return NATIVE_KEY_SEEK_FORWARD_10;" in probe
    assert "IPLAY_NOTCURSES_KEY_DOWN) return NATIVE_KEY_SEEK_BACK_10;" in probe


def test_original_listing_and_sdl_sink_share_immediate_stop_before_deinit_order() -> None:
    listing = (ROOT / "IPLAY.lst").read_text(errors="ignore")
    deinit = listing[listing.index("seg000:25B9 deinit_125B9"):listing.index("seg000:25D9 deinit_125B9")]
    assert deinit.index("call    near ptr snd_offx") < deinit.index("call    snd_deinit")
    assert deinit.index("call    snd_deinit") < deinit.index("call    initclockfromrtc")
    probe = (ROOT / "rewrite" / "modplug_audio_probe.cpp").read_text()
    close = probe[probe.index("static void native_audio_sink_close(NativeAudioSink *sink)"):probe.index("static void native_audio_sink_close_at_exit")]
    assert close.index("SDL_PauseAudioDevice(sink->device, 1)") < close.index("SDL_ClearQueuedAudio(sink->device)")
    assert close.index("SDL_ClearQueuedAudio(sink->device)") < close.index("SDL_CloseAudioDevice(sink->device)")
    assert close.index("SDL_CloseAudioDevice(sink->device)") < close.index("SDL_QuitSubSystem(SDL_INIT_AUDIO)")
    assert "native_audio_sink_drain_sdl" not in probe


def test_sdl_sink_pauses_clears_and_closes_on_block_limit_and_keyboard_exit() -> None:
    compile_audio_probe()
    for args, input_text in (
        ([str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=1", "80x25color", "--sdl-audio"], None),
        ([str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=32", "80x25color", "--sdl-audio", "--stdin-keyboard"], "q"),
    ):
        result = run(args, check=False, input_text=input_text, env={**os.environ, "SDL_AUDIODRIVER": "dummy"})
        assert result.returncode == 3
        assert "SDL audio sink: requested=1 opened=1" in result.stdout
        assert "paused=1 queue_cleared=1 closed=1" in result.stdout


def test_native_binary_applies_volume_loop_and_interpolation_controls() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=32", "80x25color", "--stdin-keyboard"],
        check=False,
        input_text="-\x1b[23~\x1b[24~q",
    )
    assert result.returncode == 3
    assert "Playback controls: volume=99 loop=1 interpolation=0 protracker=1 ignore_bpm=0 generation=3" in result.stdout
    assert re.search(r"Main Volume\s+:\s+99%", result.stdout)
    assert "stop=keyboard" in result.stdout


def test_native_binary_preserves_original_256_step_volume_range() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=128", "80x25color", "--stdin-keyboard"],
        check=False,
        input_text="-" * 100 + "q",
    )
    assert result.returncode == 3
    assert "Playback controls: volume=21" in result.stdout
    assert "generation=100" in result.stdout
    assert re.search(r"Main Volume\s+:\s+21%", result.stdout)


def test_preferred_player_supports_original_mouse_redraw_and_exit_actions() -> None:
    compile_audio_probe()
    redraw = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
        check=False,
        input_text="\x1b[<0;3;2Mq",
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
    )
    assert redraw.returncode == 3
    assert "mouse_redraws=1 mouse_exits=0" in redraw.stdout

    exit_click = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
        check=False,
        input_text="\x1b[<2;40;20M",
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
    )
    assert exit_click.returncode == 3
    assert "mouse_redraws=0 mouse_exits=1" in exit_click.stdout
    assert "stop=keyboard" in exit_click.stdout

    paused_exit = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=32", "80x25color"],
        check=False,
        input_text="p\x1b[<0;3;2M\x1b[<2;40;20M",
        env={**os.environ, "SDL_AUDIODRIVER": "dummy"},
    )
    assert paused_exit.returncode == 3
    assert "mouse_redraws=1 mouse_exits=1" in paused_exit.stdout
    assert "pause_toggles=1 paused=1" in paused_exit.stdout


def test_preferred_player_mouse_controls_visible_playback_options() -> None:
    compile_audio_probe()
    result = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=64", "80x25color"],
        check=False,
        input_text=(
            "\x1b[<0;60;18M"
            "\x1b[<0;60;19M"
            "\x1b[<0;60;20M"
            "\x1b[<0;60;21M"
            "\x1b[<0;60;22M"
            "\x1b[<0;79;23M"
            "q"
        ),
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "25"},
    )
    assert result.returncode == 3
    assert (
        "volume=99 loop=1 interpolation=0 protracker=0 ignore_bpm=1 "
        "generation=6 amplification=110"
    ) in result.stdout

    volume_and_amplification_buttons = run(
        [str(PLAYER_ALIAS), str(aryx_s3m_path()), "--blocks=64", "80x25color"],
        check=False,
        input_text=(
            "\x1b[<0;60;22M"
            "\x1b[<0;79;22M"
            "\x1b[<0;60;23M"
            "\x1b[<0;79;23M"
            "q"
        ),
        env={**os.environ, "SDL_AUDIODRIVER": "dummy", "COLUMNS": "80", "LINES": "25"},
    )
    assert volume_and_amplification_buttons.returncode == 3
    assert "volume=100" in volume_and_amplification_buttons.stdout
    assert "generation=4 amplification=100" in volume_and_amplification_buttons.stdout


def test_native_binary_loop_control_restarts_short_module(tmp_path: Path) -> None:
    compile_audio_probe()
    module = write_sparse_active_channel_mod(tmp_path / "ACTIVE.MOD")
    result = run(
        [str(AUDIO_PROBE), str(module), "--blocks=800", "80x25color", "--stdin-keyboard"],
        check=False,
        input_text="\x1b[23~",
        timeout=60,
    )
    assert result.returncode == 3
    assert "Playback pump: blocks=800" in result.stdout
    assert "source_end=0 stop=block-limit" in result.stdout
    assert "Playback controls: volume=100 loop=1 interpolation=1 protracker=1 ignore_bpm=0 generation=1" in result.stdout


def test_native_binary_volume_and_interpolation_controls_change_pcm() -> None:
    compile_audio_probe()

    def checksum(input_text: str) -> int:
        result = run(
            [str(AUDIO_PROBE), str(aryx_s3m_path()), "--blocks=32", "80x25color", "--stdin-keyboard"],
            check=False,
            input_text=input_text,
        )
        assert result.returncode == 3
        match = re.search(r"Playback pump:.*checksum=(\d+)", result.stdout)
        assert match
        return int(match.group(1))

    baseline = checksum("")
    quieter = checksum("-")
    nearest = checksum("\x1b[24~")
    assert quieter != baseline
    assert nearest != baseline


def test_native_binary_protracker_and_ignore_bpm_controls_change_mod_pcm(tmp_path: Path) -> None:
    compile_audio_probe()
    module = write_sparse_active_channel_mod(tmp_path / "COMPAT.MOD", include_compatibility_events=True)

    def checksum(input_text: str) -> int:
        result = run(
            [str(AUDIO_PROBE), str(module), "--blocks=96", "80x25color", "--stdin-keyboard"],
            check=False,
            input_text=input_text,
        )
        assert result.returncode == 3
        match = re.search(r"Playback pump:.*checksum=(\d+)", result.stdout)
        assert match
        return int(match.group(1))

    baseline = checksum("")
    non_protracker = checksum("\x1b[20~")
    ignore_bpm = checksum("\x1b[21~")
    assert non_protracker != baseline
    assert ignore_bpm != baseline


def test_modern_player_facade_plays_large_real_s3m_through_libmodplug_boundary() -> None:
    compile_audio_probe()
    result = run([str(AUDIO_PROBE), str(hacker4_s3m_path())])
    match = re.search(
        r"status=ok route_id=0 route=external-library provider=libmikmod stop=source-end source_end=1 blocks=(\d+) source_frames=(\d+) accepted_bytes=(\d+) "
        r"frames_written=(\d+) dropped=(\d+) capture_calls=(\d+) capture_bytes=(\d+) capture_checksum=(\d+) source_checksum=(\d+)",
        result.stdout,
    )
    assert match, result.stdout + result.stderr
    blocks, source_frames, accepted_bytes, frames_written, dropped, capture_calls, capture_bytes, capture_checksum, source_checksum = (
        int(group) for group in match.groups()
    )
    assert blocks > 2
    assert source_frames > 1024
    assert accepted_bytes == source_frames * 4
    assert frames_written == source_frames
    assert dropped == 0
    assert capture_calls == blocks
    assert capture_bytes == source_frames * 4
    assert capture_checksum != 0
    assert source_checksum != 0
    assert "Audio backend: SDL-compatible SB16 16-bit stereo" in result.stdout
    assert "Playback enabled" in result.stdout
    assert "screen_present=calls:1 bytes:4000 cols:80 rows:25" in result.stdout


def test_modern_playback_result_reports_keyboard_stop_reason() -> None:
    compile_audio_probe()
    result = run(
        [str(AUDIO_PROBE), str(aryx_s3m_path()), "--keyboard-after-one"],
        check=False,
    )
    assert result.returncode == 3
    assert "status=keyboard route_id=0 route=external-library provider=libmikmod stop=keyboard source_end=0 blocks=1 source_frames=512" in result.stdout
    assert "stop=keyboard" in result.stdout
    assert "Playback enabled" in result.stdout


def test_modern_failure_status_renders_disabled_screen_status(tmp_path: Path) -> None:
    compile_audio_probe()
    inr = tmp_path / "SMOKE.INR"
    inr.write_bytes(b"INR\x00SMOKE INR")
    result = run(
        [str(AUDIO_PROBE), str(inr)],
        check=False,
    )
    assert result.returncode == 3
    assert "status=project-decoder-unavailable" in result.stdout
    assert "route_id=1 route=project-owned provider=native" in result.stdout
    assert "summary=\"Audio backend: SDL-compatible SB16 16-bit stereo; Playback disabled; route=project-owned; provider=native; status=project-decoder-unavailable;" in result.stdout
    assert 'screen_playback="' in result.stdout
    assert "Playback disabled" in result.stdout
    assert "24bit Interpolation      F-12" in result.stdout
    assert 'screen_status="' in result.stdout
    assert "Module Type    INR" in result.stdout
    assert "project-decoder-unavailable" in result.stdout
    assert "screen_present=calls:1 bytes:4000 cols:80 rows:25" in result.stdout


def test_modern_player_facade_keeps_project_owned_inr_out_of_external_decoder(tmp_path: Path) -> None:
    compile_audio_probe()
    inr = tmp_path / "SMOKE.INR"
    inr.write_bytes(b"INR\x00SMOKE INR")
    result = run(
        [str(AUDIO_PROBE), str(inr)],
        check=False,
    )
    assert result.returncode == 3
    assert "status=project-decoder-unavailable" in result.stdout
    assert "route_id=1 route=project-owned provider=native" in result.stdout


def test_modern_player_facade_reports_unsupported_when_unknown_extension_cannot_decode(tmp_path: Path) -> None:
    compile_audio_probe()
    bad = tmp_path / "BAD.XYZ"
    bad.write_bytes(b"bad")
    result = run(
        [str(AUDIO_PROBE), str(bad)],
        check=False,
    )
    assert result.returncode == 3
    assert "status=unsupported-format" in result.stdout
    assert "route_id=2 route=probe-by-content provider=libmikmod" in result.stdout
    assert "summary=\"Audio backend: SDL-compatible SB16 16-bit stereo; Playback disabled; route=probe-by-content; provider=libmikmod; status=unsupported-format;" in result.stdout


def test_modern_player_facade_reports_external_decoder_failure_for_corrupt_known_tracker(tmp_path: Path) -> None:
    compile_audio_probe()
    bad = tmp_path / "BAD.S3M"
    bad.write_bytes(b"bad")
    result = run(
        [str(AUDIO_PROBE), str(bad)],
        check=False,
    )
    assert result.returncode == 3
    assert "status=external-decoder-failed" in result.stdout
    assert "route_id=0 route=external-library provider=libmikmod" in result.stdout
    assert "summary=\"Audio backend: SDL-compatible SB16 16-bit stereo; Playback disabled; route=external-library; provider=libmikmod; status=external-decoder-failed;" in result.stdout


def test_modern_player_facade_reports_external_decoder_failure_for_new_library_tracker_extensions(tmp_path: Path) -> None:
    compile_audio_probe()
    for suffix in ("WOW", "OKT", "OCT", "XM", "IT", "PTM", "AMS", "DBM", "DMF", "MDL", "DSM", "MED", "IMF", "J2B"):
        bad = tmp_path / f"BAD.{suffix}"
        bad.write_bytes(b"bad")
        result = run(
            [str(AUDIO_PROBE), str(bad)],
            check=False,
        )
        assert result.returncode == 3
        assert "status=external-decoder-failed" in result.stdout
        assert "route_id=0 route=external-library provider=libmikmod" in result.stdout
        assert "status=unsupported-format" not in result.stdout


def test_modern_player_facade_plays_valid_tracker_through_new_library_extension(tmp_path: Path) -> None:
    compile_audio_probe()
    disguised = tmp_path / "ARYX.XM"
    disguised.write_bytes(aryx_s3m_path().read_bytes())
    result = run([str(AUDIO_PROBE), str(disguised)])
    assert "status=ok" in result.stdout
    assert "route_id=0 route=external-library provider=libmikmod" in result.stdout
    assert "Audio backend: SDL-compatible SB16 16-bit stereo" in result.stdout
    assert "Playback enabled" in result.stdout
    assert "source_frames=" in result.stdout
    assert "accepted_bytes=" in result.stdout


def test_modern_player_facade_allows_tracker_header_to_override_unknown_extension(tmp_path: Path) -> None:
    compile_audio_probe()
    disguised = tmp_path / "ARYX.XYZ"
    disguised.write_bytes(aryx_s3m_path().read_bytes())
    result = run([str(AUDIO_PROBE), str(disguised)])
    assert "status=ok" in result.stdout
    assert "route_id=0 route=external-library provider=libmikmod" in result.stdout
    assert "route_id=2 route=probe-by-content" not in result.stdout
    assert "source_frames=" in result.stdout


def test_modern_player_facade_lists_external_tracker_extensions_for_library_boundary() -> None:
    compile_audio_probe()
    expected = [
        ".mod", ".nst", ".s3m", ".stm", ".669", ".mtm", ".psm", ".far", ".ult",
        ".wow", ".okt", ".oct", ".xm", ".it", ".ptm", ".ams", ".dbm", ".dmf",
        ".mdl", ".dsm", ".med", ".imf", ".j2b",
    ]
    dos_external = list(expected)
    modern_h = (ROOT / "rewrite" / "modern_player.hpp").read_text()
    modern = (ROOT / "rewrite" / "modern_player.cpp").read_text()
    player = (ROOT / "rewrite" / "iplay_player.c").read_text()
    result = run([str(AUDIO_PROBE), "--list-extensions"])
    assert result.stdout.strip() == "extensions=" + ",".join(expected)
    assert all(ext in expected for ext in dos_external)
    assert ".inr" not in expected
    assert "size_t iplay_modern_external_tracker_extension_count(void);" in modern_h
    assert "const char *iplay_modern_external_tracker_extension(size_t index);" in modern_h
    assert "int iplay_modern_path_is_external_tracker(const char *path);" in modern_h
    assert "int iplay_modern_path_is_project_owned(const char *path);" in modern_h
    assert "enum IplayModernDecoderRoute" in modern_h
    assert "IplayModernDecoderRoute iplay_modern_decoder_route(const char *path);" in modern_h
    assert "const char *iplay_modern_decoder_route_name(const char *path);" in modern_h
    assert "int iplay_modern_decoder_route_uses_external_library(const char *path);" in modern_h
    assert "static const char *const modern_external_tracker_extensions[]" in modern
    assert "int iplay_modern_path_is_external_tracker(const char *path)" in modern
    assert "int iplay_modern_path_is_project_owned(const char *path)" in modern
    assert "IplayModernDecoderRoute iplay_modern_decoder_route(const char *path)" in modern
    assert "const char *iplay_modern_decoder_route_name(const char *path)" in modern
    assert "int iplay_modern_decoder_route_uses_external_library(const char *path)" in modern
    assert 'return "external-library";' in modern
    assert 'return "project-owned";' in modern
    assert 'return "probe-by-content";' in modern
    for ext in expected:
        assert f'"{ext}"' in modern
    for ext in dos_external:
        assert f'{{"{ext}"' in player
    assert 'modern_extension_equals(modern_path_extension(path), ".inr")' in modern
    assert '".inr"' not in modern.split("static const char *const modern_external_tracker_extensions[]", 1)[1].split("};", 1)[0]
    backend = ' backend="SDL-compatible SB16 16-bit stereo"'
    assert run([str(AUDIO_PROBE), "--classify", "ARYX.S3M"]).stdout.strip() == "external=1 project=0 route_id=0 route=external-library library=1" + backend
    assert run([str(AUDIO_PROBE), "--classify", "SONG.XM"]).stdout.strip() == "external=1 project=0 route_id=0 route=external-library library=1" + backend
    assert run([str(AUDIO_PROBE), "--classify", "PROJECT.INR"]).stdout.strip() == "external=0 project=1 route_id=1 route=project-owned library=0" + backend
    assert run([str(AUDIO_PROBE), "--classify", "README.TXT"]).stdout.strip() == "external=0 project=0 route_id=2 route=probe-by-content library=1" + backend
    for ext in dos_external:
        assert run([str(AUDIO_PROBE), "--classify", "DOSFMT" + ext.upper()]).stdout.strip() == "external=1 project=0 route_id=0 route=external-library library=1" + backend
