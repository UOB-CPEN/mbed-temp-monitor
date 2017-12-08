/*-- TempQueue.cpp---------------------------------------------------------
             This file implements Queue member functions.
-------------------------------------------------------------------------*/

#include <iostream>
using namespace std;

#include "TempQueue.h"

//--- Definition of TempQueue constructor
TempQueue::TempQueue()
{
    count = 0;
    full = false;
}

int count = 0;
//--- Definition of enqueue()
void TempQueue::enqueue(const QueueElement & value)
{
    if(count > 9)
    {
        count = 0;
        full = true;
    }
    myArray[count++] = value;

}

//--- Definition of display()
void TempQueue::display(char * out) 
{
    int size;
    if(full)
        size = QUEUE_CAPACITY;
    else
        size = count;
    
   for (int i = 0; i < size ; i++)
     sprintf(out + strlen(out), "%d: %f ", i, myArray[i]);
    sprintf(out + strlen(out), "%f\n\r", average());
}

//--- Definition of front()
float TempQueue::average() const
{
    int size;
    float avg = 0;
    
    if(full)
        size = QUEUE_CAPACITY;
    else
        size = count;
    
    for (int i = 0; i < size ; i++)
    {
        avg += myArray[i];
    }
    return (avg / size);
        
        }


