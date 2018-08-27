//-----------------------------------------------------------------------------
//	Copyright © 2005-2006 Mario Zechner
//
//	This software is provided 'as-is', without any express or implied warranty. 
//	In no event will the authors be held liable for any damages arising from 
//	the use of this software.
//	
//	Permission is granted to anyone to use this software for any purpose, 
//	including commercial applications, and to alter it and redistribute it 
//	freely, subject to the following restrictions:
//	
//	1. The origin of this software must not be misrepresented; you must not claim 
//	that you wrote the original software. If you use this software in a product, 
//	an acknowledgment in the product documentation would be appreciated but is 
//	not required.
//	
//	2. Altered source versions must be plainly marked as such, and must not be 
//	misrepresented as being the original software.
//	
//	3. This notice may not be removed or altered from any source 
//	distribution. 
//-----------------------------------------------------------------------------

#include "gfx/gfxsurface.h"

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

void Yagl::GfxSurface::convertRGB24toARGB32( unsigned char** data, unsigned int size )
{
	unsigned char* data24 = *data;
	unsigned int* data32 = new unsigned int[size / 3];
	
	for( unsigned int curr_pixel = 0; curr_pixel < size / 3; curr_pixel ++ )
	{					
		data32[curr_pixel] = 255 << 24 | data24[curr_pixel * 3 + 2] << 16 | data24[curr_pixel * 3 + 1] << 8 | data24[curr_pixel * 3];
	}
	
	delete[] data24;
	*data = (unsigned char*)data32;
}

void Yagl::GfxSurface::convertRGB16toARGB32( unsigned char** data, unsigned int size )
{
	unsigned short* data16 = (unsigned short*)*data;
	unsigned int* data32 = new unsigned int[size / 2];
	
	for( unsigned int curr_pixel = 0; curr_pixel < size / 2; curr_pixel ++ )
	{					
		data32[curr_pixel] = 255 << 24 | (unsigned int)( ( data16[curr_pixel] >> 11 ) << 3 ) << 16 | (unsigned int)( ( ( data16[curr_pixel] >> 5 ) & 64 ) << 2 ) << 8 | (unsigned int)( ( data16[curr_pixel] & 32 ) << 3 ) ;
	}
	
	delete[] data16;
	*data = (unsigned char*)data32;
}

void Yagl::GfxSurface::convertRGB24toRGB16( unsigned char**data, unsigned int size )
{
	unsigned char* data24 = (unsigned char*)*data;
	unsigned short* data16 = new unsigned short[size / 3];
	
	for( unsigned int curr_pixel = 0; curr_pixel < size / 3; curr_pixel ++ )
	{	
		data16[curr_pixel] = ( data24[curr_pixel * 3 + 2] >> 3 ) << 11 | ( data24[curr_pixel * 3 + 1] >> 2 ) << 5 | data24[curr_pixel * 3] >> 3;				
	}
	
	delete[] data24;
	*data = (unsigned char*)data16;				
}

void Yagl::GfxSurface::convertARGB32toRGB16( unsigned char**data, unsigned int size )
{
	unsigned int* data32 = (unsigned int*)*data;
	unsigned short* data16 = new unsigned short[size / 4];
	
	for( unsigned int curr_pixel = 0; curr_pixel < size / 3; curr_pixel ++ )
	{	
		data16[curr_pixel] = ( ( ( data32[curr_pixel] >> 16 ) & 255 ) >> 3 ) << 11  | ( ( ( data32[curr_pixel] >> 8 ) & 255 ) >> 2 ) << 5 | ( data32[curr_pixel] & 255 ) >> 3;				
	}
	
	delete[] data32;
	*data = (unsigned char*)data16;
}

void Yagl::GfxSurface::convertGRAYSCALE8toARGB32( unsigned char** data, unsigned int size )
{
	unsigned char* data8 = ( unsigned char*)*data;
	unsigned int *data32 = new unsigned int[ size ];
	
	for( unsigned int curr_pixel = 0; curr_pixel < size; curr_pixel ++ )
	{
		data32[curr_pixel] = 255 << 24 | data8[curr_pixel] << 16 | data8[curr_pixel] << 8 | data8[curr_pixel];
	}
	
	delete[] data8;
	*data = (unsigned char*)data32;
}

void Yagl::GfxSurface::convertColorKeyToAlphaChannel( unsigned char* data, unsigned int size, unsigned int colorkey )
{
	unsigned int *data32 = (unsigned int*)data;
	colorkey &= 0xffffff;

	for( unsigned int curr_pixel = 0; curr_pixel < size / 4; curr_pixel++ )
	{
		data32[curr_pixel] = ( ( data32[curr_pixel] & 0xffffff ) == colorkey )? data32[curr_pixel] & 0xffffff: data32[curr_pixel];
	}
}

void Yagl::GfxSurface::convertARGB32toABGR32( unsigned char* data, unsigned int size )
{
	unsigned int *data32 = (unsigned int*)data;
	for( unsigned int curr_pixel = 0; curr_pixel < size / 4; curr_pixel++ )
	{
		data32[curr_pixel] = ( data32[curr_pixel] & 0xff000000 ) |
							 ( data32[curr_pixel] & 0xff ) << 16 |
							 ( data32[curr_pixel] & 0xff00 ) |
							 ( data32[curr_pixel] & 0xff0000 ) >> 16;
	}
}
