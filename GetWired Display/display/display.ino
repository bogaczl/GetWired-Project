// Enable debug prints to serial monitor
//#define MY_DEBUG
#define MY_RS485
#define MY_RS485_DE_PIN 12
#define MY_RS485_BAUD_RATE 57600
#define MY_RS485_HWSERIAL Serial
#define MY_RS485_SOH_COUNT 3
#define MY_NODE_ID 15
#define INIT_DELAY 200 

#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>
#include <MySensors.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

// Original values
#define TS_MINX 183
#define TS_MINY 190
#define TS_MAXX 940
#define TS_MAXY 790

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET 13

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Initialize motion message
MyMessage msgBin(MY_NODE_ID, V_STATUS);
MyMessage msgText(MY_NODE_ID, V_TEXT);

bool initConfirm = false;
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void before() {
  uint32_t InitDelay = MY_NODE_ID * INIT_DELAY;
  wait(InitDelay);

  tft.reset();
  tft.begin(0x9341); 
  tft.setRotation(0);
  drawBorder();
  // Initial screen
  tft.fillScreen(BLACK);
  tft.setCursor (55, 50);
  tft.setTextSize (3);
  tft.setTextColor(RED);
  tft.println("Test");
  delay(5000);
}

void presentation()
{
     // Send the sketch version information to the gateway and Controller
    sendSketchInfo("Arduino", "1.0");
    present(1, S_BINARY, "Night Mode");
    present(2, S_INFO, "Configure");
}

void receive(const MyMessage &message) {
  //if (message.type==V_TEXT) {
   ;  
  //}
  tft.fillScreen(BLACK);
  tft.setCursor (55, 50);
  tft.setTextSize (3);
  tft.setTextColor(GREEN);
  tft.print(message.type);
  tft.print("  ");
  tft.println(message.sensor);
}

void initConfirmation() {
  
    send(msgBin.setSensor(1).set(0));
    request(1, V_STATUS);
    wait(2000, C_SET, V_STATUS);
     
    send(msgText.set("-"));
    request(2, V_TEXT);
    wait(2000, C_SET, V_TEXT);
}

void loop()
{
    if (!initConfirm)  {
      initConfirmation();
      initConfirm=true;
    }
    //send(msgText.set("test"));
    delay(5000);
}

void drawBorder () {

  // Draw a border

  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;

  tft.fillScreen(RED);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);
  
}
