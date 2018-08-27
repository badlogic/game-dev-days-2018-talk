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

#include "yagl.h"
#include "yaglwrapper.h"

//-----------------------------------------------------------------------------
// graphics module
//-----------------------------------------------------------------------------
int YaglGfxDevice_setScreenMode( int width, int height, int bitdepth, int fullscreen )
{	
	if( Yagl::getGfxDevice().setScreenMode( width, height, bitdepth, fullscreen?true:false ) )
		return -1;
	else
		return 0;
}

int YaglGfxDevice_getScreenModeWidth( )
{
	return Yagl::getGfxDevice().getScreenModeWidth();
}

int YaglGfxDevice_getScreenModeHeight( )
{
	return Yagl::getGfxDevice().getScreenModeHeight();
}

void YaglGfxDevice_setWindowTitle( const char* title )
{
	if( title != 0 )
		Yagl::getGfxDevice().setWindowTitle( title );
}

void YaglGfxDevice_hideMouseCursor( )
{
	Yagl::getGfxDevice().hideMouseCursor();
}

void YaglGfxDevice_showMouseCursor( )
{
	Yagl::getGfxDevice().showMouseCursor();
}

int YaglGfxDevice_wasWindowCloseButtonPressed( )
{
	return ( Yagl::getGfxDevice().wasWindowCloseButtonPressed( )? -1: 0 );
}

void YaglGfxDevice_clear( int color )
{
	Yagl::getGfxDevice().clear( color );
}

void YaglGfxDevice_swapBuffers( )
{
	Yagl::getGfxDevice().swapBuffers();
}

void YaglGfxDevice_setClippingRegion( int clip_min_x, int clip_min_y, int clip_max_x, int clip_max_y )
{
	Yagl::getGfxDevice().setClippingRegion( clip_min_x, clip_min_y, clip_max_x, clip_max_y );
}

void YaglGfxDevice_getClippingRegion( int *clip_min_x, int *clip_min_y, int *clip_max_x, int *clip_max_y )
{
	int tmp = 0;
	
	if( !clip_min_x )
		clip_min_x = &tmp;
	if( !clip_max_x )
		clip_max_x = &tmp;
	if( !clip_min_y )
		clip_min_y = &tmp;
	if( !clip_max_y )
		clip_max_y = &tmp;
		
	Yagl::getGfxDevice().getClippingRegion( *clip_min_x, *clip_min_y, *clip_max_x, *clip_max_y );
}

YaglGfxSurface YaglGfxDevice_createSurface( )
{
	return reinterpret_cast<YaglGfxSurface>(Yagl::getGfxDevice().createSurface( ));
}

void YaglGfxDevice_destroySurface( YaglGfxSurface surface )
{
	Yagl::getGfxDevice().destroySurface( reinterpret_cast<Yagl::GfxSurface*>(surface) );
}

void YaglGfxDevice_destroyAllSurfaces( )
{
	Yagl::getGfxDevice().destroyAllSurfaces();
}

YaglGfxFont YaglGfxDevice_createFont( )
{
	return reinterpret_cast<YaglGfxFont>(Yagl::getGfxDevice().createFont( ));
}

void YaglGfxDevice_destroyFont( YaglGfxFont font )
{
	Yagl::getGfxDevice().destroyFont( reinterpret_cast<Yagl::GfxFont*>(font) );
}

void YaglGfxDevice_destroyAllFonts( )
{
	Yagl::getGfxDevice().destroyAllFonts();
}

void YaglGfxDevice_enableVSynch()
{
	Yagl::getGfxDevice().enableVSynch();
}

void YaglGfxDevice_disableVSynch()
{
	Yagl::getGfxDevice().disableVSynch();
}

int YaglGfxDevice_isVSynchEnabled()
{
	return Yagl::getGfxDevice().isVSynchEnabled()?-1:0;
}

void YaglGfxDevice_putPixel( int x, int y, unsigned int color )
{
	Yagl::getGfxDevice().putPixel( x, y, color );
}

unsigned int YaglGfxDevice_getPixel( int x, int y )
{
	return Yagl::getGfxDevice().getPixel( x, y );
}

void YaglGfxDevice_line( int x1, int y1, int x2, int y2, int color, float blendfactor )
{
	Yagl::getGfxDevice().line( x1, y1, x2, y2, color, blendfactor );
}

void YaglGfxDevice_box( int x1, int y1, int x2, int y2, int color, float blendfactor )
{
	Yagl::getGfxDevice().box( x1, y1, x2, y2, color, blendfactor );
}

void YaglGfxDevice_solidBox( int x1, int y1, int x2, int y2, int color, float blendfactor )
{
	Yagl::getGfxDevice().solidBox( x1, y1, x2, y2, color, blendfactor );
}

void YaglGfxDevice_blit( int x, int y, YaglGfxSurface surface, YAGL_BLIT_MODE mode, float blendfactor )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	Yagl::getGfxDevice().blit( x, y, surface_ptr, static_cast<Yagl::YAGL_BLIT_MODE>(mode), blendfactor );
}

void YaglGfxDevice_blitRanged( int x, int y, int src_min_x, int src_min_y, int src_max_x, int src_max_y, YaglGfxSurface surface, YAGL_BLIT_MODE mode, float blendfactor )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	Yagl::getGfxDevice().blit( x, y, src_min_x, src_min_y, src_max_x, src_max_y, surface_ptr, static_cast<Yagl::YAGL_BLIT_MODE>(mode), blendfactor );
}

void YaglGfxDevice_blitScaled( int x, int y, float scale_x, float scale_y, YaglGfxSurface surface, YAGL_BLIT_MODE mode, float blendfactor )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	Yagl::getGfxDevice().blitScaled( x, y, scale_x, scale_y, surface_ptr, static_cast<Yagl::YAGL_BLIT_MODE>(mode), blendfactor );
}

void YaglGfxDevice_blitScaledRanged( int x, int y, float scale_x, float scale_y, int src_min_x, int src_min_y, int src_max_x, int src_max_y, YaglGfxSurface surface, YAGL_BLIT_MODE mode, float blendfactor )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	Yagl::getGfxDevice().blitScaled( x, y, scale_x, scale_y, src_min_x, src_min_y, src_max_x, src_max_y, surface_ptr, static_cast<Yagl::YAGL_BLIT_MODE>(mode), blendfactor );
}

void YaglGfxDevice_blitRotated( int x, int y, float angle, YaglGfxSurface surface, YAGL_BLIT_MODE mode, float blendfactor )
{
Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	Yagl::getGfxDevice().blitRotated( x, y, angle, surface_ptr, static_cast<Yagl::YAGL_BLIT_MODE>(mode), blendfactor );
}

void YaglGfxDevice_blitRotatedRanged( int x, int y, float angle, int src_min_x, int src_min_y, int src_max_x, int src_max_y, YaglGfxSurface surface, YAGL_BLIT_MODE mode, float blendfactor )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	Yagl::getGfxDevice().blitRotated( x, y, angle, src_min_x, src_min_y, src_max_x, src_max_y, surface_ptr, static_cast<Yagl::YAGL_BLIT_MODE>(mode), blendfactor );
}

void YaglGfxDevice_blitRotatedScaled( int x, int y, float rotate, float scale_x, float scale_y, YaglGfxSurface surface, YAGL_BLIT_MODE mode, float blendfactor )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	Yagl::getGfxDevice().blitRotatedScaled( x, y, rotate, scale_x, scale_y, surface_ptr, static_cast<Yagl::YAGL_BLIT_MODE>(mode), blendfactor );
}

void YaglGfxDevice_blitRotatedScaledRanged( int x, int y, float rotate, float scale_x, float scale_y, int src_min_x, int src_min_y, int src_max_x, int src_max_y, YaglGfxSurface surface, YAGL_BLIT_MODE mode, float blendfactor )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	Yagl::getGfxDevice().blitRotatedScaled( x, y, rotate, scale_x, scale_y, src_min_x, src_min_y, src_max_x, src_max_y, surface_ptr, static_cast<Yagl::YAGL_BLIT_MODE>(mode), blendfactor );
}

void YaglGfxDevice_printAt( const char* text, int x, int y, int color, YaglGfxFont font, float blendfactor )
{
	if( text != 0 )
		Yagl::getGfxDevice().printAt( text, x, y, color, reinterpret_cast<Yagl::GfxFont*>(font), blendfactor );
}

void YaglGfxDevice_setTintingColor( unsigned int color )
{
	Yagl::getGfxDevice().setTintingColor( color );
}

unsigned int YaglGfxDevice_getTintingColor( )
{
	return Yagl::getGfxDevice().getTintingColor( );
}

void YaglGfxDevice_flushCommandBuffer( )
{
	Yagl::getGfxDevice().flushCommandBuffer( );
}

int	YaglGfxSurface_loadFile( YaglGfxSurface surface, const char* filename )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	if( surface_ptr && filename )
		return surface_ptr->loadFile( filename )?-1:0;
	else
		return 0;
}

int YaglGfxSurface_loadFromMemoryColorKeyed( YaglGfxSurface surface, const unsigned char* data, unsigned int width, unsigned int height, YAGL_BITMAP_FORMAT format, unsigned int colorkey  )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	if( surface_ptr && data )
		return ( surface_ptr->loadFromMemoryColorKeyed( data, width, height, static_cast<Yagl::YAGL_BITMAP_FORMAT>(format), colorkey ) )?-1:0;
	return -1;
}

int	YaglGfxSurface_loadFileColorKeyed( YaglGfxSurface surface, const char* filename, unsigned int colorkey )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	if( surface_ptr && filename )
		return surface_ptr->loadFileColorKeyed( filename, colorkey )?-1:0;
	else
		return 0;
}

int YaglGfxSurface_loadFromMemory( YaglGfxSurface surface, const unsigned char* data, unsigned int width, unsigned int height, YAGL_BITMAP_FORMAT format  )
{
	Yagl::GfxSurface* surface_ptr = reinterpret_cast<Yagl::GfxSurface*>(surface);
	if( surface_ptr && data )
		return ( surface_ptr->loadFromMemory( data, width, height, static_cast<Yagl::YAGL_BITMAP_FORMAT>(format) ) )?-1:0;
	return -1;
}

int YaglGfxSurface_getWidth( YaglGfxSurface surface )
{
	if( surface )
		return reinterpret_cast<Yagl::GfxSurface*>(surface)->getWidth();
	else
		return 0;
}

int YaglGfxSurface_getHeight( YaglGfxSurface surface )
{
	if( surface )
		return reinterpret_cast<Yagl::GfxSurface*>(surface)->getHeight();
	else
		return 0;
}

void YaglGfxSurface_get( YaglGfxSurface surface, YAGL_GFXSURFACE_ATTRIBUTE attribute, void *attribute_value )
{
	if( surface )
		reinterpret_cast<Yagl::GfxSurface*>(surface)->get( static_cast<Yagl::YAGL_GFXSURFACE_ATTRIBUTE>(attribute), attribute_value );
	else
		return;
}

void YaglGfxSurface_set( YaglGfxSurface surface, YAGL_GFXSURFACE_ATTRIBUTE attribute, void *attribute_value )
{
	if( surface )
		reinterpret_cast<Yagl::GfxSurface*>(surface)->set( static_cast<Yagl::YAGL_GFXSURFACE_ATTRIBUTE>(attribute), attribute_value );
	else
		return;
}

void YaglGfxSurface_getPixels( YaglGfxSurface surface, int x, int y, int x2, int y2, unsigned int *buffer, YAGL_BITMAP_FORMAT format )
{
	if( surface )
		reinterpret_cast<Yagl::GfxSurface*>(surface)->getPixels( x, y, x2, y2, buffer, static_cast<Yagl::YAGL_BITMAP_FORMAT>(format) );
	else
		return;
}

void YaglGfxSurface_putPixels( YaglGfxSurface surface, int x, int y, int width, int height, const unsigned int *buffer, YAGL_BITMAP_FORMAT format )
{
	if( surface )
		reinterpret_cast<Yagl::GfxSurface*>(surface)->putPixels( x, y, width, height, buffer, static_cast<Yagl::YAGL_BITMAP_FORMAT>(format) );
	else
		return;
}

int	YaglGfxFont_loadFont( YaglGfxFont font, const char* filename )
{
	Yagl::GfxFont* font_ptr = reinterpret_cast<Yagl::GfxFont*>(font);
	if( font_ptr && filename )
		return font_ptr->loadFont( filename )?-1:0;
	else
		return 0;
}

void YaglGfxFont_setSize( YaglGfxFont font, int size )
{
	Yagl::GfxFont* font_ptr = reinterpret_cast<Yagl::GfxFont*>(font);
	if( font_ptr )
		font_ptr->setSize( size );
}

void YaglGfxFont_getTextDimensions( YaglGfxFont font, const char* text, unsigned int *width, unsigned *height )
{
	Yagl::GfxFont* font_ptr = reinterpret_cast<Yagl::GfxFont*>(font);
	unsigned tmp_width = 0, tmp_height = 0;
	
	if( font_ptr )
		font_ptr->getTextDimensions( text, tmp_width, tmp_height );
		
	if( width )
		*width = tmp_width;
		
	if( height )
		*height = tmp_height;
}

// added - [coderjeff 2006-feb-6]
int YaglGfxFont_getAdvance( YaglGfxFont font, unsigned char asc )
{
	Yagl::GfxFont* font_ptr = reinterpret_cast<Yagl::GfxFont*>(font);
	if( font_ptr )
		return(font_ptr->getAdvance( asc ));
	return(0);
}

// added - [coderjeff 2006-feb-6]
int YaglGfxFont_getLeft( YaglGfxFont font, unsigned char asc )
{
	Yagl::GfxFont* font_ptr = reinterpret_cast<Yagl::GfxFont*>(font);
	if( font_ptr )
		return(font_ptr->getLeft( asc ));
	return(0);
}

//-----------------------------------------------------------------------------
// timer module
//-----------------------------------------------------------------------------
// added - [coderjeff 2006-feb-13]
double YaglTimer_getSecondsSinceStart( )
{
	return Yagl::Timer::getSecondsSinceStart( );
}

//-----------------------------------------------------------------------------
// input module
//-----------------------------------------------------------------------------
int YaglKeyboard_isKeyPressed( int scancode )
{
	if( Yagl::getKeyboard().isKeyPressed( scancode ) )
		return -1;
	else
		return 0;
}

int YaglKeyboard_getKey( )
{
	return Yagl::getKeyboard().getKey( );
}

void YaglMouse_setPosition( int x, int y )
{
	Yagl::getMouse().setPosition( x, y );
}

int YaglMouse_getX( )
{
	return Yagl::getMouse().getX();
}

int YaglMouse_getY( )
{
	return Yagl::getMouse().getY();
}

int YaglMouse_getZ( )
{
	return Yagl::getMouse().getZ();
}

int YaglMouse_isLeftButtonPressed( )
{
	return Yagl::getMouse().isLeftButtonPressed();
}

int YaglMouse_isRightButtonPressed( )
{
	return Yagl::getMouse().isRightButtonPressed();
}

int YaglMouse_isMiddleButtonPressed( )
{
	return Yagl::getMouse().isMiddleButtonPressed();
}

int YaglJoystick_isConnected( unsigned int id )
{
	return Yagl::Joystick::isConnected( id );
}

float YaglJoystick_getX( unsigned int id )
{
	return Yagl::Joystick::getX( id );
}

float YaglJoystick_getY( unsigned int id )
{
	return Yagl::Joystick::getY( id );
}

float YaglJoystick_getZ( unsigned int id )
{
	return Yagl::Joystick::getZ( id );
}

float YaglJoystick_getR( unsigned int id )
{
	return Yagl::Joystick::getR( id );
}

float YaglJoystick_getU( unsigned int id )
{
	return Yagl::Joystick::getU( id );
}

float YaglJoystick_getV( unsigned int id )
{
	return Yagl::Joystick::getV( id );	
}

int YaglJoystick_getButtons( unsigned id )
{
	return Yagl::Joystick::getButtons( id );
}

//-----------------------------------------------------------------------------
// sound module
//-----------------------------------------------------------------------------
int YaglSfxDevice_initialize( )
{
	return Yagl::getSfxDevice().initialize( )?-1:0;	
}

void YaglSfxDevice_deinitialize( )
{
	Yagl::getSfxDevice().deinitialize();
}

void YaglSfxDevice_setListenerPosition( float x, float y, float z )
{
	Yagl::getSfxDevice().setListenerPosition( x, y, z );
}

void YaglSfxDevice_getListenerPosition( float *x, float *y, float *z )
{
	Yagl::getSfxDevice().getListenerPosition( x, y, z );
}

void YaglSfxDevice_setListenerVelocity( float x, float y, float z )
{
	Yagl::getSfxDevice().setListenerVelocity( x, y, z );
}

void YaglSfxDevice_getListenerVelocity( float *x, float *y, float *z )
{
	Yagl::getSfxDevice().getListenerPosition( x, y, z );
}

void YaglSfxDevice_setListenerOrientation( float at_x, float at_y, float at_z, float up_x, float up_y, float up_z )
{
	Yagl::getSfxDevice().setListenerOrientation( at_x, at_y, at_z, up_x, up_y, up_z );
}

void YaglSfxDevice_getListenerOrientation( float *at_x, float *at_y, float *at_z, float *up_x, float *up_y, float *up_z )
{
	Yagl::getSfxDevice().getListenerOrientation( at_x, at_y, at_z, up_x, up_y, up_z );
}

int  YaglSfxDevice_createSoundBufferFromFile( const char* filename, unsigned int *handle )
{
	if( filename )
		return Yagl::getSfxDevice().createSoundBufferFromFile( filename, handle );
	else
		return 0;
}

void YaglSfxDevice_destroySoundBuffer( unsigned int handle )
{
	Yagl::getSfxDevice().destroySoundBuffer( handle );
}

int  YaglSfxDevice_playSoundBuffer( unsigned int buffer_handle, int relative, 
									float *position, float *velocity, 
									float pitch, float gain, float roll_off )
{
	return Yagl::getSfxDevice().playSoundBuffer( buffer_handle, relative?true:false, position, velocity, pitch, gain, roll_off )?-1:0;
}

void YaglSfxDevice_destroyAllSoundBuffers( )
{
	Yagl::getSfxDevice().destroyAllSoundBuffers( );
}

YaglSoundSource  YaglSfxDevice_createSoundSource( unsigned int buffer_handle )
{
	return reinterpret_cast<YaglSoundSource>(Yagl::getSfxDevice().createSoundSource( buffer_handle ));	
}

void YaglSfxDevice_destroySoundSource( YaglSoundSource source )
{
	Yagl::getSfxDevice().destroySoundSource( reinterpret_cast<Yagl::SoundSource*>(source) );
}

void YaglSfxDevice_destroyAllSoundSources( )
{
	Yagl::getSfxDevice().destroyAllSoundSources( );
}

YaglSoundStream  YaglSfxDevice_createSoundStream( const char* filename )
{
	if( filename )
		return reinterpret_cast<YaglSoundStream>(Yagl::getSfxDevice().createSoundStream( filename ));	
	else
		return 0;
}

void YaglSfxDevice_destroySoundStream( YaglSoundStream stream )
{
	Yagl::getSfxDevice().destroySoundStream( reinterpret_cast<Yagl::SoundStream*>(stream) );
}

void YaglSfxDevice_destroyAllSoundStreams( )
{
	Yagl::getSfxDevice().destroyAllSoundStreams( );
}


int  YaglSoundSource_setSoundBuffer( YaglSoundSource source, unsigned int buffer_handle )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		return source_ptr->setSoundBuffer( buffer_handle )?-1:0;
	else
		return 0;
}

void YaglSoundSource_setRelativeToListener( YaglSoundSource source, int relative )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		source_ptr->setRelativeToListener( relative?true:false );
}

void YaglSoundSource_setVelocity( YaglSoundSource source, float x, float y, float z )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		source_ptr->setVelocity( x, y, z );
}

void YaglSoundSource_setPosition( YaglSoundSource source, float x, float y, float z )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		source_ptr->setPosition( x, y, z );
}

void YaglSoundSource_setGain( YaglSoundSource source, float gain )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		source_ptr->setGain( gain );
}

void YaglSoundSource_setPitch( YaglSoundSource source, float pitch )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		source_ptr->setPitch( pitch );
}

void YaglSoundSource_setRollOff( YaglSoundSource source, float roll_off )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		source_ptr->setRollOff( roll_off );
}

int  YaglSoundSource_isRelativeToListener( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		return source_ptr->isRelativeToListener( )?-1:0;
	else
		return false;
}

void YaglSoundSource_getVelocity( YaglSoundSource source, float *x, float *y, float *z )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr && x && y && z )
		source_ptr->getVelocity( x, y, z );
}

void YaglSoundSource_getPosition( YaglSoundSource source, float *x, float *y, float *z )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr && x && y && z )
		source_ptr->getPosition( x, y, z );
}

float YaglSoundSource_getGain( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		return source_ptr->getGain();
	else
		return 0.0f;
}

float YaglSoundSource_getPitch( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		return source_ptr->getPitch();
	else
		return 0.0f;
}

float YaglSoundSource_getRollOff( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		return source_ptr->getPitch();
	else
		return 0.0f;
}

void YaglSoundSource_play( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		source_ptr->play();	
}

void YaglSoundSource_playLooped( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		source_ptr->playLooped();	
}

void YaglSoundSource_pause( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		source_ptr->pause();	
}

void YaglSoundSource_stop( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		source_ptr->stop();	
}

int YaglSoundSource_isPlaying( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		return source_ptr->isPlaying( )?-1:0;	
	else
		return 0;
}

int YaglSoundSource_isPaused( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		return source_ptr->isPaused( )?-1:0;	
	else
		return 0;
}

int YaglSoundSource_isStopped( YaglSoundSource source )
{
	Yagl::SoundSource* source_ptr = reinterpret_cast<Yagl::SoundSource*>(source);
	if( source_ptr )
		return source_ptr->isStopped( )?-1:0;	
	else
		return 0;
}

//-----------------------------------------------------------------------------
int  YaglSoundStream_setStreamedFile( YaglSoundStream stream, unsigned char* filename )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr && filename )
		return stream_ptr->setStreamedFile( reinterpret_cast<char*>(filename) )?-1:0;
	else
		return 0;
}

void YaglSoundStream_setRelativeToListener( YaglSoundStream stream, int relative )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		stream_ptr->setRelativeToListener( relative?true:false );
}

void YaglSoundStream_setVelocity( YaglSoundStream stream, float x, float y, float z )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		stream_ptr->setVelocity( x, y, z );
}

void YaglSoundStream_setPosition( YaglSoundStream stream, float x, float y, float z )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		stream_ptr->setPosition( x, y, z );
}

void YaglSoundStream_setGain( YaglSoundStream stream, float gain )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		stream_ptr->setGain( gain );
}

void YaglSoundStream_setPitch( YaglSoundStream stream, float pitch )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		stream_ptr->setPitch( pitch );
}

void YaglSoundStream_setRollOff( YaglSoundStream stream, float roll_off )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		stream_ptr->setRollOff( roll_off );
}

int  YaglSoundStream_isRelativeToListener( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		return stream_ptr->isRelativeToListener( )?-1:0;
	else
		return false;
}

void YaglSoundStream_getVelocity( YaglSoundStream stream, float *x, float *y, float *z )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr && x && y && z )
		stream_ptr->getVelocity( x, y, z );
}

void YaglSoundStream_getPosition( YaglSoundStream stream, float *x, float *y, float *z )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr && x && y && z )
		stream_ptr->getPosition( x, y, z );
}

float YaglSoundStream_getGain( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		return stream_ptr->getGain();
	else
		return 0.0f;
}

float YaglSoundStream_getPitch( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		return stream_ptr->getPitch();
	else
		return 0.0f;
}

float YaglSoundStream_getRollOff( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		return stream_ptr->getPitch();
	else
		return 0.0f;
}

void YaglSoundStream_play( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		stream_ptr->play();	
}

void YaglSoundStream_playLooped( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		stream_ptr->playLooped();	
}

void YaglSoundStream_pause( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		stream_ptr->pause();	
}

void YaglSoundStream_stop( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		stream_ptr->stop();	
}

int YaglSoundStream_isPlaying( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		return stream_ptr->isPlaying( )?-1:0;	
	else
		return 0;
}

int YaglSoundStream_isPaused( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		return stream_ptr->isPaused( )?-1:0;	
	else
		return 0;
}

int YaglSoundStream_isStopped( YaglSoundStream stream )
{
	Yagl::SoundStream* stream_ptr = reinterpret_cast<Yagl::SoundStream*>(stream);
	if( stream_ptr )
		return stream_ptr->isStopped( )?-1:0;	
	else
		return 0;
}
