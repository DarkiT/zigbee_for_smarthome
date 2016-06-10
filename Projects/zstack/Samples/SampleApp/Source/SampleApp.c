/*********************************************************************
  This application isn't intended to do anything useful, it is
  intended to be a simple example of an application's structure.

  This application sends it's messages either as broadcast or
  broadcast filtered group messages.  The other (more normal)
  message addressing is unicast.  Most of the other sample
  applications are written to support the unicast message model.

  Key control:
    SW1:  Sends a flash command to all devices in Group 1.
    SW2:  Adds/Removes (toggles) this device in and out
          of Group 1.  This will enable and disable the
          reception of the flash command.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <stdlib.h>
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"


#include  "MT_UART.h" 
#include  "MT.h"

/*for sensor*/
#include "DHT11.h"
#include "ds18b20.h"
#include "relay.h"
#include "input.h"
#include "bh1750.h"
#include "adc.h"
#include "pwm.h"

#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)    ) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID,
  SAMPLEAPP_FLASH_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t SampleApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

/*contact method*/
afAddrType_t SampleApp_Periodic_DstAddr;
afAddrType_t SampleApp_Flash_DstAddr;
afAddrType_t Point_To_Point_DstAddr;

aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;

uint8 RxData[20]={0};
uint8 RxDataLen=0;
uint8 deviceid;
uint8 data_to_send[50];

SER SerData[12];
static uint8 ControlMode = 0;
/*********************************************************************
 * LOCAL FUNCTIONS
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_SendPeriodicMessage( void );
void SampleApp_SendFlashMessage( uint16 flashTime );
void SampleApp_SerialCMD(mtOSALSerialData_t *cmdMsg);
void Sample_SendPointToPointMessage(void);
void PrintSnesorData(uint8 sensortype);
void SendSensorData(uint8 deviceid,uint16 deviceShortAddress,uint8 sensorid,uint16 data1,uint16 data2);
void DataEncode(uint8 deviceid,uint16 deviceShortAddress,SER*p,char *DataOut);

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SampleApp_Init( uint8 task_id )
{
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  
  MT_UartInit();
  MT_UartRegisterTaskID(task_id);
  HalUARTWrite(0,"Hello World\n",12); 
  AdcInit();

  //select the device to init 
  // #define BALCONY 1
//   #define KITCHEN 2
//#define BEDROOM 3
    #define OUTDOOR 4
//   #define DOOR    5

  #ifdef BALCONY      
  deviceid=DEVICE1ID;                 
  #define SENSORTYPE1 DHT11
  #define SENSORTYPE9 BH1750_TYPE
  #define SENSORTYPE10 DirHumity_TYPE
  #define SENSORTYPE11 LED_TYPE
  #define DHT11_ON 1
  #define BH1750_TYPE_ON  9
  #define DirHumity_TYPE_ON 10
  #define LED_TYPE_ON 11
  #endif

  #ifdef KITCHEN 
  deviceid=DEVICE2ID; 
  #define SENSORTYPE1 DHT11
  #define SENSORTYPE5  GAS_TYPE_ON
  #define SENSORTYPE7  FIRE_TYPE
  #define DHT11_ON 1
  #define GAS_TYPE_ON 5
  #define FIRE_TYPE_ON 7
  #endif

  #ifdef BEDROOM 
  deviceid=DEVICE3ID; 
  #define SENSORTYPE1 DHT11
  #define SENSORTYPE2 LIGHTED
  #define SENSORTYPE11 LED_TYPE
  #define DHT11_ON 1
  #define LIGHTED_ON 2
  #define LED_TYPE_ON 11
  MotorInit();
  StopMotor();
  #endif

  #ifdef OUTDOOR
  deviceid=DEVICE4ID; 
  #define SENSORTYPE1 DHT11
  #define SENSORTYPE2 LIGHTED
  #define SENSORTYPE6 RAIN_TYPE
  #define DHT11_ON 1
  #define LIGHTED_ON 2
  #define RAIN_TYPE_ON 6
  #endif

  #ifdef DOOR 
  deviceid=DEVICE5ID; 
  #define SENSORTYPE1 DHT11
  #define SENSORTYPE8 MAN_TYPE
  #define DHT11_ON 1
  #define MAN_TYPE_ON 8
  #endif

  #ifdef DHT11_ON
  P0SEL &=0xef;//dht11 init  
  #endif

  #ifdef LIGHTED_ON
  AdcInit();
  #endif

  #ifdef DS18B20_ON
  P0SEL &=0xdf;//ds18b20 init  
  #endif

  #ifdef GAS_TYPE_ON
  AdcInit();
  #endif

  #ifdef RAIN_TYPE_ON
  InitFire(); 
  #endif

  #ifdef FIRE_TYPE_ON
  InitFire();
  #endif

  #ifdef MAN_TYPE_ON
  InitMan();
  #endif  

  #ifdef BH1750_TYPE_ON
  Bh1750_Init();
  #endif    
  
  #ifdef DirHumity_TYPE_ON
  AdcInit();
  #endif   

  #ifdef LED_TYPE_ON
  Pwm_Init();
  GpioCarrConfig(255);
  #endif

  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

 #if defined ( BUILD_ALL_DEVICES )
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in SampleAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif

  // Setup for the periodic message's destination address
  // Broadcast to everyone
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;

  // Setup for the flash command's destination address - Group 1
  SampleApp_Flash_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  SampleApp_Flash_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Flash_DstAddr.addr.shortAddr = SAMPLEAPP_FLASH_GROUP;

  //Setup for the Point to Point message's destination address
  Point_To_Point_DstAddr.addrMode=(afAddrMode_t)Addr16Bit;
  Point_To_Point_DstAddr.endPoint=SAMPLEAPP_ENDPOINT;
  Point_To_Point_DstAddr.addr.shortAddr=0x0000;

  // Fill out the endpoint description.
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &SampleApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );

  // By default, all devices start out in Group 1
  SampleApp_Group.ID = 0x0001;
  osal_memcpy( SampleApp_Group.name, "Group 1", 7  );
  aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
#endif
}

/*********************************************************************
 * @fn      SampleApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case CMD_SERIAL_MSG:  
         SampleApp_SerialCMD((mtOSALSerialData_t *)MSGpkt);
         break;

        // Received when a key is pressed
          case KEY_CHANGE:
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          SampleApp_MessageMSGCB( MSGpkt );
          break;

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( //(SampleApp_NwkState == DEV_ZB_COORD)   
               (SampleApp_NwkState == DEV_ROUTER)
              || (SampleApp_NwkState == DEV_END_DEVICE) )
          {
            // Start sending the periodic message in a regular interval.
            osal_start_timerEx( SampleApp_TaskID,
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
          }
          else
          {
            // Device is no longer in the network
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in SampleApp_Init()).
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )
  {
    #ifdef SENSORTYPE1
    PrintSnesorData(SENSORTYPE1); 
    #endif

    #ifdef SENSORTYPE2
    PrintSnesorData(SENSORTYPE2); 
    #endif

    #ifdef SENSORTYPE5
    PrintSnesorData(SENSORTYPE5); 
    #endif

    #ifdef SENSORTYPE6
    PrintSnesorData(SENSORTYPE6); 
    #endif

    #ifdef SENSORTYPE7
    PrintSnesorData(SENSORTYPE7); 
    #endif

    #ifdef SENSORTYPE8
    PrintSnesorData(SENSORTYPE8); 
    #endif

    #ifdef SENSORTYPE9
    PrintSnesorData(SENSORTYPE9); 
    #endif

    #ifdef SENSORTYPE10
    PrintSnesorData(SENSORTYPE10); 
    #endif    
    // Send the periodic message
    //SampleApp_SendPeriodicMessage();

    // Send the PointToPoin message
    Sample_SendPointToPointMessage();

    // Setup to send message again in normal period (+ a little jitter)
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
        (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );

    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_1 )
  {
    /* This key sends the Flash Command is sent to Group 1.
     * This device will not receive the Flash Command from this
     * device (even if it belongs to group 1).
     */
    SampleApp_SendFlashMessage( SAMPLEAPP_FLASH_DURATION );
  }

  if ( keys & HAL_KEY_SW_2 )
  {
    /* The Flashr Command is sent to Group 1.
     * This key toggles this device in and out of group 1.
     * If this device doesn't belong to group 1, this application
     * will not receive the Flash command sent to group 1.
     */
    aps_Group_t *grp;
    grp = aps_FindGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    if ( grp )
    {
      // Remove from the group
      aps_RemoveGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    }
    else
    {
      // Add to the flash group
      aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
    }
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
static void LightAdjust(uint8 motion) //add 、decrese
{
  static uint8 v = 240;

  if(motion == OPEN)
  {
    v += 50;
    GpioCarrConfig(v);
  }
  else
  {
    GpioCarrConfig(v);
  }
}

static void WaterAdjust(uint8 motion)
{
  if(motion==OPEN)
  {
    P1_0 = 0;
  } 
  else 
  {
    P1_0 = 1;
  }
}

static void WindowTurn(uint8 motion) // move left 、move right 、stop
{
  if(motion == 1)
  {
    TurnMotorLeft();
  } 
  else if(motion == 2)
  {
    TurnMotorRight();
  }
  else
  {
    StopMotor();
  }
}

static void FanAdjust(uint8 motion)
{
  if(motion==OPEN)
  { 
    P1_0 = 0;
  }
  else 
  {
    P1_0 = 1;
  } 
}
//recv command from coordinator
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  uint16 value;
  char DataGetFromEndEdv[20];
  int i = 0;

  switch ( pkt->clusterId )
  {
    static uint8 GetDeviceId,GetSensorId,GetMotion;
     case SAMPLEAPP_COM_CLUSTERID:            // 07 EF FE 01 02 00 00 EF
          GetDeviceId = pkt->cmd.Data[4];
          GetSensorId = pkt->cmd.Data[5];
          GetMotion = pkt->cmd.Data[6];
          ControlMode = pkt->cmd.Data[7];
          if(ControlMode == 1)
          {
            if(GetDeviceId == deviceid)                                          
            {
                switch(GetSensorId)
                { 
                  case LightSwitch:
                  LightAdjust(GetMotion);
                  break;

                  case WaterSwitch:
                  WaterAdjust(GetMotion);
                  break;

                  case WindowSwitch:
                  WindowTurn(GetMotion);
                  break;

                  case FanSwitch:
                  FanAdjust(GetMotion);
                  break;
                }
            }
          }
          break;
 
      case SAMPLEAPP_POINT_TO_POINT_CLUSTERID:                                  
          memset(DataGetFromEndEdv,0,20);

          for(i = 0;i <20 ;i++)
          {
            DataGetFromEndEdv[i] = pkt->cmd.Data[i];
          }
        
          value=pkt->srcAddr.addr.shortAddr;
          DataGetFromEndEdv[17] = BYTE1(value);
          DataGetFromEndEdv[18] = BYTE0(value);

          HalUARTWrite(0,DataGetFromEndEdv, 20);

      break;
      
    /*case SAMPLEAPP_PERIODIC_CLUSTERID:
      
      break;

    case SAMPLEAPP_FLASH_CLUSTERID:
      flashTime = BUILD_UINT16(pkt->cmd.Data[1], pkt->cmd.Data[2] );
      HalLedBlink( HAL_LED_4, 4, 50, (flashTime / 4) );
      break;*/
  }
}

/*********************************************************************
 * @fn      SampleApp_SendPeriodicMessage
 *
 * @brief   Send the periodic message.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_SendPeriodicMessage( void )
{
  if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       1,
                       (uint8*)&SampleAppPeriodicCounter,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}

/*********************************************************************
 * @fn      SampleApp_SendFlashMessage
 *
 * @brief   Send the flash message to group 1.
 *
 * @param   flashTime - in milliseconds
 *
 * @return  none
 */
void SampleApp_SendFlashMessage( uint16 flashTime )
{
  uint8 buffer[3];
  buffer[0] = (uint8)(SampleAppFlashCounter++);
  buffer[1] = LO_UINT16( flashTime );
  buffer[2] = HI_UINT16( flashTime );

  if ( AF_DataRequest( &SampleApp_Flash_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_FLASH_CLUSTERID,
                       3,
                       buffer,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}

/*********************************************************************
*********************************************************************/
void SampleApp_SerialCMD(mtOSALSerialData_t *cmdMsg)
{
  uint8 len,*str=NULL;     
  str=cmdMsg->msg;         
  len=*str;                

  if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,    
	                    SAMPLEAPP_COM_CLUSTERID,//×Ô¼º¶¨ÒåÒ»¸ö
	                    len+1,                  // Êý¾Ý³¤¶È         
                            str,                     //Êý¾ÝÄÚÈÝ
	                    &SampleApp_TransID, 
	                    AF_DISCV_ROUTE,
	                    AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
	                     {
	                     }
  else
  {
  // Error occurred in request to send.
  } 
}

void Sample_SendPointToPointMessage(void)                                       
{
  char SendDataToCoor[20];
  SER temp[4];
  int i = 0;
  int index = 0;

  memset(SendDataToCoor,0,20);

  for(i = 0; i < 12 ; i++)
  {
    if(SerData[i].id > 0)
    {
      temp[index].id = SerData[i].id;
      temp[index].data1 = SerData[i].data1;
      temp[index].data2 = SerData[i].data2;
      index++;
    }
  }
  

  DataEncode(deviceid,0,temp,SendDataToCoor);

  // if(SampleApp_NwkState==DEV_ROUTER)
  //   device=0x01;
  // else if(SampleApp_NwkState==DEV_END_DEVICE)
  //   device=0x02;
  // else 
  //   device=0x03;
  // SendData[5]=device;

  if ( AF_DataRequest( &Point_To_Point_DstAddr,
                       &SampleApp_epDesc,
                       SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                       20,
                       SendDataToCoor,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}

void LightAutoControl(int v)
{
  if(deviceid == DEVICE1ID)
  {
    if(v < 100)
    {
      GpioCarrConfig(200);
    }
    if(v > 300)
    {
      GpioCarrConfig(240);
    }
  }

  if(deviceid == DEVICE3ID)
  {
    if(v < 70)
    {
      GpioCarrConfig(200);
    }
    if(v > 90)
    {
      GpioCarrConfig(240);
    }
  }
}

void WaterAutoControl(int v)
{
  if(deviceid == DEVICE1ID)
  {
    if( v > 90)
    {
       P1_0 = 0;
    }
    else
    {
       P1_0 = 1;
    }
  }
}

void FanAutoControl(int v)
{
  if(deviceid == DEVICE3ID)
  {
    if( v >=27)
    {
      P1_0 = 0;
    }
    else
    {
      P1_0 = 1;
    }
  }
}

void PrintSnesorData(uint8 sensortype)           
{
  int v = 0;
  switch(sensortype)
  {
    case DHT11:
    DHT11Read();
    SerData[DHT11].id = DHT11;
    SerData[DHT11].data1 = wendu_shi*10 + wendu_ge;
    SerData[DHT11].data2 = shidu_shi*10 + shidu_ge;
    if(!ControlMode)
      FanAutoControl(SerData[DHT11].data1);
    break;

    case LIGHTED:
    SerData[LIGHTED].id = LIGHTED;
    SerData[LIGHTED].data1 = ReadAd();
    SerData[LIGHTED].data2 = 0;
    if(!ControlMode)
      LightAutoControl((100 - ReadAd()));
    break;

    case DS18B20:
    Temp_test();
    SerData[DS18B20].id = DS18B20;
    SerData[DS18B20].data1 = temp2;
    break;

    case GAS_TYPE:
    SerData[GAS_TYPE].id = GAS_TYPE;
    SerData[GAS_TYPE].data1 = ReadAd();
    SerData[GAS_TYPE].data2 = 0;
    break;

    case RAIN_TYPE:
    if(FIRE == 0)
    {
      SerData[RAIN_TYPE].data1 = 0;
    }
    else
    {
      SerData[RAIN_TYPE].data1 = 1;
    }
    SerData[RAIN_TYPE].id = RAIN_TYPE;
    SerData[RAIN_TYPE].data2 = 0;
    break;

    case FIRE_TYPE:
    if(FIRE == 0)
    {
      SerData[FIRE_TYPE].data1  = 0;
    }
    else
    {
      SerData[FIRE_TYPE].data1  = 1;
    }
    SerData[FIRE_TYPE].id = FIRE_TYPE;
    break;

    case MAN_TYPE:
    if(MAN == 0)
    {
      SerData[MAN_TYPE].data1 = 0;
    }
    else
    {
      SerData[MAN_TYPE].data1 = 1;
    }
    SerData[MAN_TYPE].id = MAN_TYPE;
    SerData[MAN_TYPE].data2 = 0;
    break;    

    case BH1750_TYPE:
    v = Single_Read_BH1750();
    SerData[BH1750_TYPE].id = BH1750_TYPE;
    SerData[BH1750_TYPE].data1 = BYTE1(v);
    SerData[BH1750_TYPE].data2 = BYTE0(v);
    if(!ControlMode)
      LightAutoControl(v);
    break;

    case DirHumity_TYPE:
    SerData[DirHumity_TYPE].id = DirHumity_TYPE;
    SerData[DirHumity_TYPE].data1 = ReadAd();
    SerData[DirHumity_TYPE].data2 = 0;
    if(!ControlMode)
      WaterAutoControl(ReadAd());
    break;
  }
}


void DataEncode(uint8 deviceid,uint16 deviceShortAddress,SER*p,char *DataOut)
{
  uint8 sum=0,i=0,_cnt=0;
  
  DataOut[_cnt++]=0xAA;
  DataOut[_cnt++]=0xAA;
  DataOut[_cnt++]=0x03;
  DataOut[_cnt++]=0;
  DataOut[_cnt++]=deviceid;

  for(i =0 ;i <4 ;i++)
  {
    DataOut[_cnt++]=p[i].id;
    DataOut[_cnt++]=p[i].data1;
    DataOut[_cnt++]=p[i].data2;
  }  

  DataOut[_cnt++]=BYTE1(deviceShortAddress);     
  DataOut[_cnt++]=BYTE0(deviceShortAddress);

  DataOut[3] = _cnt-4;
  
  sum = 0;
  for(i=0;i<_cnt;i++)
    sum += DataOut[i];
  
  DataOut[_cnt++]=sum;
}

