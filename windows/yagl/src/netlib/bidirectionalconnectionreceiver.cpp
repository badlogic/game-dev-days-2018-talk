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

#include "netlib/bidirectionalconnectionreceiver.h"

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

Yagl::BiDirectionalConnectionReceiver::BiDirectionalConnectionReceiver( )
{
}
			
void Yagl::BiDirectionalConnectionReceiver::pushBiDirectionalConnection( BiDirectionalConnection* connection )
{
	if( connection == 0 )
	{
		return;
	}
	connections_list_lock_.lock();
	connections_.push_back(connection);
	connections_list_lock_.unlock();
	receivedConnection();
}

Yagl::BiDirectionalConnection* Yagl::BiDirectionalConnectionReceiver::popBiDirectionalConnection( )
{
	BiDirectionalConnection *connection;
	
	connections_list_lock_.lock();
	if( connections_.size() == 0 )
	{
		connections_list_lock_.unlock();
		return 0;
	}
	
	connection = connections_.front();
	connections_.pop_front();
	connections_list_lock_.unlock();
	return connection;
}

void Yagl::BiDirectionalConnectionReceiver::receivedConnection( )
{
}
