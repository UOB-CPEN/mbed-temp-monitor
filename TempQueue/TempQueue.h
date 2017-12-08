/* TempQueue.h contains the declaration of class TempQueue.
   Basic operations:
     Constructor: Constructs an empty queue
     empty:       Checks if a queue is empty
     enqueue:     Modifies a queue by adding a value at the back
     front:       Accesses the front queue value; leaves queue unchanged
     dequeue:     Modifies a queue by removing the value at the front
     display:     Displays the queue elements from front to back
   Class Invariant:
      1. The queue elements (if any) are stored in consecutive positions
         in myArray, beginning at position myFront.
      2. 0 <= myFront, myBack < QUEUE_CAPACITY
      3. Queue's size < QUEUE_CAPACITY
-------------------------------------------------------------------------*/

#include <iostream>

#ifndef TEMPQUEUE
#define TEMPQUEUE

//#include "AdditionProblem.h"
const int QUEUE_CAPACITY = 10;
typedef float QueueElement;

class TempQueue
{
 public:
  /***** Function Members *****/
  /***** Constructor *****/
  TempQueue();
  /*-----------------------------------------------------------------------
    Construct a Queue object.

    Precondition:  None.
    Postcondition: An empty Queue object has been constructed; myFront 
        and myBack are initialized to -1 and myArray is an array with
        QUEUE_CAPACITY elements of type QueueElement.
   ----------------------------------------------------------------------*/

  void enqueue(const QueueElement & value);
  /*-----------------------------------------------------------------------
    Add a value to a queue.

    Precondition:  value is to be added to this queue.
    Postcondition: value is added to back of queue provided there is space;
        otherwise, a queue-full message is displayed and execution is
        terminated.
   -----------------------------------------------------------------------*/

  void display(char * out) ;
  /*-----------------------------------------------------------------------
    Output the values stored in the queue.

    Precondition:  ostream out is open.
    Postcondition: Queue's contents, from front to back, have been output 
        to out.
   -----------------------------------------------------------------------*/

    float average() const;
  /*-----------------------------------------------------------------------
    Retrieve the average value of queue (if any).

    Precondition:  Queue is nonempty.
    Postcondition: Value average of queue is returned, unless queue is
        empty; in that case, the average value is equal to zero.
   ----------------------------------------------------------------------*/

 private:
  /***** Data Members *****/
   QueueElement myArray[QUEUE_CAPACITY];
    int count;
    bool full;
}; // end of class declaration

#endif
