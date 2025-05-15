#include "config.h"
#include "stdlib.h"
#include "serial.h"


#define	TaskStkLengh	100			//Define the Task0 stack length 


 
OS_STK	TaskStk0 [TaskStkLengh];		//Define the Task0 stack 
OS_STK	TaskStk1 [TaskStkLengh];		//Define the Task1 stack 

OS_EVENT *mqueue;  // handler for queue

void *mqueue_table [20];  // Queue size

void 	Task0(void *pdata);			//Task0 
void 	Task1(void *pdata);			//Task1 

int main (void)
{
	
	IO0DIR = 0x007f8000;
	TargetInit ();
  init_serial ();
	OSInit ();	
  mqueue = OSQCreate(&mqueue_table[0], 20);																									
	OSTaskCreate (Task0,(void *)0, &TaskStk0[TaskStkLengh - 1], 0);	
	OSTaskCreate (Task1,(void *)0, &TaskStk1[TaskStkLengh - 1], 1);		
	OSStart();
	return 0;															
}
/*********************************************************************************************************
**                            Task0 
********************************************************************************************************/

void Task0	(void *pdata)
{
		uint8 err;
   	char  *rxmsg;
   	while(1)
   	{
		printf("\n\rExecuting Task 0\n\rWaiting for Msg.in Queue");
		rxmsg = (char *)OSQPend(mqueue, 0, &err);                  /* Wait for message from Task #4 */
		printf("\n\rMsg Received. is \"");
		printf(rxmsg);
		printf("\"");
		//OSTimeDly(5);    	
	}
}
/*********************************************************************************************************
**                            Task1 
********************************************************************************************************/

void Task1	(void *pdata)
{
	char   txmsg1[] = "Hello Task0";
	char   txmsg2[] = "Msg. from Task 1";
	char   txmsg3[] = "Another Message";
   	while(1)
   	{
			OSTimeDlyHMSM(0,0,5,0);
			printf("\n\rExecuting Task 1\n\rPosting Msg. to Queue");
			OSQPost(mqueue, (void *)&txmsg1);  /* Send message to Task */
			OSQPost(mqueue, (void *)&txmsg2);  /* Send message to Task */
			OSQPost(mqueue, (void *)&txmsg3);
	}  

}  
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/