# Playdate-C++

You want to use C++ on the playdate! That's basically just C with extra steps right? Great, great, ok, except you keep getting linker errors about exception handling, and your global variables aren't getting initialized properly, and `__attribute__((constructor))` isn't helping, and...

## The Easy Solution

**I sure hope you're using Make and version 1.10.0 of the Playdate SDK!** Copy the contents of `buildsupport` in this repository to the `C_API/buildsupport` folder in your Playdate SDK install, add `pdnewlib.c` as asource file in your project, and call `eventHandler_pdnewlib` first thing in your `eventHandler`.

## This doesn't work for me

Oh so you're on Windows or something? Or you already modified the SDK too much and can't just paste over it all? OK, [read this forum thread for advice, I guess](https://devforum.play.date/t/cpp-guide-c-on-playdate/5085). Good luck, friend.

## This doesn't work at all

Yikes!?‽!? Please submit a bug report to NaOH#1432 on discord, or if you're able, a pull request with a fix :) :)

*TODO: write a better readme.*
