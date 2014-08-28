#include "UI.h"
#include <DueTimer.h>
#include <SimpleTimer.h>;
#include <Wire.h>
#include <SPI.h>
#include <Encoder.h>
#include <Switch.h>
#include <Fader.h>
#include <Widget.h>
#include <U8glib.h>
#include <u8g.h>
#include <SoundManager.h>
#include <Alarm.h>
#include <rtc_clock.h>

long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)

#define ENC_B 10
#define ENC_A 11
#define ENC_BUT 12
#define ALARM_SW 9
#define LIGHT_SW 8

boolean state_A = 0;
boolean state_B = 0;
boolean state_BUT = 0; 
Switch alarm_sw = Switch(ALARM_SW);
Switch light_sw = Switch(LIGHT_SW);
U8G_CLASS u8g(LCD_SCK, LCD_MOSI, LCD_CS, LCD_RS, LCD_RST);	
Encoder enc(ENC_A,ENC_B,ENC_BUT);
Fader *fader;
RTC_clock *due_clock;
Alarm *alarm;
SoundManager *sound;
UI *ui;
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
	ui->getTime();
}

void getLux_wrapper() {
	ui->getLux();
}

void draw_wrapper() {
	ui->draw();
	ui->input();
}

void blink_wrapper() {
	Widget::switch_blink();
}

void blinkfast_wrapper() {
	Widget::switch_blinkfast();
}

void fader_wrapper() {
    	fader->update(millis());
}

void alarm_wrapper() {
    	ui->check_alarm();
}

void check_switches() {
    if (alarm_sw.was_switched()) {
        alarm->set(alarm_sw.is_on());   
        Serial.println("Alarm switched");
    }
    if (light_sw.was_switched() ) {
        if (light_sw.is_on()) fader->start_last_effect();
        else fader->fade_out();        
    }         
}

void update_switches() {
    light_sw.updateSwitch();
    alarm_sw.updateSwitch();
    check_switches();
}

void setup() {

	Wire.begin();
        SPI.begin();
	
	Serial.begin(115200);

        Serial3.begin(9600);
        
        fader = new Fader();

        sound = new SoundManager(Serial3);
        
        due_clock = new RTC_clock(XTAL);

        alarm = new Alarm(due_clock,sound,fader);
        
        ui = new UI(&enc, fader, &u8g, sound, due_clock, alarm);
        
        //Serial.println("alarmlock initializing");
        
        pinMode(LEDPIN, OUTPUT);      
        digitalWrite(LEDPIN,LOW);
                        
        u8g.setColorIndex(1);         //BW Mode       
        
        fader->init();
        
      	ui->init();
        
	timer.setInterval(1000,blink_wrapper);
	timer.setInterval(500,blinkfast_wrapper);
	delay(300);

	//timer.setInterval(2000, getLux_wrapper);
	//delay(10);
        
	timer.setInterval(20, draw_wrapper);
       
        timer.setInterval(30, fader_wrapper);

        timer.setInterval(300, alarm_wrapper);
        
        timer.setInterval(10, update_switches);
        
        
        
        enc.Init();
	attachInterrupt(ENC_A,encoder_A_interrupt_fun, CHANGE);
	attachInterrupt(ENC_B,encoder_B_interrupt_fun, CHANGE);
	attachInterrupt(ENC_BUT,button_interrupt_fun, CHANGE);        
    	//attachInterrupt(ALARM_SW,alarm_sw_interrupt_fun, CHANGE);        
	//attachInterrupt(LIGHT_BUT,light_sw_interrupt_fun, CHANGE);        
            
        digitalWrite(LEDPIN,HIGH);
}

void loop() {
  timer.run();
}
