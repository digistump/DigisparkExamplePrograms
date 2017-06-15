
#include <stdio.h>
#include <string.h>

#if defined WIN
#include <lusb0_usb.h> // this is libusb, see http://libusb.sourceforge.net/
#else
#include <usb.h>       // this is libusb, see http://libusb.sourceforge.net/
#endif

#include <curses.h>
#include <stdlib.h>

int main(int argc, char **argv) {

struct usb_bus *bus = NULL;
struct usb_device *digiSpark = NULL;
struct usb_device *device = NULL;

// Initialize the USB library
usb_init();

// Enumerate the USB device tree
usb_find_busses();
usb_find_devices();

// Iterate through attached busses and devices
bus = usb_get_busses();
while(bus != NULL) {
device = bus->devices;
while(device != NULL) {
// Check to see if each USB device matches the DigiSpark Vendor and Product IDs
if((device->descriptor.idVendor == 0x16c0) && (device->descriptor.idProduct == 0x05df)) {digiSpark = device;}
device = device->next;
}
bus = bus->next;
}

if(digiSpark == NULL) {printf("DigiUSB no found"); Sleep(5000); return 1;}

int numInterfaces = 0;
struct usb_dev_handle *devHandle = NULL;
struct usb_interface_descriptor *interface = NULL;

devHandle = usb_open(digiSpark);

if(devHandle != NULL) {
numInterfaces = digiSpark->config->bNumInterfaces;
interface = &(digiSpark->config->interface[0].altsetting[0]);
}

// ~консоль~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

if (argc == 2) { // команда запуска имеет два аргумента - имя программы и строка ввода.
for (int i = 0; i < strlen(argv[1]); i++) { // перебор номеров символов 1-й строки(строки ввода) от 0-го до последнего.
usb_control_msg(devHandle, (0x01 << 5), 0x09, 0, argv[1][i], 0, 0, 1000); // отправка символа № i 1-й строки(строки ввода) в USB.
}
usb_control_msg(devHandle, (0x01 << 5), 0x09, 0,       '\n', 0, 0, 1000); // отправка символа перевода строки в USB.
return 1; // завершение программы.
}

//~PDCurses~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

WINDOW *win, *outputwin, *inputwin;
win = initscr();

// subwin (WINDOW *, lines, columns, begin    y, begin    x);
// subwin (  окно *, линии, колонки, начинать у, начинать х);
// LINES - высота окна терминала
// COLS  - ширина окна терминала
outputwin = subwin(win, LINES - 4, COLS - 2,         1, 1); // окно вывода из USB.
inputwin  = subwin(win,         1, COLS - 2, LINES - 2, 1); // окно ввода в USB.

cbreak();    // не использовать буфер для функции getch().
noecho();    // отключить эхо-печать ввода с клавиатуры в окно stdscr.
curs_set(0); // отключить курсор.

nodelay(stdscr, TRUE); // отменить стоп программы при getch() == ERR.
keypad(stdscr, TRUE);  // активировать специальные клавиши клавиатуры.

scrollok(outputwin, TRUE); // включить прокрутку окна вывода из USB.
scrollok(inputwin, TRUE);  // включить прокрутку окна ввода в USB.

//~терминал~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

char send_usb = ERR; // символ отправляемый в USB.
char receive_usb;    // символ полученный из USB.
int result_usb;      // результат отправки-получения в-из USB.

while (1) {

//~~~принять символ из USB~~~

    result_usb = usb_control_msg(devHandle, (0x01 << 5) | 0x80, 0x01, 0, 0, &receive_usb, 1, 1000); // принять символ из USB.
if (result_usb < 0) {printf("Error USB: %i", result_usb); Sleep(5000); return 1;} // в USB что-то сломалось - завершение программы.
if (result_usb == 1) { // из USB пришёл символ.

FILE *r = fopen("received_digiusb.txt", "at"); // открыть-создать файл для записи в конец файла.
if ((receive_usb >= 32 && receive_usb <= 126) || receive_usb == 8 || receive_usb == 9 || receive_usb == 10 || receive_usb == 13)                      {wprintw(outputwin,  "%c" , receive_usb); if (receive_usb != 8) fprintf(r,  "%c" , receive_usb);} // печатные символы.
if ((receive_usb <= 31 || receive_usb == 127) && receive_usb != 8 && receive_usb != 9 && receive_usb != 10 && receive_usb != 13 && receive_usb != 17) {wprintw(outputwin, "[%d]", receive_usb);                       fprintf(r, "[%d]", receive_usb);} // непечатные символы.
if (receive_usb == 17) {wprintw(outputwin, "%c", 10); fprintf(r, "%c", 10);} // LF \n [17] (Unix) -> LF \n [10] (Windows)
fclose(r); // закрыть файл.

wdelch(outputwin);   // затереть символ под курсором.
wrefresh(outputwin); // обновить окно.

mvprintw(LINES - 3, 1, "    ");            // затереть прошлую запись.
mvprintw(LINES - 3, 1, "%d", receive_usb); // печать десятичного значения символа.

}

//~~~отослать символ в USB~~~

    send_usb = getch(); // retrieve scancode. ♥
if (send_usb != ERR) {  // из клавиатуры пришёл символ.

    result_usb = usb_control_msg(devHandle, (0x01 << 5), 0x09, 0, send_usb, 0, 0, 1000); // отослать символ в USB.
if (result_usb < 0) {printf("Error USB: %i", result_usb); Sleep(5000); return 1;} // в USB что-то сломалось - завершение программы.

FILE *s = fopen("sent_digiusb.txt", "at"); // открыть-создать файл для записи в конец файла.
if ((send_usb >= 32 && send_usb <= 126) || send_usb == 8 || send_usb == 9 || send_usb == 10 || send_usb == 13)                   {wprintw(inputwin,  "%c" , send_usb); if (send_usb != 8) fprintf(s,  "%c" , send_usb);} // печатные символы.
if ((send_usb <= 31 || send_usb == 127) && send_usb != 8 && send_usb != 9 && send_usb != 10 && send_usb != 13 && send_usb != 17) {wprintw(inputwin, "[%d]", send_usb);                    fprintf(s, "[%d]", send_usb);} // непечатные символы.
if (send_usb == 17) {wprintw(inputwin, "%c", 10); fprintf(s, "%c", 10);} // LF \n [17] (Unix) -> LF \n [10] (Windows)
fclose(s); // закрыть файл.

wdelch(inputwin);   // затереть символ под курсором.
wrefresh(inputwin); // обновить окно.

mvprintw(LINES - 1, 1, "    ");         // затереть прошлую запись.
mvprintw(LINES - 1, 1, "%d", send_usb); // печать десятичного значения символа.

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~
}

usb_close(devHandle);
endwin();

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
