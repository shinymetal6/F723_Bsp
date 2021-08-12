/*
 * audio.c
 *
 *  Created on: Aug 9, 2021
 *      Author: fil
 */
#include "main.h"
#include "audio.h"
#include <stdio.h>
#include "fatfs.h"

uint8_t					wav_sample[65536];
AUDIO_BufferTypeDef  	Audio;
uint32_t				sample_rate = 44100;

void InitAudio(void)
{
	Audio.Volume = AUDIO_DEFAULT_VOLUME;
	Audio.AudioFreq = AUDIO_DEFAULT_SAMPLEFREQ;
	Audio.srcbuf_index = WAV_HEADER;
	BSP_LCD_SetFont(&Font12);
	if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, Audio.Volume, Audio.AudioFreq) == 0)
	{
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(0, 40, (uint8_t *)"  AUDIO CODEC OK  ", LEFT_MODE);
	}
	else
	{
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_DisplayStringAt(0, 40, (uint8_t *)" AUDIO CODEC FAIL ", LEFT_MODE);
		BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)" Try to reset board ", LEFT_MODE);
	}

	BSP_AUDIO_OUT_Play((uint16_t *)wav_sample, AUDIO_BUFFER_SIZE);
    BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_OFF);
}

uint32_t	i;
extern	FIL InstrumentsFile;
extern	WAVE_FormatTypeDef	WaveFormat;

uint8_t AUDIO_Process(void)
{
uint32_t	bytesread;

	if(Audio.srcbuf_index >= WaveFormat.FileSize)
	{
		BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_ON);
	}
	if (( Audio.state &= AUDIO_STATE_FLAG_HALF) == AUDIO_STATE_FLAG_HALF)
	{
		if(f_read(&InstrumentsFile, &wav_sample[0], AUDIO_BUFFER_SIZE/2,(void *)&bytesread) == FR_OK)
		{
			Audio.srcbuf_index += bytesread;
			f_lseek (&InstrumentsFile,Audio.srcbuf_index);
			Audio.state &= ~AUDIO_STATE_FLAG_HALF;
		}
	}
	if (( Audio.state &= AUDIO_STATE_FLAG_FULL) == AUDIO_STATE_FLAG_FULL)
	{
		if(f_read(&InstrumentsFile, &wav_sample[AUDIO_BUFFER_SIZE/2], AUDIO_BUFFER_SIZE/2,(void *)&bytesread) == FR_OK)
		{
			Audio.srcbuf_index += bytesread;
			f_lseek (&InstrumentsFile,Audio.srcbuf_index);
			Audio.state &= ~AUDIO_STATE_FLAG_FULL;
		}
	}
    return 0;
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	/* allows AUDIO_Process() to refill 2nd part of the buffer  */
	Audio.state &= ~AUDIO_STATE_FLAG_HALF;
	Audio.state |= AUDIO_STATE_FLAG_FULL;
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	/* allows AUDIO_Process() to refill 1st part of the buffer  */
	Audio.state &= ~AUDIO_STATE_FLAG_FULL;
	Audio.state |= AUDIO_STATE_FLAG_HALF;
}


void BSP_AUDIO_OUT_Error_CallBack(void)
{
  /* Display message on the LCD screen */
  BSP_LCD_SetBackColor(LCD_COLOR_RED);
  BSP_LCD_DisplayStringAt(0, LINE(14), (uint8_t *)"       DMA  ERROR     ", LEFT_MODE);
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

  /* Stop the program with an infinite loop */
  while (BSP_PB_GetState(BUTTON_WAKEUP) != RESET)
  {
    return;
  }

}

