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

#include "gfx/gfxdevice.h"

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

Yagl::ScreenModeInfo::ScreenModeInfo( int width, int height, int bit_depth )
{
	width_ = width;
	height_ = height;
	bpp_ = bit_depth;
}

Yagl::ScreenModeInfo::~ScreenModeInfo( )
{
	
}

int Yagl::ScreenModeInfo::getWidth( )
{
	return width_;
}

int Yagl::ScreenModeInfo::getHeight( )
{
	return height_;
}

int Yagl::ScreenModeInfo::getBitDepth( )
{
	return bpp_;
}

Yagl::GfxDevice::GfxDevice( ) 
{ 
	width_ = 0;
	height_ = 0;
	bitdepth_ = 0;
}

Yagl::GfxDevice::GfxDevice( const GfxDevice& device )
{
}

 Yagl::GfxDevice::~GfxDevice( )
{
}

void Yagl::GfxDevice::line( int x1, int y1, int x2, int y2, int color, float blend_factor )
{ };

void Yagl::GfxDevice::box( int x1, int y1, int x2, int y2, int color, float blend_factor )
{ };

void Yagl::GfxDevice::solidBox( int x1, int y1, int x2, int y2, int color, float blend_factor )
{ };

void Yagl::GfxDevice::blit( int x, int y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{ };	

void Yagl::GfxDevice::blit( int x, int y, int src_min_x, int src_min_y, int src_max_x, int src_max_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{ };

void Yagl::GfxDevice::blitScaled( int x, int y, float scale_x, float scale_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{ };	

void Yagl::GfxDevice::blitScaled( int x, int y, float scale_x, float scale_y, int src_min_x, int src_min_y, int src_max_x, int src_max_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{ };

void Yagl::GfxDevice::blitRotated( int x, int y, float angle, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{ };	

void Yagl::GfxDevice::blitRotated( int x, int y, float angle, int src_min_x, int src_min_y, int src_max_x, int src_max_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{ };

void Yagl::GfxDevice::blitRotatedScaled( int x, int y, float rotate, float scale_x, float scale_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{ };	

void Yagl::GfxDevice::blitRotatedScaled( int x, int y, float rotate, float scale_x, float scale_y, int src_min_x, int src_min_y, int src_max_x, int src_max_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{ };

void Yagl::GfxDevice::clear( int color )
{ };

void Yagl::GfxDevice::printAt( const Yagl::String& text, int x, int y, int color, GfxFont* font, float blend_factor )
{ };
