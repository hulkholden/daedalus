#ifndef INPUT_INPUTMANAGER_H_
#define INPUT_INPUTMANAGER_H_

#include "Ultra/ultra_os.h"

bool InputManager_Create();
void InputManager_Destroy();

void InputManager_GetState( OSContPad (&pPad)[4] );

#endif // INPUT_INPUTMANAGER_H_
