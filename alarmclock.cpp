
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
UI ui(&enc);
SimpleTimer timer;
Fader fade;

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
    unsigned long ms = millis();
    fade.update(ms);
    ui.cycleRBG(ms);
}

boolean fade_mode = true;
void switch_fade_mode() {
    if (fade_mode) {
        int colors[12] =  { 1024,512,265, 512,1024,512, 265,512,1024, 512,512,512 };
        fade.start_fade_to_color(colors);
        fade_mode = false;
    }
    else {
        fade.start_rainbow();
        fade_mode = true;
    }
}

void setup() {

	Wire.begin();
        SPI.begin();
	
	Serial.begin(115200);

        //Serial.println("alarmlock initializing");
        
        pinMode(LEDPIN, OUTPUT);      
        
	ui.init();
        
        fade.init();
   
	timer.setInterval(1000,blink_wrapper);
	timer.setInterval(500,blinkfast_wrapper);
	delay(300);

	//timer.setInterval(2000, getLux_wrapper);
	//delay(10);
        
	timer.setInterval(20, draw_wrapper);

        timer.setInterval(30, fader_wrapper);

        timer.setInterval(20000, switch_fade_mode);
        
        enc.Init();
	attachInterrupt(ENC_A,encoder_A_interrupt_fun, CHANGE);
	attachInterrupt(ENC_B,encoder_B_interrupt_fun, CHANGE);
	attachInterrupt(ENC_BUT,button_interrupt_fun, CHANGE);        
        
        fade.start_rainbow();
}

void loop() {
  timer.run();

}
