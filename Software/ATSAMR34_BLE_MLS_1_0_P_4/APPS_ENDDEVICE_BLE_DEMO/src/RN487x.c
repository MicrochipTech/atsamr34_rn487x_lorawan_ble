// m16946
/****************************** INCLUDES **************************************/
/*** INCLUDE FILES ************************************************************/
#include <asf.h>
#include <delay.h>
#include <sio2host.h>
#include "RN487x.h"
#include "ble_usart.h"
#include "conf_ble.h"
#include "utils.h"

/*** SYMBOLIC CONSTANTS ********************************************************/
#define RN487x_DEBUG				0
#define RN487X_RESET_DELAY_TIME		1
#define RN487X_TIMEOUT				0x0FFFFF
#define RN487X_BUFFER_SIZE			100
#define PROMPT_START				"CMD> "
#define PROMPT_END					"END\r\n"
#define REBOOT_MSG					"%REBOOT%"
#define CONNECT_MSG					"%CONNECT"
#define STREAM_OPEN_MSG				"%STREAM_OPEN%"
#define CONNECT_PARAM_MSG			"%CONN_PARAM"
#define DISCONNECT_MSG				"%DISCONNECT%"
#define REBOOTING_RESP				"Rebooting\r\n"
#define AOK_RESP					"AOK\r\n"
#define ERR_RESP					"Err\r\n"
#define STATUS_MESSAGE_DELIMITER	'%'
#define RN487x_DelayMs				delay_ms
#define RN487x_UartTx(c)			usart_write_wait(&ble_usart_instance, c)
#define RN487x_UartRx(c)			usart_read_wait(&ble_usart_instance, c)

/*** LOCAL VARIABLES & TYPE DEFINITIONS ***************************************/
static uint8_t rn487xBuffer[RN487X_BUFFER_SIZE] ;
static bool isConnected = false ;


/*** GLOBAL FUNCTION DEFINITIONS ***********************************************/

/*** RN487x_Init ****************************************************************
 \brief      RN487x Driver Init.
********************************************************************************/
bool RN487x_Init(void)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_Init] ") ;
#endif
	// module reset
	RN487x_ResetSequence() ;
	// expect to receive "%REBOOT%" message
	if (!RN487x_GetExpectedResponse(REBOOT_MSG, strlen(REBOOT_MSG)))
	{
		RN487x_HandleComIssue() ;
	}
	return true;
}

/*** RN487x_HandleComIssue ******************************************************
 \brief      Communication issue handler
********************************************************************************/
void RN487x_HandleComIssue(void)
{
	printf(ANSI_BRIGHT_RED_FG_COLOR "\r\nStay here forever! Something wrong with Host and BLE module interface") ;
	while(1) ;
}

/*** RN487x_IsConnected *********************************************************
 \brief      Return true if device is connected
********************************************************************************/
bool RN487x_IsConnected(void)
{
	return isConnected ;
}

/*** RN487x_SetResetLine ********************************************************
 \brief      Set reset line pin to high/low level
********************************************************************************/
void RN487x_SetResetLine(bool state)
{
	struct port_config pin_conf ;
	port_get_config_defaults(&pin_conf) ;
	pin_conf.direction = PORT_PIN_DIR_OUTPUT ;
	port_pin_set_config(BLE_RESET_LINE, &pin_conf) ;
	port_pin_set_output_level(BLE_RESET_LINE, state) ;
}

/*** RN487x_ResetSequence *******************************************************
 \brief      Reset sequence
********************************************************************************/
void RN487x_ResetSequence(void)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_ResetSequence]") ;
#endif	
	RN487x_SetResetLine(false) ;
	RN487x_DelayMs(RN487X_RESET_DELAY_TIME) ;
	RN487x_SetResetLine(true) ;
}

/*** RN487x_EnterCmdMode ********************************************************
 \brief      Enter into configuration command mode
********************************************************************************/
bool RN487x_EnterCmdMode(void)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_EnterCommandMode] >> $$$") ;
#endif	
	RN487x_UartTx('$') ;
	RN487x_UartTx('$') ;
	RN487x_UartTx('$') ;
	return RN487x_GetExpectedResponse(PROMPT_START, strlen(PROMPT_START)) ;
}

/*** RN487x_EnterDataMode *******************************************************
 \brief      Enter into streaming data mode
********************************************************************************/
bool RN487x_EnterDataMode(void)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_EnterDataMode] >>> ---") ;
#endif
	RN487x_UartTx('-') ;
	RN487x_UartTx('-') ;
	RN487x_UartTx('-') ;
	RN487x_UartTx('\r') ;
	RN487x_UartTx('\n') ;
	return RN487x_GetExpectedResponse(PROMPT_END, strlen(PROMPT_END)) ;
}

/*** RN487x_GetResponse *********************************************************
 \brief      Get response back from the device as is
********************************************************************************/
bool RN487x_GetResponse(uint8_t *resp)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_GetResponse]") ;
#endif
	uint32_t timeout = RN487X_TIMEOUT ;
	while ((timeout > 0) && timeout --)
	{
		if (ble_usart_is_rx_ready(&ble_usart_instance))
		{
			memset(&rn487xBuffer, 0, RN487X_BUFFER_SIZE) ;
			usart_serial_read_packet(&ble_usart_instance, &rn487xBuffer[0], RN487X_BUFFER_SIZE) ;
#if (RN487x_DEBUG == 1)
			printf("\r\n>> Received: ") ;
			sio2host_tx(rn487xBuffer, sizeof(rn487xBuffer)) ;
#endif
			memset(&resp, 0, sizeof(resp)) ;
			memcpy(resp, rn487xBuffer, sizeof(resp)) ;
			return true ;
		}
	}
	return false ;
}

/*** RN487x_GetExpectedResponse *************************************************
 \brief      Get expected response back from the device
********************************************************************************/
bool RN487x_GetExpectedResponse(const uint8_t *expectedResp, uint8_t expectedRespLen)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_GetExpectedResponse]") ;
#endif
	uint32_t timeout = RN487X_TIMEOUT ;
	while ((timeout > 0) && timeout --)
	{
		if (ble_usart_is_rx_ready(&ble_usart_instance))
		{
			memset(&rn487xBuffer, 0, RN487X_BUFFER_SIZE) ;
			usart_serial_read_packet(&ble_usart_instance, &rn487xBuffer[0], RN487X_BUFFER_SIZE) ;
#if (RN487x_DEBUG == 1)
			printf("\r\n>> Expected: %s", expectedResp) ;
			printf("\r\n>> Received: ") ;
			sio2host_tx(rn487xBuffer, strlen(rn487xBuffer)) ;
#endif
			if (strstr(rn487xBuffer, expectedResp) != 0)		// strstr finds the first occurrence in a string
			{
				return true ;
			}
		}
	}
	return false ;
}

/*** RN487x_GetDefaultResponse **************************************************
 \brief      Get default response (AOK or Err) back from the device
********************************************************************************/
bool RN487x_GetDefaultResponse(void)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_DefaultResponse]") ;
#endif
	uint32_t timeout = RN487X_TIMEOUT ;
	while ((timeout > 0) && timeout --)
	{
		if (ble_usart_is_rx_ready(&ble_usart_instance))
		{
			memset(&rn487xBuffer, 0, RN487X_BUFFER_SIZE) ;
			usart_serial_read_packet(&ble_usart_instance, &rn487xBuffer[0], RN487X_BUFFER_SIZE) ;
#if (RN487x_DEBUG == 1)
			printf("\r\n>> Received: ") ;
			sio2host_tx(rn487xBuffer, strlen(rn487xBuffer)) ;
#endif
			// read packets should contains default response (AOK or Err) and the prompt (CMD>_)
			// but the prompt is not checked
			if (strstr(rn487xBuffer, PROMPT_START) != 0)
			{
				// do nothing
			}
			if (strstr(rn487xBuffer, AOK_RESP) != 0)
			{
				return true ;
			}
			else if (strstr(rn487xBuffer, ERR_RESP) != 0)
			{
				return false ;
			}
		}
	}
	return false ;
}

/*** RN487x_GetCmdResponse ******************************************************
 \brief      Get response from a command
********************************************************************************/
bool RN487x_GetCmdResponse(const char *cmd, uint8_t cmdLen, char *cmdResp)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_GetCmdResponse]") ;
#endif
	RN487x_SendCmd((uint8_t *)cmd, cmdLen) ;
	
	uint32_t timeout = RN487X_TIMEOUT ;
	while ((timeout > 0) && timeout --)
	{
		if (ble_usart_is_rx_ready(&ble_usart_instance))
		{
			memset(&rn487xBuffer, 0, RN487X_BUFFER_SIZE) ;
			usart_serial_read_packet(&ble_usart_instance, &rn487xBuffer[0], RN487X_BUFFER_SIZE) ;
			memcpy(cmdResp, rn487xBuffer, sizeof(cmdResp)) ;
#if (RN487x_DEBUG == 1)
			printf("\r\n>> Received :") ;
			sio2host_tx(cmdResp, strlen(cmdResp)) ;
#endif
			return true ;
		}
	}
	return false ;
}

/*** RN487x_FilterMessage *******************************************************
 \brief      Return true if it is an asynchronous status message
********************************************************************************/
bool RN487x_FilterMessage(uint8_t *msg, uint8_t msgLen)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_FilterMessage]") ;
	printf("\r\n>> Async. message received: ") ;
	sio2host_tx(msg, msgLen) ;
#endif	
	
	// check for connection status
	if (strstr(msg, STREAM_OPEN_MSG) != 0)
	{
#if (RN487x_DEBUG == 1)
		printf("\r\n>> Connected") ;
#endif
		isConnected = true ;
	}
	else if (strstr(msg, DISCONNECT_MSG) != 0)
	{
#if (RN487x_DEBUG == 1)
		printf("\r\n>> Disconnected") ;
#endif
		isConnected = false ;
	}
	// filter any status message
	if ((msg[0] == STATUS_MESSAGE_DELIMITER) && (msg[msgLen-1] == STATUS_MESSAGE_DELIMITER))
	{
#if (RN487x_DEBUG == 1)
		printf("\r\n>> Message filtered: %s", msg) ;
#endif
		return true ;
	}

	return false ;
}

/*** RN487x_SendCmd *************************************************************
 \brief      Send command to device
********************************************************************************/
void RN487x_SendCmd(const uint8_t *cmd, uint8_t cmdLen)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_SendCmd] ") ;
	sio2host_tx(cmd, cmdLen) ;
#endif
    uint8_t index = 0 ;
    while (index < cmdLen)
    {
		RN487x_UartTx(cmd[index]) ;
		index++ ;
    }
}

/*** RN487x_SendData ************************************************************
 \brief      Send data to device
********************************************************************************/
void RN487x_SendData(const char *data, uint8_t dataLen)
{
#if (RN487x_DEBUG == 1)
	printf("\r\n[RN487x_SendData] ") ;
	sio2host_tx(data, dataLen) ;
#endif	
    uint8_t index = 0 ;
    while (index < dataLen)
    {
		RN487x_UartTx(data[index]) ;
		index++ ;
    }
}

/*** RN487x_RebootCmd ***********************************************************
 \brief      Reboot command
********************************************************************************/
bool RN487x_RebootCmd(void)
{
	RN487x_SendCmd("R,1\r\n", 5) ;
	return RN487x_GetExpectedResponse(REBOOT_MSG, strlen(REBOOT_MSG)) ;
}

/*** RN487x_Connect *************************************************************
 \brief      Connect to a particular peer device
********************************************************************************/
bool RN487x_Connect(uint8_t addrType, char *addr)
{
	rn487xBuffer[0] = 'C' ;
	rn487xBuffer[1] = ',' ;
	if (addrType)
	{
		// private address
		rn487xBuffer[2] = '1' ;
	}
	else
	{
		// public address
		rn487xBuffer[2] = '0' ;
	}
	rn487xBuffer[3] = ',' ;
	for (uint8_t i = 0; i < 12; i++)
	{
		rn487xBuffer[4 + i] = addr[i] ;
	}
	rn487xBuffer[16] = '\r' ;
	rn487xBuffer[17] = '\n' ;
    RN487x_SendCmd(rn487xBuffer, 18) ;
	return RN487x_GetExpectedResponse(CONNECT_MSG, strlen(CONNECT_MSG)) ;
}


/*** RN487x_Disconnect **********************************************************
 \brief      Disconnect from a peer device
********************************************************************************/
bool RN487x_Disconnect(void)
{
    RN487x_SendCmd("K,1\r\n", 5);
    return RN487x_GetDefaultResponse() ;
}

/*******************************************************************************/

