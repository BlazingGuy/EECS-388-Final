#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"

void auto_brake(int devid)
{
    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // Use the directions given in the project document
    if ('Y' == ser_read(devid) && 'Y' == ser_read(devid)) {
        uint8_t l_dist = ser_read(devid);
        uint8_t h_dist = ser_read(devid);
        uint16_t dist = (h_dist << 8) | l_dist;

        char buff[50];
        sprintf(buff, "Distance: %d cm", dist);
        ser_printline(devid, &buff);

        //printf("this output is null");

        if (dist > 200)
        {
            gpio_mode(GREEN_LED, OUTPUT);
            gpio_write(GREEN_LED, ON);
            gpio_write(BLUE_LED, OFF);
            gpio_write(RED_LED, OFF);
        }
        else if (dist > 100 && dist <= 200)
        {
            gpio_mode(GREEN_LED, OUTPUT);
            gpio_write(GREEN_LED, ON);
            gpio_write(BLUE_LED, OFF);
            gpio_mode(RED_LED, OUTPUT);
            gpio_write(RED_LED, ON);
        }
        else if (dist > 60 && dist <= 100)
        {
            gpio_write(GREEN_LED, OFF);
            gpio_write(BLUE_LED, OFF);
            gpio_mode(RED_LED, OUTPUT);
            gpio_write(RED_LED, ON);
        }
        else
        {
            gpio_mode(RED_LED, OUTPUT);
            gpio_write(RED_LED, ON);
            delay(100);
            gpio_write(RED_LED, OFF);
            delay(100);
        }
    }
}

int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.
    
    char data [100];

    while (1) {
        if (ser_isready(devid))
        {
            int angle;

            ser_readline(1, 100, data);

            // change bytes to int
            sscanf(data, "%d", &angle);

            return angle;
        }
    }
    return 0;

}

void steering(int gpio, int pos)
{
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task

    int time_cycle = ((2400 - 544)/180)*pos + 544;
    int rem_cycle = 20000 - time_cycle;

    gpio_write(gpio, ON);
    delay_usec(time_cycle);
    gpio_write(gpio, OFF);
    delay_usec(rem_cycle);
    
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        
        printf("\nangle=%d", angle) 
        int gpio = PIN_19; 
        for (int i = 0; i < 10; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            // steering(gpio, angle);
        }

    }
    return 0;
}
