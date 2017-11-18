#include "Base/Daedalus.h"
#include "Interface/UI.h"

#include <stdio.h>

#include "absl/strings/str_cat.h"

#include "Core/CPU.h"
#include "Core/ROM.h"
#include "Graphics/GL.h"
#include "Interface/SaveState.h"
#include "System/IO.h"
#include "System/Paths.h"
#include "System/Thread.h"

//static bool toggle_fullscreen = false;

static void HandleKeys(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key >= '0' && key <= '9')
		{
			int idx = key - '0';

			bool ctrl_down = (mods & GLFW_MOD_CONTROL) != 0;

			std::string path = IO::Path::Join(GetOutputFilename("SaveStates"), g_ROM.settings.GameName);
			IO::Directory::EnsureExists(path);

			std::string filename = IO::Path::Join(path, absl::StrCat("saveslot", idx, ".ss"));

			if (ctrl_down)
			{
				SaveState_RequestSave(filename);
			}
			else
			{
				if (IO::File::Exists(filename))
				{
					SaveState_RequestLoad(filename);
				}
			}
		}
// Proper full screen toggle still not fully implemented in GLF3
// BUT is in the roadmap for future 3XX release
#if 0
		if(key == GLFW_KEY_F1)
		{
			GLFWmonitor *monitor = NULL;

			// Toggle fullscreen on/off
			toggle_fullscreen ^= 1;

			u32 width = 640; //SCR_WIDTH
			u32 height= 480; //SCR_HEIGHT
			if(toggle_fullscreen)
			{
				monitor = glfwGetPrimaryMonitor();

				// Get destop resolution, this should tell us the max resolution we can use
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				width = mode->width;
				height= mode->height;
			}

			// Arg need to close and re open window to toggle fullscreen :(
			// Hopefully future releases of GLFW should make this simpler
			glfwDestroyWindow(gWindow);

			glfwWindowHint(GLFW_SAMPLES, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

		#ifdef DAEDALUS_OSX
			// OSX 10.7+ only supports 3.2 with core profile/forward compat.
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		#endif

			glfwWindowHint(GLFW_DEPTH_BITS, 24);
			//glfwWindowHint(GLFW_STENCIL_BITS, 0);

			// Open a window and create its OpenGL context
			gWindow = glfwCreateWindow( width, height,
										"Daedalus",
										monitor, NULL );

			glfwSetWindowSize(gWindow, width, height);
			if( !gWindow)
			{
				// FIX ME: What to do here? Should exit?
				fprintf( stderr, "Failed to re open GLFW window!\n" );
				//glfwTerminate();
				return;
			}
		}
#endif
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}

class PollKeyboardCpuEventHandler : public CpuEventHandler
{
	void OnVerticalBlank() override
	{
		glfwPollEvents();
		if (glfwWindowShouldClose(gWindow))
		{
			CPU_Halt("Window Closed");
		}
	}
};
static PollKeyboardCpuEventHandler gPollKeyboard;


bool UI_Init()
{
	DAEDALUS_ASSERT(gWindow != NULL, "The GLFW window should already have been initialised");
	glfwSetKeyCallback(gWindow, &HandleKeys);
	CPU_RegisterCpuEventHandler(&gPollKeyboard);
	return true;
}

void UI_Finalise()
{
	CPU_UnregisterCpuEventHandler(&gPollKeyboard);
}
