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
#ifdef _WIN32
#include <windows.h>
#endif
#include "gfx/glgfxfont.h"
#include "gfx/glgfxdevice.h"
#include <GL/gl.h>
#include "log/log.h"
#include "decoder/bmpdecoder.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

int Yagl::GlGfxFont::num_font_instances_ = 0;
FT_Library Yagl::GlGfxFont::library_;

Yagl::GlGfxGlyph::GlGfxGlyph( )
{	
	bitmap_ = 0;	
	width_ = 0;
	height_ = 0;
	pitch_ = 0;
	advance_ = 0;
	left_ = 0;
	bitmap_top_ = 0;
	texture_ = 0;
}

Yagl::GlGfxGlyph::~GlGfxGlyph( )
{	
	if( bitmap_ )
		delete[] bitmap_;
		
	if( texture_ )
	{
		delete texture_;
		/*Yagl::Log::print( "GfxDevice: destroyed surface at " );	
		Yagl::Log::print( (int)texture_ );
		Yagl::Log::print( "\n" );*/
	}
}

Yagl::GlGfxFont::GlGfxFont( )
{
	if( num_font_instances_ == 0 )
	{
		FT_Init_FreeType( &library_ );
	}
	num_font_instances_++;
	
	for( int i = 0; i < 256; i++ )
		glyphs_[i] = 0;
		
	is_bold_ = false;
	is_italic_ = false;	
	face_ = 0;
	clearGlyphCache( );
}

Yagl::GlGfxFont::GlGfxFont( const GlGfxFont& font )
{
}

Yagl::GlGfxFont::~GlGfxFont( )
{
	if( face_ )
		FT_Done_Face( face_ );
		
	num_font_instances_--;
	if( num_font_instances_ == 0 )
	{
		FT_Done_FreeType( library_ );
	}
	
	clearGlyphCache();		
}

bool Yagl::GlGfxFont::loadFont( const Yagl::String& filename )
{
	int error = 0;	
	
	if( face_ )
	{
		FT_Done_Face( face_ );
		face_ = 0;
	}
		
	error = FT_New_Face( library_, filename.c_str(), 0, &face_ );
	if( error )
	{
		Yagl::Log::print( "Font: couldn't load font from file " );
		Yagl::Log::print( filename );
		Yagl::Log::print( "\n" );
		face_ = 0;
		return false;
	}
	
	if( !FT_IS_SCALABLE( face_ ) )
	{
		FT_Done_Face( face_ );
		face_ = 0;
		Yagl::Log::print( "Font: couldn't load font from file " );
		Yagl::Log::print( filename );
		Yagl::Log::print( ", font is not scalable\n" );
		return false;
	}
		
	setSize( 10 );	

	Yagl::Log::print( "Font: loaded font from file " );
	Yagl::Log::print( filename );
	Yagl::Log::print( "\n" );	
		
	return true;
}


void Yagl::GlGfxFont::setSize( unsigned int pt )
{
	if( pt <= 0 || pt > 300 )
		return;
	
	FT_Set_Pixel_Sizes( face_, 0, pt );
	
	precacheGlyphs( );
	return;
}

void Yagl::GlGfxFont::printAt( const Yagl::String& text, int x, int y, unsigned int color, GfxDevice* device )
{		
	
	double u, v;
	int old_x = x;
	y++;	
		
	for( unsigned int i=0 ; i < text.size(); i++ )
	{			
		if( text.c_str()[i] == '\n' )
		{
			x = old_x;
			y += max_bitmap_height_;
			continue;
		}
	
		GlGfxGlyph* glyph = getGlyph( text.c_str()[i] );		
		
		if( glyph )
		{
			x += glyph->left_;
			if( glyph->texture_ )
			{				
			
				glBindTexture( GL_TEXTURE_2D, glyph->texture_->getTextureHandle( ) );	
				glBegin( GL_QUADS );
				glyph->texture_->getTextureCoordinates( 0, 0, u, v );
				glTexCoord2d( u, v );
				glVertex2i( x, y + ( max_height_ - glyph->bitmap_top_ ) );
				
				glyph->texture_->getTextureCoordinates( glyph->width_, 0, u, v );
				glTexCoord2d( u, v );
				glVertex2i( x + glyph->width_, y + ( max_height_ - glyph->bitmap_top_ ) );
				
				glyph->texture_->getTextureCoordinates( glyph->width_, glyph->height_, u, v );
				glTexCoord2d( u, v );	
				glVertex2i( x + glyph->width_, y + glyph->height_ + ( max_height_ - glyph->bitmap_top_ ) );
				
				glyph->texture_->getTextureCoordinates( 0, glyph->height_, u, v );
				glTexCoord2d( u, v );
				glVertex2i( x, y + glyph->height_ + ( max_height_ - glyph->bitmap_top_ ) );
				glEnd( );
			}
	
			x += glyph->advance_ - glyph->left_;
		}
		else
		{
			Yagl::Log::print( "Font: font at " );
			Yagl::Log::print( this );
			Yagl::Log::print( " has no glyph for chacter" );
			Yagl::Log::print( text.c_str()[i] );
			Yagl::Log::print( "\n" );
		}
	}
}

void Yagl::GlGfxFont::printAt( const Yagl::String& text_y, int x, int y, unsigned int color, GfxSurface* surface )
{
	printf( "printing to surface???" );
}

void Yagl::GlGfxFont::clearGlyphCache( )
{
	for( int i = 0; i < 256; i++ )
	{
		if( glyphs_[i] )
		{
			delete glyphs_[i];
		}
		
		glyphs_[i] = 0;
	}	
	
	Yagl::Log::print( "Font: font at " );
	Yagl::Log::print( this );
	Yagl::Log::print( " cleared glyph cache\n" );
}

void Yagl::GlGfxFont::precacheGlyphs( )
{
	FT_GlyphSlot curr_glyph = face_->glyph;

	max_bitmap_height_ = 0;
	clearGlyphCache( );	
	long used_bytes = 0;
	
	max_height_ = 0;
	
	for( int i = 0; i < 256; i++ )
	{
		unsigned int glyph_index;
		
		if( i < 0x20 || i > 0x7e )
			if( i < 0xa0 || i > 0xff )
				continue;
		
		glyph_index = FT_Get_Char_Index( face_, i );
		if( glyph_index == 0 )
		{
			continue;
		}
		
		if( FT_Load_Glyph( face_, glyph_index, FT_LOAD_DEFAULT /*FT_LOAD_TARGET_LIGHT */) )
		{
			Yagl::Log::print( "Font: font at " );
			Yagl::Log::print( this );
			Yagl::Log::print( " couldn't load glyph " );
			Yagl::Log::print( i );
			Yagl::Log::print( "\n" );
			continue;
		}
		
		
		if( FT_Render_Glyph( face_->glyph, FT_RENDER_MODE_NORMAL/*FT_RENDER_MODE_LIGHT */ ) )
		{
			Yagl::Log::print( "Font: font at " );
			Yagl::Log::print( this );
			Yagl::Log::print( " couldn't render glyph " );
			Yagl::Log::print( i );
			Yagl::Log::print( "\n" );
			continue;
		}
		
		if( curr_glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY )
		{
			Yagl::Log::print( "Font: font at " );
			Yagl::Log::print( this );
			Yagl::Log::print( " couldn't load glyph " );
			Yagl::Log::print( i );
			Yagl::Log::print( " because pixelmode of glyph is not 8-bit\n" );
			continue;
		}
		
		
		if( curr_glyph->bitmap.buffer == 0 )
		{			
			glyphs_[i] = new GlGfxGlyph( );			
			glyphs_[i]->advance_ = curr_glyph->advance.x / 64;
		}
		else
		{
			glyphs_[i] = new GlGfxGlyph( );
			glyphs_[i]->width_ = curr_glyph->bitmap.width;
			glyphs_[i]->height_ = curr_glyph->bitmap.rows;
			glyphs_[i]->pitch_ = curr_glyph->bitmap.pitch;
			glyphs_[i]->advance_ = curr_glyph->advance.x / 64;
			glyphs_[i]->bitmap_ = new char[glyphs_[i]->height_ * glyphs_[i]->width_];
			glyphs_[i]->bitmap_top_ = curr_glyph->bitmap_top;			
			//glyphs_[i]->left_ = curr_glyph->metrics.horiBearingX / 64;
			glyphs_[i]->left_ = curr_glyph->bitmap_left;
			
			for( int row = 0; row < glyphs_[i]->height_; row++ )
				for( int col = 0; col < glyphs_[i]->pitch_; col++ )
				{
					int color = curr_glyph->bitmap.buffer[col + row * glyphs_[i]->pitch_];
					glyphs_[i]->bitmap_[col + row * glyphs_[i]->pitch_] = color;
				}
				
			if( curr_glyph->metrics.horiBearingY > max_height_ )
			{				
				max_height_ = glyphs_[i]->bitmap_top_;			
			}
				
			if( curr_glyph->bitmap.rows > max_bitmap_height_ )
				max_bitmap_height_ = curr_glyph->bitmap.rows;
						
			used_bytes += glyphs_[i]->height_ * glyphs_[i]->pitch_;
		}
		
	}	
	
	//
	//	now let's be silly and create a gl texture out of all the glyphs
	// 	first we need to check how big the texture has to be
	//			
		
	for( int curr_char = 0; curr_char < 256; curr_char++ )
	{
		if( !glyphs_[curr_char] )
			continue;
		if( !glyphs_[curr_char]->bitmap_ )
			continue;		
	
		//glyphs_[curr_char]->texture_ = dynamic_cast<GlGfxSurface*>(dynamic_cast<GlGfxDevice&>(Yagl::GlGfxDevice::getInstance()).createSurfaceNoRegistering( ));
		glyphs_[curr_char]->texture_ = reinterpret_cast<GlGfxDevice&>(Yagl::GlGfxDevice::getInstance()).createSurfaceNoRegistering( );
		if( !glyphs_[curr_char]->texture_ )
		{
			Yagl::Log::print( "Font: couldn't create texture from font\n" );			
			clearGlyphCache( );
			return;
		}
		
		glyphs_[curr_char]->texture_->convertGRAYSCALE8toARGB32( (unsigned char**)&glyphs_[curr_char]->bitmap_, glyphs_[curr_char]->width_ * glyphs_[curr_char]->height_ );					
	
		int *bitmap_32 = (int*)glyphs_[curr_char]->bitmap_;
		for( int i = 0; i < glyphs_[curr_char]->width_ * glyphs_[curr_char]->height_; i++ )
			bitmap_32[i] = ( bitmap_32[i] & 0xff ) << 24 | 0xffffff;
				
		glyphs_[curr_char]->texture_->loadFromMemory( (unsigned char*)glyphs_[curr_char]->bitmap_, glyphs_[curr_char]->width_, glyphs_[curr_char]->height_, BITMAP_FORMAT_ARGB32 );
			
		delete[] glyphs_[curr_char]->bitmap_;
		glyphs_[curr_char]->bitmap_ = 0;
	}
		
	Yagl::Log::print( "Font: font at " );
	Yagl::Log::print( this );
	Yagl::Log::print( " precached first 256 glyphs\n" );
	max_bitmap_height_ += max_bitmap_height_ / 6;
}

Yagl::GlGfxGlyph* Yagl::GlGfxFont::getGlyph( unsigned char asc )
{
	return glyphs_[asc];
}

Yagl::GfxSurface* Yagl::GlGfxFont::getGlyphTexture( unsigned char glyph )
{
	return glyphs_[glyph]->texture_;
}

void Yagl::GlGfxFont::getTextDimensions( const Yagl::String& text, unsigned int &width, unsigned int &height )
{
	unsigned int old_width = 0;


	width = 0;
	height = 0;

	if( text.size() == 0 )
	{
		return;
	}	

	height = max_bitmap_height_;

	for( unsigned int i=0 ; i < text.size(); i++ )
	{			
		if( text.c_str()[i] == '\n' )
		{
			if( old_width < width )
				old_width = width;

			width = 0;
			height += max_bitmap_height_;
			continue;
		}
	
		GlGfxGlyph* glyph = getGlyph( text.c_str()[i] );		
		
		if( glyph )
		{			
			width += glyph->advance_;			
		}
	}

	if( old_width > width )
		width = old_width;
}

// added - [coderjeff 2006-feb-6]
int Yagl::GlGfxFont::getAdvance( unsigned char asc )
{
	GlGfxGlyph* glyph = getGlyph( asc );		
	if( glyph )
		return( glyph->advance_ );

	return(0);
}

// added - [coderjeff 2006-feb-6]
int Yagl::GlGfxFont::getLeft( unsigned char asc )
{
	GlGfxGlyph* glyph = getGlyph( asc );		
	if( glyph )
		return( glyph->left_ );

	return(0);
}
