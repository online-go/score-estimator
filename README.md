# About

This repository contains the source code used for the score estimator and dead
stone removal suggester on online-go.com.

This is designed to be a score estimator with a small memory and code size foot
print and fast single threaded execution speeds. While the source code is in
C++, we compile it to JavaScript using
[Emscripten](https://github.com/kripken/emscripten) for client side use within
the browser.


## Seki

Different GO rules treat territory in seki differently. The output of this
program will attempt to mark shared territory spots as being dead, but will
specifically attempt to not mark any territory in seki as being dead, and
instead leave that to a scoring ruleset to determine as being counted or
not.

That is to say
![](https://senseis.xmp.net/diagrams/30/1d10677a69ac35bb0021701f2e1c02a7.png)
points a and b will *not* be marked dead, but the two shared territory spaces
will be marked as dead.
