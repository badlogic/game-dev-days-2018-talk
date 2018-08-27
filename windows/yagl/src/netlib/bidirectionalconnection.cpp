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

#include "netlib/bidirectionalconnection.h"
#include "netlib/netdevice.h"
#include "log/log.h"

#ifdef MEMLEAK
#include "../memoryleakdetector/debug_new.h"
#endif

bool Yagl::BiDirectionalConnection::isAlive( )
{
	if( is_receiving_ )
		return true;
	else
		return false;
}

void Yagl::BiDirectionalConnection::setSocket( int socket )
{
	if( is_receiving_ )
		return;
	
	socket_ = socket;
	linger ling;
	ling.l_onoff = 1;
	ling.l_linger = 10;
	if( setsockopt( socket_, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof( linger ) ) == -1 )
	{
		Yagl::Log::print( "BiDirectionalConnection: connection at " );
		Yagl::Log::print( this );
		Yagl::Log::print( " couldn't set lingering for socket\n" );
#ifdef TARGET_WIN32
		closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
		close( socket_ );
#endif
		socket_ = 0;
		return;
	}
}

void Yagl::BiDirectionalConnection::setLocalPort( unsigned short port )
{

	if( is_receiving_ )
		return;
	
	local_port_ = port;
}

void Yagl::BiDirectionalConnection::setRemoteAddress( std::string address )
{
	if( is_receiving_ )
		return;
		
	remote_address_ = address;
}

void Yagl::BiDirectionalConnection::setRemoteAddress( std::string &address )
{
	if( is_receiving_ )
		return;
		
	remote_address_ = address;
}


void Yagl::BiDirectionalConnection::setRemotePort( unsigned short port )
{
	//
	// if we already receive this will not take effect
	//
	if( is_receiving_ )
	{
		return;
	}
	
	remote_port_ = port;
}

bool Yagl::BiDirectionalConnection::connect( const Yagl::String address, unsigned short port )
{
	//
	//	if we are receiving we simple return with false
	//
	if( is_receiving_ )
	{
		Yagl::Log::print( "BiDirectionalConnection: connection at " );
		Yagl::Log::print( this );
		Yagl::Log::print( " can't connect to new address as old connection is still established\n" );
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
		Yagl::Log::print( "BiDirectionalConnection: connection at " );
		Yagl::Log::print( this );
		Yagl::Log::print( " couldn't acquire bidirectional socket\n" );
		socket_ = 0;
		return false;
	}
	
	int yes = 1;
	if (setsockopt(socket_,SOL_SOCKET,SO_REUSEADDR,(const char*)&yes,sizeof(int)) == -1) 
	{
		Yagl::Log::print( "BiDirectionalConnection: connection at " );
		Yagl::Log::print( this );
		Yagl::Log::print( " couldn't reuse port of bidirectional connection\n" );
#ifdef TARGET_WIN32
		closesocket( socket_ );
#endif 
#ifdef TARGET_LINUX
		close( socket_ );
#endif
		socket_ = 0;
		return false;
	}
		 
	//setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes));
	
	linger ling;
	ling.l_onoff = 1;
	ling.l_linger = 10;
	
	if( setsockopt( socket_, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof( linger ) ) == -1 )
	{
		Yagl::Log::print( "BiDirectionalConnection: connection at " );
		Yagl::Log::print( this );
		Yagl::Log::print( " couldn't set lingering for socket\n" );
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
		Yagl::Log::print( "BiDirectionalConnection: connection at " );
		Yagl::Log::print( this );
		Yagl::Log::print( " couldn't bind to port " );
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
	// now let's try to connect to the specified address and port...
	//
	NetDevice::getInstance().resolveAddress(address, remote_address_);
	remote_port_ = port;
	
	sockaddr_in remote_addr;
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(remote_port_);
	remote_addr.sin_addr.s_addr = inet_addr( remote_address_.c_str() );
	memset( &(remote_addr.sin_zero), 0, 8 );
	
	if( ::connect( socket_, (sockaddr*)&remote_addr, sizeof( sockaddr ) ) == -1 )
	{
		Yagl::Log::print( "BiDirectionalConnection: connection at " );
		Yagl::Log::print( this );
		Yagl::Log::print( " couldn't connect to " );
		Yagl::Log::print( remote_address_ );
		Yagl::Log::print( " at port " );
		Yagl::Log::print( remote_port_ );
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
	// start receiving messages and return with a success message...
	//
	is_receiving_ = true;
	start();
	
	Yagl::Log::print( "BiDirectionalConnection: connection at " );
	Yagl::Log::print( this );
	Yagl::Log::print( " connected to " );
	Yagl::Log::print( remote_address_ );
	Yagl::Log::print( " at port " );
	Yagl::Log::print( remote_port_ );
	Yagl::Log::print( "\n" );		
	
	return true;
}

void Yagl::BiDirectionalConnection::registerMessageReceiver( MessageReceiver* receiver )
{
	receivers_list_lock_.lock();
	
	//
	// check wheter the receiver is already in the list
	//
	std::list<MessageReceiver*>::const_iterator iter;
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

void Yagl::BiDirectionalConnection::unregisterMessageReceiver( MessageReceiver* receiver )
{

	receivers_list_lock_.lock();
	receivers_.remove( receiver );
	receivers_list_lock_.unlock();
}

void Yagl::BiDirectionalConnection::disconnect( )
{		
	if( is_receiving_ )
	{		
		stop();
		is_receiving_ = false;
	}
	

#ifdef TARGET_WIN32

#ifndef _MSC_VER // FIXME my vc6 has no idea about SD_BOTH but knows shutdown, strange.. )
	shutdown( socket_, SD_BOTH );
#endif
	closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
	shutdown( socket_, SHUT_RDWR );
	close( socket_ );
#endif
	
	socket_ = 0;
}

bool Yagl::BiDirectionalConnection::send( Message& message )
{
	//
	// if there was no message we return with success :p
	//
	if( message.getMessageSize() <= 0 )
	{
		return true;
	}
	
	//
	// we add 4 bytes to the total bytes to send as we also send the
	// message length as a header
	//
	int send_bytes = 0;
	unsigned int left_bytes = message.getMessageSize();
	unsigned int message_length_netbyteorder = htonl(message.getMessageSize());
	
	// 
	// first we send out the message length. note that
	// we rely on the fact that a 4 byte tcp package will not be 
	// fragmented ( we hope so... )
	//
	send_bytes = ::send( socket_, (char*)&message_length_netbyteorder, sizeof( unsigned int), 0 );
	if( send_bytes == -1 )
	{
		Yagl::Log::print( "BiDirectionalConnection: connection at " );
		Yagl::Log::print( this );
		Yagl::Log::print( " couldn't send header of message" );
		disconnect();
		return false;
	}
	
	unsigned int num_full_packages = message.getMessageSize() / 2048;
	unsigned int bytes_fractional_package = message.getMessageSize() % 2048;
	
	//
	// send out all the full 1kb packages
	//
	unsigned int buff_offset = 0;
	for( unsigned int i=0; i < num_full_packages; i++ )
	{
		send_bytes = 0;	
		while( send_bytes < 1024 )
		{
			int n = ::send( socket_, message.getBufferPointer() + buff_offset, 2048 - send_bytes, 0 );
			if( n == -1 )
			{
				Yagl::Log::print( "BiDirectionalConnection: connection at " );
				Yagl::Log::print( this );
				Yagl::Log::print( " couldn't send content of message" );
				disconnect();
				return false;
			}
			
			send_bytes += n;
			left_bytes -= n;
			buff_offset += n;
			//Thread::yield();
		}
	}

	//
	// send the one fractional package
	//
	send_bytes = 0;	
	while( (unsigned int)send_bytes < bytes_fractional_package )
	{
		int n = ::send( socket_, message.getBufferPointer() + buff_offset, bytes_fractional_package - send_bytes, 0 );
		if( n == -1 )
		{
			Yagl::Log::print( "BiDirectionalConnection: connection at " );
			Yagl::Log::print( this );
			Yagl::Log::print( " couldn't send header of message" );
			disconnect();
			return false;
		}
			
		send_bytes += n;		
		buff_offset += n;
		//Thread::yield();
	}
	
	return true;
}

void Yagl::BiDirectionalConnection::receive( )
{
	Message msg;
	MessageReceiver message_receiver;
	int message_len=0;
	bool waiting_for_header = true;
	timeval timeout;
	fd_set receiving_desc;		
	
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
			
	int received_bytes = 0;
	
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
	}
	
	while( status_ == RUNNING )
	{			
		FD_ZERO( &receiving_desc );
		FD_SET( (unsigned)socket_, &receiving_desc );
		if( select( socket_ + 1, &receiving_desc, 0, 0, &timeout ) == -1 )
		{
			Yagl::Log::print( "BiDirectionalConnection: connection at " );
			Yagl::Log::print( this );
			Yagl::Log::print( " couldn't poll receiving socket" );
#ifdef TARGET_WIN32
			closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
			close( socket_ );
#endif
			socket_ = 0;
			return;
		}		
		
		if( FD_ISSET( socket_, &receiving_desc ) )
		{
			
			if( waiting_for_header )
			{				
				//
				// read in the message len from the stream
				// if we receive less than 4 bytes we know
				// that the header is screwed.
				//
				received_bytes = recv( socket_, (char*)&message_len, sizeof( unsigned int ), 0 );
				if( received_bytes != sizeof( unsigned int ) )
				{
					
					if( received_bytes == 0 )
					{
						Yagl::Log::print( "BiDirectionalConnection: connection at " );
						Yagl::Log::print( this );
						Yagl::Log::print( " was remotely closed\n" );
					}
					if( received_bytes < 0 )
					{
						Yagl::Log::print( "BiDirectionalConnection: connection at " );
						Yagl::Log::print( this );
						Yagl::Log::print( " couldn't receive a message due to unkown error\n" );
					}
					
					if( received_bytes > 0 )
					{
						Yagl::Log::print( "BiDirectionalConnection: connection at " );
						Yagl::Log::print( this );
						Yagl::Log::print( " received corrupt message header\n" );
					}
					
#ifdef TARGET_WIN32	
					closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
					close( socket_ );
#endif
					socket_ = 0;
					return;
				}
				
				//
				// now we check wheter the message_len is valid
				//
		//		printf( "message_len %d\n", message_len );
				message_len = ntohl( message_len );
				if( message_len <= 0 )
				{
					Yagl::Log::print( "BiDirectionalConnection: connection at " );
					Yagl::Log::print( this );
					Yagl::Log::print( " received message with invalid length\n" );
#ifdef TARGET_WIN32
					closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
					close( socket_ );
#endif
					socket_ = 0;
					return;
				}
				
				//
				// size the message buffer and set attributes accordingly
				// 
				msg.setBufferSize( message_len );
				msg.setMessageSize( message_len );
				msg.setSenderAddress( remote_address_ );
				msg.setSenderPort( remote_port_ );
				
				waiting_for_header = false;
				received_bytes = 0;

			}
			else
			{				
				int n=0;				
			
				//printf( "receiving\n" );
				n = recv( socket_, msg.getBufferPointer() + received_bytes, message_len - received_bytes, 0 );				
				if( n <= 0 )
				{
					Yagl::Log::print( "BiDirectionalConnection: connection at " );
					Yagl::Log::print( this );
					Yagl::Log::print( " couldn't receive a message due to unkown error\n" );
					msg.setBufferSize( 0 );
#ifdef TARGET_WIN32
					closesocket( socket_ );
#endif
#ifdef TARGET_LINUX
					close( socket_ );
#endif
					socket_ = 0;
					return;
				}
								
								
				received_bytes += n;
								
				if( received_bytes == message_len )
				{				
					
					receivers_list_lock_.lock();
					std::list<MessageReceiver*>::iterator iter = receivers_.begin();
					
					while( iter != receivers_.end() )
					{				
						(*iter)->pushMessage( msg );
						iter++;
					}
					
					receivers_list_lock_.unlock();
					
					waiting_for_header = true;
					message_len = 0;
					received_bytes = 0;					
				}
				if( received_bytes > message_len )
				{					
					msg.setBufferSize( 0 );
				}
			}
			Thread::sleep(1);
		}
		else
			Thread::sleep(1);
		
	}
	
	return;
}

void* Yagl::BiDirectionalConnection::run( )
{
	is_receiving_ = true;
	receive();				
	Yagl::Log::print( "BiDirectionalConnection: connection at " );
	Yagl::Log::print( this );
	Yagl::Log::print( " terminated receiving thread\n" );
	status_ = STOPPED;
	is_receiving_ = false;
	return 0;
}
