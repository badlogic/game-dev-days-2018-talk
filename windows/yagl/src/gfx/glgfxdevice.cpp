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
// 2006-02-10 : [coderJeff] changed setTintingColor()
//              tinting_color_ is last set tinting_color_
//              tinting_r_, tinting_g_, tinting_b_, are last executed color
//              tinting_color_ is reset to 0xffffff on Clear()
//-----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif
#include "gfx/glgfxdevice.h"
#include "input/glgfxkeyboard.h"
#include "log/log.h"
#include "GL/glfw.h"
#include "decoder/bmpdecoder.h"
#include "decoder/pngdecoder.h"
#include <cstring>
#include <cmath>

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

Yagl::GlGfxDevice Yagl::GlGfxDevice::instance_;


//
// nasty hack
//

int Yagl::GlGfxDevice::windowCloseCallback( void )
{	
	instance_.setWindowClosed( );
	return GL_FALSE;
}

Yagl::GlGfxDevice::GlGfxDevice( )
{
	is_initialized_ = false;
	is_window_closed_ = false;
	is_vsync_enabled_ = false;	
	clip_min_x_ = clip_max_x_ = clip_min_y_ = clip_max_y_ = 0;
	num_commands_ = 0;
	last_texture_handle_ = 0;
	width_ = height_ = bitdepth_ = 0;	
	tinting_color_ = 0xffffff;
	tinting_r_ = 1.0;
	tinting_g_ = 1.0;
	tinting_b_ = 1.0;
}

Yagl::GlGfxDevice::GlGfxDevice( const GlGfxDevice& device )
{
}

Yagl::GlGfxDevice::~GlGfxDevice( )
{
	deinitialize( );
}

Yagl::GfxDevice& Yagl::GlGfxDevice::getInstance( )
{
	return instance_;
}

bool Yagl::GlGfxDevice::initialize( )
{
	if( is_initialized_ )
		return true;

	if( glfwInit() != GL_TRUE )	
		return false;	
		
	is_window_closed_ = false;
	is_vsync_enabled_ = false;	
	clip_min_x_ = clip_max_x_ = clip_min_y_ = clip_max_y_ = 0;
	num_commands_ = 0;
	last_texture_handle_ = 0;
	width_ = height_ = bitdepth_ = 0;	
	tinting_color_ = 0xffffff;	
	tinting_r_ = 1.0;
	tinting_g_ = 1.0;
	tinting_b_ = 1.0;
	is_initialized_ = true;	

	flushCommands();
	Decoder::registerDecoder( new BmpDecoder() );
	Decoder::registerDecoder( new PngDecoder() );
	return true;
}

void Yagl::GlGfxDevice::deinitialize( )
{
	width_ = height_ = bitdepth_ = 0;
		
	destroyAllFonts();
	destroyAllSurfaces();
		
	while( screen_modes_.size() )
	{
		delete screen_modes_.front();
		screen_modes_.pop_front();
	}	
		
	glfwSetCharCallback( 0 );
	glfwSetKeyCallback( 0 );

	if( is_initialized_ )				
		glfwTerminate();	
	
	is_initialized_ = false;
	is_window_closed_ = false;
	is_vsync_enabled_ = false;	
	clip_min_x_ = clip_max_x_= clip_min_y_ = clip_max_y_ = 0;
	num_commands_ = 0;
	last_texture_handle_ = 0;
	width_ = height_ = bitdepth_ = 0;	
	tinting_color_ = 0xffffff;	
	tinting_r_ = 1.0;
	tinting_g_ = 1.0;
	tinting_b_ = 1.0;
}

bool Yagl::GlGfxDevice::setScreenMode( int width, int height, int bitdepth, bool fullscreen )
{
	int red_bits, blue_bits, green_bits, alpha_bits;
	
	if( is_initialized_ )
	{
		destroyAllFonts( );
		destroyAllSurfaces( );
		glfwSetCharCallback( 0 );
		glfwSetKeyCallback( 0 );
		glfwCloseWindow( );
		dynamic_cast<Yagl::GlGfxKeyboard&>(Yagl::GlGfxKeyboard::getInstance()).stop();
	}
	else
		initialize();		
	
	is_window_closed_ = false;
	
	if( bitdepth != 16 && bitdepth != 24 && bitdepth != 32 )
		return false;
	
	if( fullscreen )
	{
		switch( bitdepth )
		{
			case 16:
				red_bits = 5;
				green_bits = 6;
				blue_bits = 5;
				alpha_bits = 0;
				break;
			case 24:
				red_bits = 8;
				green_bits = 8;
				blue_bits = 8;
				alpha_bits = 0;
				break;
			case 32:
				red_bits = 8;
				green_bits = 8;
				blue_bits = 8;
				alpha_bits = 8;
				break;
		}
	}
	else
	{
		GLFWvidmode mode;
		glfwGetDesktopMode( &mode );
		red_bits = mode.RedBits;
		blue_bits = mode.BlueBits;
		green_bits = mode.GreenBits;
		alpha_bits = 0;
	}
	
	if( glfwOpenWindow( width, height, red_bits, green_bits, blue_bits, alpha_bits, 16, 0, fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW ) != GL_TRUE )
	{
		Yagl::Log::print( "GfxDevice: couldn't set ScreenMode ( " );
		Yagl::Log::print( width );
		Yagl::Log::print( ", " );
		Yagl::Log::print( height );
		Yagl::Log::print( ", " );	
		Yagl::Log::print( bitdepth );			
		Yagl::Log::print( " )\n" );
		return false; 
	}	
	
	//
	// FIXME somehow.... nasty crossmodule dependency..
	//
	glfwSetCharCallback( Yagl::GlGfxKeyboard::charCallback );
	glfwSetKeyCallback( Yagl::GlGfxKeyboard::keyCallback );
	dynamic_cast<Yagl::GlGfxKeyboard&>(Yagl::GlGfxKeyboard::getInstance()).start();
	
	setWindowTitle( "yagl - gl mode" );
	showMouseCursor( );
	
	width_ = width;
	height_ = height;
	bitdepth_ = bitdepth;
	clip_min_x_ = 0;
	clip_max_x_ = width_ - 1;
	clip_min_y_ = 0;
	clip_max_y_ = height_ - 1;

	glfwSwapInterval( 0 );
	glfwSetWindowCloseCallback( windowCloseCallback );
	is_vsync_enabled_ = false;

	flushCommands();
	clear( 0 );
	
	width_ = width;
	height_ = height;
	bitdepth_ = bitdepth;	
	
	Yagl::Log::print( "GfxDevice: set ScreenMode ( " );
	Yagl::Log::print( width );
	Yagl::Log::print( ", " );
	Yagl::Log::print( height );
	Yagl::Log::print( ", " );
	Yagl::Log::print( bitdepth );
	Yagl::Log::print( " )" );		
	
	if( fullscreen ) 
		Yagl::Log::print( " fullscreen\n" );
	else
		Yagl::Log::print( " windowed\n" );		

	return true;
}

#define MAX_NUM_MODES 400
const std::list<Yagl::ScreenModeInfo*>& Yagl::GlGfxDevice::getScreenModes( )
{
	if( screen_modes_.size() )
		return screen_modes_;
	
    GLFWvidmode modes[ MAX_NUM_MODES ];
    int     modecount, i;    

    // List available video modes
    modecount = glfwGetVideoModes( modes, MAX_NUM_MODES );
    
    for( i = 0; i < modecount; i ++ )    
		screen_modes_.push_back( new ScreenModeInfo( modes[i].Width, modes[i].Height, modes[i].RedBits + modes[i].GreenBits + modes[i].BlueBits ) );    
	
	return screen_modes_;
}

bool Yagl::GlGfxDevice::isScreenModeAvailable( int width, int height, int bitdepth )
{
	std::list<ScreenModeInfo*>::const_iterator mode_iter = getScreenModes().begin();
	
	while( mode_iter != screen_modes_.end() )
	{
		if( (*mode_iter)->getWidth() == width && 
			(*mode_iter)->getHeight() == height &&
			(*mode_iter)->getBitDepth() == bitdepth )
				return true;
				
		mode_iter++;
	}
	return false;
}

void Yagl::GlGfxDevice::setWindowTitle( const Yagl::String& title )
{	
	glfwSetWindowTitle( title.c_str() );
}

void Yagl::GlGfxDevice::hideMouseCursor( )
{
	glfwDisable( GLFW_MOUSE_CURSOR );
}

void Yagl::GlGfxDevice::showMouseCursor( )
{
	glfwEnable( GLFW_MOUSE_CURSOR );
}

bool Yagl::GlGfxDevice::wasWindowCloseButtonPressed()
{
	return is_window_closed_;
}

void Yagl::GlGfxDevice::setWindowClosed( )
{
	is_window_closed_ = true;
}

void Yagl::GlGfxDevice::swapBuffers( )
{
	executeCommands();
	glfwSwapBuffers( );
}

bool Yagl::GlGfxDevice::isMethodImplemented( YAGL_GFX_CAPS method )
{
	if( method != HAS_LINE && method != HAS_BOX && method != HAS_SOLIDBOX &&
		method != HAS_BLIT && method != HAS_BLIT_RANGED &&
		method != HAS_BLIT_SCALED && method != HAS_BLIT_SCALED_RANGED &&
		method != HAS_BLIT_ROTATED && method != HAS_BLIT_ROTATED_RANGED &&
		method != HAS_BLIT_ROTATED_SCALED && method != HAS_BLIT_ROTATED_SCALED_RANGED )
		return false;
	else
		return true;
}

bool Yagl::GlGfxDevice::isBlitterImplemented( YAGL_BLIT_MODE mode , YAGL_BLIT_METHOD method )
{
	if( mode != BLIT_SOLID && mode != BLIT_ALPHAMASKED )
		return false;
	else
		return true;
}

void Yagl::GlGfxDevice::set( int attribute, void* value )
{
}

void Yagl::GlGfxDevice::get( int attribute, void* value )
{
}

void Yagl::GlGfxDevice::setClippingRegion( int clip_min_x, int clip_min_y, int clip_max_x, int clip_max_y )
{
	pushCommand( GLGFXDEVICE_CLIPPING_REGION, clip_min_x, clip_min_y, clip_max_x, clip_max_y, 0, 0, 0, 0, 0.0f, 0.0f,
				 0.0f, 0, BLIT_SOLID, 0, 0, 0, 0.0f );
		 

	/*if( clip_min_x > clip_max_x )
	{
		int tmp = clip_max_x;
		clip_max_x = clip_min_x;
		clip_min_x = tmp;
	}
	
	if( clip_min_y > clip_max_y )
	{
		int tmp = clip_max_y;
		clip_max_y = clip_min_y;
		clip_min_y = tmp;
	}

	if( clip_min_x == 0 && clip_max_x == 0 && clip_min_y == 0 && clip_max_y == 0 )
	{
		clip_min_x_ = 0;
		clip_max_x_ = width_ - 1;
		clip_min_y_ = 0;
		clip_max_y_ = height_ - 1;
		return;
	}
	
	if( clip_min_x < 0 || clip_max_x > width_ - 1 || clip_min_y < 0 || clip_max_y > height_ -1 )
		return;
	
	clip_min_x_ = clip_min_x;
	clip_max_x_ = clip_max_x;
	clip_min_y_ = clip_min_y;
	clip_max_y_ = clip_max_y;*/
}

void Yagl::GlGfxDevice::getClippingRegion( int &clip_min_x, int &clip_min_y, int &clip_max_x, int &clip_max_y )
{
	clip_min_x = clip_min_x_;
	clip_min_y = clip_min_y_;
	clip_max_x = clip_max_x_;
	clip_max_y = clip_max_y_;
}

Yagl::GfxSurface* Yagl::GlGfxDevice::createSurface( )
{
	GlGfxSurface* new_surface = new GlGfxSurface( );
	if( new_surface->create( 0, 0, bitdepth_ ) == false )	
	{
		Yagl::Log::print( "GfxDevice: couldn't create surface\n" );
		delete new_surface;
		return 0;
	}
			
	surfaces_.push_back( new_surface );	
	
	Yagl::Log::print( "GfxDevice: create surface at " );	
	Yagl::Log::print( new_surface );
	Yagl::Log::print( "\n" );
		
	return new_surface;
}

Yagl::GlGfxSurface* Yagl::GlGfxDevice::createSurfaceNoRegistering( )
{
	GlGfxSurface* new_surface = new GlGfxSurface( );
	if( new_surface->create( 0, 0, bitdepth_ ) == false )	
	{
		Yagl::Log::print( "GfxDevice: couldn't create surface\n" );
		delete new_surface;
		return 0;
	}	
	
	/*Yagl::Log::print( "GfxDevice: create surface at " );	
	Yagl::Log::print( (int)new_surface );
	Yagl::Log::print( "\n" );*/
		
	return new_surface;
}

void Yagl::GlGfxDevice::destroySurface( Yagl::GfxSurface* surface )
{
	if( surface )
	{
		surfaces_.remove( dynamic_cast<GlGfxSurface*>(surface) );
		delete surface;
		Yagl::Log::print( "GfxDevice: destroyed surface at " );
		Yagl::Log::print( surface );
		Yagl::Log::print( "\n" );
	}
}

void Yagl::GlGfxDevice::destroyAllSurfaces( )
{
	while( surfaces_.size() )
	{
		delete dynamic_cast<GlGfxSurface*>(surfaces_.front());
		Yagl::Log::print( "GfxDevice: destroyed surface at " );
		Yagl::Log::print( surfaces_.front() );
		Yagl::Log::print( "\n" );
		surfaces_.pop_front();
	}
}

Yagl::GfxFont* Yagl::GlGfxDevice::createFont( )
{
	GlGfxFont* new_font = new GlGfxFont( );
				
	fonts_.push_back( new_font );	
	
	Yagl::Log::print( "GfxDevice: created font at " );
	Yagl::Log::print( new_font );
	Yagl::Log::print( "\n" );			
		
	return new_font;
}
			
void Yagl::GlGfxDevice::destroyFont( Yagl::GfxFont* font )
{
	if( font )
	{
		fonts_.remove( dynamic_cast<GlGfxFont*>(font) );
		delete font;
		Yagl::Log::print( "GfxDevice: destroyed Font at " );
		Yagl::Log::print( font );
		Yagl::Log::print( "\n" );
	}
}
			
void Yagl::GlGfxDevice::destroyAllFonts( )
{
	while( fonts_.size() )
	{
		delete dynamic_cast<GlGfxFont*>(fonts_.front());
		Yagl::Log::print( "GfxDevice: destroyed font at " );
		Yagl::Log::print( fonts_.front());
		Yagl::Log::print( "\n" );
		fonts_.pop_front();
	}
}

void Yagl::GlGfxDevice::pushCommand( GLGFXDEVICE_COMMAND cmd_id, 
									 int x1, int y1, int x2, int y2,
									 int src_min_x, int src_min_y, int src_max_x, int src_max_y,
									 float scale_x, float scale_y,
									 float angle,
									 unsigned int color, 
									 YAGL_BLIT_MODE mode,
									 GlGfxSurface* surface,
									 GlGfxFont* font,
									 const char* text,
									 float blend_factor
									)
{	
	if( num_commands_ >= 10000 )
	{
		executeCommands( );		
	}

	switch( cmd_id )
	{
		case GLGFXDEVICE_CLIPPING_REGION:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].box_x1 = x1;
			commands_[num_commands_].box_y1 = y1;
			commands_[num_commands_].box_x2 = x2;
			commands_[num_commands_].box_y2 = y2;
			break;

		case GLGFXDEVICE_TINTING_COLOR:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].tinting_color = color;
			break;

		case GLGFXDEVICE_LINE:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].line_x1 = x1;
			commands_[num_commands_].line_y1 = y1;
			commands_[num_commands_].line_x2 = x2;
			commands_[num_commands_].line_y2 = y2;
			commands_[num_commands_].line_color = color;
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
			
		case GLGFXDEVICE_CIRCLE:
			break;
			
		case GLGFXDEVICE_SOLIDCIRCLE:
			break;
			
		case GLGFXDEVICE_BOX:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].box_x1 = x1;
			commands_[num_commands_].box_y1 = y1;
			commands_[num_commands_].box_x2 = x2;
			commands_[num_commands_].box_y2 = y2;
			commands_[num_commands_].box_color = color;
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
		
		case GLGFXDEVICE_SOLIDBOX:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].box_x1 = x1;
			commands_[num_commands_].box_y1 = y1;
			commands_[num_commands_].box_x2 = x2;
			commands_[num_commands_].box_y2 = y2;
			commands_[num_commands_].box_color = color;
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
			
		case GLGFXDEVICE_BLIT:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].blit_x = x1;
			commands_[num_commands_].blit_y = y1;
			commands_[num_commands_].surface = surface;
			commands_[num_commands_].blit_mode = mode;
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
		
		case GLGFXDEVICE_BLIT_RANGED:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].blit_x = x1;
			commands_[num_commands_].blit_y = y1;
			commands_[num_commands_].src_min_x = src_min_x;
			commands_[num_commands_].src_min_y = src_min_y;
			commands_[num_commands_].src_max_x = src_max_x;
			commands_[num_commands_].src_max_y = src_max_y;
			commands_[num_commands_].surface = surface;
			commands_[num_commands_].blit_mode = mode;			
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
			
		case GLGFXDEVICE_BLIT_SCALED:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].blit_x = x1;
			commands_[num_commands_].blit_y = y1;
			commands_[num_commands_].blit_scale_x = scale_x;
			commands_[num_commands_].blit_scale_y = scale_y;
			commands_[num_commands_].surface = surface;
			commands_[num_commands_].blit_mode = mode;	
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
		
		case GLGFXDEVICE_BLIT_SCALED_RANGED:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].blit_x = x1;
			commands_[num_commands_].blit_y = y1;
			commands_[num_commands_].src_min_x = src_min_x;
			commands_[num_commands_].src_min_y = src_min_y;
			commands_[num_commands_].src_max_x = src_max_x;
			commands_[num_commands_].src_max_y = src_max_y;
			commands_[num_commands_].blit_scale_x = scale_x;
			commands_[num_commands_].blit_scale_y = scale_y;
			commands_[num_commands_].surface = surface;
			commands_[num_commands_].blit_mode = mode;	
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
			
		case GLGFXDEVICE_BLIT_ROTATED:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].blit_x = x1;
			commands_[num_commands_].blit_y = y1;
			commands_[num_commands_].blit_angle = angle;
			commands_[num_commands_].surface = surface;
			commands_[num_commands_].blit_mode = mode;	
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
		
		case GLGFXDEVICE_BLIT_ROTATED_RANGED:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].blit_x = x1;
			commands_[num_commands_].blit_y = y1;
			commands_[num_commands_].src_min_x = src_min_x;
			commands_[num_commands_].src_min_y = src_min_y;
			commands_[num_commands_].src_max_x = src_max_x;
			commands_[num_commands_].src_max_y = src_max_y;
			commands_[num_commands_].blit_angle = angle;
			commands_[num_commands_].surface = surface;
			commands_[num_commands_].blit_mode = mode;	
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
		
		case GLGFXDEVICE_BLIT_SCALED_ROTATED:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].blit_x = x1;
			commands_[num_commands_].blit_y = y1;
			commands_[num_commands_].blit_angle = angle;
			commands_[num_commands_].blit_scale_x = scale_x;
			commands_[num_commands_].blit_scale_y = scale_y;
			commands_[num_commands_].surface = surface;
			commands_[num_commands_].blit_mode = mode;	
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
		
		case GLGFXDEVICE_BLIT_SCALED_ROTATED_RANGED:
			commands_[num_commands_].cmd_id = cmd_id;
			commands_[num_commands_].blit_x = x1;
			commands_[num_commands_].blit_y = y1;
			commands_[num_commands_].src_min_x = src_min_x;
			commands_[num_commands_].src_min_y = src_min_y;
			commands_[num_commands_].src_max_x = src_max_x;
			commands_[num_commands_].src_max_y = src_max_y;
			commands_[num_commands_].blit_scale_x = scale_x;
			commands_[num_commands_].blit_scale_y = scale_y;
			commands_[num_commands_].blit_angle = angle;
			commands_[num_commands_].surface = surface;
			commands_[num_commands_].blit_mode = mode;	
			commands_[num_commands_].blend_factor = blend_factor;			
			break;
		case GLGFXDEVICE_PRINTAT_FONT:						
							
				commands_[num_commands_].cmd_id = cmd_id;
				commands_[num_commands_].print_x = x1;
				commands_[num_commands_].print_y = y1;	
				commands_[num_commands_].print_color = color;
				commands_[num_commands_].print_font = font;
				commands_[num_commands_].blend_factor = blend_factor;														
				//print_strings_[num_commands_] = "";												
				print_strings_[num_commands_] = text;												
			break;
			
		default:
			break;
	}
	
	num_commands_++;
}

void Yagl::GlGfxDevice::executeCommands( )
{
	int last_blit_mode = BLIT_ALPHAMASKED;
	float t_x, t_y;

	setupExecutionEnvironment( );		
	
	GLGFXDEVICE_COMMAND curr_command = GLGFXDEVICE_NOP;
	
	for( unsigned int i = 0; i < num_commands_; i++ )
	{
		//
		// not an actual gl command, process and skip ahead to next
		//
		if ( commands_[i].cmd_id == GLGFXDEVICE_TINTING_COLOR)
		{
			tinting_r_ = float( ( commands_[i].tinting_color >> 16 ) & 255 ) / 255.0f;	
			tinting_g_ = float( ( commands_[i].tinting_color >> 8 ) & 255 ) / 255.0f;	
			tinting_b_ = float( commands_[i].tinting_color & 255 ) / 255.0f;								
			continue;
		}

		//
		// setup the glBegin according to the current command
		//
		if( curr_command != commands_[i].cmd_id )
		{
			if( curr_command != GLGFXDEVICE_NOP )
			{				
				glEnd( );
				glDisable( GL_TEXTURE_2D );
				glDisable( GL_BLEND );					
			}
				
			curr_command = commands_[i].cmd_id;			

			switch( curr_command )
			{
				case GLGFXDEVICE_CLIPPING_REGION:
					executeSetClippingRegion( i );
					break;

				case GLGFXDEVICE_LINE:
					glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );					
					if( commands_[i].blend_factor != 1.0 )
					{
						glEnable( GL_BLEND );
						last_blit_mode = BLIT_ALPHAMASKED_AND_BLENDFACTOR;
					}
					else
						last_blit_mode = BLIT_SOLID;										
					glBegin( GL_LINES );
					break;
				case GLGFXDEVICE_BOX:
					glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );					
					if( commands_[i].blend_factor != 1.0 )
					{
						glEnable( GL_BLEND );
						last_blit_mode = BLIT_ALPHAMASKED_AND_BLENDFACTOR;
					}
					else
						last_blit_mode = BLIT_SOLID;
					glBegin( GL_QUADS );
					break;
				case GLGFXDEVICE_SOLIDBOX:
					glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
					if( commands_[i].blend_factor != 1.0 )
										if( commands_[i].blend_factor != 1.0 )
					{
						glEnable( GL_BLEND );
						last_blit_mode = BLIT_ALPHAMASKED_AND_BLENDFACTOR;
					}
					else
						last_blit_mode = BLIT_SOLID;
					glBegin( GL_QUADS );
					break;				
				case GLGFXDEVICE_BLIT:
				case GLGFXDEVICE_BLIT_RANGED:
					glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
					if( commands_[i].blit_mode == BLIT_SOLID )					
						glDisable( GL_BLEND );					
					if( commands_[i].blit_mode == BLIT_ALPHAMASKED || commands_[i].blit_mode == BLIT_ALPHAMASKED_AND_BLENDFACTOR )					
						glEnable( GL_BLEND );
					last_blit_mode = commands_[i].blit_mode;	
					glEnable( GL_TEXTURE_2D );
					if( commands_[i].surface->getTextureHandle() != last_texture_handle_ )
					{	
						glBindTexture( GL_TEXTURE_2D, commands_[i].surface->getTextureHandle() );
						last_texture_handle_ = commands_[i].surface->getTextureHandle();
					}
					glBegin( GL_QUADS );
					break;
				case GLGFXDEVICE_BLIT_SCALED:
				case GLGFXDEVICE_BLIT_SCALED_RANGED:
					glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
					if( commands_[i].blit_mode == BLIT_SOLID )					
						glDisable( GL_BLEND );					
					if( commands_[i].blit_mode == BLIT_ALPHAMASKED || commands_[i].blit_mode == BLIT_ALPHAMASKED_AND_BLENDFACTOR )					
						glEnable( GL_BLEND );
					last_blit_mode = commands_[i].blit_mode;					
					glEnable( GL_TEXTURE_2D );
					if( commands_[i].surface->getTextureHandle() != last_texture_handle_ )
					{	
						glBindTexture( GL_TEXTURE_2D, commands_[i].surface->getTextureHandle() );
						last_texture_handle_ = commands_[i].surface->getTextureHandle();
					}
					
					glPushMatrix();					
					if( curr_command == GLGFXDEVICE_BLIT_SCALED )
					{
						t_x = commands_[i].blit_x + ( commands_[i].surface->getWidth() / 2.0f ) * fabs(commands_[i].blit_scale_x);
						t_y = commands_[i].blit_y + ( commands_[i].surface->getHeight() / 2.0f ) * fabs(commands_[i].blit_scale_y);
					}
					else
					{
						t_x = commands_[i].blit_x + ( ( commands_[i].src_max_x - commands_[i].src_min_x + 1) / 2.0f ) * fabs(commands_[i].blit_scale_x);
						t_y = commands_[i].blit_y + ( ( commands_[i].src_max_y - commands_[i].src_min_y + 1) / 2.0f ) * fabs(commands_[i].blit_scale_y);
					}
						
					glTranslatef( t_x, t_y, 0.0f );
					glScalef( commands_[i].blit_scale_x, commands_[i].blit_scale_y, 0.0f );					
					glBegin( GL_QUADS );
					if( curr_command == GLGFXDEVICE_BLIT_SCALED )
						executeBlitScaled( i );
					else
						executeBlitScaledRanged( i );
					glEnd();				
					glPopMatrix();
					glDisable( GL_TEXTURE_2D );
					glDisable( GL_BLEND );	
					curr_command = GLGFXDEVICE_NOP;
					continue;
				
				case GLGFXDEVICE_BLIT_ROTATED:
				case GLGFXDEVICE_BLIT_ROTATED_RANGED:
					glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
					if( commands_[i].blit_mode == BLIT_SOLID )					
						glDisable( GL_BLEND );					
					if( commands_[i].blit_mode == BLIT_ALPHAMASKED || commands_[i].blit_mode == BLIT_ALPHAMASKED_AND_BLENDFACTOR )					
						glEnable( GL_BLEND );
					last_blit_mode = commands_[i].blit_mode;					
					glEnable( GL_TEXTURE_2D );
					if( commands_[i].surface->getTextureHandle() != last_texture_handle_ )
					{	
						glBindTexture( GL_TEXTURE_2D, commands_[i].surface->getTextureHandle() );
						last_texture_handle_ = commands_[i].surface->getTextureHandle();
					}
					
					glPushMatrix();											
					glTranslatef( commands_[i].blit_x + ( commands_[i].surface->getWidth() % 2?0.5f:0.0f ),
								  commands_[i].blit_y + ( commands_[i].surface->getHeight() % 2?0.5f:0.0f ), 0.0f );
					glRotatef( commands_[i].blit_angle, 0.0f, 0.0f, 1.0f );
					glBegin( GL_QUADS );
					if( curr_command == GLGFXDEVICE_BLIT_ROTATED )
						executeBlitRotated( i );
					else
						executeBlitRotatedRanged( i );
					glEnd();				
					glPopMatrix();
					glDisable( GL_TEXTURE_2D );
					glDisable( GL_BLEND );	
					curr_command = GLGFXDEVICE_NOP;
					continue;				
					
				case GLGFXDEVICE_BLIT_SCALED_ROTATED:
				case GLGFXDEVICE_BLIT_SCALED_ROTATED_RANGED:
					glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
					if( commands_[i].blit_mode == BLIT_SOLID )					
						glDisable( GL_BLEND );					
					if( commands_[i].blit_mode == BLIT_ALPHAMASKED || commands_[i].blit_mode == BLIT_ALPHAMASKED_AND_BLENDFACTOR )					
						glEnable( GL_BLEND );
					last_blit_mode = commands_[i].blit_mode;					
					glEnable( GL_TEXTURE_2D );
					if( commands_[i].surface->getTextureHandle() != last_texture_handle_ )
					{	
						glBindTexture( GL_TEXTURE_2D, commands_[i].surface->getTextureHandle() );
						last_texture_handle_ = commands_[i].surface->getTextureHandle();
					}
					
					glPushMatrix();											
					glTranslatef( commands_[i].blit_x + ( commands_[i].surface->getWidth() % 2?0.5f:0.0f ),
								  commands_[i].blit_y + ( commands_[i].surface->getHeight() % 2?0.5f:0.0f ), 0.0f );
					glRotatef( commands_[i].blit_angle, 0.0f, 0.0f, 1.0f );
					glScalef( commands_[i].blit_scale_x, commands_[i].blit_scale_y, 0.0f );
					glBegin( GL_QUADS );
					if( curr_command == GLGFXDEVICE_BLIT_SCALED_ROTATED )
						executeBlitRotated( i );
					else
						executeBlitRotatedRanged( i );
					glEnd();				
					glPopMatrix();
					glDisable( GL_TEXTURE_2D );
					glDisable( GL_BLEND );	
					curr_command = GLGFXDEVICE_NOP;
					continue;				
				
				case GLGFXDEVICE_PRINTAT_FONT:
					glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
					glEnable( GL_BLEND );
					glEnable( GL_TEXTURE_2D );					
										
					executePrintAtFont( i );										
					
					glDisable( GL_TEXTURE_2D );
					glDisable( GL_BLEND );	
					print_strings_[i] = "";
					curr_command = GLGFXDEVICE_NOP;
					last_blit_mode = BLIT_SOLID;
					last_texture_handle_ = 0;
					continue;
				default:	
					break;
			}			
		}
		
		switch( curr_command )
		{
			case GLGFXDEVICE_LINE:
				if( last_blit_mode == BLIT_SOLID && commands_[i].blend_factor != 1.0 )
				{
					glEnd( );
					glEnable( GL_BLEND );
					last_blit_mode = BLIT_ALPHAMASKED_AND_BLENDFACTOR;
					glBegin( GL_LINES );
				}
				
				if( last_blit_mode == BLIT_ALPHAMASKED_AND_BLENDFACTOR && commands_[i].blend_factor == 1.0 )
				{
					glEnd( );
					glDisable( GL_BLEND );
					last_blit_mode = BLIT_SOLID;
					glBegin( GL_LINES );
				}
				
				executeLine( i );
				break;
			case GLGFXDEVICE_BOX:
				if( last_blit_mode == BLIT_SOLID && commands_[i].blend_factor != 1.0 )
				{
					glEnd( );
					glEnable( GL_BLEND );
					last_blit_mode = BLIT_ALPHAMASKED_AND_BLENDFACTOR;
					glBegin( GL_QUADS );
				}
				
				if( last_blit_mode == BLIT_ALPHAMASKED_AND_BLENDFACTOR && commands_[i].blend_factor == 1.0 )
				{
					glEnd( );
					glDisable( GL_BLEND );
					last_blit_mode = BLIT_SOLID;
					glBegin( GL_QUADS );
				}
				
				executeBox( i );
				break;
			case GLGFXDEVICE_SOLIDBOX:
				if( last_blit_mode == BLIT_SOLID && commands_[i].blend_factor != 1.0 )
				{
					glEnd( );
					glEnable( GL_BLEND );
					last_blit_mode = BLIT_ALPHAMASKED_AND_BLENDFACTOR;
					glBegin( GL_QUADS );
				}
				
				if( last_blit_mode == BLIT_ALPHAMASKED_AND_BLENDFACTOR && commands_[i].blend_factor == 1.0 )
				{
					glEnd( );
					glDisable( GL_BLEND );
					last_blit_mode = BLIT_SOLID;
					glBegin( GL_QUADS );
				}
				
				executeSolidBox( i );
				break;
			case GLGFXDEVICE_BLIT:
				if( commands_[i].surface->getTextureHandle() != last_texture_handle_ )
				{	
					glEnd();
					glBindTexture( GL_TEXTURE_2D, commands_[i].surface->getTextureHandle() );
					last_texture_handle_ = commands_[i].surface->getTextureHandle();
					glBegin( GL_QUADS );
				}
			
				if( last_blit_mode == BLIT_SOLID && commands_[i].blit_mode != BLIT_SOLID )
				{
					glEnd();
					glEnable( GL_BLEND );
					glBegin( GL_QUADS );
					last_blit_mode = BLIT_ALPHAMASKED;
				}
				
				if( ( last_blit_mode == BLIT_ALPHAMASKED || last_blit_mode == BLIT_ALPHAMASKED_AND_BLENDFACTOR ) && commands_[i].blit_mode == BLIT_SOLID )
				{
					glEnd();
					glDisable( GL_BLEND );
					glBegin( GL_QUADS );
					last_blit_mode = BLIT_SOLID;
				}
				
				executeBlit( i );
				break;
			case GLGFXDEVICE_BLIT_RANGED:
				if( commands_[i].surface->getTextureHandle() != last_texture_handle_ )
				{	
					glEnd();
					glBindTexture( GL_TEXTURE_2D, commands_[i].surface->getTextureHandle() );
					last_texture_handle_ = commands_[i].surface->getTextureHandle();
					glBegin( GL_QUADS );
				}
			
				if( last_blit_mode == BLIT_SOLID && commands_[i].blit_mode != BLIT_SOLID )
				{
					glEnd();
					glEnable( GL_BLEND );
					glBegin( GL_QUADS );
					last_blit_mode = BLIT_ALPHAMASKED;
				}
				
				if( ( last_blit_mode == BLIT_ALPHAMASKED || last_blit_mode == BLIT_ALPHAMASKED_AND_BLENDFACTOR ) && commands_[i].blit_mode == BLIT_SOLID )
				{
					glEnd();
					glDisable( GL_BLEND );
					glBegin( GL_QUADS );
					last_blit_mode = BLIT_SOLID;
				}
				
				executeBlitRanged( i );
				break;											
				
			default:
				break;
		}
		
		/*if( i % 1000 == 1 )
			glFlush( );*/
	}
	
	if( curr_command == GLGFXDEVICE_LINE ||
		curr_command == GLGFXDEVICE_BOX ||
		curr_command == GLGFXDEVICE_SOLIDBOX ||
		curr_command == GLGFXDEVICE_BLIT ||
		curr_command == GLGFXDEVICE_BLIT_RANGED )
		glEnd( );		
	restoreOldEnvironment( );
	flushCommands( );
}

void Yagl::GlGfxDevice::flushCommands( )
{
	num_commands_ = 0;
}

void Yagl::GlGfxDevice::executeSetClippingRegion( unsigned int command_index )
{
	int clip_min_x = commands_[command_index].box_x1;
	int clip_min_y = commands_[command_index].box_y1;
	int clip_max_x = commands_[command_index].box_x2;
	int clip_max_y = commands_[command_index].box_y2;

	if( clip_min_x > clip_max_x )
	{
		int tmp = clip_max_x;
		clip_max_x = clip_min_x;
		clip_min_x = tmp;
	}
	
	if( clip_min_y > clip_max_y )
	{
		int tmp = clip_max_y;
		clip_max_y = clip_min_y;
		clip_min_y = tmp;
	}

	if( clip_min_x == 0 && clip_max_x == 0 && clip_min_y == 0 && clip_max_y == 0 )
	{
		clip_min_x_ = 0;
		clip_max_x_ = width_ - 1;
		clip_min_y_ = 0;
		clip_max_y_ = height_ - 1;	
		glScissor( clip_min_x_, height_ - clip_max_y_ - 1, clip_max_x_ - clip_min_x_ + 1, clip_max_y_ - clip_min_y_ + 1 );
		return;
	}
	
	if( clip_min_x < 0 || clip_max_x > width_ - 1 || clip_min_y < 0 || clip_max_y > height_ -1 )
		return;
	
	clip_min_x_ = clip_min_x;
	clip_max_x_ = clip_max_x;
	clip_min_y_ = clip_min_y;
	clip_max_y_ = clip_max_y;
	glScissor( clip_min_x_, height_ - clip_max_y_ - 1, clip_max_x_ - clip_min_x_ + 1, clip_max_y_ - clip_min_y_ + 1 );
}

void Yagl::GlGfxDevice::executeLine( unsigned int command_index )
{
	float r, g, b, a;
	unsigned int color = commands_[command_index].line_color;

	r = float( ( color >> 16 ) & 255 ) / 255.0f;	
	g = float( ( color >> 8 ) & 255 ) / 255.0f;	
	b = float( color & 255 ) / 255.0f;	
	a = commands_[command_index].blend_factor;	
		
	double x1 = commands_[command_index].line_x1 + 0.375;
	double y1 = commands_[command_index].line_y1 + 0.375;
	double x2 = commands_[command_index].line_x2 + 0.375;
	double y2 = commands_[command_index].line_y2 + 0.375;

	glColor4f( r, g, b, a );
	glVertex2d( x1, y1 );
	glColor4f( r, g, b, a );
	glVertex2d( x2, y2 );
}

void Yagl::GlGfxDevice::executeSolidBox( unsigned int command_index )
{
	float r, g, b, a;
	unsigned int color = commands_[command_index].line_color;
	int width = 0, height = 0;
	
	width = commands_[command_index].box_x2 - commands_[command_index].box_x1 + 1;
	height = commands_[command_index].box_y2 - commands_[command_index].box_y1 + 1;

	r = float( ( color >> 16 ) & 255 ) / 255.0f;	
	g = float( ( color >> 8 ) & 255 ) / 255.0f;	
	b = float( color & 255 ) / 255.0f;		
	a = commands_[command_index].blend_factor;	
		
	glColor4f( r, g, b, a );
	glVertex2i( commands_[command_index].box_x1, commands_[command_index].box_y1 );
	glColor4f( r, g, b, a );
	glVertex2i( commands_[command_index].box_x1 + width, commands_[command_index].box_y1 );
	glColor4f( r, g, b, a );
	glVertex2i( commands_[command_index].box_x1 + width, commands_[command_index].box_y1 + height );
	glColor4f( r, g, b, a );
	glVertex2i( commands_[command_index].box_x1, commands_[command_index].box_y1 + height );
}

void Yagl::GlGfxDevice::executeBox( unsigned int command_index )
{
	float r, g, b, a;
	unsigned int color = commands_[command_index].line_color;	

	r = float( ( color >> 16 ) & 255 ) / 255.0f;	
	g = float( ( color >> 8 ) & 255 ) / 255.0f;	
	b = float( color & 255 ) / 255.0f;		
	a = commands_[command_index].blend_factor;	
		
	glColor4f( r, g, b, a );
	glVertex2f( commands_[command_index].box_x1 + 0.5, commands_[command_index].box_y1 + 0.5);
	glColor4f( r, g, b, a );
	glVertex2f( commands_[command_index].box_x2 + 0.5, commands_[command_index].box_y1 + 0.5 );
	glColor4f( r, g, b, a );
	glVertex2f( commands_[command_index].box_x2 + 0.5, commands_[command_index].box_y2 + 0.5 );
	glColor4f( r, g, b, a );
	glVertex2f( commands_[command_index].box_x1 + 0.5, commands_[command_index].box_y2 + 0.5 );
}

void Yagl::GlGfxDevice::executeBlit( unsigned int command_index )
{
	GlGfxSurface *surface = commands_[command_index].surface;
	double u, v;	
	
	glColor4f( tinting_r_, tinting_g_, tinting_b_, commands_[command_index].blend_factor	 );
	surface->getTextureCoordinates( 0, 0, u, v );
	glTexCoord2d( u, v );
	glVertex2i( commands_[command_index].blit_x, commands_[command_index].blit_y );
	surface->getTextureCoordinates( surface->getWidth(), 0, u, v );
	glTexCoord2d( u, v );
	glVertex2i( commands_[command_index].blit_x + commands_[command_index].surface->getWidth(), commands_[command_index].blit_y );
	surface->getTextureCoordinates( surface->getWidth(), surface->getHeight(), u, v );
	glTexCoord2d( u, v );	
	glVertex2i( commands_[command_index].blit_x + commands_[command_index].surface->getWidth(), commands_[command_index].blit_y + commands_[command_index].surface->getHeight() );
	surface->getTextureCoordinates( 0, surface->getHeight(), u, v );
	glTexCoord2d( u, v );
	glVertex2i( commands_[command_index].blit_x, commands_[command_index].blit_y + commands_[command_index].surface->getHeight() );
	
}

void Yagl::GlGfxDevice::executeBlitRanged( unsigned int command_index )
{
	GlGfxSurface *surface = commands_[command_index].surface;
	double u, v;
	int width, height;
	width = commands_[command_index].src_max_x - commands_[command_index].src_min_x + 1;
	height = commands_[command_index].src_max_y - commands_[command_index].src_min_y + 1;	

	if( width == 2 )	
		commands_[command_index].src_max_x = commands_[command_index].src_max_x + 1;						

	if( height == 2 )
		commands_[command_index].src_max_y = commands_[command_index].src_max_y + 1;			

	//commands_[command_index].src_max_x = commands_[command_index].src_max_x + 1.0;	
	//commands_[command_index].src_min_x = commands_[command_index].src_min_x + 0.5;	
	//commands_[command_index].src_max_y = commands_[command_index].src_max_y + 1.0;	
	//commands_[command_index].src_min_y = commands_[command_index].src_min_y + 0.5;

	/*commands_[command_index].blit_x = commands_[command_index].blit_x + 0.5;	
	commands_[command_index].blit_y = commands_[command_index].blit_y + 0.5;	*/
	
	
	glColor4f( tinting_r_, tinting_g_, tinting_b_, commands_[command_index].blend_factor	 );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x, commands_[command_index].src_min_y, u, v );
	glTexCoord2d( u, v );
	glVertex2i( commands_[command_index].blit_x, commands_[command_index].blit_y );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x + width, commands_[command_index].src_min_y, u, v );
	glTexCoord2d( u, v );
	glVertex2i( commands_[command_index].blit_x + width , commands_[command_index].blit_y );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x + width, commands_[command_index].src_min_y + height, u, v );
	glTexCoord2d( u, v );	
	glVertex2i( commands_[command_index].blit_x + width , commands_[command_index].blit_y + height );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x, commands_[command_index].src_min_y + height, u, v );
	glTexCoord2d( u, v );
	glVertex2i( commands_[command_index].blit_x, commands_[command_index].blit_y + height );
	
}

void Yagl::GlGfxDevice::executeBlitScaled( unsigned int command_index )
{
	GlGfxSurface *surface = commands_[command_index].surface;
	double u, v;
	
	glColor4f( tinting_r_, tinting_g_, tinting_b_, commands_[command_index].blend_factor	 );
	
	surface->getTextureCoordinates( 0, 0, u, v );
	glTexCoord2d( u, v );
	glVertex2f( -( ( surface->getWidth()) / 2.0f ) , -( ( surface->getHeight() ) / 2.0f ) );
	surface->getTextureCoordinates( surface->getWidth(), 0, u, v );
	glTexCoord2d( u, v );
	glVertex2f( ( ( surface->getWidth() ) / 2.0f ) , -( ( surface->getHeight() ) / 2.0f ) );
	surface->getTextureCoordinates( surface->getWidth(), surface->getHeight(), u, v );
	glTexCoord2d( u, v );	
	glVertex2f( ( ( surface->getWidth() ) / 2.0f ) , ( ( surface->getHeight() ) / 2.0f ) );
	surface->getTextureCoordinates( 0, surface->getHeight(), u, v );
	glTexCoord2d( u, v );
	glVertex2f( -( ( surface->getWidth() ) / 2.0f ) , ( ( surface->getHeight() ) / 2.0f ) );
	
}

void Yagl::GlGfxDevice::executeBlitScaledRanged( unsigned int command_index )
{
	GlGfxSurface *surface = commands_[command_index].surface;
	double u, v;
	int width, height;
	width = commands_[command_index].src_max_x - commands_[command_index].src_min_x + 1;
	height = commands_[command_index].src_max_y - commands_[command_index].src_min_y + 1;
	
	if( width == 2 )	
		commands_[command_index].src_max_x = commands_[command_index].src_max_x + 1;						

	if( height == 2 )
		commands_[command_index].src_max_y = commands_[command_index].src_max_y + 1;			

	glColor4f( tinting_r_, tinting_g_, tinting_b_, commands_[command_index].blend_factor	 );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x, commands_[command_index].src_min_y, u, v );
	glTexCoord2d( u, v );
	glVertex2f( -width / 2.0f, -height / 2.0f );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x + width, commands_[command_index].src_min_y, u, v );
	glTexCoord2d( u, v );
	glVertex2f( width / 2.0f , -height / 2.0f );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x + width, commands_[command_index].src_min_y + height, u, v );
	glTexCoord2d( u, v );	
	glVertex2f( width / 2.0f , height / 2.0f );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x, commands_[command_index].src_min_y + height, u, v );
	glTexCoord2d( u, v );
	glVertex2f( -width / 2.0f, height / 2.0f );
	
}

void Yagl::GlGfxDevice::executeBlitRotated( unsigned int command_index )
{
	GlGfxSurface *surface = commands_[command_index].surface;
	double u, v;
	
	glColor4f( tinting_r_, tinting_g_, tinting_b_, commands_[command_index].blend_factor	 );
	
	surface->getTextureCoordinates( 0, 0, u, v );
	glTexCoord2d( u, v );
	glVertex2f( -( ( surface->getWidth()) / 2.0f ) , -( ( surface->getHeight() ) / 2.0f ) );
	surface->getTextureCoordinates( surface->getWidth(), 0, u, v );
	glTexCoord2d( u, v );
	glVertex2f( ( ( surface->getWidth() ) / 2.0f ) , -( ( surface->getHeight() ) / 2.0f ) );
	surface->getTextureCoordinates( surface->getWidth(), surface->getHeight(), u, v );
	glTexCoord2d( u, v );	
	glVertex2f( ( ( surface->getWidth() ) / 2.0f ) , ( ( surface->getHeight() ) / 2.0f ) );
	surface->getTextureCoordinates( 0, surface->getHeight(), u, v );
	glTexCoord2d( u, v );
	glVertex2f( -( ( surface->getWidth() ) / 2.0f ) , ( ( surface->getHeight() ) / 2.0f ) );
}

void Yagl::GlGfxDevice::executeBlitRotatedRanged( unsigned int command_index )
{
	GlGfxSurface *surface = commands_[command_index].surface;
	double u, v;
	int width, height;
	width = commands_[command_index].src_max_x - commands_[command_index].src_min_x + 1;
	height = commands_[command_index].src_max_y - commands_[command_index].src_min_y + 1;
	
	glColor4f( tinting_r_, tinting_g_, tinting_b_, commands_[command_index].blend_factor	 );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x, commands_[command_index].src_min_y, u, v );
	glTexCoord2d( u, v );
	glVertex2f( -width / 2.0f, -height / 2.0f );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x + width, commands_[command_index].src_min_y, u, v );
	glTexCoord2d( u, v );
	glVertex2f( width / 2.0f , -height / 2.0f );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x + width, commands_[command_index].src_min_y + height, u, v );
	glTexCoord2d( u, v );	
	glVertex2f( width / 2.0f , height / 2.0f );
	
	surface->getTextureCoordinates( commands_[command_index].src_min_x, commands_[command_index].src_min_y + height, u, v );
	glTexCoord2d( u, v );
	glVertex2f( -width / 2.0f, height / 2.0f );
}

void Yagl::GlGfxDevice::executePrintAtFont( unsigned int command_index )
{
	float r, g, b, a;
	int color = (int)commands_[command_index].print_color;

	r = float( ( color >> 16 ) & 255 ) / 255.0f;	
	g = float( ( color >> 8 ) & 255 ) / 255.0f;	
	b = float( color & 255 ) / 255.0f;		
	a = commands_[command_index].blend_factor;
		
	glColor4f( r, g, b, a );
	commands_[command_index].print_font->printAt( print_strings_[command_index], commands_[command_index].print_x, commands_[command_index].print_y, 0, this );
}

void Yagl::GlGfxDevice::line( int x1, int y1, int x2, int y2, int color, float blend_factor )
{
	pushCommand( GLGFXDEVICE_LINE, x1, y1, x2, y2, 0, 0, 0, 0, 0, 0, 0, color, BLIT_SOLID, 0, 0, 0, blend_factor );
}

void Yagl::GlGfxDevice::box( int x1, int y1, int x2, int y2, int color, float blend_factor )
{
	if( x1 > x2 )
	{
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	
	if( y1 > y2 )
	{
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}

	pushCommand( GLGFXDEVICE_BOX, x1, y1, x2, y2, 0, 0, 0, 0, 0, 0, 0, color, BLIT_SOLID, 0, 0, 0 , blend_factor );
}

void Yagl::GlGfxDevice::solidBox( int x1, int y1, int x2, int y2, int color, float blend_factor )
{
	if( x1 > x2 )
	{
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	
	if( y1 > y2 )
	{
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}

	pushCommand( GLGFXDEVICE_SOLIDBOX, x1, y1, x2, y2, 0, 0, 0, 0, 0, 0, 0, color, BLIT_SOLID, 0, 0, 0, blend_factor );
}

void Yagl::GlGfxDevice::clear( int color )
{
	float r, g, b, a;

	r = float( ( color >> 16 ) & 255 ) / 255.0f;	
	g = float( ( color >> 8 ) & 255 ) / 255.0f;	
	b = float( color & 255 ) / 255.0f;		
	a = 1.0f;
	glClearColor( r, g, b, a );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	
	flushCommands();
}

void Yagl::GlGfxDevice::blit( int x, int y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{
	if( surface )
		pushCommand( GLGFXDEVICE_BLIT, (int)x, (int)y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, mode, dynamic_cast<GlGfxSurface*>(surface), 0, 0, blend_factor );
}

void Yagl::GlGfxDevice::blit( int x, int y, int src_min_x, int src_min_y, int src_max_x, int src_max_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{
	if( surface )
	{
		if( src_min_x > src_max_x )
		{
			int tmp = src_max_x;
			src_max_x = src_min_x;
			src_min_x = tmp;
		}
		
		if( src_min_y > src_max_y )
		{
			int tmp = src_max_y;
			src_max_y = src_min_y;
			src_min_y = tmp;
		}
		
		if( src_min_x < 0 || src_min_y < 0 || src_max_x > surface->getWidth()-1 || src_max_y > surface->getHeight()-1 )
			return;
			
		pushCommand( GLGFXDEVICE_BLIT_RANGED, (int)x, (int)y, 0, 0, 
					 src_min_x, src_min_y, src_max_x, src_max_y, 
					 0, 0, 
					 0, 
					 0, 
					 mode, dynamic_cast<GlGfxSurface*>(surface), 0, 0, blend_factor );
	}
}

void Yagl::GlGfxDevice::blitScaled( int x, int y, float scale_x, float scale_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{
	if( surface )
		pushCommand( GLGFXDEVICE_BLIT_SCALED, (int)x, (int)y, 0, 0, 0, 0, 0, 0, scale_x, scale_y, 0, 0, mode, dynamic_cast<GlGfxSurface*>(surface), 0, 0, blend_factor );
}

void Yagl::GlGfxDevice::blitScaled( int x, int y, float scale_x, float scale_y, int src_min_x, int src_min_y, int src_max_x, int src_max_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{
	if( surface )
	{
		if( src_min_x > src_max_x )
		{
			int tmp = src_max_x;
			src_max_x = src_min_x;
			src_min_x = tmp;
		}
		
		if( src_min_y > src_max_y )
		{
			int tmp = src_max_y;
			src_max_y = src_min_y;
			src_min_y = tmp;
		}
		
		if( src_min_x < 0 || src_min_y < 0 || src_max_x > surface->getWidth()-1 || src_max_y > surface->getHeight()-1 )
			return;
			
		pushCommand( GLGFXDEVICE_BLIT_SCALED_RANGED, (int)x, (int)y, 0, 0, 
					 src_min_x, src_min_y, src_max_x, src_max_y, 
					 scale_x, scale_y, 
					 0, 
					 0, 
					 mode, dynamic_cast<GlGfxSurface*>(surface), 0, 0, blend_factor );
	}
}

void Yagl::GlGfxDevice::blitRotated( int x, int y, float angle, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{
	if( surface )
		pushCommand( GLGFXDEVICE_BLIT_ROTATED, (int)x, (int)y, 0, 0, 0, 0, 0, 0, 0, 0, angle, 0, mode, dynamic_cast<GlGfxSurface*>(surface), 0, 0, blend_factor );
}

void Yagl::GlGfxDevice::blitRotated( int x, int y, float angle, int src_min_x, int src_min_y, int src_max_x, int src_max_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )			
{
	if( surface )
	{
		if( src_min_x > src_max_x )
		{
			int tmp = src_max_x;
			src_max_x = src_min_x;
			src_min_x = tmp;
		}
		
		if( src_min_y > src_max_y )
		{
			int tmp = src_max_y;
			src_max_y = src_min_y;
			src_min_y = tmp;
		}
		
		if( src_min_x < 0 || src_min_y < 0 || src_max_x > surface->getWidth()-1 || src_max_y > surface->getHeight()-1 )
			return;

		pushCommand( GLGFXDEVICE_BLIT_ROTATED_RANGED, (int)x, (int)y, 0, 0, 
					 src_min_x, src_min_y, src_max_x, src_max_y, 
					 0, 0, 
					 angle, 
					 0, 
					 mode, dynamic_cast<GlGfxSurface*>(surface), 0, 0, blend_factor );
	}
}

void Yagl::GlGfxDevice::blitRotatedScaled( int x, int y, float rotate, float scale_x, float scale_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{
	if( surface )
		pushCommand( GLGFXDEVICE_BLIT_SCALED_ROTATED, (int)x, (int)y, 0, 0, 0, 0, 0, 0, scale_x, scale_y, rotate, 0, mode, dynamic_cast<GlGfxSurface*>(surface), 0, 0, blend_factor );
}

void Yagl::GlGfxDevice::blitRotatedScaled( int x, int y, float rotate, float scale_x, float scale_y, int src_min_x, int src_min_y, int src_max_x, int src_max_y, GfxSurface *surface, YAGL_BLIT_MODE mode, float blend_factor )
{
	if( surface )
	{
		if( src_min_x > src_max_x )
		{
			int tmp = src_max_x;
			src_max_x = src_min_x;
			src_min_x = tmp;
		}
		
		if( src_min_y > src_max_y )
		{
			int tmp = src_max_y;
			src_max_y = src_min_y;
			src_min_y = tmp;
		}
		
		if( src_min_x < 0 || src_min_y < 0 || src_max_x > surface->getWidth()-1 || src_max_y > surface->getHeight()-1 )
			return;

		pushCommand( GLGFXDEVICE_BLIT_SCALED_ROTATED_RANGED, (int)x, (int)y, 0, 0, 
					 src_min_x, src_min_y, src_max_x, src_max_y, 
					 scale_x, scale_y, 
					 rotate, 
					 0, 
					 mode, dynamic_cast<GlGfxSurface*>(surface), 0, 0, blend_factor );
	}
}

void Yagl::GlGfxDevice::printAt( const Yagl::String& text, int x, int y, int color, GfxFont* font, float blend_factor )
{
	if( !font )
		return;
	
	if( text.size() == 0 )
		return;	

	pushCommand( GLGFXDEVICE_PRINTAT_FONT, x, y, 0, 0, 
				 0, 0, 0, 0,
				 0, 0,
				 0,
				 color,
				 BLIT_ALPHAMASKED,
				 0,
				 dynamic_cast<GlGfxFont*>(font), 
				 text.c_str(), blend_factor );
}

void Yagl::GlGfxDevice::setupExecutionEnvironment( )
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );	
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glShadeModel( GL_SMOOTH );
	glDisable( GL_LINE_SMOOTH );
	
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );	
	last_texture_handle_ = 0xffffffff;
	
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();	
	glLoadIdentity();		
	glOrtho( 0, width_, height_, 0, -1.0f, 1.0f );	
	//glViewport( 0, 0, width_, height_);		
	glMatrixMode( GL_MODELVIEW );	
	glPushMatrix();	
	glLoadIdentity();

	glEnable( GL_SCISSOR_TEST );
	glScissor( clip_min_x_, height_ - clip_max_y_ - 1, clip_max_x_ - clip_min_x_ + 1, clip_max_y_ - clip_min_y_ + 1 );
}

void Yagl::GlGfxDevice::restoreOldEnvironment( )
{		
	glMatrixMode( GL_MODELVIEW );					
	glPopMatrix();				
	glMatrixMode(GL_PROJECTION);							
	glPopMatrix();					
	glPopAttrib( );					
}

void Yagl::GlGfxDevice::enableVSynch( ) 
{
	glfwSwapInterval( 1 );
	is_vsync_enabled_ = true;
}

void Yagl::GlGfxDevice::disableVSynch( )
{
	glfwSwapInterval( 0 );
	is_vsync_enabled_ = false;
}

bool Yagl::GlGfxDevice::isVSynchEnabled( )
{
	return is_vsync_enabled_;
}

void Yagl::GlGfxDevice::flushCommandBuffer( )
{
	executeCommands( );
}
/*
void Yagl::GlGfxDevice::lock( )
{
	if( is_locked_ )
		return;

	is_locked_ = true;

	if( width_ == 0 && height_ == 0 )
		return;

	if( framebuffer_width_ == 0 && framebuffer_height_ == 0 )
	{
		framebuffer_ = new unsigned int[ width_ * height_ ];
		framebuffer_width_ = width_;
		framebuffer_height_ = height_;
	}
	else
	{
		if( width_ != framebuffer_width_ || height_ != framebuffer_height_ )
		{
			delete[] framebuffer_;
			framebuffer_ = new unsigned int[ width_ * height_ ];
			framebuffer_width_ = width_;
			framebuffer_height_ = height_;
		}
	}	

	glPixelStorei( GL_PACK_SWAP_BYTES, 0 );
	glPixelStorei( GL_PACK_LSB_FIRST, 0 );
	glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
	glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_PACK_ALIGNMENT, 4 );
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
	
	glReadBuffer( GL_BACK_LEFT );	

	glReadPixels( 0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer_ );

}*/

unsigned int Yagl::GlGfxDevice::getPixel( int x, int y )
{
	unsigned int pixel = 0;

	if( x < 0 || x > width_ - 1 )
		return 0;

	if( y < 0 || y > height_ - 1 )
		return 0;

	glPushAttrib( GL_PIXEL_MODE_BIT );
	glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT );

	glPixelStorei( GL_PACK_SWAP_BYTES, 0 );
	glPixelStorei( GL_PACK_LSB_FIRST, 0 );
	glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
	glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_PACK_ALIGNMENT, 4 );

	glPixelTransferi( GL_MAP_COLOR, 0 );		
	glPixelTransferf( GL_RED_SCALE, 1.0f );
	glPixelTransferf( GL_GREEN_SCALE, 1.0f );
	glPixelTransferf( GL_BLUE_SCALE, 1.0f );
	glPixelTransferf( GL_ALPHA_SCALE, 1.0f );	
	glPixelTransferf( GL_RED_BIAS, 0.0f );
	glPixelTransferf( GL_GREEN_BIAS, 0.0f );
	glPixelTransferf( GL_BLUE_BIAS, 0.0f );
	glPixelTransferf( GL_ALPHA_BIAS, 0.0f );

	glReadPixels( x, height_ - 1 - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel );

	glPopClientAttrib();
	glPopAttrib();
	pixel = ( pixel & 0xff000000 ) | ( pixel & 0xff ) << 16 | ( pixel & 0xff00 ) | ( pixel & 0xff0000 ) >> 16;
	return pixel;
}

void Yagl::GlGfxDevice::putPixel( int x, int y, unsigned int color )
{	
	if( x < 0 || x > width_ - 1 )
		return;

	if( y < 0 || y > height_ - 1 )
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

	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();	
	glLoadIdentity();		
	glOrtho( 0, width_, height_, 0, -1.0f, 1.0f );		
	glMatrixMode( GL_MODELVIEW );	
	glPushMatrix();	
	glLoadIdentity();
	glViewport( 0, 0, width_, height_);		

	color = ( color & 0xff000000 ) | ( color & 0xff ) << 16 | ( color & 0xff00 ) | ( color & 0xff0000 ) >> 16;

	glRasterPos2f( x, y + 1);
	glDrawPixels( 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color );

	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glPopClientAttrib();
	glPopAttrib();
}

void Yagl::GlGfxDevice::setTintingColor( unsigned int color )
{
	if (tinting_color_ != color)
		pushCommand( GLGFXDEVICE_TINTING_COLOR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, BLIT_SOLID, 0, 0, 0, 1.0 );
	tinting_color_ = color;
}
				
unsigned int Yagl::GlGfxDevice::getTintingColor( )
{
	return tinting_color_;
}
