/*	Author: Christopher Vurbenova-Mouri
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
 
// Demo Link: https://www.youtube.com/watch?v=Ndl2lxBM0yg

#include <avr/io.h>
#define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

#include "../header/newusart.h"
#include "../header/timer.h"
#include "../header/controller.h"

#include "../header/paths.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#define C0 (~PINC & 0x01)

//------game defs--------------------------
#define MOVE_SPEED 5
#define PLAYER_SIZE 10

#define BULLET_SPEED 4
#define BULLET_SIZE 4
#define NUM_BULLETS 10

#define FACE_UP      0
#define FACE_RIGHT   1
#define FACE_DOWN    2
#define FACE_LEFT    3

#define NUM_ENEMIES  7
#define ENEMY_SIZE   10
#define ENEMY_SPEED  10
#define ATTACK       1
#define ROAM         2
#define SIGHT_LENGTH 6
//----------------------------------------

//-----game map-----------------------------
//20 x 20 pixel segments
#define ROWS 16
#define COLS 24
char game_map[16][24] = {
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'O', 'O', 'O', 'A', 'O', 'O', 'O', 'O', 'O', 'O'},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'A'},
    {'O', 'O', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'X', 'X', 'X', 'O', 'O'},
    {'O', 'A', 'O', 'O', 'O', 'X', 'X', 'X', 'X', 'X', 'O', 'A', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'X', 'X', 'X', 'O', 'O'},
    {'O', 'O', 'O', 'O', 'O', 'X', 'X', 'X', 'X', 'X', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'X', 'X', 'X', 'X', 'X'},
    {'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
    {'O', 'O', 'X', 'O', 'A', 'O', 'O', 'O', 'O', 'A', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'X', 'O', 'O'},
    {'O', 'A', 'X', 'X', 'O', 'O', 'O', 'O', 'X', 'O', 'O', 'O', 'O', 'O', 'X', 'O', 'O', 'O', 'O', 'O', 'A', 'X', 'O', 'A'},
    {'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'A', 'O', 'O', 'O', 'O', 'A', 'O', 'O', 'O', 'O', 'X', 'O', 'O'},
    {'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'X', 'O', 'O'},
    {'O', 'A', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'X', 'O', 'O', 'O', 'O', 'X', 'X', 'O', 'O', 'O', 'O', 'O', 'O'},
    {'O', 'O', 'X', 'X', 'O', 'O', 'O', 'O', 'A', 'O', 'O', 'O', 'O', 'O', 'X', 'X', 'X', 'X', 'O', 'O', 'O', 'O', 'O', 'O'},
    {'O', 'O', 'X', 'X', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'A', 'O', 'X', 'X', 'X', 'X', 'O', 'O', 'O', 'O', 'O', 'O'},
    {'O', 'O', 'X', 'X', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O'},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'A', 'O', 'O', 'O', 'X'},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'X', 'X'},
};

//------shared variables ------------------
unsigned short buttons = 0;
unsigned char game_over = 0;

unsigned short player_x = 150;
unsigned short player_y = 150;
unsigned char player_rotation;
unsigned char player_s_node;

unsigned short bullets_x[NUM_BULLETS] = {0};
unsigned short bullets_y[NUM_BULLETS] = {0};
unsigned char bullets_movement[NUM_BULLETS] = {0};
unsigned char shoot = 0; //flag to shoot bullet

unsigned short enemies_x[NUM_ENEMIES] = {0};
unsigned short enemies_y[NUM_ENEMIES] = {0};
unsigned char alive[NUM_ENEMIES] = {0};
unsigned char movement_type[NUM_ENEMIES] = {0};

unsigned char num_alive = 2;
unsigned char max_enemies = 2;
unsigned char health = 1;
//-------------------------------------------

//------helper functions -------------------
void get_controller_data();
void delay_us(unsigned int);
void calc_closest_s_node();
//------------------------------------------

//-----state machines----------------------
typedef struct _task { 
    signed char state;
    unsigned long int period;
    unsigned long int elapsedTime;
    int (*TickFct)(int);
} task;

enum playerSM_States { player_start, player_poll };
enum bulletSM_States { bullet_start, bullet_on };
enum bulletPollSM_States { bullet_poll_start, bullet_poll_wait, bullet_poll_hold };
enum rotationPollSM_States { rotation_poll_start, rotation_poll_wait, rotation_poll_hold_L, rotation_poll_hold_R };
enum enemySM_States { enemy_start, enemy_respawn, enemy_on };
enum enemyVisionSM_States { vision_start, vision_on };

int playerSMTick(int);
int bulletSMTick(int);
int bulletPollSMTick(int);
int rotationPollSMTick(int);
int enemySMTick(int);
// int enemyVisionSMTick(int);
//------------------------------------------

static task task1, task2, task3, task4, task5; // task6;
task * tasks[] = {&task1, &task2, &task3, &task4, &task5}; //, &task6};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
const char start = -1;

void increase_times() {
    for (unsigned char i = 0; i < numTasks; i++) {
        tasks[i]->elapsedTime += 10;
    }
}

int main(void) {
    //------------pin setup------------------------------------------------------------------------------------------------
    DDRA = 0xFD; PORTA = 0x02; // A0 - Data Latch (output) | A1 - Serial Data (input) | A2 - Clock for SNES Controller
    PORTA = PORTA | CLOCK_HIGH; //init clock to high
    PORTA = PORTA & DATA_LATCH_LOW; //init latch to low

    DDRC = 0x00; PORTC = 0xFF; //C0 - ready input from arduino

    DDRB = 0xFF; PORTB = 0x00; // output
    //----------------------------------------------------------------------------------------------------------------------

    //------------init state machines -----------------------------------------------------------------------------------------

    task1.state = start;
    task1.period = 70;
    task1.elapsedTime = task1.period;
    task1.TickFct = &playerSMTick;

    task2.state = start;
    task2.period = 30;
    task2.elapsedTime = task2.period;
    task2.TickFct = &bulletSMTick;

    task3.state = start;
    task3.period = 50;
    task3.elapsedTime = task3.period;
    task3.TickFct = &bulletPollSMTick;

    task4.state = start;
    task4.period = 50;
    task4.elapsedTime = task4.period;
    task4.TickFct = &rotationPollSMTick;

    task5.state = start;
    task5.period = 100;
    task5.elapsedTime = task5.period;
    task5.TickFct = &enemySMTick;

    // task6.state = start;
    // task6.period = 500;
    // task6.elapsedTime = task6.period;
    // task6.TickFct = &enemyVisionSMTick;
    //-------------------------------------------------------------------------------------------------------------------------

    USART_Init(MYUBRR);
    TimerSet(10);
    TimerOn();

    while (1) {
        //get_controller_data();

        if (TimerFlag) {
            for (unsigned char i = 0; i < numTasks; i++) {
                if (tasks[i]->elapsedTime >= tasks[i]->period) {
                    tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                    tasks[i]->elapsedTime = 0;
                }
                //tasks[i]->elapsedTime += 10;
            }
            --TimerFlag;
        }
        while(!TimerFlag) {}
    }
    return 1;
}

int playerSMTick(int state) { //change player_x, player_y
    switch (state) { //transitions
        case player_start:
            if (C0) { //only if arduino is ready
                PORTB = PORTB & 0xBF;
                player_x = 150;
                player_y = 150;
                state = player_poll;
                player_rotation = FACE_UP;
                USART_Transmit('x');
                USART_Transmit((char)(player_x >> 8));
                USART_Transmit((char)(player_x));
                USART_Transmit('y');
                USART_Transmit((char)(player_y >> 8));
                USART_Transmit((char)(player_y));
            } else { PORTB = PORTB | 0x40; }
            break;
        case player_poll: 
            if (!C0) { 
                state = player_start;
                player_x = 150; 
                player_y = 150;
            }
            if (game_over) state = start;
            break;
        default: state = player_start; break;
    }
    
    switch(state) { //actions
        case player_start: break;
        case player_poll:
            PORTB = PORTB | 0x80;
            get_controller_data();
            if (RIGHT) { //UP
                if (!(player_y == 470 || game_map[player_x/20][(player_y+PLAYER_SIZE+MOVE_SPEED-1)/20] == 'X' || game_map[(player_x+PLAYER_SIZE-1)/20][(player_y+PLAYER_SIZE+MOVE_SPEED-1)/20] == 'X')) {
                    USART_Transmit('y');
                    player_y += MOVE_SPEED;
                    USART_Transmit((char)(player_y >> 8));
                    USART_Transmit((char)(player_y));
                }
                calc_closest_s_node();

                PORTB = PORTB | 0x1; 
            } else PORTB = PORTB & 0xFE;
            if (LEFT) { //DOWN
                if (!(player_y == 0  || game_map[player_x/20][(player_y-MOVE_SPEED)/20] == 'X' || game_map[(player_x+PLAYER_SIZE-1)/20][(player_y-MOVE_SPEED)/20] == 'X')) {
                    USART_Transmit('y');
                    player_y -= MOVE_SPEED;
                    USART_Transmit((char)(player_y >> 8));
                    USART_Transmit((char)(player_y));
                }
                calc_closest_s_node();

                PORTB = PORTB | 0x2;
            } else PORTB = PORTB & 0xFD;
            if (UP) { //LEFT
                if (!(player_x == 0  || game_map[(player_x-MOVE_SPEED)/20][player_y/20] == 'X' || game_map[(player_x-MOVE_SPEED)/20][(player_y+PLAYER_SIZE-1)/20] == 'X')) {
                    USART_Transmit('x');
                    player_x -= MOVE_SPEED;
                    USART_Transmit((char)(player_x >> 8));
                    USART_Transmit((char)(player_x));
                }
                calc_closest_s_node();
            }
            if (DOWN) { //RIGHT
                if (!(player_x == 310  || game_map[(player_x+PLAYER_SIZE+MOVE_SPEED-1)/20][player_y/20] == 'X' || game_map[(player_x+PLAYER_SIZE+MOVE_SPEED-1)/20][(player_y+PLAYER_SIZE-1)/20] == 'X')) {
                    USART_Transmit('x');
                    player_x += MOVE_SPEED;
                    USART_Transmit((char)(player_x >> 8));
                    USART_Transmit((char)(player_x));
                }
                calc_closest_s_node();
            } 
            break;
    }
    return state;

}

int enemySMTick(int state) {
    static unsigned char i;
    unsigned char divisor = num_alive/2 + 1;
    switch (state) {
        case enemy_start:
            i = 0;
            if (C0) {
                state = enemy_respawn;
                num_alive = max_enemies;
            }
            break;
        case enemy_respawn: 
            i = 0;
            state = enemy_on;
            break;
        case enemy_on: 
            if (num_alive == 0) {
                state = enemy_respawn;
                if (max_enemies < NUM_ENEMIES) ++max_enemies;
                else if (health < 10) ++health;
                num_alive = max_enemies;
            }
            if (game_over) state = start;
            break;
        default: 
            memset(enemies_x, 0, sizeof(enemies_x));
            memset(enemies_y, 0, sizeof(enemies_y));
            memset(alive, 0, sizeof(alive));
            memset(movement_type, 0, sizeof(movement_type));
            state = enemy_start; 
            game_over = 0;
            break;
    }

    switch (state) {
        case enemy_start: break;
        case enemy_respawn: 
            for (unsigned j = 0; j < max_enemies; ++j) {

                unsigned char poll = rand() % 3;
                if (poll == 0) {
                    enemies_y[j] = 440 + (rand() % 3)*10;
                    enemies_x[j] = 40 + (rand() % 3)*10;
                }
                else if (poll == 1) {
                    enemies_y[j] = 0 + (rand() % 3)*10;
                    enemies_x[j] = 210 + (rand() % 8)*10;
                }
                else {
                    enemies_y[j] = 440 + (rand() % 3)*10;
                    enemies_x[j] = 120 + (rand() % 8)*10;
                }
                alive[j] = health;
                movement_type[j] = ROAM;

                USART_Transmit('e');
                USART_Transmit((char)((0 << 4) | j)); //send enemy index and saying hes being spawned for the first time
                USART_Transmit((char)(enemies_x[j] >> 8));
                USART_Transmit((char)(enemies_x[j]));
                USART_Transmit((char)(enemies_y[j] >> 8));
                USART_Transmit((char)(enemies_y[j]));
            }
            break;
        case enemy_on:
            i = (i+1) % divisor;
            for (unsigned char j = i; j < max_enemies; j += divisor) {
                if (alive[j] == 0 && enemies_x[j] != 0 && enemies_y[j] != 0) { //despawn killed enemies
                    --num_alive;
                    USART_Transmit('e');
                    USART_Transmit((char)((3 << 4) | j)); //send enemy index, saying hes being spawned for the first time
                    enemies_x[j] = enemies_y[j] = 0;
                    movement_type[j] = ROAM;
                }
                if (alive[j] && movement_type[j] == ROAM) { //ROAM logic
                    unsigned char my_i = enemies_x[j] / 20.0;
                    unsigned char my_j = enemies_y[j] / 20.0;
                    unsigned char travel_to_i = nodes[my_i][my_j].from_i[player_s_node];
                    unsigned char travel_to_j = nodes[my_i][my_j].from_j[player_s_node];

                    if (travel_to_i != 0 && travel_to_j != 0) { // we are not on the s_node
                        if (travel_to_i < my_i) {
                            enemies_x[j] -= ENEMY_SPEED;
                            USART_Transmit('e');
                            USART_Transmit((char)((1 << 4) | j)); 
                            USART_Transmit((char)(enemies_x[j] >> 8));
                            USART_Transmit((char)(enemies_x[j]));
                        }
                        else if  (travel_to_i > my_i) {
                            enemies_x[j] += ENEMY_SPEED;
                            USART_Transmit('e');
                            USART_Transmit((char)((1 << 4) | j)); 
                            USART_Transmit((char)(enemies_x[j] >> 8));
                            USART_Transmit((char)(enemies_x[j]));
                        }
                        else if (travel_to_j < my_j) {
                            enemies_y[j] -= ENEMY_SPEED;
                            USART_Transmit('e');
                            USART_Transmit((char)((2 << 4) | j));
                            USART_Transmit((char)(enemies_y[j] >> 8));
                            USART_Transmit((char)(enemies_y[j]));
                        }
                        else if  (travel_to_j > my_j) {
                            enemies_y[j] += ENEMY_SPEED;
                            USART_Transmit('e');
                            USART_Transmit((char)((2 << 4) | j));
                            USART_Transmit((char)(enemies_y[j] >> 8));
                            USART_Transmit((char)(enemies_y[j]));
                        }
                    }
                    else { // we are at the s_node, so change to attack
                        movement_type[j] = ATTACK;
                    }
                }
                if (alive[j] && movement_type[j] == ATTACK) {// ATTACK logic
                    unsigned char player_i = player_x / 20.0;
                    unsigned char player_j = player_y / 20.0;
                    unsigned char my_i = enemies_x[j] / 20.0;
                    unsigned char my_j = enemies_y[j] / 20.0;

                    if (my_i == player_i && my_j == player_j) { //enemy in the same pixel segment as player
                        if (rand() % 2 == 0 && enemies_x[j] != player_x) {
                            if (player_x < enemies_x[j]) {
                                enemies_x[j] -= ENEMY_SPEED;
                                USART_Transmit('e');
                                USART_Transmit((char)((1 << 4) | j)); 
                                USART_Transmit((char)(enemies_x[j] >> 8));
                                USART_Transmit((char)(enemies_x[j]));
                            }
                            else {
                                enemies_x[j] += ENEMY_SPEED;
                                USART_Transmit('e');
                                USART_Transmit((char)((1 << 4) | j)); 
                                USART_Transmit((char)(enemies_x[j] >> 8));
                                USART_Transmit((char)(enemies_x[j]));
                            }
                        }
                        else {
                            if (player_y < enemies_y[j]) {
                                enemies_y[j] -= ENEMY_SPEED;
                                USART_Transmit('e');
                                USART_Transmit((char)((2 << 4) | j)); 
                                USART_Transmit((char)(enemies_y[j] >> 8));
                                USART_Transmit((char)(enemies_y[j]));
                            }
                            else {
                                enemies_y[j] += ENEMY_SPEED;
                                USART_Transmit('e');
                                USART_Transmit((char)((2 << 4) | j)); 
                                USART_Transmit((char)(enemies_y[j] >> 8));
                                USART_Transmit((char)(enemies_y[j]));
                            }
                        }
                    }
                    else if (rand()%2 == 0 && my_i != player_i) { //travel on x axis
                        if (player_i < my_i) {
                            if (game_map[my_i-1][my_j] == 'X') {
                                movement_type[j] = ROAM;
                            }
                            else {
                                enemies_x[j] -= ENEMY_SPEED;
                                USART_Transmit('e');
                                USART_Transmit((char)((1 << 4) | j)); 
                                USART_Transmit((char)(enemies_x[j] >> 8));
                                USART_Transmit((char)(enemies_x[j]));
                            }
                        }
                        else {
                            if (game_map[my_i+1][my_j] == 'X') {
                                movement_type[j] = ROAM;
                            }
                            else {
                                enemies_x[j] += ENEMY_SPEED;
                                USART_Transmit('e');
                                USART_Transmit((char)((1 << 4) | j)); 
                                USART_Transmit((char)(enemies_x[j] >> 8));
                                USART_Transmit((char)(enemies_x[j]));
                            }
                        }
                    }
                    else { //travel on y axis
                        if (player_j < my_j) {
                            if (game_map[my_i][my_j-1] == 'X') {
                                movement_type[j] = ROAM;
                            }
                            else {
                                enemies_y[j] -= ENEMY_SPEED;
                                USART_Transmit('e');
                                USART_Transmit((char)((2 << 4) | j));
                                USART_Transmit((char)(enemies_y[j] >> 8));
                                USART_Transmit((char)(enemies_y[j]));
                            }
                        }
                        else {
                            if (game_map[my_i][my_j+1] == 'X') {
                                movement_type[j] = ROAM;
                            }
                            else {
                                enemies_y[j] += ENEMY_SPEED;
                                USART_Transmit('e');
                                USART_Transmit((char)((2 << 4) | j));
                                USART_Transmit((char)(enemies_y[j] >> 8));
                                USART_Transmit((char)(enemies_y[j]));
                            }
                        }
                    }
                }
                if (alive[j] && enemies_x[j] >= player_x - MOVE_SPEED && enemies_x[j] < player_x + PLAYER_SIZE) { //kill player logic
                    if (enemies_y[j] >= player_y - MOVE_SPEED && enemies_y[j] < player_y + PLAYER_SIZE) {
                        USART_Transmit('d');
                    }
                }
            }
            break;
        default: break;
    }

    return state;
}

// int enemyVisionSMTick(int state) { //enemy looks out in 4 directions with a length of 6 pixel segments looking for player
//     static unsigned char i;
//     switch (state) {
//         case vision_start: 
//             i = 0;
//             if (C0) {
//                 state = vision_on;
//             }
//             break;
//         case vision_on: break;
//         default: 
//             state = vision_start;
//             break;
//     }

//     switch (state) {
//         case vision_start: 
//             if (C0) {
//                 state = vision_on;
//             }
//             break;
//         case vision_on: 
//             while (i < max_enemies || !(alive[i] && movement_type[i] == ROAM)) {
//                 ++i;
//             } 

//             unsigned char my_i = enemies_x[i] / 20.0;
//             unsigned char my_j = enemies_y[i] / 20.0;
//             unsigned char player_i = player_x / 20.0;
//             unsigned char player_j = player_y / 20.0;
//             for (unsigned k = 1; k <= SIGHT_LENGTH && my_i+k < ROWS && game_map[my_i+k][my_j] != 'X'; ++k) {  //look down
//                 if (my_i+k == player_i) movement_type[i] = ATTACK;
//             }
//             for (unsigned k = 1; k <= SIGHT_LENGTH && my_i-k > 0 && game_map[my_i-k][my_j] != 'X'; ++k) {  //look up
//                 if (my_i-k == player_i) movement_type[i] = ATTACK;
//             }
//             for (unsigned k = 1; k <= SIGHT_LENGTH && my_j+k < COLS && game_map[my_i][my_j+k] != 'X'; ++k) {  //look right
//                 if (my_j+k == player_j) movement_type[i] = ATTACK;
//             }
//             for (unsigned k = 1; k <= SIGHT_LENGTH && my_j-k > 0 && game_map[my_i][my_j-k] != 'X'; ++k) {  //look left
//                 if (my_j-k == player_j) movement_type[i] = ATTACK;
//             }

//             i = (i+1) % max_enemies;
//             break;
//         default: 
//             state = vision_start;
//             break;
//     }

//     return state;
// }

int bulletSMTick(int state) {
    switch (state) {
        case bullet_start: 
            if (C0) {
                for (unsigned int i = 0; i < NUM_BULLETS; ++i) { //init all bullets to dead
                    bullets_movement[i] = 5;
                }
                state = bullet_on;
            }
            break;
        case bullet_on: break;
        if (game_over) state = start;
        default: 
            memset(bullets_movement, 0, sizeof(bullets_movement));
            memset(bullets_x, 0, sizeof(bullets_x));
            memset(bullets_y, 0, sizeof(bullets_y));
            state = bullet_start;
            break;
    }

    switch (state) {
        case bullet_start: break;
        case bullet_on:
            if (shoot) { //shoot new bullet
                shoot = 0;
                unsigned char i = 0;
                while (bullets_movement[i] != 5) { // search for unused/dead bullet
                    if (++i == NUM_BULLETS) { break; }
                }
                bullets_movement[i] = player_rotation; // set this bullet to alive and let it travel in the direction the player is facing
                switch (bullets_movement[i]) {
                    case FACE_UP:
                        bullets_x[i] = player_x - BULLET_SIZE -1;
                        bullets_y[i] = player_y + 3;
                        break; 
                    case FACE_DOWN:
                        bullets_x[i] = player_x + PLAYER_SIZE + BULLET_SIZE +1;
                        bullets_y[i] = player_y + 3;
                        break;
                    case FACE_LEFT:
                        bullets_x[i] = player_x + 3;
                        bullets_y[i] = player_y - BULLET_SIZE -1;
                        break;
                    case FACE_RIGHT:
                        bullets_x[i] = player_x + 3;
                        bullets_y[i] = player_y + PLAYER_SIZE + BULLET_SIZE +1;
                        break;
                }
                USART_Transmit('s');
                USART_Transmit((char)((3 << 4) | i)); //send bullet index and direction of travel
                USART_Transmit((char)(bullets_x[i] >> 8));
                USART_Transmit((char)(bullets_x[i]));
                USART_Transmit((char)(bullets_y[i] >> 8));
                USART_Transmit((char)(bullets_y[i]));
            }

            for (int i = 0; i < NUM_BULLETS; ++i) { //move alive bullets and kill those that hit walls
                if (bullets_movement[i] != 5) { //if bullet is alive
                    switch (bullets_movement[i]) {
                        case FACE_UP:
                            bullets_x[i] -= BULLET_SPEED;
                            if (!(bullets_x[i] <= BULLET_SPEED || game_map[(bullets_x[i])/20][(bullets_y[i]+BULLET_SIZE)/20] == 'X' || game_map[(bullets_x[i])/20][(bullets_y[i])/20] == 'X')) { //bullet doesn't collide with wall
                                USART_Transmit('s');
                                USART_Transmit((char)((1 << 4) | i)); //send bullet index and direction of travel
                                USART_Transmit((char)(bullets_x[i] >> 8));
                                USART_Transmit((char)(bullets_x[i]));
                            }
                            else { //kill bullet
                                USART_Transmit('s');
                                USART_Transmit((char)(0 << 4) | i);
                                bullets_movement[i] = 5;
                                bullets_x[i] = 500;
                                bullets_y[i] = 500;
                            }
                            break;
                        case FACE_DOWN:
                            bullets_x[i] += BULLET_SPEED;
                            if (!(bullets_x[i] >= 320-BULLET_SPEED || game_map[(bullets_x[i]+BULLET_SIZE)/20][(bullets_y[i]+BULLET_SIZE)/20] == 'X' || game_map[(bullets_x[i]+BULLET_SIZE)/20][(bullets_y[i])/20] == 'X')) { //bullet doesn't collide with wall
                                USART_Transmit('s');
                                USART_Transmit((char)((1 << 4) | i)); //send bullet index and direction of travel
                                USART_Transmit((char)(bullets_x[i] >> 8));
                                USART_Transmit((char)(bullets_x[i]));
                            }
                            else { //kill bullet
                                USART_Transmit('s');
                                USART_Transmit((char)(0 << 4) | i);
                                bullets_movement[i] = 5;
                                bullets_x[i] = 500;
                                bullets_y[i] = 500;
                            }
                            break;
                        case FACE_LEFT:
                            bullets_y[i] -= BULLET_SPEED;
                            if (!(bullets_y[i] <= BULLET_SPEED || game_map[(bullets_x[i]+BULLET_SIZE)/20][(bullets_y[i])/20] == 'X' || game_map[(bullets_x[i])/20][(bullets_y[i])/20] == 'X')) { //bullet doesn't collide with wall
                                USART_Transmit('s');
                                USART_Transmit((char)((2 << 4) | i)); //send bullet index and direction of travel
                                USART_Transmit((char)(bullets_y[i] >> 8));
                                USART_Transmit((char)(bullets_y[i]));
                            }
                            else { //kill bullet
                                USART_Transmit('s');
                                USART_Transmit((char)(0 << 4) | i);
                                bullets_movement[i] = 5;
                                bullets_x[i] = 500;
                                bullets_y[i] = 500;
                            }
                            break;
                        case FACE_RIGHT:
                            bullets_y[i] += BULLET_SPEED;
                            if (!(bullets_y[i] >= 480-BULLET_SPEED || game_map[(bullets_x[i]+BULLET_SIZE)/20][(bullets_y[i]+BULLET_SIZE)/20] == 'X' || game_map[(bullets_x[i])/20][(bullets_y[i]+BULLET_SIZE)/20] == 'X')) { //bullet doesn't collide with wall
                                USART_Transmit('s');
                                USART_Transmit((char)((2 << 4) | i)); //send bullet index and direction of travel
                                USART_Transmit((char)(bullets_y[i] >> 8));
                                USART_Transmit((char)(bullets_y[i]));
                            }
                            else { //kill bullet
                                USART_Transmit('s');
                                USART_Transmit((char)(0 << 4) | i);
                                bullets_movement[i] = 5;
                                bullets_x[i] = 500;
                                bullets_y[i] = 500;
                            }
                            break;
                    }
                    for (unsigned int j = 0; j < max_enemies; j++) {//check to see if bullet hit enemy
                        if (alive[j] && bullets_x[i] >= enemies_x[j] - 3 && bullets_x[i] <= enemies_x[j] + ENEMY_SIZE + 3) {
                            if (bullets_y[i] >= enemies_y[j] - 3 && bullets_y[i] <= enemies_y[j] + ENEMY_SIZE + 3) {
                                --alive[j];

                                USART_Transmit('s'); //kill bullet
                                USART_Transmit((char)(0 << 4) | i);
                                bullets_movement[i] = 5;
                                bullets_x[i] = 500;
                                bullets_y[i] = 500;
                            }
                        }
                    }
                }
            }
        default: break;
    }

    return state;
}

int bulletPollSMTick(int state) {
    static unsigned char i;
    switch (state) {
        case bullet_poll_start:
            i = 0;
            if (C0) {
                state = bullet_poll_wait;
            }
            break;
        case bullet_poll_wait: 
            i = 0;
            if (A) { state = bullet_poll_hold; shoot = 1; }
            if (game_over) state = start;
            break;
        case bullet_poll_hold:
            if (i >= 4) {
                state = bullet_poll_wait;
            }
            break;
        default:
            state = bullet_poll_start;  
            break;
    }

    switch (state) {
        case bullet_poll_start: break;
        case bullet_poll_wait: 
            shoot = 0; 
            break;
        case bullet_poll_hold: 
            ++i;
            break;
        default: break;
    }
    return state;
}

int rotationPollSMTick(int state) {
    static unsigned char i;
    switch (state) {
        case rotation_poll_start:
            i = 0;
            if (C0) {
                state = rotation_poll_wait;
            }
            break;
        case rotation_poll_wait:
            if (game_over) state = start;
            if (L) {
                if (player_rotation == 0) {
                    player_rotation = 3;
                }
                else {
                    player_rotation--;
                }
                USART_Transmit('f');
                USART_Transmit(player_rotation);
                state = rotation_poll_hold_L;
            }
            if (R) {
                player_rotation = (player_rotation+1) % 4;
                USART_Transmit('f');
                USART_Transmit(player_rotation);
                state = rotation_poll_hold_R;
            }
            i = 0;
            break;
        case rotation_poll_hold_L:
            if (i > 5 || !L) {
                state = rotation_poll_wait;
            }
            break;
        case rotation_poll_hold_R:
            if (i > 5 || !R) {
                state = rotation_poll_wait;
            }
            break;
        default:
            state = rotation_poll_start;
            break;

    }
    switch (state) {
        case rotation_poll_start: i = 0; break;
        case rotation_poll_wait: i = 0; break;
        case rotation_poll_hold_L:
            ++i;
            break;
        case rotation_poll_hold_R:
            ++i;
            break;
        default: break;
    }
    return state;
}

void get_controller_data() {
    PORTA = PORTA | DATA_LATCH_HIGH; // set data latch high
    _delay_us(12);
    PORTA = PORTA & DATA_LATCH_LOW; // set data latch low
    _delay_us(6);

    for (unsigned char i = 0; i < 16; ++i) {
        PORTA = PORTA & CLOCK_LOW;
        _delay_us(6);

        // if (i <= 11) { 
        //     buttons[i] = SERIAL_DATA;
        // }
        if (i <= 11) {
            if (SERIAL_DATA) buttons = buttons | (1 << i); 
            else buttons = buttons & ~(1 << i);
        }
        
        PORTA = PORTA | CLOCK_HIGH; 
        _delay_us(6);
    }


}

void delay_us(unsigned int us) { 
    for (unsigned int i = 0; i < us; i++) {
        asm("nop");
    }
}

void calc_closest_s_node() {
    short player_i = player_x / 20.0;
    short player_j = player_y / 20.0;

    unsigned int min = 9999;
    unsigned char min_s;
    for (unsigned int i = 0; i < num_source_nodes; ++i) {
        int a = player_i - s_nodes[i].i;
        int b = player_j - s_nodes[i].j;
        unsigned int temp = sqrt(a*a + b*b); //pythagorean theorem
        if (temp < min) {
            min = temp;
            min_s = i;
        }
    } 
    player_s_node = min_s;
}
