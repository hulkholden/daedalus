#include "Base/Daedalus.h"
#include "HLEAudio/HLEAudio.h"
#include "Config/ConfigOptions.h"

HLEAudio* gHLEAudio = nullptr;

bool CreateAudioPlugin()
{
	return true;
}

void DestroyAudioPlugin()
{

}