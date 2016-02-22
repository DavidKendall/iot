/*
 * Simple program to turn the LPC2378-STK into
 * a node that can be monitored and controlled
 * remotely
 *
 * DK - 31-01-2015
 */


#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ucos_ii.h>
#include <bsp.h>
#include <osutils.h>
#include <leds.h>
#include <accelerometer.h>
#include <uart_polled.h>
#include <xbee.h>

/*
*********************************************************************************************************
*                                            APPLICATION TASK PRIORITIES
*********************************************************************************************************
*/

enum {
  APP_TASK_LEDS_PRIO = 4,
  APP_TASK_UART_WRITE_PRIO,
  APP_TASK_MONITOR_PRIO,
  APP_TASK_COMMAND_RX_PRIO,
};

/*
*********************************************************************************************************
*                                            APPLICATION TASK STACKS
*********************************************************************************************************
*/

#define  APP_TASK_LEDS_STK_SIZE                 256
#define  APP_TASK_COMMAND_RX_STK_SIZE           256
#define  APP_TASK_UART_WRITE_STK_SIZE           256
#define  APP_TASK_MONITOR_STK_SIZE              256


static OS_STK appTaskLedsStk[APP_TASK_LEDS_STK_SIZE];
static OS_STK appTaskCommandRxStk[APP_TASK_COMMAND_RX_STK_SIZE];
static OS_STK appTaskUARTWriteStk[APP_TASK_UART_WRITE_STK_SIZE];
static OS_STK appTaskMonitorStk[APP_TASK_MONITOR_STK_SIZE];

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/
typedef struct {
  uint32_t id;
  uint32_t ax;
  uint32_t ay;
  uint32_t az;
} nodeData_t;

nodeData_t nodeData;
OS_EVENT *dataSem;

bool linkLedFlashing = true;
bool connectLedFlashing = true;
bool sdLedFlashing = true;
bool smoothed = false;

/*
*********************************************************************************************************
*                                            APPLICATION FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void appTaskLeds(void *pdata);
static void appTaskCommandRx(void *pdata);
static void appTaskUARTWrite(void *pdata);
static void appTaskMonitor(void *pdata);

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
  uart2Init(115200); /* baud rate setting */
  
  /* Initialise the OS */
  OSInit();                                                   

  /* Create the tasks */
  OSTaskCreate(appTaskLeds,                               
               (void *)0,
               (OS_STK *)&appTaskLedsStk[APP_TASK_LEDS_STK_SIZE - 1],
               APP_TASK_LEDS_PRIO);

  OSTaskCreate(appTaskCommandRx,                               
               (void *)0,
               (OS_STK *)&appTaskCommandRxStk[APP_TASK_COMMAND_RX_STK_SIZE - 1],
               APP_TASK_COMMAND_RX_PRIO);

  OSTaskCreate(appTaskUARTWrite,                               
               (void *)0,
               (OS_STK *)&appTaskUARTWriteStk[APP_TASK_UART_WRITE_STK_SIZE - 1],
               APP_TASK_UART_WRITE_PRIO);

  OSTaskCreate(appTaskMonitor,                               
               (void *)0,
               (OS_STK *)&appTaskMonitorStk[APP_TASK_MONITOR_STK_SIZE - 1],
               APP_TASK_MONITOR_PRIO);
  
  dataSem = OSSemCreate(0);
  
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

static void appTaskLeds(void *pdata) {
  /* Start the OS ticker 
   * Must be done in the highest priority task
   */
  osStartTick();
  
  /* Task main loop */
  while (true) {
    if (linkLedFlashing) {
      ledToggle(USB_LINK_LED);
    }
    if (connectLedFlashing) {
      ledToggle(USB_CONNECT_LED);
    }
    if (sdLedFlashing) {
      ledToggle(SD_LED);
    }
    OSTimeDlyHMSM(0,0,0,500);
  }
}

static void appTaskUARTWrite(void *pdata) {
  uint8_t osStatus;
  static xbeeBuffer_t packet;
  char payload[256];
  uint32_t packetLen;
  
  while ( true ) {
    OSSemPend(dataSem, 0, &osStatus);
    sprintf(payload, "type:DATA,id:SN01,ax:%d,ay:%d,az:%d", nodeData.ax, nodeData.ay, nodeData.az);
    packetLen = xbeeMkPacketFromString(packet, payload);
    xbeeTxPacket(packet, packetLen);
  }
}

static void appTaskMonitor(void *pdata) {

  while ( true ) {
    nodeData.id = 1;
    if (smoothed) {
      nodeData.ax = accelerometerRead(ACCEL_X);
      nodeData.ay = accelerometerRead(ACCEL_Y);
      nodeData.az = accelerometerRead(ACCEL_Z);
    } else {
      nodeData.ax = accelerometerReadRaw(ACCEL_X);
      nodeData.ay = accelerometerReadRaw(ACCEL_Y);
      nodeData.az = accelerometerReadRaw(ACCEL_Z);
    }
    (void)OSSemPost(dataSem); 
    OSTimeDly(200);    
  }
}

static void appTaskCommandRx(void *pdata) {
  xbeeBuffer_t buffer;
      
  while ( true ) {
    xbeeRxNetstring(buffer);
    switch (buffer[0]) {
      case 'C': {
        connectLedFlashing = !connectLedFlashing;
        break;
      }
      case 'L': {
        linkLedFlashing = !linkLedFlashing;
        break;
      }
      case 'S': {
        sdLedFlashing = !sdLedFlashing;
        break;
      }
      case 'M': {
        smoothed = !smoothed;
        break;
      }
      default: {
        break;
      }
    }
  }      
}


