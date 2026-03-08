# What is Ptilopsis Granti
> The Southern white-faced owl (Ptilopsis granti) is a fairly small owl in the family Strigidae. It is native to the southern half of Africa. It was formerly regarded as a subspecies of the northern white-faced owl (P. leucopsis) but the two are now commonly treated as separate species. 

> Their call is a series of fast, bubbling hoots, uttered at night and frequently repeated. These fast, staccato notes followed by a longer and higher-pitched 'hoot' are extensively used during breeding season and pairs of owls often sing together.

[Source](https://en.wikipedia.org/wiki/Southern_white-faced_owl)

A simple Pomodoro timer using
- [raylib](https://www.raylib.com/)
- [raygui](https://github.com/raysan5/raygui)

## Building
```bash
just build
# or 
gcc main.c \
    -L {{ray_lib_path}}/lib \
    -lraylib -lm -lpthread -ldl -lrt \
    -o main
```
Note: Since raygui is meant to be used as a single header file(STB style library),
we only need to link raylib and then in our `main.c`
```c
// Define RAYGUI_IMPLEMENTATION macro
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
...
```
