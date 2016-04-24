# About

This repository contains the source code used for the score estimator and dead
stone removal suggester on online-go.com.

This is designed to be a score estimator with a small memory and code size foot
print and fast single threaded execution speeds. While the source code is in
C++, we compile it to JavaScript using
[Emscripten](https://github.com/kripken/emscripten) for client side use within
the browser.


# Developing

For development and debugging, the c++ code can be compiled into a simple
executable that loads and scores games provided on the command line.

Simply running
```
make
```

will compile this development executable under most platforms with `g++` and some `make` implementation. `clang++` can also be used, as well as probably most other c++ compilers.

Once built, running

```
./estimator test_games/1776378.game
```

should produce something similar to the following output:

```
test_games/1776378.game
height: 19
width: 19
player to move: -1
o     o o o _
o   o   o _   _ _
  o o o o _ _ _ o       _       _ _ _
o o _ _ _ o o o o _ _ _ _ o _ _ o o _
_ _ _ o o _ _   _ _ o o o _   _ o o o
_ o o   o o _ _ _ o _ _ o _   _ o
_ _ o   o   o _ o o o   o _ _ _ _ o
_ _ o       o _ _ o   o o _ o o o   o
_ _ o   o o _ _ o o o _ o _ _ _ o o o
_ o o o o _ _ _ _ o   _ o o o o _ o _
_ _ _ o _ _ o o o _ _ _ _ _ o   _ _ _
o   _ _ o o o _ _ _ _ _ o o _ _ _   _
o _ _ o   o o o _ o _     o o _   _ _
o   _ o o o _ _ o o _ _   o _ _ _ o _
_ _ _ _ _ o       o o o _ _ o o _ o o
_ o o o _ o _ o o   o _   _ _ o o o o
o   o   o _ o   o o _   _ _ o o o o
  o o   o     _ o o _ _   _ o o o   o
o               o _ _     _ _ o o   o


o o o o o o _ _ _ _ _ _ _ _ _ _ _ _ _
o o o o o _ _ _ _ _ _ _ _ _ _ _ _ _ _
o o o o o _ _ _ _ _ _ _ _ _ _ _ _ _ _
o o _ _ _ _ _ _ _ _ _ _ _ _ _ _ o o _
_ _ _ o o _ _ _ _ _ o o o _ _ _ o o o
_ o o o o o _ _ _ o o o o _ _ _ o o o
_ _ o o o o o _ o o o o o _ _ _ _ o o
_ _ o o o o o _ _ o o o o _ o o o o o
_ _ o o o o _ _ o o o _ o _ _ _ o o o
_ o o o o _ _ _ _ o   _ o o o o _ o _
_ _ _ o _ _ o o o _ _ _ _ _ o   _ _ _
_ _ _ _ o o o _ _ _ _ _ _ _ _ _ _ _ _
_ _ _ o o o o o _ o _ _ _ _ _ _ _ _ _
_ _ _ o o o o o o o _ _ _ _ _ _ _ o _
_ _ _ _ _ o o o o o o o _ _ o o _ o o
_ o o o _ o o o o o o _ _ _ _ o o o o
o o o o o o o o o o _ _ _ _ o o o o o
o o o o o o o o o o _ _ _ _ o o o o o
o o o o o o o o o _ _ _ _ _ _ o o o o


Score: -7.500000
Time elapsed: 498.653250 ms
```

with `o` being black, `_` being white, showing how the estimator decided to
fill and assign territory.



