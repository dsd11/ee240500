#include "simpletools.h"
#include "servo.h"
#include "fdserial.h"
#include "wavplayer.h"
#include "servodiffdrive.h"
#include "ping.h" 
#define PIN_XBEE_RX      0
#define PIN_XBEE_TX      1
#define PIN_SOUND       26

//servo
int stop;
//

//PING
int cmDist;
int angle;
int increase;
//PING

//XBee
volatile int ch,first, second;
volatile int XeeChange=0;
volatile fdserial *xbee;
//XBee

//SD card and music
int DO = 22, CLK = 23, DI = 24, CS = 25;
const char techloop[] = {"aclock.wav"};       // Set up techloop string
int *mycog0,*mycog1;

void XBee();
void sense_distance();
void move();
void mymusic();
void beebee();

int main(){
   
	stop = 0;
	xbee = fdserial_open(PIN_XBEE_RX, PIN_XBEE_TX, 0, 9600);
	sd_mount(DO, CLK, DI, CS);
  
	mycog0 = cog_run(&XBee,128);
	mycog1 = cog_run(&move,128);
 
   increase=50;
   angle=500;

 
	while(1){
     sense_distance();
    	if(XeeChange){

			if((char) ch=='b'){
				stop = 1;
			}
        else{
          stop = 0;
        }

    	   XeeChange = 0;
   	}   
    	pause(1);
  	}
     
	return 0;
}

void XBee(){
   
   putchar(CLS);
   fdserial_rxFlush(xbee);
   while(1){
     ch = fdserial_rxChar(xbee);
     XeeChange = 1;
     fdserial_txChar(xbee, ch);
     fdserial_txFlush(xbee);
   }
}

void mymusic(){
  wav_play(techloop);                             // Pass to wav player
 
  wav_volume(6);                                  // Adjust volume
  pause(3500);                                    // Play for 3.5 s
  wav_volume(4);                                  // Repeat twice more
  pause(2000);
  wav_volume(8);
  pause(3500);

  wav_stop();                                      // Stop playing
} 

void sense_distance(){
    servo_angle(16, angle);
    cmDist = ping_cm(17);
    servo_angle(16, angle);                         
    pause(30);
    angle = angle + increase;
    if(angle == 1300) increase = -50;
    if(angle == 500)    increase =  50;
}

void beebee(){
  int time = 200;
  for(int i=0;i<30;i++){
    freqout(4, 200, 3000+i*50);
    pause(time);
    time = time - 10*i;
    if(time<1) time = 10;
  }
}

void move(){
	while(1){
		servo_speed(14,40);
		servo_speed(15,-40);
	    while(cmDist<25){
			servo_speed(14,0);
			servo_speed(15,0);
			pause(400);
			servo_speed(14,-20);
			servo_speed(15,20);
			pause(1000);
			if(angle>900){
				servo_speed(14,30);
				servo_speed(15,30);
				pause(800);
			}
			else{
				servo_speed(14,-30);
				servo_speed(15,-30);
				pause(800);
			}
		}
		while(stop==1){
        servo_speed(14,0);
        servo_speed(15,0);
        beebee();
        pause(2000);
        if(stop==0){
            break;
        }
        else{
          mymusic();  
        }
       
		}
	}  
}
