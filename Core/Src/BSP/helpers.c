/*
 * helpers.c
 *
 *  Created on: Jul 27, 2021
 *      Author: fil
 */

#include "main.h"
#include "usb_host.h"
#include "usbh_def.h"
#include "usbh_MIDI.h"

/* dekrispator */

#include "fatfs.h"

FATFS USBDISKFatFs;           /* File system object for USB disk logical drive */
FIL MyFile;                   /* File object */
char USBDISKPath[4];          /* USB Host logical drive path */

USBH_HandleTypeDef hUSBHost; /* USB Host handle */

extern	uint32_t	usbdisk_ready;

void InitLCD(void)
{
	  BSP_LCD_Init();
	  BSP_LCD_Clear(LCD_COLOR_BLACK);
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	  BSP_LCD_SetFont(&Font24);
	  BSP_LCD_DisplayStringAt(0, 1, (uint8_t *)"MIDI Monitor", CENTER_MODE);
	  BSP_LCD_SetFont(&Font12);
	  BSP_LCD_DisplayStringAt(0, 25, (uint8_t *)"MIDI messages displayed below", CENTER_MODE);

}

void USB_Error_Handler(char *text)
{
	  BSP_LCD_SetTextColor(LCD_COLOR_RED);
	  BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)text, CENTER_MODE);
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
}

uint8_t	midi_buf[64],msg[64],midi_buffer[64];

void MSC_Application(uint8_t from)
{
FRESULT res;                                          /* FatFs function common result code */
uint32_t  bytesread;                     /* File write/read counts */
uint8_t rtext[100];                                   /* File read buffer */

	if(f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
	{
		USB_Error_Handler("Error mounting disk");
		return;
	}
	/*
	if(f_open(&MyFile, "Even.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		USB_Error_Handler("Error creating file");
		return;
	}
	res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);
	if((byteswritten == 0) || (res != FR_OK))
	{
		USB_Error_Handler("Error writing file");
		return;
	}
	f_close(&MyFile);
*/
	if(f_open(&MyFile, "sample.wav", FA_READ) != FR_OK)
	{
		USB_Error_Handler("Error opening file");
		return;
	}
	res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);
	if((bytesread == 0) || (res != FR_OK))
	{
		USB_Error_Handler("Error reading file");
		return;
	}

	f_close(&MyFile);
	BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)"USB OK", CENTER_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	usbdisk_ready = 1;

	/* Unlink the USB disk I/O driver */
	//FATFS_UnLinkDriver(USBDISKPath);
}

uint8_t	midi_state = 0;
void MIDI_Application(uint8_t from,USBH_HandleTypeDef *phost, uint8_t id)
{
char	msg[512];

	if ( midi_state == 0 )
	{
		if ( from == 0 )
			sprintf(msg, "%s %s from FS", (char *)phost->pClass[0]->Name,(char *)phost->device.Data);
		else
			sprintf(msg, "%s %s from HS", (char *)phost->pClass[0]->Name,(char *)phost->device.Data);
		BSP_LCD_DisplayStringAt(0, 55, (uint8_t *)msg, CENTER_MODE);
		midi_state++;
	}
	else
	{
		sprintf(msg, "0x%02x 0x%02x 0x%02x 0x%02x", midi_buffer[0],midi_buffer[1],midi_buffer[2],midi_buffer[3]);

	}
	  USBH_MIDI_Receive(phost, midi_buf, 64);

}

void USB_CallFromFS(USBH_HandleTypeDef *phost, uint8_t id)
{
	if ( strncmp((char *)phost->pClass[0]->Name,"MIDI",4) == 0 )
		MIDI_Application(0,phost,id);
}

void USB_CallFromHS(USBH_HandleTypeDef *phost, uint8_t id)
{
	if ( strncmp((char *)phost->pClass[0]->Name,"MSC",3) == 0 )
		MSC_Application(1);
}

void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost)
{
uint8_t	i,k=0,j;
	for(i=0;i<64;i++)
	{

		if (( midi_buf[i] == 0 ) && (midi_buf[i+1] == 0) && ((i & 0x03 ) == 0 ))
			i = 64;
		else if ((( midi_buf[i] & 0xf0) != 0 ) && ((i & 0x03 ) == 0 ))
			i +=3;
		else
		{
			if ((midi_buf[i] == 0x0b ) || (midi_buf[i] == 0x08 ) || (midi_buf[i] == 0x09 ))
			{
				for ( j=0;j<4 ; j++,k++,i++)
					midi_buffer[k] = midi_buf[i];
				i -= 1;
			}
		}
	}
	sprintf(msg, "0x%02x 0x%02x 0x%02x 0x%02x\0", midi_buffer[0],midi_buffer[1],midi_buffer[2],midi_buffer[3]);
	BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)msg, CENTER_MODE);
	USBH_MIDI_Receive(phost, midi_buf, 64);
}
