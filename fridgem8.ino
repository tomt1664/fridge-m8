// the regular Adafruit "TouchScreen.h" library only works on AVRs

// different mcufriend shields have Touchscreen on different pins
// and rotation.
// Run the UTouch_calibr_kbv sketch for calibration of your shield

#include <Adafruit_GFX.h>    // Core graphics library
#include <Fonts/FreeSans12pt7b.h>
//#include <Adafruit_TFTLCD.h> // Hardware-specific library
//Adafruit_TFTLCD tft(A3, A2, A1, A0, A4);
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;       // hard-wired for UNO shields anyway.
#include <TouchScreen.h>


#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

// most mcufriend shields use these pins and Portrait mode:
uint8_t YP = A1;  // must be an analog pin, use "An" notation!
uint8_t XM = A2;  // must be an analog pin, use "An" notation!
uint8_t YM = 7;   // can be a digital pin
uint8_t XP = 6;   // can be a digital pin

uint16_t TS_LEFT = 880;
uint16_t TS_RT = 210;
uint16_t TS_TOP = 930;
uint16_t TS_BOT = 120;

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0

// names for some 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x73DF
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define MINPRESSURE 10
#define MAXPRESSURE 1000

int16_t HBUT, WBUT, WABUT;
int16_t BORD = 3;
int16_t RAD = 4;
int16_t PENRADIUS = 3;
uint16_t oldcolor, currentcolor;

bool bt[48] = { };
String bts[48];
String scanlab;
int numbt = 0;
bool doscan = 0;
bool domore = 0;
bool dohome = 1;
bool domilk = 0;
bool scansuc = 0;
bool scanadd = 0;
int notgot;
int lptm;

//milk levels
float skim_vol = 1.3;
float whole_vol = 0.7;

// function to draw an item button
void itembutton(int ix, int iy,String sitem,bool b_on) {
  if(b_on){
    tft.fillRoundRect(BORD+ix*WBUT,BORD+iy*HBUT,WBUT-2*BORD,HBUT-2*BORD,RAD,RED);  
  } else {
    tft.fillRoundRect(BORD+ix*WBUT,BORD+iy*HBUT,WBUT-2*BORD,HBUT-2*BORD,RAD,CYAN);
  }
// place text in centre of button
  int tsize = sitem.length();
  int tlength = tsize*5;
  tft.setCursor(ix*WBUT+WBUT/2-tlength,32+HBUT*iy);
  if(b_on){
    tft.setTextColor(WHITE);       
  } else {
    tft.setTextColor(BLACK);
  }
  tft.setTextSize(1);
  tft.println(sitem);
}


// function to draw an action button
void actionbutton(int ix,String sitem,bool b_on) {
  if(b_on){
    tft.fillRoundRect(12+ix*WABUT,12+8*HBUT,WABUT-2*12,HBUT*1.4-2*12,RAD,RED);  
  } else {
    tft.fillRoundRect(12+ix*WABUT,12+8*HBUT,WABUT-2*12,HBUT*1.4-2*12,RAD,BLUE);
  }
// place text in centre of button
  int tsize = sitem.length();
  int tlength = tsize*6;
  tft.setCursor(ix*WABUT+WABUT/2-tlength,42+HBUT*8);
  if(b_on){
    tft.setTextColor(WHITE);       
  } else {
    tft.setTextColor(BLACK);
  }
  tft.setTextSize(1);
  tft.println(sitem);
}


// scan screen
void scanscreen() {

  tft.fillScreen(BLACK);
  //draw the scan screen
  tft.fillRect(40,40,2*WBUT-2*40,400,CYAN);  
  tft.setCursor(92,85);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.println("Scan Product");
  tft.setCursor(115,120);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.println("Barcode");
  //barcode icon
  tft.drawLine(105,150,105,220,BLACK);
  tft.drawLine(106,150,106,220,BLACK);
  tft.drawLine(110,150,110,220,BLACK);
  tft.drawLine(113,150,113,220,BLACK);
  tft.drawLine(119,150,119,220,BLACK);
  tft.drawLine(122,150,122,220,BLACK);
  tft.drawLine(123,150,123,220,BLACK);        
  tft.drawLine(127,150,127,220,BLACK);
  tft.drawLine(128,150,128,220,BLACK);        
  tft.drawLine(133,150,133,220,BLACK);
  tft.drawLine(135,150,135,220,BLACK);
  
  tft.drawLine(215,150,215,220,BLACK);


}

// scan screen
void showmilk() {
  tft.fillScreen(BLACK);
  //draw the scan screen
  tft.setCursor(105,70);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.println("Milk Levels");

  tft.setCursor(45,135);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.println("Skimmed");

  tft.setCursor(185,135);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.println("Whole");

  //milk boxes
  tft.drawRect(50,160,90,181,CYAN);
  tft.drawRect(170,160,90,181,CYAN);
  int skmht = (int) 180*(skim_vol/2.3);
  int whlht = (int) 180*(whole_vol/2.3);
  if(skmht > 180) skmht = 180;
  if(whlht > 180) whlht = 180;
  Serial.println(skim_vol);
  Serial.println(whole_vol);
  Serial.println(skmht);
  Serial.println(whlht);
  tft.fillRect(51,160+(180-skmht),88,skmht,RED);
  tft.fillRect(171,160+(180-whlht),88,whlht,BLUE);

  //milk values
  tft.setCursor(60,380);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print(skim_vol);
  tft.println(" L");

  tft.setCursor(184,380);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print(whole_vol);
  tft.println(" L");    

  // back button (or wait 1 minute)
  actionbutton(1,"Back",0);  

  lptm = millis();
  notgot = 1;
  while(millis() - lptm < 60000 && notgot) {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      // scale screen mappings
      p.x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
      p.y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());

      if (p.x > WABUT && p.x < WABUT*2 && p.y > HBUT*8) {
        actionbutton(1,"Back",1);
        notgot = 0;
        delay(200);
      }   
    }
  }

}


void setup(void)
{
    Serial.begin(9600);
    Serial.println(F("NSSM Buttons"));


    tft.reset();
    uint16_t identifier = tft.readID();

    ts = TouchScreen(XP, YP, XM, YM, 300);     //call the constructor AGAIN with new values.

    tft.setFont(&FreeSans12pt7b);

    tft.begin(identifier);

    HBUT = tft.height()/10 + 2;
    WBUT = tft.width()/2;
    WABUT = tft.width()/3;
    BORD = 3;

    tft.fillScreen(BLACK);

    numbt = 48;

    for(int ibt = 0; ibt < numbt ;ibt++){
      bt[ibt] = 0;
    }

//define the button values

    bts[0] = "Skim Milk";
    bts[1] = "Whl Milk";   
    bts[2] = "Eggs";
    bts[3] = "Bread";
    bts[4] = "Butter";
    bts[5] = "Cheese";
    bts[6] = "Lettuce";
    bts[7] = "Tomatoes";
    bts[8] = "Courgette";
    bts[9] = "Cabbage";   
    bts[10] = "Peppers";
    bts[11] = "Corn-Cob";
    bts[12] = "Carrots";
    bts[13] = "New pots";
    bts[14] = "Bake pots";
    bts[15] = "Apples";
    bts[16] = "Banannas";
    bts[17] = "Pears";
    bts[18] = "Strawberries";
    bts[19] = "Blueberries";
    bts[20] = "Pasta";
    bts[21] = "Beans";
    bts[22] = "Chop Toms";
    bts[23] = "Noodles";
    bts[24] = "SF Veg";
    bts[25] = "Soy Sauce";
    bts[26] = "Soup";
    bts[27] = "Tuna";
    bts[28] = "Olives";
    bts[29] = "Frylight";
    bts[30] = "Crackers";
    bts[31] = "Rice";
    bts[32] = "Child Yog";
    bts[33] = "Mull Lghts";  
    bts[34] = "Tea";
    bts[35] = "Decaf Tea";
    bts[36] = "Coffee";       
    bts[37] = "Beer";
    bts[38] = "Red Wine";           
    bts[39] = "White Wine";
    bts[40] = "Vitamins";  
    bts[41] = "Painkillers";
    bts[42] = "Lemsip";
    bts[43] = "Wash Liq";
    bts[44] = "Liqui-tabs";
    bts[45] = "Softener";      
    bts[46] = "Vanish";
    bts[47] = "Spray";
            
// draw the buttons

    for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
// do columns and rows
      int ic = ibt % 2;
      int ir = ibt / 2;
      itembutton(ic,ir,bts[ibt],bt[ibt]);
    }
    actionbutton(0,"Scan",0);
    actionbutton(1,"Milk",0);
    actionbutton(2,"More",0);
    
    delay(1000);
}

void loop()
{
    int tmp;
    TSPoint p = ts.getPoint();

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    if(doscan) {
      scanscreen();
      doscan = 0;
      
      // redraw home screen
      tft.fillScreen(BLACK);
        for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
        // do columns and rows
        int ic = ibt % 2;
        int ir = ibt / 2;
        itembutton(ic,ir,bts[ibt],bt[ibt]);
      }
      actionbutton(0,"Scan",0);
      actionbutton(1,"Milk",0);
      actionbutton(2,"More",0);
      
    } else if(domore == 1) {

      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        // scale screen mappings
        p.x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
        p.y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
        // loop over all buttons to check for press
            // item buttons
            for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
              int ic = ibt % 2;
              int ir = ibt / 2;
              if(p.x > WBUT*ic && p.x < WBUT*(ic+1) && p.y > HBUT*ir && p.y < HBUT*(ir+1)) {
                if(bt[ibt+16]){
                  bt[ibt+16] = 0;
                  itembutton(ic,ir,bts[ibt+16],bt[ibt+16]);
                } else {
                  bt[ibt+16] = 1;
                  itembutton(ic,ir,bts[ibt+16],bt[ibt+16]);
                }
                delay(200);
              }
            }
            // action buttons
            if(p.x < WABUT && p.y > HBUT*8) {
               actionbutton(0,"Back",1);
               dohome = 1;
               domore = 0;
               delay(200);
               // redraw home screen
               tft.fillScreen(BLACK);
               for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
               // do columns and rows
                int ic = ibt % 2;
                int ir = ibt / 2;
                itembutton(ic,ir,bts[ibt],bt[ibt]);
              }
              actionbutton(0,"Scan",0);
              actionbutton(1,"Milk",0);
              actionbutton(2,"More",0);
               
            } else if (p.x > WABUT && p.x < WABUT*2 && p.y > HBUT*8) {
               actionbutton(1,"Milk",1);
               domilk = 1;
               delay(200);
            } else if (p.x > WABUT*2 && p.y > HBUT*8) {
               actionbutton(2,"More",1);
               domore = 2;
               dohome = 0;
               delay(200);

               // draw the more screen
               tft.fillScreen(BLACK);
               for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
               // do columns and rows
                 int ic = ibt % 2;
                 int ir = ibt / 2;
                 itembutton(ic,ir,bts[ibt+32],bt[ibt+32]);
              }
              actionbutton(0,"Back",0);
              actionbutton(1,"Milk",0);
              actionbutton(2,"Home",0);    

            }
      }
      
      
    } else if(domore == 2) {

      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        // scale screen mappings
        p.x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
        p.y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
        // loop over all buttons to check for press
            // item buttons
            for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
              int ic = ibt % 2;
              int ir = ibt / 2;
              if(p.x > WBUT*ic && p.x < WBUT*(ic+1) && p.y > HBUT*ir && p.y < HBUT*(ir+1)) {
                if(bt[ibt+32]){
                  bt[ibt+32] = 0;
                  itembutton(ic,ir,bts[ibt+32],bt[ibt+32]);
                } else {
                  bt[ibt+32] = 1;
                  itembutton(ic,ir,bts[ibt+32],bt[ibt+32]);
                }
                delay(200);
              }
            }
            // action buttons
            if(p.x < WABUT && p.y > HBUT*8) {
               actionbutton(0,"Back",1);
               domore = 1;
               delay(200);

           // draw the more screen
              tft.fillScreen(BLACK);
              for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
             // do columns and rows
                int ic = ibt % 2;
                int ir = ibt / 2;
                itembutton(ic,ir,bts[ibt+16],bt[ibt+16]);
              }
              actionbutton(0,"Back",0);
              actionbutton(1,"Milk",0);
              actionbutton(2,"More",0);    
            }               
               
            } else if (p.x > WABUT && p.x < WABUT*2 && p.y > HBUT*8) {
               actionbutton(1,"Milk",1);
               domilk = 1;
               delay(200);
            } else if (p.x > WABUT*2 && p.y > HBUT*8) {
               actionbutton(2,"Home",1);
               domore = 0;
               dohome = 1;
               delay(200);

               // redraw home screen
               tft.fillScreen(BLACK);
               for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
               // do columns and rows
                int ic = ibt % 2;
                int ir = ibt / 2;
                itembutton(ic,ir,bts[ibt],bt[ibt]);
              }
              actionbutton(0,"Scan",0);
              actionbutton(1,"Milk",0);
              actionbutton(2,"More",0);               
            }
      
    } else if(domilk) {
      showmilk();
      domilk = 0;
      dohome = 1;

      Serial.println("here");

      // redraw home screen
      tft.fillScreen(BLACK);
        for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
        // do columns and rows
        int ic = ibt % 2;
        int ir = ibt / 2;
        itembutton(ic,ir,bts[ibt],bt[ibt]);
      }
      actionbutton(0,"Scan",0);
      actionbutton(1,"Milk",0);
      actionbutton(2,"More",0);      

    } else if(dohome) {
      
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        // scale screen mappings
        p.x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
        p.y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
        // loop over all buttons to check for press
            // item buttons
            for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
              int ic = ibt % 2;
              int ir = ibt / 2;
              if(p.x > WBUT*ic && p.x < WBUT*(ic+1) && p.y > HBUT*ir && p.y < HBUT*(ir+1)) {
                if(bt[ibt]){
                  bt[ibt] = 0;
                  itembutton(ic,ir,bts[ibt],bt[ibt]);
                } else {
                  bt[ibt] = 1;
                  itembutton(ic,ir,bts[ibt],bt[ibt]);
                }
                delay(200);
              }
            }
            // action buttons
            if(p.x < WABUT && p.y > HBUT*8) {
               actionbutton(0,"Scan",1);
               doscan = 1;
               delay(200);
            } else if (p.x > WABUT && p.x < WABUT*2 && p.y > HBUT*8) {
               actionbutton(1,"Milk",1);
               domilk = 1;
               delay(200);
            } else if (p.x > WABUT*2 && p.y > HBUT*8) {
               actionbutton(2,"More",1);
               domore = 1;
               delay(200);

           // draw the more screen
              tft.fillScreen(BLACK);
              for(int ibt = 0; ibt < numbt && ibt < 16; ibt++){
             // do columns and rows
                int ic = ibt % 2;
                int ir = ibt / 2;
                itembutton(ic,ir,bts[ibt+16],bt[ibt+16]);
              }
              actionbutton(0,"Back",0);
              actionbutton(1,"Milk",0);
              actionbutton(2,"More",0);    
            }
      }
    }

    
}

