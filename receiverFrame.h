#ifndef _RECEIVERFRAME_
#define _RECEIVERFRAME_

#include "dcomm.h"
#include <stdio.h>
#include "crc16.h"

class ReceiverFrame {
private:
    char ack;
    char frameNumber;
    bool error;

public:
    ReceiverFrame(char frameNumber) {
        this->frameNumber = frameNumber;
        this->error = false;
    }

    ReceiverFrame(char * frame) {
        this->error = false;
        //// SETTING ACK
        this->setAck(frame[0]);

        ////SETTING FRAME NUMBER
        this->setFrameNumber(frame[1]);

        if (!this->error) {
            char checksum[6];
            for (int i = 1 + 1 /* after ETX */; i < 1 + 1 + 4; i++) {
                checksum[i - 2] = frame[i];
            }
            char * framex = this->toBytes();
            char trueChecksum[5];
            for (int i = 1 + 1 /* after ETX */; i < 1 + 1 + 4; i++) {
                trueChecksum[i - 2] = framex[i];
            }
            //check equality    
            for (int i = 0; i < 6 && !this->error; i++) {
                if (checksum[i] != trueChecksum[i]) {
                    this->error = true;
                }
            }
        }
    }

    char getAck() { return this->ack; }
    void setAck(char newAck) { this->ack = newAck; }

    char getFrameNumber() { return this->frameNumber; }
    void setFrameNumber(char newNumber) { this->frameNumber = newNumber; }

    char * toBytes() {
        char * o = new char[1 + 1 + 4];
        o[0] = this->ack;
        o[1] = this->frameNumber;

        unsigned short c = calc_crc16(o, strlen(o));
        char * a = new char[2];
        a[0] = c & 0xff;
        a[1] = (c >> 8) & 0xff;
        sprintf(o, "%s%s", o, a);
        return o;
    }

    int getBytesLength() { return 1 + 1 + 4; }

    void printBytes() {
        char * buffer = this->toBytes();
        for(int j = 0; buffer[j] != 0; j++)
            printf("%02hhX ", buffer[j]);
    }

    bool isError() { return this->error; }
};

#endif