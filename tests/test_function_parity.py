#!/usr/bin/env python3
from __future__ import annotations

import os
import re
import struct
import subprocess
import tempfile
import math
from pathlib import Path

from player_behavior_fixtures import (
    VGA_COLOR_TEXT_SEG,
    VGA_MONO_TEXT_SEG,
    SB16_BOUNDED_BLOCK_BYTES,
    SB16_BOUNDED_BLOCK_FRAMES,
    SB16_CONTINUOUS_BLOCK_BYTES,
    SB16_CONTINUOUS_BLOCK_FRAMES,
    assert_decoder_event,
    assert_decoder_route,
    assert_decoder_route_absent,
    assert_decoder_handoff,
    assert_decoder_handoff_absent,
    assert_decoder_progress,
    assert_decoder_progress_block,
    assert_decoder_geometry,
    assert_decoder_voice,
    assert_module_loaded,
    assert_module_not_loaded,
    assert_module_loader,
    assert_module_size,
    assert_module_title,
    assert_module_type_tag,
    assert_ffi_marker,
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
    assert_sb16_stereo_block_accounting,
    assert_sb16_stereo_frame_bytes,
    assert_supported_dos_formats,
    assert_text_backend,
    assert_text_backend_memory,
    assert_text_screen_geometry,
    assert_unsupported_module,
    assert_text_memory_matches_player_hw_text,
    assert_text_memory_matches_screen_present,
    dos_physical_address,
    parse_decoder_events,
    parse_decoder_geometry,
    parse_decoder_handoff,
    parse_decoder_route,
    parse_decoder_progress,
    parse_decoder_voices,
    parse_module_loaded,
    parse_module_loader,
    parse_module_size,
    parse_module_title,
    parse_module_type_tag,
    parse_ffi_marker,
    parse_orders_channels,
    parse_pcm_source,
    parse_player_hw_audio_digest,
    parse_player_hw_text_digest,
    parse_playback_loop,
    parse_playback_disabled,
    parse_playback_output,
    parse_playback_pump,
    parse_screen_present_digest,
    parse_unsupported_module,
    sb16_stereo_byte_count,
    text_cell_checksum,
    text_cell_digest,
    text_cell_nonblank_count,
    text_memory_digest,
    text_memory_slice,
    text_mode_byte_count,
)


ROOT = Path(__file__).resolve().parents[1]
ORIGINAL_EXE = ROOT / "original" / "IPLAY.EXE"
ORIGINAL_LST = ROOT / "IPLAY.lst"
KVIKDOS = Path(os.environ.get("KVIKDOS", "/home/xor/kvikdos/kvikdos"))
BUILD_DIR = ROOT / "tests" / ".build"
RUNNER = Path(os.environ["IPLAY_TRANSLATED_RUNNER"]) if os.environ.get("IPLAY_TRANSLATED_RUNNER") else None
REWRITE_RUNNER = ROOT / "rewrite" / ".build" / "iplay_rewrite_runner"

LOAD_SEG = 0x0110
SEG001_DELTA = 0x0905
DSEG_DELTA = 0x0C7F
DATA_SEG = LOAD_SEG + 0x145A
DSEG = LOAD_SEG + DSEG_DELTA
WRAPPER_IP = 0x8000
SRC_OFF = 0x9000
DST_OFF = 0x9100
CHANNEL_OFF = 0x9000
REAL_CHANNELS_OFF = 0x1368
DSEG_SCRATCH = 0x2800
HEADER_SIZE = 0x1C0

FALLBACK_OFFSETS = {
    "copy_printable": 0x1C31,
    "cpy_printable": 0x09C5,
    "u32tox": 0x8C19,
    "u16tox": 0x8C24,
    "u8tox": 0x8C2B,
    "u4tox": 0x8C33,
    "my_i8toa10_0": 0x8C41,
    "my_i8toa10": 0x350A,
    "my_i16toa10_0": 0x8C42,
    "my_i32toa10_0": 0x8C44,
    "my_u8toa_10": 0x8C55,
    "my_u8toa10": 0x351E,
    "my_u16toa_10": 0x8C57,
    "my_u16toa10": 0x3520,
    "my_u32toa10_0": 0x8C5B,
    "my_u32toa10": 0x3524,
    "my_u32toa": 0x352C,
    "my_u32tox": 0x34E2,
    "my_u16tox": 0x34ED,
    "my_u8tox": 0x34F4,
    "my_u4tox": 0x34FC,
    "my_i16toa10": 0x350B,
    "my_i32toa10": 0x350D,
    "my_putdigit": 0x8C78,
    "myputdigit": 0x3541,
    "loc_157F2": 0x57F2,
    "get_playsettings": 0x2AD3,
    "volume_12A66": 0x2A66,
    "set_playsettings": 0x2ADE,
    "sub_12AFD": 0x2AFD,
    "sub_11BA6": 0x1BA6,
    "sub_11C0C": 0x1C0C,
    "sub_1265D": 0x265D,
    "sub_126A9": 0x26A9,
    "sub_12B18": 0x2B18,
    "sub_12B83": 0x2B83,
    "sub_12D05": 0x2D05,
    "someplaymode": 0x2BF8,
    "int24": 0x30F9,
    "getset_playstate": 0x2C99,
    "get_12F7C": 0x2F7C,
    "hex_1BE39": 0x2DE9,
    "draw_frame": 0x2E73,
    "snd_offx": 0x2F48,
    "sub_13177": 0x3177,
    "sub_131DA": 0x31DA,
    "sub_131EF": 0x31EF,
    "nullsub_5": 0x31CF,
    "eff_nullsub": 0x387E,
    "eff_1387F": 0x387F,
    "eff_13886": 0x3886,
    "eff_1389D": 0x389D,
    "eff_138A4": 0x38A4,
    "eff_138D2": 0x38D2,
    "eff_1392F": 0x392F,
    "eff_139AC": 0x39AC,
    "eff_139B2": 0x39B2,
    "eff_139B9": 0x39B9,
    "eff_13A43": 0x3A43,
    "eff_13A94": 0x3A94,
    "eff_13AD7": 0x3AD7,
    "eff_13B06": 0x3B06,
    "eff_13B78": 0x3B78,
    "eff_13B88": 0x3B88,
    "eff_13BA3": 0x3BA3,
    "eff_13BB2": 0x3BB2,
    "eff_13BC0": 0x3BC0,
    "eff_13BC8": 0x3BC8,
    "eff_13C02": 0x3C02,
    "eff_13C34": 0x3C34,
    "eff_13C3F": 0x3C3F,
    "eff_13C64": 0x3C64,
    "eff_13C88": 0x3C88,
    "eff_13C95": 0x3C95,
    "eff_13CA2": 0x3CA2,
    "eff_13CB3": 0x3CB3,
    "eff_13CC9": 0x3CC9,
    "eff_13CDD": 0x3CDD,
    "eff_13CE8": 0x3CE8,
    "eff_13DE5": 0x3DE5,
    "eff_13DEF": 0x3DEF,
    "eff_13E1E": 0x3E1E,
    "eff_13E2D": 0x3E2D,
    "eff_13E32": 0x3E32,
    "eff_13E7F": 0x3E7F,
    "eff_13E84": 0x3E84,
    "eff_13E8C": 0x3E8C,
    "eff_13F05": 0x3F05,
    "eff_13F3B": 0x3F3B,
    "eff_13FBE": 0x3FBE,
    "eff_14020": 0x4020,
    "eff_14030": 0x4030,
    "calc_14043": 0x4043,
    "change_amplif": 0x2AAE,
    "change_volume": 0x2A83,
    "eff_14067": 0x4067,
    "memalloc": 0x8AC3,
    "memalloc12k": 0x1B73,
    "_2stm_module": 0x03EB,
    "e669_module": 0x0900,
    "f2_waves": 0x13C6,
    "init_vga_waves": 0x1CDF,
    "f2_draw_waves": 0x1E6A,
    "f2_draw_waves2": 0x1F13,
    "far_module": 0x0F1E,
    "inr_module": 0x19C7,
    "keyb_19EFD": 0x0EAD,
    "mtm_module": 0x0AD5,
    "moduleread": 0x0000,
    "mod_n_t_module": 0x00BD,
    "mod_read_10311": 0x0311,
    "modules_search": 0x0A03,
    "psm_module": 0x0D26,
    "readallmoules": 0x0D1D,
    "read_module": 0x0DC1,
    "s3m_module": 0x0597,
    "start": 0x0042,
    "ult_module": 0x1239,
    "memclean": 0x2A56,
    "memfree_125DA": 0x25DA,
    "memfree_18A28": 0x8A28,
    "memrealloc": 0x8AE7,
    "mod_1021E": 0x021E,
    "mod_1024A": 0x024A,
    "mod_102F5": 0x02F5,
    "mod_sub_delta": 0x2220,
    "snd_initialze": 0x41F6,
    "snd_on": 0x420F,
    "snd_off": 0x422D,
    "snd_deinit": 0x424F,
    "ems_init": 0x1D79,
    "ems_release": 0x1E02,
    "ems_realloc": 0x1E1E,
    "ems_deinit": 0x1E37,
    "ems_save_mapctx": 0x1E47,
    "ems_restore_mapctx": 0x1E68,
    "ems_mapmem": 0x1E8B,
    "ems_mapmem2": 0x1EC5,
    "ems_mapmemx": 0x22E8,
    "ems_mapmemy": 0x24A2,
    "nullsub_2": 0x3F04,
    "nullsub_4": 0x544C,
    "nullsub_3": 0x7DC5,
    "midi_154DA": 0x54DA,
    "midi_154DE": 0x54DE,
    "midi_154AC": 0x54AC,
    "mouse_show": 0x36EF,
    "mouse_hide": 0x3706,
    "mouse_getpos": 0x371D,
    "mouse_deinit": 0x36BF,
    "mouse_showcur": 0x373B,
    "mouse_hide2": 0x3749,
    "read_sndsettings": 0x2CCF,
    "mystrlen_0": 0x8D8F,
    "strcpy_count_0": 0x8D9D,
    "strcpy_count": 0x3665,
    "put_message2": 0x2F3E,
    "text_1BF69": 0x2F19,
    "write_scr": 0x2F09,
}


def run(cmd: list[str], check: bool = True, **kwargs) -> subprocess.CompletedProcess[str]:
    kwargs.setdefault("timeout", int(os.environ.get("IPLAY_TEST_TIMEOUT", "30")))
    return subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=check, **kwargs)


def build_runner() -> None:
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    common = ["g++", "-std=c++17", "-O0", "-I", str(ROOT / "tests" / "stubs"), "-I", str(ROOT)]
    objects = [
        (ROOT / "iplay_masm_.cpp", BUILD_DIR / "iplay_masm_.o", ["-Dmain=iplay_translated_main"]),
        (ROOT / "tests" / "translated_runtime_stubs.cpp", BUILD_DIR / "translated_runtime_stubs.o", []),
        (ROOT / "tests" / "translated_function_runner.cpp", BUILD_DIR / "translated_function_runner.o", []),
    ]
    for src, obj, extra in objects:
        run(common + extra + ["-c", str(src), "-o", str(obj)])
    run(["g++", "-std=c++17", "-O0", *(str(obj) for _, obj, _ in objects), "-lncurses", "-pthread", "-o", str(RUNNER)])


def translated(*args: str) -> str | None:
    if RUNNER is None:
        return None
    if not RUNNER.exists():
        raise AssertionError(f"IPLAY_TRANSLATED_RUNNER does not exist: {RUNNER}")
    out = run([str(RUNNER), *args], cwd=ROOT).stdout
    out = "\n".join(
        line for line in out.splitlines()
        if line.strip() != "*** NULL assignment detected"
    )
    return out.strip()


def rewritten(*args: str) -> str:
    if not REWRITE_RUNNER.exists():
        raise AssertionError(f"rewrite runner does not exist: {REWRITE_RUNNER}")
    return run([str(REWRITE_RUNNER), *args], cwd=ROOT).stdout.strip()


def iplayc(*args: str) -> str:
    exe = ROOT / "rewrite" / ".build" / "IPLAYC.EXE"
    if not exe.exists():
        raise AssertionError(f"IPLAYC.EXE does not exist: {exe}")
    if not KVIKDOS.exists():
        raise AssertionError(f"kvikdos not found: {KVIKDOS}")
    kvikdos_timeout = int(os.environ.get("IPLAY_KVIKDOS_TEST_TIMEOUT", "3"))
    return run(
        ["timeout", "-k", "1", str(kvikdos_timeout), str(KVIKDOS), str(exe), *args],
        cwd=ROOT,
        timeout=kvikdos_timeout + 2,
    ).stdout.replace("\r\n", "\n").strip()


def assert_iplayc_usage(out: str) -> None:
    assert "Inertia Player C rewrite" in out
    assert "Usage: IPLAY [Switches] [FileName.Ext|@FileList.Ext]" in out
    assert " /i  Display current soundcard settings" in out
    assert "Audio driver scope: SB16 16-bit stereo only." in out
    assert "Text backend: VGA color/BW text memory at B800:0000/B000:0000." in out
    assert "Audio backend: SB16 16-bit stereo hardware wrapper, SDL-compatible callback boundary." in out


def test_iplayc_dos_player_usage_without_arguments() -> None:
    assert_iplayc_usage(iplayc())


def test_iplayc_dos_player_usage_with_question_switches() -> None:
    assert_iplayc_usage(iplayc("-?"))
    assert_iplayc_usage(iplayc("/?"))


def test_iplayc_dos_player_usage_with_long_help_switch() -> None:
    assert_iplayc_usage(iplayc("--help"))


def original_offset(symbol: str) -> int:
    if ORIGINAL_LST.exists():
        pattern = re.compile(rf"^seg[0-9A-Fa-f]+:([0-9A-Fa-f]{{4}})\s+{re.escape(symbol)}\s+proc\b")
        for line in ORIGINAL_LST.read_text(errors="replace").splitlines():
            match = pattern.match(line)
            if match:
                return int(match.group(1), 16)
    return FALLBACK_OFFSETS[symbol]


def patch_word(data: bytearray, offset: int, value: int) -> None:
    data[offset : offset + 2] = struct.pack("<H", value & 0xFFFF)


def write_at_load_offset(data: bytearray, offset: int, payload: bytes) -> None:
    start = HEADER_SIZE + offset
    data[start : start + len(payload)] = payload


def mov_ax(value: int) -> bytes:
    return b"\xb8" + struct.pack("<H", value & 0xFFFF)


def mov_cx(value: int) -> bytes:
    return b"\xb9" + struct.pack("<H", value & 0xFFFF)


def mov_dx(value: int) -> bytes:
    return b"\xba" + struct.pack("<H", value & 0xFFFF)


def mov_bx(value: int) -> bytes:
    return b"\xbb" + struct.pack("<H", value & 0xFFFF)


def mov_di(value: int) -> bytes:
    return b"\xbf" + struct.pack("<H", value & 0xFFFF)


def mov_bp(value: int) -> bytes:
    return b"\xbd" + struct.pack("<H", value & 0xFFFF)


def mov_eax(value: int) -> bytes:
    return b"\x66\xb8" + struct.pack("<I", value & 0xFFFFFFFF)


def mov_esi(value: int) -> bytes:
    return b"\x66\xbe" + struct.pack("<I", value & 0xFFFFFFFF)


def mov_ds_byte(offset: int, value: int) -> bytes:
    return b"\xc6\x06" + struct.pack("<H", offset & 0xFFFF) + bytes([value & 0xFF])


def mov_ds_word(offset: int, value: int) -> bytes:
    return b"\xc7\x06" + struct.pack("<H", offset & 0xFFFF) + struct.pack("<H", value & 0xFFFF)


def mov_ds_dword(offset: int, value: int) -> bytes:
    return b"\x66\xc7\x06" + struct.pack("<H", offset & 0xFFFF) + struct.pack("<I", value & 0xFFFFFFFF)


def mov_cs_byte(offset: int, value: int) -> bytes:
    return b"\x2e\xc6\x06" + struct.pack("<H", offset & 0xFFFF) + bytes([value & 0xFF])


def call_rel16(target: int, next_ip: int) -> bytes:
    return b"\xe8" + struct.pack("<h", target - next_ip)


def jmp_rel16(target: int, next_ip: int) -> bytes:
    return b"\xe9" + struct.pack("<h", target - next_ip)


def make_wrapper(target: int, setup: bytes) -> bytes:
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(target, call_ip + 3) + b"\xc3"


def make_far_wrapper(target: int, setup: bytes) -> bytes:
    return setup + b"\x9a" + struct.pack("<HH", target & 0xFFFF, LOAD_SEG) + b"\xc3"


def make_stack_jmp_wrapper(target: int, setup: bytes, fake_saved_si: int) -> bytes:
    continuation_ip = WRAPPER_IP + len(setup) + 11
    return (
        setup
        + mov_ax(continuation_ip)
        + b"\x50"
        + mov_ax(fake_saved_si)
        + b"\x50"
        + jmp_rel16(target, continuation_ip)
        + b"\xc3"
    )


def make_saved_es_jmp_wrapper(target: int, setup: bytes, saved_es: int) -> bytes:
    continuation_ip = WRAPPER_IP + len(setup) + 11
    return (
        setup
        + mov_ax(continuation_ip)
        + b"\x50"
        + mov_ax(saved_es)
        + b"\x50"
        + jmp_rel16(target, continuation_ip)
        + b"\xc3"
    )


def make_iret_wrapper(target: int, setup: bytes, return_cs: int = LOAD_SEG + SEG001_DELTA) -> bytes:
    continuation_ip = WRAPPER_IP + len(setup) + 12
    return (
        setup
        + b"\x9c"
        + mov_bx(return_cs)
        + b"\x53"
        + mov_bx(continuation_ip)
        + b"\x53"
        + jmp_rel16(target, continuation_ip)
        + b"\xc3"
    )


def original_label_offset(symbol: str) -> int:
    if ORIGINAL_LST.exists():
        pattern = re.compile(rf"^seg000:([0-9A-Fa-f]{{4}})\s+{re.escape(symbol)}:")
        for line in ORIGINAL_LST.read_text(errors="replace").splitlines():
            match = pattern.match(line)
            if match:
                return int(match.group(1), 16)
    return FALLBACK_OFFSETS[symbol]


def original_run(
    wrapper: bytes,
    src: bytes = b"",
    dump_count: int = 0,
    dump_offset: int = DST_OFF,
    dump_seg: int = LOAD_SEG,
    call_cs: int = LOAD_SEG,
    wrapper_load_offset: int = WRAPPER_IP,
    strict: bool = True,
    extra_files: dict[str, bytes] | None = None,
    exe_maxalloc: int = 0x2000,
) -> tuple[str, bytes]:
    if not KVIKDOS.exists():
        raise AssertionError(f"kvikdos not found: {KVIKDOS}")
    data = bytearray(ORIGINAL_EXE.read_bytes())
    patch_word(data, 0x0A, exe_maxalloc)
    write_at_load_offset(data, wrapper_load_offset, wrapper)
    if src:
        write_at_load_offset(data, SRC_OFF, src)
    if dump_count:
        write_at_load_offset(data, DST_OFF, b"." * dump_count)

    with tempfile.TemporaryDirectory(prefix="iplay-function-test-") as td:
        exe = Path(td) / "IPLAY.EXE"
        dump = Path(td) / "mem.dmp"
        exe.write_bytes(data)
        for name, content in (extra_files or {}).items():
            (Path(td) / name).write_bytes(content)
        cmd = [
            "timeout",
            "-k",
            "1",
            str(int(os.environ.get("IPLAY_KVIKDOS_TEST_TIMEOUT", "5"))),
            str(KVIKDOS),
            f"--call-cs=0x{call_cs:04x}",
            f"--call-ds=0x{LOAD_SEG:04x}",
            f"--hlt-dump={dump}",
            f"--call-near=0x{WRAPPER_IP:04x}",
            str(exe),
        ]
        if strict:
            cmd.insert(5, "--strict")
        proc = run(cmd, check=False, timeout=int(os.environ.get("IPLAY_KVIKDOS_TEST_TIMEOUT", "3")) + 2)
        if "kvikdos-call-result" not in proc.stdout:
            raise AssertionError(f"kvikdos call failed\nstdout:\n{proc.stdout}\nstderr:\n{proc.stderr}")
        memory = dump.read_bytes()
        start = dump_seg * 16 + dump_offset
        return proc.stdout.strip(), memory[start : start + dump_count]


def original_call(
    target: int,
    setup: bytes,
    src: bytes = b"",
    dump_count: int = 0,
    strict: bool = True,
    exe_maxalloc: int = 0x2000,
) -> tuple[str, bytes]:
    return original_run(make_wrapper(target, setup), src=src, dump_count=dump_count, strict=strict, exe_maxalloc=exe_maxalloc)


def original_seg001_call(
    target: int,
    setup: bytes,
    src: bytes = b"",
    dump_count: int = 0,
    dump_offset: int = DSEG_SCRATCH,
    dump_seg: int = DSEG,
    strict: bool = True,
) -> tuple[str, bytes]:
    return original_run(
        make_wrapper(target, setup),
        src=src,
        dump_count=dump_count,
        dump_offset=dump_offset,
        dump_seg=dump_seg,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=strict,
    )


def original_seg001_iret_call(
    target: int,
    setup: bytes,
    dump_count: int = 0,
    dump_offset: int = DSEG_SCRATCH,
    dump_seg: int = DSEG,
    strict: bool = True,
) -> tuple[str, bytes]:
    return original_run(
        make_iret_wrapper(target, setup),
        dump_count=dump_count,
        dump_offset=dump_offset,
        dump_seg=dump_seg,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=strict,
    )


def original_seg000_iret_call(
    target: int,
    setup: bytes,
    dump_count: int = 0,
    dump_offset: int = DSEG_SCRATCH,
    dump_seg: int = DATA_SEG,
    strict: bool = True,
) -> tuple[str, bytes]:
    return original_run(
        make_iret_wrapper(target, setup, return_cs=LOAD_SEG),
        dump_count=dump_count,
        dump_offset=dump_offset,
        dump_seg=dump_seg,
        call_cs=LOAD_SEG,
        wrapper_load_offset=WRAPPER_IP,
        strict=strict,
    )


def original_far_call(target: int, setup: bytes, src: bytes = b"", dump_count: int = 0) -> tuple[str, bytes]:
    return original_run(make_far_wrapper(target, setup), src=src, dump_count=dump_count)


def setup_common() -> bytes:
    return mov_ax(LOAD_SEG) + b"\x8e\xd8" + b"\x8e\xc0"


def setup_data_common() -> bytes:
    return mov_ax(DATA_SEG) + b"\x8e\xd8" + b"\x8e\xc0"


def setup_data_with_fs_common() -> bytes:
    return setup_data_common() + b"\x8e\xe0"


def setup_load_common() -> bytes:
    return mov_ax(LOAD_SEG) + b"\x8e\xd8" + b"\x8e\xc0"


def setup_dseg_common() -> bytes:
    return mov_ax(DSEG) + b"\x8e\xd8" + b"\x8e\xc0"


def setup_int24(ah_value: int) -> bytes:
    return setup_dseg_common() + mov_ax((ah_value & 0xFF) << 8)


def setup_dual_data_byte(offset: int, value: int) -> bytes:
    return setup_data_common() + mov_ds_byte(offset, value) + setup_load_common() + mov_ds_byte(offset, value)


def setup_interpolation_patch_probe(patch_value: int) -> bytes:
    return (
        setup_data_common()
        + mov_cx((patch_value & 0xFF) << 8)
        + mov_ds_word(0x0044, 0)
        + mov_ds_word(0x0074, (patch_value & 0xFF) << 8)
        + mov_ds_byte(0x00D2, 0x10)
        + mov_ds_dword(CHANNEL_OFF + 0x04, 0)
        + mov_ds_dword(CHANNEL_OFF + 0x48, 0xFFFFFFFF)
        + mov_esi(0)
    )


def setup_get_playsettings(value: int) -> bytes:
    return setup_dual_data_byte(0x00D2, value)


def setup_volume_12a66(channels: int) -> bytes:
    return setup_data_common() + mov_ds_word(0x0034, channels) + mov_ds_word(0x002E, FALLBACK_OFFSETS["nullsub_5"])


def setup_vlm_141df() -> bytes:
    return (
        setup_volume_12a66(1)
        + mov_ds_byte(0x00D1, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def setup_change_volume(value: int, channels: int, channel_volume: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_word(0x0034, channels)
        + mov_ds_byte(REAL_CHANNELS_OFF + 0x08, channel_volume)
        + mov_ax(value)
    )


def setup_memclean(size: int, fill_count: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x0072, size)
        + mov_di(DSEG_SCRATCH)
        + mov_cx(fill_count)
        + b"\xfc\xb0\xa5\xf3\xaa"
        + mov_di(DSEG_SCRATCH)
    )


def setup_set_playsettings(value: int, initial_config_hi: int, freq: int, channels: int, shift: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x00D3, initial_config_hi)
        + mov_ds_word(0x00BE, freq)
        + mov_ds_word(0x0034, channels)
        + mov_ds_byte(0x007A, shift)
        + mov_ds_word(REAL_CHANNELS_OFF + 0x3E, 0xAAAA)
        + setup_load_common()
        + mov_ax(value)
    )


def setup_sub_12afd(value: int, channels: int, channel_index: int, flags: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x0034, channels)
        + mov_ds_byte(REAL_CHANNELS_OFF + 0x17, flags)
        + setup_load_common()
        + mov_ax(value)
        + mov_cx(channel_index << 8)
    )


def setup_sub_12b18(channels: int, sndflags: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x0034, channels)
        + mov_ds_byte(0x0082, sndflags)
        + setup_load_common()
        + mov_esi(SRC_OFF)
    )


def setup_sub_12b83(value: int, channel_types: bytes, sound_mode: int = 0) -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_byte(0x00D2, 0)
        + mov_ds_byte(0x0082, 0)
        + mov_ds_byte(0x007A, 0)
        + mov_ds_byte(0x0089, 0x20)
        + mov_ds_byte(0x00DE, sound_mode)
        + mov_ds_word(0x00BE, 22050)
        + mov_ds_word(0x005E, 100)
    )
    for i, channel_type in enumerate(channel_types):
        setup += mov_ds_byte(REAL_CHANNELS_OFF + i * 0x50 + 0x1D, channel_type)
        setup += mov_ds_word(REAL_CHANNELS_OFF + i * 0x50 + 0x3E, 0xAAAA)
    return setup + mov_ax(value)


def setup_read_sndsettings(
    sndcard_type: int,
    base_port: int,
    irq: int,
    dma: int,
    freq_code: int,
    byte_246d8: int,
    byte_246d9: int,
    output_freq: int,
    freq2: int,
    config_word: int,
    sndflags: int,
) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x0132, sndcard_type)
        + mov_ds_word(0x0133, base_port)
        + mov_ds_byte(0x0135, irq)
        + mov_ds_byte(0x0136, dma)
        + mov_ds_byte(0x0137, freq_code)
        + mov_ds_byte(0x0138, byte_246d8)
        + mov_ds_byte(0x0139, byte_246d9)
        + mov_ds_word(0x00BE, output_freq)
        + mov_ds_word(0x0098, freq2)
        + mov_ds_word(0x013A, config_word)
        + mov_ds_byte(0x0082, sndflags)
    )


def setup_sub_12d05(snd_init: int, sndcard_type: int = 0) -> bytes:
    return setup_data_common() + mov_ds_byte(0x00E0, snd_init) + mov_ds_byte(0x0132, sndcard_type) + mov_di(DSEG_SCRATCH)


def setup_someplaymode(playsettings: int, freq: int, channels: int, shift: int, sndflags: int = 0, byte_24629: int = 0x20) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x00D2, playsettings)
        + mov_ds_byte(0x0082, sndflags)
        + mov_ds_byte(0x0089, byte_24629)
        + mov_ds_word(0x00BE, freq)
        + mov_ds_word(0x0034, channels)
        + mov_ds_byte(0x007A, shift)
        + mov_ds_word(REAL_CHANNELS_OFF + 0x3E, 0xAAAA)
    )


def setup_getset_playstate(initial: int, request: int) -> bytes:
    return setup_dual_data_byte(0x00DF, initial) + mov_ax(request)


def setup_get_12f7c(word_245f0: int, word_245f6: int) -> bytes:
    return setup_data_common() + mov_ds_word(0x0050, word_245f0) + mov_ds_word(0x0056, word_245f6)


def setup_snd_off(snd_init: int, snd_set_flag: int, sndcard_type: int) -> bytes:
    return setup_data_common() + mov_ds_byte(0x00E0, snd_init) + mov_ds_byte(0x00E1, snd_set_flag) + mov_ds_byte(0x010C, sndcard_type)


def setup_channel_base() -> bytes:
    return setup_data_common() + mov_bx(CHANNEL_OFF)


def setup_sub_131da(channel_type: int, flags: int, note_byte: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_byte(CHANNEL_OFF + 0x1D, channel_type)
        + mov_ds_byte(CHANNEL_OFF + 0x17, flags)
        + mov_ds_byte(CHANNEL_OFF + 0x35, note_byte)
    )


def setup_sub_131ef(value: int, volume: int, max_volume: int, old_fine: int, flags_3d: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x005C, volume)
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_byte(CHANNEL_OFF + 0x23, old_fine)
        + mov_ds_byte(CHANNEL_OFF + 0x3D, flags_3d)
        + mov_ax(value)
    )


def setup_sub_13177(period: int, dword_245bc: int, dword_245c0: int, shift: int, flags_3d: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_dword(0x001C, dword_245bc)
        + mov_ds_dword(0x0020, dword_245c0)
        + mov_ds_byte(0x007A, shift)
        + mov_ds_byte(CHANNEL_OFF + 0x3D, flags_3d)
        + mov_ds_word(CHANNEL_OFF + 0x3E, 0)
        + mov_ax(period)
    )


def setup_midi_channel(byte_18: int = 0, byte_35: int = 0) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_byte(CHANNEL_OFF + 0x18, byte_18)
        + mov_ds_byte(CHANNEL_OFF + 0x35, byte_35)
    )


def setup_midi_154ac(value: int, max_volume: int, current_volume: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_byte(CHANNEL_OFF + 0x1B, current_volume)
        + mov_ax(value)
    )


def setup_midi_15413_guard(value: int) -> bytes:
    return setup_data_common() + mov_ds_byte(0x00D7, value) + mov_ax(((value & 0xFF) << 8) | 0x34) + mov_dx(0x5678)


def setup_sub_15577_guard() -> bytes:
    return (
        setup_channel_base()
        + mov_ds_byte(CHANNEL_OFF + 0x17, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
        + mov_si(CHANNEL_OFF)
        + mov_di(0x2468)
    )


def setup_sub_15577_mix(
    interpolation: bool,
    wide_accumulator: bool,
    frames: int,
    period: int,
    position: int,
    sample_end: int = 0x00FFFFFF,
    loop_start: int = 0,
    loop_length: int = 0,
    looping: bool = False,
) -> bytes:
    channel = 0x2900
    volume_table = 0x3D68
    sample_segment = LOAD_SEG + SRC_OFF // 16
    setup = (
        setup_data_common()
        + mov_ds_word(0x0044, frames)
        + mov_ds_byte(0x00D2, 0x10 if interpolation else 0)
        + mov_ds_byte(0x0085, 1 if wide_accumulator else 0)
        + mov_ds_byte(channel + 0x17, 1)
        + mov_ds_byte(channel + 0x19, 8 if looping else 0)
        + mov_ds_dword(channel + 0x04, position)
        + mov_ds_word(channel + 0x20, period)
        + mov_ds_byte(channel + 0x23, 0)
        + mov_ds_word(channel + 0x24, sample_segment)
        + mov_ds_word(channel + 0x26, 0xFFFF)
        + mov_ds_word(channel + 0x36, 0)
        + mov_ds_byte(channel + 0x35, 0x77)
        + mov_ds_dword(channel + 0x40, loop_start)
        + mov_ds_dword(channel + 0x44, loop_length)
        + mov_ds_dword(channel + 0x48, sample_end)
        + mov_si(channel)
        + mov_di(DSEG_SCRATCH)
    )
    for index in range(frames * 8):
        setup += mov_ds_byte(DSEG_SCRATCH + index, 0)
    for sample in range(32):
        setup += mov_ds_word(volume_table + sample * 2, sample * 257 - 1000)
    return setup


def sub_15577_mix_state(data: bytes, frames: int) -> tuple[bytes, bytes]:
    channel_offset = 0x2900 - DSEG_SCRATCH
    mix = data[: frames * 8]
    channel = data[channel_offset : channel_offset + 0x50]
    state = (
        channel[0x04:0x08]
        + channel[0x17:0x1A]
        + channel[0x20:0x24]
        + channel[0x35:0x38]
    )
    return mix, state


def setup_sub_1609f_disabled(buffer_size: int = 0x12) -> bytes:
    setup = (
        setup_channel_base()
        + mov_ds_byte(CHANNEL_OFF + 0x17, 0)
        + mov_ds_word(0x0044, buffer_size)
        + mov_si(CHANNEL_OFF)
        + mov_di(DSEG_SCRATCH)
    )
    for index in range(buffer_size * 8):
        setup += mov_ds_byte(DSEG_SCRATCH + index, 0xA5)
    return setup


def setup_setvideomode_noop(mode: int) -> bytes:
    return (
        setup_dseg_common()
        + mov_ds_byte(0x1680, mode)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def setup_text_setup_target(symbol: str) -> bytes:
    setup = (
        setup_dseg_common()
        + mov_ds_dword(0x1630, ((DSEG & 0xFFFF) << 16) | DSEG_SCRATCH)
        + mov_ds_dword(0x162C, 0)
        + mov_ds_word(0x164C, 0xAAAA)
        + mov_ds_word(0x164E, 0xBBBB)
        + mov_ds_word(0x1650, 0xCCCC)
        + mov_ds_word(0x1652, 0xDDDD)
        + mov_ds_word(0x1654, 3)
        + mov_ds_word(0x167E, 0xEEEE)
        + mov_ds_byte(0x1680, 0)
        + mov_ds_byte(0x1696, 1)
        + mov_ds_byte(0x1502, 1)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = (
        copy_bytes_to_scratch(0x164C, DSEG_SCRATCH + 0x3A0, 8)
        + copy_bytes_to_scratch(0x167E, DSEG_SCRATCH + 0x3A8, 2)
        + copy_bytes_to_scratch(0x1680, DSEG_SCRATCH + 0x3AA, 1)
        + copy_bytes_to_scratch(0x1696, DSEG_SCRATCH + 0x3AB, 1)
        + copy_bytes_to_scratch(0x162C, DSEG_SCRATCH + 0x3AC, 4)
    )
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def setup_graph_setup_target(symbol: str) -> bytes:
    mode = 4 if symbol in {"f5_graphspectr", "init_f5_spectr"} else 3
    setup = (
        setup_dseg_common()
        + mov_ds_word(0x164C, 0xAAAA)
        + mov_ds_word(0x164E, 0xBBBB)
        + mov_ds_word(0x1650, 0xCCCC)
        + mov_ds_word(0x1652, 0xDDDD)
        + mov_ds_byte(0x1680, mode)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x164C, DSEG_SCRATCH + 0x3C0, 8)
    post += copy_bytes_to_scratch(0x1680, DSEG_SCRATCH + 0x3C8, 1)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def setup_sub_1ab8c(note_byte: int, transpose: int) -> bytes:
    return (
        setup_dseg_common()
        + b"\x8e\xe0"
        + mov_ds_byte(DSEG_SCRATCH + 0x35, note_byte)
        + mov_bx(DSEG_SCRATCH)
        + mov_cx(transpose)
        + mov_si(0x2222)
    )


def setup_txt_1abae(text: bytes) -> bytes:
    setup = setup_dseg_common() + b"\x8e\xe0"
    for index, value in enumerate(text[:0x16]):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    return setup + mov_si(DSEG_SCRATCH) + mov_di(DSEG_SCRATCH + 0x40)


def setup_sub_13826(value: int, table_word: int) -> bytes:
    di = (((value & 0x0F) - 1) & 0x0F) * 2
    return (
        setup_channel_base()
        + mov_ds_byte(0x007A, 1)
        + mov_ds_word(0x013E + di, table_word)
        + mov_ds_word(CHANNEL_OFF + 0x14, 0)
        + mov_ax(value)
    )


def setup_sub_137d5_out_of_range(flags_3d: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_byte(CHANNEL_OFF + 0x0A, 33)
        + mov_ds_byte(CHANNEL_OFF + 0x0B, 0x77)
        + mov_ds_byte(CHANNEL_OFF + 0x3D, flags_3d)
        + mov_ax(0x1234)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def setup_sub_13429_disabled() -> bytes:
    return (
        setup_channel_base()
        + mov_ds_byte(CHANNEL_OFF + 0x17, 0)
        + mov_ds_byte(CHANNEL_OFF + 0x03, 0x55)
        + mov_ax(0x1234)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def setup_sub_13d95(divisor: int) -> bytes:
    return setup_data_common() + mov_cx(divisor)


def setup_sub_13cf6(value: int, freq: int, buffer_size: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x0082, 0)
        + mov_ds_word(0x00BE, freq)
        + mov_ds_word(0x0048, buffer_size)
        + mov_ax(value)
    )


def setup_spectr_1bce9_equal(value: int) -> bytes:
    return (
        setup_dseg_common()
        + mov_ds_byte(DSEG_SCRATCH, value)
        + mov_ds_byte(DSEG_SCRATCH + 0x64, value)
        + mov_bx(DSEG_SCRATCH)
        + mov_bp(DSEG_SCRATCH + 0x1000)
    )


def setup_spectr_1bc2d_equal() -> bytes:
    setup = setup_dseg_common()
    for index in range(99):
        setup += mov_ds_byte(DSEG_SCRATCH + index, 0)
        setup += mov_ds_byte(DSEG_SCRATCH + 0x64 + index, 0)
        setup += mov_ds_byte(DSEG_SCRATCH + 0x12C + index, 0)
    return setup + mov_bx(DSEG_SCRATCH) + mov_bp(DSEG_SCRATCH + 0x1000)


def setup_spectr_1bbc1_zero_bin() -> bytes:
    return (
        setup_dseg_common()
        + mov_ds_byte(0x1691, 0)
        + mov_cx(1)
        + mov_si(DSEG_SCRATCH)
        + mov_di(DSEG_SCRATCH + 0x100)
    )


def setup_video_prp_mtr_positn(values: list[int]) -> bytes:
    setup = (
        setup_dseg_common()
        + mov_ds_dword(0x1638, ((DSEG & 0xFFFF) << 16) | (DSEG_SCRATCH & 0xFFFF))
        + mov_ds_word(0x1654, len(values))
    )
    for index, value in enumerate(values):
        setup += mov_ds_byte(DSEG_SCRATCH + index * 0x50 + 0x3A, value)
    return setup


def setup_noop_probe() -> bytes:
    return setup_data_common() + mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)


def setup_eff_nibble(initial: int, value: int) -> bytes:
    return setup_channel_base() + mov_ds_byte(CHANNEL_OFF + 0x09, initial) + mov_ax(value)


def setup_sub_14087(value: int, stored: int, byte_24668: int) -> bytes:
    return setup_channel_base() + mov_ds_byte(CHANNEL_OFF + 0x34, stored) + mov_ds_byte(0x00C8, byte_24668) + mov_ax(value)


def setup_eff_13a43(flags: int, sndflags: int, value: int) -> bytes:
    return setup_channel_base() + mov_ds_byte(0x0082, sndflags) + mov_ds_byte(CHANNEL_OFF + 0x17, flags) + mov_ax(value)


def setup_eff_13a94(byte_16: int, sample_end: int, byte_2461a: int, flags: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002E, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x007A, byte_2461a)
        + mov_ds_byte(CHANNEL_OFF + 0x16, byte_16)
        + mov_ds_byte(CHANNEL_OFF + 0x17, flags)
        + mov_ds_dword(CHANNEL_OFF + 0x30, sample_end)
        + mov_ds_word(CHANNEL_OFF + 0x4C, 0xAAAA)
        + mov_ax(value)
    )


def setup_eff_13ad7(volume: int, max_volume: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ax(value)
    )


def setup_eff_13b06(playsettings: int, value: int) -> bytes:
    return setup_data_common() + mov_ds_byte(0x00D2, playsettings) + mov_ds_word(0x0050, 0xAAAA) + mov_ax(value)


def setup_eff_13b78(volume: int, max_volume: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ax(volume)
    )


def setup_eff_13b88(initial_24669: int, initial_2466a: int, value: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x00C9, initial_24669)
        + mov_ds_byte(0x00CA, initial_2466a)
        + mov_ax(value)
    )


def setup_eff_13bb2(flags: int, value: int) -> bytes:
    return setup_channel_base() + mov_ds_byte(CHANNEL_OFF + 0x17, flags) + mov_ax(value)


def setup_eff_13ba3(flags: int, value: int) -> bytes:
    return setup_channel_base() + mov_ds_byte(CHANNEL_OFF + 0x17, flags) + mov_ax(value)


def setup_eff_13bc8(byte_2461a: int, dx: int, value: int) -> bytes:
    return setup_channel_base() + mov_ds_byte(0x007A, byte_2461a) + mov_dx(dx) + mov_ax(value)


def setup_eff_13c02(byte_24668: int, word_245f6: int, byte_3b: int, byte_3c: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_word(0x0056, word_245f6)
        + mov_ds_byte(0x00C9, 0xAA)
        + mov_ds_byte(0x00CB, 0xBB)
        + mov_ds_byte(CHANNEL_OFF + 0x3B, byte_3b)
        + mov_ds_byte(CHANNEL_OFF + 0x3C, byte_3c)
        + mov_ax(value)
    )


def setup_eff_13c3f(byte_24668: int, flags: int, sndflags: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_byte(0x0082, sndflags)
        + mov_ds_byte(CHANNEL_OFF + 0x17, flags)
        + mov_ax(value)
    )


def setup_eff_13c64(byte_24668: int, flags_3d: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x0028, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_byte(CHANNEL_OFF + 0x3D, flags_3d)
        + mov_ax(value)
    )


def setup_eff_13c88(volume: int, byte_24668: int, max_volume: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ax(value)
    )


def setup_eff_13c95(volume: int, byte_24668: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ax(value)
    )


def setup_eff_13ca2(byte_24668: int, value: int) -> bytes:
    return setup_channel_base() + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"]) + mov_ds_byte(0x00C8, byte_24668) + mov_ax(value)


def setup_eff_13cb3(period: int, byte_0a: int, byte_0b: int, byte_24668: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_word(CHANNEL_OFF, period)
        + mov_ds_byte(CHANNEL_OFF + 0x0A, byte_0a)
        + mov_ds_byte(CHANNEL_OFF + 0x0B, byte_0b)
        + mov_ax(value)
    )


def setup_eff_13cc9(byte_24668: int, byte_2466d: int, initial_2466c: int, value: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_byte(0x00CD, byte_2466d)
        + mov_ds_byte(0x00CC, initial_2466c)
        + mov_ax(value)
    )


def setup_eff_13cdd(playsettings: int, initial_24667: int, initial_24668: int, value: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x00D2, playsettings)
        + mov_ds_byte(0x00C7, initial_24667)
        + mov_ds_byte(0x00C8, initial_24668)
        + mov_ax(value)
    )


def setup_eff_13de5(initial_period: int, byte_24668: int, stored_34: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002A, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_word(CHANNEL_OFF, initial_period)
        + mov_ds_byte(CHANNEL_OFF + 0x34, stored_34)
        + mov_ax(value)
    )


def setup_eff_13e1e(current: int, target: int, step: int, flags: int, value: int) -> bytes:
    return setup_effect_target_slide(current, target, step, flags, value)


def setup_eff_13e2d(period: int, byte_09: int, byte_0c: int, byte_0d: int, playsettings: int, value: int) -> bytes:
    return setup_eff_1392f(period, byte_09, byte_0c, byte_0d, playsettings, value)


def setup_eff_13e32(volume: int, byte_24668: int, max_volume: int, stored_34: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ds_byte(CHANNEL_OFF + 0x34, stored_34)
        + mov_ax(value)
    )


def setup_eff_13e7f(
    period: int,
    target: int,
    step: int,
    flags: int,
    volume: int,
    byte_24668: int,
    max_volume: int,
    stored_34: int,
    value: int,
) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002A, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_word(CHANNEL_OFF, period)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ds_word(CHANNEL_OFF + 0x10, target)
        + mov_ds_word(CHANNEL_OFF + 0x12, step)
        + mov_ds_byte(CHANNEL_OFF + 0x17, flags)
        + mov_ds_byte(CHANNEL_OFF + 0x34, stored_34)
        + mov_ax(value)
    )


def setup_eff_13e84(
    period: int,
    byte_09: int,
    byte_0c: int,
    byte_0d: int,
    playsettings: int,
    volume: int,
    byte_24668: int,
    max_volume: int,
    stored_34: int,
    value: int,
) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002A, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x005E, playsettings)
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_word(CHANNEL_OFF, period)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ds_byte(CHANNEL_OFF + 0x09, byte_09)
        + mov_ds_byte(CHANNEL_OFF + 0x0C, byte_0c)
        + mov_ds_byte(CHANNEL_OFF + 0x0D, byte_0d)
        + mov_ds_byte(CHANNEL_OFF + 0x34, stored_34)
        + mov_ax(value)
    )


def setup_eff_13e8c(value: int, freq: int, buffer_size: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x0082, 0)
        + mov_ds_word(0x00BE, freq)
        + mov_ds_word(0x0048, buffer_size)
        + mov_ax(value)
    )


def setup_eff_13f05(volume: int, byte_24668: int, stored_34: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ds_byte(CHANNEL_OFF + 0x34, stored_34)
        + mov_ax(value)
    )


def setup_eff_13f3b(volume: int, byte_24668: int, max_volume: int, flags_3d: int, stored_34: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x0028, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ds_byte(CHANNEL_OFF + 0x34, stored_34)
        + mov_ds_byte(CHANNEL_OFF + 0x3D, flags_3d)
        + mov_ax(value)
    )


def setup_eff_13fbe(period: int, byte_0b: int, byte_24668: int, stored_34: int, byte_35: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002A, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00C8, byte_24668)
        + mov_ds_word(CHANNEL_OFF, period)
        + mov_ds_byte(CHANNEL_OFF + 0x0B, byte_0b)
        + mov_ds_byte(CHANNEL_OFF + 0x34, stored_34)
        + mov_ds_byte(CHANNEL_OFF + 0x35, byte_35)
        + mov_ax(value)
    )


def setup_tempo_common(freq: int, buffer_size: int) -> bytes:
    return mov_ds_byte(0x0082, 0) + mov_ds_word(0x00BE, freq) + mov_ds_word(0x0048, buffer_size)


def setup_eff_14020(value: int, sound_mode: int, channels: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x00DE, sound_mode)
        + mov_ds_word(0x0036, channels)
        + mov_ax(value)
    )


def setup_change_amplif(value: int, sound_mode: int, channels: int) -> bytes:
    return setup_eff_14020(value, sound_mode, channels)


def setup_eff_14030(value: int, byte_2467c: int, freq: int, buffer_size: int) -> bytes:
    return setup_data_common() + setup_tempo_common(freq, buffer_size) + mov_ds_byte(0x00DC, byte_2467c) + mov_ax(value)


def setup_calc_14043(byte_2467b: int, byte_2467c: int) -> bytes:
    return setup_data_common() + mov_ds_byte(0x00DB, byte_2467b) + mov_ds_byte(0x00DC, byte_2467c)


def setup_eff_14067(value: int, byte_2467b: int, byte_2467c: int, freq: int, buffer_size: int) -> bytes:
    return (
        setup_data_common()
        + setup_tempo_common(freq, buffer_size)
        + mov_ds_byte(0x00DB, byte_2467b)
        + mov_ds_byte(0x00DC, byte_2467c)
        + mov_ax(value)
    )


def setup_effect_slide(initial_period: int, value: int, active_channel: int = 0) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002A, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00C8, active_channel)
        + mov_ds_word(CHANNEL_OFF, initial_period)
        + mov_ax(value)
    )


def setup_effect_target_slide(current: int, target: int, step: int, flags: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002A, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_word(CHANNEL_OFF, current)
        + mov_ds_word(CHANNEL_OFF + 0x10, target)
        + mov_ds_word(CHANNEL_OFF + 0x12, step)
        + mov_ds_byte(CHANNEL_OFF + 0x17, flags)
        + mov_ax(value)
    )


def setup_eff_1392f(period: int, byte_09: int, byte_0c: int, byte_0d: int, playsettings: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002A, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x005E, playsettings)
        + mov_ds_word(CHANNEL_OFF, period)
        + mov_ds_byte(CHANNEL_OFF + 0x09, byte_09)
        + mov_ds_byte(CHANNEL_OFF + 0x0C, byte_0c)
        + mov_ds_byte(CHANNEL_OFF + 0x0D, byte_0d)
        + mov_ax(value)
    )


def setup_eff_139ac(period: int, target: int, step: int, flags: int, volume: int, max_volume: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002A, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_word(CHANNEL_OFF, period)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ds_word(CHANNEL_OFF + 0x10, target)
        + mov_ds_word(CHANNEL_OFF + 0x12, step)
        + mov_ds_byte(CHANNEL_OFF + 0x17, flags)
        + mov_ax(value)
    )


def setup_eff_139b2(
    period: int,
    byte_09: int,
    byte_0c: int,
    byte_0d: int,
    playsettings: int,
    volume: int,
    max_volume: int,
    value: int,
) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002A, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x005E, playsettings)
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_word(CHANNEL_OFF, period)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ds_byte(CHANNEL_OFF + 0x09, byte_09)
        + mov_ds_byte(CHANNEL_OFF + 0x0C, byte_0c)
        + mov_ds_byte(CHANNEL_OFF + 0x0D, byte_0d)
        + mov_ax(value)
    )


def setup_eff_139b9(volume: int, byte_09: int, byte_0e: int, byte_0f: int, max_volume: int, value: int) -> bytes:
    return (
        setup_channel_base()
        + mov_ds_word(0x002C, FALLBACK_OFFSETS["nullsub_5"])
        + mov_ds_byte(0x00DD, max_volume)
        + mov_ds_byte(CHANNEL_OFF + 0x08, volume)
        + mov_ds_byte(CHANNEL_OFF + 0x09, byte_09)
        + mov_ds_byte(CHANNEL_OFF + 0x0E, byte_0e)
        + mov_ds_byte(CHANNEL_OFF + 0x0F, byte_0f)
        + mov_ax(value)
    )


def setup_eff_13ce8(initial_24667: int, initial_24668: int, value: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x00C7, initial_24667)
        + mov_ds_byte(0x00C8, initial_24668)
        + mov_ax(value)
    )


def setup_hex16(value: int) -> bytes:
    return setup_common() + mov_ax(value) + b"\xbe" + struct.pack("<H", DST_OFF)


def setup_hex8(value: int) -> bytes:
    return setup_common() + mov_ax(value) + b"\xbe" + struct.pack("<H", DST_OFF)


def setup_hex4(value: int) -> bytes:
    return setup_common() + mov_ax(value) + b"\xbe" + struct.pack("<H", DST_OFF)


def setup_dseg_hex(value: int) -> bytes:
    return setup_dseg_common() + mov_eax(value) + b"\xbe" + struct.pack("<H", DSEG_SCRATCH)


def setup_dseg_hex_1be39(value: int, attr: int) -> bytes:
    return setup_dseg_common() + mov_ax(((attr & 0xFF) << 8) | (value & 0xFF)) + mov_di(DSEG_SCRATCH)


def setup_putdigit(value: int, count: int = 0) -> bytes:
    return setup_common() + mov_dx(value) + b"\xbe" + struct.pack("<H", DST_OFF) + mov_cx(count)


def setup_dseg_putdigit(value: int, count: int = 0) -> bytes:
    return setup_dseg_common() + mov_dx(value) + b"\xbe" + struct.pack("<H", DSEG_SCRATCH) + mov_cx(count)


def setup_decimal16(value: int) -> bytes:
    return setup_common() + mov_ax(value) + b"\xbe" + struct.pack("<H", DST_OFF)


def setup_decimal32(value: int) -> bytes:
    return setup_common() + mov_eax(value) + b"\xbe" + struct.pack("<H", DST_OFF)


def setup_dseg_decimal16(value: int) -> bytes:
    return setup_dseg_common() + mov_ax(value) + b"\xbe" + struct.pack("<H", DSEG_SCRATCH)


def setup_dseg_decimal32(value: int) -> bytes:
    return setup_dseg_common() + mov_eax(value) + b"\xbe" + struct.pack("<H", DSEG_SCRATCH)


def setup_dseg_u32toa(value: int, radix: int) -> bytes:
    return setup_dseg_common() + mov_eax(value) + b"\x66\xbb" + struct.pack("<I", radix & 0xFFFFFFFF) + b"\xbe" + struct.pack("<H", DSEG_SCRATCH) + mov_cx(0)


def setup_u32toa0_direct(value: int, radix: int) -> bytes:
    return setup_common() + mov_eax(value) + b"\x66\xbb" + struct.pack("<I", radix & 0xFFFFFFFF) + b"\xbe" + struct.pack("<H", DST_OFF) + mov_cx(0)


def setup_dseg_strlen(text: bytes) -> bytes:
    setup = setup_dseg_common()
    for index, value in enumerate(text + b"\0"):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    return setup + b"\xbe" + struct.pack("<H", DSEG_SCRATCH)


def setup_dseg_u32toa_fill(value: int, count: int) -> bytes:
    return setup_dseg_common() + mov_eax(value) + mov_di(DSEG_SCRATCH) + mov_bp(count)


def setup_dseg_copyprint(text: bytes, count: int) -> bytes:
    setup = setup_dseg_common()
    for index, value in enumerate(text):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    return setup + b"\xbe" + struct.pack("<H", DSEG_SCRATCH) + mov_di(DSEG_SCRATCH + 0x40) + mov_cx(count)


def setup_dseg_strcpy_count(text: bytes) -> bytes:
    setup = setup_dseg_common()
    for index, value in enumerate(text + b"\0"):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    return setup + b"\xbe" + struct.pack("<H", DSEG_SCRATCH) + mov_di(DSEG_SCRATCH + 0x40)


def setup_dseg_put_message2(text: bytes, attr: int) -> bytes:
    setup = setup_dseg_common()
    for index, value in enumerate(text[1:] + b"\0"):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    return setup + b"\x8e\xe0" + b"\xbe" + struct.pack("<H", DSEG_SCRATCH) + mov_di(DSEG_SCRATCH + 0x40) + mov_ax(((attr & 0xFF) << 8) | text[0])


def setup_dseg_text1bf69(text: bytes, attr: int) -> bytes:
    setup = setup_dseg_common()
    for index, value in enumerate(text + b"\0"):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    return setup + b"\xbe" + struct.pack("<H", DSEG_SCRATCH) + mov_di(DSEG_SCRATCH + 0x40) + mov_ax((attr & 0xFF) << 8)


def setup_dseg_put_message(text: bytes, attr: int) -> bytes:
    setup = setup_dseg_common()
    for index, value in enumerate(text + b"\0"):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    return setup + b"\xbe" + struct.pack("<H", DSEG_SCRATCH) + mov_di(DSEG_SCRATCH + 0x40) + mov_ax((attr & 0xFF) << 8)


def setup_dseg_draw_frame(style: int, attr: int, fill_attr: int, x: int, y: int, right: int, bottom: int) -> bytes:
    return (
        setup_dseg_common()
        + mov_ds_dword(0x1630, ((DSEG & 0xFFFF) << 16) | (DSEG_SCRATCH & 0xFFFF))
        + mov_ax(((attr & 0xFF) << 8) | (style & 0xFF))
        + mov_bx(fill_attr)
        + mov_cx(((y & 0xFF) << 8) | (x & 0xFF))
        + mov_dx(((bottom & 0xFF) << 8) | (right & 0xFF))
    )


def setup_dseg_txt_draw_top_title() -> bytes:
    return setup_dseg_common() + mov_ds_dword(0x1630, ((DSEG & 0xFFFF) << 16) | (DSEG_SCRATCH & 0xFFFF))


def setup_dseg_message_1be77(text: bytes, y: int, attr: int) -> bytes:
    src = DSEG_SCRATCH + 0x500
    setup = setup_dseg_common() + mov_ds_dword(0x1630, ((DSEG & 0xFFFF) << 16) | (DSEG_SCRATCH & 0xFFFF))
    for index, value in enumerate(text + b"\0"):
        setup += mov_ds_byte(src + index, value)
    return setup + mov_ax(((attr & 0xFF) << 8) | (y & 0xFF)) + mov_esi(src)


def recolortxt_base(row: int) -> int:
    return row * 160 + 80 * 2 * 10 + 8 * 2 + 1


def setup_dseg_recolortxt(row: int, color: int) -> bytes:
    base = recolortxt_base(row)
    setup = setup_dseg_common()
    for index in range(64):
        setup += mov_ds_byte(base + index * 2, 0xA0 | (index & 0x0F))
    return setup + mov_ax(row) + mov_bx(color)


def setup_mouse_state(exists: int, visible: int) -> bytes:
    return (
        setup_dseg_common()
        + mov_ds_word(0x169C, 0xAAAA)
        + mov_ds_word(0x169E, 0xBBBB)
        + mov_ds_byte(0x16A0, 0xCC)
        + mov_ds_byte(0x16A1, exists)
        + mov_ds_byte(0x16A2, visible)
        + mov_bx(0x1111)
        + mov_cx(0x2222)
        + mov_dx(0x3333)
    )


def setup_mouse_1c7a9(x: int, y: int, left: int, top: int, right: int, bottom: int) -> bytes:
    return setup_dseg_common() + mov_ax(x) + mov_bp(y) + mov_cx(left) + mov_dx(top) + mov_esi(right) + mov_di(bottom)


def mouse_rect_record(left: int, top: int, right: int, bottom: int, value: int) -> bytes:
    return struct.pack("<HHHHH", left & 0xFFFF, top & 0xFFFF, right & 0xFFFF, bottom & 0xFFFF, value & 0xFFFF)


def mouse_rect_sentinel() -> bytes:
    return struct.pack("<H", 0xFFFF) + b"\0" * 8


def setup_mouse_1c7cf(x: int, y: int, records: bytes) -> bytes:
    setup = setup_dseg_common()
    for index, value in enumerate(records):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    return setup + mov_ax(x) + mov_bp(y) + mov_bx(DSEG_SCRATCH)


def setup_strlen() -> bytes:
    return setup_common() + b"\xbe" + struct.pack("<H", SRC_OFF)


def setup_strcpy() -> bytes:
    return setup_common() + b"\xbe" + struct.pack("<H", SRC_OFF) + b"\xbf" + struct.pack("<H", DST_OFF)


def setup_copyprint(count: int) -> bytes:
    return setup_strcpy() + mov_cx(count)


def setup_fill_dseg_byte(offset: int, count: int, value: int) -> bytes:
    return setup_data_common() + mov_di(offset) + mov_cx(count) + b"\xfc\xb0" + bytes([value & 0xFF]) + b"\xf3\xaa"


def field(output: str, name: str) -> str:
    match = re.search(rf"\b{name}=([^\s]+)", output)
    if not match:
        raise AssertionError(f"missing {name}= in {output!r}")
    return match.group(1).lower()


def test_original_and_translated_hex16() -> None:
    for value, expected in [(0, b"0000"), (0x1234, b"1234"), (0xFFFF, b"FFFF")]:
        out, got = original_call(original_offset("u16tox"), setup_hex16(value), dump_count=4)
        assert got == expected
        translated_out = translated("hex16", hex(value))
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
        assert field(out, "si") == f"{DST_OFF + 4:04x}"


def test_original_and_abi_hex16_public_symbol() -> None:
    for value, expected in [(0, b"0000"), (0x1234, b"1234"), (0xFFFF, b"FFFF")]:
        out, got = original_call(original_offset("u16tox"), setup_hex16(value), dump_count=4)
        translated_out = translated("abihex16", hex(value))
        assert got == expected
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
            assert field(translated_out, "si") == field(out, "si")
        assert field(out, "si") == f"{DST_OFF + 4:04x}"


def test_original_hex8_and_hex4() -> None:
    for value, expected in [(0, b"00"), (0x7B, b"7B"), (0xFF, b"FF")]:
        out, got = original_call(original_offset("u8tox"), setup_hex8(value), dump_count=2)
        translated_out = translated("hex8", hex(value))
        assert got == expected
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
        assert field(out, "si") == f"{DST_OFF + 2:04x}"

    for value, expected in [(0, b"0"), (9, b"9"), (10, b"A"), (15, b"F")]:
        out, got = original_call(original_offset("u4tox"), setup_hex4(value), dump_count=1)
        translated_out = translated("hex4", hex(value))
        assert got == expected
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
        assert field(out, "si") == f"{DST_OFF + 1:04x}"


def test_original_and_abi_hex8_and_hex4_public_symbols() -> None:
    for value, expected in [(0, b"00"), (0x7B, b"7B"), (0xFF, b"FF")]:
        out, got = original_call(original_offset("u8tox"), setup_hex8(value), dump_count=2)
        translated_out = translated("abihex8", hex(value))
        assert got == expected
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
            assert field(translated_out, "si") == field(out, "si")
        assert field(out, "si") == f"{DST_OFF + 2:04x}"

    for value, expected in [(0, b"0"), (9, b"9"), (10, b"A"), (15, b"F")]:
        out, got = original_call(original_offset("u4tox"), setup_hex4(value), dump_count=1)
        translated_out = translated("abihex4", hex(value))
        assert got == expected
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
            assert field(translated_out, "si") == field(out, "si")
        assert field(out, "si") == f"{DST_OFF + 1:04x}"


def test_original_putdigit_writes_dl_and_advances() -> None:
    for value, expected in [(ord("0"), b"0"), (ord("5"), b"5"), (ord("-"), b"-")]:
        out, got = original_call(original_offset("my_putdigit"), setup_putdigit(value, count=0x22), dump_count=1)
        translated_out = translated("putdigit", hex(value), "0x22")
        assert got == expected
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
            assert field(translated_out, "cx") == "0023"
        assert field(out, "si") == f"{DST_OFF + 1:04x}"
        assert field(out, "cx") == "0023"


def test_original_unsigned_decimal_converters() -> None:
    cases = [
        ("my_u8toa_10", 0, setup_decimal16(0), b"0"),
        ("my_u8toa_10", 255, setup_decimal16(255), b"255"),
        ("my_u16toa_10", 65535, setup_decimal16(65535), b"65535"),
        ("my_u32toa10_0", 0, setup_decimal32(0), b"0"),
        ("my_u32toa10_0", 1234567890, setup_decimal32(1234567890), b"1234567890"),
    ]
    for symbol, value, setup, expected in cases:
        out, got = original_call(original_offset(symbol), setup, dump_count=len(expected))
        translated_out = translated("decimal", symbol, hex(value))
        assert got == expected
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
            assert field(translated_out, "cx") == f"{len(expected):04x}"
        assert field(out, "si") == f"{DST_OFF + len(expected):04x}"
        assert field(out, "cx") == f"{len(expected):04x}"


def test_original_and_abi_unsigned_decimal16_public_symbols() -> None:
    cases = [
        ("my_u8toa_10", 0, setup_decimal16(0), b"0"),
        ("my_u8toa_10", 255, setup_decimal16(255), b"255"),
        ("my_u16toa_10", 65535, setup_decimal16(65535), b"65535"),
    ]
    for symbol, value, setup, expected in cases:
        out, got = original_call(original_offset(symbol), setup, dump_count=len(expected))
        translated_out = translated("abidecimal16", symbol, hex(value))
        assert got == expected
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
            assert field(translated_out, "cx") == field(out, "cx")
            assert field(translated_out, "si") == field(out, "si")
        assert field(out, "si") == f"{DST_OFF + len(expected):04x}"
        assert field(out, "cx") == f"{len(expected):04x}"


def test_original_signed_decimal_converters() -> None:
    cases = [
        ("my_i8toa10_0", 0xFF, setup_decimal16(0xFF), b"-1"),
        ("my_i8toa10_0", 0x7F, setup_decimal16(0x7F), b"127"),
        ("my_i16toa10_0", 0xFFFF, setup_decimal16(0xFFFF), b"-1"),
        ("my_i16toa10_0", 0x8000, setup_decimal16(0x8000), b"-32768"),
        ("my_i32toa10_0", 0xFFFFFFFF, setup_decimal32(0xFFFFFFFF), b"-1"),
        ("my_i32toa10_0", 0x7FFFFFFF, setup_decimal32(0x7FFFFFFF), b"2147483647"),
    ]
    for symbol, value, setup, expected in cases:
        out, got = original_call(original_offset(symbol), setup, dump_count=len(expected))
        translated_out = translated("decimal", symbol, hex(value))
        assert got == expected
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
            assert field(translated_out, "cx") == f"{len(expected):04x}"
        assert field(out, "si") == f"{DST_OFF + len(expected):04x}"
        assert field(out, "cx") == f"{len(expected):04x}"


def test_original_and_abi_signed_decimal16_public_symbols() -> None:
    cases = [
        ("my_i8toa10_0", 0xFF, setup_decimal16(0xFF), b"-1"),
        ("my_i8toa10_0", 0x7F, setup_decimal16(0x7F), b"127"),
        ("my_i16toa10_0", 0xFFFF, setup_decimal16(0xFFFF), b"-1"),
        ("my_i16toa10_0", 0x8000, setup_decimal16(0x8000), b"-32768"),
    ]
    for symbol, value, setup, expected in cases:
        out, got = original_call(original_offset(symbol), setup, dump_count=len(expected))
        translated_out = translated("abidecimal16", symbol, hex(value))
        assert got == expected
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
            assert field(translated_out, "cx") == field(out, "cx")
            assert field(translated_out, "si") == field(out, "si")
        assert field(out, "si") == f"{DST_OFF + len(expected):04x}"
        assert field(out, "cx") == f"{len(expected):04x}"


def test_original_and_translated_seg001_hex_converters() -> None:
    cases = [
        ("my_u4tox", 0x0A, 1),
        ("my_u8tox", 0x7B, 2),
        ("my_u16tox", 0x1234, 4),
        ("my_u32tox", 0x89ABCDEF, 8),
    ]
    for symbol, value, count in cases:
        out, got = original_seg001_call(original_offset(symbol), setup_dseg_hex(value), dump_count=count)
        translated_out = translated("seg1hex", symbol, hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + got.hex())
        assert field(out, "si") == f"{DSEG_SCRATCH + count:04x}"


def test_original_and_abi_seg001_hex_public_symbols() -> None:
    cases = [
        ("my_u4tox", 0x0A, 1),
        ("my_u8tox", 0x7B, 2),
        ("my_u16tox", 0x1234, 4),
        ("my_u32tox", 0x89ABCDEF, 8),
    ]
    for symbol, value, count in cases:
        out, data = original_seg001_call(original_offset(symbol), setup_dseg_hex(value), dump_count=count)
        got = translated("abimyhex", symbol, hex(value))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "si") == field(out, "si")


def test_original_and_translated_hex_1be39_digit_writer() -> None:
    cases = [
        (0x00, 0x07, b"0\x07"),
        (0x09, 0x1E, b"9\x1e"),
        (0x0A, 0x2F, b"A\x2f"),
        (0x0F, 0x70, b"F\x70"),
        (0xAB, 0x44, b"B\x44"),
    ]
    for value, attr, expected in cases:
        out, got = original_seg001_call(original_offset("hex_1BE39"), setup_dseg_hex_1be39(value, attr), dump_count=2)
        assert got == expected
        translated_out = translated("hex1be39", hex(value), hex(attr))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert field(translated_out, "di") == field(out, "di")
            assert translated_out.endswith("data=" + expected.hex())
        assert field(out, "di") == f"{DSEG_SCRATCH + 2:04x}"


def test_original_and_abi_hex_1be39_public_symbol() -> None:
    for value, attr in [(0x00, 0x07), (0x09, 0x1E), (0x0A, 0x2F), (0x0F, 0x70), (0xAB, 0x44)]:
        out, data = original_seg001_call(original_offset("hex_1BE39"), setup_dseg_hex_1be39(value, attr), dump_count=2)
        got = translated("abihex1be39", hex(value), hex(attr))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "di") == field(out, "di")
            assert got.endswith("data=" + data.hex())


def test_original_and_translated_seg001_putdigit() -> None:
    for value in [ord("0"), ord("7"), ord("-")]:
        out, got = original_seg001_call(original_offset("myputdigit"), setup_dseg_putdigit(value, count=0x10), dump_count=1)
        translated_out = translated("seg1putdigit", hex(value), "0x10")
        assert got == bytes([value])
        if translated_out is not None:
            assert field(translated_out, "cx") == "0011"
            assert translated_out.endswith("data=" + got.hex())
        assert field(out, "cx") == "0011"
        assert field(out, "si") == f"{DSEG_SCRATCH + 1:04x}"


def test_original_and_abi_seg001_myputdigit_public_symbol() -> None:
    for value in [ord("0"), ord("7"), ord("-")]:
        out, data = original_seg001_call(original_offset("myputdigit"), setup_dseg_putdigit(value, count=0x10), dump_count=1)
        got = translated("abimyputdigit", "0x10", hex(value))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")


def test_original_and_translated_seg001_decimal_converters() -> None:
    cases = [
        ("my_u8toa10", 255, setup_dseg_decimal16(255), b"255"),
        ("my_u16toa10", 65535, setup_dseg_decimal16(65535), b"65535"),
        ("my_u32toa10", 1234567890, setup_dseg_decimal32(1234567890), b"1234567890"),
        ("my_i8toa10", 0xFF, setup_dseg_decimal16(0xFF), b"-1"),
        ("my_i8toa10", 0x7F, setup_dseg_decimal16(0x7F), b"127"),
    ]
    for symbol, value, setup, expected in cases:
        out, got = original_seg001_call(original_offset(symbol), setup, dump_count=len(expected))
        assert got == expected
        translated_out = translated("seg1decimal", symbol, hex(value))
        if translated_out is not None:
            assert field(translated_out, "cx") == f"{len(expected):04x}"
            assert translated_out.endswith("data=" + expected.hex())
        assert field(out, "cx") == f"{len(expected):04x}"
        assert field(out, "si") == f"{DSEG_SCRATCH + len(expected):04x}"


def test_original_and_abi_seg001_unsigned_decimal_public_symbols() -> None:
    cases = [
        ("my_u8toa10", 255, setup_dseg_decimal16(255), b"255"),
        ("my_u16toa10", 65535, setup_dseg_decimal16(65535), b"65535"),
        ("my_u32toa10", 1234567890, setup_dseg_decimal32(1234567890), b"1234567890"),
    ]
    for symbol, value, setup, expected in cases:
        out, data = original_seg001_call(original_offset(symbol), setup, dump_count=len(expected))
        got = translated("abimyutoa10", symbol, hex(value))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")
        assert data == expected


def test_original_and_abi_seg001_signed_decimal_public_symbols() -> None:
    cases = [
        ("my_i8toa10", 0xFF, setup_dseg_decimal16(0xFF), b"-1"),
        ("my_i8toa10", 0x7F, setup_dseg_decimal16(0x7F), b"127"),
        ("my_i16toa10", 0x8000, setup_dseg_decimal16(0x8000), b"-32768"),
        ("my_i32toa10", 0xFFFE1DC0, setup_dseg_decimal32(0xFFFE1DC0), b"-123456"),
    ]
    for symbol, value, setup, expected in cases:
        out, data = original_seg001_call(original_offset(symbol), setup, dump_count=len(expected))
        got = translated("abimyitoa10", symbol, hex(value))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")
        assert data == expected


def test_original_and_translated_seg001_recursive_u32toa() -> None:
    for value, expected in [(0, b"0"), (12345, b"12345"), (0xFFFFFFFF, b"4294967295")]:
        out, got = original_seg001_call(original_offset("my_u32toa"), setup_dseg_u32toa(value, 10), dump_count=len(expected))
        assert got == expected
        translated_out = translated("seg1u32toa", hex(value), "10")
        if translated_out is not None:
            assert field(translated_out, "cx") == f"{len(expected):04x}"
            assert translated_out.endswith("data=" + expected.hex())
        assert field(out, "cx") == f"{len(expected):04x}"
        assert field(out, "si") == f"{DSEG_SCRATCH + len(expected):04x}"


def test_original_and_abi_seg001_recursive_u32toa_public_symbol() -> None:
    for value, base, expected in [(0, 10, b"0"), (12345, 10, b"12345"), (0xFFFFFFFF, 10, b"4294967295")]:
        out, data = original_seg001_call(original_offset("my_u32toa"), setup_dseg_u32toa(value, base), dump_count=len(expected))
        got = translated("abimyu32toa", hex(value), hex(base))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")
        assert data == expected


def test_original_and_translated_seg000_recursive_u32toa_0() -> None:
    for value, expected in [(0, b"0"), (12345, b"12345"), (0xFFFFFFFF, b"4294967295")]:
        out, got = original_call(original_offset("my_u32toa_0"), setup_u32toa0_direct(value, 10), dump_count=len(expected))
        assert got == expected
        translated_out = translated("u32toa0direct", hex(value), "10")
        if translated_out is not None:
            assert field(translated_out, "cx") == f"{len(expected):04x}"
            assert translated_out.endswith("data=" + expected.hex())
        assert field(out, "cx") == f"{len(expected):04x}"
        assert field(out, "si") == f"{DST_OFF + len(expected):04x}"


def test_original_and_abi_seg000_recursive_u32toa_0_public_symbol() -> None:
    for value, expected in [(0, b"0"), (12345, b"12345"), (0xFFFFFFFF, b"4294967295")]:
        out, got = original_call(original_offset("my_u32toa_0"), setup_u32toa0_direct(value, 10), dump_count=len(expected))
        translated_out = translated("abimyu32toa0", hex(value), "10")
        assert got == expected
        if translated_out is not None:
            assert field(translated_out, "cx") == field(out, "cx")
            assert field(translated_out, "si") == field(out, "si")
            assert translated_out.endswith("data=" + expected.hex())
        assert field(out, "cx") == f"{len(expected):04x}"
        assert field(out, "si") == f"{DST_OFF + len(expected):04x}"


def test_original_and_translated_seg001_strlen() -> None:
    for text in [b"", b"Inertia", b"HACKER4.S3M"]:
        out, _ = original_seg001_call(original_offset("mystrlen"), setup_dseg_strlen(text))
        translated_out = translated("seg1strlen", text.decode("ascii"))
        assert field(out, "ax") == f"{len(text):04x}"
        assert field(out, "si") == f"{DSEG_SCRATCH:04x}"
        if translated_out is not None:
            assert field(translated_out, "ax") == f"{len(text):04x}"


def test_original_and_abi_strlen_public_symbols() -> None:
    for text in [b"", b"Inertia", b"HACKER4.S3M"]:
        out, _ = original_call(original_offset("mystrlen_0"), setup_strlen(), src=text + b"\0")
        got = translated("abistrlen", "mystrlen_0", text.decode("ascii"))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "si") == field(out, "si")
        assert field(out, "ax") == f"{len(text):04x}"
        assert field(out, "si") == f"{SRC_OFF:04x}"

        out, _ = original_seg001_call(original_offset("mystrlen"), setup_dseg_strlen(text))
        got = translated("abistrlen", "mystrlen", text.decode("ascii"))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "si") == field(out, "si")
        assert field(out, "ax") == f"{len(text):04x}"
        assert field(out, "si") == f"{DSEG_SCRATCH:04x}"


def test_original_and_translated_seg001_u32toa_fill_helpers() -> None:
    cases = [
        ("my_u32toa_fill", 42, 5, b"   42"),
        ("my_u32toa_fill", 123456, 4, b"3456"),
        ("my_pnt_u32toa_fill", 42, 5, b"\x02\x7f   42"),
        ("my_pnt_u32toa_fill", 123456, 4, b"\x02\x7f3456"),
    ]
    for symbol, value, count, expected in cases:
        out, got = original_seg001_call(original_offset(symbol), setup_dseg_u32toa_fill(value, count), dump_count=len(expected))
        assert got == expected
        translated_out = translated("seg1fill", symbol, hex(value), str(count))
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())
        assert field(out, "di") == f"{DSEG_SCRATCH + (2 if symbol == 'my_pnt_u32toa_fill' else 0) + count:04x}"


def test_original_and_abi_seg001_u32toa_fill_public_symbols() -> None:
    cases = [
        ("my_u32toa_fill", 42, 5, b"   42"),
        ("my_u32toa_fill", 123456, 4, b"3456"),
        ("my_pnt_u32toa_fill", 42, 5, b"\x02\x7f   42"),
        ("my_pnt_u32toa_fill", 123456, 4, b"\x02\x7f3456"),
    ]
    for symbol, value, count, expected in cases:
        out, data = original_seg001_call(original_offset(symbol), setup_dseg_u32toa_fill(value, count), dump_count=len(expected))
        got = translated("abifill", symbol, hex(value), str(count))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "di") == field(out, "di")
        assert data == expected


def test_original_interpolation_entry_patches_self_modified_bytes() -> None:
    patch_value = 0x5A
    patched_offsets = [
        0x58B4, 0x58E3, 0x5912, 0x5941, 0x5970, 0x599F, 0x59CE, 0x59FD,
        0x5A2C, 0x5A5B, 0x5A8A, 0x5AB9, 0x5AE8, 0x5B17, 0x5B46, 0x5B81,
        0x5BAD, 0x5BDA, 0x5C07, 0x5C34, 0x5C61, 0x5C8E, 0x5CBB, 0x5CE8,
        0x5D15, 0x5D42, 0x5D6F, 0x5D9C, 0x5DC9, 0x5DF6, 0x5E23,
    ]
    dump_base = min(patched_offsets)
    dump_end = max(patched_offsets) + 1
    wrapper = make_stack_jmp_wrapper(
        original_label_offset("loc_157F2"),
        setup_interpolation_patch_probe(patch_value),
        CHANNEL_OFF,
    )
    _, code = original_run(wrapper, dump_count=dump_end - dump_base, dump_offset=dump_base, dump_seg=LOAD_SEG)
    for offset in patched_offsets:
        assert code[offset - dump_base] == patch_value
    translated_out = translated("interppatch", hex(patch_value))
    if translated_out is not None:
        assert translated_out.endswith("data=" + bytes([patch_value] * len(patched_offsets)).hex())


def test_original_and_translated_get_playsettings_far_api() -> None:
    for value in [0x00, 0x10, 0x1F]:
        out, _ = original_far_call(original_offset("get_playsettings"), setup_get_playsettings(value))
        assert field(out, "ax")[-2:] == f"{value:02x}"
        translated_out = translated("getplaysettings", hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax")[-2:] == f"{value:02x}"


def test_original_and_abi_get_playsettings_public_symbol_far_api() -> None:
    for value in [0x00, 0x10, 0x1F]:
        out, _ = original_far_call(original_offset("get_playsettings"), setup_get_playsettings(value))
        got = translated("abigetplaysettings", hex(value))
        if got is not None:
            assert field(got, "ax")[-2:] == field(out, "ax")[-2:]


def test_original_and_translated_volume_12a66_callback_loop() -> None:
    out, _ = original_far_call(original_offset("volume_12A66"), setup_volume_12a66(1))
    translated_out = translated("volume12a66", "1")
    if translated_out is not None:
        assert field(translated_out, "ax") == field(out, "ax")
        assert field(translated_out, "bx") == field(out, "bx")
        assert field(translated_out, "cx") == field(out, "cx")


def test_original_and_abi_volume_12a66_public_symbol_callback_loop() -> None:
    out, _ = original_far_call(original_offset("volume_12A66"), setup_volume_12a66(1))
    got = translated("abivolume12a66", "1")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")


def test_original_and_translated_vlm_141df_sets_volume_update_flag() -> None:
    out, data = original_run(
        make_wrapper(original_offset("vlm_141DF"), setup_vlm_141df()),
        dump_count=1,
        dump_offset=0x00D1,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("vlm141df")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()
    assert data == b"\x01"


def test_original_and_abi_vlm_141df_public_symbol_sets_volume_update_flag() -> None:
    out, data = original_run(
        make_wrapper(original_offset("vlm_141DF"), setup_vlm_141df()),
        dump_count=1,
        dump_offset=0x00D1,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abivlm141df")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()
    assert data == b"\x01"


def test_original_and_translated_change_volume_far_entry() -> None:
    cases = [
        (0x0040, 1, 0x11),
        (0x0100, 1, 0x7F),
        (0xFFFF, 1, 0x22),
    ]
    for value, channels, channel_volume in cases:
        wrapper = make_far_wrapper(original_offset("change_volume"), setup_change_volume(value, channels, channel_volume))
        out, globals_ = original_run(wrapper, dump_count=0x1368 + 0x10, dump_offset=0x0000, dump_seg=DATA_SEG)
        expected = globals_[0x005C:0x005E] + globals_[REAL_CHANNELS_OFF + 0x08 : REAL_CHANNELS_OFF + 0x09]
        translated_out = translated("changevolume", hex(value), hex(channels), hex(channel_volume))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_change_volume_public_symbol_far_entry() -> None:
    cases = [
        (0x0040, 1, 0x11),
        (0x0100, 1, 0x7F),
        (0xFFFF, 1, 0x22),
    ]
    for value, channels, channel_volume in cases:
        wrapper = make_far_wrapper(original_offset("change_volume"), setup_change_volume(value, channels, channel_volume))
        out, globals_ = original_run(wrapper, dump_count=0x1368 + 0x10, dump_offset=0x0000, dump_seg=DATA_SEG)
        expected = globals_[0x005C:0x005E] + globals_[REAL_CHANNELS_OFF + 0x08 : REAL_CHANNELS_OFF + 0x09]
        got = translated("abichangevolume", hex(value), hex(channels), hex(channel_volume))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_memclean_zeroes_exact_size() -> None:
    cases = [
        (0, 4),
        (1, 4),
        (7, 10),
        (8, 10),
    ]
    for size, fill_count in cases:
        wrapper = make_wrapper(original_offset("memclean"), setup_memclean(size, fill_count))
        _, data = original_run(wrapper, dump_count=fill_count, dump_offset=DSEG_SCRATCH, dump_seg=DATA_SEG)
        translated_out = translated("memclean", hex(size), hex(fill_count))
        if translated_out is not None:
            assert translated_out.endswith("data=" + data.hex())


def test_original_and_abi_memclean_public_symbol_zeroes_exact_size() -> None:
    cases = [
        (0, 4),
        (1, 4),
        (7, 10),
        (8, 10),
    ]
    for size, fill_count in cases:
        wrapper = make_wrapper(original_offset("memclean"), setup_memclean(size, fill_count))
        _, data = original_run(wrapper, dump_count=fill_count, dump_offset=DSEG_SCRATCH, dump_seg=DATA_SEG)
        got = translated("abimemclean", hex(size), hex(fill_count))
        if got is not None:
            assert field(got, "data") == data.hex()


def test_original_and_translated_set_playsettings_far_api() -> None:
    cases = [
        (0x00, 0xFF, 22050, 1, 0),
        (0x10, 0xFE, 22050, 1, 0),
        (0x08, 0xFF, 11025, 1, 1),
    ]
    for value, initial_config_hi, freq, channels, shift in cases:
        wrapper = make_far_wrapper(original_offset("set_playsettings"), setup_set_playsettings(value, initial_config_hi, freq, channels, shift))
        out, globals_ = original_run(wrapper, dump_count=0x1368 + 0x40, dump_offset=0x0000, dump_seg=DATA_SEG)
        expected = (
            globals_[0x00D2:0x00D4]
            + globals_[0x001C:0x0024]
            + globals_[REAL_CHANNELS_OFF + 0x3E : REAL_CHANNELS_OFF + 0x40]
        )
        translated_out = translated("setplaysettings", hex(value), hex(initial_config_hi), hex(freq), hex(channels), hex(shift))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_set_playsettings_public_symbol_far_api() -> None:
    cases = [
        (0x00, 0xFF, 22050, 1, 0),
        (0x10, 0xFE, 22050, 1, 0),
        (0x08, 0xFF, 11025, 1, 1),
    ]
    for value, initial_config_hi, freq, channels, shift in cases:
        wrapper = make_far_wrapper(original_offset("set_playsettings"), setup_set_playsettings(value, initial_config_hi, freq, channels, shift))
        out, globals_ = original_run(wrapper, dump_count=0x1368 + 0x40, dump_offset=0x0000, dump_seg=DATA_SEG)
        expected = (
            globals_[0x00D2:0x00D4]
            + globals_[0x001C:0x0024]
            + globals_[REAL_CHANNELS_OFF + 0x3E : REAL_CHANNELS_OFF + 0x40]
        )
        got = translated("abisetplaysettings", hex(value), hex(initial_config_hi), hex(freq), hex(channels), hex(shift))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert got.endswith("data=" + expected.hex())


def test_original_and_translated_sub_12afd_effect_dispatch_far_entry() -> None:
    cases = [
        (0x00A4, 1, 0, 0x01),
        (0x00A5, 1, 0, 0x81),
        (0x00A4, 1, 1, 0x22),
    ]
    for value, channels, channel_index, flags in cases:
        wrapper = make_far_wrapper(original_offset("sub_12AFD"), setup_sub_12afd(value, channels, channel_index, flags))
        _, data = original_run(wrapper, dump_count=1, dump_offset=REAL_CHANNELS_OFF + 0x17, dump_seg=DATA_SEG)
        translated_out = translated("sub12afd", hex(value), hex(channels), hex(channel_index), hex(flags))
        if translated_out is not None:
            assert translated_out.endswith("data=" + data.hex())


def test_original_and_abi_sub_12afd_public_symbol_effect_dispatch_far_entry() -> None:
    cases = [
        (0x00A4, 1, 0, 0x01),
        (0x00A5, 1, 0, 0x81),
        (0x00A4, 1, 1, 0x22),
    ]
    for value, channels, channel_index, flags in cases:
        wrapper = make_far_wrapper(original_offset("sub_12AFD"), setup_sub_12afd(value, channels, channel_index, flags))
        _, data = original_run(wrapper, dump_count=1, dump_offset=REAL_CHANNELS_OFF + 0x17, dump_seg=DATA_SEG)
        got = translated("abisub12afd", hex(value), hex(channels), hex(channel_index), hex(flags))
        if got is not None:
            assert got.endswith("data=" + data.hex())


def test_original_and_translated_sub_12b18_channel_map_far_entry() -> None:
    cases = [
        (4, bytes([0x00, 0x3F, 0x40, 0x80] + [0xCC] * 28)),
        (3, bytes([0x7F, 0x01, 0x40] + [0x55] * 29)),
    ]
    for channels, src in cases:
        wrapper = make_far_wrapper(original_offset("sub_12B18"), setup_sub_12b18(channels, sndflags=0))
        _, globals_ = original_run(wrapper, src=src, dump_count=0x1368 + 0x150, dump_offset=0x0000, dump_seg=DATA_SEG)
        expected = (
            globals_[0x007C:0x007E]
            + b"".join(
                globals_[REAL_CHANNELS_OFF + i * 0x50 + 0x18 : REAL_CHANNELS_OFF + i * 0x50 + 0x19]
                + globals_[REAL_CHANNELS_OFF + i * 0x50 + 0x3A : REAL_CHANNELS_OFF + i * 0x50 + 0x3B]
                for i in range(channels)
            )
        )
        translated_out = translated("sub12b18", hex(channels), src[:channels].hex())
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_sub_12b18_public_symbol_channel_map_far_entry() -> None:
    cases = [
        (4, bytes([0x00, 0x3F, 0x40, 0x80] + [0xCC] * 28)),
        (3, bytes([0x7F, 0x01, 0x40] + [0x55] * 29)),
    ]
    for channels, src in cases:
        wrapper = make_far_wrapper(original_offset("sub_12B18"), setup_sub_12b18(channels, sndflags=0))
        _, globals_ = original_run(wrapper, src=src, dump_count=0x1368 + 0x150, dump_offset=0x0000, dump_seg=DATA_SEG)
        expected = (
            globals_[0x007C:0x007E]
            + b"".join(
                globals_[REAL_CHANNELS_OFF + i * 0x50 + 0x18 : REAL_CHANNELS_OFF + i * 0x50 + 0x19]
                + globals_[REAL_CHANNELS_OFF + i * 0x50 + 0x3A : REAL_CHANNELS_OFF + i * 0x50 + 0x3B]
                for i in range(channels)
            )
        )
        got = translated("abisub12b18", hex(channels), src[:channels].hex())
        if got is not None:
            assert got.endswith("data=" + expected.hex())


def test_original_and_translated_sub_12b83_channel_type_setup_far_entry() -> None:
    cases = [
        (1, bytes([0, 1, 2, 3]), 0),
        (4, bytes([0, 1, 2, 3]), 0),
        (0x30, bytes([0, 1, 2, 0] + [0] * 28), 1),
    ]
    for value, channel_types, sound_mode in cases:
        wrapper = make_far_wrapper(original_offset("sub_12B83"), setup_sub_12b83(value, channel_types, sound_mode))
        channel_count = max(2, min(value & 0xFF, 0x20))
        _, globals_ = original_run(wrapper, dump_count=0x1368 + channel_count * 0x50, dump_offset=0x0000, dump_seg=DATA_SEG)
        expected = (
            globals_[0x0034:0x003C]
            + globals_[0x007C:0x007E]
            + globals_[0x001C:0x0024]
            + globals_[0x00DD:0x00DF]
            + b"".join(
                globals_[REAL_CHANNELS_OFF + i * 0x50 + 0x18 : REAL_CHANNELS_OFF + i * 0x50 + 0x19]
                + globals_[REAL_CHANNELS_OFF + i * 0x50 + 0x3E : REAL_CHANNELS_OFF + i * 0x50 + 0x40]
                for i in range(channel_count)
            )
        )
        translated_out = translated("sub12b83", hex(value), channel_types[:channel_count].hex(), hex(sound_mode))
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_sub_12b83_public_symbol_channel_type_setup_far_entry() -> None:
    cases = [
        (1, bytes([0, 1, 2, 3]), 0),
        (4, bytes([0, 1, 2, 3]), 0),
        (0x30, bytes([0, 1, 2, 0] + [0] * 28), 1),
    ]
    for value, channel_types, sound_mode in cases:
        wrapper = make_far_wrapper(original_offset("sub_12B83"), setup_sub_12b83(value, channel_types, sound_mode))
        channel_count = max(2, min(value & 0xFF, 0x20))
        _, globals_ = original_run(wrapper, dump_count=0x1368 + channel_count * 0x50, dump_offset=0x0000, dump_seg=DATA_SEG)
        expected = (
            globals_[0x0034:0x003C]
            + globals_[0x007C:0x007E]
            + globals_[0x001C:0x0024]
            + globals_[0x00DD:0x00DF]
            + b"".join(
                globals_[REAL_CHANNELS_OFF + i * 0x50 + 0x18 : REAL_CHANNELS_OFF + i * 0x50 + 0x19]
                + globals_[REAL_CHANNELS_OFF + i * 0x50 + 0x3E : REAL_CHANNELS_OFF + i * 0x50 + 0x40]
                for i in range(channel_count)
            )
        )
        got = translated("abisub12b83", hex(value), channel_types[:channel_count].hex(), hex(sound_mode))
        if got is not None:
            assert got.endswith("data=" + expected.hex())


def test_original_and_translated_someplaymode_timing_helper() -> None:
    cases = [
        (0x00, 22050, 1, 0, 0x00),
        (0x08, 11025, 1, 1, 0x00),
        (0x08, 22050, 1, 0, 0x04),
    ]
    for playsettings, freq, channels, shift, sndflags in cases:
        wrapper = make_wrapper(original_offset("someplaymode"), setup_someplaymode(playsettings, freq, channels, shift, sndflags))
        _, globals_ = original_run(wrapper, dump_count=0x1368 + 0x40, dump_offset=0x0000, dump_seg=DATA_SEG)
        expected = (
            globals_[0x001C:0x0024]
            + globals_[0x009C:0x00A0]
            + globals_[REAL_CHANNELS_OFF + 0x3E : REAL_CHANNELS_OFF + 0x40]
        )
        translated_out = translated("someplaymode", hex(playsettings), hex(freq), hex(channels), hex(shift), hex(sndflags))
        if translated_out is not None:
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_someplaymode_public_symbol_timing_helper() -> None:
    cases = [
        (0x00, 22050, 1, 0, 0x00),
        (0x08, 11025, 1, 1, 0x00),
        (0x08, 22050, 1, 0, 0x04),
    ]
    for playsettings, freq, channels, shift, sndflags in cases:
        wrapper = make_wrapper(original_offset("someplaymode"), setup_someplaymode(playsettings, freq, channels, shift, sndflags))
        _, globals_ = original_run(wrapper, dump_count=0x1368 + 0x40, dump_offset=0x0000, dump_seg=DATA_SEG)
        expected = (
            globals_[0x001C:0x0024]
            + globals_[0x009C:0x00A0]
            + globals_[REAL_CHANNELS_OFF + 0x3E : REAL_CHANNELS_OFF + 0x40]
        )
        got = translated("abisomeplaymode", hex(playsettings), hex(freq), hex(channels), hex(shift), hex(sndflags))
        if got is not None:
            assert got.endswith("data=" + expected.hex())


def test_original_and_translated_read_sndsettings_far_api() -> None:
    cases = [
        (3, 0x0220, 5, 1, 44, 0x12, 0x34, 44100, 48000, 0x1357, 0x00, 44100),
        (5, 0x0388, 7, 3, 22, 0x56, 0x78, 22050, 33075, 0x2468, 0x04, 33075),
    ]
    for sndcard_type, base_port, irq, dma, freq_code, byte_246d8, byte_246d9, output_freq, freq2, config_word, sndflags, expected_bp in cases:
        setup = setup_read_sndsettings(
            sndcard_type,
            base_port,
            irq,
            dma,
            freq_code,
            byte_246d8,
            byte_246d9,
            output_freq,
            freq2,
            config_word,
            sndflags,
        )
        out, _ = original_far_call(original_offset("read_sndsettings"), setup)
        translated_out = translated(
            "readsndsettings",
            hex(sndcard_type),
            hex(base_port),
            hex(irq),
            hex(dma),
            hex(freq_code),
            hex(byte_246d8),
            hex(byte_246d9),
            hex(output_freq),
            hex(freq2),
            hex(config_word),
            hex(sndflags),
        )
        if translated_out is not None:
            for reg in ["ax", "bx", "cx", "dx", "bp", "si"]:
                assert field(translated_out, reg) == field(out, reg)
        assert field(out, "ax") == f"{((freq_code & 0xFF) << 8) | (sndcard_type & 0xFF):04x}"
        assert field(out, "bx") == f"{((byte_246d9 & 0xFF) << 8) | (byte_246d8 & 0xFF):04x}"
        assert field(out, "cx") == f"{((dma & 0xFF) << 8) | (irq & 0xFF):04x}"
        assert field(out, "dx") == f"{base_port & 0xFFFF:04x}"
        assert field(out, "bp") == f"{expected_bp & 0xFFFF:04x}"
        assert field(out, "si") == f"{config_word & 0xFFFF:04x}"


def test_original_and_abi_read_sndsettings_public_symbol_far_api() -> None:
    cases = [
        (3, 0x0220, 5, 1, 44, 0x12, 0x34, 44100, 48000, 0x1357, 0x00),
        (5, 0x0388, 7, 3, 22, 0x56, 0x78, 22050, 33075, 0x2468, 0x04),
    ]
    for sndcard_type, base_port, irq, dma, freq_code, byte_246d8, byte_246d9, output_freq, freq2, config_word, sndflags in cases:
        setup = setup_read_sndsettings(
            sndcard_type,
            base_port,
            irq,
            dma,
            freq_code,
            byte_246d8,
            byte_246d9,
            output_freq,
            freq2,
            config_word,
            sndflags,
        )
        out, _ = original_far_call(original_offset("read_sndsettings"), setup)
        got = translated(
            "abireadsndsettings",
            hex(sndcard_type),
            hex(base_port),
            hex(irq),
            hex(dma),
            hex(freq_code),
            hex(byte_246d8),
            hex(byte_246d9),
            hex(output_freq),
            hex(freq2),
            hex(config_word),
            hex(sndflags),
        )
        if got is not None:
            for reg in ["ax", "bx", "cx", "dx", "bp", "si"]:
                assert field(got, reg) == field(out, reg)


def test_original_and_translated_sub_12d05_default_device_message() -> None:
    expected = b"Device not initialised!"
    wrapper = make_far_wrapper(original_offset("sub_12D05"), setup_sub_12d05(snd_init=0))
    out, got = original_run(wrapper, dump_count=len(expected), dump_offset=DSEG_SCRATCH, dump_seg=DATA_SEG)
    assert got == expected
    translated_out = translated("sub12d05", "0", "0")
    if translated_out is not None:
        assert field(translated_out, "cx") == field(out, "cx")
        assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_sub_12d05_public_symbol_default_device_message() -> None:
    expected = b"Device not initialised!"
    wrapper = make_far_wrapper(original_offset("sub_12D05"), setup_sub_12d05(snd_init=0))
    out, got = original_run(wrapper, dump_count=len(expected), dump_offset=DSEG_SCRATCH, dump_seg=DATA_SEG)
    assert got == expected
    abi_out = translated("abisub12d05", "0", "0")
    if abi_out is not None:
        assert field(abi_out, "cx") == field(out, "cx")
        assert abi_out.endswith("data=" + expected.hex())


def test_original_and_translated_getset_playstate_far_api() -> None:
    cases = [
        (0x02, 0xFF, 0x02),
        (0x00, 0x01, 0x01),
        (0x03, 0x00, 0x00),
    ]
    for initial, request, expected in cases:
        out, _ = original_far_call(original_offset("getset_playstate"), setup_getset_playstate(initial, request))
        assert field(out, "ax")[-2:] == f"{expected:02x}"
        translated_out = translated("getsetplaystate", hex(initial), hex(request))
        if translated_out is not None:
            assert field(translated_out, "ax")[-2:] == f"{expected:02x}"


def test_original_and_abi_getset_playstate_public_symbol_far_api() -> None:
    cases = [
        (0x02, 0xFF, 0x02),
        (0x00, 0x01, 0x01),
        (0x03, 0x00, 0x00),
    ]
    for initial, request, expected in cases:
        out, _ = original_far_call(original_offset("getset_playstate"), setup_getset_playstate(initial, request))
        got = translated("abigetsetplaystate", hex(initial), hex(request))
        if got is not None:
            assert field(got, "ax")[-2:] == field(out, "ax")[-2:] == f"{expected:02x}"


def test_original_and_translated_get_12f7c_far_api() -> None:
    out, _ = original_far_call(original_offset("get_12F7C"), setup_get_12f7c(0x1234, 0xABCD))
    assert field(out, "ax") == "1234"
    assert field(out, "bx") == "abcd"
    translated_out = translated("get12f7c", "0x1234", "0xabcd")
    if translated_out is not None:
        assert field(translated_out, "ax") == "1234"
        assert field(translated_out, "bx") == "abcd"


def test_original_and_abi_get_12f7c_public_symbol_far_api() -> None:
    out, _ = original_far_call(original_offset("get_12F7C"), setup_get_12f7c(0x1234, 0xABCD))
    got = translated("abiget12f7c", "0x1234", "0xabcd")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")


def test_original_and_translated_snd_off_helpers_noop_when_uninitialized() -> None:
    cases = [
        ("snd_initialze", "sndinit", 1, 0, 3),
        ("snd_on", "sndon", 0, 0, 3),
        ("snd_off", "sndoff", 0, 1, 3),
        ("snd_deinit", "snddeinit", 0, 1, 3),
        ("snd_offx", "sndoffx", 0, 1, 3),
    ]
    for symbol, command, snd_init, snd_set_flag, sndcard_type in cases:
        setup = setup_snd_off(snd_init, snd_set_flag, sndcard_type)
        wrapper = make_far_wrapper(original_offset(symbol), setup) if symbol == "snd_offx" else make_wrapper(original_offset(symbol), setup)
        out, globals_ = original_run(wrapper, dump_count=3, dump_offset=0x00E0, dump_seg=DATA_SEG)
        translated_out = translated(command, hex(snd_init), hex(snd_set_flag), hex(sndcard_type))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + globals_.hex())


def test_translated_audio_sink_accepts_sb16_16bit_stereo_frames() -> None:
    got = translated("audiosb16sink")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "framebytes") == "4"
        assert field(got, "frames") == "2"
        assert field(got, "bytes") == "8"
        assert field(got, "data") == "0102030405060708"


def test_translated_audio_sink_lifecycle_gates_writes() -> None:
    got = translated("audiolifecycle")
    if got is not None:
        assert field(got, "inactive_frames") == "0"
        assert field(got, "inactive_underrun") == "0"
        assert field(got, "inactive_bytes") == "0"
        assert field(got, "active_frames") == "2"
        assert field(got, "active_underrun") == "1"
        assert field(got, "active_bytes") == "4"
        assert field(got, "stopped_frames") == "2"
        assert field(got, "stopped_underrun") == "1"
        assert field(got, "stopped_bytes") == "4"
        assert field(got, "active") == "0"


def test_translated_audio_sink_reset_clears_counters_without_stopping() -> None:
    got = translated("audioreset")
    if got is not None:
        assert field(got, "before_frames") == "3"
        assert field(got, "before_underrun") == "2"
        assert field(got, "before_dropped") == "0"
        assert field(got, "after_frames") == "0"
        assert field(got, "after_underrun") == "0"
        assert field(got, "after_dropped") == "0"
        assert field(got, "active") == "1"


def test_translated_audio_sink_capacity_limits_written_frames() -> None:
    got = translated("audiocapacity")
    if got is not None:
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "bytes") == "8"
        assert field(got, "data") == "0102030405060708"
        assert field(got, "after_frames") == "2"
        assert field(got, "after_dropped") == "2"
        assert field(got, "after_capacity") == "0"
        assert field(got, "refill_frames") == "3"
        assert field(got, "refill_dropped") == "2"
        assert field(got, "refill_capacity") == "0"
        assert field(got, "refill_bytes") == "4"
        assert field(got, "refill_data") == "090a0b0c"


def test_translated_audio_sink_writes_sb16_signed_silence_frames() -> None:
    got = translated("audiosb16silence")
    if got is not None:
        assert field(got, "frames") == "3"
        assert field(got, "underrun") == "3"
        assert field(got, "bytes") == "12"
        assert field(got, "data") == "000000000000000000000000"


def test_translated_audio_converts_unsigned_8bit_to_signed_16bit_stereo() -> None:
    got = translated("audiou8tos16stereo")
    if got is not None:
        assert field(got, "mono_bytes") == "12"
        assert field(got, "mono") == "0080008000000000ff7fff7f"
        assert field(got, "stereo_bytes") == "8"
        assert field(got, "stereo") == "0080ff7f000000c0"


def test_translated_audio_converts_signed_16bit_to_signed_16bit_stereo() -> None:
    got = translated("audios16tos16stereo")
    if got is not None:
        assert field(got, "mono_bytes") == "8"
        assert field(got, "mono") == "3412341200800080"
        assert field(got, "stereo_bytes") == "8"
        assert field(got, "stereo") == "341278560080ff7f"


def test_translated_audio_unified_converter_targets_sb16_sink_format() -> None:
    got = translated("audioconvert")
    if got is not None:
        assert field(got, "u8_bytes") == "8"
        assert field(got, "u8") == "00800080ff7fff7f"
        assert field(got, "s16_bytes") == "4"
        assert field(got, "s16") == "34120080"
        assert field(got, "bad_bytes") == "0"
        assert field(got, "rate_bytes") == "0"
        assert field(got, "rates_match") == "1"
        assert field(got, "sink_equals") == "1"


def test_translated_audio_source_format_maps_supported_mixer_modes() -> None:
    for rate, bits, channels, signed_samples, framebytes in [
        (11025, 8, 1, 0, "1"),
        (22050, 8, 2, 0, "2"),
        (44100, 16, 1, 1, "2"),
        (48000, 16, 2, 1, "4"),
    ]:
        got = translated("audiosourcefmt", str(rate), str(bits), str(channels), str(signed_samples))
        if got is not None:
            assert field(got, "ok") == "1"
            assert field(got, "rate") == str(rate)
            assert field(got, "bits") == str(bits)
            assert field(got, "channels") == str(channels)
            assert field(got, "signed") == str(signed_samples)
            assert field(got, "framebytes") == framebytes
            assert field(got, "frames10") == str(10 // int(framebytes))
    got = translated("audiosourcefmt", "44100", "8", "2", "1")
    if got is not None:
        assert field(got, "ok") == "0"


def test_translated_audio_format_names_are_stable() -> None:
    for bits, channels, signed_samples, name in [
        (8, 1, 0, "u8-mono"),
        (8, 2, 0, "u8-stereo"),
        (16, 1, 1, "s16-mono"),
        (16, 2, 1, "s16-stereo"),
        (8, 2, 1, "unsupported"),
    ]:
        got = translated("audiofmtname", "44100", str(bits), str(channels), str(signed_samples))
        if got is not None:
            assert field(got, "name") == name


def test_translated_audio_sink_writes_converted_pcm() -> None:
    got = translated("audiowriteconverted")
    if got is not None:
        assert field(got, "bytes") == "8"
        assert field(got, "paused_bytes") == "0"
        assert field(got, "paused") == "0"
        assert field(got, "frames") == "2"
        assert field(got, "captured") == "8"
        assert field(got, "data") == "0080ff7f000000c0"


def test_translated_audio_output_routes_mixer_frames_to_sb16_stereo_sink() -> None:
    got = translated("audiooutputmixer")
    if got is not None:
        assert field(got, "bytes") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "2"
        assert field(got, "captured") == "8"
        assert field(got, "data") == "0080ff7f000000c0"
        assert field(got, "scratch") == "0080ff7f000000c0ff7f0080a5a5a5a5"


def test_translated_audio_output_direct_sb16_stereo_path_needs_no_scratch() -> None:
    got = translated("audiosb16output")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "bytes") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "1"
        assert field(got, "captured") == "8"
        assert field(got, "data") == "0102030405060708"


def test_translated_audio_output_refill_and_underrun_counters_are_backend_level() -> None:
    got = translated("audiooutputrefill")
    if got is not None:
        assert field(got, "first_frames") == "1"
        assert field(got, "first_dropped") == "1"
        assert field(got, "first_capacity") == "0"
        assert field(got, "first_bytes") == "4"
        assert field(got, "first_data") == "01020304"
        assert field(got, "refill_frames") == "2"
        assert field(got, "refill_underrun") == "1"
        assert field(got, "refill_dropped") == "1"
        assert field(got, "refill_capacity") == "1"
        assert field(got, "refill_bytes") == "4"
        assert field(got, "refill_data") == "00000000"


def test_translated_audio_levels_scan_sb16_stereo_pcm_for_16_color_meter() -> None:
    got = translated("audiolevels")
    if got is not None:
        assert field(got, "left_peak") == "32768"
        assert field(got, "right_peak") == "32767"
        assert field(got, "left16") == "15"
        assert field(got, "right16") == "15"
        assert field(got, "scale0") == "0"
        assert field(got, "scale1") == "0"
        assert field(got, "scale2048") == "1"
        assert field(got, "scale16384") == "8"
        assert field(got, "scale32768") == "15"


def test_translated_audio_output_updates_levels_from_direct_and_converted_pcm() -> None:
    got = translated("audiooutputlevels")
    if got is not None:
        assert field(got, "direct_left") == "32768"
        assert field(got, "direct_right") == "32767"
        assert field(got, "direct_l16") == "15"
        assert field(got, "direct_r16") == "15"
        assert field(got, "conv_left") == "32768"
        assert field(got, "conv_right") == "32512"
        assert field(got, "conv_l16") == "15"
        assert field(got, "conv_r16") == "15"
        assert field(got, "scratch") == "00800000ff7f00c00000ff7f"
        assert field(got, "reset") == "0,0"


def test_translated_audio_level_draws_16_step_stereo_meter_to_text_plane() -> None:
    got = translated("audioleveldraw")
    if got is not None:
        assert field(got, "left") == ("db2a" * 9) + ("b008" * 7)
        assert field(got, "right") == "db4c" * 16
        assert field(got, "clip") == "231e" * 4
        assert field(got, "after") == "2007"


def test_translated_audio_output_draws_latest_levels_to_text_plane() -> None:
    got = translated("audiooutputdraw")
    if got is not None:
        assert field(got, "left") == "db2a" * 16
        assert field(got, "right") == "db4c" * 16
        assert field(got, "captured") == "8"
        assert field(got, "data") == "00c00020ff7f0080"


def test_original_and_abi_snd_guard_public_symbols_noop_when_uninitialized() -> None:
    cases = [
        ("snd_initialze", 1, 0, 3),
        ("snd_on", 0, 0, 3),
        ("snd_off", 0, 1, 3),
        ("snd_deinit", 0, 1, 3),
        ("snd_offx", 0, 1, 3),
    ]
    for symbol, snd_init, snd_set_flag, sndcard_type in cases:
        setup = setup_snd_off(snd_init, snd_set_flag, sndcard_type)
        wrapper = make_far_wrapper(original_offset(symbol), setup) if symbol == "snd_offx" else make_wrapper(original_offset(symbol), setup)
        out, globals_ = original_run(wrapper, dump_count=3, dump_offset=0x00E0, dump_seg=DATA_SEG)
        got = translated("abisndguard", symbol, hex(snd_init), hex(snd_set_flag), hex(sndcard_type))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == globals_.hex()


def test_original_and_translated_sub_131da_channel_stop_helper() -> None:
    wrapper = make_wrapper(original_offset("sub_131DA"), setup_sub_131da(channel_type=0, flags=0x13, note_byte=0x77))
    _, channel = original_run(wrapper, dump_count=0x40, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
    assert channel[0x17] == 0x12
    assert channel[0x35] == 0x00
    translated_out = translated("sub131da", "0", "0x13", "0x77")
    if translated_out is not None:
        assert translated_out.endswith("data=1200")

    wrapper = make_wrapper(original_offset("sub_131DA"), setup_sub_131da(channel_type=1, flags=0x13, note_byte=0x77))
    _, channel = original_run(wrapper, dump_count=0x40, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
    assert channel[0x17] == 0x13
    assert channel[0x35] == 0x77


def test_original_and_abi_sub_131da_public_symbol_channel_stop_helper() -> None:
    cases = [
        (0, 0x13, 0x77),
        (1, 0x13, 0x77),
        (0, 0x12, 0x77),
    ]
    for channel_type, flags, note_byte in cases:
        wrapper = make_wrapper(original_offset("sub_131DA"), setup_sub_131da(channel_type=channel_type, flags=flags, note_byte=note_byte))
        _, channel = original_run(wrapper, dump_count=0x40, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x17:0x18] + channel[0x35:0x36]
        got = translated("abisub131da", hex(channel_type), hex(flags), hex(note_byte))
        if got is not None:
            assert field(got, "data") == expected.hex()


def test_original_and_translated_sub_131ef_volume_helper() -> None:
    wrapper = make_wrapper(
        original_offset("sub_131EF"),
        setup_sub_131ef(value=0x20, volume=0x0100, max_volume=0x40, old_fine=0x12, flags_3d=0xFF),
    )
    _, channel = original_run(wrapper, dump_count=0x40, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
    assert channel[0x22] == 0x20
    assert channel[0x23] == 0x20
    assert channel[0x36:0x38] == b"\x12\x20"
    assert channel[0x3D] == 0xBF
    translated_out = translated("sub131ef", "0x20", "0x0100", "0x40", "0x12", "0xff")
    if translated_out is not None:
        assert translated_out.endswith("data=20201220bf")


def test_original_and_abi_sub_131ef_public_symbol_volume_helper() -> None:
    wrapper = make_wrapper(
        original_offset("sub_131EF"),
        setup_sub_131ef(value=0x20, volume=0x0100, max_volume=0x40, old_fine=0x12, flags_3d=0xFF),
    )
    _, channel = original_run(wrapper, dump_count=0x40, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
    expected = channel[0x22:0x24] + channel[0x36:0x38] + channel[0x3D:0x3E]
    got = translated("abisub131ef", "0x20", "0x0100", "0x40", "0x12", "0xff")
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_original_and_translated_sub_13177_period_helper() -> None:
    wrapper = make_wrapper(
        original_offset("sub_13177"),
        setup_sub_13177(period=1000, dword_245bc=500000, dword_245c0=1000000, shift=0, flags_3d=0x10),
    )
    _, channel = original_run(wrapper, dump_count=0x42, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
    assert channel[0x1E:0x20] == struct.pack("<H", 1000)
    assert channel[0x20:0x22] == struct.pack("<H", 500)
    assert channel[0x3D] == 0x14
    assert channel[0x3E:0x40] == struct.pack("<H", 1000)
    translated_out = translated("sub13177", "1000", "500000", "1000000", "0", "0x10")
    if translated_out is not None:
        assert translated_out.endswith("data=e803f40114e803")


def test_original_and_abi_sub_13177_public_symbol_period_helper() -> None:
    wrapper = make_wrapper(
        original_offset("sub_13177"),
        setup_sub_13177(period=1000, dword_245bc=500000, dword_245c0=1000000, shift=0, flags_3d=0x10),
    )
    _, channel = original_run(wrapper, dump_count=0x42, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
    expected = channel[0x1E:0x22] + channel[0x3D:0x40]
    got = translated("abisub13177", "1000", "500000", "1000000", "0", "0x10")
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_original_and_translated_midi_154da_reads_channel_byte_18_to_ah() -> None:
    for value in [0x00, 0x34, 0xFF]:
        out, _ = original_call(original_offset("midi_154DA"), setup_midi_channel(byte_18=value))
        assert field(out, "ax")[:2] == f"{value:02x}"
        translated_out = translated("midi154da", hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax")[:2] == f"{value:02x}"


def test_original_and_abi_midi_154da_public_symbol_reads_channel_byte_18_to_ah() -> None:
    for value in [0x00, 0x34, 0xFF]:
        out, _ = original_call(original_offset("midi_154DA"), setup_midi_channel(byte_18=value))
        got = translated("abimidi154da", hex(value))
        if got is not None:
            assert field(got, "ax")[:2] == field(out, "ax")[:2]


def test_original_and_translated_midi_154de_converts_packed_note_to_ah() -> None:
    cases = [
        (0x11, 0x0C0C),
        (0x31, 0x2424),
        (0x4C, 0x3B3B),
    ]
    for packed, expected_ax in cases:
        out, _ = original_call(original_offset("midi_154DE"), setup_midi_channel(byte_35=packed))
        assert field(out, "ax") == f"{expected_ax:04x}"
        translated_out = translated("midi154de", hex(packed))
        if translated_out is not None:
            assert field(translated_out, "ax") == f"{expected_ax:04x}"


def test_original_and_abi_midi_154de_public_symbol_converts_packed_note_to_ah() -> None:
    cases = [
        (0x11, 0x0C0C),
        (0x31, 0x2424),
        (0x4C, 0x3B3B),
    ]
    for packed, expected_ax in cases:
        out, _ = original_call(original_offset("midi_154DE"), setup_midi_channel(byte_35=packed))
        got = translated("abimidi154de", hex(packed))
        if got is not None:
            assert field(got, "ax") == field(out, "ax") == f"{expected_ax:04x}"
            assert field(got, "dx")[-2:] == field(out, "dx")[-2:]


def test_original_and_translated_midi_154ac_noop_when_volume_unchanged() -> None:
    cases = [
        (0x20, 0x40, 0x20),
        (0x50, 0x40, 0x40),
    ]
    for value, max_volume, current_volume in cases:
        out, channel = original_call(original_offset("midi_154AC"), setup_midi_154ac(value, max_volume, current_volume), dump_count=0x1C)
        expected = channel[0x1B:0x1C]
        translated_out = translated("midi154ac", hex(value), hex(max_volume), hex(current_volume))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert field(translated_out, "di") == field(out, "di")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_midi_154ac_public_symbol_noop_when_volume_unchanged() -> None:
    cases = [
        (0x20, 0x40, 0x20),
        (0x50, 0x40, 0x40),
    ]
    for value, max_volume, current_volume in cases:
        out, channel = original_call(original_offset("midi_154AC"), setup_midi_154ac(value, max_volume, current_volume), dump_count=0x1C)
        expected = channel[0x1B:0x1C]
        got = translated("abimidi154ac", hex(value), hex(max_volume), hex(current_volume))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "di") == field(out, "di")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_midi_15413_guarded_duplicate_status_byte() -> None:
    for value in [0x80, 0xC0, 0xFF]:
        wrapper = make_wrapper(original_offset("midi_15413"), setup_midi_15413_guard(value))
        out, data = original_run(wrapper, dump_count=1, dump_offset=0x00D7, dump_seg=DATA_SEG)
        translated_out = translated("midi15413guard", hex(value))
        assert data == bytes([value])
        assert field(out, "ax") == f"{((value & 0xFF) << 8) | 0x34:04x}"
        assert field(out, "dx") == "5678"
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert field(translated_out, "dx") == field(out, "dx")
            assert translated_out.endswith(f"data={value:02x}")


def test_original_and_abi_midi_15413_public_symbol_guarded_duplicate_status_byte() -> None:
    for value in [0x80, 0xC0, 0xFF]:
        wrapper = make_wrapper(original_offset("midi_15413"), setup_midi_15413_guard(value))
        out, data = original_run(wrapper, dump_count=1, dump_offset=0x00D7, dump_seg=DATA_SEG)
        got = translated("abimidi15413guard", hex(value))
        assert data == bytes([value])
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == f"{value:02x}"


def test_original_and_translated_sub_15577_disabled_channel_guard() -> None:
    wrapper = make_wrapper(original_offset("sub_15577"), setup_sub_15577_guard())
    out, data = original_run(wrapper, dump_count=1, dump_offset=CHANNEL_OFF + 0x17, dump_seg=DATA_SEG)
    got = translated("sub15577guard")
    assert data == b"\x00"
    assert field(out, "ax") == "1234"
    assert field(out, "bx") == "5678"
    assert field(out, "cx") == "9abc"
    assert field(out, "dx") == "def0"
    assert field(out, "si") == f"{CHANNEL_OFF:04x}"
    assert field(out, "di") == "2468"
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert got.endswith("data=00")


def test_original_and_abi_sub_15577_public_symbol_disabled_channel_guard() -> None:
    wrapper = make_wrapper(original_offset("sub_15577"), setup_sub_15577_guard())
    out, data = original_run(wrapper, dump_count=1, dump_offset=CHANNEL_OFF + 0x17, dump_seg=DATA_SEG)
    got = translated("abisub15577guard")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()


def test_original_and_rewritten_sub_15577_active_narrow_mix_output_and_state() -> None:
    frames = 4
    period = 0x0100
    position = 0
    src = bytes(index * 2 + 1 for index in range(80))
    wrapper = make_wrapper(
        original_offset("sub_15577"),
        setup_sub_15577_mix(False, False, frames, period, position),
    )
    _, data = original_run(
        wrapper,
        src=src,
        dump_count=0x150,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
    )
    mix, state = sub_15577_mix_state(data, frames)
    got = rewritten(
        "sub15577mix", "0", "0", str(frames), hex(period), hex(position),
        "0xffffff", "0", "0", "0",
    )
    assert field(got, "mix") == mix.hex()
    assert field(got, "state") == state.hex()


def test_original_and_rewritten_sub_15577_active_interpolated_mix_output_and_state() -> None:
    frames = 4
    period = 0x0180
    position = 0x40
    src = bytes(index * 2 + 1 for index in range(80))
    wrapper = make_wrapper(
        original_offset("sub_15577"),
        setup_sub_15577_mix(True, False, frames, period, position),
    )
    _, data = original_run(
        wrapper,
        src=src,
        dump_count=0x150,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
    )
    mix, state = sub_15577_mix_state(data, frames)
    got = rewritten(
        "sub15577mix", "1", "0", str(frames), hex(period), hex(position),
        "0xffffff", "0", "0", "0",
    )
    assert field(got, "mix") == mix.hex()
    assert field(got, "state") == state.hex()


def test_original_and_rewritten_sub_15577_active_wide_mix_output_and_state() -> None:
    frames = 4
    period = 0x0100
    position = 0
    src = bytes(index * 2 + 1 for index in range(80))
    wrapper = make_wrapper(
        original_offset("sub_15577"),
        setup_sub_15577_mix(False, True, frames, period, position),
    )
    _, data = original_run(
        wrapper,
        src=src,
        dump_count=0x150,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
    )
    mix, state = sub_15577_mix_state(data, frames)
    got = rewritten(
        "sub15577mix", "0", "1", str(frames), hex(period), hex(position),
        "0xffffff", "0", "0", "0",
    )
    assert field(got, "mix") == mix.hex()
    assert field(got, "state") == state.hex()


def test_original_and_rewritten_sub_15577_sample_end_stops_voice() -> None:
    frames = 2
    period = 0x0100
    src = bytes(index * 2 + 1 for index in range(80))
    wrapper = make_wrapper(
        original_offset("sub_15577"),
        setup_sub_15577_mix(False, False, frames, period, 0, sample_end=1, loop_start=1),
    )
    _, data = original_run(
        wrapper,
        src=src,
        dump_count=0x150,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
    )
    mix, state = sub_15577_mix_state(data, frames)
    got = rewritten("sub15577mix", "0", "0", str(frames), hex(period), "0", "1", "1", "0", "0")
    assert field(got, "mix") == mix.hex()
    assert field(got, "state") == state.hex()


def test_original_and_rewritten_sub_15577_sample_end_wraps_loop() -> None:
    frames = 4
    period = 0x0100
    src = bytes(index * 2 + 1 for index in range(80))
    wrapper = make_wrapper(
        original_offset("sub_15577"),
        setup_sub_15577_mix(
            False,
            False,
            frames,
            period,
            0,
            sample_end=2,
            loop_start=1,
            loop_length=2,
            looping=True,
        ),
    )
    _, data = original_run(
        wrapper,
        src=src,
        dump_count=0x150,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
    )
    mix, state = sub_15577_mix_state(data, frames)
    got = rewritten("sub15577mix", "0", "0", str(frames), hex(period), "0", "2", "1", "2", "1")
    assert field(got, "mix") == mix.hex()
    assert field(got, "state") == state.hex()


def test_original_and_translated_sub_1609f_disabled_channel_zero_fill() -> None:
    buffer_size = 0x12
    wrapper = make_wrapper(original_offset("sub_1609F"), setup_sub_1609f_disabled(buffer_size))
    out, data = original_run(wrapper, dump_count=buffer_size * 8, dump_offset=DSEG_SCRATCH, dump_seg=DATA_SEG)
    got = translated("sub1609fdisabled", hex(buffer_size))
    expected = b"".join(b"\x00\x00\x00\x00\xa5\xa5\xa5\xa5" for _ in range(buffer_size))
    assert data == expected
    assert field(out, "ax") == "0000"
    assert field(out, "bx") == "0004"
    assert field(out, "cx") == "0000"
    assert field(out, "si") == f"{CHANNEL_OFF:04x}"
    assert field(out, "di") == f"{DSEG_SCRATCH + buffer_size * 8:04x}"
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert got.endswith("data=" + expected.hex())


def test_original_and_abi_sub_1609f_public_symbol_disabled_channel_zero_fill() -> None:
    buffer_size = 0x12
    wrapper = make_wrapper(original_offset("sub_1609F"), setup_sub_1609f_disabled(buffer_size))
    out, data = original_run(wrapper, dump_count=buffer_size * 8, dump_offset=DSEG_SCRATCH, dump_seg=DATA_SEG)
    got = translated("abisub1609fdisabled", hex(buffer_size))
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()


def test_original_and_translated_setvideomode_noop_modes() -> None:
    for mode in [0, 1]:
        out, data = original_seg001_call(
            original_offset("setvideomode"),
            setup_setvideomode_noop(mode),
            dump_count=1,
            dump_offset=0x1680,
            dump_seg=DSEG,
        )
        got = translated("setvideomodenoop", hex(mode))
        assert data == bytes([mode])
        assert field(out, "ax") == "1234"
        assert field(out, "bx") == "5678"
        assert field(out, "cx") == "9abc"
        assert field(out, "dx") == "def0"
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert got.endswith(f"data={mode:02x}")


def test_original_and_abi_setvideomode_noop_modes() -> None:
    for mode in [0, 1]:
        out, data = original_seg001_call(
            original_offset("setvideomode"),
            setup_setvideomode_noop(mode),
            dump_count=1,
            dump_offset=0x1680,
            dump_seg=DSEG,
        )
        got = translated("abisetvideomode", hex(mode))
        assert data == bytes([mode])
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert got.endswith(f"data={mode:02x}")


def test_translated_text_mode_geometry_supports_project_modes() -> None:
    expected = {
        0: ("40", "25", "80", "1000", "2000"),
        1: ("40", "25", "80", "1000", "2000"),
        2: ("80", "25", "160", "2000", "4000"),
        3: ("80", "25", "160", "2000", "4000"),
        0x50: ("80", "50", "160", "4000", "8000"),
    }
    for mode, (cols, rows, rowbytes, cells, screenbytes) in expected.items():
        got = translated("textmodegeom", hex(mode))
        if got is not None:
            assert field(got, "cols") == cols
            assert field(got, "rows") == rows
            assert field(got, "rowbytes") == rowbytes
            assert field(got, "cells") == cells
            assert field(got, "screenbytes") == screenbytes
        got = translated("settextmodegeom", hex(mode))
        if got is not None:
            assert field(got, "cols") == cols
            assert field(got, "rows") == rows
            assert field(got, "rowbytes") == rowbytes
            assert field(got, "cells") == cells
            assert field(got, "screenbytes") == screenbytes
            assert field(got, "data") == f"{mode:02x}"


def test_translated_supported_text_mode_inventory_has_project_geometries() -> None:
    expected = {
        0: ("40", "25", "80", "1000", "2000"),
        1: ("80", "25", "160", "2000", "4000"),
        2: ("80", "50", "160", "4000", "8000"),
    }
    for index, (cols, rows, rowbytes, cells, screenbytes) in expected.items():
        got = translated("textmodesupported", str(index))
        if got is not None:
            assert field(got, "present") == "1"
            assert field(got, "cols") == cols
            assert field(got, "rows") == rows
            assert field(got, "rowbytes") == rowbytes
            assert field(got, "cells") == cells
            assert field(got, "screenbytes") == screenbytes
    got = translated("textmodesupported", "3")
    if got is not None:
        assert field(got, "present") == "0"


def test_translated_text_screen_resize_rebinds_root_plane_to_new_mode() -> None:
    got = translated("textscreenresize")
    if got is not None:
        assert field(got, "cols") == "80"
        assert field(got, "rows") == "50"
        assert field(got, "stride") == "80"
        assert field(got, "oldtail") == "411e"
        assert field(got, "newtail") == "422f"
        assert field(got, "screenbytes") == "8000"


def test_translated_text_screen_rejects_unsupported_resize_without_rebinding_root() -> None:
    got = translated("textscreenresizebad")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "cols") == "80"
        assert field(got, "rows") == "25"
        assert field(got, "stride") == "80"
        assert field(got, "tail") == "522a"
        assert field(got, "screenbytes") == "4000"
        assert field(got, "supported") == "0"


def test_translated_text_screen_rejects_resize_that_exceeds_capacity_through_split_runner() -> None:
    got = translated("textscreenresizecapacity")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "cols") == "40"
        assert field(got, "rows") == "25"
        assert field(got, "stride") == "40"
        assert field(got, "tail") == "431d"
        assert field(got, "screenbytes") == "2000"
        assert field(got, "capacity") == "4000"
        assert field(got, "can80x25") == "1"
        assert field(got, "can80x50") == "0"
        assert field(got, "fits80x25") == "1"
        assert field(got, "fits80x50") == "0"


def test_translated_text_screen_resize_to_80x25_rebinds_root_through_split_runner() -> None:
    got = translated("textscreenresize80x25")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "cols") == "80"
        assert field(got, "rows") == "25"
        assert field(got, "stride") == "80"
        assert field(got, "first") == "531b"
        assert field(got, "tail") == "5a2c"
        assert field(got, "old40") == "4f1a"
        assert field(got, "screenbytes") == "4000"
        assert field(got, "supported") == "1"


def test_translated_text_screen_resize_to_80x50_rebinds_root_through_split_runner() -> None:
    got = translated("textscreenresize80x50")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "cols") == "80"
        assert field(got, "rows") == "50"
        assert field(got, "stride") == "80"
        assert field(got, "first") == "541d"
        assert field(got, "tail") == "462e"
        assert field(got, "old40") == "4f1c"
        assert field(got, "screenbytes") == "8000"
        assert field(got, "supported") == "1"


def test_translated_text_screen_resize_cycle_rebinds_root_through_split_runner() -> None:
    got = translated("textscreenresizecycle")
    if got is not None:
        assert field(got, "wide_ok") == "1"
        assert field(got, "narrow_ok") == "1"
        assert field(got, "cols") == "40"
        assert field(got, "rows") == "25"
        assert field(got, "stride") == "40"
        assert field(got, "first") == "4e2f"
        assert field(got, "tail") == "523a"
        assert field(got, "oldwide") == "571e"
        assert field(got, "screenbytes") == "2000"
        assert field(got, "supported") == "1"


def test_translated_text_screen_set_video_mode_resizes_and_selects_layout() -> None:
    expected = {
        0: ("40", "25", "40", "80", "2000", "9", "24"),
        3: ("80", "25", "80", "160", "4000", "21", "46"),
        0x50: ("80", "50", "80", "160", "8000", "21", "46"),
    }
    for mode, (cols, rows, stride, rowbytes, screenbytes, left_x, play_x) in expected.items():
        got = translated("textscreenvideomode", hex(mode))
        if got is not None:
            assert field(got, "cols") == cols
            assert field(got, "rows") == rows
            assert field(got, "stride") == stride
            assert field(got, "rowbytes") == rowbytes
            assert field(got, "screenbytes") == screenbytes
            assert field(got, "layout_left") == left_x
            assert field(got, "layout_play") == play_x
            assert field(got, "fits") == "1"


def test_translated_text_screen_draws_ui_sections_through_screen_api() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("textscreenui")
    if got is not None:
        assert field(got, "fits") == "1"
        assert field(got, "mode") == "40,25"
        assert field(got, "top") == "da7f"
        assert field(got, "module") == cells("2/9       ", 0x7F)
        assert field(got, "meter_l") == "db2a" * 16
        assert field(got, "meter_r") == "db4c" * 16


def test_translated_terminal_wrapper_keeps_vga_memory_backend_swappable() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("terminalwrap")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "mode") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "top") == "da7f"
        assert field(got, "module") == cells("2/9       ", 0x7F)
        assert field(got, "meter_l") == "db2a" * 16
        assert field(got, "meter_r") == "db4c" * 16


def test_translated_notcurses_wrapper_exposes_standard_plane() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("notcurseswrap")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "mode") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "top") == "da7f"
        assert field(got, "module") == cells("2/9       ", 0x7F)
        assert field(got, "std") == cells("NC   ", 0x1E)


def test_translated_sdl_audio_facade_wraps_sb16_stereo_path() -> None:
    got = translated("sdlaudiofacade")
    if got is not None:
        assert field(got, "backend") == "1"
        assert field(got, "backend_name") == "sdl-compatible-sb16-stereo"
        assert field(got, "hw") == "0"
        assert field(got, "spec_backend") == "1"
        assert field(got, "spec_hw") == "0"
        assert field(got, "spec_rate") == "44100"
        assert field(got, "audio_status") == "Playback"
        assert field(got, "format") == "s16-stereo"
        assert field(got, "rate") == "44100"
        assert field(got, "bits") == "16"
        assert field(got, "channels") == "2"
        assert field(got, "signed") == "1"
        assert field(got, "framebytes") == "4"
        assert field(got, "sb16") == "1"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "bytes") == "8"
        assert field(got, "levels") == "15,16"
        assert field(got, "data") == "ff7f0080004000c0"
        assert field(got, "silence_frames") == "3"
        assert field(got, "underrun") == "1"
        assert field(got, "reset") == "0,0"
        assert field(got, "silence_bytes") == "4"


def test_translated_sdl_audio_init_uses_sb16_stereo_format_through_split_runner() -> None:
    got = translated("sdlaudioinitformat")
    if got is not None:
        assert field(got, "backend") == "1"
        assert field(got, "format") == "signed-16-stereo"
        assert field(got, "rate") == "44100"
        assert field(got, "bits") == "16"
        assert field(got, "channels") == "2"
        assert field(got, "signed") == "1"
        assert field(got, "framebytes") == "4"
        assert field(got, "samples") == "2"
        assert field(got, "sb16") == "1"
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "frames10") == "2"
        assert field(got, "bytes3") == "12"
        assert field(got, "captured") == "0"


def test_translated_sdl_audio_write_returns_accepted_bytes_through_split_runner() -> None:
    got = translated("sdlaudiowriteaccepted")
    if got is not None:
        assert field(got, "accepted") == "8"
        assert field(got, "captured") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "0100020003000400"


def test_translated_sdl_audio_write_accepts_zero_for_null_pcm_through_split_runner() -> None:
    got = translated("sdlaudiowritenull")
    if got is not None:
        assert field(got, "accepted") == "0"
        assert field(got, "captured") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "2"


def test_translated_sdl_audio_write_levels_use_only_accepted_signed_frames_through_split_runner() -> None:
    got = translated("sdlaudiowritesignedlevels")
    if got is not None:
        assert field(got, "accepted") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "levels") == "8,1"
        assert field(got, "data") == "00c000f8"


def test_translated_sdl_audio_callback_returns_accepted_bytes_through_split_runner() -> None:
    got = translated("sdlaudiocallbackaccepted")
    if got is not None:
        assert field(got, "accepted") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "31003200"


def test_translated_sdl_audio_callback_submits_only_complete_sb16_frames_through_split_runner() -> None:
    got = translated("sdlaudiocallbackpartial")
    if got is not None:
        assert field(got, "frames_for_bytes") == "1"
        assert field(got, "accepted") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "61006200"


def test_translated_sdl_audio_callback_levels_use_only_accepted_signed_frames_through_split_runner() -> None:
    got = translated("sdlaudiocallbacksignedlevels")
    if got is not None:
        assert field(got, "accepted") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "levels") == "8,1"
        assert field(got, "data") == "00c000f8"


def test_translated_sdl_audio_callback_accepts_zero_while_paused_through_split_runner() -> None:
    got = translated("sdlaudiocallbackpaused")
    if got is not None:
        assert field(got, "accepted") == "0"
        assert field(got, "captured") == "0"
        assert field(got, "paused") == "1"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "2"


def test_translated_sdl_audio_callback_accepts_zero_for_null_user_through_split_runner() -> None:
    got = translated("sdlaudiocallbacknull")
    if got is not None:
        assert field(got, "accepted") == "0"


def test_translated_sdl_audio_callback_accepts_zero_for_null_stream_through_split_runner() -> None:
    got = translated("sdlaudiocallbacknullstream")
    if got is not None:
        assert field(got, "accepted") == "0"
        assert field(got, "captured") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "2"


def test_translated_runtime_sdl_init_uses_sb16_stereo_format_through_split_runner() -> None:
    got = translated("runtimeinitformat")
    if got is not None:
        assert field(got, "backend") == "1"
        assert field(got, "format") == "s16-stereo"
        assert field(got, "rate") == "44100"
        assert field(got, "bits") == "16"
        assert field(got, "channels") == "2"
        assert field(got, "signed") == "1"
        assert field(got, "framebytes") == "4"
        assert field(got, "samples") == "2"
        assert field(got, "sb16") == "1"
        assert field(got, "sdl") == "1"
        assert field(got, "sbhw") == "0"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "frames10") == "2"
        assert field(got, "bytes3") == "12"
        assert field(got, "captured") == "0"


def test_translated_runtime_sdl_init_counters_start_empty_through_split_runner() -> None:
    got = translated("runtimeinitcounters")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "captured") == "0"


def test_translated_runtime_sdl_start_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimeaudiostartclean")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "captured") == "0"


def test_translated_runtime_sdl_stop_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimeaudiostopclean")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "1"
        assert field(got, "can_queue") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "captured") == "0"


def test_translated_runtime_sdl_pause_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimeaudiopauseclean")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "1"
        assert field(got, "can_queue") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "captured") == "0"


def test_translated_runtime_sdl_pause_resume_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimeaudiopauseresumeclean")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "captured") == "0"


def test_translated_runtime_sdl_capacity_change_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimeaudiocapacityclean")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "1"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "3"
        assert field(got, "captured") == "0"


def test_translated_runtime_sdl_add_capacity_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimeaudioaddcapacityclean")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "1"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "5"
        assert field(got, "captured") == "0"


def test_translated_runtime_sdl_clear_empty_queue_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimeaudioclearqueuedclean")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "1"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "4"
        assert field(got, "captured") == "0"


def test_translated_runtime_sdl_reset_clean_counters_preserves_capacity_through_split_runner() -> None:
    got = translated("runtimeaudioresetcountersclean")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "1"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "4"
        assert field(got, "captured") == "0"


def test_translated_runtime_audio_write_returns_accepted_bytes_through_split_runner() -> None:
    got = translated("runtimewriteaccepted")
    if got is not None:
        assert field(got, "accepted") == "8"
        assert field(got, "captured") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "1100120013001400"


def test_translated_runtime_audio_write_accepts_zero_for_null_pcm_through_split_runner() -> None:
    got = translated("runtimewritenull")
    if got is not None:
        assert field(got, "accepted") == "0"
        assert field(got, "captured") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "2"


def test_translated_runtime_audio_bad_reopen_preserves_active_sb16_device_through_split_runner() -> None:
    got = translated("runtimeaudioopenpreservesactive")
    if got is not None:
        assert field(got, "bad_reopen") == "0"
        assert field(got, "format") == "s16-stereo"
        assert field(got, "bits") == "16"
        assert field(got, "channels") == "2"
        assert field(got, "signed") == "1"
        assert field(got, "sb16") == "1"
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "accepted") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "22114433"


def test_translated_runtime_audio_levels_track_signed_stereo_peaks_through_split_runner() -> None:
    got = translated("runtimesignedlevels")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "accepted") == "8"
        assert field(got, "active") == "1"
        assert field(got, "captured") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "levels") == "15,14"
        assert field(got, "data") == "0080001000200090"


def test_translated_runtime_audio_queue_submits_only_complete_sb16_frames_through_split_runner() -> None:
    got = translated("runtimequeuepartial")
    if got is not None:
        assert field(got, "frames_for_bytes") == "1"
        assert field(got, "queued") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "queued_frames") == "1"
        assert field(got, "queued_bytes") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "71007200"


def test_translated_runtime_audio_queue_levels_use_only_accepted_signed_frames_through_split_runner() -> None:
    got = translated("runtimequeuesignedlevels")
    if got is not None:
        assert field(got, "queued") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "queued_frames") == "1"
        assert field(got, "queued_bytes") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "levels") == "8,1"
        assert field(got, "data") == "00c000f8"


def test_translated_runtime_audio_pause_blocks_queue_through_split_runner() -> None:
    got = translated("runtimequeuepaused")
    if got is not None:
        assert field(got, "paused_queue") == "0"
        assert field(got, "live_queue") == "4"
        assert field(got, "paused") == "0"
        assert field(got, "captured") == "4"
        assert field(got, "queued_frames") == "1"
        assert field(got, "queued_bytes") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "91009200"


def test_translated_runtime_audio_pause_blocks_writes_but_preserves_levels_and_meter_display_through_split_runner() -> None:
    got = translated("runtimepausepreserveslevels")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "first") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "paused") == "0"
        assert field(got, "paused_flag") == "1"
        assert field(got, "active") == "0"
        assert field(got, "after") == "15,16"
        assert field(got, "after_l") == "db2a" * 16
        assert field(got, "after_r") == "db4c" * 16
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_audio_pause_resume_updates_levels_and_meter_display_after_accepted_frame_through_split_runner() -> None:
    got = translated("runtimepauseresumelevels")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "first") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "paused") == "0"
        assert field(got, "paused_levels") == "15,16"
        assert field(got, "paused_l") == "db2a" * 16
        assert field(got, "paused_r") == "db4c" * 16
        assert field(got, "live") == "4"
        assert field(got, "paused_flag") == "0"
        assert field(got, "active") == "1"
        assert field(got, "after") == "0,0"
        assert field(got, "after_l") == "b008" * 16
        assert field(got, "after_r") == "b008" * 16
        assert field(got, "captured") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "ff7f008000000000"


def test_translated_runtime_audio_stop_start_gates_queue_through_split_runner() -> None:
    got = translated("runtimequeuestopstart")
    if got is not None:
        assert field(got, "stopped_queue") == "0"
        assert field(got, "live_queue") == "4"
        assert field(got, "active") == "1"
        assert field(got, "captured") == "4"
        assert field(got, "queued_frames") == "1"
        assert field(got, "queued_bytes") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "b100b200"


def test_translated_runtime_audio_stop_blocks_writes_but_preserves_levels_and_meter_display_through_split_runner() -> None:
    got = translated("runtimestoppreserveslevels")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "first") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "stopped") == "0"
        assert field(got, "paused_flag") == "1"
        assert field(got, "active") == "0"
        assert field(got, "after") == "15,16"
        assert field(got, "after_l") == "db2a" * 16
        assert field(got, "after_r") == "db4c" * 16
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_audio_stop_start_updates_levels_and_meter_display_after_accepted_frame_through_split_runner() -> None:
    got = translated("runtimestopstartlevels")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "first") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "stopped") == "0"
        assert field(got, "stopped_levels") == "15,16"
        assert field(got, "stopped_l") == "db2a" * 16
        assert field(got, "stopped_r") == "db4c" * 16
        assert field(got, "live") == "4"
        assert field(got, "paused_flag") == "0"
        assert field(got, "active") == "1"
        assert field(got, "after") == "0,0"
        assert field(got, "after_l") == "b008" * 16
        assert field(got, "after_r") == "b008" * 16
        assert field(got, "captured") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "ff7f008000000000"


def test_translated_runtime_audio_clear_queued_preserves_levels_until_explicit_reset_through_split_runner() -> None:
    got = translated("runtimeclearqueuedlevels")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "queued") == "4"
        assert field(got, "before") == "1,4"
        assert field(got, "levels_before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "after_clear") == "0,0"
        assert field(got, "levels_clear") == "15,16"
        assert field(got, "clear_l") == "db2a" * 16
        assert field(got, "clear_r") == "db4c" * 16
        assert field(got, "after_reset") == "0,0"
        assert field(got, "reset_l") == "b008" * 16
        assert field(got, "reset_r") == "b008" * 16
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_audio_reset_counters_preserves_levels_and_meter_display_through_split_runner() -> None:
    got = translated("runtimeresetcounterslevels")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "accepted") == "8"
        assert field(got, "before") == "2,1,0,15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "after") == "0,0,0,15,16"
        assert field(got, "after_l") == "db2a" * 16
        assert field(got, "after_r") == "db4c" * 16
        assert field(got, "active") == "1"
        assert field(got, "captured") == "8"
        assert field(got, "data") == "ff7f0080004000c0"


def test_translated_runtime_audio_reset_counters_clears_underrun_and_preserves_empty_meter_display_through_split_runner() -> None:
    got = translated("runtimeresetunderrunlevels")
    if got is not None:
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "before") == "2,2,0,0,0"
        assert field(got, "before_l") == "b008" * 16
        assert field(got, "before_r") == "b008" * 16
        assert field(got, "after") == "0,0,0,0,0"
        assert field(got, "after_l") == "b008" * 16
        assert field(got, "after_r") == "b008" * 16
        assert field(got, "active") == "1"
        assert field(got, "captured") == "8"
        assert field(got, "data") == "0000000000000000"


def test_translated_runtime_audio_levels_display_updates_on_80x50_text_surface_through_split_runner() -> None:
    got = translated("runtimelevelsdisplay80x50")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "accepted") == "4"
        assert field(got, "levels") == "15,16"
        assert field(got, "high_l") == "db2a" * 16
        assert field(got, "high_r") == "db4c" * 16
        assert field(got, "guard") == "00000000"
        assert field(got, "captured") == "4"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_audio_reset_levels_redraws_empty_meters_on_80x50_text_surface_through_split_runner() -> None:
    got = translated("runtimelevelsreset80x50")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "sdl") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "accepted") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "after") == "0,0"
        assert field(got, "after_l") == "b008" * 16
        assert field(got, "after_r") == "b008" * 16
        assert field(got, "guard") == "00000000"
        assert field(got, "captured") == "4"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_sb16_hardware_init_uses_sb16_stereo_format_through_split_runner() -> None:
    got = translated("runtimehwinitformat")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "format") == "s16-stereo"
        assert field(got, "rate") == "44100"
        assert field(got, "bits") == "16"
        assert field(got, "channels") == "2"
        assert field(got, "signed") == "1"
        assert field(got, "framebytes") == "4"
        assert field(got, "samples") == "2"
        assert field(got, "sb16") == "1"
        assert field(got, "sdl") == "0"
        assert field(got, "sbhw") == "1"
        assert field(got, "hw") == "1"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "frames10") == "2"
        assert field(got, "bytes3") == "12"
        assert field(got, "captured") == "0"


def test_translated_runtime_sb16_hardware_status_text_stays_on_hardware_boundary_through_split_runner() -> None:
    got = translated("runtimehwstatustext")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "sbhw") == "1"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "1"
        assert field(got, "initial") == "Playback"
        assert field(got, "started") == "Playback"
        assert field(got, "paused_text") == "Playback"
        assert field(got, "resumed") == "Playback"
        assert field(got, "stopped") == "Playback"
        assert "Playback enabled" in got
        assert field(got, "captured") == "0"


def test_translated_player_sb16_hardware_wrapper_programs_dma_and_dsp_through_split_runner() -> None:
    got = translated("playersb16hwblock")
    if got is not None:
        assert field(got, "detected") == "1"
        assert field(got, "active") == "0"
        assert field(got, "dma") == "0"
        assert field(got, "last") == "8"
        assert field(got, "blocks") == "1"
        assert field(got, "bytes") == "8"
        assert field(got, "copy") == "8"
        assert field(got, "reads") == "258,9"
        assert field(got, "physical") == "74560"
        assert field(got, "data") == "1000200030004000"
        assert field(got, "writes") == (
            "226:01,226:00,"
            "0d4:05,0d8:00,0d6:49,0c4:a0,0c4:91,08b:01,0c6:03,0c6:00,0d4:01,"
            "22c:41,22c:ac,22c:44,22c:d1,22c:b0,22c:30,22c:03,22c:00,22c:d5,226:01,226:00"
        )


def test_translated_player_sb16_hardware_keeps_rate_and_speaker_live_across_blocks() -> None:
    got = translated("playersb16hwtwoblocks")
    if got is not None:
        assert field(got, "detected") == "1"
        assert field(got, "active") == "0"
        assert field(got, "dma") == "0"
        assert field(got, "last") == "8"
        assert field(got, "blocks") == "2"
        assert field(got, "bytes") == "16"
        assert field(got, "copy") == "8"
        assert field(got, "tail") == "8000"
        writes = field(got, "writes")
        assert writes.count("22c:41") == 1
        assert writes.count("22c:d1") == 1
        assert writes.count("22c:b0") == 2
        assert writes.endswith("22c:b0,22c:30,22c:03,22c:00,22c:d5,226:01,226:00")


def test_translated_player_sb16_hardware_programs_configured_dma6_ports() -> None:
    got = translated("playersb16hwdma6")
    if got is not None:
        assert field(got, "dma16") == "6"
        assert field(got, "detected") == "1"
        assert field(got, "active") == "0"
        assert field(got, "dma") == "0"
        assert field(got, "last") == "8"
        writes = field(got, "writes")
        assert "0c8:a0,0c8:91,089:01,0ca:03,0ca:00" in writes
        assert "0c4:a0" not in writes
        assert "08b:01" not in writes


def test_translated_player_sb16_hardware_programs_configured_dma7_ports() -> None:
    got = translated("playersb16hwdma7")
    if got is not None:
        assert field(got, "dma16") == "7"
        assert field(got, "detected") == "1"
        assert field(got, "active") == "0"
        assert field(got, "dma") == "0"
        assert field(got, "last") == "8"
        writes = field(got, "writes")
        assert "0cc:a0,0cc:91,08a:01,0ce:03,0ce:00" in writes
        assert "0c4:a0" not in writes
        assert "08b:01" not in writes


def test_translated_player_sb16_hardware_programs_configured_base_port() -> None:
    got = translated("playersb16hwbase240")
    if got is not None:
        assert field(got, "base") == "240"
        assert field(got, "detected") == "1"
        assert field(got, "active") == "0"
        assert field(got, "dma") == "0"
        assert field(got, "last") == "8"
        assert field(got, "reads") == "258,9"
        writes = field(got, "writes")
        assert writes.startswith("246:01,246:00,")
        assert "24c:41,24c:ac,24c:44,24c:d1,24c:b0,24c:30,24c:03,24c:00,24c:d5,246:01,246:00" in writes
        assert "226:01" not in writes
        assert "22c:41" not in writes


def test_translated_player_playback_timer_uses_wrapped_bios_tick_source() -> None:
    got = translated("playerplaybacktimerhw")
    if got is not None:
        assert field(got, "prime") == "1"
        assert field(got, "ready0") == "0"
        assert field(got, "ready1") == "0"
        assert field(got, "ready2") == "1"
        assert field(got, "ready3") == "1"
        assert field(got, "last") == "109"
        assert field(got, "elapsed") == "0"
        assert field(got, "count") == "2"
        assert field(got, "ticks") == "109"
        assert field(got, "interval") == "3"


def test_translated_player_continuous_loop_uses_nonzero_bios_tick_interval() -> None:
    got = translated("playercontinuousloophw")
    if got is not None:
        assert field(got, "policy") == "2"
        assert field(got, "max") == "0"
        assert int(field(got, "frames")) == SB16_CONTINUOUS_BLOCK_FRAMES
        assert field(got, "interval") == "1"
        assert field(got, "cadence") == "timer"
        assert field(got, "name") == "playback"


def test_translated_player_continuous_keyboard_exit_uses_dos_hardware_wrapper() -> None:
    got = translated("playerkeyboardhw")
    if got is not None:
        assert field(got, "before") == "0"
        assert field(got, "after") == "1"
        assert field(got, "policy") == "2"
        assert field(got, "interval") == "1"


def test_translated_player_continuous_playback_loop_stops_on_keyboard_after_one_block() -> None:
    got = translated("playerkeyboardstophw")
    if got is not None:
        assert_playback_loop(got, "playback", "timer-keyboard", "timer", 0, SB16_CONTINUOUS_BLOCK_FRAMES)
        assert field(got, "blocks") == "1"
        assert int(field(got, "frames")) == SB16_CONTINUOUS_BLOCK_FRAMES
        assert int(field(got, "accepted")) == SB16_CONTINUOUS_BLOCK_BYTES
        assert int(field(got, "audio_bytes")) == sb16_stereo_byte_count(SB16_CONTINUOUS_BLOCK_FRAMES)
        assert_sb16_stereo_block_accounting(1, int(field(got, "frames")), int(field(got, "accepted")), SB16_CONTINUOUS_BLOCK_FRAMES)
        assert_sb16_stereo_frame_bytes(int(field(got, "frames")), int(field(got, "audio_bytes")))
        assert field(got, "limit") == "0"
        assert field(got, "source_end") == "0"
        assert field(got, "stop") == "keyboard"


def test_translated_player_playback_submission_refreshes_dos_text_audio_levels() -> None:
    got = translated("playerplaybacklevelshw")
    if got is not None:
        audio_digest = parse_player_hw_audio_digest(got)
        text_digest = parse_player_hw_text_digest(got)
        assert_playback_loop(got, "level-proof", "bounded-trial", "immediate", 1, SB16_BOUNDED_BLOCK_FRAMES)
        assert field(got, "blocks") == "1"
        assert int(field(got, "frames")) == SB16_BOUNDED_BLOCK_FRAMES
        assert int(field(got, "accepted")) == SB16_BOUNDED_BLOCK_BYTES
        assert audio_digest["copies"] == 1
        assert audio_digest["bytes"] == sb16_stereo_byte_count(SB16_BOUNDED_BLOCK_FRAMES)
        assert_sb16_stereo_block_accounting(1, int(field(got, "frames")), int(field(got, "accepted")), SB16_BOUNDED_BLOCK_FRAMES)
        assert_sb16_stereo_frame_bytes(int(field(got, "frames")), audio_digest["bytes"])
        assert audio_digest["checksum"] != 0
        assert audio_digest["first"] != audio_digest["tail"]
        assert text_digest["copies"] == 2
        assert text_digest["segment"] == VGA_COLOR_TEXT_SEG
        assert text_digest["offset"] == 0
        assert text_digest["bytes"] == 4000
        assert text_digest["checksum"] != 0
        assert text_digest["nonblank"] > 0
        assert field(got, "ui_module") == "1"
        assert field(got, "ui_sb16") == "1"
        assert field(got, "ui_playback") == "1"
        assert field(got, "stop") == "block-limit"


def test_translated_player_continuous_module_source_stops_on_keyboard_after_sb16_block() -> None:
    got = translated("playermodulekeyboardstophw")
    if got is not None:
        audio_digest = parse_player_hw_audio_digest(got)
        text_digest = parse_player_hw_text_digest(got)
        assert_playback_loop(got, "playback", "timer-keyboard", "timer", 0, SB16_CONTINUOUS_BLOCK_FRAMES)
        assert field(got, "blocks") == "1"
        assert int(field(got, "frames")) == SB16_CONTINUOUS_BLOCK_FRAMES
        assert int(field(got, "accepted")) == SB16_CONTINUOUS_BLOCK_BYTES
        assert audio_digest["copies"] == 1
        assert audio_digest["bytes"] == sb16_stereo_byte_count(SB16_CONTINUOUS_BLOCK_FRAMES)
        assert_sb16_stereo_block_accounting(1, int(field(got, "frames")), int(field(got, "accepted")), SB16_CONTINUOUS_BLOCK_FRAMES)
        assert_sb16_stereo_frame_bytes(int(field(got, "frames")), audio_digest["bytes"])
        assert audio_digest["checksum"] != 0
        assert audio_digest["first"] != audio_digest["tail"]
        assert text_digest["copies"] == 2
        assert text_digest["segment"] == VGA_COLOR_TEXT_SEG
        assert text_digest["offset"] == 0
        assert text_digest["bytes"] == 4000
        assert text_digest["checksum"] != 0
        assert text_digest["nonblank"] > 0
        assert field(got, "stopcode") == "3"
        assert field(got, "interval") == "1"


def test_translated_player_text_hardware_presenter_routes_color_and_mono_segments_through_split_runner() -> None:
    got = translated("playertexthwpresent")
    if got is not None:
        assert field(got, "color_seg") == "b800"
        assert field(got, "color_off") == "0000"
        assert field(got, "color_bytes") == "4000"
        assert field(got, "color_first") == "431e"
        assert field(got, "color_tail") == "5a2c"
        assert field(got, "mono_seg") == "b000"
        assert field(got, "mono_off") == "0000"
        assert field(got, "mono_bytes") == "2000"
        assert field(got, "mono_first") == "4d70"
        assert field(got, "mono_tail") == "5707"
        assert field(got, "project_seg") == "b800"
        assert field(got, "project_off") == "0000"
        assert field(got, "project_bytes") == "8000"
        assert field(got, "project_first") == "463a"
        assert field(got, "project_tail") == "515e"


def test_translated_player_loaded_module_path_drives_audio_and_text_hardware_wrappers_through_split_runner() -> None:
    got = translated("plhw25")
    if got is not None:
        audio_digest = parse_player_hw_audio_digest(got)
        text_digest = parse_player_hw_text_digest(got)
        assert "Module: HWPATH.MOD" in got
        assert "Loader: mod_n_t_module (ProTracker/NoiseTracker MOD)" in got
        assert "Playback output: SB16 16-bit stereo hardware wrapper enabled." in got
        assert "Playback pump:" in got
        assert audio_digest["copies"] == 16
        assert audio_digest["bytes"] == 128
        assert audio_digest["checksum"] == 7456
        assert audio_digest["first"] == 0x006D
        assert audio_digest["tail"] == 0x007C
        assert field(got, "text_copies") == "1"
        assert field(got, "text_seg") == "b800"
        assert field(got, "text_off") == "0000"
        assert field(got, "text_bytes") == "4000"
        assert text_digest["copies"] == 1
        assert text_digest["segment"] == VGA_COLOR_TEXT_SEG
        assert text_digest["offset"] == 0
        assert text_digest["bytes"] == 4000
        assert text_digest["checksum"] == int(field(got, "text_checksum"))
        assert text_digest["nonblank"] == int(field(got, "text_nonblank"))
        assert int(field(got, "text_checksum")) != 0
        assert int(field(got, "text_nonblank")) > 0
        assert field(got, "text_first") == "2007"
        assert field(got, "text_tail") == "2007"
        assert field(got, "ui_module") == "1"
        assert field(got, "ui_sb16") == "1"
        assert field(got, "ui_playback") == "1"
        assert field(got, "sb_detected") == "1"
        assert field(got, "sb_active") == "0"
        assert field(got, "sb_dma") == "0"
        assert field(got, "sb_blocks") == "16"
        assert field(got, "sb_bytes") == "128"
        assert field(got, "writes") == "275"
        assert field(got, "last_write") == "22c:d5"


def test_translated_player_loaded_module_path_presents_40x25_bw_hardware_text_mode() -> None:
    got = translated("plhw40")
    if got is not None:
        audio_digest = parse_player_hw_audio_digest(got)
        text_digest = parse_player_hw_text_digest(got)
        assert "Module: HWPATH.MOD" in got
        assert "Playback pump:" in got
        assert audio_digest["copies"] == 16
        assert audio_digest["bytes"] == 128
        assert audio_digest["checksum"] == 7456
        assert field(got, "text_copies") == "1"
        assert field(got, "text_seg") == "b000"
        assert field(got, "text_off") == "0000"
        assert field(got, "text_bytes") == "2000"
        assert text_digest["copies"] == 1
        assert text_digest["segment"] == VGA_MONO_TEXT_SEG
        assert text_digest["offset"] == 0
        assert text_digest["bytes"] == 2000
        assert int(field(got, "text_checksum")) != 0
        assert int(field(got, "text_nonblank")) > 0
        assert field(got, "ui_module") == "1"
        assert field(got, "ui_sb16") == "1"
        assert field(got, "ui_playback") == "1"
        assert field(got, "sb_detected") == "1"
        assert field(got, "sb_active") == "0"
        assert field(got, "sb_dma") == "0"
        assert field(got, "sb_blocks") == "16"
        assert field(got, "sb_bytes") == "128"
        assert field(got, "writes") == "275"
        assert field(got, "last_write") == "22c:d5"


def test_translated_player_loaded_module_path_presents_80x25_bw_hardware_text_mode() -> None:
    got = translated("plhw8b")
    if got is not None:
        audio_digest = parse_player_hw_audio_digest(got)
        text_digest = parse_player_hw_text_digest(got)
        assert "Module: HWPATH.MOD" in got
        assert "Playback pump:" in got
        assert audio_digest["copies"] == 16
        assert audio_digest["bytes"] == 128
        assert audio_digest["checksum"] == 7456
        assert field(got, "text_copies") == "1"
        assert field(got, "text_seg") == "b000"
        assert field(got, "text_off") == "0000"
        assert field(got, "text_bytes") == "4000"
        assert text_digest["copies"] == 1
        assert text_digest["segment"] == VGA_MONO_TEXT_SEG
        assert text_digest["offset"] == 0
        assert text_digest["bytes"] == 4000
        assert int(field(got, "text_checksum")) != 0
        assert int(field(got, "text_nonblank")) > 0
        assert field(got, "ui_module") == "1"
        assert field(got, "ui_sb16") == "1"
        assert field(got, "ui_playback") == "1"
        assert field(got, "sb_detected") == "1"
        assert field(got, "sb_active") == "0"
        assert field(got, "sb_dma") == "0"
        assert field(got, "sb_blocks") == "16"
        assert field(got, "sb_bytes") == "128"
        assert field(got, "writes") == "275"
        assert field(got, "last_write") == "22c:d5"


def test_translated_player_loaded_module_path_presents_80x50_hardware_text_mode() -> None:
    got = translated("plhw50")
    if got is not None:
        audio_digest = parse_player_hw_audio_digest(got)
        text_digest = parse_player_hw_text_digest(got)
        assert "Module: HWPATH.MOD" in got
        assert "Playback pump:" in got
        assert audio_digest["copies"] == 16
        assert audio_digest["bytes"] == 128
        assert audio_digest["checksum"] == 7456
        assert field(got, "text_copies") == "1"
        assert field(got, "text_seg") == "b800"
        assert field(got, "text_off") == "0000"
        assert field(got, "text_bytes") == "8000"
        assert text_digest["copies"] == 1
        assert text_digest["segment"] == VGA_COLOR_TEXT_SEG
        assert text_digest["offset"] == 0
        assert text_digest["bytes"] == 8000
        assert int(field(got, "text_checksum")) != 0
        assert int(field(got, "text_nonblank")) > 0
        assert field(got, "ui_module") == "1"
        assert field(got, "ui_sb16") == "1"
        assert field(got, "ui_playback") == "1"
        assert field(got, "sb_active") == "0"
        assert field(got, "sb_dma") == "0"
        assert field(got, "sb_blocks") == "16"
        assert field(got, "sb_bytes") == "128"
        assert field(got, "writes") == "275"
        assert field(got, "last_write") == "22c:d5"


def test_translated_player_runtime_can_present_80x50_through_dos_hardware_wrapper() -> None:
    got = translated("playerruntimehw80x50")
    if got is not None:
        text_digest = parse_player_hw_text_digest(got)
        assert field(got, "presented") == "8000"
        assert field(got, "text_copies") == "1"
        assert field(got, "text_seg") == "b800"
        assert field(got, "text_off") == "0000"
        assert field(got, "text_bytes") == "8000"
        assert text_digest["copies"] == 1
        assert text_digest["segment"] == VGA_COLOR_TEXT_SEG
        assert text_digest["offset"] == 0
        assert text_digest["bytes"] == 8000
        assert text_digest["checksum"] == int(field(got, "text_checksum"))
        assert text_digest["nonblank"] == int(field(got, "text_nonblank"))
        assert int(field(got, "text_checksum")) != 0
        assert int(field(got, "text_nonblank")) > 0
        assert field(got, "text_first") == "2007"
        assert field(got, "text_tail") == "2007"
        assert field(got, "title") == "1"
        assert field(got, "sb16") == "0"
        assert field(got, "playback") == "0"
        assert field(got, "mode") == "80"


def test_translated_player_runtime_can_draw_sb16_levels_on_80x50_dos_hardware_text() -> None:
    got = translated("playerruntimehw80x50levels")
    if got is not None:
        text_digest = parse_player_hw_text_digest(got)
        assert field(got, "accepted") == "4"
        assert field(got, "levels") == "15,15"
        assert field(got, "presented") == "8000"
        assert field(got, "audio_copies") == "1"
        assert field(got, "audio_bytes") == "4"
        assert field(got, "text_copies") == "1"
        assert field(got, "text_seg") == "b800"
        assert field(got, "text_off") == "0000"
        assert field(got, "text_bytes") == "8000"
        assert text_digest["copies"] == 1
        assert text_digest["segment"] == VGA_COLOR_TEXT_SEG
        assert text_digest["offset"] == 0
        assert text_digest["bytes"] == 8000
        assert text_digest["checksum"] == int(field(got, "text_checksum"))
        assert text_digest["nonblank"] == int(field(got, "text_nonblank"))
        assert int(field(got, "text_checksum")) != 0
        assert int(field(got, "text_nonblank")) > 0
        assert field(got, "text_first") == "2007"
        assert field(got, "text_tail") == "2007"
        assert field(got, "level_l") == ("db2a" * 15) + "b008"
        assert field(got, "level_r") == ("db4c" * 15) + "b008"
        assert field(got, "title") == "1"
        assert field(got, "mode") == "80"


def test_translated_runtime_sb16_hardware_init_counters_start_empty_through_split_runner() -> None:
    got = translated("runtimehwinitcounters")
    if got is not None:
        assert field(got, "sdl") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "sbhw") == "1"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "captured") == "0"


def test_translated_runtime_sb16_hardware_start_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimehwaudiostartclean")
    if got is not None:
        assert field(got, "sdl") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "sbhw") == "1"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "captured") == "0"


def test_translated_runtime_sb16_hardware_stop_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimehwaudiostopclean")
    if got is not None:
        assert field(got, "sdl") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "sbhw") == "1"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "1"
        assert field(got, "can_queue") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "captured") == "0"


def test_translated_runtime_sb16_hardware_pause_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimehwaudiopauseclean")
    if got is not None:
        assert field(got, "sdl") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "sbhw") == "1"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "1"
        assert field(got, "can_queue") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "captured") == "0"


def test_translated_runtime_sb16_hardware_pause_resume_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimehwaudiopauseresumeclean")
    if got is not None:
        assert field(got, "sdl") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "sbhw") == "1"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "captured") == "0"


def test_translated_runtime_sb16_hardware_capacity_change_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimehwaudiocapacityclean")
    if got is not None:
        assert field(got, "sdl") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "sbhw") == "1"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "1"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "3"
        assert field(got, "captured") == "0"


def test_translated_runtime_sb16_hardware_add_capacity_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimehwaudioaddcapacityclean")
    if got is not None:
        assert field(got, "sdl") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "sbhw") == "1"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "1"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "5"
        assert field(got, "captured") == "0"


def test_translated_runtime_sb16_hardware_clear_empty_queue_keeps_counters_empty_through_split_runner() -> None:
    got = translated("runtimehwaudioclearqueuedclean")
    if got is not None:
        assert field(got, "sdl") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "sbhw") == "1"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "1"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "4"
        assert field(got, "captured") == "0"


def test_translated_runtime_sb16_hardware_reset_clean_counters_preserves_capacity_through_split_runner() -> None:
    got = translated("runtimehwaudioresetcountersclean")
    if got is not None:
        assert field(got, "sdl") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "sbhw") == "1"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "can_queue") == "1"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "queued_frames") == "0"
        assert field(got, "queued_bytes") == "0"
        assert field(got, "capacity") == "4"
        assert field(got, "captured") == "0"


def test_translated_runtime_sb16_hardware_write_returns_accepted_bytes_through_split_runner() -> None:
    got = translated("runtimehwwriteaccepted")
    if got is not None:
        assert field(got, "accepted") == "8"
        assert field(got, "captured") == "8"
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "2100220023002400"


def test_translated_runtime_sb16_hardware_write_accepts_zero_for_null_pcm_through_split_runner() -> None:
    got = translated("runtimehwwritenull")
    if got is not None:
        assert field(got, "accepted") == "0"
        assert field(got, "captured") == "0"
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "2"


def test_translated_runtime_sb16_hardware_bad_reopen_preserves_active_device_through_split_runner() -> None:
    got = translated("runtimehwaudioopenpreservesactive")
    if got is not None:
        assert field(got, "bad_reopen") == "0"
        assert field(got, "format") == "s16-stereo"
        assert field(got, "bits") == "16"
        assert field(got, "channels") == "2"
        assert field(got, "signed") == "1"
        assert field(got, "sb16") == "1"
        assert field(got, "sdl") == "0"
        assert field(got, "sbhw") == "1"
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "accepted") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "66558877"


def test_translated_runtime_sb16_hardware_queue_submits_only_complete_frames_through_split_runner() -> None:
    got = translated("runtimehwqueuepartial")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "frames_for_bytes") == "1"
        assert field(got, "queued") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "queued_frames") == "1"
        assert field(got, "queued_bytes") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "81008200"


def test_translated_runtime_sb16_hardware_queue_levels_use_only_accepted_signed_frames_through_split_runner() -> None:
    got = translated("runtimehwqueuesignedlevels")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "queued") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "queued_frames") == "1"
        assert field(got, "queued_bytes") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "levels") == "8,1"
        assert field(got, "data") == "00c000f8"


def test_translated_runtime_sb16_hardware_pause_blocks_queue_through_split_runner() -> None:
    got = translated("runtimehwqueuepaused")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "paused_queue") == "0"
        assert field(got, "live_queue") == "4"
        assert field(got, "paused") == "0"
        assert field(got, "captured") == "4"
        assert field(got, "queued_frames") == "1"
        assert field(got, "queued_bytes") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "a100a200"


def test_translated_runtime_sb16_hardware_pause_blocks_direct_write_through_split_runner() -> None:
    got = translated("runtimehwwritepaused")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "accepted") == "0"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "1"
        assert field(got, "captured") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "2"
        assert field(got, "levels") == "0,0"
        assert field(got, "data") == ""


def test_translated_runtime_sb16_hardware_pause_blocks_writes_but_preserves_levels_and_meter_display_through_split_runner() -> None:
    got = translated("runtimehwpausepreserveslevels")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "first") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "paused") == "0"
        assert field(got, "paused_flag") == "1"
        assert field(got, "active") == "0"
        assert field(got, "after") == "15,16"
        assert field(got, "after_l") == "db2a" * 16
        assert field(got, "after_r") == "db4c" * 16
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_sb16_hardware_pause_resume_updates_levels_and_meter_display_after_accepted_frame_through_split_runner() -> None:
    got = translated("runtimehwpauseresumelevels")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "first") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "paused") == "0"
        assert field(got, "paused_levels") == "15,16"
        assert field(got, "paused_l") == "db2a" * 16
        assert field(got, "paused_r") == "db4c" * 16
        assert field(got, "live") == "4"
        assert field(got, "paused_flag") == "0"
        assert field(got, "active") == "1"
        assert field(got, "after") == "0,0"
        assert field(got, "after_l") == "b008" * 16
        assert field(got, "after_r") == "b008" * 16
        assert field(got, "captured") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "ff7f008000000000"


def test_translated_runtime_sb16_hardware_stop_blocks_direct_write_through_split_runner() -> None:
    got = translated("runtimehwwritestopped")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "accepted") == "0"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "1"
        assert field(got, "captured") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "2"
        assert field(got, "levels") == "0,0"
        assert field(got, "data") == ""


def test_translated_runtime_sb16_hardware_stop_blocks_writes_but_preserves_levels_and_meter_display_through_split_runner() -> None:
    got = translated("runtimehwstoppreserveslevels")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "first") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "stopped") == "0"
        assert field(got, "paused_flag") == "1"
        assert field(got, "active") == "0"
        assert field(got, "after") == "15,16"
        assert field(got, "after_l") == "db2a" * 16
        assert field(got, "after_r") == "db4c" * 16
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_sb16_hardware_stop_start_updates_levels_and_meter_display_after_accepted_frame_through_split_runner() -> None:
    got = translated("runtimehwstopstartlevels")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "first") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "stopped") == "0"
        assert field(got, "stopped_levels") == "15,16"
        assert field(got, "stopped_l") == "db2a" * 16
        assert field(got, "stopped_r") == "db4c" * 16
        assert field(got, "live") == "4"
        assert field(got, "paused_flag") == "0"
        assert field(got, "active") == "1"
        assert field(got, "after") == "0,0"
        assert field(got, "after_l") == "b008" * 16
        assert field(got, "after_r") == "b008" * 16
        assert field(got, "captured") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "ff7f008000000000"


def test_translated_runtime_sb16_hardware_pause_resume_allows_later_direct_write_through_split_runner() -> None:
    got = translated("runtimehwwritepauseresume")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "paused_accepted") == "0"
        assert field(got, "live_accepted") == "4"
        assert field(got, "active") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "levels") == "1,1"
        assert field(got, "data") == "f300f400"


def test_translated_runtime_sb16_hardware_stop_start_gates_queue_through_split_runner() -> None:
    got = translated("runtimehwqueuestopstart")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "stopped_queue") == "0"
        assert field(got, "live_queue") == "4"
        assert field(got, "active") == "1"
        assert field(got, "captured") == "4"
        assert field(got, "queued_frames") == "1"
        assert field(got, "queued_bytes") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "1"
        assert field(got, "data") == "c100c200"


def test_translated_runtime_sb16_hardware_shutdown_stops_audio_and_clears_levels_through_split_runner() -> None:
    got = translated("runtimehwshutdown")
    if got is not None:
        assert field(got, "accepted") == "8"
        assert field(got, "captured") == "8"
        assert field(got, "active_before") == "1"
        assert field(got, "levels_before") == "15,16"
        assert field(got, "active_after") == "0"
        assert field(got, "levels_after") == "0,0"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "ff7f0080004000c0"


def test_translated_runtime_sb16_hardware_levels_display_updates_from_pcm_through_split_runner() -> None:
    got = translated("runtimehwlevelsdisplay")
    if got is not None:
        assert field(got, "accepted") == "4,4"
        assert field(got, "levels") == "15,16"
        assert field(got, "low_l") == "b008" * 16
        assert field(got, "low_r") == "b008" * 16
        assert field(got, "high_l") == "db2a" * 16
        assert field(got, "high_r") == "db4c" * 16
        assert field(got, "captured") == "8"
        assert field(got, "data") == "00000000ff7f0080"


def test_translated_runtime_sb16_hardware_levels_display_updates_on_80x50_text_surface_through_split_runner() -> None:
    got = translated("runtimehwlevelsdisplay80x50")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "accepted") == "4"
        assert field(got, "levels") == "15,16"
        assert field(got, "high_l") == "db2a" * 16
        assert field(got, "high_r") == "db4c" * 16
        assert field(got, "guard") == "00000000"
        assert field(got, "captured") == "4"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_sb16_hardware_reset_levels_redraws_empty_meters_through_split_runner() -> None:
    got = translated("runtimehwresetlevels")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "accepted") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "after") == "0,0"
        assert field(got, "after_l") == "b008" * 16
        assert field(got, "after_r") == "b008" * 16
        assert field(got, "active") == "1"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_sb16_hardware_reset_levels_redraws_empty_meters_on_80x50_text_surface_through_split_runner() -> None:
    got = translated("runtimehwlevelsreset80x50")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "accepted") == "4"
        assert field(got, "before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "after") == "0,0"
        assert field(got, "after_l") == "b008" * 16
        assert field(got, "after_r") == "b008" * 16
        assert field(got, "guard") == "00000000"
        assert field(got, "active") == "1"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_sb16_hardware_levels_track_signed_stereo_peaks_through_split_runner() -> None:
    got = translated("runtimehwsignedlevels")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "accepted") == "8"
        assert field(got, "active") == "1"
        assert field(got, "captured") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "levels") == "15,14"
        assert field(got, "data") == "0080001000200090"


def test_translated_runtime_sb16_hardware_write_silence_counts_underrun_through_split_runner() -> None:
    got = translated("runtimehwwritesilence")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "active") == "1"
        assert field(got, "captured") == "12"
        assert field(got, "frames") == "3"
        assert field(got, "underrun") == "3"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "levels") == "0,0"
        assert field(got, "data") == "000000000000000000000000"


def test_translated_runtime_sb16_hardware_stopped_silence_does_not_emit_through_split_runner() -> None:
    got = translated("runtimehwstoppedsilence")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "active") == "0"
        assert field(got, "captured") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "3"
        assert field(got, "levels") == "0,0"
        assert field(got, "data") == ""


def test_translated_runtime_sb16_hardware_paused_silence_does_not_emit_through_split_runner() -> None:
    got = translated("runtimehwpausedsilence")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "active") == "0"
        assert field(got, "paused") == "1"
        assert field(got, "captured") == "0"
        assert field(got, "frames") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "3"
        assert field(got, "levels") == "0,0"
        assert field(got, "data") == ""


def test_translated_runtime_sb16_hardware_capacity_refill_accepts_later_frames_through_split_runner() -> None:
    got = translated("runtimehwcapacityrefill")
    if got is not None:
        assert field(got, "accepted") == "4,4"
        assert field(got, "captured") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "3"
        assert field(got, "capacity") == "0"
        assert field(got, "levels") == "1,1"
        assert field(got, "data") == "0100020011001200"


def test_translated_runtime_sb16_hardware_clear_queued_resets_pending_capacity_through_split_runner() -> None:
    got = translated("runtimehwclearqueued")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "queued") == "8"
        assert field(got, "before") == "1,4"
        assert field(got, "after") == "0,0"
        assert field(got, "captured") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "3100320033003400"


def test_translated_runtime_sb16_hardware_clear_queued_preserves_levels_until_explicit_reset_through_split_runner() -> None:
    got = translated("runtimehwclearqueuedlevels")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "queued") == "4"
        assert field(got, "before") == "1,4"
        assert field(got, "levels_before") == "15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "after_clear") == "0,0"
        assert field(got, "levels_clear") == "15,16"
        assert field(got, "clear_l") == "db2a" * 16
        assert field(got, "clear_r") == "db4c" * 16
        assert field(got, "after_reset") == "0,0"
        assert field(got, "reset_l") == "b008" * 16
        assert field(got, "reset_r") == "b008" * 16
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "dropped") == "0"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "ff7f0080"


def test_translated_runtime_sb16_hardware_reset_counters_keeps_device_state_through_split_runner() -> None:
    got = translated("runtimehwresetcounters")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "accepted") == "8"
        assert field(got, "before") == "2,1,0,2,2"
        assert field(got, "after") == "0,0,0,2,2"
        assert field(got, "active") == "1"
        assert field(got, "captured") == "8"
        assert field(got, "data") == "4100420043004400"


def test_translated_runtime_sb16_hardware_reset_counters_preserves_levels_and_meter_display_through_split_runner() -> None:
    got = translated("runtimehwresetcounterslevels")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "accepted") == "8"
        assert field(got, "before") == "2,1,0,15,16"
        assert field(got, "before_l") == "db2a" * 16
        assert field(got, "before_r") == "db4c" * 16
        assert field(got, "after") == "0,0,0,15,16"
        assert field(got, "after_l") == "db2a" * 16
        assert field(got, "after_r") == "db4c" * 16
        assert field(got, "active") == "1"
        assert field(got, "captured") == "8"
        assert field(got, "data") == "ff7f0080004000c0"


def test_translated_runtime_sb16_hardware_reset_counters_clears_underrun_through_split_runner() -> None:
    got = translated("runtimehwresetunderrun")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "before") == "2,2,0,8"
        assert field(got, "after") == "0,0,0,8"
        assert field(got, "active") == "1"
        assert field(got, "data") == "0000000000000000"


def test_translated_runtime_sb16_hardware_reset_counters_clears_underrun_and_preserves_empty_meter_display_through_split_runner() -> None:
    got = translated("runtimehwresetunderrunlevels")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "before") == "2,2,0,0,0"
        assert field(got, "before_l") == "b008" * 16
        assert field(got, "before_r") == "b008" * 16
        assert field(got, "after") == "0,0,0,0,0"
        assert field(got, "after_l") == "b008" * 16
        assert field(got, "after_r") == "b008" * 16
        assert field(got, "active") == "1"
        assert field(got, "captured") == "8"
        assert field(got, "data") == "0000000000000000"


def test_translated_sb16_hardware_facade_uses_same_sdl_like_shape() -> None:
    got = translated("sb16hardwarefacade")
    if got is not None:
        assert field(got, "backend") == "0"
        assert field(got, "backend_name") == "sb16-stereo"
        assert field(got, "hw") == "1"
        assert field(got, "spec_backend") == "0"
        assert field(got, "spec_hw") == "1"
        assert field(got, "status") == "Playback"
        assert field(got, "format") == "s16-stereo"
        assert field(got, "rate") == "44100"
        assert field(got, "frames") == "1"
        assert field(got, "bytes") == "4"
        assert field(got, "data") == "3412cdab"


def test_translated_sdl_audio_callback_adapts_stream_to_sb16_frames() -> None:
    got = translated("sdlaudiocallback")
    if got is not None:
        assert field(got, "bytes") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "dropped") == "0"
        assert field(got, "levels") == "16,15"
        assert field(got, "callback") == "1"
        assert field(got, "data") == "0100ff7f00803412"
        assert field(got, "stopped") == "1"


def test_translated_sdl_audio_open_uses_sdl_like_device_config() -> None:
    got = translated("sdlaudioopen")
    if got is not None:
        assert field(got, "opened") == "1"
        assert field(got, "freq") == "44100"
        assert field(got, "bits") == "16"
        assert field(got, "channels") == "2"
        assert field(got, "signed") == "1"
        assert field(got, "samples") == "1024"
        assert field(got, "obtained_freq") == "44100"
        assert field(got, "obtained_samples") == "1024"
        assert field(got, "backend") == "1"
        assert field(got, "hw") == "0"
        assert field(got, "cb") == "1"
        assert field(got, "userdata") == "1"
        assert field(got, "bytes") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "data") == "1000200030004000"
        assert field(got, "bad_open") == "0"


def test_translated_sdl_audio_open_rejects_non_sb16_stereo_configs_through_split_runner() -> None:
    got = translated("sdlaudioopenrejectnonsb16")
    if got is not None:
        assert field(got, "bad_u8") == "0"
        assert field(got, "bad_mono") == "0"
        assert field(got, "good") == "1"
        assert field(got, "format") == "s16-stereo"
        assert field(got, "bits") == "16"
        assert field(got, "channels") == "2"
        assert field(got, "signed") == "1"
        assert field(got, "sb16") == "1"
        assert field(got, "accepted") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "data") == "44332211"


def test_translated_sdl_audio_bad_reopen_preserves_active_sb16_device_through_split_runner() -> None:
    got = translated("sdlaudioopenpreservesactive")
    if got is not None:
        assert field(got, "good") == "1"
        assert field(got, "bad_reopen") == "0"
        assert field(got, "format") == "s16-stereo"
        assert field(got, "bits") == "16"
        assert field(got, "channels") == "2"
        assert field(got, "signed") == "1"
        assert field(got, "sb16") == "1"
        assert field(got, "paused") == "0"
        assert field(got, "accepted") == "4"
        assert field(got, "captured") == "4"
        assert field(got, "frames") == "1"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "7a563412"


def test_translated_sdl_audio_queue_submits_complete_sb16_frames() -> None:
    got = translated("sdlaudioqueue")
    if got is not None:
        assert field(got, "queued") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "capacity") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "paused_queue") == "0"
        assert field(got, "data") == "0100020003000400"


def test_translated_runtime_facade_combines_terminal_and_audio_paths() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("runtimefacade")
    if got is not None:
        assert field(got, "mode") == "40,25"
        assert field(got, "backend") == "0"
        assert field(got, "spec_backend") == "0"
        assert field(got, "backend_name") == "sb16-stereo"
        assert field(got, "hw") == "1"
        assert field(got, "spec_hw") == "1"
        assert field(got, "format") == "s16-stereo"
        assert field(got, "spec_format") == "s16-stereo"
        assert field(got, "rate") == "44100"
        assert field(got, "spec_rate") == "44100"
        assert field(got, "framebytes") == "4"
        assert field(got, "sb16") == "1"
        assert field(got, "frames") == "2"
        assert field(got, "bytes") == "8"
        assert field(got, "top") == "da7f"
        assert field(got, "module") == cells("2/9       ", 0x7F)
        assert field(got, "meter_l") == "db2a" * 16
        assert field(got, "meter_r") == "db4c" * 16
        assert field(got, "data") == "00c00020ff7f0080"
        assert field(got, "status") == cells("READY ", 0x1F)
        assert field(got, "field") == "461e691e6c1e651e3a1e201e" + cells("DEMO.MOD", 0x2F)
        assert field(got, "size") == "531e691e7a1e651e3a1e201e" + cells("123456", 0x2F)
        assert field(got, "tag") == "541e611e671e3a1e201e" + cells("1234ABCD", 0x2F)
        assert field(got, "present") == "2000"
        assert field(got, "stopped") == "0"
        assert field(got, "reset") == "0,0"


def test_translated_runtime_audio_queue_keeps_sdl_boundary_at_runtime_layer() -> None:
    got = translated("runtimeaudioqueue")
    if got is not None:
        assert field(got, "queued") == "8"
        assert field(got, "frames") == "2"
        assert field(got, "capacity") == "0"
        assert field(got, "underrun") == "0"
        assert field(got, "dropped") == "0"
        assert field(got, "stopped_queue") == "0"
        assert field(got, "data") == "1100220033004400"


def test_translated_runtime_audio_pause_blocks_queue_without_nested_device_access() -> None:
    got = translated("runtimeaudiopause")
    if got is not None:
        assert field(got, "paused_queue") == "0"
        assert field(got, "resumed_queue") == "8"
        assert field(got, "paused") == "0"
        assert field(got, "frames") == "2"
        assert field(got, "capacity") == "0"
        assert field(got, "data") == "2100220023002400"


def test_translated_runtime_audio_reset_counters_preserves_queue_capacity() -> None:
    got = translated("runtimeaudioresetcounters")
    if got is not None:
        assert field(got, "before") == "2,2,0,1"
        assert field(got, "reset") == "0,0,0,1"
        assert field(got, "queued") == "4"
        assert field(got, "after") == "1,0,0,1"
        assert field(got, "data") == "000000000000000031003200"


def test_translated_runtime_module_status_helpers_draw_status_block() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("runtimemodulestatus")
    if got is not None:
        assert field(got, "title") == cells("ProTracker MOD", 0x1F)
        assert field(got, "module") == "4d1e6f1e641e751e6c1e651e3a1e201e" + cells("SONG.MOD", 0x2F)
        assert field(got, "size") == "531e691e7a1e651e3a1e201e" + cells("123456", 0x2F)
        assert field(got, "loader") == "4c1e6f1e611e641e651e721e3a1e201e" + cells("mod_n_t_module", 0x2F)
        assert field(got, "tag") == "541e611e671e3a1e201e" + cells("1234ABCD", 0x2F)


def test_translated_runtime_status_block_draws_module_audio_and_tag() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("runtimestatusblock")
    if got is not None:
        assert field(got, "title") == cells("ProTracker MOD", 0x1F)
        assert field(got, "audio") == "411e751e641e691e6f1e3a1e201e" + cells("sb16-stereo", 0x2F)
        assert field(got, "video") == "561e691e641e651e6f1e3a1e201e312f"
        assert field(got, "tag") == "541e611e671e3a1e201e" + cells("1234ABCD", 0x2F)


def test_translated_module_status_api_exposes_typed_handoff() -> None:
    got = translated("modulestatusapi")
    if got is not None:
        assert field(got, "title") == "ProTracker_MOD"
        assert field(got, "path") == "SONG.MOD"
        assert field(got, "size") == "123456"
        assert field(got, "loader") == "mod_n_t_module"
        assert field(got, "type0") == "00000000"
        assert field(got, "type1") == "1234ABCD"
        assert field(got, "tag") == "1234ABCD"
        assert field(got, "clear") == "00000000"
        assert field(got, "clear_tag") == "00000000"


def test_translated_runtime_audio_status_helper_draws_status_block() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("runtimeaudiostatus")
    if got is not None:
        assert field(got, "audio") == "411e751e641e691e6f1e3a1e201e" + cells("sb16-stereo", 0x2F)
        assert field(got, "hardware") == "481e611e721e641e771e611e721e651e3a1e201e312f"
        assert field(got, "video") == "200720072007200720"
        assert field(got, "playback") == cells("Playback enabled", 0x4F)


def test_translated_runtime_video_status_helper_draws_mode_result() -> None:
    got = translated("runtimevideostatus")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "token") == "rejected"
        assert field(got, "text") == "Video"
        assert field(got, "video") == "561e691e641e651e6f1e3a1e201e302f"


def test_translated_runtime_present_callback_observes_backbuffer() -> None:
    got = translated("runtimepresentcb")
    if got is not None:
        assert field(got, "cols") == "40"
        assert field(got, "rows") == "25"
        assert field(got, "bytes") == "2000"
        assert field(got, "first") == "20072007"


def test_translated_runtime_present_callback_tracks_resized_text_mode() -> None:
    got = translated("runtimepresentresize")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "mode") == "80,50"
        assert field(got, "bytes") == "8000"
        assert field(got, "callback") == "80,50,8000"
        assert field(got, "first") == "20072007"


def test_translated_runtime_audio_callback_init_path_writes_sink() -> None:
    got = translated("runtimeaudiocb")
    if got is not None:
        assert field(got, "backend") == "1"
        assert field(got, "frames") == "1"
        assert field(got, "bytes") == "4"
        assert field(got, "data") == "01020304"


def test_translated_runtime_combined_callbacks_cover_video_and_audio() -> None:
    got = translated("runtimecallbacks")
    if got is not None:
        assert field(got, "video") == "40,25,2000"
        assert field(got, "audio") == "1,4"
        assert field(got, "data") == "05060708"


def test_translated_runtime_config_init_covers_video_and_audio_callbacks() -> None:
    got = translated("runtimeconfig")
    if got is not None:
        assert field(got, "video") == "40,25,2000"
        assert field(got, "audio") == "1,4"
        assert field(got, "data") == "090a0b0c"


def test_translated_runtime_sb16_hardware_callbacks_cover_resized_text_and_audio() -> None:
    got = translated("runtimehwresizecallbacks")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "backend") == "0"
        assert field(got, "hw") == "1"
        assert field(got, "video") == "80,50,8000"
        assert field(got, "audio") == "1,4"
        assert field(got, "framebytes") == "4"
        assert field(got, "data") == "21436587"


def test_translated_runtime_video_spec_tracks_vga_backend_and_present_callback() -> None:
    got = translated("runtimevideospec")
    if got is not None:
        assert field(got, "nohw_backend") == "0"
        assert field(got, "nohw_present") == "0"
        assert field(got, "nohw_mode") == "40,25"
        assert field(got, "cfg_backend") == "0"
        assert field(got, "cfg_present") == "0"
        assert field(got, "cb_backend") == "0"
        assert field(got, "cb_present") == "1"
        assert field(got, "cb_mode") == "80,25"
        assert field(got, "cb_cfg_backend") == "0"
        assert field(got, "cb_cfg_present") == "1"


def test_translated_runtime_start_config_initializes_mode_and_audio() -> None:
    got = translated("runtimestartconfig")
    if got is not None:
        assert field(got, "mode") == "80,50"
        assert field(got, "active") == "1"
        assert field(got, "mode_ok") == "1"
        assert field(got, "capacity") == "8000"


def test_translated_runtime_start_config_checked_reports_mode_failure() -> None:
    got = translated("runtimestartconfigchecked")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "mode") == "40,25"
        assert field(got, "active") == "1"
        assert field(got, "mode_ok") == "0"
        assert field(got, "capacity") == "2000"


def test_translated_runtime_no_hardware_config_helper_sets_memory_and_audio_boundary() -> None:
    got = translated("runtimenohwconfig")
    if got is not None:
        assert field(got, "cells") == "1"
        assert field(got, "capacity") == "2000"
        assert field(got, "mode") == "40,25"
        assert field(got, "present") == "0"
        assert field(got, "video_backend") == "0"
        assert field(got, "video_present") == "0"
        assert field(got, "audio") == "1"
        assert field(got, "has_video") == "0"
        assert field(got, "has_audio") == "1"
        assert field(got, "has_capacity") == "1"
        assert field(got, "valid") == "1"


def test_translated_runtime_sdl_config_helper_sets_video_and_audio_callbacks() -> None:
    got = translated("runtimesdlconfig")
    if got is not None:
        assert field(got, "cells") == "1"
        assert field(got, "capacity") == "2000"
        assert field(got, "mode") == "40,25"
        assert field(got, "present") == "1"
        assert field(got, "puser") == "1"
        assert field(got, "video_backend") == "0"
        assert field(got, "video_present") == "1"
        assert field(got, "audio") == "1"
        assert field(got, "auser") == "1"
        assert field(got, "has_video") == "1"
        assert field(got, "has_audio") == "1"
        assert field(got, "has_capacity") == "1"
        assert field(got, "valid") == "1"


def test_translated_runtime_config_validity_rejects_missing_required_fields() -> None:
    got = translated("runtimeinvalidconfig")
    if got is not None:
        assert field(got, "valid") == "1"
        assert field(got, "err") == "0"
        assert field(got, "name") == "ok"
        assert field(got, "nocells") == "0"
        assert field(got, "err_cells") == "1"
        assert field(got, "name_cells") == "missing-cells"
        assert field(got, "nomode") == "0"
        assert field(got, "err_mode") == "2"
        assert field(got, "name_mode") == "missing-mode"
        assert field(got, "noaudio") == "0"
        assert field(got, "err_audio") == "3"
        assert field(got, "name_audio") == "missing-audio"
        assert field(got, "small") == "0"
        assert field(got, "err_small") == "4"
        assert field(got, "name_small") == "small-cells"
        assert field(got, "unknown") == "unknown"


def test_translated_runtime_invalid_config_init_falls_back_to_inert_runtime() -> None:
    got = translated("runtimeinvalidinit")
    if got is not None:
        assert field(got, "mode") == "40,25"
        assert field(got, "backend") == "1"
        assert field(got, "active") == "0"
        assert field(got, "present") == "2000"
        assert field(got, "valid") == "0"


def test_translated_text_mode_geometry_is_explicit_and_queryable() -> None:
    got = translated("textmodegeometry")
    if got is not None:
        assert field(got, "count") == "3"
        assert field(got, "default") == "80,25"
        assert field(got, "fallback") == "40,25"
        assert field(got, "max") == "8000"
        assert field(got, "mode0") == "40,25,80,1000,2000"
        assert field(got, "mode1") == "80,25,160,2000,4000"
        assert field(got, "mode2") == "80,50,160,4000,8000"
        assert field(got, "bysize") == "80,50"
        assert field(got, "missing") == "1"


def test_translated_runtime_capacity_controls_text_resizes() -> None:
    got = translated("runtimecapacityresize")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "wide") == "80,50"
        assert field(got, "bytes") == "8000"
        assert field(got, "capacity") == "8000"
        assert field(got, "small_ok") == "0"
        assert field(got, "small") == "40,25"
        assert field(got, "bytes_small") == "2000"
        assert field(got, "capacity_small") == "2000"


def test_translated_runtime_resize_facade_tracks_terminal_geometry_and_present() -> None:
    got = translated("runtimeresizefacade")
    if got is not None:
        assert field(got, "ok_wide") == "1"
        assert field(got, "mode") == "80,25"
        assert field(got, "root") == "80,25"
        assert field(got, "stride") == "80"
        assert field(got, "fits") == "1"
        assert field(got, "present") == "80,25,4000"
        assert field(got, "tail") == "523e"
        assert field(got, "small_ok") == "0"
        assert field(got, "small_mode") == "40,25"
        assert field(got, "video_ok") == "0"


def test_translated_bottom_layout_tracks_text_mode_width() -> None:
    expected = {
        0: ("5", "6", "7", "1", "9", "1", "29", "19", "24"),
        1: ("5", "6", "7", "1", "9", "1", "29", "19", "24"),
        2: ("20", "21", "22", "16", "21", "11", "61", "44", "46"),
        3: ("20", "21", "22", "16", "21", "11", "61", "44", "46"),
        0x28: ("23", "24", "25", "19", "21", "11", "61", "44", "46"),
        0x50: ("45", "46", "47", "41", "21", "11", "61", "44", "46"),
    }
    for mode, (module_y, pattern_y, timing_y, play_y, left_x, mode_x, value_x, flag_x, play_x) in expected.items():
        got = translated("bottomlayout", hex(mode))
        if got is not None:
            assert field(got, "module_y") == module_y
            assert field(got, "pattern_y") == pattern_y
            assert field(got, "timing_y") == timing_y
            assert field(got, "left_x") == left_x
            assert field(got, "mode_x") == mode_x
            assert field(got, "value_x") == value_x
            assert field(got, "flag_x") == flag_x
            assert field(got, "play_y") == play_y
            assert field(got, "play_x") == play_x
            assert field(got, "module_w") == "10"
            assert field(got, "pattern_w") == "7"
            assert field(got, "timing_w") == "13"
            assert field(got, "mode_w") == "6"
            assert field(got, "value_w") == "5"
            assert field(got, "play_w") == "4"
        got = translated("bottomlayoutfits", hex(mode))
        if got is not None:
            assert field(got, "fits") == "1"


def test_translated_bottom_status_draws_through_plane_layout() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("txtdrawbottomplane")
    if got is not None:
        assert field(got, "module") == cells("2/9       ", 0x7F)
        assert field(got, "pattern") == cells("3/64   ", 0x7F)
        assert field(got, "timing") == cells("6 at 125bpm  ", 0x7F)
        assert field(got, "mode") == cells("(PAL) ", 0x7E)
        assert field(got, "values") == cells("50%  ", 0x7F) + cells("123% ", 0x7F)
        assert field(got, "flags") == "fe7c" * 4
        assert field(got, "play") == cells("Play", 0x7E)


def test_translated_top_title_draws_through_plane() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("txtdrawtoptitleplane")
    if got is not None:
        assert field(got, "corner") == "da7f"
        assert field(got, "title") == cells("Inertia Player", 0x7F)
        assert field(got, "copy") == cells("Copyright", 0x7F)


def test_translated_ncplane_subplane_and_resize_are_notcurses_style() -> None:
    got = translated("ncplanesubresize")
    if got is not None:
        assert field(got, "rows") == "1"
        assert field(got, "cols") == "1"
        assert field(got, "origin") == "1,2"
        assert field(got, "nested") == "2,3"
        assert field(got, "stride") == "40"
        assert field(got, "data") == "433a"
        assert field(got, "tail") == "422f"
        assert field(got, "clipped") == "0000"


def test_translated_text_subplane_clips_to_parent_edges_through_split_runner() -> None:
    got = translated("textsubplaneclip")
    if got is not None:
        assert field(got, "rows") == "2"
        assert field(got, "cols") == "2"
        assert field(got, "origin") == "23,38"
        assert field(got, "stride") == "40"
        assert field(got, "inside") == "5a6c5a6c"
        assert field(got, "edge") == "5a6c"
        assert field(got, "clipped") == "0000"


def test_translated_text_subplane_at_parent_edge_has_zero_size_and_noops_through_split_runner() -> None:
    got = translated("textsubplanezeroedge")
    if got is not None:
        assert field(got, "rows") == "0"
        assert field(got, "cols") == "0"
        assert field(got, "origin") == "25,40"
        assert field(got, "stride") == "40"
        assert field(got, "tail") == "0000"
        assert field(got, "before") == "00000000"


def test_translated_text_subplane_at_80x50_parent_edge_has_zero_size_and_noops_through_split_runner() -> None:
    got = translated("textsubplane80x50zeroedge")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "rows") == "0"
        assert field(got, "cols") == "0"
        assert field(got, "origin") == "50,80"
        assert field(got, "stride") == "80"
        assert field(got, "tail") == "506a"
        assert field(got, "before") == "0000506a"


def test_translated_notcurses_present_callback_tracks_80x50_mode_through_split_runner() -> None:
    got = translated("textpresent80x50")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "mode") == "80,50"
        assert field(got, "root") == "80,50"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "8000"
        assert field(got, "cb") == "80,50,8000"
        assert field(got, "first") == "481e"
        assert field(got, "tail") == "542f"


def test_translated_notcurses_present_callback_tracks_80x25_bw_alias_through_split_runner() -> None:
    got = translated("textpresent80x25bw")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "mode") == "80,25"
        assert field(got, "root") == "80,25"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "4000"
        assert field(got, "cb") == "80,25,4000"
        assert field(got, "first") == "4217"
        assert field(got, "tail") == "5770"


def test_translated_notcurses_resize_to_80x25_presents_through_split_runner() -> None:
    got = translated("textresize80x25present")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "mode") == "80,25"
        assert field(got, "root") == "80,25"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "4000"
        assert field(got, "cb") == "80,25,4000"
        assert field(got, "first") == "521b"
        assert field(got, "tail") == "532c"


def test_translated_notcurses_present_callback_tracks_mode_cycles_through_split_runner() -> None:
    got = translated("textmodecyclepresent")
    if got is not None:
        assert field(got, "wide_ok") == "1"
        assert field(got, "wide_mode") == "80,25"
        assert field(got, "wide_root") == "80,25"
        assert field(got, "wide_presented") == "4000"
        assert field(got, "wide_cb") == "80,25,4000"
        assert field(got, "wide_first") == "571e"
        assert field(got, "wide_tail") == "452f"
        assert field(got, "narrow_ok") == "1"
        assert field(got, "narrow_mode") == "40,25"
        assert field(got, "narrow_root") == "40,25"
        assert field(got, "narrow_presented") == "2000"
        assert field(got, "narrow_cb") == "40,25,2000"
        assert field(got, "narrow_first") == "4e3a"
        assert field(got, "narrow_tail") == "524b"


def test_translated_notcurses_present_callback_can_be_cleared_through_split_runner() -> None:
    got = translated("textpresentclear")
    if got is not None:
        assert field(got, "no_cb_has") == "0"
        assert field(got, "no_cb_presented") == "2000"
        assert field(got, "cb_has") == "1"
        assert field(got, "cb_presented") == "2000"
        assert field(got, "clear_has") == "0"
        assert field(got, "clear_presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "first") == "422b"
        assert field(got, "current") == "433c"


def test_translated_notcurses_present_callback_can_be_replaced_through_split_runner() -> None:
    got = translated("textpresentreplace")
    if got is not None:
        assert field(got, "first_has") == "1"
        assert field(got, "first_presented") == "2000"
        assert field(got, "second_has") == "1"
        assert field(got, "second_presented") == "2000"
        assert field(got, "first_cb") == "40,25,2000"
        assert field(got, "first") == "492c"
        assert field(got, "second_cb") == "40,25,2000"
        assert field(got, "second") == "4a2d"
        assert field(got, "current") == "4a2d"


def test_translated_notcurses_rejects_unsupported_video_mode_without_rebinding_through_split_runner() -> None:
    got = translated("textbadmodepresent")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "mode") == "40,25"
        assert field(got, "root") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "first") == "4b1b"
        assert field(got, "tail") == "5a2c"


def test_translated_notcurses_rejects_resize_that_exceeds_capacity_through_split_runner() -> None:
    got = translated("textresizecapacitypresent")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "mode") == "40,25"
        assert field(got, "root") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "capacity") == "4000"
        assert field(got, "first") == "431d"
        assert field(got, "tail") == "502e"
        assert field(got, "fits80x25") == "1"
        assert field(got, "fits80x50") == "0"


def test_translated_terminal_rejects_resize_that_exceeds_capacity_through_split_runner() -> None:
    got = translated("terminalresizecapacitypresent")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "backend") == "1"
        assert field(got, "has") == "1"
        assert field(got, "mode") == "40,25"
        assert field(got, "root") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "capacity") == "4000"
        assert field(got, "first") == "541f"
        assert field(got, "tail") == "4330"
        assert field(got, "fits80x25") == "1"
        assert field(got, "fits80x50") == "0"


def test_translated_terminal_resize_to_80x25_presents_through_split_runner() -> None:
    got = translated("terminalresize80x25present")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "backend") == "1"
        assert field(got, "has") == "1"
        assert field(got, "mode") == "80,25"
        assert field(got, "root") == "80,25"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "4000"
        assert field(got, "cb") == "80,25,4000"
        assert field(got, "capacity") == "8000"
        assert field(got, "first") == "4121"
        assert field(got, "tail") == "4232"


def test_translated_terminal_accepts_80x25_bw_alias_and_presents_through_split_runner() -> None:
    got = translated("terminalpresent80x25bw")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "backend") == "1"
        assert field(got, "has") == "1"
        assert field(got, "mode") == "80,25"
        assert field(got, "root") == "80,25"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "4000"
        assert field(got, "cb") == "80,25,4000"
        assert field(got, "capacity") == "8000"
        assert field(got, "first") == "4225"
        assert field(got, "tail") == "5737"


def test_translated_terminal_accepts_80x25_color_mode_and_presents_through_split_runner() -> None:
    got = translated("terminalpresent80x25color")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "backend") == "1"
        assert field(got, "has") == "1"
        assert field(got, "mode") == "80,25"
        assert field(got, "root") == "80,25"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "4000"
        assert field(got, "cb") == "80,25,4000"
        assert field(got, "capacity") == "8000"
        assert field(got, "first") == "4326"
        assert field(got, "tail") == "4f38"


def test_translated_terminal_rejects_unsupported_video_mode_without_rebinding_through_split_runner() -> None:
    got = translated("terminalbadmodepresent")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "backend") == "1"
        assert field(got, "has") == "1"
        assert field(got, "mode") == "40,25"
        assert field(got, "root") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "capacity") == "8000"
        assert field(got, "first") == "5827"
        assert field(got, "tail") == "5939"


def test_translated_terminal_present_callback_can_be_cleared_through_split_runner() -> None:
    got = translated("terminalpresentclear")
    if got is not None:
        assert field(got, "no_cb_has") == "0"
        assert field(got, "no_cb_presented") == "2000"
        assert field(got, "cb_has") == "1"
        assert field(got, "cb_presented") == "2000"
        assert field(got, "clear_has") == "0"
        assert field(got, "clear_presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "first") == "4529"
        assert field(got, "current") == "463b"


def test_translated_terminal_present_callback_can_be_replaced_through_split_runner() -> None:
    got = translated("terminalpresentreplace")
    if got is not None:
        assert field(got, "first_has") == "1"
        assert field(got, "first_presented") == "2000"
        assert field(got, "second_has") == "1"
        assert field(got, "second_presented") == "2000"
        assert field(got, "first_cb") == "40,25,2000"
        assert field(got, "first") == "472a"
        assert field(got, "second_cb") == "40,25,2000"
        assert field(got, "second") == "482b"
        assert field(got, "current") == "482b"


def test_translated_terminal_resize_to_80x50_presents_through_split_runner() -> None:
    got = translated("terminalresize80x50present")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "backend") == "1"
        assert field(got, "has") == "1"
        assert field(got, "mode") == "80,50"
        assert field(got, "root") == "80,50"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "8000"
        assert field(got, "cb") == "80,50,8000"
        assert field(got, "capacity") == "8000"
        assert field(got, "first") == "5723"
        assert field(got, "tail") == "4834"


def test_translated_terminal_resize_cycle_presents_through_split_runner() -> None:
    got = translated("terminalresizecyclepresent")
    if got is not None:
        assert field(got, "wide_ok") == "1"
        assert field(got, "narrow_ok") == "1"
        assert field(got, "backend") == "1"
        assert field(got, "has") == "1"
        assert field(got, "mode") == "40,25"
        assert field(got, "root") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "capacity") == "8000"
        assert field(got, "first") == "4e24"
        assert field(got, "tail") == "5236"
        assert field(got, "oldwide") == "5735"


def test_translated_notcurses_subwindow_present_keeps_outside_cells_through_split_runner() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("textsubwindowpresent")
    if got is not None:
        assert field(got, "origin") == "3,5"
        assert field(got, "rows") == "4"
        assert field(got, "cols") == "16"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "outside") == "216d"
        assert field(got, "title") == cells("SUB             ", 0x1E)
        assert field(got, "field") == cells("Song: ", 0x2A) + cells("DEMO      ", 0x4C)


def test_translated_notcurses_subwindow_redraw_replaces_only_window_region_through_split_runner() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("textsubwindowredraw")
    if got is not None:
        assert field(got, "before") == cells("OLD             ", 0x1E)
        assert field(got, "after") == cells("NEW             ", 0x5A)
        assert field(got, "cleared") == cells("                ", 0x03)
        assert field(got, "outside") == "3f6e"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"


def test_translated_notcurses_subwindow_at_parent_edge_has_zero_size_and_noops_through_split_runner() -> None:
    got = translated("textsubwindowzeroedge")
    if got is not None:
        assert field(got, "origin") == "25,40"
        assert field(got, "rows") == "0"
        assert field(got, "cols") == "0"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "tail") == "542d"
        assert field(got, "before") == "0000542d"


def test_translated_notcurses_subwindow_clips_at_80x50_edges_through_split_runner() -> None:
    got = translated("textsubwindow80x50clip")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "origin") == "48,78"
        assert field(got, "rows") == "2"
        assert field(got, "cols") == "2"
        assert field(got, "presented") == "8000"
        assert field(got, "cb") == "80,50,8000"
        assert field(got, "inside") == "573c573c"
        assert field(got, "tail") == "573c"
        assert field(got, "clipped") == "0000"


def test_translated_notcurses_subwindow_at_80x50_parent_edge_has_zero_size_and_noops_through_split_runner() -> None:
    got = translated("textsubwindow80x50zeroedge")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "origin") == "50,80"
        assert field(got, "rows") == "0"
        assert field(got, "cols") == "0"
        assert field(got, "presented") == "8000"
        assert field(got, "cb") == "80,50,8000"
        assert field(got, "tail") == "595f"
        assert field(got, "before") == "0000595f"


def test_translated_text_cursor_clamps_when_subplane_resizes_through_split_runner() -> None:
    got = translated("textcursorresize")
    if got is not None:
        assert field(got, "rows") == "2"
        assert field(got, "cols") == "3"
        assert field(got, "before") == "1,2"
        assert field(got, "after") == "1,3"
        assert field(got, "cell") == "515d"
        assert field(got, "outside") == "0000"


def test_translated_text_cursor_clamps_and_putc_is_noop_after_zero_resize_through_split_runner() -> None:
    got = translated("textcursorresizezero")
    if got is not None:
        assert field(got, "rows") == "0"
        assert field(got, "cols") == "0"
        assert field(got, "before") == "0,0"
        assert field(got, "after") == "0,0"
        assert field(got, "origin") == "0000"
        assert field(got, "neighbor") == "0000"


def test_translated_text_cursor_clamps_and_putc_is_noop_after_80x50_zero_resize_through_split_runner() -> None:
    got = translated("textcursor80x50resizezero")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "rows") == "0"
        assert field(got, "cols") == "0"
        assert field(got, "before") == "0,0"
        assert field(got, "after") == "0,0"
        assert field(got, "tail") == "437a"
        assert field(got, "neighbor") == "0000437a"


def test_translated_notcurses_text_preserves_16_vga_attribute_values_through_split_runner() -> None:
    got = translated("textcolorattrs16")
    if got is not None:
        assert field(got, "mode") == "40,25"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "attrs") == "".join(f"{ord('A') + i:02x}{((i << 4) | i):02x}" for i in range(16))
        assert field(got, "empty") == "0000"


def test_translated_text_cell_digest_uses_same_raw_and_screen_facade_path() -> None:
    got = translated("textcelldigest")
    if got is not None:
        assert field(got, "bytes") == "2000"
        assert field(got, "raw_checksum") == field(got, "screen_checksum")
        assert int(field(got, "raw_checksum")) != 0
        assert field(got, "raw_nonblank") == "3"
        assert field(got, "screen_nonblank") == "3"
        assert field(got, "first") == "411e422f"
        assert field(got, "tail") == "433a"


def test_translated_runtime_text_digest_uses_same_raw_and_runtime_facade_path() -> None:
    got = translated("runtimetextdigest")
    if got is not None:
        assert field(got, "mode") == "80,25"
        assert field(got, "bytes") == "4000"
        assert field(got, "raw_checksum") == field(got, "runtime_checksum")
        assert int(field(got, "raw_checksum")) != 0
        assert field(got, "raw_nonblank") == "3"
        assert field(got, "runtime_nonblank") == "3"
        assert field(got, "first") == "521a"
        assert field(got, "mid") == "552b"
        assert field(got, "tail") == "4e3c"


def test_translated_runtime_present_digest_uses_same_raw_runtime_and_callback_path() -> None:
    got = translated("runtimepresentdigest")
    if got is not None:
        assert field(got, "presented") == "4000"
        assert field(got, "cb") == "80,25,4000"
        assert field(got, "raw_checksum") == field(got, "runtime_checksum")
        assert field(got, "raw_checksum") == field(got, "cb_checksum")
        assert int(field(got, "raw_checksum")) != 0
        assert field(got, "raw_nonblank") == "3"
        assert field(got, "runtime_nonblank") == "3"
        assert field(got, "cb_nonblank") == "3"
        assert field(got, "first") == "501d"
        assert field(got, "mid") == "432e"
        assert field(got, "tail") == "423f"


def test_python_text_cell_digest_matches_b800_cell_semantics() -> None:
    cells = bytearray(12)
    cells[0:6] = bytes([ord("A"), 0x1E, ord(" "), 0x2F, ord("B"), 0x3A])
    cells[10:12] = bytes([ord("C"), 0x4B])
    digest = text_cell_digest(cells)

    assert digest["bytes"] == 12
    assert digest["checksum"] == text_cell_checksum(cells)
    assert digest["nonblank"] == text_cell_nonblank_count(cells)
    assert digest["nonblank"] == 3
    assert text_cell_nonblank_count(bytes([0, 0x07, ord(" "), 0x07, ord("X"), 0x1F])) == 1


def test_python_text_memory_digest_extracts_real_mode_vga_aperture() -> None:
    memory = bytearray(dos_physical_address(VGA_COLOR_TEXT_SEG) + text_mode_byte_count(80, 25))
    color_start = dos_physical_address(VGA_COLOR_TEXT_SEG)
    memory[color_start:color_start + 4] = bytes([ord("V"), 0x1E, ord("G"), 0x2F])
    memory[color_start + text_mode_byte_count(80, 25) - 2:color_start + text_mode_byte_count(80, 25)] = bytes([ord("A"), 0x3A])

    cells = text_memory_slice(bytes(memory), VGA_COLOR_TEXT_SEG, 80, 25)
    digest = text_memory_digest(bytes(memory), VGA_COLOR_TEXT_SEG, 80, 25)

    assert cells[:4] == bytes([ord("V"), 0x1E, ord("G"), 0x2F])
    assert cells[-2:] == bytes([ord("A"), 0x3A])
    assert digest["bytes"] == 4000
    assert digest["checksum"] == text_cell_checksum(cells)
    assert digest["nonblank"] == 3
    assert dos_physical_address(VGA_MONO_TEXT_SEG) == 0xB0000


def test_python_screen_present_digest_parser_matches_rewrite_diagnostic_shape() -> None:
    output = (
        "ignored\n"
        "Screen present: reason=status scope=full-screen bytes=4000 screen_bytes=4000 "
        "screen_checksum=123456 screen_nonblank=37 full=1 cols=80 rows=25 mode_ok=1 "
        "audio_frames=0 levels=3/4\n"
    )
    digest = parse_screen_present_digest(output, "status")

    assert digest == {
        "scope": "full-screen",
        "bytes": 4000,
        "checksum": 123456,
        "nonblank": 37,
        "presented": 4000,
        "full": 1,
        "cols": 80,
        "rows": 25,
        "mode_ok": 1,
        "audio_frames": 0,
        "left_level": 3,
        "right_level": 4,
    }


def test_python_screen_present_content_helper_accepts_status_only_full_screen_copy() -> None:
    digest = {
        "scope": "status-only",
        "bytes": 4000,
        "checksum": 123456,
        "nonblank": 37,
        "presented": 4000,
        "full": 1,
        "cols": 80,
        "rows": 25,
        "mode_ok": 1,
        "audio_frames": 16384,
        "left_level": 3,
        "right_level": 4,
    }

    assert_screen_present_content(digest, "status-only", expected_audio_frames=16384)


def test_python_screen_present_content_helper_rejects_stale_audio_frame_counter() -> None:
    digest = {
        "scope": "full-screen",
        "bytes": 4000,
        "checksum": 123456,
        "nonblank": 37,
        "presented": 4000,
        "full": 1,
        "cols": 80,
        "rows": 25,
        "mode_ok": 1,
        "audio_frames": 512,
        "left_level": 3,
        "right_level": 4,
    }

    try:
        assert_screen_present_content(digest, "full-screen", expected_audio_frames=1024)
    except AssertionError:
        pass
    else:
        raise AssertionError("stale audio-frame counter was not rejected")


def test_python_text_screen_geometry_helper_accepts_expected_mode_bytes() -> None:
    digest = {
        "scope": "full-screen",
        "bytes": 8000,
        "checksum": 123456,
        "nonblank": 37,
        "presented": 8000,
        "full": 1,
        "cols": 80,
        "rows": 50,
        "mode_ok": 1,
        "audio_frames": 0,
        "left_level": 3,
        "right_level": 4,
    }

    assert_text_screen_geometry(digest, 80, 50)


def test_python_text_screen_geometry_helper_rejects_byte_count_mismatch() -> None:
    digest = {
        "scope": "full-screen",
        "bytes": 4000,
        "checksum": 123456,
        "nonblank": 37,
        "presented": 4000,
        "full": 1,
        "cols": 80,
        "rows": 50,
        "mode_ok": 1,
        "audio_frames": 0,
        "left_level": 3,
        "right_level": 4,
    }

    try:
        assert_text_screen_geometry(digest, 80, 50)
    except AssertionError:
        pass
    else:
        raise AssertionError("text screen byte-count mismatch was not rejected")


def test_python_playback_pump_parser_matches_rewrite_diagnostic_shape() -> None:
    output = (
        "ignored\n"
        "Playback pump: blocks=32 frames=16384 accepted=65536 checksum=123456789 "
        "limit=1 source_end=0 stop=block-limit\n"
    )
    pump = parse_playback_pump(output)

    assert pump == {
        "blocks": 32,
        "frames": 16384,
        "accepted": 65536,
        "checksum": 123456789,
        "limit": 1,
        "source_end": 0,
        "stop": "block-limit",
    }


def test_python_playback_loop_parser_matches_rewrite_diagnostic_shape() -> None:
    output = (
        "ignored\n"
        "Playback loop: mode=playback policy=bounded-trial cadence=immediate "
        "max_blocks=32 frames/block=512\n"
    )

    assert parse_playback_loop(output) == {
        "mode": "playback",
        "policy": "bounded-trial",
        "cadence": "immediate",
        "max_blocks": 32,
        "frames_per_block": 512,
    }


def test_python_playback_loop_assertion_helper_rejects_wrong_policy() -> None:
    output = (
        "ignored\n"
        "Playback loop: mode=playback policy=timer-keyboard cadence=timer "
        "max_blocks=0 frames/block=1024\n"
        # inventory marker: frames/block={SB16_CONTINUOUS_BLOCK_FRAMES}
    )

    try:
        assert_playback_loop(output, "playback", "bounded-trial", "immediate", 32, 512)
    except AssertionError:
        pass
    else:
        raise AssertionError("playback loop policy mismatch was not rejected")


def test_python_decoder_progress_parser_matches_rewrite_diagnostic_shape() -> None:
    output = (
        "ignored\n"
        "Decoder progress: block=63/128 order=2 pattern=1 row=0 channel=0 "
        "tick=0/1 speed=1 tempo=125 ended=1 loop=0\n"
    )

    assert parse_decoder_progress(output) == {
        "block": 63,
        "total_blocks": 128,
        "order": 2,
        "pattern": 1,
        "row": 0,
        "channel": 0,
        "tick": 0,
        "speed": 1,
        "tempo": 125,
        "ended": 1,
        "loop": 0,
    }


def test_python_decoder_progress_assertion_helper_rejects_wrong_row() -> None:
    output = (
        "ignored\n"
        "Decoder progress: block=32/128 order=1 pattern=0 row=32 channel=0 "
        "tick=0/1 speed=1 tempo=125 ended=0 loop=0\n"
    )

    try:
        assert_decoder_progress(output, 32, 128, 1, 0, 31, 0, 0, 1, 125, 0, 0)
    except AssertionError:
        pass
    else:
        raise AssertionError("decoder progress row mismatch was not rejected")


def test_python_decoder_progress_block_helper_accepts_prefix_check() -> None:
    output = (
        "ignored\n"
        "Decoder progress: block=32/7680 order=0 pattern=0 row=5 channel=0 "
        "tick=2/6 speed=6 tempo=125 ended=0 loop=0\n"
    )

    progress = assert_decoder_progress_block(output, 32, 7680)
    assert progress["row"] == 5


def test_python_module_loaded_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nModule: aryx.s3m\n"

    assert parse_module_loaded(output) == {"name": "aryx.s3m"}
    assert assert_module_loaded(output, "aryx.s3m") == {"name": "aryx.s3m"}


def test_python_module_loaded_assertion_helper_rejects_wrong_case() -> None:
    output = "ignored\nModule: aryx.s3m\n"

    try:
        assert_module_loaded(output, "ARYX.S3M")
    except AssertionError:
        pass
    else:
        raise AssertionError("module-loaded case mismatch was not rejected")


def test_python_module_not_loaded_helper_rejects_present_module() -> None:
    assert_module_not_loaded("ignored\n", "BAD.MOD")

    try:
        assert_module_not_loaded("ignored\nModule: BAD.MOD\n", "BAD.MOD")
    except AssertionError:
        pass
    else:
        raise AssertionError("module-not-loaded helper did not reject present module")


def test_python_module_size_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nSize: 20800 bytes\n"

    assert parse_module_size(output) == {"size": 20800}
    assert assert_module_size(output, 20800) == {"size": 20800}


def test_python_module_size_assertion_helper_rejects_wrong_size() -> None:
    output = "ignored\nSize: 20800 bytes\n"

    try:
        assert_module_size(output, 24577)
    except AssertionError:
        pass
    else:
        raise AssertionError("module-size mismatch was not rejected")


def test_python_module_loader_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nLoader: s3m_module (Scream Tracker 3)\n"

    assert parse_module_loader(output) == {"loader": "s3m_module (Scream Tracker 3)"}
    assert assert_module_loader(output, "s3m_module (Scream Tracker 3)") == {"loader": "s3m_module (Scream Tracker 3)"}


def test_python_module_loader_assertion_helper_rejects_wrong_loader() -> None:
    output = "ignored\nLoader: s3m_module (Scream Tracker 3)\n"

    try:
        assert_module_loader(output, "mod_n_t_module (ProTracker/NoiseTracker MOD)")
    except AssertionError:
        pass
    else:
        raise AssertionError("module-loader mismatch was not rejected")


def test_python_decoder_handoff_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nDecoder handoff: external tracker -> SB16 PCM seam.\n"

    assert parse_decoder_handoff(output) == {"handoff": "external tracker -> SB16 PCM seam."}
    assert assert_decoder_handoff(output, "external tracker -> SB16 PCM seam.") == {"handoff": "external tracker -> SB16 PCM seam."}


def test_python_decoder_handoff_assertion_helper_rejects_wrong_handoff() -> None:
    output = "ignored\nDecoder handoff: external tracker -> SB16 PCM seam.\n"

    try:
        assert_decoder_handoff(output, "project INR -> SB16 PCM.")
    except AssertionError:
        pass
    else:
        raise AssertionError("decoder-handoff mismatch was not rejected")


def test_python_decoder_route_absent_helper_rejects_present_route() -> None:
    assert_decoder_route_absent("ignored\n", 1, "project-owned")

    try:
        assert_decoder_route_absent("ignored\nDecoder route: id=1 name=project-owned\n", 1, "project-owned")
    except AssertionError:
        pass
    else:
        raise AssertionError("decoder-route-absent helper did not reject present route")


def test_python_decoder_handoff_absent_helper_rejects_present_handoff() -> None:
    assert_decoder_handoff_absent("ignored\n", "project INR -> SB16 PCM.")

    try:
        assert_decoder_handoff_absent("ignored\nDecoder handoff: project INR -> SB16 PCM.\n", "project INR -> SB16 PCM.")
    except AssertionError:
        pass
    else:
        raise AssertionError("decoder-handoff-absent helper did not reject present handoff")


def test_python_module_type_tag_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nModule type tag: 204D3353\n"

    assert parse_module_type_tag(output) == {"tag": "204D3353"}
    assert assert_module_type_tag(output, "204D3353") == {"tag": "204D3353"}


def test_python_module_type_tag_assertion_helper_rejects_wrong_tag() -> None:
    output = "ignored\nModule type tag: 204D3353\n"

    try:
        assert_module_type_tag(output, "39363645")
    except AssertionError:
        pass
    else:
        raise AssertionError("module-type-tag mismatch was not rejected")


def test_python_module_title_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nTitle: SMOKE S3M\n"

    assert parse_module_title(output) == {"title": "SMOKE S3M"}
    assert assert_module_title(output, "SMOKE S3M") == {"title": "SMOKE S3M"}


def test_python_module_title_assertion_helper_rejects_wrong_title() -> None:
    output = "ignored\nTitle: SMOKE S3M\n"

    try:
        assert_module_title(output, "SMOKE MOD")
    except AssertionError:
        pass
    else:
        raise AssertionError("module-title mismatch was not rejected")


def test_python_unsupported_module_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nUnsupported module type: BAD.XYZ\n"

    assert parse_unsupported_module(output) == {"name": "BAD.XYZ"}
    assert assert_unsupported_module(output, "BAD.XYZ") == {"name": "BAD.XYZ"}


def test_python_unsupported_module_assertion_helper_rejects_wrong_name() -> None:
    output = "ignored\nUnsupported module type: BAD.XYZ\n"

    try:
        assert_unsupported_module(output, "BAD.MOD")
    except AssertionError:
        pass
    else:
        raise AssertionError("unsupported-module name mismatch was not rejected")


def test_python_playback_output_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nPlayback output: SB16 16-bit stereo hardware wrapper enabled.\n"

    assert parse_playback_output(output) == {"output": "SB16 16-bit stereo hardware wrapper enabled."}
    assert assert_playback_output(output, "SB16 16-bit stereo hardware wrapper enabled.") == {
        "output": "SB16 16-bit stereo hardware wrapper enabled."
    }


def test_python_playback_output_assertion_helper_rejects_wrong_output() -> None:
    output = "ignored\nPlayback output: SB16 16-bit stereo hardware wrapper enabled.\n"

    try:
        assert_playback_output(output, "other")
    except AssertionError:
        pass
    else:
        raise AssertionError("playback-output mismatch was not rejected")


def test_python_playback_disabled_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nPlayback disabled: SB16 not detected\n"

    assert parse_playback_disabled(output) == {"reason": "SB16 not detected"}
    assert assert_playback_disabled(output, "SB16 not detected") == {"reason": "SB16 not detected"}


def test_python_playback_disabled_assertion_helper_rejects_wrong_reason() -> None:
    output = "ignored\nPlayback disabled: SB16 not detected\n"

    try:
        assert_playback_disabled(output, "no module")
    except AssertionError:
        pass
    else:
        raise AssertionError("playback-disabled reason mismatch was not rejected")


def test_python_ffi_marker_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nFFI: 0001\n"

    assert parse_ffi_marker(output) == {"marker": "0001"}
    assert assert_ffi_marker(output, "0001") == {"marker": "0001"}


def test_python_ffi_marker_assertion_helper_rejects_wrong_marker() -> None:
    output = "ignored\nFFI: 0001\n"

    try:
        assert_ffi_marker(output, "0002")
    except AssertionError:
        pass
    else:
        raise AssertionError("FFI marker mismatch was not rejected")


def test_python_orders_channels_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nOrders: 129 Channels: 4\n"

    assert parse_orders_channels(output) == {"orders": 129, "channels": 4}
    assert assert_orders_channels(output, 129, 4) == {"orders": 129, "channels": 4}


def test_python_orders_channels_assertion_helper_rejects_wrong_channel_count() -> None:
    output = "ignored\nOrders: 129 Channels: 4\n"

    try:
        assert_orders_channels(output, 129, 1)
    except AssertionError:
        pass
    else:
        raise AssertionError("orders/channels mismatch was not rejected")


def test_python_help_and_capability_helpers_accept_expected_text() -> None:
    output = (
        "Usage: IPLAY [Switches] [FileName.Ext|@FileList.Ext]\n"
        "Supported by this DOS hardware build: MOD NST S3M STM 669 MTM PSM FAR ULT WOW OKT OCT XM IT PTM AMS DBM DMF MDL DSM MED IMF J2B INR\n"
        "Audio driver scope: SB16 16-bit stereo only.\n"
        "Text backend: VGA color/BW text memory at B800:0000/B000:0000.\n"
        "Audio backend: SB16 16-bit stereo hardware wrapper, SDL-compatible callback boundary.\n"
    )

    assert_help_usage(output)
    assert_supported_dos_formats(output)
    assert_sb16_audio_scope(output)
    assert_text_backend(output)
    assert_text_backend_memory(output)
    assert_sdl_compatible_audio_backend(output)


def test_python_help_usage_helper_rejects_missing_usage() -> None:
    try:
        assert_help_usage("ignored\n")
    except AssertionError:
        pass
    else:
        raise AssertionError("missing help usage text was not rejected")


def test_python_decoder_geometry_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nDecoder geometry: orders=3 rows/order=64 restart=0 speed=6 tempo=125 channels=4\n"

    assert parse_decoder_geometry(output) == {
        "orders": 3,
        "rows_per_order": 64,
        "restart": 0,
        "speed": 6,
        "tempo": 125,
        "channels": 4,
    }


def test_python_decoder_geometry_assertion_helper_rejects_wrong_channel_count() -> None:
    output = "ignored\nDecoder geometry: orders=1 rows/order=64 restart=0 speed=1 tempo=125 channels=1\n"

    try:
        assert_decoder_geometry(output, 1, 64, 0, 1, 125, 4)
    except AssertionError:
        pass
    else:
        raise AssertionError("decoder geometry channel-count mismatch was not rejected")


def test_python_decoder_event_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nDecoder event: period=855 note=1 octave=1 instrument=1 volume=64 effect=12 param=127\n"

    assert parse_decoder_events(output) == [{
        "period": 855,
        "note": 1,
        "octave": 1,
        "instrument": 1,
        "volume": 64,
        "effect": 12,
        "param": 127,
    }]


def test_python_decoder_event_assertion_helper_rejects_wrong_effect() -> None:
    output = "ignored\nDecoder event: period=855 note=1 octave=1 instrument=1 volume=64 effect=12 param=127\n"

    try:
        assert_decoder_event(output, 855, 1, 1, 1, 64, 15, 127)
    except AssertionError:
        pass
    else:
        raise AssertionError("decoder event effect mismatch was not rejected")


def test_python_decoder_voice_parser_matches_rewrite_diagnostic_shape() -> None:
    output = (
        "ignored\n"
        "Decoder voice: active=1 period=855 note=1 octave=1 instrument=1 volume=64 "
        "sample_len=4 sample_vol=64 loop=0/2 data=6204\n"
    )

    assert parse_decoder_voices(output) == [{
        "active": 1,
        "period": 855,
        "note": 1,
        "octave": 1,
        "instrument": 1,
        "volume": 64,
        "sample_len": 4,
        "sample_vol": 64,
        "loop_start": 0,
        "loop_len": 2,
        "data": 6204,
    }]


def test_python_decoder_voice_assertion_helper_rejects_wrong_loop_length() -> None:
    output = (
        "ignored\n"
        "Decoder voice: active=1 period=855 note=1 octave=1 instrument=1 volume=64 "
        "sample_len=4 sample_vol=64 loop=0/2 data=6204\n"
    )

    try:
        assert_decoder_voice(output, 1, 855, 1, 1, 1, 64, 4, 64, 0, 3, 6204)
    except AssertionError:
        pass
    else:
        raise AssertionError("decoder voice loop-length mismatch was not rejected")


def test_python_decoder_route_parser_matches_rewrite_diagnostic_shape() -> None:
    output = "ignored\nDecoder route: id=0 name=external-library\n"

    assert parse_decoder_route(output) == {"id": 0, "name": "external-library"}


def test_python_decoder_route_parser_rejects_missing_diagnostic() -> None:
    try:
        parse_decoder_route("ignored\n")
    except AssertionError:
        pass
    else:
        raise AssertionError("missing decoder route diagnostic was not rejected")


def test_python_pcm_source_parser_matches_rewrite_diagnostic_shape() -> None:
    output = (
        "ignored\n"
        "PCM source: s3m_module seed=23 truncated=0 input=memory renderer=e "
        "route=0 provider=native-preview hook_provider=none stream_start=352\n"
    )

    assert parse_pcm_source(output) == {
        "source": "s3m_module",
        "seed": 23,
        "truncated": 0,
        "input": "memory",
        "renderer": "e",
        "route": 0,
        "provider": "native-preview",
        "hook_provider": "none",
        "stream_start": 352,
    }


def test_python_pcm_source_parser_rejects_missing_diagnostic() -> None:
    try:
        parse_pcm_source("ignored\n")
    except AssertionError:
        pass
    else:
        raise AssertionError("missing PCM source diagnostic was not rejected")


def test_python_decoder_route_assertion_helper_accepts_expected_route() -> None:
    output = "ignored\nDecoder route: id=1 name=project-owned\n"

    assert assert_decoder_route(output, 1, "project-owned") == {"id": 1, "name": "project-owned"}


def test_python_decoder_route_assertion_helper_rejects_wrong_route_id() -> None:
    output = "ignored\nDecoder route: id=0 name=external-library\n"

    try:
        assert_decoder_route(output, 1, "project-owned")
    except AssertionError:
        pass
    else:
        raise AssertionError("decoder route assertion mismatch was not rejected")


def test_python_pcm_source_route_assertion_helper_accepts_expected_source() -> None:
    output = (
        "ignored\n"
        "PCM source: inr_module seed=7 truncated=0 input=memory renderer=p "
        "route=1 provider=native hook_provider=none stream_start=0\n"
    )

    pcm = assert_pcm_source_route(output, 1, "p", "native", source="inr_module", truncated=0, input_kind="memory", hook_provider="none", stream_start=0)
    assert pcm["source"] == "inr_module"


def test_python_pcm_source_route_assertion_helper_rejects_wrong_provider() -> None:
    output = (
        "ignored\n"
        "PCM source: s3m_module seed=23 truncated=0 input=memory renderer=e "
        "route=0 provider=native-preview hook_provider=none stream_start=352\n"
    )

    try:
        assert_pcm_source_route(output, 0, "e", "dos-fallback")
    except AssertionError:
        pass
    else:
        raise AssertionError("PCM source route assertion mismatch was not rejected")


def test_python_pcm_source_route_assertion_helper_rejects_wrong_input_kind() -> None:
    output = (
        "ignored\n"
        "PCM source: s3m_module seed=23 truncated=1 input=file-path renderer=e "
        "route=0 provider=dos-fallback hook_provider=none stream_start=107\n"
    )

    try:
        assert_pcm_source_route(output, 0, "e", "dos-fallback", truncated=1, input_kind="memory", stream_start=107)
    except AssertionError:
        pass
    else:
        raise AssertionError("PCM source input-kind mismatch was not rejected")


def test_python_pcm_source_route_assertion_helper_rejects_wrong_hook_provider() -> None:
    output = (
        "ignored\n"
        "PCM source: s3m_module seed=23 truncated=0 input=memory renderer=e "
        "route=0 provider=native-preview hook_provider=none stream_start=352\n"
    )

    try:
        assert_pcm_source_route(output, 0, "e", "native-preview", hook_provider="libmodplug")
    except AssertionError:
        pass
    else:
        raise AssertionError("PCM source hook-provider mismatch was not rejected")


def test_python_sb16_stereo_block_accounting_helper_accepts_expected_bytes() -> None:
    assert_sb16_stereo_block_accounting(32, 32 * SB16_BOUNDED_BLOCK_FRAMES, 32 * SB16_BOUNDED_BLOCK_BYTES, SB16_BOUNDED_BLOCK_FRAMES)


def test_python_sb16_stereo_block_accounting_helper_rejects_frame_count_mismatch() -> None:
    try:
        assert_sb16_stereo_block_accounting(32, 31 * SB16_BOUNDED_BLOCK_FRAMES, 32 * SB16_BOUNDED_BLOCK_BYTES, SB16_BOUNDED_BLOCK_FRAMES)
    except AssertionError:
        pass
    else:
        raise AssertionError("SB16 block frame-count mismatch was not rejected")


def test_python_sb16_stereo_frame_byte_helper_rejects_unaligned_byte_count() -> None:
    try:
        assert_sb16_stereo_frame_bytes(1, 2)
    except AssertionError:
        pass
    else:
        raise AssertionError("SB16 unaligned stereo byte count was not rejected")


def test_python_playback_pump_sb16_helper_rejects_accepted_byte_mismatch() -> None:
    pump = {
        "blocks": 1,
        "frames": SB16_CONTINUOUS_BLOCK_FRAMES,
        "accepted": SB16_CONTINUOUS_BLOCK_BYTES - 2,
        "checksum": 123456789,
        "limit": 0,
        "source_end": 1,
        "stop": "source-end",
    }

    try:
        assert_playback_pump_sb16_stereo(pump, 1, SB16_CONTINUOUS_BLOCK_FRAMES)
    except AssertionError:
        pass
    else:
        raise AssertionError("SB16 playback-pump accepted-byte mismatch was not rejected")


def test_python_playback_pump_sb16_helper_accepts_parsed_bounded_pump() -> None:
    pump = {
        "blocks": 32,
        "frames": 32 * SB16_BOUNDED_BLOCK_FRAMES,
        "accepted": 32 * SB16_BOUNDED_BLOCK_BYTES,
        "checksum": 123456789,
        "limit": 1,
        "source_end": 0,
        "stop": "block-limit",
    }

    assert_playback_pump_sb16_stereo(pump, 32, SB16_BOUNDED_BLOCK_FRAMES)


def test_python_playback_pump_sb16_helper_rejects_block_count_mismatch() -> None:
    pump = {
        "blocks": 31,
        "frames": 32 * SB16_BOUNDED_BLOCK_FRAMES,
        "accepted": 32 * SB16_BOUNDED_BLOCK_BYTES,
        "checksum": 123456789,
        "limit": 1,
        "source_end": 0,
        "stop": "block-limit",
    }

    try:
        assert_playback_pump_sb16_stereo(pump, 32, SB16_BOUNDED_BLOCK_FRAMES)
    except AssertionError:
        pass
    else:
        raise AssertionError("SB16 playback-pump block-count mismatch was not rejected")


def test_python_playback_pump_stop_state_helper_accepts_source_end() -> None:
    pump = {
        "blocks": 1,
        "frames": SB16_CONTINUOUS_BLOCK_FRAMES,
        "accepted": SB16_CONTINUOUS_BLOCK_BYTES,
        "checksum": 123456789,
        "limit": 0,
        "source_end": 1,
        "stop": "source-end",
    }

    assert_playback_pump_stop_state(pump, 0, 1, "source-end")


def test_python_playback_pump_stop_state_helper_rejects_wrong_stop_reason() -> None:
    pump = {
        "blocks": 32,
        "frames": 32 * SB16_BOUNDED_BLOCK_FRAMES,
        "accepted": 32 * SB16_BOUNDED_BLOCK_BYTES,
        "checksum": 123456789,
        "limit": 1,
        "source_end": 0,
        "stop": "block-limit",
    }

    try:
        assert_playback_pump_stop_state(pump, 1, 0, "source-end")
    except AssertionError:
        pass
    else:
        raise AssertionError("playback-pump stop reason mismatch was not rejected")


def test_python_text_memory_to_screen_present_comparison_helper_matches_digest_fields() -> None:
    memory = bytearray(dos_physical_address(VGA_COLOR_TEXT_SEG) + text_mode_byte_count(80, 25))
    start = dos_physical_address(VGA_COLOR_TEXT_SEG)
    memory[start:start + 4] = bytes([ord("M"), 0x1E, ord("P"), 0x2F])
    digest = text_memory_digest(bytes(memory), VGA_COLOR_TEXT_SEG, 80, 25)
    output = (
        f"Screen present: reason=status scope=full-screen bytes=4000 screen_bytes={digest['bytes']} "
        f"screen_checksum={digest['checksum']} screen_nonblank={digest['nonblank']} full=1 "
        "cols=80 rows=25 mode_ok=1 audio_frames=0 levels=0/0\n"
    )

    assert_text_memory_matches_screen_present(bytes(memory), output, "status", VGA_COLOR_TEXT_SEG, 80, 25)

    status_only_output = output.replace("reason=status scope=full-screen", "reason=post-playback-status scope=status-only")
    assert_text_memory_matches_screen_present(
        bytes(memory),
        status_only_output,
        "post-playback-status",
        VGA_COLOR_TEXT_SEG,
        80,
        25,
        expected_scope="status-only",
    )


def test_python_text_memory_to_screen_present_comparison_helper_rejects_digest_mismatch() -> None:
    memory = bytearray(dos_physical_address(VGA_COLOR_TEXT_SEG) + text_mode_byte_count(80, 25))
    start = dos_physical_address(VGA_COLOR_TEXT_SEG)
    memory[start:start + 2] = bytes([ord("X"), 0x1E])
    digest = text_memory_digest(bytes(memory), VGA_COLOR_TEXT_SEG, 80, 25)
    wrong_checksum = (digest["checksum"] + 1) & 0xFFFFFFFF
    output = (
        f"Screen present: reason=status scope=full-screen bytes=4000 screen_bytes={digest['bytes']} "
        f"screen_checksum={wrong_checksum} screen_nonblank={digest['nonblank']} full=1 "
        "cols=80 rows=25 mode_ok=1 audio_frames=0 levels=0/0\n"
    )

    try:
        assert_text_memory_matches_screen_present(bytes(memory), output, "status", VGA_COLOR_TEXT_SEG, 80, 25)
    except AssertionError:
        pass
    else:
        raise AssertionError("digest mismatch was not rejected")


def test_python_text_memory_to_screen_present_comparison_helper_rejects_scope_mismatch() -> None:
    memory = bytearray(dos_physical_address(VGA_COLOR_TEXT_SEG) + text_mode_byte_count(80, 25))
    digest = text_memory_digest(bytes(memory), VGA_COLOR_TEXT_SEG, 80, 25)
    output = (
        f"Screen present: reason=post-playback-status scope=status-only bytes=4000 screen_bytes={digest['bytes']} "
        f"screen_checksum={digest['checksum']} screen_nonblank={digest['nonblank']} full=1 "
        "cols=80 rows=25 mode_ok=1 audio_frames=0 levels=0/0\n"
    )

    try:
        assert_text_memory_matches_screen_present(bytes(memory), output, "post-playback-status", VGA_COLOR_TEXT_SEG, 80, 25)
    except AssertionError:
        pass
    else:
        raise AssertionError("screen-present scope mismatch was not rejected")


def test_python_player_hw_text_digest_parser_matches_runner_output_shape() -> None:
    output = (
        "audio_copies=16 audio_bytes=128 text_copies=1 text_seg=b800 text_off=0000 "
        "text_bytes=4000 text_checksum=987654 text_nonblank=121 text_first=2007\n"
    )
    digest = parse_player_hw_text_digest(output)

    assert digest == {
        "copies": 1,
        "segment": VGA_COLOR_TEXT_SEG,
        "offset": 0,
        "bytes": 4000,
        "checksum": 987654,
        "nonblank": 121,
    }


def test_python_player_hw_audio_digest_parser_matches_runner_output_shape() -> None:
    output = (
        "audio_copies=16 audio_bytes=128 audio_checksum=7456 audio_first=006d audio_tail=007c "
        "text_copies=1 text_seg=b800 text_off=0000 text_bytes=4000\n"
    )
    digest = parse_player_hw_audio_digest(output)

    assert digest == {
        "copies": 16,
        "bytes": 128,
        "checksum": 7456,
        "first": 0x006D,
        "tail": 0x007C,
    }


def test_python_text_memory_to_player_hw_text_comparison_helper_matches_digest_fields() -> None:
    memory = bytearray(dos_physical_address(VGA_COLOR_TEXT_SEG) + text_mode_byte_count(80, 25))
    start = dos_physical_address(VGA_COLOR_TEXT_SEG)
    memory[start:start + 4] = bytes([ord("H"), 0x1E, ord("W"), 0x2F])
    digest = text_memory_digest(bytes(memory), VGA_COLOR_TEXT_SEG, 80, 25)
    output = (
        f"text_copies=1 text_seg=b800 text_off=0000 text_bytes={digest['bytes']} "
        f"text_checksum={digest['checksum']} text_nonblank={digest['nonblank']} text_first=481e\n"
    )

    assert_text_memory_matches_player_hw_text(bytes(memory), output, VGA_COLOR_TEXT_SEG, 80, 25)


def test_python_text_memory_to_player_hw_text_comparison_helper_rejects_digest_mismatch() -> None:
    memory = bytearray(dos_physical_address(VGA_MONO_TEXT_SEG) + text_mode_byte_count(40, 25))
    start = dos_physical_address(VGA_MONO_TEXT_SEG)
    memory[start:start + 2] = bytes([ord("B"), 0x70])
    digest = text_memory_digest(bytes(memory), VGA_MONO_TEXT_SEG, 40, 25)
    wrong_nonblank = digest["nonblank"] + 1
    output = (
        f"text_copies=1 text_seg=b000 text_off=0000 text_bytes={digest['bytes']} "
        f"text_checksum={digest['checksum']} text_nonblank={wrong_nonblank} text_first=4270\n"
    )

    try:
        assert_text_memory_matches_player_hw_text(bytes(memory), output, VGA_MONO_TEXT_SEG, 40, 25)
    except AssertionError:
        pass
    else:
        raise AssertionError("player hardware text digest mismatch was not rejected")


def test_translated_runtime_present_callback_can_be_replaced_and_cleared_through_split_runner() -> None:
    got = translated("runtimepresentclear")
    if got is not None:
        assert field(got, "first_has") == "1"
        assert field(got, "first_presented") == "2000"
        assert field(got, "second_has") == "1"
        assert field(got, "second_presented") == "2000"
        assert field(got, "clear_has") == "0"
        assert field(got, "clear_presented") == "2000"
        assert field(got, "first_cb") == "40,25,2000"
        assert field(got, "first") == "521d"
        assert field(got, "second_cb") == "40,25,2000"
        assert field(got, "second") == "532e"
        assert field(got, "current") == "543f"


def test_translated_runtime_rejects_unsupported_video_mode_without_rebinding_through_split_runner() -> None:
    got = translated("runtimebadmodepresent")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "flag") == "0"
        assert field(got, "status") == "Video mode rejected"
        assert field(got, "token") == "rejected"
        assert field(got, "mode") == "40,25"
        assert field(got, "root") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "first") == "561c"
        assert field(got, "tail") == "582d"


def test_translated_runtime_rejects_unsupported_resize_without_rebinding_through_split_runner() -> None:
    got = translated("runtimeresizebadpresent")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "flag") == "0"
        assert field(got, "status") == "Video mode rejected"
        assert field(got, "token") == "rejected"
        assert field(got, "mode") == "40,25"
        assert field(got, "root") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "supported") == "0"
        assert field(got, "first") == "591f"
        assert field(got, "tail") == "512e"


def test_translated_runtime_rejects_resize_that_exceeds_capacity_through_split_runner() -> None:
    got = translated("runtimeresizecapacitypresent")
    if got is not None:
        assert field(got, "ok") == "0"
        assert field(got, "flag") == "0"
        assert field(got, "status") == "Video mode rejected"
        assert field(got, "token") == "rejected"
        assert field(got, "mode") == "40,25"
        assert field(got, "root") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "capacity") == "4000"
        assert field(got, "first") == "431e"
        assert field(got, "tail") == "522f"
        assert field(got, "fits80x25") == "1"
        assert field(got, "fits80x50") == "0"


def test_translated_runtime_accepts_80x25_bw_alias_and_presents_through_split_runner() -> None:
    got = translated("runtimepresent80x25bw")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "status") == "Video mode accepted"
        assert field(got, "token") == "accepted"
        assert field(got, "mode") == "80,25"
        assert field(got, "root") == "80,25"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "4000"
        assert field(got, "cb") == "80,25,4000"
        assert field(got, "first") == "4218"
        assert field(got, "tail") == "5771"


def test_translated_runtime_accepts_80x25_color_mode_and_presents_through_split_runner() -> None:
    got = translated("runtimepresent80x25color")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "status") == "Video mode accepted"
        assert field(got, "token") == "accepted"
        assert field(got, "mode") == "80,25"
        assert field(got, "root") == "80,25"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "4000"
        assert field(got, "cb") == "80,25,4000"
        assert field(got, "first") == "4319"
        assert field(got, "tail") == "4f72"


def test_translated_runtime_resize_to_80x50_rebinds_and_presents_through_split_runner() -> None:
    got = translated("runtimeresize80x50present")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "status") == "Video mode accepted"
        assert field(got, "token") == "accepted"
        assert field(got, "mode") == "80,50"
        assert field(got, "root") == "80,50"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "8000"
        assert field(got, "cb") == "80,50,8000"
        assert field(got, "supported") == "1"
        assert field(got, "first") == "411a"
        assert field(got, "tail") == "422b"


def test_translated_runtime_resize_to_80x25_rebinds_and_presents_through_split_runner() -> None:
    got = translated("runtimeresize80x25present")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "status") == "Video mode accepted"
        assert field(got, "token") == "accepted"
        assert field(got, "mode") == "80,25"
        assert field(got, "root") == "80,25"
        assert field(got, "stride") == "80"
        assert field(got, "presented") == "4000"
        assert field(got, "cb") == "80,25,4000"
        assert field(got, "supported") == "1"
        assert field(got, "first") == "531b"
        assert field(got, "tail") == "5a2c"


def test_translated_runtime_resize_cycle_shrinks_back_to_40x25_through_split_runner() -> None:
    got = translated("runtimeresizecyclepresent")
    if got is not None:
        assert field(got, "wide_ok") == "1"
        assert field(got, "narrow_ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "status") == "Video mode accepted"
        assert field(got, "token") == "accepted"
        assert field(got, "mode") == "40,25"
        assert field(got, "root") == "40,25"
        assert field(got, "stride") == "40"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "first") == "433c"
        assert field(got, "tail") == "444d"
        assert field(got, "old_wide_tail") == "571a"


def test_translated_runtime_subwindow_clips_after_80x50_resize_through_split_runner() -> None:
    got = translated("runtimesubwindow80x50clip")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "origin") == "48,78"
        assert field(got, "rows") == "2"
        assert field(got, "cols") == "2"
        assert field(got, "presented") == "8000"
        assert field(got, "cb") == "80,50,8000"
        assert field(got, "inside") == "525e525e"
        assert field(got, "tail") == "525e"
        assert field(got, "clipped") == "0000"


def test_translated_runtime_cursor_clamps_and_putc_is_noop_after_80x50_zero_resize_through_split_runner() -> None:
    got = translated("runtimecursor80x50resizezero")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "rows") == "0"
        assert field(got, "cols") == "0"
        assert field(got, "before") == "0,0"
        assert field(got, "after") == "0,0"
        assert field(got, "presented") == "8000"
        assert field(got, "cb") == "80,50,8000"
        assert field(got, "tail") == "4d6d"
        assert field(got, "neighbor") == "00004d6d"


def test_translated_runtime_subwindow_clips_after_resize_cycle_through_split_runner() -> None:
    got = translated("runtimesubwindowresizecycleclip")
    if got is not None:
        assert field(got, "wide_ok") == "1"
        assert field(got, "narrow_ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "origin") == "23,38"
        assert field(got, "rows") == "2"
        assert field(got, "cols") == "2"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "inside") == "536f536f"
        assert field(got, "tail") == "536f"
        assert field(got, "clipped") == "0000"
        assert field(got, "old_wide_tail") == "571a"


def test_translated_runtime_subwindow_at_parent_edge_has_zero_size_and_noops_through_split_runner() -> None:
    got = translated("runtimesubwindowzeroedge")
    if got is not None:
        assert field(got, "flag") == "1"
        assert field(got, "origin") == "25,40"
        assert field(got, "rows") == "0"
        assert field(got, "cols") == "0"
        assert field(got, "presented") == "2000"
        assert field(got, "cb") == "40,25,2000"
        assert field(got, "tail") == "553d"
        assert field(got, "before") == "0000553d"


def test_translated_runtime_subwindow_at_80x50_parent_edge_has_zero_size_and_noops_through_split_runner() -> None:
    got = translated("runtimesubwindow80x50zeroedge")
    if got is not None:
        assert field(got, "ok") == "1"
        assert field(got, "flag") == "1"
        assert field(got, "origin") == "50,80"
        assert field(got, "rows") == "0"
        assert field(got, "cols") == "0"
        assert field(got, "presented") == "8000"
        assert field(got, "cb") == "80,50,8000"
        assert field(got, "tail") == "564e"
        assert field(got, "before") == "0000564e"


def test_translated_ncplane_fill_rect_clips_to_subplane() -> None:
    got = translated("ncplanefillrect")
    if got is not None:
        assert field(got, "visible") == "2,3"
        assert field(got, "origin") == "2007232e232e232e2007"
        assert field(got, "row1") == "2007232e232e232e2007"
        assert field(got, "clipped") == "0000"


def test_translated_window_wrapper_tracks_nested_subwindows() -> None:
    got = translated("windowwrapper")
    if got is not None:
        assert field(got, "child") == "2,4,2,3"
        assert field(got, "nested") == "3,6"
        assert field(got, "rows") == "2"
        assert field(got, "cols") == "3"
        assert field(got, "data") == "571e491e4e1e201e201e201e"
        assert field(got, "nested_cell") == "5a4f"


def test_translated_window_status_helpers_draw_inside_subwindow() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("windowstatus")
    if got is not None:
        assert field(got, "line") == cells("READY ", 0x1F)
        assert field(got, "field") == "461e691e6c1e651e3a1e201e" + cells("SONG.MOD", 0x2F)
        assert field(got, "size") == "531e691e7a1e651e3a1e201e" + cells("1234", 0x2F)
        assert field(got, "tag") == "541e611e671e3a1e201e" + cells("89ABCDEF", 0x2F)
        assert field(got, "outside") == "2007"


def test_translated_window_drawing_primitives_wrap_plane_operations() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("windowdraw")
    if got is not None:
        assert field(got, "top") == "da1ec41ec41ec41ec41ec41ec42fbf2f"
        assert field(got, "fill") == cells("......", 0x3A)
        assert field(got, "text") == cells("ABC   ", 0x4B)
        assert field(got, "outside") == "2007"


def test_translated_window_cursor_and_text_helpers_wrap_plane_cursor() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("windowcursor")
    if got is not None:
        assert field(got, "row0") == "2007" + cells("A", 0x1E) + cells("BC", 0x2F) + "2007200720072007"
        assert field(got, "row1") == cells("WXY", 0x3A) + "200720072007" + cells("Z", 0x4C)
        assert field(got, "cursor") == "1,8"
        assert field(got, "outside") == "2007"


def test_translated_window_scroll_helpers_wrap_plane_scrolling() -> None:
    def cells(text: str, attr: int) -> str:
        return "".join(f"{ord(ch):02x}{attr:02x}" for ch in text)

    got = translated("windowscroll")
    if got is not None:
        assert field(got, "up0") == cells("BBBBBB", 0x2F)
        assert field(got, "up2") == cells("DDDDDD", 0x4B)
        assert field(got, "up3") == cells("      ", 0x5C)
        assert field(got, "down0") == cells("      ", 0x6D)
        assert field(got, "down1") == cells("BBBBBB", 0x2F)
        assert field(got, "outside") == "2007"


def test_translated_window_audio_level_helper_draws_meter_inside_subwindow() -> None:
    got = translated("windowaudiolevels")
    if got is not None:
        assert field(got, "left") == "231e231e231e231e231e2e082e082e08"
        assert field(got, "right") == "232f232f232f2e082e082e082e082e08"
        assert field(got, "outside") == "2007"


def test_translated_ncplane_box_matches_style3_frame_cells() -> None:
    got = translated("ncplanebox")
    if got is not None:
        assert field(got, "top") == "da1ec41ec41ebf2f"
        assert field(got, "mid") == "b31e201e201eb32f"
        assert field(got, "bot") == "c01ec42fc42fd92f"
        assert field(got, "vline") == "214c"
        assert field(got, "vclip") == "214c"
        assert field(got, "vend") == "2007"


def test_translated_draw_frame_plane_wraps_style3_box_and_ignores_other_styles() -> None:
    got = translated("drawframeplane")
    if got is not None:
        assert field(got, "top") == "da1ec41ec41ebf2f"
        assert field(got, "mid") == "b31e201e201eb32f"
        assert field(got, "bot") == "c01ec42fc42fd92f"
        assert field(got, "ignored") == "2007200720072007"


def test_translated_ncplane_putnstr_clips_to_field_width() -> None:
    got = translated("ncplaneputnstr")
    if got is not None:
        assert field(got, "data") == "611e621e631e200720072007"
        assert field(got, "fill") == "782f792f202f202f"
        assert field(got, "clipped") == "773a783a"
        assert field(got, "after") == "2007"


def test_translated_ncplane_cursor_puts_and_clips_like_terminal_plane() -> None:
    got = translated("ncplanecursor")
    if got is not None:
        assert field(got, "cursor") == "24,40"
        assert field(got, "data") == "411e421e432f202f202f"
        assert field(got, "edge") == "583a"
        assert field(got, "after") == "2007583a"


def test_translated_ncplane_scroll_up_moves_region_and_clears_tail() -> None:
    got = translated("ncplanescroll")
    if got is not None:
        assert field(got, "row0") == "432e432e432e432e"
        assert field(got, "row1") == "433a433a433a433a"
        assert field(got, "row2") == "204c204c204c204c"
        assert field(got, "outside") == "2007"


def test_translated_ncplane_scroll_down_moves_region_and_clears_head() -> None:
    got = translated("ncplanescrolldown")
    if got is not None:
        assert field(got, "row0") == "204c204c204c204c"
        assert field(got, "row1") == "411e411e411e411e"
        assert field(got, "row2") == "422f422f422f422f"
        assert field(got, "outside") == "2007"


def test_translated_text_attr_helpers_pack_vga_16_color_attributes() -> None:
    cases = [
        (7, 0, 0, "07", "7", "0", "0"),
        (15, 7, 0, "7f", "15", "7", "0"),
        (14, 1, 1, "9e", "14", "1", "1"),
        (12, 8, 1, "8c", "12", "0", "1"),
    ]
    for fg, bg, blink, attr, out_fg, out_bg, out_blink in cases:
        got = translated("textattr16", str(fg), str(bg), str(blink))
        if got is not None:
            assert field(got, "attr") == attr
            assert field(got, "fg") == out_fg
            assert field(got, "bg") == out_bg
            assert field(got, "blink") == out_blink


def test_original_and_translated_text_setup_wrappers_small_screen_path() -> None:
    for symbol in ["text_init", "text_init2", "f1_help", "f3_textmetter", "f4_patternnae", "f6_undoc"]:
        out, data = original_seg001_call(
            original_offset(symbol),
            setup_text_setup_target(symbol),
            dump_count=16,
            dump_offset=DSEG_SCRATCH + 0x3A0,
            dump_seg=DSEG,
            strict=False,
        )
        got = translated("textsetup", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def test_original_and_abi_text_setup_public_symbols_small_screen_path() -> None:
    for symbol in ["text_init", "text_init2", "f1_help", "f3_textmetter", "f4_patternnae", "f6_undoc"]:
        out, data = original_seg001_call(
            original_offset(symbol),
            setup_text_setup_target(symbol),
            dump_count=16,
            dump_offset=DSEG_SCRATCH + 0x3A0,
            dump_seg=DSEG,
            strict=False,
        )
        got = translated("abitextsetup", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def test_original_and_translated_graph_setup_wrappers_already_initialized_path() -> None:
    for symbol in ["f5_graphspectr", "init_f5_spectr"]:
        out, data = original_seg001_call(
            original_offset(symbol),
            setup_graph_setup_target(symbol),
            dump_count=9,
            dump_offset=DSEG_SCRATCH + 0x3C0,
            dump_seg=DSEG,
            strict=False,
        )
        got = translated("graphsetup", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def expected_sub_1ab8c(note_byte: int, transpose: int) -> int:
    notes = b"  C-C#D-D#E-F-F#G-G#A-A#B-"
    index = (note_byte & 0x0F) + (transpose & 0xFF)
    if index > 0x0C:
        index -= 0x0C
    lo = notes[index * 2]
    hi = notes[index * 2 + 1]
    if hi == ord("-"):
        hi = ord(" ")
    return lo | (hi << 8)


def test_original_and_translated_sub_1ab8c_note_name_lookup() -> None:
    for note_byte, transpose in [(0x00, 0), (0x01, 0), (0x05, 2), (0x0B, 1), (0x1F, 3)]:
        out, _ = original_seg001_call(
            original_offset("sub_1AB8C"),
            setup_sub_1ab8c(note_byte, transpose),
            dump_count=0,
        )
        got = translated("sub1ab8c", hex(note_byte), hex(transpose))
        assert int(field(out, "ax"), 16) == expected_sub_1ab8c(note_byte, transpose)
        assert field(out, "si") == "2222"
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "si") == field(out, "si")


def test_original_and_abi_sub_1ab8c_public_symbol_note_name_lookup() -> None:
    for note_byte, transpose in [(0x00, 0), (0x01, 0), (0x05, 2), (0x0B, 1), (0x1F, 3)]:
        out, _ = original_seg001_call(
            original_offset("sub_1AB8C"),
            setup_sub_1ab8c(note_byte, transpose),
            dump_count=0,
        )
        got = translated("abisub1ab8c", hex(note_byte), hex(transpose))
        assert int(field(out, "ax"), 16) == expected_sub_1ab8c(note_byte, transpose)
        assert field(out, "si") == "2222"
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "si") == field(out, "si")


def test_original_and_translated_txt_1abae_fixed_width_fs_copy() -> None:
    text = b"ABCDEFGHIJKLMNOPQRSTUV"
    out, data = original_seg001_call(
        original_offset("txt_1ABAE"),
        setup_txt_1abae(text),
        dump_count=len(text) * 2,
        dump_offset=DSEG_SCRATCH + 0x40,
        dump_seg=DSEG,
    )
    got = translated("txt1abae", text.decode("latin1"))
    expected = b"".join(bytes([value, 0x7B]) for value in text)
    assert data == expected
    assert field(out, "si") == f"{DSEG_SCRATCH + len(text):04x}"
    assert field(out, "di") == f"{DSEG_SCRATCH + 0x40 + len(text) * 2:04x}"
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert got.endswith("data=" + expected.hex())


def test_original_and_abi_txt_1abae_public_symbol() -> None:
    text = b"ABCDEFGHIJKLMNOPQRSTUV"
    out, data = original_seg001_call(
        original_offset("txt_1ABAE"),
        setup_txt_1abae(text),
        dump_count=len(text) * 2,
        dump_offset=DSEG_SCRATCH + 0x40,
        dump_seg=DSEG,
    )
    got = translated("abitxt1abae", text.decode("latin1"))
    if got is not None:
        assert got.endswith("data=" + data.hex())
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")


def test_original_and_translated_sub_13826_period_table_lookup() -> None:
    for value, table_word in [(0x01, 0x6B00), (0x12, 0x6500), (0x2F, 0x5000)]:
        out, _ = original_call(original_offset("sub_13826"), setup_sub_13826(value, table_word))
        got = translated("sub13826", hex(value), hex(table_word))
        expected = (table_word >> (value >> 4)) & 0xFFFF
        assert int(field(out, "ax"), 16) == expected
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "di") == field(out, "di")


def test_original_and_abi_sub_13826_public_symbol_period_table_lookup() -> None:
    for value, table_word in [(0x01, 0x6B00), (0x12, 0x6500), (0x2F, 0x5000)]:
        out, _ = original_call(original_offset("sub_13826"), setup_sub_13826(value, table_word))
        got = translated("abisub13826", hex(value), hex(table_word))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "di") == field(out, "di")


def test_original_and_translated_sub_137d5_out_of_range_effect_guard() -> None:
    for flags_3d in [0x00, 0x40]:
        out, data = original_run(
            make_wrapper(original_offset("sub_137D5"), setup_sub_137d5_out_of_range(flags_3d)),
            dump_count=0x3E,
            dump_offset=CHANNEL_OFF,
            dump_seg=DATA_SEG,
        )
        got = translated("sub137d5guard", hex(flags_3d))
        assert data[0x0A] == 33
        assert data[0x3D] == flags_3d
        assert field(out, "ax") == "1234"
        assert field(out, "bx") == f"{CHANNEL_OFF:04x}"
        assert field(out, "cx") == "9abc"
        assert field(out, "dx") == "def0"
        assert field(out, "di") == "0021"
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "di") == field(out, "di")
            assert got.endswith("data=" + bytes([33, flags_3d]).hex())


def test_original_and_abi_sub_137d5_public_symbol_out_of_range_effect_guard() -> None:
    for flags_3d in [0x00, 0x40]:
        out, data = original_run(
            make_wrapper(original_offset("sub_137D5"), setup_sub_137d5_out_of_range(flags_3d)),
            dump_count=0x3E,
            dump_offset=CHANNEL_OFF,
            dump_seg=DATA_SEG,
        )
        got = translated("abisub137d5guard", hex(flags_3d))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "di") == field(out, "di")
            assert field(got, "data") == bytes([data[0x0A], data[0x3D]]).hex()


def test_original_and_translated_sub_13429_disabled_channel_guard() -> None:
    out, data = original_run(
        make_wrapper(original_offset("sub_13429"), setup_sub_13429_disabled()),
        dump_count=0x18,
        dump_offset=CHANNEL_OFF,
        dump_seg=DATA_SEG,
    )
    got = translated("sub13429guard")
    assert data[0x03] == 0x55
    assert data[0x17] == 0
    assert field(out, "ax") == "1234"
    assert field(out, "bx") == f"{CHANNEL_OFF:04x}"
    assert field(out, "cx") == "9abc"
    assert field(out, "dx") == "def0"
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert got.endswith("data=5500")


def test_original_and_abi_sub_13429_public_symbol_disabled_channel_guard() -> None:
    out, data = original_run(
        make_wrapper(original_offset("sub_13429"), setup_sub_13429_disabled()),
        dump_count=0x18,
        dump_offset=CHANNEL_OFF,
        dump_seg=DATA_SEG,
    )
    got = translated("abisub13429guard")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == bytes([data[0x03], data[0x17]]).hex()


def expected_sub_13d95(divisor: int) -> tuple[int, int]:
    level = 1
    cx = divisor
    while True:
        quotient = 31250 // cx
        if quotient <= 0xFF:
            return (level << 8) | ((-quotient) & 0xFF), level
        cx *= 2
        level += 1


def test_original_and_translated_sub_13d95_timer_divisor() -> None:
    for divisor in [200, 500, 1000]:
        out, data = original_run(
            make_wrapper(original_offset("sub_13D95"), setup_sub_13d95(divisor)),
            dump_count=2,
            dump_offset=0x0078,
            dump_seg=DATA_SEG,
        )
        got = translated("sub13d95", hex(divisor))
        expected_ax, expected_level = expected_sub_13d95(divisor)
        assert int(field(out, "ax"), 16) == expected_ax
        assert data == bytes([expected_level, expected_level])
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert got.endswith("data=" + data.hex())


def test_original_and_abi_sub_13d95_public_symbol_timer_divisor() -> None:
    for divisor in [200, 500, 1000]:
        out, data = original_run(
            make_wrapper(original_offset("sub_13D95"), setup_sub_13d95(divisor)),
            dump_count=2,
            dump_offset=0x0078,
            dump_seg=DATA_SEG,
        )
        got = translated("abisub13d95", hex(divisor))
        expected_ax, expected_level = expected_sub_13d95(divisor)
        assert int(field(out, "ax"), 16) == expected_ax
        assert data == bytes([expected_level, expected_level])
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert got.endswith("data=" + data.hex())


def expected_sub_13cf6(value: int, freq: int, buffer_size: int) -> bytes:
    first_div = (5 * freq) // ((value & 0xFF) * 2)
    repeat = first_div // buffer_size + 1
    remainder = first_div % buffer_size
    if remainder == 0:
        repeat -= 1
        remainder = buffer_size
    return bytes([value & 0xFF]) + struct.pack("<HHHH", remainder, repeat, repeat, buffer_size)


def test_original_and_translated_sub_13cf6_nonhardware_buffer_timing() -> None:
    for value, freq, buffer_size in [(50, 22050, 256), (80, 44100, 512)]:
        out, data = original_run(
            make_wrapper(original_offset("sub_13CF6"), setup_sub_13cf6(value, freq, buffer_size)),
            dump_count=0x85,
            dump_offset=0x0044,
            dump_seg=DATA_SEG,
        )
        got = translated("sub13cf6", hex(value), hex(freq), hex(buffer_size))
        selected = bytes([data[0x82]]) + data[0x06:0x0C] + data[0x00:0x02]
        assert selected == expected_sub_13cf6(value, freq, buffer_size)
        if got is not None:
            assert got.endswith("data=" + selected.hex())


def test_original_and_abi_sub_13cf6_public_symbol_nonhardware_buffer_timing() -> None:
    for value, freq, buffer_size in [(50, 22050, 256), (80, 44100, 512)]:
        out, data = original_run(
            make_wrapper(original_offset("sub_13CF6"), setup_sub_13cf6(value, freq, buffer_size)),
            dump_count=0x85,
            dump_offset=0x0044,
            dump_seg=DATA_SEG,
        )
        got = translated("abisub13cf6", hex(value), hex(freq), hex(buffer_size))
        selected = bytes([data[0x82]]) + data[0x06:0x0C] + data[0x00:0x02]
        assert selected == expected_sub_13cf6(value, freq, buffer_size)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert got.endswith("data=" + selected.hex())


def test_original_and_translated_spectr_1bce9_equal_height_noop() -> None:
    for value in [0, 7, 90, 120]:
        out, data = original_seg001_call(
            original_offset("spectr_1BCE9"),
            setup_spectr_1bce9_equal(value),
            dump_count=8,
            dump_offset=DSEG_SCRATCH + 0x1000,
            dump_seg=DSEG,
        )
        got = translated("spectr1bce9equal", hex(value))
        assert data == b"\x00" * 8
        assert field(out, "bx") == f"{DSEG_SCRATCH:04x}"
        assert field(out, "bp") == f"{DSEG_SCRATCH + 0x1000:04x}"
        if got is not None:
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "bp") == field(out, "bp")
            assert got.endswith("data=" + data.hex())


def test_original_and_abi_spectr_1bce9_public_symbol_equal_height_noop() -> None:
    for value in [0, 7, 90, 120]:
        out, data = original_seg001_call(
            original_offset("spectr_1BCE9"),
            setup_spectr_1bce9_equal(value),
            dump_count=8,
            dump_offset=DSEG_SCRATCH + 0x1000,
            dump_seg=DSEG,
        )
        got = translated("abispectr1bce9equal", hex(value))
        assert data == b"\x00" * 8
        assert field(out, "bx") == f"{DSEG_SCRATCH:04x}"
        assert field(out, "bp") == f"{DSEG_SCRATCH + 0x1000:04x}"
        if got is not None:
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "bp") == field(out, "bp")
            assert got.endswith("data=" + data.hex())


def test_original_and_translated_spectr_1bc2d_equal_heights_noop_loop() -> None:
    out, data = original_seg001_call(
        original_offset("spectr_1BC2D"),
        setup_spectr_1bc2d_equal(),
        dump_count=8,
        dump_offset=DSEG_SCRATCH + 0x1000,
        dump_seg=DSEG,
    )
    got = translated("spectr1bc2dequal")
    assert data == b"\x00" * 8
    assert field(out, "bx") == f"{DSEG_SCRATCH + 99:04x}"
    assert field(out, "bp") == f"{DSEG_SCRATCH + 0x1000 + 99 * 3:04x}"
    if got is not None:
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "bp") == field(out, "bp")
        assert got.endswith("data=" + data.hex())


def test_original_and_abi_spectr_1bc2d_public_symbol_equal_heights_noop_loop() -> None:
    out, data = original_seg001_call(
        original_offset("spectr_1BC2D"),
        setup_spectr_1bc2d_equal(),
        dump_count=8,
        dump_offset=DSEG_SCRATCH + 0x1000,
        dump_seg=DSEG,
    )
    got = translated("abispectr1bc2dequal")
    assert data == b"\x00" * 8
    assert field(out, "bx") == f"{DSEG_SCRATCH + 99:04x}"
    assert field(out, "bp") == f"{DSEG_SCRATCH + 0x1000 + 99 * 3:04x}"
    if got is not None:
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "bp") == field(out, "bp")
        assert got.endswith("data=" + data.hex())


def test_original_and_translated_spectr_1bbc1_zero_bin_peak_init() -> None:
    out, data = original_seg001_call(
        original_offset("spectr_1BBC1"),
        setup_spectr_1bbc1_zero_bin(),
        dump_count=0x12D,
        dump_offset=DSEG_SCRATCH + 0x100,
        dump_seg=DSEG,
    )
    got = translated("spectr1bbc1zero")
    selected = bytes([data[0], data[0xC8], data[0x12C]])
    assert selected == b"\x00\x00\x14"
    assert field(out, "si") == f"{DSEG_SCRATCH + 8:04x}"
    assert field(out, "di") == f"{DSEG_SCRATCH + 0x101:04x}"
    assert field(out, "cx") == "0000"
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert field(got, "cx") == field(out, "cx")
        assert got.endswith("data=" + selected.hex())


def test_original_and_abi_spectr_1bbc1_public_symbol_zero_bin_peak_init() -> None:
    out, data = original_seg001_call(
        original_offset("spectr_1BBC1"),
        setup_spectr_1bbc1_zero_bin(),
        dump_count=0x12D,
        dump_offset=DSEG_SCRATCH + 0x100,
        dump_seg=DSEG,
    )
    got = translated("abispectr1bbc1zero")
    selected = bytes([data[0], data[0xC8], data[0x12C]])
    assert selected == b"\x00\x00\x14"
    assert field(out, "si") == f"{DSEG_SCRATCH + 8:04x}"
    assert field(out, "di") == f"{DSEG_SCRATCH + 0x101:04x}"
    assert field(out, "cx") == "0000"
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert field(got, "cx") == field(out, "cx")
        assert got.endswith("data=" + selected.hex())


def test_original_and_translated_video_prp_mtr_positn_three_channel_layout() -> None:
    values = [0x20, 0x40, 0x50]
    out, data = original_seg001_call(
        original_offset("video_prp_mtr_positn"),
        setup_video_prp_mtr_positn(values),
        dump_count=0x29,
        dump_offset=0x1689,
        dump_seg=DSEG,
    )
    got = translated("videoprp", "20", "40", "50")
    selected = data[0:2] + data[8:9] + data[0x23:0x29]
    expected = bytes([2, 2, 11]) + struct.pack("<HHH", 0x15E1, 0x41B5, 0x160A)
    assert selected == expected
    if got is not None:
        assert got.endswith("data=" + expected.hex())


def test_original_and_abi_video_prp_mtr_positn_public_symbol_three_channel_layout() -> None:
    values = [0x20, 0x40, 0x50]
    _, data = original_seg001_call(
        original_offset("video_prp_mtr_positn"),
        setup_video_prp_mtr_positn(values),
        dump_count=0x29,
        dump_offset=0x1689,
        dump_seg=DSEG,
    )
    got = translated("abivideoprp", "20", "40", "50")
    selected = data[0:2] + data[8:9] + data[0x23:0x29]
    expected = bytes([2, 2, 11]) + struct.pack("<HHH", 0x15E1, 0x41B5, 0x160A)
    assert selected == expected
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_translated_video_prp_mtr_positn_uses_active_text_mode_width() -> None:
    expected_40 = bytes([2, 2, 11]) + struct.pack("<HHH", 0x0AF1, 0x20DB, 0x0B06)
    expected_80 = bytes([2, 2, 11]) + struct.pack("<HHH", 0x15E1, 0x41B5, 0x160A)
    got = translated("videoprpmode", "1", "20", "40", "50")
    if got is not None:
        assert field(got, "mode") == "1"
        assert field(got, "cols") == "40"
        assert field(got, "rows") == "25"
        assert field(got, "data") == expected_40.hex()
    got = translated("videoprpmode", "0x50", "20", "40", "50")
    if got is not None:
        assert field(got, "mode") == "80"
        assert field(got, "cols") == "80"
        assert field(got, "rows") == "50"
        assert field(got, "data") == expected_80.hex()


def test_original_and_translated_noop_return_helpers_preserve_registers() -> None:
    for symbol, command in [
        ("nullsub_5", "nullsub5"),
        ("eff_nullsub", "effnullsub"),
        ("nullsub_2", "nullsub2"),
        ("nullsub_4", "nullsub4"),
        ("nullsub_3", "nullsub3"),
    ]:
        out, _ = original_call(original_offset(symbol), setup_noop_probe())
        assert field(out, "ax") == "1234"
        assert field(out, "bx") == "5678"
        assert field(out, "cx") == "9abc"
        assert field(out, "dx") == "def0"
        translated_out = translated(command)
        if translated_out is not None:
            assert field(translated_out, "ax") == "1234"
            assert field(translated_out, "bx") == "5678"
            assert field(translated_out, "cx") == "9abc"
            assert field(translated_out, "dx") == "def0"


def test_original_and_abi_noop_public_symbols_preserve_registers() -> None:
    for symbol in ["nullsub_5", "eff_nullsub", "nullsub_2", "nullsub_4", "nullsub_3"]:
        out, _ = original_call(original_offset(symbol), setup_noop_probe())
        got = translated("abinoop", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def test_original_and_translated_effect_nibble_helpers() -> None:
    wrapper = make_wrapper(original_offset("eff_13BC0"), setup_eff_nibble(initial=0xA0, value=0x05))
    _, channel = original_run(wrapper, dump_count=0x10, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
    assert channel[0x09] == 0xA5
    translated_out = translated("eff13bc0", "0xa0", "0x05")
    if translated_out is not None:
        assert translated_out.endswith("data=a5")

    wrapper = make_wrapper(original_offset("eff_13C34"), setup_eff_nibble(initial=0x05, value=0x0B))
    _, channel = original_run(wrapper, dump_count=0x10, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
    assert channel[0x09] == 0xB5
    translated_out = translated("eff13c34", "0x05", "0x0b")
    if translated_out is not None:
        assert translated_out.endswith("data=b5")


def test_original_and_abi_eff_13bc0_public_symbol_low_nibble_helper() -> None:
    wrapper = make_wrapper(original_offset("eff_13BC0"), setup_eff_nibble(initial=0xA0, value=0x05))
    out, channel = original_run(wrapper, dump_count=0x10, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
    got = translated("abieff13bc0", "0xa0", "0x05")
    assert channel[0x09] == 0xA5
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "data") == "a5"


def test_original_and_abi_eff_13c34_public_symbol_high_nibble_helper() -> None:
    wrapper = make_wrapper(original_offset("eff_13C34"), setup_eff_nibble(initial=0x05, value=0x0B))
    out, channel = original_run(wrapper, dump_count=0x10, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
    got = translated("abieff13c34", "0x05", "0x0b")
    assert channel[0x09] == 0xB5
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "data") == "b5"


def test_original_and_translated_eff_13a43_flag_helper() -> None:
    cases = [
        (0x00, 0x00, 0x00A4, 0x80),
        (0x80, 0x00, 0x00A5, 0x00),
        (0x80, 0x00, 0x00A6, 0x00),
        (0x12, 0x00, 0x0080, 0x12),
        (0x12, 0x00, 0x0081, 0x12),
    ]
    for flags, sndflags, value, expected_flags in cases:
        wrapper = make_wrapper(original_offset("eff_13A43"), setup_eff_13a43(flags, sndflags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        assert channel[0x17] == expected_flags
        translated_out = translated("eff13a43", hex(flags), hex(sndflags), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith(f"data={expected_flags:02x}")


def test_original_and_abi_eff_13a43_public_symbol_flag_helper() -> None:
    cases = [
        (0x00, 0x00, 0x00A4, 0x80),
        (0x80, 0x00, 0x00A5, 0x00),
        (0x80, 0x00, 0x00A6, 0x00),
        (0x12, 0x00, 0x0080, 0x12),
        (0x12, 0x00, 0x0081, 0x12),
    ]
    for flags, sndflags, value, expected_flags in cases:
        wrapper = make_wrapper(original_offset("eff_13A43"), setup_eff_13a43(flags, sndflags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        got = translated("abieff13a43", hex(flags), hex(sndflags), hex(value))
        assert channel[0x17] == expected_flags
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == f"{expected_flags:02x}"


def test_original_and_translated_eff_13a94_sample_position_helper() -> None:
    cases = [
        (0x04, 0x00001000, 0x01, 0x00, 0x0000),
        (0x04, 0x00000300, 0x01, 0x55, 0x0000),
        (0x04, 0x00001000, 0x01, 0x00, 0x0006),
    ]
    for byte_16, sample_end, byte_2461a, flags, value in cases:
        wrapper = make_wrapper(original_offset("eff_13A94"), setup_eff_13a94(byte_16, sample_end, byte_2461a, flags, value))
        out, channel = original_run(wrapper, dump_count=0x50, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = bytes([channel[0x16], channel[0x17]]) + channel[0x4C:0x4E]
        translated_out = translated("eff13a94", hex(byte_16), hex(sample_end), hex(byte_2461a), hex(flags), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13a94_public_symbol_sample_position_helper() -> None:
    cases = [
        (0x04, 0x00001000, 0x01, 0x00, 0x0000),
        (0x04, 0x00000300, 0x01, 0x55, 0x0000),
        (0x04, 0x00001000, 0x01, 0x00, 0x0006),
    ]
    for byte_16, sample_end, byte_2461a, flags, value in cases:
        wrapper = make_wrapper(original_offset("eff_13A94"), setup_eff_13a94(byte_16, sample_end, byte_2461a, flags, value))
        out, channel = original_run(wrapper, dump_count=0x50, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = bytes([channel[0x16], channel[0x17]]) + channel[0x4C:0x4E]
        got = translated("abieff13a94", hex(byte_16), hex(sample_end), hex(byte_2461a), hex(flags), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13ad7_volume_delta_helper() -> None:
    cases = [
        (0x20, 0x40, 0x0005),
        (0x02, 0x40, 0x0005),
        (0x3E, 0x40, 0x0020),
    ]
    for volume, max_volume, value in cases:
        wrapper = make_wrapper(original_offset("eff_13AD7"), setup_eff_13ad7(volume, max_volume, value))
        out, channel = original_run(wrapper, dump_count=0x09, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09]
        translated_out = translated("eff13ad7", hex(volume), hex(max_volume), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13ad7_public_symbol_volume_delta_helper() -> None:
    cases = [
        (0x20, 0x40, 0x0005),
        (0x02, 0x40, 0x0005),
        (0x3E, 0x40, 0x0020),
    ]
    for volume, max_volume, value in cases:
        wrapper = make_wrapper(original_offset("eff_13AD7"), setup_eff_13ad7(volume, max_volume, value))
        out, channel = original_run(wrapper, dump_count=0x09, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09]
        got = translated("abieff13ad7", hex(volume), hex(max_volume), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13b06_position_and_break_helper() -> None:
    cases = [
        (0x04, 0x0001),
        (0x04, 0x0020),
    ]
    for playsettings, value in cases:
        wrapper = make_wrapper(original_offset("eff_13B06"), setup_eff_13b06(playsettings, value))
        out, globals_ = original_run(wrapper, dump_count=2, dump_offset=0x0050, dump_seg=DATA_SEG)
        expected = globals_[0:2]
        translated_out = translated("eff13b06", hex(playsettings), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13b06_public_symbol_position_and_break_helper() -> None:
    cases = [
        (0x04, 0x0001),
        (0x04, 0x0020),
    ]
    for playsettings, value in cases:
        wrapper = make_wrapper(original_offset("eff_13B06"), setup_eff_13b06(playsettings, value))
        out, globals_ = original_run(wrapper, dump_count=2, dump_offset=0x0050, dump_seg=DATA_SEG)
        expected = globals_[0:2]
        got = translated("abieff13b06", hex(playsettings), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13b78_set_volume_helper() -> None:
    cases = [
        (0x20, 0x40),
        (0x50, 0x40),
    ]
    for volume, max_volume in cases:
        wrapper = make_wrapper(original_offset("eff_13B78"), setup_eff_13b78(volume, max_volume))
        out, channel = original_run(wrapper, dump_count=0x09, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09]
        translated_out = translated("eff13b78", hex(volume), hex(max_volume))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13b78_public_symbol_set_volume_helper() -> None:
    cases = [
        (0x20, 0x40),
        (0x50, 0x40),
    ]
    for volume, max_volume in cases:
        wrapper = make_wrapper(original_offset("eff_13B78"), setup_eff_13b78(volume, max_volume))
        out, channel = original_run(wrapper, dump_count=0x09, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09]
        got = translated("abieff13b78", hex(volume), hex(max_volume))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13b88_decimal_break_helper() -> None:
    cases = [
        (0xAA, 0xBB, 0x0012),
        (0xAA, 0xBB, 0x0064),
    ]
    for initial_24669, initial_2466a, value in cases:
        wrapper = make_wrapper(original_offset("eff_13B88"), setup_eff_13b88(initial_24669, initial_2466a, value))
        out, globals_ = original_run(wrapper, dump_count=2, dump_offset=0x00C9, dump_seg=DATA_SEG)
        translated_out = translated("eff13b88", hex(initial_24669), hex(initial_2466a), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + globals_.hex())


def test_original_and_abi_eff_13b88_public_symbol_decimal_break_helper() -> None:
    cases = [
        (0xAA, 0xBB, 0x0012),
        (0xAA, 0xBB, 0x0064),
    ]
    for initial_24669, initial_2466a, value in cases:
        wrapper = make_wrapper(original_offset("eff_13B88"), setup_eff_13b88(initial_24669, initial_2466a, value))
        out, globals_ = original_run(wrapper, dump_count=2, dump_offset=0x00C9, dump_seg=DATA_SEG)
        got = translated("abieff13b88", hex(initial_24669), hex(initial_2466a), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == globals_.hex()


def test_original_and_translated_eff_13bb2_flag_20_helper() -> None:
    cases = [
        (0x00, 0x0001),
        (0x20, 0x0000),
        (0xA0, 0x0000),
    ]
    for flags, value in cases:
        wrapper = make_wrapper(original_offset("eff_13BB2"), setup_eff_13bb2(flags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x17:0x18]
        translated_out = translated("eff13bb2", hex(flags), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13bb2_public_symbol_flag_20_helper() -> None:
    cases = [
        (0x00, 0x0001),
        (0x20, 0x0000),
        (0xA0, 0x0000),
    ]
    for flags, value in cases:
        wrapper = make_wrapper(original_offset("eff_13BB2"), setup_eff_13bb2(flags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x17:0x18]
        got = translated("abieff13bb2", hex(flags), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13ba3_dispatch_helper() -> None:
    cases = [
        (0x00, 0x0031),
        (0x20, 0x0030),
    ]
    for flags, value in cases:
        wrapper = make_wrapper(original_offset("eff_13BA3"), setup_eff_13ba3(flags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x17:0x18]
        translated_out = translated("eff13ba3", hex(flags), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13ba3_public_symbol_dispatch_helper() -> None:
    cases = [
        (0x00, 0x0031),
        (0x20, 0x0030),
    ]
    for flags, value in cases:
        wrapper = make_wrapper(original_offset("eff_13BA3"), setup_eff_13ba3(flags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x17:0x18]
        got = translated("abieff13ba3", hex(flags), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13bc8_table_pointer_helper() -> None:
    cases = [
        (0x00, 0x1234, 0x0005),
        (0x01, 0x1234, 0x0005),
    ]
    for byte_2461a, dx, value in cases:
        wrapper = make_wrapper(original_offset("eff_13BC8"), setup_eff_13bc8(byte_2461a, dx, value))
        out, channel = original_run(wrapper, dump_count=0x3A, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x14:0x16] + channel[0x38:0x3A]
        translated_out = translated("eff13bc8", hex(byte_2461a), hex(dx), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert field(translated_out, "dx") == field(out, "dx")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13bc8_public_symbol_table_pointer_helper() -> None:
    cases = [
        (0x00, 0x1234, 0x0005),
        (0x01, 0x1234, 0x0005),
    ]
    for byte_2461a, dx, value in cases:
        wrapper = make_wrapper(original_offset("eff_13BC8"), setup_eff_13bc8(byte_2461a, dx, value))
        out, channel = original_run(wrapper, dump_count=0x3A, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x14:0x16] + channel[0x38:0x3A]
        got = translated("abieff13bc8", hex(byte_2461a), hex(dx), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13c3f_note_map_to_flag_helper() -> None:
    cases = [
        (0x01, 0x12, 0x00, 0x000F),
        (0x00, 0x12, 0x00, 0x000F),
    ]
    for byte_24668, flags, sndflags, value in cases:
        wrapper = make_wrapper(original_offset("eff_13C3F"), setup_eff_13c3f(byte_24668, flags, sndflags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x17:0x18]
        translated_out = translated("eff13c3f", hex(byte_24668), hex(flags), hex(sndflags), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13c3f_public_symbol_note_map_to_flag_helper() -> None:
    cases = [
        (0x01, 0x12, 0x00, 0x000F),
        (0x00, 0x12, 0x00, 0x000F),
    ]
    for byte_24668, flags, sndflags, value in cases:
        wrapper = make_wrapper(original_offset("eff_13C3F"), setup_eff_13c3f(byte_24668, flags, sndflags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x17:0x18]
        got = translated("abieff13c3f", hex(byte_24668), hex(flags), hex(sndflags), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13c64_periodic_callback_guard() -> None:
    cases = [
        (0x00, 0x08, 0x0004),
        (0x08, 0x00, 0x0004),
        (0x09, 0x00, 0x0004),
    ]
    for byte_24668, flags_3d, value in cases:
        wrapper = make_wrapper(original_offset("eff_13C64"), setup_eff_13c64(byte_24668, flags_3d, value))
        out, channel = original_run(wrapper, dump_count=0x3E, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x3D:0x3E]
        translated_out = translated("eff13c64", hex(byte_24668), hex(flags_3d), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13c64_public_symbol_periodic_callback_guard() -> None:
    cases = [
        (0x00, 0x08, 0x0004),
        (0x08, 0x00, 0x0004),
        (0x09, 0x00, 0x0004),
    ]
    for byte_24668, flags_3d, value in cases:
        wrapper = make_wrapper(original_offset("eff_13C64"), setup_eff_13c64(byte_24668, flags_3d, value))
        out, channel = original_run(wrapper, dump_count=0x3E, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x3D:0x3E]
        got = translated("abieff13c64", hex(byte_24668), hex(flags_3d), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13c88_conditional_volume_up_helper() -> None:
    cases = [
        (0x20, 0x00, 0x40, 0x0005),
        (0x3F, 0x00, 0x40, 0x0005),
        (0x20, 0x01, 0x40, 0x0005),
    ]
    for volume, byte_24668, max_volume, value in cases:
        wrapper = make_wrapper(original_offset("eff_13C88"), setup_eff_13c88(volume, byte_24668, max_volume, value))
        out, channel = original_run(wrapper, dump_count=0x09, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09]
        translated_out = translated("eff13c88", hex(volume), hex(byte_24668), hex(max_volume), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13c88_public_symbol_conditional_volume_up_helper() -> None:
    cases = [
        (0x20, 0x00, 0x40, 0x0005),
        (0x3F, 0x00, 0x40, 0x0005),
        (0x20, 0x01, 0x40, 0x0005),
    ]
    for volume, byte_24668, max_volume, value in cases:
        wrapper = make_wrapper(original_offset("eff_13C88"), setup_eff_13c88(volume, byte_24668, max_volume, value))
        out, channel = original_run(wrapper, dump_count=0x09, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09]
        got = translated("abieff13c88", hex(volume), hex(byte_24668), hex(max_volume), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13c95_conditional_volume_down_helper() -> None:
    cases = [
        (0x20, 0x00, 0x0005),
        (0x02, 0x00, 0x0005),
        (0x20, 0x01, 0x0005),
    ]
    for volume, byte_24668, value in cases:
        wrapper = make_wrapper(original_offset("eff_13C95"), setup_eff_13c95(volume, byte_24668, value))
        out, channel = original_run(wrapper, dump_count=0x09, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09]
        translated_out = translated("eff13c95", hex(volume), hex(byte_24668), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13c95_public_symbol_conditional_volume_down_helper() -> None:
    cases = [
        (0x20, 0x00, 0x0005),
        (0x02, 0x00, 0x0005),
        (0x20, 0x01, 0x0005),
    ]
    for volume, byte_24668, value in cases:
        wrapper = make_wrapper(original_offset("eff_13C95"), setup_eff_13c95(volume, byte_24668, value))
        out, channel = original_run(wrapper, dump_count=0x09, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09]
        got = translated("abieff13c95", hex(volume), hex(byte_24668), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13ca2_pattern_match_volume_callback_helper() -> None:
    cases = [
        (0x12, 0x0012),
        (0x12, 0x0034),
    ]
    for byte_24668, value in cases:
        wrapper = make_wrapper(original_offset("eff_13CA2"), setup_eff_13ca2(byte_24668, value))
        out, globals_ = original_run(wrapper, dump_count=1, dump_offset=0x00C8, dump_seg=DATA_SEG)
        translated_out = translated("eff13ca2", hex(byte_24668), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + globals_.hex())


def test_original_and_abi_eff_13ca2_public_symbol_pattern_match_volume_callback_helper() -> None:
    cases = [
        (0x12, 0x0012),
        (0x12, 0x0034),
    ]
    for byte_24668, value in cases:
        wrapper = make_wrapper(original_offset("eff_13CA2"), setup_eff_13ca2(byte_24668, value))
        out, globals_ = original_run(wrapper, dump_count=1, dump_offset=0x00C8, dump_seg=DATA_SEG)
        got = translated("abieff13ca2", hex(byte_24668), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == globals_.hex()


def test_original_and_translated_eff_13cb3_guarded_note_cut_helper() -> None:
    cases = [
        (0x1234, 0xAA, 0xBB, 0x12, 0x0034),
        (0x0000, 0xAA, 0xBB, 0x12, 0x0012),
    ]
    for period, byte_0a, byte_0b, byte_24668, value in cases:
        wrapper = make_wrapper(original_offset("eff_13CB3"), setup_eff_13cb3(period, byte_0a, byte_0b, byte_24668, value))
        out, channel = original_run(wrapper, dump_count=0x0C, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x0A:0x0C]
        translated_out = translated("eff13cb3", hex(period), hex(byte_0a), hex(byte_0b), hex(byte_24668), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13cb3_public_symbol_guarded_note_cut_helper() -> None:
    cases = [
        (0x1234, 0xAA, 0xBB, 0x12, 0x0034),
        (0x0000, 0xAA, 0xBB, 0x12, 0x0012),
    ]
    for period, byte_0a, byte_0b, byte_24668, value in cases:
        wrapper = make_wrapper(original_offset("eff_13CB3"), setup_eff_13cb3(period, byte_0a, byte_0b, byte_24668, value))
        out, channel = original_run(wrapper, dump_count=0x0C, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x0A:0x0C]
        got = translated("abieff13cb3", hex(period), hex(byte_0a), hex(byte_0b), hex(byte_24668), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13cc9_tick_delay_helper() -> None:
    cases = [
        (0x01, 0x00, 0xAA, 0x0005),
        (0x00, 0x01, 0xAA, 0x0005),
        (0x00, 0x00, 0xAA, 0x0005),
    ]
    for byte_24668, byte_2466d, initial_2466c, value in cases:
        wrapper = make_wrapper(original_offset("eff_13CC9"), setup_eff_13cc9(byte_24668, byte_2466d, initial_2466c, value))
        out, globals_ = original_run(wrapper, dump_count=1, dump_offset=0x00CC, dump_seg=DATA_SEG)
        translated_out = translated("eff13cc9", hex(byte_24668), hex(byte_2466d), hex(initial_2466c), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + globals_.hex())


def test_original_and_abi_eff_13cc9_public_symbol_tick_delay_helper() -> None:
    cases = [
        (0x01, 0x00, 0xAA, 0x0005),
        (0x00, 0x01, 0xAA, 0x0005),
        (0x00, 0x00, 0xAA, 0x0005),
    ]
    for byte_24668, byte_2466d, initial_2466c, value in cases:
        wrapper = make_wrapper(original_offset("eff_13CC9"), setup_eff_13cc9(byte_24668, byte_2466d, initial_2466c, value))
        out, globals_ = original_run(wrapper, dump_count=1, dump_offset=0x00CC, dump_seg=DATA_SEG)
        got = translated("abieff13cc9", hex(byte_24668), hex(byte_2466d), hex(initial_2466c), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == globals_.hex()


def test_original_and_translated_eff_13cdd_speed_helper_without_timer_path() -> None:
    cases = [
        (0x00, 0x77, 0x88, 0x0000),
        (0x00, 0x77, 0x88, 0x0012),
        (0x02, 0x77, 0x88, 0x0040),
    ]
    for playsettings, initial_24667, initial_24668, value in cases:
        wrapper = make_wrapper(original_offset("eff_13CDD"), setup_eff_13cdd(playsettings, initial_24667, initial_24668, value))
        out, globals_ = original_run(wrapper, dump_count=2, dump_offset=0x00C7, dump_seg=DATA_SEG)
        translated_out = translated("eff13cdd", hex(playsettings), hex(initial_24667), hex(initial_24668), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + globals_.hex())


def test_original_and_abi_eff_13cdd_public_symbol_speed_helper_without_timer_path() -> None:
    cases = [
        (0x00, 0x77, 0x88, 0x0000),
        (0x00, 0x77, 0x88, 0x0012),
        (0x02, 0x77, 0x88, 0x0040),
    ]
    for playsettings, initial_24667, initial_24668, value in cases:
        wrapper = make_wrapper(original_offset("eff_13CDD"), setup_eff_13cdd(playsettings, initial_24667, initial_24668, value))
        out, globals_ = original_run(wrapper, dump_count=2, dump_offset=0x00C7, dump_seg=DATA_SEG)
        got = translated("abieff13cdd", hex(playsettings), hex(initial_24667), hex(initial_24668), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == globals_.hex()


def test_original_and_translated_eff_13c02_pattern_delay_helper() -> None:
    cases = [
        (0x01, 0x0034, 0x12, 0x02, 0x0005),
        (0x00, 0x0034, 0x12, 0x00, 0x0005),
        (0x00, 0x0034, 0x12, 0x00, 0x0000),
    ]
    for byte_24668, word_245f6, byte_3b, byte_3c, value in cases:
        wrapper = make_wrapper(original_offset("eff_13C02"), setup_eff_13c02(byte_24668, word_245f6, byte_3b, byte_3c, value))
        out, channel = original_run(wrapper, dump_count=0x3D, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x3B:0x3D]
        translated_out = translated("eff13c02", hex(byte_24668), hex(word_245f6), hex(byte_3b), hex(byte_3c), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13c02_public_symbol_pattern_delay_helper() -> None:
    cases = [
        (0x01, 0x0034, 0x12, 0x02, 0x0005),
        (0x00, 0x0034, 0x12, 0x00, 0x0005),
        (0x00, 0x0034, 0x12, 0x00, 0x0000),
    ]
    for byte_24668, word_245f6, byte_3b, byte_3c, value in cases:
        wrapper = make_wrapper(original_offset("eff_13C02"), setup_eff_13c02(byte_24668, word_245f6, byte_3b, byte_3c, value))
        out, channel = original_run(wrapper, dump_count=0x3D, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        _, globals_ = original_run(wrapper, dump_count=4, dump_offset=0x00C8, dump_seg=DATA_SEG)
        got = translated("abieff13c02", hex(byte_24668), hex(word_245f6), hex(byte_3b), hex(byte_3c), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == channel[0x3B:0x3D].hex()
            assert field(got, "globals") == globals_.hex()


def test_original_and_translated_effect_period_slide_helpers() -> None:
    cases = [
        ("eff_1387F", "eff1387f", 0x0200, 0x05, 0x01B0),
        ("eff_13886", "eff13886", 0x00C0, 0x05, 0x00A0),
        ("eff_1389D", "eff1389d", 0x0200, 0x05, 0x0250),
        ("eff_138A4", "eff138a4", 0x3540, 0x05, 0x3580),
    ]
    for symbol, command, initial, value, expected in cases:
        wrapper = make_wrapper(original_offset(symbol), setup_effect_slide(initial, value))
        out, channel = original_run(wrapper, dump_count=2, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        assert channel == struct.pack("<H", expected)
        assert field(out, "ax") == f"{expected:04x}"
        translated_out = translated(command, hex(initial), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == f"{expected:04x}"
            assert translated_out.endswith("data=" + struct.pack("<H", expected).hex())


def test_original_and_abi_effect_period_slide_public_symbols() -> None:
    cases = [
        ("eff_1387F", 0x0200, 0x05),
        ("eff_13886", 0x00C0, 0x05),
        ("eff_1389D", 0x0200, 0x05),
        ("eff_138A4", 0x3540, 0x05),
    ]
    for symbol, initial, value in cases:
        wrapper = make_wrapper(original_offset(symbol), setup_effect_slide(initial, value))
        out, channel = original_run(wrapper, dump_count=2, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        got = translated("abieffslide", symbol, hex(initial), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == channel.hex()


def test_original_and_translated_effect_period_slide_helpers_skip_when_channel_inactive() -> None:
    for symbol, command in [("eff_1387F", "eff1387f"), ("eff_1389D", "eff1389d")]:
        wrapper = make_wrapper(original_offset(symbol), setup_effect_slide(0x0200, 0x05, active_channel=1))
        out, channel = original_run(wrapper, dump_count=2, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        assert channel == struct.pack("<H", 0x0200)
        assert field(out, "ax") == "0005"
        translated_out = translated(command, "0x0200", "0x05", "1")
        if translated_out is not None:
            assert field(translated_out, "ax") == "0005"
            assert translated_out.endswith("data=0002")


def test_original_and_abi_effect_period_slide_public_symbols_skip_when_channel_inactive() -> None:
    for symbol in ["eff_1387F", "eff_1389D"]:
        wrapper = make_wrapper(original_offset(symbol), setup_effect_slide(0x0200, 0x05, active_channel=1))
        out, channel = original_run(wrapper, dump_count=2, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        got = translated("abieffslide", symbol, "0x0200", "0x05", "1")
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == channel.hex()


def test_original_and_translated_eff_1392f_vibrato_period_helper() -> None:
    cases = [
        (0x0200, 0x00, 0x34, 0x04, 0x00, 0x0000),
        (0x0200, 0x01, 0x25, 0x84, 0x00, 0x0000),
        (0x0200, 0x02, 0x10, 0x04, 0x01, 0x00A7),
    ]
    for period, byte_09, byte_0c, byte_0d, playsettings, value in cases:
        wrapper = make_wrapper(original_offset("eff_1392F"), setup_eff_1392f(period, byte_09, byte_0c, byte_0d, playsettings, value))
        out, channel = original_run(wrapper, dump_count=0x0E, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x0C:0x0E]
        translated_out = translated("eff1392f", hex(period), hex(byte_09), hex(byte_0c), hex(byte_0d), hex(playsettings), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_1392f_public_symbol_vibrato_period_helper() -> None:
    cases = [
        (0x0200, 0x00, 0x34, 0x04, 0x00, 0x0000),
        (0x0200, 0x01, 0x25, 0x84, 0x00, 0x0000),
        (0x0200, 0x02, 0x10, 0x04, 0x01, 0x00A7),
    ]
    for period, byte_09, byte_0c, byte_0d, playsettings, value in cases:
        wrapper = make_wrapper(original_offset("eff_1392F"), setup_eff_1392f(period, byte_09, byte_0c, byte_0d, playsettings, value))
        out, channel = original_run(wrapper, dump_count=0x0E, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x0C:0x0E]
        got = translated("abieff1392f", hex(period), hex(byte_09), hex(byte_0c), hex(byte_0d), hex(playsettings), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_139ac_volume_and_target_slide_helper() -> None:
    cases = [
        (0x0200, 0x0250, 0x0050, 0x30, 0x20, 0x40, 0x0005),
        (0x0300, 0x0200, 0x0050, 0x00, 0x02, 0x40, 0x0005),
    ]
    for period, target, step, flags, volume, max_volume, value in cases:
        wrapper = make_wrapper(original_offset("eff_139AC"), setup_eff_139ac(period, target, step, flags, volume, max_volume, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x08:0x09] + channel[0x10:0x14] + channel[0x17:0x18]
        translated_out = translated("eff139ac", hex(period), hex(target), hex(step), hex(flags), hex(volume), hex(max_volume), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_139ac_public_symbol_volume_and_target_slide_helper() -> None:
    cases = [
        (0x0200, 0x0250, 0x0050, 0x30, 0x20, 0x40, 0x0005),
        (0x0300, 0x0200, 0x0050, 0x00, 0x02, 0x40, 0x0005),
    ]
    for period, target, step, flags, volume, max_volume, value in cases:
        wrapper = make_wrapper(original_offset("eff_139AC"), setup_eff_139ac(period, target, step, flags, volume, max_volume, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x08:0x09] + channel[0x10:0x14] + channel[0x17:0x18]
        got = translated("abieff139ac", hex(period), hex(target), hex(step), hex(flags), hex(volume), hex(max_volume), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_139b2_volume_and_vibrato_helper() -> None:
    cases = [
        (0x0200, 0x00, 0x34, 0x04, 0x00, 0x20, 0x40, 0x0005),
        (0x0200, 0x01, 0x25, 0x84, 0x00, 0x3E, 0x40, 0x0020),
    ]
    for period, byte_09, byte_0c, byte_0d, playsettings, volume, max_volume, value in cases:
        wrapper = make_wrapper(
            original_offset("eff_139B2"),
            setup_eff_139b2(period, byte_09, byte_0c, byte_0d, playsettings, volume, max_volume, value),
        )
        out, channel = original_run(wrapper, dump_count=0x0E, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x08:0x09] + channel[0x0C:0x0E]
        translated_out = translated("eff139b2", hex(period), hex(byte_09), hex(byte_0c), hex(byte_0d), hex(playsettings), hex(volume), hex(max_volume), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_139b2_public_symbol_volume_and_vibrato_helper() -> None:
    cases = [
        (0x0200, 0x00, 0x34, 0x04, 0x00, 0x20, 0x40, 0x0005),
        (0x0200, 0x01, 0x25, 0x84, 0x00, 0x3E, 0x40, 0x0020),
    ]
    for period, byte_09, byte_0c, byte_0d, playsettings, volume, max_volume, value in cases:
        wrapper = make_wrapper(
            original_offset("eff_139B2"),
            setup_eff_139b2(period, byte_09, byte_0c, byte_0d, playsettings, volume, max_volume, value),
        )
        out, channel = original_run(wrapper, dump_count=0x0E, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x08:0x09] + channel[0x0C:0x0E]
        got = translated("abieff139b2", hex(period), hex(byte_09), hex(byte_0c), hex(byte_0d), hex(playsettings), hex(volume), hex(max_volume), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_139b9_tremolo_volume_helper() -> None:
    cases = [
        (0x20, 0x00, 0x34, 0x04, 0x40, 0x0000),
        (0x20, 0x10, 0x25, 0x84, 0x40, 0x0000),
        (0x3E, 0x20, 0x10, 0x04, 0x40, 0x00A7),
    ]
    for volume, byte_09, byte_0e, byte_0f, max_volume, value in cases:
        wrapper = make_wrapper(original_offset("eff_139B9"), setup_eff_139b9(volume, byte_09, byte_0e, byte_0f, max_volume, value))
        out, channel = original_run(wrapper, dump_count=0x10, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09] + channel[0x0E:0x10]
        translated_out = translated("eff139b9", hex(volume), hex(byte_09), hex(byte_0e), hex(byte_0f), hex(max_volume), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_139b9_public_symbol_tremolo_volume_helper() -> None:
    cases = [
        (0x20, 0x00, 0x34, 0x04, 0x40, 0x0000),
        (0x20, 0x10, 0x25, 0x84, 0x40, 0x0000),
        (0x3E, 0x20, 0x10, 0x04, 0x40, 0x00A7),
    ]
    for volume, byte_09, byte_0e, byte_0f, max_volume, value in cases:
        wrapper = make_wrapper(original_offset("eff_139B9"), setup_eff_139b9(volume, byte_09, byte_0e, byte_0f, max_volume, value))
        out, channel = original_run(wrapper, dump_count=0x10, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09] + channel[0x0E:0x10]
        got = translated("abieff139b9", hex(volume), hex(byte_09), hex(byte_0e), hex(byte_0f), hex(max_volume), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_138d2_target_slide_helper() -> None:
    cases = [
        (0x0200, 0x0250, 0x0050, 0x30, 0x0000, 0x0250, 0x0000, 0x0050, 0x20),
        (0x0300, 0x0200, 0x0050, 0x00, 0x0000, 0x02B0, 0x0200, 0x0050, 0x00),
        (0x0100, 0x0180, 0x0001, 0x10, 0x0004, 0x0140, 0x0180, 0x0040, 0x10),
    ]
    for current, target, step, flags, value, expected_current, expected_target, expected_step, expected_flags in cases:
        wrapper = make_wrapper(original_offset("eff_138D2"), setup_effect_target_slide(current, target, step, flags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = struct.pack("<HHHB", expected_current, expected_target, expected_step, expected_flags)
        selected = channel[0:2] + channel[0x10:0x14] + channel[0x17:0x18]
        assert selected == expected
        translated_out = translated("eff138d2", hex(current), hex(target), hex(step), hex(flags), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_138d2_public_symbol_target_slide_helper() -> None:
    cases = [
        (0x0200, 0x0250, 0x0050, 0x30, 0x0000),
        (0x0300, 0x0200, 0x0050, 0x00, 0x0000),
        (0x0100, 0x0180, 0x0001, 0x10, 0x0004),
    ]
    for current, target, step, flags, value in cases:
        wrapper = make_wrapper(original_offset("eff_138D2"), setup_effect_target_slide(current, target, step, flags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x10:0x14] + channel[0x17:0x18]
        got = translated("abieff138d2", hex(current), hex(target), hex(step), hex(flags), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13de5_and_13def_period_memory_slide_helpers() -> None:
    cases = [
        ("eff_13DE5", "eff13de5", 0x0200, 0x01, 0x00, 0x0005),
        ("eff_13DEF", "eff13def", 0x0200, 0x01, 0x00, 0x0005),
        ("eff_13DE5", "eff13de5", 0x0200, 0x00, 0x00, 0x00E0),
        ("eff_13DEF", "eff13def", 0x0200, 0x00, 0x00, 0x00E0),
    ]
    for symbol, command, initial_period, byte_24668, stored_34, value in cases:
        wrapper = make_wrapper(original_offset(symbol), setup_eff_13de5(initial_period, byte_24668, stored_34, value))
        out, channel = original_run(wrapper, dump_count=0x35, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x34:0x35]
        translated_out = translated(command, hex(initial_period), hex(byte_24668), hex(stored_34), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13de5_and_13def_public_symbols_period_memory_slide_helpers() -> None:
    cases = [
        ("eff_13DE5", 0x0200, 0x01, 0x00, 0x0005),
        ("eff_13DEF", 0x0200, 0x01, 0x00, 0x0005),
        ("eff_13DE5", 0x0200, 0x00, 0x00, 0x00E0),
        ("eff_13DEF", 0x0200, 0x00, 0x00, 0x00E0),
    ]
    for symbol, initial_period, byte_24668, stored_34, value in cases:
        wrapper = make_wrapper(original_offset(symbol), setup_eff_13de5(initial_period, byte_24668, stored_34, value))
        out, channel = original_run(wrapper, dump_count=0x35, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x34:0x35]
        got = translated("abieff13de", symbol, hex(initial_period), hex(byte_24668), hex(stored_34), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13e1e_target_slide_step_helper() -> None:
    cases = [
        (0x0100, 0x0180, 0x0001, 0x10, 0x0004),
        (0x0200, 0x0250, 0x0008, 0x30, 0x0000),
    ]
    for current, target, step, flags, value in cases:
        wrapper = make_wrapper(original_offset("eff_13E1E"), setup_eff_13e1e(current, target, step, flags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x10:0x14] + channel[0x17:0x18]
        translated_out = translated("eff13e1e", hex(current), hex(target), hex(step), hex(flags), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13e1e_public_symbol_target_slide_step_helper() -> None:
    cases = [
        (0x0100, 0x0180, 0x0001, 0x10, 0x0004),
        (0x0200, 0x0250, 0x0008, 0x30, 0x0000),
    ]
    for current, target, step, flags, value in cases:
        wrapper = make_wrapper(original_offset("eff_13E1E"), setup_eff_13e1e(current, target, step, flags, value))
        out, channel = original_run(wrapper, dump_count=0x18, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x10:0x14] + channel[0x17:0x18]
        got = translated("abieff13e1e", hex(current), hex(target), hex(step), hex(flags), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13e2d_vibrato_period_helper() -> None:
    cases = [
        (0x0200, 0x00, 0x34, 0x04, 0x00, 0x0000),
        (0x0200, 0x01, 0x25, 0x84, 0x00, 0x0000),
    ]
    for period, byte_09, byte_0c, byte_0d, playsettings, value in cases:
        wrapper = make_wrapper(original_offset("eff_13E2D"), setup_eff_13e2d(period, byte_09, byte_0c, byte_0d, playsettings, value))
        out, channel = original_run(wrapper, dump_count=0x0E, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x0C:0x0E]
        translated_out = translated("eff13e2d", hex(period), hex(byte_09), hex(byte_0c), hex(byte_0d), hex(playsettings), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13e2d_public_symbol_vibrato_period_helper() -> None:
    cases = [
        (0x0200, 0x00, 0x34, 0x04, 0x00, 0x0000),
        (0x0200, 0x01, 0x25, 0x84, 0x00, 0x0000),
    ]
    for period, byte_09, byte_0c, byte_0d, playsettings, value in cases:
        wrapper = make_wrapper(original_offset("eff_13E2D"), setup_eff_13e2d(period, byte_09, byte_0c, byte_0d, playsettings, value))
        out, channel = original_run(wrapper, dump_count=0x0E, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x0C:0x0E]
        got = translated("abieff13e2d", hex(period), hex(byte_09), hex(byte_0c), hex(byte_0d), hex(playsettings), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13e32_volume_memory_delta_helper() -> None:
    cases = [
        (0x20, 0x00, 0x40, 0x00, 0x0005),
        (0x20, 0x00, 0x40, 0x00, 0x0050),
        (0x20, 0x01, 0x40, 0x00, 0x00F5),
    ]
    for volume, byte_24668, max_volume, stored_34, value in cases:
        wrapper = make_wrapper(original_offset("eff_13E32"), setup_eff_13e32(volume, byte_24668, max_volume, stored_34, value))
        out, channel = original_run(wrapper, dump_count=0x35, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09] + channel[0x34:0x35]
        translated_out = translated("eff13e32", hex(volume), hex(byte_24668), hex(max_volume), hex(stored_34), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13e32_public_symbol_volume_memory_delta_helper() -> None:
    cases = [
        (0x20, 0x00, 0x40, 0x00, 0x0005),
        (0x20, 0x00, 0x40, 0x00, 0x0050),
        (0x20, 0x01, 0x40, 0x00, 0x00F5),
    ]
    for volume, byte_24668, max_volume, stored_34, value in cases:
        wrapper = make_wrapper(original_offset("eff_13E32"), setup_eff_13e32(volume, byte_24668, max_volume, stored_34, value))
        out, channel = original_run(wrapper, dump_count=0x35, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09] + channel[0x34:0x35]
        got = translated("abieff13e32", hex(volume), hex(byte_24668), hex(max_volume), hex(stored_34), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13e7f_volume_and_target_slide_helper() -> None:
    cases = [
        (0x0200, 0x0250, 0x0008, 0x30, 0x20, 0x00, 0x40, 0x00, 0x0005),
        (0x0300, 0x0200, 0x0004, 0x00, 0x20, 0x01, 0x40, 0x00, 0x00F5),
    ]
    for period, target, step, flags, volume, byte_24668, max_volume, stored_34, value in cases:
        wrapper = make_wrapper(
            original_offset("eff_13E7F"),
            setup_eff_13e7f(period, target, step, flags, volume, byte_24668, max_volume, stored_34, value),
        )
        out, channel = original_run(wrapper, dump_count=0x35, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x08:0x09] + channel[0x10:0x14] + channel[0x17:0x18] + channel[0x34:0x35]
        translated_out = translated(
            "eff13e7f",
            hex(period),
            hex(target),
            hex(step),
            hex(flags),
            hex(volume),
            hex(byte_24668),
            hex(max_volume),
            hex(stored_34),
            hex(value),
        )
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13e7f_public_symbol_volume_and_target_slide_helper() -> None:
    cases = [
        (0x0200, 0x0250, 0x0008, 0x30, 0x20, 0x00, 0x40, 0x00, 0x0005),
        (0x0300, 0x0200, 0x0004, 0x00, 0x20, 0x01, 0x40, 0x00, 0x00F5),
    ]
    for period, target, step, flags, volume, byte_24668, max_volume, stored_34, value in cases:
        wrapper = make_wrapper(
            original_offset("eff_13E7F"),
            setup_eff_13e7f(period, target, step, flags, volume, byte_24668, max_volume, stored_34, value),
        )
        out, channel = original_run(wrapper, dump_count=0x35, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x08:0x09] + channel[0x10:0x14] + channel[0x17:0x18] + channel[0x34:0x35]
        got = translated(
            "abieff13e7f",
            hex(period),
            hex(target),
            hex(step),
            hex(flags),
            hex(volume),
            hex(byte_24668),
            hex(max_volume),
            hex(stored_34),
            hex(value),
        )
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13e84_volume_and_vibrato_helper() -> None:
    cases = [
        (0x0200, 0x00, 0x34, 0x04, 0x00, 0x20, 0x00, 0x40, 0x00, 0x0005),
        (0x0200, 0x01, 0x25, 0x84, 0x00, 0x20, 0x01, 0x40, 0x00, 0x00F5),
    ]
    for period, byte_09, byte_0c, byte_0d, playsettings, volume, byte_24668, max_volume, stored_34, value in cases:
        wrapper = make_wrapper(
            original_offset("eff_13E84"),
            setup_eff_13e84(period, byte_09, byte_0c, byte_0d, playsettings, volume, byte_24668, max_volume, stored_34, value),
        )
        out, channel = original_run(wrapper, dump_count=0x35, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x08:0x09] + channel[0x0C:0x0E] + channel[0x34:0x35]
        translated_out = translated(
            "eff13e84",
            hex(period),
            hex(byte_09),
            hex(byte_0c),
            hex(byte_0d),
            hex(playsettings),
            hex(volume),
            hex(byte_24668),
            hex(max_volume),
            hex(stored_34),
            hex(value),
        )
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13e84_public_symbol_volume_and_vibrato_helper() -> None:
    cases = [
        (0x0200, 0x00, 0x34, 0x04, 0x00, 0x20, 0x00, 0x40, 0x00, 0x0005),
        (0x0200, 0x01, 0x25, 0x84, 0x00, 0x20, 0x01, 0x40, 0x00, 0x00F5),
    ]
    for period, byte_09, byte_0c, byte_0d, playsettings, volume, byte_24668, max_volume, stored_34, value in cases:
        wrapper = make_wrapper(
            original_offset("eff_13E84"),
            setup_eff_13e84(period, byte_09, byte_0c, byte_0d, playsettings, volume, byte_24668, max_volume, stored_34, value),
        )
        out, channel = original_run(wrapper, dump_count=0x35, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x08:0x09] + channel[0x0C:0x0E] + channel[0x34:0x35]
        got = translated(
            "abieff13e84",
            hex(period),
            hex(byte_09),
            hex(byte_0c),
            hex(byte_0d),
            hex(playsettings),
            hex(volume),
            hex(byte_24668),
            hex(max_volume),
            hex(stored_34),
            hex(value),
        )
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13e8c_tempo_ratio_helper() -> None:
    cases = [
        (0x0012, 22050, 0x0400),
        (0x007f, 11025, 0x0200),
    ]
    for value, freq, buffer_size in cases:
        wrapper = make_wrapper(original_offset("eff_13E8C"), setup_eff_13e8c(value, freq, buffer_size))
        out, globals_ = original_run(wrapper, dump_count=0x85, dump_offset=0x0044, dump_seg=DATA_SEG)
        expected = globals_[0x4A - 0x44 : 0x50 - 0x44] + globals_[0x44:0x46] + globals_[0xC6 - 0x44 : 0xC9 - 0x44]
        translated_out = translated("eff13e8c", hex(value), hex(freq), hex(buffer_size))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13e8c_public_symbol_tempo_ratio_helper() -> None:
    cases = [
        (0x0012, 22050, 0x0400),
        (0x007f, 11025, 0x0200),
    ]
    for value, freq, buffer_size in cases:
        wrapper = make_wrapper(original_offset("eff_13E8C"), setup_eff_13e8c(value, freq, buffer_size))
        out, globals_ = original_run(wrapper, dump_count=0x85, dump_offset=0x0044, dump_seg=DATA_SEG)
        expected = globals_[0x4A - 0x44 : 0x50 - 0x44] + globals_[0x44:0x46] + globals_[0xC6 - 0x44 : 0xC9 - 0x44]
        got = translated("abieff13e8c", hex(value), hex(freq), hex(buffer_size))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13f05_retrigger_volume_gate_helper() -> None:
    cases = [
        (0x20, 0x02, 0x00, 0x0031),
        (0x20, 0x03, 0x00, 0x0031),
        (0x20, 0x03, 0x31, 0x0000),
    ]
    for volume, byte_24668, stored_34, value in cases:
        wrapper = make_wrapper(original_offset("eff_13F05"), setup_eff_13f05(volume, byte_24668, stored_34, value))
        out, channel = original_run(wrapper, dump_count=0x35, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09] + channel[0x34:0x35]
        translated_out = translated("eff13f05", hex(volume), hex(byte_24668), hex(stored_34), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13f05_public_symbol_retrigger_volume_gate_helper() -> None:
    cases = [
        (0x20, 0x02, 0x00, 0x0031),
        (0x20, 0x03, 0x00, 0x0031),
        (0x20, 0x03, 0x31, 0x0000),
    ]
    for volume, byte_24668, stored_34, value in cases:
        wrapper = make_wrapper(original_offset("eff_13F05"), setup_eff_13f05(volume, byte_24668, stored_34, value))
        out, channel = original_run(wrapper, dump_count=0x35, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09] + channel[0x34:0x35]
        got = translated("abieff13f05", hex(volume), hex(byte_24668), hex(stored_34), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13f3b_fine_volume_and_callback_helper() -> None:
    cases = [
        (0x20, 0x00, 0x40, 0x00, 0x00, 0x0014),
        (0x20, 0x00, 0x40, 0x00, 0x00, 0x0064),
        (0x20, 0x00, 0x40, 0x00, 0x00, 0x00E4),
    ]
    for volume, byte_24668, max_volume, flags_3d, stored_34, value in cases:
        wrapper = make_wrapper(original_offset("eff_13F3B"), setup_eff_13f3b(volume, byte_24668, max_volume, flags_3d, stored_34, value))
        out, channel = original_run(wrapper, dump_count=0x3E, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09] + channel[0x34:0x35] + channel[0x3D:0x3E]
        translated_out = translated("eff13f3b", hex(volume), hex(byte_24668), hex(max_volume), hex(flags_3d), hex(stored_34), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13f3b_public_symbol_fine_volume_and_callback_helper() -> None:
    cases = [
        (0x20, 0x00, 0x40, 0x00, 0x00, 0x0014),
        (0x20, 0x00, 0x40, 0x00, 0x00, 0x0064),
        (0x20, 0x00, 0x40, 0x00, 0x00, 0x00E4),
    ]
    for volume, byte_24668, max_volume, flags_3d, stored_34, value in cases:
        wrapper = make_wrapper(original_offset("eff_13F3B"), setup_eff_13f3b(volume, byte_24668, max_volume, flags_3d, stored_34, value))
        out, channel = original_run(wrapper, dump_count=0x3E, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0x08:0x09] + channel[0x34:0x35] + channel[0x3D:0x3E]
        got = translated("abieff13f3b", hex(volume), hex(byte_24668), hex(max_volume), hex(flags_3d), hex(stored_34), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13fbe_pattern_arpeggio_helper() -> None:
    cases = [
        (0x0200, 0x00, 0x00, 0x00, 0x31, 0x0000),
        (0x0200, 0x31, 0x01, 0x00, 0x31, 0x0012),
        (0x0200, 0x31, 0x03, 0x00, 0x31, 0x0000),
    ]
    for period, byte_0b, byte_24668, stored_34, byte_35, value in cases:
        wrapper = make_wrapper(original_offset("eff_13FBE"), setup_eff_13fbe(period, byte_0b, byte_24668, stored_34, byte_35, value))
        out, channel = original_run(wrapper, dump_count=0x36, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x0B:0x0C] + channel[0x34:0x36]
        translated_out = translated("eff13fbe", hex(period), hex(byte_0b), hex(byte_24668), hex(stored_34), hex(byte_35), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13fbe_public_symbol_pattern_arpeggio_helper() -> None:
    cases = [
        (0x0200, 0x00, 0x00, 0x00, 0x31, 0x0000),
        (0x0200, 0x31, 0x01, 0x00, 0x31, 0x0012),
        (0x0200, 0x31, 0x03, 0x00, 0x31, 0x0000),
    ]
    for period, byte_0b, byte_24668, stored_34, byte_35, value in cases:
        wrapper = make_wrapper(original_offset("eff_13FBE"), setup_eff_13fbe(period, byte_0b, byte_24668, stored_34, byte_35, value))
        out, channel = original_run(wrapper, dump_count=0x36, dump_offset=CHANNEL_OFF, dump_seg=DATA_SEG)
        expected = channel[0:2] + channel[0x0B:0x0C] + channel[0x34:0x36]
        got = translated("abieff13fbe", hex(period), hex(byte_0b), hex(byte_24668), hex(stored_34), hex(byte_35), hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_14020_amplification_helper() -> None:
    cases = [
        (0x0019, 0x00, 0x0004),
        (0x0020, 0x01, 0x0004),
    ]
    for value, sound_mode, channels in cases:
        wrapper = make_wrapper(original_offset("eff_14020"), setup_eff_14020(value, sound_mode, channels))
        out, globals_ = original_run(wrapper, dump_count=0x80, dump_offset=0x005E, dump_seg=DATA_SEG)
        expected = globals_[0:2] + globals_[0x85 - 0x5E : 0x86 - 0x5E] + globals_[0xDD - 0x5E : 0xDE - 0x5E]
        translated_out = translated("eff14020", hex(value), hex(sound_mode), hex(channels))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_14020_public_symbol_amplification_helper() -> None:
    cases = [
        (0x0019, 0x00, 0x0004),
        (0x0020, 0x01, 0x0004),
    ]
    for value, sound_mode, channels in cases:
        wrapper = make_wrapper(original_offset("eff_14020"), setup_eff_14020(value, sound_mode, channels))
        out, globals_ = original_run(wrapper, dump_count=0x80, dump_offset=0x005E, dump_seg=DATA_SEG)
        expected = globals_[0:2] + globals_[0x85 - 0x5E : 0x86 - 0x5E] + globals_[0xDD - 0x5E : 0xDE - 0x5E]
        got = translated("abiamplif", "eff_14020", hex(value), hex(sound_mode), hex(channels))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_change_amplif_far_entry() -> None:
    cases = [
        (0x0019, 0x00, 0x0004),
        (0x0065, 0x01, 0x0004),
        (0x0080, 0x02, 0x0006),
    ]
    for value, sound_mode, channels in cases:
        wrapper = make_far_wrapper(original_offset("change_amplif"), setup_change_amplif(value, sound_mode, channels))
        out, globals_ = original_run(wrapper, dump_count=0x80, dump_offset=0x005E, dump_seg=DATA_SEG)
        expected = globals_[0:2] + globals_[0x85 - 0x5E : 0x86 - 0x5E] + globals_[0xDD - 0x5E : 0xDE - 0x5E]
        translated_out = translated("changeamplif", hex(value), hex(sound_mode), hex(channels))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_change_amplif_public_symbol_far_entry() -> None:
    cases = [
        (0x0019, 0x00, 0x0004),
        (0x0065, 0x01, 0x0004),
        (0x0080, 0x02, 0x0006),
    ]
    for value, sound_mode, channels in cases:
        wrapper = make_far_wrapper(original_offset("change_amplif"), setup_change_amplif(value, sound_mode, channels))
        out, globals_ = original_run(wrapper, dump_count=0x80, dump_offset=0x005E, dump_seg=DATA_SEG)
        expected = globals_[0:2] + globals_[0x85 - 0x5E : 0x86 - 0x5E] + globals_[0xDD - 0x5E : 0xDE - 0x5E]
        got = translated("abiamplif", "change_amplif", hex(value), hex(sound_mode), hex(channels))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_calc_14043_tempo_sum_helper() -> None:
    cases = [
        (0x10, 0x00),
        (0x20, 0x03),
        (0xFF, 0x02),
    ]
    for byte_2467b, byte_2467c in cases:
        wrapper = make_wrapper(original_offset("calc_14043"), setup_calc_14043(byte_2467b, byte_2467c))
        out, globals_ = original_run(wrapper, dump_count=2, dump_offset=0x00DB, dump_seg=DATA_SEG)
        translated_out = translated("calc14043", hex(byte_2467b), hex(byte_2467c))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + globals_.hex())


def test_original_and_abi_calc_14043_public_symbol_tempo_sum_helper() -> None:
    cases = [
        (0x10, 0x00),
        (0x20, 0x03),
        (0xFF, 0x02),
    ]
    for byte_2467b, byte_2467c in cases:
        wrapper = make_wrapper(original_offset("calc_14043"), setup_calc_14043(byte_2467b, byte_2467c))
        out, globals_ = original_run(wrapper, dump_count=2, dump_offset=0x00DB, dump_seg=DATA_SEG)
        got = translated("abicalc14043", hex(byte_2467b), hex(byte_2467c))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == globals_.hex()


def test_original_and_translated_eff_14030_set_base_tempo_helper() -> None:
    cases = [
        (0x0000, 0x00, 22050, 0x0400),
        (0x000F, 0x02, 11025, 0x0200),
    ]
    for value, byte_2467c, freq, buffer_size in cases:
        wrapper = make_wrapper(original_offset("eff_14030"), setup_eff_14030(value, byte_2467c, freq, buffer_size))
        out, globals_ = original_run(wrapper, dump_count=0x99, dump_offset=0x0044, dump_seg=DATA_SEG)
        expected = (
            globals_[0xDB - 0x44 : 0xDD - 0x44]
            + globals_[0x4A - 0x44 : 0x50 - 0x44]
            + globals_[0x44:0x46]
            + globals_[0xC6 - 0x44 : 0xC7 - 0x44]
        )
        translated_out = translated("eff14030", hex(value), hex(byte_2467c), hex(freq), hex(buffer_size))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_14030_public_symbol_set_base_tempo_helper() -> None:
    cases = [
        (0x0000, 0x00, 22050, 0x0400),
        (0x000F, 0x02, 11025, 0x0200),
    ]
    for value, byte_2467c, freq, buffer_size in cases:
        wrapper = make_wrapper(original_offset("eff_14030"), setup_eff_14030(value, byte_2467c, freq, buffer_size))
        out, globals_ = original_run(wrapper, dump_count=0x99, dump_offset=0x0044, dump_seg=DATA_SEG)
        expected = (
            globals_[0xDB - 0x44 : 0xDD - 0x44]
            + globals_[0x4A - 0x44 : 0x50 - 0x44]
            + globals_[0x44:0x46]
            + globals_[0xC6 - 0x44 : 0xC7 - 0x44]
        )
        got = translated("abieff14030", hex(value), hex(byte_2467c), hex(freq), hex(buffer_size))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_14067_adjust_base_tempo_helper() -> None:
    cases = [
        (0x0000, 0x20, 0x05, 22050, 0x0400),
        (0x0003, 0x20, 0x05, 22050, 0x0400),
        (0x0030, 0x20, 0x05, 11025, 0x0200),
    ]
    for value, byte_2467b, byte_2467c, freq, buffer_size in cases:
        wrapper = make_wrapper(original_offset("eff_14067"), setup_eff_14067(value, byte_2467b, byte_2467c, freq, buffer_size))
        out, globals_ = original_run(wrapper, dump_count=0x99, dump_offset=0x0044, dump_seg=DATA_SEG)
        expected = (
            globals_[0xDB - 0x44 : 0xDD - 0x44]
            + globals_[0x4A - 0x44 : 0x50 - 0x44]
            + globals_[0x44:0x46]
            + globals_[0xC6 - 0x44 : 0xC7 - 0x44]
        )
        translated_out = translated("eff14067", hex(value), hex(byte_2467b), hex(byte_2467c), hex(freq), hex(buffer_size))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_14067_public_symbol_adjust_base_tempo_helper() -> None:
    cases = [
        (0x0000, 0x20, 0x05, 22050, 0x0400),
        (0x0003, 0x20, 0x05, 22050, 0x0400),
        (0x0030, 0x20, 0x05, 11025, 0x0200),
    ]
    for value, byte_2467b, byte_2467c, freq, buffer_size in cases:
        wrapper = make_wrapper(original_offset("eff_14067"), setup_eff_14067(value, byte_2467b, byte_2467c, freq, buffer_size))
        out, globals_ = original_run(wrapper, dump_count=0x99, dump_offset=0x0044, dump_seg=DATA_SEG)
        expected = (
            globals_[0xDB - 0x44 : 0xDD - 0x44]
            + globals_[0x4A - 0x44 : 0x50 - 0x44]
            + globals_[0x44:0x46]
            + globals_[0xC6 - 0x44 : 0xC7 - 0x44]
        )
        got = translated("abieff14067", hex(value), hex(byte_2467b), hex(byte_2467c), hex(freq), hex(buffer_size))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_eff_13ce8_global_counter_helper() -> None:
    cases = [
        (0x77, 0x88, 0x0000, 0x77, 0x88),
        (0x77, 0x88, 0x0012, 0x12, 0x00),
    ]
    for initial_24667, initial_24668, value, expected_24667, expected_24668 in cases:
        wrapper = make_wrapper(original_offset("eff_13CE8"), setup_eff_13ce8(initial_24667, initial_24668, value))
        out, globals_ = original_run(wrapper, dump_count=2, dump_offset=0x00C7, dump_seg=DATA_SEG)
        expected = bytes([expected_24667, expected_24668])
        assert globals_ == expected
        translated_out = translated("eff13ce8", hex(initial_24667), hex(initial_24668), hex(value))
        if translated_out is not None:
            assert field(translated_out, "ax") == field(out, "ax")
            assert translated_out.endswith("data=" + expected.hex())


def test_original_and_abi_eff_13ce8_public_symbol_global_counter_helper() -> None:
    cases = [
        (0x77, 0x88, 0x0000, 0x77, 0x88),
        (0x77, 0x88, 0x0012, 0x12, 0x00),
    ]
    for initial_24667, initial_24668, value, expected_24667, expected_24668 in cases:
        wrapper = make_wrapper(original_offset("eff_13CE8"), setup_eff_13ce8(initial_24667, initial_24668, value))
        out, globals_ = original_run(wrapper, dump_count=2, dump_offset=0x00C7, dump_seg=DATA_SEG)
        expected = bytes([expected_24667, expected_24668])
        got = translated("abieff13ce8", hex(initial_24667), hex(initial_24668), hex(value))
        assert globals_ == expected
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == expected.hex()


def test_original_and_translated_strlen() -> None:
    for text in ["", "Inertia", "HACKER4.S3M"]:
        src = text.encode("ascii") + b"\0"
        out, _ = original_call(original_offset("mystrlen_0"), setup_strlen(), src=src)
        got = translated("strlen", text)
        assert field(out, "ax") == f"{len(text):04x}"
        if got is not None:
            assert field(got, "ax") == f"{len(text):04x}"


def test_original_and_translated_strcpy_count() -> None:
    for text in ["A", "Inertia Player"]:
        src = text.encode("ascii") + b"\0"
        out, copied = original_call(original_offset("strcpy_count_0"), setup_strcpy(), src=src, dump_count=len(src))
        got = translated("strcpy", text)
        assert copied == text.encode("ascii") + b"."
        assert field(out, "cx") == f"{len(src):04x}"
        if got is not None:
            assert field(got, "cx") == f"{len(src):04x}"
            assert got.endswith("data=" + (text.encode("ascii") + b".").hex())


def test_original_and_abi_strcpy_count_public_symbols() -> None:
    for text in ["A", "Inertia"]:
        src = text.encode("ascii") + b"\0"
        out, copied = original_call(original_offset("strcpy_count_0"), setup_strcpy(), src=src, dump_count=len(src))
        got = translated("abistrcpy", "strcpy_count_0", text)
        if got is not None:
            assert got.endswith("data=" + copied.hex())
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_translated_copy_printable() -> None:
    text = b"ABC\x1fDEF"
    count = 6
    expected = b"ABC" + b"." * (count - 3)
    _, copied = original_call(original_offset("copy_printable"), setup_copyprint(count), src=text, dump_count=count)
    got = translated("copyprint", text.decode("latin1"), str(count))
    assert copied == expected
    if got is not None:
        assert got.endswith("data=" + expected.hex())


def test_original_and_abi_copy_printable_public_symbol() -> None:
    text = b"ABC\x1fDEF"
    count = 6
    out, copied = original_call(original_offset("copy_printable"), setup_copyprint(count), src=text, dump_count=count)
    got = translated("abicopyprint", "copy_printable", text.hex(), str(count))
    if got is not None:
        assert got.endswith("data=" + copied.hex())
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")


def setup_myasmsprintf() -> tuple[bytes, bytes]:
    fmt = bytearray()
    fmt += b"U="
    fmt += bytes([4, ord("u")]) + struct.pack("<H", DSEG_SCRATCH + 0x50)
    fmt += b" I="
    fmt += bytes([8, ord("i")]) + struct.pack("<H", DSEG_SCRATCH + 0x52)
    fmt += b" X="
    fmt += bytes([11, ord("x")]) + struct.pack("<H", DSEG_SCRATCH + 0x54)
    fmt += b"\0\0"
    setup = setup_data_common()
    for index, value in enumerate(fmt):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    setup += mov_ds_byte(DSEG_SCRATCH + 0x50, 200)
    setup += mov_ds_word(DSEG_SCRATCH + 0x52, 0xFB2E)
    setup += mov_ds_word(DSEG_SCRATCH + 0x54, 0xABCD)
    return setup + mov_si(DSEG_SCRATCH) + mov_di(DSEG_SCRATCH + 0x40), b"U=200 I=-1234 X=ABCD"


def test_original_and_translated_myasmsprintf_control_formatting() -> None:
    setup, expected = setup_myasmsprintf()
    out, data = original_run(
        make_wrapper(original_offset("myasmsprintf"), setup),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x40,
        dump_seg=DATA_SEG,
    )
    got = translated("myasmsprintf")
    assert data == expected
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert got.endswith("data=" + expected.hex())


def test_original_and_abi_myasmsprintf_public_symbol_control_formatting() -> None:
    setup, expected = setup_myasmsprintf()
    out, data = original_run(
        make_wrapper(original_offset("myasmsprintf"), setup),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x40,
        dump_seg=DATA_SEG,
    )
    got = translated("abimyasmsprintf")
    assert data == expected
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert got.endswith("data=" + expected.hex())


def setup_useless_mysprintf() -> tuple[bytes, bytes]:
    fmt = bytearray()
    fmt += b"U="
    fmt += bytes([4, ord("u")]) + struct.pack("<H", DSEG_SCRATCH + 0x50)
    fmt += b" I="
    fmt += bytes([8, ord("i")]) + struct.pack("<H", DSEG_SCRATCH + 0x52)
    fmt += b" X="
    fmt += bytes([11, ord("x")]) + struct.pack("<H", DSEG_SCRATCH + 0x54)
    fmt += b"\0\0"
    setup = setup_dseg_common()
    for index, value in enumerate(fmt):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    setup += mov_ds_byte(DSEG_SCRATCH + 0x50, 200)
    setup += mov_ds_word(DSEG_SCRATCH + 0x52, 0xFB2E)
    setup += mov_ds_word(DSEG_SCRATCH + 0x54, 0xABCD)
    return setup + mov_si(DSEG_SCRATCH) + mov_di(DSEG_SCRATCH + 0x40), b"U=200 I=-1234 X=ABCD"


def test_original_useless_mysprintf_matches_translated_formatter_contract() -> None:
    setup, expected = setup_useless_mysprintf()
    _, data = original_seg001_call(
        original_offset("useless_mysprintf"),
        setup,
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x40,
        dump_seg=DSEG,
    )
    got = translated("myasmsprintf")
    assert data == expected
    if got is not None:
        assert "data=" + expected.hex() in got


def test_original_and_translated_seg001_cpy_printable() -> None:
    text = b"ABC\x1fDEF"
    count = 6
    expected = b"ABC" + b" " * (count - 3)
    out, copied = original_seg001_call(
        original_offset("cpy_printable"),
        setup_dseg_copyprint(text, count),
        dump_count=count,
        dump_offset=DSEG_SCRATCH + 0x40,
        dump_seg=DSEG,
    )
    got = translated("seg1copyprint", text.decode("latin1"), str(count))
    assert copied == expected
    if got is not None:
        assert got.endswith("data=" + expected.hex())
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")


def test_original_and_abi_seg001_cpy_printable_public_symbol() -> None:
    text = b"ABC\x1fDEF"
    count = 6
    out, copied = original_seg001_call(
        original_offset("cpy_printable"),
        setup_dseg_copyprint(text, count),
        dump_count=count,
        dump_offset=DSEG_SCRATCH + 0x40,
        dump_seg=DSEG,
    )
    got = translated("abicopyprint", "cpy_printable", text.hex(), str(count))
    if got is not None:
        assert got.endswith("data=" + copied.hex())
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")


def test_original_and_translated_seg001_strcpy_count() -> None:
    for text in [b"", b"Inertia", b"ABC def"]:
        out, copied = original_seg001_call(
            original_offset("strcpy_count"),
            setup_dseg_strcpy_count(text),
            dump_count=len(text),
            dump_offset=DSEG_SCRATCH + 0x40,
            dump_seg=DSEG,
        )
        got = translated("seg1strcpycount", text.decode("latin1"))
        assert copied == text
        if got is not None:
            assert got.endswith("data=" + text.hex())
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")
        assert field(out, "cx") == f"{len(text) + 1:04x}"


def test_original_and_abi_seg001_strcpy_count_public_symbol() -> None:
    for text in [b"", b"Inertia", b"ABCdef"]:
        out, copied = original_seg001_call(
            original_offset("strcpy_count"),
            setup_dseg_strcpy_count(text),
            dump_count=len(text),
            dump_offset=DSEG_SCRATCH + 0x40,
            dump_seg=DSEG,
        )
        got = translated("abistrcpy", "strcpy_count", text.decode("latin1"))
        if got is not None:
            assert got.endswith("data=" + copied.hex())
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_translated_seg001_mystrlen() -> None:
    for text in [b"", b"Inertia", b"ABC def"]:
        out, _ = original_seg001_call(original_offset("mystrlen"), setup_dseg_strlen(text), dump_count=0)
        got = translated("seg1strlen", text.decode("latin1"))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "si") == field(out, "si")
        assert field(out, "ax") == f"{len(text):04x}"
        assert field(out, "si") == f"{DSEG_SCRATCH:04x}"


def setup_parse_cmdline(command: bytes) -> bytes:
    tail = command + b"\r"
    setup = setup_dseg_common() + mov_ds_word(0x164A, DSEG)
    setup += mov_ds_byte(0x0080, len(command))
    for index, value in enumerate(tail):
        setup += mov_ds_byte(0x0081 + index, value)
    for index in range(32):
        setup += mov_ds_byte(0x137C + index, 0x2E)
    return setup


def test_original_and_translated_parse_cmdline_psp_tail() -> None:
    cases = [
        (b"", b"", 0x0000),
        (b" /d /l song.s3m", b"song.s3m", 0x0808),
    ]
    for command, expected_name, expected_bp in cases:
        out, data = original_seg001_call(
            original_offset("parse_cmdline"),
            setup_parse_cmdline(command),
            dump_count=max(1, len(expected_name) + 1),
            dump_offset=0x137C,
            dump_seg=DSEG,
        )
        got = translated("parsecmdline", command.decode("ascii"))
        assert data == expected_name + b"\0"
        assert int(field(out, "bp"), 16) == expected_bp
        if got is not None:
            assert field(got, "bp") == field(out, "bp")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")
            assert got.endswith("data=" + data.hex())


def setup_get_comspec() -> bytes:
    env = b"COMSPEC=X\0\0"
    setup = setup_dseg_common() + mov_ds_word(0x164A, DSEG) + mov_ds_word(0x002C, DSEG)
    for index, value in enumerate(env):
        setup += mov_ds_byte(index, value)
    return setup


def test_original_and_translated_get_comspec_environment_lookup() -> None:
    out, _ = original_seg001_call(original_offset("get_comspec"), setup_get_comspec(), dump_count=0)
    got = translated("getcomspec")
    assert field(out, "di") == "0008"
    if got is not None:
        assert field(got, "di") == field(out, "di")


def test_original_and_abi_get_comspec_public_symbol_environment_lookup() -> None:
    out, _ = original_seg001_call(original_offset("get_comspec"), setup_get_comspec(), dump_count=0)
    got = translated("abigetcomspec")
    assert field(out, "di") == "0008"
    if got is not None:
        assert field(got, "di") == field(out, "di")


def setup_getexename(path: bytes) -> bytes:
    env = b"A=B\0\0" + struct.pack("<H", 1) + path + b"\0"
    setup = setup_dseg_common() + mov_ds_word(0x164A, DSEG) + mov_ds_word(0x002C, DSEG)
    for index, value in enumerate(env):
        setup += mov_ds_byte(index, value)
    return setup + mov_si(DSEG_SCRATCH)


def test_original_and_translated_getexename_environment_path_copy() -> None:
    for path in [b"C:\\IPLAY.EXE", b"D:\\DIR\\PLAYER.EXE"]:
        out, data = original_seg001_call(
            original_offset("getexename"),
            setup_getexename(path),
            dump_count=len(path) + 1,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DSEG,
        )
        got = translated("getexename", path.decode("ascii"))
        assert data == path + b"\0"
        if got is not None:
            assert field(got, "si") == field(out, "si")
            assert got.endswith("data=" + data.hex())


def test_original_and_abi_getexename_public_symbol_environment_path_copy() -> None:
    path = b"C:\\IPLAY.EXE"
    out, data = original_seg001_call(
        original_offset("getexename"),
        setup_getexename(path),
        dump_count=len(path) + 1,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
    )
    got = translated("abigetexename")
    assert data == path + b"\0"
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert got.endswith("data=" + data.hex())


def wrapper_int2f_checkmyself(dl_value: int) -> bytes:
    pre = setup_dseg_common() + mov_ds_byte(0x168C, 0)
    regs = mov_ax(0x60FF) + mov_bx(0x5344) + mov_cx(0x4D50) + mov_dx(dl_value)
    frame_len = 9
    continuation_ip = WRAPPER_IP + len(pre) + frame_len + len(regs) + 3
    frame = b"\x9c" + mov_di(LOAD_SEG + SEG001_DELTA) + b"\x57" + mov_di(continuation_ip) + b"\x57"
    return pre + frame + regs + jmp_rel16(original_offset("int2f_checkmyself"), continuation_ip) + b"\xc3"


def test_original_and_translated_int2f_checkmyself_signature_paths() -> None:
    for dl_value, expected_ax, expected_flag in [
        (0, 0x4F4B, 0),
        (1, 0x60FF, 1),
    ]:
        out, data = original_run(
            wrapper_int2f_checkmyself(dl_value),
            dump_count=1,
            dump_offset=0x168C,
            dump_seg=DSEG,
            call_cs=LOAD_SEG + SEG001_DELTA,
            wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        )
        got = translated("int2fcheck", hex(dl_value))
        assert int(field(out, "ax"), 16) == expected_ax
        assert data == bytes([expected_flag])
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert got.endswith("data=" + data.hex())


def setup_sub_12d35_disable(config_word: int) -> bytes:
    return setup_data_common() + mov_ds_word(0x013A, config_word) + mov_ax(0)


def test_original_and_translated_sub_12d35_disable_exec_mode() -> None:
    for config_word in [0x0000, 0x0001]:
        out, data = original_run(
            make_far_wrapper(original_offset("sub_12D35"), setup_sub_12d35_disable(config_word)),
            dump_count=1,
            dump_offset=0x4F71,
            dump_seg=LOAD_SEG,
        )
        got = translated("sub12d35disable", hex(config_word))
        assert data == b"\0"
        assert field(out, "ax") == "0000"
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert got.endswith("data=" + data.hex())


def test_original_and_abi_sub_12d35_public_symbol_disable_exec_mode() -> None:
    for config_word in [0x0000, 0x0001]:
        out, data = original_run(
            make_far_wrapper(original_offset("sub_12D35"), setup_sub_12d35_disable(config_word)),
            dump_count=1,
            dump_offset=0x4F71,
            dump_seg=LOAD_SEG,
        )
        got = translated("abisub12d35disable", hex(config_word))
        assert data == b"\0"
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "data") == data.hex()


def wrapper_spectr_1b406_small(payload: bytes) -> bytes:
    setup = setup_dseg_common() + mov_ds_word(0x7D30, 1)
    for index, value in enumerate((payload + bytes(8))[:8]):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    setup += mov_di(DSEG_SCRATCH)
    call_ip = WRAPPER_IP + len(setup)
    return (
        setup
        + call_rel16(original_offset("spectr_1B406"), call_ip + 3)
        + copy_bytes_to_scratch(0x7D1C, DSEG_SCRATCH + 8, 0x18)
        + b"\xc3"
    )


def test_original_and_translated_spectr_1b406_small_fft_setup() -> None:
    payload = bytes.fromhex("0102030405060708")
    _, data = original_run(
        wrapper_spectr_1b406_small(payload),
        dump_count=0x20,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    got = translated("spectr1b406small", payload.hex())
    assert data[:8] == payload
    if got is not None:
        assert got.endswith("data=" + data.hex())


def test_original_and_abi_spectr_1b406_public_symbol_small_fft_setup() -> None:
    payload = bytes.fromhex("0102030405060708")
    _, data = original_run(
        wrapper_spectr_1b406_small(payload),
        dump_count=0x20,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    got = translated("abispectr1b406small", payload.hex())
    assert data[:8] == payload
    if got is not None:
        assert got.endswith("data=" + data.hex())


def test_original_and_translated_spectr_1c4f8_integer_sqrt() -> None:
    for value in [0, 1, 2, 3, 4, 15, 16, 17, 65535, 65536, 0xFFFFFFFF]:
        out, _ = original_seg001_call(original_offset("spectr_1C4F8"), setup_dseg_common() + mov_ebx(value), dump_count=0)
        got = translated("spectrsqrt", hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
        root = math.isqrt(value)
        expected = root + (1 if value - root * root > root else 0)
        assert int(field(out, "ax"), 16) == (expected & 0xFFFF)


def test_original_and_abi_spectr_1c4f8_public_symbol_integer_sqrt() -> None:
    for value in [0, 1, 2, 3, 4, 15, 16, 17, 65535, 65536, 0xFFFFFFFF]:
        out, _ = original_seg001_call(original_offset("spectr_1C4F8"), setup_dseg_common() + mov_ebx(value), dump_count=0)
        got = translated("abispectrsqrt", hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
        root = math.isqrt(value)
        expected = root + (1 if value - root * root > root else 0)
        assert int(field(out, "ax"), 16) == (expected & 0xFFFF)


def expected_sub_13e9b(value: int) -> int:
    table = [140, 50, 25, 15, 10, 7, 6, 4, 3, 3, 2, 2, 2, 2, 1, 1]
    low = value & 0x0F
    high = (value >> 4) & 0x0F
    al = (((0x31 - ((low * table[high]) >> 4)) * 5) >> 1) & 0xFF
    return (high << 8) | al


def test_original_and_translated_sub_13e9b_table_transform() -> None:
    for value in [0x00, 0x01, 0x0F, 0x10, 0x37, 0x7F, 0x80, 0xFE, 0xFF]:
        out, _ = original_call(original_offset("sub_13E9B"), setup_common() + mov_ax(value), dump_count=0)
        got = translated("sub13e9b", hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "di") == field(out, "di")
        assert int(field(out, "ax"), 16) == expected_sub_13e9b(value)


def test_original_and_abi_sub_13e9b_public_symbol_table_transform() -> None:
    for value in [0x00, 0x01, 0x0F, 0x10, 0x37, 0x7F, 0x80, 0xFE, 0xFF]:
        out, _ = original_call(original_offset("sub_13E9B"), setup_common() + mov_ax(value), dump_count=0)
        got = translated("abisub13e9b", hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "di") == field(out, "di")
        assert int(field(out, "ax"), 16) == expected_sub_13e9b(value)


def expected_sub_14087(value: int, stored: int, byte_24668: int) -> tuple[int, int]:
    effective = value & 0xFF
    stored_out = stored & 0xFF
    if effective:
        stored_out = effective
    else:
        effective = stored_out
    if byte_24668:
        return ((effective << 2) & 0xFFFF, stored_out) if effective < 0xE0 else (0, stored_out)
    if effective <= 0xE0:
        return 0, stored_out
    ax = effective & 0x0F
    if effective > 0xF0:
        ax = (ax << 2) & 0xFFFF
    return ax, stored_out


def test_original_and_translated_sub_14087_effect_value_transform() -> None:
    cases = [
        (0x00, 0x12, 1),
        (0x20, 0x99, 1),
        (0xE0, 0x55, 1),
        (0x00, 0xE1, 0),
        (0xE0, 0x33, 0),
        (0xEF, 0x44, 0),
        (0xF1, 0x44, 0),
    ]
    for value, stored, byte_24668 in cases:
        wrapper = make_wrapper(original_offset("sub_14087"), setup_sub_14087(value, stored, byte_24668))
        out, data = original_run(
            wrapper,
            dump_count=1,
            dump_offset=CHANNEL_OFF + 0x34,
            dump_seg=DATA_SEG,
        )
        got = translated("sub14087", hex(value), hex(stored), hex(byte_24668))
        expected_ax, expected_stored = expected_sub_14087(value, stored, byte_24668)
        assert int(field(out, "ax"), 16) == expected_ax
        assert data == bytes([expected_stored])
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "dx") == field(out, "dx")
            assert got.endswith("data=" + data.hex())


def test_original_and_abi_sub_14087_public_symbol_effect_value_transform() -> None:
    cases = [
        (0x00, 0x12, 1),
        (0x20, 0x99, 1),
        (0xE0, 0x55, 1),
        (0x00, 0xE1, 0),
        (0xE0, 0x33, 0),
        (0xEF, 0x44, 0),
        (0xF1, 0x44, 0),
    ]
    for value, stored, byte_24668 in cases:
        wrapper = make_wrapper(original_offset("sub_14087"), setup_sub_14087(value, stored, byte_24668))
        out, data = original_run(
            wrapper,
            dump_count=1,
            dump_offset=CHANNEL_OFF + 0x34,
            dump_seg=DATA_SEG,
        )
        got = translated("abisub14087", hex(value), hex(stored), hex(byte_24668))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def setup_sub_13044(mode: int, divisor: int, amplification: int, high_amplif: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x0036, divisor)
        + mov_ds_word(0x005E, amplification)
        + mov_ds_byte(0x0085, high_amplif)
        + mov_ds_byte(0x00DE, mode)
    )


def wrapper_sub_13044(mode: int, divisor: int, amplification: int, high_amplif: int) -> bytes:
    setup = setup_sub_13044(mode, divisor, amplification, high_amplif)
    call_ip = WRAPPER_IP + len(setup)
    return (
        setup
        + call_rel16(original_offset("sub_13044"), call_ip + 3)
        + copy_bytes_to_scratch(0x008E, DSEG_SCRATCH, 2)
        + copy_bytes_to_scratch(0x00B6, DSEG_SCRATCH + 2, 2)
        + copy_bytes_to_scratch(0x00DD, DSEG_SCRATCH + 4, 2)
        + copy_bytes_to_scratch(0x3D68, DSEG_SCRATCH + 6, 32)
        + b"\xc3"
    )


def test_original_and_translated_sub_13044_volume_table_generation() -> None:
    for mode, divisor, amplification, high_amplif in [
        (0, 2, 100, 0),
        (1, 4, 120, 0),
        (2, 8, 80, 0),
        (3, 16, 100, 0),
    ]:
        _, data = original_run(
            wrapper_sub_13044(mode, divisor, amplification, high_amplif),
            dump_count=38,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("sub13044", hex(mode), hex(divisor), hex(amplification), hex(high_amplif))
        if got is not None:
            assert got.endswith("data=" + data.hex())


def test_original_and_abi_sub_13044_public_symbol_volume_table_generation() -> None:
    for mode, divisor, amplification, high_amplif in [
        (0, 2, 100, 0),
        (1, 4, 120, 0),
        (2, 8, 80, 0),
        (3, 16, 100, 0),
    ]:
        _, data = original_run(
            wrapper_sub_13044(mode, divisor, amplification, high_amplif),
            dump_count=38,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("abisub13044", hex(mode), hex(divisor), hex(amplification), hex(high_amplif))
        if got is not None:
            assert got.endswith("data=" + data.hex())


def test_original_and_translated_put_message2_fs_string_writer() -> None:
    cases = [
        (b"A", 0x07),
        (b"ABC", 0x1E),
        (b"Level!", 0x70),
    ]
    for text, attr in cases:
        out, data = original_seg001_call(
            original_offset("put_message2"),
            setup_dseg_put_message2(text, attr),
            dump_count=len(text) * 2,
            dump_offset=DSEG_SCRATCH + 0x40,
            dump_seg=DSEG,
        )
        got = translated("putmessage2", text.decode("latin1"), hex(attr))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")
        assert field(out, "di") == f"{DSEG_SCRATCH + 0x40 + len(text) * 2:04x}"


def test_original_and_abi_put_message2_public_symbol() -> None:
    for text, attr in [(b"A", 0x07), (b"ABC", 0x1E), (b"Level!", 0x70)]:
        out, data = original_seg001_call(
            original_offset("put_message2"),
            setup_dseg_put_message2(text, attr),
            dump_count=len(text) * 2,
            dump_offset=DSEG_SCRATCH + 0x40,
            dump_seg=DSEG,
        )
        got = translated("abiputmessage", "put_message2", text.decode("latin1"), hex(attr))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_translated_text_1bf69_plain_text_writer() -> None:
    cases = [
        (b"A", 0x07),
        (b"ABC", 0x1E),
        (b"Level!", 0x70),
    ]
    for text, attr in cases:
        out, data = original_seg001_call(
            original_offset("text_1BF69"),
            setup_dseg_text1bf69(text, attr),
            dump_count=len(text) * 2,
            dump_offset=DSEG_SCRATCH + 0x40,
            dump_seg=DSEG,
        )
        got = translated("text1bf69", text.decode("latin1"), hex(attr))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")
        assert field(out, "di") == f"{DSEG_SCRATCH + 0x40 + len(text) * 2:04x}"


def test_original_and_abi_text_1bf69_public_symbol() -> None:
    for text, attr in [(b"A", 0x07), (b"ABC", 0x1E), (b"Level!", 0x70)]:
        out, data = original_seg001_call(
            original_offset("text_1BF69"),
            setup_dseg_text1bf69(text, attr),
            dump_count=len(text) * 2,
            dump_offset=DSEG_SCRATCH + 0x40,
            dump_seg=DSEG,
        )
        got = translated("abitext1bf69", text.hex(), hex(attr), str(len(text) * 2))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_translated_put_message_plain_text_writer() -> None:
    for text, attr in [(b"A", 0x07), (b"ABC", 0x1E), (b"Level!", 0x70)]:
        out, data = original_seg001_call(
            original_offset("put_message"),
            setup_dseg_put_message(text, attr),
            dump_count=len(text) * 2,
            dump_offset=DSEG_SCRATCH + 0x40,
            dump_seg=DSEG,
        )
        expected = b"".join(bytes([ch, attr]) for ch in text)
        got = translated("putmessage", text.decode("latin1"), hex(attr))
        assert data == expected
        if got is not None:
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_abi_put_message_public_symbol() -> None:
    for text, attr in [(b"A", 0x07), (b"ABC", 0x1E), (b"Level!", 0x70)]:
        out, data = original_seg001_call(
            original_offset("put_message"),
            setup_dseg_put_message(text, attr),
            dump_count=len(text) * 2,
            dump_offset=DSEG_SCRATCH + 0x40,
            dump_seg=DSEG,
        )
        got = translated("abiputmessage", "put_message", text.decode("latin1"), hex(attr))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_translated_draw_frame_scratch_video_buffer() -> None:
    style, attr, fill_attr, x, y, right, bottom = (3, 0x7F, 0x78, 1, 1, 5, 3)
    _, data = original_seg001_call(
        original_offset("draw_frame"),
        setup_dseg_draw_frame(style, attr, fill_attr, x, y, right, bottom),
        dump_count=400,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
    )
    got = translated("drawframe", hex(style), hex(attr), hex(fill_attr), hex(x), hex(y), hex(right), hex(bottom))
    if got is not None:
        assert got.endswith("data=" + data.hex())


def test_original_and_abi_draw_frame_public_symbol_scratch_video_buffer() -> None:
    style, attr, fill_attr, x, y, right, bottom = (3, 0x7F, 0x78, 1, 1, 5, 3)
    _, data = original_seg001_call(
        original_offset("draw_frame"),
        setup_dseg_draw_frame(style, attr, fill_attr, x, y, right, bottom),
        dump_count=400,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
    )
    got = translated("abidrawframe", hex(style), hex(attr), hex(fill_attr), hex(x), hex(y), hex(right), hex(bottom))
    if got is not None:
        assert got.endswith("data=" + data.hex())


def test_original_and_translated_txt_draw_top_title_scratch_video_buffer() -> None:
    _, data = original_seg001_call(
        original_offset("txt_draw_top_title"),
        setup_dseg_txt_draw_top_title(),
        dump_count=0x500,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
    )
    got = translated("txtdrawtoptitle")
    if got is not None:
        assert got.endswith("data=" + data.hex())


def test_original_and_abi_txt_draw_top_title_public_symbol_scratch_video_buffer() -> None:
    _, data = original_seg001_call(
        original_offset("txt_draw_top_title"),
        setup_dseg_txt_draw_top_title(),
        dump_count=0x500,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
    )
    got = translated("abitxtdrawtoptitle")
    if got is not None:
        assert got.endswith("data=" + data.hex())


def mov_es_byte(offset: int, value: int) -> bytes:
    return b"\x26\xc6\x06" + struct.pack("<H", offset & 0xFFFF) + bytes([value & 0xFF])


def mov_es_word(offset: int, value: int) -> bytes:
    return b"\x26\xc7\x06" + struct.pack("<H", offset & 0xFFFF) + struct.pack("<H", value & 0xFFFF)


def mov_es_dword(offset: int, value: int) -> bytes:
    return b"\x26\x66\xc7\x06" + struct.pack("<H", offset & 0xFFFF) + struct.pack("<I", value & 0xFFFFFFFF)


def setup_filelist_entry(entry_type: int, flags: int, time_word: int, date_word: int, size: int, name: bytes) -> bytes:
    video = DSEG_SCRATCH + 0x1000
    entry_seg = DSEG + 0x0200
    setup = (
        setup_dseg_common()
        + mov_ds_dword(0x1630, (DSEG << 16) | video)
        + mov_ds_word(0x1662, entry_seg)
        + mov_ds_word(0x1664, 1)
        + mov_ds_word(0x166E, 0)
        + mov_ds_word(0x1670, 0xFFFF)
        + mov_di(video)
        + mov_cx(0x800)
        + b"\xb0\xcc\xf3\xaa"
        + mov_ax(entry_seg)
        + b"\x8e\xc0"
    )
    for index in range(0x30):
        setup += mov_es_byte(index, 0)
    setup += mov_es_byte(2, entry_type)
    setup += mov_es_byte(3, flags)
    setup += mov_es_word(4, time_word)
    setup += mov_es_word(6, date_word)
    setup += mov_es_dword(8, size)
    for index, value in enumerate(name[:12]):
        setup += mov_es_byte(0x0C + index, value)
    for index, value in enumerate(b"Description"):
        setup += mov_es_byte(0x1A + index, value)
    return setup_dseg_common() + setup


def test_original_and_translated_filelist_single_row_video_format() -> None:
    cases = [
        (2, 0, 0xA7A0, 0x56A5, 123456, b"TRACK.MOD"),
        (1, 0, 0x0000, 0x0000, 0, b"DIR"),
    ]
    for entry_type, flags, time_word, date_word, size, name in cases:
        _, data = original_seg001_call(
            original_offset("filelist_198B8"),
            setup_filelist_entry(entry_type, flags, time_word, date_word, size, name),
            dump_count=160,
            dump_offset=DSEG_SCRATCH + 0x1000 + 0x654,
            dump_seg=DSEG,
        )
        got = translated("filelist", hex(entry_type), hex(flags), hex(time_word), hex(date_word), hex(size), name.decode("latin1"))
        if got is not None:
            assert field(got, "data") == data.hex()


def test_original_and_abi_filelist_public_symbol_single_row_video_format() -> None:
    cases = [
        (2, 0, 0xA7A0, 0x56A5, 123456, b"TRACK.MOD"),
        (1, 0, 0x0000, 0x0000, 0, b"DIR"),
    ]
    for entry_type, flags, time_word, date_word, size, name in cases:
        _, data = original_seg001_call(
            original_offset("filelist_198B8"),
            setup_filelist_entry(entry_type, flags, time_word, date_word, size, name),
            dump_count=160,
            dump_offset=DSEG_SCRATCH + 0x1000 + 0x654,
            dump_seg=DSEG,
        )
        got = translated("abifilelist", hex(entry_type), hex(flags), hex(time_word), hex(date_word), hex(size), name.decode("latin1"))
        if got is not None:
            assert field(got, "data") == data.hex()


def wrapper_find_mods_no_nul_guard() -> bytes:
    setup = setup_dseg_common()
    for index in range(120):
        setup += mov_ds_byte(0x137C + index, ord("X"))
    setup += mov_ds_byte(0x168E, 0xAA)
    setup += mov_ds_dword(0x1640, 0xCCCCBBBB)
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x168E, DSEG_SCRATCH + 0x300, 1)
    post += copy_bytes_to_scratch(0x1640, DSEG_SCRATCH + 0x301, 4)
    return setup + call_rel16(original_offset("find_mods"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_find_mods_no_nul_guard_before_dos_findfirst() -> None:
    out, data = original_seg001_call(
        original_offset("find_mods"),
        wrapper_find_mods_no_nul_guard(),
        dump_count=5,
        dump_offset=DSEG_SCRATCH + 0x300,
        dump_seg=DSEG,
    )
    got = translated("findmodsguard")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()
    assert data[0] == 2


def test_original_and_abi_find_mods_public_symbol_no_nul_guard_before_dos_findfirst() -> None:
    out, data = original_seg001_call(
        original_offset("find_mods"),
        wrapper_find_mods_no_nul_guard(),
        dump_count=5,
        dump_offset=DSEG_SCRATCH + 0x300,
        dump_seg=DSEG,
    )
    got = translated("abifindmodsguard")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()


def setup_txt_draw_bottom(byte_1de72: int, byte_1de73: int, byte_1de74: int, byte_1de75: int, byte_1de76: int, flags: int, volume: int, amplif: int) -> bytes:
    video = DSEG_SCRATCH + 0x1000
    return (
        setup_dseg_common()
        + mov_ds_dword(0x1634, (DSEG << 16) | video)
        + mov_ds_byte(0x1682, byte_1de72)
        + mov_ds_byte(0x1683, byte_1de73)
        + mov_ds_byte(0x1684, byte_1de74)
        + mov_ds_byte(0x1685, byte_1de75)
        + mov_ds_byte(0x1686, byte_1de76)
        + mov_ds_byte(0x1687, flags)
        + mov_ds_word(0x167A, volume)
        + mov_ds_word(0x167C, amplif)
        + mov_ds_byte(0x00DF, 1)
        + mov_di(video)
        + mov_cx(0x600)
        + b"\xb0\xcc\xf3\xaa"
    )


def test_original_and_translated_txt_draw_bottom_status_line() -> None:
    cases = [
        (2, 15, 3, 12, 34, 0x00, 128, 150),
        (7, 31, 63, 99, 125, 0x1F, 220, 2048),
    ]
    for case in cases:
        _, data = original_seg001_call(
            original_offset("txt_draw_bottom"),
            setup_txt_draw_bottom(*case),
            dump_count=0x600,
            dump_offset=DSEG_SCRATCH + 0x1000,
            dump_seg=DSEG,
        )
        got = translated("txtdrawbottom", *(hex(value) for value in case))
        if got is not None:
            assert field(got, "data") == data.hex()


def test_original_and_abi_txt_draw_bottom_public_symbol_status_line() -> None:
    cases = [
        (2, 15, 3, 12, 34, 0x00, 128, 150),
        (7, 31, 63, 99, 125, 0x1F, 220, 2048),
    ]
    for case in cases:
        _, data = original_seg001_call(
            original_offset("txt_draw_bottom"),
            setup_txt_draw_bottom(*case),
            dump_count=0x600,
            dump_offset=DSEG_SCRATCH + 0x1000,
            dump_seg=DSEG,
        )
        got = translated("abitxtdrawbottom", *(hex(value) for value in case))
        if got is not None:
            assert field(got, "data") == data.hex()


def test_original_and_translated_message_1be77_framed_message() -> None:
    text, y, attr = b"OK", 3, 0x1E
    out, data = original_seg001_call(
        original_offset("message_1BE77"),
        setup_dseg_message_1be77(text, y, attr),
        dump_count=1000,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
    )
    got = translated("message1be77", text.decode("latin1"), hex(y), hex(attr))
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert got.endswith("data=" + data.hex())
    assert bytes([ord("O"), attr, ord("K"), attr]) in data


def test_original_and_abi_message_1be77_public_symbol_framed_message() -> None:
    text, y, attr = b"OK", 3, 0x1E
    out, data = original_seg001_call(
        original_offset("message_1BE77"),
        setup_dseg_message_1be77(text, y, attr),
        dump_count=1000,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
    )
    got = translated("abimessage1be77", text.decode("latin1"), hex(y), hex(attr))
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert got.endswith("data=" + data.hex())


def test_original_and_translated_recolortxt_attribute_row() -> None:
    for row, color in [(0, 0x70), (2, 0x20)]:
        base = recolortxt_base(row)
        out, data = original_seg001_call(
            original_offset("recolortxt"),
            setup_dseg_recolortxt(row, color),
            dump_count=128,
            dump_offset=base,
            dump_seg=DSEG,
        )
        attrs = data[0::2]
        expected = bytes([(color | (index & 0x0F)) & 0xFF for index in range(64)])
        got = translated("recolortxt", hex(row), hex(color))
        assert attrs == expected
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert got.endswith("data=" + expected.hex())


def test_original_and_abi_recolortxt_public_symbol_attribute_row() -> None:
    for row, color in [(0, 0x70), (2, 0x20)]:
        base = recolortxt_base(row)
        out, data = original_seg001_call(
            original_offset("recolortxt"),
            setup_dseg_recolortxt(row, color),
            dump_count=128,
            dump_offset=base,
            dump_seg=DSEG,
        )
        attrs = data[0::2]
        expected = bytes([(color | (index & 0x0F)) & 0xFF for index in range(64)])
        got = translated("abirecolortxt", hex(row), hex(color))
        assert attrs == expected
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert got.endswith("data=" + expected.hex())


def test_original_and_translated_mouse_helpers_no_driver_paths() -> None:
    cases = [
        ("mouse_init", "mouse_init", 1, 1),
        ("mouse_deinit", "mouse_deinit", 0, 1),
        ("mouse_show", "mouse_show", 0, 0),
        ("mouse_hide", "mouse_hide", 0, 1),
        ("mouse_getpos", "mouse_getpos", 0, 1),
        ("mouse_showcur", "mouse_showcur", 0, 0),
        ("mouse_hide2", "mouse_hide2", 0, 1),
    ]
    for symbol, command, exists, visible in cases:
        out, globals_ = original_seg001_call(
            original_offset(symbol),
            setup_mouse_state(exists, visible),
            dump_count=7,
            dump_offset=0x169C,
            dump_seg=DSEG,
            strict=False,
        )
        translated_out = translated(command, hex(exists), hex(visible))
        if translated_out is not None:
            assert field(translated_out, "bx") == field(out, "bx")
            assert field(translated_out, "cx") == field(out, "cx")
            assert field(translated_out, "dx") == field(out, "dx")
            assert translated_out.endswith("data=" + globals_.hex())


def test_original_and_abi_mouse_getpos_public_symbol_no_driver_path() -> None:
    out, globals_ = original_seg001_call(
        original_offset("mouse_getpos"),
        setup_mouse_state(0, 1),
        dump_count=7,
        dump_offset=0x169C,
        dump_seg=DSEG,
        strict=False,
    )
    got = translated("abimousegetpos")
    if got is not None:
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert got.endswith("data=" + globals_.hex())


def test_original_and_abi_mouse_cursor_public_symbols_no_driver_paths() -> None:
    cases = [
        ("mouse_showcur", 0, 0),
        ("mouse_hide2", 0, 1),
    ]
    for symbol, exists, visible in cases:
        out, globals_ = original_seg001_call(
            original_offset(symbol),
            setup_mouse_state(exists, visible),
            dump_count=7,
            dump_offset=0x169C,
            dump_seg=DSEG,
            strict=False,
        )
        got = translated("abimousecursor", symbol)
        if got is not None:
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert got.endswith("data=" + globals_.hex())


def test_original_and_abi_mouse_wrapper_public_symbols_no_driver_paths() -> None:
    cases = [
        ("mouse_show", 0, 0),
        ("mouse_hide", 0, 1),
    ]
    for symbol, exists, visible in cases:
        out, globals_ = original_seg001_call(
            original_offset(symbol),
            setup_mouse_state(exists, visible),
            dump_count=7,
            dump_offset=0x169C,
            dump_seg=DSEG,
            strict=False,
        )
        got = translated("abimousewrapper", symbol)
        if got is not None:
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert got.endswith("data=" + globals_.hex())


def test_original_and_abi_mouse_deinit_public_symbol_no_driver_path() -> None:
    out, globals_ = original_seg001_call(
        original_offset("mouse_deinit"),
        setup_mouse_state(0, 1),
        dump_count=7,
        dump_offset=0x169C,
        dump_seg=DSEG,
        strict=False,
    )
    got = translated("abimousedeinit")
    if got is not None:
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert got.endswith("data=" + globals_.hex())


def test_original_and_abi_mouse_init_public_symbol_no_driver_path() -> None:
    out, globals_ = original_seg001_call(
        original_offset("mouse_init"),
        setup_mouse_state(1, 1),
        dump_count=7,
        dump_offset=0x169C,
        dump_seg=DSEG,
        strict=False,
    )
    got = translated("abimouseinit")
    if got is not None:
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert got.endswith("data=" + globals_.hex())


def test_original_and_translated_mouse_1c7a9_rectangle_hit_test() -> None:
    cases = [
        (12, 8, 10, 5, 20, 12),
        (12, 8, 20, 12, 10, 5),
        (9, 8, 10, 5, 20, 12),
        (12, 13, 10, 5, 20, 12),
    ]
    for x, y, left, top, right, bottom in cases:
        out, _ = original_seg001_call(
            original_offset("mouse_1C7A9"),
            setup_mouse_1c7a9(x, y, left, top, right, bottom),
            dump_count=0,
        )
        got = translated("mouse_1c7a9", hex(x), hex(y), hex(left), hex(top), hex(right), hex(bottom))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bp") == field(out, "bp")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_abi_mouse_1c7a9_public_symbol() -> None:
    cases = [
        (12, 8, 10, 5, 20, 12),
        (12, 8, 20, 12, 10, 5),
        (9, 8, 10, 5, 20, 12),
        (12, 13, 10, 5, 20, 12),
    ]
    for x, y, left, top, right, bottom in cases:
        out, _ = original_seg001_call(
            original_offset("mouse_1C7A9"),
            setup_mouse_1c7a9(x, y, left, top, right, bottom),
            dump_count=0,
        )
        got = translated("abimouse1c7a9", hex(x), hex(y), hex(left), hex(top), hex(right), hex(bottom))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bp") == field(out, "bp")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_translated_mouse_1c7cf_rectangle_list_search() -> None:
    cases = [
        (12, 8, mouse_rect_record(10, 5, 20, 12, 0x2222) + mouse_rect_sentinel(), 0x2222),
        (32, 8, mouse_rect_record(10, 5, 20, 12, 0x1111) + mouse_rect_record(30, 5, 40, 12, 0x3333) + mouse_rect_sentinel(), 0x3333),
        (50, 8, mouse_rect_record(10, 5, 20, 12, 0x1111) + mouse_rect_sentinel(), DSEG_SCRATCH + 0x0A),
    ]
    for x, y, records, expected_bx in cases:
        out, _ = original_seg001_call(
            original_offset("mouse_1C7CF"),
            setup_mouse_1c7cf(x, y, records),
            dump_count=0,
        )
        got = translated("mouse_1c7cf", hex(x), hex(y), records.hex())
        assert field(out, "bx") == f"{expected_bx & 0xFFFF:04x}"
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "bp") == field(out, "bp")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_abi_mouse_1c7cf_public_symbol_rectangle_list_search() -> None:
    cases = [
        (12, 8, mouse_rect_record(10, 5, 20, 12, 0x2222) + mouse_rect_sentinel(), 0x2222),
        (32, 8, mouse_rect_record(10, 5, 20, 12, 0x1111) + mouse_rect_record(30, 5, 40, 12, 0x3333) + mouse_rect_sentinel(), 0x3333),
        (50, 8, mouse_rect_record(10, 5, 20, 12, 0x1111) + mouse_rect_sentinel(), DSEG_SCRATCH + 0x0A),
    ]
    for x, y, records, expected_bx in cases:
        out, _ = original_seg001_call(
            original_offset("mouse_1C7CF"),
            setup_mouse_1c7cf(x, y, records),
            dump_count=0,
        )
        got = translated("abimouse1c7cf", hex(x), hex(y), records.hex())
        assert field(out, "bx") == f"{expected_bx & 0xFFFF:04x}"
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "bp") == field(out, "bp")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_translated_int24_critical_error_return_code() -> None:
    cases = [
        (0x08, "0803"),
        (0x20, "2000"),
        (0x00, "0001"),
    ]
    for ah_value, expected_ax in cases:
        out, _ = original_seg001_iret_call(original_offset("int24"), setup_int24(ah_value))
        translated_out = translated("int24", hex(ah_value))
        assert field(out, "ax") == expected_ax
        if translated_out is not None:
            assert field(translated_out, "ax") == expected_ax


def test_original_and_abi_int24_public_symbol_transform() -> None:
    for ah_value, expected_ax in [(0x08, "0803"), (0x20, "2000"), (0x00, "0001")]:
        out, _ = original_seg001_iret_call(original_offset("int24"), setup_int24(ah_value))
        got = translated("abiint24", hex(ah_value))
        assert field(out, "ax") == expected_ax
        if got is not None:
            assert field(got, "ax") == expected_ax


def test_harness_inputs_exist() -> None:
    assert ORIGINAL_EXE.exists()
    assert ORIGINAL_LST.exists()
    assert KVIKDOS.exists()
    if RUNNER is not None:
        assert RUNNER.exists()


def setup_dseg_write_scr(text: bytes, attr: int, delta: int) -> bytes:
    setup = setup_dseg_common()
    setup += mov_ds_word(DSEG_SCRATCH, delta)
    setup += mov_ds_byte(DSEG_SCRATCH + 2, attr)
    for index, value in enumerate(text + b"\0"):
        setup += mov_ds_byte(DSEG_SCRATCH + 3 + index, value)
    return setup + b"\xbe" + struct.pack("<H", DSEG_SCRATCH) + mov_di(DSEG_SCRATCH + 0x40)


def test_original_and_translated_write_scr_text_record() -> None:
    cases = [
        (b"A", 0x07, 0),
        (b"ABC", 0x1E, 4),
        (b"Level!", 0x70, 10),
    ]
    for text, attr, delta in cases:
        out, data = original_seg001_call(
            original_offset("write_scr"),
            setup_dseg_write_scr(text, attr, delta),
            dump_count=len(text) * 2,
            dump_offset=DSEG_SCRATCH + 0x40 + delta,
            dump_seg=DSEG,
        )
        got = translated("writescr", text.decode("latin1"), hex(attr), hex(delta))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")
        assert field(out, "di") == f"{DSEG_SCRATCH + 0x40 + delta + len(text) * 2:04x}"


def test_original_and_abi_write_scr_public_symbol() -> None:
    cases = [
        (b"A", 0x07, 0),
        (b"ABC", 0x1E, 4),
        (b"Level!", 0x70, 10),
    ]
    for text, attr, delta in cases:
        out, data = original_seg001_call(
            original_offset("write_scr"),
            setup_dseg_write_scr(text, attr, delta),
            dump_count=len(text) * 2,
            dump_offset=DSEG_SCRATCH + 0x40 + delta,
            dump_seg=DSEG,
        )
        got = translated("abiwritescr", text.decode("latin1"), hex(attr), hex(delta))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def setup_ems_restore_mapctx(ems_enabled: int, mapctx_saved: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x0104, ems_enabled)
        + mov_ds_byte(0x0105, mapctx_saved)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def test_original_and_translated_ems_restore_mapctx_guard_return() -> None:
    for ems_enabled, mapctx_saved in [(0, 0), (0, 1), (1, 0)]:
        out, _ = original_call(original_offset("ems_restore_mapctx"), setup_ems_restore_mapctx(ems_enabled, mapctx_saved))
        got = translated("emsrestore", hex(ems_enabled), hex(mapctx_saved))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
        assert field(out, "ax") == "1234"
        assert field(out, "bx") == "5678"
        assert field(out, "cx") == "9abc"
        assert field(out, "dx") == "def0"


def test_original_and_abi_ems_restore_mapctx_public_symbol_guard_return() -> None:
    for ems_enabled, mapctx_saved in [(0, 0), (0, 1), (1, 0)]:
        out, _ = original_call(original_offset("ems_restore_mapctx"), setup_ems_restore_mapctx(ems_enabled, mapctx_saved))
        got = translated("abiemsrestore", hex(ems_enabled), hex(mapctx_saved))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def setup_ems_disabled_guard() -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x0104, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def setup_ems_init_config(config_word: int) -> bytes:
    return setup_data_common() + mov_ds_byte(0x0104, 0xFF) + mov_ds_word(0x013A, config_word)


def test_original_and_translated_ems_init_config_disabled() -> None:
    out, _ = original_call(original_offset("ems_init"), setup_ems_init_config(0))
    got = translated("emsinit", "0")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "ems") == "00"
    assert field(out, "ax") == "0001"


def test_original_and_abi_ems_init_public_symbol_config_disabled() -> None:
    out, _ = original_call(original_offset("ems_init"), setup_ems_init_config(0))
    got = translated("abiemsinit", "0")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "ems") == "00"
    assert field(out, "ax") == "0001"


def test_original_and_translated_ems_disabled_guard_returns() -> None:
    symbols = ["ems_release", "ems_realloc", "ems_deinit", "ems_save_mapctx", "ems_mapmem", "ems_mapmem2"]
    for symbol in symbols:
        out, _ = original_call(original_offset(symbol), setup_ems_disabled_guard())
        got = translated("emsguard", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
        assert field(out, "ax") == "1234"
        assert field(out, "bx") == "5678"
        assert field(out, "cx") == "9abc"
        assert field(out, "dx") == "def0"


def test_original_and_abi_ems_disabled_guard_public_symbols_return() -> None:
    symbols = ["ems_release", "ems_realloc", "ems_deinit", "ems_save_mapctx", "ems_mapmem", "ems_mapmem2"]
    for symbol in symbols:
        out, _ = original_call(original_offset(symbol), setup_ems_disabled_guard())
        got = translated("abiemsguard", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
        assert field(out, "ax") == "1234"
        assert field(out, "bx") == "5678"
        assert field(out, "cx") == "9abc"
        assert field(out, "dx") == "def0"


def setup_ems_local_mapcopy(symbol: str, payload: bytes) -> tuple[bytes, int]:
    setup = setup_data_common()
    setup += mov_ds_dword(DSEG_SCRATCH + 0x20, DSEG_SCRATCH)
    setup += mov_ds_dword(DSEG_SCRATCH + 0x2C, DSEG_SCRATCH)
    setup += mov_ds_word(DSEG_SCRATCH + 0x32, 0xFFFF)
    setup += mov_ds_byte(DSEG_SCRATCH + 0x3C, 0)
    if symbol == "ems_mapmemx":
        source = DSEG_SCRATCH + 1
        dest = DSEG_SCRATCH + 0x800
    else:
        source = DSEG_SCRATCH + 0x800
        dest = DSEG_SCRATCH + 1
    for index, value in enumerate(payload):
        setup += mov_ds_byte(source + index, value)
    return setup + mov_ax(DATA_SEG) + mov_di(DSEG_SCRATCH), dest


def test_original_and_translated_ems_local_mapcopy_without_ems_interrupts() -> None:
    payload = bytes(range(0x31, 0x41))
    for symbol in ["ems_mapmemx", "ems_mapmemy"]:
        setup, dest = setup_ems_local_mapcopy(symbol, payload)
        out, data = original_run(
            make_wrapper(original_offset(symbol), setup),
            dump_count=len(payload),
            dump_offset=dest,
            dump_seg=DATA_SEG,
        )
        got = translated("emsmapcopy", symbol)
        if got is not None:
            assert got.endswith("data=" + data.hex())
        assert data == payload


def test_original_and_abi_ems_local_mapcopy_public_symbols_without_ems_interrupts() -> None:
    payload = bytes(range(0x31, 0x41))
    for symbol in ["ems_mapmemx", "ems_mapmemy"]:
        setup, dest = setup_ems_local_mapcopy(symbol, payload)
        _, data = original_run(
            make_wrapper(original_offset(symbol), setup),
            dump_count=len(payload),
            dump_offset=dest,
            dump_seg=DATA_SEG,
        )
        got = translated("abiemsmapcopy", symbol)
        if got is not None:
            assert got.endswith("data=" + data.hex())
        assert data == payload


def setup_ems_realloc2_limit(initial_count: int, requested_size: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x0077, initial_count)
        + mov_ds_byte(0x0104, 0)
        + mov_ds_dword(DSEG_SCRATCH + 0x20, requested_size)
        + mov_di(DSEG_SCRATCH)
    )


def test_original_and_translated_ems_realloc2_oversized_fallback_guard() -> None:
    cases = [
        (0, 0x100000),
        (7, 0x101000),
    ]
    for initial_count, requested_size in cases:
        out, data = original_run(
            make_wrapper(original_offset("ems_realloc2"), setup_ems_realloc2_limit(initial_count, requested_size)),
            dump_count=1,
            dump_offset=0x0077,
            dump_seg=DATA_SEG,
        )
        got = translated("emsrealloc2limit", hex(initial_count), hex(requested_size))
        assert field(out, "ax") == "0008"
        assert field(out, "cx") == "ffff"
        assert data == bytes([(initial_count + 1) & 0xFF])
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "cx") == field(out, "cx")
            assert got.endswith("data=" + data.hex())


def test_original_and_abi_ems_realloc2_public_symbol_oversized_fallback_guard() -> None:
    cases = [
        (0, 0x100000),
        (7, 0x101000),
    ]
    for initial_count, requested_size in cases:
        out, data = original_run(
            make_wrapper(original_offset("ems_realloc2"), setup_ems_realloc2_limit(initial_count, requested_size)),
            dump_count=1,
            dump_offset=0x0077,
            dump_seg=DATA_SEG,
        )
        got = translated("abiemsrealloc2limit", hex(initial_count), hex(requested_size))
        assert field(out, "ax") == "0008"
        assert field(out, "cx") == "ffff"
        assert data == bytes([(initial_count + 1) & 0xFF])
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "cx") == field(out, "cx")
            assert got.endswith("data=" + data.hex())


def setup_clean_11c43(flag: int, byte_2461e: int, byte_2461f: int) -> bytes:
    setup = setup_data_common()
    setup += mov_ds_byte(0x00D2, flag)
    setup += mov_ds_byte(0x007E, byte_2461e)
    setup += mov_ds_byte(0x007F, byte_2461f)
    for offset in [
        0x0032, 0x0034, 0x0036, 0x0038, 0x003A, 0x003E,
        0x0050, 0x0054, 0x0056, 0x0058, 0x005A, 0x005E,
        0x0090, 0x0130,
    ]:
        setup += mov_ds_word(offset, 0xFFFF)
    for offset in [0x007A, 0x0085, 0x00D3, 0x00D9, 0x00DA, 0x00DE]:
        setup += mov_ds_byte(offset, 0xFF)
    for offset in range(0x3648, 0x364C):
        setup += mov_ds_byte(offset, 0xFF)
    for base in [0x3A48, 0x3B48, 0x3D48]:
        for index in range(4):
            setup += mov_ds_byte(base + index, 0xFF)
    for index in range(4):
        setup += mov_ds_byte(0x3C48 + index, 0)
    return setup


def wrapper_clean_11c43(flag: int, byte_2461e: int, byte_2461f: int) -> bytes:
    setup = setup_clean_11c43(flag, byte_2461e, byte_2461f)
    post = b""
    post += copy_bytes_to_scratch(0x0032, DSEG_SCRATCH, 10)
    post += copy_bytes_to_scratch(0x003E, DSEG_SCRATCH + 10, 2)
    post += copy_bytes_to_scratch(0x0050, DSEG_SCRATCH + 12, 10)
    post += copy_bytes_to_scratch(0x005E, DSEG_SCRATCH + 22, 2)
    post += copy_bytes_to_scratch(0x007A, DSEG_SCRATCH + 24, 1)
    post += copy_bytes_to_scratch(0x0085, DSEG_SCRATCH + 25, 1)
    post += copy_bytes_to_scratch(0x00D3, DSEG_SCRATCH + 26, 1)
    post += copy_bytes_to_scratch(0x0090, DSEG_SCRATCH + 27, 2)
    post += copy_bytes_to_scratch(0x00D9, DSEG_SCRATCH + 29, 1)
    post += copy_bytes_to_scratch(0x00DA, DSEG_SCRATCH + 30, 1)
    post += copy_bytes_to_scratch(0x00DE, DSEG_SCRATCH + 31, 1)
    post += copy_bytes_to_scratch(0x0130, DSEG_SCRATCH + 32, 2)
    post += copy_bytes_to_scratch(0x3648, DSEG_SCRATCH + 34, 4)
    post += copy_bytes_to_scratch(0x3A48, DSEG_SCRATCH + 38, 4)
    post += copy_bytes_to_scratch(0x3B48, DSEG_SCRATCH + 42, 4)
    post += copy_bytes_to_scratch(0x3C48, DSEG_SCRATCH + 46, 4)
    post += copy_bytes_to_scratch(0x3D48, DSEG_SCRATCH + 50, 4)
    post += copy_bytes_to_scratch(0x3628, DSEG_SCRATCH + 54, 3)
    return setup + b"\x9a" + struct.pack("<HH", original_offset("clean_11C43"), LOAD_SEG) + post + b"\xc3"


def test_original_and_translated_clean_11c43_initializes_playback_state() -> None:
    cases = [
        (0x00, 0x12, 0x34, 8363),
        (0x08, 0x56, 0x78, 8287),
    ]
    for flag, byte_2461e, byte_2461f, expected_freq in cases:
        out, data = original_run(
            wrapper_clean_11c43(flag, byte_2461e, byte_2461f),
            dump_count=57,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("clean11c43", hex(flag), hex(byte_2461e), hex(byte_2461f))
        expected = bytearray(57)
        expected[2:4] = struct.pack("<H", 4)
        expected[4:6] = struct.pack("<H", 4)
        expected[10:12] = struct.pack("<H", expected_freq)
        expected[22:24] = struct.pack("<H", 100)
        expected[27:29] = struct.pack("<H", 2)
        expected[29] = 6
        expected[30] = 125
        expected[32:34] = struct.pack("<H", 1)
        expected[46:50] = b"????"
        expected[54:57] = bytes([byte_2461e, byte_2461f, byte_2461f])
        assert data == bytes(expected)
        if got is not None:
            assert got.endswith("data=" + data.hex())


def test_original_and_abi_clean_11c43_public_symbol_initializes_playback_state() -> None:
    cases = [
        (0x00, 0x12, 0x34, 8363),
        (0x08, 0x56, 0x78, 8287),
    ]
    for flag, byte_2461e, byte_2461f, expected_freq in cases:
        _, data = original_run(
            wrapper_clean_11c43(flag, byte_2461e, byte_2461f),
            dump_count=57,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("abiclean11c43", hex(flag), hex(byte_2461e), hex(byte_2461f))
        expected = bytearray(57)
        expected[2:4] = struct.pack("<H", 4)
        expected[4:6] = struct.pack("<H", 4)
        expected[10:12] = struct.pack("<H", expected_freq)
        expected[22:24] = struct.pack("<H", 100)
        expected[27:29] = struct.pack("<H", 2)
        expected[29] = 6
        expected[30] = 125
        expected[32:34] = struct.pack("<H", 1)
        expected[46:50] = b"????"
        expected[54:57] = bytes([byte_2461e, byte_2461f, byte_2461f])
        assert data == bytes(expected)
        if got is not None:
            assert got.endswith("data=" + data.hex())


def setup_ems_local_mapcopy(symbol: str, payload: bytes) -> tuple[bytes, int]:
    setup = setup_data_common()
    if symbol == "ems_mapmemx":
        source = DSEG_SCRATCH + 0x100
        dest = DSEG_SCRATCH + 0x900
        field_20 = dest - 0x800
        field_2c = source - 1
    else:
        source = DSEG_SCRATCH + 0x900
        dest = DSEG_SCRATCH + 0x100
        field_20 = source - 0x800
        field_2c = dest - 1
    setup += mov_ds_dword(DSEG_SCRATCH + 0x20, field_20)
    setup += mov_ds_dword(DSEG_SCRATCH + 0x2C, field_2c)
    setup += mov_ds_word(DSEG_SCRATCH + 0x32, 0xFFFF)
    setup += mov_ds_byte(DSEG_SCRATCH + 0x3C, 0)
    for index, value in enumerate(payload):
        setup += mov_ds_byte(source + index, value)
    return setup + mov_ax(DATA_SEG) + mov_di(DSEG_SCRATCH), dest


def setup_mod_sub_delta(flag: int, reset: int, previous: int, payload: bytes) -> bytes:
    setup = setup_data_common()
    setup += mov_ds_byte(0x00D4, flag)
    setup += mov_ds_byte(0x00D5, reset)
    setup += mov_ds_byte(0x00D6, previous)
    for index, value in enumerate(payload):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    return setup + mov_cx(len(payload)) + b"\xbe" + struct.pack("<H", DSEG_SCRATCH)


def delta_decoded(payload: bytes, flag: int, reset: int, previous: int) -> bytes:
    if flag != 1:
        return payload
    acc = 0 if reset else previous
    out = []
    for value in payload:
        acc = (acc + value) & 0xFF
        out.append(acc)
    return bytes(out)


def test_original_and_translated_mod_sub_delta_buffer_transform() -> None:
    cases = [
        (0, 0, 0x44, b"ABCD"),
        (1, 1, 0x44, b"ABCD"),
        (1, 0, 0x10, b"WXYZ"),
    ]
    for flag, reset, previous, payload in cases:
        out, data = original_run(
            make_wrapper(original_offset("mod_sub_delta"), setup_mod_sub_delta(flag, reset, previous, payload)),
            dump_count=len(payload),
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("modsubdelta", hex(flag), hex(reset), hex(previous), payload.decode("latin1"))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "cx") == field(out, "cx")
        assert data == delta_decoded(payload, flag, reset, previous)


def test_original_and_abi_mod_sub_delta_public_symbol_buffer_transform() -> None:
    cases = [
        (0, 0, 0x44, b"ABCD"),
        (1, 1, 0x44, b"ABCD"),
        (1, 0, 0x10, b"WXYZ"),
    ]
    for flag, reset, previous, payload in cases:
        out, data = original_run(
            make_wrapper(original_offset("mod_sub_delta"), setup_mod_sub_delta(flag, reset, previous, payload)),
            dump_count=len(payload),
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("abimodsubdelta", hex(flag), hex(reset), hex(previous), payload.decode("latin1"))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "cx") == field(out, "cx")
        assert data == delta_decoded(payload, flag, reset, previous)


def setup_sub_11ba6(ch: int, cl: int, bx: int, dx: int, current_max: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x007B, current_max)
        + mov_cx(((ch & 0xFF) << 8) | (cl & 0xFF))
        + mov_bx(bx)
        + mov_dx(dx)
        + mov_di(DSEG_SCRATCH)
    )


def test_original_and_translated_sub_11ba6_event_stream_packer() -> None:
    cases = [
        (0x00, 0x80, 0x0000, 0x0000, 0x00),
        (0x03, 0x20, 0x0000, 0x0000, 0x01),
        (0x04, 0x41, 0x1234, 0x5678, 0x02),
        (0x1F, 0x40, 0x00FF, 0x1200, 0x10),
    ]
    for ch, cl, bx, dx, current_max in cases:
        out, data = original_call(
            original_offset("sub_11BA6"),
            setup_sub_11ba6(ch, cl, bx, dx, current_max),
            dump_count=8,
        )
        got = translated("sub11ba6", hex(ch), hex(cl), hex(bx), hex(dx), hex(current_max))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "di") == field(out, "di")


def test_original_and_abi_sub_11ba6_public_symbol_event_stream_packer() -> None:
    cases = [
        (0x00, 0x80, 0x0000, 0x0000, 0x00),
        (0x03, 0x20, 0x0000, 0x0000, 0x01),
        (0x04, 0x41, 0x1234, 0x5678, 0x02),
        (0x1F, 0x40, 0x00FF, 0x1200, 0x10),
    ]
    for ch, cl, bx, dx, current_max in cases:
        out, data = original_call(
            original_offset("sub_11BA6"),
            setup_sub_11ba6(ch, cl, bx, dx, current_max),
            dump_count=8,
        )
        got = translated("abisub11ba6", hex(ch), hex(cl), hex(bx), hex(dx), hex(current_max))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "di") == field(out, "di")


def setup_sub_12cad_out_of_range(ch: int, cl: int, bx: int, dx: int, channels: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x0034, channels)
        + mov_cx(((ch & 0xFF) << 8) | (cl & 0xFF))
        + mov_bx(bx)
        + mov_dx(dx)
    )


def test_original_and_translated_sub_12cad_out_of_range_event_store() -> None:
    for ch, cl, bx, dx, channels in [
        (0x00, 0x12, 0x3456, 0x789A, 0),
        (0x03, 0x44, 0x1111, 0x2222, 2),
        (0x1F, 0xFE, 0xABCD, 0x1357, 8),
    ]:
        out, data = original_run(
            make_far_wrapper(original_offset("sub_12CAD"), setup_sub_12cad_out_of_range(ch, cl, bx, dx, channels)),
            dump_count=5,
            dump_offset=0x0106,
            dump_seg=DATA_SEG,
        )
        got = translated("sub12cadguard", hex(ch), hex(cl), hex(bx), hex(dx), hex(channels))
        expected = struct.pack("<HBH", dx & 0xFFFF, cl & 0xFF, bx & 0xFFFF)
        assert data == expected
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert got.endswith("data=" + data.hex())


def test_original_and_abi_sub_12cad_public_symbol_out_of_range_event_store() -> None:
    for ch, cl, bx, dx, channels in [
        (0x00, 0x12, 0x3456, 0x789A, 0),
        (0x03, 0x44, 0x1111, 0x2222, 2),
        (0x1F, 0xFE, 0xABCD, 0x1357, 8),
    ]:
        out, data = original_run(
            make_far_wrapper(original_offset("sub_12CAD"), setup_sub_12cad_out_of_range(ch, cl, bx, dx, channels)),
            dump_count=5,
            dump_offset=0x0106,
            dump_seg=DATA_SEG,
        )
        got = translated("abisub12cadguard", hex(ch), hex(cl), hex(bx), hex(dx), hex(channels))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "data") == data.hex()


def setup_sub_13623_out_of_range(value: int, dx: int, channels: int) -> bytes:
    return setup_data_common() + mov_ds_word(0x0034, channels) + mov_ax(value) + mov_dx(dx) + mov_si(DSEG_SCRATCH)


def test_original_and_translated_sub_13623_out_of_range_event_decoder_guard() -> None:
    for value, dx, channels in [
        (0xE0, 0x1234, 0),
        (0xC3, 0xABCD, 2),
        (0xFF, 0x0055, 8),
    ]:
        out, _ = original_call(
            original_offset("sub_13623"),
            setup_sub_13623_out_of_range(value, dx, channels),
            dump_count=0,
        )
        got = translated("sub13623guard", hex(value), hex(dx), hex(channels))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")


def test_original_and_abi_sub_13623_public_symbol_out_of_range_event_decoder_guard() -> None:
    for value, dx, channels in [
        (0xE0, 0x1234, 0),
        (0xC3, 0xABCD, 2),
        (0xFF, 0x0055, 8),
    ]:
        out, _ = original_call(
            original_offset("sub_13623"),
            setup_sub_13623_out_of_range(value, dx, channels),
            dump_count=0,
        )
        got = translated("abisub13623guard", hex(value), hex(dx), hex(channels))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")


def setup_mod_102f5(values: bytes) -> bytes:
    setup = setup_data_common()
    padded = (values + bytes(128))[:128]
    for index, value in enumerate(padded):
        setup += mov_ds_byte(0x3A48 + index, value)
    return setup


def test_original_and_translated_mod_102f5_pattern_count_scan() -> None:
    cases = [
        bytes([0] * 128),
        bytes([0x80, 0x01, 0x7E, 0xFF] + [0x02] * 124),
        bytes((i * 17 + 3) & 0xFF for i in range(128)),
    ]
    for values in cases:
        out, data = original_run(
            make_wrapper(original_offset("mod_102F5"), setup_mod_102f5(values)),
            dump_count=2,
            dump_offset=0x0052,
            dump_seg=DATA_SEG,
        )
        got = translated("mod102f5", values.hex())
        if got is not None:
            assert got.endswith("data=" + data.hex())
        expected = (max(value & 0x7F for value in values[:128]) + 1) & 0xFFFF
        assert data == struct.pack("<H", expected)


def test_original_and_abi_mod_102f5_public_symbol_pattern_count_scan() -> None:
    cases = [
        bytes([0] * 128),
        bytes([0x80, 0x01, 0x7E, 0xFF] + [0x02] * 124),
        bytes((i * 17 + 3) & 0xFF for i in range(128)),
    ]
    for values in cases:
        _, data = original_run(
            make_wrapper(original_offset("mod_102F5"), setup_mod_102f5(values)),
            dump_count=2,
            dump_offset=0x0052,
            dump_seg=DATA_SEG,
        )
        got = translated("abimod102f5", values.hex())
        if got is not None:
            assert got.endswith("data=" + data.hex())
        expected = (max(value & 0x7F for value in values[:128]) + 1) & 0xFFFF
        assert data == struct.pack("<H", expected)


def setup_sub_126a9(word_245fa: int, size1: int, channels: int, realloc_count: int, module_type: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x005A, word_245fa)
        + mov_ds_word(0x0032, size1)
        + mov_ds_word(0x0034, channels)
        + mov_ds_byte(0x0077, realloc_count)
        + mov_ds_dword(0x010C, module_type)
    )


def setup_ult_read_fast(word_3063b: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0xC09B, word_3063b)
        + mov_ds_dword(0xC09D, 0xA5A5A5A5)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def test_original_and_translated_sub_126a9_module_metadata_snapshot() -> None:
    cases = [
        (0x0012, 0x0034, 0x0004, 0x05, 0x2E4B2E4D),
        (0x00FE, 0x001F, 0x0008, 0x7A, 0x20574F57),
    ]
    for word_245fa, size1, channels, realloc_count, module_type in cases:
        out, _ = original_far_call(original_offset("sub_126A9"), setup_sub_126a9(word_245fa, size1, channels, realloc_count, module_type))
        got = translated("sub126a9", hex(word_245fa), hex(size1), hex(channels), hex(realloc_count), hex(module_type))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_abi_sub_126a9_public_symbol_module_metadata_snapshot() -> None:
    cases = [
        (0x0012, 0x0034, 0x0004, 0x05, 0x2E4B2E4D),
        (0x00FE, 0x001F, 0x0008, 0x7A, 0x20574F57),
    ]
    for word_245fa, size1, channels, realloc_count, module_type in cases:
        out, _ = original_far_call(original_offset("sub_126A9"), setup_sub_126a9(word_245fa, size1, channels, realloc_count, module_type))
        got = translated("abisub126a9", hex(word_245fa), hex(size1), hex(channels), hex(realloc_count), hex(module_type))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")


def test_original_and_translated_ult_read_counter_fast_path() -> None:
    for word_value in [0x0201, 0x7F55]:
        out, data = original_run(
            make_wrapper(original_offset("ult_read"), setup_ult_read_fast(word_value)),
            dump_count=6,
            dump_offset=0xC09B,
            dump_seg=DATA_SEG,
        )
        got = translated("ultreadfast", hex(word_value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()
        assert data[:2] == struct.pack("<H", (word_value - 0x0100) & 0xFFFF)


def test_original_and_abi_ult_read_public_symbol_counter_fast_path() -> None:
    for word_value in [0x0201, 0x7F55]:
        out, data = original_run(
            make_wrapper(original_offset("ult_read"), setup_ult_read_fast(word_value)),
            dump_count=6,
            dump_offset=0xC09B,
            dump_seg=DATA_SEG,
        )
        got = translated("abiultreadfast", hex(word_value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()
        assert data[:2] == struct.pack("<H", (word_value - 0x0100) & 0xFFFF)


def setup_sub_1265d(volume: int, sndcard: int, byte_24666: int, byte_24667: int, sndflags: int, byte_24628: int, stereo: int, byte_24671: int, word_245f6: int, word_245f0: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x005C, volume)
        + mov_ds_byte(0x0132, sndcard)
        + mov_ds_byte(0x00C6, byte_24666)
        + mov_ds_byte(0x00C7, byte_24667)
        + mov_ds_byte(0x0082, sndflags)
        + mov_ds_byte(0x0088, byte_24628)
        + mov_ds_byte(0x0083, stereo)
        + mov_ds_byte(0x00D1, byte_24671)
        + mov_ds_word(0x0056, word_245f6)
        + mov_ds_word(0x0050, word_245f0)
    )


def test_original_and_translated_sub_1265d_playback_metadata_snapshot() -> None:
    cases = [
        (0x0100, 2, 3, 4, 0x11, 1, 0, 1, 0x00AA, 0x0055),
        (0x0034, 9, 0x12, 0x34, 0x82, 4, 1, 0, 0x00FE, 0x00DC),
    ]
    for case in cases:
        out, _ = original_far_call(original_offset("sub_1265D"), setup_sub_1265d(*case))
        got = translated("sub1265d", *(hex(value) for value in case))
        if got is not None:
            for reg in ["ax", "bx", "cx", "dx", "bp", "si", "di"]:
                assert field(got, reg) == field(out, reg)


def setup_memfree_125da_guard() -> bytes:
    return (
        setup_data_common()
        + mov_ds_dword(0x00A0, 0)
        + mov_ds_byte(0x00C5, 0)
        + mov_ds_byte(0x0104, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def test_original_and_translated_memfree_125da_guard_return_without_dos_free() -> None:
    out, _ = original_far_call(original_offset("memfree_125DA"), setup_memfree_125da_guard())
    got = translated("memfree125da")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_memfree_125da_public_symbol_guard_return_without_dos_free() -> None:
    out, _ = original_far_call(original_offset("memfree_125DA"), setup_memfree_125da_guard())
    got = translated("abimemfree125da")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def mov_si(value: int) -> bytes:
    return b"\xbe" + struct.pack("<H", value & 0xFFFF)


def copy_bytes_to_scratch(src: int, dst: int, count: int) -> bytes:
    return mov_si(src) + mov_di(dst) + mov_cx(count) + b"\xf3\xa4"


def setup_mod_1021e(first: int, second: int, pattern: bytes, title: bytes) -> bytes:
    setup = setup_data_common()
    setup += mov_ds_byte(DSEG_SCRATCH, first)
    setup += mov_ds_byte(DSEG_SCRATCH + 1, second)
    padded_pattern = (pattern + bytes(128))[:128]
    for index, value in enumerate(padded_pattern):
        setup += mov_ds_byte(DSEG_SCRATCH + 2 + index, value)
    for index, value in enumerate((title + bytes(20))[:20]):
        setup += mov_ds_byte(0xBF68 + index, value)
    return setup + mov_si(DSEG_SCRATCH)


def mod_1021e_wrapper(first: int, second: int, pattern: bytes, title: bytes) -> bytes:
    setup = setup_mod_1021e(first, second, pattern, title)
    continuation_ip = WRAPPER_IP + len(setup) + 9
    abs_call = mov_ax(continuation_ip) + b"\x50" + mov_ax(original_offset("mod_1021E")) + b"\x50\xc3"
    post = b""
    post += copy_bytes_to_scratch(0x0058, DSEG_SCRATCH, 4)
    post += copy_bytes_to_scratch(0x3A48, DSEG_SCRATCH + 4, 128)
    post += copy_bytes_to_scratch(0x0110, DSEG_SCRATCH + 132, 20)
    return setup + abs_call + post + b"\xc3"


def printable_prefix(data: bytes, count: int) -> bytes:
    out = bytearray()
    for value in data:
        if value < 0x20 or len(out) == count:
            break
        out.append(value)
    return bytes(out).ljust(count, b" ")


def test_original_and_translated_mod_1021e_header_unpack() -> None:
    cases = [
        (3, 0x77, bytes(range(128)), b"Song Title"),
        (9, 0x80, bytes((i * 5 + 1) & 0xFF for i in range(128)), b"Name\x00Hidden"),
    ]
    for first, second, pattern, title in cases:
        _, data = original_run(
            mod_1021e_wrapper(first, second, pattern, title),
            dump_count=152,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("mod1021e", hex(first), hex(second), pattern.hex(), title.hex())
        if got is not None:
            assert got.endswith("data=" + data.hex())
        expected_second = second if second < 0x78 else 0
        assert data[:4] == struct.pack("<HH", expected_second, first)
        assert data[4:132] == pattern[:128]
        assert data[132:] == printable_prefix(title, 20)


def test_original_and_abi_mod_1021e_public_symbol_header_unpack() -> None:
    cases = [
        (3, 0x77, bytes(range(128)), b"Song Title"),
        (9, 0x80, bytes((i * 5 + 1) & 0xFF for i in range(128)), b"Name\x00Hidden"),
    ]
    for first, second, pattern, title in cases:
        _, data = original_run(
            mod_1021e_wrapper(first, second, pattern, title),
            dump_count=152,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("abimod1021e", hex(first), hex(second), pattern.hex(), title.hex())
        if got is not None:
            assert got.endswith("data=" + data.hex())


def write_mod_sample_header(setup: bytes, index: int, name: bytes, length_words: int, finetune: int, volume: int, repeat_words: int, repeat_len_words: int) -> bytes:
    base = 0xBF68 + index * 0x1E
    for pos, value in enumerate((name + bytes(22))[:22]):
        setup += mov_ds_byte(base + 20 + pos, value)
    for off, value in [
        (0x2A, (length_words >> 8) & 0xFF),
        (0x2B, length_words & 0xFF),
        (0x2C, finetune),
        (0x2D, volume),
        (0x2E, (repeat_words >> 8) & 0xFF),
        (0x2F, repeat_words & 0xFF),
        (0x30, (repeat_len_words >> 8) & 0xFF),
        (0x31, repeat_len_words & 0xFF),
    ]:
        setup += mov_ds_byte(base + off, value)
    return setup


def setup_mod_1024a(samples: list[tuple[bytes, int, int, int, int, int]], freq: int = 8363) -> bytes:
    setup = setup_data_common()
    setup += mov_ds_word(0x0032, len(samples))
    setup += mov_ds_word(0x003E, freq)
    setup += mov_ds_dword(0x0024, 0)
    setup += mov_ds_word(0x00C2, 0)
    for index, sample in enumerate(samples):
        setup = write_mod_sample_header(setup, index, *sample)
    return setup


def mod_1024a_wrapper(samples: list[tuple[bytes, int, int, int, int, int]], freq: int = 8363) -> bytes:
    setup = setup_mod_1024a(samples, freq)
    continuation_ip = WRAPPER_IP + len(setup) + 9
    abs_call = mov_ax(continuation_ip) + b"\x50" + mov_ax(original_offset("mod_1024A")) + b"\x50\xc3"
    post = b""
    post += copy_bytes_to_scratch(0x0024, DSEG_SCRATCH, 4)
    post += copy_bytes_to_scratch(0x00C2, DSEG_SCRATCH + 4, 2)
    post += copy_bytes_to_scratch(0x1D68, DSEG_SCRATCH + 6, len(samples) * 0x40)
    return setup + abs_call + post + b"\xc3"


def encode_mod1024_samples(samples: list[tuple[bytes, int, int, int, int, int]]) -> str:
    encoded = bytearray()
    for name, length_words, finetune, volume, repeat_words, repeat_len_words in samples:
        encoded += (name + bytes(22))[:22]
        encoded += struct.pack(">HBBHH", length_words & 0xFFFF, finetune & 0xFF, volume & 0xFF, repeat_words & 0xFFFF, repeat_len_words & 0xFFFF)
    return encoded.hex()


def test_original_and_translated_mod_1024a_sample_header_unpack() -> None:
    cases = [
        [(b"Kick", 0x0010, 0x03, 0x40, 0x0000, 0x0001)],
        [(b"Looped", 0x0040, 0x0E, 0x20, 0x0010, 0x0008), (b"Tiny\x00Tail", 0x0001, 0x01, 0x10, 0x0000, 0x0001)],
    ]
    for samples in cases:
        _, data = original_run(
            mod_1024a_wrapper(samples),
            dump_count=6 + len(samples) * 0x40,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("mod1024a", hex(len(samples)), encode_mod1024_samples(samples))
        if got is not None:
            assert got.endswith("data=" + data.hex())
        total = sum((sample[1] & 0xFFFF) * 2 for sample in samples) & 0xFFFFFFFF
        assert data[:4] == struct.pack("<I", total)


def test_original_and_abi_mod_1024a_public_symbol_sample_header_unpack() -> None:
    cases = [
        [(b"Kick", 0x0010, 0x03, 0x40, 0x0000, 0x0001)],
        [(b"Looped", 0x0040, 0x0E, 0x20, 0x0010, 0x0008), (b"Tiny\x00Tail", 0x0001, 0x01, 0x10, 0x0000, 0x0001)],
    ]
    for samples in cases:
        _, data = original_run(
            mod_1024a_wrapper(samples),
            dump_count=6 + len(samples) * 0x40,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("abimod1024a", hex(len(samples)), encode_mod1024_samples(samples))
        if got is not None:
            assert got.endswith("data=" + data.hex())


def setup_memfree_18a28_guard(memflag: int) -> bytes:
    return setup_data_common() + mov_ds_byte(0x00FA, memflag) + mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)


def test_original_and_translated_memfree_18a28_guard_return_without_dos_free() -> None:
    for memflag in [0, 2, 0xFF]:
        out, _ = original_call(original_offset("memfree_18A28"), setup_memfree_18a28_guard(memflag))
        got = translated("memfree18a28", hex(memflag))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_memfree_18a28_public_symbol_guard_return_without_dos_free() -> None:
    for memflag in [0, 2, 0xFF]:
        out, _ = original_call(original_offset("memfree_18A28"), setup_memfree_18a28_guard(memflag))
        got = translated("abimemfree18a28", hex(memflag))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def setup_sub_11c0c(count: int, values: bytes) -> bytes:
    setup = setup_data_common()
    for index, value in enumerate(values):
        setup += mov_ds_byte(index, value)
    return setup + mov_ax(count)


def test_original_and_translated_sub_11c0c_encoded_item_skipper() -> None:
    cases = [
        (0, b"\x00\x00\x00"),
        (1, b"\x01\x02\x03"),
        (3, b"\x01\x02\x03\x04"),
    ]
    for count, values in cases:
        out, _ = original_call(original_offset("sub_11C0C"), setup_sub_11c0c(count, values))
        got = translated("sub11c0c", hex(count), values.hex())
        if got is not None:
            assert field(got, "si") == field(out, "si")
            assert field(got, "ax") == field(out, "ax")


def test_original_and_abi_sub_11c0c_public_symbol_encoded_item_skipper() -> None:
    cases = [
        (0, b"\x00\x00\x00"),
        (1, b"\x01\x02\x03"),
        (3, b"\x01\x02\x03\x04"),
    ]
    for count, values in cases:
        out, _ = original_call(original_offset("sub_11C0C"), setup_sub_11c0c(count, values))
        got = translated("abisub11c0c", hex(count), values.hex())
        if got is not None:
            assert field(got, "si") == field(out, "si")
            assert field(got, "ax") == field(out, "ax")


def setup_sub_1415e(index: int, total: int, segment_index: int, pending: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_dword(0x0014, 0xAAAAAAAA)
        + mov_ds_word(0x0050, index)
        + mov_ds_word(0x0054, 0x7777)
        + mov_ds_word(0x0056, 0x7777)
        + mov_ds_word(0x005A, total)
        + mov_ds_byte(0x00C9, pending)
        + mov_ds_byte(0x00CA, 0xAA)
        + mov_ds_byte(0x00CB, 0xBB)
        + mov_ds_byte(0x00CC, 0xCC)
        + mov_ds_byte(0x00CD, 0xDD)
        + mov_ds_word(0x3648 + (segment_index * 2), DATA_SEG)
        + mov_ds_byte(0x3A48 + index, segment_index)
        + mov_ds_byte(0x3B48 + index, 0)
        + mov_ds_byte(0x3D48 + (index >> 3), 0)
    )


def wrapper_sub_1415e(index: int, total: int, segment_index: int, pending: int) -> bytes:
    setup = setup_sub_1415e(index, total, segment_index, pending)
    call_ip = WRAPPER_IP + len(setup)
    post = b""
    post += copy_bytes_to_scratch(0x0014, DSEG_SCRATCH, 2)
    post += copy_bytes_to_scratch(0x0050, DSEG_SCRATCH + 2, 12)
    post += copy_bytes_to_scratch(0x00C9, DSEG_SCRATCH + 14, 5)
    post += copy_bytes_to_scratch(0x3D48 + (index >> 3), DSEG_SCRATCH + 19, 1)
    return setup + call_rel16(original_offset("sub_1415E"), call_ip + 3) + post + b"\xc3"


def wrapper_sub_1415e_regs(index: int, total: int, segment_index: int, pending: int) -> bytes:
    setup = setup_sub_1415e(index, total, segment_index, pending)
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("sub_1415E"), call_ip + 3) + b"\xc3"


def test_original_and_translated_sub_1415e_segment_cursor_setup() -> None:
    cases = [
        (0, 1, 0, 0),
        (5, 8, 3, 0),
    ]
    for index, total, segment_index, pending in cases:
        out, data = original_run(
            wrapper_sub_1415e(index, total, segment_index, pending),
            dump_count=20,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("sub1415e", hex(index), hex(total), hex(segment_index), hex(pending))
        expected = bytearray(20)
        expected[2:4] = struct.pack("<H", index)
        expected[6:8] = struct.pack("<H", segment_index)
        expected[8:10] = struct.pack("<H", pending)
        expected[12:14] = struct.pack("<H", total)
        expected[19] = 1 << (index & 7)
        assert data == bytes(expected)
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")


def test_original_and_abi_sub_1415e_public_symbol_segment_cursor_setup() -> None:
    cases = [
        (0, 1, 0, 0),
        (5, 8, 3, 0),
    ]
    for index, total, segment_index, pending in cases:
        _, data = original_run(
            wrapper_sub_1415e(index, total, segment_index, pending),
            dump_count=20,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        out_regs, _ = original_run(wrapper_sub_1415e_regs(index, total, segment_index, pending))
        got = translated("abisub1415e", hex(index), hex(total), hex(segment_index), hex(pending))
        if got is not None:
            assert field(got, "data") == data.hex()
            assert field(got, "si") == field(out_regs, "si")


def setup_sub_12f56(index: int, total: int, segment_index: int, pending: int, bh: int) -> bytes:
    return (
        setup_sub_1415e(index, total, segment_index, pending)
        + mov_ds_byte(0x00C9, 0x99)
        + mov_ax(index)
        + mov_bx(((bh & 0xFF) << 8) | (pending & 0xFF))
    )


def wrapper_sub_12f56(index: int, total: int, segment_index: int, pending: int, bh: int) -> bytes:
    setup = setup_sub_12f56(index, total, segment_index, pending, bh)
    post = b""
    post += copy_bytes_to_scratch(0x0014, DSEG_SCRATCH, 2)
    post += copy_bytes_to_scratch(0x0050, DSEG_SCRATCH + 2, 12)
    post += copy_bytes_to_scratch(0x00C9, DSEG_SCRATCH + 14, 5)
    post += copy_bytes_to_scratch(0x3D48 + (index >> 3), DSEG_SCRATCH + 19, 1)
    return setup + b"\x9a" + struct.pack("<HH", original_offset("sub_12F56"), LOAD_SEG) + post + b"\xc3"


def wrapper_sub_12f56_regs(index: int, total: int, segment_index: int, pending: int, bh: int) -> bytes:
    setup = setup_sub_12f56(index, total, segment_index, pending, bh)
    return setup + b"\x9a" + struct.pack("<HH", original_offset("sub_12F56"), LOAD_SEG) + b"\xc3"


def test_original_and_translated_sub_12f56_far_segment_cursor_setup() -> None:
    cases = [
        (2, 4, 1, 0, 0),
        (6, 9, 4, 0, 2),
    ]
    for index, total, segment_index, pending, bh in cases:
        out, data = original_run(
            wrapper_sub_12f56(index, total, segment_index, pending, bh),
            dump_count=20,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        got = translated("sub12f56", hex(index), hex(total), hex(segment_index), hex(pending), hex(bh))
        expected = bytearray(20)
        expected[2:4] = struct.pack("<H", index)
        expected[6:8] = struct.pack("<H", segment_index)
        expected[8:10] = struct.pack("<H", pending)
        expected[12:14] = struct.pack("<H", total)
        expected[19] = 1 << (index & 7)
        assert data == bytes(expected)
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")


def test_original_and_abi_sub_12f56_public_symbol_far_segment_cursor_setup() -> None:
    cases = [
        (2, 4, 1, 0, 0),
        (6, 9, 4, 0, 2),
    ]
    for index, total, segment_index, pending, bh in cases:
        _, data = original_run(
            wrapper_sub_12f56(index, total, segment_index, pending, bh),
            dump_count=20,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
        )
        out_regs, _ = original_run(wrapper_sub_12f56_regs(index, total, segment_index, pending, bh))
        got = translated("abisub12f56", hex(index), hex(total), hex(segment_index), hex(pending), hex(bh))
        if got is not None:
            assert field(got, "data") == data.hex()
            assert field(got, "si") == field(out_regs, "si")


def setup_sub_154f4(buffer_size2: int, flag: int, sample_ptr: int, period: int, volume_index: int, seg_base: int, interp_word: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x0044, buffer_size2)
        + mov_ds_byte(0x00D2, flag)
        + mov_ds_byte(0x0104, 0)
        + mov_ds_dword(DSEG_SCRATCH + 0x04, sample_ptr)
        + mov_ds_word(DSEG_SCRATCH + 0x20, period)
        + mov_ds_byte(DSEG_SCRATCH + 0x23, volume_index)
        + mov_ds_word(DSEG_SCRATCH + 0x24, seg_base)
        + mov_ds_word(DSEG_SCRATCH + 0x26, 0xFFFF)
        + mov_ds_word(DSEG_SCRATCH + 0x36, interp_word)
        + mov_si(DSEG_SCRATCH)
    )


def wrapper_sub_154f4(buffer_size2: int, flag: int, sample_ptr: int, period: int, volume_index: int, seg_base: int, interp_word: int) -> bytes:
    setup = setup_sub_154f4(buffer_size2, flag, sample_ptr, period, volume_index, seg_base, interp_word)
    call_ip = WRAPPER_IP + len(setup)
    post = setup_data_common()
    post += copy_bytes_to_scratch(0x0074, DSEG_SCRATCH + 0x80, 3)
    post += copy_bytes_to_scratch(0x00E3, DSEG_SCRATCH + 0x83, 1)
    return setup + call_rel16(original_offset("sub_154F4"), call_ip + 3) + post + b"\xc3"


def wrapper_sub_154f4_direct(buffer_size2: int, flag: int, sample_ptr: int, period: int, volume_index: int, seg_base: int, interp_word: int) -> bytes:
    setup = setup_sub_154f4(buffer_size2, flag, sample_ptr, period, volume_index, seg_base, interp_word)
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("sub_154F4"), call_ip + 3) + b"\xc3"


def test_original_and_translated_sub_154f4_sample_cursor_setup_without_ems() -> None:
    cases = [
        (0x0120, 0x00, 0x00123456, 0x3456, 0x07, 0x2000, 0x1212),
        (0x0230, 0x10, 0x00ABCDEF, 0x89AB, 0x03, 0x3000, 0x3412),
        (0x0FF0, 0x10, 0x00000F80, 0x1201, 0x7F, 0x4000, 0x3434),
        (0x0000, 0x00, 0x00FFF123, 0x00F0, 0x00, 0x5000, 0x7856),
    ]
    for case in cases:
        out, data = original_run(
            wrapper_sub_154f4_direct(*case),
            dump_count=0x70,
            dump_offset=0x0074,
            dump_seg=DATA_SEG,
        )
        got = translated("sub154f4", *(hex(value) for value in case))
        expected_data = data[:3] + data[0x6F:0x70]
        expected_interp = 1 if (case[1] & 0x10) and ((case[6] & 0xFF) != ((case[6] >> 8) & 0xFF)) else 0
        assert expected_data[:2] == struct.pack("<H", case[6])
        assert expected_data[2] == expected_interp
        assert expected_data[3] == (case[0] >> 4) & 0xFF
        if got is not None:
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")
            assert got.endswith("data=" + expected_data.hex())


def test_original_and_abi_sub_154f4_public_symbol_sample_cursor_setup_without_ems() -> None:
    cases = [
        (0x0120, 0x00, 0x00123456, 0x3456, 0x07, 0x2000, 0x1212),
        (0x0230, 0x10, 0x00ABCDEF, 0x89AB, 0x03, 0x3000, 0x3412),
        (0x0FF0, 0x10, 0x00000F80, 0x1201, 0x7F, 0x4000, 0x3434),
        (0x0000, 0x00, 0x00FFF123, 0x00F0, 0x00, 0x5000, 0x7856),
    ]
    for case in cases:
        out, data = original_run(
            wrapper_sub_154f4_direct(*case),
            dump_count=0x70,
            dump_offset=0x0074,
            dump_seg=DATA_SEG,
        )
        expected_data = data[:3] + data[0x6F:0x70]
        got = translated("abisub154f4", *(hex(value) for value in case))
        if got is not None:
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "data") == expected_data.hex()


def wrapper_sub_135ca_zero_event() -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_word(0x0034, 1)
        + mov_ds_dword(0x0014, (DATA_SEG << 16) | DSEG_SCRATCH)
        + mov_ds_byte(DSEG_SCRATCH, 0)
        + mov_ds_word(0x1368 + 0x0A, 0xBEEF)
        + mov_ds_byte(0x1368 + 0x17, 0)
        + mov_ds_byte(0x1368 + 0x3D, 0xAA)
        + mov_ds_word(0x002A, FALLBACK_OFFSETS["nullsub_5"])
    )
    call_ip = WRAPPER_IP + len(setup)
    post = b""
    post += copy_bytes_to_scratch(0x0014, DSEG_SCRATCH + 0x80, 2)
    post += copy_bytes_to_scratch(0x1368 + 0x0A, DSEG_SCRATCH + 0x82, 2)
    post += copy_bytes_to_scratch(0x1368 + 0x17, DSEG_SCRATCH + 0x84, 1)
    post += copy_bytes_to_scratch(0x1368 + 0x3D, DSEG_SCRATCH + 0x85, 1)
    return setup + call_rel16(original_offset("sub_135CA"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_sub_135ca_zero_event_channel_reset() -> None:
    out, data = original_run(
        wrapper_sub_135ca_zero_event(),
        dump_count=6,
        dump_offset=DSEG_SCRATCH + 0x80,
        dump_seg=DATA_SEG,
    )
    got = translated("sub135ca")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "si") == field(out, "si")
    assert data == struct.pack("<H", DSEG_SCRATCH + 1) + b"\x00\x00\x00\x00"


def test_original_and_abi_sub_135ca_public_symbol_zero_event_channel_reset() -> None:
    _, data = original_run(
        wrapper_sub_135ca_zero_event(),
        dump_count=6,
        dump_offset=DSEG_SCRATCH + 0x80,
        dump_seg=DATA_SEG,
    )
    got = translated("abisub135ca")
    if got is not None:
        assert field(got, "data") == data.hex()
    assert data == struct.pack("<H", DSEG_SCRATCH + 1) + b"\x00\x00\x00\x00"


def setup_sub_13813_out_of_range() -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(DSEG_SCRATCH + 0x0A, 33)
        + mov_ds_byte(DSEG_SCRATCH + 0x0B, 0x7C)
        + mov_bx(DSEG_SCRATCH)
        + mov_ax(0x1234)
        + mov_cx(0x5678)
        + mov_dx(0x9ABC)
        + mov_di(0xDEF0)
    )


def test_original_and_translated_sub_13813_out_of_range_effect_guard() -> None:
    out, data = original_run(
        make_wrapper(original_offset("sub_13813"), setup_sub_13813_out_of_range()),
        dump_count=2,
        dump_offset=DSEG_SCRATCH + 0x0A,
        dump_seg=DATA_SEG,
    )
    got = translated("sub13813")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()
    assert data == b"\x21\x7c"


def test_original_and_abi_sub_13813_public_symbol_out_of_range_effect_guard() -> None:
    out, data = original_run(
        make_wrapper(original_offset("sub_13813"), setup_sub_13813_out_of_range()),
        dump_count=2,
        dump_offset=DSEG_SCRATCH + 0x0A,
        dump_seg=DATA_SEG,
    )
    got = translated("abisub13813")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()


def wrapper_sub_140b6_early_guard() -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_byte(0x00D1, 1)
        + mov_ds_byte(0x00C8, 0)
        + mov_ds_byte(0x00C8, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = b""
    post += copy_bytes_to_scratch(0x00D1, DSEG_SCRATCH + 0x90, 1)
    post += copy_bytes_to_scratch(0x00C8, DSEG_SCRATCH + 0x91, 1)
    return setup + call_rel16(original_offset("sub_140B6"), call_ip + 3) + post + b"\xc3"


def setup_sub_140b6_early_guard_direct() -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x00D1, 1)
        + mov_ds_byte(0x00C8, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
    )


def test_original_and_translated_sub_140b6_volume_mode_early_return() -> None:
    out, data = original_run(
        wrapper_sub_140b6_early_guard(),
        dump_count=2,
        dump_offset=DSEG_SCRATCH + 0x90,
        dump_seg=DATA_SEG,
    )
    got = translated("sub140b6guard")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "data") == data.hex()
    assert data == b"\x01\x00"


def test_original_and_abi_sub_140b6_public_symbol_volume_mode_early_return() -> None:
    out, data = original_run(
        make_wrapper(original_offset("sub_140B6"), setup_sub_140b6_early_guard_direct()),
        dump_count=2,
        dump_offset=0x00D1,
        dump_seg=DATA_SEG,
    )
    got = translated("abisub140b6guard")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "data") == data.hex()


def wrapper_volume_prep_inactive(word_24610: int, size: int) -> bytes:
    out = DSEG_SCRATCH + 0x100
    dump = DSEG_SCRATCH + 0x200
    setup = (
        setup_data_common()
        + mov_ds_word(0x0034, 1)
        + mov_ds_byte(0x0082, 0)
        + mov_ds_byte(0x0104, 0)
        + mov_ds_byte(0x1368 + 0x17, 0)
        + mov_ax(word_24610)
        + mov_cx(size)
        + mov_di(out)
    )
    for index in range(size):
        setup += mov_ds_byte(out + index, 0xA5)
    post = b""
    post += copy_bytes_to_scratch(0x0070, dump, 4)
    post += copy_bytes_to_scratch(out, dump + 4, 8)
    return setup + b"\x9a" + struct.pack("<HH", original_offset("volume_prep"), LOAD_SEG) + post + b"\xc3"


def test_original_and_translated_volume_prep_inactive_channel_zero_fill() -> None:
    cases = [(0x0012, 8), (0x0034, 8)]
    for word_24610, size in cases:
        out, data = original_run(
            wrapper_volume_prep_inactive(word_24610, size),
            dump_count=12,
            dump_offset=DSEG_SCRATCH + 0x200,
            dump_seg=DATA_SEG,
        )
        got = translated("volumeprepinactive", hex(word_24610), hex(size))
        if got is not None:
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "di") == field(out, "di")
            assert field(got, "data") == data.hex()
        assert data[:4] == struct.pack("<HH", word_24610, size)
        assert data[4:] == b"\x00" * 8


def test_original_and_abi_volume_prep_public_symbol_inactive_channel_zero_fill() -> None:
    cases = [(0x0012, 8), (0x0034, 8)]
    for word_24610, size in cases:
        out, data = original_run(
            wrapper_volume_prep_inactive(word_24610, size),
            dump_count=12,
            dump_offset=DSEG_SCRATCH + 0x200,
            dump_seg=DATA_SEG,
        )
        got = translated("abivolumeprepinactive", hex(word_24610), hex(size))
        if got is not None:
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "data") == data.hex()
        assert data[:4] == struct.pack("<HH", word_24610, size)
        assert data[4:] == b"\x00" * 8


def wrapper_sub_1281a_small_mix(sample: int = 0) -> bytes:
    out = DSEG_SCRATCH + 0x2A0
    dump = DSEG_SCRATCH + 0x2C0
    channel = 0x1368
    table = 0x3D68
    setup = (
        setup_data_with_fs_common()
        + mov_ds_word(0x0070, 0x0001)
        + mov_ds_word(0x0072, 4)
        + mov_ds_word(channel + 0x20, 0x0100)
        + mov_ds_byte(channel + 0x23, 0)
        + mov_ds_byte(0, sample)
        + mov_ds_byte(1, 1)
        + mov_ds_byte(2, 2)
        + mov_ds_byte(3, 3)
        + mov_ds_byte(table + 1, 0x11)
        + mov_ds_byte(table + 3, 0x22)
        + mov_ds_byte(table + 5, 0x33)
        + mov_ds_byte(table + 7, 0x44)
        + mov_ax(0)
        + mov_di(out)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(out, dump, 4)
    return setup + call_rel16(original_offset("sub_1281A"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_sub_1281a_small_mix_tail() -> None:
    for sample, expected in [(0, 0x11), (2, 0x33)]:
        out, data = original_run(
            wrapper_sub_1281a_small_mix(sample),
            dump_count=4,
            dump_offset=DSEG_SCRATCH + 0x2C0,
            dump_seg=DATA_SEG,
        )
        got = translated("sub1281asmallmix", hex(sample))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")
            assert field(got, "data") == data.hex()
        assert data == bytes([expected, expected, expected, expected])


def test_original_and_abi_sub_1281a_public_symbol_small_mix_tail() -> None:
    for sample in [0, 2]:
        out, data = original_run(
            wrapper_sub_1281a_small_mix(sample),
            dump_count=4,
            dump_offset=DSEG_SCRATCH + 0x2C0,
            dump_seg=DATA_SEG,
        )
        got = translated("abisub1281asmallmix", hex(sample))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")
            assert field(got, "data") == data.hex()


def wrapper_spectr_1b084_len2() -> bytes:
    buf = DSEG_SCRATCH + 0x300
    dump = DSEG_SCRATCH + 0x340
    setup = (
        setup_dseg_common()
        + mov_ds_word(0x7D24, 2)
        + mov_ds_word(0x7D30, 1)
        + mov_ds_dword(buf + 0, 0x00010000)
        + mov_ds_dword(buf + 4, 0x00020000)
        + mov_ds_dword(buf + 8, 0x00030000)
        + mov_ds_dword(buf + 12, 0x00040000)
        + mov_di(buf)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = (
        copy_bytes_to_scratch(buf, dump, 16)
        + copy_bytes_to_scratch(0x7D1E, dump + 16, 2)
        + copy_bytes_to_scratch(0x7CD8, dump + 18, 12)
    )
    return setup + call_rel16(original_offset("spectr_1B084"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_spectr_1b084_len2_transform() -> None:
    out, data = original_run(
        wrapper_spectr_1b084_len2(),
        dump_count=30,
        dump_offset=DSEG_SCRATCH + 0x340,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    got = translated("spectr1b084len2")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()
    assert data[:16] != bytes(16)


def wrapper_f5_draw_spectr_inactive() -> bytes:
    dump = DSEG_SCRATCH + 0x260
    setup = (
        setup_data_common()
        + mov_ds_word(0x0034, 1)
        + mov_ds_byte(0x0082, 0)
        + mov_ds_byte(0x0104, 0)
        + mov_ds_word(0x1654, 1)
        + mov_ds_dword(0x1638, ((DSEG & 0xFFFF) << 16) | DSEG_SCRATCH)
        + mov_ds_word(0x7D34, 2)
        + mov_ds_byte(0x1368 + 0x17, 0)
        + mov_ds_byte(DSEG_SCRATCH + 0x3A, 0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = (
        copy_bytes_to_scratch(0x7A14, dump, 16)
        + copy_bytes_to_scratch(0x7758, dump + 16, 16)
    )
    return setup + call_rel16(original_offset("f5_draw_spectr"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_f5_draw_spectr_inactive_channel_snapshot() -> None:
    out, data = original_run(
        wrapper_f5_draw_spectr_inactive(),
        dump_count=32,
        dump_offset=DSEG_SCRATCH + 0x260,
        dump_seg=DATA_SEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("f5drawspectrinactive")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()
    assert data == bytes(32)


def wrapper_fillbuf(symbol: str, count: int) -> bytes:
    src = DSEG_SCRATCH
    dst = DSEG_SCRATCH + 0x100
    dump = DSEG_SCRATCH + 0x200
    setup = setup_data_common() + mov_ds_byte(0x0085, 0) + mov_si(src) + mov_di(dst) + mov_cx(count)
    for index in range(64):
        setup += mov_ds_byte(src + index, 0x10 + index)
    for index in range(8):
        setup += mov_ds_byte(dst + index, 0xA5)
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(dst, dump, 8)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_dma_buffer_fillers_small_chunks() -> None:
    cases = [
        ("fill_dmabuf8", 3, bytes([0x90, 0x98, 0xA0]) + b"\xA5" * 5),
        ("fill_dmabuf8stereo", 2, bytes([0x91, 0x95]) + b"\xA5" * 6),
        ("fill_dmabuf16stereo", 1, bytes([0x10, 0x11, 0x14, 0x15]) + b"\xA5" * 4),
    ]
    for symbol, count, expected in cases:
        out, data = original_run(
            wrapper_fillbuf(symbol, count),
            dump_count=8,
            dump_offset=DSEG_SCRATCH + 0x200,
            dump_seg=DATA_SEG,
        )
        got = translated("fillbuf", symbol, hex(count))
        if got is not None:
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")
            assert field(got, "data") == data.hex()
        assert data == expected


def test_original_and_abi_dma_buffer_fillers_public_symbols_small_chunks() -> None:
    cases = [
        ("fill_dmabuf8", 3),
        ("fill_dmabuf8stereo", 2),
        ("fill_dmabuf16stereo", 1),
    ]
    for symbol, count in cases:
        out, data = original_run(
            wrapper_fillbuf(symbol, count),
            dump_count=8,
            dump_offset=DSEG_SCRATCH + 0x200,
            dump_seg=DATA_SEG,
        )
        got = translated("abidmafillbuf", symbol, hex(count))
        if got is not None:
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")
            assert field(got, "data") == data.hex()


def wrapper_fill_dma_inactive_mono_small() -> bytes:
    dma_seg = DATA_SEG + 0x0300
    setup = (
        setup_data_common()
        + mov_ds_word(0x0018, 0)
        + mov_ds_word(0x001A, dma_seg)
        + mov_ds_word(0x0034, 1)
        + mov_ds_word(0x0048, 3)
        + mov_ds_word(0x004E, 2)
        + mov_ds_word(0x005E, 0)
        + mov_ds_byte(0x0083, 0)
        + mov_ds_byte(0x0085, 0)
        + mov_ds_byte(0x1368 + 0x1D, 1)
        + mov_ds_byte(0xBF68 + 1, 0x10)
        + mov_ds_byte(0xBF68 + 9, 0x18)
        + mov_ds_byte(0xBF68 + 17, 0x20)
    )
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("fill_dma"), call_ip + 3) + b"\xc3"


def test_original_and_translated_fill_dma_inactive_mono_small_chunk() -> None:
    out, data = original_run(
        wrapper_fill_dma_inactive_mono_small(),
        dump_count=8,
        dump_offset=0,
        dump_seg=DATA_SEG + 0x0300,
        strict=False,
    )
    got = translated("filldmainactivemono")
    if got is not None:
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()
    assert data == bytes([0x90, 0x98, 0xA0]) + b"\x00" * 5


def test_original_and_abi_fill_dma_public_symbol_inactive_mono_small_chunk() -> None:
    out, data = original_run(
        wrapper_fill_dma_inactive_mono_small(),
        dump_count=8,
        dump_offset=0,
        dump_seg=DATA_SEG + 0x0300,
        strict=False,
    )
    got = translated("abifilldmainactivemono")
    if got is not None:
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()


def setup_seg001_keyboard_flags(value: int) -> bytes:
    return mov_ax(LOAD_SEG + SEG001_DELTA) + b"\x8e\xd8" + mov_ds_word(0x30F6, value)


def setup_int9_keyb_no_scancode() -> bytes:
    return (
        mov_ax(LOAD_SEG + SEG001_DELTA)
        + b"\x8e\xd8"
        + mov_ds_word(0x30F4, 0xAAAA)
        + mov_ds_word(0x30F6, 0x1357)
        + mov_ds_byte(0x30F8, 0)
        + mov_ds_byte(0x3168, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def test_original_and_translated_get_keybsw_reads_bios_flag_word() -> None:
    _, data = original_seg001_call(
        original_offset("get_keybsw"),
        setup_seg001_keyboard_flags(0xBEEF),
        dump_count=2,
        dump_offset=0x30F6,
        dump_seg=LOAD_SEG + SEG001_DELTA,
    )
    got = translated("keybsw", "get", hex(struct.unpack("<H", data)[0]))
    if got is not None:
        assert field(got, "data") == data.hex()


def test_original_and_abi_get_keybsw_public_symbol_reads_bios_flag_word() -> None:
    _, data = original_seg001_call(
        original_offset("get_keybsw"),
        setup_seg001_keyboard_flags(0xBEEF),
        dump_count=2,
        dump_offset=0x30F6,
        dump_seg=LOAD_SEG + SEG001_DELTA,
    )
    got = translated("abikeybsw", "get", hex(0xBEEF))
    if got is not None:
        assert field(got, "data") == data.hex()


def test_original_and_translated_int9_keyboard_interrupt_no_scancode_path() -> None:
    out, data = original_seg001_iret_call(
        original_offset("int9_keyb"),
        setup_int9_keyb_no_scancode(),
        dump_count=5,
        dump_offset=0x30F4,
        dump_seg=LOAD_SEG + SEG001_DELTA,
        strict=False,
    )
    got = translated("int9keyb")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()
    assert data == b"\x00\x00\x57\x13\x00"


def test_original_and_translated_set_keybsw_writes_bios_flag_word() -> None:
    _, data = original_seg001_call(
        original_offset("set_keybsw"),
        setup_seg001_keyboard_flags(0x1234),
        dump_count=2,
        dump_offset=0x0017,
        dump_seg=0,
        strict=False,
    )
    got = translated("keybsw", "set", hex(0x1234))
    if got is not None:
        assert field(got, "data") == data.hex()


def test_original_and_abi_set_keybsw_public_symbol_writes_bios_flag_word() -> None:
    _, data = original_seg001_call(
        original_offset("set_keybsw"),
        setup_seg001_keyboard_flags(0x1234),
        dump_count=2,
        dump_offset=0x0017,
        dump_seg=0,
        strict=False,
    )
    got = translated("abikeybsw", "set", hex(0x1234))
    if got is not None:
        assert field(got, "data") == data.hex()


def wrapper_sub_197f2_config_label(configword: int) -> bytes:
    setup = (
        setup_dseg_common()
        + mov_ds_word(0x1509, configword)
        + mov_ds_word(0x0E24, 0x2020)
        + mov_ds_byte(0x0E26, 0x20)
        + mov_ds_word(0x0E79, 0x2020)
        + mov_ds_byte(0x0E7B, 0x20)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x0E24, DSEG_SCRATCH + 0, 3) + copy_bytes_to_scratch(0x0E79, DSEG_SCRATCH + 3, 3)
    return setup + call_rel16(original_offset("sub_197F2"), call_ip + 3) + post + b"\xc3"


def wrapper_sub_197f2_regs(configword: int) -> bytes:
    setup = (
        setup_dseg_common()
        + mov_ds_word(0x1509, configword)
        + mov_ds_word(0x0E24, 0x2020)
        + mov_ds_byte(0x0E26, 0x20)
        + mov_ds_word(0x0E79, 0x2020)
        + mov_ds_byte(0x0E7B, 0x20)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("sub_197F2"), call_ip + 3) + b"\xc3"


def test_original_and_translated_sub_197f2_updates_quickread_labels() -> None:
    for configword, expected in [(0x0000, b"OffOff"), (0x0020, b"On On ")]:
        out, data = original_run(
            wrapper_sub_197f2_config_label(configword),
            dump_count=6,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DSEG,
            call_cs=LOAD_SEG + SEG001_DELTA,
            wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        )
        got = translated("sub197f2", hex(configword))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()
        assert data == expected


def test_original_and_abi_sub_197f2_public_symbol_updates_quickread_labels() -> None:
    for configword, expected in [(0x0000, b"OffOff"), (0x0020, b"On On ")]:
        out, _ = original_run(
            wrapper_sub_197f2_regs(configword),
            call_cs=LOAD_SEG + SEG001_DELTA,
            wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        )
        got = translated("abisub197f2", hex(configword))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == expected.hex()


def wrapper_useless_11787_zero_length_sample() -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_dword(REAL_CHANNELS_OFF + 0x20, 0)
        + mov_ds_word(REAL_CHANNELS_OFF + 0x30, 0x5555)
        + mov_ds_word(REAL_CHANNELS_OFF + 0x32, 0x6666)
        + mov_eax(0x87654321)
        + b"\x66\xbb" + struct.pack("<I", 0x11112222)
        + b"\x66\xb9" + struct.pack("<I", 0x12345678)
        + b"\x66\xba" + struct.pack("<I", 0x33334444)
        + mov_di(REAL_CHANNELS_OFF)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(REAL_CHANNELS_OFF + 0x20, DSEG_SCRATCH, 4)
    post += copy_bytes_to_scratch(REAL_CHANNELS_OFF + 0x30, DSEG_SCRATCH + 4, 4)
    return setup + call_rel16(original_offset("useless_11787"), call_ip + 3) + post + b"\xc3"


def wrapper_useless_11787_zero_regs() -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_dword(REAL_CHANNELS_OFF + 0x20, 0)
        + mov_ds_word(REAL_CHANNELS_OFF + 0x30, 0x5555)
        + mov_ds_word(REAL_CHANNELS_OFF + 0x32, 0x6666)
        + mov_eax(0x87654321)
        + b"\x66\xbb" + struct.pack("<I", 0x11112222)
        + b"\x66\xb9" + struct.pack("<I", 0x12345678)
        + b"\x66\xba" + struct.pack("<I", 0x33334444)
        + mov_di(REAL_CHANNELS_OFF)
    )
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("useless_11787"), call_ip + 3) + b"\xc3"


def test_original_and_translated_useless_11787_zero_length_sample_returns_cleanly() -> None:
    out, data = original_run(
        wrapper_useless_11787_zero_length_sample(),
        dump_count=8,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
    )
    got = translated("useless11787zero")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()
    assert data == b"\x00\x00\x00\x00\x55\x55\x66\x66"


def test_original_and_abi_useless_11787_public_symbol_zero_length_sample_returns_cleanly() -> None:
    out, _ = original_run(wrapper_useless_11787_zero_regs())
    got = translated("abiuseless11787zero")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == b"\x00\x00\x00\x00\x55\x55\x66\x66".hex()


def wrapper_useless_doswrite2_header_snapshot() -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_word(0x00C0, 0xFFFF)
        + mov_ds_dword(0xBF68, 0)
        + mov_ds_dword(0xBF6C, 0)
        + mov_eax(0x504D4153)
        + b"\x66\xb9" + struct.pack("<I", 0x12345678)
        + mov_dx(0x2222)
        + mov_bx(0x3333)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0xBF68, DSEG_SCRATCH, 8)
    return setup + call_rel16(original_offset("useless_doswrite2"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_useless_doswrite2_snapshots_chunk_header_before_write() -> None:
    _, data = original_run(
        wrapper_useless_doswrite2_header_snapshot(),
        dump_count=8,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("uselessdoswrite2")
    if got is not None:
        assert field(got, "data") == data.hex()
    assert data == struct.pack("<II", 0x504D4153, 0x12345678)


def test_original_and_abi_useless_doswrite2_public_symbol_snapshots_chunk_header_before_write() -> None:
    _, data = original_run(
        wrapper_useless_doswrite2_header_snapshot(),
        dump_count=8,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abiuselessdoswrite2")
    if got is not None:
        assert field(got, "data") == data.hex()


def wrapper_useless_doswrite_header_snapshot() -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_word(0x00C0, 0xFFFF)
        + mov_ds_dword(0xBF68, 0)
        + mov_ds_dword(0xBF6C, 0)
        + mov_eax(0x54534C50)
        + b"\x66\xb9" + struct.pack("<I", 0x00000080)
        + mov_dx(0x27FE8 & 0xFFFF)
        + mov_bx(0x3333)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0xBF68, DSEG_SCRATCH, 8)
    return setup + call_rel16(original_offset("useless_doswrite"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_useless_doswrite_snapshots_chunk_header_before_payload_write() -> None:
    out, data = original_run(
        wrapper_useless_doswrite_header_snapshot(),
        dump_count=8,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("uselessdoswrite")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "dx") == field(out, "dx")
    assert data == struct.pack("<II", 0x54534C50, 0x00000080)


def test_original_and_abi_useless_doswrite_public_symbol_snapshots_chunk_header_before_payload_write() -> None:
    out, data = original_run(
        wrapper_useless_doswrite_header_snapshot(),
        dump_count=8,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abiuselessdoswrite")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "dx") == field(out, "dx")


def setup_ult_1150b(value: int) -> bytes:
    return setup_load_common() + mov_ax(value) + mov_cx(0x55AA) + mov_dx(0xA55A)


def test_original_and_translated_ult_1150b_effect_byte_normalizer() -> None:
    for value in [0x1205, 0xFC0A, 0x7B0B, 0xA40C, 0xEA0E, 0xEB0E, 0x120E, 0x9933]:
        out, _ = original_call(original_offset("ult_1150B"), setup_ult_1150b(value))
        got = translated("ult1150b", hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_ult_1150b_public_symbol_effect_byte_normalizer() -> None:
    for value in [0x1205, 0xFC0A, 0x7B0B, 0xA40C, 0xEA0E, 0xEB0E, 0x120E, 0x9933]:
        out, _ = original_call(original_offset("ult_1150B"), setup_ult_1150b(value))
        got = translated("abiult1150b", hex(value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def setup_sb_helper(base_port: int, ax_value: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x00B2, base_port)
        + mov_ax(ax_value)
        + mov_bx(0x1357)
        + mov_cx(0x2468)
        + mov_dx(0x369A)
    )


def test_original_and_translated_sound_blaster_port_helpers_no_device_path() -> None:
    cases = [
        ("WriteMixerSB", 0x220, 0x1234),
        ("ReadMixerSB", 0x220, 0x5634),
        ("WriteSB", 0x240, 0x00D1),
        ("ReadSB", 0x240, 0xBEEF),
        ("CheckSB", 0x220, 0x7777),
    ]
    for symbol, base_port, ax_value in cases:
        out, _ = original_call(original_offset(symbol), setup_sb_helper(base_port, ax_value), strict=False)
        got = translated("sbhelper", symbol, hex(base_port), hex(ax_value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
        if symbol in {"ReadMixerSB", "ReadSB", "CheckSB"}:
            assert field(out, "ax")[-2:] == "00"


def test_original_and_abi_readsb_public_symbol_no_device_path() -> None:
    out, _ = original_call(original_offset("ReadSB"), setup_sb_helper(0x240, 0xBEEF), strict=False)
    got = translated("abireadsb")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
    assert field(out, "ax")[-2:] == "00"


def test_original_and_abi_readmixersb_public_symbol_no_device_path() -> None:
    out, _ = original_call(original_offset("ReadMixerSB"), setup_sb_helper(0x220, 0x5634), strict=False)
    got = translated("abireadmixersb")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
    assert field(out, "ax")[-2:] == "00"


def test_original_and_abi_writesb_public_symbol_no_device_path() -> None:
    out, _ = original_call(original_offset("WriteSB"), setup_sb_helper(0x240, 0x00D1), strict=False)
    got = translated("abiwritesb")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_writemixersb_public_symbol_no_device_path() -> None:
    out, _ = original_call(original_offset("WriteMixerSB"), setup_sb_helper(0x220, 0x1234), strict=False)
    got = translated("abiwritemixersb")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_checksb_public_symbol_no_device_path() -> None:
    out, _ = original_call(original_offset("CheckSB"), setup_sb_helper(0x220, 0x7777), strict=False)
    got = translated("abichecksb")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
    assert field(out, "ax")[-2:] == "00"


def setup_set_dmachn_mask(channel: int) -> bytes:
    return setup_data_common() + mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(channel) + mov_dx(0x9ABC)


def test_original_and_translated_set_dmachn_mask_no_device_port_write() -> None:
    for channel in [0, 2, 4, 5, 7]:
        out, _ = original_call(original_offset("set_dmachn_mask"), setup_set_dmachn_mask(channel), strict=False)
        got = translated("setdmamask", hex(channel))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_set_dmachn_mask_public_symbol_no_device_path() -> None:
    for channel in [0, 2, 4, 5, 7]:
        out, _ = original_call(original_offset("set_dmachn_mask"), setup_set_dmachn_mask(channel), strict=False)
        got = translated("abisetdmamask", hex(channel))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def setup_adlib_delay(ax_value: int) -> bytes:
    return setup_data_common() + mov_ax(ax_value) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)


def test_original_and_translated_adlib_delay_helpers_no_device_ports() -> None:
    for symbol, ax_value in [("adlib_18395", 0x1234), ("adlib_18389", 0x7777)]:
        out, _ = original_call(original_offset(symbol), setup_adlib_delay(ax_value), strict=False)
        got = translated("adlibdelay", symbol, hex(ax_value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_adlib_18389_public_symbol_delay() -> None:
    out, _ = original_call(original_offset("adlib_18389"), setup_adlib_delay(0x7777), strict=False)
    got = translated("abiadlib18389")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_adlib_18395_public_symbol_delay() -> None:
    out, _ = original_call(original_offset("adlib_18395"), setup_adlib_delay(0x1234), strict=False)
    got = translated("abiadlib18395")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def setup_ega_sequencer_helper() -> bytes:
    return setup_data_common() + mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)


def test_original_and_translated_ega_sequencer_helpers_no_device_ports() -> None:
    for symbol in ["set_egasequencer", "graph_1C070"]:
        out, _ = original_seg001_call(original_offset(symbol), setup_ega_sequencer_helper(), strict=False)
        got = translated("egaseq", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_set_egasequencer_public_symbol_no_device_path() -> None:
    out, _ = original_seg001_call(original_offset("set_egasequencer"), setup_ega_sequencer_helper(), strict=False)
    got = translated("abisetegasequencer")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_graph_1c070_public_symbol_no_device_path() -> None:
    out, _ = original_seg001_call(original_offset("graph_1C070"), setup_ega_sequencer_helper(), strict=False)
    got = translated("abigraph1c070")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def wrapper_useless_unset_egaseq(mode_bits: int) -> bytes:
    return make_wrapper(original_offset("useless_unset_egaseq"), mov_ax(mode_bits & 0xFF) + mov_dx(0xA55A))


def test_original_and_translated_useless_unset_egaseq_updates_ega_mode_low_bits() -> None:
    for mode_bits in [0, 1, 2, 3]:
        out, _ = original_run(
            wrapper_useless_unset_egaseq(mode_bits),
            call_cs=LOAD_SEG + SEG001_DELTA,
            wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
            strict=False,
        )
        ax = int(field(out, "ax"), 16)
        assert ((ax >> 8) & 0xFF) == mode_bits
        assert (ax & 0x03) == mode_bits
        assert field(out, "dx") == "03cf"
        got = translated("uselessunsetegaseq", hex(mode_bits))
        if got is not None:
            got_ax = int(field(got, "ax"), 16)
            assert ((got_ax >> 8) & 0xFF) == mode_bits
            assert (got_ax & 0x03) == mode_bits
            assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_useless_unset_egaseq_public_symbol_no_device_path() -> None:
    for mode_bits in [0, 1, 2, 3]:
        out, _ = original_run(
            wrapper_useless_unset_egaseq(mode_bits),
            call_cs=LOAD_SEG + SEG001_DELTA,
            wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
            strict=False,
        )
        got = translated("abiuselessunsetegaseq", hex(mode_bits))
        if got is not None:
            got_ax = int(field(got, "ax"), 16)
            assert ((got_ax >> 8) & 0xFF) == mode_bits
            assert (got_ax & 0x03) == mode_bits
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def wrapper_useless_strange_short_stream() -> bytes:
    stream = struct.pack("<H", 0) + bytes([0x1E, 0x3F, 0xF4, 0x0A])
    setup = (
        setup_dseg_common()
        + mov_ds_dword(0x1630, ((DSEG & 0xFFFF) << 16) | DSEG_SCRATCH)
        + mov_si(0x7777)
    )
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("useless_strange"), call_ip + 3) + stream + b"\xc3"


def test_original_and_translated_useless_strange_decodes_stack_stream_to_video() -> None:
    out, data = original_run(
        wrapper_useless_strange_short_stream(),
        dump_count=4,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    got = translated("uselessstrange")
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()
    assert data == b"O\x1eK\x1e"
    assert field(out, "si") == "7777"
    assert field(out, "di") == f"{DSEG_SCRATCH + 4:04x}"


def wrapper_useless_writeinr_118_sample_header() -> bytes:
    sample = 0x1D68 + 0x40
    setup = setup_data_common() + mov_ds_word(0x00C0, 0xFFFF) + mov_dx(1)
    name = b"SHORT SAMPLE NAME"
    for index in range(0x40):
        setup += mov_ds_byte(sample + index, 0)
    for index, value in enumerate(name):
        setup += mov_ds_byte(sample + index, value)
    setup += mov_ds_dword(sample + 0x20, 0x12345678)
    setup += mov_ds_dword(sample + 0x24, 0x11111111)
    setup += mov_ds_dword(sample + 0x2C, 0x22222222)
    setup += mov_ds_word(sample + 0x36, 0x4321)
    setup += mov_ds_byte(sample + 0x3C, 0xA5)
    setup += mov_ds_byte(sample + 0x3D, 0x40)
    setup += mov_ds_byte(sample + 0x3E, 0x7F)
    return make_far_wrapper(original_offset("useless_writeinr_118"), setup)


def test_original_and_translated_useless_writeinr_118_builds_sample_header_before_write() -> None:
    out, data = original_run(
        wrapper_useless_writeinr_118_sample_header(),
        dump_count=96,
        dump_offset=0x12A6,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("uselesswriteinr118")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "dx") == field(out, "dx")
    assert data[0x10:0x10 + 17] == b"SHORT SAMPLE NAME"
    assert data[0x40:0x44] == struct.pack("<I", 0x12345678)
    assert data[0x45] == 0x40
    assert data[0x46] == 0x7F
    assert data[0x47] == 0xA5
    assert data[0x48:0x4A] == struct.pack("<H", 0x4321)
    assert data[0x4C:0x50] == struct.pack("<I", 0x11111111)
    assert data[0x50:0x54] == struct.pack("<I", 0x22222222)


def test_original_and_abi_useless_writeinr_118_public_symbol_builds_sample_header_before_write() -> None:
    out, data = original_run(
        wrapper_useless_writeinr_118_sample_header(),
        dump_count=96,
        dump_offset=0x12A6,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abiuselesswriteinr118")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "dx") == field(out, "dx")


def wrapper_useless_writeinr_create_failure() -> bytes:
    return make_far_wrapper(original_offset("useless_writeinr"), setup_data_common() + mov_dx(0xFFFF))


def test_original_and_translated_useless_writeinr_create_failure_returns_minus_one() -> None:
    out, _ = original_run(wrapper_useless_writeinr_create_failure(), strict=False)
    got = translated("uselesswriteinrfail")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
    assert field(out, "ax") == "ffff"
    assert field(out, "cx") == "0020"
    assert field(out, "dx") == "ffff"


def test_original_and_abi_useless_writeinr_public_symbol_create_failure_returns_minus_one() -> None:
    out, _ = original_run(wrapper_useless_writeinr_create_failure(), strict=False)
    got = translated("abiuselesswriteinrfail")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def wrapper_useless_12d61_no_device_detection() -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_byte(0x0132, 0x7E)
        + mov_ds_word(0x0133, 0x1234)
        + mov_ds_byte(0x0135, 0x56)
        + mov_ds_byte(0x0136, 0x78)
        + mov_ds_byte(0x0137, 0x22)
        + mov_ds_byte(0x0138, 0x9A)
        + mov_ds_byte(0x0139, 0xBC)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x0132, DSEG_SCRATCH, 8)
    return setup + call_rel16(original_offset("useless_12D61"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_useless_12d61_no_device_detection_resets_sound_config() -> None:
    out, data = original_run(
        wrapper_useless_12d61_no_device_detection(),
        dump_count=8,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("useless12d61")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "data") == data.hex()
    assert field(out, "ax") == "0000"
    assert data == b"\x00\xff\xff\xff\xff\x22\xff\xff"


def test_original_and_abi_useless_12d61_public_symbol_no_device_detection_resets_sound_config() -> None:
    out, data = original_run(
        wrapper_useless_12d61_no_device_detection(),
        dump_count=8,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abiuseless12d61")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "data") == data.hex()


def wrapper_sb_legacy_init_no_device(symbol: str) -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_byte(0x0082, 0xAA)
        + mov_ds_byte(0x0083, 0xBB)
        + mov_ds_byte(0x0084, 0xCC)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x0082, DSEG_SCRATCH, 3)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_legacy_sb_init_no_device_paths_set_mode_flags_and_fail() -> None:
    cases = [
        ("sbpro_init", b"\x09\x01\x08"),
        ("sb_init", b"\x09\x00\x08"),
    ]
    for symbol, expected in cases:
        out, data = original_run(
            wrapper_sb_legacy_init_no_device(symbol),
            dump_count=3,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("sbinitnodevice", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "dx") == field(out, "dx")
            assert int(field(got, "flags"), 16) & 1 == int(field(out, "flags"), 16) & 1
            assert field(got, "data") == data.hex()
        assert int(field(out, "flags"), 16) & 1
        assert field(out, "dx") == "0ff6"
        assert data == expected


def test_original_and_abi_legacy_sb_init_public_symbol_no_device_paths_set_mode_flags_and_fail() -> None:
    cases = [
        ("sbpro_init", b"\x09\x01\x08"),
        ("sb_init", b"\x09\x00\x08"),
    ]
    for symbol, expected in cases:
        out, data = original_run(
            wrapper_sb_legacy_init_no_device(symbol),
            dump_count=3,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abisbinitnodevice", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "dx") == field(out, "dx")
            assert int(field(got, "flags"), 16) & 1 == int(field(out, "flags"), 16) & 1
            assert field(got, "data") == data.hex()
        assert data == expected


def test_original_and_translated_sb_detect_irq_no_device_fails_with_error_message_pointer() -> None:
    setup = setup_data_common() + mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)
    out, _ = original_call(original_offset("sb_detect_irq"), setup, strict=False)
    got = translated("sbdetectirqnodevice")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert int(field(got, "flags"), 16) & 1 == int(field(out, "flags"), 16) & 1
    assert int(field(out, "flags"), 16) & 1
    assert field(out, "dx") == "0ff6"


def test_original_and_abi_sb_detect_irq_public_symbol_no_device_path() -> None:
    setup = setup_data_common() + mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)
    out, _ = original_call(original_offset("sb_detect_irq"), setup, strict=False)
    got = translated("abisbdetectirq")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert int(field(got, "flags"), 16) & 1 == int(field(out, "flags"), 16) & 1
    assert int(field(out, "flags"), 16) & 1
    assert field(out, "dx") == "0ff6"


def wrapper_sb_test_interrupt_no_device() -> bytes:
    setup = setup_data_common() + mov_ds_byte(0x00D0, 0xAA) + mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x00D0, DSEG_SCRATCH, 1)
    return setup + call_rel16(original_offset("sb_test_interrupt"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_sb_test_interrupt_no_device_times_out_with_carry() -> None:
    out, data = original_run(
        wrapper_sb_test_interrupt_no_device(),
        dump_count=1,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("sbtestinterruptnodevice")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "si") == field(out, "si")
        assert int(field(got, "flags"), 16) & 1 == int(field(out, "flags"), 16) & 1
        assert field(got, "data") == data.hex()
    assert int(field(out, "flags"), 16) & 1
    assert data == b"\x00"


def test_original_and_abi_sb_test_interrupt_public_symbol_no_device_times_out_with_carry() -> None:
    out, data = original_run(
        wrapper_sb_test_interrupt_no_device(),
        dump_count=1,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abisbtestinterruptnodevice")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "si") == field(out, "si")
        assert int(field(got, "flags"), 16) & 1 == int(field(out, "flags"), 16) & 1
        assert field(got, "data") == data.hex()


def wrapper_sub_13017_bounded_fill_dma() -> bytes:
    fill_dma_stub = b"\xc7\x06\x60\x00\x01\x08\xc3"
    setup = setup_data_common()
    for index, value in enumerate(fill_dma_stub):
        setup += mov_cs_byte(0x6C69 + index, value)
    setup += (
        mov_ds_word(0x0032, 2)
        + mov_ds_dword(0x1D68 + 0x24, 0xAAAAAAAA)
        + mov_ds_dword(0x1D68 + 0x2C, 0x11111111)
        + mov_ds_byte(0x1D68 + 0x3C, 0)
        + mov_ds_dword(0x1D68 + 0x40 + 0x24, 0x22222222)
        + mov_ds_dword(0x1D68 + 0x40 + 0x2C, 0x33333333)
        + mov_ds_byte(0x1D68 + 0x40 + 0x3C, 8)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x1D68 + 0x24, DSEG_SCRATCH, 4)
    post += copy_bytes_to_scratch(0x1D68 + 0x40 + 0x24, DSEG_SCRATCH + 4, 4)
    post += copy_bytes_to_scratch(0x0060, DSEG_SCRATCH + 8, 2)
    return setup + call_rel16(original_offset("sub_13017"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_sub_13017_updates_unlooped_sample_end_and_waits_for_dma_fill() -> None:
    out, data = original_run(
        wrapper_sub_13017_bounded_fill_dma(),
        dump_count=10,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("sub13017bounded")
    if got is not None:
        assert field(got, "data") == data.hex()
    assert data == struct.pack("<IIH", 0x11111111, 0x22222222, 0x0801)


def wrapper_configure_timer_bounded_fill_dma() -> bytes:
    fill_dma_stub = b"\xc7\x06\x60\x00\x01\x08\xc3"
    setup = setup_data_common()
    for index, value in enumerate(fill_dma_stub):
        setup += mov_cs_byte(0x6C69 + index, value)
    setup += (
        mov_ds_word(0x00BE, 22050)
        + mov_ds_word(0x0032, 2)
        + mov_ds_dword(0x1D68 + 0x24, 0xAAAAAAAA)
        + mov_ds_dword(0x1D68 + 0x2C, 0x11111111)
        + mov_ds_byte(0x1D68 + 0x3C, 0)
        + mov_ds_dword(0x1D68 + 0x40 + 0x24, 0x22222222)
        + mov_ds_dword(0x1D68 + 0x40 + 0x2C, 0x33333333)
        + mov_ds_byte(0x1D68 + 0x40 + 0x3C, 8)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x1D68 + 0x24, DSEG_SCRATCH, 4)
    post += copy_bytes_to_scratch(0x1D68 + 0x40 + 0x24, DSEG_SCRATCH + 4, 4)
    post += copy_bytes_to_scratch(0x0060, DSEG_SCRATCH + 8, 2)
    return setup + call_rel16(original_offset("configure_timer"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_configure_timer_runs_bounded_dma_prefill_before_timer_programming() -> None:
    out, data = original_run(
        wrapper_configure_timer_bounded_fill_dma(),
        dump_count=10,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("configuretimerbounded")
    if got is not None:
        assert field(got, "data") == data.hex()
    assert data == struct.pack("<IIH", 0x11111111, 0x22222222, 0x0801)


def test_original_and_abi_timer_public_symbols_run_bounded_dma_prefill() -> None:
    cases = [
        ("sub_13017", wrapper_sub_13017_bounded_fill_dma()),
        ("configure_timer", wrapper_configure_timer_bounded_fill_dma()),
    ]
    for symbol, wrapper in cases:
        _, data = original_run(
            wrapper,
            dump_count=10,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abitimerbounded", symbol)
        if got is not None:
            assert field(got, "data") == data.hex()


def wrapper_snd_on_parnt_bounded_children() -> bytes:
    setup = setup_data_common()
    for offset in [0x2F56, 0x2B18]:
        setup += mov_cs_byte(offset, 0xCB)
    for offset in [0x3CF6, 0x3CE8, 0x420F]:
        setup += mov_cs_byte(offset, 0xC3)
    setup += (
        mov_ds_byte(0x00C9, 0x91)
        + mov_ds_byte(0x00CA, 0x92)
        + mov_ds_byte(0x00CB, 0x93)
        + mov_ds_byte(0x00CC, 0x94)
        + mov_ds_byte(0x00CD, 0x95)
        + mov_ds_byte(0x00D1, 0x96)
        + mov_ds_byte(0x00DF, 0x97)
        + mov_ds_word(0x0060, 0x1111)
        + mov_ds_word(0x0062, 0x2222)
        + mov_ds_byte(0x0080, 0x33)
        + mov_ds_byte(0x0081, 0x44)
        + mov_ds_byte(0x00D9, 6)
        + mov_ds_byte(0x00DA, 125)
    )
    for index in range(0x20):
        setup += mov_ds_byte(0x1368 + index, 0xA5)
    call = setup + b"\x9a" + struct.pack("<HH", original_offset("snd_on_parnt"), LOAD_SEG)
    post = copy_bytes_to_scratch(0x00C9, DSEG_SCRATCH, 5)
    post += copy_bytes_to_scratch(0x00D1, DSEG_SCRATCH + 5, 1)
    post += copy_bytes_to_scratch(0x00DF, DSEG_SCRATCH + 6, 1)
    post += copy_bytes_to_scratch(0x0060, DSEG_SCRATCH + 7, 4)
    post += copy_bytes_to_scratch(0x0080, DSEG_SCRATCH + 11, 2)
    post += copy_bytes_to_scratch(0x00C8, DSEG_SCRATCH + 13, 1)
    post += copy_bytes_to_scratch(0x00DB, DSEG_SCRATCH + 14, 2)
    post += copy_bytes_to_scratch(0x1368, DSEG_SCRATCH + 16, 1)
    post += copy_bytes_to_scratch(0x1368 + 0x1F, DSEG_SCRATCH + 17, 1)
    return call + post + b"\xc3"


def test_original_and_translated_snd_on_parnt_initializes_parent_state_with_bounded_children() -> None:
    out, data = original_run(
        wrapper_snd_on_parnt_bounded_children(),
        dump_count=18,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("sndonparntbounded")
    if got is not None:
        assert field(got, "data") == data.hex()
    assert data == bytes([
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0,
        6, 50, 0,
        0, 0,
    ])


def test_original_and_abi_snd_on_parnt_public_symbol_initializes_parent_state_with_bounded_children() -> None:
    _, data = original_run(
        wrapper_snd_on_parnt_bounded_children(),
        dump_count=18,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abisndonparntbounded")
    if got is not None:
        assert field(got, "data") == data.hex()


def wrapper_sb_on_bounded_hardware_waits(
    symbol: str,
    *,
    bit_mode: int = 8,
    halt_out_offset: int | None = None,
) -> bytes:
    setup = setup_data_common()
    for offset in [0x3017, 0x8A3D, 0x86E3]:
        setup += mov_cs_byte(offset, 0xC3)
    if symbol == "sb_on":
        setup += mov_cs_byte(0x8B63, 0xC3)
        wait_offsets = [0x4DC3, 0x4DCB, 0x4DD8]
    else:
        wait_offsets = [0x4B39, 0x4B41, 0x4B4A, 0x4B53, 0x4B7A, 0x4B8A, 0x4BA3]
    for offset in wait_offsets:
        setup += mov_cs_byte(offset, 0x90) + mov_cs_byte(offset + 1, 0x90)
    if halt_out_offset is not None:
        setup += mov_cs_byte(halt_out_offset, 0xC3)
    setup += (
        mov_ds_word(0x00BE, 22050)
        + mov_ds_word(0x00B2, 0x0220)
        + mov_ds_byte(0x00B9, 7)
        + mov_ds_byte(0x00BA, 0x55)
        + mov_ds_byte(0x00B8, 1)
        + mov_ds_byte(0x0083, 1 if symbol == "sb16_on" else 0)
        + mov_ds_byte(0x0084, bit_mode)
        + mov_ds_word(0x006E, 0)
        + mov_ds_byte(0x00CE, 0)
        + mov_ds_byte(0x00CF, 0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x006E, DSEG_SCRATCH, 2)
    post += copy_bytes_to_scratch(0x00CE, DSEG_SCRATCH + 2, 2)
    if halt_out_offset is not None:
        post += b"\xa3" + struct.pack("<H", DSEG_SCRATCH + 4)
        post += b"\x89\x16" + struct.pack("<H", DSEG_SCRATCH + 6)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def test_original_and_rewrite_sb16_start_command_sequence_16bit_stereo() -> None:
    commands = []
    ports = []
    for out_offset in [0x4B3D, 0x4B46, 0x4B4F, 0x4B76, 0x4B86, 0x4B9F, 0x4BA7]:
        _, data = original_run(
            wrapper_sb_on_bounded_hardware_waits(
                "sb16_on",
                bit_mode=16,
                halt_out_offset=out_offset,
            ),
            dump_count=8,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
            strict=False,
        )
        commands.append(data[4])
        ports.append(struct.unpack("<H", data[6:8])[0])

    got = run(
        [
            str(ROOT / "rewrite" / ".build" / "sb16_commands_runner"),
            "22050",
            "16",
            "1",
            "4096",
        ],
        cwd=ROOT,
    ).stdout.strip()
    assert field(got, "count") == "7"
    assert bytes(commands).hex() == field(got, "data")
    assert ports == [0x022C] * 7


def test_original_and_translated_sb_on_paths_program_bounded_start_state() -> None:
    for symbol in ["sb_on", "sb16_on"]:
        out, data = original_run(
            wrapper_sb_on_bounded_hardware_waits(symbol),
            dump_count=4,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("sbonbounded", symbol)
        if got is not None:
            assert field(got, "data") == data.hex()
        assert data == b"\x00\x10\x01\x58"


def test_original_and_abi_sb_on_public_symbol_paths_program_bounded_start_state() -> None:
    for symbol in ["sb_on", "sb16_on"]:
        out, data = original_run(
            wrapper_sb_on_bounded_hardware_waits(symbol),
            dump_count=4,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abisbonbounded", symbol)
        if got is not None:
            assert field(got, "data") == data.hex()
        assert data == b"\x00\x10\x01\x58"


def wrapper_sb_handler_int_bounded_epilogue() -> bytes:
    setup = setup_data_common()
    for offset in [0x4DF6, 0x4DFE, 0x4E0B]:
        setup += mov_cs_byte(offset, 0x90) + mov_cs_byte(offset + 1, 0x90)
    for index, value in enumerate([0x1F, 0x5A, 0x58, 0xC3]):
        setup += mov_cs_byte(0x4E10 + index, value)
    setup += mov_ds_word(0x006E, 0x1000) + mov_ax(0x1234) + mov_dx(0x022E)
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x006E, DSEG_SCRATCH, 2)
    return setup + call_rel16(original_offset("sb_handler_int"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_sb_handler_int_bounded_ack_sequence_preserves_regs() -> None:
    out, data = original_run(
        wrapper_sb_handler_int_bounded_epilogue(),
        dump_count=2,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("sbhandlerintbounded")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()
    assert field(out, "ax") == "1234"
    assert field(out, "dx") == "022e"
    assert data == b"\x00\x10"


def test_original_and_abi_sb_handler_int_public_symbol_bounded_ack_sequence_preserves_regs() -> None:
    out, data = original_run(
        wrapper_sb_handler_int_bounded_epilogue(),
        dump_count=2,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abisbhandlerintbounded")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()
    assert data == b"\x00\x10"


def wrapper_sub_19050_bounded_error_exit() -> bytes:
    setup = setup_dseg_common()
    setup += mov_cs_byte(0x33DF, 0xF9) + mov_cs_byte(0x33E0, 0xC3)
    setup += mov_cs_byte(0x0273, 0xC3)
    msg = DSEG_SCRATCH + 0x500
    for index, value in enumerate(b"ERR$"):
        setup += mov_ds_byte(msg + index, value)
    setup += mov_ds_dword(0x1634, ((DSEG & 0xFFFF) << 16) | msg)
    setup += mov_ds_byte(0x167E, 7)
    post = copy_bytes_to_scratch(0x167E, DSEG_SCRATCH, 1)
    return setup + mov_ax(original_offset("sub_19050")) + b"\xff\xd0" + post + b"\xc3"


def test_original_and_translated_sub_19050_bounded_error_path_prints_message_and_skips_exit() -> None:
    out, data = original_run(
        wrapper_sub_19050_bounded_error_exit(),
        dump_count=1,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("sub19050bounded")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()
    assert field(out, "ax") == "0900"
    assert field(out, "dx") == "0000"
    assert data == b"\x07"


def test_original_and_abi_sub_19050_public_symbol_bounded_error_path_prints_message_and_skips_exit() -> None:
    out, data = original_run(
        wrapper_sub_19050_bounded_error_exit(),
        dump_count=1,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("abisub19050bounded")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def test_original_and_translated_text_blink_bios_wrappers_no_device_int() -> None:
    setup = setup_ega_sequencer_helper()
    for symbol in ["txt_blinkingoff", "txt_enableblink"]:
        out, _ = original_seg001_call(original_offset(symbol), setup, strict=False)
        got = translated("txtblink", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_text_blink_public_symbols_no_device_int() -> None:
    setup = setup_ega_sequencer_helper()
    for symbol in ["txt_blinkingoff", "txt_enableblink"]:
        out, _ = original_seg001_call(original_offset(symbol), setup, strict=False)
        got = translated("abitxtblink", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def wrapper_useless_sprint_chunk(symbol: str, value: int) -> bytes:
    param = DSEG_SCRATCH + 0x100
    value_ptr = DSEG_SCRATCH + 0x120
    out = DSEG_SCRATCH + 0x140
    setup = (
        setup_dseg_common()
        + mov_ds_word(param, value_ptr)
        + mov_ds_byte(param + 2, 0)
        + mov_ds_dword(value_ptr, value)
        + b"".join(mov_ds_byte(out + i, 0xA5) for i in range(16))
        + mov_si(param)
        + mov_di(out)
    )
    return make_saved_es_jmp_wrapper(original_offset(symbol), setup, DSEG)


def test_original_and_translated_useless_sprintf_numeric_chunks() -> None:
    cases = [
        ("useless_sprint_6", 12345678, b"12345678"),
        ("useless_sprint_7", 0xFB, b"-5"),
        ("useless_sprint_8", 0xFF85, b"-123"),
        ("useless_sprint_9", 0xFFFE1DC0, b"-123456"),
        ("useless_sprint_10", 0xAB, b"AB"),
        ("useless_sprint_11", 0xBEEF, b"BEEF"),
        ("useless_sprint_12", 0x1234ABCD, b"1234ABCD"),
    ]
    for symbol, value, expected in cases:
        out, data = original_run(
            wrapper_useless_sprint_chunk(symbol, value),
            dump_count=16,
            dump_offset=DSEG_SCRATCH + 0x140,
            dump_seg=DSEG,
            call_cs=LOAD_SEG + SEG001_DELTA,
            wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        )
        got = translated("mysprintfchunk", symbol, hex(value))
        if got is not None:
            assert field(got, "si") == field(out, "si")
            assert field(got, "di") == field(out, "di")
            assert field(got, "es") == field(out, "es")
            assert field(got, "data") == data.hex()
        assert data[: len(expected)] == expected


def wrapper_memfill8080() -> bytes:
    dma_seg = DATA_SEG + 0x0300
    setup = (
        setup_data_common()
        + mov_ds_word(0x0018, 0)
        + mov_ds_word(0x001A, dma_seg)
        + mov_eax(0x12345678)
        + mov_ebx(0x9ABCDEF0)
        + mov_cx(0x1357)
        + mov_dx(0x2468)
    )
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("memfill8080"), call_ip + 3) + b"\xc3"


def wrapper_dma_silence_target(symbol: str) -> bytes:
    dma_seg = DATA_SEG + 0x0300
    setup = (
        setup_data_common()
        + mov_ds_word(0x0018, 0)
        + mov_ds_word(0x001A, dma_seg)
        + mov_eax(0x12345678)
        + mov_ebx(0x9ABCDEF0)
        + mov_cx(0x1357)
        + mov_dx(0x2468)
    )
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + b"\xc3"


def test_original_and_translated_memfill8080_dma_silence_fill_no_device_timer() -> None:
    out, data = original_run(
        wrapper_memfill8080(),
        dump_count=16,
        dump_offset=0,
        dump_seg=DATA_SEG + 0x0300,
        strict=False,
    )
    got = translated("memfill8080")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()
    assert data == b"\x80" * 16


def test_original_and_abi_memfill8080_public_symbol_dma_silence_fill_no_device_timer() -> None:
    out, data = original_run(
        wrapper_memfill8080(),
        dump_count=16,
        dump_offset=0,
        dump_seg=DATA_SEG + 0x0300,
        strict=False,
    )
    got = translated("abimemfill8080")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()


def test_original_and_translated_sndoff_wrappers_fill_dma_silence() -> None:
    for symbol in ["covox_sndoff", "stereo_sndoff", "adlib_sndoff", "pcspeaker_sndoff"]:
        out, data = original_run(
            wrapper_dma_silence_target(symbol),
            dump_count=16,
            dump_offset=0,
            dump_seg=DATA_SEG + 0x0300,
            strict=False,
        )
        got = translated("sndofffill", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "di") == field(out, "di")
            assert field(got, "data") == data.hex()
        assert data == b"\x80" * 16


def test_original_and_abi_sndoff_public_symbol_wrappers_fill_dma_silence() -> None:
    for symbol in ["covox_sndoff", "stereo_sndoff", "adlib_sndoff", "pcspeaker_sndoff"]:
        out, data = original_run(
            wrapper_dma_silence_target(symbol),
            dump_count=16,
            dump_offset=0,
            dump_seg=DATA_SEG + 0x0300,
            strict=False,
        )
        got = translated("abisndofffill", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "di") == field(out, "di")
            assert field(got, "data") == data.hex()


def wrapper_audio_init_failure(symbol: str) -> bytes:
    text_offsets = {
        "covox_init": (0x4FC0, 16),
        "stereo_init": (0x5048, 16),
        "pcspeaker_init": (0x519B, 16),
        "adlib_init": (0x5120, 16),
    }
    text_offset, text_count = text_offsets[symbol]
    setup = (
        setup_data_common()
        + mov_ds_word(0x0132, 0x0378)
        + mov_ds_word(0x0018, 0)
        + mov_ds_word(0x001A, 0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = (
        mov_ax(DATA_SEG)
        + b"\x8e\xc0"
        + mov_ax(LOAD_SEG)
        + b"\x8e\xd8"
        + copy_bytes_to_scratch(text_offset, DSEG_SCRATCH + 0x3A0, text_count)
        + mov_ax(DATA_SEG)
        + b"\x8e\xd8"
        + copy_bytes_to_scratch(0x0082, DSEG_SCRATCH + 0x3B0, 3)
        + copy_bytes_to_scratch(0x0132, DSEG_SCRATCH + 0x3B3, 2)
        + copy_bytes_to_scratch(0x0018, DSEG_SCRATCH + 0x3B5, 4)
    )
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def audio_init_failure_ok(symbol: str, data: bytes) -> bool:
    text = data[:16]
    common = data[16:25]
    sndflags, is_stereo, bit_mode = common[0], common[1], common[2]
    dma_ptr = struct.unpack("<I", common[5:9])[0]
    if bit_mode != 8 or dma_ptr != 0:
        return False
    if symbol == "covox_init":
        return (
            sndflags == 3
            and is_stereo == 0
            and struct.unpack("<H", text[0:2])[0] == 0xF108
            and struct.unpack("<H", text[5:7])[0] == 0xF000
        )
    if symbol == "stereo_init":
        return (
            sndflags == 3
            and is_stereo == 1
            and struct.unpack("<H", text[0:2])[0] == 0xF108
            and struct.unpack("<H", text[14:16])[0] == 0xF000
        )
    if symbol == "adlib_init":
        return (
            sndflags == 0x0B
            and is_stereo == 0
            and struct.unpack("<H", text[1:3])[0] == 0xF108
            and struct.unpack("<H", text[6:8])[0] == 0xF000
        )
    return (
        sndflags == 3
        and is_stereo == 0
        and struct.unpack("<H", text[0:2])[0] == 0xF108
        and struct.unpack("<H", text[8:10])[0] == 0xF000
    )


def test_original_and_translated_audio_init_allocation_failure_contracts() -> None:
    for symbol in ["covox_init", "stereo_init", "pcspeaker_init", "adlib_init"]:
        out, data = original_run(
            wrapper_audio_init_failure(symbol),
            dump_count=25,
            dump_offset=DSEG_SCRATCH + 0x3A0,
            dump_seg=DATA_SEG,
            strict=False,
        )
        expected = bytes([audio_init_failure_ok(symbol, data)])
        got = translated("audioinitfail", symbol)
        if got is not None:
            assert field(got, "data") == expected.hex()
        assert expected == b"\x01"


def test_original_and_abi_audio_init_public_symbol_allocation_failure_contracts() -> None:
    for symbol in ["covox_init", "stereo_init", "pcspeaker_init", "adlib_init"]:
        out, data = original_run(
            wrapper_audio_init_failure(symbol),
            dump_count=25,
            dump_offset=DSEG_SCRATCH + 0x3A0,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abiaudioinitfail", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "data") == data.hex()


def wrapper_sub_12da8_sndinit_guard() -> bytes:
    dump = DSEG_SCRATCH + 0x3C0
    setup = (
        setup_data_common()
        + mov_ds_byte(0x00E0, 1)
        + mov_ax(0x1603)
        + mov_bx(0x7856)
        + mov_cx(0x0907)
        + mov_dx(0x0220)
        + mov_si(0x0084)
        + mov_di(0x1234)
    )
    post = (
        copy_bytes_to_scratch(0x0132, dump, 11)
        + copy_bytes_to_scratch(0x00BE, dump + 11, 2)
        + copy_bytes_to_scratch(0x00E0, dump + 13, 1)
    )
    return setup + b"\x9a" + struct.pack("<HH", original_offset("sub_12DA8"), LOAD_SEG) + post + b"\xc3"


def test_original_and_translated_sub_12da8_sndinit_guard_failure_contract() -> None:
    _, data = original_run(
        wrapper_sub_12da8_sndinit_guard(),
        dump_count=14,
        dump_offset=DSEG_SCRATCH + 0x3C0,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("sub12da8guard")
    if got is not None:
        assert field(got, "data") == data.hex()
    assert data == bytes([0x03, 0x20, 0x02, 0x07, 0x09, 0x16, 0x56, 0x78, 0x84, 0x00, 0x4B, 0xF0, 0x55, 0x01])


def test_original_and_abi_sub_12da8_public_symbol_sndinit_guard_failure_contract() -> None:
    _, data = original_run(
        wrapper_sub_12da8_sndinit_guard(),
        dump_count=14,
        dump_offset=DSEG_SCRATCH + 0x3C0,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abisub12da8guard")
    if got is not None:
        assert field(got, "data") == data.hex()


def wrapper_timer_port(symbol: str, ax_value: int) -> bytes:
    setup = setup_data_common() + mov_ax(ax_value) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x4F6E, DSEG_SCRATCH + 0x280, 2)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def wrapper_timer_port_regs(symbol: str, ax_value: int) -> bytes:
    setup = setup_data_common() + mov_ax(ax_value) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + b"\xc3"


def test_original_and_translated_timer_port_helpers_no_device_ports() -> None:
    cases = [("set_timer", 0x1234), ("set_timer", 0x00FF), ("clean_timer", 0xBEEF)]
    for symbol, ax_value in cases:
        out, data = original_run(
            wrapper_timer_port(symbol, ax_value),
            dump_count=2,
            dump_offset=DSEG_SCRATCH + 0x280,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("timerport", symbol, hex(ax_value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def test_original_and_abi_timer_public_symbols_no_device_ports() -> None:
    cases = [("set_timer", 0x1234), ("set_timer", 0x00FF), ("clean_timer", 0xBEEF)]
    for symbol, ax_value in cases:
        out, _ = original_run(wrapper_timer_port_regs(symbol, ax_value), strict=False)
        got = translated("abitimerport", symbol, hex(ax_value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def wrapper_set_timer_int_success(target: int) -> bytes:
    setup = setup_data_common() + mov_dx(target)
    call_timer_ip = WRAPPER_IP + len(setup)
    call_timer = call_rel16(original_offset("set_timer_int"), call_timer_ip + 3)
    set_al = b"\xb0\x08"
    call_get_ip = call_timer_ip + len(call_timer) + len(set_al)
    post = copy_bytes_to_scratch(0x0018, DSEG_SCRATCH + 0x290, 4)
    return setup + call_timer + set_al + call_rel16(original_offset("getint_vect"), call_get_ip + 3) + post + b"\xc3"


def test_original_and_translated_set_timer_int_allocation_failure_leaves_int8_unchanged() -> None:
    target = original_offset("timer_int_end")
    out, data = original_run(
        wrapper_set_timer_int_success(target),
        dump_count=4,
        dump_offset=DSEG_SCRATCH + 0x290,
        dump_seg=DATA_SEG,
        strict=False,
    )
    expected_flags = bytes(
        [
            data[0:2] == b"\x00\x00",
            data[2:4] == b"\x00\x00",
            field(out, "bx") != f"{target:04x}",
            field(out, "dx") != f"{LOAD_SEG:04x}",
        ]
    )
    got = translated("settimerint")
    if got is not None:
        assert field(got, "data") == expected_flags.hex()
    assert expected_flags == b"\x01\x01\x01\x01"


def test_original_and_abi_set_timer_int_public_symbol_allocation_failure_leaves_int8_unchanged() -> None:
    target = original_offset("timer_int_end")
    out, data = original_run(
        wrapper_set_timer_int_success(target),
        dump_count=4,
        dump_offset=DSEG_SCRATCH + 0x290,
        dump_seg=DATA_SEG,
        strict=False,
    )
    expected_flags = bytes(
        [
            data[0:2] == b"\x00\x00",
            data[2:4] == b"\x00\x00",
            field(out, "bx") != f"{target:04x}",
            field(out, "dx") != f"{LOAD_SEG:04x}",
        ]
    )
    got = translated("abisettimerint", hex(target))
    if got is not None:
        assert field(got, "data") == expected_flags.hex()


def setup_memfree_invalid(segment: int) -> bytes:
    return setup_data_common() + mov_ax(segment) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)


def test_original_and_translated_memfree_invalid_segment_error() -> None:
    for segment in [0x0000, 0x1234, 0xFFFF]:
        out, _ = original_call(original_offset("memfree"), setup_memfree_invalid(segment), strict=False)
        got = translated("memfree", hex(segment))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
        assert field(out, "ax") == "0007"


def test_original_and_abi_memfree_public_symbol_invalid_segment_error() -> None:
    for segment in [0x0000, 0x1234, 0xFFFF]:
        out, _ = original_call(original_offset("memfree"), setup_memfree_invalid(segment), strict=False)
        got = translated("abimemfree", hex(segment))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
        assert field(out, "ax") == "0007"


def wrapper_midi_port(symbol: str, base_port: int, ax_value: int) -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_word(0x00BC, base_port)
        + mov_ds_byte(0x00D7, 0x55)
        + mov_ds_byte(0x00D8, 0xA0)
        + mov_ax(ax_value)
        + mov_bx(0x5678)
        + mov_cx(0x0003)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x00D7, DSEG_SCRATCH + 0x2A0, 2)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_midi_port_helpers_no_device_ports() -> None:
    cases = [
        ("midi_clean", 0x0330, 0x1234),
        ("midi_sndoff", 0x0330, 0x1234),
        ("midi_153C0", 0x0330, 0x1234),
        ("midi_153D6", 0x0330, 0x1234),
        ("midi_153F1", 0x0330, 0xFF34),
        ("midi_15442", 0x0330, 0x1234),
    ]
    for symbol, base_port, ax_value in cases:
        out, data = original_run(
            wrapper_midi_port(symbol, base_port, ax_value),
            dump_count=2,
            dump_offset=DSEG_SCRATCH + 0x2A0,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("midiport", symbol, hex(base_port), hex(ax_value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def test_original_and_abi_midi_port_public_symbols_no_device_ports() -> None:
    cases = [
        ("midi_clean", 0x0330, 0x1234),
        ("midi_sndoff", 0x0330, 0x1234),
        ("midi_153C0", 0x0330, 0x1234),
        ("midi_153D6", 0x0330, 0x1234),
        ("midi_153F1", 0x0330, 0xFF34),
        ("midi_15442", 0x0330, 0x1234),
    ]
    for symbol, base_port, ax_value in cases:
        out, data = original_run(
            wrapper_midi_port(symbol, base_port, ax_value),
            dump_count=2,
            dump_offset=DSEG_SCRATCH + 0x2A0,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abimidiport", symbol, hex(base_port), hex(ax_value))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def wrapper_midi_set() -> bytes:
    setup = setup_data_common() + mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("midi_set"), call_ip + 3) + b"\xc3"


def test_original_and_translated_midi_set_interrupt_vector_no_device() -> None:
    out, _ = original_run(wrapper_midi_set(), strict=False)
    got = translated("midiset")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_midi_set_public_symbol_interrupt_vector_no_device() -> None:
    out, _ = original_run(wrapper_midi_set(), strict=False)
    got = translated("abimidiset")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")


def wrapper_midi_channel_port(symbol: str) -> bytes:
    setup = (
        setup_channel_base()
        + mov_ds_byte(CHANNEL_OFF + 0x02, 0x05)
        + mov_ds_byte(CHANNEL_OFF + 0x03, 0x02)
        + mov_ds_byte(CHANNEL_OFF + 0x08, 0x20)
        + mov_ds_byte(CHANNEL_OFF + 0x17, 0x83 if symbol == "midi_1544D" else 0x00)
        + mov_ds_byte(CHANNEL_OFF + 0x18, 0x04)
        + mov_ds_byte(CHANNEL_OFF + 0x1B, 0x20)
        + mov_ds_byte(CHANNEL_OFF + 0x35, 0x31)
        + mov_ds_word(0x00BC, 0x0330)
        + mov_ds_byte(0x00D7, 0x55)
        + mov_ds_byte(0x00D8, 0xA0)
        + mov_ax(0x1234)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(CHANNEL_OFF, DSEG_SCRATCH + 0x2C0, 0x40)
    post += copy_bytes_to_scratch(0x00D7, DSEG_SCRATCH + 0x300, 2)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_midi_channel_port_helpers_no_device_ports() -> None:
    for symbol in ["midi_1544D", "midi_15466"]:
        out, data = original_run(
            wrapper_midi_channel_port(symbol),
            dump_count=0x42,
            dump_offset=DSEG_SCRATCH + 0x2C0,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("midichannelport", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def test_original_and_abi_midi_channel_port_public_symbols_no_device_ports() -> None:
    for symbol in ["midi_1544D", "midi_15466"]:
        out, data = original_run(
            wrapper_midi_channel_port(symbol),
            dump_count=0x42,
            dump_offset=DSEG_SCRATCH + 0x2C0,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abimidichannelport", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def wrapper_set_get_int_vector(int_number: int, vector_off: int, vector_seg: int) -> bytes:
    setup = setup_data_common() + mov_ax(int_number) + mov_bx(vector_off) + mov_dx(vector_seg)
    call_set_ip = WRAPPER_IP + len(setup)
    set_call = call_rel16(original_offset("setint_vect"), call_set_ip + 3)
    after_set_ip = call_set_ip + len(set_call)
    set_al = b"\xb0" + bytes([int_number & 0xFF])
    call_get_ip = after_set_ip + len(set_al)
    return setup + set_call + set_al + call_rel16(original_offset("getint_vect"), call_get_ip + 3) + b"\xc3"


def test_original_and_translated_interrupt_vector_set_get_roundtrip() -> None:
    for int_number, vector_off, vector_seg in [(0x66, 0x3456, 0x4567), (0x67, 0x1111, 0x2222)]:
        out, _ = original_run(
            wrapper_set_get_int_vector(int_number, vector_off, vector_seg),
            strict=False,
        )
        got = translated("intvect", hex(int_number), hex(vector_off), hex(vector_seg))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "dx") == field(out, "dx")
        assert field(out, "bx") == f"{vector_off:04x}"
        assert field(out, "dx") == f"{vector_seg:04x}"


def test_original_and_abi_interrupt_vector_public_symbols_roundtrip() -> None:
    for int_number, vector_off, vector_seg in [(0x66, 0x3456, 0x4567), (0x67, 0x1111, 0x2222)]:
        out, _ = original_run(
            wrapper_set_get_int_vector(int_number, vector_off, vector_seg),
            strict=False,
        )
        got = translated("abiintvect", hex(int_number), hex(vector_off), hex(vector_seg))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "dx") == field(out, "dx")


def wrapper_sound_vector_roundtrip(irq: int, old_off: int, old_seg: int) -> bytes:
    vector_offset = ((irq + 8) if irq < 8 else (irq + 0x68)) * 4
    setup = (
        setup_data_common()
        + mov_ax(0)
        + b"\x8e\xc0"
        + mov_es_word(vector_offset, old_off)
        + mov_es_word(vector_offset + 2, old_seg)
        + mov_ax(DATA_SEG)
        + b"\x8e\xc0"
        + mov_ax(irq)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
        + mov_si(0x4321)
    )
    call_set_ip = WRAPPER_IP + len(setup)
    set_call = call_rel16(original_offset("setsnd_handler"), call_set_ip + 3)
    call_restore_ip = call_set_ip + len(set_call)
    post = copy_bytes_to_scratch(0x0064, DSEG_SCRATCH + 0x320, 8)
    return setup + set_call + call_rel16(original_offset("restore_intvector"), call_restore_ip + 3) + post + b"\xc3"


def test_original_and_translated_sound_vector_handler_bookkeeping_roundtrip() -> None:
    for irq, old_off, old_seg in [(5, 0x1111, 0x2222), (10, 0x3333, 0x4444)]:
        out, data = original_run(
            wrapper_sound_vector_roundtrip(irq, old_off, old_seg),
            dump_count=8,
            dump_offset=DSEG_SCRATCH + 0x320,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("sndvector", hex(irq), hex(old_off), hex(old_seg))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "data") == data.hex()
        assert data[2:4] == struct.pack("<H", old_off)
        assert data[4:6] == struct.pack("<H", old_seg)


def test_original_and_abi_sound_vector_public_symbols_bookkeeping_roundtrip() -> None:
    for irq, old_off, old_seg in [(5, 0x1111, 0x2222), (10, 0x3333, 0x4444)]:
        out, data = original_run(
            wrapper_sound_vector_roundtrip(irq, old_off, old_seg),
            dump_count=8,
            dump_offset=DSEG_SCRATCH + 0x320,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abisndvector", hex(irq), hex(old_off), hex(old_seg))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "data") == data.hex()


def wrapper_sb16_probe_target(symbol: str) -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_word(0x00B2, 0x1111)
        + mov_ds_word(0x00B4, 0x2222)
        + mov_ds_byte(0x00B8, 0x33)
        + mov_ds_byte(0x00B9, 0x44)
        + mov_ds_word(0x0132, 0xFFFF)
        + mov_ds_byte(0x0134, 0x55)
        + mov_ds_byte(0x0135, 0x66)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x00B2, DSEG_SCRATCH + 0x380, 6)
    post += copy_bytes_to_scratch(0x0132, DSEG_SCRATCH + 0x386, 4)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def wrapper_sb16_init_failure() -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_byte(0x0082, 0xAA)
        + mov_ds_byte(0x0083, 0xBB)
        + mov_ds_byte(0x0084, 0xCC)
        + mov_ds_word(0x00B2, 0x1111)
        + mov_ds_word(0x00B4, 0x2222)
        + mov_ds_byte(0x00B8, 0x33)
        + mov_ds_byte(0x00B9, 0x44)
        + mov_ds_word(0x0133, 0xFFFF)
        + mov_ds_byte(0x0135, 0xFF)
        + mov_ds_byte(0x0136, 0xFF)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x0082, DSEG_SCRATCH + 0x390, 3)
    post += copy_bytes_to_scratch(0x00B2, DSEG_SCRATCH + 0x393, 8)
    post += copy_bytes_to_scratch(0x0133, DSEG_SCRATCH + 0x39B, 4)
    return setup + call_rel16(original_offset("sb16_init"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_sb16_probe_helpers_no_device_paths() -> None:
    for symbol in ["sb16_detect_port", "sb16_sound_on"]:
        out, data = original_run(
            wrapper_sb16_probe_target(symbol),
            dump_count=10,
            dump_offset=DSEG_SCRATCH + 0x380,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("sb16probe", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def test_original_and_abi_sb16_probe_public_symbols_no_device_paths() -> None:
    for symbol in ["sb16_detect_port", "sb16_sound_on"]:
        out, data = original_run(
            wrapper_sb16_probe_target(symbol),
            dump_count=10,
            dump_offset=DSEG_SCRATCH + 0x380,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abisb16probe", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def test_original_and_translated_sb16_init_no_device_failure() -> None:
    out, data = original_run(
        wrapper_sb16_init_failure(),
        dump_count=15,
        dump_offset=DSEG_SCRATCH + 0x390,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("sb16initfail")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()
    assert data[:3] == b"\x09\x01\x10"


def test_original_and_abi_sb16_init_public_symbol_no_device_failure() -> None:
    out, data = original_run(
        wrapper_sb16_init_failure(),
        dump_count=15,
        dump_offset=DSEG_SCRATCH + 0x390,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abisb16initfail")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def setup_sb16_handler_int() -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x00B2, 0x0220)
        + mov_ds_byte(0x00D0, 5)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def setup_sb16_18540_no_interrupt() -> bytes:
    return (
        setup_data_common()
        + mov_ds_dword(0x0018, 0x12345678)
        + mov_ds_word(0x006E, 0xAAAA)
        + mov_ds_word(0x00B2, 0x0220)
        + mov_ds_byte(0x00B8, 1)
        + mov_ds_byte(0x00B9, 5)
        + mov_ds_byte(0x00CF, 0xAA)
        + mov_ds_byte(0x00D0, 0xCC)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def setup_inr_read_119b7() -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x00C0, 0)
        + mov_ds_dword(0xBF6C, 16)
        + b"".join(mov_ds_byte(0xBF68 + i, 0xA5) for i in range(16))
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
        + mov_di(0xBF68)
    )


def setup_mod_readfile_11f4e_guard() -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x0077, 0xAA)
        + mov_ds_byte(0x0082, 0)
        + mov_ds_word(0x00C2, 1)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def setup_mod_readfile_12247_eof() -> bytes:
    return (
        setup_data_common()
        + mov_ax(DSEG)
        + b"\x8e\xc0"
        + b"".join(mov_es_byte(i, 0xA5) for i in range(16))
        + mov_ax(DATA_SEG)
        + b"\x8e\xd8"
        + mov_ax(0x1234)
        + mov_bx(0)
        + mov_cx(16)
        + mov_dx(0xFFFF)
        + mov_esi(0x2222)
        + mov_di(0x3333)
    )


def wrapper_int1a_timer_passthrough() -> bytes:
    seg001 = LOAD_SEG + SEG001_DELTA
    setup = (
        mov_ax(seg001)
        + b"\x8e\xd8"
        + mov_ds_word(0x3158, 0)
        + mov_ds_word(0x315A, seg001)
        + mov_ax(0x0100)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
        + mov_ax(DSEG)
        + b"\x8e\xc0"
        + mov_ax(0x0100)
    )
    iret_ip = WRAPPER_IP + len(setup) + 13
    setup = (
        mov_ax(seg001)
        + b"\x8e\xd8"
        + mov_ds_word(0x3158, iret_ip)
        + mov_ds_word(0x315A, seg001)
        + mov_ax(0x0100)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
        + mov_ax(DSEG)
        + b"\x8e\xc0"
        + mov_ax(0x0100)
    )
    continuation_ip = WRAPPER_IP + len(setup) + 12
    return (
        setup
        + b"\x9c"
        + mov_bx(seg001)
        + b"\x53"
        + mov_bx(continuation_ip)
        + b"\x53"
        + jmp_rel16(original_offset("int1a_timer"), continuation_ip)
        + b"\xc3"
        + b"\xcf"
    )


def setup_stereo_timer_int() -> bytes:
    return (
        mov_ax(LOAD_SEG)
        + b"\x8e\xd8"
        + mov_ds_word(0x4F6C, 3)
        + mov_ds_word(0x5056, 0x1234)
        + mov_ax(DATA_SEG)
        + b"\x8e\xd8"
        + b"\x8e\xc0"
        + mov_ax(0x5678)
        + mov_bx(0x9ABC)
        + mov_cx(0xDEF0)
        + mov_dx(0x037A)
    )


def setup_timer_int_end_disabled() -> bytes:
    return (
        mov_ax(LOAD_SEG)
        + b"\x8e\xd8"
        + mov_ds_word(0x4F68, 0x4F4F)
        + mov_ds_word(0x4F6A, LOAD_SEG)
        + mov_ds_word(0x4F6C, 0x7777)
        + mov_ds_byte(0x4F70, 0)
        + mov_ax(DATA_SEG)
        + b"\x8e\xd8"
        + b"\x8e\xc0"
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )


def test_original_and_translated_sb16_interrupt_handler_acknowledges_and_counts() -> None:
    out, data = original_seg000_iret_call(
        original_offset("sb16_handler_int"),
        setup_sb16_handler_int(),
        dump_count=1,
        dump_offset=0x00D0,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("sb16int")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "ds") == field(out, "ds")
        assert field(got, "data") == data.hex()
    assert data == b"\x06"


def test_original_and_abi_sb16_interrupt_handler_public_symbol_acknowledges_and_counts() -> None:
    out, data = original_seg000_iret_call(
        original_offset("sb16_handler_int"),
        setup_sb16_handler_int(),
        dump_count=1,
        dump_offset=0x00D0,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abisb16int")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "ds") == field(out, "ds")
        assert field(got, "data") == data.hex()


def test_original_and_translated_sb16_18540_times_out_without_interrupt() -> None:
    out, memory = original_run(
        make_wrapper(original_offset("sb16_18540"), setup_sb16_18540_no_interrupt()),
        dump_count=0x00D1 - 0x0018,
        dump_offset=0x0018,
        dump_seg=DATA_SEG,
        strict=False,
    )
    data = bytes(
        [
            memory[0x0018 - 0x0018],
            memory[0x0019 - 0x0018],
            memory[0x001A - 0x0018],
            memory[0x001B - 0x0018],
            memory[0x006E - 0x0018],
            memory[0x006F - 0x0018],
            memory[0x00B8 - 0x0018],
            memory[0x00B9 - 0x0018],
            memory[0x00CF - 0x0018],
            memory[0x00D0 - 0x0018],
        ]
    )
    got = translated("sb16dmafail")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()
    assert data[:6] == b"\x00\x00\x00\x00\x02\x00"
    assert data[6:] == b"\x01\x05\x48\x01"


def test_original_and_abi_sb16_18540_public_symbol_times_out_without_interrupt() -> None:
    out, memory = original_run(
        make_wrapper(original_offset("sb16_18540"), setup_sb16_18540_no_interrupt()),
        dump_count=0x00D1 - 0x0018,
        dump_offset=0x0018,
        dump_seg=DATA_SEG,
        strict=False,
    )
    data = bytes(
        [
            memory[0x0018 - 0x0018],
            memory[0x0019 - 0x0018],
            memory[0x001A - 0x0018],
            memory[0x001B - 0x0018],
            memory[0x006E - 0x0018],
            memory[0x006F - 0x0018],
            memory[0x00B8 - 0x0018],
            memory[0x00B9 - 0x0018],
            memory[0x00CF - 0x0018],
            memory[0x00D0 - 0x0018],
        ]
    )
    got = translated("abisb16dmafail")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def test_original_and_translated_int1a_timer_passthrough_for_nonzero_ah() -> None:
    out, _ = original_run(
        wrapper_int1a_timer_passthrough(),
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("int1apass")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "ds") == field(out, "ds")
        assert field(got, "es") == field(out, "es")


def test_original_and_abi_int1a_timer_public_symbol_passthrough_for_nonzero_ah() -> None:
    out, _ = original_run(
        wrapper_int1a_timer_passthrough(),
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("abitimerint", "int1a_timer")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "ds") == field(out, "ds")
        assert field(got, "es") == field(out, "es")


def test_original_and_translated_inr_read_119b7_eof_read_helper() -> None:
    out, data = original_run(
        make_wrapper(original_offset("inr_read_119B7"), setup_inr_read_119b7()),
        dump_count=16,
        dump_offset=0xBF68,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("inrread119b7")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()
    assert data == b"\xA5" * 16


def test_original_and_abi_inr_read_119b7_public_symbol_eof_read_helper() -> None:
    out, data = original_run(
        make_wrapper(original_offset("inr_read_119B7"), setup_inr_read_119b7()),
        dump_count=16,
        dump_offset=0xBF68,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abiinrread119b7")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()


def test_original_and_translated_mod_readfile_11f4e_stops_on_prior_error() -> None:
    out, data = original_run(
        make_wrapper(original_offset("mod_readfile_11F4E"), setup_mod_readfile_11f4e_guard()),
        dump_count=0x00C4 - 0x0077,
        dump_offset=0x0077,
        dump_seg=DATA_SEG,
        strict=False,
    )
    selected = bytes([data[0], data[0x00C2 - 0x0077], data[0x00C3 - 0x0077], data[0x0082 - 0x0077]])
    got = translated("modread11f4eguard")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == selected.hex()
    assert selected == b"\x00\x01\x00\x00"


def test_original_and_abi_mod_readfile_11f4e_public_symbol_stops_on_prior_error() -> None:
    out, data = original_run(
        make_wrapper(original_offset("mod_readfile_11F4E"), setup_mod_readfile_11f4e_guard()),
        dump_count=0x00C4 - 0x0077,
        dump_offset=0x0077,
        dump_seg=DATA_SEG,
        strict=False,
    )
    selected = bytes([data[0], data[0x00C2 - 0x0077], data[0x00C3 - 0x0077], data[0x0082 - 0x0077]])
    got = translated("abimodread11f4eguard")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == selected.hex()


def test_original_and_translated_mod_readfile_12247_returns_zero_on_eof() -> None:
    out, data = original_run(
        make_wrapper(original_offset("mod_readfile_12247"), setup_mod_readfile_12247_eof()),
        dump_count=16,
        dump_offset=0,
        dump_seg=DSEG,
        strict=False,
    )
    got = translated("modread12247eof")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()
    assert data == b"\xA5" * 16


def test_original_and_abi_mod_readfile_12247_public_symbol_returns_zero_on_eof() -> None:
    out, data = original_run(
        make_wrapper(original_offset("mod_readfile_12247"), setup_mod_readfile_12247_eof()),
        dump_count=16,
        dump_offset=0,
        dump_seg=DSEG,
        strict=False,
    )
    got = translated("abimodread12247eof")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "si") == field(out, "si")
        assert field(got, "di") == field(out, "di")
        assert field(got, "data") == data.hex()


def test_original_and_translated_stereo_timer_interrupt_advances_dma_pointer() -> None:
    out, data = original_seg000_iret_call(
        original_offset("stereo_timer_int"),
        setup_stereo_timer_int(),
        dump_count=(0x5058 - 0x4F6C),
        dump_offset=0x4F6C,
        dump_seg=LOAD_SEG,
        strict=False,
    )
    snapshot = data[:2] + data[0x5056 - 0x4F6C : 0x5058 - 0x4F6C]
    got = translated("stereoint")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "ds") == field(out, "ds")
        assert field(got, "data") == snapshot.hex()
    assert snapshot == struct.pack("<HH", 2, 0x1236)


def test_original_and_abi_stereo_timer_int_public_symbol_advances_dma_pointer() -> None:
    out, data = original_seg000_iret_call(
        original_offset("stereo_timer_int"),
        setup_stereo_timer_int(),
        dump_count=(0x5058 - 0x4F6C),
        dump_offset=0x4F6C,
        dump_seg=LOAD_SEG,
        strict=False,
    )
    snapshot = data[:2] + data[0x5056 - 0x4F6C : 0x5058 - 0x4F6C]
    got = translated("abitimerint", "stereo_timer_int")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "ds") == field(out, "ds")
        assert field(got, "data") == snapshot.hex()


def test_original_and_translated_timer_int_end_disabled_chains_old_int8() -> None:
    out, data = original_seg000_iret_call(
        original_offset("timer_int_end"),
        setup_timer_int_end_disabled(),
        dump_count=5,
        dump_offset=0x4F6C,
        dump_seg=LOAD_SEG,
        strict=False,
    )
    snapshot = data[:2] + data[4:5]
    got = translated("timerend")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "ds") == field(out, "ds")
        assert field(got, "data") == snapshot.hex()
    assert snapshot == b"\x01\x00\x00"


def test_original_and_abi_timer_int_end_public_symbol_disabled_chains_old_int8() -> None:
    out, data = original_seg000_iret_call(
        original_offset("timer_int_end"),
        setup_timer_int_end_disabled(),
        dump_count=5,
        dump_offset=0x4F6C,
        dump_seg=LOAD_SEG,
        strict=False,
    )
    snapshot = data[:2] + data[4:5]
    got = translated("abitimerint", "timer_int_end")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "ds") == field(out, "ds")
        assert field(got, "data") == snapshot.hex()


def wrapper_sb16_off_target(symbol: str) -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_word(0x00B2, 0x0220)
        + mov_ds_byte(0x00CE, 0)
        + mov_ds_byte(0x00FA, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x00CE, DSEG_SCRATCH + 0x340, 1)
    post += copy_bytes_to_scratch(0x00FA, DSEG_SCRATCH + 0x341, 1)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_sb16_off_helpers_no_device_paths() -> None:
    for symbol in ["sb16_sound_off", "sb16_off", "sb16_deinit", "sb_clean", "sb_sndoff", "sbpro_clean", "sbpro_sndoff"]:
        out, data = original_run(
            wrapper_sb16_off_target(symbol),
            dump_count=2,
            dump_offset=DSEG_SCRATCH + 0x340,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("sb16off", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def test_original_and_abi_sb16_off_public_symbols_no_device_paths() -> None:
    for symbol in ["sb16_sound_off", "sb16_off", "sb16_deinit", "sb_clean", "sb_sndoff", "sbpro_clean", "sbpro_sndoff"]:
        out, data = original_run(
            wrapper_sb16_off_target(symbol),
            dump_count=2,
            dump_offset=DSEG_SCRATCH + 0x340,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abisb16off", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def wrapper_clean_deinit_target(symbol: str) -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_word(0x0018, 0)
        + mov_ds_word(0x001A, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x0018, DSEG_SCRATCH + 0x350, 4)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_cleanup_deinit_no_device_paths() -> None:
    for symbol in ["clean_int8_mem_timr", "covox_deinit", "stereo_deinit", "adlib_clean", "pcspeaker_clean"]:
        out, data = original_run(
            wrapper_clean_deinit_target(symbol),
            dump_count=4,
            dump_offset=DSEG_SCRATCH + 0x350,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("cleandeinit", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def test_original_and_abi_cleanup_deinit_public_symbols_no_device_paths() -> None:
    for symbol in ["clean_int8_mem_timr", "covox_deinit", "stereo_deinit", "adlib_clean", "pcspeaker_clean"]:
        out, data = original_run(
            wrapper_clean_deinit_target(symbol),
            dump_count=4,
            dump_offset=DSEG_SCRATCH + 0x350,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abicleandeinit", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "data") == data.hex()


def wrapper_dosdir(symbol: str) -> bytes:
    setup = setup_data_common() + mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)
    if symbol == "doschdir":
        setup += mov_ds_byte(DSEG_SCRATCH, 4) + mov_ds_byte(DSEG_SCRATCH + 1, ord("\\")) + mov_ds_byte(DSEG_SCRATCH + 2, 0)
    setup += mov_si(DSEG_SCRATCH)
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(DSEG_SCRATCH, DSEG_SCRATCH + 0x360, 70)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_dos_directory_wrappers_no_device_stub() -> None:
    for symbol in ["dosgetcurdir", "doschdir"]:
        out, data = original_run(
            wrapper_dosdir(symbol),
            dump_count=70,
            dump_offset=DSEG_SCRATCH + 0x360,
            dump_seg=DATA_SEG,
            call_cs=LOAD_SEG + SEG001_DELTA,
            wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
            strict=False,
        )
        got = translated("dosdir", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "data") == data.hex()


def test_original_and_abi_dos_directory_public_symbols_no_device_stub() -> None:
    for symbol in ["dosgetcurdir", "doschdir"]:
        out, data = original_run(
            wrapper_dosdir(symbol),
            dump_count=70,
            dump_offset=DSEG_SCRATCH + 0x360,
            dump_seg=DATA_SEG,
            call_cs=LOAD_SEG + SEG001_DELTA,
            wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
            strict=False,
        )
        got = translated("abidosdir", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")
            assert field(got, "si") == field(out, "si")
            assert field(got, "data") == data.hex()


def wrapper_dosfindnext_failure() -> bytes:
    setup = setup_dseg_common() + mov_ds_byte(0x13FC, 0x5A) + mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + mov_dx(0xDEF0)
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x13FC, DSEG_SCRATCH + 0x3E0, 1)
    return setup + call_rel16(original_offset("dosfindnext"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_dosfindnext_no_prior_search_failure_path() -> None:
    out, data = original_run(
        wrapper_dosfindnext_failure(),
        dump_count=1,
        dump_offset=DSEG_SCRATCH + 0x3E0,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("dosfindnext")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def test_original_and_abi_dosfindnext_public_symbol_no_prior_search_failure_path() -> None:
    out, data = original_run(
        wrapper_dosfindnext_failure(),
        dump_count=1,
        dump_offset=DSEG_SCRATCH + 0x3E0,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("abidosfindnext")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def wrapper_dosfread_eof() -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_word(0x00C0, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(16)
        + mov_dx(0xBF68)
    )
    for index in range(16):
        setup += mov_ds_byte(0xBF68 + index, 0xA5)
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("dosfread"), call_ip + 3) + b"\xc3"


def test_original_and_translated_dosfread_stdin_eof_success_path() -> None:
    out, data = original_run(
        wrapper_dosfread_eof(),
        dump_count=16,
        dump_offset=0xBF68,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("dosfread")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def test_original_and_abi_dosfread_public_symbol_stdin_eof_success_path() -> None:
    out, data = original_run(
        wrapper_dosfread_eof(),
        dump_count=16,
        dump_offset=0xBF68,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abidosfread")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def wrapper_dosseek_zero_length_read_success() -> bytes:
    setup = setup_data_common()
    for index, value in enumerate(b"SEEK.DAT\x00"):
        setup += mov_ds_byte(DSEG_SCRATCH + index, value)
    for index in range(16):
        setup += mov_ds_byte(0xBF68 + index, 0xA5)
    setup += (
        mov_dx(DSEG_SCRATCH)
        + mov_ax(0x3D00)
        + b"\xcd\x21"
        + b"\xa3" + struct.pack("<H", 0x00C0)
        + mov_eax(0)
        + mov_cx(0)
        + mov_dx(0xBF68)
    )
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("dosseek"), call_ip + 3) + b"\xc3"


def test_original_and_translated_dosseek_zero_length_read_success_path() -> None:
    out, data = original_run(
        wrapper_dosseek_zero_length_read_success(),
        dump_count=16,
        dump_offset=0xBF68,
        dump_seg=DATA_SEG,
        strict=False,
        extra_files={"SEEK.DAT": b"seek payload"},
    )
    got = translated("dosseeksuccess")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def test_original_and_abi_dosseek_public_symbol_zero_length_read_success_path() -> None:
    out, data = original_run(
        wrapper_dosseek_zero_length_read_success(),
        dump_count=16,
        dump_offset=0xBF68,
        dump_seg=DATA_SEG,
        strict=False,
        extra_files={"SEEK.DAT": b"seek payload"},
    )
    got = translated("abidosseeksuccess")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def setup_inr_read_118b0_invalid_handle() -> bytes:
    return setup_data_common() + mov_ds_word(0x00C0, 0xFFFF) + mov_dx(2)


def test_original_and_translated_inr_read_118b0_invalid_handle_error_return() -> None:
    out, _ = original_call(original_offset("inr_read_118B0"), setup_inr_read_118b0_invalid_handle(), strict=False)
    got = translated("inrread118b0fail")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "ds") == field(out, "ds")


def test_original_and_abi_inr_read_118b0_public_symbol_invalid_handle_error_return() -> None:
    out, _ = original_call(original_offset("inr_read_118B0"), setup_inr_read_118b0_invalid_handle(), strict=False)
    got = translated("abiinrread118b0fail")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "ds") == field(out, "ds")


def wrapper_read2buffer() -> bytes:
    setup = (
        setup_data_common()
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
        + mov_si(0x1111)
        + mov_di(0x2222)
    )
    for index in range(16):
        setup += mov_ds_byte(0x2800 + index, 0xA5)
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x2800, DSEG_SCRATCH + 0x380, 16)
    return setup + call_rel16(original_offset("read2buffer"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_read2buffer_no_input_keeps_buffer() -> None:
    out, data = original_run(
        wrapper_read2buffer(),
        dump_count=16,
        dump_offset=DSEG_SCRATCH + 0x380,
        dump_seg=DATA_SEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("read2buffer")
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert field(got, "data") == data.hex()
    assert data == b"\xA5" * 16


def test_original_and_abi_read2buffer_public_symbol_no_input_keeps_buffer() -> None:
    out, data = original_run(
        wrapper_read2buffer(),
        dump_count=16,
        dump_offset=DSEG_SCRATCH + 0x380,
        dump_seg=DATA_SEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("abiread2buffer")
    if got is not None:
        assert field(got, "si") == field(out, "si")
        assert field(got, "data") == data.hex()


def mov_ebx(value: int) -> bytes:
    return b"\x66\xbb" + struct.pack("<I", value & 0xFFFFFFFF)


def setup_mem_size_limit(size: int) -> bytes:
    return setup_data_common() + mov_ebx(size) + mov_ax(0x2345)


def test_original_and_translated_memory_allocators_reject_oversized_requests_before_dos_int() -> None:
    cases = [
        ("memalloc", 0x100000),
        ("memalloc", 0x100010),
        ("memrealloc", 0x100000),
        ("memrealloc", 0x100010),
    ]
    for symbol, size in cases:
        out, _ = original_call(original_offset(symbol), setup_mem_size_limit(size))
        got = translated("memlimit", symbol, hex(size))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
        assert field(out, "ax") == "0008"


def test_original_and_abi_memory_allocators_public_symbols_reject_oversized_requests_before_dos_int() -> None:
    cases = [
        ("memalloc", 0x100000),
        ("memalloc", 0x100010),
        ("memrealloc", 0x100000),
        ("memrealloc", 0x100010),
    ]
    for symbol, size in cases:
        out, _ = original_call(original_offset(symbol), setup_mem_size_limit(size))
        got = translated("abimemlimit", symbol, hex(size))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")


def setup_alloc_dma_buf_oversized(size: int, channel: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x00EC, 0xBEEF)
        + mov_ds_dword(0x00F4, 0xA5A5A5A5)
        + mov_ds_word(0x00F8, 0xCAFE)
        + mov_ds_byte(0x00FA, 0xA5)
        + mov_ds_byte(0x00FB, 0xA5)
        + mov_ds_byte(0x00FC, 0xA5)
        + mov_eax(size)
        + mov_cx(channel)
    )


def wrapper_alloc_dma_buf_oversized(size: int, channel: int) -> bytes:
    setup = setup_alloc_dma_buf_oversized(size, channel)
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x00E4, DSEG_SCRATCH + 0x240, 0x19)
    return setup + call_rel16(original_offset("alloc_dma_buf"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_alloc_dma_buf_oversized_failure_before_dos_alloc() -> None:
    for size, channel in [(0x80000, 2), (0x80008, 5)]:
        out, data = original_run(
            wrapper_alloc_dma_buf_oversized(size, channel),
            dump_count=0x19,
            dump_offset=DSEG_SCRATCH + 0x240,
            dump_seg=DATA_SEG,
        )
        got = translated("allocdmafail", hex(size), hex(channel))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "data") == data.hex()


def test_original_and_abi_alloc_dma_buf_public_symbol_oversized_failure_before_dos_alloc() -> None:
    for size, channel in [(0x80000, 2), (0x80008, 5)]:
        out, data = original_run(
            wrapper_alloc_dma_buf_oversized(size, channel),
            dump_count=0x19,
            dump_offset=DSEG_SCRATCH + 0x240,
            dump_seg=DATA_SEG,
        )
        got = translated("abiallocdmafail", hex(size), hex(channel))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "data") == data.hex()


def setup_gravis_dma_control() -> bytes:
    return (
        setup_data_common()
        + mov_ds_byte(0x00CE, 0)
        + mov_ds_byte(0x008C, 1)
        + mov_ds_word(0x0086, 0x0220)
        + mov_ds_dword(0x0018, 0x12345000)
        + mov_ds_byte(0x00D3, 0x80)
        + mov_ax(0x0100)
        + mov_cx(0x0020)
    )


def wrapper_gravis_dma_control(symbol: str) -> bytes:
    setup = setup_gravis_dma_control()
    call_ip = WRAPPER_IP + len(setup)
    post = (
        mov_si(0x00CF)
        + mov_di(DSEG_SCRATCH)
        + mov_cx(1)
        + b"\xf3\xa4"
        + mov_si(0x00A5)
        + mov_di(DSEG_SCRATCH + 1)
        + mov_cx(1)
        + b"\xf3\xa4"
        + copy_bytes_to_scratch(0x006E, DSEG_SCRATCH + 2, 2)
        + copy_bytes_to_scratch(0x0096, DSEG_SCRATCH + 4, 2)
        + copy_bytes_to_scratch(0x0094, DSEG_SCRATCH + 6, 2)
        + copy_bytes_to_scratch(0x0092, DSEG_SCRATCH + 8, 2)
        + mov_si(0x00CE)
        + mov_di(DSEG_SCRATCH + 10)
        + mov_cx(1)
        + b"\xf3\xa4"
    )
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_gravis_dma_control_paths() -> None:
    for symbol in ["sub_182DB", "nongravis_dma"]:
        out, data = original_run(
            wrapper_gravis_dma_control(symbol),
            dump_count=11,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("gravisdma", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "data") == data.hex()


def test_original_and_abi_gravis_dma_control_public_symbols_paths() -> None:
    for symbol in ["sub_182DB", "nongravis_dma"]:
        out, data = original_run(
            wrapper_gravis_dma_control(symbol),
            dump_count=11,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
            strict=False,
        )
        got = translated("abigravisdma", symbol)
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "data") == data.hex()


def setup_sub_1279a_tail_dma() -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x0070, 1)
        + mov_ds_word(0x0072, 0x20)
        + mov_ds_byte(0x00CE, 0)
        + mov_ds_byte(0x008C, 1)
        + mov_ds_word(0x0086, 0x0220)
        + mov_ds_dword(DSEG_SCRATCH + 4, 0)
        + mov_ds_byte(DSEG_SCRATCH + 0x19, 0)
        + mov_ds_word(DSEG_SCRATCH + 0x20, 2)
        + mov_si(DSEG_SCRATCH)
        + mov_eax(0x12345000)
    )


def wrapper_sub_1279a_tail_dma() -> bytes:
    setup = setup_sub_1279a_tail_dma()
    call_ip = WRAPPER_IP + len(setup)
    post = (
        copy_bytes_to_scratch(0x0018, DSEG_SCRATCH + 0x20, 4)
        + mov_si(0x00CF)
        + mov_di(DSEG_SCRATCH + 0x24)
        + mov_cx(1)
        + b"\xf3\xa4"
        + mov_si(0x00A5)
        + mov_di(DSEG_SCRATCH + 0x25)
        + mov_cx(1)
        + b"\xf3\xa4"
        + copy_bytes_to_scratch(0x006E, DSEG_SCRATCH + 0x26, 2)
        + mov_si(0x00CE)
        + mov_di(DSEG_SCRATCH + 0x28)
        + mov_cx(1)
        + b"\xf3\xa4"
    )
    return setup + call_rel16(original_offset("sub_1279A"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_sub_1279a_mono_tail_dma_path() -> None:
    out, data = original_run(
        wrapper_sub_1279a_tail_dma(),
        dump_count=9,
        dump_offset=DSEG_SCRATCH + 0x20,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("sub1279dma")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "data") == data.hex()


def test_original_and_abi_sub_1279a_public_symbol_mono_tail_dma_path() -> None:
    out, data = original_run(
        wrapper_sub_1279a_tail_dma(),
        dump_count=9,
        dump_offset=DSEG_SCRATCH + 0x20,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abisub1279dma")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "data") == data.hex()


def setup_program_dma_channel1() -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x013A, 0x1000)
        + mov_ds_byte(0x00CF, 0x58)
        + mov_ds_dword(0x0018, 0x12345000)
        + mov_ds_dword(0x00F4, 0x10)
        + mov_ds_word(0x006E, 0x20)
        + mov_cx(1)
    )


def wrapper_program_dma_channel5_checkpoint(out_offset: int) -> bytes:
    out_offsets = [0x887A, 0x887E, 0x8885, 0x88A2, 0x88A6, 0x88AC, 0x88B7, 0x88BB, 0x88BF]
    setup = (
        setup_data_common()
        + mov_ds_word(0x013A, 0x1000)
        + mov_ds_byte(0x00CF, 0x58)
        + mov_ds_dword(0x0018, 0x12345000)
        + mov_ds_dword(0x00F4, 0x10)
        + mov_ds_word(0x006E, 0x1000)
        + mov_cx(5)
    )
    for prior_offset in out_offsets[:out_offsets.index(out_offset)]:
        setup += mov_cs_byte(prior_offset, 0x90)
        setup += mov_cs_byte(prior_offset + 1, 0x90)
    setup += mov_cs_byte(out_offset, 0xC3)
    call_ip = WRAPPER_IP + len(setup)
    post = b"\xa3" + struct.pack("<H", DSEG_SCRATCH)
    return setup + call_rel16(original_offset("program_dma"), call_ip + 3) + post + b"\xc3"


def test_original_and_rewrite_sb16_dma_channel5_port_value_sequence() -> None:
    out_offsets = [0x887A, 0x887E, 0x8885, 0x88A2, 0x88A6, 0x88AC, 0x88B7, 0x88BB, 0x88BF]
    ports = [0x00D4, 0x00D8, 0x00D6, 0x00C4, 0x00C4, 0x008B, 0x00C6, 0x00C6, 0x00D4]
    original_events = bytearray()
    for out_offset, port in zip(out_offsets, ports):
        _, data = original_run(
            wrapper_program_dma_channel5_checkpoint(out_offset),
            dump_count=2,
            dump_offset=DSEG_SCRATCH,
            dump_seg=DATA_SEG,
            strict=False,
        )
        original_events += struct.pack("<H", port)
        original_events.append(data[0])

    got = run(
        [
            str(ROOT / "rewrite" / ".build" / "sb16_commands_runner"),
            "dma16",
            "0x1234",
            "0x5000",
            "0x10",
            "0x1000",
            "0x58",
            "0x1000",
        ],
        cwd=ROOT,
    ).stdout.strip()
    assert field(got, "count") == "9"
    rewritten_events = bytes.fromhex(field(got, "data"))
    assert [
        (struct.unpack("<H", original_events[i:i + 2])[0], original_events[i + 2])
        for i in range(0, len(original_events), 3)
    ] == [
        (struct.unpack("<H", rewritten_events[i:i + 2])[0], rewritten_events[i + 2])
        for i in range(0, len(rewritten_events), 3)
    ]


def wrapper_program_dma_channel1() -> bytes:
    setup = setup_program_dma_channel1()
    call_ip = WRAPPER_IP + len(setup)
    post = mov_si(0x00CF) + mov_di(DSEG_SCRATCH + 0x2A) + mov_cx(1) + b"\xf3\xa4"
    return setup + call_rel16(original_offset("program_dma"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_program_dma_channel1_mode_mask() -> None:
    out, data = original_run(
        wrapper_program_dma_channel1(),
        dump_count=1,
        dump_offset=DSEG_SCRATCH + 0x2A,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("programdma")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def test_original_and_abi_program_dma_channel1_mode_mask() -> None:
    out, data = original_run(
        wrapper_program_dma_channel1(),
        dump_count=1,
        dump_offset=DSEG_SCRATCH + 0x2A,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abiprogramdma")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "cx") == field(out, "cx")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "data") == data.hex()


def setup_mem_strategy(config_word: int) -> bytes:
    return (
        setup_data_common()
        + mov_ds_word(0x013A, config_word)
        + mov_ax(0xAAAA)
        + mov_bx(0xBBBB)
        + mov_cx(0xCCCC)
        + mov_dx(0xDDDD)
    )


def test_original_and_translated_memory_strategy_helpers() -> None:
    cases = [
        ("setmemalloc1", 0x0000),
        ("setmemalloc1", 0x0001),
        ("setmemalloc2", 0x0000),
        ("setmemallocstrat", 0x0181),
        ("getmemallocstrat", 0x0000),
    ]
    for symbol, config_word in cases:
        out, _ = original_call(original_offset(symbol), setup_mem_strategy(config_word))
        got = translated("memstrat", symbol, hex(config_word))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def test_original_and_abi_memory_strategy_public_symbols() -> None:
    cases = [
        ("setmemalloc1", 0x0000),
        ("setmemalloc1", 0x0001),
        ("setmemalloc2", 0x0000),
        ("setmemallocstrat", 0x0181),
        ("getmemallocstrat", 0x0000),
    ]
    for symbol, config_word in cases:
        out, _ = original_call(original_offset(symbol), setup_mem_strategy(config_word))
        got = translated("abimemstrat", symbol, hex(config_word))
        if got is not None:
            assert field(got, "ax") == field(out, "ax")
            assert field(got, "bx") == field(out, "bx")
            assert field(got, "cx") == field(out, "cx")
            assert field(got, "dx") == field(out, "dx")


def copy_ds_to_load(src: int, dst: int, count: int) -> bytes:
    return (
        b"\xbe" + struct.pack("<H", src & 0xFFFF)
        + b"\xbf" + struct.pack("<H", dst & 0xFFFF)
        + mov_cx(count)
        + b"\xfc\xf3\xa4"
    )


def wrapper_mem_reallocx(size: int) -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_word(0x0054, 1)
        + mov_ds_word(0x00C2, 0x2222)
        + mov_ds_word(0x364A, 0xFFFF)
        + mov_ds_word(0x384A, 0xFFFF)
        + mov_di(size)
    )
    call_ip = WRAPPER_IP + len(setup)
    return (
        setup
        + call_rel16(original_offset("mem_reallocx"), call_ip + 3)
        + mov_ax(LOAD_SEG)
        + b"\x8e\xc0"
        + copy_ds_to_load(0x0054, DST_OFF, 2)
        + copy_ds_to_load(0x00C2, DST_OFF + 2, 2)
        + copy_ds_to_load(0x364A, DST_OFF + 4, 2)
        + copy_ds_to_load(0x384A, DST_OFF + 6, 2)
        + b"\xc3"
    )


def test_original_and_translated_mem_reallocx_segment_bookkeeping() -> None:
    for size in [0x0000, 0x0040]:
        out, data = original_run(wrapper_mem_reallocx(size), dump_count=8)
        got = translated("memreallocx", hex(size))
        if got is not None:
            assert field(got, "data") == data.hex()
            assert field(got, "di") == f"{size:04x}"
        assert data[0:2] == b"\x02\x00"
        assert data[4:6] == struct.pack("<H", DATA_SEG)
        assert data[6:8] == struct.pack("<H", size)


def test_original_and_abi_mem_reallocx_public_symbol_segment_bookkeeping() -> None:
    for size in [0x0000, 0x0040]:
        out, data = original_run(wrapper_mem_reallocx(size), dump_count=8)
        got = translated("abimemreallocx", hex(size))
        if got is not None:
            assert field(got, "data") == data.hex()
            assert field(got, "di") == f"{size:04x}"
        assert data[0:2] == b"\x02\x00"
        assert data[4:6] == struct.pack("<H", DATA_SEG)
        assert data[6:8] == struct.pack("<H", size)


def wrapper_deinit_125b9_idle_cleanup() -> bytes:
    setup = (
        setup_data_common()
        + mov_ds_word(0x006C, 0)
        + mov_ds_dword(0x00A0, 0x34561234)
        + mov_ds_byte(0x00C5, 0)
        + mov_ds_byte(0x00E0, 0)
        + mov_ds_byte(0x00E1, 0x55)
        + mov_ds_word(0x00FE, 0)
    )
    post = (
        mov_ax(DATA_SEG)
        + b"\x8e\xc0"
        + copy_bytes_to_scratch(0x006C, DSEG_SCRATCH + 0, 2)
        + copy_bytes_to_scratch(0x00A0, DSEG_SCRATCH + 2, 4)
        + copy_bytes_to_scratch(0x00C5, DSEG_SCRATCH + 6, 1)
        + copy_bytes_to_scratch(0x00E0, DSEG_SCRATCH + 7, 2)
        + copy_bytes_to_scratch(0x00FE, DSEG_SCRATCH + 9, 2)
    )
    return setup + b"\x9a" + struct.pack("<HH", original_offset("deinit_125B9"), LOAD_SEG) + post + b"\xc3"


def test_original_and_translated_deinit_125b9_idle_cleanup_clears_memory_handle() -> None:
    out, data = original_run(
        wrapper_deinit_125b9_idle_cleanup(),
        dump_count=11,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("deinit125b9idle")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "ds") == field(out, "ds")
    assert data == b"\x00\x00\x00\x00\x00\x00\x00\x00\x55\x00\x00"


def test_original_and_abi_deinit_125b9_public_symbol_idle_cleanup_clears_memory_handle() -> None:
    out, data = original_run(
        wrapper_deinit_125b9_idle_cleanup(),
        dump_count=11,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DATA_SEG,
        strict=False,
    )
    got = translated("abideinit125b9idle")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "ds") == field(out, "ds")
    assert data == b"\x00\x00\x00\x00\x00\x00\x00\x00\x55\x00\x00"


def wrapper_rtc_clock_call(target: int, wrapper_segment: int) -> bytes:
    handler_ip = WRAPPER_IP + 13 + 3 + 1
    setup = (
        mov_ax(wrapper_segment)
        + b"\x8e\xd8"
        + mov_dx(handler_ip)
        + mov_ax(0x251A)
        + b"\xcd\x21"
    )
    return (
        setup
        + call_rel16(target, WRAPPER_IP + len(setup) + 3)
        + b"\xc3"
        + b"\xb5\x12\xb1\x34\xb6\x56\xf8\xcf"
    )


def expected_rtc_ticks_for_123456() -> int:
    seconds = 12 * 3600 + 34 * 60 + 56
    return ((seconds * 1193180) >> 16) & 0xFFFFFFFF


def test_original_and_translated_initclockfromrtc_uses_bcd_rtc_time_for_bios_ticks() -> None:
    out, data = original_run(
        wrapper_rtc_clock_call(original_offset("initclockfromrtc"), LOAD_SEG),
        dump_count=4,
        dump_offset=0x046C,
        dump_seg=0,
        strict=False,
    )
    got = translated("rtcclock", "initclockfromrtc")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "es") == field(out, "es")
    assert field(out, "ax") == f"{expected_rtc_ticks_for_123456() & 0xFFFF:04x}"
    assert field(out, "dx") == "0000"
    assert field(out, "es") == "0000"


def test_original_and_abi_initclockfromrtc_public_symbol_uses_bcd_rtc_time_for_bios_ticks() -> None:
    out, data = original_run(
        wrapper_rtc_clock_call(original_offset("initclockfromrtc"), LOAD_SEG),
        dump_count=4,
        dump_offset=0x046C,
        dump_seg=0,
        strict=False,
    )
    got = translated("abirtcclock", "initclockfromrtc")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "es") == field(out, "es")
    assert field(out, "ax") == f"{expected_rtc_ticks_for_123456() & 0xFFFF:04x}"
    assert field(out, "dx") == "0000"
    assert field(out, "es") == "0000"


def test_original_and_translated_rereadrtc_settmr_uses_bcd_rtc_time_for_bios_ticks() -> None:
    out, data = original_run(
        wrapper_rtc_clock_call(original_offset("rereadrtc_settmr"), LOAD_SEG + SEG001_DELTA),
        dump_count=4,
        dump_offset=0x046C,
        dump_seg=0,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("rtcclock", "rereadrtc_settmr")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "es") == field(out, "es")
    assert field(out, "ax") == f"{expected_rtc_ticks_for_123456() & 0xFFFF:04x}"
    assert field(out, "dx") == "0000"
    assert field(out, "es") == "0000"


def test_original_and_abi_rereadrtc_settmr_public_symbol_uses_bcd_rtc_time_for_bios_ticks() -> None:
    out, data = original_run(
        wrapper_rtc_clock_call(original_offset("rereadrtc_settmr"), LOAD_SEG + SEG001_DELTA),
        dump_count=4,
        dump_offset=0x046C,
        dump_seg=0,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("abirtcclock", "rereadrtc_settmr")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "dx") == field(out, "dx")
        assert field(got, "es") == field(out, "es")
    assert field(out, "ax") == f"{expected_rtc_ticks_for_123456() & 0xFFFF:04x}"
    assert field(out, "dx") == "0000"
    assert field(out, "es") == "0000"


def wrapper_loadcfg_success() -> bytes:
    setup = setup_dseg_common()
    for index, value in enumerate(b"IPLAY.CFG\x00"):
        setup += mov_ds_byte(0x136F + index, value)
    call_ip = WRAPPER_IP + len(setup)
    post = (
        copy_bytes_to_scratch(0x1500, DSEG_SCRATCH, 4)
        + copy_bytes_to_scratch(0x1501, DSEG_SCRATCH + 4, 12)
    )
    return setup + call_rel16(original_offset("loadcfg"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_loadcfg_success_reads_magic_and_zero_checksum_settings() -> None:
    config = b"\x49\x4e\x52\x10" + bytes(12)
    out, data = original_run(
        wrapper_loadcfg_success(),
        dump_count=16,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
        extra_files={"IPLAY.CFG": config},
    )
    got = translated("loadcfgsuccess")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "ds") == field(out, "ds")
    assert data == b"\x49\x00\x00\x00" + bytes(12)


def test_original_and_abi_loadcfg_public_symbol_success_reads_magic_and_zero_checksum_settings() -> None:
    config = b"\x49\x4e\x52\x10" + bytes(12)
    out, data = original_run(
        wrapper_loadcfg_success(),
        dump_count=16,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
        extra_files={"IPLAY.CFG": config},
    )
    got = translated("abiloadcfgsuccess")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "ds") == field(out, "ds")
    assert data == b"\x49\x00\x00\x00" + bytes(12)


def wrapper_dosexec_no_comspec() -> bytes:
    setup = (
        setup_dseg_common()
        + mov_ds_word(0x002C, DSEG)
        + mov_ds_byte(0x0000, 0)
        + mov_ds_dword(0x1630, ((DSEG & 0xFFFF) << 16) | DSEG_SCRATCH)
        + mov_ds_word(0x164A, DSEG)
        + mov_ds_byte(0x1688, 0)
        + mov_ds_byte(0x1680, 0xAA)
        + mov_cs_byte(0x3168, 0x55)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = (
        copy_bytes_to_scratch(0x1680, DSEG_SCRATCH + 0, 1)
        + copy_bytes_to_scratch(0x7C55, DSEG_SCRATCH + 2, 2)
        + mov_ax(LOAD_SEG + SEG001_DELTA)
        + b"\x8e\xd8"
        + mov_ax(DSEG)
        + b"\x8e\xc0"
        + copy_bytes_to_scratch(0x3168, DSEG_SCRATCH + 1, 1)
    )
    return setup + call_rel16(original_offset("dosexec"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_dosexec_no_comspec_toggles_exec_guard_and_returns() -> None:
    out, data = original_run(
        wrapper_dosexec_no_comspec(),
        dump_count=4,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("dosexecnocomspec")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "ds") == field(out, "ds")
    assert data == bytes([0xFF, 0x00]) + struct.pack("<H", DSEG)


def test_original_and_abi_dosexec_public_symbol_no_comspec_toggles_exec_guard_and_returns() -> None:
    out, data = original_run(
        wrapper_dosexec_no_comspec(),
        dump_count=4,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("abidosexecnocomspec")
    if got is not None:
        assert field(got, "data") == data.hex()
        assert field(got, "ds") == field(out, "ds")
    assert data == bytes([0xFF, 0x00]) + struct.pack("<H", DSEG)


def wrapper_callsubx_sb16_failure() -> bytes:
    setup = (
        setup_dseg_common()
        + mov_ds_byte(0x1501, 3)
        + mov_ds_word(0x1502, 0x0220)
        + mov_ds_byte(0x1504, 5)
        + mov_ds_byte(0x1505, 1)
        + mov_ds_byte(0x1506, 22)
        + mov_ds_byte(0x1507, 0x33)
        + mov_ds_byte(0x1508, 0x44)
        + mov_ds_word(0x1509, 0x0181)
        + mov_ds_byte(0x150B, 0x55)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = (
        mov_ax(DATA_SEG)
        + b"\x8e\xd8"
        + mov_ax(DSEG)
        + b"\x8e\xc0"
        + copy_bytes_to_scratch(0x0132, DSEG_SCRATCH + 0, 1)
        + copy_bytes_to_scratch(0x0133, DSEG_SCRATCH + 1, 2)
        + copy_bytes_to_scratch(0x0135, DSEG_SCRATCH + 3, 1)
        + copy_bytes_to_scratch(0x0136, DSEG_SCRATCH + 4, 1)
        + copy_bytes_to_scratch(0x0137, DSEG_SCRATCH + 5, 1)
        + copy_bytes_to_scratch(0x0138, DSEG_SCRATCH + 6, 1)
        + copy_bytes_to_scratch(0x0139, DSEG_SCRATCH + 7, 1)
        + copy_bytes_to_scratch(0x00BE, DSEG_SCRATCH + 8, 2)
        + copy_bytes_to_scratch(0x013A, DSEG_SCRATCH + 10, 2)
        + mov_ax(DSEG)
        + b"\x8e\xd8"
        + copy_bytes_to_scratch(0x168E, DSEG_SCRATCH + 12, 1)
        + copy_bytes_to_scratch(0x1640, DSEG_SCRATCH + 13, 4)
    )
    return setup + call_rel16(original_offset("callsubx"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_callsubx_sb16_failure_records_requested_sound_settings() -> None:
    out, data = original_run(
        wrapper_callsubx_sb16_failure(),
        dump_count=17,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("callsubxfail")
    if got is not None:
        assert field(got, "data") == data.hex()
    assert data[:12] == bytes([3, 0x20, 0x02, 5, 1, 22, 0x33, 0x44]) + struct.pack("<H", 22000) + struct.pack("<H", 0x0181)
    assert data[12] == 1


def test_original_and_abi_callsubx_public_symbol_sb16_failure_records_requested_sound_settings() -> None:
    out, data = original_run(
        wrapper_callsubx_sb16_failure(),
        dump_count=17,
        dump_offset=DSEG_SCRATCH,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    got = translated("abicallsubxfail")
    if got is not None:
        assert field(got, "data") == data.hex()
    assert data[:12] == bytes([3, 0x20, 0x02, 5, 1, 22, 0x33, 0x44]) + struct.pack("<H", 22000) + struct.pack("<H", 0x0181)
    assert data[12] == 1


def setup_u32tox(value: int) -> bytes:
    return setup_common() + mov_eax(value) + mov_esi(DST_OFF)


def test_original_and_translated_u32tox_hex8_near_helper() -> None:
    for value in [0, 1, 0x1234ABCD, 0xFFFFFFFF]:
        out, data = original_call(original_offset("u32tox"), setup_u32tox(value), dump_count=8)
        got = translated("hex32", hex(value))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
        assert data == f"{value & 0xFFFFFFFF:08X}".encode("ascii")


def test_original_and_abi_u32tox_public_symbol() -> None:
    for value in [0, 1, 0x1234ABCD, 0xFFFFFFFF]:
        out, data = original_call(original_offset("u32tox"), setup_u32tox(value), dump_count=8)
        got = translated("abihex32", hex(value))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
        assert data == f"{value & 0xFFFFFFFF:08X}".encode("ascii")


def setup_my_putdigit(value: int, cx: int) -> bytes:
    return setup_common() + mov_dx(value) + mov_cx(cx) + mov_esi(DST_OFF)


def test_original_and_abi_my_putdigit_public_symbol() -> None:
    for value, cx in [(ord("0"), 0), (ord("9"), 5), (ord("A"), 0xFFFE)]:
        out, data = original_call(original_offset("my_putdigit"), setup_my_putdigit(value, cx), dump_count=1)
        got = translated("abiputdigit", hex(cx), hex(value))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "cx") == field(out, "cx")
        assert data == bytes([value & 0xFF])


def test_original_and_abi_my_u32toa10_public_symbol() -> None:
    for value, expected in [(0, b"0"), (1, b"1"), (42, b"42"), (1234567890, b"1234567890"), (0xFFFFFFFF, b"4294967295")]:
        out, data = original_call(original_offset("my_u32toa10_0"), setup_decimal32(value), dump_count=len(expected))
        got = translated("abiu32toa10", hex(value))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "cx") == field(out, "cx")
        assert data == expected


def test_original_and_abi_my_i32toa10_public_symbol() -> None:
    cases = [
        (0, b"0"),
        (1, b"1"),
        (0xFFFFFFFF, b"-1"),
        (0xFFFE1DC0, b"-123456"),
        (0x80000000, b"-2147483648"),
        (0x7FFFFFFF, b"2147483647"),
    ]
    for value, expected in cases:
        out, data = original_call(original_offset("my_i32toa10_0"), setup_decimal32(value), dump_count=len(expected))
        got = translated("abii32toa10", hex(value))
        if got is not None:
            assert got.endswith("data=" + data.hex())
            assert field(got, "si") == field(out, "si")
            assert field(got, "cx") == field(out, "cx")
        assert data == expected


def setup_memalloc12k_bounded_success() -> bytes:
    setup = setup_data_common()
    for index, value in enumerate(b"\xb8\x45\x23\xf8\xc3"):
        setup += mov_cs_byte(original_offset("memalloc") + index, value)
    setup += mov_ax(0x1111)
    setup += b"\x66\xbb" + struct.pack("<I", 0x22222222)
    setup += mov_cx(0x3333)
    setup += mov_dx(0x4444)
    setup += mov_di(0x5555)
    return setup


def test_original_and_translated_memalloc12k_uses_fixed_success_segment() -> None:
    out, _ = original_call(original_offset("memalloc12k"), setup_memalloc12k_bounded_success())
    assert field(out, "ax") == "2345"
    assert field(out, "bx") == "3040"
    assert field(out, "di") == "0000"
    assert field(out, "es") == "2345"

    got = translated("memalloc12kbounded")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "di") == field(out, "di")
        assert field(got, "es") == field(out, "es")


def test_original_and_abi_memalloc12k_public_symbol_uses_fixed_success_segment() -> None:
    out, _ = original_call(original_offset("memalloc12k"), setup_memalloc12k_bounded_success())
    assert field(out, "ax") == "2345"
    assert field(out, "bx") == "3040"
    assert field(out, "di") == "0000"
    assert field(out, "es") == "2345"

    got = translated("abimemalloc12kbounded")
    if got is not None:
        assert field(got, "ax") == field(out, "ax")
        assert field(got, "bx") == field(out, "bx")
        assert field(got, "di") == field(out, "di")
        assert field(got, "es") == field(out, "es")


def setup_f2_waves_bounded_pointer_install() -> bytes:
    setup = (
        setup_dseg_common()
        + mov_cs_byte(original_offset("init_vga_waves"), 0xC3)
        + mov_ds_word(0x164C, 0xAAAA)
        + mov_ds_word(0x164E, 0xBBBB)
        + mov_ds_word(0x1650, 0xCCCC)
        + mov_ds_word(0x1652, 0xDDDD)
        + mov_ds_byte(0x1680, 0xEE)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x164C, DSEG_SCRATCH + 0x3C0, 8)
    post += copy_bytes_to_scratch(0x1680, DSEG_SCRATCH + 0x3C8, 1)
    return setup + call_rel16(original_offset("f2_waves"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_f2_waves_installs_graph_callbacks() -> None:
    _, data = original_seg001_call(
        original_offset("f2_waves"),
        setup_f2_waves_bounded_pointer_install(),
        dump_count=9,
        dump_offset=DSEG_SCRATCH + 0x3C0,
        dump_seg=DSEG,
    )
    expected = struct.pack(
        "<HHHHB",
        original_offset("init_vga_waves"),
        original_offset("f2_draw_waves"),
        original_offset("f2_draw_waves2"),
        original_offset("init_vga_waves"),
        0xEE,
    )
    assert data == expected

    got = translated("graphsetup", "f2_waves")
    if got is not None:
        translated_data = bytes.fromhex(field(got, "data"))
        assert translated_data[:8] == expected[:8]


def test_original_and_abi_f2_waves_public_symbol_installs_graph_callbacks() -> None:
    _, data = original_seg001_call(
        original_offset("f2_waves"),
        setup_f2_waves_bounded_pointer_install(),
        dump_count=9,
        dump_offset=DSEG_SCRATCH + 0x3C0,
        dump_seg=DSEG,
    )
    expected = struct.pack(
        "<HHHHB",
        original_offset("init_vga_waves"),
        original_offset("f2_draw_waves"),
        original_offset("f2_draw_waves2"),
        original_offset("init_vga_waves"),
        0xEE,
    )
    assert data == expected

    got = translated("abigraphsetup", "f2_waves")
    if got is not None:
        translated_data = bytes.fromhex(field(got, "data"))
        assert translated_data[:8] == expected[:8]


def setup_init_vga_waves_bounded_already_initialized() -> bytes:
    bounded_tail = bytes.fromhex("8cd8bb800201d8a3f066058002a3f266c3")
    setup = (
        setup_dseg_common()
        + mov_cs_byte(original_offset("f2_draw_waves2"), 0xC3)
        + mov_cs_byte(original_offset("video_prp_mtr_positn"), 0xC3)
        + mov_ds_word(0x66F0, 0xAAAA)
        + mov_ds_word(0x66F2, 0xBBBB)
        + mov_ds_byte(0x1680, 3)
    )
    for index, value in enumerate(bounded_tail):
        setup += mov_cs_byte(0x1E2E + index, value)
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x66F0, DSEG_SCRATCH + 0x3D0, 4)
    post += copy_bytes_to_scratch(0x1680, DSEG_SCRATCH + 0x3D4, 1)
    return setup + call_rel16(original_offset("init_vga_waves"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_init_vga_waves_sets_wave_buffers() -> None:
    _, data = original_seg001_call(
        original_offset("init_vga_waves"),
        setup_init_vga_waves_bounded_already_initialized(),
        dump_count=5,
        dump_offset=DSEG_SCRATCH + 0x3D0,
        dump_seg=DSEG,
    )
    buffer_1seg = (DSEG + 0x0280) & 0xFFFF
    expected = struct.pack("<HHB", buffer_1seg, (buffer_1seg + 0x0280) & 0xFFFF, 3)
    assert data == expected

    got = translated("initvgabounded")
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_original_and_abi_init_vga_waves_public_symbol_sets_wave_buffers() -> None:
    _, data = original_seg001_call(
        original_offset("init_vga_waves"),
        setup_init_vga_waves_bounded_already_initialized(),
        dump_count=5,
        dump_offset=DSEG_SCRATCH + 0x3D0,
        dump_seg=DSEG,
    )
    buffer_1seg = (DSEG + 0x0280) & 0xFFFF
    expected = struct.pack("<HHB", buffer_1seg, (buffer_1seg + 0x0280) & 0xFFFF, 3)
    assert data == expected

    got = translated("abiinitvgabounded")
    if got is not None:
        assert field(got, "data") == expected.hex()


def wrapper_f2_draw_waves_bounded(symbol: str) -> bytes:
    setup = (
        setup_dseg_common()
        + mov_ax(LOAD_SEG)
        + b"\x8e\xc0"
        + mov_es_byte(original_offset("volume_prep"), 0xCB)
        + mov_ds_word(0x66F0, DSEG)
        + mov_ds_word(0x66F2, DSEG)
        + mov_ds_word(0x1654, 1)
        + mov_ds_word(0x16AC, 0)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    if symbol == "f2_draw_waves2":
        setup += mov_cs_byte(original_offset("f2_draw_waves2") + 0x69, 0xC3)
    call_ip = WRAPPER_IP + len(setup)
    post = mov_ax(DSEG) + b"\x8e\xc0" + copy_bytes_to_scratch(0x66F0, DSEG_SCRATCH + 0x3D8, 4)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_f2_draw_waves_swaps_wave_buffers() -> None:
    _, data = original_run(
        wrapper_f2_draw_waves_bounded("f2_draw_waves"),
        dump_count=4,
        dump_offset=DSEG_SCRATCH + 0x3D8,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    expected = struct.pack("<HH", DSEG, DSEG)
    assert data == expected

    got = translated("f2drawbounded", "f2_draw_waves")
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_original_and_abi_f2_draw_waves_public_symbol_swaps_wave_buffers() -> None:
    _, data = original_run(
        wrapper_f2_draw_waves_bounded("f2_draw_waves"),
        dump_count=4,
        dump_offset=DSEG_SCRATCH + 0x3D8,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    expected = struct.pack("<HH", DSEG, DSEG)
    assert data == expected

    got = translated("abif2drawbounded", "f2_draw_waves")
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_original_and_translated_f2_draw_waves2_keeps_wave_buffers() -> None:
    _, data = original_run(
        wrapper_f2_draw_waves_bounded("f2_draw_waves2"),
        dump_count=4,
        dump_offset=DSEG_SCRATCH + 0x3D8,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    expected = struct.pack("<HH", DSEG, DSEG)
    assert data == expected

    got = translated("f2drawbounded", "f2_draw_waves2")
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_original_and_abi_f2_draw_waves2_public_symbol_keeps_wave_buffers() -> None:
    _, data = original_run(
        wrapper_f2_draw_waves_bounded("f2_draw_waves2"),
        dump_count=4,
        dump_offset=DSEG_SCRATCH + 0x3D8,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
        strict=False,
    )
    expected = struct.pack("<HH", DSEG, DSEG)
    assert data == expected

    got = translated("abif2drawbounded", "f2_draw_waves2")
    if got is not None:
        assert field(got, "data") == expected.hex()


def wrapper_readallmoules_bounded_success() -> bytes:
    setup = (
        setup_dseg_common()
        + mov_cs_byte(original_offset("read_module"), 0xF8)
        + mov_cs_byte(original_offset("read_module") + 1, 0xC3)
        + mov_ds_word(0x1660, 1)
        + mov_ds_byte(0x167E, 0xAA)
        + mov_ax(0x1234)
        + mov_bx(0x5678)
        + mov_cx(0x9ABC)
        + mov_dx(0xDEF0)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x1660, DSEG_SCRATCH + 0x3E0, 2)
    post += copy_bytes_to_scratch(0x167E, DSEG_SCRATCH + 0x3E2, 1)
    return setup + call_rel16(original_offset("readallmoules"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_readallmoules_success_after_one_module() -> None:
    out, data = original_run(
        wrapper_readallmoules_bounded_success(),
        dump_count=3,
        dump_offset=DSEG_SCRATCH + 0x3E0,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    expected = struct.pack("<HB", 1, 0xAA)
    assert data == expected
    assert int(field(out, "flags"), 16) & 1 == 0

    got = translated("readallmoulesbounded")
    if got is not None:
        assert field(got, "data") == expected.hex()
        assert int(field(got, "flags"), 16) & 1 == 0


def test_original_and_abi_readallmoules_public_symbol_success_after_one_module() -> None:
    out, data = original_run(
        wrapper_readallmoules_bounded_success(),
        dump_count=3,
        dump_offset=DSEG_SCRATCH + 0x3E0,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    expected = struct.pack("<HB", 1, 0xAA)
    assert data == expected
    assert int(field(out, "flags"), 16) & 1 == 0

    got = translated("abireadallmoulesbounded")
    if got is not None:
        assert field(got, "data") == expected.hex()
        assert int(field(got, "flags"), 16) & 1 == 0


def wrapper_read_module_bounded_moduleread_failure() -> bytes:
    filename = b"c:\\mods\\demo.s3m\x00"
    setup = setup_dseg_common()
    for index, value in enumerate(filename):
        setup += mov_ds_byte(DSEG_SCRATCH + 0x100 + index, value)
    for index, value in enumerate(b"\xf9\x90\x90\x90\x90"):
        setup += mov_cs_byte(original_offset("read_module") + 0x56 + index, value)
    setup += mov_dx(DSEG_SCRATCH + 0x100)
    call_ip = WRAPPER_IP + len(setup)
    post = b"\x9c\x58" + mov_ds_word(DSEG_SCRATCH + 0x3E8, 0)
    post += b"\xa3" + struct.pack("<H", DSEG_SCRATCH + 0x3E8)
    post += copy_bytes_to_scratch(0x168E, DSEG_SCRATCH + 0x3EA, 1)
    post += copy_bytes_to_scratch(0x1640, DSEG_SCRATCH + 0x3EB, 4)
    post += copy_bytes_to_scratch(0x0472, DSEG_SCRATCH + 0x3EF, 12)
    return setup + call_rel16(original_offset("read_module"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_read_module_records_moduleread_failure() -> None:
    _, data = original_run(
        wrapper_read_module_bounded_moduleread_failure(),
        dump_count=19,
        dump_offset=DSEG_SCRATCH + 0x3E8,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    expected_tail = (
        bytes([3])
        + struct.pack("<HH", 0x128B, DSEG)
        + b"DEMO.S3M    "
    )
    assert struct.unpack("<H", data[:2])[0] & 1
    assert data[2:] == expected_tail

    got = translated("readmodulefail")
    if got is not None:
        got_data = bytes.fromhex(field(got, "data"))
        assert struct.unpack("<H", got_data[:2])[0] & 1
        assert got_data[2:] == expected_tail


def test_original_and_abi_read_module_public_symbol_records_moduleread_failure() -> None:
    _, data = original_run(
        wrapper_read_module_bounded_moduleread_failure(),
        dump_count=19,
        dump_offset=DSEG_SCRATCH + 0x3E8,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    expected_tail = (
        bytes([3])
        + struct.pack("<HH", 0x128B, DSEG)
        + b"DEMO.S3M    "
    )
    assert struct.unpack("<H", data[:2])[0] & 1
    assert data[2:] == expected_tail

    got = translated("abireadmodulefail")
    if got is not None:
        got_data = bytes.fromhex(field(got, "data"))
        assert struct.unpack("<H", got_data[:2])[0] & 1
        assert got_data[2:] == expected_tail


def wrapper_moduleread_missing_file_failure() -> bytes:
    filename = b"missing.nope\x00"
    setup = setup_data_common()
    for index, value in enumerate(filename):
        setup += mov_ds_byte(DSEG_SCRATCH + 0x120 + index, value)
    setup += mov_cs_byte(original_offset("snd_offx"), 0xCB)
    setup += mov_cs_byte(original_offset("memfree_125DA"), 0xCB)
    setup += mov_cs_byte(original_offset("ems_restore_mapctx"), 0xC3)
    setup += mov_ds_word(0x00C0, 0xA5A5)
    setup += mov_ds_word(0x00C2, 0xBEEF)
    setup += mov_ds_byte(0x007B, 0x5A)
    setup += mov_dx(DSEG_SCRATCH + 0x120)
    post = b"\x9c\x58" + b"\xa3" + struct.pack("<H", DSEG_SCRATCH + 0x3F8)
    post += copy_bytes_to_scratch(0x00C0, DSEG_SCRATCH + 0x3FA, 4)
    post += copy_bytes_to_scratch(0x007B, DSEG_SCRATCH + 0x3FE, 1)
    return setup + b"\x9a" + struct.pack("<HH", original_offset("moduleread"), LOAD_SEG) + post + b"\xc3"


def test_original_and_translated_moduleread_missing_file_failure() -> None:
    _, data = original_run(
        wrapper_moduleread_missing_file_failure(),
        dump_count=7,
        dump_offset=DSEG_SCRATCH + 0x3F8,
        dump_seg=DATA_SEG,
    )
    assert struct.unpack("<H", data[:2])[0] & 1
    assert data[2:4] == struct.pack("<H", 2)
    assert data[4:6] == struct.pack("<H", 0xBEEF)
    assert data[6] == 0x5A

    got = translated("modulereadfail")
    if got is not None:
        got_data = bytes.fromhex(field(got, "data"))
        assert struct.unpack("<H", got_data[:2])[0] & 1
        assert got_data[2:] == data[2:]


def test_original_and_abi_moduleread_public_symbol_missing_file_failure() -> None:
    _, data = original_run(
        wrapper_moduleread_missing_file_failure(),
        dump_count=7,
        dump_offset=DSEG_SCRATCH + 0x3F8,
        dump_seg=DATA_SEG,
    )
    assert struct.unpack("<H", data[:2])[0] & 1
    assert data[2:4] == struct.pack("<H", 2)
    assert data[4:6] == struct.pack("<H", 0xBEEF)
    assert data[6] == 0x5A

    got = translated("abimodulereadfail")
    if got is not None:
        got_data = bytes.fromhex(field(got, "data"))
        assert struct.unpack("<H", got_data[:2])[0] & 1
        assert got_data[2:] == data[2:]


def wrapper_mod_read_10311_one_empty_pattern() -> bytes:
    output = DSEG_SCRATCH + 0x500
    alloc_stub = mov_ax(DATA_SEG) + b"\x8e\xc0" + mov_di(output) + b"\xf8\xc3"
    setup = (
        setup_data_common()
        + mov_cs_byte(original_offset("dosfread"), 0xC3)
        + mov_cs_byte(original_offset("mem_reallocx"), 0xC3)
        + mov_ds_word(0x0052, 1)
        + mov_ds_word(0x0034, 0)
        + mov_ds_word(0x0130, 0)
    )
    for index, value in enumerate(alloc_stub):
        setup += mov_cs_byte(original_offset("memalloc12k") + index, value)
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset("mod_read_10311"), call_ip + 3) + b"\xc3"


def test_original_and_translated_mod_read_10311_one_empty_pattern() -> None:
    expected = bytes(64)
    _, data = original_run(
        wrapper_mod_read_10311_one_empty_pattern(),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x500,
        dump_seg=DATA_SEG,
    )
    assert data == expected

    got = translated("modread10311bounded")
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_original_and_abi_mod_read_10311_public_symbol_one_empty_pattern() -> None:
    expected = bytes(64)
    _, data = original_run(
        wrapper_mod_read_10311_one_empty_pattern(),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x500,
        dump_seg=DATA_SEG,
    )
    assert data == expected

    got = translated("abimodread10311bounded")
    if got is not None:
        assert field(got, "data") == expected.hex()


def wrapper_mod_n_t_module_bounded_header_path() -> bytes:
    setup = (
        setup_data_common()
        + mov_cs_byte(original_offset("mod_1021E"), 0xC3)
        + mov_cs_byte(original_offset("mod_1024A"), 0xC3)
        + mov_cs_byte(original_offset("mod_102F5"), 0xC3)
        + mov_cs_byte(original_offset("mod_read_10311"), 0xC3)
        + mov_cs_byte(original_offset("mod_readfile_11F4E"), 0xC3)
        + mov_cs_byte(0x00E7, 0xF8)
        + mov_cs_byte(0x00E8, 0x90)
        + mov_ds_word(0x0130, 0xBEEF)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x010C, DSEG_SCRATCH + 0x580, 4)
    post += copy_bytes_to_scratch(0x0032, DSEG_SCRATCH + 0x584, 2)
    post += copy_bytes_to_scratch(0x0034, DSEG_SCRATCH + 0x586, 2)
    post += copy_bytes_to_scratch(0x0130, DSEG_SCRATCH + 0x588, 2)
    return setup + call_rel16(original_offset("mod_n_t_module"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_mod_n_t_module_bounded_header_path() -> None:
    expected = struct.pack("<IHHH", 0x2E542E4E, 0x000F, 0x0004, 0xBEEF)
    _, data = original_run(
        wrapper_mod_n_t_module_bounded_header_path(),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x580,
        dump_seg=DATA_SEG,
    )
    assert data == expected

    got = translated("modntbounded")
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_original_and_abi_mod_n_t_module_public_symbol_bounded_header_path() -> None:
    expected = struct.pack("<IHHH", 0x2E542E4E, 0x000F, 0x0004, 0xBEEF)
    _, data = original_run(
        wrapper_mod_n_t_module_bounded_header_path(),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x580,
        dump_seg=DATA_SEG,
    )
    assert data == expected

    got = translated("abimodntbounded")
    if got is not None:
        assert field(got, "data") == expected.hex()


def loader_header_snapshot() -> bytes:
    post = copy_bytes_to_scratch(0x010C, DSEG_SCRATCH + 0x5A0, 4)
    post += copy_bytes_to_scratch(0x0130, DSEG_SCRATCH + 0x5A4, 2)
    post += copy_bytes_to_scratch(0x0032, DSEG_SCRATCH + 0x5A6, 2)
    post += copy_bytes_to_scratch(0x0034, DSEG_SCRATCH + 0x5A8, 2)
    post += copy_bytes_to_scratch(0x0052, DSEG_SCRATCH + 0x5AA, 2)
    post += copy_bytes_to_scratch(0x005A, DSEG_SCRATCH + 0x5AC, 2)
    post += copy_bytes_to_scratch(0x003E, DSEG_SCRATCH + 0x5AE, 2)
    post += copy_bytes_to_scratch(0x00D3, DSEG_SCRATCH + 0x5B0, 1)
    post += copy_bytes_to_scratch(0x00DE, DSEG_SCRATCH + 0x5B1, 1)
    post += copy_bytes_to_scratch(0x00D9, DSEG_SCRATCH + 0x5B2, 1)
    post += copy_bytes_to_scratch(0x00DA, DSEG_SCRATCH + 0x5B3, 1)
    return post


def wrapper_format_loader_header(symbol: str) -> bytes:
    setup = setup_data_common()
    if symbol == "_2stm_module":
        for index, value in enumerate(mov_ax(0x1234) + b"\xc3"):
            setup += mov_cs_byte(original_offset("sub_13E9B") + index, value)
        setup += mov_cs_byte(0x0423, 0xC3)
    elif symbol == "e669_module":
        setup += mov_cs_byte(0x0952, 0xC3)
    elif symbol == "mtm_module":
        setup += mov_ds_byte(0x0132, 1)
        setup += mov_cs_byte(0x0B3E, 0xC3)
    elif symbol == "psm_module":
        setup += mov_cs_byte(0x0D71, 0xC3)
    elif symbol == "far_module":
        setup += mov_ds_byte(0x0132, 1)
        for index, value in enumerate(b"\xb0\x66\xc3"):
            setup += mov_cs_byte(original_offset("calc_14043") + index, value)
        setup += mov_cs_byte(0x0F90, 0xC3)
    elif symbol == "ult_module":
        setup += mov_cs_byte(0x127F, 0xC3)
    elif symbol == "s3m_module":
        setup += mov_cs_byte(0x0683, 0xC3)
    elif symbol == "inr_module":
        setup += mov_cs_byte(0x1A39, 0xC3)
    else:
        raise AssertionError(f"unsupported bounded loader: {symbol}")
    call_ip = WRAPPER_IP + len(setup)
    return setup + call_rel16(original_offset(symbol), call_ip + 3) + loader_header_snapshot() + b"\xc3"


def expected_loader_header(
    module_type: int,
    moduleflag: int,
    size1: int,
    channels: int,
    patterns: int,
    orders: int,
    freq: int,
    byte_24673: int,
    byte_2467e: int,
    byte_24679: int,
    byte_2467a: int,
) -> bytes:
    return struct.pack(
        "<IHHHHHHBBBB",
        module_type,
        moduleflag,
        size1,
        channels,
        patterns,
        orders,
        freq,
        byte_24673,
        byte_2467e,
        byte_24679,
        byte_2467a,
    )


def test_original_and_translated_format_loader_bounded_headers() -> None:
    cases = [
        ("_2stm_module", expected_loader_header(0x4D545332, 0x0008, 0x001F, 0x0004, 0, 0, 8448, 0, 0, 0x12, 0x34)),
        ("e669_module", expected_loader_header(0x39363645, 0x0004, 0, 0x0008, 0, 0, 0, 0x80, 2, 0, 0)),
        ("mtm_module", expected_loader_header(0x204D544D, 0x0020, 0, 0, 1, 1, 0, 0x80, 0, 6, 0x7D)),
        ("psm_module", expected_loader_header(0x204D5350, 0x0040, 0, 0, 0, 0, 8448, 0, 0, 0, 0)),
        ("far_module", expected_loader_header(0x20524146, 0x0080, 0, 0x0010, 0, 0, 0, 0, 2, 4, 0x66)),
        ("ult_module", expected_loader_header(0x20544C55, 0x0200, 0, 0, 0, 0, 0, 0, 0, 6, 0x7D)),
        ("s3m_module", expected_loader_header(0x204D3353, 0x0010, 0, 0x0020, 0, 0, 8363, 0, 1, 0, 0)),
        ("inr_module", expected_loader_header(0x20524E49, 0x0100, 0, 4, 0, 0, 0, 0, 0, 0, 0)),
    ]
    for symbol, expected in cases:
        _, data = original_run(
            wrapper_format_loader_header(symbol),
            dump_count=len(expected),
            dump_offset=DSEG_SCRATCH + 0x5A0,
            dump_seg=DATA_SEG,
        )
        assert data == expected

        got = translated("formatloaderheader", symbol)
        if got is not None:
            assert field(got, "data") == expected.hex()


def test_original_and_abi_format_loader_public_symbols_bounded_headers() -> None:
    cases = [
        ("_2stm_module", expected_loader_header(0x4D545332, 0x0008, 0x001F, 0x0004, 0, 0, 8448, 0, 0, 0x12, 0x34)),
        ("e669_module", expected_loader_header(0x39363645, 0x0004, 0, 0x0008, 0, 0, 0, 0x80, 2, 0, 0)),
        ("mtm_module", expected_loader_header(0x204D544D, 0x0020, 0, 0, 1, 1, 0, 0x80, 0, 6, 0x7D)),
        ("psm_module", expected_loader_header(0x204D5350, 0x0040, 0, 0, 0, 0, 8448, 0, 0, 0, 0)),
        ("far_module", expected_loader_header(0x20524146, 0x0080, 0, 0x0010, 0, 0, 0, 0, 2, 4, 0x66)),
        ("ult_module", expected_loader_header(0x20544C55, 0x0200, 0, 0, 0, 0, 0, 0, 0, 6, 0x7D)),
        ("s3m_module", expected_loader_header(0x204D3353, 0x0010, 0, 0x0020, 0, 0, 8363, 0, 1, 0, 0)),
        ("inr_module", expected_loader_header(0x20524E49, 0x0100, 0, 4, 0, 0, 0, 0, 0, 0, 0)),
    ]
    for symbol, expected in cases:
        _, data = original_run(
            wrapper_format_loader_header(symbol),
            dump_count=len(expected),
            dump_offset=DSEG_SCRATCH + 0x5A0,
            dump_seg=DATA_SEG,
        )
        assert data == expected

        got = translated("abiformatloaderheader", symbol)
        if got is not None:
            assert field(got, "data") == expected.hex()


def wrapper_modules_search_bounded_entry() -> bytes:
    setup = (
        setup_dseg_common()
        + mov_cs_byte(original_offset("modules_search") + 0x1B, 0xC3)
        + mov_ds_word(0x1662, 0)
        + mov_ds_word(0x1674, 0xAAAA)
        + mov_ds_word(0x1676, 0xBBBB)
    )
    call_ip = WRAPPER_IP + len(setup)
    post = copy_bytes_to_scratch(0x1674, DSEG_SCRATCH + 0x5C0, 4)
    post += copy_bytes_to_scratch(0x1662, DSEG_SCRATCH + 0x5C4, 2)
    return setup + call_rel16(original_offset("modules_search"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_modules_search_bounded_entry_state() -> None:
    expected = struct.pack("<HHH", 2192, 0, 0)
    _, data = original_run(
        wrapper_modules_search_bounded_entry(),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x5C0,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    assert data == expected

    got = translated("modulessearchbounded")
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_original_and_abi_modules_search_public_symbol_bounded_entry_state() -> None:
    expected = struct.pack("<HHH", 2192, 0, 0)
    _, data = original_run(
        wrapper_modules_search_bounded_entry(),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x5C0,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    assert data == expected

    got = translated("abimodulessearchbounded")
    if got is not None:
        assert field(got, "data") == expected.hex()


def wrapper_start_bounded_entry() -> bytes:
    setup = (
        setup_dseg_common()
        + mov_cs_byte(original_offset("start") + 0x1C, 0xC3)
        + mov_ds_word(0x164A, 0)
        + mov_ax(0x2222)
        + b"\x8e\xc0"
    )
    call_ip = WRAPPER_IP + len(setup)
    post = mov_ax(DSEG) + b"\x8e\xc0" + copy_bytes_to_scratch(0x164A, DSEG_SCRATCH + 0x5D0, 2)
    return setup + call_rel16(original_offset("start"), call_ip + 3) + post + b"\xc3"


def test_original_and_translated_start_sets_data_segment_on_entry() -> None:
    expected = struct.pack("<H", 0)
    out, data = original_run(
        wrapper_start_bounded_entry(),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x5D0,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    assert data == expected
    assert field(out, "ds") == f"{DSEG:04x}"

    got = translated("startbounded")
    if got is not None:
        assert field(got, "data") == expected.hex()
        assert field(got, "ds") == field(out, "ds")


def test_original_and_abi_start_public_symbol_sets_data_segment_on_entry() -> None:
    expected = struct.pack("<H", 0)
    out, data = original_run(
        wrapper_start_bounded_entry(),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x5D0,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    assert data == expected
    assert field(out, "ds") == f"{DSEG:04x}"

    got = translated("abistartbounded")
    if got is not None:
        assert field(got, "data") == expected.hex()
        assert field(got, "ds") == field(out, "ds")


def wrapper_keyb_19efd_bounded_entry_state() -> bytes:
    sub1265d_stub = mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + b"\xcb"
    sub1265d_near_stub = mov_ax(0x1234) + mov_bx(0x5678) + mov_cx(0x9ABC) + b"\xc3"
    setup = (
        setup_dseg_common()
        + mov_ax(LOAD_SEG)
        + b"\x8e\xc0"
        + mov_cs_byte(original_offset("keyb_19EFD") + 0x14, 0xC3)
    )
    for index, value in enumerate(sub1265d_stub):
        setup += mov_es_byte(original_offset("sub_1265D") + index, value)
    for index, value in enumerate(sub1265d_near_stub):
        setup += mov_cs_byte(original_offset("sub_1265D") + index, value)
    post = mov_ax(DSEG) + b"\x8e\xc0"
    post += copy_bytes_to_scratch(0x1682, DSEG_SCRATCH + 0x5E0, 1)
    post += copy_bytes_to_scratch(0x1684, DSEG_SCRATCH + 0x5E1, 3)
    continuation_ip = WRAPPER_IP + len(setup) + 9
    abs_call = mov_ax(continuation_ip) + b"\x50" + mov_ax(original_offset("keyb_19EFD")) + b"\x50\xc3"
    return setup + abs_call + post + b"\xc3"


def test_original_and_translated_keyb_19efd_entry_state_snapshot() -> None:
    expected = bytes([0x12, 0x34, 0x56, 0x9A])
    _, data = original_run(
        wrapper_keyb_19efd_bounded_entry_state(),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x5E0,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    assert data == expected

    got = translated("keybbounded")
    if got is not None:
        assert field(got, "data") == expected.hex()


def test_original_and_abi_keyb_19efd_public_symbol_entry_state_snapshot() -> None:
    expected = bytes([0x12, 0x34, 0x56, 0x9A])
    _, data = original_run(
        wrapper_keyb_19efd_bounded_entry_state(),
        dump_count=len(expected),
        dump_offset=DSEG_SCRATCH + 0x5E0,
        dump_seg=DSEG,
        call_cs=LOAD_SEG + SEG001_DELTA,
        wrapper_load_offset=SEG001_DELTA * 16 + WRAPPER_IP,
    )
    assert data == expected

    got = translated("abikeybbounded")
    if got is not None:
        assert field(got, "data") == expected.hex()
