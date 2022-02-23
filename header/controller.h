        // Clock Cycle     Button Reported
        // ===========     ===============
        // 1               B
        // 2               Y
        // 3               Select
        // 4               Start
        // 5               Up on joypad
        // 6               Down on joypad
        // 7               Left on joypad
        // 8               Right on joypad
        // 9               A
        // 10              X
        // 11              L
        // 12              R
        // 13              none (always high)
        // 14              none (always high)
        // 15              none (always high)
        // 16              none (always high)

#define B      (buttons & 0x1) //0
#define Y      (buttons & 0x2)
#define SELECT (buttons & 0x4)
#define START  (buttons & 0x8)
#define UP     (buttons & 0x10)
#define DOWN   (buttons & 0x20)
#define LEFT   (buttons & 0x40)
#define RIGHT  (buttons & 0x80)
#define A      (buttons & 0x100)
#define X      (buttons & 0x200)
#define L      (buttons & 0x400)
#define R      (buttons & 0x800)


#define DATA_LATCH_HIGH (0x1) //A0 - Data Latch (output)
#define DATA_LATCH_LOW (0xFE) //A0 - Data Latch (output)

#define CLOCK_HIGH (0x4) //A2 - Clock (output)
#define CLOCK_LOW (0xFB) //A2 - Clock (output)

#define SERIAL_DATA (~PINA & 0x02) //A1 - Serial Data (input)