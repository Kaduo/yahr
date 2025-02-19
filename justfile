build:
    gcc -I/usr/include/libsocket src/*.c -lraylib -lsocket -lGL -lm -lpthread -ldl -lrt -lX11  -o haptic_rods -Wall

pull:
    git pull

run:
    ./haptic_rods

push:
    git commit -am "This message wasn't typed by a human."
    git push

pbr: pull build run

build-gui:
    zig cc -I/usr/include/libsocket -Isrc/external src/hapticgui.c src/lib/*.c -lraylib -lsocket -lGL -lm -lpthread -ldl -lrt -lX11  -o build/haptic_gui -Wall