# dcc Whetstone — findings & KWIPS (parked)

Measured 2026-08-09. Parked on branch `park/whetstone-dcc-kwips`.

## Can dcc run Whetstone?

Yes, with two mechanical source adaptations (both captured in
`tests/whetstonf.c`, a converted copy of `tests/whetston.c`):

1. **`double` → `float`.** dcc has a single 32-bit `float` floating type and
   rejects the `double` keyword outright (`DCC-E1201: double is not supported
   by dcc's CP/M/Z80 target; use float`). The stock `tests/whetston.c` uses
   `double` and therefore does **not** build under dcc as-is. The conversion is
   semantically free — the target has only one FP type anyway.
2. **`%e` → `%f`.** dcc's `printf` does not implement the `%e` (scientific)
   conversion; POUT's `%12.4e` prints a literal `e`. Switching to `%f` renders
   the values.

Build with `DCC_FLOATIO=1` so the float `printf` path is linked.

The converted benchmark runs and the per-module check values match the
canonical Whetstone reference (module 1 `1.0000/-1.0000`, trig ≈ `0.4940`,
sqrt/exp/log ≈ `0.835`), confirming the computation is valid, not degenerate.
The transcendental libm (`sin/cos/atan/sqrt/exp/log` → the `...f` runtime
routines) and 32-bit float arithmetic all work.

## KWIPS

`KWIPS = 100 × LOOP × FREQ / cycles`, with `LOOP = 10`, `FREQ = 4 000 000`
(4 MHz) → `KWIPS = 4e9 / cycles`.

| Measurement                     | Z80 cycles   | KWIPS @ 4 MHz |
|---------------------------------|-------------:|--------------:|
| dcc, compute-only (empty POUT)  | 426 960 432  | **9.37**      |
| dcc, full (incl. `%f` printf)   | 428 705 067  | 9.33          |

printf I/O is ~1.7 M cycles (0.4 %), so compute-only ≈ full.

Cross-reference (measured earlier this session, different lane/emulator):
llvmz80 = 271 231 890 cycles → **14.75 KWIPS**. So dcc is ~1.6× slower on
Whetstone — expected, since dcc uses its own soft-float while llvmz80 reuses
z88dk's optimized math32.

## How it was measured — and an emulator gotcha

Measured with **`ntvcm -p`** (`ntvcm/ntvcm -p build/WHETSTONF.COM`), whose
performance report gives the Z80 cycle count at exit. ntvcm terminates this
program correctly in ~0.19 s wall.

**Do not use `runticks.sh` (z88dk-ticks) for this program.** It did not
terminate — the dcc float build's exit path is not caught by ticks' `-end 0`
(PC never lands exactly on `0x0000`), so it burns T-states up to the ceiling
(observed hitting both the 4e9 default and a 40e9 raised ceiling). The llvmz80
number above came from z88dk-ticks measuring a bracketed `TIMER_START` /
`TIMER_STOP` region, so the cross-comparison is **indicative, not
same-emulator certified** (the methodology delta — CRT startup — is <1 %).

### Reproduce
```sh
cd dcc
# native tools m80c/l80c must exist (build once if missing):
#   clang -std=c89 -O2 -o m80c src/m80c/m80c.c
#   clang -std=c89 -O2 -o l80c src/l80c/*.c
PATH="$PWD:$PATH" DCC_FLOATIO=1 ./ma.sh whetstonf fast
../ntvcm/ntvcm -p build/WHETSTONF.COM      # read "Z80 cycles" from the report
```

## Follow-ups (not done)

- Same-emulator head-to-head (run the llvmz80 whetstone under `ntvcm -p` too)
  for a certified comparison.
- A permanent multi-compiler KWIPS lane (cf. z88dk issue ravn/z88dk#56).
- dcc `%e` printf support and/or accepting `double` as an alias for `float`
  would let the stock `tests/whetston.c` build unmodified.
