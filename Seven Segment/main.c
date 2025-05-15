#include <lpc214x.h>

void delay(unsigned int);
unsigned int seg[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// Define keypad rows and columns
#define c0 0x00010000  // P1.16
#define c1 0x00020000  // P1.17
#define c2 0x00040000  // P1.18
#define r0 0x00080000  // P1.19
#define r1 0x00100000  // P1.20
#define r2 0x00200000  // P1.21
#define r3 0x00400000  // P1.22

void delay(unsigned int count)
{
    unsigned int i, j;
    for (i = 0; i < count; i++)
        for (j = 0; j < 3000; j++);
}

int get_keypad_value()
{
    int row, col, value = -1;

    for (row = 0; row < 4; row++) {
        IO1SET = (r0 | r1 | r2 | r3); // Set all rows HIGH
        switch (row) {
            case 0: IO1CLR = r0; break;
            case 1: IO1CLR = r1; break;
            case 2: IO1CLR = r2; break;
            case 3: IO1CLR = r3; break;
        }

        if (!(IO1PIN & c0)) { col = 0; }
        else if (!(IO1PIN & c1)) { col = 1; }
        else if (!(IO1PIN & c2)) { col = 2; }
        else continue;

        delay(100);  // Debounce

        value = row * 3 + col + 1;

        if (value == 11) value = 0; // if pressed 0
        if (value > 9) value = -1; // skip *, #, A-D

        break;
    }

    return value;
}

int main(void)
{
    int key = -1;
    IO0DIR = 0xFF;               // P0.0 to P0.7 as output for 7-segment
    IO1DIR = r0 | r1 | r2 | r3;  // Rows as output
    IO1DIR &= ~(c0 | c1 | c2);   // Columns as input

    while (1) {
        key = get_keypad_value();
        if (key >= 0 && key <= 9) {
            IO0CLR = 0xFF;         // Clear 7-segment output
            IO0SET = seg[key];     // Display digit
        }
    }
}
