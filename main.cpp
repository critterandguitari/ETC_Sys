
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>

#include "OSC/OSCMessage.h"
#include "OSC/SimpleWriter.h"
#include "Serial.h"
#include "UdpSocket.h"
#include "SLIPEncodedSerial.h"
#include "OledScreen.h"
#include "UI.h"
#include "Timer.h"

Serial serial;
SLIPEncodedSerial slip;
SimpleWriter dump;

// for communicating with Pd or other program
UdpSocket udpSock(4001);

// main menu interface program
UI ui;

// exit flag
int quit = 0;

/** OSC messages received internally (from PD or other program) **/
void setLED(OSCMessage &msg);
void reload(OSCMessage &msg);
void sendReady(OSCMessage &msg);
void sendShutdown(OSCMessage &msg);
void quitMother(OSCMessage &msg);
/* end internal OSC messages received */

/* helpers */
void updateScreenPage(uint8_t page, OledScreen &screen);
void setScreenLine(OledScreen &screen, int lineNum, OSCMessage &msg);
void sendGetKnobs(void);
/* end helpers */

int main(int argc, char* argv[]) {
      
    uint32_t seconds = 0;
    char udpPacketIn[256];
    uint8_t i = 0;
    int len = 0;
    int page = 0;


    Timer knobPollTimer, pingTimer, upTime;

    knobPollTimer.reset();
    pingTimer.reset();
    upTime.reset();

    udpSock.setDestination(4000, "localhost");
    OSCMessage msgIn;

    ui.loadPatchList();
    ui.drawPatchList();

    // send ready to wake up MCU
    // MCU is ignoring stuff over serial port until this message comes through
    // don't empty the message because it gets sent out periodically incase MCU resets
    OSCMessage rdyMsg("/ready");
    rdyMsg.add(1);
    rdyMsg.send(dump);
    // send it a few times just in case
    for(i = 0; i<4; i++) {
       slip.sendMessage(dump.buffer, dump.length, serial);
       usleep(20000); // wait 20 ms
    }
    
    quit = 0;

    // full udp -> serial -> serial -> udp
    for (;;){
        // receive udp, send to serial
        len = udpSock.readBuffer(udpPacketIn, 256, 0);
        if (len > 0){
            msgIn.empty();
            for (i = 0; i < len; i++){
                msgIn.fill(udpPacketIn[i]);
            }    
            if(!msgIn.hasError()){
                msgIn.dispatch("/ready", sendReady, 0);
                msgIn.dispatch("/shutdown", sendShutdown, 0);
                msgIn.dispatch("/led", setLED, 0);
                msgIn.dispatch("/reload", reload, 0);
                msgIn.dispatch("/quitmother", quitMother, 0);
            }
            else {
                printf("bad message\n");
            }
            msgIn.empty();
        }   

        // receive serial, send udp
        if(slip.recvMessage(serial)) {
            udpSock.writeBuffer(slip.decodedBuf, slip.decodedLength);
            
            // check if we need to do something with this message
            msgIn.empty();
            msgIn.fill(slip.decodedBuf, slip.decodedLength);
            msgIn.empty();
        }

        // sleep for .5ms
        usleep(750);
        
        // every 1 second send a ping in case MCU resets
        if (pingTimer.getElapsed() > 1000.f){
          //  printf("pinged the MCU at %f ms.\n", upTime.getElapsed());
            pingTimer.reset();
            rdyMsg.send(dump);
            slip.sendMessage(dump.buffer, dump.length, serial);
        }

        // poll for knobs
        if (knobPollTimer.getElapsed() > 40.f){
            knobPollTimer.reset();
            sendGetKnobs();
        }
        
        // check exit flag
        if (quit) {
            printf("quitting\n");
            return 0;
        }
    } // for;;
}


void quitMother(OSCMessage &msg){
    quit = 1;
}

void setLED(OSCMessage &msg){
    msg.send(dump);
    slip.sendMessage(dump.buffer, dump.length, serial);
}

void reload(OSCMessage &msg){
    printf("received reload msg\n");
    ui.loadPatchList();
}

void sendReady(OSCMessage &msg){   
    printf("sending ready...\n");
    OSCMessage rdyMsg("/ready");
    rdyMsg.add(1);
    rdyMsg.send(dump);
    slip.sendMessage(dump.buffer, dump.length, serial);
    rdyMsg.empty();
}

void sendShutdown(OSCMessage &msg){
    printf("sending shutdown...\n");
    OSCMessage rdyMsg("/shutdown");
    rdyMsg.add(1);
    rdyMsg.send(dump);
    slip.sendMessage(dump.buffer, dump.length, serial);
    rdyMsg.empty();
}
/* end internal OSC messages received */

void sendGetKnobs(void){
    OSCMessage msg("/getknobs");
    msg.add(1);
    msg.send(dump);
    slip.sendMessage(dump.buffer, dump.length, serial);
}



