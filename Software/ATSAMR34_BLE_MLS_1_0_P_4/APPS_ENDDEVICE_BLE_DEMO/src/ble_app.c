// m16946
/****************************** INCLUDES **************************************/
/*** INCLUDE FILES ************************************************************/
#include <asf.h>
#include <sio2host.h>
#include "ble_app.h"
#include "ble_usart.h"
#include "conf_ble.h"
#include "RN487x.h"
#include "utils.h"
#include "lorawan_app.h"

/*** SYMBOLIC CONSTANTS ********************************************************/
#define BLE_APP_BUFFER_SIZE		100
#define PROVISIONING_LEN		79
#define PROVISIONING_DELIMITER '|'

/*** LOCAL VARIABLES & TYPE DEFINITIONS ***************************************/
static uint8_t ble_appBuffer[BLE_APP_BUFFER_SIZE] ;
static int rxChar ;

/*** LOCAL FUNCTION PROTOTYPES *************************************************/
static bool ble_provisioning(uint8_t *buffer, uint8_t len) ;
static void serial_provisioning(uint8_t *buffer, uint8_t len) ;

/*** GLOBAL FUNCTION DEFINITIONS ***********************************************/

/*** ble_app_init ***************************************************************
 \brief      BLE Application Init.
********************************************************************************/
void ble_app_init(void)
{
	printf(ANSI_RESET_COLOR "\r\nTry to initialize BLE device ...\r\n") ;
	ble_usart_init() ;
	RN487x_Init() ;
}

/*** ble_app_print_menu *********************************************************
 \brief      BLE Application menu printing
********************************************************************************/
void ble_app_print_menu(void)
{
	printf("\r\n") ;
	printf("===============================================\n\r") ;
	printf("Menu: select a BLE application from the list\r\n") ;
	printf("===============================================\n\r") ;
	printf("1) Serial bridge\r\n") ;
	printf("2) Beacon mode - https://www.microchip.com URL\r\n") ;
	printf("3) Transparent chat mode\r\n") ;
	printf("4) Scan mode\r\n") ;
	printf("5) Search mode\r\n") ;
	printf("6) Search and connect\r\n") ;
	printf("7) Sleep mode\r\n") ;
	printf("8) Wake-up\r\n") ;
	printf("9) LoRaWAN provisioning\r\n") ;
	printf("-----------------------------------------------\r\n") ;
	printf(">> ") ;
}

/*** ble_app_select_application *************************************************
 \brief      BLE Application selection
********************************************************************************/
void ble_app_select_application(void)
{
	bool exit = false ;
	ble_app_print_menu() ;	
	while (!exit)
	{
		if ((-1) != (rxChar = sio2host_getchar_nowait()))
		{
			switch (rxChar)
			{
				case '1':
					ble_app_serial_bridge() ;
				break ;
				case '2':
					ble_app_beacon() ;
				break ;
				case '3':
					ble_app_transparent() ;
				break ;
				case '4':
					ble_app_scan() ;
				break ;
				case '5':
					ble_app_search(false) ;
				break ;
				case '6':
					ble_app_search(true) ;
				break ;
				case '7':
					ble_app_power_down() ;
				break ;
				case '8':
					ble_app_power_up() ;
				break ;
				case '9':
					ble_app_lorawan_provisioning() ;
				break ;
				default:
					ble_app_print_menu() ;
				break ;
			}
		}		
	}
}

/*** ble_app_serial_bridge ******************************************************
 \brief      BLE Serial Bridge Application
********************************************************************************/
void ble_app_serial_bridge(void)
{
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	
	printf("\r\nStarting BLE Serial Bridge Application...\r\n") ;
	printf("Check out RN487x User's Guide documentation on https://www.microchip.com/wwwproducts/en/RN4870 \r\n") ;
	printf("Type '$$$' to enter into command mode \r\n") ;
	
	// stay in serial bridge app forever
	while(1)
	{
		// verify if there is any character received from the user
		if ((-1) != (rxChar = sio2host_getchar_nowait()))
		{
			usart_write_wait(&ble_usart_instance, (uint16_t)rxChar) ;
			//sio2host_putchar(rxChar) ;//echo for testing
		}
	
		// verify if there is any character received from the module
		if (ble_usart_is_rx_ready(&ble_usart_instance))
		{
			memset(&ble_appBuffer, 0, sizeof(ble_appBuffer)) ;
			usart_serial_read_packet(&ble_usart_instance, &ble_appBuffer[0], sizeof(ble_appBuffer)) ;
			sio2host_tx(&ble_appBuffer[0], sizeof(ble_appBuffer)) ;
		}		
	}
}

/*** ble_app_beacon *************************************************************
 \brief      BLE Eddystone URL Beacon Application
********************************************************************************/
void ble_app_beacon(void)
{
	printf("\r\nStarting BLE Eddystone URL Beacon Application...\r\n") ;
	
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	
	// stop advertising
	RN487x_SendCmd("Y\r\n", 3) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// set the advertising output power to max. value
	RN487x_SendCmd("SGA,0\r\n", 7) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// clear immediate beacon
	RN487x_SendCmd("IB,Z\r\n", 6) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// start immediate beacon
	// check out RN487x User's Guide documentation on https://www.microchip.com/wwwproducts/en/RN4870
	// check out Eddystone frame format on https://github.com/google/eddystone/tree/master/eddystone-url
	RN487x_SendCmd("IB,03,AAFE\r\n", 12) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// add the rest of the frame with URI
	const char* url = "microchip.com" ;		// https://www.microchip.com	// up to 17 bytes
	char out_url[30] = {0} ;
	char tempCmd[] = "IB,16,AAFE10C501" ;	// Eddystone ID (0xFEAA), URL Frame type (0x10), Tx power ref (0xC5), URL prefix (0x01 for https://www.)
	charArrayToHexArray(&url[0], &out_url[0]) ;
	strcat(tempCmd, out_url) ;
	strcat(tempCmd, "\r\n") ;
	RN487x_SendCmd(tempCmd, strlen(tempCmd)) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// back to default data mode
	if (!RN487x_EnterDataMode()) RN487x_HandleComIssue() ;	
	// User interaction message
	printf("\r\n- Open Microchip Bluetooth Data Smartphone App") ;
	printf("\r\n- Select Beacon Ranging") ;
	printf("\r\n- Observe the URL advertised by your beacon\r\n") ;
}

/*** ble_app_transparent ********************************************************
 \brief      BLE Transparent UART Application
********************************************************************************/
void ble_app_transparent(void)
{
	printf("\r\nStarting BLE Transparent UART Application...\r\n") ;
	
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	
	// set serialized name
	RN487x_SendCmd("S-,SAMR34\r\n", 11) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// enable device information profile (0x80) and transparent service (0x40), bitmap = 0x80 | 0x40 = 0xC0
	RN487x_SendCmd("SS,C0\r\n", 7) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// reboot device
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	// User interaction message
	printf("\r\n- Open Microchip SmartData App or BLE Terminal App") ;
	printf("\r\n- Initiate a BLE connection from the App with SAMR34_xxxx peripheral device") ;
	printf("\r\n- Make sure to communicate over the Transparent UART Service (49535343-1E4D-4BD9-BA61-23C647249616)") ;
	printf("\r\n- Over the chat, exchange text message from one device to the other\r\n") ;
	// stay in this app forever
	while(1)
	{
		// verify if there is any character received from the user
		if ((-1) != (rxChar = sio2host_getchar_nowait()))
		{
			if (RN487x_IsConnected())
			{
				usart_write_wait(&ble_usart_instance, (uint16_t)rxChar) ;
			}
		}
		
		// verify if there is any character received from the module
		if (ble_usart_is_rx_ready(&ble_usart_instance))
		{
			memset(&ble_appBuffer, 0, sizeof(ble_appBuffer)) ;
			usart_serial_read_packet(&ble_usart_instance, &ble_appBuffer[0], sizeof(ble_appBuffer)) ;
			// filter asynchronous status message like %CONNECT, ...
			if (!RN487x_FilterMessage(&ble_appBuffer[0], strlen(ble_appBuffer)))
			{
				printf(ANSI_BRIGHT_BLUE_BG_COLOR ">> %s\r\n" ANSI_RESET_COLOR, ble_appBuffer) ;
			}
		}
	}
}

/*** ble_app_scan ***************************************************************
 \brief      BLE Scan Application
********************************************************************************/
void ble_app_scan(void)
{
	printf("\r\nStarting BLE Scan Application...\r\n") ;
	
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	
	// remove GATT service
	RN487x_SendCmd("SS,00\r\n", 7) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// set passive scan (0x0200) and does not filter out duplicate scan results (0x0400), bitmap = 0x0200 | 0x0400 = 0x0600
	RN487x_SendCmd("SR,0600\r\n", 9) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// reboot device
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	// enter into command mode
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	// stop advertising
	RN487x_SendCmd("Y\r\n", 3) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;

	// User interaction message
	printf("\r\n- Initiate a scan with F command") ;
	printf("\r\n- Stop the active scan with X command") ;
	printf("\r\n>> ") ;

	while(1)
	{
		// verify if there is any character received from the user
		if ((-1) != (rxChar = sio2host_getchar_nowait()))
		{
			usart_write_wait(&ble_usart_instance, (uint16_t)rxChar) ;
		}
	
		// verify if there is any character received from the module
		if (ble_usart_is_rx_ready(&ble_usart_instance))
		{
			memset(&ble_appBuffer, 0, sizeof(ble_appBuffer)) ;
			usart_serial_read_packet(&ble_usart_instance, &ble_appBuffer[0], sizeof(ble_appBuffer)) ;
			sio2host_tx(&ble_appBuffer[0], sizeof(ble_appBuffer)) ;
		}
	}
}

/*** ble_app_search *************************************************************
 \brief      BLE Search Application
********************************************************************************/
void ble_app_search(bool connect)
{
	printf("\r\nStarting BLE Search Application...\r\n") ;
	
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	
	// remove GATT service
	RN487x_SendCmd("SS,00\r\n", 7) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// set passive scan (0x0200) and does not filter out duplicate scan results (0x0400), bitmap = 0x0200 | 0x0400 = 0x0600
	RN487x_SendCmd("SR,0600\r\n", 9) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// reboot device
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	// enter into command mode
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	// stop advertising
	RN487x_SendCmd("Y\r\n", 3) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;

	// User interaction message
	printf("\r\n- Enter the MAC address of the Bluetooth device you want to search for (BD_ADDR format: 11:22:33:44:55:66)") ;
	printf("\r\n>> ") ;
	uint8_t bdAddr[12] ;//"641CB015C633";
	uint8_t bdAddrExtracted[12] ;	
	uint8_t rxCnt = 0 ;
	while (rxCnt < 12)
	{
		rxChar = sio2host_getchar() ;
		if (isHexFormat(rxChar))
		{
			bdAddr[rxCnt] = convertHexToUpperCase(rxChar) ;
			rxCnt++ ;
		}
	}
	printf("\r\nStart searching for ") ;
	sio2host_tx(&bdAddr[0], sizeof(bdAddr)) ;
	printf("\r\n") ;
	// start scanning
	RN487x_SendCmd("F\r\n", 3) ;
	while(1)
	{
		// verify if there is any character received from the module
		if (ble_usart_is_rx_ready(&ble_usart_instance))
		{
			memset(&ble_appBuffer, 0, sizeof(ble_appBuffer)) ;
			usart_serial_read_packet(&ble_usart_instance, &ble_appBuffer[0], sizeof(ble_appBuffer)) ;
			sio2host_tx(&ble_appBuffer[0], sizeof(ble_appBuffer)) ;

			memset(&bdAddrExtracted, 0, sizeof(bdAddrExtracted)) ;
			memcpy(bdAddrExtracted, &ble_appBuffer[1], sizeof(bdAddrExtracted)) ;
			if (strncmp(bdAddrExtracted, bdAddr, 12) == 0)
			{
				printf(ANSI_BRIGHT_GREEN_FG_COLOR "\r\nDevice %s found !" ANSI_RESET_COLOR, bdAddr) ;
				printf("\r\nStop scanning.") ;				
				// stop scanning
				RN487x_SendCmd("X\r\n", 3) ;
				if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
				if (!connect)
				{
					printf("\r\nEnd of Search Application.") ;
					// back to default data mode
					if (!RN487x_EnterDataMode()) RN487x_HandleComIssue() ;
					break ;					
				}
				else
				{
					printf("\r\nTrying to connect to the peer device ...") ;
					if (!RN487x_Connect(1, &bdAddr))
					{
						printf("\r\nUnable to connect. End of Search Application.") ;
						// back to default data mode
						if (!RN487x_EnterDataMode()) RN487x_HandleComIssue() ;
					}
					else
					{
						printf("Connected to peer device.") ;
					}
					ble_app_print_menu() ;
					break ;
				}
			}
		}
	}
}

/*** ble_app_power_down *********************************************************
 \brief      BLE Power down mode
********************************************************************************/
void ble_app_power_down(void)
{
	printf("\r\nPlacing BLE device immediately into dormant power-saving mode") ;

	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	
	RN487x_SendCmd("O,0\r\n", 5) ;
	printf("\r\n") ;
	ble_app_print_menu() ;
}

/*** ble_app_power_up ***********************************************************
 \brief      BLE Power up mode
********************************************************************************/
void ble_app_power_up(void)
{
	printf("\r\nWaking up BLE device from dormant power-saving mode") ;
	RN487x_Init() ;
	printf("\r\n") ;
	ble_app_print_menu() ;
}


/*** ble_app_lorawan_provisioning ***********************************************
 \brief      LoRaWAN Provisioning over BLE Application
********************************************************************************/
void ble_app_lorawan_provisioning(void)
{
	printf("\r\nStarting LoRaWAN provisioning over BLE Transparent UART Application...\r\n") ;
	
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	if (!RN487x_EnterCmdMode()) RN487x_HandleComIssue() ;
	
	// set serialized name
	RN487x_SendCmd("S-,SAMR34\r\n", 11) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// enable device information profile (0x80) and transparent service (0x40), bitmap = 0x80 | 0x40 = 0xC0
	RN487x_SendCmd("SS,C0\r\n", 7) ;
	if (!RN487x_GetDefaultResponse()) RN487x_HandleComIssue() ;
	// reboot device
	if (!RN487x_RebootCmd()) RN487x_HandleComIssue() ;
	// User interaction message
	printf("\r\n- Open Microchip SmartData App or BLE Terminal App") ;
	printf("\r\n- Initiate a BLE connection from the App with SAMR34_xxxx peripheral device") ;
	printf("\r\n- Make sure to communicate over the Transparent UART Service (49535343-1E4D-4BD9-BA61-23C647249616)") ;
	printf("\r\n- Provision your LoRaWAN device from the App using the following command format :") ;
	printf("\r\n       mac set otaa <8-Bytes deveui>|<8-Bytes appeui>|<16-Bytes appkey>") ;
	printf("\r\n  e.g. mac set otaa 000425191801D545|70B3D57EF00044A3|35FBBE0864909A2FB12D6477BC3002F2\r\n") ;
	// stay in this app forever
	while(1)
	{
		// verify if there is any character received from the module
		if (ble_usart_is_rx_ready(&ble_usart_instance))
		{
			memset(&ble_appBuffer, 0, sizeof(ble_appBuffer)) ;
			usart_serial_read_packet(&ble_usart_instance, &ble_appBuffer[0], sizeof(ble_appBuffer)) ;
						
			// filter asynchronous status message like %CONNECT, ...
			if (!RN487x_FilterMessage(&ble_appBuffer[0], strlen(ble_appBuffer)))
			{
				sio2host_tx(&ble_appBuffer[0], sizeof(ble_appBuffer)) ;
				if (strstr(ble_appBuffer, "mac set otaa") == 0)
				{
					printf(ANSI_BRIGHT_RED_FG_COLOR "\r\nWrong command format. Provisioning aborted" ANSI_RESET_COLOR) ;
					ble_app_debug("\r\nWrong command format. Provisioning aborted") ;
					break ;
				}
				if (strlen(ble_appBuffer) != PROVISIONING_LEN)
				{
					printf(ANSI_BRIGHT_RED_FG_COLOR "\r\nWrong command length. Provisioning aborted" ANSI_RESET_COLOR) ;
					ble_app_debug("\r\nWrong command length. Provisioning aborted") ;
					break ;					
				}
				if (ble_appBuffer[29] != PROVISIONING_DELIMITER || ble_appBuffer[46] != PROVISIONING_DELIMITER)
				{
					printf(ANSI_BRIGHT_RED_FG_COLOR "\r\nWrong command format. Provisioning aborted" ANSI_RESET_COLOR) ;
					ble_app_debug("\r\nWrong command format. Provisioning aborted") ;
					break ;
				}
				if (ble_provisioning(ble_appBuffer, strlen(ble_appBuffer)) == true)
				{
					printf(ANSI_BRIGHT_GREEN_FG_COLOR "\r\nOTAA Keys provisioned successfully over BLE\r\n" ANSI_RESET_COLOR) ;
					ble_app_debug("\r\nOTAA Keys provisioned successfully over BLE") ;
					
					lorawan_app_configuration(ISM_EU868) ;
					lorawan_app_join() ;
					while(1)
					{
						lorawan_app_run_tasks() ;
						lorawan_app_sleep() ;
					}
				}
			}
		}
	}
}

/*** ble_app_debug **************************************************************
 \brief      Debug message over BLE Transparent UART
********************************************************************************/
void ble_app_debug(const char *str)
{
	if (RN487x_IsConnected())
	{
		usart_write_buffer_wait(&ble_usart_instance, &str[0], strlen(str)) ;
	}
}

/*** ble_app_debug_array ********************************************************
 \brief      Debug message array over BLE Transparent UART
********************************************************************************/
void ble_app_debug_array(uint8_t *array, uint8_t length)
{
	static char str[20] ;
	uint8_t j = 0 ;
	memset(&str, 0, sizeof(str)) ;
	for (uint8_t i = 0; i < length; i++)
	{
		str[j] = NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(array[i])) ;
		j++ ;
		str[j] = NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(array[i])) ;
		j++ ;
	}
	ble_app_debug(str) ;
}

/*******************************************************************************/

/*** LOCAL FUNCTION DEFINITIONS ************************************************/

static bool ble_provisioning(uint8_t *buffer, uint8_t len)
{
	uint8_t idx = 0 ;
	uint8_t start_idx ;
	uint8_t devEuiExtracted[8] ;
	uint8_t appEuiExtracted[8] ;
	uint8_t appKeyExtracted[16] ;
	// extract the devEui
	start_idx = 13 ;
	for (uint8_t i = 0; i < sizeof(devEuiExtracted); i++)
	{
		idx = start_idx + i * 2 ;
		devEuiExtracted[i] = HEX_PAIR_TO_BYTE(buffer[idx], buffer[idx+1]) ;
		if ((devEuiExtracted[i] < 0) || (devEuiExtracted[i] > 255))
		{
			printf(ANSI_BRIGHT_RED_FG_COLOR "\r\nWrong DevEUI format." ANSI_RESET_COLOR) ;
			return false ;
		}
	}
	printf("\r\nDevEUI extracted: ") ;
	print_array(devEuiExtracted, sizeof(devEuiExtracted)) ;
	
	// extract the appEui
	start_idx = 30 ;
	for (uint8_t i = 0; i < sizeof(appEuiExtracted); i++)
	{
		idx = start_idx + i * 2 ;
		appEuiExtracted[i] = HEX_PAIR_TO_BYTE(buffer[idx], buffer[idx+1]) ;
		if ((appEuiExtracted[i] < 0) || (appEuiExtracted[i] > 255))
		{
			printf(ANSI_BRIGHT_RED_FG_COLOR "\r\nWrong AppEUI format." ANSI_RESET_COLOR) ;
			return false ;
		}
	}
	printf("\r\nAppEUI extracted: ") ;
	print_array(appEuiExtracted, sizeof(appEuiExtracted)) ;
	
	// extract the appKey
	start_idx = 47 ;
	for (uint8_t i = 0; i < sizeof(appKeyExtracted); i++)
	{
		idx = start_idx + i * 2 ;
		appKeyExtracted[i] = HEX_PAIR_TO_BYTE(buffer[idx], buffer[idx+1]) ;
		if ((appKeyExtracted[i] < 0) || (appKeyExtracted[i] > 255))
		{
			printf(ANSI_BRIGHT_RED_FG_COLOR "\r\nWrong AppKey format." ANSI_RESET_COLOR) ;
			return false ;
		}
	}
	printf("\r\nAppKey extracted: ") ;
	print_array(appKeyExtracted, sizeof(appKeyExtracted)) ;
	
	// pass the extracted credentials to the lorawan_app
	lorawan_app_setDevEui(devEuiExtracted, sizeof(devEuiExtracted)) ;
	lorawan_app_setAppEui(appEuiExtracted, sizeof(appEuiExtracted)) ;
	lorawan_app_setAppKey(appKeyExtracted, sizeof(appKeyExtracted)) ;
	
	return(true) ;
}

static void serial_provisioning(uint8_t *buffer, uint8_t len)
{
	uint8_t i = 0 ;
	uint8_t rxInt ;
	uint8_t temp = 0 ;
	while (i != len * 2)
	{
		rxInt = charToInt(sio2host_getchar()) ;
		if (i % 2 == 0) temp = rxInt << 4 ;
		else if (i % 2 != 0) buffer[i/2] = temp + rxInt ;
		if (rxInt != 255) i++ ; // increment count only when receiving a valid hex value
	}
}