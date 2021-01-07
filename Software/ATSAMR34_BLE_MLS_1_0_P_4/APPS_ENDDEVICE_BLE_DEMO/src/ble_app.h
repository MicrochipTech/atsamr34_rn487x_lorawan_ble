#ifndef BLE_APP_H
#define	BLE_APP_H

void ble_app_init(void) ;
void ble_app_print_menu(void) ;
void ble_app_select_application(void) ;
void ble_app_serial_bridge(void) ;
void ble_app_beacon(void) ;
void ble_app_transparent(void) ;
void ble_app_scan(void) ;
void ble_app_search(bool connect) ;
void ble_app_power_down(void) ;
void ble_app_power_up(void) ;
void ble_app_lorawan_provisioning(void) ;
void ble_app_debug(const char *str) ;
void ble_app_debug_array(uint8_t *array, uint8_t length) ;

#endif