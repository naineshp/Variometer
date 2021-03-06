#include <scp1000.h>
#include <nokia_3310_lcd.h>

#include "scale2.h"
#include "scale3.h"
#include "pointer.h"


 
//keypad debounce parameter
#define DEBOUNCE_MAX 15
#define DEBOUNCE_ON  10
#define DEBOUNCE_OFF 3 

#define NUM_KEYS 5
#define NUM_MENU_ITEM	4

// joystick number
#define UP_KEY 3
#define LEFT_KEY 0
#define CENTER_KEY 1
#define DOWN_KEY 2
#define RIGHT_KEY 4

// menu starting points
#define MENU_X	10		// 0-83
#define MENU_Y	1		// 0-5

// define csb
#define selectPin 6
SCP1000 scp1000(selectPin);

// adc preset value, represent top value,incl. noise & margin,that the adc reads, when a key is pressed
// set noise & margin = 30 (0.15V@5V)
int  adc_key_val[5] ={
  30, 150, 360, 535, 760 };

// debounce counters
byte button_count[NUM_KEYS];
// button status - pressed/released
byte button_status[NUM_KEYS];
// button on flags for user program 
byte button_flag[NUM_KEYS];



// menu definition
char menu_items[NUM_MENU_ITEM][12]={
  " DATA      ",
  " IN_FLIGHT ",
  " STATS     ",
  " ABOUT     "
};

void (*menu_funcs[NUM_MENU_ITEM])(void) = {
  data,
  in_flight,
  stats,
  about,
};

char current_menu_item;


Nokia_3310_lcd lcd=Nokia_3310_lcd();




void setup()
{

  delay(1000);		// Allow SCP1000 to stabalize

  Serial.begin(9600); // Open serial connection to report values to host
  Serial.println("Starting up");

  scp1000.init();



  Serial.println("scp1000 init complete");

  // setup interrupt-driven keypad arrays  
  // reset button arrays
  for(byte i=0; i<NUM_KEYS; i++){
    button_count[i]=0;
    button_status[i]=0;
    button_flag[i]=0;             
  }

  // Setup timer2 -- Prescaler/256
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);
  TCCR2B = (1<<CS22)|(1<<CS21);      

  ASSR |=(0<<AS2);

  // Use normal mode  
  TCCR2A =0;    
  //Timer2 Overflow Interrupt Enable  
  TIMSK2 |= (0<<OCIE2A);
  TCNT2=0x6;  // counting starts from 6;  
  TIMSK2 = (1<<TOIE2);    


  SREG|=1<<SREG_I;

  lcd.LCD_3310_init();
  lcd.LCD_3310_clear();

  //menu initialization
  init_MENU();
  current_menu_item = 0;

  Serial.println("stage 1 complete");

}


/* loop -----------------------------------------------------------------*/

void loop()
{
  Serial.println("stage 2 begin");

  byte i;
  for(i=0; i<NUM_KEYS; i++){
    if(button_flag[i] !=0){

      button_flag[i]=0;  // reset button flag
      switch(i){

      case UP_KEY:
        // current item to normal display
        lcd.LCD_3310_write_string(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_NORMAL );
        current_menu_item -=1;
        if(current_menu_item <0)  current_menu_item = NUM_MENU_ITEM -1;
        // next item to highlight display
        lcd.LCD_3310_write_string(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_HIGHLIGHT );
        break;
      case DOWN_KEY:
        // current item to normal display
        lcd.LCD_3310_write_string(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_NORMAL );
        current_menu_item +=1;
        if(current_menu_item >(NUM_MENU_ITEM-1))  current_menu_item = 0;
        // next item to highlight display
        lcd.LCD_3310_write_string(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_HIGHLIGHT );
        break;
      case LEFT_KEY:
        init_MENU();
        current_menu_item = 0;
        break;   
      case RIGHT_KEY:
        lcd.LCD_3310_clear();
        (*menu_funcs[current_menu_item])();
        lcd.LCD_3310_clear();
        init_MENU();
        current_menu_item = 0;           
        break;	
      } 

    }
  }
delay(2000);                                              // remove when done
  
  Serial.println("stage 2 complete");

}

/* menu functions ---------------------------------  */

void init_MENU(void){

  byte i;

  lcd.LCD_3310_clear();

  lcd.LCD_3310_write_string(MENU_X, MENU_Y, menu_items[0], MENU_HIGHLIGHT );

  for (i=1; i<NUM_MENU_ITEM; i++){
    lcd.LCD_3310_write_string(MENU_X, MENU_Y+i, menu_items[i], MENU_NORMAL);
  }


}
//-------------------------------------------------------
// waiting for center key press
void waitfor_OKkey(){
  byte i;
  byte key = 0xFF;
  while (key!= LEFT_KEY){
    for(i=0; i<NUM_KEYS; i++){
      if(button_flag[i] !=0){
        button_flag[i]=0;  // reset button flag
        if(i== LEFT_KEY) key=LEFT_KEY;
      }
    }
  }

}



//-------------------------------------------------------------------------
void data()
{
  Serial.println("stage 3.1 data start");

  byte i;
  byte key = 0xFF;
  while (key!= LEFT_KEY)

  {



    Serial.println("stage 3.1 enter while loop");

    delay(500);       //                      allow sensor to be read
    scp1000.readSensor();
    delay(500);       //                      allow sensor to be read
   
    //Serial.print("Temprature (C): ");
    // Serial.println(scp1000.TempC);
    //Serial.print("Pressure (hPa): ");
   // Serial.println(scp1000.BaroP);

    // display temp  on lcd display ****************************
   
    int temp_s = (scp1000.TempC)*100;        //error?
    int temp = temp_s; // + 26;       
    
    //Serial.print("temp");
    Serial.print(temp);
   // Serial.print("============");

    float t1 = temp/1000;
    int t1a = t1;
    float t2 = (temp-(t1a*1000))/100;
    int t2a = t2;
    float t3 = ((temp-(t1a*1000))-(t2a*100))/10;
    long t3a = t3;
    // float temp4 = (((temp-(temp1a*1000))-(temp2a*100)-(temp3a*10));
    // int temp4a  = temp4;

    //define temp
        
    lcd.LCD_3310_set_XY(10*6,3);
    lcd.LCD_3310_write_char(t1a+48, MENU_NORMAL);
    lcd.LCD_3310_set_XY(11*6,3);
    lcd.LCD_3310_write_char(t2a+48, MENU_NORMAL);
    lcd.LCD_3310_set_XY(12*6,3);
    lcd.LCD_3310_write_char(46, MENU_NORMAL);
    lcd.LCD_3310_set_XY(13*6,3);
    lcd.LCD_3310_write_char(t3a+48, MENU_NORMAL);   



    //****************************************************************

    //*** display pressure on LCD
    int err = 0;               //+80 error?? 583

    float pressure_s = (scp1000.BaroP+err); 
    long pressure = pressure_s;
    //Serial.println(pressure);

    float p1 = pressure/100000;
    long p1a  = p1; 
    float p2 = (pressure-(p1a*100000))/10000;
    long p2a  = p2; 
    float p3 = ((pressure-(p1a*100000))-(p2a*10000))/1000;
    long p3a  = p3; 
    float p4 = (((pressure-(p1a*100000))-(p2a*10000))-(p3a*1000))/100;
    long p4a  = p4; 
    float p5 = ((((pressure-(p1a*100000))-(p2a*10000))-(p3a*1000))-(p4a*100))/10;
    long p5a  = p5; 
    float p6 = (((((pressure-(p1a*100000))-(p2a*10000))-(p3a*1000))-(p4a*100))-(p5a*10));
    long p6a  = p6; 

  Serial.println(scp1000.BaroP);
  Serial.println(pressure);

 // Serial.println("p1");
  //Serial.println(p1a);
   // Serial.println("p2");
   // Serial.println(p2a);
   //   Serial.println("p3");
    //    Serial.println(p3a);
    //      Serial.println("p4");
//Serial.println(p4a);
 // Serial.println("p5");
// Serial.println(p5a);
  //Serial.println("p6");
// Serial.println(p6a);


    lcd.LCD_3310_set_XY(7*6,4);
    lcd.LCD_3310_write_char(p1a+48, MENU_NORMAL);
    lcd.LCD_3310_set_XY(8*6,4);
    lcd.LCD_3310_write_char(p2a+48, MENU_NORMAL);
    lcd.LCD_3310_set_XY(9*6,4);
    lcd.LCD_3310_write_char(p3a+48, MENU_NORMAL);
    lcd.LCD_3310_set_XY(10*6,4);
    lcd.LCD_3310_write_char(p4a+48, MENU_NORMAL);   
    lcd.LCD_3310_set_XY(11*6,4);
    lcd.LCD_3310_write_char(46, MENU_NORMAL);
    lcd.LCD_3310_set_XY(12*6,4);
    lcd.LCD_3310_write_char(p5a+48, MENU_NORMAL);
    lcd.LCD_3310_set_XY(13*6,4);
    lcd.LCD_3310_write_char(p6a+48, MENU_NORMAL);

    //****************************************************************

    //**** display altitude 

    float Lzero = -0.0065;                           //altitude calculation
    float R = 287.052;
    float Pzero = 1013.250;
    float G = 9.80665;
    float Tzero = 288.15;
    float  alpha = (scp1000.BaroP+err)/Pzero;
    float alti = (Tzero/Lzero)*(pow(double(alpha),-((Lzero*R)/G)) -1);

   // Serial.println(alti);
    
    //*************************************
    long alt = alti*100;

    float a1 = alt/100000;
    long a1a  = a1; 
    float a2 = (alt-(a1a*100000))/10000;
    long a2a  = a2; 
    float a3 = ((alt-(a1a*10000))-(a2a*10000))/1000;
    long a3a  = a3; 
    float a4 = (((alt-(a1a*10000))-(a2a*10000))-(a3a*1000))/100;
    long a4a  = a4; 
    float a5 = ((((alt-(a1a*10000))-(a2a*10000))-(a3a*1000))-(a4a*100))/10;
    long a5a  = a5; 
    long a6 = (((((alt-(a1a*10000))-(a2a*10000))-(a3a*1000))-(a4a*100))-(a5a*10));
    long a6a  = a6; 

    lcd.LCD_3310_set_XY(7*6,0);
    lcd.LCD_3310_write_char(a1a+48, MENU_NORMAL);
    lcd.LCD_3310_set_XY(8*6,0);
    lcd.LCD_3310_write_char(a2a+48, MENU_NORMAL);
    lcd.LCD_3310_set_XY(9*6,0);
    lcd.LCD_3310_write_char(a3a+48, MENU_NORMAL);
    lcd.LCD_3310_set_XY(10*6,0);
    lcd.LCD_3310_write_char(a4a+48, MENU_NORMAL);   
    lcd.LCD_3310_set_XY(11*6,0);
    lcd.LCD_3310_write_char(46, MENU_NORMAL);
    lcd.LCD_3310_set_XY(12*6,0);
    lcd.LCD_3310_write_char(a5a+48, MENU_NORMAL);
    lcd.LCD_3310_set_XY(13*6,0);
    lcd.LCD_3310_write_char(a6a+48, MENU_NORMAL);


    //*********************     alti 2 = alti- alti_initial     ...display data 

    //**** display acceleration 
    //*********************





    //**** display duration 
    //*********************





    //**************************************************************

    lcd.LCD_3310_write_string(0,0, "NH(M)", MENU_NORMAL);
    lcd.LCD_3310_write_string(0,1, "FE(M)"   , MENU_NORMAL);
    lcd.LCD_3310_write_string(0,2, "A(M/S)", MENU_NORMAL);
    lcd.LCD_3310_write_string(0,3, "Temp(C)", MENU_NORMAL);
    lcd.LCD_3310_write_string(0,4, "hPa", MENU_NORMAL);
    lcd.LCD_3310_write_string(0,5, "T(min)        ", MENU_NORMAL);


    /*
  lcd.LCD_3310_write_string(0,0, "Alt(M) ****.**", MENU_NORMAL);
     lcd.LCD_3310_write_string(0,1, "H(M)  ±****.**"   , MENU_NORMAL);
     lcd.LCD_3310_write_string(0,2, "A(M/S)    **.*", MENU_NORMAL);
     lcd.LCD_3310_write_string(0,3, "T(C)      **.*", MENU_NORMAL);
     lcd.LCD_3310_write_string(0,4, "hPa    ****.**", MENU_NORMAL);
     lcd.LCD_3310_write_string(0,5, "T(min)     ***", MENU_NORMAL);
     
     */
    delay (2000);

    Serial.println("stage 3.1 end while loop ");
    for(i=0; i<NUM_KEYS; i++)
    {
      if(button_flag[i] !=0)
      {
        button_flag[i]=0;  // reset button flag
        if(i== LEFT_KEY) key=LEFT_KEY; 
      }
    }


  }


  Serial.println("stage 3.1 complete ");


}
//----------------------------------------------------------------------------
void in_flight(){

  Serial.println("stage 3.2 data start");

  byte i;
  byte key = 0xFF;
  while (key!= LEFT_KEY)

  {



    Serial.println("stage 3.2 enter while loop");


    float  roc;
    roc =0.5; 

    //float maxRoc;
    //maxRoc = 0;
    //if(roc>maxRoc);
    // {
    //   roc=maxRoc;
    // } 


    int location;

    if (roc > 0)
    {
      lcd.LCD_3310_draw_bmp_pixel(0,2, scale2 , 96,24);           
      location = roc*20;                                     //edit equation

    }
    else
    {
      lcd.LCD_3310_draw_bmp_pixel(0,2, scale3 , 96,24);  
      location = roc*-20;                                       //edit equation
    }


    //delay(1000);                                     //waiting a second


    lcd.LCD_3310_draw_bmp_pixel(location ,5, pointer , 5, 8);  

    lcd.LCD_3310_write_string( 0, 0, "QFE **** m     ", MENU_NORMAL);
    lcd.LCD_3310_write_string( 0, 1, "Acc *.* m/s   ", MENU_NORMAL);

    delay(2000);

    Serial.println("stage 3.2 end while loop ");
    for(i=0; i<NUM_KEYS; i++)
    {
      if(button_flag[i] !=0)
      {
        button_flag[i]=0;  // reset button flag
        if(i== LEFT_KEY) key=LEFT_KEY; 
      }
    }


  }



}

//------------------------------------------------------------------------------
void stats(){



  lcd.LCD_3310_write_string( 0, 1, "maxAlt  **** M", MENU_NORMAL);
  lcd.LCD_3310_write_string( 0, 2, "maxAcc  *.*M/S", MENU_NORMAL);
  lcd.LCD_3310_write_string( 0, 3, "minAcc  *.*M/S", MENU_NORMAL);
  lcd.LCD_3310_write_string( 0, 4, "dur     ***Min", MENU_NORMAL);



  //lcd.LCD_3310_write_string( 48, 0, maxRoc, MENU_NORMAL);




  waitfor_OKkey();


}


//------------------------------------------------------------------------------

void about()
{
  lcd.LCD_3310_write_string(29,1, "Vario", MENU_NORMAL);
  lcd.LCD_3310_write_string(29,2, "V 1.0", MENU_NORMAL);
  lcd.LCD_3310_write_string(38,3, "By", MENU_NORMAL);
  lcd.LCD_3310_write_string(23,4, "Nainesh", MENU_NORMAL);

  waitfor_OKkey();
}





// The followinging are interrupt-driven keypad reading functions
//  which includes DEBOUNCE ON/OFF mechanism, and continuous pressing detection


// Convert ADC value to key number
char get_key(unsigned int input)
{
  char k;

  for (k = 0; k < NUM_KEYS; k++)
  {
    if (input < adc_key_val[k])
    {

      return k;
    }
  }

  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed

  return k;
}

void update_adc_key(){
  int adc_key_in;
  char key_in;
  byte i;

  adc_key_in = analogRead(0);
  key_in = get_key(adc_key_in);
  for(i=0; i<NUM_KEYS; i++)
  {
    if(key_in==i)  //one key is pressed 
    { 
      if(button_count[i]<DEBOUNCE_MAX)
      {
        button_count[i]++;
        if(button_count[i]>DEBOUNCE_ON)
        {
          if(button_status[i] == 0)
          {
            button_flag[i] = 1;
            button_status[i] = 1; //button debounced to 'pressed' status
          }

        }
      }

    }
    else // no button pressed
    {
      if (button_count[i] >0)
      {  
        button_flag[i] = 0;	
        button_count[i]--;
        if(button_count[i]<DEBOUNCE_OFF){
          button_status[i]=0;   //button debounced to 'released' status
        }
      }
    }

  }
}

// Timer2 interrupt routine -
// 1/(160000000/256/(256-6)) = 4ms interval

ISR(TIMER2_OVF_vect) {  
  TCNT2  = 6;
  update_adc_key();
}






