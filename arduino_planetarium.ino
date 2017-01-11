//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
// Calculation the Positions of Planets by Circle Orbit in the Solarsystem between Year 2000-2100
// all results are tested with Stellarium on Linux :-)
// Author: Andreas Jahnke, aajahnke@aol.com
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
#include <Adafruit_GFX.h>    // Core graphics library
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
//--------------------------------------------
//------- DATE-TIME --------------------------
//--------------------------------------------
#include <Time.h>
#include <Timezone.h>
TimeChangeRule CEST = {"", Last, Sun, Mar, 2, 120};
TimeChangeRule CET = {"", Last, Sun, Oct, 3, 60};
Timezone CE(CEST, CET);
TimeChangeRule *tcr;

String Line = "";    // a string to hold incoming data
int daylightsavingtime = 1; // add hour 1=winter  2=sommer
int utc_hour;
boolean valid_sync = false;
boolean valid_signal = false;
boolean update_ = false;
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

// Sechs Bahnelemente:                                                                 / Variable:
// a: Länge der großen Halbachse                                                       / [0] semi major axis in AE
// e: numerische Exzentrizität                                                         / [2] eccentricity
// i: Bahnneigung, Inklination                                                         / [4] inclination
// L                                                                                   / [6] meanLongitude
// ω: Argument der Periapsis, Periapsisabstand                                         / [8] longitude of perihelion
// Ω: Länge/Rektaszension des aufsteigenden Knotens                                    / [10] longitude ascending node

// Tables:
String object_name[8] = {"Mercury", "Venus", "Sun", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};
//String star_name[1] = {"Sun"};

// http://ssd.jpl.nasa.gov/txt/aprx_pos_planets.pdf
const float object_data[8][12] = {// a, aΔ, e, eΔ, i, iΔ,  L, LΔ, ω, ωΔ, Ω, ΩΔ  >>> L2000
  {0.38709927, 0.00000037, 0.20563593, 0.00001906, 7.00497902, -0.00594749, 252.25032350, 149472.67411175, 77.45779628, 0.16047689, 48.33076593, -0.12534081},  // Mercury
  {0.72333566, 0.00000390, 0.00677672, -0.00004107, 3.39467605, -0.00078890, 181.97909950, 58517.81538729, 131.60246718, 0.00268329, 76.67984255, -0.27769418}, // Venus
  {1.00000261, 0.00000562, 0.01671123, -0.00004392, -0.00001531, -0.01294668, 100.46457166, 35999.37244981, 102.93768193, 0.32327364, 0, 0},                    // Earth
  {1.52371034, 0.00001847, 0.09339410, 0.00007882, 1.84969142, -0.00813131, -4.55343205, 19140.30268499, -23.94362959, 0.44441088, 49.55953891, -0.29257343},   // Mars
  {5.20288700, -0.00011607, 0.04838624, -0.00013253, 1.30439695, -0.00183714, 34.39644051, 3034.74612775, 14.72847983, 0.21252668, 100.47390909, 0.20469106},   // Jupiter
  {9.53667594, -0.00125060, 0.05386179, -0.00050991, 2.48599187, 0.00193609, 49.95424423, 1222.49362201, 92.59887831, -0.41897216, 113.66242448, -0.28867794},  // Saturn
  {19.1891646, -0.00196176, 0.04725744, -0.00004397, 0.77263783, -0.00242939, 313.23810451, 428.48202785, 170.95427630, 0.40805281, 074.01692503, 0.04240589},  // Uranus
  {30.06992276, 0.00026291, 0.00859048, 0.00005105, 1.77004347, 0.00035372, -55.12002969, 218.45945325, 44.96476227, -0.32241464, 131.78422574, -0.00508664},   // Neptun
};

// global factors:
const float rad = 0.017453293; // deg to rad
const float deg = 57.29577951; // rad to deg
const float pi = 3.1415926535; // PI
float jd;                      // Juliane date
float jd_frac;
float eclipticAngle = 23.43928;

//global coordinates:
float x_coord;
float y_coord;
float z_coord;

float x_earth;
float y_earth;
float z_earth;

float azimuth;
float altitude;

float lat; //GPS Position of Hamburg in deg
float lon;
int minute_lat;
int minute_lon;

float ra; //deg
float dec;
float dis;


//------------------------------------------------------------------------------------------------------------------

float object_position[8][4] = {// azimuth, altitude, distance, bodysize
  {0, 0, 0, 2},    // Mercury
  {0, 0, 0, 2},    // Venus
  {0, 0, 0, 4},    // Sun
  {0, 0, 0, 2},    // Mars
  {0, 0, 0, 3},    // Jupiter
  {0, 0, 0, 3},    // Saturn
  {0, 5, 0, 2},    // Uranus
  {0, 0, 0, 2},    // Neptun
};

int tft_position[8][2] = {//x, y
  { 0, 0},    // Mercury
  { 0, 0},    // Venus
  { 0, 0},    // Sun
  { 0, 0},    // Mars
  { 0, 0},    // Jupiter
  { 0, 0},    // Saturn
  { 0, 0},    // Uranus
  { 0, 0},    // Neptun
};

//Version:
String sw_version = "Version: 0.1 Beta";

//Display Size:
int x_size = 240;
int y_size = 320;
//--------------------------------------------------------------------------------------------------------------
void setup() {

  Line.reserve(100);
  tft.reset();
  tft.begin(0x9341);
  tft.fillScreen(BLACK);
  ScreenText(WHITE, 0, 0, 1 , sw_version);
  draw_star_map(200);

  delay(3000);
  tft.fillScreen(BLACK);
  Serial.begin(9600);
  Serial.println(sw_version);

}
//--------------------------------------------------------------------------------------------------------------
void loop() {

  static int os = -1;

  if (!(os % 4))if (getline(F("$GPRMC")))RMC();
  SerialClear();


  if (second() != os) { //every second

  }

  if (second() == 59) {
    utc_hour = hour() - daylightsavingtime;
    if (utc_hour < 0)utc_hour += 24;
    jd = get_julian_date (day(), month(), year(), utc_hour, minute(), second());//UTC
    //Serial.println("JD:" + String(jd, DEC) + "+" + String(jd_frac, DEC)); // jd = 2457761.375000;
    get_object_position (2, jd, jd_frac);//earth
    get_object_position (0, jd, jd_frac);
    get_object_position (1, jd, jd_frac);
    get_object_position (3, jd, jd_frac);
    get_object_position (4, jd, jd_frac);
    get_object_position (5, jd, jd_frac);
    get_object_position (6, jd, jd_frac);
    get_object_position (7, jd, jd_frac);
    update_ = true;
  }

  if (second() == 1) {
    if (valid_signal == true) {
      if ((lat > 0) && (lon > 0) && (lat < 90) && (lon < 180)) {
        if (update_ == true) {
          gui_planetarium();
          update_ = false;
        }
      }
    }
  }

  os = second();
}
//--------------------------------------------------------------------------------------------------------------
void gui_planetarium() {

  color_set(object_position[2][1]);
  //tft.fillScreen(background_color);
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
void RMC() { //TIME DATE

  setTime(getparam(1).substring(0, 0 + 2).toInt(),
          getparam(1).substring(2, 2 + 2).toInt(),
          getparam(1).substring(4, 4 + 2).toInt(),
          getparam(9).substring(0, 0 + 2).toInt(),
          getparam(9).substring(2, 2 + 2).toInt(),
          getparam(9).substring(4, 4 + 2).toInt());
  time_t cet = CE.toLocal(now(), &tcr);
  setTime(cet);

  if (CE.locIsDST(cet)) { //ask for DST=Daylight saving time
    daylightsavingtime = 2; //true = 2 hour
  }
  else {
    daylightsavingtime = 1;//false = 1 hour
  }

  if (getparam(2) == "A") { //valid GPS-signal  A/V
    valid_signal = true;
    lat = getparam(3).substring(0, 2).toInt();
    lon = getparam(5).substring(0, 3).toInt();
    minute_lat = getparam(3).substring(2, 4).toInt();//minute value
    minute_lon = getparam(5).substring(3, 5).toInt();//minute value
    if ((lat > 0) && (lon > 0) && (lat < 90) && (lon < 180)) {
      valid_sync = true;
      Serial.println("valid_sync");
    }
  }
  else {
    valid_signal = false;
  }
}
//----------------------------------------------
//--------------RS232-ROUTINEN------------------
//----------------------------------------------
void SerialClear() {
  while (Serial.available())Serial.read();
}
//----------------------------------------------
boolean getline(String phrase) { //HARD POLLING

  char s[100];//simu: $GPRMC,205733,A,5335.5634,N,01039.3538,E,0.0,0.0,110117,0.4,E,A*19
  byte b, n;
  unsigned long t = millis();

  for (unsigned int i = 0; i < sizeof(s); i++) {
    s[i] = 0;
  }
  Line = "";
  do {
    b = Serial.read();
    if (millis() > (t + 100))return false;
  }
  while (b != '$');

  s[0] = b;
  n = Serial.readBytesUntil('\n', &s[1], 90);
  s[n] = 0;

  for (int i = 0; i < n; i++) {
    Line += s[i];
  }
  Serial.println(Line);
  int index = Line.indexOf(phrase);
  if (index > -1) {
    //Serial.println("found:" + phrase);
    //Serial.println(Line);
    return true;
  }
  return false;
}

#define hex(i)  ((i<=9) ? ('0'+i): ('A'- 10 + i))
//----------------------------------------------
boolean checksum() {
  byte b = 0; int e;
  e = Line.indexOf('*');
  if (e > 10)
  { for (int i = 1; i < e; i++)b ^= Line[i];
    if ((hex((b & 15)) == Line[e + 2]) &&
        (hex(b / 16) == Line[e + 1]))return true;
  }
  return false;
}
//----------------------------------------------
String getparam(int ix) {
  int c, cc = 0;
  // if (checksum()) {
  do
  { c = Line.indexOf(',', cc);
    if (c >= 0)cc = c + 1; else break;
  } while (--ix);
  return (Line.substring(c + 1, Line.indexOf(',', c + 1)));
  //}
  return F("xx"); //debug
}
//--------------------------------------------------------------------------------------------------------------
void draw_coord_net() {

  SetFilledRect(background_color , 0,  y_size / 2 - 89, 240, 89);
  //horizontal
  SetLines(foreground_color, 0, y_size / 2 - 90 , x_size - 20, y_size / 2 - 90 );  // 90 deg:zenith
  SetLines(foreground_color, 0, y_size / 2 - 60 , x_size - 20, y_size / 2 - 60 );  // 60 deg
  SetLines(foreground_color, 0, y_size / 2 - 30 , x_size - 20, y_size / 2 - 30 );  // 30 deg
  SetLines(foreground_color, 0, y_size / 2 , x_size - 1, y_size / 2 );             //  0 deg:horizon
  SetLines(foreground_color, 0, y_size / 2 + 90 , x_size, y_size / 2 + 90 );       // -90 deg:down
  //vertical
  SetLines(foreground_color, 0, y_size / 2  , 0, y_size / 2 - 90 );               //   0 deg:azimuth
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
  SetFilledRect(earthground_color , 0,  y_size / 2 + 1, 240, 89);
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

  //if ( tft_position[number][0] != x || tft_position[number][1] != y) {
  //SetFilledCircle(BLACK , x , y , body_size);
  tft_position[number][0] = x;
  tft_position[number][1] = y;
  //}

  if (altitude >= 0) {
    if (number == 0)SetFilledCircle(WHITE , x , y , body_size);        // Set object
    if (number == 1)SetFilledCircle(GREENYELLOW , x , y , body_size);
    if (number == 2)SetFilledCircle(YELLOW , x , y , body_size);
    if (number == 3)SetFilledCircle(RED , x , y , body_size);
    if (number == 4)SetFilledCircle(MAROON , x , y , body_size);
    if (number == 5) {
      SetFilledCircle(MAGENTA , x , y , body_size);
      SetLines(BLACK , x - 4, y, x + 4, y);
    }
    if (number == 6)SetFilledCircle(BLUE , x , y , body_size);
    if (number == 7)SetFilledCircle(PURPLE , x , y , body_size);
  }
  else {
    SetFilledCircle(BLACK , x , y , body_size);
    if (number == 5)SetLines(BLACK , x - 4, y, x + 4, y);
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

  float alt = object_position[2][1];//Sun
  if (alt < 0) {
    for (int i = 0; i < 30; ++i) { // star map
      int randnumber_x = random(5, 235);
      int randnumber_y = random(72, 155);
      SetPoint(foreground_color, randnumber_x, randnumber_y);
      delay(delay_);
    }
  }
}
//--------------------------------------------------------------------------------------------------------------
void draw_Information() {// text info

  char s[20];
  sprintf(s, "%02u.%02u.%04u   %02u:%02u", day(), month(), year(), hour(), minute());
  SetFilledRect(background_color , 5, 0, 120, 10);
  ScreenText(text_color, 5, 5, 1 , s);

  float az = object_position[2][0];
  float alt = object_position[2][1];
  SetFilledRect(background_color , 5, 255, 120, 10);
  ScreenText(text_color, 5, 260, 1 , "Sun: " + String(az, 1) + " / " + String(alt, 1));

  //raise next object:
  for (int i = 0 ; i < 8; i++) {
    float alt_object = object_position[i][1];
    if (alt_object < 0 && alt_object > -20) {
      float az_object = object_position[i][0];
      if (az_object > 0 && az_object < 180) {
        SetFilledRect(background_color, 5, 270, 120, 10);
        ScreenText(text_color, 5, 275, 1 , object_name[i]  + ": next raise");
        break;
      }
    }
  }
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
//##############################################################################################################
//##############################################################################################################
float get_julian_date (float day_, float month_, float year_, float hour_, float minute_, float seconds_) { // UTC

  if (month_ <= 2) {
    year_ -= 1;
    month_ += 12;
  }

  long A = year_ / 100;
  long B = A / 4;
  long C = 2 - A + B;
  long E = 365.25 * (year_ + 4716);
  long F = 30.6001 * (month_ + 1);
  jd = C + day_ + E + F - 1524.5;
  jd_frac = (hour_ / 24) + (minute_ / 1440) + (seconds_ / 86400);
  return jd;
}
//------------------------------------------------------------------------------------------------------------------
// =========================================================================
// object position
// =========================================================================
void get_object_position (int object_number, float jd, float jd_frac) {

  //Serial.println(F("----------------------------------------------------"));
  //Serial.println("Object:" + object_name[object_number]);

  float T = jd - 2451545;
  T += jd_frac;
  T /= 36525;
  //Serial.println("T:" + String(T, DEC));

  float sidereal_time = calc_siderealTime (jd, jd_frac, lon);
  //Serial.println("ST:" + String(sidereal_time, DEC));

  float semiMajorAxis = object_data[object_number][0] + (T * object_data[object_number][1]); // offset + T * delta
  float eccentricity = object_data[object_number][2] + (T * object_data[object_number][3]);
  float inclination = object_data[object_number][4] + (T * object_data[object_number][5]);
  float meanLongitude = object_data[object_number][6] + (T * object_data[object_number][7]);
  float longitudePerihelion = object_data[object_number][8] + (T * object_data[object_number][9]);
  float longitudeAscendingNode = object_data[object_number][10] + (T * object_data[object_number][11]);
  float meanAnomaly = meanLongitude - longitudePerihelion;
  float argumentPerihelion = longitudePerihelion - longitudeAscendingNode;

  //Serial.println("semiMajorAxis:" + String(semiMajorAxis, DEC));
  //Serial.println("eccentricity:" + String(eccentricity, DEC));

  inclination = calc_format_angle_deg (inclination);
  //Serial.println("inclination:" + String(inclination, DEC));

  meanLongitude = calc_format_angle_deg (meanLongitude);
  //Serial.println("meanLongitude:" + String(meanLongitude, DEC));

  longitudePerihelion = calc_format_angle_deg (longitudePerihelion);
  //Serial.println("longitudePerihelion:" + String(longitudePerihelion, DEC));

  longitudeAscendingNode = calc_format_angle_deg (longitudeAscendingNode);
  //Serial.println("longitudeAscendingNode:" + String(longitudeAscendingNode, DEC));

  meanAnomaly = calc_format_angle_deg (meanAnomaly);
  //Serial.println("meanAnomaly:" + String(meanAnomaly, DEC));

  argumentPerihelion = calc_format_angle_deg (argumentPerihelion);
  //Serial.println("argumentPerihelion:" + String(argumentPerihelion, DEC));
  //---------------------------------
  float eccentricAnomaly = calc_eccentricAnomaly(meanAnomaly, eccentricity);
  eccentricAnomaly = calc_format_angle_deg (eccentricAnomaly);
  //Serial.println("eccentricAnomaly:" + String(eccentricAnomaly, DEC));
  //---------------------------------
  //to orbital Coordinates:
  //Serial.println(F("orbital coordinates:"));
  calc_orbital_coordinates (semiMajorAxis, eccentricity, eccentricAnomaly);
  //---------------------------------
  //to heliocentric ecliptic coordinates:
  rot_z (argumentPerihelion);
  rot_x (inclination);
  rot_z (longitudeAscendingNode);
  //---------------------------------
  if (object_number == 2) {//object earth

    x_earth = x_coord;
    y_earth = y_coord;
    z_earth = z_coord;
    //---------------------------------
    //calc the sun position from earth:
    //Serial.println(F("geocentric ecliptic results of sun:"));
    calc_vector_subtract(x_earth, 0 , y_earth, 0, z_earth , 0);// earth - sun coordinates
    calc_vector(x_coord, y_coord, z_coord, "");
    //Serial.println(F("geocentric equatorial results of sun:"));
    rot_x (eclipticAngle);//rotate x > earth ecliptic angle
    calc_vector(x_coord, y_coord, z_coord, "");
    calc_azimuthal_position(ra, dec, lat, sidereal_time, object_number);
  }
  //---------------------------------
  if (object_number != 2) {//all other objects
    //Serial.println(F("geocentric ecliptic results of object:"));
    calc_vector_subtract(x_earth, x_coord , y_earth, y_coord, z_earth , z_coord);// earth - object coordinates
    calc_vector(x_coord, y_coord, z_coord, "");
    //Serial.println(F("geocentric equatorial results of object:"));
    rot_x (eclipticAngle);//rotate x > earth ecliptic angle
    calc_vector(x_coord, y_coord, z_coord, "");
    calc_azimuthal_position(ra, dec, lat, sidereal_time, object_number);
  }
}
//------------------------------------------------------------------------------------------------------------------
float calc_format_angle_deg (float deg) {  //0-360 degrees

  if (deg >= 360 || deg < 0) {
    if (deg < 0) {
      while (deg < 0) {
        deg += 360;
      }
    }
    long x = (long)deg;
    float comma = deg - x;
    long y = x % 360; //modulo 360
    return comma += y;
  }
  return deg;
}
//------------------------------------------------------------------------------------------------------------------
float calc_eccentricAnomaly (float meanAnomaly, float eccentricity) {

  meanAnomaly *= rad;

  int iterations = 0;
  float eccentricAnomaly = meanAnomaly + (eccentricity * sin(meanAnomaly));
  //Serial.println(String(eccentricAnomaly, DEC));
  float deltaEccentricAnomaly = 1;

  while (fabs(deltaEccentricAnomaly) > 0.000001) { // 0.0000001

    deltaEccentricAnomaly = (meanAnomaly - eccentricAnomaly + (eccentricity * sin(eccentricAnomaly))) / (1 - eccentricity * cos(eccentricAnomaly));
    //Serial.println(String(deltaEccentricAnomaly, DEC));
    eccentricAnomaly += deltaEccentricAnomaly;
    //Serial.println(String(eccentricAnomaly, DEC));

    iterations++;
    if (iterations > 20) {
      //Serial.println(F("Error:Keplergleichung!!!!!"));
      eccentricAnomaly = 0;
      break;
    }
  }
  eccentricAnomaly *= deg;
  return eccentricAnomaly;
}
//------------------------------------------------------------------------------------------------------------------
void calc_orbital_coordinates (float semiMajorAxis, float eccentricity, float eccentricAnomaly) {

  eccentricAnomaly *= rad;
  float true_Anomaly = 2 * atan(sqrt((1 + eccentricity) / (1 - eccentricity)) * tan(eccentricAnomaly / 2));
  true_Anomaly *= deg;
  true_Anomaly = calc_format_angle_deg (true_Anomaly);

  float radius = semiMajorAxis * (1 - (eccentricity * cos(eccentricAnomaly)));
  //Serial.println("true_Anomaly:" + String(true_Anomaly, DEC));
  //Serial.println("radius:" + String(radius, DEC));

  calc_vector(0, true_Anomaly, radius, "to_rectangular"); // x = beta / y = true_Anomaly / z = radius
}
//------------------------------------------------------------------------------------------------------------------
void calc_vector(float x, float y, float z, String mode) {

  // convert to rectangular coordinates:
  if (mode == F("to_rectangular")) {

    x *= rad;
    y *= rad;

    x_coord = z * cos(x) * cos(y);
    y_coord = z * cos(x) * sin(y);
    z_coord = z * sin(x);

    x = x_coord;
    y = y_coord;
    z = z_coord;
  }

  //  Serial.println("x_coord:" + String(x, DEC));
  //  Serial.println("y_coord:" + String(y, DEC));
  //  Serial.println("z_coord:" + String(z, DEC));

  // convert to spherical coordinates:
  //get Longitude:
  float lon = atan2(y, x);
  lon *= deg;
  lon = calc_format_angle_deg (lon);
  ra = lon;
  //Serial.println("LON:" + String(lon, DEC));
  format_angle(lon, F("degrees"));

  //get Latitude:
  float lat = atan2(z, (sqrt(x * x + y * y)));
  lat *= deg;
  lat = calc_format_angle_deg (lat);
  dec = lat;
  //Serial.println("LAT:" + String(lat, DEC));
  format_angle(lat, F("degrees-latitude"));

  //getDistance:
  float dist = sqrt(x * x + y * y + z * z);
  dis = dist;
  //Serial.println("DIS:" + String(dist, DEC));
}
//------------------------------------------------------------------------------------------------------------------
void format_angle(float angle, String format) {

  int d = 0;
  int m = 0;
  int s = 0;
  float rest = 0;
  String sign = "";

  if (format == F("degrees") || format == F("degrees-latitude")) {

    rest = calc_format_angle_deg (angle);

    if (format == F("degrees-latitude") && rest > 90) {
      rest -= 360;
    }
    if (rest >= 0) {
      sign = "+";
    }
    else {
      sign = "-";
    }

    rest = fabs(rest);
    d = (int)(rest);
    rest = (rest - (float)d) * 60;
    m = (int)(rest);
    rest = (rest - (float)m) * 60;
    s = (int)rest;
    //Serial.println(sign + String(d) + ":" + String(m) + ":" + String(s));
  }
}
//--------------------------------------------------------------------------------------------------------------------
void rot_x(float alpha) {

  alpha *= rad;
  float y = cos(alpha) * y_coord - sin(alpha) * z_coord;
  float z = sin(alpha) * y_coord + cos(alpha) * z_coord;
  y_coord = y;
  z_coord = z;
}
//------------------------------------------------------------------------------------------------------------------
void rot_y (float alpha) {

  alpha *= rad;
  float x = cos(alpha) * x_coord + sin(alpha) * z_coord;
  float z = sin(alpha) * x_coord + cos(alpha) * z_coord;
  x_coord = x;
  z_coord = z;
}
//------------------------------------------------------------------------------------------------------------------
void rot_z (float alpha) {

  alpha *= rad;
  float x = cos(alpha) * x_coord - sin(alpha) * y_coord;
  float y = sin(alpha) * x_coord + cos(alpha) * y_coord;
  x_coord = x;
  y_coord = y;
}
//------------------------------------------------------------------------------------------------------------------
void calc_vector_subtract(float xe, float xo, float ye, float yo, float ze, float zo) {

  x_coord = xo - xe;
  y_coord = yo - ye;
  z_coord = zo - ze;
}
//------------------------------------------------------------------------------------------------------------------
float calc_siderealTime (float jd, float jd_frac, float lon) { //03:50:00 = 2457761.375

  float T = jd - 2451545;
  T /= 36525;
  float UT = jd_frac * 24;
  float T0 = 6.697374558 + (2400.051336 * T) + (0.000025862 * T * T) + (UT * 1.0027379093);
  T0 = fmod(T0, 24);
  float siderial_time = T0 + (lon / 15);
  return siderial_time;
}
//------------------------------------------------------------------------------------------------------------------
void calc_azimuthal_position(float ra, float dec, float lat, float sidereal_time, int number) {

  float ha = (sidereal_time * 15) - ra; //ha = hours of angle  (-180 to 180 deg)
  if (ha < -180) ha += 360;
  if (ha > 180) ha -= 360;
  if (dec < -90) dec += 360;
  if (dec > 90) dec -= 360;

  ha *= rad;
  dec *= rad;
  lat *= rad;

  float x = cos(ha) * cos(dec);
  float y = sin(ha) * cos(dec);
  float z = sin(dec);

  //rotate y
  float x_hor = x * sin(lat) - z * cos(lat);//horizon position
  float y_hor = y;
  float z_hor = x * cos(lat) + z * sin(lat);

  azimuth = atan2(y_hor, x_hor) + pi;
  altitude = atan2(z_hor, sqrt(x_hor * x_hor + y_hor * y_hor));
  azimuth *= deg;//0=north, 90=east, 180=south, 270=west
  altitude *= deg;//0=horizon, 90=zenith, -90=down
  object_position[number][0] = azimuth; // azimuth, altitude, distance, bodysize
  object_position[number][1] = altitude;

  //Serial.println("azimuth:" + String(azimuth, DEC));
  //Serial.println("altitude:" + String(altitude, DEC));
  //Serial.println("distance:" + String(dis, DEC));
}
//------------------------------------------------------------------------------------------------------------------
