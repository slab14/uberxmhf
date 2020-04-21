/*
	UART para-virtualized driver uberapp backend

	author: amit vasudevan (amitvasudevan@acm.org)
*/

#ifndef __UAPP_PVDRIVER_UART_H__
#define __UAPP_PVDRIVER_UART_H__

#define UAPP_PVDRIVER_UART_UHCALL	0xE0

#define UAPP_PVDRIVER_UART_UHCALL_INIT			    1
#define UAPP_PVDRIVER_UART_UHCALL_SEND				2
#define UAPP_PVDRIVER_UART_UHCALL_RECV				3
#define UAPP_PVDRIVER_UART_UHCALL_CAN_SEND			4
#define UAPP_PVDRIVER_UART_UHCALL_CAN_RECV			5
#define UAPP_PVDRIVER_UART_UHCALL_FLUSH				6


#ifndef __ASSEMBLY__

typedef struct {
	uint8_t uhcall_fn;
	uint32_t iparam_1;
	uint32_t iparam_2;
	uint32_t iparam_3;
	uint32_t iparam_4;
	uint32_t oparam_1;
	uint32_t oparam_2;
	uint32_t status;
}uapp_pvdriver_uart_param_t;



#endif // __ASSEMBLY__



#endif //__UAPP_PVDRIVER_UART_H__
