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

#include "netlib/netdevice.h"
#include "log/log.h"

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

Yagl::NetDevice Yagl::NetDevice::instance_;


void Yagl::NetDevice::initialize( )
{
	if( initialized_ )
		return;
#ifdef TARGET_WIN32		
	WSADATA wsaData;
	if( WSAStartup(MAKEWORD( 1, 1 ), &wsaData) != 0 )
	{
		Yagl::Log::print( "NetDevice: WSAStartup failed\n" );
		initialized_ = false;
	}
	else
	{
		initialized_ = true;
	}
#endif

	Yagl::Log::print( "NetDevice: initialized\n" );
	initialized_ = true;
}

void Yagl::NetDevice::deinitialize()
{
	if( !initialized_ )
		return;
		
	destroyAllListeningConnections();
	destroyAllBiDirectionalConnections();

#ifdef TARGET_WIN32		
	WSACleanup();
#endif
	initialized_ = false;
	Yagl::Log::print( "NetDevice: deinitialized\n" );
}

Yagl::NetDevice& Yagl::NetDevice::getInstance()
{
	return instance_;
}

Yagl::ListeningConnection* Yagl::NetDevice::createListeningConnection ( )
{
	ListeningConnection *connection = new ListeningConnection();
	listening_connections_lock_.lock();
	listening_connections_.push_back( connection );
	listening_connections_lock_.unlock();
	
	Yagl::Log::print( "NetDevice: created a listening connection at " );
	Yagl::Log::print( connection );
	Yagl::Log::print( "\n" );
	
	return connection;
}

void Yagl::NetDevice::destroyListeningConnection ( ListeningConnection* connection )
{
	listening_connections_lock_.lock();
	std::list<ListeningConnection*>::iterator iter;
	for (iter=listening_connections_.begin(); iter != listening_connections_.end(); iter++)
	{
		if( (*iter) == connection )
		{
			delete *iter;
			Yagl::Log::print( "NetDevice: destroyed listening connection at " );
			Yagl::Log::print( *iter );
			Yagl::Log::print( "\n" );
			listening_connections_.erase( iter );
			listening_connections_lock_.unlock();
			return;
		}
	}
	listening_connections_lock_.unlock();
}

void Yagl::NetDevice::destroyAllListeningConnections ( )
{
	listening_connections_lock_.lock();
	
	while( listening_connections_.size() != 0 )
	{
		delete listening_connections_.front();
		Yagl::Log::print( "NetDevice: destroyed listening connection at " );
		Yagl::Log::print( listening_connections_.front() );
		Yagl::Log::print( "\n" );
		listening_connections_.pop_front();
	}

	listening_connections_lock_.unlock();
}

Yagl::BiDirectionalConnection* Yagl::NetDevice::createBiDirectionalConnection ( )
{
	BiDirectionalConnection *connection = new BiDirectionalConnection();
	bidirectional_connections_lock_.lock();
	bidirectional_connections_.push_back( connection );
	bidirectional_connections_lock_.unlock();
	Yagl::Log::print( "NetDevice: created a bidirectional connection at " );
	Yagl::Log::print( connection );
	Yagl::Log::print( "\n" );
	return connection;
}

void Yagl::NetDevice::destroyBiDirectionalConnection ( BiDirectionalConnection* connection )
{
	bidirectional_connections_lock_.lock();
	std::list<BiDirectionalConnection*>::iterator iter;
	for (iter=bidirectional_connections_.begin(); iter != bidirectional_connections_.end(); iter++)
	{
		if( (*iter) == connection )
		{
			delete *iter;
			Yagl::Log::print( "NetDevice: destroyed bidirectional connection at " );
			Yagl::Log::print( *iter );
			Yagl::Log::print( "\n" );
			bidirectional_connections_.erase( iter );
			bidirectional_connections_lock_.unlock();
			return;
		}
	}
	bidirectional_connections_lock_.unlock();
}

void Yagl::NetDevice::destroyAllBiDirectionalConnections ( )
{
	bidirectional_connections_lock_.lock();
	
	while( bidirectional_connections_.size() != 0 )
	{
		delete bidirectional_connections_.front();
		Yagl::Log::print( "NetDevice: destroyed bidirectional connection at " );
		Yagl::Log::print( bidirectional_connections_.front() );
		Yagl::Log::print( "\n" );
		bidirectional_connections_.pop_front();
	}

	bidirectional_connections_lock_.unlock();
}

void Yagl::NetDevice::getIPAdresses( std::list<std::string> &addresses )
{
}

void Yagl::NetDevice::resolveAddress( const Yagl::String& address, std::string &result )
{
	struct hostent* host_info;
	
	if( (host_info = gethostbyname( address.c_str() )) == 0 )
	{
		Yagl::Log::print( "NetDevice: couldn't resolve address " );
		Yagl::Log::print( address );
		Yagl::Log::print( "\n" );
		result = "";
		return;
	}
	
	result = inet_ntoa( *((struct in_addr *)host_info->h_addr) );
}
