from __future__ import annotations

from pathlib import Path
from typing import Optional, Union
import re


FNV1A_32_OFFSET = 2166136261
FNV1A_32_PRIME = 16777619
VGA_COLOR_TEXT_SEG = 0xB800
VGA_MONO_TEXT_SEG = 0xB000
VGA_TEXT_OFFSET = 0x0000
VGA_TEXT_CELL_BYTES = 2
VGA_TEXT_COLS_40 = 40
VGA_TEXT_COLS_80 = 80
VGA_TEXT_ROWS_25 = 25
VGA_TEXT_ROWS_50 = 50
SB16_STEREO_BYTES_PER_FRAME = 4
SB16_BOUNDED_BLOCK_FRAMES = 512
SB16_CONTINUOUS_BLOCK_FRAMES = 1024
SB16_BOUNDED_BLOCK_BYTES = SB16_BOUNDED_BLOCK_FRAMES * SB16_STEREO_BYTES_PER_FRAME
SB16_CONTINUOUS_BLOCK_BYTES = SB16_CONTINUOUS_BLOCK_FRAMES * SB16_STEREO_BYTES_PER_FRAME


TextCells = Union[bytes, bytearray]

MODULE_LOADED_RE = re.compile(
    r"^Module: (?P<name>\S+)$",
    re.MULTILINE,
)

MODULE_SIZE_RE = re.compile(
    r"^Size: (?P<size>\d+) bytes$",
    re.MULTILINE,
)

MODULE_LOADER_RE = re.compile(
    r"^Loader: (?P<loader>.+)$",
    re.MULTILINE,
)

DECODER_HANDOFF_RE = re.compile(
    r"^Decoder handoff: (?P<handoff>.+)$",
    re.MULTILINE,
)

MODULE_TYPE_TAG_RE = re.compile(
    r"^Module type tag: (?P<tag>[0-9A-F]+)$",
    re.MULTILINE,
)

MODULE_TITLE_RE = re.compile(
    r"^Title: (?P<title>.+)$",
    re.MULTILINE,
)

UNSUPPORTED_MODULE_RE = re.compile(
    r"^Unsupported module type: (?P<name>\S+)$",
    re.MULTILINE,
)

PLAYBACK_OUTPUT_RE = re.compile(
    r"^Playback output: (?P<output>.+)$",
    re.MULTILINE,
)

PLAYBACK_DISABLED_RE = re.compile(
    r"^Playback disabled: (?P<reason>.+)$",
    re.MULTILINE,
)

FFI_MARKER_RE = re.compile(
    r"^FFI: (?P<marker>[0-9A-F]{4})$",
    re.MULTILINE,
)

ORDERS_CHANNELS_RE = re.compile(
    r"^Orders: (?P<orders>\d+) Channels: (?P<channels>\d+)$",
    re.MULTILINE,
)

HELP_USAGE_TEXT = "Usage: IPLAY [Switches] [FileName.Ext|@FileList.Ext]"
SUPPORTED_DOS_FORMATS_TEXT = "Supported by this DOS hardware build: MOD NST S3M STM 669 MTM PSM FAR ULT WOW OKT OCT XM IT PTM AMS DBM DMF MDL DSM MED IMF J2B"
SB16_AUDIO_SCOPE_TEXT = "Audio driver scope: SB16 16-bit stereo only."
TEXT_BACKEND_TEXT = "Text backend: VGA color/BW text memory"
TEXT_BACKEND_MEMORY_TEXT = "Text backend: VGA color/BW text memory at B800:0000/B000:0000."
SDL_COMPAT_AUDIO_BACKEND_TEXT = "Audio backend: SB16 16-bit stereo hardware wrapper, SDL-compatible callback boundary."

SCREEN_PRESENT_RE = re.compile(
    r"^Screen present: "
    r"reason=(?P<reason>\S+) "
    r"scope=(?P<scope>\S+) "
    r"bytes=(?P<bytes>\d+) "
    r"screen_bytes=(?P<screen_bytes>\d+) "
    r"screen_checksum=(?P<checksum>\d+) "
    r"screen_nonblank=(?P<nonblank>\d+) "
    r"full=(?P<full>[01]) "
    r"cols=(?P<cols>\d+) "
    r"rows=(?P<rows>\d+) "
    r"mode_ok=(?P<mode_ok>[01]) "
    r"audio_frames=(?P<audio_frames>\d+) "
    r"levels=(?P<left_level>\d+)/(?P<right_level>\d+)$",
    re.MULTILINE,
)

PLAYER_HW_TEXT_RE = re.compile(
    r"\btext_copies=(?P<text_copies>\d+) "
    r"text_seg=(?P<segment>[0-9a-fA-F]{4}) "
    r"text_off=(?P<offset>[0-9a-fA-F]{4}) "
    r"text_bytes=(?P<bytes>\d+) "
    r"text_checksum=(?P<checksum>\d+) "
    r"text_nonblank=(?P<nonblank>\d+)\b"
)

PLAYER_HW_AUDIO_RE = re.compile(
    r"\baudio_copies=(?P<audio_copies>\d+) "
    r"audio_bytes=(?P<bytes>\d+) "
    r"audio_checksum=(?P<checksum>\d+) "
    r"audio_first=(?P<first>[0-9a-fA-F]{4}) "
    r"audio_tail=(?P<tail>[0-9a-fA-F]{4})\b"
)

PLAYBACK_PUMP_RE = re.compile(
    r"^Playback pump: "
    r"blocks=(?P<blocks>\d+) "
    r"frames=(?P<frames>\d+) "
    r"accepted=(?P<accepted>\d+) "
    r"checksum=(?P<checksum>\d+) "
    r"limit=(?P<limit>[01]) "
    r"source_end=(?P<source_end>[01]) "
    r"stop=(?P<stop>\S+)$",
    re.MULTILINE,
)

PLAYBACK_LOOP_RE = re.compile(
    r"^Playback loop: "
    r"mode=(?P<mode>\S+) "
    r"policy=(?P<policy>\S+) "
    r"cadence=(?P<cadence>\S+) "
    r"max_blocks=(?P<max_blocks>\d+) "
    r"frames/block=(?P<frames_per_block>\d+)$",
    re.MULTILINE,
)

DECODER_ROUTE_RE = re.compile(
    r"^Decoder route: "
    r"id=(?P<id>\d+) "
    r"name=(?P<name>\S+)$",
    re.MULTILINE,
)

DECODER_PROGRESS_RE = re.compile(
    r"^Decoder progress: "
    r"block=(?P<block>\d+)/(?P<total_blocks>\d+) "
    r"order=(?P<order>\d+) "
    r"pattern=(?P<pattern>\d+) "
    r"row=(?P<row>\d+) "
    r"channel=(?P<channel>\d+) "
    r"tick=(?P<tick>\d+)/(?P<speed>\d+) "
    r"speed=(?P<reported_speed>\d+) "
    r"tempo=(?P<tempo>\d+) "
    r"ended=(?P<ended>[01]) "
    r"loop=(?P<loop>[01])$",
    re.MULTILINE,
)

DECODER_GEOMETRY_RE = re.compile(
    r"^Decoder geometry: "
    r"orders=(?P<orders>\d+) "
    r"rows/order=(?P<rows_per_order>\d+) "
    r"restart=(?P<restart>\d+) "
    r"speed=(?P<speed>\d+) "
    r"tempo=(?P<tempo>\d+) "
    r"channels=(?P<channels>\d+)$",
    re.MULTILINE,
)

DECODER_EVENT_RE = re.compile(
    r"^Decoder event: "
    r"period=(?P<period>\d+) "
    r"note=(?P<note>\d+) "
    r"octave=(?P<octave>\d+) "
    r"instrument=(?P<instrument>\d+) "
    r"volume=(?P<volume>\d+) "
    r"effect=(?P<effect>\d+) "
    r"param=(?P<param>\d+)$",
    re.MULTILINE,
)

DECODER_VOICE_RE = re.compile(
    r"^Decoder voice: "
    r"active=(?P<active>[01]) "
    r"period=(?P<period>\d+) "
    r"note=(?P<note>\d+) "
    r"octave=(?P<octave>\d+) "
    r"instrument=(?P<instrument>\d+) "
    r"volume=(?P<volume>\d+) "
    r"sample_len=(?P<sample_len>\d+) "
    r"sample_vol=(?P<sample_vol>\d+) "
    r"loop=(?P<loop_start>\d+)/(?P<loop_len>\d+) "
    r"data=(?P<data>\d+)$",
    re.MULTILINE,
)

PCM_SOURCE_RE = re.compile(
    r"^PCM source: "
    r"(?P<source>\S+) "
    r"seed=(?P<seed>\d+) "
    r"truncated=(?P<truncated>[01]) "
    r"input=(?P<input>\S+) "
    r"renderer=(?P<renderer>\S+) "
    r"route=(?P<route>\d+) "
    r"provider=(?P<provider>\S+) "
    r"hook_provider=(?P<hook_provider>\S+) "
    r"stream_start=(?P<stream_start>\d+)$",
    re.MULTILINE,
)


def text_cell_checksum(cells: TextCells, byte_count: Optional[int] = None) -> int:
    if byte_count is None:
        byte_count = len(cells)
    checksum = FNV1A_32_OFFSET
    for value in cells[:byte_count]:
        checksum ^= value
        checksum = (checksum * FNV1A_32_PRIME) & 0xFFFFFFFF
    return checksum


def text_cell_nonblank_count(cells: TextCells, byte_count: Optional[int] = None) -> int:
    if byte_count is None:
        byte_count = len(cells)
    count = 0
    for index in range(0, max(byte_count - 1, 0), 2):
        if cells[index] != 0 and cells[index] != ord(" "):
            count += 1
    return count


def text_cell_digest(cells: TextCells, byte_count: Optional[int] = None) -> dict[str, int]:
    if byte_count is None:
        byte_count = len(cells)
    return {
        "bytes": byte_count,
        "checksum": text_cell_checksum(cells, byte_count),
        "nonblank": text_cell_nonblank_count(cells, byte_count),
    }


def text_mode_byte_count(cols: int, rows: int) -> int:
    return cols * rows * VGA_TEXT_CELL_BYTES


def assert_text_screen_geometry(digest: dict[str, object], cols: int, rows: int) -> None:
    assert digest["cols"] == cols
    assert digest["rows"] == rows
    assert digest["bytes"] == text_mode_byte_count(cols, rows)
    assert digest["presented"] == text_mode_byte_count(cols, rows)


def assert_screen_present_content(
    digest: dict[str, object],
    scope: str,
    require_audio_frames: bool = False,
    expected_audio_frames: Optional[int] = None,
) -> None:
    assert digest["scope"] == scope
    assert digest["checksum"] != 0
    assert digest["nonblank"] > 0
    assert digest["full"] == 1
    assert digest["mode_ok"] == 1
    if require_audio_frames:
        assert digest["audio_frames"] > 0
    if expected_audio_frames is not None:
        assert digest["audio_frames"] == expected_audio_frames


def sb16_stereo_byte_count(frames: int) -> int:
    return frames * SB16_STEREO_BYTES_PER_FRAME


def assert_sb16_stereo_frame_bytes(frames: int, byte_count: int) -> None:
    assert byte_count == sb16_stereo_byte_count(frames)
    assert byte_count % SB16_STEREO_BYTES_PER_FRAME == 0


def assert_sb16_stereo_block_accounting(blocks: int, frames: int, byte_count: int, frames_per_block: int) -> None:
    assert frames == blocks * frames_per_block
    assert_sb16_stereo_frame_bytes(frames, byte_count)


def assert_playback_pump_sb16_stereo(pump: dict[str, object], blocks: int, frames_per_block: int) -> None:
    assert pump["blocks"] == blocks
    assert_sb16_stereo_block_accounting(blocks, int(pump["frames"]), int(pump["accepted"]), frames_per_block)


def assert_playback_pump_stop_state(pump: dict[str, object], limit: int, source_end: int, stop: str) -> None:
    assert pump["limit"] == limit
    assert pump["source_end"] == source_end
    assert pump["stop"] == stop


def assert_module_loaded(output: str, name: str) -> dict[str, object]:
    module = parse_module_loaded(output)
    assert module == {"name": name}
    return module


def assert_module_not_loaded(output: str, name: str) -> None:
    assert f"Module: {name}" not in output


def assert_module_size(output: str, size: int) -> dict[str, object]:
    module_size = parse_module_size(output)
    assert module_size == {"size": size}
    return module_size


def assert_module_loader(output: str, loader: str) -> dict[str, object]:
    module_loader = parse_module_loader(output)
    assert module_loader == {"loader": loader}
    return module_loader


def assert_decoder_handoff(output: str, handoff: str) -> dict[str, object]:
    decoder_handoff = parse_decoder_handoff(output)
    assert decoder_handoff == {"handoff": handoff}
    return decoder_handoff


def assert_decoder_handoff_absent(output: str, handoff: str) -> None:
    assert f"Decoder handoff: {handoff}" not in output


def assert_module_type_tag(output: str, tag: str) -> dict[str, object]:
    module_type_tag = parse_module_type_tag(output)
    assert module_type_tag == {"tag": tag}
    return module_type_tag


def assert_module_title(output: str, title: str) -> dict[str, object]:
    module_title = parse_module_title(output)
    assert module_title == {"title": title}
    return module_title


def assert_unsupported_module(output: str, name: str) -> dict[str, object]:
    unsupported = parse_unsupported_module(output)
    assert unsupported == {"name": name}
    return unsupported


def assert_playback_output(output: str, expected_output: str) -> dict[str, object]:
    playback_output = parse_playback_output(output)
    assert playback_output == {"output": expected_output}
    return playback_output


def assert_playback_disabled(output: str, reason: str) -> dict[str, object]:
    playback_disabled = parse_playback_disabled(output)
    assert playback_disabled == {"reason": reason}
    return playback_disabled


def assert_ffi_marker(output: str, marker: str) -> dict[str, object]:
    ffi_marker = parse_ffi_marker(output)
    assert ffi_marker == {"marker": marker}
    return ffi_marker


def assert_orders_channels(output: str, orders: int, channels: int) -> dict[str, object]:
    summary = parse_orders_channels(output)
    assert summary == {"orders": orders, "channels": channels}
    return summary


def assert_help_usage(output: str) -> None:
    assert HELP_USAGE_TEXT in output


def assert_supported_dos_formats(output: str) -> None:
    assert SUPPORTED_DOS_FORMATS_TEXT in output


def assert_sb16_audio_scope(output: str) -> None:
    assert SB16_AUDIO_SCOPE_TEXT in output


def assert_text_backend(output: str) -> None:
    assert TEXT_BACKEND_TEXT in output


def assert_text_backend_memory(output: str) -> None:
    assert TEXT_BACKEND_MEMORY_TEXT in output


def assert_sdl_compatible_audio_backend(output: str) -> None:
    assert SDL_COMPAT_AUDIO_BACKEND_TEXT in output


def assert_playback_loop(
    output: str,
    mode: str,
    policy: str,
    cadence: str,
    max_blocks: int,
    frames_per_block: int,
) -> dict[str, object]:
    loop = parse_playback_loop(output)
    assert loop == {
        "mode": mode,
        "policy": policy,
        "cadence": cadence,
        "max_blocks": max_blocks,
        "frames_per_block": frames_per_block,
    }
    return loop


def assert_decoder_progress(
    output: str,
    block: int,
    total_blocks: int,
    order: int,
    pattern: int,
    row: int,
    channel: int,
    tick: int,
    speed: int,
    tempo: int,
    ended: int,
    loop: int,
) -> dict[str, object]:
    progress = parse_decoder_progress(output)
    assert progress == {
        "block": block,
        "total_blocks": total_blocks,
        "order": order,
        "pattern": pattern,
        "row": row,
        "channel": channel,
        "tick": tick,
        "speed": speed,
        "tempo": tempo,
        "ended": ended,
        "loop": loop,
    }
    return progress


def assert_decoder_progress_block(output: str, block: int, total_blocks: int) -> dict[str, object]:
    progress = parse_decoder_progress(output)
    assert progress["block"] == block
    assert progress["total_blocks"] == total_blocks
    return progress


def assert_decoder_geometry(
    output: str,
    orders: int,
    rows_per_order: int,
    restart: int,
    speed: int,
    tempo: int,
    channels: int,
) -> dict[str, object]:
    geometry = parse_decoder_geometry(output)
    assert geometry == {
        "orders": orders,
        "rows_per_order": rows_per_order,
        "restart": restart,
        "speed": speed,
        "tempo": tempo,
        "channels": channels,
    }
    return geometry


def assert_decoder_event(
    output: str,
    period: int,
    note: int,
    octave: int,
    instrument: int,
    volume: int,
    effect: int,
    param: int,
) -> dict[str, object]:
    expected = {
        "period": period,
        "note": note,
        "octave": octave,
        "instrument": instrument,
        "volume": volume,
        "effect": effect,
        "param": param,
    }
    for event in parse_decoder_events(output):
        if event == expected:
            return event
    raise AssertionError(f"missing Decoder event {expected!r} in output:\n{output}")


def assert_decoder_voice(
    output: str,
    active: int,
    period: int,
    note: int,
    octave: int,
    instrument: int,
    volume: int,
    sample_len: int,
    sample_vol: int,
    loop_start: int,
    loop_len: int,
    data: int,
) -> dict[str, object]:
    expected = {
        "active": active,
        "period": period,
        "note": note,
        "octave": octave,
        "instrument": instrument,
        "volume": volume,
        "sample_len": sample_len,
        "sample_vol": sample_vol,
        "loop_start": loop_start,
        "loop_len": loop_len,
        "data": data,
    }
    for voice in parse_decoder_voices(output):
        if voice == expected:
            return voice
    raise AssertionError(f"missing Decoder voice {expected!r} in output:\n{output}")


def dos_physical_address(segment: int, offset: int = VGA_TEXT_OFFSET) -> int:
    return segment * 16 + offset


def text_memory_slice(memory: bytes, segment: int, cols: int, rows: int, offset: int = VGA_TEXT_OFFSET) -> bytes:
    start = dos_physical_address(segment, offset)
    byte_count = text_mode_byte_count(cols, rows)
    end = start + byte_count
    if end > len(memory):
        raise ValueError(f"text memory range {segment:04x}:{offset:04x}+{byte_count} exceeds dump size {len(memory)}")
    return memory[start:end]


def text_memory_digest(memory: bytes, segment: int, cols: int, rows: int, offset: int = VGA_TEXT_OFFSET) -> dict[str, int]:
    cells = text_memory_slice(memory, segment, cols, rows, offset)
    return text_cell_digest(cells)


def text_cells_visible_text(cells: bytes, cols: int, rows: int) -> str:
    lines: list[str] = []
    for row in range(rows):
        chars: list[str] = []
        for col in range(cols):
            value = cells[(row * cols + col) * VGA_TEXT_CELL_BYTES]
            chars.append(chr(value) if 32 <= value < 127 else " ")
        lines.append("".join(chars).rstrip())
    return "\n".join(lines)


def text_memory_visible_text(memory: bytes, segment: int, cols: int, rows: int, offset: int = VGA_TEXT_OFFSET) -> str:
    return text_cells_visible_text(text_memory_slice(memory, segment, cols, rows, offset), cols, rows)


def text_cells_find_text(cells: bytes, cols: int, rows: int, text: str) -> tuple[int, int]:
    for row in range(rows):
        line = "".join(
            chr(cells[(row * cols + col) * VGA_TEXT_CELL_BYTES])
            if 32 <= cells[(row * cols + col) * VGA_TEXT_CELL_BYTES] < 127
            else " "
            for col in range(cols)
        )
        col = line.find(text)
        if col >= 0:
            return row, col
    raise AssertionError(f"text {text!r} not found in text cells")


def text_cells_span(cells: bytes, cols: int, row: int, col: int, width: int) -> bytes:
    start = (row * cols + col) * VGA_TEXT_CELL_BYTES
    end = start + width * VGA_TEXT_CELL_BYTES
    return cells[start:end]


def assert_text_cell_span_equal(
    original: bytes,
    rewrite: bytes,
    cols: int,
    rows: int,
    text: str,
) -> None:
    original_row, original_col = text_cells_find_text(original, cols, rows, text)
    rewrite_row, rewrite_col = text_cells_find_text(rewrite, cols, rows, text)
    assert (rewrite_row, rewrite_col) == (original_row, original_col), (
        f"text {text!r} original at {(original_row, original_col)} "
        f"rewrite at {(rewrite_row, rewrite_col)}"
    )
    assert text_cells_span(rewrite, cols, rewrite_row, rewrite_col, len(text)) == text_cells_span(
        original,
        cols,
        original_row,
        original_col,
        len(text),
    )


def assert_text_cell_span_at_original_location_equal(
    original: bytes,
    rewrite: bytes,
    cols: int,
    rows: int,
    text: str,
) -> None:
    original_row, original_col = text_cells_find_text(original, cols, rows, text)
    assert text_cells_span(rewrite, cols, original_row, original_col, len(text)) == text_cells_span(
        original,
        cols,
        original_row,
        original_col,
        len(text),
    ), f"text {text!r} differs at original location {(original_row, original_col)}"


def assert_original_visible_row_cells_equal(
    original: bytes,
    rewrite: bytes,
    cols: int,
    row: int,
) -> None:
    for col in range(cols):
        offset = (row * cols + col) * VGA_TEXT_CELL_BYTES
        ch = original[offset]
        if ch == ord(" ") or ch == 0:
            continue
        assert rewrite[offset] == original[offset], f"row={row} col={col} char"
        assert rewrite[offset + 1] == original[offset + 1], f"row={row} col={col} attr"


def assert_no_extra_rewrite_visible_text_on_original_blank_cells(
    original: bytes,
    rewrite: bytes,
    cols: int,
    rows: int,
) -> None:
    extras: list[tuple[int, int, int]] = []
    for row in range(rows):
        for col in range(cols):
            offset = (row * cols + col) * VGA_TEXT_CELL_BYTES
            original_ch = original[offset]
            rewrite_ch = rewrite[offset]
            original_visible = original_ch not in (0, ord(" "))
            rewrite_printable = 32 < rewrite_ch < 127
            if not original_visible and rewrite_printable:
                extras.append((row, col, rewrite_ch))
    assert not extras, "rewrite has visible diagnostics where original is blank: " + repr(extras[:20])


def parse_screen_present_digest(output: str, reason: str) -> dict[str, object]:
    found = None
    for match in SCREEN_PRESENT_RE.finditer(output):
        if match.group("reason") == reason:
            found = {
                "scope": match.group("scope"),
                "bytes": int(match.group("screen_bytes")),
                "checksum": int(match.group("checksum")),
                "nonblank": int(match.group("nonblank")),
                "presented": int(match.group("bytes")),
                "full": int(match.group("full")),
                "cols": int(match.group("cols")),
                "rows": int(match.group("rows")),
                "mode_ok": int(match.group("mode_ok")),
                "audio_frames": int(match.group("audio_frames")),
                "left_level": int(match.group("left_level")),
                "right_level": int(match.group("right_level")),
            }
    if found is not None:
        return found
    raise AssertionError(f"missing Screen present reason={reason!r} in output:\n{output}")


def parse_module_loaded(output: str) -> dict[str, object]:
    match = MODULE_LOADED_RE.search(output)
    if not match:
        raise AssertionError(f"missing Module diagnostic in output:\n{output}")
    return {"name": match.group("name")}


def parse_module_size(output: str) -> dict[str, object]:
    match = MODULE_SIZE_RE.search(output)
    if not match:
        raise AssertionError(f"missing Size diagnostic in output:\n{output}")
    return {"size": int(match.group("size"))}


def parse_module_loader(output: str) -> dict[str, object]:
    match = MODULE_LOADER_RE.search(output)
    if not match:
        raise AssertionError(f"missing Loader diagnostic in output:\n{output}")
    return {"loader": match.group("loader")}


def parse_decoder_handoff(output: str) -> dict[str, object]:
    match = DECODER_HANDOFF_RE.search(output)
    if not match:
        raise AssertionError(f"missing Decoder handoff diagnostic in output:\n{output}")
    return {"handoff": match.group("handoff")}


def parse_module_type_tag(output: str) -> dict[str, object]:
    match = MODULE_TYPE_TAG_RE.search(output)
    if not match:
        raise AssertionError(f"missing Module type tag diagnostic in output:\n{output}")
    return {"tag": match.group("tag")}


def parse_module_title(output: str) -> dict[str, object]:
    match = MODULE_TITLE_RE.search(output)
    if not match:
        raise AssertionError(f"missing Title diagnostic in output:\n{output}")
    return {"title": match.group("title")}


def parse_unsupported_module(output: str) -> dict[str, object]:
    match = UNSUPPORTED_MODULE_RE.search(output)
    if not match:
        raise AssertionError(f"missing unsupported-module diagnostic in output:\n{output}")
    return {"name": match.group("name")}


def parse_playback_output(output: str) -> dict[str, object]:
    match = PLAYBACK_OUTPUT_RE.search(output)
    if not match:
        raise AssertionError(f"missing Playback output diagnostic in output:\n{output}")
    return {"output": match.group("output")}


def parse_playback_disabled(output: str) -> dict[str, object]:
    match = PLAYBACK_DISABLED_RE.search(output)
    if not match:
        raise AssertionError(f"missing Playback disabled diagnostic in output:\n{output}")
    return {"reason": match.group("reason")}


def parse_ffi_marker(output: str) -> dict[str, object]:
    match = FFI_MARKER_RE.search(output)
    if not match:
        raise AssertionError(f"missing FFI marker diagnostic in output:\n{output}")
    return {"marker": match.group("marker")}


def parse_orders_channels(output: str) -> dict[str, object]:
    match = ORDERS_CHANNELS_RE.search(output)
    if not match:
        raise AssertionError(f"missing Orders/Channels diagnostic in output:\n{output}")
    return {
        "orders": int(match.group("orders")),
        "channels": int(match.group("channels")),
    }


def parse_player_hw_text_digest(output: str) -> dict[str, int]:
    match = PLAYER_HW_TEXT_RE.search(output)
    if not match:
        raise AssertionError(f"missing player hardware text digest in output:\n{output}")
    return {
        "copies": int(match.group("text_copies")),
        "segment": int(match.group("segment"), 16),
        "offset": int(match.group("offset"), 16),
        "bytes": int(match.group("bytes")),
        "checksum": int(match.group("checksum")),
        "nonblank": int(match.group("nonblank")),
    }


def parse_player_hw_audio_digest(output: str) -> dict[str, int]:
    match = PLAYER_HW_AUDIO_RE.search(output)
    if not match:
        raise AssertionError(f"missing player hardware audio digest in output:\n{output}")
    return {
        "copies": int(match.group("audio_copies")),
        "bytes": int(match.group("bytes")),
        "checksum": int(match.group("checksum")),
        "first": int(match.group("first"), 16),
        "tail": int(match.group("tail"), 16),
    }


def parse_playback_pump(output: str) -> dict[str, object]:
    match = PLAYBACK_PUMP_RE.search(output)
    if not match:
        raise AssertionError(f"missing Playback pump diagnostic in output:\n{output}")
    return {
        "blocks": int(match.group("blocks")),
        "frames": int(match.group("frames")),
        "accepted": int(match.group("accepted")),
        "checksum": int(match.group("checksum")),
        "limit": int(match.group("limit")),
        "source_end": int(match.group("source_end")),
        "stop": match.group("stop"),
    }


def parse_playback_loop(output: str) -> dict[str, object]:
    match = PLAYBACK_LOOP_RE.search(output)
    if not match:
        raise AssertionError(f"missing Playback loop diagnostic in output:\n{output}")
    return {
        "mode": match.group("mode"),
        "policy": match.group("policy"),
        "cadence": match.group("cadence"),
        "max_blocks": int(match.group("max_blocks")),
        "frames_per_block": int(match.group("frames_per_block")),
    }


def parse_decoder_route(output: str) -> dict[str, object]:
    match = DECODER_ROUTE_RE.search(output)
    if not match:
        raise AssertionError(f"missing Decoder route diagnostic in output:\n{output}")
    return {
        "id": int(match.group("id")),
        "name": match.group("name"),
    }


def parse_decoder_progress(output: str) -> dict[str, object]:
    match = DECODER_PROGRESS_RE.search(output)
    if not match:
        raise AssertionError(f"missing Decoder progress diagnostic in output:\n{output}")
    speed = int(match.group("speed"))
    assert int(match.group("reported_speed")) == speed
    return {
        "block": int(match.group("block")),
        "total_blocks": int(match.group("total_blocks")),
        "order": int(match.group("order")),
        "pattern": int(match.group("pattern")),
        "row": int(match.group("row")),
        "channel": int(match.group("channel")),
        "tick": int(match.group("tick")),
        "speed": speed,
        "tempo": int(match.group("tempo")),
        "ended": int(match.group("ended")),
        "loop": int(match.group("loop")),
    }


def parse_decoder_geometry(output: str) -> dict[str, object]:
    match = DECODER_GEOMETRY_RE.search(output)
    if not match:
        raise AssertionError(f"missing Decoder geometry diagnostic in output:\n{output}")
    return {
        "orders": int(match.group("orders")),
        "rows_per_order": int(match.group("rows_per_order")),
        "restart": int(match.group("restart")),
        "speed": int(match.group("speed")),
        "tempo": int(match.group("tempo")),
        "channels": int(match.group("channels")),
    }


def parse_decoder_events(output: str) -> list[dict[str, object]]:
    events = []
    for match in DECODER_EVENT_RE.finditer(output):
        events.append({
            "period": int(match.group("period")),
            "note": int(match.group("note")),
            "octave": int(match.group("octave")),
            "instrument": int(match.group("instrument")),
            "volume": int(match.group("volume")),
            "effect": int(match.group("effect")),
            "param": int(match.group("param")),
        })
    if not events:
        raise AssertionError(f"missing Decoder event diagnostic in output:\n{output}")
    return events


def parse_decoder_voices(output: str) -> list[dict[str, object]]:
    voices = []
    for match in DECODER_VOICE_RE.finditer(output):
        voices.append({
            "active": int(match.group("active")),
            "period": int(match.group("period")),
            "note": int(match.group("note")),
            "octave": int(match.group("octave")),
            "instrument": int(match.group("instrument")),
            "volume": int(match.group("volume")),
            "sample_len": int(match.group("sample_len")),
            "sample_vol": int(match.group("sample_vol")),
            "loop_start": int(match.group("loop_start")),
            "loop_len": int(match.group("loop_len")),
            "data": int(match.group("data")),
        })
    if not voices:
        raise AssertionError(f"missing Decoder voice diagnostic in output:\n{output}")
    return voices


def parse_pcm_source(output: str) -> dict[str, object]:
    match = PCM_SOURCE_RE.search(output)
    if not match:
        raise AssertionError(f"missing PCM source diagnostic in output:\n{output}")
    return {
        "source": match.group("source"),
        "seed": int(match.group("seed")),
        "truncated": int(match.group("truncated")),
        "input": match.group("input"),
        "renderer": match.group("renderer"),
        "route": int(match.group("route")),
        "provider": match.group("provider"),
        "hook_provider": match.group("hook_provider"),
        "stream_start": int(match.group("stream_start")),
    }


def assert_decoder_route(output: str, route_id: int, name: str) -> dict[str, object]:
    route = parse_decoder_route(output)
    assert route == {"id": route_id, "name": name}
    return route


def assert_decoder_route_absent(output: str, route_id: int, name: str) -> None:
    assert f"Decoder route: id={route_id} name={name}" not in output


def assert_pcm_source_route(
    output: str,
    route_id: int,
    renderer: str,
    provider: str,
    source: Optional[str] = None,
    truncated: Optional[int] = None,
    input_kind: Optional[str] = None,
    hook_provider: Optional[str] = None,
    stream_start: Optional[int] = None,
) -> dict[str, object]:
    pcm = parse_pcm_source(output)
    assert pcm["route"] == route_id
    assert pcm["renderer"] == renderer
    assert pcm["provider"] == provider
    if source is not None:
        assert pcm["source"] == source
    if truncated is not None:
        assert pcm["truncated"] == truncated
    if input_kind is not None:
        assert pcm["input"] == input_kind
    if hook_provider is not None:
        assert pcm["hook_provider"] == hook_provider
    if stream_start is not None:
        assert pcm["stream_start"] == stream_start
    return pcm
    return pcm


def assert_text_memory_matches_screen_present(
    memory: bytes,
    output: str,
    reason: str,
    segment: int,
    cols: int,
    rows: int,
    offset: int = VGA_TEXT_OFFSET,
    expected_scope: str = "full-screen",
) -> None:
    original = text_memory_digest(memory, segment, cols, rows, offset)
    rewrite = parse_screen_present_digest(output, reason)
    expected = {
        "bytes": original["bytes"],
        "checksum": original["checksum"],
        "nonblank": original["nonblank"],
        "scope": expected_scope,
        "cols": cols,
        "rows": rows,
    }
    actual = {
        "bytes": rewrite["bytes"],
        "checksum": rewrite["checksum"],
        "nonblank": rewrite["nonblank"],
        "scope": rewrite["scope"],
        "cols": rewrite["cols"],
        "rows": rewrite["rows"],
    }
    assert actual == expected


def assert_text_memory_matches_player_hw_text(
    memory: bytes,
    output: str,
    segment: int,
    cols: int,
    rows: int,
    offset: int = VGA_TEXT_OFFSET,
) -> None:
    original = text_memory_digest(memory, segment, cols, rows, offset)
    rewrite = parse_player_hw_text_digest(output)
    expected = {
        "segment": segment,
        "offset": offset,
        "bytes": original["bytes"],
        "checksum": original["checksum"],
        "nonblank": original["nonblank"],
    }
    actual = {
        "segment": rewrite["segment"],
        "offset": rewrite["offset"],
        "bytes": rewrite["bytes"],
        "checksum": rewrite["checksum"],
        "nonblank": rewrite["nonblank"],
    }
    assert actual == expected


def write_endcont_module(directory: Path) -> None:
    directory.mkdir(parents=True, exist_ok=True)

    s3m = bytearray(0x80)
    s3m[:8] = b"ENDCONT"
    s3m[0x20:0x22] = (0).to_bytes(2, "little")
    s3m[0x22:0x24] = (0).to_bytes(2, "little")
    s3m[0x24:0x26] = (0).to_bytes(2, "little")
    s3m[0x2C:0x30] = b"SCRM"
    s3m[0x31] = 1
    s3m[0x32] = 125
    s3m[0x40] = 0
    s3m[0x41:0x60] = bytes([0xFF]) * 31
    (directory / "ENDCONT.S3M").write_bytes(s3m)


def write_smoke_modules(directory: Path) -> None:
    directory.mkdir(parents=True, exist_ok=True)

    s3m = bytearray(99)
    s3m[:9] = b"SMOKE S3M"
    s3m[0x20:0x22] = (3).to_bytes(2, "little")
    s3m[0x22:0x24] = (2).to_bytes(2, "little")
    s3m[0x24:0x26] = (1).to_bytes(2, "little")
    s3m[0x2C:0x30] = b"SCRM"
    s3m[0x60:0x63] = bytes([0, 1, 2])
    (directory / "SMOKE.S3M").write_bytes(s3m)
    (directory / "SMOKE.XYZ").write_bytes(s3m)

    mod = bytearray(1084 + 5 * 1024 + 10)
    mod[:9] = b"SMOKE MOD"
    mod[42:44] = (2).to_bytes(2, "big")
    mod[45] = 127
    mod[48:50] = (1).to_bytes(2, "big")
    mod[72:74] = (3).to_bytes(2, "big")
    mod[75] = 32
    mod[76:78] = (4).to_bytes(2, "big")
    mod[78:80] = (4).to_bytes(2, "big")
    mod[950] = 5
    mod[952:957] = bytes([0, 1, 2, 4, 0xFE])
    mod[1080:1084] = b"M.K."
    mod[1084:1088] = bytes([0x03, 0x57, 0x1C, 0x7F])
    mod[1088:1092] = bytes([0x03, 0x28, 0x0F, 0x07])
    mod[1092:1096] = bytes([0x03, 0x28, 0x2C, 0x00])
    mod[1096:1100] = bytes([0x32, 0xFA, 0x0C, 0x40])
    sample_base = 1084 + 5 * 1024
    mod[sample_base:sample_base + 4] = bytes([5, 6, 7, 8])
    mod[sample_base + 4:sample_base + 10] = bytes([9, 10, 11, 12, 13, 14])
    (directory / "SMOKE.MOD").write_bytes(mod)
    nst = bytearray(mod)
    nst[:9] = b"SMOKE NST"
    (directory / "SMOKE.NST").write_bytes(nst)

    badord = bytearray(mod)
    badord[:10] = b"BADORD MOD"
    badord[950] = 129
    badord[952] = 0xFE
    (directory / "BADORD.MOD").write_bytes(badord)

    fastrow = bytearray(mod)
    fastrow[:11] = b"FASTROW MOD"
    fastrow[950] = 3
    fastrow[952:955] = bytes([0, 0, 0])
    fastrow[1088:1092] = bytes([0x03, 0x28, 0x0F, 0x01])
    fastrow[1100:1104] = bytes([0x02, 0xFA, 0x1C, 0x20])
    fastrow[1104:1108] = bytes([0x00, 0x00, 0x0E, 0x60])
    fastrow[1116:1120] = bytes([0x00, 0x00, 0x0E, 0x62])
    fastrow[1136:1140] = bytes([0x00, 0x00, 0x0D, 0x03])
    fastrow[1140:1144] = bytes([0x00, 0x00, 0x0B, 0x02])
    (directory / "FASTROW.MOD").write_bytes(fastrow)

    mtm = bytearray(0x42)
    mtm[0:3] = b"MTM"
    mtm[4:13] = b"SMOKE MTM"
    mtm[0x1A:0x1C] = (12).to_bytes(2, "little")
    mtm[0x1C] = 2
    mtm[0x1E] = 4
    mtm[0x20] = 6
    mtm[0x22:0x27] = bytes([0, 1, 2, 3, 4])
    (directory / "SMOKE.MTM").write_bytes(mtm)

    far = bytearray(0x80)
    far[0:4] = b"FAR\xfe"
    far[4:13] = b"SMOKE FAR"
    (directory / "SMOKE.FAR").write_bytes(far)

    e669 = bytearray(0x80)
    e669[0:2] = b"if"
    e669[2:11] = b"SMOKE669"
    e669[0x6E] = 4
    e669[0x6F] = 3
    e669[0x70] = 1
    (directory / "SMOKE.669").write_bytes(e669)

    ult = bytearray(96)
    ult[:15] = b"MAS_UTrack_V001"
    ult[15:24] = b"SMOKE ULT"
    (directory / "SMOKE.ULT").write_bytes(ult)

    for ext in ["WOW", "OKT", "OCT", "PTM", "AMS", "DBM", "DMF", "MDL", "DSM", "MED", "IMF", "J2B"]:
        generic = bytearray(128)
        generic[:9] = f"SMOKE {ext}".encode("ascii")[:9].ljust(9, b" ")
        (directory / f"SMOKE.{ext}").write_bytes(generic)

    xm = bytearray(128)
    xm[:17] = b"Extended Module: "
    xm[17:26] = b"SMOKE XM "
    (directory / "SMOKE.XM").write_bytes(xm)

    it = bytearray(128)
    it[:4] = b"IMPM"
    it[4:13] = b"SMOKE IT "
    (directory / "SMOKE.IT").write_bytes(it)

    psm = bytearray(0x80)
    psm[0:4] = b"PSM "
    psm[4:13] = b"SMOKE PSM"
    (directory / "SMOKE.PSM").write_bytes(psm)

    inr = bytearray(0x80)
    inr[0:4] = b"IMPM"
    inr[4:13] = b"SMOKE INR"
    (directory / "SMOKE.INR").write_bytes(inr)

    stm = bytearray(64)
    stm[:9] = b"SMOKE STM"
    stm[20:28] = b"!Scream!"
    (directory / "SMOKE.STM").write_bytes(stm)

    unsupported = directory / "BAD.XYZ"
    unsupported.write_bytes(b"bad")
