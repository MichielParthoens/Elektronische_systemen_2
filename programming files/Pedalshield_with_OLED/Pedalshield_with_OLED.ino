// Licensed under a Creative Commons Attribution 3.0 Unported License.
// Based on rcarduino.blogspot.com previous work.
// www.electrosmash.com/pedalshield
// code zou input signaal clean doorgeven aan de output

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

int in_ADC0, in_ADC1;  //variables for 2 ADCs values (ADC0, ADC1)
int POT0, POT1, POT2, out_DAC0, out_DAC1; //variables for 3 pots (ADC8, ADC9, ADC10)
int LED = 3;
int FOOTSWITCH = 7;
int TOGGLE = 2;

const int lowerlimit = 0;
const int upperlimit = 4000;

int maxValue = 0;

char freqBuffer[20];
char maxBuffer[20];

int prevYCoordinates[45] = {27};

//Constructor for OLED screen
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL1, /* data=*/ SDA1, /* reset=*/ U8X8_PIN_NONE);

void plotter();

void setup()
{
  //serial com for plotter
  Serial.begin(9600);
  //ADC Configuration
  ADC->ADC_MR |= 0x80;   // DAC in free running mode.
  ADC->ADC_CR = 2;       // Starts ADC conversion.
  ADC->ADC_CHER = 0x1CC0; // Enable ADC channels 0 and 1.

  //DAC Configuration
  analogWrite(DAC0, 0); // Enables DAC0
  analogWrite(DAC1, 0); // Enables DAC0

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

  plotter();

  u8g2.sendBuffer(); // transfer internal memory to the display
}

void plotter()
{
  int currentY;

  for (int i = 0; i <= 44; i++)
  {
    //Read the ADCs
    while ((ADC->ADC_ISR & 0x1CC0) != 0x1CC0); // wait for ADC 0, 1, 8, 9, 10 conversion complete.
    in_ADC0 = ADC->ADC_CDR[7];             // read data from ADC0
    in_ADC1 = ADC->ADC_CDR[6];             // read data from ADC1
    POT0 = ADC->ADC_CDR[10];               // read data from ADC8
    POT1 = ADC->ADC_CDR[11];               // read data from ADC9
    POT2 = ADC->ADC_CDR[12];               // read data from ADC10

    Serial.print("adc_0:");
    Serial.println(in_ADC0);

    //Add volume feature with POT2
    currentY = map(in_ADC0, 0, 4095, 27, 3);
    prevYCoordinates[i + 1] = currentY;
    u8g2.drawLine(i + 3, prevYCoordinates[i], i + 4, prevYCoordinates[i + 1]); //x0, y0, x1, y1
  }

  int currentFreq = in_ADC0 / 100;

  if (currentFreq > maxValue)
  {
    maxValue = currentFreq;
  }

  sprintf(maxBuffer, "Max: %d", maxValue);
  sprintf(freqBuffer, "Freq: %d", currentFreq);

  //Shows the frequency in digits for user
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(60, 12, freqBuffer);
  u8g2.drawStr(60, 25, maxBuffer);
}
