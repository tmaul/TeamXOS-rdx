#include <stdio.h>
#include "G_Input.h"
#include "io.h"
#include "G_main.h"

#define KEYDOWN(key) (Keys[key] & 0x80) 
#define MAX_JOYS 8


int Nb_Joys = 0;

long MouseX, MouseY;

int String_Size(char *Chaine)
{
	int i = 0;

	while (*(Chaine + i++));

	return(i - 1);	
}


void End_Input()
{
}



int Init_Input(HINSTANCE hInst, HWND hWnd)
{

	Nb_Joys = 2 ;
	return 1;
}


void Restore_Input()
{

}


void Update_Input()
{

}


int Check_Key_Pressed(unsigned int key)
{

	return 0;
}


unsigned int Get_Key(void)
{

	return 0 ;
}

#ifdef __cplusplus
extern "C" {
#endif


int xbox_check_events();


#ifdef __cplusplus
}
#endif

void Update_Controllers()
{
	xbox_check_events() ;
}


int Setting_Keys(HWND hset, int Player, int TypeP)
{
	return 1;
}
