// m16946
/****************************** INCLUDES **************************************/
/*** INCLUDE FILES ************************************************************/
#include <asf.h>
#include "ble_usart.h"
#include "conf_ble.h"

/*** SYMBOLIC CONSTANTS ********************************************************/

/*** GLOBAL VARIABLES & TYPE DEFINITIONS ***************************************/

/*** LOCAL FUNCTION PROTOTYPES *************************************************/


/*** GLOBAL FUNCTION DEFINITIONS ***********************************************/

/*** ble_usart_init *************************************************************
 \brief      RN487x USART Init.
********************************************************************************/
void ble_usart_init(void)
{
	struct usart_config config_usart ;
	usart_get_config_defaults(&config_usart) ;
	config_usart.mux_setting = BLE_SERCOM_MUX_SETTING ;
	config_usart.pinmux_pad0 = BLE_SERCOM_PINMUX_PAD0 ;
	config_usart.pinmux_pad1 = BLE_SERCOM_PINMUX_PAD1 ;
	config_usart.pinmux_pad2 = BLE_SERCOM_PINMUX_PAD2 ;
	config_usart.pinmux_pad3 = BLE_SERCOM_PINMUX_PAD3 ;
	config_usart.run_in_standby = false ;
	config_usart.baudrate = BLE_USART_BAUDRATE ;
	
	//m16946 usart needs some time to complete the init. process
	while (usart_init(&ble_usart_instance, BLE_USART, & config_usart) != STATUS_OK) continue ;
	//	usart_init(&ble_usart_instance, BLE_USART, &config_usart) ;
	
	usart_enable(&ble_usart_instance) ;
	usart_enable_transceiver(&ble_usart_instance,USART_TRANSCEIVER_TX) ;
	usart_enable_transceiver(&ble_usart_instance,USART_TRANSCEIVER_RX) ;
}

/*** ble_usart_deinit ***********************************************************
 \brief      RN487x USART De-Init.
********************************************************************************/
void ble_usart_deinit(void)
{
	usart_disable(&ble_usart_instance) ;
	usart_disable_transceiver(&ble_usart_instance, USART_TRANSCEIVER_TX) ;
	usart_disable_transceiver(&ble_usart_instance, USART_TRANSCEIVER_RX) ;
}

/*** ble_usart_is_rx_ready ******************************************************
 \brief      RN487x USART Rx ready check
********************************************************************************/
bool ble_usart_is_rx_ready(struct usart_module *const usart)
{
	return (usart->hw->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXC);
}