#include "Arduino.h"




#ifndef DFPlayerMini_Fast_cpp
	#define DFPlayerMini_Fast_cpp




	//-------------------------------------------------------------------------------------//
	// Packet Values
	//-------------------------------------------------------------------------------------//
	#define DFPLAYER_SEND_LENGTH		10
	#define SB				0x7E	//start byte
	#define VER				0xFF	//version
	#define LEN				0x6	//number of bytes after "LEN" (except for checksum data and EB)
	#define FEEDBACK			1	//feedback requested
	#define NO_FEEDBACK			0	//no feedback requested
	#define EB				0xEF	//end byte

	//-------------------------------------------------------------------------------------//
	// Control Command Values
	//-------------------------------------------------------------------------------------//
	#define NEXT_COMMAND			0x01
	#define PREV_COMMAND			0x02
	#define PLAY_COMMAND			0x03
	#define INC_VOL_COMMAND			0x04
	#define DEC_VOL_COMMAND			0x05
	#define VOLUME_COMMAND			0x06
	#define EQ_COMMAND			0x07
	#define PLAYBACK_MODE_COMMAND		0x08
	#define PLAYBACK_SRC_COMMAND		0x09
	#define STANDBY_COMMAND			0x0A
	#define NORMAL_COMMAND			0x0B
	#define RESET_COMMAND			0x0C
	#define PLAYBACK_COMMAND		0x0D
	#define PAUSE_COMMAND			0x0E
	#define SPEC_FOLDER_COMMAND		0x0F
	#define VOL_ADJ_COMMAND			0x10
	#define REPEAT_PLAY_COMMAND		0x11

	//-------------------------------------------------------------------------------------//
	// Query Command Values
	//-------------------------------------------------------------------------------------//
	#define STAY_1				0x3C
	#define STAY_2				0x3D
	#define STAY_3				0x3E
	#define SEND_INIT_COMMAND		0x3F
	#define RETRANSMIT_COMMAND		0x40
	#define REPLY_COMMAND			0x41
	#define GET_STATUS_COMMAND		0x42
	#define GET_VOL_COMMAND			0x43
	#define GET_EQ_COMMAND			0x44
	#define GET_MODE_COMMAND		0x45
	#define GET_VERSION_COMMAND		0x46
	#define GET_TF_FILES_COMMAND		0x47
	#define GET_U_FILES_COMMAND		0x48
	#define GET_FLASH_FILES_COMMAND		0x49
	#define KEEP_ON_COMMAND			0x4A
	#define GET_TF_TRACK_COMMAND		0x4B
	#define GET_U_TRACK_COMMAND		0x4C
	#define GET_FLASH_TRACK_COMMAND		0x4D

	//-------------------------------------------------------------------------------------//
	// EQ Values
	//-------------------------------------------------------------------------------------//
	#define EQ_NORMAL			0
	#define EQ_POP				1
	#define EQ_ROCK				2
	#define EQ_JAZZ				3
	#define EQ_CLASSIC			4
	#define EQ_Base				5

	//-------------------------------------------------------------------------------------//
	// Mode Values
	//-------------------------------------------------------------------------------------//
	#define REPEAT				0
	#define FOLDER_REPEAT			1
	#define SINGLE_REPEAT			2
	#define RANDOM				3

	//-------------------------------------------------------------------------------------//
	// Playback Source Values
	//-------------------------------------------------------------------------------------//
	#define U				1
	#define TF				2
	#define AUX				3
	#define SLEEP				4
	#define FLASH				5

	//-------------------------------------------------------------------------------------//
	// Base Volume Adjust Value
	//-------------------------------------------------------------------------------------//
	#define VOL_ADJUST			0x10

	//-------------------------------------------------------------------------------------//
	// Repeat Play Values
	//-------------------------------------------------------------------------------------//
	#define STOP_REPEAT			0
	#define START_REPEAT			1




	class DFPlayerMini_Fast
	{
	public:
		Stream* _serial;
    
		uint8_t _sending[DFPLAYER_SEND_LENGTH] = {0};
  
		uint8_t commandValue = 0;
		uint8_t feedbackValue = 0;
		uint8_t paramMSB = 0;
		uint8_t paramLSB = 0;
		uint8_t checksumMSB = 0;
		uint8_t checksumLSB = 0;




		void begin(Stream& stream);

		void playNext();
		void playPrevious();
		void play(uint16_t trackNum);
		void incVolume();
		void decVolume();
		void volume(uint8_t volume);
		void EQSelect(uint8_t setting);
		void playbackMode(uint8_t mode);
		void playbackSource(uint8_t source);
		void standbyMode();
		void normalMode();
		void reset();
		void playback();
		void pause();
		void playFolder(uint8_t folderNum, uint8_t trackNum);
		void volumeAdjustSet(uint8_t gain);
		void startRepeatPlay();
		void stopRepeatPlay();

		void sleep();
		void wakeUp();
		void loop(uint16_t trackNum);

		void findChecksum();
		void sendData();
	};

#endif
