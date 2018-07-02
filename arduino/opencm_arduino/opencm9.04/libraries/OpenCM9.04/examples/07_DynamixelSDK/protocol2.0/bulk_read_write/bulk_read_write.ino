/*
 * Controller : OpenCM9.04 with 485 EXP board
 * Dynamixel : MX-28/64/106(with Protocol 2.0), X-series(except XL-320)
 * Power source : 12V SMPS to 485 EXP board(or 24V for Dynamixel Pro series)
 * 
 * Dynamixels are connected to Dynamixel BUS on 485 EXP board
 * http://emanual.robotis.com/docs/en/parts/controller/opencm485exp/#layout
*/

#include <DynamixelSDK.h>

// Control table address could be differ by Dynamixel Series
#define ADDRESS_TORQUE_ENABLE           64
#define ADDRESS_LED                     65
#define ADDRESS_GOAL_POSITION           116
#define ADDRESS_PRESENT_POSITION        132

// Data Byte Length
#define LENGTH_LED                      1
#define LENGTH_GOAL_POSITION            4
#define LENGTH_PRESENT_POSITION         4

// Protocol version
#define PROTOCOL_VERSION                2.0                 // See which protocol version is used in the Dynamixel

// Default setting
#define DXL1_ID                         1                   // Dynamixel#1 ID: 1
#define DXL2_ID                         2                   // Dynamixel#2 ID: 2
#define BAUDRATE                        1000000
#define DEVICENAME                      "1"                 // Check which port is being used on your controller
                                                            // DEVICENAME "1" -> Serial1
                                                            // DEVICENAME "2" -> Serial2
                                                            // DEVICENAME "3" -> Serial3(OpenCM 485 EXP)

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
#define DXL_MINIMUM_POSITION_VALUE      100                 // Dynamixel will rotate between this value
#define DXL_MAXIMUM_POSITION_VALUE      4000                // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
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

  // Initialize GroupBulkWrite instance
  dynamixel::GroupBulkWrite groupBulkWrite(portHandler, packetHandler);

  // Initialize GroupBulkRead instance
  dynamixel::GroupBulkRead groupBulkRead(portHandler, packetHandler);

  int index = 0;
  int dxl_comm_result = COMM_TX_FAIL;             // Communication result
  bool dxl_addparam_result = false;                // addParam result
  bool dxl_getdata_result = false;                 // GetParam result
  int dxl_goal_position[2] = {DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE};         // Goal position

  uint8_t dxl_error = 0;                          // Dynamixel error
  uint8_t dxl_led_value[2] = {0x00, 0x01};        // Dynamixel LED value for write
  uint8_t param_goal_position[4];
  int32_t dxl1_present_position = 0;              // Present position
  uint8_t dxl2_led_value_read;                    // Dynamixel LED value for read

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

  // Enable Dynamixel#1 Torque
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL1_ID, ADDRESS_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS)
  {
    packetHandler->getTxRxResult(dxl_comm_result);
  }
  else if (dxl_error != 0)
  {
    packetHandler->getRxPacketError(dxl_error);
  }

  // Enable Dynamixel#2 Torque
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL2_ID, ADDRESS_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS)
  {
    packetHandler->getTxRxResult(dxl_comm_result);
  }
  else if (dxl_error != 0)
  {
    packetHandler->getRxPacketError(dxl_error);
  }

  // Add parameter storage for Dynamixel#1 present position
  dxl_addparam_result = groupBulkRead.addParam(DXL1_ID, ADDRESS_PRESENT_POSITION, LENGTH_PRESENT_POSITION);

  // Add parameter storage for Dynamixel#2 LED value
  dxl_addparam_result = groupBulkRead.addParam(DXL2_ID, ADDRESS_LED, LENGTH_LED);

  while(1)
  {
    Serial.print("Press any key to continue! (or press q to quit!)\n");


    while(Serial.available()==0);

    int ch;

    ch = Serial.read();
    if( ch == 'q' )
      break;

    // Allocate goal position value into byte array
    param_goal_position[0] = DXL_LOBYTE(DXL_LOWORD(dxl_goal_position[index]));
    param_goal_position[1] = DXL_HIBYTE(DXL_LOWORD(dxl_goal_position[index]));
    param_goal_position[2] = DXL_LOBYTE(DXL_HIWORD(dxl_goal_position[index]));
    param_goal_position[3] = DXL_HIBYTE(DXL_HIWORD(dxl_goal_position[index]));

    // Add parameter storage for Dynamixel#1 goal position
    dxl_addparam_result = groupBulkWrite.addParam(DXL1_ID, ADDRESS_GOAL_POSITION, LENGTH_GOAL_POSITION, param_goal_position);

    // Add parameter storage for Dynamixel#2 LED value
    dxl_addparam_result = groupBulkWrite.addParam(DXL2_ID, ADDRESS_LED, LENGTH_LED, &dxl_led_value[index]);

    // Bulkwrite goal position and LED value
    dxl_comm_result = groupBulkWrite.txPacket();
    if (dxl_comm_result != COMM_SUCCESS) packetHandler->getTxRxResult(dxl_comm_result);

    // Clear bulkwrite parameter storage
    groupBulkWrite.clearParam();

    do
    {
      // Bulkread present position and LED status
      dxl_comm_result = groupBulkRead.txRxPacket();
      if (dxl_comm_result != COMM_SUCCESS) packetHandler->getTxRxResult(dxl_comm_result);

      // Check if groupbulkread data of Dynamixel#1 is available
      dxl_getdata_result = groupBulkRead.isAvailable(DXL1_ID, ADDRESS_PRESENT_POSITION, LENGTH_PRESENT_POSITION);

      // Check if groupbulkread data of Dynamixel#2 is available
      dxl_getdata_result = groupBulkRead.isAvailable(DXL2_ID, ADDRESS_LED, LENGTH_LED);

      // Get present position value
      dxl1_present_position = groupBulkRead.getData(DXL1_ID, ADDRESS_PRESENT_POSITION, LENGTH_PRESENT_POSITION);

      // Get LED value
      dxl2_led_value_read = groupBulkRead.getData(DXL2_ID, ADDRESS_LED, LENGTH_LED);

      Serial.print("[ID:");      Serial.print(DXL1_ID);
      Serial.print(" PresPos:");  Serial.print(dxl1_present_position);
      Serial.print(" [ID:");      Serial.print(DXL2_ID);
      Serial.print(" LED Value:");  Serial.print(dxl2_led_value_read);
      Serial.println(" ");
//      printf("[ID:%03d] Present Position : %d \t [ID:%03d] LED Value: %d\n", DXL1_ID, dxl1_present_position, DXL2_ID, dxl2_led_value_read);

    }while(abs(dxl_goal_position[index] - dxl1_present_position) > DXL_MOVING_STATUS_THRESHOLD);

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

  // Disable Dynamixel#1 Torque
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL1_ID, ADDRESS_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
  if (dxl_comm_result != COMM_SUCCESS)
  {
    packetHandler->getTxRxResult(dxl_comm_result);
  }
  else if (dxl_error != 0)
  {
    packetHandler->getRxPacketError(dxl_error);
  }

  // Disable Dynamixel#2 Torque
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL2_ID, ADDRESS_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
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
