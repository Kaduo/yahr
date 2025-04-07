
cflags := ""

CC := "zig cc"
CC_ARGS := "-I/usr/include/libsocket -Isrc/lib -Isrc/expe/lib src/expe/lib/*.c src/lib/*.c -Isrc/external -lraylib -lsocket -lGL -lm -lpthread -ldl -lrt -lX11 -Wall -std=c23" + " " + cflags
BUILD_DIR := "build"

build:
    {{CC}} {{CC_ARGS}} src/main.c -o {{BUILD_DIR}}/haptic_rods

pull:
    git pull

run:
    {{BUILD_DIR}}/haptic_rods

push:
    git commit -am "This message wasn't typed by a human."
    git push

pbr: pull build run

build-gui:
    {{CC}} {{CC_ARGS}} src/hapticgui.c -o {{BUILD_DIR}}/haptic_gui

# build-pretest:
#     {{CC}} {{CC_ARGS}} src/pretest.c -o {{BUILD_DIR}}/pretest

build-pretest:
    {{CC}} {{CC_ARGS}} src/pretest/pretest-client.c -o {{BUILD_DIR}}/pretest_client
    {{CC}} {{CC_ARGS}} src/pretest/pretest-server.c -o {{BUILD_DIR}}/pretest_server
    {{CC}} {{CC_ARGS}} src/pretest/pretest-generator.c -o {{BUILD_DIR}}/pretest_generator

pb: pull build-pretest

pbs: pb
    build/pretest_server

pbc: pb
    build/pretest_client

build-expe:
    {{CC}} {{CC_ARGS}} src/expe/expe_test.c -o {{BUILD_DIR}}/expe_test
    {{CC}} {{CC_ARGS}} src/expe/expe_client.c -o {{BUILD_DIR}}/expe_client
    {{CC}} {{CC_ARGS}} src/expe/expe_server.c -o {{BUILD_DIR}}/expe_server
    {{CC}} {{CC_ARGS}} src/expe/expe_generator.c -o {{BUILD_DIR}}/expe_generator