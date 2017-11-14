# rapeizefm
simple mp3 player

O rapeizefm é um tocador de mp3 simples e funcional.

Compile usando:
gcc player.c -o player `pkg-config --cflags --libs gtk+-2.0` -lSDL2 -lSDL2_mixer -lid3v2

Adicionais:
Você precisará do SDL2, do GTK+2.0 e da id3v2lib.
