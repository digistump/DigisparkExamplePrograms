
#include <DigiUSB.h>

void setup() {DigiUSB.begin();}

void loop() {

if (DigiUSB.available()) {

char RX_s;
char TX_s;

RX_s = DigiUSB.read();
TX_s = RX_s;
DigiUSB.write(TX_s);

}

DigiUSB.delay(1);

}

