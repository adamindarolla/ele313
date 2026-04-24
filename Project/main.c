// Adam was actually here 22/4/26
// So was Tom. Shout out Tom Seipp
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



// Space to declare wall following function

		


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

    // main
    int SensorValue[8];
    int currentVal = 0;
    int strongestSensor =-1; // put it as -1 to avoid confusing code - will be from 0 to 7 later
    int maxVal = 0;
    int maxValThreshold = 100; // for collision detection

    int followSide =0; // will be -1 or +1 later
    
    // wall follow
    int prefSide; // for which sensor is which once a wall follow side is decided - will be set during loop
    int offSide;
    int prefCorner;
    int offCorner;
    int prefFront;
    int offFront;

    int maxirread = 300;
    int minirread = 50;

    float wheelRatio = 1;

    int pSide;
    int oSide;
    int pCorner;
    int oCorner;
    int pFront;
    int oFront;

    //main
    bool wallsexplored = 1; // flicks to 1 (true) after doing a lap around the walls
    int followCounter =0; //counts steps taken during wall following
    int followDuration = 1200; // steps before wall follow mode ends

    enum RobotState {  FOUNDWALL, EXPLORING };
	enum RobotState currentState = EXPLORING;

    // Main Loop!
    while(1){
        //Need some logic that checks immediate surroundings
        //AND takes into account previous exploration
        //To decide what currentMode is
            maxVal = 0;
            // Check all sensors and get values
            for (int i = 0; i < 8 ; i++){

                SensorValue[i] = get_prox(i); // Start at 0 and end at 7 for corresponding sensors
                currentVal = get_prox(i);

                if (currentVal > maxVal){
                    maxVal = currentVal;
                    strongestSensor = i;
                }

                        
        

        //Different behaviour based on which state we decided


            				// sets which sensors are preffered side and offside



            else{
                left_motor_set_speed(1000);
                right_motor_set_speed(1000);
            }
            }
        }

        if(currentState == EXPLORING){
                    // we already know what the sensor values are so no need to get_prox again

                	set_rgb_led(LED2,1,0,0);

                    if (maxVal > maxValThreshold){
                    	currentState = FOUNDWALL;
                        switch (strongestSensor) {
                        case 0:
                        case 1:
                        case 2:
                            followSide = 1; //right

                            break;

                        case 3:
                        case 4:
                            followside = 0; // ignore these two
                            currentState = EXPLORING;
                            left_motor_set_speed(1000);
                            right_motor_set_speed(1000);

                            break;
                        case 5:
                        case 6:
                        case 7:
                            followSide = -1; //left

                            break;
                        }

                    }else{
                        left_motor_set_speed(1000);
                        right_motor_set_speed(1000);

                        }
                    } // exploring end



        if (currentState == FOUNDWALL){
        	set_rgb_led(LED2,0,1,0);
        	set_rgb_led(LED4,0,1,0);
        	set_rgb_led(LED6,0,0,1);

            {

                left_motor_set_speed(-750*followSide);
                right_motor_set_speed(750*followSide);
                delay_ms(1000); // rotate for a certain amount of time
                left_motor_set_speed(0);
                right_motor_set_speed(0);
                currentState = EXPLORING; 

                // This is if we do not want to re-enter wall follow mode i.e. bounce right off the wall
            }
        } // foundwall end
        
            
        
        


            delay_ms(100);
 }   //closing bracket for while loop     
}   // int min void end

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
