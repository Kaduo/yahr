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
    gcc -Iexternaltemp -I/usr/include/libsocket -Isrc externaltemp/*.c src/*.c -lraylib -lsocket -lGL -lm -lpthread -ldl -lrt -lX11  -o haptic_gui -Wall