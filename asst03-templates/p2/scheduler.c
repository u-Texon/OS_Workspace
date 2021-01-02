#include "scheduler.h"
#include <stdlib.h>
#include <assert.h>

typedef struct _QueueItem
{
    /*
     * The data of this item.
     */
    int data;
    /*
     * The next item in the queue.
     * NULL if there is no next item.
     */
    struct _QueueItem *next;
} QueueItem;

typedef struct _Queue
{
    /*
     * The first item of the queue.
     * NULL if the queue is empty.
     */
    QueueItem *head;
    /*
     * The last item of the queue.
     * undefined if the queue is empty.
     */
    QueueItem *tail;
} Queue;

typedef enum _ProcessState
{
    STATE_UNUSED = 0, // This entry in the _processes array is unused.

    STATE_READY,   // The process is ready and should be on a ready queue for
                   //   selection by the scheduler
    STATE_RUNNING, // The process is running and should not be on a ready queue
    STATE_WAITING  // The process is blocked and should not be on a ready queue
} ProcessState;

typedef struct _Process
{
    int processId;
    ProcessState state;
    /*
     * Range: 0 ... HIGHEST_PRIORITY (including)
     * HIGHEST_PRIORITY is highest priority
     */
    int priority;
} Process;

Process _processes[MAX_PROCESSES] = {{0}};

Queue readyQ;
Queue *readyP = &readyQ;

Queue runningQ;
Queue *runningP = &runningQ;

Queue waitingQ;
Queue *waitingP = &waitingQ;

int sameProcessCounter = 0;
int lastProcess = -1;

/*
 * Adds a new, waiting process.
 */
int startProcess(int processId, int priority)
{
    if (((processId < 0) || (processId >= MAX_PROCESSES) ||
         (_processes[processId].state != STATE_UNUSED)) ||
        (priority < 0) || (priority > HIGHEST_PRIORITY))
    {

        return -1;
    }

    _processes[processId].processId = processId;
    _processes[processId].state = STATE_WAITING;
    _processes[processId].priority = priority;
    return 0;
}

/*
 * Append to the tail of the queue.
 * Does nothing on error.
 */
void _enqueue(Queue *queue, int data)
{
    QueueItem newItem;
    newItem.data = data;
    void *nextPtr = NULL;
    newItem.next = nextPtr;
    QueueItem *p = malloc(sizeof(newItem));
    *p = newItem;
    if (queue->head == NULL)
    {
        *queue->tail = *p;
        *queue->head = *p;
    }
    else
    {
        *queue->tail->next = *p;
        *queue->tail = *p;
    }
}

/*
 * Remove and get the head of the queue.
 * Return -1 if the queue is empty.
 */
int _dequeue(Queue *queue)
{
    if (queue->head != NULL)
    {
        QueueItem *p = queue->head;
        int returnValue = queue->head->data;
        *queue->head = *queue->head->next;
        free(p);
        return returnValue;
    }
    return -1;
}

void initScheduler()
{
    readyP->head = NULL;
    runningP->head = NULL;
    waitingP->head = NULL;
}

/*
* removes a process with the given id from a specific queue.
*/
void removeFromQueue(Queue *queue, int iD)
{
    int headID = _dequeue(queue);
    while (headID != iD && headID != -1)
    {
        _enqueue(queue, headID);
    }
}

/*
 * Called whenever a waiting process gets ready to run.
 */
void onProcessReady(int processId)
{
    if (_processes[processId].state == STATE_WAITING)
    {
        _processes[processId].state = STATE_READY;
        // removeFromQueue(waitingP, processId);
        // _enqueue(readyP, processId);
    }
    else if (_processes[processId].state == STATE_UNUSED)
    {
        _processes[processId].state = STATE_READY;
        // _enqueue(readyP, processId);
    }
    else
    {
        //error
    }
}

/*
 * Called whenever a running process is forced of the CPU
 * (e.g., through a timer interrupt).
 */
void onProcessPreempted(int processId)
{
    if (_processes[processId].state == STATE_RUNNING)
    {
        // removeFromQueue(runningP, processId);
        // _enqueue(readyP, processId);
        _processes[processId].state = STATE_READY;
    }
    else
    {
        //error
    }
}

/*
 * Called whenever a running process is blocked (e.g., after requesting an I/O
 * operation) and needs to wait.
 */
void onProcessBlocked(int processId)
{
    if (_processes[processId].state == STATE_RUNNING)
    {
        //  removeFromQueue(runningP, processId);
        //  _enqueue(waitingP, processId);
        _processes[processId].state = STATE_WAITING;
    }
    else
    {
        //error
    }
}

void findNext(int *bestPrio, int *nextID)
{
    for (size_t i = 0; i < MAX_PROCESSES; i++)
    {
        if (_processes[i].state == STATE_READY && _processes[i].priority > *bestPrio)
        {
            *bestPrio = _processes[i].priority;
            *nextID = _processes[i].processId;
        }
    }
}
/*
 * Gets the id of the next process to run and sets its state to running.
 */
int scheduleNextProcess()
{
    int bestPrio = -1;
    int nextID = -1;
    int *bestP = &bestPrio;
    int *nextP = &nextID;

    if (sameProcessCounter == 4 && _processes[lastProcess].state == STATE_READY)
    {
        _processes[lastProcess].state = STATE_WAITING;
        findNext(bestP, nextP);
        _processes[lastProcess].state = STATE_READY;
    }
    else
    {
        findNext(bestP, nextP);
    }

    // removeFromQueue(readyP, returnValue);
    _processes[nextID].state = STATE_RUNNING;
    // _enqueue(runningP, returnValue);

    if (nextID == lastProcess)
    {
        sameProcessCounter++;
    }
    else
    {
        sameProcessCounter = 0;
    }
    lastProcess = nextID;

    return nextID;
} //sorry fürs codegolfen, ich verstehe nicht warum meine Queues nicht funktionieren :(A
