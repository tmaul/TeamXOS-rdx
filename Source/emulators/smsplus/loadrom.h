#ifndef _LOADROM_H_
#define _LOADROM_H_

#ifdef __cplusplus
extern "C" {
#endif
/* Function prototypes */
int load_rom(char *filename);
unsigned char *loadzip(char *archive, char *filename, int *filesize);
#ifdef __cplusplus
}
#endif

#endif /* _LOADROM_H_ */

