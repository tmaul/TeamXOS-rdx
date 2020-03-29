#include <stdio.h>

#include "types.h"
#include "fceu.h"

#include "driver.h"
#include "sound.h"
#include "wave.h"

static FILE *soundlog=0;
static long wsize;

/* Checking whether the file exists before wiping it out is left up to the 
   reader..err...I mean, the driver code, if it feels so inclined(I don't feel
   so).
*/
void FCEU_WriteWaveData(int32 *Buffer, int Count)
{

}

int FCEUI_EndWaveRecord(void)
{
 
 return 1;
}


int FCEUI_BeginWaveRecord(char *fn)
{

 return(1);
}
