#define LCD_RD   A0
#define LCD_WR   A1     
#define LCD_RS   A2        
#define LCD_CS   A3       
#define LCD_REST A4

#define WR_MASK B00000010
#define RS_MASK B00000100

#define PLAYER_SIZE 10
#define MAP_SIZE 100

#define FACE_UP      0
#define FACE_RIGHT   1
#define FACE_DOWN    2
#define FACE_LEFT    3

#define NUM_BULLETS  10
#define BULLET_SIZE  4

#define ENEMY_SIZE   9
#define NUM_ENEMIES  7

//--------------shared variables --------------------------------
unsigned char data;
unsigned short player_x = 150;
unsigned short player_y = 150;
unsigned char player_rotation = 0;

unsigned short bullets_x[NUM_BULLETS] = {0};
unsigned short bullets_y[NUM_BULLETS] = {0};

unsigned short enemies_x[NUM_ENEMIES] = {0};
unsigned short enemies_y[NUM_ENEMIES] = {0};
//---------------------------------------------------------------

void Lcd_Writ_Bus(unsigned char d)
{
  PORTH &= ~(0x78);
  PORTH |= ((d&0xC0) >> 3) | ((d&0x3) << 5);
  PORTE &= ~(0x38);
  PORTE |= ((d & 0xC) << 2) | ((d & 0x20) >> 2);
  PORTG &= ~(0x20);
  PORTG |= (d & 0x10) << 1;  
  PORTF &= ~WR_MASK;
  PORTF |=  WR_MASK;
}


void Lcd_Write_Com(unsigned char VH)  
{   
  PORTF &= ~RS_MASK;//RS=0
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Data(unsigned char VH)
{
  PORTF |= RS_MASK;//LCD_RS=1;
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Com_Data(unsigned char com,unsigned char dat)
{
  Lcd_Write_Com(com);
  Lcd_Write_Data(dat);
}

void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
        Lcd_Write_Com(0x2a);
	Lcd_Write_Data(x1>>8);
	Lcd_Write_Data(x1);
	Lcd_Write_Data(x2>>8);
	Lcd_Write_Data(x2);
        Lcd_Write_Com(0x2b);
	Lcd_Write_Data(y1>>8);
	Lcd_Write_Data(y1);
	Lcd_Write_Data(y2>>8);
	Lcd_Write_Data(y2);
	Lcd_Write_Com(0x2c); 							 
}

void Lcd_Init(void)
{
  digitalWrite(LCD_REST,HIGH);
  delay(5); 
  digitalWrite(LCD_REST,LOW);
  delay(15);
  digitalWrite(LCD_REST,HIGH);
  delay(15);

  digitalWrite(LCD_CS,HIGH);
  digitalWrite(LCD_WR,HIGH);
  digitalWrite(LCD_CS,LOW);  //CS
  Lcd_Write_Com(0x11);
  delay(20);
  Lcd_Write_Com(0xD0);
  Lcd_Write_Data(0x07);
  Lcd_Write_Data(0x42);
  Lcd_Write_Data(0x18);
  
  Lcd_Write_Com(0xD1);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x07);//07
  Lcd_Write_Data(0x10);
  
  Lcd_Write_Com(0xD2);
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0x02);
  
  Lcd_Write_Com(0xC0);
  Lcd_Write_Data(0x10);
  Lcd_Write_Data(0x3B);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x02);
  Lcd_Write_Data(0x11);
  
  Lcd_Write_Com(0xC5);
  Lcd_Write_Data(0x03);
  Lcd_Write_Com(0x36);
  Lcd_Write_Data(0x0A);
  
  Lcd_Write_Com(0x3A);
  Lcd_Write_Data(0x55);
  
  Lcd_Write_Com(0x2A);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0x3F);
  
  Lcd_Write_Com(0x2B);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0xE0);
  delay(120);
  Lcd_Write_Com(0x29);
  Lcd_Write_Com(0x002c); 
}

void filled_square(unsigned int x, unsigned int y, unsigned int size, unsigned int c) {
    unsigned int i,m;
    
    Lcd_Write_Com(0x02c); //write_memory_start
    digitalWrite(LCD_RS,HIGH);
    digitalWrite(LCD_CS,LOW);

    Address_set(x,y,x+size,y+size);


    for(i=x;i<x+size;i++) {
        for(m=y;m<y+size;m++) {
            Lcd_Write_Data(c>>8);
            Lcd_Write_Data(c);
        }
    }
  digitalWrite(LCD_CS,HIGH);  
}

void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)                   
{	
  unsigned int i,j;
  Lcd_Write_Com(0x02c); //write_memory_start
  digitalWrite(LCD_RS,HIGH);
  digitalWrite(LCD_CS,LOW);
  l=l+x;
  Address_set(x,y,l,y);
  j=l*2;
  for(i=1;i<=j;i++)
  {
    Lcd_Write_Data(c);
  }
  digitalWrite(LCD_CS,HIGH);   
}

void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)                   
{	
  unsigned int i,j;
  Lcd_Write_Com(0x02c); //write_memory_start
  digitalWrite(LCD_RS,HIGH);
  digitalWrite(LCD_CS,LOW);
  l=l+y;
  Address_set(x,y,x,l);
  j=l*2;
  for(i=1;i<=j;i++)
  { 
    Lcd_Write_Data(c);
  }
  digitalWrite(LCD_CS,HIGH);   
}

// x, y, width, height, color
void Rect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}

void Rectf(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  unsigned int i;
  for(i=0;i<h;i++)
  {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
}
int RGB(int r,int g,int b)
{
  return (r << 16 | g << 8 | b);
}
void LCD_Clear(unsigned int j)                   
{	
    unsigned int i,m;
    Address_set(0,0,320,480);
    //Lcd_Write_Com(0x02c); //write_memory_start
    //digitalWrite(LCD_RS,HIGH);
    digitalWrite(LCD_CS,LOW);


    for(i=0;i<320;i++)
        for(m=0;m<480;m++)
        {
            Lcd_Write_Data(j>>8);
            Lcd_Write_Data(j);

        }
    digitalWrite(LCD_CS,HIGH);   
}

void draw_gun() {
  switch (player_rotation) {
    case FACE_UP:
      // V_line(player_x-1, player_y+3,3,c);
      // V_line(player_x-2, player_y+3,3,c);
      // V_line(player_x-3, player_y+3,3,c);

      Rectf(player_x, player_y+3, 4, 4, 0x07E0);
      break;
    case FACE_DOWN:
      // V_line(player_x+PLAYER_SIZE+1, player_y+3,3,c);
      // V_line(player_x+PLAYER_SIZE+2, player_y+3,3,c);
      // V_line(player_x+PLAYER_SIZE+3, player_y+3,3,c);

      Rectf(player_x+PLAYER_SIZE-4, player_y+3, 4, 4, 0x07E0);
      break;
    case FACE_LEFT:
      Rectf(player_x+3, player_y, 4, 4, 0x07E0);
      break;
    case FACE_RIGHT:
      Rectf(player_x+3, player_y+PLAYER_SIZE-5, 4, 4, 0x07E0);
      break;
  }
}

void setup()
{/*
  for(int p=0;p<10;p++)
  {
    pinMode(p,OUTPUT);
  }*/
  DDRH |= 0x78;
  DDRE |= 0x38;
  DDRG |= 0x20;
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  pinMode(A4,OUTPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  Lcd_Init();
  //LCD_Clear(0xf800);
  Serial.begin(9600);
  Serial2.begin(9600);

  pinMode(53,OUTPUT); //READY pin
  digitalWrite(53,LOW);

}

#define WAIT_UART 4000

void loop()
{  
  Serial.println("start");
  LCD_Clear(0x0000);
  unsigned char playing = 1; //reset
  
  // Rectf(40, 320, 120, 120, RGB(0xFF, 0xFF, 0xFF));
  //320 x 480 gets reduced to 12x24
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

  for (unsigned int i = 0; i < 16; ++i) {
    for (unsigned int j = 0; j < 24; ++j) {
      if (game_map[i][j] == 'X') {
        Rectf(i*20, j*20, 20, 20, 0x000f);
      }
    }
  }

  Serial.println("end");
  digitalWrite(53,HIGH); // READY for game to start.
  // Rect(20, 20, ENEMY_SIZE, ENEMY_SIZE, 0x07E0); //draw new

  // 0x07E0 <- red
  // 0x0010 <- blue
  // 0x2222 <- dark green
  // 0x00f0 <- purple
  // 0x000f <- map color

  while(playing) {
    // int x = random(310);
    // int y = random(470);
    // filled_square(x,y,10,RGB(0xFF,0xFF,0xFF));
    // delay(50);
    // filled_square(x,y,10,RGB(0x0,0x0,0x0));
    int i = 0;
    
    if (Serial2.available()) {
          data = Serial2.read();
          //Serial.println(data);
          if (data == 'x') {
            unsigned short old_x = player_x;
            for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
            data = Serial2.read();
            player_x = (player_x & 0x00FF) | data << 8;
            for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
            data = Serial2.read();
            player_x = (player_x & 0xFF00) | data;

            filled_square(old_x, player_y, PLAYER_SIZE, RGB(0x0,0x0,0x0)); //delete old
            filled_square(player_x, player_y, PLAYER_SIZE, RGB(0xFF,0xFF,0xFF)); //draw new
            draw_gun();
          }

          else if (data == 'y') {
            unsigned short old_y = player_y;
            for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
            data = Serial2.read();
            player_y = (player_y & 0x00FF) | data << 8;
            for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
            data = Serial2.read();
            player_y = (player_y & 0xFF00) | data;

            filled_square(player_x, old_y, PLAYER_SIZE, RGB(0x0,0x0,0x0)); //delete old
            filled_square(player_x, player_y, PLAYER_SIZE, RGB(0xFF,0xFF,0xFF)); //draw new
            draw_gun();
          }

          else if (data == 'f') { //changing rotation of player
            for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
            player_rotation = Serial2.read();
            filled_square(player_x, player_y, PLAYER_SIZE, RGB(0x0,0x0,0x0)); //delete old
            filled_square(player_x, player_y, PLAYER_SIZE, RGB(0xFF,0xFF,0xFF)); //draw new
            draw_gun();
          }

          else if (data == 's')  { //shooting bullets
            unsigned char index;
            unsigned char direction;
            for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
            data = Serial2.read(); //which bullet is updating
            index = data & 0x0F;
            direction = (data >> 4);

            if (direction == 0) { //delete values
              filled_square(bullets_x[index], bullets_y[index], BULLET_SIZE, 0x0); //delete old
            }
            if (direction == 1) { //direction is 1 if x changed. direction is 0 if y changed.
              filled_square(bullets_x[index], bullets_y[index], BULLET_SIZE, 0x0); //delete old
              for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue; //update x values
              data = Serial2.read();
              bullets_x[index] = (bullets_x[index] & 0x00FF) | data << 8;
              for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
              data = Serial2.read();
              bullets_x[index] = (bullets_x[index] & 0xFF00) | data;
              filled_square(bullets_x[index], bullets_y[index], BULLET_SIZE, 0x00f0); //draw new bullet
            }
            else if (direction == 2) {
              filled_square(bullets_x[index], bullets_y[index], BULLET_SIZE, 0x0); //delete old
              for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue; //update y values
              data = Serial2.read();
              bullets_y[index] = (bullets_y[index] & 0x00FF) | data << 8;
              for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
              data = Serial2.read();
              bullets_y[index] = (bullets_y[index] & 0xFF00) | data;
              filled_square(bullets_x[index], bullets_y[index], BULLET_SIZE, 0x00f0); //draw new bullet
            }
            else if (direction == 3) { //spawn new bullet
              for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue; //update x values
              data = Serial2.read();
              bullets_x[index] = (bullets_x[index] & 0x00FF) | data << 8;
              for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
              data = Serial2.read();
              bullets_x[index] = (bullets_x[index] & 0xFF00) | data;
              
              for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue; //update y values
              data = Serial2.read();
              bullets_y[index] = (bullets_y[index] & 0x00FF) | data << 8;
              for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
              data = Serial2.read();
              bullets_y[index] = (bullets_y[index] & 0xFF00) | data;

              filled_square(bullets_x[index], bullets_y[index], BULLET_SIZE, 0x00f0); //draw new bullet
            } 
          }

          else if (data == 'e') {
              //Serial.println("spawn enemy");
              unsigned char index;
              unsigned char type;
              for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
              data = Serial2.read(); 
              index = data & 0x0F; //which enemy is updating
              type = (data >> 4); //type of character update

              if (type == 0) { //spawn a new enemy no need to delete old values
                for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue; //update x values
                data = Serial2.read();
                enemies_x[index] = (enemies_x[index] & 0x00FF) | data << 8;
                for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
                data = Serial2.read();
                enemies_x[index] = (enemies_x[index] & 0xFF00) | data;

                for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue; //update y values
                data = Serial2.read();
                enemies_y[index] = (enemies_y[index] & 0x00FF) | data << 8;
                for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
                data = Serial2.read();
                enemies_y[index] = (enemies_y[index] & 0xFF00) | data;

                Rect(enemies_x[index], enemies_y[index], ENEMY_SIZE, ENEMY_SIZE, 0x07E0);
              }
              else if (type == 1) { //enemy moved in the x direction
                Rect(enemies_x[index], enemies_y[index], ENEMY_SIZE, ENEMY_SIZE, 0x0); //delete old

                for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue; //update x values
                data = Serial2.read();
                enemies_x[index] = (enemies_x[index] & 0x00FF) | data << 8;
                for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
                data = Serial2.read();
                enemies_x[index] = (enemies_x[index] & 0xFF00) | data;

                Rect(enemies_x[index], enemies_y[index], ENEMY_SIZE, ENEMY_SIZE, 0x07E0); //draw new
              }
              else if (type == 2) { //enemy moved in the y direction
                Rect(enemies_x[index], enemies_y[index], ENEMY_SIZE, ENEMY_SIZE, 0x0); //delete old

                for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue; //update y values
                data = Serial2.read();
                enemies_y[index] = (enemies_y[index] & 0x00FF) | data << 8;
                for (i = 0; !Serial2.available() && i < WAIT_UART; i++) {} if (i == WAIT_UART) continue;
                data = Serial2.read();
                enemies_y[index] = (enemies_y[index] & 0xFF00) | data;

                Rect(enemies_x[index], enemies_y[index], ENEMY_SIZE, ENEMY_SIZE, 0x07E0); //draw new
              }
              else if (type == 3) { //delete enemy hes dead :(
                Rect(enemies_x[index], enemies_y[index], ENEMY_SIZE, ENEMY_SIZE, 0x0); //delete old
                enemies_x[index] = 0;
                enemies_y[index] = 0;
              }
          }
          else if (data == 'd') { //death
            while (1) {
              Rect(rand() % 320, rand() % 480, ENEMY_SIZE, ENEMY_SIZE, 0x07E0);
            }
          }
          else {
              Serial.print("unknown data: ");
              Serial.println(data);
          }
    } 
  }
}