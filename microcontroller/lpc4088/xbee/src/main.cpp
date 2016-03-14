/*
 * Simple program to turn the LPC4088QSB into
 * a node that can be monitored and controlled
 * remotely
 *
 * DK - 12-03-2016
 */

#include <mbed.h>
#include <MMA7455.h>
#include <LM75B.h>
#include <display.h>
#include <ucos_ii.h>
#include <string.h>
#include "xb.h"

/*
*********************************************************************************************************
*                                 NODE IDENTIFIER and DESCRIPTION
*********************************************************************************************************
*/
char const * const NODE_ID = "SN00";
char const * const NODE_DESCRIPTION = "Revolutionary Headquarters";

/*
*********************************************************************************************************
*                                            APPLICATION TASK PRIORITIES
*********************************************************************************************************
*/
enum {
  APP_TASK_COMMAND_PRIO = 4,
  APP_TASK_LEDS_PRIO,
  APP_TASK_UART_WRITE_PRIO,
  APP_TASK_MONITOR_PRIO,
	APP_TASK_UART_READ_PRIO
};

/*
*********************************************************************************************************
*                                            APPLICATION TASK STACKS
*********************************************************************************************************
*/

#define  APP_TASK_COMMAND_STK_SIZE              128
#define  APP_TASK_LEDS_STK_SIZE                 256
#define  APP_TASK_UART_WRITE_STK_SIZE           384
#define  APP_TASK_MONITOR_STK_SIZE              512
#define  APP_TASK_UART_READ_STK_SIZE            128


static OS_STK appTaskCommandStk[APP_TASK_COMMAND_STK_SIZE];
static OS_STK appTaskLedsStk[APP_TASK_LEDS_STK_SIZE];
static OS_STK appTaskUARTWriteStk[APP_TASK_UART_WRITE_STK_SIZE];
static OS_STK appTaskMonitorStk[APP_TASK_MONITOR_STK_SIZE];
static OS_STK appTaskUARTReadStk[APP_TASK_UART_READ_STK_SIZE];

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/
typedef struct {
  int32_t ax;
  int32_t ay;
  int32_t az;
	int32_t pt;
	int32_t js;
	float   tm;
} nodeData_t;

static nodeData_t nodeData;
static OS_EVENT *dataSem;
static xbeeBuffer_t packetBuf;
static OS_EVENT *packetSem;

static bool led1Flashing = true;
static bool led2Flashing = true;
static bool led3Flashing = true;
static bool led4Flashing = true;
static char testMessage[61];
static Display *d = Display::theDisplay();
static Xb xbee(P4_22, P4_23);


/*
*********************************************************************************************************
*                                            APPLICATION FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void appTaskCommand(void *pdata);
static void appTaskLeds(void *pdata);
static void appTaskUARTWrite(void *pdata);
static void appTaskMonitor(void *pdata);
static void appTaskUARTRead(void *pdata);

/*
*********************************************************************************************************
*                                            LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void payloadData(char *payload, nodeData_t nodeData);
void payloadPublish(char *payload, char const *description);

/*
*********************************************************************************************************
*                                            GLOBAL FUNCTION DEFINITIONS
*********************************************************************************************************
*/

int main() {

	/* Initialise the display */	
	d->fillScreen(WHITE);
	d->setTextColor(BLACK, WHITE);
	
	/* ...and the network */
	xbee.xbeeNodeDiscovery();

  /* Initialise the OS */
  OSInit();                                                   

  /* Create the tasks */
  OSTaskCreate(appTaskCommand,                               
               (void *)0,
               (OS_STK *)&appTaskCommandStk[APP_TASK_COMMAND_STK_SIZE - 1],
               APP_TASK_COMMAND_PRIO);

  OSTaskCreate(appTaskLeds,                               
               (void *)0,
               (OS_STK *)&appTaskLedsStk[APP_TASK_LEDS_STK_SIZE - 1],
               APP_TASK_LEDS_PRIO);

  OSTaskCreate(appTaskUARTWrite,                               
               (void *)0,
               (OS_STK *)&appTaskUARTWriteStk[APP_TASK_UART_WRITE_STK_SIZE - 1],
               APP_TASK_UART_WRITE_PRIO);

  OSTaskCreate(appTaskMonitor,                               
               (void *)0,
               (OS_STK *)&appTaskMonitorStk[APP_TASK_MONITOR_STK_SIZE - 1],
               APP_TASK_MONITOR_PRIO);
  
  OSTaskCreate(appTaskUARTRead,                               
               (void *)0,
               (OS_STK *)&appTaskUARTReadStk[APP_TASK_UART_READ_STK_SIZE - 1],
               APP_TASK_UART_READ_PRIO);

  dataSem = OSSemCreate(0);
  packetSem = OSSemCreate(0);
  
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
static void appTaskCommand(void *pdata) {
      
  uint8_t osStatus;
	char command[72];
	uint32_t commandLen;
	uint32_t i;
	uint32_t j;

  /* Start the OS ticker -- must be done in the highest priority task */
  SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
  
  while ( true ) {
      OSSemPend(packetSem, 0, &osStatus);
		  commandLen = xbee.xbeeNetstring(packetBuf, command);
      switch (command[0]) {
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
          break;
			  }
        case '6': {
          memset(testMessage, ' ', 60);
					testMessage[60] = '\0';
					for (i = 1, j = 0; j < commandLen - 1; i += 1, j += 1) {
						testMessage[j] = command[i];
					}
				  d->setCursor(2,22);
				  d->printf("%s", testMessage);
          break;
        }
				case 'R': {
					d->fillScreen(RED);
					break;
				}
				case 'G': {
					d->fillScreen(GREEN);
					break;
				}
				case 'B': {
					d->fillScreen(BLUE);
					break;
				}
				case 'W': {
					d->fillScreen(WHITE);
					break;
				}
        default: {
          break;
        }
			}
  }
}      


static void appTaskLeds(void *pdata) {
  DigitalOut led1(LED1);
  DigitalOut led2(LED2);
  DigitalOut led3(LED3);
  DigitalOut led4(LED4);

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
  
	payloadPublish(payload, NODE_DESCRIPTION);
  packetLen = xbee.xbeeMkPacketFromString(packet, payload);
  xbee.xbeeTxPacket(packet, packetLen);
	
  while ( true ) {
    OSSemPend(dataSem, 0, &osStatus);
		payloadData(payload, nodeData);
    packetLen = xbee.xbeeMkPacketFromString(packet, payload);
    xbee.xbeeTxPacket(packet, packetLen);
  }
}

static void appTaskMonitor(void *pdata) {

	MMA7455 acc(P0_27, P0_28);
	AnalogIn pot(p15);
  LM75B lm75B(P0_27, P0_28, LM75B::ADDRESS_1);


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
  lm75B.open();
	
  while ( true ) {
		acc.read(nodeData.ax, nodeData.ay, nodeData.az);
		nodeData.pt = (int32_t)((1.0F - pot.read()) * 100);
    nodeData.js = 0;
		nodeData.tm = lm75B.temp();
    (void)OSSemPost(dataSem); 
    OSTimeDly(200);    
  }
}


static void appTaskUARTRead(void *pdata) {
	/* MUST be the lowest priority task */
  while ( true ) {
		xbee.xbeeReceivePacket(packetBuf);
    (void)OSSemPost(packetSem); 
	}
}

void payloadData(char *payload, nodeData_t nodeData) {
	sprintf(payload, "type:DATA,id:%s,ax:%d,ay:%d,az:%d,pt:%d,js:%d,tm:%.1f",
          NODE_ID,	
		      nodeData.ax, nodeData.ay, nodeData.az, 
	        nodeData.pt, nodeData.js,nodeData.tm
	);
}

void payloadPublish(char *payload, char const *description) {
	sprintf(payload, "type:PUBLISH,id:%s,ds:%s",
	        NODE_ID, description
	);
}
