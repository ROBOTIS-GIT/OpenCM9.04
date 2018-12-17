/*
 * Controller : OpenCM9.04 + OpenCM 485 EXP
 * Dynamixel : XL-320, X-Series, MX-Series with Protocol 2.0
 * Power source
 *   - XL-320 : LB-040/041(x2) on OpenCM9.04C
 *   - X-Series/MX-Series : SMPS 12V 5A on OpenCM 485 EXP
 * 
 * XL-320 is connected to Dynamixel TTL 3PIN of the OpenCM9.04
 * http://emanual.robotis.com/docs/en/parts/controller/opencm904/#layoutpin-map
 
 * X-Series and MX-Series are connected to Dynamixel BUS on 485 EXP board
 * http://emanual.robotis.com/docs/en/parts/controller/opencm485exp/#layout
 * 
 * This example will test only one Dynamixel at a time.
*/

#include <DynamixelSDK.h>

#define ADDRESS_XL320_TORQUE_ENABLE     24
#define ADDRESS_XL320_GOAL_POSITION     30
#define ADDRESS_XL320_PRESENT_POSITION  37

#define ADDRESS_X_TORQUE_ENABLE         64
#define ADDRESS_X_GOAL_POSITION         116
#define ADDRESS_X_PRESENT_POSITION      132

/* Uncomment below definition in order to use the sketch for X series/MX-series with Protocol 2.0 other than XL-320*/
// #define use_XL_320

#ifdef use_XL_320
  // Definition 1 : Control table address for XL-320
  // Use "Definition 1" for XL-320
  #define ADDRESS_TORQUE_ENABLE           ADDRESS_XL320_TORQUE_ENABLE
  #define ADDRESS_GOAL_POSITION           ADDRESS_XL320_GOAL_POSITION
  #define ADDRESS_PRESENT_POSITION        ADDRESS_XL320_PRESENT_POSITION
#else
  // Definition 2 : Control table address for X Series and MX Series with Protocol 2.0
  // Ues "Definition 2" for X Series or MX Series with Protocol 2.0
  #define ADDRESS_TORQUE_ENABLE           ADDRESS_X_TORQUE_ENABLE
  #define ADDRESS_GOAL_POSITION           ADDRESS_X_GOAL_POSITION
  #define ADDRESS_PRESENT_POSITION        ADDRESS_X_PRESENT_POSITION
#endif

// Protocol version
#define PROTOCOL_VERSION                2.0                 // See which protocol version is used in the Dynamixel

// Default setting
#define DXL_ID                          1                   // Dynamixel ID: 1

#ifdef use_XL_320
  #define BAUDRATE                        1000000             // XL-320 default baudrate : 1000000 (Default Baudrate for X series and MX series(with Protocol 2.0) is 57600)
#else
  #define BAUDRATE                        57600               // XL-320 default baudrate : 1000000 (Default Baudrate for X series and MX series(with Protocol 2.0) is 57600)
#endif

#define DEVICENAME                      "3"                 // Check which port is being used on your controller
                                                            // DEVICENAME "1" -> Serial1
                                                            // DEVICENAME "2" -> Serial2
                                                            // DEVICENAME "3" -> Serial3(OpenCM 485 EXP)

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
#define DXL_MINIMUM_POSITION_VALUE      100                 // Dynamixel will rotate between this value
#define DXL_MAXIMUM_POSITION_VALUE      900                 // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MOVING_STATUS_THRESHOLD     20                  // Dynamixel moving status threshold

#define ESC_ASCII_VALUE                 0x1b



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);


  Serial.println("Start..");


  // Initialize PortHandler instance
  // Set the port path
  // Get methods and members of PortHandlerLinux or PortHandlerWindows
  dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);

  // Initialize PacketHandler instance
  // Set the protocol version
  // Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
  dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

  int index = 0;
  int dxl_comm_result = COMM_TX_FAIL;             // Communication result
  int dxl_goal_position[2] = {DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE};         // Goal position

  uint8_t dxl_error = 0;                          // Dynamixel error
  
  #ifdef use_XL_320
    // Variable to save 2 Byte Present Position for XL-320
    int16_t dxl_present_position = 0;
  #else
    // Variable to save 4 Byte Present Position for X-Series and MX-Series with Protocol 2.0
    int32_t dxl_present_position = 0;
  #endif

  // Open port
  if (portHandler->openPort())
  {
    Serial.print("Succeeded to open the port!\n");
  }
  else
  {
    Serial.print("Failed to open the port!\n");
    Serial.print("Press any key to terminate...\n");
    return;
  }

  // Set port baudrate
  if (portHandler->setBaudRate(BAUDRATE))
  {
    Serial.print("Succeeded to change the baudrate!\n");
  }
  else
  {
    Serial.print("Failed to change the baudrate!\n");
    Serial.print("Press any key to terminate...\n");
    return;
  }

  // Enable Dynamixel Torque
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDRESS_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS)
  {
    packetHandler->getTxRxResult(dxl_comm_result);
  }
  else if (dxl_error != 0)
  {
    packetHandler->getRxPacketError(dxl_error);
  }
  else
  {
    Serial.print("Dynamixel has been successfully connected \n");
  }


  while(1)
  {
    Serial.print("Press any key to continue! (or press q to quit!)\n");


    while(Serial.available()==0);

    int ch;

    ch = Serial.read();
    if (ch == 'q')
      break;

    #ifdef use_XL_320
      // Write 2Byte Goal Position data for XL-320
      dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, DXL_ID, ADDRESS_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
    #else
      // Write 4Byte Goal Position data for X-Series and MX-Series with Protocol 2.0
      dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, DXL_ID, ADDRESS_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
    #endif

    if (dxl_comm_result != COMM_SUCCESS)
    {
      packetHandler->getTxRxResult(dxl_comm_result);
    }
    else if (dxl_error != 0)
    {
      packetHandler->getRxPacketError(dxl_error);
    }

    do
    {
      #ifdef use_XL_320
        // Read 2Byte Present Position data for XL-320
        dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, DXL_ID, ADDRESS_PRESENT_POSITION, (uint16_t*)&dxl_present_position, &dxl_error);
      #else
        // Read 4Byte Present Position data for X-Series and MX-Series with Protocol 2.0
        dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, DXL_ID, ADDRESS_PRESENT_POSITION, (uint32_t*)&dxl_present_position, &dxl_error);
      #endif

      if (dxl_comm_result != COMM_SUCCESS)
      {
        packetHandler->getTxRxResult(dxl_comm_result);
      }
      else if (dxl_error != 0)
      {
        packetHandler->getRxPacketError(dxl_error);
      }

      Serial.print("[ID:");      Serial.print(DXL_ID);
      Serial.print(" GoalPos:"); Serial.print(dxl_goal_position[index]);
      Serial.print(" PresPos:");  Serial.print(dxl_present_position);
      Serial.println(" ");


    }while((abs(dxl_goal_position[index] - dxl_present_position) > DXL_MOVING_STATUS_THRESHOLD));

    // Change goal position
    if (index == 0)
    {
      index = 1;
    }
    else
    {
      index = 0;
    }
  }

  // Disable Dynamixel Torque
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDRESS_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS)
  {
    packetHandler->getTxRxResult(dxl_comm_result);
  }
  else if (dxl_error != 0)
  {
    packetHandler->getRxPacketError(dxl_error);
  }

  // Close port
  portHandler->closePort();

}

void loop() {
  // put your main code here, to run repeatedly:

}
