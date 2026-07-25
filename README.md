# Inertia Player

A clean-room modernization of Inertia Player V1.22, preserving the original DOS program and reverse-engineering references alongside the modern SDL2/notcurses/libmikmod player and behavioral tests.

## Layout

- `original/`: original executable, disassembly, listing, and translated reference output.
- `rewrite/`: DOS-compatible rewrite, modern host player, SDL/notcurses presentation, SB16-compatible audio seam, and build scripts.
- `tests/`: behavioral, ABI, rendering, audio, and original-vs-rewrite regression tests.
- `samples/`: tracker modules used by integration tests and manual playback.

## Host player

Dependencies: C/C++17 compiler, SDL2, SDL2_image, notcurses, libmikmod, libmodplug, libpng, Python 3, and pytest.

```bash
./iplay.sh samples/aryx.s3m
./iplay.sh
```

Running without a module opens the interactive file selector.

## Tests

```bash
pytest -q
```

Some DOS/original-binary tests additionally require OpenWatcom, kvikdos, and mzretools. Paths can be configured in the existing test/build environment variables documented by the scripts.
