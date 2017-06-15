
#include <DigiUSB.h>

void setup() {
digitalWrite(14, 1);
pinMode(14, OUTPUT); // led01
DigiUSB.begin();
}

void loop() {

if (DigiUSB.available()) { // в USB что-то есть.

char a = DigiUSB.read(); // считать символ из USB в переменную.
static String b = ""; // объявление переменной строки.

if (a != '\b') {b = b + a;               } // дописать символ в конец строки, если это не символ клавиши Backspace.
if (a == '\b') {b.remove(b.length() - 1);} // удалить последний символ строки, если получен символ клавиши Backspace.

if (a == '\n') {                           // что-то сделать, после получения символа клавиши Enter.
if (b == "led01 on\n") {digitalWrite(14, 0); DigiUSB.print("ok! led01 on\n");} // зажечь светодиод, если принята строка "ххх\n".
if (b == "led01 of\n") {digitalWrite(14, 1); DigiUSB.print("ok! led01 of\n");} // погасить светодиод, если принята строка "ххх\n".
b = ""; // очистка строки.
}

}

DigiUSB.delay(1);

}

