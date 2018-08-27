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

#include "netlib/listeningconnection.h"
#include "netlib/netdevice.h"
#include "log/log.h"

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

bool Yagl::ListeningConnection::isAlive( )
{
	if( is_listening_ )
		return true;
	else
		return false;
}

Yagl::ListeningConnection::~ListeningConnection( )
{
	if( is_listening_ )
	{
		stop();
	}
	
#ifdef TARGET_WIN32
	closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
	close( socket_ );
#endif
	
	Yagl::Log::print( "listening connection listening on port " );
	Yagl::Log::print( local_port_ );
	Yagl::Log::print( " destroyed\n" );
}

void Yagl::ListeningConnection::setLocalPort( unsigned short port )
{
	//
	// if we are listening we can't change the port
	//
	if( is_listening_ )	
		return;
		
	local_port_ = port;
	
}

unsigned short Yagl::ListeningConnection::getLocalPort( )
{
	return local_port_;
}

void Yagl::ListeningConnection::registerBiDirectionalConnectionReceiver( BiDirectionalConnectionReceiver *receiver )
{	
	receivers_list_lock_.lock();
	
	//
	// check wheter the receiver is already in the list
	//
	std::list<BiDirectionalConnectionReceiver*>::const_iterator iter;
	for (iter=receivers_.begin(); iter != receivers_.end(); iter++)
	{
		if( (*iter) == receiver )
		{
			receivers_list_lock_.unlock();
			return;
		}
	}
	
	receivers_.push_back( receiver );
	receivers_list_lock_.unlock();
}

void Yagl::ListeningConnection::unregisterBiDirectionalConnectionReceiver( BiDirectionalConnectionReceiver *receiver )
{
	receivers_list_lock_.lock();
	receivers_.remove( receiver );
	receivers_list_lock_.unlock();
}

bool Yagl::ListeningConnection::startListening( )
{
	//
	// if we are already listening we return false
	//
	if( is_listening_ )
	{
		Yagl::Log::print( "ListeningConnection: can't start listening as connection is already listening\n" );
		return false;
	}
	
	//
	// if there was already a socket assigned to this we close it
	//
	if( socket_ != 0 )
	{
#ifdef TARGET_WIN32
		closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
		close( socket_ );
#endif
	}
	
	//
	//  retrieve a new socket and set it so it's port can be reused
	//
	socket_ = socket( PF_INET, SOCK_STREAM, 0 );
	if( socket_ == -1 )
	{
		Yagl::Log::print( "ListeningConnection: couldn't acquire listening socket\n" );
		socket_ = 0;
		return false;
	}
	
	int yes = 1;
	if (setsockopt(socket_,SOL_SOCKET,SO_REUSEADDR,(const char*)&yes,sizeof(int)) == -1) 
	{
		Yagl::Log::print( "ListeningConnection: couldn't reuse port of listening connection\n" );
#ifdef TARGET_WIN32
		closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
		close( socket_ );
#endif
		socket_ = 0;
		return false;
	}
	
	//
	// try to bind the socket to the specified local port
	//
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons( local_port_ );
	local_addr.sin_addr.s_addr = INADDR_ANY;
	memset( &( local_addr.sin_zero ), 0, 8 );
	if( bind( socket_, ( sockaddr* )&local_addr, sizeof( sockaddr ) ) == -1 )
	{
		Yagl::Log::print( "ListeningConnection: couldn't bind listening connection to port " );
		Yagl::Log::print( local_port_ );
		Yagl::Log::print( "\n" );
#ifdef TARGET_WIN32
		closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
		close( socket_ );
#endif
		socket_ = 0;
		return false;
	}
	
	
	//
	// now we set the connection to be a listening connection 
	// and are done
	//
	if( listen( socket_, 20 ) == -1 )
	{		
		Yagl::Log::print( "ListeningConnection: couldn't make socket listening\n" );		
#ifdef TARGET_WIN32
		closesocket( socket_ );
#endif 
#ifdef TARGET_LINUX
		close( socket_ );
#endif
		socket_ = 0;
		return false;
	}
	
	//
	// start the listening thread
	//
	is_listening_ = true;
	start();
	
	Yagl::Log::print( "ListeningConnection: listening connection at " );		
	Yagl::Log::print( this );
	Yagl::Log::print( " started listening\n" );
	return true;
}

void Yagl::ListeningConnection::stopListening( )
{
	stop();
	is_listening_ = false;
	Yagl::Log::print( "ListeningConnection: listening connection at " );		
	Yagl::Log::print( this );
	Yagl::Log::print( " stopped listening\n" );
}

void Yagl::ListeningConnection::listening( )
{
	timeval timeout;
	fd_set listening_desc;
	sockaddr_in remote_addr;
	int addr_size = sizeof( sockaddr_in );
	int new_socket = 0;
	
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	
	bool got_receivers = false;
	
	while( !got_receivers )
	{	
		receivers_list_lock_.lock();
		if( receivers_.size() != 0 )
			got_receivers = true;
		receivers_list_lock_.unlock();
		Thread::yield();
		if( status_ != RUNNING )
		{			
			return;
		}
		Thread::sleep(1);
	}
	
	while( status_ == RUNNING )
	{		
		FD_ZERO( &listening_desc );
		FD_SET( (unsigned)socket_, &listening_desc );
		if( select( socket_ + 1, &listening_desc, 0, 0, &timeout ) == -1 )
		{
			Yagl::Log::print( "ListeningConnection: listening connection at " );		
			Yagl::Log::print( this );
			Yagl::Log::print( " couldn't poll socket\n" );
#ifdef TARGET_WIN32
			closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
			close( socket_ );
#endif
			socket_ = 0;
			return;
		}
		
		if( FD_ISSET( socket_, &listening_desc ) )
		{			
		
#ifdef TARGET_WIN32
			if( ( new_socket = accept( socket_, (sockaddr*)&remote_addr, &addr_size ) ) == -1 )
#endif
#ifdef TARGET_LINUX
			if( ( new_socket = accept( socket_, (sockaddr*)&remote_addr, (socklen_t*)&addr_size ) ) == -1 )
#endif
			{
				Yagl::Log::print( "ListeningConnection: listening connection at " );		
				Yagl::Log::print( this );
				Yagl::Log::print( " couldn't accept incoming connection\n" );
#ifdef TARGET_WIN32
				closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
				close( socket_ );
#endif
				socket_ = 0;
				return;
			}
				
			BiDirectionalConnection *connection = NetDevice::getInstance().createBiDirectionalConnection();
			connection->setSocket( new_socket );
			connection->setRemoteAddress( inet_ntoa(remote_addr.sin_addr) );
			connection->setRemotePort( remote_addr.sin_port );
			connection->start();
			
			std::list<BiDirectionalConnectionReceiver*>::iterator iter = receivers_.begin();
			receivers_list_lock_.lock();
			if( receivers_.size() == 0 )
			{
				Yagl::Log::print( "ListeningConnection: listening connection at " );		
				Yagl::Log::print( this );
				Yagl::Log::print( " received connection but no connection receivers are registered\n" );
			}
			else
			{
				while( iter != receivers_.end() )
				{
					(*iter)->pushBiDirectionalConnection( connection );
					iter++;
				}
			}
			receivers_list_lock_.unlock();
		}
		Thread::sleep(1);
	}
	
#ifdef TARGET_WIN32	
	closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
	close( socket_ );
#endif
	socket_ = 0;
}

void* Yagl::ListeningConnection::run( )
{
	listening();
	Yagl::Log::print( "ListeningConnection: connection at " );
	Yagl::Log::print( this );
	Yagl::Log::print( " terminated listening thread\n" );
	status_ = STOPPED;
	is_listening_ = false;
	return 0;
}
