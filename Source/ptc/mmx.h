//
// TinyPTC by Gaffer
// www.gaffer.org/tinyptc
//

#ifndef __TINYPTC_MMX_H
#define __TINYPTC_MMX_H
#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

void __cdecl mmx_memcpy(void *d,void *s,int bytes);
void __cdecl mmx_convert_32_to_32_bgr888(void *d,void *s,int pixels);
void __cdecl mmx_convert_32_to_24_rgb888(void *d,void *s,int pixels);
void __cdecl mmx_convert_32_to_24_bgr888(void *d,void *s,int pixels);
void __cdecl mmx_convert_32_to_16_rgb565(void *d,void *s,int pixels);
void __cdecl mmx_convert_32_to_16_bgr565(void *d,void *s,int pixels);
void __cdecl mmx_convert_32_to_16_rgb555(void *d,void *s,int pixels);
void __cdecl mmx_convert_32_to_16_bgr555(void *d,void *s,int pixels);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif
