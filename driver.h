#include "mbed.h"
#include "rtos.h"
#include "TextLCD.h"
#include "TempQueue.h"

// The main output of the program. Currently connected to an LED but
// can be potentially connected to a fan, motor, etc.
PwmOut mypwm(D5);
// The LM35 sensor for temperature 
AnalogIn temp_sensor(A0);
// Connected to a red LED
DigitalOut red(D4);
// Connected to a yellow LED
DigitalOut yellow(D3);
// Connected to a green LED
DigitalOut green(D2);
// Row 0 of the 4x4 keypad
DigitalOut r0(PB_1);
// Row 1 of the 4x4 keypad
DigitalOut r1(PB_15);
// Row 2 of the 4x4 keypad
DigitalOut r2(PB_14);
// Row 3 of the 4x4 keypad
DigitalOut r3(PB_13);
// Column 0 of the 4x4 keypad
DigitalIn c0(PA_12);
// Column 1 of the 4x4 keypad
DigitalIn c1(PA_11);
// Column 2 of the 4x4 keypad
DigitalIn c2(PB_12);
// Column 3 of the 4x4 keypad
DigitalIn c3(PB_2);
// Defining threads for the rtos
// This thread reads the temperature from temp_sensor
Thread read_temp_thread;
// This thread displays the temperature read before onto the LCD screen
Thread display_temp_thread;
// This thread lights the leds based on the read value for temperature
Thread led_thread;
// This thread operates the PWM output based on the read value for temperature
Thread  pwm_thread;
// This thread sends temperature data serially through UART to the computer
Thread uart_thread;
// This thread calculates the average temperature based on the read temp. values
Thread temperature_average_thread;
// This thread operates when an emergency button is pressed
Thread emergency_thread;
// This thread has the highest priority and does not allow other threads 
// to overtake
Thread password_thread;
// This thread initializes values of temperature minimum, medium, maximum, 
// the emergency timeout and the startup password
Thread init_mode_thread;
// This thread determines if a remote session or an emergency are called
// through the user's keyboard input, or none of these
Thread keyboard_input_thread;
// This thread prompts the PC keyboard to configure few settings 
Thread remote_session_thread;
// This thread checks whether the user have enter a letter
Thread keyboard_readable_thread;
// The text lcd is a 16x2 characters connected to the GPIO
// It will help us display values as well as to operate the keypad
TextLCD lcd ( PB_8, PB_9, PA_5,   PA_6,   PA_7,   PB_6);
// This button starts the emergency thread when pressed
InterruptIn emerg_button(USER_BUTTON);
// This serial port allows us to send data to the pc terminal
Serial pc(SERIAL_TX, SERIAL_RX);
// This array of pointers will hold messages about the temperature values
char *msg[3];
// This is the default waiting time for short durations
const int thread_wait_short = 300;
// This is the default waiting time for medium durations
const int thread_wait_med = 1500;
// This is the default waiting time for long durations
const int thread_wait_long = 3000;
// This is the default size of a string
const int buffer = 1024;
// Temp holds the value of the temperature. Starting value is 22
float temp = 22;
// Temp holds the value of the average of the temperature values.
// Starting value is 20
float temp_avg = 20;
// TempMin holds the value of the minimum temperature. Default value is 10
int tempMin = 10;
// TempMid holds the value of the medium temperature. Default value is 50
int tempMid = 50;
// TempMax holds the value of the maximum temperature. Default value is 100
int tempMax = 100;
// Pass holds the value of the numeric password. Default value is 1313
int pass = 1313;
// TempQueue holds the values for the previous <= 10 temperature values
// These values will be used when calculating the average
TempQueue averages;
// TIMEOUT holds the value of the emergency timeout duration. Default = 3 secs
int TIMEOUT = 3;

/** char keypad(void);
* Objective: Keypad returns the ascii code of the character pressed 
*            by the keypad. 
* Pre-conditions: A keypad is connected through the GPIO and in pullup mode
*                 and we have r0, r1, r2, r3 as outputs, 
*                 and c0, c1, c2, c3 as inputs
* Post-conditions: Returns \0 if no button is pressed, or the button otherwise
*/
char keypad(void);

/** bool keypad_pressed(void);
* Objective: A fast function that indicates if a key is pressed or not
             without showing which key is pressed.
* Pre-conditions: A keypad is connected and in pullup mode
* Post-conditions: Returns \0 if no button is pressed, or the button otherwise
*/
bool keypad_pressed(void);

/** char keypad_wait(void);
* Objective: A function that halts the execution of the program until the 
             user enters an acceptable input through the keypad
* Pre-conditions: A keypad is connected and in pullup mode
*                 A or D: submits the value. C: clears the line. B: backspace
* Post-conditions: Returns \0 if no button is pressed, or the button otherwise
*                  ( only numeric values)
*/
char keypad_wait(void);

/** char keypad_ABCD(bool, bool, bool, bool );
* Objective: A function that halts the execution of the program until the 
*            user enters an acceptable input through the keypad
* Pre-conditions: A keypad is connected and in pullup mode
*                 If further limiations are needed for letters, a true argument
*                 can be passed to disable A, B, C, or D respectively.
* Post-conditions: Returns \0 if no button is pressed, or the button otherwise
*                  ( only alphabetic values)
*/
char keypad_ABCD(bool, bool, bool, bool );

/** int keypad_disp(int column, int row, int size);
* Objective: A function that halts the execution of the program until the 
*            user enters an acceptable input through the keypad.
*            It also displays the input of the user as he inputs it.
* Pre-conditions: A keypad is connected and in pullup mode
*                 A or D: Submit data. B: Backspace. C: Clear
*                 You can specify where to start the input field and the size
* Post-conditions: Returns \0 if no button is pressed, or the button otherwise
*                  ( only numeric values)
*/
int keypad_disp(int column, int row, int size);

/** void password(void);
* Objective: This function halts the execution of the rest of the program
             until the user enters the password
* Pre-conditions: The password does not contain more than 8 digits
*                 Keypad is working
* Post-conditions: The rest of the program will resume. After this stage
*                  the user might be able to see the password 
*/
void password(void);

/** void changeTempMax(void);
* Objective: This function changes the maximum temperature using the keypad
* Pre-conditions: Keypad is working
*                 temp_min and temp_mid is < temp_max
* Post-conditions: The temp_max value will be changed
*/
void changeTempMax(void);

/** void changeTempMid(void);
* Objective: This function changes the medium temperature using the keypad
* Pre-conditions: Keypad is working
*                 temp_min is < temp_mid < temp_max
* Post-conditions: The temp_mid value will be changed
*/
void changeTempMid(void);

/** void changeTempMin(void);
* Objective: This function changes the medium temperature using the keypad
* Pre-conditions: Keypad is working
*                 temp_min is < temp_mid < temp_max
* Post-conditions: The temp_min value will be changed
*/
void changeTempMin(void);

/** void changeTempEmergTimer(void);
* Objective: This function changes the emerg. timeout using the keypad
* Pre-conditions: Keypad is working
* Post-conditions: The TIMEOUT value will be changed
*/
void changeTempEmergTimer(void);

/** bool sureD(void);
* Objective: Asks the user if he is sure that he wants to select Default values
             Meanwhile, as he chooses, the default values are displayed
* Pre-conditions: Keypad is working
* Post-conditions: none. Returns true if yes he is sure, or false otherwise
*/
bool sureD(void);

/** void changeTempPass(void);
* Objective: Changes the password of the unit
* Pre-conditions: Keypad is working
* Post-conditions: The pass will be changed
*/
void changeTempPass(void);

/** void changeInit(void);
* Objective: Calls other initialization functions to setup the unit
* Pre-conditions: Keypad is working
* Post-conditions: none.
*/
void changeInit(void);

/** void read_temp(void);
* Objective: Reads the voltage at the LM35 temperature sensor
* Pre-conditions: Sensor is working and connected
* Post-conditions: Returns the reading in temp float variable
*/
void read_temp(void);

/** void pwm(void);
* Objective: Changes the pulse-width of the PWM pin
* Pre-conditions: myPwm is connected
* Post-conditions: myPwm is changed based on temp, temp_min/mid/max
*/
void pwm(void);

/** void led(void);
* Objective: Toggles the status of the leds based on temperature
* Pre-conditions: leds red, yellow, green are connected
* Post-conditions: leds red, yellow, green change based on temp,temp_min/mid/max
*/
void led(void);

/** void uart(void);
* Objective: Displays temperature and description on UART
* Pre-conditions: pc is serially connected to the unit
* Post-conditions: data will be displayed on the port terminal on PC
*/
void uart(void);

/** void display_temp(void);
* Objective: Displays temperature on keypad with average temperature
* Pre-conditions: The temperature have been calculate
* Post-conditions: none.
*/
void display_temp(void);


/** void temperature_average(void);
* Objective: Calculates the average temperature
* Pre-conditions: none.
* Post-conditions: temp_average is changed
*/
void temperature_average(void);

/** void flash_emergency_message(bool &exclamation);
* Objective: Flashes a line that reads emergency on line1 of the LCD
* Pre-conditions: An LCD is connected to the GPIO
* Post-conditions: The first line of the LCD will be changed
*/
void flash_emergency_message(bool &exclamation);

/** void emergency(void);
* Objective: Stops the PWM and the leds, and goes into a timeout
* Pre-conditions: none.
* Post-conditions: Stops all executions until the timeout is over
*/
void emergency(void);

/** void modify_password(void);
* Objective: Changes the password from the pc keyboard
* Pre-conditions: PC is connected
* Post-conditions: pass is changed
*/
void modify_password(void);

/** void modify_temp_min(void);
* Objective: Changes the minimum temperature temp_min
* Pre-conditions: PC is connected
* Post-conditions: temp_min is changed
*/
void modify_temp_min(void);

/** void modify_temp_mid(void);
* Objective: Changes the medium temperature temp_mid
* Pre-conditions: PC is connected
* Post-conditions: temp_mid is changed
*/
void modify_temp_mid(void);

/** void modify_temp_max(void);
* Objective: Changes the minimum temperature temp_max
* Pre-conditions: PC is connected
* Post-conditions: temp_max is changed
*/
void modify_temp_max(void);


/** void modify_temp(void);
* Objective: Changes the minimum/medium/maximum temperature values
* Pre-conditions: PC is connected
* Post-conditions: temp_min/mid/max are changed
*/
void modify_temp(void);

/** void modify_timer(void);
* Objective: Changes the timeout timer of the emergency
* Pre-conditions: PC is connected
* Post-conditions: TIMEOUT is changed
*/
void modify_timer(void);

/**  void remote_session(void);
* Objective: Prompts the user to choose what he wants to modify
* Pre-conditions: PC is connected
* Post-conditions: temp_min/mid/max, TIMEOUT, pass all might be modified
*/
void remote_session(void);

/** void emerg_thread_activation(void);
* Objective: Activates the emergency thread if a button is pressed
* Pre-conditions: Emergency thread is waiting for a signal
* Post-conditions: Sets the signal emergency thread is waiting for
*/
void emerg_thread_activation(void);

/** void keyboard_readable(void);
* Objective: Keeps checking if there is a key pressed on the keyboard
* Pre-conditions: PC is connected
* Post-conditions: Sets a signal for keyboard_input to resume
*/
void keyboard_readable(void);

/** void keyboard_input(void);
* Objective: If a key is pressed, check if it is either E or R
* Pre-conditions: PC is connected
* Post-conditions: Sets a signal for the corresponding thread
*/
void keyboard_input(void);
