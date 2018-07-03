﻿/*
 * terminal.c
 *
 * Created: 11.02.2016 13:39:44
 *  Author: Михеенко Е
 */ 




#include <terminal.h>



static char data_str[MAX_PARAMS][MAX_PARAMS_STR_LEN];
static char trt[32];


void debug_print( uint16_t data )
{
	char buf[8];
	int len = sprintf(buf, "%d", data);
	buf[len++] = '\r';
	buf[len++] = '\n';
	debug_print_str( (const char*)buf, len );
}

void debug_print_str( const char *lpStr, int strLen )
{
	uint16_t BufferCount;
	cli();
	
	for (register int i = 0; i < strLen; i++)
	{
		BufferCount = RingBuffer_GetCount( &Buffer );
		if( BufferCount >= ringbuffsize )
		break;
		
		RingBuffer_Insert( &Buffer, (const uint8_t)*(lpStr++) );
	}
	
	if(	(UCSR0B & (1 << 6)) == 0)
	{
		UCSR0B |= (1 << 6);
		UDR0 = RingBuffer_Remove( &Buffer );
	}
	
	sei();
}

void debug_print_u16buf( uint16_t* src, uint8_t size ) {
	if( (src == NULL) || ( size == 0 ) )
	return;
	
	#define TMP_SIZE		70

	char buf[TMP_SIZE];
	int index = 0;
	int len = 0;
	
	for( int i = 0; i < 2; i++ ){
		len = sprintf(&buf[index], "0x%04X", *src++);
		if( len <= 0 )
		break;
		
		index += len;
		buf[index++] = ' ';
		if( index >= TMP_SIZE )
		break;
	}
	
	if( ( index + 2 ) < TMP_SIZE ) {
		buf[index++] = '\r';
		buf[index++] = '\n';
	}
	debug_print_str( (const char*)buf, index );
}



void rx_parse( void )			//Parse message from terminal
{
	uint8_t j = 0;
	uint8_t k = 0;
	uint8_t space_flag = 0;
	bool waitSplitter = false;
	for( int i = 0; i < rx_num; i++)
	{
		if(j >= MAX_PARAMS)
		break;
		if( (rx_data_buf[i] != ' ') && ( !waitSplitter ) ) {
			data_str[j][k++] = rx_data_buf[i];
			space_flag++;
			continue;
		}
		
		if( space_flag == 0 )
		continue;
		
		if( k >= MAX_PARAMS_STR_LEN ) {
			waitSplitter = true;
		}
		
		if( ( waitSplitter ) ) {
			if ( rx_data_buf[i] == ' ' ) {
				waitSplitter = false;
				k = MAX_PARAMS_STR_LEN - 1;
			}
			else
			continue;
		}
		
		data_str[j][k] = '\0';
		j++;
		k=0;
		space_flag = 0;
	}

	if( k != 0 ) {
		data_str[j][k] = '\0';
		j++;
	}

	
	if( strcmp(data_str[0], "set") == 0 && j == 3)
	{
		
		if( strcmp(data_str[1],"A") == 0)
		{
			kA  = (float)atof(data_str[2]);
			DEBUG_PRINT("\n is A\n\r");
			DEBUG_PRINT("\n\r");
		}
		else if( strcmp(data_str[1],"B") == 0)

		{
			kB  = (float)atof(data_str[2]);
			DEBUG_PRINT("\n is B\n\r");
			DEBUG_PRINT("\n\r");
		}
		else if( strcmp(data_str[1],"C") == 0)
		{
			kC  = (float)atof(data_str[2]);
			DEBUG_PRINT("\n is C\n\r");
			DEBUG_PRINT("\n\r");
		}
		else if( strcmp(data_str[1],"D") == 0)
		{
			kD  = (float)atof(data_str[2]);
			DEBUG_PRINT("\n is D\n\r");
			DEBUG_PRINT("\n\r");
		}
		else if( strcmp(data_str[1], "default") == 0)
		{
			if( strcmp(data_str[2], "gdu") == 0)
			{				
				write_params_to_eeprom(GDU);
				DEBUG_PRINT("\n\r is default gdu\n\r");
			}
			else if( strcmp(data_str[2], "sip") == 0)
			{
				write_params_to_eeprom(SIP);
				DEBUG_PRINT("\n\r is default sip\n\r");
			}
			else
			{
				DEBUG_PRINT("\n\r unknown default \n\r");
			}

		}
		
		else
		{
			DEBUG_PRINT("\n\r unknown parameter\n\r");
		}
	}
	else if(strcmp(data_str[0], "read") == 0 && j == 2)
	{
		DEBUG_PRINT("\nis read\n");
		if(strcmp(data_str[1], "A") == 0)
		{
			int leng = sprintf(trt, "%.5F", kA);
			
			DEBUG_PRINT("\n\rA= ");
			debug_print_str(trt, leng);
			DEBUG_PRINT("\n\r");
		}
		else if(strcmp(data_str[1], "B") == 0)
		{
			int leng = sprintf(trt, "%.5F", kB);
			
			DEBUG_PRINT("\n\rB= ");
			debug_print_str(trt, leng);
			DEBUG_PRINT("\n\r");
		}
		else if(strcmp(data_str[1], "C") == 0)
		{
			int leng = sprintf(trt, "%.5F", kC);
			
			DEBUG_PRINT("\n\rC= ");
			debug_print_str(trt, leng);
			DEBUG_PRINT("\n\r");
		}
		else if(strcmp(data_str[1], "D") == 0)
		{
			int leng = sprintf(trt, "%.5F", kD);
			
			DEBUG_PRINT("\n\rD= ");
			debug_print_str(trt, leng);
			DEBUG_PRINT("\n\r");
		}
		else
		{
			DEBUG_PRINT("\n\runknown parameter\n\r");
		}
	}
	else if(strcmp(data_str[0], "save") == 0 && j == 1)
	{
		
		write_params_to_eeprom(SET_VAL);
		DEBUG_PRINT("\nsaved\n");
	}
	else if(strcmp(data_str[0], "help") == 0 && j == 1)
	{
		DEBUG_PRINT("\n\r__________\r");
		DEBUG_PRINT("\n\r COMMANDS \r");
		DEBUG_PRINT("\n\r set	\n\rread	\n\rsave\n\r");
		
// 		DEBUG_PRINT("\n\r__________\r");
 		DEBUG_PRINT("\n\r PARAMETERS \r");
 		DEBUG_PRINT("\n\r A\n\r B \n\r C \n\r D");
//		DEBUG_PRINT("\n\r__________\r");
		DEBUG_PRINT("\n\r To set default settings, type:\r");
		DEBUG_PRINT("\n\r set default gdu \n\r or \n\r set default sip\r");
		DEBUG_PRINT("\n\r_________\r\n");
	}
	else
	{
		DEBUG_PRINT("\n\runknown command\n\r");
	}
}


