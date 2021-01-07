#ifndef BLE_CONFIG_H_
#define BLE_CONFIG_H_

/****************************** INCLUDES **************************************/

/****************************** MACROS **************************************/
/* Reset line */
#define BLE_RESET_LINE	EXT1_PIN_5

/* usart configuration */
#define BLE_USART					SERCOM1
#define BLE_SERCOM_MUX_SETTING		EXT3_UART_SERCOM_MUX_SETTING
#define BLE_SERCOM_PINMUX_PAD0		EXT3_UART_SERCOM_PINMUX_PAD0
#define BLE_SERCOM_PINMUX_PAD1		EXT3_UART_SERCOM_PINMUX_PAD1
#define BLE_SERCOM_PINMUX_PAD2		EXT3_UART_SERCOM_PINMUX_PAD2//3
#define BLE_SERCOM_PINMUX_PAD3		EXT3_UART_SERCOM_PINMUX_PAD3//2
#define BLE_USART_BAUDRATE			115200

struct usart_module ble_usart_instance ;

#endif /* BLE_CONFIG_H_ */

