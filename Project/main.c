// Adam was actually here 22/4/26
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>
#include "leds.h"
#include "spi_comm.h"
#include "sensors/proximity.h"
#include "motors.h"
#include "epuck1x/uart/e_uart_char.h"
#include "stdio.h"
#include "serial_comm.h"
#include "selector.h"

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

// Globally declare variables

    int SensorValue[8];
    int currentVal = 0;
    int strongestSensor =-1; // put it as -1 to avoid confusing code - will be from 0 to 7 later
    int maxVal = 0;
    int maxValThreshold = 500; // for collision detection

    int followSide =0; // will be -1 or +1 later
    

    int maxirread = 1000;
    int minirread = 300;

    bool wallsexplored = 0; // flicks to 1 (true) after doing a lap around the walls
    int followCounter =0; //counts steps taken during wall following
    int followDuration = 1200 // steps before wall follow mode ends

    enum RobotState { NOWALL, FOUNDWALL, EXPLORING };
	enum RobotState currentState = NOWALL;

// Space to declare wall following function
void follow_wall();
    float wheelRatio = 1.0;
    int pSide = get_prox(prefSide); // this should be fine
    int oSide = get_prox(offSide); 
    int pCorner = get_prox(prefCorner);
    int oCorner = get_prox(offCorner);
    int pFront = get_prox(prefFront);
    int oFront = get_prox(offFront);
    if (pSide<minirread){
				//turn towards wall
				wheelratio = 0.9;
			} else if (pSide>maxirread && oSide<maxirread) || (pCorner>maxirread && oCorner<maxirread)){
				//turn away from preffered wall
				wheelratio = 1.1;
			} else if (oSide>maxirread && pSide<maxirread) || (oCorner>maxirread && pCorner<maxirread){
				// turn away from offside wall
				wheelratio = 0.9;
			} else if ((pSide>maxirread && oSide>maxirread) || (pCorner>maxirread && oCorner>maxirread)){
				// if robot is too close to both walls, turn around 180 ish degrees
				left_motor_set_speed(-500 * followSide);
				right_motor_set_speed(500 * followSide); 
				chThdSleepMilliseconds(1000); // 1 second is abritary amount of time
			} else if (pFront>maxirread || (oFront>maxirread)){
                // if wall is infront of robot, turn away from desired side
				left_motor_set_speed(-500 * followSide);
				right_motor_set_speed(500 * followSide); 
				chThdSleepMilliseconds(500); // 1 second is abritary amount of time
            }

			// sets wheel speeds based case seen before
			if (followsSide ==-1){
				if (wheelRatio<1){
					left_motor_set_speed(1000 * wheelRatio);
					right_motor_set_speed(1000); 
				}else{
					left_motor_set_speed(1000);
					right_motor_set_speed(1000 / wheelRatio); 
				}	
			} else if (followside ==1){
				if (wheelRatio<1){
					left_motor_set_speed(1000);
					right_motor_set_speed(1000 * wheelRatio); 
				} else {
					left_motor_set_speed(1000 / wheelRatio);
					right_motor_set_speed(1000); 
				}
			}
		


// Space for some other stuff

// Initialising stuff

int main(void){ 
    halInit();
    chSysInit();
    mpu_init();

    // Proximity
    messagebus_init(&bus, &bus_lock, &bus_condvar);
    proximity_start(0);
    calibrate_ir();

    //LED
    clear_leds();
    spi_comm_start();
	rgb_led_name_t allRGBs[] = {LED2, LED4, LED6, LED8};

    //Motors
    motors_init();

	//Bluetooth
	serial_start();

    
    // Wall follower function here



    // Main Loop!
    while(1){
        //Need some logic that checks immediate surroundings
        //AND takes into account previous exploration
        //To decide what currentMode is

            // Check all sensors and get values
            for (int i = 0, i < 8 , i++){

                int SensorValue(i) = get_prox(i); // Start at 0 and end at 7 for corresponding sensors
                int currentVal = get_prox(i);

                if (currentVal > maxVal){
                    maxVal = currentVal;
                    strongestSensor = i;
                }
            }
                        
        }

        //Different behaviour based on which state we decided

        if(currentState == NOWALL){
             if(maxVal >= maxValThreshold){ 
            switch (strongestSensor) {
                case 0
                case 1
                case 2
                case 3  
                    followSide = 1; //right
                    currentState = FOUNDWALL;
                case 4
                case 5
                case 6
                case 7
                    followSide = -1; //left
                    currentState = FOUNDWALL;
            }
            else{
                left_motor_set_speed(500);
                right_motor_set_speed(500);
            }
        }

        if (currentState == FOUNDWALL){
            if (wallsexplored = 0){
                //DO THE WALL FOLLOWING CODE
                follow_wall(); // is this going to work haha
                followCounter = followCounter + int 1; // increases followCounter

                if(followCounter >= followDuration){
                wallsexplored = 1; // ensures we do not re-enter this state - needs a thing that only does it after a certain amount of time
                }
            }
            else{

                left_motor_set_speed(500*followSide);
                right_motor_set_speed(500*followSide);
                delay_ms(500); // rotate for a certain amount of time
                left_motor_set_speed(0);
                right_motor_set_speed(0)
                currentState = EXPLORING; 

                // This is if we do not want to re-enter wall follow mode i.e. bounce right off the wall
            }
        }
        
            
        
        
        if(currentState == EXPLORING){
            // we already know what the sensor values are so no need to get_prox again
                        
            if (maxVal > maxValThreshold){
                switch (strongestSensor) {
                case 0
                case 1
                case 2
                    followSide = 1; //right
                    currentState = FOUNDWALL;
                    
                case 3  
                case 4
                    followside = 0; // ignore these two
                    currentState = EXPLORING;
                    left_motor_set_speed(500);
                    right_motor_set_speed(500);
                case 5
                case 6
                case 7
                    followSide = -1; //left
                    currentState = FOUNDWALL;
                }

            else{
                left_motor_set_speed(500);
                right_motor_set_speed(500);

                }
            }
            
        }   
        

        delay_ms(50);



        // the purple } is for the while(1) loop leave that!
    }
}
