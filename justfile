build:
    gcc -I/usr/include/libsocket src/*.c -lraylib -lsocket -lGL -lm -lpthread -ldl -lrt -lX11  -o haptic_rods -Wall

update:
    git pull

run:
    ./haptic_rods