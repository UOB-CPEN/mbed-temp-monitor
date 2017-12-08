/* Author(s): Yehia Naja & Mohammad Khodor
 *            Copyright (c) 2018
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Contributors:
 *                   mbed Microcontroller Library
 *                   Copyright (c) 2006-2012 ARM Limited
 *
 *          This  project  was  made  possible  by  the  generous  
 *          donation    from   The   ARM     University   Program 
 *          of   STMicroelectronics   NUCLEO-F401RE   Development                  
 *          Boards    to    the    University    of      Balamand
 *         
 *      This   Project   was  done  at   the  University  of   Balamand
 *      under the supervision of Dr. Rafic Ayoubi and Mr. Ghattas Akkad
 *      as   part    of   the   CPEN309    course    in    Fall    2017
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "driver.h"


// definition of the function that waits for the user to enter a value
char keypad_wait(void)
{
    // The user is expected to press a key after we enter the function.
    // If the user is already pressing a key, wait for the key to be done.
    while(keypad_pressed());
    // This character will store the key
    char c = 0;
    // As long as the user haven't entered a key
    while(c == 0) {
        // Keep reading from the keypad
        c = keypad();
    }
    // return the entered key
    return c;
}

// definition of the function that waits for the user to enter an alphabetic key
// disable_[letter] allows disabling a specific letter from being accepted
char keypad_ABCD(bool disable_a = false, bool disable_b = false,
                 bool disable_c = false, bool disable_d = false )
{

    // The user is expected to press a key after we enter the function
    // if the user is already pressing a key, wait for the key to be done
    while(keypad_pressed());
    // This character will store the key
    char c = 0;
    // As long as the user haven't entered an acceptable key
    while( ( c != 'A' || disable_a) && (c != 'B' || disable_b)
            && (c != 'C' || disable_c) && (c != 'D' || disable_d) ) {
        // Keep reading from the keypad
        c = keypad();
    }
    // return the entered key
    return c;
}

// definition of the function that waits for the user to enter a key and
// displays it along his entry. Location on the LCD can be specified
// along with the maximum size of the display. A, B, C, and D letters are not
// inputs and therefore can assist the user in entry:
// A: Submit
// B: Backspace
// C: Clear
// D: Submit
// Returns: A number from the keypad neglecting the # and the * keys
int keypad_disp(int column, int row, int size)
{
    // Intialize the character that will hold the input from the user
    char c = 0;
    // The value that the user is trying to enter iteratively until he submits
    char value[size + 1];
    // Locate the lcd to the desired spot
    lcd.locate(column, row);
    // Enter blanks '_' on the screen equal to the maximum size of entry
    for(int i = column; i < column + size; i++) {
        lcd.putc('_');
    }
    // Relocate the LCD back at the beginning of the entry blanks
    lcd.locate(column, row);
    // How many values are entered shifting the cursor from the origin
    int shift = 0;
    // Keep looping until a condition breaks the loop
    while(true) {
        // Reads a value entered by the user (this function waits for the user)
        c = keypad_wait();
        // If the user is done, return from this loop:
        if(c == 'D' || c == 'A')
            break;
       
        // If the user wants to go back a character:
        if(c == 'B') {
            // go back if he is not on the first character (nothing entered yet)
            shift = (shift <= 0 ? 0 : shift - 1);
            // relocate the lcd whether on new position or on the same on
            lcd.locate(column + shift, row);
            // declare this position as available by putting blank '_'
            lcd.putc('_');

            // if the user wants to clear the line:
        } else if( c == 'C') {
            // if the user decides to re-enter the values, shift back to the
            // beginning
            shift = 0;
            // relocate the screen accordingly
            lcd.locate(column + shift, row);
            // Enter blanks '_' on the screen equal to the maximum size of entry
            for(int i = column; i < column + size; i++) {
                lcd.putc('_');
            }
            // Relocate the LCD back at the beginning of the entry blanks
            lcd.locate(column, row);

            // if the user enters an actual number, make sure it fits
        } else if(shift < size ) {
            // relocate the cursor according to the shift value
            lcd.locate(column + shift, row);
            // insert the character on screen
            lcd.putc(c);
            // add the inserted value to the list of collected valuess
            value[shift] = c;
            // shift by a character position for the next one, or to declare
            // that no more characters can be placed
            shift++;
        }

    }
    // add an end of string at the end to be able to parse the value
    value[shift] = '\0';
    // return the parsed value to the user
    // any non-digit character will be ignored
    // if no values are entered, simply return 0
    return atoi(value);
}

// Definition of keypad fast checking if there is a pressed keypad or not
bool keypad_pressed(void)
{
    // Make all rows = 0 for fast checking
    r0 = r1 = r2 = r3 = 0;
    wait_ms(1);
    // See if any of the columns transmitted these zeros
    return !c0 || !c1 || !c2 || !c3;
}

// Keypad: returns a value when a key is pressed, or \0 otherwise
char keypad(void)
{
    // fast check that indeed a key is pressed
    if(!keypad_pressed())
        // if not, return \0 declaring no key was found
        return 0;
    // make sure that all outputs do show a digital HIGH in order to change
    // this value later on
    r0 = r1 = r2 = r3 = 1;
    // give enough time for the previous line to take place
    wait_ms(1);

    // start by the first row, soft ground it and see if any columns become 0
    r0 = 0;
    // if the first column became 0: return 1
    if(c0 == 0)
        return '1';
    // if the second column became 0: return 2
    if(c1 == 0)
        return '2';
    // if the third column became 0: return 3
    if(c2 == 0)
        return '3';
    // if the fourth column became 0: return A
    if(c3 == 0)
        return 'A';
    // if no key was pressed on the first row, return its value to 1
    r0 = 1;

    // Go to the second row, soft ground it and see if any columns become 0
    r1 = 0;
    // if the first column became 0: return 4
    if(c0 == 0)
        return '4';
    // if the second column became 0: return 5
    if(c1 == 0)
        return '5';
    // if the third column became 0: return 6
    if(c2 == 0)
        return '6';
    // if the fourth column became 0: return B
    if(c3 == 0)
        return 'B';
    // if no key was pressed on the second row, return its value to 1
    r1 = 1;

    // Go to the third row, soft ground it and see if any columns become 0
    r2 = 0;
    // if the first column became 0: return 7
    if(c0 == 0)
        return '7';
    // if the second column became 0: return 8
    if(c1 == 0)
        return '8';
    // if the third column became 0: return 9
    if(c2 == 0)
        return '9';
    // if the fourth column became 0: return C
    if(c3 == 0)
        return 'C';
    // if no key was pressed on the third row, return its value to 1
    r2 = 1;

    // Go to the fourth row, soft ground it and see if any columns become 0
    r3 = 0;
    // if the first column became 0: return *
    if(c0 == 0)
        return '*';
    // if the second column became 0: return 0
    if(c1 == 0)
        return '0';
    // if the third column became 0: return #
    if(c2 == 0)
        return '#';
    // if the fourth column became 0: return D
    if(c3 == 0)
        return 'D';
    // if no key was pressed on the fourth row, return its value to 1
    r3 = 1;

    // Keep waiting until the key is released
    // while(keypad_pressed());

    // return the \0 indicating: failed to identify the key
    return 0;
}

// Definition of maximum temperature mutator by the aid of the LCD and a keypad
void changeTempMax(void)
{
    // Clear previous values from the screen
    lcd.cls();
    // Relocate the screen to the origin
    lcd.locate(0,0);
    // Display a message with the current maximum temperature
    lcd.printf("TempMax = %3dC  ", tempMax);
    // Display a message on the UART declaring the current state
    pc.printf("Prompting the user to change temperature maximum.\n\r");
    // Read the temperature from the user
    tempMax = keypad_disp(6, 1, 3);
    // Clear the screen after input
    lcd.cls();
    // Relocate the screen back to the origin
    lcd.locate(0,0);
    // display a suitable message
    lcd.printf("      DONE!      ");
    // wait for the user to release the key
    while(keypad_pressed());
    // Display a message on the UART declaring the current state
    pc.printf("Temperature Maximum changed to: %d\n\r", tempMax);
    // wait some time after the user releases the key
    //  before the message disappears
    wait_ms(100);
}

// Definition of medium temperature mutator by the aid of the LCD and a keypad
void changeTempMid(void)
{
    // Clear previous values from the screen
    lcd.cls();
    // Relocate the screen to the origin
    lcd.locate(0,0);
    // Display a message with the current medium temperature
    lcd.printf("TempMid = %3dC  ", tempMid);
    // Display a message on the UART declaring the current state
    pc.printf("Prompting the user to change temperature average.\n\r");
    // Read the temperature from the user
    tempMid = keypad_disp(6, 1, 3);
    // Clear the screen after input
    lcd.cls();
    // Relocate the screen back to the origin
    lcd.locate(0,0);
    // display a suitable message
    lcd.printf("      DONE!      ");
    // wait for the user to release the key
    while(keypad_pressed());
    // Display a message on the UART declaring the current state
    pc.printf("Temperature Medium changed to: %d\n\r", tempMid);
    // wait some time after the user releases the key
    //  before the message disappears
    wait_ms(100);
}

// Definition of minimum temperature mutator by the aid of the LCD and a keypad
void changeTempMin(void)
{
    // Clear previous values from the screen
    lcd.cls();
    // Relocate the screen to the origin
    lcd.locate(0,0);
    // Display a message with the current minimum temperature
    lcd.printf("TempMin = %3dC  ", tempMin);
    // Display a message on the UART declaring the current state
    pc.printf("Prompting the user to change temperature minimum.\n\r");
    // Read the temperature from the user
    tempMin = keypad_disp(6, 1, 3);
    // Clear the screen after input
    lcd.cls();
    // Relocate the screen back to the origin
    lcd.locate(0,0);
    // display a suitable message
    lcd.printf("      DONE!      ");
    // wait for the user to release the key
    while(keypad_pressed());
    // Display a message on the UART declaring the current state
    pc.printf("Temperature Minimum changed to: %d\n\r", tempMin);
    // wait some time after the user releases the key
    //  before the message disappears
    wait_ms(100);
}

// Definition of emergency timeout mutator by the aid of the LCD and a keypad
void changeTempEmergTimer(void)
{
    // Clear previous values from the screen
    lcd.cls();
    // Relocate the screen to the origin
    lcd.locate(0,0);
    // Display a message with the current timeout value
    lcd.printf("TIMEOUT = %3d    ", TIMEOUT);
    // Display a message on the UART declaring the current state
    pc.printf("Prompting the user to change the"
        " emergency timer value from: %d.\n\r", TIMEOUT);
    // Read the timeout value from the user
    TIMEOUT = keypad_disp(6, 1, 3);
    // Clear the screen after input
    lcd.cls();
    // Relocate the screen back to the origin
    lcd.locate(0,0);
    // display a suitable message
    lcd.printf("      DONE!      ");
    // wait for the user to release the key
    while(keypad_pressed());
    // Display a message on the UART declaring the current state
    pc.printf("Emergency timer value changed to: %d\n\r", TIMEOUT);
    // wait some time after the user releases the key
    //  before the message disappears
    wait_ms(100);

}
// Definition of the verification function that the user wants default values
bool sureD(void)
{
    // wait for the user to release the key
    while(keypad_pressed());
    // Clear previous values from the screen
    lcd.cls();
    // Relocate the screen to the origin
    lcd.locate(0,0);
    // Display a message with the confirmation
    lcd.printf("Sure? A:Yes B:No");
    // Display a message on the UART declaring the current state
    pc.printf("Making sure the user wants to keep the default\n\r");
    // variable to save the user's entry
    char choice;
    // this variable states the index of the message being printed
    int i = 0;
    // Timer to switch the messages on the screen
    Timer t2;
    // Start the timer
    t2.start();
    do {
        // if the timer is greater than 2 seconds
        if(t2 > 2) {
            // if the index is out of bound, go back to zero
            if(i == 3) i = 0;
            // Relocate the screen to the second line
            lcd.locate(0,1);
            // Print the messages that were prepared by the previous function
            // without recreating them
            lcd.printf("%s", msg[i++]);
            // reset the timer to wait for another 2 seconds before updating
            t2.reset();
        }
        // reach the input from the user -if any
        choice = keypad();
        // keep repeating until the user enters an A or B
    } while(choice != 'A' && choice != 'B');
    // clear the LCD screen from the previous values
    lcd.cls();
    // Relocate the screen back to the origin
    lcd.locate(0,0);
    // If the user is sure
    if(choice == 'A')
        // display a suitable message
        lcd.printf("Using default   ");
    // If the user is not sure
    if(choice == 'B')
        // display a suitable message
        lcd.printf("Changing Default");
    // make sure previous command was fully executed
    wait_ms(20);
    // wait for the user to release the key
    while(keypad_pressed());
    // Display a message on the UART declaring the current state
    pc.printf("Default values is %schoosen\n\r", choice == 'A'? "": "not ");
    // make sure previous command was fully executed
    wait_ms(20);
    // Clear the screen after input
    lcd.cls();
    // wait some time after the user releases the key
    //  before the message disappears
    wait(0.1);
    // return the answer to the question: is the user sure?
    return choice == 'A';
}

// Definition of password mutator by the aid of the LCD and a keypad
void changeTempPass(void)
{
    // Clear previous values from the screen
    lcd.cls();
    // Relocate the screen to the origin
    lcd.locate(0,0);
    // Display a message with the current password
    lcd.printf("PASS = %8d", pass);
    // Display a message on the UART declaring the current state
    pc.printf("Prompting the user to change the password from: %d.\n\r", pass);
    // Read the password value from the user
    pass = keypad_disp(4, 1, 8);
    // Clear the screen after input
    lcd.cls();
    // Relocate the screen back to the origin
    lcd.locate(0,0);
    // display a suitable message
    lcd.printf("      DONE!      ");
    // wait for the user to release the key
    while(keypad_pressed());
    // Display a message on the UART declaring the current state
    pc.printf("Password changed to: %d\n\r", pass);
    // wait some time after the user releases the key
    //  before the message disappears
    wait_ms(100);
}

// Definition of aggregated temperature mutator by the aid of 
// the LCD and a keypad
void changeInit()
{
    // change minimum temperature
    changeTempMin();
    // change medium temperature
    changeTempMid();
    // change maximum temperature
    changeTempMax();
    // change emergency timeout
    changeTempEmergTimer();
    // change password
    changeTempPass();
}


// Definition of initialization function that uses the keypad to allow the 
// user to enter reference values. With the aid of the lcd screen, the user
// can choose default or new values and see the already available values.
void init_mode(void)
{
    // counts the current message on screen
    int count = 0;
    // The input of the user is stored in option
    char option = 0;
    // allocate a place in the memory for the messages to be displayed
    for(int i = 0; i < 3; i++)
        msg[i] = new char[buffer];
    // relocate the lcd to the origin
    lcd.locate(0,0);
    // prompt the user to choose either default or custom values
    lcd.printf("Custom/Default?");
    // timer is created that will allow us to change the displayed message
    Timer t;
    // start the counter before looping
    t.start();
    // as long as the user did not choose an option, keep looping
    while(1) {
        // update the values in the relative message in order to display them
        if(count == 0) {
            // the message is updated to current tempMin
            // the size of the message is able to fit in our lcd: 16 characaters
            sprintf( msg[0], "TempLow = %3dC  ",tempMin);
        }
        // update the values in the relative message in order to display them
        if(count == 1) {
            // the message is updated to current tempMid
            // the size of the message is able to fit in our lcd: 16 characaters
            sprintf( msg[1], "TempMid = %3dC  ",tempMid);
        }
        // update the values in the relative message in order to display them
        if(count == 2) {
            // the message is updated to current tempMax
            // the size of the message is able to fit in our lcd: 16 characaters
            sprintf( msg[2], "TempHigh = %3dC ",tempMax);
        }
        // relocate the lcd to the origin
        lcd.locate(0,1);
        // print the message that we just updated on the screen
        lcd.printf("%s", msg[count]);
        // return the value of option to 0 
        option = 0;
        // do not test user's input unless a key is pressed
        if(keypad_pressed()) {
            // save the value of the entered key
            option = keypad();
            // wait till the user releases the key
            while(keypad_pressed());

        }
        // if the timer value is greater than 1.5 seconds
        if(t.read() > 1.5f) {
            // reset the timer to start counting again
            t.reset();
            // increment the count value to display the second message
            if(++count > 2)
                // if count is greater than 2 (incremented beyond our array)
                // return its value to 0
                count = 0;
        }
        // if C is pressed then break from the loop and change initialization
        if(option == 'C')
            break;
        // if D is pressed then make sure the user wants the default values
        if(option == 'D') {
            // if the user is sure he wants the default values exit loop
            if(sureD())
                break;
            else {
                // if the user is not sure, this means he wants to customize
                // then change the option value to C and exit the loop
                option = 'C';
                break;
            }
        }
    }
    // if the option choosen is C, then go ahead and change the initial values
    if(option == 'C')
        changeInit();
}

// Defiinition of password function that halts the system unless the password
// is enter
void password(void)
{
    // number of attempts permitted 
    int attempts = 3;
    // the state of our lock
    bool correct = false;
    // delete previous values on the screen
    lcd.cls();
    // relocate the lcd back to the origin
    lcd.locate(0,0);
    // display the current state of the system: Locked
    lcd.printf("     LOCKED     ");

    // keep trying as long as there is an attempt left
    while(attempts > 0) {

        // if the keypad contains the password
        if( pass == keypad_disp(4, 1, 8)) {
            // Display a message on the UART declaring the current state
            pc.printf("Password is correct! Entered system.\n\r");
            // Declare the password entered as correct
            correct = true;
            // break out of the loop
            break;
        } else {
            // Display a message on the UART declaring the current state
            pc.printf("Entered password is wrong!\n\r");
            // relocate the lcd back to the origin
            lcd.locate(0,0);
            // decrement the number of attempts left
            attempts--;
            // display a message on the lcd stating that the password is 
            // incorrect along with the number of remaining attempts
            lcd.printf("Wrong:%d attempts", attempts);
            
        }

    }
    // clear the lcd screen to display the current state of the system
    lcd.cls();
    // relocate the lcd back to the origin
    lcd.locate(0,0);
    // if the password is incorrect
    if(!correct) {
        // display a message stating that the system is locked
        lcd.printf("     LOCKED     ");
        // Display a message on the UART declaring the current state
        pc.printf("The system has been locked "
            "due to many failed attempts.\n\r");
        // halt the system in an endless loop
        while(1);
    } else
        // if the password is correct, display a message stating that the 
        // system is unlocked
        lcd.printf("    UNLOCKED    ");
        // Display a message on the UART declaring the current state
        pc.printf("The system has been unlocked.\n\r");

    // wait before proceeding to leave enough time for reading
    wait(1);
}

// definition of temperature reading thread
void read_temp(void)
{   // thread loop
    while(1) {
        // Read the analog voltage to temperature reading
        temp = temp_sensor * 100;
        // Set a signal that the temperature has been read for avg to be calc.
        temperature_average_thread.signal_set(1);
        // Wait some time before reading the temperature again
        Thread::wait(thread_wait_long);
    }

}

// definiton of a thread to control the pulse width of the PWM output
void pwm(void)
{
    // thread loop
    while(1) {
        // if the temperature is less than the minimum temperature
        if(temp < tempMin)
            // turn of the PWM output
            mypwm = 0;
        // if the temperature is greater than the minimum temperature 
        // but less than the medium temperature
        else if (temp < tempMid)
            // turn on the pwm at 30% 
            mypwm = 0.3;
        // if the temperature is greater than the medium temperature
        // but less than the maximum temperature
        else if (temp < tempMax)
            // turn on the pwm at 60%
            mypwm = 0.6;
        // if the temperature is greater than the maximum temperature
        else
            // turn on fully the pwm
            mypwm = 1;
        // wait sometime before checking again for changes
        // Let other threads before their work meanwhile (e.g. read temperature)
        Thread::wait(thread_wait_long);
    }
}

// definition for led thread that controls the 3 colored leds
void led(void)
{
    // thread loop
    while(1) {
        // if the temperature is less than the minimum temperature
        if(temp < tempMin) {
            // turn on yellow only
            green = 0;
            yellow = 1;
            red = 0;
        // if the temperature is greater than the minimum temperature
        // but less than the medium temperature
        } else if(temp < tempMid) {
            // turn on both green and yellow
            green = 1;
            yellow = 1;
            red = 0;
        // if the temperature is greater than the medium temperature
        // but less than the maximum temperature
        } else if(temp < tempMax) {
            // turn on both green and red 
            green = 1;
            yellow = 0;
            red = 1;
        // if the temperature is even greater than the maximum temperature
        } else {
            // turn on red only
            green = 0;
            yellow = 0;
            red = 1;
        }
        // wait sometime before checking again for changes
        // Let other threads before their work meanwhile (e.g. read temperature)
        Thread::wait(thread_wait_long);
    }
}

// definition for uart thread that transmits the temperature information by uart
void uart(void)
{
    // thread while
    while(1) {
        // if the temperature is less than the minimum temperature
        if(temp < tempMin) {
            // Display a message on the UART declaring the current state
            pc.printf("Cold %fC\r\n", temp);
        // if the temperature is greater than the minimum temperature
        // but less than the medium temperature
        } else if(temp < tempMid) {
            // Display a message on the UART declaring the current state
            pc.printf("Stable %fC\r\n", temp);
        // if the temperature is greater than the medium temperature
        // but less than the maximum temperature
        } else if(temp < tempMax) {
            // Display a message on the UART declaring the current state
            pc.printf("High %fC\r\n", temp);
        // if the temperature is even greater than the maximum temperature
        } else {
            // Display a message on the UART declaring the current state
            pc.printf("Heated %fC\r\n", temp);

        }
        // wait sometime before checking again for changes
        // Let other threads before their work meanwhile (e.g. read temperature)
        Thread::wait(thread_wait_med);
    }

}

// This thread displays the read temperature onto the lcd screen
void display_temp(void)
{
    // thread loop
    while(1) {
        // Relocate the lcd to its origin
        lcd.locate(0,0);
        // Display the temperature along with the average temperature
        lcd.printf("T: %3.0fC TA: %3.0fC", temp, temp_avg);
        // Wait sometime before displaying the new values
        Thread::wait(thread_wait_long);
    }
}

// This thread calculates the average temperature of the last ten values
void temperature_average(void)
{
    // thread loop
    while(1) {
        // make sure that a new value of the temperature has been read before
        // calculating the average
        temperature_average_thread.signal_wait(1);
        // add the current temperature reading to the queue
        // in case the queue already holds ten elements, replace the oldest
        averages.enqueue(temp);
        // save the value of the average
        temp_avg = averages.average();
    }
}

// Definition of the emergency line flasher 
void flash_emergency_message(bool &exclamation)
{
    // Relocate the lcd screen to its origin
    lcd.locate(0,0);
    // If last time exclamations were used in the message, hide them
    if(exclamation)
        // display emergency statement on the lcd screen
        lcd.printf("   EMERGENCY    ");
    else
        // display emergency statement on the lcd screen with exclamations
        lcd.printf("!! EMERGENCY !! ");
    // indicate whether exlamations were used this time in the message or not
    exclamation = !exclamation;
}

// Definition of emergency thread that halts the system for some time defined
// by the user or uses the default time
void emergency(void)
{
    // saves the state of the previous message
    bool exclamation = true;
    // timer for the timeout duration
    Timer t;
    // Thread loop
    while(1) {
        // wait for the conditions of this emergency process to take place
        emergency_thread.signal_wait(1);
        // reset the timer back to zero to start timing the duration
        t.reset();
        // turn off all outputs
        mypwm = 0;
        red = 0;
        yellow = 0;
        green = 0;
        // start the timer
        t.start();
        // keep looping until the time waited have met the timeout duration
        while((int)(t.read()) < TIMEOUT) {
            // meanwhile, flash the emergency statement
            flash_emergency_message(exclamation);
            // relocate the lcd back to the origin
            lcd.locate(0,1);
            // print the time left to go back to normal state on the lcd screen
            lcd.printf("Back In: %2d     ", TIMEOUT - (int)(t.read())  );
            // Display a message on the UART declaring the current state
            pc.printf(" Emergency: timer = %f\r\n", t.read());
            // wait for sometime before checking again if the duration is met,
            // and before changing the exclamation message again
            wait_ms(200);
        }
        // if we reached the timeout duration, stop the timer
        t.stop();
        // wait for sometime before proceeding
        // this gives the user enough time to know that time is up
        wait_ms(1000);
        // clear the lcd from the previous values
        lcd.cls();
        // keep the screen off for some time
        wait_ms(100);
        // clear the signal so that we can detect new signals
        emergency_thread.signal_clr(1);
    }
}

/*
int main2()
{
    set_time(1256729737);  // Set RTC time to Wed, 28 Oct 2009 11:35:37
}
*/

// Definition of password modification using the pc keyboard
void modify_password(void)
{
    // Display a message on the UART declaring the current state
    pc.printf("Password is: %d\n\rEnter new password: ", pass);
    // The maximum characters allowed by the keyboard
    const int char_limit = 8;
    // save the input of the user in this array
    char * password_user = new char[char_limit + 1];
    // get the first character from the pc
    char c = pc.getc();
    // the index of the free position in our array
    int i = 0;
    // keep looping until the user enters a space or an enter
    while(c != '\n' && c != ' ') {
        // if the character is backspace, go back a character
        if(c == '\b' && i != 0) {
            // decrement the last index of the array
            i--;
            // go back a single character on the screen
            pc.putc('\b');
            // display a space instead of the previous character
            pc.putc(' ');
            // then go back again to the previous character position
            pc.putc('\b');
        // if the character is numeric and there is a space in our array
        } else if( c >= '0' && c <= '9' && i < char_limit ) {
            // add the character
            password_user[i++] = c;
            // display the character on the terminal
            pc.putc(c);
        }
        // update the character that we have to the next one
        c = pc.getc();
    }
    // add the end of string to be able to use string functions
    password_user[i] = '\0';
    // Display a message on the UART declaring the current state
    pc.printf("\n\rPassword was changed from %d", pass);
    // convert the entered password to an integer
    pass = atoi(password_user);
    // Display a message on the UART declaring the current state
    pc.printf(" to %d\n\r", pass);
    // free password_user 
    delete password_user;
}

// Definition of minimum temperature modification using terminal
void modify_temp_min(void)
{
    // Display a message on the UART declaring the current state
    pc.printf("Minimum temperature is: %dC\n\rEnter new minimum temperature: ",
     tempMin);
    // The character limits of the input
    const int char_limit = 3;
    // the array that will hold the input of the user
    char * user = new char[char_limit + 1];
    // get the first character from the terminal
    char c = pc.getc();
    // the index of the array
    int i = 0;
    // keep looping until the character is space or enter
    while(c != '\n' && c != ' ') {
        // if the character is backspace and there is a character previously
        // entered
        if(c == '\b' && i != 0) {
            // decrement the index of the array
            i--;
            // go back a single character on the terminal
            pc.putc('\b');
            // overwrite the character with whitespace
            pc.putc(' ');
            // then go back again a single character
            pc.putc('\b');
        // if the character is numeric and the index of the array is within the
        // limit
        } else if( c >= '0' && c <= '9' && i < char_limit ) {
            // add the character
            user[i++] = c;
            // display the character on the terminal
            pc.putc(c);
        }
        // fetch the next character from the terminal
        c = pc.getc();
    }
    // Add the end of string at the end of the array to enable use to use
    // string functions
    user[i] = '\0';
    // Display a message on the UART declaring the current state
    pc.printf("\n\rMinimum temperature changed from %dC", tempMin);
    // Convert the user input to integer and store in tempMin
    tempMin = atoi(user);
    // Display a message on the UART declaring the current state
    pc.printf(" to %dC\n\r", tempMin);
    // free user
    delete [] user;

}
// Definition of medium temperature modification using terminal
void modify_temp_mid(void)
{   
    // Display a message on the UART declaring the current state
    pc.printf("Medium temperature is: %dC\n\rEnter new medium temperature: ",
     tempMid);
    // The character limits of the input
    const int char_limit = 3;
    // the array that will hold the input of the user
    char * user = new char[char_limit + 1];
    // get the first character from the terminal
    char c = pc.getc();
    // the index of the array
    int i = 0;
    // keep looping until the character is space or enter
    while(c != '\n' && c != ' ') {
        // if the character is backspace and there is a character previously
        // entered
        if(c == '\b' && i != 0) {
            // decrement the index of the array
            i--;
            // go back a single character on the terminal
            pc.putc('\b');
            // overwrite the character with whitespace
            pc.putc(' ');
            // then go back again a single character
            pc.putc('\b');
        // if the character is numeric and the index of the array is within the
        // limit
        } else if( c >= '0' && c <= '9' && i < char_limit ) {
            // add the character
            user[i++] = c;
            // display the character on the terminal
            pc.putc(c);
        }
        // fetch the next character from the terminal
        c = pc.getc();
    }
    // Add the end of string at the end of the array to enable use to use
    // string functions
    user[i] = '\0';
    // Display a message on the UART declaring the current state
    pc.printf("\n\rMedium temperature changed from %dC", tempMid);
    // Convert the user input to integer and store in tempMid
    tempMid = atoi(user);
    // Display a message on the UART declaring the current state
    pc.printf(" to %dC\n\r", tempMid);
    // free user
    delete [] user;
}

// Definition of maximum temperature modification using terminal
void modify_temp_max(void)
{
    // Display a message on the UART declaring the current state
    pc.printf("Maximum temperature is: %dC\n\rEnter new maximum temperature: ",
     tempMax);
    // The character limits of the input
    const int char_limit = 3;
    // the array that will hold the input of the user
    char * user = new char[char_limit + 1];
    // get the first character from the terminal
    char c = pc.getc();
    // the index of the array
    int i = 0;
    // keep looping until the character is space or enter
    while(c != '\n' && c != ' ') {
        // if the character is backspace and there is a character previously
        // entered
        if(c == '\b' && i != 0) {
            // decrement the index of the array
            i--;
            // go back a single character on the terminal
            pc.putc('\b');
            // overwrite the character with whitespace
            pc.putc(' ');
            // then go back again a single character
            pc.putc('\b');
        // if the character is numeric and the index of the array is within the
        // limit
        } else if( c >= '0' && c <= '9' && i < char_limit ) {
            // add the character
            user[i++] = c;
            // display the character on the terminal
            pc.putc(c);
        }
        // fetch the next character from the terminal
        c = pc.getc();
    }
    // Add the end of string at the end of the array to enable use to use
    // string functions
    user[i] = '\0';
    // Display a message on the UART declaring the current state
    pc.printf("\n\rMaximum temperature changed from %dC", tempMax);
    // Convert the user input to integer and store in tempMin
    tempMax = atoi(user);
    // Display a message on the UART declaring the current state
    pc.printf(" to %dC\n\r", tempMax);
    // free user
    delete [] user;
}
// definition of temperature parameters modification through the terminal
void modify_temp(void)
{
    // modifies the minimum temperature through the terminal
    modify_temp_min();
    // modifies the medium temperature through the terminal
    modify_temp_mid();
    // modifies the maximum temperature through the terminal
    modify_temp_max();
}

// definition of timeout parameter modification through the terminal
void modify_timer(void)
{
    // Display a message on the UART declaring the current state
    pc.printf("Value of timeout timer is: %ds\n\rEnter new value: ", TIMEOUT);
    // The character limits of the input
    const int char_limit = 8;
    // the array that will hold the input of the user
    char * user = new char[char_limit + 1];
    // get the first character from the terminal
    char c = pc.getc();
    // the index of the array
    int i = 0;
    // keep looping until the character is space or enter
    while(c != '\n' && c != ' ') {
        // if the character is backspace and there is a character previously
        // entered
        if(c == '\b' && i != 0) {
            // decrement the index of the array
            i--;
            // go back a single character on the terminal
            pc.putc('\b');
            // overwrite the character with whitespace
            pc.putc(' ');
            // then go back again a single character
            pc.putc('\b');
        // if the character is numeric and the index of the array is within the
        // limit
        } else if( c >= '0' && c <= '9' && i < char_limit ) {
            // add the character
            user[i++] = c;
            // display the character on the terminal
            pc.putc(c);
        }
        // fetch the next character from the terminal
        c = pc.getc();
    }
    // Add the end of string at the end of the array to enable use to use
    // string functions
    user[i] = '\0';
    // Display a message on the UART declaring the current state
    pc.printf("\n\rTimeout value changed from %ds", TIMEOUT);
    // Convert the user input to integer and store in TIMEOUT
    TIMEOUT = atoi(user);
    // Display a message on the UART declaring the current state
    pc.printf(" to %ds\n\r", TIMEOUT);
    // free user
    delete [] user;

}

// Definition of remote session thread
void remote_session(void)
{
    // Thread loop
    while(1) {
        // Wait for the conditions of this emergency process to take place
        remote_session_thread.signal_wait(1);
        // Turn off all the outputs
        yellow = 0;
        green = 0;
        red = 0;
        mypwm = 0;
        // Character c stores the input of the user
        char c = 0;
        // Keep looping until the user terminates the session
        while(c != '4') {
            // Reset the input of the user
            c = 0;
            // Display a message on the UART declaring the current state
            pc.printf("Remote session is activated\n\r");
            // Display a message on the UART declaring the current state
            pc.printf("All outputs are turned off\n\r");
            // Display a message on the UART explaining the valid options
            pc.printf("Enter a number corresponding"
                " to any of the following options: \n\r");
            // Display a message on the UART explaining the first option
            pc.printf("1. Modify Systems Startup Password\n\r");
            // Display a message on the UART explaining the second option
            pc.printf("2. Modify Temperature Limit Parameters\n\r");
            // Display a message on the UART explaining the third option
            pc.printf("3. Modify Emergency Timer Value\n\r");
            // Display a message on the UART explaining the fourth option
            pc.printf("4. Terminate Session\n\r");
            
            // Wait for the user to enter one of the given options
            while(c != '1' && c != '2' && c != '3' && c != '4')
            // Read the input from the user
                c = pc.getc();
            // If the user choose the first option
            if(c == '1')
                // modify the password
                modify_password();
            // If the user choose the second option
            if(c == '2')
                // modify the temperature parameters
                modify_temp();
            // If the user choose the third option
            if(c == '3')
                // modify the timeout duration
                modify_timer();
            // If the user choose the fourth option
            if(c == '4')
                // Display a message indicating that the session is over
                // before exiting
                pc.printf("Terminating\n\r");

        }// c != 4/

    } // while true/
} // remote_session/

// Definition of emergency button interrupt
void emerg_thread_activation(void)
{   
    // Send a signal for the emergency thread to resume
    emergency_thread.signal_set(1);
}

// Definition of terminal available thread
void keyboard_readable(void)
{
    // Thread loop
    while(1) {
        // If the terminal carries an input
        if(pc.readable()) {
            pc.printf("The input has been detected, analyzing the key...\n\r");
            // Send a signal to analyze the character and perform the 
            // necessary operation
            keyboard_input_thread.signal_set(1);
        }
        /*
        else //!\\
            osThreadYield();
            */
    }
}

// Definition of the keyboard input interpretation 
void keyboard_input(void)
{
    // This will hold the input from the keyboard
    char c;
    // Thread loop
    while(1) {
        // Wait until an input is detected
        keyboard_input_thread.signal_wait(1);
        // Store this input 
        c = pc.getc();
        // Display a message on the UART declaring the current state
        pc.printf("Entered keyboard input: %c\n\r", c);
        // If the input is E
        if(c == 'E' || c == 'e') {
            // enable the emergency thread
            emergency_thread.signal_set(1);
        // If the input is R
        } else if (c == 'R' || c == 'r') {
            // enable the remote session thread
            remote_session_thread.signal_set(1);
        }
    }

}

// Definition of the main function of the program
int main()
{
    // Clear the LCD screen
    lcd.cls();
    // Relocate the lcd screen back to the origin
    lcd.locate(0,0);
    // Enable the pullups of the inputs
    c0.mode(PullUp);
    c1.mode(PullUp);
    c2.mode(PullUp);
    c3.mode(PullUp);
    // Start the threads of the RTOS
    // Password thread is the first to start
    password_thread.start(password);
    // Set a high priority for the RTOS to leave the main function and perform
    // this thread
    password_thread.set_priority(osPriorityHigh);
    // Once the password thread is terminated, start the initalization mode
    init_mode_thread.start(init_mode);
    // Again, give it a high priority to perform it before the rest
    init_mode_thread.set_priority(osPriorityAboveNormal);
    // Then enable to threads that will perform in the round-robin
    read_temp_thread.start(read_temp);
    display_temp_thread.start(display_temp);
    temperature_average_thread.start(temperature_average);
    led_thread.start(led);
    pwm_thread.start(pwm);
    uart_thread.start(uart);
    // These threads will have a high priority, but will wait for a
    // signal to arrive. Once a signal arrives, the round-robin will stop until
    // the thread is once again looking for the same signal
    emergency_thread.start(emergency);
    emergency_thread.set_priority(osPriorityHigh);
    remote_session_thread.start(remote_session);
    remote_session_thread.set_priority(osPriorityHigh);
    keyboard_input_thread.start(keyboard_input);
    keyboard_input_thread.set_priority(osPriorityHigh);
    // The following interrupt sends a signal to enable a high priority 
    // thread to overtake RTOS
    emerg_button.rise(&emerg_thread_activation);
    // The following thread sends a signal to enable a high priority 
    // thread to overtake RTOS
    keyboard_readable_thread.start(keyboard_readable);

}