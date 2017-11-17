#include "Base/Daedalus.h"
#include "HLEAudio/AudioPlugin.h"
#include "Config/ConfigOptions.h"

CAudioPlugin* gAudioPlugin = nullptr;
EAudioPluginMode gAudioPluginEnabled = APM_DISABLED;

bool CreateAudioPlugin()
{
	return true;
}

void DestroyAudioPlugin()
{

}