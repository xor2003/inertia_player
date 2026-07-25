# IPLAY parity runner contract

`test_function_parity.py` always executes the original DOS `IPLAY.EXE` through `kvikdos`.

The translated/rewrite side is optional and selected with:

```sh
IPLAY_TRANSLATED_RUNNER=/path/to/runner ./run_function_tests.sh
```

The runner is a black-box executable contract. It does not have to use masm2c internals.
A future C/C++ rewrite can satisfy the tests by accepting the same command names and printing the same fields.

Required output format:

- register fields use lowercase hex, e.g. `ax=1234 bx=abcd`;
- byte buffers are printed as lowercase hex after `data=`;
- extra fields are allowed, but tested fields must be present.

Current command families:

- formatting: `hex16`, `hex8`, `hex4`, `decimal`, `putdigit`;
- strings: `strlen`, `strcpy`, `copyprint`;
- far/public state APIs: `getplaysettings`, `getsetplaystate`, `get12f7c`;
- channel helpers: `sub13177`, `sub131da`, `sub131ef`;
- MIDI helpers: `midi154da`, `midi154de`;
- interpolation setup: `interppatch`.

The original binary remains the oracle for expected behavior.
