/*
 * Simple program to turn the LPC4088QSB into
 * a node that can be monitored and controlled
 * remotely
 *
 * DK - 31-01-2015
 */

#include <mbed.h>
#include <MMA7455.h>
#include <display.h>
#include <ucos_ii.h>
#include "xb.h"

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
  int32_t id;
  int32_t ax;
  int32_t ay;
  int32_t az;
} nodeData_t;

nodeData_t nodeData;
OS_EVENT *dataSem;

bool led1Flashing = true;
bool led2Flashing = true;
bool led3Flashing = true;
bool led4Flashing = true;
bool smoothed = false;
char* testMessage[2] = {"Hello world",
	                      "           " 
                       };
int32_t testMessageNum = 0;
Display *d = Display::theDisplay();
Xb xbee(P4_22, P4_23);


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

	/* Initialise the display */	
	d->fillScreen(WHITE);
	d->setTextColor(BLACK, WHITE);

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
	
  DigitalOut led1(LED1);
  DigitalOut led2(LED2);
  DigitalOut led3(LED3);
  DigitalOut led4(LED4);

  /* Start the OS ticker -- must be done in the highest priority task */
  SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
  
  /* Task main loop */
  while (true) {
    if (led1Flashing) {
      led1 = !led1;
    }
    if (led2Flashing) {
      led2 = !led2;
    }
    if (led3Flashing) {
      led3 = !led3;
    }
    if (led4Flashing) {
      led4 = !led4;
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
    sprintf(payload, "type:DATA,id:SN02,ax:%d,ay:%d,az:%d", nodeData.ax, nodeData.ay, nodeData.az);
    packetLen = xbee.xbeeMkPacketFromString(packet, payload);
    xbee.xbeeTxPacket(packet, packetLen);
  }
}

static void appTaskMonitor(void *pdata) {

	MMA7455 acc(P0_27, P0_28);

	d->setCursor(2,2);
	if (!acc.setMode(MMA7455::ModeMeasurement)) {
    d->printf("Unable to set mode for MMA7455!\n");
  }
	else {
	  if (!acc.calibrate()) {
      d->printf("Failed to calibrate MMA7455!\n");
    }
		else {
	    d->printf("MMA7455 initialised\n");
		}
  }

  while ( true ) {
    nodeData.id = 1;
		acc.read(nodeData.ax, nodeData.ay, nodeData.az);
    (void)OSSemPost(dataSem); 
    OSTimeDly(200);    
  }
}

static void appTaskCommandRx(void *pdata) {
  xbeeBuffer_t buffer;
      
  while ( true ) {
    xbee.xbeeRxNetstring(buffer);
    switch (buffer[0]) {
      case '1': {
        led1Flashing = !led1Flashing;
        break;
      }
      case '2': {
        led2Flashing = !led2Flashing;
        break;
      }
      case '3': {
        led3Flashing = !led3Flashing;
        break;
      }
      case '4': {
        led4Flashing = !led4Flashing;
        break;
			}
      case '5': {
        smoothed = !smoothed;
        break;
			}
      case '6': {
        testMessageNum = 1 - testMessageNum;
				d->setCursor(2,22);
				d->printf("%s", testMessage[testMessageNum]);
        break;
      }
      default: {
        break;
      }
    }
  }      
}


