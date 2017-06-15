
#include <DigiUSB.h>

void setup() {
randomSeed(1);
DigiUSB.begin();
}

void loop() {

static String b = "                                                                                                    ";

static unsigned int n = b.length();

if  (DigiUSB.tx_remaining() > (n + 1)) {

for ( unsigned int i = 0; i <= n; i++) {if (b.charAt   (i) ==     32      ) {
if  (!random(0,  81)                 ) {    b.setCharAt(i, random(33, 127));}
}
}

for ( unsigned int i = 0; i <= n; i++) {if (b.charAt   (i) !=     32      ) {
if  (!random(0,  11)                 ) {    b.setCharAt(i,        32      );}
else                                   {    b.setCharAt(i, random(33, 127));}
}
}

DigiUSB.print(b +'\n');

}

DigiUSB.delay(1);

}
