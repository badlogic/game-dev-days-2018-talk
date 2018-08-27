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

#include "netlib/message.h"
#include <cassert>

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

Yagl::Message::Message( )
{
	buffer_ = 0;
	buffer_size_ = 0;
	message_size_ = 0;
	sender_address_ = "";
	sender_port_ = 0;
}

Yagl::Message::Message( const Message& message )
{
	if( buffer_ )
		delete[] buffer_;
		
	buffer_ = 0;
	buffer_size_ = 0;
	message_size_ = 0;
	sender_address_ = "";
	sender_port_ = 0;
	
	if( message.buffer_ )
	{
		buffer_ = new char[message.buffer_size_];
		assert( buffer_ != 0 );
		buffer_size_ = message.buffer_size_;
		for( unsigned int i = 0; i < message.message_size_; i++ )
			buffer_[i] = message.buffer_[i];
		message_size_ = message.message_size_;
		sender_address_ = message.sender_address_;
		sender_port_ = message.sender_port_;		
	}
}

Yagl::Message::~Message( )
{
	if( buffer_ )
		delete[] buffer_;
}

void Yagl::Message::setMessageSize( unsigned int size )
{
	if( size > buffer_size_ )
		message_size_ = buffer_size_;
	else
		message_size_ = size;
}

void Yagl::Message::setBufferSize( unsigned int size )
{
	if( buffer_ )
		delete[] buffer_;
	
	buffer_ = 0;
	buffer_size_ = 0;
	message_size_ = 0;
	sender_address_ = "";
	sender_port_ = 0;	
	
	if( size == 0 ) 
		return;
		
	buffer_ = new char[size];
	assert( buffer_ != 0 );
	buffer_size_ = size;
}

void Yagl::Message::enlargeBuffer( unsigned int additional_bytes )
{
	if( !buffer_ )
	{
		buffer_ = new char[additional_bytes];
		assert( buffer_ != 0 );
		buffer_size_ = additional_bytes;
		return;
	}
	
	if( (int)additional_bytes <= 0 )
		return;
	
	char* tmp_buffer = new char[(buffer_size_ + additional_bytes)*2];
	assert( tmp_buffer != 0 );
	for( unsigned int i = 0; i < buffer_size_; i++ )
		tmp_buffer[i] = buffer_[i];
	buffer_size_ = (buffer_size_ + additional_bytes)*2;
	delete[] buffer_;
	buffer_ = tmp_buffer;
}

void Yagl::Message::setSenderAddress( std::string address )
{
	sender_address_ = address;
}

void Yagl::Message::setSenderPort( unsigned short port )
{
	sender_port_ = port;
}

char* Yagl::Message::getBufferPointer( )
{
	return buffer_;
}

unsigned int Yagl::Message::getBufferSize( )
{
	return buffer_size_;
}

unsigned int Yagl::Message::getMessageSize( )
{
	return message_size_;
}

std::string& Yagl::Message::getSenderAddress( )
{
	return sender_address_;
}

unsigned short Yagl::Message::getSenderPort( )
{
	return sender_port_;
}

void Yagl::Message::insert( const void* data, unsigned int data_size )
{
	setBufferSize( data_size );
	for( unsigned int i = 0; i < data_size; i++ )
		buffer_[i] = ((char*)data)[i];
	message_size_ = data_size;
	buffer_size_ = data_size;
}

void Yagl::Message::append( const void* data, unsigned int data_size )
{
	unsigned int free_bytes = buffer_size_ - message_size_;
	unsigned int needed_bytes = data_size - free_bytes;
	
	enlargeBuffer( needed_bytes );
	
	for( unsigned int i = message_size_; i < message_size_ + data_size; i++ )
	{
		buffer_[i] = ((char*)data)[i - message_size_];
	}
	message_size_ += data_size;
}
