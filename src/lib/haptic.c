#include "haptic.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <asm-generic/termbits-common.h>

#include "raymath.h"

// Low-level stuff

typedef enum HapticCommand {
    HapticCommand_PING = 1,
    HapticCommand_SET_DIR = 130,
    HapticCommand_ADD_SIGNAL = 131,
    HapticCommand_PLAY = 132,
    HapticCommand_CLEAR = 133
} HapticCommand;

#define TERMINAL "/dev/ttyUSB0"

int SetDriverAttributes(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
         printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
         printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void SetMincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
         printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
         printf("Error tcsetattr: %s\n", strerror(errno));
}

int ConnectToTTY() {
    char *portname = TERMINAL;

    int fd;
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
         printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    /*baudrate 1000000, 8 bits, no parity, 1 stop bit */
    SetDriverAttributes(fd, B1000000);

    return fd;
}

void write_to_tty(int fd, unsigned char *buffer, int buffer_len) {
    int wlen = write(fd, buffer, buffer_len);
    if (wlen != buffer_len) {
        //  printf("Error from write: %d, %d\n", wlen, errno);
    }
    tcdrain(fd);    /* delay for output */
}

void AddSignal(int fd, Signal signal) {
    #define ADD_BUFFER_LEN  11
    unsigned char buffer[ADD_BUFFER_LEN] = {
        (unsigned char)HapticCommand_ADD_SIGNAL,
        (unsigned char)signal.angle,
        (unsigned char)signal.pulses,
        (unsigned char)signal.signalShape,
        (unsigned char)signal.amplitude,
        (unsigned char)signal.offset,
        (unsigned char)signal.duty,
        (unsigned char)signal.period,
        (unsigned char)(signal.period >> 8),
        (unsigned char)signal.phase,
        (unsigned char)(signal.phase >> 8),
    };
    write_to_tty(fd, buffer, ADD_BUFFER_LEN);
}


void PlayCurrentSignal(int fd, int play) {
    #define PLAY_BUFFER_LEN 2
    unsigned char buffer[PLAY_BUFFER_LEN] = {HapticCommand_PLAY, play};
    write_to_tty(fd, buffer, PLAY_BUFFER_LEN);
}


void ClearSignal(int fd) {
    #define CLEAR_BUFFER_LEN 1
    unsigned char buffer[CLEAR_BUFFER_LEN] = {HapticCommand_CLEAR};
    write_to_tty(fd, buffer, CLEAR_BUFFER_LEN);
    PlayCurrentSignal(fd, 0);
}

void Ping(int fd) {
    #define PING_BUFFER_LEN 1
    unsigned char buffer[PING_BUFFER_LEN] = {HapticCommand_PING};
    write_to_tty(fd, buffer, PING_BUFFER_LEN);
}

void SetSignal(int fd, Signal signal) {
    printf("lowlevel\n");
    PrintSignal(signal);
    ClearSignal(fd);
    AddSignal(fd, signal);
    PlayCurrentSignal(fd, 1);
}

void SetDirection(int fd, uint8_t angle, uint16_t speed) {
    #define DIR_BUFFER_LEN 4
    unsigned char buffer[DIR_BUFFER_LEN] = {HapticCommand_SET_DIR, angle, speed, speed >> 8};
    write_to_tty(fd, buffer, DIR_BUFFER_LEN);
}

void PrintSignal(Signal sig) {
     printf("signal shape : %d, amplitude : %d, offset : %d, duty : %d, period: %d, phase: %d\n", sig.signalShape, sig.amplitude, sig.offset, sig.duty, sig.period, sig.phase);
}



// High-level Driver

void HapticDriver_update(HapticDriver *me, float frameTime) {
    Vector2 mouseDelta = me->inputService->getMouseDelta(me->inputService);
    if (frameTime > 0) {
        float floatSpeed = Vector2Length(mouseDelta)/frameTime;
        uint16_t intSpeed;
        if (floatSpeed > 65535) {
            intSpeed = 65535;
        } else {
            intSpeed = (uint16_t)floatSpeed;
        }

        float floatAngle = Vector2Angle((Vector2){.x=1, .y=0}, mouseDelta); // Between -pi and pi
        uint8_t intAngle = (uint8_t)((floatAngle + PI)*127/(2*PI)); // normalize so that it falls between 0 and 127

        if (floatSpeed > EPSILON) {
            me->setDirection(me, intAngle, intSpeed);
        } else {
            me->setDirection(me, 0, 0);
        }
    }
};

void PhysicalHapticDriver_setSignal(HapticDriver *_me, Signal signal) {
    printf("uh!\n");
    PhysicalHapticDriver *me = (PhysicalHapticDriver*)_me;
    SetSignal(me->fd, signal);
}

void PhysicalHapticDriver_clearSignal(HapticDriver *_me) {
    PhysicalHapticDriver *me = (PhysicalHapticDriver*)_me;
    ClearSignal(me->fd);
}

void PhysicalHapticDriver_setDirection(HapticDriver *_me, uint8_t angle, uint16_t speed) {
    PhysicalHapticDriver *me = (PhysicalHapticDriver*)_me;
    SetDirection(me->fd, angle, speed);
}

void PhysicalHapticDriver_Construct(PhysicalHapticDriver *me) {
    me->super.setSignal = PhysicalHapticDriver_setSignal;
    me->super.clearSignal = PhysicalHapticDriver_clearSignal;
    me->super.setDirection = PhysicalHapticDriver_setDirection;
}

PhysicalHapticDriver NewPhysicalHapticDriver(InputService *inputService) {
    int fd = ConnectToTTY();
    HapticDriver super = {.inputService = inputService};
    PhysicalHapticDriver physicalHapticDriver = {
        .super = super,
        .fd = fd
    };
    PhysicalHapticDriver_Construct(&physicalHapticDriver);

    SetDirection(fd, -1, 100);
    return physicalHapticDriver;
}


// High-level service

void HapticService_playQueue(HapticService *me) {
    me->shouldPlay = true;
}

void HapticService_pauseQueue(HapticService *me) {
    me->shouldPlay = false;
    me->currentlyPlaying = false;
}


void HapticService_clearAndPauseQueue(HapticService *me) {
    ClearBuffer(&me->signalQueue);
    HapticService_pauseQueue(me);
}

void HapticService_addSignalToQueue(HapticService *me, Signal signal) {
    AppendToBuffer(&me->signalQueue, (void*)&signal);
}

void HapticService_playSignal(HapticService *me, Signal signal) {
    // PrintSignal(signal);
    HapticService_clearAndPauseQueue(me);
    HapticService_addSignalToQueue(me, signal);
    HapticService_playQueue(me);
}

void HapticService_tryPlayingFirstInQueue(HapticService *me) {
    if (me->signalQueue.size > 0) {
        Signal sig = *(Signal*)ReadBuffer(&me->signalQueue, 0);
        me->driver->setSignal(me->driver, sig);
        me->duration = sig.duration;
        me->currentlyPlaying = true;
    } else {
        me->duration = 0;
        me->currentlyPlaying = false;
    }
}

void HapticService_update(HapticService *me, float frameTime) {
    me->currentlyPlaying = me->currentlyPlaying && me->shouldPlay;
    if (me->currentlyPlaying) {
        if (me->duration > 0) {
            me->duration -= frameTime;
            if (me->duration <= 0) {
                PopBuffer(&me->signalQueue);
                HapticService_tryPlayingFirstInQueue(me);
            }
        }
    } else if (me->shouldPlay) {
        HapticService_tryPlayingFirstInQueue(me);
    } else {
        me->driver->clearSignal(me->driver);
    }
    HapticDriver_update(me->driver, frameTime);
}

HapticService NewHapticService(HapticDriver *driver) {
    return (HapticService){
        .currentlyPlaying = false,
        .shouldPlay = false,
        .driver = driver,
        .duration = 0,
        .signalQueue = EmptyRingBuffer(5, sizeof(Signal)),
    };
}

void MockHapticDriver_setSignal(HapticDriver *me, Signal signal) {
}
void MockHapticDriver_clearSignal(HapticDriver *me) {
}
