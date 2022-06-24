# Playdate-C++

*for SDK version 1.12.0*

You want to use C++ on the playdate! Great, except you keep getting linker errors about exception handling, and your global variables aren't getting initialized properly, and `__attribute__((constructor))` isn't helping, and...

## The Easy Solution

**This works for Make (not cmake).**

Copy the contents of `buildsupport` in this repository to the `C_API/buildsupport` folder in your Playdate SDK install, add `pdnewlib.c` as asource file in your project, and call `eventHandler_pdnewlib` first thing in your `eventHandler`.

TODO: cmake version.

## This doesn't work for me

The base principles of this are explained on this [forum thread](https://devforum.play.date/t/cpp-guide-c-on-playdate/5085). Good luck.

## This doesn't work at all

Yikes!?‽!? Please submit a bug report to NaOH#1432 on discord, or if you're able, a please submit pull request with a fix :) :)