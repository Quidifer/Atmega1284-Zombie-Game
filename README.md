# Atmega1284-Zombie-Game

Video Link to project: https://www.youtube.com/watch?v=Ndl2lxBM0yg

## Project Description: 
The goal of this project is to make a prototype/playable version of a simple zombie game on 3.5” TFT LCD display. The LCD displays a map that the player can traverse. The player can shoot zombies that follow them around the map. The zombies utilize SSSP on several source nodes to traverse the map and follow the player. If the player gets hit by a zombie, they die.

## Build Upons: 
*	SNES game controller
*	USART (Baud – 9600)
*	3.5” TFT LCD display
*	Pathfinding for zombies
*	Collision Detection - Player runs into wall or zombie. Bullet hits wall or zombie. Zombies are smart enough to not run into walls (see below).

## User Guide:
### Rules:
Shoot and run away from zombies before they get to you. Every wave spawns more zombies with more health. Fewer zombies means faster zombies so be careful!
### Controls:
A – Shoot <br />
L – Rotate counterclockwise<br />
R – Rotate clockwise<br />
D-Pad – Player Movement<br />

##Increasing Runtime
The game logic pushes the limits of an 8 MGhz microcontroller, so I brainstormed many ways to improve the runtime:
###Condensed Map Size
The screen is 320x480 pixels. I condensed the map into a 16x24 (20x20 pixel segments) game map to improve the runtime on the Atmega. 
Note: all logic is run on the Atmega. The Arduino acts a servant to the Atmega. It receives positional data (via USART) to draw on the display. The Arduino only tells the Atmega when its ready for the game to start, which is done via I/O pins.
###About Pathfinding
Running Dijkstra’s SSSP on the microcontroller severely ruins the runtime, so it was best run it via computer using c++. My c++ program runs SSSP and uploads the results to a header file that is included in the Atmega’s c program. Even after condensing, the game map is still to large to run SSSP on every single node. This is why I strategically chose highway nodes. For my game, I chose 14 of them. This means that I have a shortest path from every node to all highway nodes. The enemies (zombies) follow this shortest path to the highway node that is closest to the player. If they see the player in their vision along the way, they will go directly after the player in ATTACK mode. If the reach the highway node without seeing the player, they will also go into ATTACK mode. During ATTACK mode, if the zombie tries moving into a wall, it will go back to following the highway (I call it ROAM mode).
