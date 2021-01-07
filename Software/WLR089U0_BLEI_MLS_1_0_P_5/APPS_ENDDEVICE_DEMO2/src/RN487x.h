#ifndef RN487X_H
#define	RN487X_H

void RN487x_HandleComIssue(void) ;
bool RN487x_IsConnected(void) ;
bool RN487x_Init(void) ;
void RN487x_SetResetLine(bool state) ;
void RN487x_ResetSequence(void) ;
bool RN487x_EnterCmdMode(void) ;
bool RN487x_EnterDataMode(void) ;
bool RN487x_GetResponse(uint8_t *resp) ;
bool RN487x_GetExpectedResponse(const uint8_t *expectedResp, uint8_t expectedRespLen) ;
bool RN487x_GetDefaultResponse(void) ;
bool RN487x_GetCmdResponse(const char *cmd, uint8_t cmdLen, char *cmdResp) ;
bool RN487x_FilterMessage(uint8_t *msg, uint8_t msgLen) ;
void RN487x_SendCmd(const uint8_t *cmd, uint8_t cmdLen) ;
void RN487x_SendData(const char *data, uint8_t dataLen) ;
bool RN487x_RebootCmd(void) ;
bool RN487x_Connect(uint8_t addrType, char *addr) ;
bool RN487x_Disconnect(void) ;


#endif