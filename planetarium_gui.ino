//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
// Calculation the Positions of Planets by Circle Orbit in the Solarsystem between Year 2000-2100
// all results are tested with Stellarium on Linux :-)
// Author: Andreas Jahnke, aajahnke@aol.com
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).
// Assign human-readable names to some common 16-bit color values:
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFBE0
#define GRAY        0x7BEF
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGRAY   0xC618
#define DARKGRAY    0x7BEF
#define GREENYELLOW 0xAFE5

unsigned int foreground_color;
unsigned int background_color;
unsigned int earthground_color;
unsigned int text_color;


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;

float object_position[8][4] = {// azimuth, altitude, distance, bodysize
  {30, 10, 0, 2},     // Mercury
  {60, 30, 0, 2},     // Venus
  {90, 20, 0, 3},     // Sun
  {120, 60, 0, 2},    // Mars
  {180, 90, 0, 2},    // Jupiter
  {240, 60, 0, 2},    // Saturn
  {270, 45, 0, 2},    // Uranus
  {300, 30, 0, 2},    // Neptun
};

int tft_position[8][2] = {//x, y
  { -20, 0},    // Mercury
  { -20, 0},    // Venus
  { -20, 0},    // Sun
  { -20, 0},    // Mars
  { -20, 0},    // Jupiter
  { -20, 0},    // Saturn
  { -20, 0},    // Uranus
  { -20, 0},    // Neptun
};

//Version:
String sw_version = "Version: 0.1 Beta";

//Display Size:
int x_size = 240;
int y_size = 320;
//--------------------------------------------------------------------------------------------------------------
void setup() {

  tft.reset();
  tft.begin(0x9341);
  tft.fillScreen(BLACK);
  ScreenText(WHITE, 0, 0, 1 , sw_version);
  //draw_star_map(200);

  //delay(3000);
  tft.fillScreen(BLACK);
  Serial.begin(9600);
  Serial.println(sw_version);

  gui_planetarium();
}
//--------------------------------------------------------------------------------------------------------------
void loop() {


}
//--------------------------------------------------------------------------------------------------------------
void gui_planetarium() {

  color_set(object_position[2][1]);
  draw_coord_net();
  draw_Information();
  draw_star_map(1);
  draw_object(0);
  draw_object(1);
  draw_object(2);
  draw_object(3);
  draw_object(4);
  draw_object(5);
  draw_object(6);
  draw_object(7);
}
//--------------------------------------------------------------------------------------------------------------
void draw_coord_net() {

  //horizontal
  SetLines(foreground_color, 0, y_size / 2 - 90 , x_size - 20, y_size / 2 - 90 );  // 90 deg:zenith
  SetLines(foreground_color, 0, y_size / 2 - 60 , x_size - 20, y_size / 2 - 60 );  // 60 deg
  SetLines(foreground_color, 0, y_size / 2 - 30 , x_size - 20, y_size / 2 - 30 );  // 30 deg
  SetLines(foreground_color, 0, y_size / 2 , x_size - 1, y_size / 2 );            //  0 deg:horizon
  //vertical
  SetLines(foreground_color, 0, y_size / 2 , 0, y_size / 2 - 90 );                //   0 deg:azimuth
  SetLines(foreground_color, 60, y_size / 2 , 60, y_size / 2 - 90 );              //  90 deg:azimuth
  SetLines(foreground_color, 119, y_size / 2 , 119, y_size / 2 - 90 );            // 180 deg:azimuth
  SetLines(foreground_color, 179, y_size / 2 , 179, y_size / 2 - 90 );            // 270 deg:azimuth
  SetLines(foreground_color, 239, y_size / 2 , 239, y_size / 2 - 90 );            // 359 deg:azimuth
  //notes horizontal
  ScreenText(foreground_color, x_size - 15, y_size / 2 - 90, 1 , "90");
  ScreenText(foreground_color, x_size - 15, y_size / 2 - 60, 1 , "60");
  ScreenText(foreground_color, x_size - 15, y_size / 2 - 30, 1 , "30");
  //ScreenText(foreground_color, x_size - 15, y_size / 2, 1 , "0");
  //notes vertical
  ScreenText(foreground_color, 0 + 2, y_size / 2 - 100, 1 , "N");
  ScreenText(foreground_color, 60, y_size / 2 - 100, 1 , "O");
  ScreenText(foreground_color, 119, y_size / 2 - 100, 1 , "S");
  ScreenText(foreground_color, 179, y_size / 2 - 100, 1 , "W");
  ScreenText(foreground_color, 239 - 8, y_size / 2 - 100, 1 , "N");
  //earth ground
  SetFilledRect(earthground_color , 0,  y_size / 2 + 1, 240, y_size / 2);
}
//--------------------------------------------------------------------------------------------------------------
void draw_object(int number) {

  float x_factor = 1;
  float y_factor = 0.663888889;
  float azimuth = object_position[number][0];// azimuth, altitude, distance, bodysize
  float altitude = object_position[number][1];
  int body_size = (int)object_position[number][3];

  int x = (int)(azimuth * y_factor); //239=360deg
  int y = (int)(altitude * x_factor);
  y = (y_size / 2) - y;

  if ( tft_position[number][0] != x || tft_position[number][1] != y) {
    SetFilledCircle(BLACK , x , y , body_size);
    tft_position[number][0] = x;
    tft_position[number][1] = y;
  }

  if (altitude >= 0) {
    if (number == 0)SetFilledCircle(WHITE , x , y , body_size);        // Set object
    if (number == 1)SetFilledCircle(GREENYELLOW , x , y , body_size);
    if (number == 2)SetFilledCircle(YELLOW , x , y , body_size);
    if (number == 3)SetFilledCircle(RED , x , y , body_size);
    if (number == 4)SetFilledCircle(MAROON , x , y , body_size);
    if (number == 5)SetFilledCircle(MAGENTA , x , y , body_size);
    if (number == 6)SetFilledCircle(BLUE , x , y , body_size);
    if (number == 7)SetFilledCircle(PURPLE , x , y , body_size);
  }
  else {
    SetFilledCircle(GRAY , x , y , body_size);        // reset object
  }
}
//--------------------------------------------------------------------------------------------------------------
void color_set(float sun_altitude) {

  unsigned int color = background_color;

  if (sun_altitude >= 0) {
    foreground_color = BLACK;
    background_color = WHITE;
    earthground_color = GREEN;
    text_color = BLACK;
  }
  if (sun_altitude >= 0 && sun_altitude <= 3) {
    foreground_color = WHITE;
    background_color = BLUE;
    earthground_color = DARKGREEN;
    text_color = WHITE;
  }
  if (sun_altitude < 0) {
    foreground_color = GRAY;
    background_color = BLACK;
    earthground_color = GRAY;
    text_color = WHITE;
  }
  if (color != background_color)tft.fillScreen(background_color);
}
//--------------------------------------------------------------------------------------------------------------
void draw_star_map(int delay_) {

  for (int i = 0; i < 30; ++i) { // star map
    int randnumber_x = random(20, 220);
    int randnumber_y = random(20, 220);
    SetPoint(foreground_color, randnumber_x, randnumber_y - 55);
    delay(delay_);
  }
}
//--------------------------------------------------------------------------------------------------------------
void draw_Information() {// text info

}
//--------------------------------------------------------------------------------------------------------------
void ScreenText(uint16_t color, int xtpos, int ytpos, int text_size , String text) {
  tft.setCursor(xtpos, ytpos);
  tft.setTextColor(color);
  tft.setTextSize(text_size);
  tft.println(text);
}
//--------------------------------------------------------------------------------------------------------------
void SetLines(uint16_t color , int xl1pos, int yl1pos, int xl2pos, int yl2pos) {
  tft.drawLine(xl1pos, yl1pos, xl2pos, yl2pos, color);
}
//--------------------------------------------------------------------------------------------------------------
void SetPoint(uint16_t color, int xppos, int yppos) {
  tft.drawPixel(xppos, yppos, color);
}
//--------------------------------------------------------------------------------------------------------------
void SetRect(uint16_t color , int xr1pos, int yr1pos, int xr2width, int yr2hight) {
  tft.drawRect(xr1pos, yr1pos, xr2width, yr2hight, color);
}
//--------------------------------------------------------------------------------------------------------------
void SetFilledRect(uint16_t color , int xr1pos, int yr1pos, int xr2width, int yr2hight) {
  tft.fillRect(xr1pos, yr1pos, xr2width, yr2hight, color);
}
//--------------------------------------------------------------------------------------------------------------
void SetCircle(uint16_t color , int xcpos, int ycpos, int radius) {
  tft.drawCircle(xcpos, ycpos, radius, color);
}
//--------------------------------------------------------------------------------------------------------------
void SetFilledCircle(uint16_t color , int xcpos, int ycpos, int radius) {
  tft.fillCircle(xcpos, ycpos, radius, color);
}
//--------------------------------------------------------------------------------------------------------------
