#include "UI.h"
#include <DueTimer.h>
#include <SimpleTimer.h>;
#include <Wire.h>
#include <SPI.h>
#include <Encoder.h>
#include <Fader.h>

long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)

#define ENC_B 10
#define ENC_A 11
#define ENC_BUT 12
boolean state_A = 0;
boolean state_B = 0;
boolean state_BUT = 0; 
Encoder enc(ENC_A,ENC_B,ENC_BUT,0,254);
Fader fader;
UI ui(&enc, &fader);
SimpleTimer timer;

#define LEDPIN 13


// Interrupt for Encoder
void encoder_A_interrupt_fun() {
	enc.updateEncoder_A();
}

void encoder_B_interrupt_fun() {
	enc.updateEncoder_B();
}

void button_interrupt_fun() {
	enc.updateButton();
}

void getTime_wrapper() {
	ui.getTime();
}

void getLux_wrapper() {
	ui.getLux();
}

void draw_wrapper() {
	ui.draw();
	ui.update_input();
}

void blink_wrapper() {
	ui.switch_blink();
}

void blinkfast_wrapper() {
	ui.switch_blinkfast();
}

void fader_wrapper() {
    	fader.update(millis());
}

/*boolean fade_mode = true;
void switch_fade_mode() {
    if (fade_mode) {
        int colors[12] =  { 512,1023,512, 1023,512,265, 265,512,1023, 512,512,512 };
        //int colors[12] =  { 1023,1023,1023, 1023,1023,1023, 1023,1023,1023, 1023,1023,1023 };
        fade.start_fade_to_color(colors,3000);
        fade_mode = false;
    }
    else {
        fade.start_rainbow();
        //int colors[12] =  { 0,0,0, 0,0,0, 0,0,0, 0,0,0 };
        //fade.start_fade_to_color(colors,3000);        
        fade_mode = true;
    }
}*/

void setup() {

	Wire.begin();
        SPI.begin();
	
	Serial.begin(115200);

        //Serial.println("alarmlock initializing");
        
        pinMode(LEDPIN, OUTPUT);      

      	ui.init();
   
        fader.init();
        
	timer.setInterval(1000,blink_wrapper);
	timer.setInterval(500,blinkfast_wrapper);
	delay(300);

	//timer.setInterval(2000, getLux_wrapper);
	//delay(10);
        
	timer.setInterval(20, draw_wrapper);
       
        timer.setInterval(30, fader_wrapper);
        
        enc.Init();
	attachInterrupt(ENC_A,encoder_A_interrupt_fun, CHANGE);
	attachInterrupt(ENC_B,encoder_B_interrupt_fun, CHANGE);
	attachInterrupt(ENC_BUT,button_interrupt_fun, CHANGE);        
        
        //fade.start_rainbow();
        //int colors[12] =  { 512,1023,512, 1023,512,265, 265,512,1023, 512,512,512 };
        //fader.start_fade_to_color(colors,1000);        
        //fader.start_rainbow();
}

void loop() {
  timer.run();

}
