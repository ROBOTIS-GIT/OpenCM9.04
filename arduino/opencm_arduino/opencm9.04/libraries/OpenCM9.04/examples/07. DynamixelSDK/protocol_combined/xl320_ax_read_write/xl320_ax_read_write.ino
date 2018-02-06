/*
 * Controller : OpenCM9.04C with 485 EXP board
 * Dynamixel : AX Series(x1), XL320(x1)
 * Power source : LB-040/041(x2) for OpenCM9.04C, SMPS2Dynamixel
 * 
 * AX is connected to 485 EXP with 12V input   
 * XL320 is connected to OpenCM9.04 with 7.4V input
 * 
 * 12V power source from 485 EXP should NEVER be supplied to XL320(7.4V)   
 * http://emanual.robotis.com/docs/en/parts/controller/opencm485exp/#power-circuit-connection
*/

#include <DynamixelSDK.h>

//Define AX Series Settings
// Control table address for AX
// Each Dynamixel Series have different Control Table Addresses and Data Length(size)
#define ADDR_AX_TORQUE_ENABLE          24
#define ADDR_AX_GOAL_POSITION          30
#define ADDR_AX_PRESENT_POSITION       36
#define AX_PROTOCOL_VERSION            1.0          // AX series use Protocol 1.0

// Factory Setting of AX
#define AX_ID                          1            //Default ID is 1
#define AX_BAUDRATE                    1000000      //Default Baudrate is 1Mbps
#define AX_PORT                        "3"          // Check which port is being used on OpenCM9.04
                                                    // 1: Serial1(XL320), 2: Serial2(UART2), 3: Serial3(485 EXP)
#define DXL_AX_MIN_POSITION            0            // Dynamixel will rotate between this value
#define DXL_AX_MAX_POSITION            1023         // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)

//Define XL320 Settings
// Control table address for XL320
#define ADDR_XL320_TORQUE_ENABLE       24
#define ADDR_XL320_GOAL_POSITION       30
#define ADDR_XL320_PRESENT_POSITION    37
#define XL320_PROTOCOL_VERSION         2.0          //XL320 uses Protocol 2.0

// Factory Setting of XL320
#define XL320_ID                       1            //Default ID is 1
#define XL320_BAUDRATE                 1000000      //Default Baudrate is 1Mbps
#define XL320_PORT                     "1"          // Check which port is being used on OpenCM9.04
                                                    // 1: Serial1(XL320), 2: Serial2(UART2), 3: Serial3(485 EXP)
#define DXL_XL320_MIN_POSITION         0            // Dynamixel will rotate between this value
#define DXL_XL320_MAX_POSITION         1023

//Common Values
#define TORQUE_ENABLE                  1            // Value for enabling the torque
#define TORQUE_DISABLE                 0            // Value for disabling the torque
#define DXL_MOVING_STATUS_THRESHOLD    20           // Dynamixel moving status threshold

#define ESC_ASCII_VALUE                0x1b


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Start..");

  // Initialize PortHandler instance for XL320
  // Set the port path
  // Get methods and members of PortHandlerLinux or PortHandlerWindows
  dynamixel::PortHandler *xl320_portHandler = dynamixel::PortHandler::getPortHandler(XL320_PORT);
  dynamixel::PortHandler *ax_portHandler = dynamixel::PortHandler::getPortHandler(AX_PORT);

  // Initialize PacketHandler instance for AX Series
  // Set the protocol version
  // Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
  dynamixel::PacketHandler *xl320_packetHandler = dynamixel::PacketHandler::getPacketHandler(XL320_PROTOCOL_VERSION);
  dynamixel::PacketHandler *ax_packetHandler = dynamixel::PacketHandler::getPacketHandler(AX_PROTOCOL_VERSION);

  int index = 0;
  int xl320_comm_result = COMM_TX_FAIL;          // XL320 Communication result
  int ax_comm_result = COMM_TX_FAIL;             // AX Communication result
  int xl320_goal_position[2] = {DXL_XL320_MIN_POSITION, DXL_XL320_MAX_POSITION};      // XL320 Goal position
  int ax_goal_position[2] = {DXL_AX_MIN_POSITION, DXL_AX_MAX_POSITION};               // AX Goal position

  uint8_t xl320_error = 0;                       // XL320 error
  uint8_t ax_error = 0;                          // AX error
  int16_t xl320_present_position = 0;            // XL320 Present position(2Byte)
  int16_t ax_present_position = 0;               // AX Present position(2Byte)

  // Open Serial1 port of OpenCM9.04 for XL320
  if (xl320_portHandler->openPort())
  {
    Serial.print("Succeeded to open XL320 port!\n");
  }
  else
  {
    Serial.print("Failed to open XL320 port!\n");
    Serial.print("Press any key to terminate...\n");
    return;
  }

  // Set port baudrate
  if (xl320_portHandler->setBaudRate(XL320_BAUDRATE))
  {
    Serial.print("Succeeded to open the baudrate!\n");
  }
  else
  {
    Serial.print("Failed to open the baudrate!\n");
    Serial.print("Press any key to terminate...\n");
    return;
  }

  // Open Serial3 port of OpenCM9.04 for AX Series
  if (ax_portHandler->openPort())
  {
    Serial.print("Succeeded to open AX Port\n");
  }
  else
  {
    Serial.print("Failed to open AX port!\n");
    Serial.print("Press any key to terminate...\n");
    return;
  }

  if (ax_portHandler->setBaudRate(AX_BAUDRATE))
  {
    Serial.print("Succeeded to open the baudrate!\n");
  }
  else
  {
    Serial.print("Failed to open the baudrate!\n");
    Serial.print("Press any key to terminate...\n");
    return;
  }

  // Enable XL320 Torque
  xl320_comm_result = xl320_packetHandler->write1ByteTxRx(xl320_portHandler, XL320_ID, ADDR_XL320_TORQUE_ENABLE, TORQUE_ENABLE, &xl320_error);
  if (xl320_comm_result != COMM_SUCCESS)
  {
    xl320_packetHandler->getTxRxResult(xl320_comm_result);
    Serial.print("XL320 Torque is ON\n");
  }
  else if (xl320_error != 0)
  {
    xl320_packetHandler->getRxPacketError(xl320_error);
  }
  else
  {  }

  // Enable AX Series Torque
  ax_comm_result = ax_packetHandler->write1ByteTxRx(ax_portHandler, AX_ID, ADDR_AX_TORQUE_ENABLE, TORQUE_ENABLE, &ax_error);
  if (ax_comm_result != COMM_SUCCESS)
  {
    ax_packetHandler->getTxRxResult(ax_comm_result);
    Serial.print("AX Torque is ON\n");
  }
  else if (ax_error != 0)
  {
    ax_packetHandler->getRxPacketError(ax_error);
  }
  else
  {  }

  while (1)
  {
    Serial.print("Press any key to continue! (or press q to quit!)\n");

    while (Serial.available() == 0);

    int ch;

    ch = Serial.read();
    if ( ch == 'q' )
      break;

    // Write goal position to XL320
    xl320_comm_result = xl320_packetHandler->write2ByteTxRx(xl320_portHandler, XL320_ID, ADDR_XL320_GOAL_POSITION, xl320_goal_position[index], &xl320_error);
    if (xl320_comm_result != COMM_SUCCESS)
    {
      xl320_packetHandler->getTxRxResult(xl320_comm_result);
    }
    else if (xl320_error != 0)
    {
      xl320_packetHandler->getRxPacketError(xl320_error);
    }

    // Write goal position to AX Series
    ax_comm_result = ax_packetHandler->write2ByteTxRx(ax_portHandler, AX_ID, ADDR_AX_GOAL_POSITION, ax_goal_position[index], &ax_error);
    if (ax_comm_result != COMM_SUCCESS)
    {
      ax_packetHandler->getTxRxResult(ax_comm_result);
    }
    else if (ax_error != 0)
    {
      ax_packetHandler->getRxPacketError(ax_error);
    }

    do
    {
      // Read present position of XL320
      xl320_comm_result = xl320_packetHandler->read2ByteTxRx(xl320_portHandler, XL320_ID, ADDR_XL320_PRESENT_POSITION, (uint16_t*)&xl320_present_position, &xl320_error);
      if (xl320_comm_result != COMM_SUCCESS)
      {
        xl320_packetHandler->getTxRxResult(xl320_comm_result);
      }
      else if (xl320_error != 0)
      {
        xl320_packetHandler->getRxPacketError(xl320_error);
      }

      // Read present position of AX Series
      ax_comm_result = ax_packetHandler->read2ByteTxRx(ax_portHandler, AX_ID, ADDR_AX_PRESENT_POSITION, (uint16_t*)&ax_present_position, &ax_error);
      if (ax_comm_result != COMM_SUCCESS)
      {
        ax_packetHandler->getTxRxResult(ax_comm_result);
      }
      else if (ax_error != 0)
      {
        ax_packetHandler->getRxPacketError(ax_error);
      }

      Serial.print("[XL320 ID]:");      Serial.print(XL320_ID);
      Serial.print(" GoalPos:"); Serial.print(xl320_goal_position[index]);
      Serial.print(" PresPos:");  Serial.print(xl320_present_position);
      Serial.print("  [AX-Series ID]:");      Serial.print(AX_ID);
      Serial.print(" GoalPos:"); Serial.print(ax_goal_position[index]);
      Serial.print(" PresPos:");  Serial.print(ax_present_position);
      Serial.println(" ");
      Serial.println(" ");


    } while ((abs(xl320_goal_position[index] - xl320_present_position) > DXL_MOVING_STATUS_THRESHOLD) || (abs(ax_goal_position[index] - ax_present_position) > DXL_MOVING_STATUS_THRESHOLD));

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

  // Disable XL320 Torque
  xl320_comm_result = xl320_packetHandler->write1ByteTxRx(xl320_portHandler, XL320_ID, ADDR_XL320_TORQUE_ENABLE, TORQUE_DISABLE, &xl320_error);
  if (xl320_comm_result != COMM_SUCCESS)
  {
    xl320_packetHandler->getTxRxResult(xl320_comm_result);
  }
  else if (xl320_error != 0)
  {
    xl320_packetHandler->getRxPacketError(xl320_error);
  }

  // Disable AX Series Torque
  ax_comm_result = ax_packetHandler->write1ByteTxRx(ax_portHandler, AX_ID, ADDR_AX_TORQUE_ENABLE, TORQUE_DISABLE, &ax_error);
  if (ax_comm_result != COMM_SUCCESS)
  {
    ax_packetHandler->getTxRxResult(ax_comm_result);
  }
  else if (ax_error != 0)
  {
    ax_packetHandler->getRxPacketError(ax_error);
  }

  // Close port
  xl320_portHandler->closePort();
  ax_portHandler->closePort();

}

void loop() {
  // put your main code here, to run repeatedly:

}
