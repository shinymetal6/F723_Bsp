/*
 * helpers.c
 *
 *  Created on: Jul 27, 2021
 *      Author: fil
 */

#include "main.h"
#include "usb_host.h"
#include "fatfs.h"

FATFS USBDISKFatFs;           /* File system object for USB disk logical drive */
FIL MyFile;                   /* File object */
char USBDISKPath[4];          /* USB Host logical drive path */
USBH_HandleTypeDef hUSBHost; /* USB Host handle */

void InitLCD(void)
{
	  BSP_LCD_Init();
	  BSP_LCD_Clear(LCD_COLOR_BLACK);
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	  BSP_LCD_SetFont(&Font24);
	  BSP_LCD_DisplayStringAt(0, 1, (uint8_t *)"LCD", CENTER_MODE);
	  BSP_LCD_SetFont(&Font12);
	  BSP_LCD_DisplayStringAt(0, 20, (uint8_t *)"This example shows LCD", CENTER_MODE);
	  BSP_LCD_DisplayStringAt(0, 35, (uint8_t *)"features, press button", CENTER_MODE);
	  BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)"to display next page", CENTER_MODE);

}

void USB_Error_Handler(char *text)
{
	  BSP_LCD_SetTextColor(LCD_COLOR_RED);
	  BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)text, CENTER_MODE);
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
}

static void MSC_Application(uint8_t from)
{
  FRESULT res;                                          /* FatFs function common result code */
  uint32_t byteswritten, bytesread;                     /* File write/read counts */
  uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
  uint8_t rtext[100];                                   /* File read buffer */
if ( from == 0 )
	  BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)"From FS", CENTER_MODE);
else
	  BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)"From HS", CENTER_MODE);

  /* Register the file system object to the FatFs module */
  if(f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
  {
    /* FatFs Initialization Error */
      USB_Error_Handler("Error mounting disk");
  }
  else
  {
      /* Create and Open a new text file object with write access */
      if(f_open(&MyFile, "Even.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
      {
        /* 'STM32.TXT' file Open for write Error */
        USB_Error_Handler("Error creating file");
      }
      else
      {
        /* Write data to the text file */
        res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

        if((byteswritten == 0) || (res != FR_OK))
        {
          /* 'STM32.TXT' file Write or EOF Error */
            USB_Error_Handler("Error writing file");
        }
        else
        {
          /* Close the open text file */
          f_close(&MyFile);

        /* Open the text file object with read access */
        if(f_open(&MyFile, "Even.TXT", FA_READ) != FR_OK)
        {
          /* 'STM32.TXT' file Open for read Error */
            USB_Error_Handler("Error opening file");
        }
        else
        {
          /* Read data from the text file */
          res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

          if((bytesread == 0) || (res != FR_OK))
          {
            /* 'STM32.TXT' file Read or EOF Error */
              USB_Error_Handler("Error reading file");
          }
          else
          {
            /* Close the open text file */
            f_close(&MyFile);

            /* Compare read data with the expected data */
            if((bytesread != byteswritten))
            {
              /* Read data is different from the expected data */
                USB_Error_Handler("Error file mismatch");
            }
            else
            {
          /* Success of the demo: no error occurrence */
          	  BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)"USB OK", CENTER_MODE);
          	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
          	Explore_Disk("\\",0);
            }
          }
        }
      }
    }
  }

  /* Unlink the USB disk I/O driver */
  FATFS_UnLinkDriver(USBDISKPath);
}


void USB_CallFromFS(void)
{
	MSC_Application(0);
}

void USB_CallFromHS(void)
{
	MSC_Application(1);
}
