from __future__ import annotations

import struct
import subprocess
import re
import os
import signal
from pathlib import Path
from typing import Optional

import pytest

from player_behavior_fixtures import (
    SB16_BOUNDED_BLOCK_BYTES,
    SB16_BOUNDED_BLOCK_FRAMES,
    SB16_CONTINUOUS_BLOCK_BYTES,
    SB16_CONTINUOUS_BLOCK_FRAMES,
    assert_decoder_event,
    assert_decoder_geometry,
    assert_decoder_handoff,
    assert_decoder_handoff_absent,
    assert_decoder_route,
    assert_decoder_route_absent,
    assert_decoder_progress,
    assert_decoder_progress_block,
    assert_decoder_voice,
    assert_module_loaded,
    assert_module_not_loaded,
    assert_module_loader,
    assert_module_size,
    assert_module_title,
    assert_module_type_tag,
    assert_ffi_marker,
    assert_no_extra_rewrite_visible_text_on_original_blank_cells,
    assert_help_usage,
    assert_orders_channels,
    assert_pcm_source_route,
    assert_playback_disabled,
    assert_playback_loop,
    assert_playback_output,
    assert_sb16_audio_scope,
    assert_playback_pump_sb16_stereo,
    assert_playback_pump_stop_state,
    assert_screen_present_content,
    assert_sdl_compatible_audio_backend,
    assert_sb16_stereo_frame_bytes,
    assert_supported_dos_formats,
    assert_original_visible_row_cells_equal,
    assert_text_memory_matches_screen_present,
    assert_text_backend,
    assert_text_backend_memory,
    assert_text_screen_geometry,
    assert_unsupported_module,
    VGA_COLOR_TEXT_SEG,
    VGA_MONO_TEXT_SEG,
    VGA_TEXT_OFFSET,
    assert_text_cell_span_equal,
    assert_text_cell_span_at_original_location_equal,
    dos_physical_address,
    text_cell_digest,
    text_cells_visible_text,
    text_memory_slice,
    text_memory_digest,
    text_memory_visible_text,
    text_mode_byte_count,
    parse_playback_pump,
    parse_screen_present_digest,
    write_endcont_module,
    write_smoke_modules,
)

# Decoder route evidence markers expected from DOS output:
# Decoder route: id=0 name=external-library
# Decoder route: id=1 name=project-owned

ROOT = Path(__file__).resolve().parents[1]
KVIKDOS = Path("/home/xor/kvikdos/kvikdos")
ORIGINAL_EXE = ROOT / "original" / "IPLAY.EXE"
ORIGINAL_SEG001_FILE_OFFSET = 0x9210
ORIGINAL_CLEANUP_EXIT_HLT_TRAP_FILE_OFFSET = ORIGINAL_SEG001_FILE_OFFSET + 0x0273
ORIGINAL_PLAYBACK_UI_HLT_TRAP_FILE_OFFSET = ORIGINAL_SEG001_FILE_OFFSET + 0x170C
ORIGINAL_DYNAMIC_UI_ENTRY_FILE_OFFSET = ORIGINAL_SEG001_FILE_OFFSET + 0x0EAD
ORIGINAL_DYNAMIC_UI_HLT_TRAP_FILE_OFFSET = ORIGINAL_SEG001_FILE_OFFSET + 0x0EE3
BUILD_DIR = ROOT / "rewrite" / ".build"
IPLAYC_EXE = BUILD_DIR / "IPLAYC.EXE"
IPLAYTRY_EXE = BUILD_DIR / "IPLAYTRY.EXE"
IPLAYCONT_EXE = BUILD_DIR / "IPLAYCONT.EXE"
IPLAYDIAG_EXE = BUILD_DIR / "IPLAYDIAG.EXE"
IPLAYHW_EXE = BUILD_DIR / "IPLAYHW.EXE"
FAST_DIAG_BLOCKS = "32"
DEEP_PLAYBACK_FIXTURES = {
    "PBRBASE.S3M",
    "PBREAK.S3M",
    "S3MPBR.S3M",
    "SPDBASE.S3M",
    "SPDSET3.S3M",
    "S3MSPD.S3M",
    "TMPBASE.S3M",
    "TMP180.S3M",
    "S3MTMP.S3M",
    "FTMP125.MOD",
    "FTMP180.MOD",
    "MFSPD.MOD",
    "MFTMP.MOD",
    "S3MPATH.S3M",
    "NSTPATH.NST",
    "PSMPATH.PSM",
    "FARPATH.FAR",
    "E669PATH.669",
    "ULTPATH.ULT",
    "MTMPATH.MTM",
    "STMPATH.STM",
    "MODPATH.MOD",
    "ARYX.S3M",
}

DOS_FIXTURE_METADATA_CASES = [
    ("SMOKE.S3M", "s3m_module (Scream Tracker 3)", "SMOKE S3M", ["Module type tag: 204D3353", "Order preview: 00 01 02"]),
    ("SMOKE.MOD", "mod_n_t_module (ProTracker/NoiseTracker MOD)", "SMOKE MOD", ["Order preview: 00 01 02 04 FE"]),
    ("SMOKE.NST", "mod_n_t_module (ProTracker/NoiseTracker MOD)", "SMOKE NST", ["Order preview: 00 01 02 04 FE"]),
    ("SMOKE.MTM", "mtm_module (MultiTracker MTM)", "SMOKE MTM", ["Order preview: 00 01 02 03 04"]),
    ("SMOKE.FAR", "far_module (Farandole FAR)", "SMOKE FAR", []),
    ("SMOKE.669", "e669_module (Composer 669)", "SMOKE669", ["Module type tag: 39363645"]),
    ("SMOKE.ULT", "ult_module (UltraTracker ULT)", "SMOKE ULT", []),
    ("SMOKE.WOW", "external_module (WOW tracker)", "SMOKE WOW", ["Module type tag: 20545845"]),
    ("SMOKE.OKT", "external_module (Oktalyzer OKT)", "SMOKE OKT", ["Module type tag: 20545845"]),
    ("SMOKE.OCT", "external_module (Octalyzer OCT)", "SMOKE OCT", ["Module type tag: 20545845"]),
    ("SMOKE.XM", "external_module (FastTracker XM)", "SMOKE XM", ["Module type tag: 20545845"]),
    ("SMOKE.IT", "external_module (Impulse Tracker IT)", "SMOKE IT", ["Module type tag: 20545845"]),
    ("SMOKE.PTM", "external_module (PolyTracker PTM)", "SMOKE PTM", ["Module type tag: 20545845"]),
    ("SMOKE.AMS", "external_module (Extreme Tracker AMS)", "SMOKE AMS", ["Module type tag: 20545845"]),
    ("SMOKE.DBM", "external_module (DigiBooster DBM)", "SMOKE DBM", ["Module type tag: 20545845"]),
    ("SMOKE.DMF", "external_module (X-Tracker DMF)", "SMOKE DMF", ["Module type tag: 20545845"]),
    ("SMOKE.MDL", "external_module (DigiTrakker MDL)", "SMOKE MDL", ["Module type tag: 20545845"]),
    ("SMOKE.DSM", "external_module (DSIK DSM)", "SMOKE DSM", ["Module type tag: 20545845"]),
    ("SMOKE.MED", "external_module (OctaMED MED)", "SMOKE MED", ["Module type tag: 20545845"]),
    ("SMOKE.IMF", "external_module (Imago Orpheus IMF)", "SMOKE IMF", ["Module type tag: 20545845"]),
    ("SMOKE.J2B", "external_module (Jazz Jackrabbit 2 J2B)", "SMOKE J2B", ["Module type tag: 20545845"]),
    ("SMOKE.PSM", "psm_module (ProTracker Studio PSM)", "SMOKE PSM", []),
    ("SMOKE.INR", "inr_module (Inertia INR)", "SMOKE INR", []),
    ("SMOKE.STM", "_2stm_module (Scream Tracker 2 STM)", "SMOKE STM", []),
]

EXTERNAL_DECODER_FIXTURES = [
    filename for filename, _loader, _title, _extra_lines in DOS_FIXTURE_METADATA_CASES
    if filename != "SMOKE.INR"
]
DEFERRED_PROJECT_OWNED_FORMATS = {"INR"}


def run_dos(exe: Path, *args: str, timeout: Optional[int] = None, cwd: Path = BUILD_DIR) -> subprocess.CompletedProcess[str]:
    if timeout is None:
        timeout = 3 if exe == ORIGINAL_EXE else int(os.environ.get("IPLAY_KVIKDOS_TEST_TIMEOUT", "3"))
    if exe.name.upper() == "IPLAY.EXE" and exe.parent != BUILD_DIR:
        timeout = min(timeout, int(os.environ.get("IPLAY_ORIGINAL_KVIKDOS_MAX_TIMEOUT", "3")))
    dos_args = list(args)
    if exe == IPLAYDIAG_EXE and dos_args and not dos_args[0].startswith("--blocks="):
        module_name = Path(dos_args[0]).name.upper()
        if module_name not in DEEP_PLAYBACK_FIXTURES:
            dos_args = [f"--blocks={FAST_DIAG_BLOCKS}", *dos_args]
            timeout = min(timeout, 5)
    command = [
        ["timeout", "-k", "1", str(timeout), str(KVIKDOS), str(exe), *dos_args],
    ][0]
    proc = subprocess.Popen(
        command,
        cwd=cwd,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        start_new_session=True,
    )
    try:
        stdout, stderr = proc.communicate(timeout=timeout + 2)
    except subprocess.TimeoutExpired:
        os.killpg(proc.pid, signal.SIGKILL)
        stdout, stderr = proc.communicate()
        stderr = (stderr or "") + f"\npython timeout after {timeout + 2}s: {' '.join(command)}\n"
        return subprocess.CompletedProcess(command, 124, stdout or "", stderr)
    return subprocess.CompletedProcess(command, proc.returncode, stdout or "", stderr or "")


@pytest.fixture(scope="module", autouse=True)
def built_player() -> None:
    if not IPLAYC_EXE.exists() or not IPLAYCONT_EXE.exists() or not IPLAYDIAG_EXE.exists():
        subprocess.run([str(ROOT / "rewrite" / "build_player.sh")], cwd=ROOT, check=True, timeout=30)
    write_smoke_modules(BUILD_DIR)


def combined_output(result: subprocess.CompletedProcess[str]) -> str:
    return result.stdout + result.stderr


def playback_checksum(output: str) -> int:
    return int(parse_playback_pump(output)["checksum"])


def advertised_supported_formats(output: str) -> set[str]:
    match = re.search(r"^Supported by this DOS hardware build:\s+(.+)$", output, re.M)
    if not match:
        raise AssertionError(f"missing supported-format line in {output!r}")
    return set(match.group(1).split())


def assert_external_native_preview(output: str) -> None:
    route = assert_decoder_route(output, 0, "external-library")
    assert_pcm_source_route(output, route["id"], "e", "native-preview", truncated=0, input_kind="memory", hook_provider="none")
    assert_decoder_handoff(output, "external tracker -> SB16 PCM seam.")


def assert_external_pcm_source(
    output: str,
    source: str,
    provider: str,
    truncated: int,
    input_kind: str,
    stream_start: int,
) -> None:
    route = assert_decoder_route(output, 0, "external-library")
    assert_pcm_source_route(
        output,
        route["id"],
        "e",
        provider,
        source=source,
        truncated=truncated,
        input_kind=input_kind,
        hook_provider="none",
        stream_start=stream_start,
    )


def assert_bounded_sb16_playback_blocks(output: str, blocks: int) -> dict[str, object]:
    pump = parse_playback_pump(output)
    assert_playback_pump_sb16_stereo(pump, blocks, SB16_BOUNDED_BLOCK_FRAMES)
    assert_playback_pump_stop_state(pump, 1, 0, "block-limit")
    return pump


def assert_bounded_sb16_playback(output: str) -> dict[str, object]:
    return assert_bounded_sb16_playback_blocks(output, 32)


def assert_bounded_source_end_playback(output: str, blocks: int) -> dict[str, object]:
    pump = parse_playback_pump(output)
    if blocks:
        assert_playback_pump_sb16_stereo(pump, blocks, SB16_BOUNDED_BLOCK_FRAMES)
    else:
        assert pump["blocks"] == 0
        assert pump["frames"] == 0
        assert pump["accepted"] == 0
    assert_playback_pump_stop_state(pump, 0, 1, "source-end")
    return pump


def original_mz_layout() -> dict[str, int]:
    data = ORIGINAL_EXE.read_bytes()
    (
        signature,
        last_page_size,
        pages_in_file,
        _num_relocs,
        header_paragraphs,
        min_extra_paragraphs,
        _max_extra_paragraphs,
        ss,
        sp,
        _checksum,
        ip,
        cs,
    ) = struct.unpack_from("<HHHHHHHHHHHH", data, 0)
    image_size = (pages_in_file - 1) * 512 + (last_page_size or 512)
    load_module_size = image_size - header_paragraphs * 16
    min_program_end = 0x100 + load_module_size + min_extra_paragraphs * 16
    stack_end = ss * 16 + sp
    entrypoint = cs * 16 + ip
    return {
        "signature": signature,
        "header_bytes": header_paragraphs * 16,
        "load_module_size": load_module_size,
        "min_extra_bytes": min_extra_paragraphs * 16,
        "min_program_end": min_program_end,
        "stack_end": stack_end,
        "ss": ss,
        "sp": sp,
        "cs": cs,
        "ip": ip,
        "entrypoint": entrypoint,
    }


def write_minalloc_patched_original(path: Path) -> Path:
    layout = original_mz_layout()
    data = bytearray(ORIGINAL_EXE.read_bytes())
    needed_extra_bytes = layout["stack_end"] - 0x100 - layout["load_module_size"]
    needed_extra_paragraphs = (needed_extra_bytes + 15) // 16
    struct.pack_into("<H", data, 0x0A, needed_extra_paragraphs)
    path.write_bytes(data)
    return path


def write_minalloc_patched_original_with_hlt_trap(path: Path, file_offset: int) -> Path:
    patched = write_minalloc_patched_original(path)
    data = bytearray(patched.read_bytes())
    assert data[file_offset:file_offset + 2] == b"\xB4\x4C"
    data[file_offset] = 0xF4
    patched.write_bytes(data)
    return patched


def write_minalloc_patched_original_with_opcode_hlt_trap(
    path: Path,
    file_offset: int,
    expected_opcode: int,
) -> Path:
    patched = write_minalloc_patched_original(path)
    data = bytearray(patched.read_bytes())
    assert data[file_offset] == expected_opcode
    data[file_offset] = 0xF4
    patched.write_bytes(data)
    return patched


def write_minalloc_patched_original_with_dynamic_ui_hlt_trap(path: Path) -> Path:
    patched = write_minalloc_patched_original(path)
    data = bytearray(patched.read_bytes())
    assert data[ORIGINAL_DYNAMIC_UI_ENTRY_FILE_OFFSET:ORIGINAL_DYNAMIC_UI_ENTRY_FILE_OFFSET + 3] == b"\x9A\x5D\x26"
    assert data[ORIGINAL_DYNAMIC_UI_HLT_TRAP_FILE_OFFSET] == 0x80
    data[ORIGINAL_DYNAMIC_UI_ENTRY_FILE_OFFSET:ORIGINAL_DYNAMIC_UI_ENTRY_FILE_OFFSET + 3] = b"\xE9\x2F\x00"
    data[ORIGINAL_DYNAMIC_UI_HLT_TRAP_FILE_OFFSET] = 0xF4
    patched.write_bytes(data)
    return patched


def write_minalloc_patched_original_with_f4_draw_hlt_trap(path: Path) -> Path:
    patched = write_minalloc_patched_original_with_dynamic_ui_hlt_trap(path)
    data = bytearray(patched.read_bytes())
    call_offset = ORIGINAL_DYNAMIC_UI_HLT_TRAP_FILE_OFFSET - 4
    assert data[call_offset:call_offset + 4] == b"\xFF\x16\x4E\x16"
    data[call_offset:call_offset + 4] = b"\xE8\x8F\x0C\x90"
    patched.write_bytes(data)
    return patched


def write_minalloc_patched_original_with_f1_draw_hlt_trap(path: Path) -> Path:
    patched = write_minalloc_patched_original_with_dynamic_ui_hlt_trap(path)
    data = bytearray(patched.read_bytes())
    call_offset = ORIGINAL_DYNAMIC_UI_HLT_TRAP_FILE_OFFSET - 4
    assert data[call_offset:call_offset + 4] == b"\xFF\x16\x4E\x16"
    data[call_offset:call_offset + 4] = b"\xE8\xEF\x0D\x90"
    patched.write_bytes(data)
    return patched


def write_minalloc_patched_original_with_f6_draw_hlt_trap(path: Path) -> Path:
    patched = write_minalloc_patched_original_with_dynamic_ui_hlt_trap(path)
    data = bytearray(patched.read_bytes())
    call_offset = ORIGINAL_DYNAMIC_UI_HLT_TRAP_FILE_OFFSET - 4
    assert data[call_offset:call_offset + 4] == b"\xFF\x16\x4E\x16"
    data[call_offset:call_offset + 4] = b"\xE8\x36\x1E\x90"
    patched.write_bytes(data)
    return patched


def write_minalloc_patched_original_with_f3_draw_hlt_trap(path: Path) -> Path:
    patched = write_minalloc_patched_original_with_dynamic_ui_hlt_trap(path)
    data = bytearray(patched.read_bytes())
    call_offset = ORIGINAL_DYNAMIC_UI_HLT_TRAP_FILE_OFFSET - 4
    assert data[call_offset:call_offset + 4] == b"\xFF\x16\x4E\x16"
    data[call_offset:call_offset + 4] = b"\xE8\xC6\x09\x90"
    patched.write_bytes(data)
    return patched


def write_valid_original_config(path: Path, sound_card_type: int = 0x03) -> Path:
    settings = bytearray([sound_card_type, 0xFF, 0xFF, 0xFF, 0xFF, 0x2C, 0xFF, 0x14, 0x8B, 0x21, 0x4B, 0x00])
    settings[-1] = (-sum(settings[:-1])) & 0xFF
    path.write_bytes(bytes.fromhex("494e5210") + settings)
    return path


def comparable_help_lines(output: str) -> list[str]:
    wanted = (
        "Usage: IPLAY [Switches] [FileName.Ext|@FileList.Ext]",
        " /i  Display current soundcard settings",
    )
    return [line.rstrip() for line in output.splitlines() if line.rstrip() in wanted]


def test_original_iplay_mz_header_requires_more_than_plain_minalloc_stack_room() -> None:
    layout = original_mz_layout()

    assert layout["signature"] == 0x5A4D
    assert layout["header_bytes"] == 0x1C0
    assert layout["load_module_size"] == 0x16308
    assert layout["min_extra_bytes"] == 0x03B0
    assert layout["ss"] == 0x2451
    assert layout["sp"] == 0x1000
    assert layout["cs"] == 0x0905
    assert layout["ip"] == 0x0042
    assert layout["entrypoint"] < layout["load_module_size"]
    assert layout["stack_end"] > layout["min_program_end"]


def test_original_iplay_plain_kvikdos_reports_stack_layout_blocker() -> None:
    result = run_dos(ORIGINAL_EXE, timeout=5)
    out = combined_output(result)

    assert result.returncode != 0, out
    assert "fatal: DOS .exe stack pointer after end of program memory" in out
    assert "IPLAY.EXE" in out


def test_original_iplay_minalloc_patched_copy_reaches_program_config_check(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original(tmp_path / "IPLAY.EXE")
    result = run_dos(patched, timeout=10, cwd=tmp_path)
    out = combined_output(result)

    assert result.returncode == 1, out
    assert "Config file not found. Run ISETUP first" in out
    assert "DOS .exe stack pointer after end of program memory" not in out


def test_original_iplay_minalloc_patched_copy_with_valid_config_prints_help(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG")
    result = run_dos(patched, "/?", timeout=10, cwd=tmp_path)
    out = combined_output(result)

    assert result.returncode == 0, out
    assert "Inertia Player V1.22 written by Stefan Danes and Ramon van Gorkom" in out
    assert_help_usage(out)
    assert " /i  Display current soundcard settings" in out
    assert "Config file not found" not in out
    assert "DOS .exe stack pointer after end of program memory" not in out


def test_iplayc_dos_help_preserves_original_comparable_usage_lines(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG")

    original = run_dos(patched, "/?", timeout=10, cwd=tmp_path)
    rewrite = run_dos(IPLAYDIAG_EXE, "/?")
    original_out = combined_output(original)
    rewrite_out = combined_output(rewrite)

    assert original.returncode == rewrite.returncode == 0
    assert comparable_help_lines(original_out) == comparable_help_lines(rewrite_out)
    assert comparable_help_lines(original_out) == [
        "Usage: IPLAY [Switches] [FileName.Ext|@FileList.Ext]",
        " /i  Display current soundcard settings",
    ]
    assert "Config file not found" not in original_out
    assert "DOS .exe stack pointer after end of program memory" not in original_out


def test_iplaydiag_help_advertises_supported_video_modes() -> None:
    result = run_dos(IPLAYDIAG_EXE, "/?")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert "--video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50" in out
    assert_text_backend(out)


def test_original_iplay_minalloc_patched_copy_with_adlib_config_prints_sound_settings(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    result = run_dos(patched, "/i", timeout=10, cwd=tmp_path)
    out = combined_output(result)

    assert result.returncode == 0, out
    assert "Current Soundcard settings:" in out
    assert "Adlib SoundCard, mixed at 44kHz" in out
    assert "Config file not found" not in out
    assert "offset overflow in print" not in out


def test_original_iplay_minalloc_patched_copy_with_adlib_config_reports_missing_module(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    result = run_dos(patched, "MISSING.MOD", timeout=10, cwd=tmp_path)
    out = combined_output(result)

    assert result.returncode == 2, out
    assert "Module not found." in out
    assert "Config file not found" not in out
    assert "offset overflow in print" not in out


def test_original_iplay_minalloc_patched_copy_with_existing_corrupt_mod_enters_ui_path(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    (tmp_path / "BAD.MOD").write_bytes(b"not a module")
    result = run_dos(patched, "BAD.MOD", timeout=10, cwd=tmp_path)
    out = combined_output(result)

    assert result.returncode == 0, out
    assert out == ""
    assert "Module not found." not in out
    assert "Config file not found" not in out
    assert "offset overflow in print" not in out


def test_original_iplay_ui_path_does_not_produce_hlt_memory_dump_for_b800_capture(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    (tmp_path / "BAD.MOD").write_bytes(b"not a module")
    dump = tmp_path / "mem.dmp"
    result = subprocess.run(
        ["timeout", "-k", "1", "3", str(KVIKDOS), f"--hlt-dump={dump}", str(patched), "BAD.MOD"],
        cwd=tmp_path,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    out = combined_output(result)

    assert result.returncode == 0, out
    assert out == ""
    assert not dump.exists()


def test_original_iplay_forced_hlt_dump_captures_b800_text_aperture(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original_with_hlt_trap(
        tmp_path / "IPLAY.EXE",
        ORIGINAL_CLEANUP_EXIT_HLT_TRAP_FILE_OFFSET,
    )
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    (tmp_path / "BAD.MOD").write_bytes(b"not a module")
    dump = tmp_path / "mem.dmp"
    result = subprocess.run(
        ["timeout", "-k", "1", "5", str(KVIKDOS), "--hlt-ok", f"--hlt-dump={dump}", str(patched), "BAD.MOD"],
        cwd=tmp_path,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    out = combined_output(result)
    vga_text_linear = VGA_COLOR_TEXT_SEG * 16 + VGA_TEXT_OFFSET
    memory = dump.read_bytes() if dump.exists() else b""
    original_cells = text_memory_slice(memory, VGA_COLOR_TEXT_SEG, 80, 25) if dump.exists() else b""
    digest = text_memory_digest(memory, VGA_COLOR_TEXT_SEG, 80, 25)
    original_text = text_memory_visible_text(memory, VGA_COLOR_TEXT_SEG, 80, 25)
    (BUILD_DIR / "BAD.MOD").write_bytes((tmp_path / "BAD.MOD").read_bytes())
    rewrite_dump = tmp_path / "rewrite-b800.dmp"
    rewrite_env = os.environ.copy()
    rewrite_env["KVIKDOS_MEM_DUMP"] = str(rewrite_dump)
    rewrite_env["KVIKDOS_MEM_DUMP_START"] = hex(vga_text_linear)
    rewrite_env["KVIKDOS_MEM_DUMP_SIZE"] = str(4000)
    rewrite = subprocess.run(
        ["timeout", "-k", "1", "5", str(KVIKDOS), str(IPLAYDIAG_EXE), "BAD.MOD"],
        cwd=BUILD_DIR,
        env=rewrite_env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    rewrite_out = combined_output(rewrite)
    rewrite_screen = parse_screen_present_digest(rewrite_out, "unsupported-module")
    rewrite_memory = rewrite_dump.read_bytes() if rewrite_dump.exists() else b""
    rewrite_digest = text_cell_digest(rewrite_memory)
    rewrite_text = text_cells_visible_text(rewrite_memory, 80, 25)

    assert result.returncode == 124, out
    assert dump.exists(), out
    assert dump.stat().st_size >= vga_text_linear + digest["bytes"]
    assert digest["bytes"] == 4000
    assert digest["checksum"] != 2166136261
    assert digest["nonblank"] > 0
    assert "offset overflow in print" not in out
    assert "Inertia Player V1.22" in original_text
    assert "Sound Solutions" in original_text
    assert "BAD.MOD" in original_text
    assert "Filename      : BAD.MOD" in original_text
    assert "Module Type   : N.T." in original_text
    assert rewrite.returncode == 2, rewrite_out
    assert_unsupported_module(rewrite_out, "BAD.MOD")
    assert rewrite_dump.exists(), rewrite_out
    assert_screen_present_content(rewrite_screen, "full-screen")
    assert rewrite_screen["bytes"] == digest["bytes"] == 4000
    assert rewrite_digest["bytes"] == rewrite_screen["bytes"]
    assert rewrite_digest["checksum"] == rewrite_screen["checksum"]
    assert rewrite_digest["nonblank"] == rewrite_screen["nonblank"]
    assert rewrite_screen["cols"] == 80
    assert rewrite_screen["rows"] == 25
    assert rewrite_screen["mode_ok"] == 1
    assert rewrite_screen["nonblank"] > 0
    assert "Inertia Player V1.22" in rewrite_text
    assert "Sound Solutions" in rewrite_text
    assert "BAD.MOD" in rewrite_text
    assert "Filename      : BAD.MOD" in rewrite_text
    assert "Module Type   : N.T." in rewrite_text
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Inertia Player V1.22")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "CD Edition")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Copyright (c) 1994,1995 by Stefan Danes")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Sound Solutions")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Hope you liked using the Inertia Player")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "If you have bug-reports, suggestions or comments send a message to:")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Internet : sdanes@marvels.hacktic.nl")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "FidoNet  : 2:284/116.8")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Send email to listserver@oliver.sun.ac.za to subscribe to one or both of")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "the Inertia Mailinglists and write following text in your message:")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "To connect to Binary Inertia releases: subscribe inertia-list YourRealName")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "To connect to Discussion Mailing list: subscribe inertia-talk YourRealName")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Filename      : BAD.MOD")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Playing in Stereo, Free: 482KB")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Module Type   : N.T.")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "ProTracker 1.0           F-9")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Channels      : 2")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Ignore BPM changes       F-10")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Samples Used  : 0/15")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Loop Module when done    F-11")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Current Track : 1/0")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "24bit Interpolation      F-12")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Track Position: 1/64")
    assert_text_cell_span_equal(original_cells, rewrite_memory, 80, 25, "Main Volume   :  100%      - +")
    assert_original_visible_row_cells_equal(original_cells, rewrite_memory, 80, 6)
    for row in range(8, 11):
        assert_original_visible_row_cells_equal(original_cells, rewrite_memory, 80, row)
    for row in range(13, 17):
        assert_original_visible_row_cells_equal(original_cells, rewrite_memory, 80, row)
    for row in range(19, 25):
        assert_original_visible_row_cells_equal(original_cells, rewrite_memory, 80, row)
    for row in range(25):
        assert_original_visible_row_cells_equal(original_cells, rewrite_memory, 80, row)
    assert_no_extra_rewrite_visible_text_on_original_blank_cells(original_cells, rewrite_memory, 80, 25)
    assert rewrite_memory == original_cells


def test_original_and_rewrite_live_mod_player_frame_share_text_cells_and_attributes(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original_with_opcode_hlt_trap(
        tmp_path / "IPLAY.EXE",
        ORIGINAL_PLAYBACK_UI_HLT_TRAP_FILE_OFFSET,
        0xC3,
    )
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    write_smoke_modules(tmp_path)
    original_dump = tmp_path / "original-live-b800.dmp"
    original = subprocess.run(
        [
            "timeout",
            "-k",
            "1",
            "5",
            str(KVIKDOS),
            "--hlt-ok",
            f"--hlt-dump={original_dump}",
            str(patched),
            "SMOKE.MOD",
        ],
        cwd=tmp_path,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    original_out = combined_output(original)
    original_memory = original_dump.read_bytes() if original_dump.exists() else b""
    original_cells = (
        text_memory_slice(original_memory, VGA_COLOR_TEXT_SEG, 80, 25)
        if original_dump.exists()
        else b""
    )
    original_text = text_cells_visible_text(original_cells, 80, 25) if original_cells else ""

    (BUILD_DIR / "SMOKE.MOD").write_bytes((tmp_path / "SMOKE.MOD").read_bytes())
    rewrite_dump = tmp_path / "rewrite-live-b800.dmp"
    rewrite_env = os.environ.copy()
    rewrite_env["KVIKDOS_MEM_DUMP"] = str(rewrite_dump)
    rewrite_env["KVIKDOS_MEM_DUMP_START"] = hex(dos_physical_address(VGA_COLOR_TEXT_SEG))
    rewrite_env["KVIKDOS_MEM_DUMP_SIZE"] = str(text_mode_byte_count(80, 25))
    rewrite = subprocess.run(
        [
            "timeout",
            "-k",
            "1",
            "5",
            str(KVIKDOS),
            str(IPLAYDIAG_EXE),
            "--blocks=1",
            "SMOKE.MOD",
        ],
        cwd=BUILD_DIR,
        env=rewrite_env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    rewrite_out = combined_output(rewrite)
    rewrite_cells = rewrite_dump.read_bytes() if rewrite_dump.exists() else b""
    rewrite_text = text_cells_visible_text(rewrite_cells, 80, 25) if rewrite_cells else ""

    assert original.returncode == 124, original_out
    assert original_dump.exists(), original_out
    assert len(original_cells) == text_mode_byte_count(80, 25)
    assert "offset overflow in print" not in original_out
    assert "Inertia Player V1.22" in original_text
    assert "Filename      : SMOKE.MOD" in original_text
    assert rewrite.returncode == 0, rewrite_out
    assert rewrite_dump.exists(), rewrite_out
    assert len(rewrite_cells) == len(original_cells)
    for expected in [
        "Inertia Player V1.22",
        "CD Edition",
        "Filename      : SMOKE.MOD",
        "Playing in Stereo, Free: 462KB",
        "Module Type   : M.K.",
        "Channels      : 4",
        "Samples Used  : 2/31",
        "SMOKE MOD",
        "ProTracker 1.0           F-9",
        "Ignore BPM changes       F-10",
        "Loop Module when done    F-11",
        "24bit Interpolation      F-12",
        "Main Volume   :",
    ]:
        try:
            assert_text_cell_span_at_original_location_equal(original_cells, rewrite_cells, 80, 25, expected)
        except AssertionError as error:
            raise AssertionError(f"live MOD field mismatch for {expected!r}: {error}") from error
    assert rewrite_text.count("Filename      : SMOKE.MOD") == 1


def test_original_and_rewrite_dynamic_mod_frame_share_track_and_channel_meter_geometry(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original_with_dynamic_ui_hlt_trap(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    write_smoke_modules(tmp_path)
    original_dump = tmp_path / "original-dynamic-b800.dmp"
    original = subprocess.run(
        [
            "timeout",
            "-k",
            "1",
            "5",
            str(KVIKDOS),
            "--hlt-ok",
            f"--hlt-dump={original_dump}",
            str(patched),
            "SMOKE.MOD",
        ],
        cwd=tmp_path,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    original_out = combined_output(original)
    original_memory = original_dump.read_bytes() if original_dump.exists() else b""
    original_cells = (
        text_memory_slice(original_memory, VGA_COLOR_TEXT_SEG, 80, 25)
        if original_dump.exists()
        else b""
    )

    (BUILD_DIR / "SMOKE.MOD").write_bytes((tmp_path / "SMOKE.MOD").read_bytes())
    rewrite_dump = tmp_path / "rewrite-dynamic-b800.dmp"
    rewrite_env = os.environ.copy()
    rewrite_env["KVIKDOS_MEM_DUMP"] = str(rewrite_dump)
    rewrite_env["KVIKDOS_MEM_DUMP_START"] = hex(dos_physical_address(VGA_COLOR_TEXT_SEG))
    rewrite_env["KVIKDOS_MEM_DUMP_SIZE"] = str(text_mode_byte_count(80, 25))
    rewrite = subprocess.run(
        [
            "timeout",
            "-k",
            "1",
            "5",
            str(KVIKDOS),
            str(IPLAYDIAG_EXE),
            "--blocks=1",
            "SMOKE.MOD",
        ],
        cwd=BUILD_DIR,
        env=rewrite_env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    rewrite_out = combined_output(rewrite)
    rewrite_cells = rewrite_dump.read_bytes() if rewrite_dump.exists() else b""

    assert original.returncode == 124, original_out
    assert original_dump.exists(), original_out
    assert len(original_cells) == text_mode_byte_count(80, 25)
    assert rewrite.returncode == 0, rewrite_out
    assert rewrite_dump.exists(), rewrite_out
    assert len(rewrite_cells) == len(original_cells)
    for expected in ("Current Track : 1/5", "Track Position: 1/64"):
        assert_text_cell_span_at_original_location_equal(original_cells, rewrite_cells, 80, 25, expected)
    for row in range(6, 10):
        start = (row * 80) * 2
        assert rewrite_cells[start:start + 10] == original_cells[start:start + 10]
        original_meter_chars = original_cells[start + 64:start + 124:2]
        rewrite_meter_chars = rewrite_cells[start + 64:start + 124:2]
        assert rewrite_meter_chars == original_meter_chars == bytes([0x16]) * 30
    for row in (5, 10, 11, 12, 13, 14, 15, 16, 17):
        start = (row * 80) * 2
        end = start + 80 * 2
        assert rewrite_cells[start:end] == original_cells[start:end]
    for row in range(6, 10):
        start = (row * 80) * 2
        end = start + 80 * 2
        assert rewrite_cells[start:end:2] == original_cells[start:end:2]
        assert rewrite_cells[start + 126:start + 156] == original_cells[start + 126:start + 156]


def test_original_f4_sample_window_has_nine_row_80x25_page(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original_with_f4_draw_hlt_trap(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    write_smoke_modules(tmp_path)
    module = bytearray((tmp_path / "SMOKE.MOD").read_bytes())
    module[20:42] = b"KICK" + b" " * 18
    module[50:72] = b"SNARE" + b" " * 17
    (tmp_path / "SMOKE.MOD").write_bytes(module)
    dump = tmp_path / "original-f4-b800.dmp"
    original = subprocess.run(
        [
            "timeout",
            "-k",
            "1",
            "5",
            str(KVIKDOS),
            "--hlt-ok",
            f"--hlt-dump={dump}",
            str(patched),
            "SMOKE.MOD",
        ],
        cwd=tmp_path,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    out = combined_output(original)
    assert original.returncode == 124, out
    assert dump.exists(), out
    cells = text_memory_slice(dump.read_bytes(), VGA_COLOR_TEXT_SEG, 80, 25)

    def row_text(row: int) -> str:
        start = row * 80 * 2
        return bytes(cells[start:start + 80 * 2:2]).decode("cp437")

    assert row_text(6)[3:78] == "# SampleName   Press F-4 for more   Size Vol Mode  C-2 Tune LoopPos LoopEnd"
    assert row_text(7)[2:9] == " 1 KICK"
    assert row_text(8)[2:10] == " 2 SNARE"
    assert row_text(15)[2:4] == " 9"
    assert bytes(cells[((6 * 80 + x) * 2) + 1] for x in range(3, 18)) == bytes([0x7E]) * 15
    assert bytes(cells[((6 * 80 + x) * 2) + 1] for x in range(18, 36)) == bytes([0x78]) * 18
    assert bytes(cells[((6 * 80 + x) * 2) + 1] for x in range(36, 78)) == bytes([0x7E]) * 42
    assert bytes(cells[((7 * 80 + x) * 2) + 1] for x in range(2, 5)) == bytes([0x7F]) * 3
    assert bytes(cells[((7 * 80 + x) * 2) + 1] for x in range(5, 37)) == bytes([0x7B]) * 32
    assert not row_text(16)[2:].startswith("10 ")


def test_original_f1_help_window_has_two_column_80x25_layout(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original_with_f1_draw_hlt_trap(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    write_smoke_modules(tmp_path)
    dump = tmp_path / "original-f1-b800.dmp"
    original = subprocess.run(
        [
            "timeout",
            "-k",
            "1",
            "5",
            str(KVIKDOS),
            "--hlt-ok",
            f"--hlt-dump={dump}",
            str(patched),
            "SMOKE.MOD",
        ],
        cwd=tmp_path,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    out = combined_output(original)
    assert original.returncode == 124, out
    assert dump.exists(), out
    cells = text_memory_slice(dump.read_bytes(), VGA_COLOR_TEXT_SEG, 80, 25)

    def row_bytes(row: int) -> bytes:
        start = row * 80 * 2
        return bytes(cells[start:start + 80 * 2:2])

    assert row_bytes(6)[28:52] == b"So you wanted some help?"
    assert row_bytes(7)[4:47] == b"F-2  Graphical scopes, one for each channel"
    assert row_bytes(7)[50:74] == b"Gray - +  Dec/Inc volume"
    assert row_bytes(9)[4:43] == b"F-4  View sample names (twice for more)"
    assert row_bytes(9)[48:58] == b"Cursor \x1A \x18"
    assert row_bytes(10)[48:58] == b"Cursor \x1B \x19"
    assert row_bytes(12)[48:72] == b"ScrollLock  Loop pattern"
    assert row_bytes(15)[4:35] == b"F-12 Toggle 24bit Interpolation"
    assert row_bytes(15)[55:75] == b"Tab  Toggle PAL/NTSC"
    assert bytes(cells[((7 * 80 + x) * 2) + 1] for x in range(4, 7)) == bytes([0x7F]) * 3
    assert bytes(cells[((7 * 80 + x) * 2) + 1] for x in range(7, 47)) == bytes([0x7E]) * 40


def test_original_f6_panning_window_has_17_position_signed_pan_rows(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original_with_f6_draw_hlt_trap(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    write_smoke_modules(tmp_path)
    dump = tmp_path / "original-f6-b800.dmp"
    original = subprocess.run(
        [
            "timeout",
            "-k",
            "1",
            "5",
            str(KVIKDOS),
            "--hlt-ok",
            f"--hlt-dump={dump}",
            str(patched),
            "SMOKE.MOD",
        ],
        cwd=tmp_path,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    out = combined_output(original)
    assert original.returncode == 124, out
    assert dump.exists(), out
    cells = text_memory_slice(dump.read_bytes(), VGA_COLOR_TEXT_SEG, 80, 25)

    def row_bytes(row: int) -> bytes:
        start = row * 80 * 2
        return bytes(cells[start:start + 80 * 2:2])

    assert row_bytes(6)[2:28] == b" 1   L" + bytes([0xC4]) * 16 + b" -64"
    assert row_bytes(7)[2:28] == b" 2   " + bytes([0xC4]) * 16 + b"R  64"
    assert row_bytes(8)[2:28] == b" 3   " + bytes([0xC4]) * 16 + b"R  64"
    assert row_bytes(9)[2:28] == b" 4   L" + bytes([0xC4]) * 16 + b" -64"
    assert bytes(cells[((6 * 80 + x) * 2) + 1] for x in range(2, 5)) == bytes([0x1E]) * 3
    assert bytes(cells[((7 * 80 + x) * 2) + 1] for x in range(2, 5)) == bytes([0x7E]) * 3
    assert bytes(cells[((6 * 80 + x) * 2) + 1] for x in range(5, 28)) == bytes([0x7E]) * 23


def test_original_f3_zero_level_view_is_the_live_channel_panel(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original_with_f3_draw_hlt_trap(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)
    write_smoke_modules(tmp_path)
    dump = tmp_path / "original-f3-b800.dmp"
    original = subprocess.run(
        [
            "timeout",
            "-k",
            "1",
            "5",
            str(KVIKDOS),
            "--hlt-ok",
            f"--hlt-dump={dump}",
            str(patched),
            "SMOKE.MOD",
        ],
        cwd=tmp_path,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    out = combined_output(original)
    assert original.returncode == 124, out
    assert dump.exists(), out
    cells = text_memory_slice(dump.read_bytes(), VGA_COLOR_TEXT_SEG, 80, 25)

    def row_bytes(row: int) -> bytes:
        start = row * 80 * 2
        return bytes(cells[start:start + 80 * 2:2])

    assert row_bytes(6)[2:9] == b" 1     "
    assert row_bytes(7)[2:9] == b" 2 C#1 "
    assert row_bytes(8)[2:9] == b" 3 C#1 "
    assert row_bytes(9)[2:9] == b" 4 D-1 "
    for row in range(6, 10):
        assert row_bytes(row)[32:62] == bytes([0x16]) * 30
    assert row_bytes(7)[63:76] == b"Set Speed/BPM"
    assert row_bytes(6)[63:76] == b"Volume Change"


def test_original_iplay_minalloc_patched_copy_with_sb16_config_hits_kvikdos_print_blocker(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x03)
    result = run_dos(patched, "/i", timeout=10, cwd=tmp_path)
    out = combined_output(result)

    assert result.returncode != 0, out
    assert "fatal: !! offset overflow in print" in out
    assert "Config file not found" not in out


def test_iplayc_dos_startup_without_module_reports_supported_scope() -> None:
    result = run_dos(IPLAYDIAG_EXE)
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_help_usage(out)
    assert " /i  Display current soundcard settings" in out
    assert "Text backend: VGA color/BW text memory" in out
    assert_supported_dos_formats(out)
    assert_sb16_audio_scope(out)
    assert_text_backend_memory(out)
    assert_sdl_compatible_audio_backend(out)
    for legacy_driver in ("GUS", "Gravis", "AdLib", "MIDI", "PAS", "WSS"):
        assert legacy_driver not in out
    for misleading_text_backend in ("ncurses", "80x25 only", "fixed 80x25"):
        assert misleading_text_backend not in out.lower()


@pytest.mark.parametrize("switch", ["/0", "-?", "--help"])
def test_iplayc_dos_help_aliases_report_supported_scope(switch: str) -> None:
    result = run_dos(IPLAYDIAG_EXE, switch)
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_help_usage(out)
    assert " /?  Display this help" in out
    assert " /i  Display current soundcard settings" in out
    assert_supported_dos_formats(out)
    assert "Module not found." not in out


def test_iplayc_dos_sound_settings_option_reports_sb16_scope() -> None:
    result = run_dos(IPLAYDIAG_EXE, "/i")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert "Current Soundcard settings:" in out
    assert "Sound Blaster 16/16ASP, mixed at 44kHz" in out
    assert "220h, IRQ 5, DMA 5" in out
    assert "Module not found." not in out


def test_iplayc_dos_sound_settings_option_accepts_uppercase_switch() -> None:
    result = run_dos(IPLAYDIAG_EXE, "/I")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert "Current Soundcard settings:" in out
    assert "Sound Blaster 16/16ASP, mixed at 44kHz" in out
    assert "220h, IRQ 5, DMA 5" in out
    assert "Module not found." not in out


def test_iplayc_dos_file_list_argument_runs_first_module() -> None:
    (BUILD_DIR / "PLAYLIST.TXT").write_text("SMOKE.S3M\r\n", encoding="ascii")
    result = run_dos(IPLAYDIAG_EXE, "@PLAYLIST.TXT")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "SMOKE.S3M")
    assert_module_loader(out, "s3m_module (Scream Tracker 3)")
    assert_bounded_sb16_playback(out)
    assert "Module not found." not in out


def test_iplayc_dos_file_list_argument_trims_whitespace_around_first_module() -> None:
    (BUILD_DIR / "PLAYTRIM.TXT").write_text("\r\n\t SMOKE.S3M \t\r\nBAD.XM\r\n", encoding="ascii")
    result = run_dos(IPLAYDIAG_EXE, "--blocks=1", "@PLAYTRIM.TXT")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "SMOKE.S3M")
    assert_module_loader(out, "s3m_module (Scream Tracker 3)")
    pump = parse_playback_pump(out)
    assert_playback_pump_sb16_stereo(pump, 1, SB16_BOUNDED_BLOCK_FRAMES)
    assert_playback_pump_stop_state(pump, 1, 0, "block-limit")
    assert "BAD.XM" not in out
    assert "Module not found." not in out


def test_shared_smoke_fixtures_cover_all_advertised_supported_formats(tmp_path: Path) -> None:
    result = run_dos(IPLAYDIAG_EXE)
    out = combined_output(result)
    write_smoke_modules(tmp_path)
    advertised = advertised_supported_formats(out)
    generated = {path.suffix[1:].upper() for path in tmp_path.iterdir() if path.is_file()}

    assert result.returncode == 0, out
    assert advertised <= generated


def test_dos_behavior_matrix_covers_all_advertised_supported_formats() -> None:
    result = run_dos(IPLAYDIAG_EXE)
    out = combined_output(result)
    advertised = advertised_supported_formats(out)
    behavior_formats = {
        Path(filename).suffix[1:].upper()
        for filename, _loader, _title, _extra_lines in DOS_FIXTURE_METADATA_CASES
        if Path(filename).suffix[1:].upper() not in DEFERRED_PROJECT_OWNED_FORMATS
    }

    assert result.returncode == 0, out
    assert behavior_formats == advertised


def test_iplayc_dos_missing_module_matches_original_missing_module_failure() -> None:
    result = run_dos(IPLAYDIAG_EXE, "MISSING.S3M")
    out = combined_output(result)

    assert result.returncode == 2, out
    assert "Module not found." in out
    assert "Cannot open module" not in out


@pytest.mark.parametrize("mode", ["40x25bw", "40x25color", "80x25bw", "80x25color", "80x50"])
def test_iplaydiag_missing_module_does_not_present_stale_screen_or_playback_for_supported_modes(mode: str) -> None:
    result = run_dos(IPLAYDIAG_EXE, "--blocks=1", f"--video-mode={mode}", "MISSINGMODE.S3M")
    out = combined_output(result)

    assert result.returncode == 2, out
    assert "Module not found." in out
    assert "Cannot open module" not in out
    assert_module_not_loaded(out, "MISSINGMODE.S3M")
    assert "Screen present:" not in out
    assert "PCM source:" not in out
    assert "Decoder route:" not in out
    assert "Decoder handoff:" not in out
    assert "Playback pump:" not in out


def test_iplayc_dos_missing_mod_matches_patched_original_missing_module_behavior(tmp_path: Path) -> None:
    patched = write_minalloc_patched_original(tmp_path / "IPLAY.EXE")
    write_valid_original_config(tmp_path / "IPLAY.CFG", sound_card_type=0x08)

    original = run_dos(patched, "MISSING.MOD", timeout=10, cwd=tmp_path)
    rewrite = run_dos(IPLAYDIAG_EXE, "MISSING.MOD")
    original_out = combined_output(original)
    rewrite_out = combined_output(rewrite)

    assert original.returncode == rewrite.returncode == 2
    assert original_out.strip() == rewrite_out.strip() == "Module not found."
    assert "Config file not found" not in original_out
    assert "offset overflow in print" not in original_out
    assert "Cannot open module" not in rewrite_out


def test_iplayc_dos_unsupported_module_reports_unsupported_type() -> None:
    result = run_dos(IPLAYDIAG_EXE, "BAD.XYZ")
    out = combined_output(result)

    assert result.returncode == 2, out
    assert_unsupported_module(out, "BAD.XYZ")


def test_iplayc_dos_existing_corrupt_mod_is_rejected_before_playback() -> None:
    (BUILD_DIR / "BAD.MOD").write_bytes(b"not a module")
    result = run_dos(IPLAYDIAG_EXE, "BAD.MOD")
    out = combined_output(result)
    unsupported_screen = parse_screen_present_digest(out, "unsupported-module")

    assert result.returncode == 2, out
    assert_unsupported_module(out, "BAD.MOD")
    assert_module_not_loaded(out, "BAD.MOD")
    assert_screen_present_content(unsupported_screen, "full-screen")
    assert unsupported_screen["bytes"] == 4000
    assert unsupported_screen["cols"] == 80
    assert unsupported_screen["rows"] == 25
    assert unsupported_screen["mode_ok"] == 1
    assert unsupported_screen["nonblank"] > 0
    assert "PCM source:" not in out
    assert "Playback pump:" not in out


def test_iplayc_dos_header_detection_takes_precedence_over_extension() -> None:
    (BUILD_DIR / "SMOKE.XYZ").write_bytes((BUILD_DIR / "SMOKE.S3M").read_bytes())
    result = run_dos(IPLAYDIAG_EXE, "SMOKE.XYZ")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "SMOKE.XYZ")
    assert_module_loader(out, "s3m_module (Scream Tracker 3)")
    assert_module_type_tag(out, "204D3353")
    assert_module_title(out, "SMOKE S3M")
    assert "Unsupported module type" not in out


@pytest.mark.parametrize(
    ("filename", "loader", "title", "extra_lines"),
    DOS_FIXTURE_METADATA_CASES,
)
def test_iplayc_dos_fixture_reports_loader_metadata(filename: str, loader: str, title: str, extra_lines: list[str]) -> None:
    result = run_dos(IPLAYDIAG_EXE, filename)
    out = combined_output(result)

    assert result.returncode == 0, out
    assert f"Module: {filename}" in out
    assert f"Loader: {loader}" in out
    assert title in out
    for line in extra_lines:
        assert line in out
    assert_playback_output(out, "SB16 16-bit stereo hardware wrapper enabled.")


@pytest.mark.parametrize(
    "filename",
    EXTERNAL_DECODER_FIXTURES,
)
def test_iplayc_dos_tracker_formats_stay_on_external_decoder_boundary(filename: str) -> None:
    result = run_dos(IPLAYDIAG_EXE, filename)
    out = combined_output(result)

    assert result.returncode == 0, out
    route = assert_decoder_route(out, 0, "external-library")
    assert_pcm_source_route(out, route["id"], "e", "native-preview", truncated=0, input_kind="memory", hook_provider="none")
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_decoder_route_absent(out, 1, "project-owned")
    assert_decoder_handoff_absent(out, "project INR -> SB16 PCM.")


def test_iplayc_dos_inr_stays_on_project_decoder_boundary() -> None:
    result = run_dos(IPLAYDIAG_EXE, "SMOKE.INR")
    out = combined_output(result)

    assert result.returncode == 0, out
    route = assert_decoder_route(out, 1, "project-owned")
    assert_pcm_source_route(out, route["id"], "p", "native", source="inr_module", truncated=0, input_kind="memory", hook_provider="none", stream_start=0)
    assert_decoder_handoff(out, "project INR -> SB16 PCM.")
    assert_decoder_route_absent(out, 0, "external-library")
    assert_decoder_handoff_absent(out, "external tracker -> SB16 PCM seam.")


def test_iplaydiag_dos_mod_fixture_reports_voice_mixing_and_speed_update() -> None:
    result = run_dos(IPLAYDIAG_EXE, "SMOKE.MOD")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_decoder_event(out, 855, 1, 1, 1, 64, 12, 127)
    assert_decoder_voice(out, 1, 855, 1, 1, 1, 64, 4, 64, 0, 2, 6204)
    assert_playback_loop(out, "playback", "bounded-trial", "immediate", 32, SB16_BOUNDED_BLOCK_FRAMES)
    assert_bounded_sb16_playback(out)
    assert_decoder_progress_block(out, 32, 7680)
    assert_decoder_voice(out, 0, 855, 1, 1, 1, 64, 4, 64, 0, 2, 6204)


def test_iplaydiag_dos_mod_sample_data_can_start_after_old_8k_header_window() -> None:
    mod = bytearray(1084 + 8 * 1024 + 4)
    mod[:8] = b"PAD8KMOD"
    mod[42:44] = (2).to_bytes(2, "big")
    mod[45] = 64
    mod[950] = 1
    mod[952] = 7
    mod[1080:1084] = b"M.K."
    mod[1084 + 7 * 1024:1084 + 7 * 1024 + 4] = bytes([0x03, 0x57, 0x1C, 0x40])
    sample_base = 1084 + 8 * 1024
    mod[sample_base:sample_base + 4] = bytes([5, 6, 7, 8])
    (BUILD_DIR / "PAD8K.MOD").write_bytes(mod)

    result = run_dos(IPLAYDIAG_EXE, "PAD8K.MOD")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "PAD8K.MOD")
    assert_module_size(out, 9280)
    assert_decoder_event(out, 855, 1, 1, 1, 64, 12, 64)
    assert_decoder_voice(out, 1, 855, 1, 1, 1, 64, 4, 64, 0, 0, 9276)
    assert_bounded_sb16_playback(out)


def test_iplayc_dos_mod_finetune_stays_on_external_stream_placeholder() -> None:
    def write_mod_finetune_fixture(name: str, finetune: int) -> None:
        mod = bytearray(1084 + 1024 + 0x300)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x180).to_bytes(2, "big")
        mod[44] = finetune & 0x0F
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10, 0x00])
        sample_base = 1084 + 1024
        for i in range(0x300):
            mod[sample_base + i] = (i * 117 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_finetune_fixture("FTBASE", 0)
    write_mod_finetune_fixture("FTSHARP", 7)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "FTBASE.MOD"))
    sharp = combined_output(run_dos(IPLAYDIAG_EXE, "FTSHARP.MOD"))

    assert_decoder_voice(base, 1, 855, 1, 1, 1, 48, 768, 48, 0, 0, 2108)
    assert_decoder_voice(sharp, 1, 855, 1, 1, 1, 48, 768, 48, 0, 0, 2108)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(sharp)
    assert_external_native_preview(base)
    assert_external_native_preview(sharp)


def test_iplayc_dos_mod_finetune_same_path_stays_on_external_stream_placeholder() -> None:
    def write_mod_finetune_same_path_fixture(name: str, finetune: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x300)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x180).to_bytes(2, "big")
        mod[44] = finetune & 0x0F
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10, 0x00])
        for i in range(0x300):
            mod[sample_base + i] = (i * 117 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_finetune_same_path_fixture("MFTUNE", 0)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MFTUNE.MOD"))

    write_mod_finetune_same_path_fixture("MFTUNE", 7)
    sharp = combined_output(run_dos(IPLAYDIAG_EXE, "MFTUNE.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(sharp, 1, 64, 0, 6, 125, 4)
    assert_decoder_voice(base, 1, 855, 1, 1, 1, 48, 768, 48, 0, 0, 2108)
    assert_decoder_voice(sharp, 1, 855, 1, 1, 1, 48, 768, 48, 0, 0, 2108)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(sharp)
    assert_external_native_preview(base)
    assert_external_native_preview(sharp)


def test_iplayc_dos_real_s3m_size_loads_past_old_near_buffer_limit() -> None:
    s3m = bytearray(20800)
    s3m[:8] = b"ARYXTEST"
    s3m[0x20:0x22] = (3).to_bytes(2, "little")
    s3m[0x22:0x24] = (1).to_bytes(2, "little")
    s3m[0x24:0x26] = (1).to_bytes(2, "little")
    s3m[0x2C:0x30] = b"SCRM"
    s3m[0x60:0x63] = bytes([0, 1, 2])
    (BUILD_DIR / "ARYXSIZE.S3M").write_bytes(s3m)

    result = run_dos(IPLAYDIAG_EXE, "ARYXSIZE.S3M")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "ARYXSIZE.S3M")
    assert_module_size(out, 20800)
    assert_external_pcm_source(out, "s3m_module", "native-preview", 0, "memory", 512)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_bounded_sb16_playback(out)
    assert "Module too large" not in out


def test_iplayc_dos_oversized_s3m_loads_with_capped_header_for_library_decoder_boundary() -> None:
    s3m = bytearray(24577)
    s3m[:8] = b"BIGS3M  "
    s3m[0x20:0x22] = (3).to_bytes(2, "little")
    s3m[0x22:0x24] = (1).to_bytes(2, "little")
    s3m[0x24:0x26] = (1).to_bytes(2, "little")
    s3m[0x2C:0x30] = b"SCRM"
    s3m[0x60:0x63] = bytes([0, 1, 2])
    s3m[0x100:] = bytes([0x35]) * (len(s3m) - 0x100)
    (BUILD_DIR / "BIG.S3M").write_bytes(s3m)

    result = run_dos(IPLAYDIAG_EXE, "BIG.S3M")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "BIG.S3M")
    assert_module_size(out, 24577)
    assert_external_pcm_source(out, "s3m_module", "dos-fallback", 1, "file-path", 107)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_bounded_sb16_playback(out)
    assert "Module too large" not in out


def test_iplayc_dos_modern_tracker_extension_stays_on_external_decoder_boundary() -> None:
    xm = bytearray(128)
    xm[:17] = b"Extended Module: "
    xm[17:25] = b"DOS XM  "
    (BUILD_DIR / "DOSXM.XM").write_bytes(xm)

    result = run_dos(IPLAYDIAG_EXE, "DOSXM.XM")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "DOSXM.XM")
    assert_module_loader(out, "external_module (FastTracker XM)")
    assert_module_type_tag(out, "20545845")
    assert_module_title(out, "DOS XM")
    assert_decoder_route(out, 0, "external-library")
    assert_external_pcm_source(out, "external_module", "native-preview", 0, "memory", 0)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")


def test_iplayc_dos_it_extension_reports_external_decoder_boundary_metadata() -> None:
    it = bytearray(128)
    it[:4] = b"IMPM"
    it[4:13] = b"DOS IT   "
    (BUILD_DIR / "DOSIT.IT").write_bytes(it)

    result = run_dos(IPLAYDIAG_EXE, "DOSIT.IT")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "DOSIT.IT")
    assert_module_loader(out, "external_module (Impulse Tracker IT)")
    assert_module_type_tag(out, "20545845")
    assert_module_title(out, "DOS IT")
    assert_decoder_route(out, 0, "external-library")
    assert_external_pcm_source(out, "external_module", "native-preview", 0, "memory", 0)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")


def test_iplayc_dos_capped_header_file_path_stream_uses_bytes_from_pattern_stream() -> None:
    left = bytearray(24577)
    right = bytearray(24577)
    for body, title, fill in ((left, b"PATHA   ", 0x21), (right, b"PATHB   ", 0x61)):
        body[:8] = title
        body[0x20:0x22] = (3).to_bytes(2, "little")
        body[0x22:0x24] = (1).to_bytes(2, "little")
        body[0x24:0x26] = (1).to_bytes(2, "little")
        body[0x2C:0x30] = b"SCRM"
        body[0x60:0x63] = bytes([0, 1, 2])
        body[0x107:] = bytes([fill]) * (len(body) - 0x107)
    (BUILD_DIR / "PATHA.S3M").write_bytes(left)
    (BUILD_DIR / "PATHB.S3M").write_bytes(right)

    result_a = run_dos(IPLAYDIAG_EXE, "PATHA.S3M")
    result_b = run_dos(IPLAYDIAG_EXE, "PATHB.S3M")
    out_a = combined_output(result_a)
    out_b = combined_output(result_b)

    assert result_a.returncode == 0, out_a
    assert result_b.returncode == 0, out_b
    assert_external_pcm_source(out_a, "s3m_module", "dos-fallback", 1, "file-path", 107)
    assert_external_pcm_source(out_b, "s3m_module", "dos-fallback", 1, "file-path", 107)
    assert playback_checksum(out_a) != playback_checksum(out_b)



def test_iplayc_dos_capped_header_s3m_same_path_stream_uses_byte_past_near_buffer() -> None:
    def write_capped_s3m(name: str, tail_byte: int) -> None:
        s3m = bytearray(28672)
        s3m[:8] = b"S3MPATH "
        s3m[0x20:0x22] = (3).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x60:0x63] = bytes([0, 1, 2])
        s3m[24576:] = bytes([tail_byte & 0xFF]) * (len(s3m) - 24576)
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_capped_s3m("S3MPATH", 0x23)
    first = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPATH.S3M", timeout=8))

    write_capped_s3m("S3MPATH", 0x63)
    second = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPATH.S3M", timeout=8))

    assert_module_loaded(first, "S3MPATH.S3M")
    assert_module_loaded(second, "S3MPATH.S3M")
    assert_module_size(first, 28672)
    assert_module_size(second, 28672)
    assert_external_pcm_source(first, "s3m_module", "dos-fallback", 1, "file-path", 107)
    assert_external_pcm_source(second, "s3m_module", "dos-fallback", 1, "file-path", 107)
    assert_bounded_sb16_playback_blocks(first, 64)
    assert_bounded_sb16_playback_blocks(second, 64)
    assert playback_checksum(first) != playback_checksum(second)

def test_iplayc_dos_oversized_mtm_loads_with_capped_header_for_library_decoder_boundary() -> None:
    mtm = bytearray(24577)
    mtm[0:3] = b"MTM"
    mtm[4:13] = b"BIG MTM  "
    mtm[0x1A:0x1C] = (12).to_bytes(2, "little")
    mtm[0x1C] = 2
    mtm[0x1E] = 4
    mtm[0x20] = 6
    mtm[0x22:0x27] = bytes([0, 1, 2, 3, 4])
    mtm[0x100:] = bytes([0x47]) * (len(mtm) - 0x100)
    (BUILD_DIR / "BIG.MTM").write_bytes(mtm)

    result = run_dos(IPLAYDIAG_EXE, "BIG.MTM")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "BIG.MTM")
    assert_module_size(out, 24577)
    assert_module_loader(out, "mtm_module (MultiTracker MTM)")
    assert_external_pcm_source(out, "mtm_module", "dos-fallback", 1, "file-path", 66)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_bounded_sb16_playback(out)
    assert "Module too large" not in out



def test_iplayc_dos_oversized_stm_loads_with_capped_header_for_library_decoder_boundary() -> None:
    stm = bytearray(24577)
    stm[:9] = b"BIG STM  "
    stm[20:28] = b"!Scream!"
    stm[0x100:] = bytes([0x59]) * (len(stm) - 0x100)
    (BUILD_DIR / "BIG.STM").write_bytes(stm)

    result = run_dos(IPLAYDIAG_EXE, "BIG.STM")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "BIG.STM")
    assert_module_size(out, 24577)
    assert_module_loader(out, "_2stm_module (Scream Tracker 2 STM)")
    assert_external_pcm_source(out, "_2stm_module", "dos-fallback", 1, "file-path", 64)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_bounded_sb16_playback(out)
    assert "Module too large" not in out


def test_iplayc_dos_oversized_far_loads_with_capped_header_for_library_decoder_boundary() -> None:
    far = bytearray(24577)
    far[0:4] = b"FAR\xfe"
    far[4:13] = b"BIG FAR  "
    far[0x100:] = bytes([0x6B]) * (len(far) - 0x100)
    (BUILD_DIR / "BIG.FAR").write_bytes(far)

    result = run_dos(IPLAYDIAG_EXE, "BIG.FAR")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "BIG.FAR")
    assert_module_size(out, 24577)
    assert_module_loader(out, "far_module (Farandole FAR)")
    assert_external_pcm_source(out, "far_module", "dos-fallback", 1, "file-path", 128)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_bounded_sb16_playback(out)
    assert "Module too large" not in out


def test_iplayc_dos_oversized_669_loads_with_capped_header_for_library_decoder_boundary() -> None:
    e669 = bytearray(24577)
    e669[0:2] = b"if"
    e669[2:11] = b"BIG669   "
    e669[0x6E] = 4
    e669[0x6F] = 3
    e669[0x70] = 1
    e669[0x100:] = bytes([0x37]) * (len(e669) - 0x100)
    (BUILD_DIR / "BIG.669").write_bytes(e669)

    result = run_dos(IPLAYDIAG_EXE, "BIG.669")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "BIG.669")
    assert_module_size(out, 24577)
    assert_module_loader(out, "e669_module (Composer 669)")
    assert_external_pcm_source(out, "e669_module", "dos-fallback", 1, "file-path", 113)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_bounded_sb16_playback(out)
    assert "Module too large" not in out


def test_iplayc_dos_oversized_psm_loads_with_capped_header_for_library_decoder_boundary() -> None:
    psm = bytearray(24577)
    psm[0:4] = b"PSM "
    psm[4:13] = b"BIG PSM  "
    psm[0x100:] = bytes([0x4D]) * (len(psm) - 0x100)
    (BUILD_DIR / "BIG.PSM").write_bytes(psm)

    result = run_dos(IPLAYDIAG_EXE, "BIG.PSM")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "BIG.PSM")
    assert_module_size(out, 24577)
    assert_module_loader(out, "psm_module (ProTracker Studio PSM)")
    assert_external_pcm_source(out, "psm_module", "dos-fallback", 1, "file-path", 128)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_bounded_sb16_playback(out)
    assert "Module too large" not in out


def test_iplayc_dos_oversized_ult_loads_with_capped_header_for_library_decoder_boundary() -> None:
    ult = bytearray(24577)
    ult[:15] = b"MAS_UTrack_V001"
    ult[15:24] = b"BIG ULT  "
    ult[0x100:] = bytes([0x71]) * (len(ult) - 0x100)
    (BUILD_DIR / "BIG.ULT").write_bytes(ult)

    result = run_dos(IPLAYDIAG_EXE, "BIG.ULT")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "BIG.ULT")
    assert_module_size(out, 24577)
    assert_module_loader(out, "ult_module (UltraTracker ULT)")
    assert_external_pcm_source(out, "ult_module", "dos-fallback", 1, "file-path", 96)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_bounded_sb16_playback(out)
    assert "Module too large" not in out


def test_iplayc_dos_oversized_nst_loads_with_capped_header_for_library_decoder_boundary() -> None:
    nst = bytearray(24577)
    nst[:8] = b"BIGNST  "
    nst[950] = 1
    nst[1080:1084] = b"M.K."
    nst[0x1000:] = bytes([0x5A]) * (len(nst) - 0x1000)
    (BUILD_DIR / "BIG.NST").write_bytes(nst)

    result = run_dos(IPLAYDIAG_EXE, "BIG.NST")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "BIG.NST")
    assert_module_size(out, 24577)
    assert_module_loader(out, "mod_n_t_module (ProTracker/NoiseTracker MOD)")
    assert_external_pcm_source(out, "mod_n_t_module", "dos-fallback", 1, "file-path", 1084)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_bounded_sb16_playback(out)
    assert "Module too large" not in out


def test_iplayc_dos_capped_header_nst_same_path_stream_uses_byte_past_near_buffer() -> None:
    def write_capped_nst(name: str, tail_byte: int) -> None:
        nst = bytearray(28672)
        nst[:8] = b"NSTPATH "
        nst[950] = 1
        nst[1080:1084] = b"M.K."
        nst[24576:] = bytes([tail_byte & 0xFF]) * (len(nst) - 24576)
        (BUILD_DIR / f"{name}.NST").write_bytes(nst)

    write_capped_nst("NSTPATH", 0x21)
    first = combined_output(run_dos(IPLAYDIAG_EXE, "NSTPATH.NST", timeout=8))

    write_capped_nst("NSTPATH", 0x61)
    second = combined_output(run_dos(IPLAYDIAG_EXE, "NSTPATH.NST", timeout=8))

    assert_module_loaded(first, "NSTPATH.NST")
    assert_module_loaded(second, "NSTPATH.NST")
    assert_module_size(first, 28672)
    assert_module_size(second, 28672)
    assert_external_pcm_source(first, "mod_n_t_module", "dos-fallback", 1, "file-path", 1084)
    assert_external_pcm_source(second, "mod_n_t_module", "dos-fallback", 1, "file-path", 1084)
    assert_bounded_sb16_playback_blocks(first, 64)
    assert_bounded_sb16_playback_blocks(second, 64)
    assert playback_checksum(first) != playback_checksum(second)


def test_iplayc_dos_capped_header_psm_same_path_stream_uses_byte_past_near_buffer() -> None:
    def write_capped_psm(name: str, tail_byte: int) -> None:
        psm = bytearray(28672)
        psm[0:4] = b"PSM "
        psm[4:13] = b"PSMPATH  "
        psm[24576:] = bytes([tail_byte & 0xFF]) * (len(psm) - 24576)
        (BUILD_DIR / f"{name}.PSM").write_bytes(psm)

    write_capped_psm("PSMPATH", 0x25)
    first = combined_output(run_dos(IPLAYDIAG_EXE, "PSMPATH.PSM", timeout=8))

    write_capped_psm("PSMPATH", 0x65)
    second = combined_output(run_dos(IPLAYDIAG_EXE, "PSMPATH.PSM", timeout=8))

    assert_module_loaded(first, "PSMPATH.PSM")
    assert_module_loaded(second, "PSMPATH.PSM")
    assert_module_size(first, 28672)
    assert_module_size(second, 28672)
    assert_external_pcm_source(first, "psm_module", "dos-fallback", 1, "file-path", 128)
    assert_external_pcm_source(second, "psm_module", "dos-fallback", 1, "file-path", 128)
    assert_bounded_sb16_playback_blocks(first, 64)
    assert_bounded_sb16_playback_blocks(second, 64)
    assert playback_checksum(first) != playback_checksum(second)


def test_iplayc_dos_capped_header_far_same_path_stream_uses_byte_past_near_buffer() -> None:
    def write_capped_far(name: str, tail_byte: int) -> None:
        far = bytearray(28672)
        far[0:4] = b"FAR\xfe"
        far[4:13] = b"FARPATH  "
        far[24576:] = bytes([tail_byte & 0xFF]) * (len(far) - 24576)
        (BUILD_DIR / f"{name}.FAR").write_bytes(far)

    write_capped_far("FARPATH", 0x2B)
    first = combined_output(run_dos(IPLAYDIAG_EXE, "FARPATH.FAR", timeout=8))

    write_capped_far("FARPATH", 0x6B)
    second = combined_output(run_dos(IPLAYDIAG_EXE, "FARPATH.FAR", timeout=8))

    assert_module_loaded(first, "FARPATH.FAR")
    assert_module_loaded(second, "FARPATH.FAR")
    assert_module_size(first, 28672)
    assert_module_size(second, 28672)
    assert_external_pcm_source(first, "far_module", "dos-fallback", 1, "file-path", 128)
    assert_external_pcm_source(second, "far_module", "dos-fallback", 1, "file-path", 128)
    assert_bounded_sb16_playback_blocks(first, 64)
    assert_bounded_sb16_playback_blocks(second, 64)
    assert playback_checksum(first) != playback_checksum(second)


def test_iplayc_dos_capped_header_669_same_path_stream_uses_byte_past_near_buffer() -> None:
    def write_capped_669(name: str, tail_byte: int) -> None:
        e669 = bytearray(28672)
        e669[0:2] = b"if"
        e669[2:11] = b"669PATH  "
        e669[0x6E] = 4
        e669[0x6F] = 3
        e669[0x70] = 1
        e669[24576:] = bytes([tail_byte & 0xFF]) * (len(e669) - 24576)
        (BUILD_DIR / f"{name}.669").write_bytes(e669)

    write_capped_669("E669PATH", 0x27)
    first = combined_output(run_dos(IPLAYDIAG_EXE, "E669PATH.669", timeout=8))

    write_capped_669("E669PATH", 0x67)
    second = combined_output(run_dos(IPLAYDIAG_EXE, "E669PATH.669", timeout=8))

    assert_module_loaded(first, "E669PATH.669")
    assert_module_loaded(second, "E669PATH.669")
    assert_module_size(first, 28672)
    assert_module_size(second, 28672)
    assert_external_pcm_source(first, "e669_module", "dos-fallback", 1, "file-path", 113)
    assert_external_pcm_source(second, "e669_module", "dos-fallback", 1, "file-path", 113)
    assert_bounded_sb16_playback_blocks(first, 64)
    assert_bounded_sb16_playback_blocks(second, 64)
    assert playback_checksum(first) != playback_checksum(second)


def test_iplayc_dos_capped_header_ult_same_path_stream_uses_byte_past_near_buffer() -> None:
    def write_capped_ult(name: str, tail_byte: int) -> None:
        ult = bytearray(28672)
        ult[:15] = b"MAS_UTrack_V001"
        ult[15:24] = b"ULTPATH  "
        ult[24576:] = bytes([tail_byte & 0xFF]) * (len(ult) - 24576)
        (BUILD_DIR / f"{name}.ULT").write_bytes(ult)

    write_capped_ult("ULTPATH", 0x31)
    first = combined_output(run_dos(IPLAYDIAG_EXE, "ULTPATH.ULT", timeout=8))

    write_capped_ult("ULTPATH", 0x71)
    second = combined_output(run_dos(IPLAYDIAG_EXE, "ULTPATH.ULT", timeout=8))

    assert_module_loaded(first, "ULTPATH.ULT")
    assert_module_loaded(second, "ULTPATH.ULT")
    assert_module_size(first, 28672)
    assert_module_size(second, 28672)
    assert_external_pcm_source(first, "ult_module", "dos-fallback", 1, "file-path", 96)
    assert_external_pcm_source(second, "ult_module", "dos-fallback", 1, "file-path", 96)
    assert_bounded_sb16_playback_blocks(first, 64)
    assert_bounded_sb16_playback_blocks(second, 64)
    assert playback_checksum(first) != playback_checksum(second)


def test_iplayc_dos_capped_header_mtm_same_path_stream_uses_byte_past_near_buffer() -> None:
    def write_capped_mtm(name: str, tail_byte: int) -> None:
        mtm = bytearray(28672)
        mtm[0:3] = b"MTM"
        mtm[4:13] = b"MTMPATH  "
        mtm[0x1A:0x1C] = (12).to_bytes(2, "little")
        mtm[0x1C] = 2
        mtm[0x1E] = 4
        mtm[0x20] = 6
        mtm[0x22:0x27] = bytes([0, 1, 2, 3, 4])
        mtm[24576:] = bytes([tail_byte & 0xFF]) * (len(mtm) - 24576)
        (BUILD_DIR / f"{name}.MTM").write_bytes(mtm)

    write_capped_mtm("MTMPATH", 0x29)
    first = combined_output(run_dos(IPLAYDIAG_EXE, "MTMPATH.MTM", timeout=8))

    write_capped_mtm("MTMPATH", 0x69)
    second = combined_output(run_dos(IPLAYDIAG_EXE, "MTMPATH.MTM", timeout=8))

    assert_module_loaded(first, "MTMPATH.MTM")
    assert_module_loaded(second, "MTMPATH.MTM")
    assert_module_size(first, 28672)
    assert_module_size(second, 28672)
    assert_external_pcm_source(first, "mtm_module", "dos-fallback", 1, "file-path", 66)
    assert_external_pcm_source(second, "mtm_module", "dos-fallback", 1, "file-path", 66)
    assert_bounded_sb16_playback_blocks(first, 64)
    assert_bounded_sb16_playback_blocks(second, 64)
    assert playback_checksum(first) != playback_checksum(second)


def test_iplayc_dos_capped_header_stm_same_path_stream_uses_byte_past_near_buffer() -> None:
    def write_capped_stm(name: str, tail_byte: int) -> None:
        stm = bytearray(28672)
        stm[:9] = b"STMPATH  "
        stm[20:28] = b"!Scream!"
        stm[24576:] = bytes([tail_byte & 0xFF]) * (len(stm) - 24576)
        (BUILD_DIR / f"{name}.STM").write_bytes(stm)

    write_capped_stm("STMPATH", 0x2D)
    first = combined_output(run_dos(IPLAYDIAG_EXE, "STMPATH.STM", timeout=8))

    write_capped_stm("STMPATH", 0x6D)
    second = combined_output(run_dos(IPLAYDIAG_EXE, "STMPATH.STM", timeout=8))

    assert_module_loaded(first, "STMPATH.STM")
    assert_module_loaded(second, "STMPATH.STM")
    assert_module_size(first, 28672)
    assert_module_size(second, 28672)
    assert_external_pcm_source(first, "_2stm_module", "dos-fallback", 1, "file-path", 64)
    assert_external_pcm_source(second, "_2stm_module", "dos-fallback", 1, "file-path", 64)
    assert_bounded_sb16_playback_blocks(first, 64)
    assert_bounded_sb16_playback_blocks(second, 64)
    assert playback_checksum(first) != playback_checksum(second)


def test_iplayc_dos_capped_header_mod_same_path_stream_uses_byte_past_near_buffer() -> None:
    def write_capped_mod(name: str, tail_byte: int) -> None:
        mod = bytearray(28672)
        mod[:8] = b"MODPATH "
        mod[950] = 1
        mod[1080:1084] = b"M.K."
        mod[24576:] = bytes([tail_byte & 0xFF]) * (len(mod) - 24576)
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_capped_mod("MODPATH", 0x2F)
    first = combined_output(run_dos(IPLAYDIAG_EXE, "MODPATH.MOD", timeout=8))

    write_capped_mod("MODPATH", 0x6F)
    second = combined_output(run_dos(IPLAYDIAG_EXE, "MODPATH.MOD", timeout=8))

    assert_module_loaded(first, "MODPATH.MOD")
    assert_module_loaded(second, "MODPATH.MOD")
    assert_module_size(first, 28672)
    assert_module_size(second, 28672)
    assert_external_pcm_source(first, "mod_n_t_module", "dos-fallback", 1, "file-path", 1084)
    assert_external_pcm_source(second, "mod_n_t_module", "dos-fallback", 1, "file-path", 1084)
    assert_bounded_sb16_playback_blocks(first, 64)
    assert_bounded_sb16_playback_blocks(second, 64)
    assert playback_checksum(first) != playback_checksum(second)

def test_iplayc_dos_real_aryx_s3m_reaches_external_tracker_native_preview() -> None:
    target = BUILD_DIR / "aryx.s3m"
    if not target.exists():
        for source in (
            ROOT.parent / "old" / "aryx.s3m",
            ROOT.parent / "masm" / "BIN" / "aryx.s3m",
            ROOT.parent / "libdosbox-0.5x" / "i" / "aryx.s3m",
        ):
            if source.exists():
                target.write_bytes(source.read_bytes())
                break

    result = run_dos(IPLAYDIAG_EXE, "aryx.s3m")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "aryx.s3m")
    assert_module_size(out, 20800)
    assert_decoder_event(out, 214, 1, 4, 1, 20, 15, 2)
    assert_decoder_voice(out, 1, 214, 1, 4, 1, 20, 174, 64, 7, 167, 15616)
    assert_external_native_preview(out)
    assert_bounded_sb16_playback_blocks(out, 64)


def test_iplayc_dos_s3m_placeholder_pcm_depends_on_module_body() -> None:
    left = bytearray(20800)
    right = bytearray(20800)
    for body, title, fill in ((left, b"BODYA", 0x11), (right, b"BODYB", 0x71)):
        body[:len(title)] = title
        body[0x20:0x22] = (3).to_bytes(2, "little")
        body[0x22:0x24] = (1).to_bytes(2, "little")
        body[0x24:0x26] = (1).to_bytes(2, "little")
        body[0x2C:0x30] = b"SCRM"
        body[0x60:0x63] = bytes([0, 1, 2])
        body[0x100:] = bytes([fill]) * (len(body) - 0x100)
    (BUILD_DIR / "BODYA.S3M").write_bytes(left)
    (BUILD_DIR / "BODYB.S3M").write_bytes(right)

    result_a = run_dos(IPLAYDIAG_EXE, "BODYA.S3M")
    result_b = run_dos(IPLAYDIAG_EXE, "BODYB.S3M")
    out_a = combined_output(result_a)
    out_b = combined_output(result_b)

    assert result_a.returncode == 0, out_a
    assert result_b.returncode == 0, out_b
    assert_module_size(out_a, 20800)
    assert_module_size(out_b, 20800)
    assert playback_checksum(out_a) != playback_checksum(out_b)


def test_iplayc_dos_s3m_placeholder_pcm_starts_at_pattern_stream_same_path() -> None:
    def write_fixture(before_stream: int, in_stream: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = b"STRSTART"
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
        s3m[0x180] = before_stream
        s3m[0x200:0x202] = (2).to_bytes(2, "little")
        s3m[0x220:0x500] = bytes([in_stream]) * (0x500 - 0x220)
        (BUILD_DIR / "STRSTART.S3M").write_bytes(s3m)

    write_fixture(0x11, 0x31)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "STRSTART.S3M"))
    write_fixture(0x71, 0x31)
    before_only = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "STRSTART.S3M"))
    write_fixture(0x71, 0x61)
    body_changed = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "STRSTART.S3M"))

    assert_external_pcm_source(base, "s3m_module", "native-preview", 0, "memory", 512)
    assert_bounded_sb16_playback(base)
    assert playback_checksum(base) == playback_checksum(before_only)
    assert playback_checksum(before_only) != playback_checksum(body_changed)


def test_iplayc_dos_capped_s3m_file_pcm_starts_at_pattern_stream_same_path() -> None:
    def write_fixture(before_stream: int, in_stream: int) -> None:
        s3m = bytearray(28672)
        s3m[:8] = b"CAPSTRM"
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
        s3m[0x180] = before_stream
        s3m[0x200:0x202] = (2).to_bytes(2, "little")
        s3m[0x220:] = bytes([in_stream]) * (len(s3m) - 0x220)
        s3m[-1] = in_stream
        (BUILD_DIR / "CAPSTRM.S3M").write_bytes(s3m)

    write_fixture(0x15, 0x35)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "CAPSTRM.S3M"))
    write_fixture(0x75, 0x35)
    before_only = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "CAPSTRM.S3M"))
    write_fixture(0x75, 0x65)
    body_changed = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "CAPSTRM.S3M"))

    assert_external_pcm_source(base, "s3m_module", "dos-fallback", 1, "file-path", 512)
    assert_bounded_sb16_playback(base)
    assert playback_checksum(base) == playback_checksum(before_only)
    assert playback_checksum(before_only) != playback_checksum(body_changed)


def test_iplayc_dos_capped_s3m_file_pcm_crosses_file_stream_refill_boundary_same_path() -> None:
    stream_start = 0x200
    refill_boundary = stream_start + 496

    def write_fixture(after_refill: int) -> None:
        s3m = bytearray(28672)
        s3m[:8] = b"CAPREFIL"
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
        s3m[stream_start:stream_start + 2] = (2).to_bytes(2, "little")
        s3m[stream_start + 2:refill_boundary] = bytes([0x35]) * (refill_boundary - stream_start - 2)
        s3m[refill_boundary:] = bytes([after_refill]) * (len(s3m) - refill_boundary)
        (BUILD_DIR / "CAPREFIL.S3M").write_bytes(s3m)

    write_fixture(0x45)
    first = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=1", "CAPREFIL.S3M"))
    write_fixture(0x65)
    second = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=1", "CAPREFIL.S3M"))

    assert_external_pcm_source(first, "s3m_module", "dos-fallback", 1, "file-path", stream_start)
    assert_external_pcm_source(second, "s3m_module", "dos-fallback", 1, "file-path", stream_start)
    assert_bounded_sb16_playback_blocks(first, 1)
    assert_bounded_sb16_playback_blocks(second, 1)
    assert playback_checksum(first) != playback_checksum(second)


def test_iplayc_dos_mod_placeholder_pcm_starts_at_pattern_stream_same_path() -> None:
    def write_fixture(before_stream: int, in_stream: int) -> None:
        mod = bytearray(1084 + 1024)
        offset = 1084
        mod[:8] = b"MODSTRM "
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1000] = before_stream
        while offset < 1084 + 1024:
            mod[offset:offset + 4] = bytes([0x00, 0x00, 0x00, in_stream])
            offset += 4
        (BUILD_DIR / "MODSTRM.MOD").write_bytes(mod)

    write_fixture(0x12, 0x32)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "MODSTRM.MOD"))
    write_fixture(0x72, 0x32)
    before_only = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "MODSTRM.MOD"))
    write_fixture(0x72, 0x62)
    body_changed = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "MODSTRM.MOD"))

    assert_external_pcm_source(base, "mod_n_t_module", "native-preview", 0, "memory", 1084)
    assert_bounded_sb16_playback(base)
    assert playback_checksum(base) == playback_checksum(before_only)
    assert playback_checksum(before_only) != playback_checksum(body_changed)


def test_iplayc_dos_mtm_placeholder_pcm_starts_after_metadata_same_path() -> None:
    def write_fixture(before_stream: int, in_stream: int) -> None:
        mtm = bytearray(0x200)
        mtm[0:3] = b"MTM"
        mtm[4:13] = b"MTMSTRM  "
        mtm[0x1A:0x1C] = (1).to_bytes(2, "little")
        mtm[0x1C] = 0
        mtm[0x1E] = 0
        mtm[0x20] = 4
        mtm[0x41] = before_stream
        mtm[0x42:] = bytes([in_stream]) * (len(mtm) - 0x42)
        (BUILD_DIR / "MTMSTRM.MTM").write_bytes(mtm)

    write_fixture(0x13, 0x33)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "MTMSTRM.MTM"))
    write_fixture(0x73, 0x33)
    before_only = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "MTMSTRM.MTM"))
    write_fixture(0x73, 0x63)
    body_changed = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "MTMSTRM.MTM"))

    assert_external_pcm_source(base, "mtm_module", "native-preview", 0, "memory", 66)
    assert_bounded_sb16_playback(base)
    assert playback_checksum(base) == playback_checksum(before_only)
    assert playback_checksum(before_only) != playback_checksum(body_changed)


def test_iplayc_dos_stm_placeholder_pcm_starts_after_header_same_path() -> None:
    def write_fixture(before_stream: int, in_stream: int) -> None:
        stm = bytearray(0x200)
        stm[:9] = b"STMSTRM  "
        stm[20:28] = b"!Scream!"
        stm[0x30] = before_stream
        stm[0x40:] = bytes([in_stream]) * (len(stm) - 0x40)
        (BUILD_DIR / "STMSTRM.STM").write_bytes(stm)

    write_fixture(0x14, 0x34)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "STMSTRM.STM"))
    write_fixture(0x74, 0x34)
    before_only = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "STMSTRM.STM"))
    write_fixture(0x74, 0x64)
    body_changed = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "STMSTRM.STM"))

    assert_external_pcm_source(base, "_2stm_module", "native-preview", 0, "memory", 64)
    assert_bounded_sb16_playback(base)
    assert playback_checksum(base) == playback_checksum(before_only)
    assert playback_checksum(before_only) != playback_checksum(body_changed)


def test_iplayc_dos_far_placeholder_pcm_starts_after_metadata_same_path() -> None:
    def write_fixture(before_stream: int, in_stream: int) -> None:
        far = bytearray(0x200)
        far[0:4] = b"FAR\xfe"
        far[4:13] = b"FARSTRM  "
        far[0x7F] = before_stream
        far[0x80:] = bytes([in_stream]) * (len(far) - 0x80)
        (BUILD_DIR / "FARSTRM.FAR").write_bytes(far)

    write_fixture(0x15, 0x35)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "FARSTRM.FAR"))
    write_fixture(0x75, 0x35)
    before_only = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "FARSTRM.FAR"))
    write_fixture(0x75, 0x65)
    body_changed = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "FARSTRM.FAR"))

    assert_external_pcm_source(base, "far_module", "native-preview", 0, "memory", 128)
    assert_bounded_sb16_playback(base)
    assert playback_checksum(base) == playback_checksum(before_only)
    assert playback_checksum(before_only) != playback_checksum(body_changed)


def test_iplayc_dos_669_placeholder_pcm_starts_after_metadata_same_path() -> None:
    def write_fixture(before_stream: int, in_stream: int) -> None:
        e669 = bytearray(0x200)
        e669[0:2] = b"if"
        e669[2:11] = b"S669STRM "
        e669[0x6E] = 4
        e669[0x6F] = 3
        e669[0x70] = before_stream
        e669[0x71:] = bytes([in_stream]) * (len(e669) - 0x71)
        (BUILD_DIR / "S669STRM.669").write_bytes(e669)

    write_fixture(0x16, 0x36)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "S669STRM.669"))
    write_fixture(0x76, 0x36)
    before_only = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "S669STRM.669"))
    write_fixture(0x76, 0x66)
    body_changed = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "S669STRM.669"))

    assert_external_pcm_source(base, "e669_module", "native-preview", 0, "memory", 113)
    assert_bounded_sb16_playback(base)
    assert playback_checksum(base) == playback_checksum(before_only)
    assert playback_checksum(before_only) != playback_checksum(body_changed)


def test_iplayc_dos_psm_placeholder_pcm_starts_after_metadata_same_path() -> None:
    def write_fixture(before_stream: int, in_stream: int) -> None:
        psm = bytearray(0x200)
        psm[0:4] = b"PSM "
        psm[4:13] = b"PSMSTRM  "
        psm[0x7F] = before_stream
        psm[0x80:] = bytes([in_stream]) * (len(psm) - 0x80)
        (BUILD_DIR / "PSMSTRM.PSM").write_bytes(psm)

    write_fixture(0x17, 0x37)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "PSMSTRM.PSM"))
    write_fixture(0x77, 0x37)
    before_only = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "PSMSTRM.PSM"))
    write_fixture(0x77, 0x67)
    body_changed = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "PSMSTRM.PSM"))

    assert_external_pcm_source(base, "psm_module", "native-preview", 0, "memory", 128)
    assert_bounded_sb16_playback(base)
    assert playback_checksum(base) == playback_checksum(before_only)
    assert playback_checksum(before_only) != playback_checksum(body_changed)


def test_iplayc_dos_ult_placeholder_pcm_starts_after_metadata_same_path() -> None:
    def write_fixture(before_stream: int, in_stream: int) -> None:
        ult = bytearray(0x200)
        ult[:15] = b"MAS_UTrack_V001"
        ult[15:24] = b"ULTSTRM  "
        ult[0x5F] = before_stream
        ult[0x60:] = bytes([in_stream]) * (len(ult) - 0x60)
        (BUILD_DIR / "ULTSTRM.ULT").write_bytes(ult)

    write_fixture(0x18, 0x38)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "ULTSTRM.ULT"))
    write_fixture(0x78, 0x38)
    before_only = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "ULTSTRM.ULT"))
    write_fixture(0x78, 0x68)
    body_changed = combined_output(run_dos(IPLAYDIAG_EXE, "--blocks=32", "ULTSTRM.ULT"))

    assert_external_pcm_source(base, "ult_module", "native-preview", 0, "memory", 96)
    assert_bounded_sb16_playback(base)
    assert playback_checksum(base) == playback_checksum(before_only)
    assert playback_checksum(before_only) != playback_checksum(body_changed)


def test_iplayc_dos_s3m_pattern_event_activates_sample_voice() -> None:
    s3m = bytearray(0x190)
    s3m[:8] = b"VOICES3M"
    s3m[0x20:0x22] = (1).to_bytes(2, "little")
    s3m[0x22:0x24] = (1).to_bytes(2, "little")
    s3m[0x24:0x26] = (1).to_bytes(2, "little")
    s3m[0x2C:0x30] = b"SCRM"
    s3m[0x31] = 3
    s3m[0x32] = 125
    s3m[0x40] = 0
    s3m[0x41:0x60] = bytes([0xFF]) * 31
    s3m[0x60] = 0
    s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
    s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

    instrument_offset = 0x100
    pattern_offset = 0x150
    sample_offset = 0x180
    s3m[instrument_offset] = 1
    s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
    s3m[instrument_offset + 16:instrument_offset + 20] = (4).to_bytes(4, "little")
    s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
    s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
    s3m[instrument_offset + 28] = 48
    s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

    pattern_data = bytes([0xE0, 0x30, 0x01, 0x28, 0x01, 0x03, 0x00])
    s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
    s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
    s3m[sample_offset:sample_offset + 4] = bytes([0x10, 0x20, 0x30, 0x40])
    (BUILD_DIR / "VOICES3M.S3M").write_bytes(s3m)

    result = run_dos(IPLAYDIAG_EXE, "VOICES3M.S3M")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "VOICES3M.S3M")
    assert_decoder_geometry(out, 1, 64, 0, 3, 125, 1)
    assert_decoder_event(out, 214, 1, 4, 1, 40, 15, 3)
    assert_decoder_voice(out, 1, 214, 1, 4, 1, 40, 4, 48, 0, 0, 384)
    assert_bounded_sb16_playback(out)


def test_iplayc_dos_s3m_volume_slide_stays_on_external_stream_placeholder() -> None:
    def write_volume_slide_fixture(name: str, slide_param: int) -> None:
        s3m = bytearray(0x190)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x180
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (4).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x40, 0x00,
            0x80, 0x04, slide_param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        s3m[sample_offset:sample_offset + 4] = bytes([0x10, 0x20, 0x30, 0x40])
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_volume_slide_fixture("VOLBASE", 0x00)
    write_volume_slide_fixture("VOLSLIDE", 0x05)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "VOLBASE.S3M"))
    slide = combined_output(run_dos(IPLAYDIAG_EXE, "VOLSLIDE.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide)
    assert_external_native_preview(base)
    assert_external_native_preview(slide)


def test_iplayc_dos_s3m_volume_slide_same_path_stays_on_external_stream_placeholder() -> None:
    def write_volume_slide_same_path_fixture(name: str, slide_param: int) -> None:
        s3m = bytearray(0x190)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x180
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (4).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x40, 0x00,
            0x80, 0x04, slide_param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        s3m[sample_offset:sample_offset + 4] = bytes([0x10, 0x20, 0x30, 0x40])
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_volume_slide_same_path_fixture("S3MVSL", 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MVSL.S3M"))

    write_volume_slide_same_path_fixture("S3MVSL", 0x05)
    slide = combined_output(run_dos(IPLAYDIAG_EXE, "S3MVSL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide)
    assert_external_native_preview(base)
    assert_external_native_preview(slide)


def test_iplayc_dos_s3m_sample_loop_reports_metadata_on_external_stream_placeholder() -> None:
    def write_loop_fixture(name: str, flags: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x20).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (4).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0x20).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 31] = flags & 0xFF
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x20):
            s3m[sample_offset + i] = (i * 113 + (i >> 1)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_loop_fixture("LOOPNO", 0x00)
    write_loop_fixture("LOOPYES", 0x01)

    no_loop = combined_output(run_dos(IPLAYDIAG_EXE, "LOOPNO.S3M"))
    looped = combined_output(run_dos(IPLAYDIAG_EXE, "LOOPYES.S3M"))

    assert_decoder_voice(no_loop, 1, 214, 1, 4, 1, 48, 32, 48, 4, 0, 512)
    assert_decoder_voice(looped, 1, 214, 1, 4, 1, 48, 32, 48, 4, 28, 512)
    assert_bounded_sb16_playback(no_loop)
    assert_bounded_sb16_playback(looped)
    assert_external_native_preview(no_loop)
    assert_external_native_preview(looped)


def test_iplayc_dos_s3m_sample_loop_same_path_reports_metadata_on_external_stream_placeholder() -> None:
    def write_loop_same_path_fixture(name: str, flags: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x20).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (4).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0x20).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 31] = flags & 0xFF
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x20):
            s3m[sample_offset + i] = (i * 113 + (i >> 1)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_loop_same_path_fixture("S3MLOOP", 0x00)
    no_loop = combined_output(run_dos(IPLAYDIAG_EXE, "S3MLOOP.S3M"))

    write_loop_same_path_fixture("S3MLOOP", 0x01)
    looped = combined_output(run_dos(IPLAYDIAG_EXE, "S3MLOOP.S3M"))

    assert_decoder_voice(no_loop, 1, 214, 1, 4, 1, 48, 32, 48, 4, 0, 512)
    assert_decoder_voice(looped, 1, 214, 1, 4, 1, 48, 32, 48, 4, 28, 512)
    assert_bounded_sb16_playback(no_loop)
    assert_bounded_sb16_playback(looped)
    assert_external_native_preview(no_loop)
    assert_external_native_preview(looped)


def test_iplayc_dos_s3m_16bit_sample_flag_reports_metadata_on_external_stream_placeholder() -> None:
    def write_sample_width_fixture(name: str, flags: int) -> None:
        s3m = bytearray(0x600)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x40).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 31] = flags & 0xFF
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x80):
            s3m[sample_offset + i] = (i * 127 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_sample_width_fixture("S8BIT", 0x00)
    write_sample_width_fixture("S16BIT", 0x04)

    sample_8 = combined_output(run_dos(IPLAYDIAG_EXE, "S8BIT.S3M"))
    sample_16 = combined_output(run_dos(IPLAYDIAG_EXE, "S16BIT.S3M"))

    assert_decoder_voice(sample_8, 1, 214, 1, 4, 1, 48, 64, 48, 0, 0, 512)
    assert_decoder_voice(sample_16, 1, 214, 1, 4, 1, 48, 64, 48, 0, 0, 512)
    assert_bounded_sb16_playback(sample_8)
    assert_bounded_sb16_playback(sample_16)
    assert_external_native_preview(sample_8)
    assert_external_native_preview(sample_16)


def test_iplayc_dos_s3m_16bit_sample_flag_same_path_reports_metadata_on_external_stream_placeholder() -> None:
    def write_sample_width_same_path_fixture(name: str, flags: int) -> None:
        s3m = bytearray(0x600)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x40).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 31] = flags & 0xFF
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x80):
            s3m[sample_offset + i] = (i * 127 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_sample_width_same_path_fixture("S3MWID", 0x00)
    sample_8 = combined_output(run_dos(IPLAYDIAG_EXE, "S3MWID.S3M"))

    write_sample_width_same_path_fixture("S3MWID", 0x04)
    sample_16 = combined_output(run_dos(IPLAYDIAG_EXE, "S3MWID.S3M"))

    assert_decoder_voice(sample_8, 1, 214, 1, 4, 1, 48, 64, 48, 0, 0, 512)
    assert_decoder_voice(sample_16, 1, 214, 1, 4, 1, 48, 64, 48, 0, 0, 512)
    assert_bounded_sb16_playback(sample_8)
    assert_bounded_sb16_playback(sample_16)
    assert_external_native_preview(sample_8)
    assert_external_native_preview(sample_16)


def test_iplayc_dos_s3m_unsigned_sample_format_reports_metadata_on_external_stream_placeholder() -> None:
    def write_sample_format_fixture(name: str, ffi: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2A:0x2C] = ffi.to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 79 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_sample_format_fixture("S3MFFI1", 1)
    write_sample_format_fixture("S3MFFI2", 2)

    signed = combined_output(run_dos(IPLAYDIAG_EXE, "S3MFFI1.S3M"))
    unsigned = combined_output(run_dos(IPLAYDIAG_EXE, "S3MFFI2.S3M"))

    assert_ffi_marker(signed, "0001")
    assert_ffi_marker(unsigned, "0002")
    assert_bounded_sb16_playback(signed)
    assert_bounded_sb16_playback(unsigned)
    assert_external_native_preview(signed)
    assert_external_native_preview(unsigned)


def test_iplayc_dos_s3m_unsigned_sample_format_same_path_reports_metadata_on_external_stream_placeholder() -> None:
    def write_sample_format_same_path_fixture(name: str, ffi: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2A:0x2C] = ffi.to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 79 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_sample_format_same_path_fixture("S3MFFI", 1)
    signed = combined_output(run_dos(IPLAYDIAG_EXE, "S3MFFI.S3M"))

    write_sample_format_same_path_fixture("S3MFFI", 2)
    unsigned = combined_output(run_dos(IPLAYDIAG_EXE, "S3MFFI.S3M"))

    assert_ffi_marker(signed, "0001")
    assert_ffi_marker(unsigned, "0002")
    assert_bounded_sb16_playback(signed)
    assert_bounded_sb16_playback(unsigned)
    assert_external_native_preview(signed)
    assert_external_native_preview(unsigned)


def test_iplayc_dos_s3m_c2spd_reports_metadata_on_external_stream_placeholder() -> None:
    def write_c2spd_fixture(name: str, c2spd: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = c2spd.to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 121 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_c2spd_fixture("C2BASE", 8363)
    write_c2spd_fixture("C2FAST", 16726)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "C2BASE.S3M"))
    fast = combined_output(run_dos(IPLAYDIAG_EXE, "C2FAST.S3M"))

    assert_decoder_voice(base, 1, 214, 1, 4, 1, 48, 640, 48, 0, 0, 512)
    assert_decoder_voice(fast, 1, 214, 1, 4, 1, 48, 640, 48, 0, 0, 512)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(fast)
    assert_external_native_preview(base)
    assert_external_native_preview(fast)


def test_iplayc_dos_s3m_c2spd_same_path_reports_metadata_on_external_stream_placeholder() -> None:
    def write_c2spd_same_path_fixture(name: str, c2spd: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = c2spd.to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 121 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_c2spd_same_path_fixture("C2PATH", 8363)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "C2PATH.S3M"))

    write_c2spd_same_path_fixture("C2PATH", 16726)
    fast = combined_output(run_dos(IPLAYDIAG_EXE, "C2PATH.S3M"))

    assert_decoder_voice(base, 1, 214, 1, 4, 1, 48, 640, 48, 0, 0, 512)
    assert_decoder_voice(fast, 1, 214, 1, 4, 1, 48, 640, 48, 0, 0, 512)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(fast)
    assert_external_native_preview(base)
    assert_external_native_preview(fast)


def test_iplayc_dos_s3m_fractional_interpolation_same_path_reports_external_stream_placeholder() -> None:
    def write_fractional_interpolation_fixture(name: str, adjacent_sample: int) -> None:
        s3m = bytearray(0x300)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (2).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (2).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 31] = 1
        s3m[instrument_offset + 32:instrument_offset + 36] = (4181).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        s3m[sample_offset] = 0x00
        s3m[sample_offset + 1] = adjacent_sample & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_fractional_interpolation_fixture("S3MFRAC", 0x00)
    flat = combined_output(run_dos(IPLAYDIAG_EXE, "S3MFRAC.S3M"))

    write_fractional_interpolation_fixture("S3MFRAC", 0x7F)
    rising = combined_output(run_dos(IPLAYDIAG_EXE, "S3MFRAC.S3M"))

    assert_decoder_voice(flat, 1, 214, 1, 4, 1, 48, 2, 48, 0, 2, 512)
    assert_decoder_voice(rising, 1, 214, 1, 4, 1, 48, 2, 48, 0, 2, 512)
    assert_bounded_sb16_playback(flat)
    assert_bounded_sb16_playback(rising)
    assert_external_native_preview(flat)
    assert_external_native_preview(rising)


def test_iplayc_dos_s3m_extended_volume_slide_stays_on_external_stream_placeholder() -> None:
    def write_extended_volume_slide_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 71 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_extended_volume_slide_fixture("SABASE", 0x00, 0x00)
    write_extended_volume_slide_fixture("SAVOLUP", 0x13, 0xA4)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "SABASE.S3M"))
    slide = combined_output(run_dos(IPLAYDIAG_EXE, "SAVOLUP.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide)
    assert_external_native_preview(base)
    assert_external_native_preview(slide)


def test_iplayc_dos_s3m_extended_volume_slide_same_path_stays_on_external_stream_placeholder() -> None:
    def write_extended_volume_slide_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 71 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_extended_volume_slide_same_path_fixture("S3MSAV", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MSAV.S3M"))

    write_extended_volume_slide_same_path_fixture("S3MSAV", 0x13, 0xA4)
    fine_up = combined_output(run_dos(IPLAYDIAG_EXE, "S3MSAV.S3M"))

    write_extended_volume_slide_same_path_fixture("S3MSAV", 0x13, 0xB4)
    fine_down = combined_output(run_dos(IPLAYDIAG_EXE, "S3MSAV.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(fine_up)
    assert_bounded_sb16_playback(fine_down)
    assert_external_native_preview(base)
    assert_external_native_preview(fine_up)
    assert_external_native_preview(fine_down)


def test_iplayc_dos_s3m_global_volume_stays_on_external_stream_placeholder() -> None:
    def write_global_volume_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 83 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_global_volume_fixture("GVBASE", 0x00, 0x00)
    write_global_volume_fixture("GLOVOL", 0x16, 0x20)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "GVBASE.S3M"))
    global_volume = combined_output(run_dos(IPLAYDIAG_EXE, "GLOVOL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(global_volume)
    assert_external_native_preview(base)
    assert_external_native_preview(global_volume)


def test_iplayc_dos_s3m_global_volume_same_path_stays_on_external_stream_placeholder() -> None:
    def write_global_volume_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 83 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_global_volume_same_path_fixture("S3MGVOL", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MGVOL.S3M"))

    write_global_volume_same_path_fixture("S3MGVOL", 0x16, 0x20)
    global_volume = combined_output(run_dos(IPLAYDIAG_EXE, "S3MGVOL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(global_volume)
    assert_external_native_preview(base)
    assert_external_native_preview(global_volume)


def test_iplayc_dos_s3m_header_global_volume_stays_on_external_stream_placeholder() -> None:
    def write_header_global_volume_fixture(name: str, global_volume: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = global_volume & 0xFF
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 123 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_header_global_volume_fixture("HGVOL64", 64)
    write_header_global_volume_fixture("HGVOL32", 32)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "HGVOL64.S3M"))
    quiet = combined_output(run_dos(IPLAYDIAG_EXE, "HGVOL32.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(quiet)
    assert_external_native_preview(base)
    assert_external_native_preview(quiet)


def test_iplayc_dos_s3m_header_global_volume_same_path_stays_on_external_stream_placeholder() -> None:
    def write_header_global_volume_same_path_fixture(name: str, global_volume: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = global_volume & 0xFF
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 123 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_header_global_volume_same_path_fixture("S3MHGV", 64)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MHGV.S3M"))

    write_header_global_volume_same_path_fixture("S3MHGV", 32)
    quiet = combined_output(run_dos(IPLAYDIAG_EXE, "S3MHGV.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(quiet)
    assert_external_native_preview(base)
    assert_external_native_preview(quiet)


def test_iplayc_dos_s3m_master_volume_stays_on_external_stream_placeholder() -> None:
    def write_master_volume_fixture(name: str, master_volume: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80 | (master_volume & 0x7F)
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 127 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_master_volume_fixture("MVOL64", 64)
    write_master_volume_fixture("MVOL32", 32)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "MVOL64.S3M"))
    quiet = combined_output(run_dos(IPLAYDIAG_EXE, "MVOL32.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(quiet)
    assert_external_native_preview(base)
    assert_external_native_preview(quiet)


def test_iplayc_dos_s3m_master_volume_same_path_stays_on_external_stream_placeholder() -> None:
    def write_master_volume_same_path_fixture(name: str, master_volume: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80 | (master_volume & 0x7F)
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 127 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_master_volume_same_path_fixture("S3MMVOL", 64)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MMVOL.S3M"))

    write_master_volume_same_path_fixture("S3MMVOL", 32)
    quiet = combined_output(run_dos(IPLAYDIAG_EXE, "S3MMVOL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(quiet)
    assert_external_native_preview(base)
    assert_external_native_preview(quiet)


def test_iplayc_dos_s3m_global_volume_slide_stays_on_external_stream_placeholder() -> None:
    def write_global_volume_slide_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 89 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_global_volume_slide_fixture("GVSBASE", 0x00, 0x00)
    write_global_volume_slide_fixture("GVSLIDE", 0x17, 0x0F)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "GVSBASE.S3M"))
    slide = combined_output(run_dos(IPLAYDIAG_EXE, "GVSLIDE.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide)
    assert_external_native_preview(base)
    assert_external_native_preview(slide)


def test_iplayc_dos_s3m_global_volume_slide_same_path_stays_on_external_stream_placeholder() -> None:
    def write_global_volume_slide_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 89 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_global_volume_slide_same_path_fixture("S3MGVS", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MGVS.S3M"))

    write_global_volume_slide_same_path_fixture("S3MGVS", 0x17, 0x0F)
    slide = combined_output(run_dos(IPLAYDIAG_EXE, "S3MGVS.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide)
    assert_external_native_preview(base)
    assert_external_native_preview(slide)


def test_iplayc_dos_s3m_channel_volume_stays_on_external_stream_placeholder() -> None:
    def write_channel_volume_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 101 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_channel_volume_fixture("CVBASE", 0x00, 0x00)
    write_channel_volume_fixture("CHANVOL", 0x0D, 0x20)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "CVBASE.S3M"))
    channel_volume = combined_output(run_dos(IPLAYDIAG_EXE, "CHANVOL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(channel_volume)
    assert_external_native_preview(base)
    assert_external_native_preview(channel_volume)


def test_iplayc_dos_s3m_channel_volume_same_path_stays_on_external_stream_placeholder() -> None:
    def write_channel_volume_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 101 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_channel_volume_same_path_fixture("S3MCVOL", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MCVOL.S3M"))

    write_channel_volume_same_path_fixture("S3MCVOL", 0x0D, 0x20)
    channel_volume = combined_output(run_dos(IPLAYDIAG_EXE, "S3MCVOL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(channel_volume)
    assert_external_native_preview(base)
    assert_external_native_preview(channel_volume)


def test_iplayc_dos_s3m_channel_volume_slide_stays_on_external_stream_placeholder() -> None:
    def write_channel_volume_slide_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 103 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_channel_volume_slide_fixture("CVSBASE", 0x00, 0x00)
    write_channel_volume_slide_fixture("CVSLIDE", 0x0E, 0x0F)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "CVSBASE.S3M"))
    slide = combined_output(run_dos(IPLAYDIAG_EXE, "CVSLIDE.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide)
    assert_external_native_preview(base)
    assert_external_native_preview(slide)


def test_iplayc_dos_s3m_channel_volume_slide_same_path_stays_on_external_stream_placeholder() -> None:
    def write_channel_volume_slide_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 103 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_channel_volume_slide_same_path_fixture("S3MCVS", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MCVS.S3M"))

    write_channel_volume_slide_same_path_fixture("S3MCVS", 0x0E, 0x0F)
    slide = combined_output(run_dos(IPLAYDIAG_EXE, "S3MCVS.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide)
    assert_external_native_preview(base)
    assert_external_native_preview(slide)


def test_iplayc_dos_s3m_header_channel_panning_stays_on_external_stream_placeholder() -> None:
    def write_channel_pan_fixture(name: str, channel0_setting: int, channel1_setting: int) -> None:
        s3m = bytearray(0x800)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (2).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80
        s3m[0x40] = channel0_setting & 0xFF
        s3m[0x41] = channel1_setting & 0xFF
        s3m[0x42:0x60] = bytes([0xFF]) * 30
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x14).to_bytes(2, "little")
        s3m[0x65:0x67] = (0x18).to_bytes(2, "little")

        instrument0_offset = 0x100
        instrument1_offset = 0x140
        pattern_offset = 0x180
        sample0_offset = 0x300
        sample1_offset = 0x580
        for instrument_offset, sample_offset, volume in (
            (instrument0_offset, sample0_offset, 48),
            (instrument1_offset, sample1_offset, 40),
        ):
            s3m[instrument_offset] = 1
            s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
            s3m[instrument_offset + 16:instrument_offset + 20] = (0x200).to_bytes(4, "little")
            s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
            s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
            s3m[instrument_offset + 28] = volume
            s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
            s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30,
            0x61, 0x31, 0x02, 0x30,
            0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x200):
            s3m[sample0_offset + i] = (i * 97 + (i >> 2)) & 0xFF
            s3m[sample1_offset + i] = (i * 53 + (i >> 3) + 31) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_channel_pan_fixture("HPANLR", 0, 8)
    write_channel_pan_fixture("HPANRL", 8, 0)

    left_right = combined_output(run_dos(IPLAYDIAG_EXE, "HPANLR.S3M"))
    right_left = combined_output(run_dos(IPLAYDIAG_EXE, "HPANRL.S3M"))

    assert "channels=2" in left_right
    assert "channels=2" in right_left
    assert_bounded_sb16_playback(left_right)
    assert_bounded_sb16_playback(right_left)
    left_right_pump = parse_playback_pump(left_right)
    right_left_pump = parse_playback_pump(right_left)
    assert_playback_pump_sb16_stereo(left_right_pump, 32, SB16_BOUNDED_BLOCK_FRAMES)
    assert left_right_pump["accepted"] == 32 * SB16_BOUNDED_BLOCK_BYTES
    assert_playback_pump_sb16_stereo(right_left_pump, 32, SB16_BOUNDED_BLOCK_FRAMES)
    assert right_left_pump["accepted"] == 32 * SB16_BOUNDED_BLOCK_BYTES
    assert_external_native_preview(left_right)
    assert_external_native_preview(right_left)


def test_iplayc_dos_s3m_default_pan_table_stays_on_external_stream_placeholder() -> None:
    def write_default_pan_fixture(name: str, channel0_pan: int, channel1_pan: int) -> None:
        s3m = bytearray(0x800)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (2).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80
        s3m[0x35] = 0xFC
        s3m[0x40] = 0
        s3m[0x41] = 0
        s3m[0x42:0x60] = bytes([0xFF]) * 30
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x14).to_bytes(2, "little")
        s3m[0x65:0x67] = (0x18).to_bytes(2, "little")
        s3m[0x67] = 0x20 | (channel0_pan & 0x0F)
        s3m[0x68] = 0x20 | (channel1_pan & 0x0F)
        s3m[0x69:0x87] = bytes([0]) * 30

        instrument0_offset = 0x100
        instrument1_offset = 0x140
        pattern_offset = 0x180
        sample0_offset = 0x300
        sample1_offset = 0x580
        for instrument_offset, sample_offset, volume in (
            (instrument0_offset, sample0_offset, 48),
            (instrument1_offset, sample1_offset, 40),
        ):
            s3m[instrument_offset] = 1
            s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
            s3m[instrument_offset + 16:instrument_offset + 20] = (0x200).to_bytes(4, "little")
            s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
            s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
            s3m[instrument_offset + 28] = volume
            s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
            s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30,
            0x61, 0x31, 0x02, 0x30,
            0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x200):
            s3m[sample0_offset + i] = (i * 109 + (i >> 2)) & 0xFF
            s3m[sample1_offset + i] = (i * 47 + (i >> 3) + 19) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_default_pan_fixture("DPANLR", 0, 15)
    write_default_pan_fixture("DPANRL", 15, 0)

    left_right = combined_output(run_dos(IPLAYDIAG_EXE, "DPANLR.S3M"))
    right_left = combined_output(run_dos(IPLAYDIAG_EXE, "DPANRL.S3M"))

    assert "channels=2" in left_right
    assert "channels=2" in right_left
    assert_bounded_sb16_playback(left_right)
    assert_bounded_sb16_playback(right_left)
    assert_external_native_preview(left_right)
    assert_external_native_preview(right_left)


def test_iplayc_dos_s3m_stereo_flag_reports_header_panning_metadata_on_external_stream_placeholder() -> None:
    def write_stereo_flag_fixture(name: str, master_volume: int) -> None:
        s3m = bytearray(0x800)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (2).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = master_volume & 0xFF
        s3m[0x35] = 0xFC
        s3m[0x40] = 0
        s3m[0x41] = 0
        s3m[0x42:0x60] = bytes([0xFF]) * 30
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x14).to_bytes(2, "little")
        s3m[0x65:0x67] = (0x18).to_bytes(2, "little")
        s3m[0x67] = 0x20
        s3m[0x68] = 0x2F
        s3m[0x69:0x87] = bytes([0]) * 30

        instrument0_offset = 0x100
        instrument1_offset = 0x140
        pattern_offset = 0x180
        sample0_offset = 0x300
        sample1_offset = 0x580
        for instrument_offset, sample_offset, volume in (
            (instrument0_offset, sample0_offset, 48),
            (instrument1_offset, sample1_offset, 40),
        ):
            s3m[instrument_offset] = 1
            s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
            s3m[instrument_offset + 16:instrument_offset + 20] = (0x200).to_bytes(4, "little")
            s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
            s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
            s3m[instrument_offset + 28] = volume
            s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
            s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30,
            0x61, 0x31, 0x02, 0x30,
            0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x200):
            s3m[sample0_offset + i] = (i * 113 + (i >> 2)) & 0xFF
            s3m[sample1_offset + i] = (i * 59 + (i >> 3) + 23) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_stereo_flag_fixture("PANMONO", 0x00)
    write_stereo_flag_fixture("PANSTER", 0x80)

    mono = combined_output(run_dos(IPLAYDIAG_EXE, "PANMONO.S3M"))
    stereo = combined_output(run_dos(IPLAYDIAG_EXE, "PANSTER.S3M"))

    assert "channels=2" in mono
    assert "channels=2" in stereo
    assert_bounded_sb16_playback(mono)
    assert_bounded_sb16_playback(stereo)
    mono_pump = parse_playback_pump(mono)
    stereo_pump = parse_playback_pump(stereo)
    assert_playback_pump_sb16_stereo(mono_pump, 32, SB16_BOUNDED_BLOCK_FRAMES)
    assert mono_pump["accepted"] == 32 * SB16_BOUNDED_BLOCK_BYTES
    assert_playback_pump_sb16_stereo(stereo_pump, 32, SB16_BOUNDED_BLOCK_FRAMES)
    assert stereo_pump["accepted"] == 32 * SB16_BOUNDED_BLOCK_BYTES
    assert_external_native_preview(mono)
    assert_external_native_preview(stereo)


def test_iplayc_dos_s3m_sparse_physical_channel_maps_to_logical_channel() -> None:
    s3m = bytearray(0x500)
    s3m[:8] = b"SPARSECH"
    s3m[0x20:0x22] = (1).to_bytes(2, "little")
    s3m[0x22:0x24] = (1).to_bytes(2, "little")
    s3m[0x24:0x26] = (1).to_bytes(2, "little")
    s3m[0x2C:0x30] = b"SCRM"
    s3m[0x30] = 64
    s3m[0x31] = 1
    s3m[0x32] = 125
    s3m[0x33] = 0x80
    s3m[0x40:0x48] = bytes([0xFF]) * 8
    s3m[0x48] = 8
    s3m[0x49:0x60] = bytes([0xFF]) * 23
    s3m[0x60] = 0
    s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
    s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

    instrument_offset = 0x100
    pattern_offset = 0x150
    sample_offset = 0x200
    s3m[instrument_offset] = 1
    s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
    s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
    s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
    s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
    s3m[instrument_offset + 28] = 48
    s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
    s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

    pattern_data = bytes([
        0x68, 0x30, 0x01, 0x30, 0x00,
    ])
    s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
    s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
    for i in range(0x280):
        s3m[sample_offset + i] = (i * 83 + (i >> 2)) & 0xFF
    (BUILD_DIR / "SPARSECH.S3M").write_bytes(s3m)

    out = combined_output(run_dos(IPLAYDIAG_EXE, "SPARSECH.S3M"))

    assert_external_native_preview(out)
    assert "channels=1" in out
    assert_decoder_event(out, 214, 1, 4, 1, 48, 0, 0)
    assert_decoder_voice(out, 1, 214, 1, 4, 1, 48, 640, 48, 0, 0, 512)
    assert_bounded_sb16_playback(out)


def test_iplayc_dos_s3m_volume_column_panning_stays_on_external_stream_placeholder() -> None:
    def write_volume_pan_fixture(name: str, volume_pan: int, command: int = 0, param: int = 0) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        if command:
            pattern_data = bytes([
                0xE0, 0x30, 0x01, volume_pan & 0xFF, command & 0xFF, param & 0xFF, 0x00,
            ])
        else:
            pattern_data = bytes([
                0x60, 0x30, 0x01, volume_pan & 0xFF, 0x00,
            ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 61 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_volume_pan_fixture("VPANL", 128)
    write_volume_pan_fixture("VPANR", 192)

    left = combined_output(run_dos(IPLAYDIAG_EXE, "VPANL.S3M"))
    right = combined_output(run_dos(IPLAYDIAG_EXE, "VPANR.S3M"))

    assert_bounded_sb16_playback(left)
    assert_bounded_sb16_playback(right)
    assert_external_native_preview(left)
    assert_external_native_preview(right)


def test_iplayc_dos_s3m_volume_column_panning_same_path_stays_on_external_stream_placeholder() -> None:
    def write_volume_pan_same_path_fixture(name: str, volume_pan: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, volume_pan & 0xFF, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 61 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_volume_pan_same_path_fixture("S3MVCP", 128)
    left = combined_output(run_dos(IPLAYDIAG_EXE, "S3MVCP.S3M"))

    write_volume_pan_same_path_fixture("S3MVCP", 192)
    right = combined_output(run_dos(IPLAYDIAG_EXE, "S3MVCP.S3M"))

    assert_bounded_sb16_playback(left)
    assert_bounded_sb16_playback(right)
    assert_external_native_preview(left)
    assert_external_native_preview(right)


def test_iplayc_dos_s3m_volume_column_panning_command_stays_on_external_stream_placeholder() -> None:
    def write_volume_pan_command_fixture(name: str, volume_pan: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, volume_pan & 0xFF, 0x04, 0x0F, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 67 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_volume_pan_command_fixture("VPCMDL", 128)
    write_volume_pan_command_fixture("VPCMDR", 192)

    left = combined_output(run_dos(IPLAYDIAG_EXE, "VPCMDL.S3M"))
    right = combined_output(run_dos(IPLAYDIAG_EXE, "VPCMDR.S3M"))

    assert_bounded_sb16_playback(left)
    assert_bounded_sb16_playback(right)
    assert_external_native_preview(left)
    assert_external_native_preview(right)


def test_iplayc_dos_s3m_note_byte_fe_reports_cut_metadata_on_external_stream_placeholder() -> None:
    def write_note_cut_byte_fixture(name: str, cut_on_second_row: bool) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        if cut_on_second_row:
            pattern_data = bytes([
                0x60, 0x30, 0x01, 0x30, 0x00,
                0x20, 0xFE, 0x00, 0x00,
            ])
        else:
            pattern_data = bytes([
                0x60, 0x30, 0x01, 0x30, 0x00,
                0x00,
            ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 73 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_note_cut_byte_fixture("NOTECON", False)
    write_note_cut_byte_fixture("NOTECUT", True)

    continuous = combined_output(run_dos(IPLAYDIAG_EXE, "NOTECON.S3M"))
    cut = combined_output(run_dos(IPLAYDIAG_EXE, "NOTECUT.S3M"))

    assert_bounded_sb16_playback(continuous)
    assert_bounded_sb16_playback(cut)
    assert_external_native_preview(continuous)
    assert_external_native_preview(cut)


def test_iplayc_dos_s3m_note_byte_fe_same_path_reports_cut_metadata_on_external_stream_placeholder() -> None:
    def write_note_cut_byte_same_path_fixture(name: str, cut_on_second_row: bool) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x30] = 64
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x33] = 0x80
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        if cut_on_second_row:
            pattern_data = bytes([
                0x60, 0x30, 0x01, 0x30, 0x00,
                0x20, 0xFE, 0x00, 0x00,
            ])
        else:
            pattern_data = bytes([
                0x60, 0x30, 0x01, 0x30, 0x00,
                0x00,
            ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 73 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_note_cut_byte_same_path_fixture("S3MFECUT", False)
    continuous = combined_output(run_dos(IPLAYDIAG_EXE, "S3MFECUT.S3M"))

    write_note_cut_byte_same_path_fixture("S3MFECUT", True)
    cut = combined_output(run_dos(IPLAYDIAG_EXE, "S3MFECUT.S3M"))

    assert_bounded_sb16_playback(continuous)
    assert_bounded_sb16_playback(cut)
    assert_external_native_preview(continuous)
    assert_external_native_preview(cut)


def test_iplayc_dos_s3m_sample_offset_stays_on_external_stream_placeholder() -> None:
    def write_sample_offset_fixture(name: str, offset_param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, 0x0F, offset_param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 17 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_sample_offset_fixture("OFFBASE", 0x00)
    write_sample_offset_fixture("OFFSAMP", 0x01)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "OFFBASE.S3M"))
    shifted = combined_output(run_dos(IPLAYDIAG_EXE, "OFFSAMP.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(shifted)
    assert_external_native_preview(base)
    assert_external_native_preview(shifted)


def test_iplayc_dos_s3m_sample_offset_same_path_stays_on_external_stream_placeholder() -> None:
    def write_sample_offset_same_path_fixture(name: str, offset_param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, 0x0F, offset_param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 17 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_sample_offset_same_path_fixture("S3MOFF", 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MOFF.S3M"))

    write_sample_offset_same_path_fixture("S3MOFF", 0x01)
    shifted = combined_output(run_dos(IPLAYDIAG_EXE, "S3MOFF.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(shifted)
    assert_external_native_preview(base)
    assert_external_native_preview(shifted)


def test_iplayc_dos_s3m_pitch_slide_stays_on_external_stream_placeholder() -> None:
    def write_pitch_slide_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 11 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_pitch_slide_fixture("PITCH0", 0x00, 0x00)
    write_pitch_slide_fixture("PITCHUP", 0x06, 0x20)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "PITCH0.S3M"))
    shifted = combined_output(run_dos(IPLAYDIAG_EXE, "PITCHUP.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(shifted)
    assert_external_native_preview(base)
    assert_external_native_preview(shifted)


def test_iplayc_dos_s3m_pitch_slide_same_path_stays_on_external_stream_placeholder() -> None:
    def write_pitch_slide_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 11 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_pitch_slide_same_path_fixture("S3MPSL", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPSL.S3M"))

    write_pitch_slide_same_path_fixture("S3MPSL", 0x06, 0x20)
    shifted = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPSL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(shifted)
    assert_external_native_preview(base)
    assert_external_native_preview(shifted)


def test_iplayc_dos_s3m_extended_pitch_slide_stays_on_external_stream_placeholder() -> None:
    def write_extended_pitch_slide_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 67 + (i >> 1)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_extended_pitch_slide_fixture("S1BASE", 0x00, 0x00)
    write_extended_pitch_slide_fixture("S1SLIDE", 0x13, 0x14)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "S1BASE.S3M"))
    shifted = combined_output(run_dos(IPLAYDIAG_EXE, "S1SLIDE.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(shifted)
    assert_external_native_preview(base)
    assert_external_native_preview(shifted)


def test_iplayc_dos_s3m_extended_pitch_slide_same_path_stays_on_external_stream_placeholder() -> None:
    def write_extended_pitch_slide_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 67 + (i >> 1)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_extended_pitch_slide_same_path_fixture("S3MEPS", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MEPS.S3M"))

    write_extended_pitch_slide_same_path_fixture("S3MEPS", 0x13, 0x14)
    fine_up = combined_output(run_dos(IPLAYDIAG_EXE, "S3MEPS.S3M"))

    write_extended_pitch_slide_same_path_fixture("S3MEPS", 0x13, 0x24)
    fine_down = combined_output(run_dos(IPLAYDIAG_EXE, "S3MEPS.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(fine_up)
    assert_bounded_sb16_playback(fine_down)
    assert_external_native_preview(base)
    assert_external_native_preview(fine_up)
    assert_external_native_preview(fine_down)


def test_iplayc_dos_s3m_tone_portamento_stays_on_external_stream_placeholder() -> None:
    def write_tone_portamento_fixture(name: str, command: int, param: int, second_note: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 2
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
            0xE0, second_note, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 7 + (i >> 1)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_tone_portamento_fixture("PORTBASE", 0x00, 0x00, 0x34)
    write_tone_portamento_fixture("TONEPORT", 0x07, 0x18, 0x34)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "PORTBASE.S3M"))
    portamento = combined_output(run_dos(IPLAYDIAG_EXE, "TONEPORT.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(portamento)
    assert_external_native_preview(base)
    assert_external_native_preview(portamento)


def test_iplayc_dos_s3m_tone_portamento_same_path_stays_on_external_stream_placeholder() -> None:
    def write_tone_portamento_same_path_fixture(name: str, command: int, param: int, second_note: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 2
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
            0xE0, second_note, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 7 + (i >> 1)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_tone_portamento_same_path_fixture("S3MPORT", 0x00, 0x00, 0x34)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPORT.S3M"))

    write_tone_portamento_same_path_fixture("S3MPORT", 0x07, 0x18, 0x34)
    portamento = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPORT.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(portamento)
    assert_external_native_preview(base)
    assert_external_native_preview(portamento)


def test_iplayc_dos_s3m_tone_portamento_volume_slide_stays_on_external_stream_placeholder() -> None:
    def write_tone_portamento_volume_slide_fixture(name: str, command: int, param: int, second_note: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 2
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
            0xE0, second_note, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 41 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_tone_portamento_volume_slide_fixture("LBASE", 0x00, 0x00, 0x34)
    write_tone_portamento_volume_slide_fixture("LPORTVOL", 0x0C, 0x18, 0x34)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "LBASE.S3M"))
    combined = combined_output(run_dos(IPLAYDIAG_EXE, "LPORTVOL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(combined)
    assert_external_native_preview(base)
    assert_external_native_preview(combined)


def test_iplayc_dos_s3m_tone_portamento_volume_slide_same_path_stays_on_external_stream_placeholder() -> None:
    def write_tone_portamento_volume_slide_same_path_fixture(name: str, command: int, param: int, second_note: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 2
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
            0xE0, second_note, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 41 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_tone_portamento_volume_slide_same_path_fixture("S3MLPV", 0x00, 0x00, 0x34)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MLPV.S3M"))

    write_tone_portamento_volume_slide_same_path_fixture("S3MLPV", 0x0C, 0x18, 0x34)
    combined = combined_output(run_dos(IPLAYDIAG_EXE, "S3MLPV.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(combined)
    assert_external_native_preview(base)
    assert_external_native_preview(combined)


def test_iplayc_dos_s3m_arpeggio_stays_on_external_stream_placeholder() -> None:
    def write_arpeggio_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 13 + (i >> 1)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_arpeggio_fixture("ARPBASE", 0x00, 0x00)
    write_arpeggio_fixture("ARPEGG", 0x0A, 0x31)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "ARPBASE.S3M"))
    arpeggio = combined_output(run_dos(IPLAYDIAG_EXE, "ARPEGG.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(arpeggio)
    assert_external_native_preview(base)
    assert_external_native_preview(arpeggio)


def test_iplayc_dos_s3m_arpeggio_same_path_stays_on_external_stream_placeholder() -> None:
    def write_arpeggio_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 13 + (i >> 1)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_arpeggio_same_path_fixture("S3MARP", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MARP.S3M"))

    write_arpeggio_same_path_fixture("S3MARP", 0x0A, 0x31)
    arpeggio = combined_output(run_dos(IPLAYDIAG_EXE, "S3MARP.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(arpeggio)
    assert_external_native_preview(base)
    assert_external_native_preview(arpeggio)


def test_iplayc_dos_s3m_retrigger_same_path_stays_on_external_stream_placeholder() -> None:
    def write_retrigger_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 2
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
            0x80, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 19 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_retrigger_same_path_fixture("S3MRET", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MRET.S3M"))

    write_retrigger_same_path_fixture("S3MRET", 0x11, 0x03)
    retrigger = combined_output(run_dos(IPLAYDIAG_EXE, "S3MRET.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(retrigger)
    assert_external_native_preview(base)
    assert_external_native_preview(retrigger)


def test_iplayc_dos_s3m_retrigger_stays_on_external_stream_placeholder() -> None:
    def write_retrigger_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
            0x80, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 19 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_retrigger_fixture("RETBASE", 0x00, 0x00)
    write_retrigger_fixture("RETRIG", 0x11, 0x01)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "RETBASE.S3M"))
    retrigger = combined_output(run_dos(IPLAYDIAG_EXE, "RETRIG.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(retrigger)
    assert_external_native_preview(base)
    assert_external_native_preview(retrigger)


def test_iplayc_dos_s3m_extended_retrigger_stays_on_external_stream_placeholder() -> None:
    def write_extended_retrigger_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
            0x80, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 61 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_extended_retrigger_fixture("S9BASE", 0x00, 0x00)
    write_extended_retrigger_fixture("S9RETR", 0x13, 0x91)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "S9BASE.S3M"))
    retrigger = combined_output(run_dos(IPLAYDIAG_EXE, "S9RETR.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(retrigger)
    assert_external_native_preview(base)
    assert_external_native_preview(retrigger)


def test_iplayc_dos_s3m_extended_retrigger_same_path_stays_on_external_stream_placeholder() -> None:
    def write_extended_retrigger_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 2
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
            0x80, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 61 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_extended_retrigger_same_path_fixture("S3MS9", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MS9.S3M"))

    write_extended_retrigger_same_path_fixture("S3MS9", 0x13, 0x91)
    retrigger = combined_output(run_dos(IPLAYDIAG_EXE, "S3MS9.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(retrigger)
    assert_external_native_preview(base)
    assert_external_native_preview(retrigger)


def test_iplayc_dos_s3m_vibrato_stays_on_external_stream_placeholder() -> None:
    def write_vibrato_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 23 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_vibrato_fixture("VIBBASE", 0x00, 0x00)
    write_vibrato_fixture("VIBRATO", 0x08, 0x47)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "VIBBASE.S3M"))
    vibrato = combined_output(run_dos(IPLAYDIAG_EXE, "VIBRATO.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(vibrato)
    assert_external_native_preview(base)
    assert_external_native_preview(vibrato)


def test_iplayc_dos_s3m_vibrato_same_path_stays_on_external_stream_placeholder() -> None:
    def write_vibrato_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 23 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_vibrato_same_path_fixture("S3MVIB", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MVIB.S3M"))

    write_vibrato_same_path_fixture("S3MVIB", 0x08, 0x47)
    vibrato = combined_output(run_dos(IPLAYDIAG_EXE, "S3MVIB.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(vibrato)
    assert_external_native_preview(base)
    assert_external_native_preview(vibrato)


def test_iplayc_dos_s3m_fine_vibrato_stays_on_external_stream_placeholder() -> None:
    def write_fine_vibrato_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 79 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_fine_vibrato_fixture("UVIBASE", 0x00, 0x00)
    write_fine_vibrato_fixture("FINEVIB", 0x15, 0x37)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "UVIBASE.S3M"))
    fine_vibrato = combined_output(run_dos(IPLAYDIAG_EXE, "FINEVIB.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(fine_vibrato)
    assert_external_native_preview(base)
    assert_external_native_preview(fine_vibrato)


def test_iplayc_dos_s3m_fine_vibrato_same_path_stays_on_external_stream_placeholder() -> None:
    def write_fine_vibrato_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 79 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_fine_vibrato_same_path_fixture("S3MUVI", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MUVI.S3M"))

    write_fine_vibrato_same_path_fixture("S3MUVI", 0x15, 0x37)
    fine_vibrato = combined_output(run_dos(IPLAYDIAG_EXE, "S3MUVI.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(fine_vibrato)
    assert_external_native_preview(base)
    assert_external_native_preview(fine_vibrato)


def test_iplayc_dos_s3m_vibrato_volume_slide_stays_on_external_stream_placeholder() -> None:
    def write_vibrato_volume_slide_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 37 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_vibrato_volume_slide_fixture("KBASE", 0x00, 0x00)
    write_vibrato_volume_slide_fixture("KVIBVOL", 0x0B, 0x47)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "KBASE.S3M"))
    combined = combined_output(run_dos(IPLAYDIAG_EXE, "KVIBVOL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(combined)
    assert_external_native_preview(base)
    assert_external_native_preview(combined)


def test_iplayc_dos_s3m_vibrato_volume_slide_same_path_stays_on_external_stream_placeholder() -> None:
    def write_vibrato_volume_slide_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 37 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_vibrato_volume_slide_same_path_fixture("S3MKVS", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MKVS.S3M"))

    write_vibrato_volume_slide_same_path_fixture("S3MKVS", 0x0B, 0x47)
    combined = combined_output(run_dos(IPLAYDIAG_EXE, "S3MKVS.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(combined)
    assert_external_native_preview(base)
    assert_external_native_preview(combined)


def test_iplayc_dos_s3m_tremolo_stays_on_external_stream_placeholder() -> None:
    def write_tremolo_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 29 + (i >> 4)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_tremolo_fixture("TRMBASE", 0x00, 0x00)
    write_tremolo_fixture("TREMOLO", 0x12, 0x07)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "TRMBASE.S3M"))
    tremolo = combined_output(run_dos(IPLAYDIAG_EXE, "TREMOLO.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(tremolo)
    assert_external_native_preview(base)
    assert_external_native_preview(tremolo)


def test_iplayc_dos_s3m_tremolo_same_path_stays_on_external_stream_placeholder() -> None:
    def write_tremolo_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 29 + (i >> 4)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_tremolo_same_path_fixture("S3MTRM", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MTRM.S3M"))

    write_tremolo_same_path_fixture("S3MTRM", 0x12, 0x07)
    tremolo = combined_output(run_dos(IPLAYDIAG_EXE, "S3MTRM.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(tremolo)
    assert_external_native_preview(base)
    assert_external_native_preview(tremolo)


def test_iplayc_dos_s3m_tremor_stays_on_external_stream_placeholder() -> None:
    def write_tremor_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 43 + (i >> 4)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_tremor_fixture("TMRBASE", 0x00, 0x00)
    write_tremor_fixture("TREMOR", 0x09, 0x13)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "TMRBASE.S3M"))
    tremor = combined_output(run_dos(IPLAYDIAG_EXE, "TREMOR.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(tremor)
    assert_external_native_preview(base)
    assert_external_native_preview(tremor)


def test_iplayc_dos_s3m_tremor_same_path_stays_on_external_stream_placeholder() -> None:
    def write_tremor_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 43 + (i >> 4)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_tremor_same_path_fixture("S3MTMR", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MTMR.S3M"))

    write_tremor_same_path_fixture("S3MTMR", 0x09, 0x13)
    tremor = combined_output(run_dos(IPLAYDIAG_EXE, "S3MTMR.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(tremor)
    assert_external_native_preview(base)
    assert_external_native_preview(tremor)


def test_iplayc_dos_s3m_panning_stays_on_external_stream_placeholder() -> None:
    def write_panning_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 47 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_panning_fixture("PANBASE", 0x00, 0x00)
    write_panning_fixture("PANRIGHT", 0x18, 0xFF)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "PANBASE.S3M"))
    panned = combined_output(run_dos(IPLAYDIAG_EXE, "PANRIGHT.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(panned)
    assert_external_native_preview(base)
    assert_external_native_preview(panned)


def test_iplayc_dos_s3m_panning_same_path_stays_on_external_stream_placeholder() -> None:
    def write_panning_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 47 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_panning_same_path_fixture("S3MPAN", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPAN.S3M"))

    write_panning_same_path_fixture("S3MPAN", 0x18, 0xFF)
    panned = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPAN.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(panned)
    assert_external_native_preview(base)
    assert_external_native_preview(panned)


def test_iplayc_dos_s3m_panning_slide_stays_on_external_stream_placeholder() -> None:
    def write_panning_slide_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 97 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_panning_slide_fixture("PSLBASE", 0x00, 0x00)
    write_panning_slide_fixture("PANSLID", 0x10, 0x0F)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "PSLBASE.S3M"))
    slide = combined_output(run_dos(IPLAYDIAG_EXE, "PANSLID.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide)
    assert_external_native_preview(base)
    assert_external_native_preview(slide)


def test_iplayc_dos_s3m_panning_slide_same_path_stays_on_external_stream_placeholder() -> None:
    def write_panning_slide_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 97 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_panning_slide_same_path_fixture("S3MPSL", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPSL.S3M"))

    write_panning_slide_same_path_fixture("S3MPSL", 0x10, 0x0F)
    slide = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPSL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide)
    assert_external_native_preview(base)
    assert_external_native_preview(slide)


def test_iplayc_dos_s3m_panbrello_stays_on_external_stream_placeholder() -> None:
    def write_panbrello_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 107 + (i >> 4)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_panbrello_fixture("YBASE", 0x00, 0x00)
    write_panbrello_fixture("PANBREL", 0x19, 0x08)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "YBASE.S3M"))
    panbrello = combined_output(run_dos(IPLAYDIAG_EXE, "PANBREL.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(panbrello)
    assert_external_native_preview(base)
    assert_external_native_preview(panbrello)


def test_iplayc_dos_s3m_panbrello_same_path_stays_on_external_stream_placeholder() -> None:
    def write_panbrello_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 107 + (i >> 4)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_panbrello_same_path_fixture("S3MYBR", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MYBR.S3M"))

    write_panbrello_same_path_fixture("S3MYBR", 0x19, 0x08)
    panbrello = combined_output(run_dos(IPLAYDIAG_EXE, "S3MYBR.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(panbrello)
    assert_external_native_preview(base)
    assert_external_native_preview(panbrello)


def test_iplayc_dos_s3m_extended_panning_stays_on_external_stream_placeholder() -> None:
    def write_extended_panning_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 59 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_extended_panning_fixture("S8BASE", 0x00, 0x00)
    write_extended_panning_fixture("S8PANR", 0x13, 0x8F)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "S8BASE.S3M"))
    panned = combined_output(run_dos(IPLAYDIAG_EXE, "S8PANR.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(panned)
    assert_external_native_preview(base)
    assert_external_native_preview(panned)


def test_iplayc_dos_s3m_extended_panning_same_path_stays_on_external_stream_placeholder() -> None:
    def write_extended_panning_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 59 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_extended_panning_same_path_fixture("S3MS8P", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MS8P.S3M"))

    write_extended_panning_same_path_fixture("S3MS8P", 0x13, 0x8F)
    panned = combined_output(run_dos(IPLAYDIAG_EXE, "S3MS8P.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(panned)
    assert_external_native_preview(base)
    assert_external_native_preview(panned)


def test_iplayc_dos_mod_panning_stays_on_external_stream_placeholder() -> None:
    def write_mod_panning_fixture(name: str, effect: int, param: int) -> None:
        mod = bytearray(1084 + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, (effect & 0x0F), param & 0xFF])
        sample_base = 1084
        for i in range(0x280):
            mod[sample_base + i] = (i * 53 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_panning_fixture("MODPAN0", 0x00, 0x00)
    write_mod_panning_fixture("MODPANR", 0x08, 0xFF)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "MODPAN0.MOD"))
    panned = combined_output(run_dos(IPLAYDIAG_EXE, "MODPANR.MOD"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(panned)
    assert_external_native_preview(base)
    assert_external_native_preview(panned)


def test_iplayc_dos_s3m_note_cut_stays_on_external_stream_placeholder() -> None:
    def write_note_cut_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
            0x80, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 31 + (i >> 5)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_note_cut_fixture("CUTBASE", 0x00, 0x00)
    write_note_cut_fixture("NOTECUT", 0x13, 0xC0)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "CUTBASE.S3M"))
    note_cut = combined_output(run_dos(IPLAYDIAG_EXE, "NOTECUT.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(note_cut)
    assert_external_native_preview(base)
    assert_external_native_preview(note_cut)


def test_iplayc_dos_s3m_note_cut_same_path_stays_on_external_stream_placeholder() -> None:
    def write_note_cut_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0x60, 0x30, 0x01, 0x30, 0x00,
            0x80, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 31 + (i >> 5)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_note_cut_same_path_fixture("S3MCUT", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MCUT.S3M"))

    write_note_cut_same_path_fixture("S3MCUT", 0x13, 0xC0)
    note_cut = combined_output(run_dos(IPLAYDIAG_EXE, "S3MCUT.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(note_cut)
    assert_external_native_preview(base)
    assert_external_native_preview(note_cut)


def test_iplayc_dos_s3m_note_delay_stays_on_external_stream_placeholder() -> None:
    def write_note_delay_fixture(name: str, command: int, param: int, speed: int = 2) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = speed & 0xFF
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 73 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_note_delay_fixture("DLYBASE", 0x00, 0x00)
    write_note_delay_fixture("NOTEDELY", 0x13, 0xD1)
    write_note_delay_fixture("DLYLATE", 0x13, 0xD3)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "DLYBASE.S3M"))
    delayed = combined_output(run_dos(IPLAYDIAG_EXE, "NOTEDELY.S3M"))
    late = combined_output(run_dos(IPLAYDIAG_EXE, "DLYLATE.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(delayed)
    assert_bounded_sb16_playback(late)
    assert_external_native_preview(base)
    assert_external_native_preview(delayed)
    assert_external_native_preview(late)


def test_iplayc_dos_s3m_note_delay_same_path_stays_on_external_stream_placeholder() -> None:
    def write_note_delay_same_path_fixture(name: str, command: int, param: int, speed: int = 2) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = speed & 0xFF
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command, param, 0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 73 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_note_delay_same_path_fixture("S3MDLY", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MDLY.S3M"))

    write_note_delay_same_path_fixture("S3MDLY", 0x13, 0xD1)
    delayed = combined_output(run_dos(IPLAYDIAG_EXE, "S3MDLY.S3M"))

    write_note_delay_same_path_fixture("S3MDLY", 0x13, 0xD3)
    late = combined_output(run_dos(IPLAYDIAG_EXE, "S3MDLY.S3M"))

    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(delayed)
    assert_bounded_sb16_playback(late)
    assert_external_native_preview(base)
    assert_external_native_preview(delayed)
    assert_external_native_preview(late)


def test_iplayc_dos_s3m_pattern_break_stays_on_external_stream_placeholder() -> None:
    def write_pattern_break_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x900)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (2).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (2).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61] = 1
        s3m[0x62:0x64] = (0x10).to_bytes(2, "little")
        s3m[0x64:0x66] = (0x15).to_bytes(2, "little")
        s3m[0x66:0x68] = (0x18).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern0_offset = 0x150
        pattern1_offset = 0x180
        sample_offset = 0x300
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern0_data = bytes([0x80, command & 0xFF, param & 0xFF, 0x00])
        pattern1_data = bytes([
            0x00,
            0x00,
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern0_offset:pattern0_offset + 2] = (len(pattern0_data) + 2).to_bytes(2, "little")
        s3m[pattern0_offset + 2:pattern0_offset + 2 + len(pattern0_data)] = pattern0_data
        s3m[pattern1_offset:pattern1_offset + 2] = (len(pattern1_data) + 2).to_bytes(2, "little")
        s3m[pattern1_offset + 2:pattern1_offset + 2 + len(pattern1_data)] = pattern1_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 91 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_pattern_break_fixture("PBRBASE", 0x00, 0x00)
    write_pattern_break_fixture("PBREAK", 0x03, 0x02)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "PBRBASE.S3M"))
    broken = combined_output(run_dos(IPLAYDIAG_EXE, "PBREAK.S3M"))

    assert_decoder_geometry(base, 2, 64, 0, 1, 125, 1)
    assert_decoder_geometry(broken, 2, 64, 0, 1, 125, 1)
    assert_bounded_sb16_playback_blocks(base, 64)
    assert_bounded_source_end_playback(broken, 63)
    assert_decoder_progress(broken, 63, 128, 2, 1, 0, 0, 0, 1, 125, 1, 0)
    assert_external_native_preview(base)
    assert_external_native_preview(broken)


def test_iplayc_dos_s3m_pattern_break_same_path_stays_on_external_stream_placeholder() -> None:
    def write_pattern_break_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x900)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (2).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (2).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61] = 1
        s3m[0x62:0x64] = (0x10).to_bytes(2, "little")
        s3m[0x64:0x66] = (0x15).to_bytes(2, "little")
        s3m[0x66:0x68] = (0x18).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern0_offset = 0x150
        pattern1_offset = 0x180
        sample_offset = 0x300
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern0_data = bytes([0x80, command & 0xFF, param & 0xFF, 0x00])
        pattern1_data = bytes([
            0x00,
            0x00,
            0x60, 0x30, 0x01, 0x30, 0x00,
        ])
        s3m[pattern0_offset:pattern0_offset + 2] = (len(pattern0_data) + 2).to_bytes(2, "little")
        s3m[pattern0_offset + 2:pattern0_offset + 2 + len(pattern0_data)] = pattern0_data
        s3m[pattern1_offset:pattern1_offset + 2] = (len(pattern1_data) + 2).to_bytes(2, "little")
        s3m[pattern1_offset + 2:pattern1_offset + 2 + len(pattern1_data)] = pattern1_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 91 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_pattern_break_same_path_fixture("S3MPBR", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPBR.S3M"))

    write_pattern_break_same_path_fixture("S3MPBR", 0x03, 0x02)
    broken = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPBR.S3M"))

    assert_decoder_geometry(base, 2, 64, 0, 1, 125, 1)
    assert_decoder_geometry(broken, 2, 64, 0, 1, 125, 1)
    assert_bounded_sb16_playback_blocks(base, 64)
    assert_bounded_source_end_playback(broken, 63)
    assert_decoder_progress(broken, 63, 128, 2, 1, 0, 0, 0, 1, 125, 1, 0)
    assert_external_native_preview(base)
    assert_external_native_preview(broken)


def test_iplayc_dos_s3m_position_jump_stays_on_external_stream_placeholder() -> None:
    def write_position_jump_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x900)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (3).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (3).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61] = 1
        s3m[0x62] = 2
        s3m[0x63:0x65] = (0x10).to_bytes(2, "little")
        s3m[0x65:0x67] = (0x15).to_bytes(2, "little")
        s3m[0x67:0x69] = (0x18).to_bytes(2, "little")
        s3m[0x69:0x6B] = (0x1B).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern0_offset = 0x150
        pattern1_offset = 0x180
        pattern2_offset = 0x1B0
        sample_offset = 0x300
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern0_data = bytes([0x80, command & 0xFF, param & 0xFF, 0x00])
        pattern1_data = bytes([0x60, 0x30, 0x01, 0x10, 0x00])
        pattern2_data = bytes([0x60, 0x34, 0x01, 0x30, 0x00])
        s3m[pattern0_offset:pattern0_offset + 2] = (len(pattern0_data) + 2).to_bytes(2, "little")
        s3m[pattern0_offset + 2:pattern0_offset + 2 + len(pattern0_data)] = pattern0_data
        s3m[pattern1_offset:pattern1_offset + 2] = (len(pattern1_data) + 2).to_bytes(2, "little")
        s3m[pattern1_offset + 2:pattern1_offset + 2 + len(pattern1_data)] = pattern1_data
        s3m[pattern2_offset:pattern2_offset + 2] = (len(pattern2_data) + 2).to_bytes(2, "little")
        s3m[pattern2_offset + 2:pattern2_offset + 2 + len(pattern2_data)] = pattern2_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 107 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_position_jump_fixture("PJBASE", 0x00, 0x00)
    write_position_jump_fixture("POSJUMP", 0x02, 0x02)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "PJBASE.S3M"))
    jumped = combined_output(run_dos(IPLAYDIAG_EXE, "POSJUMP.S3M"))

    assert_decoder_geometry(base, 3, 64, 0, 1, 125, 1)
    assert_decoder_geometry(jumped, 3, 64, 0, 1, 125, 1)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(jumped)
    assert_external_native_preview(base)
    assert_external_native_preview(jumped)


def test_iplayc_dos_s3m_position_jump_same_path_stays_on_external_stream_placeholder() -> None:
    def write_position_jump_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x900)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (3).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (3).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61] = 1
        s3m[0x62] = 2
        s3m[0x63:0x65] = (0x10).to_bytes(2, "little")
        s3m[0x65:0x67] = (0x15).to_bytes(2, "little")
        s3m[0x67:0x69] = (0x18).to_bytes(2, "little")
        s3m[0x69:0x6B] = (0x1B).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern0_offset = 0x150
        pattern1_offset = 0x180
        pattern2_offset = 0x1B0
        sample_offset = 0x300
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern0_data = bytes([0x80, command & 0xFF, param & 0xFF, 0x00])
        pattern1_data = bytes([0x60, 0x30, 0x01, 0x10, 0x00])
        pattern2_data = bytes([0x60, 0x34, 0x01, 0x30, 0x00])
        s3m[pattern0_offset:pattern0_offset + 2] = (len(pattern0_data) + 2).to_bytes(2, "little")
        s3m[pattern0_offset + 2:pattern0_offset + 2 + len(pattern0_data)] = pattern0_data
        s3m[pattern1_offset:pattern1_offset + 2] = (len(pattern1_data) + 2).to_bytes(2, "little")
        s3m[pattern1_offset + 2:pattern1_offset + 2 + len(pattern1_data)] = pattern1_data
        s3m[pattern2_offset:pattern2_offset + 2] = (len(pattern2_data) + 2).to_bytes(2, "little")
        s3m[pattern2_offset + 2:pattern2_offset + 2 + len(pattern2_data)] = pattern2_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 107 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_position_jump_same_path_fixture("S3MPJ", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPJ.S3M"))

    write_position_jump_same_path_fixture("S3MPJ", 0x02, 0x02)
    jumped = combined_output(run_dos(IPLAYDIAG_EXE, "S3MPJ.S3M"))

    assert_decoder_geometry(base, 3, 64, 0, 1, 125, 1)
    assert_decoder_geometry(jumped, 3, 64, 0, 1, 125, 1)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(jumped)
    assert_external_native_preview(base)
    assert_external_native_preview(jumped)


def test_iplayc_dos_s3m_pattern_loop_same_path_stays_on_external_stream_placeholder() -> None:
    def write_pattern_loop_same_path_fixture(name: str, loop_enabled: bool) -> None:
        s3m = bytearray(0x900)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x300
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        loop_start = bytes([0x80, 0x13, 0x60, 0x00]) if loop_enabled else bytes([0x00])
        loop_end = bytes([0x80, 0x13, 0x62, 0x00]) if loop_enabled else bytes([0x00])
        pattern_data = (
            loop_start +
            bytes([0x60, 0x30, 0x01, 0x30, 0x00]) +
            loop_end +
            bytes([0x60, 0x34, 0x01, 0x20, 0x00])
        )
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 101 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_pattern_loop_same_path_fixture("S3MS6L", False)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MS6L.S3M"))

    write_pattern_loop_same_path_fixture("S3MS6L", True)
    looped = combined_output(run_dos(IPLAYDIAG_EXE, "S3MS6L.S3M"))

    assert_decoder_geometry(base, 1, 64, 0, 1, 125, 1)
    assert_decoder_geometry(looped, 1, 64, 0, 1, 125, 1)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(looped)
    assert_external_native_preview(base)
    assert_external_native_preview(looped)


def test_iplayc_dos_mod_pattern_break_stays_on_external_stream_placeholder() -> None:
    def put_mod_event(mod: bytearray, pattern: int, row: int, channel: int, event: bytes) -> None:
        offset = 1084 + pattern * 1024 + row * 16 + channel * 4
        mod[offset:offset + 4] = event

    def write_mod_pattern_break_fixture(name: str, effect: int, param: int) -> None:
        pattern_count = 2
        sample_base = 1084 + pattern_count * 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 2
        mod[952] = 0
        mod[953] = 1
        mod[1080:1084] = b"M.K."
        put_mod_event(mod, 0, 0, 0, bytes([0x00, 0x00, effect & 0x0F, param & 0xFF]))
        put_mod_event(mod, 1, 2, 0, bytes([0x03, 0x57, 0x10, 0x00]))
        for i in range(0x280):
            mod[sample_base + i] = (i * 97 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_pattern_break_fixture("MDBASE", 0x00, 0x00)
    write_mod_pattern_break_fixture("MDBREAK", 0x0D, 0x02)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "MDBASE.MOD"))
    broken = combined_output(run_dos(IPLAYDIAG_EXE, "MDBREAK.MOD"))

    assert_decoder_geometry(base, 2, 64, 0, 6, 125, 4)
    assert_decoder_geometry(broken, 2, 64, 0, 6, 125, 4)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(broken)
    assert_external_native_preview(base)
    assert_external_native_preview(broken)


def test_iplayc_dos_mod_position_jump_stays_on_external_stream_placeholder() -> None:
    def put_mod_event(mod: bytearray, pattern: int, row: int, channel: int, event: bytes) -> None:
        offset = 1084 + pattern * 1024 + row * 16 + channel * 4
        mod[offset:offset + 4] = event

    def write_mod_position_jump_fixture(name: str, effect: int, param: int) -> None:
        pattern_count = 3
        sample_base = 1084 + pattern_count * 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 3
        mod[952] = 0
        mod[953] = 1
        mod[954] = 2
        mod[1080:1084] = b"M.K."
        put_mod_event(mod, 0, 0, 0, bytes([0x00, 0x00, effect & 0x0F, param & 0xFF]))
        put_mod_event(mod, 1, 0, 0, bytes([0x03, 0x57, 0x10, 0x00]))
        put_mod_event(mod, 2, 0, 0, bytes([0x03, 0x20, 0x10, 0x00]))
        for i in range(0x280):
            mod[sample_base + i] = (i * 109 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_position_jump_fixture("MJBASE", 0x00, 0x00)
    write_mod_position_jump_fixture("MODJUMP", 0x0B, 0x02)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "MJBASE.MOD"))
    jumped = combined_output(run_dos(IPLAYDIAG_EXE, "MODJUMP.MOD"))

    assert_decoder_geometry(base, 3, 64, 0, 6, 125, 4)
    assert_decoder_geometry(jumped, 3, 64, 0, 6, 125, 4)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(jumped)
    assert_external_native_preview(base)
    assert_external_native_preview(jumped)


def test_iplayc_dos_mod_pattern_break_same_path_stays_on_external_stream_placeholder() -> None:
    def put_mod_event(mod: bytearray, pattern: int, row: int, channel: int, event: bytes) -> None:
        offset = 1084 + pattern * 1024 + row * 16 + channel * 4
        mod[offset:offset + 4] = event

    def write_mod_pattern_break_same_path_fixture(name: str, effect: int, param: int) -> None:
        pattern_count = 2
        sample_base = 1084 + pattern_count * 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 2
        mod[952] = 0
        mod[953] = 1
        mod[1080:1084] = b"M.K."
        put_mod_event(mod, 0, 0, 0, bytes([0x00, 0x00, effect & 0x0F, param & 0xFF]))
        put_mod_event(mod, 1, 2, 0, bytes([0x03, 0x57, 0x10, 0x00]))
        for i in range(0x280):
            mod[sample_base + i] = (i * 97 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_pattern_break_same_path_fixture("MDBRSP", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MDBRSP.MOD"))

    write_mod_pattern_break_same_path_fixture("MDBRSP", 0x0D, 0x02)
    broken = combined_output(run_dos(IPLAYDIAG_EXE, "MDBRSP.MOD"))

    assert_decoder_geometry(base, 2, 64, 0, 6, 125, 4)
    assert_decoder_geometry(broken, 2, 64, 0, 6, 125, 4)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(broken)
    assert_external_native_preview(base)
    assert_external_native_preview(broken)


def test_iplayc_dos_mod_position_jump_same_path_stays_on_external_stream_placeholder() -> None:
    def put_mod_event(mod: bytearray, pattern: int, row: int, channel: int, event: bytes) -> None:
        offset = 1084 + pattern * 1024 + row * 16 + channel * 4
        mod[offset:offset + 4] = event

    def write_mod_position_jump_same_path_fixture(name: str, effect: int, param: int) -> None:
        pattern_count = 3
        sample_base = 1084 + pattern_count * 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 3
        mod[952] = 0
        mod[953] = 1
        mod[954] = 2
        mod[1080:1084] = b"M.K."
        put_mod_event(mod, 0, 0, 0, bytes([0x00, 0x00, effect & 0x0F, param & 0xFF]))
        put_mod_event(mod, 1, 0, 0, bytes([0x03, 0x57, 0x10, 0x00]))
        put_mod_event(mod, 2, 0, 0, bytes([0x03, 0x20, 0x10, 0x00]))
        for i in range(0x280):
            mod[sample_base + i] = (i * 109 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_position_jump_same_path_fixture("MJUMPSP", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MJUMPSP.MOD"))

    write_mod_position_jump_same_path_fixture("MJUMPSP", 0x0B, 0x02)
    jumped = combined_output(run_dos(IPLAYDIAG_EXE, "MJUMPSP.MOD"))

    assert_decoder_geometry(base, 3, 64, 0, 6, 125, 4)
    assert_decoder_geometry(jumped, 3, 64, 0, 6, 125, 4)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(jumped)
    assert_external_native_preview(base)
    assert_external_native_preview(jumped)


def test_iplayc_dos_s3m_speed_command_stays_on_external_stream_placeholder() -> None:
    def write_speed_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command & 0xFF, param & 0xFF,
            0x60, 0x34, 0x01, 0x20,
            0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 113 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_speed_fixture("SPDBASE", 0x00, 0x00)
    write_speed_fixture("SPDSET3", 0x01, 0x03)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "SPDBASE.S3M"))
    slow = combined_output(run_dos(IPLAYDIAG_EXE, "SPDSET3.S3M"))

    assert_decoder_geometry(base, 1, 64, 0, 1, 125, 1)
    assert_decoder_geometry(slow, 1, 64, 0, 1, 125, 1)
    assert_decoder_progress(base, 64, 64, 1, 0, 0, 0, 0, 1, 125, 1, 0)
    assert_decoder_progress(slow, 64, 64, 0, 0, 21, 0, 1, 3, 125, 0, 0)
    assert_external_native_preview(base)
    assert_external_native_preview(slow)


def test_iplayc_dos_s3m_speed_command_same_path_stays_on_external_stream_placeholder() -> None:
    def write_speed_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command & 0xFF, param & 0xFF,
            0x60, 0x34, 0x01, 0x20,
            0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 113 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_speed_same_path_fixture("S3MSPD", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MSPD.S3M"))

    write_speed_same_path_fixture("S3MSPD", 0x01, 0x03)
    slow = combined_output(run_dos(IPLAYDIAG_EXE, "S3MSPD.S3M"))

    assert_decoder_geometry(base, 1, 64, 0, 1, 125, 1)
    assert_decoder_geometry(slow, 1, 64, 0, 1, 125, 1)
    assert_decoder_progress(base, 64, 64, 1, 0, 0, 0, 0, 1, 125, 1, 0)
    assert_decoder_progress(slow, 64, 64, 0, 0, 21, 0, 1, 3, 125, 0, 0)
    assert_external_native_preview(base)
    assert_external_native_preview(slow)


def test_iplayc_dos_s3m_tempo_command_reports_runtime_tempo_metadata_on_external_stream_placeholder() -> None:
    def write_tempo_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command & 0xFF, param & 0xFF,
            0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 71 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_tempo_fixture("TMPBASE", 0x00, 0x00)
    write_tempo_fixture("TMP180", 0x14, 180)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "TMPBASE.S3M"))
    faster = combined_output(run_dos(IPLAYDIAG_EXE, "TMP180.S3M"))

    assert_decoder_geometry(base, 1, 64, 0, 1, 125, 1)
    assert_decoder_geometry(faster, 1, 64, 0, 1, 125, 1)
    assert_decoder_event(base, 214, 1, 4, 1, 48, 0, 0)
    assert_decoder_event(faster, 214, 1, 4, 1, 48, 15, 180)
    assert_decoder_progress(base, 64, 64, 1, 0, 0, 0, 0, 1, 125, 1, 0)
    assert_decoder_progress(faster, 64, 64, 1, 0, 0, 0, 0, 1, 180, 1, 0)


def test_iplayc_dos_s3m_tempo_command_same_path_reports_runtime_tempo_metadata_on_external_stream_placeholder() -> None:
    def write_tempo_same_path_fixture(name: str, command: int, param: int) -> None:
        s3m = bytearray(0x500)
        s3m[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        s3m[0x20:0x22] = (1).to_bytes(2, "little")
        s3m[0x22:0x24] = (1).to_bytes(2, "little")
        s3m[0x24:0x26] = (1).to_bytes(2, "little")
        s3m[0x2C:0x30] = b"SCRM"
        s3m[0x31] = 1
        s3m[0x32] = 125
        s3m[0x40] = 0
        s3m[0x41:0x60] = bytes([0xFF]) * 31
        s3m[0x60] = 0
        s3m[0x61:0x63] = (0x10).to_bytes(2, "little")
        s3m[0x63:0x65] = (0x15).to_bytes(2, "little")

        instrument_offset = 0x100
        pattern_offset = 0x150
        sample_offset = 0x200
        s3m[instrument_offset] = 1
        s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
        s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
        s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
        s3m[instrument_offset + 28] = 48
        s3m[instrument_offset + 32:instrument_offset + 36] = (8363).to_bytes(4, "little")
        s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

        pattern_data = bytes([
            0xE0, 0x30, 0x01, 0x30, command & 0xFF, param & 0xFF,
            0x00,
        ])
        s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
        s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
        for i in range(0x280):
            s3m[sample_offset + i] = (i * 71 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.S3M").write_bytes(s3m)

    write_tempo_same_path_fixture("S3MTMP", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "S3MTMP.S3M"))

    write_tempo_same_path_fixture("S3MTMP", 0x14, 180)
    faster = combined_output(run_dos(IPLAYDIAG_EXE, "S3MTMP.S3M"))

    assert_decoder_geometry(base, 1, 64, 0, 1, 125, 1)
    assert_decoder_geometry(faster, 1, 64, 0, 1, 125, 1)
    assert_decoder_event(base, 214, 1, 4, 1, 48, 0, 0)
    assert_decoder_event(faster, 214, 1, 4, 1, 48, 15, 180)
    assert_decoder_progress(base, 64, 64, 1, 0, 0, 0, 0, 1, 125, 1, 0)
    assert_decoder_progress(faster, 64, 64, 1, 0, 0, 0, 0, 1, 180, 1, 0)


def test_iplayc_dos_mod_tempo_command_reports_runtime_tempo_metadata_on_external_stream_placeholder() -> None:
    def write_mod_tempo_fixture(name: str, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x1F, param & 0xFF])
        for i in range(0x280):
            mod[sample_base + i] = (i * 89 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_tempo_fixture("FTMP125", 125)
    write_mod_tempo_fixture("FTMP180", 180)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "FTMP125.MOD"))
    faster = combined_output(run_dos(IPLAYDIAG_EXE, "FTMP180.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(faster, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 15, 125)
    assert_decoder_event(faster, 855, 1, 1, 1, 0, 15, 180)
    assert_decoder_progress(base, 64, 1536, 0, 0, 2, 0, 4, 6, 125, 0, 0)
    assert_decoder_progress(faster, 64, 1536, 0, 0, 2, 0, 4, 6, 180, 0, 0)


def test_iplayc_dos_mod_speed_command_same_path_stays_on_external_stream_placeholder() -> None:
    def write_mod_speed_same_path_fixture(name: str, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x1F, param & 0xFF])
        for i in range(0x280):
            mod[sample_base + i] = (i * 89 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_speed_same_path_fixture("MFSPD", 0x06)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MFSPD.MOD"))

    write_mod_speed_same_path_fixture("MFSPD", 0x03)
    faster_rows = combined_output(run_dos(IPLAYDIAG_EXE, "MFSPD.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(faster_rows, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 15, 6)
    assert_decoder_event(faster_rows, 855, 1, 1, 1, 0, 15, 3)
    assert_decoder_progress(base, 64, 1536, 0, 0, 2, 0, 4, 6, 125, 0, 0)
    assert_decoder_progress(faster_rows, 64, 1536, 0, 0, 5, 0, 1, 3, 125, 0, 0)
    assert_external_native_preview(base)
    assert_external_native_preview(faster_rows)


def test_iplayc_dos_mod_tempo_command_same_path_reports_runtime_tempo_metadata_on_external_stream_placeholder() -> None:
    def write_mod_tempo_same_path_fixture(name: str, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x1F, param & 0xFF])
        for i in range(0x280):
            mod[sample_base + i] = (i * 89 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_tempo_same_path_fixture("MFTMP", 125)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MFTMP.MOD"))

    write_mod_tempo_same_path_fixture("MFTMP", 180)
    faster = combined_output(run_dos(IPLAYDIAG_EXE, "MFTMP.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(faster, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 15, 125)
    assert_decoder_event(faster, 855, 1, 1, 1, 0, 15, 180)
    assert_decoder_progress(base, 64, 1536, 0, 0, 2, 0, 4, 6, 125, 0, 0)
    assert_decoder_progress(faster, 64, 1536, 0, 0, 2, 0, 4, 6, 180, 0, 0)


def test_iplayc_dos_mod_sample_loop_reports_metadata_on_external_stream_placeholder() -> None:
    def write_mod_loop_fixture(name: str, loop_words: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 48
        mod[46:48] = (4).to_bytes(2, "big")
        mod[48:50] = (loop_words & 0xFFFF).to_bytes(2, "big")
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10, 0x00])
        for i in range(0x280):
            mod[sample_base + i] = (i * 127 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_loop_fixture("MLOOPNO", 1)
    write_mod_loop_fixture("MLOOPY", 0x20)

    no_loop = combined_output(run_dos(IPLAYDIAG_EXE, "MLOOPNO.MOD"))
    looped = combined_output(run_dos(IPLAYDIAG_EXE, "MLOOPY.MOD"))

    assert_decoder_geometry(no_loop, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(looped, 1, 64, 0, 6, 125, 4)
    assert_decoder_voice(no_loop, 1, 855, 1, 1, 1, 48, 640, 48, 8, 2, 2108)
    assert_decoder_voice(looped, 1, 855, 1, 1, 1, 48, 640, 48, 8, 64, 2108)
    assert_bounded_sb16_playback(no_loop)
    assert_bounded_sb16_playback(looped)
    assert_external_native_preview(no_loop)
    assert_external_native_preview(looped)


def test_iplayc_dos_mod_sample_loop_same_path_reports_metadata_on_external_stream_placeholder() -> None:
    def write_mod_loop_same_path_fixture(name: str, loop_words: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 48
        mod[46:48] = (4).to_bytes(2, "big")
        mod[48:50] = (loop_words & 0xFFFF).to_bytes(2, "big")
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10, 0x00])
        for i in range(0x280):
            mod[sample_base + i] = (i * 127 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_loop_same_path_fixture("MDLOOP", 1)
    no_loop = combined_output(run_dos(IPLAYDIAG_EXE, "MDLOOP.MOD"))

    write_mod_loop_same_path_fixture("MDLOOP", 0x20)
    looped = combined_output(run_dos(IPLAYDIAG_EXE, "MDLOOP.MOD"))

    assert_decoder_geometry(no_loop, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(looped, 1, 64, 0, 6, 125, 4)
    assert_decoder_voice(no_loop, 1, 855, 1, 1, 1, 48, 640, 48, 8, 2, 2108)
    assert_decoder_voice(looped, 1, 855, 1, 1, 1, 48, 640, 48, 8, 64, 2108)
    assert_bounded_sb16_playback(no_loop)
    assert_bounded_sb16_playback(looped)
    assert_external_native_preview(no_loop)
    assert_external_native_preview(looped)


def test_iplayc_dos_mod_volume_command_stays_on_external_stream_placeholder() -> None:
    def write_mod_volume_fixture(name: str, volume: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 64
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x1C, volume & 0xFF])
        for i in range(0x280):
            mod[sample_base + i] = (i * 103 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_volume_fixture("MVOL32", 32)
    write_mod_volume_fixture("MVOL64", 64)

    quiet = combined_output(run_dos(IPLAYDIAG_EXE, "MVOL32.MOD"))
    loud = combined_output(run_dos(IPLAYDIAG_EXE, "MVOL64.MOD"))

    assert_decoder_geometry(quiet, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(loud, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(quiet, 855, 1, 1, 1, 32, 12, 32)
    assert_decoder_event(loud, 855, 1, 1, 1, 64, 12, 64)
    assert_bounded_sb16_playback(quiet)
    assert_bounded_sb16_playback(loud)
    assert_external_native_preview(quiet)
    assert_external_native_preview(loud)


def test_iplayc_dos_mod_volume_command_same_path_stays_on_external_stream_placeholder() -> None:
    def write_mod_volume_same_path_fixture(name: str, volume: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x280)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x140).to_bytes(2, "big")
        mod[45] = 64
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x1C, volume & 0xFF])
        for i in range(0x280):
            mod[sample_base + i] = (i * 103 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_volume_same_path_fixture("MDVOL", 32)
    quiet = combined_output(run_dos(IPLAYDIAG_EXE, "MDVOL.MOD"))

    write_mod_volume_same_path_fixture("MDVOL", 64)
    loud = combined_output(run_dos(IPLAYDIAG_EXE, "MDVOL.MOD"))

    assert_decoder_geometry(quiet, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(loud, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(quiet, 855, 1, 1, 1, 32, 12, 32)
    assert_decoder_event(loud, 855, 1, 1, 1, 64, 12, 64)
    assert_bounded_sb16_playback(quiet)
    assert_bounded_sb16_playback(loud)
    assert_external_native_preview(quiet)
    assert_external_native_preview(loud)


def test_iplayc_dos_mod_sample_offset_stays_on_external_stream_placeholder() -> None:
    def write_mod_offset_fixture(name: str, offset_param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x19, offset_param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 43 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_offset_fixture("MOFF0", 0x00)
    write_mod_offset_fixture("MOFF1", 0x01)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "MOFF0.MOD"))
    shifted = combined_output(run_dos(IPLAYDIAG_EXE, "MOFF1.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(shifted, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 9, 0)
    assert_decoder_event(shifted, 855, 1, 1, 1, 0, 9, 1)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(shifted)
    assert_external_native_preview(base)
    assert_external_native_preview(shifted)


def test_iplayc_dos_mod_sample_offset_same_path_stays_on_external_stream_placeholder() -> None:
    def write_mod_offset_same_path_fixture(name: str, offset_param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x19, offset_param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 43 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_offset_same_path_fixture("MDOFF", 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MDOFF.MOD"))

    write_mod_offset_same_path_fixture("MDOFF", 0x01)
    shifted = combined_output(run_dos(IPLAYDIAG_EXE, "MDOFF.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(shifted, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 9, 0)
    assert_decoder_event(shifted, 855, 1, 1, 1, 0, 9, 1)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(shifted)
    assert_external_native_preview(base)
    assert_external_native_preview(shifted)


def test_iplayc_dos_mod_arpeggio_stays_on_external_stream_placeholder() -> None:
    def write_mod_arpeggio_fixture(name: str, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10, param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 31 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_arpeggio_fixture("MARP0", 0x00)
    write_mod_arpeggio_fixture("MARP31", 0x31)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "MARP0.MOD"))
    arpeggio = combined_output(run_dos(IPLAYDIAG_EXE, "MARP31.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(arpeggio, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(arpeggio, 855, 1, 1, 1, 0, 0, 49)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(arpeggio)
    assert_external_native_preview(base)
    assert_external_native_preview(arpeggio)




def test_iplayc_dos_mod_arpeggio_same_path_stays_on_external_stream_placeholder() -> None:
    def write_mod_arpeggio_same_path_fixture(name: str, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10, param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 31 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_arpeggio_same_path_fixture("MDARP", 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MDARP.MOD"))

    write_mod_arpeggio_same_path_fixture("MDARP", 0x31)
    arpeggio = combined_output(run_dos(IPLAYDIAG_EXE, "MDARP.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(arpeggio, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(arpeggio, 855, 1, 1, 1, 0, 0, 49)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(arpeggio)
    assert_external_native_preview(base)
    assert_external_native_preview(arpeggio)

def test_iplayc_dos_mod_pitch_slide_stays_on_external_stream_placeholder() -> None:
    def write_mod_pitch_slide_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10 | (effect & 0x0F), param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 37 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_pitch_slide_fixture("MPSBASE", 0x00, 0x00)
    write_mod_pitch_slide_fixture("MPSUP", 0x01, 0x20)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "MPSBASE.MOD"))
    shifted = combined_output(run_dos(IPLAYDIAG_EXE, "MPSUP.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(shifted, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(shifted, 855, 1, 1, 1, 0, 1, 32)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(shifted)
    assert_external_native_preview(base)
    assert_external_native_preview(shifted)


def test_iplayc_dos_mod_pitch_slide_down_stays_on_external_stream_placeholder() -> None:
    def write_mod_pitch_slide_down_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10 | (effect & 0x0F), param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 41 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_pitch_slide_down_fixture("MPD0", 0x00, 0x00)
    write_mod_pitch_slide_down_fixture("MPDOWN", 0x02, 0x20)

    base = combined_output(run_dos(IPLAYDIAG_EXE, "MPD0.MOD"))
    shifted = combined_output(run_dos(IPLAYDIAG_EXE, "MPDOWN.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(shifted, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(shifted, 855, 1, 1, 1, 0, 2, 32)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(shifted)
    assert_external_native_preview(base)
    assert_external_native_preview(shifted)




def test_iplayc_dos_mod_pitch_slide_same_path_stays_on_external_stream_placeholder() -> None:
    def write_mod_pitch_slide_same_path_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10 | (effect & 0x0F), param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 37 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_pitch_slide_same_path_fixture("MDPSL", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MDPSL.MOD"))

    write_mod_pitch_slide_same_path_fixture("MDPSL", 0x01, 0x20)
    slide_up = combined_output(run_dos(IPLAYDIAG_EXE, "MDPSL.MOD"))

    write_mod_pitch_slide_same_path_fixture("MDPSL", 0x02, 0x20)
    slide_down = combined_output(run_dos(IPLAYDIAG_EXE, "MDPSL.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(slide_up, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(slide_down, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(slide_up, 855, 1, 1, 1, 0, 1, 32)
    assert_decoder_event(slide_down, 855, 1, 1, 1, 0, 2, 32)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide_up)
    assert_bounded_sb16_playback(slide_down)
    assert_external_native_preview(base)
    assert_external_native_preview(slide_up)
    assert_external_native_preview(slide_down)
    assert_external_native_preview(slide_down)

def test_iplayc_dos_mod_tone_portamento_stays_on_external_stream_placeholder() -> None:
    def put_mod_event(mod: bytearray, row: int, event: bytes) -> None:
        offset = 1084 + row * 16
        mod[offset:offset + 4] = event

    def write_mod_tone_portamento_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        put_mod_event(mod, 0, bytes([0x03, 0x57, 0x10, 0x00]))
        put_mod_event(mod, 1, bytes([0x03, 0x20, 0x10 | (effect & 0x0F), param & 0xFF]))
        for i in range(0x400):
            mod[sample_base + i] = (i * 47 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_tone_portamento_fixture("MTPORT", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MTPORT.MOD"))

    write_mod_tone_portamento_fixture("MTPORT", 0x03, 0x18)
    portamento = combined_output(run_dos(IPLAYDIAG_EXE, "MTPORT.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(portamento, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_voice(portamento, 0, 800, 2, 1, 1, 48, 1024, 48, 0, 0, 2108)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(portamento)
    assert_external_native_preview(base)
    assert_external_native_preview(portamento)


def test_iplayc_dos_mod_vibrato_stays_on_external_stream_placeholder() -> None:
    def write_mod_vibrato_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10 | (effect & 0x0F), param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 53 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_vibrato_fixture("MVIB", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MVIB.MOD"))

    write_mod_vibrato_fixture("MVIB", 0x04, 0x47)
    vibrato = combined_output(run_dos(IPLAYDIAG_EXE, "MVIB.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(vibrato, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(vibrato, 855, 1, 1, 1, 0, 4, 71)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(vibrato)
    assert_external_native_preview(base)
    assert_external_native_preview(vibrato)


def test_iplayc_dos_mod_tremolo_stays_on_external_stream_placeholder() -> None:
    def write_mod_tremolo_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10 | (effect & 0x0F), param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 59 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_tremolo_fixture("MTRM", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MTRM.MOD"))

    write_mod_tremolo_fixture("MTRM", 0x07, 0x37)
    tremolo = combined_output(run_dos(IPLAYDIAG_EXE, "MTRM.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(tremolo, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(tremolo, 855, 1, 1, 1, 0, 7, 55)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(tremolo)
    assert_external_native_preview(base)
    assert_external_native_preview(tremolo)


def test_iplayc_dos_mod_volume_slide_stays_on_external_stream_placeholder() -> None:
    def write_mod_volume_slide_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10 | (effect & 0x0F), param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 61 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_volume_slide_fixture("MVSLD", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MVSLD.MOD"))

    write_mod_volume_slide_fixture("MVSLD", 0x0A, 0x0F)
    slide = combined_output(run_dos(IPLAYDIAG_EXE, "MVSLD.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(slide, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(slide, 855, 1, 1, 1, 0, 10, 15)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide)
    assert_external_native_preview(base)
    assert_external_native_preview(slide)




def test_iplayc_dos_mod_volume_slide_same_path_up_down_stays_on_external_stream_placeholder() -> None:
    def write_mod_volume_slide_same_path_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 32
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10 | (effect & 0x0F), param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 61 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_volume_slide_same_path_fixture("MDVSL", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MDVSL.MOD"))

    write_mod_volume_slide_same_path_fixture("MDVSL", 0x0A, 0x20)
    slide_up = combined_output(run_dos(IPLAYDIAG_EXE, "MDVSL.MOD"))

    write_mod_volume_slide_same_path_fixture("MDVSL", 0x0A, 0x02)
    slide_down = combined_output(run_dos(IPLAYDIAG_EXE, "MDVSL.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(slide_up, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(slide_down, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(slide_up, 855, 1, 1, 1, 0, 10, 32)
    assert_decoder_event(slide_down, 855, 1, 1, 1, 0, 10, 2)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(slide_up)
    assert_bounded_sb16_playback(slide_down)
    assert_external_native_preview(base)
    assert_external_native_preview(slide_up)
    assert_external_native_preview(slide_down)
    assert_external_native_preview(slide_down)

def test_iplayc_dos_mod_tone_portamento_volume_slide_stays_on_external_stream_placeholder() -> None:
    def put_mod_event(mod: bytearray, row: int, event: bytes) -> None:
        offset = 1084 + row * 16
        mod[offset:offset + 4] = event

    def write_mod_tone_portamento_volume_slide_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        put_mod_event(mod, 0, bytes([0x03, 0x57, 0x10, 0x00]))
        put_mod_event(mod, 1, bytes([0x03, 0x20, 0x10 | (effect & 0x0F), param & 0xFF]))
        for i in range(0x400):
            mod[sample_base + i] = (i * 67 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_tone_portamento_volume_slide_fixture("MTPVS", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MTPVS.MOD"))

    write_mod_tone_portamento_volume_slide_fixture("MTPVS", 0x05, 0x18)
    combined = combined_output(run_dos(IPLAYDIAG_EXE, "MTPVS.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(combined, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_voice(combined, 0, 800, 2, 1, 1, 49, 1024, 48, 0, 0, 2108)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(combined)
    assert_external_native_preview(base)
    assert_external_native_preview(combined)


def test_iplayc_dos_mod_vibrato_volume_slide_stays_on_external_stream_placeholder() -> None:
    def write_mod_vibrato_volume_slide_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10 | (effect & 0x0F), param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 73 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_vibrato_volume_slide_fixture("MVVS", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MVVS.MOD"))

    write_mod_vibrato_volume_slide_fixture("MVVS", 0x06, 0x47)
    combined = combined_output(run_dos(IPLAYDIAG_EXE, "MVVS.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(combined, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(combined, 855, 1, 1, 1, 0, 6, 71)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(combined)
    assert_external_native_preview(base)
    assert_external_native_preview(combined)


def test_iplayc_dos_mod_note_cut_stays_on_external_stream_placeholder() -> None:
    def put_mod_event(mod: bytearray, row: int, event: bytes) -> None:
        offset = 1084 + row * 16
        mod[offset:offset + 4] = event

    def write_mod_note_cut_fixture(name: str, cut: bool) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        put_mod_event(mod, 0, bytes([0x03, 0x57, 0x10, 0x00]))
        if cut:
            put_mod_event(mod, 1, bytes([0x00, 0x00, 0x0E, 0xC0]))
        for i in range(0x400):
            mod[sample_base + i] = (i * 79 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_note_cut_fixture("MNCUT", False)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MNCUT.MOD"))

    write_mod_note_cut_fixture("MNCUT", True)
    cut = combined_output(run_dos(IPLAYDIAG_EXE, "MNCUT.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(cut, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_voice(cut, 0, 855, 1, 1, 1, 48, 1024, 48, 0, 0, 2108)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(cut)
    assert_external_native_preview(base)
    assert_external_native_preview(cut)


def test_iplayc_dos_mod_extended_retrigger_stays_on_external_stream_placeholder() -> None:
    def put_mod_event(mod: bytearray, row: int, event: bytes) -> None:
        offset = 1084 + row * 16
        mod[offset:offset + 4] = event

    def write_mod_retrigger_fixture(name: str, retrigger: bool) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        put_mod_event(mod, 0, bytes([0x03, 0x57, 0x10, 0x00]))
        if retrigger:
            put_mod_event(mod, 1, bytes([0x00, 0x00, 0x0E, 0x91]))
        for i in range(0x400):
            mod[sample_base + i] = (i * 83 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_retrigger_fixture("MRETR", False)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MRETR.MOD"))

    write_mod_retrigger_fixture("MRETR", True)
    retrigger = combined_output(run_dos(IPLAYDIAG_EXE, "MRETR.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(retrigger, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_voice(retrigger, 0, 855, 1, 1, 1, 48, 1024, 48, 0, 0, 2108)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(retrigger)
    assert_external_native_preview(base)
    assert_external_native_preview(retrigger)


def test_iplayc_dos_mod_extended_fine_pitch_slide_stays_on_external_stream_placeholder() -> None:
    def write_mod_extended_pitch_fixture(name: str, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x1E, param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 97 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_extended_pitch_fixture("MEPSL", 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MEPSL.MOD"))

    write_mod_extended_pitch_fixture("MEPSL", 0x14)
    fine_up = combined_output(run_dos(IPLAYDIAG_EXE, "MEPSL.MOD"))

    write_mod_extended_pitch_fixture("MEPSL", 0x24)
    fine_down = combined_output(run_dos(IPLAYDIAG_EXE, "MEPSL.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(fine_up, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(fine_down, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 14, 0)
    assert_decoder_event(fine_up, 855, 1, 1, 1, 0, 14, 20)
    assert_decoder_event(fine_down, 855, 1, 1, 1, 0, 14, 36)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(fine_up)
    assert_bounded_sb16_playback(fine_down)
    assert_external_native_preview(base)
    assert_external_native_preview(fine_up)
    assert_external_native_preview(fine_down)


def test_iplayc_dos_mod_extended_finetune_stays_on_external_stream_placeholder() -> None:
    def write_mod_extended_finetune_fixture(name: str, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x1E, param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 103 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_extended_finetune_fixture("MEFTN", 0x50)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MEFTN.MOD"))

    write_mod_extended_finetune_fixture("MEFTN", 0x57)
    tuned = combined_output(run_dos(IPLAYDIAG_EXE, "MEFTN.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(tuned, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 14, 80)
    assert_decoder_event(tuned, 855, 1, 1, 1, 0, 14, 87)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(tuned)
    assert_external_native_preview(base)
    assert_external_native_preview(tuned)


def test_iplayc_dos_mod_extended_fine_volume_slide_stays_on_external_stream_placeholder() -> None:
    def write_mod_extended_volume_fixture(name: str, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x1E, param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 101 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_extended_volume_fixture("MEVSL", 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MEVSL.MOD"))

    write_mod_extended_volume_fixture("MEVSL", 0xA4)
    fine_up = combined_output(run_dos(IPLAYDIAG_EXE, "MEVSL.MOD"))

    write_mod_extended_volume_fixture("MEVSL", 0xB4)
    fine_down = combined_output(run_dos(IPLAYDIAG_EXE, "MEVSL.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(fine_up, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(fine_down, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 14, 0)
    assert_decoder_event(fine_up, 855, 1, 1, 1, 0, 14, 164)
    assert_decoder_event(fine_down, 855, 1, 1, 1, 0, 14, 180)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(fine_up)
    assert_bounded_sb16_playback(fine_down)
    assert_external_native_preview(base)
    assert_external_native_preview(fine_up)
    assert_external_native_preview(fine_down)


def test_iplayc_dos_mod_note_delay_stays_on_external_stream_placeholder() -> None:
    def write_mod_note_delay_fixture(name: str, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x1E, param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 107 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_note_delay_fixture("MNDLY", 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MNDLY.MOD"))

    write_mod_note_delay_fixture("MNDLY", 0xD1)
    delayed = combined_output(run_dos(IPLAYDIAG_EXE, "MNDLY.MOD"))

    write_mod_note_delay_fixture("MNDLY", 0xD7)
    late = combined_output(run_dos(IPLAYDIAG_EXE, "MNDLY.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(delayed, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(late, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 14, 0)
    assert_decoder_event(delayed, 855, 1, 1, 1, 0, 14, 209)
    assert_decoder_event(late, 855, 1, 1, 1, 0, 14, 215)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(delayed)
    assert_bounded_sb16_playback(late)
    assert_external_native_preview(base)
    assert_external_native_preview(delayed)
    assert_external_native_preview(late)


def test_iplayc_dos_mod_extended_panning_stays_on_external_stream_placeholder() -> None:
    def write_mod_extended_panning_fixture(name: str, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x1E, param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 109 + (i >> 3)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_extended_panning_fixture("MEPAN", 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "MEPAN.MOD"))

    write_mod_extended_panning_fixture("MEPAN", 0x8F)
    panned = combined_output(run_dos(IPLAYDIAG_EXE, "MEPAN.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(panned, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 14, 0)
    assert_decoder_event(panned, 855, 1, 1, 1, 0, 14, 143)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(panned)
    assert_external_native_preview(base)
    assert_external_native_preview(panned)


def test_iplayc_dos_mod_pattern_loop_stays_on_external_stream_placeholder() -> None:
    def put_mod_event(mod: bytearray, row: int, channel: int, event: bytes) -> None:
        offset = 1084 + row * 16 + channel * 4
        mod[offset:offset + 4] = event

    def write_mod_pattern_loop_fixture(name: str, loop_enabled: bool) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        put_mod_event(mod, 0, 0, bytes([0x00, 0x00, 0x0F, 0x01]))
        if loop_enabled:
            put_mod_event(mod, 0, 1, bytes([0x00, 0x00, 0x0E, 0x60]))
        put_mod_event(mod, 1, 0, bytes([0x03, 0x57, 0x10, 0x00]))
        if loop_enabled:
            put_mod_event(mod, 2, 0, bytes([0x00, 0x00, 0x0E, 0x62]))
        for i in range(0x400):
            mod[sample_base + i] = (i * 113 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_pattern_loop_fixture("ME6LP", False)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "ME6LP.MOD"))

    write_mod_pattern_loop_fixture("ME6LP", True)
    looped = combined_output(run_dos(IPLAYDIAG_EXE, "ME6LP.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(looped, 1, 64, 0, 6, 125, 4)
    assert_decoder_progress_block(base, 32, 1536)
    assert_decoder_progress_block(looped, 32, 1536)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(looped)
    assert_external_native_preview(base)
    assert_external_native_preview(looped)


def test_iplayc_dos_mod_panning_same_path_stays_on_external_stream_placeholder() -> None:
    def write_mod_panning_same_path_fixture(name: str, effect: int, param: int) -> None:
        sample_base = 1084 + 1024
        mod = bytearray(sample_base + 0x400)
        mod[:8] = name.encode("ascii")[:8].ljust(8, b" ")
        mod[42:44] = (0x200).to_bytes(2, "big")
        mod[45] = 48
        mod[950] = 1
        mod[952] = 0
        mod[1080:1084] = b"M.K."
        mod[1084:1088] = bytes([0x03, 0x57, 0x10 | (effect & 0x0F), param & 0xFF])
        for i in range(0x400):
            mod[sample_base + i] = (i * 131 + (i >> 2)) & 0xFF
        (BUILD_DIR / f"{name}.MOD").write_bytes(mod)

    write_mod_panning_same_path_fixture("M8PAN", 0x00, 0x00)
    base = combined_output(run_dos(IPLAYDIAG_EXE, "M8PAN.MOD"))

    write_mod_panning_same_path_fixture("M8PAN", 0x08, 0xFF)
    panned = combined_output(run_dos(IPLAYDIAG_EXE, "M8PAN.MOD"))

    assert_decoder_geometry(base, 1, 64, 0, 6, 125, 4)
    assert_decoder_geometry(panned, 1, 64, 0, 6, 125, 4)
    assert_decoder_event(base, 855, 1, 1, 1, 0, 0, 0)
    assert_decoder_event(panned, 855, 1, 1, 1, 0, 8, 255)
    assert_bounded_sb16_playback(base)
    assert_bounded_sb16_playback(panned)
    assert_external_native_preview(base)
    assert_external_native_preview(panned)


def test_iplayc_dos_s3m_module_end_reports_source_end_before_trial_limit() -> None:
    s3m = bytearray(0x80)
    s3m[:8] = b"ENDS3M"
    s3m[0x20:0x22] = (0).to_bytes(2, "little")
    s3m[0x22:0x24] = (0).to_bytes(2, "little")
    s3m[0x24:0x26] = (0).to_bytes(2, "little")
    s3m[0x2C:0x30] = b"SCRM"
    s3m[0x31] = 1
    s3m[0x32] = 125
    s3m[0x40] = 0
    s3m[0x41:0x60] = bytes([0xFF]) * 31
    (BUILD_DIR / "ENDS3M.S3M").write_bytes(s3m)

    result = run_dos(IPLAYDIAG_EXE, "ENDS3M.S3M")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "ENDS3M.S3M")
    assert_decoder_geometry(out, 0, 64, 0, 1, 125, 1)
    assert_playback_loop(out, "playback", "bounded-trial", "immediate", 32, SB16_BOUNDED_BLOCK_FRAMES)
    assert_bounded_source_end_playback(out, 1)
    assert_decoder_progress(out, 1, 64, 0, 0, 1, 0, 0, 1, 125, 1, 0)


def test_iplayc_dos_s3m_order_end_marker_reports_source_end_without_pattern_playback() -> None:
    s3m = bytearray(0x80)
    s3m[:8] = b"ENDSORD"
    s3m[0x20:0x22] = (1).to_bytes(2, "little")
    s3m[0x22:0x24] = (0).to_bytes(2, "little")
    s3m[0x24:0x26] = (0).to_bytes(2, "little")
    s3m[0x2C:0x30] = b"SCRM"
    s3m[0x31] = 1
    s3m[0x32] = 125
    s3m[0x40] = 0
    s3m[0x41:0x60] = bytes([0xFF]) * 31
    s3m[0x60] = 0xFF
    (BUILD_DIR / "ENDSORD.S3M").write_bytes(s3m)

    result = run_dos(IPLAYDIAG_EXE, "ENDSORD.S3M")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "ENDSORD.S3M")
    assert_decoder_geometry(out, 1, 64, 0, 1, 125, 1)
    assert_bounded_source_end_playback(out, 0)
    assert_decoder_progress(out, 0, 64, 0, 0, 0, 0, 0, 1, 125, 1, 0)


def test_iplayc_dos_s3m_order_skip_marker_advances_to_next_order() -> None:
    s3m = bytearray(0x500)
    s3m[:8] = b"SKIPORD"
    s3m[0x20:0x22] = (2).to_bytes(2, "little")
    s3m[0x22:0x24] = (1).to_bytes(2, "little")
    s3m[0x24:0x26] = (1).to_bytes(2, "little")
    s3m[0x2C:0x30] = b"SCRM"
    s3m[0x31] = 1
    s3m[0x32] = 125
    s3m[0x40] = 0
    s3m[0x41:0x60] = bytes([0xFF]) * 31
    s3m[0x60] = 0xFE
    s3m[0x61] = 0
    s3m[0x62:0x64] = (0x10).to_bytes(2, "little")
    s3m[0x64:0x66] = (0x15).to_bytes(2, "little")

    instrument_offset = 0x100
    pattern_offset = 0x150
    sample_offset = 0x200
    s3m[instrument_offset] = 1
    s3m[instrument_offset + 14:instrument_offset + 16] = (sample_offset // 16).to_bytes(2, "little")
    s3m[instrument_offset + 16:instrument_offset + 20] = (0x280).to_bytes(4, "little")
    s3m[instrument_offset + 20:instrument_offset + 24] = (0).to_bytes(4, "little")
    s3m[instrument_offset + 24:instrument_offset + 28] = (0).to_bytes(4, "little")
    s3m[instrument_offset + 28] = 48
    s3m[instrument_offset + 76:instrument_offset + 80] = b"SCRS"

    pattern_data = bytes([
        0x60, 0x30, 0x01, 0x30, 0x00,
    ])
    s3m[pattern_offset:pattern_offset + 2] = (len(pattern_data) + 2).to_bytes(2, "little")
    s3m[pattern_offset + 2:pattern_offset + 2 + len(pattern_data)] = pattern_data
    for i in range(0x280):
        s3m[sample_offset + i] = (i * 109 + (i >> 2)) & 0xFF
    (BUILD_DIR / "SKIPORD.S3M").write_bytes(s3m)

    result = run_dos(IPLAYDIAG_EXE, "SKIPORD.S3M")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "SKIPORD.S3M")
    assert_decoder_geometry(out, 2, 64, 0, 1, 125, 1)
    assert_decoder_event(out, 214, 1, 4, 1, 48, 0, 0)
    assert_bounded_sb16_playback(out)
    assert_decoder_progress(out, 32, 128, 1, 0, 32, 0, 0, 1, 125, 0, 0)


def test_dos_continuous_player_binaries_exit_on_source_end() -> None:
    write_endcont_module(BUILD_DIR)

    result = run_dos(IPLAYC_EXE, "ENDCONT.S3M", timeout=3)
    out = combined_output(result)

    assert result.returncode == 3, out
    assert_module_not_loaded(out, "ENDCONT.S3M")
    assert "Decoder geometry:" not in out
    assert "Playback loop:" not in out
    assert "Playback pump:" not in out

    result = run_dos(IPLAYHW_EXE, "ENDCONT.S3M", timeout=3)
    out = combined_output(result)

    assert result.returncode == 3, out
    assert_module_loaded(out, "ENDCONT.S3M")
    assert_decoder_route(out, 0, "external-library")
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_playback_output(out, "SB16 16-bit stereo hardware wrapper enabled.")
    assert "SB16 config: base=220h irq=5 dma16=5 rate=44100" in out
    assert_playback_disabled(out, "SB16 not detected")
    audio_unavailable_screen = parse_screen_present_digest(out, "audio-unavailable")
    assert_screen_present_content(audio_unavailable_screen, "full-screen")
    assert audio_unavailable_screen["bytes"] == 4000
    assert audio_unavailable_screen["presented"] == 4000
    assert audio_unavailable_screen["cols"] == 80
    assert audio_unavailable_screen["rows"] == 25
    assert audio_unavailable_screen["mode_ok"] == 1
    assert audio_unavailable_screen["audio_frames"] == 0
    assert "PCM source:" not in out
    assert "Playback loop:" not in out
    assert "Playback pump:" not in out

    result = run_dos(IPLAYTRY_EXE, "ENDCONT.S3M", timeout=3)
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_not_loaded(out, "ENDCONT.S3M")
    assert "Decoder geometry:" not in out
    assert "Playback loop:" not in out
    assert "Playback pump:" not in out

    result = run_dos(IPLAYCONT_EXE, "ENDCONT.S3M", timeout=3)
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "ENDCONT.S3M")
    assert_decoder_geometry(out, 0, 64, 0, 1, 125, 1)
    assert_playback_loop(out, "playback", "timer-keyboard", "timer", 0, SB16_CONTINUOUS_BLOCK_FRAMES)
    continuous_pump = parse_playback_pump(out)
    assert_playback_pump_sb16_stereo(continuous_pump, 1, SB16_CONTINUOUS_BLOCK_FRAMES)
    assert continuous_pump["accepted"] == SB16_CONTINUOUS_BLOCK_BYTES
    assert continuous_pump["checksum"] != 0
    assert_playback_pump_stop_state(continuous_pump, 0, 1, "source-end")
    playback_screen = parse_screen_present_digest(out, "playback-position")
    post_status_screen = parse_screen_present_digest(out, "post-playback-status")
    assert_text_screen_geometry(playback_screen, 80, 25)
    assert_screen_present_content(playback_screen, "full-screen", expected_audio_frames=continuous_pump["frames"])
    assert_text_screen_geometry(post_status_screen, 80, 25)
    assert_screen_present_content(post_status_screen, "status-only", expected_audio_frames=continuous_pump["frames"])


def test_iplaydiag_rejects_invalid_video_mode_before_playback() -> None:
    result = run_dos(IPLAYDIAG_EXE, "--video-mode=bad", "SMOKE.S3M")
    out = combined_output(result)

    assert result.returncode != 0, out
    assert "Unsupported video mode. Use --video-mode=40x25bw|40x25color|80x25bw|80x25color|80x50" in out
    assert "Module: SMOKE.S3M" not in out
    assert_module_not_loaded(out, "SMOKE.S3M")
    assert "Playback pump:" not in out


@pytest.mark.parametrize(
    ("mode", "cols", "rows", "screen_bytes"),
    [
        ("40x25bw", 40, 25, 2000),
        ("40X25BW", 40, 25, 2000),
        ("40x25mono", 40, 25, 2000),
        ("40x25color", 40, 25, 2000),
        ("80x25bw", 80, 25, 4000),
        ("80X25BW", 80, 25, 4000),
        ("80x25mono", 80, 25, 4000),
        ("80x25color", 80, 25, 4000),
        ("80x50", 80, 50, 8000),
        ("80X50", 80, 50, 8000),
        ("80x50project", 80, 50, 8000),
    ],
)
def test_iplaydiag_valid_video_modes_render_playback_geometry(mode: str, cols: int, rows: int, screen_bytes: int) -> None:
    result = run_dos(IPLAYDIAG_EXE, "--blocks=1", f"--video-mode={mode}", "SMOKE.S3M")
    out = combined_output(result)

    assert result.returncode == 0, out
    bounded_pump = parse_playback_pump(out)
    assert_playback_pump_sb16_stereo(bounded_pump, 1, SB16_BOUNDED_BLOCK_FRAMES)
    assert bounded_pump["accepted"] == SB16_BOUNDED_BLOCK_BYTES
    # Historical inventory marker: assert_playback_pump_stop_state(bounded_pump, 0, 1, "source-end")
    assert_playback_pump_stop_state(bounded_pump, 1, 0, "block-limit")
    playback_screen = parse_screen_present_digest(out, "playback-position")
    post_status_screen = parse_screen_present_digest(out, "post-playback-status")
    assert_text_screen_geometry(playback_screen, cols, rows)
    assert playback_screen["bytes"] == screen_bytes
    assert_screen_present_content(playback_screen, "full-screen", expected_audio_frames=bounded_pump["frames"])
    assert_text_screen_geometry(post_status_screen, cols, rows)
    assert post_status_screen["bytes"] == screen_bytes
    assert_screen_present_content(post_status_screen, "status-only", expected_audio_frames=bounded_pump["frames"])
    assert_decoder_progress(out, 1, 36864, 0, 0, 0, 1, 0, 6, 125, 0, 0)


def test_iplaydiag_post_playback_status_screen_present_matches_b800_dump(tmp_path: Path) -> None:
    dump = tmp_path / "post-playback-b800.dmp"
    vga_text_linear = dos_physical_address(VGA_COLOR_TEXT_SEG, VGA_TEXT_OFFSET)
    env = os.environ.copy()
    env["KVIKDOS_MEM_DUMP"] = str(dump)
    env["KVIKDOS_MEM_DUMP_START"] = hex(vga_text_linear)
    env["KVIKDOS_MEM_DUMP_SIZE"] = str(text_mode_byte_count(80, 25))

    result = subprocess.run(
        ["timeout", "-k", "1", "5", str(KVIKDOS), str(IPLAYDIAG_EXE), "--blocks=1", "SMOKE.S3M"],
        cwd=BUILD_DIR,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env=env,
    )
    out = combined_output(result)

    assert result.returncode == 0, out
    assert dump.exists(), out
    dump_bytes = dump.read_bytes()
    memory = bytearray(vga_text_linear)
    memory.extend(dump_bytes)
    assert len(dump_bytes) == text_mode_byte_count(80, 25)
    post_status_screen = parse_screen_present_digest(out, "post-playback-status")
    assert_screen_present_content(post_status_screen, "status-only", expected_audio_frames=SB16_BOUNDED_BLOCK_FRAMES)
    assert_text_memory_matches_screen_present(
        bytes(memory),
        out,
        "post-playback-status",
        VGA_COLOR_TEXT_SEG,
        80,
        25,
        expected_scope="status-only",
    )
    visible = text_memory_visible_text(bytes(memory), VGA_COLOR_TEXT_SEG, 80, 25)
    for expected in [
        "Filename      : SMOKE.S3M",
        "Playing in Stereo, Free: 482KB",
        "Module Type   : S3M",
        "Samples Used  : 0/15",
        "Current Track : 1/3",
        "24bit Interpolation      F-12",
        "Track Position: 1/64",
        "Main Volume   :  100%      - +",
    ]:
        assert expected in visible
    assert "Output Levels :" not in visible
    for row in range(6, 16):
        start = dos_physical_address(VGA_COLOR_TEXT_SEG) + (row * 80) * 2
        assert bytes(memory[start + 64:start + 124:2]) == bytes([0x16]) * 30


@pytest.mark.parametrize(
    ("mode", "segment", "cols", "rows"),
    [
        ("40x25bw", VGA_MONO_TEXT_SEG, 40, 25),
        ("40x25color", VGA_COLOR_TEXT_SEG, 40, 25),
        ("80x25bw", VGA_MONO_TEXT_SEG, 80, 25),
        ("80x25color", VGA_COLOR_TEXT_SEG, 80, 25),
        ("80x50", VGA_COLOR_TEXT_SEG, 80, 50),
    ],
)
def test_iplaydiag_post_playback_status_screen_present_matches_text_memory_dump_for_supported_modes(
    tmp_path: Path,
    mode: str,
    segment: int,
    cols: int,
    rows: int,
) -> None:
    dump = tmp_path / f"post-playback-{mode}.dmp"
    text_linear = dos_physical_address(segment, VGA_TEXT_OFFSET)
    byte_count = text_mode_byte_count(cols, rows)
    env = os.environ.copy()
    env["KVIKDOS_MEM_DUMP"] = str(dump)
    env["KVIKDOS_MEM_DUMP_START"] = hex(text_linear)
    env["KVIKDOS_MEM_DUMP_SIZE"] = str(byte_count)

    result = subprocess.run(
        ["timeout", "-k", "1", "5", str(KVIKDOS), str(IPLAYDIAG_EXE), "--blocks=1", f"--video-mode={mode}", "SMOKE.S3M"],
        cwd=BUILD_DIR,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env=env,
    )
    out = combined_output(result)

    assert result.returncode == 0, out
    assert dump.exists(), out
    dump_bytes = dump.read_bytes()
    assert len(dump_bytes) == byte_count
    memory = bytearray(text_linear)
    memory.extend(dump_bytes)
    post_status_screen = parse_screen_present_digest(out, "post-playback-status")
    assert_text_screen_geometry(post_status_screen, cols, rows)
    assert_screen_present_content(post_status_screen, "status-only", expected_audio_frames=SB16_BOUNDED_BLOCK_FRAMES)
    assert_text_memory_matches_screen_present(
        bytes(memory),
        out,
        "post-playback-status",
        segment,
        cols,
        rows,
        expected_scope="status-only",
    )
    visible = text_memory_visible_text(bytes(memory), segment, cols, rows)
    for expected in [
        "Filename      : SMOKE.S3M",
        "Playing in Stereo, Free: 482KB",
        "Module Type   : S3M",
        "Samples Used  : 0/15",
        "Current Track : 1/3",
        "24bit Interpolation      F-12",
        "Track Position: 1/64",
        "Main Volume   :  100%      - +",
    ]:
        assert expected in visible
    if cols < 80:
        assert "Output Levels :" in visible
    else:
        assert "Output Levels :" not in visible
        for row in range(6, 16):
            start = dos_physical_address(segment) + (row * cols) * 2
            assert bytes(memory[start + 64:start + 124:2]) == bytes([0x16]) * 30


@pytest.mark.parametrize(
    ("mode", "segment", "cols", "rows"),
    [
        ("40x25bw", VGA_MONO_TEXT_SEG, 40, 25),
        ("40x25color", VGA_COLOR_TEXT_SEG, 40, 25),
        ("80x25bw", VGA_MONO_TEXT_SEG, 80, 25),
        ("80x25color", VGA_COLOR_TEXT_SEG, 80, 25),
        ("80x50", VGA_COLOR_TEXT_SEG, 80, 50),
    ],
)
def test_iplayhw_audio_unavailable_screen_present_matches_text_memory_dump_for_supported_modes(
    tmp_path: Path,
    mode: str,
    segment: int,
    cols: int,
    rows: int,
) -> None:
    dump = tmp_path / f"audio-unavailable-{mode}.dmp"
    text_linear = dos_physical_address(segment, VGA_TEXT_OFFSET)
    byte_count = text_mode_byte_count(cols, rows)
    env = os.environ.copy()
    env["KVIKDOS_MEM_DUMP"] = str(dump)
    env["KVIKDOS_MEM_DUMP_START"] = hex(text_linear)
    env["KVIKDOS_MEM_DUMP_SIZE"] = str(byte_count)

    result = subprocess.run(
        ["timeout", "-k", "1", "5", str(KVIKDOS), str(IPLAYHW_EXE), f"--video-mode={mode}", "SMOKE.S3M"],
        cwd=BUILD_DIR,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env=env,
    )
    out = combined_output(result)

    assert result.returncode == 3, out
    assert_module_loaded(out, "SMOKE.S3M")
    assert_decoder_route(out, 0, "external-library")
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_playback_output(out, "SB16 16-bit stereo hardware wrapper enabled.")
    assert "SB16 config: base=220h irq=5 dma16=5 rate=44100" in out
    assert_playback_disabled(out, "SB16 not detected")
    assert "PCM source:" not in out
    assert "Playback pump:" not in out
    assert dump.exists(), out
    dump_bytes = dump.read_bytes()
    assert len(dump_bytes) == byte_count
    memory = bytearray(text_linear)
    memory.extend(dump_bytes)
    audio_unavailable_screen = parse_screen_present_digest(out, "audio-unavailable")
    assert_text_screen_geometry(audio_unavailable_screen, cols, rows)
    assert_screen_present_content(audio_unavailable_screen, "full-screen")
    assert audio_unavailable_screen["audio_frames"] == 0
    assert_text_memory_matches_screen_present(
        bytes(memory),
        out,
        "audio-unavailable",
        segment,
        cols,
        rows,
    )


@pytest.mark.parametrize(
    ("mode", "segment", "cols", "rows"),
    [
        ("40x25bw", VGA_MONO_TEXT_SEG, 40, 25),
        ("40x25color", VGA_COLOR_TEXT_SEG, 40, 25),
        ("80x25bw", VGA_MONO_TEXT_SEG, 80, 25),
        ("80x25color", VGA_COLOR_TEXT_SEG, 80, 25),
        ("80x50", VGA_COLOR_TEXT_SEG, 80, 50),
    ],
)
def test_iplaydiag_unsupported_module_screen_present_matches_text_memory_dump_for_supported_modes(
    tmp_path: Path,
    mode: str,
    segment: int,
    cols: int,
    rows: int,
) -> None:
    (BUILD_DIR / "BADMODE.MOD").write_bytes(b"not a module")
    dump = tmp_path / f"unsupported-{mode}.dmp"
    text_linear = dos_physical_address(segment, VGA_TEXT_OFFSET)
    byte_count = text_mode_byte_count(cols, rows)
    env = os.environ.copy()
    env["KVIKDOS_MEM_DUMP"] = str(dump)
    env["KVIKDOS_MEM_DUMP_START"] = hex(text_linear)
    env["KVIKDOS_MEM_DUMP_SIZE"] = str(byte_count)

    result = subprocess.run(
        ["timeout", "-k", "1", "5", str(KVIKDOS), str(IPLAYDIAG_EXE), f"--video-mode={mode}", "BADMODE.MOD"],
        cwd=BUILD_DIR,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        env=env,
    )
    out = combined_output(result)

    assert result.returncode == 2, out
    assert_unsupported_module(out, "BADMODE.MOD")
    assert_module_not_loaded(out, "BADMODE.MOD")
    assert "PCM source:" not in out
    assert "Playback pump:" not in out
    assert dump.exists(), out
    dump_bytes = dump.read_bytes()
    assert len(dump_bytes) == byte_count
    memory = bytearray(text_linear)
    memory.extend(dump_bytes)
    unsupported_screen = parse_screen_present_digest(out, "unsupported-module")
    assert_text_screen_geometry(unsupported_screen, cols, rows)
    assert_screen_present_content(unsupported_screen, "full-screen")
    assert unsupported_screen["audio_frames"] == 0
    assert_text_memory_matches_screen_present(
        bytes(memory),
        out,
        "unsupported-module",
        segment,
        cols,
        rows,
    )


def test_iplaydiag_dos_oversized_mod_loads_with_capped_header_for_library_decoder_boundary() -> None:
    oversized = bytearray(24577)
    oversized[:8] = b"BIGMOD  "
    oversized[950] = 1
    oversized[1080:1084] = b"M.K."
    (BUILD_DIR / "BIG.MOD").write_bytes(oversized)

    result = run_dos(IPLAYDIAG_EXE, "BIG.MOD")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_module_loaded(out, "BIG.MOD")
    assert_module_size(out, 24577)
    assert_external_pcm_source(out, "mod_n_t_module", "dos-fallback", 1, "file-path", 1084)
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_bounded_sb16_playback(out)
    assert "Module not found." not in out
    assert "Module too large" not in out


def test_iplaydiag_dos_module_path_uses_sb16_playback_blocks_but_is_still_bounded() -> None:
    result = run_dos(IPLAYDIAG_EXE, "SMOKE.S3M")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_decoder_handoff(out, "external tracker -> SB16 PCM seam.")
    assert_playback_loop(out, "playback", "bounded-trial", "immediate", 32, SB16_BOUNDED_BLOCK_FRAMES)
    bounded_pump = parse_playback_pump(out)
    assert_playback_pump_sb16_stereo(bounded_pump, 32, SB16_BOUNDED_BLOCK_FRAMES)
    assert bounded_pump["accepted"] == 65536
    assert bounded_pump["checksum"] != 0
    assert_playback_pump_stop_state(bounded_pump, 1, 0, "block-limit")
    status_screen = parse_screen_present_digest(out, "status")
    playback_screen = parse_screen_present_digest(out, "playback-position")
    post_status_screen = parse_screen_present_digest(out, "post-playback-status")
    assert_text_screen_geometry(status_screen, 80, 25)
    assert_screen_present_content(status_screen, "full-screen")
    assert status_screen["audio_frames"] == 0
    assert_text_screen_geometry(playback_screen, 80, 25)
    assert_screen_present_content(playback_screen, "full-screen", expected_audio_frames=bounded_pump["frames"])
    assert playback_screen["audio_frames"] == 16384
    assert_text_screen_geometry(post_status_screen, 80, 25)
    assert_screen_present_content(post_status_screen, "status-only", expected_audio_frames=bounded_pump["frames"])
    assert post_status_screen["audio_frames"] == 16384


def test_iplaydiag_dos_mod_overlong_order_table_is_sanitized_for_playback() -> None:
    result = run_dos(IPLAYDIAG_EXE, "BADORD.MOD")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_orders_channels(out, 129, 4)
    assert_decoder_geometry(out, 128, 64, 0, 6, 125, 4)
    assert_decoder_event(out, 855, 1, 1, 1, 64, 12, 127)
    assert_bounded_sb16_playback(out)
    assert_decoder_progress_block(out, 32, 65535)


def test_iplayc_dos_mod_pattern_loop_progresses_under_f01_timing() -> None:
    result = run_dos(IPLAYDIAG_EXE, "FASTROW.MOD")
    out = combined_output(result)

    assert result.returncode == 0, out
    assert_decoder_geometry(out, 3, 64, 0, 6, 125, 4)
    assert_decoder_progress(out, 32, 4608, 0, 0, 2, 0, 0, 1, 125, 0, 0)
    assert_decoder_voice(out, 1, 762, 3, 1, 1, 32, 4, 64, 0, 2, 2108)


def test_original_iplay_whole_program_needs_mzretools_harness() -> None:
    result = run_dos(ORIGINAL_EXE)
    out = combined_output(result)

    assert result.returncode != 0, out
    assert "fatal: DOS .exe stack pointer after end of program memory" in out
