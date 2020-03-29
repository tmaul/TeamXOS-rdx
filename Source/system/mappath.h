#pragma once
#include <xtl.h>

char * getpath(char * DriveLetter);
HRESULT mappath(char * Device, char * DriveLetter);
void resetpaths();