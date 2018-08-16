/******************************************************************************
 * Filename:       PIN_CONTROL.c
 *
 * Description:    This file contains the implementation of the service.
 *
 **********************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include <string.h>

//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#ifdef UARTLOG_ENABLE
#  include "UartLog.h"
#endif

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "pin_control.h"

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

// PIN_CONTROL Service UUID
CONST uint8_t PinControlUUID[ATT_UUID_SIZE] =
{
  PIN_CONTROL_SERV_UUID_BASE128(PIN_CONTROL_SERV_UUID)
};

// LED0 UUID
CONST uint8_t pc_LED0UUID[ATT_UUID_SIZE] =
{
  PC_LED0_UUID_BASE128(PC_LED0_UUID)
};

// LED1 UUID
CONST uint8_t pc_LED1UUID[ATT_UUID_SIZE] =
{
  PC_LED1_UUID_BASE128(PC_LED1_UUID)
};

// LED2 UUID
CONST uint8_t pc_LED2UUID[ATT_UUID_SIZE] =
{
  PC_LED2_UUID_BASE128(PC_LED2_UUID)
};

// Buzzer UUID
CONST uint8_t pc_BuzzerUUID[ATT_UUID_SIZE] =
{
  PC_BUZZER_UUID_BASE128(PC_BUZZER_UUID)
};


/*********************************************************************
 * LOCAL VARIABLES
 */

static PinControlCBs_t *pAppCBs = NULL;
static uint8_t pc_icall_rsp_task_id = INVALID_TASK_ID;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t PinControlDecl = { ATT_UUID_SIZE, PinControlUUID };

// Characteristic "LED0" Properties (for declaration)
static uint8_t pc_LED0Props = GATT_PROP_NOTIFY | GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_WRITE_NO_RSP;

// Characteristic "LED0" Value variable
static uint8_t pc_LED0Val[PC_LED0_LEN] = {0};

// Length of data in characteristic "LED0" Value variable, initialized to minimal size.
static uint16_t pc_LED0ValLen = PC_LED0_LEN_MIN;

// Characteristic "LED0" Client Characteristic Configuration Descriptor
static gattCharCfg_t *pc_LED0Config;



// Characteristic "LED1" Properties (for declaration)
static uint8_t pc_LED1Props = GATT_PROP_NOTIFY | GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_WRITE_NO_RSP;

// Characteristic "LED1" Value variable
static uint8_t pc_LED1Val[PC_LED1_LEN] = {0};

// Length of data in characteristic "LED1" Value variable, initialized to minimal size.
static uint16_t pc_LED1ValLen = PC_LED1_LEN_MIN;

// Characteristic "LED1" Client Characteristic Configuration Descriptor
static gattCharCfg_t *pc_LED1Config;



// Characteristic "LED2" Properties (for declaration)
static uint8_t pc_LED2Props = GATT_PROP_NOTIFY | GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_WRITE_NO_RSP;

// Characteristic "LED2" Value variable
static uint8_t pc_LED2Val[PC_LED2_LEN] = {0};

// Length of data in characteristic "LED2" Value variable, initialized to minimal size.
static uint16_t pc_LED2ValLen = PC_LED2_LEN_MIN;

// Characteristic "LED2" Client Characteristic Configuration Descriptor
static gattCharCfg_t *pc_LED2Config;



// Characteristic "Buzzer" Properties (for declaration)
static uint8_t pc_BuzzerProps = GATT_PROP_NOTIFY | GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_WRITE_NO_RSP;

// Characteristic "Buzzer" Value variable
static uint8_t pc_BuzzerVal[PC_BUZZER_LEN] = {0};

// Length of data in characteristic "Buzzer" Value variable, initialized to minimal size.
static uint16_t pc_BuzzerValLen = PC_BUZZER_LEN_MIN;

// Characteristic "Buzzer" Client Characteristic Configuration Descriptor
static gattCharCfg_t *pc_BuzzerConfig;



/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t PIN_CONTROLAttrTbl[] =
{
  // PIN_CONTROL Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&PinControlDecl
  },
    // LED0 Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &pc_LED0Props
    },
      // LED0 Characteristic Value
      {
        { ATT_UUID_SIZE, pc_LED0UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_WRITE,
        0,
        pc_LED0Val
      },
      // LED0 CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&pc_LED0Config
      },
    // LED1 Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &pc_LED1Props
    },
      // LED1 Characteristic Value
      {
        { ATT_UUID_SIZE, pc_LED1UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_WRITE,
        0,
        pc_LED1Val
      },
      // LED1 CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&pc_LED1Config
      },
    // LED2 Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &pc_LED2Props
    },
      // LED2 Characteristic Value
      {
        { ATT_UUID_SIZE, pc_LED2UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_WRITE,
        0,
        pc_LED2Val
      },
      // LED2 CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&pc_LED2Config
      },
    // Buzzer Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &pc_BuzzerProps
    },
      // Buzzer Characteristic Value
      {
        { ATT_UUID_SIZE, pc_BuzzerUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_WRITE,
        0,
        pc_BuzzerVal
      },
      // Buzzer CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&pc_BuzzerConfig
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t PIN_CONTROL_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );
static bStatus_t PIN_CONTROL_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t PIN_CONTROLCBs =
{
  PIN_CONTROL_ReadAttrCB,  // Read callback function pointer
  PIN_CONTROL_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * PinControl_AddService- Initializes the PinControl service by registering
 *          GATT attributes with the GATT server.
 *
 *    rspTaskId - The ICall Task Id that should receive responses for Indications.
 */
extern bStatus_t PinControl_AddService( uint8_t rspTaskId )
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  pc_LED0Config = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( pc_LED0Config == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, pc_LED0Config );
  // Allocate Client Characteristic Configuration table
  pc_LED1Config = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( pc_LED1Config == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, pc_LED1Config );
  // Allocate Client Characteristic Configuration table
  pc_LED2Config = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( pc_LED2Config == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, pc_LED2Config );
  // Allocate Client Characteristic Configuration table
  pc_BuzzerConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( pc_BuzzerConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, pc_BuzzerConfig );
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( PIN_CONTROLAttrTbl,
                                        GATT_NUM_ATTRS( PIN_CONTROLAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &PIN_CONTROLCBs );
  Log_info1("Registered service, %d attributes", (IArg)GATT_NUM_ATTRS( PIN_CONTROLAttrTbl ));
  pc_icall_rsp_task_id = rspTaskId;

  return ( status );
}

/*
 * PinControl_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t PinControl_RegisterAppCBs( PinControlCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pAppCBs = appCallbacks;
    Log_info1("Registered callbacks to application. Struct %p", (IArg)appCallbacks);
    return ( SUCCESS );
  }
  else
  {
    Log_warning0("Null pointer given for app callbacks.");
    return ( FAILURE );
  }
}

/*
 * PinControl_SetParameter - Set a PinControl parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
bStatus_t PinControl_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  uint8_t  *pAttrVal;
  uint16_t *pValLen;
  uint16_t valMinLen;
  uint16_t valMaxLen;
  uint8_t sendNotiInd = FALSE;
  gattCharCfg_t *attrConfig;
  uint8_t needAuth;

  switch ( param )
  {
    case PC_LED0_ID:
      pAttrVal  =  pc_LED0Val;
      pValLen   = &pc_LED0ValLen;
      valMinLen =  PC_LED0_LEN_MIN;
      valMaxLen =  PC_LED0_LEN;
      sendNotiInd = TRUE;
      attrConfig  = pc_LED0Config;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"LED0", (IArg)len);
      break;

    case PC_LED1_ID:
      pAttrVal  =  pc_LED1Val;
      pValLen   = &pc_LED1ValLen;
      valMinLen =  PC_LED1_LEN_MIN;
      valMaxLen =  PC_LED1_LEN;
      sendNotiInd = TRUE;
      attrConfig  = pc_LED1Config;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"LED1", (IArg)len);
      break;

    case PC_LED2_ID:
      pAttrVal  =  pc_LED2Val;
      pValLen   = &pc_LED2ValLen;
      valMinLen =  PC_LED2_LEN_MIN;
      valMaxLen =  PC_LED2_LEN;
      sendNotiInd = TRUE;
      attrConfig  = pc_LED2Config;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"LED2", (IArg)len);
      break;

    case PC_BUZZER_ID:
      pAttrVal  =  pc_BuzzerVal;
      pValLen   = &pc_BuzzerValLen;
      valMinLen =  PC_BUZZER_LEN_MIN;
      valMaxLen =  PC_BUZZER_LEN;
      sendNotiInd = TRUE;
      attrConfig  = pc_BuzzerConfig;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"Buzzer", (IArg)len);
      break;

    default:
      Log_error1("SetParameter: Parameter #%d not valid.", (IArg)param);
      return INVALIDPARAMETER;
  }

  // Check bounds, update value and send notification or indication if possible.
  if ( len <= valMaxLen && len >= valMinLen )
  {
    memcpy(pAttrVal, value, len);
    *pValLen = len; // Update length for read and get.

    if (sendNotiInd)
    {
      Log_info2("Transmitting noti/ind- connHandle %d, %s", (IArg)attrConfig[0].connHandle,
                                                    (IArg)( (attrConfig[0].value==0)?"Noti/ind disabled":
                                                    (attrConfig[0].value==1)?"Notification enabled":
                                                     "Indication enabled" ) );
      // Try to send notification.
      GATTServApp_ProcessCharCfg( attrConfig, pAttrVal, needAuth,
                                  PIN_CONTROLAttrTbl, GATT_NUM_ATTRS( PIN_CONTROLAttrTbl ),
                                  pc_icall_rsp_task_id,  PIN_CONTROL_ReadAttrCB);
    }
  }
  else
  {
    Log_error3("Length outside bounds: Len: %d MinLen: %d MaxLen: %d.", (IArg)len, (IArg)valMinLen, (IArg)valMaxLen);
    ret = bleInvalidRange;
  }

  return ret;
}


/*
 * PinControl_GetParameter - Get a PinControl parameter.
 *
 *    param - Profile parameter ID
 *    len   - pointer to a variable that contains the maximum length that can be written to *value.
              After the call, this value will contain the actual returned length.
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
bStatus_t PinControl_GetParameter( uint8_t param, uint16_t *len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case PC_LED0_ID:
      *len = MIN(*len, pc_LED0ValLen);
      memcpy(value, pc_LED0Val, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"LED0", (IArg)*len);
      break;

    case PC_LED1_ID:
      *len = MIN(*len, pc_LED1ValLen);
      memcpy(value, pc_LED1Val, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"LED1", (IArg)*len);
      break;

    case PC_LED2_ID:
      *len = MIN(*len, pc_LED2ValLen);
      memcpy(value, pc_LED2Val, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"LED2", (IArg)*len);
      break;

    case PC_BUZZER_ID:
      *len = MIN(*len, pc_BuzzerValLen);
      memcpy(value, pc_BuzzerVal, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"Buzzer", (IArg)*len);
      break;

    default:
      Log_error1("GetParameter: Parameter #%d not valid.", (IArg)param);
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}

/*********************************************************************
 * @internal
 * @fn          PIN_CONTROL_findCharParamId
 *
 * @brief       Find the logical param id of an attribute in the service's attr table.
 *
 *              Works only for Characteristic Value attributes and
 *              Client Characteristic Configuration Descriptor attributes.
 *
 * @param       pAttr - pointer to attribute
 *
 * @return      uint8_t paramID (ref PIN_CONTROL.h) or 0xFF if not found.
 */
static uint8_t PIN_CONTROL_findCharParamId(gattAttribute_t *pAttr)
{
  // Is this a Client Characteristic Configuration Descriptor?
  if (ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID == *(uint16_t *)pAttr->type.uuid)
    return PIN_CONTROL_findCharParamId(pAttr - 1); // Assume the value attribute precedes CCCD and recurse

  // Is this attribute in "LED0"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, pc_LED0UUID, pAttr->type.len))
    return PC_LED0_ID;

  // Is this attribute in "LED1"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, pc_LED1UUID, pAttr->type.len))
    return PC_LED1_ID;

  // Is this attribute in "LED2"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, pc_LED2UUID, pAttr->type.len))
    return PC_LED2_ID;

  // Is this attribute in "Buzzer"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, pc_BuzzerUUID, pAttr->type.len))
    return PC_BUZZER_ID;

  else
    return 0xFF; // Not found. Return invalid.
}

/*********************************************************************
 * @fn          PIN_CONTROL_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t PIN_CONTROL_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;
  uint16_t valueLen;
  uint8_t paramID = 0xFF;

  // Find settings for the characteristic to be read.
  paramID = PIN_CONTROL_findCharParamId( pAttr );
  switch ( paramID )
  {
    case PC_LED0_ID:
      valueLen = pc_LED0ValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"LED0",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for LED0 can be inserted here */
      break;

    case PC_LED1_ID:
      valueLen = pc_LED1ValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"LED1",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for LED1 can be inserted here */
      break;

    case PC_LED2_ID:
      valueLen = pc_LED2ValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"LED2",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for LED2 can be inserted here */
      break;

    case PC_BUZZER_ID:
      valueLen = pc_BuzzerValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"Buzzer",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for Buzzer can be inserted here */
      break;

    default:
      Log_error0("Attribute was not found.");
      return ATT_ERR_ATTR_NOT_FOUND;
  }
  // Check bounds and return the value
  if ( offset > valueLen )  // Prevent malicious ATT ReadBlob offsets.
  {
    Log_error0("An invalid offset was requested.");
    status = ATT_ERR_INVALID_OFFSET;
  }
  else
  {
    *pLen = MIN(maxLen, valueLen - offset);  // Transmit as much as possible
    memcpy(pValue, pAttr->pValue + offset, *pLen);
  }

  return status;
}

/*********************************************************************
 * @fn      PIN_CONTROL_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t PIN_CONTROL_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t len, uint16_t offset,
                                        uint8_t method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;
  uint8_t   changeParamID = 0xFF;
  uint16_t writeLenMin;
  uint16_t writeLenMax;
  uint16_t *pValueLenVar;

  // See if request is regarding a Client Characterisic Configuration
  if (ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID == *(uint16_t *)pAttr->type.uuid)
  {
    // Allow notification and indication, but do not check if really allowed per CCCD.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY |
                                                     GATT_CLIENT_CFG_INDICATE );
    if (SUCCESS == status && pAppCBs && pAppCBs->pfnCfgChangeCb)
       pAppCBs->pfnCfgChangeCb( connHandle, PIN_CONTROL_SERV_UUID,
                                PIN_CONTROL_findCharParamId(pAttr), pValue, len );

     return status;
  }

  // Find settings for the characteristic to be written.
  paramID = PIN_CONTROL_findCharParamId( pAttr );
  switch ( paramID )
  {
    case PC_LED0_ID:
      writeLenMin  = PC_LED0_LEN_MIN;
      writeLenMax  = PC_LED0_LEN;
      pValueLenVar = &pc_LED0ValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"LED0",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for LED0 can be inserted here */
      break;

    case PC_LED1_ID:
      writeLenMin  = PC_LED1_LEN_MIN;
      writeLenMax  = PC_LED1_LEN;
      pValueLenVar = &pc_LED1ValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"LED1",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for LED1 can be inserted here */
      break;

    case PC_LED2_ID:
      writeLenMin  = PC_LED2_LEN_MIN;
      writeLenMax  = PC_LED2_LEN;
      pValueLenVar = &pc_LED2ValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"LED2",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for LED2 can be inserted here */
      break;

    case PC_BUZZER_ID:
      writeLenMin  = PC_BUZZER_LEN_MIN;
      writeLenMax  = PC_BUZZER_LEN;
      pValueLenVar = &pc_BuzzerValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"Buzzer",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for Buzzer can be inserted here */
      break;

    default:
      Log_error0("Attribute was not found.");
      return ATT_ERR_ATTR_NOT_FOUND;
  }
  // Check whether the length is within bounds.
  if ( offset >= writeLenMax )
  {
    Log_error0("An invalid offset was requested.");
    status = ATT_ERR_INVALID_OFFSET;
  }
  else if ( offset + len > writeLenMax )
  {
    Log_error0("Invalid value length was received.");
    status = ATT_ERR_INVALID_VALUE_SIZE;
  }
  else if ( offset + len < writeLenMin && ( method == ATT_EXECUTE_WRITE_REQ || method == ATT_WRITE_REQ ) )
  {
    // Refuse writes that are lower than minimum.
    // Note: Cannot determine if a Reliable Write (to several chars) is finished, so those will
    //       only be refused if this attribute is the last in the queue (method is execute).
    //       Otherwise, reliable writes are accepted and parsed piecemeal.
    Log_error0("Invalid value length was received.");
    status = ATT_ERR_INVALID_VALUE_SIZE;
  }
  else
  {
    // Copy pValue into the variable we point to from the attribute table.
    memcpy(pAttr->pValue + offset, pValue, len);

    // Only notify application and update length if enough data is written.
    //
    // Note: If reliable writes are used (meaning several attributes are written to using ATT PrepareWrite),
    //       the application will get a callback for every write with an offset + len larger than _LEN_MIN.
    // Note: For Long Writes (ATT Prepare + Execute towards only one attribute) only one callback will be issued,
    //       because the write fragments are concatenated before being sent here.
    if ( offset + len >= writeLenMin )
    {
      changeParamID = paramID;
      *pValueLenVar = offset + len; // Update data length.
    }
  }

  // Let the application know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (changeParamID != 0xFF)
    if ( pAppCBs && pAppCBs->pfnChangeCb )
      pAppCBs->pfnChangeCb( connHandle, PIN_CONTROL_SERV_UUID, paramID, pValue, len+offset ); // Call app function from stack task context.

  return status;
}
