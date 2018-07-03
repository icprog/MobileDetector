/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"
#include <lpc17xx_uart.h>
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/

#define RxE		LPC_GPIO0 -> FIOCLR |= (0x60000)
#define DxE		LPC_GPIO0 -> FIOSET |= (0x60000)

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
	
	if(xRxEnable)
	{
		UART_IntConfig((LPC_UART_TypeDef*)LPC_UART1, UART_INTCFG_RBR, ENABLE);
		RxE;
	}
	else
	{
		UART_IntConfig((LPC_UART_TypeDef*)LPC_UART1, UART_INTCFG_RBR, DISABLE);
		DxE;
	}
	
	if(xTxEnable)
	{
		UART_IntConfig((LPC_UART_TypeDef*)LPC_UART1, UART_INTCFG_THRE,	ENABLE); 
	}
	else
	{
		UART_IntConfig((LPC_UART_TypeDef*)LPC_UART1, UART_INTCFG_THRE, DISABLE);
	}
	
	
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
}


UART_CFG_Type UARTINITSTR;
BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	
	
		LPC_PINCON -> PINMODE0 |= (1 << 30);
		LPC_PINCON -> PINMODE1 |= 1;
		UARTINITSTR.Baud_rate = 19200;
		UARTINITSTR.Databits = UART_DATABIT_8;
		UARTINITSTR.Parity = UART_PARITY_NONE;
		UARTINITSTR.Stopbits = UART_STOPBIT_1;
	
		UART_Init((LPC_UART_TypeDef*)LPC_UART1, &UARTINITSTR);
		
    return TRUE1;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
	
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
	
	
		UART_SendByte((LPC_UART_TypeDef*)LPC_UART1, ucByte);
    return TRUE1;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
	
	
		*pucByte = UART_ReceiveByte((LPC_UART_TypeDef*)LPC_UART1);
    return TRUE1;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
//static void prvvUARTRxISR( void )
static void UART1_IRQHandler( void )
{
    pxMBFrameCBByteReceived(  );
}