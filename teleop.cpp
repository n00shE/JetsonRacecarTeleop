#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <JHPWMPCA9685.h>
#include "joystick.hh"
#include <unistd.h>
#include <cmath>

#define SERVO_CHANNEL 0
#define ESC_CHANNEL 1

#define PWM_FULL_REVERSE 250 // 1ms/20ms * 4096
#define PWM_NEUTRAL 307      // 1.5ms/20ms * 4096
#define PWM_FULL_FORWARD 350 // 2ms/20ms * 4096
#define REVERSE_RANGE 47
#define FORWARD_RANGE 39

#define SERVO_FULL_LEFT 350
#define SERVO_NEUTRAL 300
#define SERVO_FULL_RIGHT 250
#define LEFT_RANGE 50
#define RIGHT_RANGE 50

Joystick joystick("/dev/input/js0");

PCA9685 *pca9685;

float currentDrivePWM = PWM_NEUTRAL ;
float currentServoPWM = SERVO_NEUTRAL ;
int currentChannel = ESC_CHANNEL ;
int currentAxis ;
float currentDriveValue ;
float currentServoValue ;
int scaledESC ;

int setup() {
    pca9685 = new PCA9685() ;
    int err = pca9685->openPCA9685();
    if (err < 0){
        printf("Error: %d", pca9685->error);
    }
    printf("PCA9685 Device Address: 0x%02X\n",pca9685->kI2CAddress) ;
    pca9685->setAllPWM(0,0) ;
    pca9685->reset() ;
    pca9685->setPWMFrequency(50) ;
    if (joystick.isFound())
      {
        printf("Found Joystick!\n");
      }
    sleep(1) ;
    pca9685->setPWM(ESC_CHANNEL,0,PWM_NEUTRAL);
    pca9685->setPWM(SERVO_CHANNEL,0,SERVO_NEUTRAL);
    currentDrivePWM = PWM_NEUTRAL ;
    sleep(2) ;
    printf("Setup Complete!\n") ;
}

int updatePWM() {
    printf("%f\n", currentDriveValue);
    pca9685->setPWM(currentChannel,0,currentDrivePWM);
}

int scaleESC(int value) {
    scaledESC = (value - -32767) / (PWM_FULL_REVERSE - -32767) * (PWM_FULL_FORWARD - 32767) + 32767;
    printf("%d\n", scaledESC);
}
int main() {
    setup() ;

    while (true) {
      // Restrict rate
      usleep(500);

      // Attempt to sample an event from the joystick
      JoystickEvent event;
      if (joystick.sample(&event))
      {
        if (event.isButton())
        {
          printf("Button %u is %s\n", event.number, event.value == 0 ? "up" : "down");
        }
            if (event.number == 1 && event.value == 1)
              {
                pca9685->setPWM(ESC_CHANNEL,0,PWM_NEUTRAL);
                printf("EMERGENCY STOP\n");
                sleep(5);
              }
        else if (event.isAxis())
        {
          printf("Axis %u is at position %d\n", event.number, event.value);
          currentAxis = event.number;
            if (currentAxis == 1)
            {
              currentDriveValue = event.value;
              currentChannel = ESC_CHANNEL ;
              currentDriveValue /= 32767;
              printf("currentDriveValue is %f\n", currentDriveValue);
                if (currentDriveValue < 0) //FORWARD
                {
                  //currentPWM = 350 - ((-1*currentDriveValue) / 762.023256);
                  currentDrivePWM = (PWM_NEUTRAL + (-currentDriveValue * FORWARD_RANGE));
                  printf("%f\n", currentDrivePWM);

                }
                else if (currentDriveValue > 0) //REVERSE
                {
                  //currentPWM = 307 - (currentDriveValue / 574.859649);
                  currentDrivePWM = (PWM_NEUTRAL - (currentDriveValue * REVERSE_RANGE));
                  printf("%f\n", currentDrivePWM);
                }
                else if (currentDriveValue == 0)
                {
                  currentDrivePWM = PWM_NEUTRAL;
                }
                //int flooredPWM = floor(currentPWM);
                pca9685->setPWM(currentChannel,0,currentDrivePWM);
                printf("Set PWM to %f on channel %d\n", currentDrivePWM, currentChannel);
            }
            if (currentAxis == 2)
            {
              currentServoValue = event.value;
              currentChannel = SERVO_CHANNEL;
              if (currentServoValue > 0) //LEFT
              {
                
              }
              else if (currentServoValue < 0) //RIGHT
              {
                
              }
              printf("%f\n", currentServoPWM);

            }  

        }
      }
      else
      {
        //pca9685->setPWM(ESC_CHANNEL,0,PWM_NEUTRAL);
      }
    }
/**/
}
