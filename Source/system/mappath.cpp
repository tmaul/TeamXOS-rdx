/////
//
//	This code is part of the rdx project. (c) Team Xos
//
//  Team xos grant you the right to use this code in any non commerical way,
//  under the condidtion that you release your source code with any compiled binaries
//
//  If you make any useful changes to the code you are encouraged to email to us
//  for inclusion in a future version. Full credit will be given.
//
//  rdxdev@gmail.com
//

#include "..\dash.h"

struct pathconv_s {
	char * DriveLetter;
	char * FullPath;
} pathconv_table[] = {
	{ "A:", "\\Device\\Cdrom0" },
	{ "C:", "\\Device\\Harddisk0\\Partition2" },
	{ "E:", "\\Device\\Harddisk0\\Partition1" },
	{ "F:", "\\Device\\Harddisk0\\Partition6" },
	{ "X:", "\\Device\\Harddisk0\\Partition3" },
	{ "Y:", "\\Device\\Harddisk0\\Partition4" },
	{ "Z:", "\\Device\\Harddisk0\\Partition5" },

	{ NULL, NULL }
};

char * getpath(char * DriveLetter) {
	int i;

	for(i=0; pathconv_table[i].DriveLetter!=NULL; i++) {
		if(!lstrcmpi(pathconv_table[i].DriveLetter,DriveLetter))
			return pathconv_table[i].FullPath;
	}
	return NULL;
}

HRESULT mappath(char * Device, char * DriveLetter) {
	char *tmp=NULL;

	if(DriveLetter[0]!='\\') {
		tmp=(char *)malloc(5+strlen(DriveLetter));
		sprintf(tmp, "\\??\\%s", DriveLetter);
		DriveLetter=tmp;
	}

	ANSI_STRING DeviceName = {
		strlen(Device),
		strlen(Device) + 1,
		Device
	};

	ANSI_STRING LinkName = {
		strlen(DriveLetter),
		strlen(DriveLetter) + 1,
		DriveLetter
	};

#if 0
	IoDeleteSymbolicLink(&LinkName);
	IoCreateSymbolicLink(&LinkName, &DeviceName);
#else
	char U, M;

	if(IoDeleteSymbolicLink(&LinkName))
		U='N';
	else
		U='Y';
	if(IoCreateSymbolicLink(&LinkName, &DeviceName))
		M='N';
	else
		M='Y';
#endif

	if(tmp)
		free(tmp);

	return S_OK;
}

void resetpaths() {
	int i;

	for(i=0; pathconv_table[i].DriveLetter!=NULL; i++) {
		mappath(pathconv_table[i].FullPath , pathconv_table[i].DriveLetter);
	}
}

