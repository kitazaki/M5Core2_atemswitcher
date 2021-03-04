// for ATEM Mini Extreme / Extreme ISO
// This code is based on
// https://github.com/aaronpk/m5-core2-atem-controller/
//
// Requirements
//
// Download Arduino Libraries from here
// https://github.com/kasperskaarhoj/SKAARHOJ-Open-Engineering/tree/master/ArduinoLibs
//
// 1. copy these folders to libraries directory
// ATEMbase
// ATEMmax
// ATEMstd
// SkaarhojPgmspace
//
// 2. change code in ATEMbase.cpp (1 change) and ATEMbase.h (2 changes)
// refer to: https://oneguyoneblog.com/2020/07/13/autopilot-for-blackmagic-design-atem-switcher/
//
//  from                  to
//  --------------------  --------------------
//  #ifdef ESP8266        WiFiUDP Udp;
//  WiFiUDP Udp;
//  #else
//  EthernetUDP Udp;
//  #endif

#include <M5Core2.h>
#include <Fonts/EVA_20px.h>
#include <stdio.h>
#include <WiFi.h>

#include <SkaarhojPgmspace.h>
#include <ATEMmax.h>
#include <ATEMstd.h>

// Define the IP address of your ATEM switcher
IPAddress switcherIp(0, 0, 0, 0);

// Put your wifi SSID and password here
const char* ssid = "";  // Wi-Fi SSID
const char* password =  "";  // Wi-Fi Password

#define CAMERA_OFF 0
#define CAMERA_PREVIEW 1
#define CAMERA_PROGRAM 2

#define CAMERA_BUTTON_Y 42
#define MACRO_BUTTON_Y 150

ATEMstd AtemSwitcher;

TouchButton c1 = TouchButton(0, CAMERA_BUTTON_Y, 40, 72, "c1");
TouchButton c2 = TouchButton(40, CAMERA_BUTTON_Y, 40, 72, "c2");
TouchButton c3 = TouchButton(40+40, CAMERA_BUTTON_Y, 40, 72, "c3");
TouchButton c4 = TouchButton(40+40+40, CAMERA_BUTTON_Y, 40, 72, "c4");
TouchButton c5 = TouchButton(40+40+40+40, CAMERA_BUTTON_Y, 40, 72, "c1");
TouchButton c6 = TouchButton(40+40+40+40+40, CAMERA_BUTTON_Y, 40, 72, "c6");
TouchButton c7 = TouchButton(40+40+40+40+40+40, CAMERA_BUTTON_Y, 40, 72, "c7");
TouchButton c8 = TouchButton(40+40+40+40+40+40+40, CAMERA_BUTTON_Y, 40, 72, "c8");

TouchButton b1 = TouchButton(0, MACRO_BUTTON_Y, 40, 72, "b1");
TouchButton b2 = TouchButton(40, MACRO_BUTTON_Y, 40, 72, "b2");
TouchButton b3 = TouchButton(40+40, MACRO_BUTTON_Y, 40, 72, "b3");
TouchButton b4 = TouchButton(40+40+40, MACRO_BUTTON_Y, 40, 72, "b4");
TouchButton b5 = TouchButton(40+40+40+40, MACRO_BUTTON_Y, 40, 72, "b5");
TouchButton b6 = TouchButton(40+40+40+40+40, MACRO_BUTTON_Y, 40, 72, "b6");
TouchButton b7 = TouchButton(40+40+40+40+40+40, MACRO_BUTTON_Y, 40, 72, "b7");
TouchButton b8 = TouchButton(40+40+40+40+40+40+40, MACRO_BUTTON_Y, 40, 72, "b8");

void vibration() {
  M5.Axp.SetLDOEnable(3, true);
  delay(50);
  M5.Axp.SetLDOEnable(3, false);
}

void buttonWasPressed(TouchEvent& e) {
  TouchButton& b = *e.button;

  char buttonType = b.name[0];
  int buttonNumber = b.name[1]-'0';

  Serial.printf("debug: buttontype = %c, buttonNumber = %d\n", buttonType, buttonNumber);
  vibration();
  
  if(buttonType == 'c') {
    AtemSwitcher.changeProgramInput(buttonNumber);
  }
  else if( buttonType == 'b') {
    drawButton(buttonNumber, b.isPressed(), false);
//    AtemSwitcher.setMacroAction(buttonNumber-1, 0); 
    AtemSwitcher.changePreviewInput(buttonNumber);
  }
  else {
    if(M5.BtnA.isPressed()) {
      AtemSwitcher.doAuto();
      Serial.println("debug: Button A is Pressed");
    }
    else if(M5.BtnB.isPressed()) {
      AtemSwitcher.doCut();
      Serial.println("debug: Button B is Pressed");
    }
    else if(M5.BtnC.isPressed()) {
      // do something
      Serial.println("debug: Button C is Pressed");
    }
  }
}

void drawCameraButton(int buttonNumber, int state) {
  Serial.printf("debug: buttonNumber = %d, state = %d\n", buttonNumber, state);

  int font = 1;
  M5.Lcd.setTextSize(6);
  
  M5.Lcd.setTextColor(state == CAMERA_OFF ? WHITE : BLACK);

  int buttonColor;
  switch(state) {
    case CAMERA_OFF:
      buttonColor = TFT_DARKGREY; break;
    case CAMERA_PREVIEW:
      buttonColor = TFT_GREEN; break;
    case CAMERA_PROGRAM:
      buttonColor = TFT_RED; break;
  }

  switch(buttonNumber) {
    case 1:
      M5.Lcd.fillRoundRect(0, CAMERA_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("1", 2+(40/2), CAMERA_BUTTON_Y+15, font);
      break;
    case 2:
      M5.Lcd.fillRoundRect(40, CAMERA_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("2", 2+40+(40/2), CAMERA_BUTTON_Y+15, font);
      break;
    case 3:
      M5.Lcd.fillRoundRect(40+40, CAMERA_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("3", 2+40+40+(40/2), CAMERA_BUTTON_Y+15, font);
      break;
    case 4:
      M5.Lcd.fillRoundRect(40+40+40, CAMERA_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("4", 2+40+40+40+(40/2), CAMERA_BUTTON_Y+15, font);
      break;
    case 5:
      M5.Lcd.fillRoundRect(40+40+40+40, CAMERA_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("5", 2+40+40+40+40+(40/2), CAMERA_BUTTON_Y+15, font);
      break;
    case 6:
      M5.Lcd.fillRoundRect(40+40+40+40+40, CAMERA_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("6", 2+40+40+40+40+40+(40/2), CAMERA_BUTTON_Y+15, font);
      break;
    case 7:
      M5.Lcd.fillRoundRect(40+40+40+40+40+40, CAMERA_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("7", 2+40+40+40+40+40+40+(40/2), CAMERA_BUTTON_Y+15, font);
      break;
    case 8:
      M5.Lcd.fillRoundRect(40+40+40+40+40+40+40, CAMERA_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("8", 2+40+40+40+40+40+40+40+(40/2), CAMERA_BUTTON_Y+15, font);
      break;
  }
}

void drawButton(int buttonNumber, bool isPressed, bool isRunning) {
  int font = 1;
  M5.Lcd.setTextSize(6);
  
  M5.Lcd.setTextColor(isPressed || isRunning ? BLACK : WHITE);

  int buttonColor = isPressed ? WHITE : (isRunning ? TFT_RED : TFT_DARKGREY);
  
  switch(buttonNumber) {
    case 1:
      M5.Lcd.fillRoundRect(0, MACRO_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("1", 2+(40/2), MACRO_BUTTON_Y+15, font);
      break;
    case 2:
      M5.Lcd.fillRoundRect(40, MACRO_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("2", 2+40+(40/2), MACRO_BUTTON_Y+15, font);
      break;
    case 3:
      M5.Lcd.fillRoundRect(40+40, MACRO_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("3", 2+40+40+(40/2), MACRO_BUTTON_Y+15, font);
      break;
    case 4:
      M5.Lcd.fillRoundRect(40+40+40, MACRO_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("4", 2+40+40+40+(40/2), MACRO_BUTTON_Y+15, font);
      break;
    case 5:
      M5.Lcd.fillRoundRect(40+40+40+40, MACRO_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("5", 2+40+40+40+40+(40/2), MACRO_BUTTON_Y+15, font);
      break;
    case 6:
      M5.Lcd.fillRoundRect(40+40+40+40+40, MACRO_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("6", 2+40+40+40+40+40+(40/2), MACRO_BUTTON_Y+15, font);
      break;
    case 7:
      M5.Lcd.fillRoundRect(40+40+40+40+40+40, MACRO_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("7", 2+40+40+40+40+40+40+(40/2), MACRO_BUTTON_Y+15, font);
      break;
    case 8:
      M5.Lcd.fillRoundRect(40+40+40+40+40+40+40, MACRO_BUTTON_Y, 39, 72, 6, buttonColor);
      M5.Lcd.drawCentreString("8", 2+40+40+40+40+40+40+40+(40/2), MACRO_BUTTON_Y+15, font);
      break;
  }
}

void setupButtons() {
  M5.Touch.addHandler(buttonWasPressed, TE_BTNONLY + TE_TOUCH + TE_RELEASE);

  M5.Lcd.fillScreen(BLACK);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.drawString("Program", 6, CAMERA_BUTTON_Y-20, 1);
  M5.Lcd.drawString("Preview", 6, MACRO_BUTTON_Y-20, 1);

  drawButton(1, false, false);
  drawButton(2, false, false);
  drawButton(3, false, false);
  drawButton(4, false, false);
  drawButton(5, false, false);
  drawButton(6, false, false);
  drawButton(7, false, false);
  drawButton(8, false, false);
  
  drawCameraButton(1, CAMERA_OFF);
  drawCameraButton(2, CAMERA_OFF);
  drawCameraButton(3, CAMERA_OFF);
  drawCameraButton(4, CAMERA_OFF);
  drawCameraButton(5, CAMERA_OFF);
  drawCameraButton(6, CAMERA_OFF);
  drawCameraButton(7, CAMERA_OFF);
  drawCameraButton(8, CAMERA_OFF);
}

void setup() {
  M5.begin(true, true, false, true);
  
  Serial.begin(115200);

  Serial.println("About to connect to wifi");
  WiFi.begin(ssid, password);

  int lpcnt=0 ;
  int lpcnt2=0 ;

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lpcnt += 1 ;                           //
    if (lpcnt > 6) {                       // 6回目(3秒) で切断/再接続
      WiFi.disconnect(true,true);          //
      WiFi.begin(ssid, password);    //
      lpcnt = 0 ;                          //
      lpcnt2 += 1 ;                        // 再接続の回数をカウント
    }                                      //
    if (lpcnt2 > 3) {                      // 3回 接続できなければ、
      ESP.restart() ;                      // ソフトウェアリセット
    }                  
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");

  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();
  Serial.println("Connected to AtemSwitcher");

  setupButtons();
}

int currentRunningMacro = -1;
int tallyNum = 8;
int tallyStates[8];

void loop() {
  M5.update();

  AtemSwitcher.runLoop();

  for(int i=1; i<=tallyNum; i++) {
    int currentTallyState = AtemSwitcher.getProgramTally(i) ? CAMERA_PROGRAM : (AtemSwitcher.getPreviewTally(i) ? CAMERA_PREVIEW : CAMERA_OFF);
    if(currentTallyState != tallyStates[i-1]) {
      drawCameraButton(i, currentTallyState);
    }
    tallyStates[i-1] = currentTallyState;
  }
}
