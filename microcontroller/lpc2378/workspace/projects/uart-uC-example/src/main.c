/*
 * Simple example to illustrate use of polled UART driver
 * in a multi-tasking uC/OS-II program
 *
 * Acts as an echo server on UART0
 * 
 * DK - 31/01/2015
 */


#include <stdbool.h>
#include <ucos_ii.h>
#include <bsp.h>
#include <osutils.h>
#include <leds.h>
#include <uart_polled.h>

/*
*********************************************************************************************************
*                                            APPLICATION TASK PRIORITIES
*********************************************************************************************************
*/

enum {
  APP_TASK_LINK_PRIO = 4,
  APP_TASK_CONNECT_PRIO,
  APP_TASK_UART_WRITE_PRIO,
  APP_TASK_UART_READ_PRIO    // Priority of READ should be less than WRITE, as here. 
};

/*
*********************************************************************************************************
*                                            APPLICATION TASK STACKS
*********************************************************************************************************
*/

#define  APP_TASK_LINK_STK_SIZE                 256
#define  APP_TASK_CONNECT_STK_SIZE              256
#define  APP_TASK_UART_WRITE_STK_SIZE           256
#define  APP_TASK_UART_READ_STK_SIZE            256


static OS_STK appTaskLinkStk[APP_TASK_LINK_STK_SIZE];
static OS_STK appTaskConnectStk[APP_TASK_CONNECT_STK_SIZE];
static OS_STK appTaskUARTWriteStk[APP_TASK_UART_WRITE_STK_SIZE];
static OS_STK appTaskUARTReadStk[APP_TASK_UART_READ_STK_SIZE];

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES and CONSTANTS
*********************************************************************************************************
*/

enum {
  BUFLEN = 256
};

uint8_t buffer[BUFLEN];
uint32_t count;
OS_EVENT *bufSem;

 

/*
*********************************************************************************************************
*                                            APPLICATION FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void appTaskLink(void *pdata);
static void appTaskConnect(void *pdata);
static void appTaskUARTWrite(void *pdata);
static void appTaskUARTRead(void *pdata);

/*
*********************************************************************************************************
*                                            LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            GLOBAL FUNCTION DEFINITIONS
*********************************************************************************************************
*/

int main() {

  /* Initialise the board support package */
  bspInit();
  uart0Init(9600); /* baud rate setting */
  
  /* Initialise the OS */
  OSInit();                                                   

  /* Create the tasks */
  OSTaskCreate(appTaskLink,                               
               (void *)0,
               (OS_STK *)&appTaskLinkStk[APP_TASK_LINK_STK_SIZE - 1],
               APP_TASK_LINK_PRIO);

  OSTaskCreate(appTaskConnect,                               
               (void *)0,
               (OS_STK *)&appTaskConnectStk[APP_TASK_CONNECT_STK_SIZE - 1],
               APP_TASK_CONNECT_PRIO);

  OSTaskCreate(appTaskUARTWrite,                               
               (void *)0,
               (OS_STK *)&appTaskUARTWriteStk[APP_TASK_UART_WRITE_STK_SIZE - 1],
               APP_TASK_UART_WRITE_PRIO);

  OSTaskCreate(appTaskUARTRead,                               
               (void *)0,
               (OS_STK *)&appTaskUARTReadStk[APP_TASK_UART_READ_STK_SIZE - 1],
               APP_TASK_UART_READ_PRIO);
  
  bufSem = OSSemCreate(0);
  
  /* Start the OS */
  OSStart();                                                  
  
  /* Should never arrive here */ 
  return 0;      
}

/*
*********************************************************************************************************
*                                            APPLICATION TASK DEFINITIONS
*********************************************************************************************************
*/

static void appTaskLink(void *pdata) {
  /* Start the OS ticker 
   * Must be done in the highest priority task
   */
  osStartTick();
  
  /* Task main loop */
  while (true) {
    ledToggle(USB_LINK_LED);
    OSTimeDlyHMSM(0,0,0,500);
  }
}

static void appTaskConnect(void *pdata) {
  
  while (true) {
    OSTimeDlyHMSM(0,0,0,500);
    ledToggle(USB_CONNECT_LED);
  } 
}


static void appTaskUARTWrite(void *pdata) {
  uint8_t osStatus;
  uint32_t i;
  
  while ( true ) {
    OSSemPend(bufSem, 0, &osStatus);
    for (i = 0; i < count; i++) {
      uart0Put(buffer[i]);
    }
  }
}


static void appTaskUARTRead(void *pdata) {
  uint32_t i;
    
  while ( true ) {
    if (uart0ByteAvailable()) {
      i = 0;
      do {
        buffer[i++] = uart0Get();
      } while (uart0ByteAvailable() && (i < BUFLEN));
      count = i;
      (void)OSSemPost(bufSem);
    }      
  }
}

