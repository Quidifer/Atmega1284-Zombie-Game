# Atmega1284-Zombie-Game

Video Demonstration of Project: https://www.youtube.com/watch?v=Ndl2lxBM0yg

## Project Description: 
The goal of this project is to make a prototype/playable version of a simple zombie game on 3.5” TFT LCD display. The LCD displays a map that the player can traverse. The player can shoot zombies that follow them around the map. The zombies utilize SSSP on several source nodes to traverse the map and follow the player. If the player gets hit by a zombie, they die.

## Build Upons: 
*	SNES game controller
*	USART (Baud – 9600)
*	3.5” TFT LCD display
*	Pathfinding for zombies
*	Collision Detection - Player runs into wall or zombie. Bullet hits wall or zombie. Zombies are smart enough to not run into walls (see below).

# User Guide:
### Rules:
Shoot and run away from zombies before they get to you. Every wave spawns more zombies with more health. Fewer zombies means faster zombies so be careful!
### Controls:
A – Shoot <br />
L – Rotate counterclockwise<br />
R – Rotate clockwise<br />
D-Pad – Player Movement<br />

# Increasing Runtime
The game logic pushes the limits of an 8 MGhz microcontroller, so I brainstormed many ways to improve the runtime:
### Condensed Map Size
The screen is 320x480 pixels. I condensed the map into a 16x24 (20x20 pixel segments) game map to improve the runtime on the Atmega. 
Note: all logic is runs on the Atmega. The Arduino acts a servant to the Atmega. It receives positional data (via USART) to draw on the display. The Arduino only tells the Atmega when it is ready for the game to start, which is done via I/O pins.
### About Pathfinding
Running Dijkstra’s SSSP on the microcontroller severely ruins the runtime, so it was best run it via computer using c++. My c++ program runs SSSP and uploads the results to a header file that is included in the Atmega’s c program. Even after condensing, the game map is still to large to run SSSP on every single node. This is why I strategically chose highway nodes. For my game, I chose 14 of them. This means that I have a shortest path from every node to all highway nodes. The enemies (zombies) follow this shortest path to the highway node that is closest to the player. If they see the player in their vision along the way, they will go directly after the player in ATTACK mode. If the reach the highway node without seeing the player, they will also go into ATTACK mode. During ATTACK mode, if the zombie tries moving into a wall, it will go back to following the highway (I call it ROAM mode).

# Specifics
### SNES Controller
Sends a 12-bit data frame when commanded by microcontroller. The SNES game console runs at 3.579545 MHz. This means we must delay our microcontroller properly to work with the controller. The Data Latch is normally set to low. We pulse it HIGH for 2 clock cycles to request Serial Data. We oscillate the clock signal at 3.579545 MHz as we are receiving data from the data line. Each bit of the frame (1 or 0) tells whether one of the buttons on the controller is pressed or not. Each bit of the frame is set to a global short variable called “buttons”.
### TFT LCD
Writes data using SPI. Every time an object in the game gets updated, the Arduino fills the old pixels with color 0x0, and the new pixels with the color of the object.
### USART
Both the Arduino and Atmega are set up to a baud rate of 9600. The Atmega sends a 1-byte data value that specifies the upcoming data. The following bytes update object location on the screen. If the object moved, the Arduino first fills the old pixels, then writes the new ones.
### Dijkstra’s Algorithm / SSSP
Coded in c++. Dijkstra’s algorithm is a greedy algorithm. Starting at the source node, it attempts to relax all neighbor nodes. If a neighbor is successfully relaxed (relaxed when taking that path is less than the path before it), it is pushed to a priority queue. Then, pop() from the priority queue and repeat the same steps. Continue this process until the priority queue is empty. <br /> <br />
Uses fstream to write to a header file with the results of the SSSP.  Uses the results to navigate the map and take optimal routes to the player.
### Collision Detection
Only the player and bullets require collision detection. Zombies are smart enough to never run into walls due to SSSP. As described above, we have a 16x24 game map of pixel segments. Since the pixel segments are 20x20 pixels each, it is easy to map the player and bullets to the game map. If the player or a bullet attempts to go into a wall (described as ‘X’ in the game map), then we destroy the bullet / hold the player still. It is also important to check each corner of the bullet/player since 1 corner might be in 1 pixel segment while another corner could be another segment.


