#include "DFPlayerMini_Fast.h"

void DFPlayerMini_Fast::begin(Stream &stream)
{
	_serial = &stream;
  
	return;
}

void DFPlayerMini_Fast::playNext()
{
	commandValue = NEXT_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = 1;

	findChecksum();
	sendData();

	return;
}

void DFPlayerMini_Fast::playPrevious()
{
	commandValue = PREV_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = 1;

	findChecksum();
	sendData();

	return;
}

void DFPlayerMini_Fast::play(uint16_t trackNum)
{
	commandValue = PLAY_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = (trackNum >> 8) & 0xFF;
	paramLSB = trackNum & 0xFF;

	findChecksum();
	sendData();

	return;
}

void DFPlayerMini_Fast::incVolume()
{
	commandValue = INC_VOL_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = 1;

	findChecksum();
	sendData();

	return;
}




void DFPlayerMini_Fast::decVolume()
{
	commandValue = DEC_VOL_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = 1;

	findChecksum();
	sendData();

	return;
}




void DFPlayerMini_Fast::volume(uint8_t volume)
{
	if (volume <= 30)
	{
		commandValue = VOLUME_COMMAND;
		feedbackValue = NO_FEEDBACK;
		paramMSB = 0;
		paramLSB = volume;
  
		findChecksum();
		sendData();
	}
  
	return;
}




void DFPlayerMini_Fast::EQSelect(uint8_t setting)
{
	if (setting <= 5)
	{
		commandValue = EQ_COMMAND;
		feedbackValue = NO_FEEDBACK;
		paramMSB = 0;
		paramLSB = setting;

		findChecksum();
		sendData();
	}

	return;
}




void DFPlayerMini_Fast::playbackMode(uint8_t mode)
{
	if (mode <= 5)
	{
		commandValue = PLAYBACK_MODE_COMMAND;
		feedbackValue = NO_FEEDBACK;
		paramMSB = 0;
		paramLSB = mode;

		findChecksum();
		sendData();
	}

	return;
}




void DFPlayerMini_Fast::playbackSource(uint8_t source)
{
	if ((source > 0) && (source <= 5))
	{
		commandValue = PLAYBACK_SRC_COMMAND;
		feedbackValue = NO_FEEDBACK;
		paramMSB = 0;
		paramLSB = source;

		findChecksum();
		sendData();
	}

	return;
}




void DFPlayerMini_Fast::standbyMode()
{
	commandValue = STANDBY_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = 1;

	findChecksum();
	sendData();

	return;
}




void DFPlayerMini_Fast::normalMode()
{
	commandValue = NORMAL_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = 1;

	findChecksum();
	sendData();

	return;
}




void DFPlayerMini_Fast::reset()
{
	commandValue = RESET_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = 1;

	findChecksum();
	sendData();

	return;
}




void DFPlayerMini_Fast::playback()
{
	commandValue = PLAYBACK_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = 1;

	findChecksum();
	sendData();

	return;
}




void DFPlayerMini_Fast::pause()
{
	commandValue = PAUSE_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = 1;

	findChecksum();
	sendData();

	return;
}




void DFPlayerMini_Fast::playFolder(uint8_t folderNum, uint8_t trackNum)
{
	commandValue = SPEC_FOLDER_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = folderNum;
	paramLSB = trackNum;

	findChecksum();
	sendData();

	return;
}




void DFPlayerMini_Fast::volumeAdjustSet(uint8_t gain)
{
	if (gain <= 31)
	{
		commandValue = VOL_ADJ_COMMAND;
		feedbackValue = NO_FEEDBACK;
		paramMSB = 0;
		paramLSB = VOL_ADJUST + gain;

		findChecksum();
		sendData();
	}

	return;
}




void DFPlayerMini_Fast::startRepeatPlay()
{
	commandValue = REPEAT_PLAY_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = START_REPEAT;

	findChecksum();
	sendData();

	return;
}




void DFPlayerMini_Fast::stopRepeatPlay()
{
	commandValue = REPEAT_PLAY_COMMAND;
	feedbackValue = NO_FEEDBACK;
	paramMSB = 0;
	paramLSB = STOP_REPEAT;

	findChecksum();
	sendData();

	return;
}




void DFPlayerMini_Fast::sleep()
{
	playbackSource(SLEEP);

	return;
}




void DFPlayerMini_Fast::wakeUp()
{
	playbackSource(TF);

	delay(100);

	return;
}




void DFPlayerMini_Fast::loop(uint16_t trackNum)
{
  commandValue = PLAYBACK_MODE_COMMAND;
  feedbackValue = NO_FEEDBACK;
  paramMSB = (trackNum >> 8) & 0xFF;
  paramLSB = trackNum & 0xFF;
  
  findChecksum();
  sendData();
  
  return;
}




void DFPlayerMini_Fast::findChecksum()
{
	uint16_t checksum = (~(VER + LEN + commandValue + feedbackValue + paramMSB + paramLSB)) + 1;

	checksumMSB = checksum >> 8;
	checksumLSB = checksum & 0xFF;

	return;
}




void DFPlayerMini_Fast::sendData()
{
	_sending[0] = SB;
	_sending[1] = VER;
	_sending[2] = LEN;
	_sending[3] = commandValue;
	_sending[4] = feedbackValue;
	_sending[5] = paramMSB;
	_sending[6] = paramLSB;
	_sending[7] = checksumMSB;
	_sending[8] = checksumLSB;
	_sending[9] = EB;
  
	_serial->write(_sending, DFPLAYER_SEND_LENGTH);
  
	return;
}



