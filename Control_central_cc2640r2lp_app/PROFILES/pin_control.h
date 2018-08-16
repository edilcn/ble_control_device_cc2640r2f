/******************************************************************************
 * Filename:       PIN_CONTROL.h
 *
 * Description:    This file contains the PIN_CONTROL service definitions and
 *                 prototypes.
 *
 ******************************************************************************/

#ifndef _PIN_CONTROL_H_
#define _PIN_CONTROL_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <bcomdef.h>

/*********************************************************************
 * CONSTANTS
 */
// Service UUID
#define PIN_CONTROL_SERV_UUID 0xFFF0
#define PIN_CONTROL_SERV_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0

// LED0 Characteristic defines
#define PC_LED0_ID                 0
#define PC_LED0_UUID               0xFFF1
#define PC_LED0_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0
#define PC_LED0_LEN                1
#define PC_LED0_LEN_MIN            1

// LED1 Characteristic defines
#define PC_LED1_ID                 1
#define PC_LED1_UUID               0xFFF2
#define PC_LED1_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0
#define PC_LED1_LEN                1
#define PC_LED1_LEN_MIN            1

// LED2 Characteristic defines
#define PC_LED2_ID                 2
#define PC_LED2_UUID               0xFFF3
#define PC_LED2_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0
#define PC_LED2_LEN                1
#define PC_LED2_LEN_MIN            1

// Buzzer Characteristic defines
#define PC_BUZZER_ID                 3
#define PC_BUZZER_UUID               0xFFF4
#define PC_BUZZER_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0
#define PC_BUZZER_LEN                1
#define PC_BUZZER_LEN_MIN            1

/*********************************************************************
 * TYPEDEFS
 */

// Fields in characteristic "LED0"
//   Field "State" format: uint8, bits: 8

// Fields in characteristic "LED1"
//   Field "STATE" format: uint8, bits: 8

// Fields in characteristic "LED2"
//   Field "State" format: uint8, bits: 8

// Fields in characteristic "Buzzer"
//   Field "State" format: uint8, bits: 8

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*PinControlChange_t)( uint16_t connHandle, uint16_t svcUuid, uint8_t paramID, uint8_t *pValue, uint16_t len );

typedef struct
{
  PinControlChange_t        pfnChangeCb;     // Called when characteristic value changes
  PinControlChange_t        pfnCfgChangeCb;  // Called when characteristic CCCD changes
} PinControlCBs_t;



/*********************************************************************
 * API FUNCTIONS
 */


/*
 * PinControl_AddService- Initializes the PinControl service by registering
 *          GATT attributes with the GATT server.
 *
 *    rspTaskId - The ICall Task Id that should receive responses for Indications.
 */
extern bStatus_t PinControl_AddService( uint8_t rspTaskId );

/*
 * PinControl_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t PinControl_RegisterAppCBs( PinControlCBs_t *appCallbacks );

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
extern bStatus_t PinControl_SetParameter( uint8_t param, uint16_t len, void *value );

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
extern bStatus_t PinControl_GetParameter( uint8_t param, uint16_t *len, void *value );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _PIN_CONTROL_H_ */

