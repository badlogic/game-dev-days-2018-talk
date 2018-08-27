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

#include "netlib/messagereceiver.h"

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

Yagl::MessageReceiver::MessageReceiver( )
{
}

Yagl::MessageReceiver::~MessageReceiver( )
{
	while( messages_.size() != 0 )
	{
		delete messages_.front();
		messages_.pop_front();
	}
}

void Yagl::MessageReceiver::pushMessage( Message& message )
{
	Message *tmp_message = new Message( );
	
	(*tmp_message) = message;
	  
	message_list_lock_.lock( );
	messages_.push_back( tmp_message );
	message_list_lock_.unlock( );
	receivedMessage();
	
}

Yagl::Message* Yagl::MessageReceiver::popMessage( )
{
	Message *tmp_message = 0;
	
	message_list_lock_.lock();
	if( messages_.size() == 0 )
	{
		message_list_lock_.unlock();
		return 0;
	}
	else
	{
		tmp_message = messages_.front();
		messages_.pop_front();
	}
	message_list_lock_.unlock();
	return tmp_message;
}

void Yagl::MessageReceiver::receivedMessage( )
{
}
