/*
 * audio.h
 *
 *  Created on: Aug 10, 2021
 *      Author: fil
 */

#ifndef SRC_BSP_SUBSYSTEM_AUDIO_AUDIO_H_
#define SRC_BSP_SUBSYSTEM_AUDIO_AUDIO_H_

#define AUDIO_DEFAULT_VOLUME    	40
#define AUDIO_DEFAULT_SAMPLEFREQ    44100

typedef struct {
	uint8_t 	state;
	uint8_t		Volume;
	uint32_t	AudioFreq;	//11500 , 12525, 16000, 22050, 32000, 44115, 48000, 96000
	uint32_t	srcbuf_index;
	uint32_t	srcbuf_size;
}AUDIO_BufferTypeDef;

extern AUDIO_BufferTypeDef  Audio;

#define	AUDIO_STATE_FLAG_HALF		1
#define	AUDIO_STATE_FLAG_FULL		2

#define AUDIO_BUFFER_SIZE           2048
#define AUDIO_LEN 					2048
extern	uint16_t audio_sample[AUDIO_LEN];

#endif /* SRC_BSP_SUBSYSTEM_AUDIO_AUDIO_H_ */
