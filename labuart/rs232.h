#ifndef _G01_UARTDEF_H_
#define _G01_UARTDEF_H_

#define COM1_UART_IRQ 4
#define COM1_UART_ADDRESS 0X3F8

#define UART_LSR 5
#define UART_LCR 3
#define UART_RBR 0 /// Receiver Buffer Register
#define UART_THR 0 /// Transmitter Holding Register 
#define UART_DLL 0
#define UART_DLH 1
#define UART_FCR 2
#define UART_IIR 2
#define UART_IER 1

#define FIFO_ENABLE BIT(0)
#define FIFO_CLR_RCVR BIT(1)
#define FIFO_CLR_SEND BIT(2)
#define FIFO_ONE_BYTE 0
#define IIR_RECV_DATA_AVAIL BIT(2)
#define IIR_TRNSMT_EMPTY BIT(1)
#define IIR_STATUS BIT(1) | BIT(2)

#define RECEIVE_DATA BIT(0)
#define OVERRUN_ERR BIT(1)
#define PARITY_ERR BIT(2)
#define FRAMING_ERR BIT(3)
#define TRANS_HOLD_EMPTY BIT(5)
#define FIFO_ERR BIT(7)

#define ENABLE_RECEIVE BIT(0)
#define ENABLE_TRANSMIT BIT(1)
#define ENABLE_LINE_STATUS BIT(2)

#define BITS_PER_CHAR(n) (n-5)
#define STOP_BIT_2 BIT(2)
#define ODD_PARITY BIT(3)
#define EVEN_PARITY BIT(3) | BIT(4)
#define C_PARITY_1 BIT(3) | BIT(5)
#define C_PARITY_2 BIT(3) | BIT(4) | BIT(5)
#define DLAB_OFF ~BIT(7)
#define DLAB_ON BIT(7)

#define UART_MAX_TRIES 15
#define UART_DELAY 20


#define BAUD_115200 0x0001
#define BAUD_57600 0x0002

#define GET_MSB(n) ((n >> 8) & 0xff)
#define GET_LSB(n) (n & 0xff)

#endif
