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

#include "gfx/glgfxsurface.h"
#include "gfx/gfxenums.h"
#include "decoder/decoder.h"
#include "log/log.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glfw.h>
#include <GL/glext.h>

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

Yagl::GlGfxSurface::GlGfxSurface( )
{
	width_ = 0;
	height_ = 0;
	bitdepth_ = 0;
	texture_handle_ = 0;
	is_valid_ = false;
}

Yagl::GlGfxSurface::GlGfxSurface( const GlGfxSurface& surface )
{
}

Yagl::GlGfxSurface::~GlGfxSurface( )
{
	destroy();
}

unsigned int Yagl::GlGfxSurface::getTextureHandle( )
{
	return texture_handle_;
}
						
bool Yagl::GlGfxSurface::create( int width, int height, int bitdepth )
{
	unsigned int res = glGetError();

	glGenTextures( 1, &texture_handle_ );
	if( ( res = glGetError() ) != GL_NO_ERROR )
	{
		Yagl::Log::print( "GlGfxSurface: couldn't create texture handle, error code" );
		Yagl::Log::print( (int)res );
		Yagl::Log::print( "\n" );
		texture_handle_ = 0;
		is_valid_ = false;
		return false;
	}
	
	is_valid_ = true;
	return true;
}

void Yagl::GlGfxSurface::destroy( )
{
	if( is_valid_ )
		glDeleteTextures( 1, &texture_handle_ );
	width_ = 0;
	height_ = 0;
	bitdepth_ = 0;
	texture_handle_ = 0;
	is_valid_ = false;
}

bool Yagl::GlGfxSurface::loadFile( const Yagl::String& filename )
{
	Decoder *decoder;
	int read_bytes;
	int size;
	char *bitmap = 0;		
	
	destroy();
	if( !create( 0, 0, 0 ) )
		return false;
	
	decoder = Decoder::getDecoder( filename.c_str() );	
	
	if ( !decoder ) {
		Yagl::Log::print( "GlGfxSurface:: couldn't load GlGfxSurface at " );
		Yagl::Log::print( this );
		Yagl::Log::print( " from file " );
		Yagl::Log::print( filename );
		Yagl::Log::print( " because no proper decoder could be found\n" );
		return false;
	}	
	
	width_ = decoder->getSourceInfo().width_;
	height_ = decoder->getSourceInfo().height_;
	texture_width_ = width_;
	texture_height_ = height_;
	bitdepth_ = decoder->getSourceInfo().bitsperpixel_;		
	
	size = width_ * height_ * bitdepth_ / 8;
	bitmap = new char[size];
	
	if ( decoder->decode( bitmap, size, &read_bytes ) == false) {
		Yagl::Log::print( "GlGfxSurface:: couldn't load GlGfxSurface at " );
		Yagl::Log::print( this );		
		Yagl::Log::print( " because an error occured while decoding\n" );
		
		destroy();
		delete[] bitmap;
		delete decoder;
		return false;
	}	
	
	if( bitdepth_ == 8 )
	{
		convertGRAYSCALE8toARGB32( (unsigned char**)&bitmap, width_ * height_ );
	}
	if( bitdepth_ == 16 )
	{
		convertRGB16toARGB32( (unsigned char**)&bitmap, width_ * height_ * 2 );
	}
	if( bitdepth_ == 24 )
	{
		convertRGB24toARGB32( (unsigned char**)&bitmap, width_ * height_ * 3 );
	}	
	bitdepth_ = 32;	
	
	convertARGB32toABGR32( (unsigned char*)bitmap, width_ * height_ * 4 );	
	
	bitmap = fixSize( bitmap, (unsigned int&)texture_width_, (unsigned int&)texture_height_ );	
	
	glBindTexture ( GL_TEXTURE_2D, texture_handle_ );
	gluBuild2DMipmaps( GL_TEXTURE_2D,
                      4,
                      texture_width_,
                      texture_height_,
                      GL_RGBA,
                      GL_UNSIGNED_BYTE,
                      bitmap);	

	delete[] bitmap;
	delete decoder;
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	/*glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );*/

	

	float priority = 1.0f;
	glPrioritizeTextures( 1, &texture_handle_, &priority );
	int result = glGetError();
	if( result == GL_INVALID_VALUE || result == GL_INVALID_OPERATION )
	{
		Yagl::Log::print( "GfxSurface: couldn't prioritize texture for surface at " );
		Yagl::Log::print( this );
		Yagl::Log::print( "\n" );
	}
	
	if( glIsTexture( texture_handle_ ) == GL_FALSE )
	{
		destroy();		
		glBindTexture( GL_TEXTURE_2D, 0 );
		return false;
	}
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	glGetError( );
	is_valid_ = true;
	return true;
}

bool Yagl::GlGfxSurface::loadFileColorKeyed( const Yagl::String& filename, unsigned int colorkey )
{
		Decoder *decoder;
	int read_bytes;
	int size;
	char *bitmap = 0;		
	
	destroy();
	if( !create( 0, 0, 0 ) )
		return false;
	
	decoder = Decoder::getDecoder( filename.c_str() );	
	
	if ( !decoder ) {
		Yagl::Log::print( "GlGfxSurface:: couldn't load GlGfxSurface at " );
		Yagl::Log::print( this );
		Yagl::Log::print( " from file " );
		Yagl::Log::print( filename );
		Yagl::Log::print( " because no proper decoder could be found\n" );
		return false;
	}	
	
	width_ = decoder->getSourceInfo().width_;
	height_ = decoder->getSourceInfo().height_;
	texture_width_ = width_;
	texture_height_ = height_;
	bitdepth_ = decoder->getSourceInfo().bitsperpixel_;		
	
	size = width_ * height_ * bitdepth_ / 8;
	bitmap = new char[size];
	
	if ( decoder->decode( bitmap, size, &read_bytes ) == false) {
		Yagl::Log::print( "GlGfxSurface:: couldn't load GlGfxSurface at " );
		Yagl::Log::print( this );		
		Yagl::Log::print( " because an error occured while decoding\n" );
		
		destroy();
		delete[] bitmap;
		delete decoder;
		return false;
	}	
	
	if( bitdepth_ == 8 )
	{
		convertGRAYSCALE8toARGB32( (unsigned char**)&bitmap, width_ * height_ );
	}
	if( bitdepth_ == 16 )
	{
		convertRGB16toARGB32( (unsigned char**)&bitmap, width_ * height_ * 2 );
	}
	if( bitdepth_ == 24 )
	{
		convertRGB24toARGB32( (unsigned char**)&bitmap, width_ * height_ * 3 );
	}	
	bitdepth_ = 32;	
	
	convertARGB32toABGR32( (unsigned char*)bitmap, width_ * height_ * 4 );
	convertColorKeyToAlphaChannel( (unsigned char*)bitmap, width_ * height_ * 4, colorkey );
	
	bitmap = fixSize( bitmap, (unsigned int&)texture_width_, (unsigned int&)texture_height_ );	
	
	glBindTexture ( GL_TEXTURE_2D, texture_handle_ );
	gluBuild2DMipmaps( GL_TEXTURE_2D,
                      4,
                      texture_width_,
                      texture_height_,
                      GL_RGBA,
                      GL_UNSIGNED_BYTE,
                      bitmap);	

	delete[] bitmap;
	delete decoder;
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	/*glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );*/

	

	float priority = 1.0f;
	glPrioritizeTextures( 1, &texture_handle_, &priority );
	int result = glGetError();
	if( result == GL_INVALID_VALUE || result == GL_INVALID_OPERATION )
	{
		Yagl::Log::print( "GfxSurface: couldn't prioritize texture for surface at " );
		Yagl::Log::print( this );
		Yagl::Log::print( "\n" );
	}
	
	if( glIsTexture( texture_handle_ ) == GL_FALSE )
	{
		destroy();		
		glBindTexture( GL_TEXTURE_2D, 0 );
		return false;
	}
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	glGetError( );
	is_valid_ = true;
	return true;
}

bool Yagl::GlGfxSurface::loadFromMemory( const unsigned char* data, unsigned int width, unsigned int height, YAGL_BITMAP_FORMAT format )
{	
	int size;
	char *bitmap = 0;
	
	destroy();
	if( !create( 0, 0, 0 ) )
		return false;
	
	width_ = width;
	height_ = height;
	texture_width_ = width_;
	texture_height_ = height_;
	
	switch( format )
	{
		case BITMAP_FORMAT_GRAYSCALE8:
			bitdepth_ = 8;
			break;
		case BITMAP_FORMAT_RGB16:
			bitdepth_ = 16;
			break;
		case BITMAP_FORMAT_RGB24:
			bitdepth_ = 24;
			break;
		case BITMAP_FORMAT_ARGB32:
			bitdepth_ = 32;
			break;
		case BITMAP_FORMAT_ABGR32:
			bitdepth_ = 32;
			break;
	}		
	
	size = width_ * height_ * bitdepth_ / 8;
	bitmap = new char[size];
	
	memcpy( bitmap, data, size );
	
	if( bitdepth_ == 8 )
	{
		convertGRAYSCALE8toARGB32( (unsigned char**)&bitmap, width_ * height_ );
	}
	if( bitdepth_ == 16 )
	{
		convertRGB16toARGB32( (unsigned char**)&bitmap, width_ * height_ * 2 );
	}
	if( bitdepth_ == 24 )
	{
		convertRGB24toARGB32( (unsigned char**)&bitmap, width_ * height_ * 3 );
	}	
	bitdepth_ = 32;	
	
	if( format != BITMAP_FORMAT_ABGR32 )
		convertARGB32toABGR32( (unsigned char*)bitmap, width_ * height_ * 4 );	
	
	bitmap = fixSize( bitmap, (unsigned int&)texture_width_, (unsigned int&)texture_height_ );	
	
	glBindTexture ( GL_TEXTURE_2D, texture_handle_ );
	gluBuild2DMipmaps( GL_TEXTURE_2D,
                      4,
                      texture_width_,
                      texture_height_,
                      GL_RGBA,
                      GL_UNSIGNED_BYTE,
                      bitmap);
	
	delete[] bitmap;	
	if( glGetError( ) != GL_NO_ERROR )
	{
		destroy();
		Yagl::Log::print( "GfxSurface: couldn't create Texture from bitmap with size " );
		Yagl::Log::print( (int)texture_width_ );
		Yagl::Log::print( ", " );
		Yagl::Log::print( (int)texture_height_ );
		Yagl::Log::print( "\n" );
		return false;
	}
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	/*glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );*/
	
	float priority = 1.0f;
	glPrioritizeTextures( 1, &texture_handle_, &priority );
	int result = glGetError();
	if( result == GL_INVALID_VALUE || result == GL_INVALID_OPERATION )
	{
		Yagl::Log::print( "GfxSurface: couldn't prioritize texture for surface at " );
		Yagl::Log::print( this );
		Yagl::Log::print( "\n" );
	}
	
	if( glIsTexture( texture_handle_ ) == GL_FALSE )
	{
		destroy();
		glBindTexture( GL_TEXTURE_2D, 0 );
		return false;
	}	
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	glGetError( );	
	is_valid_ = true;
	return true;
};

bool Yagl::GlGfxSurface::loadFromMemoryColorKeyed( const unsigned char* data, unsigned int width, unsigned int height, YAGL_BITMAP_FORMAT format, unsigned int colorkey )
{	
	int size;
	char *bitmap = 0;
	
	destroy();
	if( !create( 0, 0, 0 ) )
		return false;
	
	width_ = width;
	height_ = height;
	texture_width_ = width_;
	texture_height_ = height_;
	
	switch( format )
	{
		case BITMAP_FORMAT_GRAYSCALE8:
			bitdepth_ = 8;
			break;
		case BITMAP_FORMAT_RGB16:
			bitdepth_ = 16;
			break;
		case BITMAP_FORMAT_RGB24:
			bitdepth_ = 24;
			break;
		case BITMAP_FORMAT_ARGB32:
			bitdepth_ = 32;
			break;
		case BITMAP_FORMAT_ABGR32:
			bitdepth_ = 32;
			break;
	}		
	
	size = width_ * height_ * bitdepth_ / 8;
	bitmap = new char[size];
	
	memcpy( bitmap, data, size );
	
	if( bitdepth_ == 8 )
	{
		convertGRAYSCALE8toARGB32( (unsigned char**)&bitmap, width_ * height_ );
	}
	if( bitdepth_ == 16 )
	{
		convertRGB16toARGB32( (unsigned char**)&bitmap, width_ * height_ * 2 );
	}
	if( bitdepth_ == 24 )
	{
		convertRGB24toARGB32( (unsigned char**)&bitmap, width_ * height_ * 3 );
	}	
	bitdepth_ = 32;	
	
	if( format != BITMAP_FORMAT_ABGR32 )
		convertARGB32toABGR32( (unsigned char*)bitmap, width_ * height_ * 4 );
	convertColorKeyToAlphaChannel( (unsigned char*)bitmap, width_ * height_ * 4, colorkey & 0xffffff );
	
	bitmap = fixSize( bitmap, (unsigned int&)texture_width_, (unsigned int&)texture_height_ );	
	
	glBindTexture ( GL_TEXTURE_2D, texture_handle_ );
	gluBuild2DMipmaps( GL_TEXTURE_2D,
                      4,
                      texture_width_,
                      texture_height_,
                      GL_RGBA,
                      GL_UNSIGNED_BYTE,
                      bitmap);
	
	delete[] bitmap;	
	if( glGetError( ) != GL_NO_ERROR )
	{
		destroy();
		Yagl::Log::print( "GfxSurface: couldn't create Texture from bitmap with size " );
		Yagl::Log::print( (int)texture_width_ );
		Yagl::Log::print( ", " );
		Yagl::Log::print( (int)texture_height_ );
		Yagl::Log::print( "\n" );
		return false;
	}
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	/*glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );*/
	
	float priority = 1.0f;
	glPrioritizeTextures( 1, &texture_handle_, &priority );
	int result = glGetError();
	if( result == GL_INVALID_VALUE || result == GL_INVALID_OPERATION )
	{
		Yagl::Log::print( "GfxSurface: couldn't prioritize texture for surface at " );
		Yagl::Log::print( this );
		Yagl::Log::print( "\n" );
	}
	
	if( glIsTexture( texture_handle_ ) == GL_FALSE )
	{
		destroy();
		glBindTexture( GL_TEXTURE_2D, 0 );
		return false;
	}	
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	glGetError( );	
	is_valid_ = true;
	return true;
};

bool Yagl::GlGfxSurface::isValid( )
{
	return is_valid_;
}


void Yagl::GlGfxSurface::get( YAGL_GFXSURFACE_ATTRIBUTE attribute, void* attribute_value )
{
	if( attribute_value == 0 )
		return;

	switch( attribute )
	{
		case GFXSURFACE_GLTEXTURE_HANDLE:
			*((unsigned int*)attribute_value) = texture_handle_;
			break;
		case GFXSURFACE_GLTEXTURE_WIDTH:
			*((unsigned int*)attribute_value) = texture_width_;
			break;
		case GFXSURFACE_GLTEXTURE_HEIGHT:
			*((unsigned int*)attribute_value) = texture_height_;
			break;
		default:
			break;
	}
}

void Yagl::GlGfxSurface::set( YAGL_GFXSURFACE_ATTRIBUTE attribute, void* attribute_value )
{
}


bool Yagl::GlGfxSurface::isMethodImplemented( YAGL_GFX_CAPS method )
{	
	return false;
}

bool Yagl::GlGfxSurface::isBlitterImplemented( YAGL_BLIT_MODE mode , YAGL_BLIT_METHOD method )
{
	return false;
}


char* Yagl::GlGfxSurface::fixSize( char* data, unsigned int &width, unsigned int &height )
{
	unsigned int new_width, new_height;
	char* new_data;
	char* final_data;
	char* old_data = data;

	if( nextPowerOf2( width ) == width && nextPowerOf2( height ) == height ) 
		return data;
		
	new_width = nextPowerOf2( width );
	new_height = nextPowerOf2( height );
	new_data = new char[ new_width * new_height * 4 ];
	final_data = new_data;

	for( unsigned int y = 0; y < height; y++ )
	{
		memcpy( new_data, data, width * 4 );
		new_data += new_width * 4;
		data += width * 4;
	}

	//
	// OpenGL sucks. please refere to the bottom of this 
	// file for an explanation of the following
	//	
	/*int *new_data_32 = reinterpret_cast<int*>(final_data);
	for( int y = 0; y < new_height; y++ )
		for( int x = 0; x < new_width; x++ )
		{
			if( x > width - 1 )
			{
				new_data_32[ x + y * new_width ] = new_data_32[ x - 1 + y * new_width ];
			}		
			
			if( y > height - 1 )
			{
				new_data_32[ x + y * new_width ] = new_data_32[ x + ( y - 1 ) * new_width ];
			}

			if( x == width && y == height )
			{
				new_data_32[ x + y * new_width ] = new_data_32[ x - 1 + ( y - 1 ) * new_width ];
			}
		}*/


	delete[] old_data;
	
	width = new_width;
	height = new_height;
	return final_data;
}

void Yagl::GlGfxSurface::fixColors( char* data, unsigned int width, unsigned int height )
{
	unsigned int *data_4byte = (unsigned int*)data;

	for( unsigned int curr_pixel = 0; curr_pixel < width * height; curr_pixel++ )
	{	
		data_4byte[curr_pixel] = ( ( data_4byte[curr_pixel] & (unsigned int)0xff00ff ) == 0xff00ff )? data_4byte[curr_pixel] = 0x00000000: data_4byte[curr_pixel] = data_4byte[curr_pixel];
	}
}

unsigned int Yagl::GlGfxSurface::nextPowerOf2( unsigned int number )
{
	unsigned int power = 0x8000000;

	//
	// FIXME this is a hack
	//
	if( number == 1 )
		return 2;
	
	do
	{
		power >>= 1;
	}
	while( number <= power );
	
	if( number == power )
		return number;
	else
	{
		power <<= 1;
		return power;
	}
}

void Yagl::GlGfxSurface::getTextureCoordinates( int x, int y, double &u, double &v )
{
	u = (double)x / ((double)(texture_width_) );
	v = (double)y / ((double)(texture_height_) );
}



void Yagl::GlGfxSurface::getPixels( int x, int y, int x2, int y2, unsigned int* buffer, Yagl::YAGL_BITMAP_FORMAT format )
{
	unsigned int *bitmap = 0;

	if( !isValid() )
		return;

	if( buffer == 0 )
		return;

	if( x < 0 || x > texture_width_ - 1 )
		return;

	if( y < 0 || y > texture_height_ - 1 )
		return;

	if( x2 < 0 || x2 > texture_width_ - 1 )
		return;

	if( y2 < 0 || y2 > texture_height_ - 1 )
		return;

	if( x > x2 )
	{
		int tmp = x2;
		x2 = x;
		x = tmp;
	}

	if( y > y2 )
	{
		int tmp = y2;
		y2 = y;
		y = tmp;
	}

	bitmap = new unsigned int[ texture_width_ * texture_height_ ];

	//if( format != Yagl::BITMAP_FORMAT_ABGR32 && format != Yagl::BITMAP_FORMAT_ARGB32 )
	//	return;

	glPushAttrib( GL_PIXEL_MODE_BIT | GL_CURRENT_BIT | GL_VIEWPORT_BIT );	
	glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT );

	glPixelStorei( GL_UNPACK_SWAP_BYTES, 0 );
	glPixelStorei( GL_UNPACK_LSB_FIRST, 0 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
	glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

	glPixelTransferi( GL_MAP_COLOR, 0 );		
	glPixelTransferf( GL_RED_SCALE, 1.0f );
	glPixelTransferf( GL_GREEN_SCALE, 1.0f );
	glPixelTransferf( GL_BLUE_SCALE, 1.0f );
	glPixelTransferf( GL_ALPHA_SCALE, 1.0f );	
	glPixelTransferf( GL_RED_BIAS, 0.0f );
	glPixelTransferf( GL_GREEN_BIAS, 0.0f );
	glPixelTransferf( GL_BLUE_BIAS, 0.0f );
	glPixelTransferf( GL_ALPHA_BIAS, 0.0f );


	glBindTexture( GL_TEXTURE_2D, texture_handle_ );
	glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap );
	glBindTexture( GL_TEXTURE_2D, 0 );	

	glPopClientAttrib();
	glPopAttrib();

	unsigned int bmp_y = 0;
	unsigned int width = x2 - x + 1;
	unsigned int height = y2 - y + 1;
	for( ; y <= y2; y++, bmp_y++ )
	{			
		memcpy( &buffer[ bmp_y * width  ], &bitmap[ x + y * texture_width_ ], sizeof( unsigned int ) * width );
	}

	delete[] bitmap;

	if( format == Yagl::BITMAP_FORMAT_ARGB32 )
		convertARGB32toABGR32( (unsigned char*)buffer, width * height * 4 );
}

void Yagl::GlGfxSurface::putPixels( int x, int y, int width, int height, const unsigned int* buffer, Yagl::YAGL_BITMAP_FORMAT format )
{
	unsigned int *bitmap = 0;

	if( !isValid() )
		return;

	if( buffer == 0 )
		return;

	if( format != Yagl::BITMAP_FORMAT_ABGR32 && format != Yagl::BITMAP_FORMAT_ARGB32 )
		return;

	if( x < 0 || x > width_ - 1 )
		return;

	if( y < 0 || x > height_ - 1 )
		return;

	if( x + width > width_ )
		return;

	if( y + height > height_ )
		return;

	glPushAttrib( GL_PIXEL_MODE_BIT | GL_CURRENT_BIT | GL_VIEWPORT_BIT );	
	glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT );

	glPixelStorei( GL_UNPACK_SWAP_BYTES, 0 );
	glPixelStorei( GL_UNPACK_LSB_FIRST, 0 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
	glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

	glPixelTransferi( GL_MAP_COLOR, 0 );		
	glPixelTransferf( GL_RED_SCALE, 1.0f );
	glPixelTransferf( GL_GREEN_SCALE, 1.0f );
	glPixelTransferf( GL_BLUE_SCALE, 1.0f );
	glPixelTransferf( GL_ALPHA_SCALE, 1.0f );	
	glPixelTransferf( GL_RED_BIAS, 0.0f );
	glPixelTransferf( GL_GREEN_BIAS, 0.0f );
	glPixelTransferf( GL_BLUE_BIAS, 0.0f );
	glPixelTransferf( GL_ALPHA_BIAS, 0.0f );


	glBindTexture( GL_TEXTURE_2D, texture_handle_ );

	if( format == Yagl::BITMAP_FORMAT_ARGB32 )
	{
		bitmap = new unsigned int[width*height];
		memcpy( bitmap, buffer, width*height * sizeof( unsigned int ) );
		convertARGB32toABGR32( (unsigned char*)bitmap, width * height * 4 );
		glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, bitmap );
	}
	else
	{		
		glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
	}
		
	glBindTexture( GL_TEXTURE_2D, 0 );

	if( format == Yagl::BITMAP_FORMAT_ARGB32 )
	{
		delete[] bitmap;
	}

	glPopClientAttrib();
	glPopAttrib();
}
