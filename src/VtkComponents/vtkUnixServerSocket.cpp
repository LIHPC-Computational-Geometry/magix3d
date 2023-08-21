#include "VtkComponents/vtkUnixServerSocket.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>


vtkUnixServerSocket::vtkUnixServerSocket ( )
	: vtkServerSocket ( )
{
}	// vtkUnixServerSocket::vtkUnixServerSocket


vtkUnixServerSocket::vtkUnixServerSocket (const vtkUnixServerSocket&)
	: vtkServerSocket ( )
{
}	// vtkUnixServerSocket::vtkUnixServerSocket


vtkUnixServerSocket& vtkUnixServerSocket::operator = (
													const vtkUnixServerSocket&)
{
	return *this;
}	// vtkUnixServerSocket::vtkUnixServerSocket


vtkUnixServerSocket::~vtkUnixServerSocket ( )
{
}	// vtkUnixServerSocket::~vtkUnixServerSocket


vtkUnixServerSocket* vtkUnixServerSocket::New ( )
{
	vtkUnixServerSocket*	socket	= new vtkUnixServerSocket ( );
#ifndef VTK_5
	socket->InitializeObjectBase ( );
#endif  // VTK_5

	return socket;
}	// vtkUnixServerSocket::New


int vtkUnixServerSocket::CreateServer (const char* file)
{
// LEGACY CODE :
	if (this->SocketDescriptor != -1)
	{
		vtkWarningMacro("Server Socket already exists. Closing old socket.");
		this->CloseSocket(this->SocketDescriptor);
		this->SocketDescriptor = -1;
	}
// LEGACY CODE :
//	this->SocketDescriptor = this->CreateSocket();
// CODE CP :
	this->SocketDescriptor	= this->CreateUnixSocket ( );
	if (this->SocketDescriptor < 0)
	{
		return -1;
	}
// LEGACY CODE :
//	if (this->BindSocket(this->SocketDescriptor, port) != 0 || 
// CODE CP :
// cout << __FILE__ << ' ' << __LINE__ << endl;
	if ((0 != BindSocket (this->SocketDescriptor, file)) ||
// LEGACY CODE :
	    this->Listen (this->SocketDescriptor) != 0)
	{
		// failed to bind or listen.
		this->CloseSocket(this->SocketDescriptor);
		this->SocketDescriptor = -1;
		return -1;
	}

	// Success.
	return 0;
}	// vtkUnixServerSocket::CreateServer


int vtkUnixServerSocket::CreateUnixSocket ( )
{
	const int	sock	= socket (AF_UNIX, SOCK_STREAM, 0);

	return sock;
}	// vtkUnixServerSocket::CreateUnixSocket


int vtkUnixServerSocket::BindSocket (int socketdescriptor, const char* file)
{
	struct sockaddr_un	server;
	memset (&server, 0, sizeof (struct sockaddr_un));
	server.sun_family	= AF_UNIX;
	strcpy (server.sun_path, file);

	int	success	= bind (
		socketdescriptor, reinterpret_cast<sockaddr*>(&server),sizeof (server));

	return success;
}	// vtkUnixServerSocket::CreateServer
