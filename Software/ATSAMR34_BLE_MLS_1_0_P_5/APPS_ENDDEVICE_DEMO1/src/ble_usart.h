#ifndef BLE_USART_H
#define	BLE_USART_H

void ble_usart_init(void) ;
void ble_usart_deinit(void) ;
bool ble_usart_is_rx_ready(struct usart_module *const usart) ;

#endif