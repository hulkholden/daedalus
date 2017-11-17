#include "Base/Daedalus.h"
#include "HLEAudio/AudioPlugin.h"
#include "Config/ConfigOptions.h"

CAudioPlugin* gAudioPlugin = nullptr;

bool CreateAudioPlugin()
{
	return true;
}

void DestroyAudioPlugin()
{

}