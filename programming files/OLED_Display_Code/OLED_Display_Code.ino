#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define GRAPH_X_MIN 4
#define GRAPH_X_MAX 48

#define GRAPH_Y_MIN 3
#define GRAPH_Y_MAX 27

int y = 0;
int x = GRAPH_X_MIN;

int maxValue = 0;

char freqBuffer[20];
char maxBuffer[20];

int prevYCoordinates[45] = {27};

//Limits are reversed (lower digit is higher on screen)

//graph pixel limits:
/*
   4, 3

   4, 27        48, 27
*/


//Constructor for OLED screen
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

void graphPlotter(int x);

void setup(void)
{
  pinMode(A0, INPUT);
  Serial.begin(9600);
  u8g2.begin();
  u8g2.clearBuffer(); // clear the internal memory
}

void loop(void)
{
  u8g2.clearBuffer(); // clear the internal memory
  //Screen code

  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);

  //Draw the axis for the graph
  u8g2.drawLine(3, 3, 3, 28);
  u8g2.drawLine(3, 28, 48, 28);

  graphPlotter(GRAPH_X_MIN);

  u8g2.sendBuffer(); // transfer internal memory to the display
}

void graphPlotter(int x)
{
  int currentY;
  int potValue;
  for (int i = 0; i <= 44; i++)
  {
    potValue = analogRead(A0);
    currentY = map(potValue, 0, 1024, 27, 3);
    prevYCoordinates[i+1] = currentY;
    u8g2.drawLine(i+3, prevYCoordinates[i], i+4, prevYCoordinates[i+1]); //x0, y0, x1, y1
    delay(10);
  }

  if (potValue > maxValue)
  {
    maxValue = potValue;
  }

  sprintf(maxBuffer, "Max: %d", maxValue);
  sprintf(freqBuffer, "Freq: %d", potValue);

  //Shows the frequency in digits for user
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(60, 12, freqBuffer);
  u8g2.drawStr(60, 25, maxBuffer);
}
