#include "VtkComponents/vtkUnixClientSocket.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>


vtkUnixClientSocket::vtkUnixClientSocket ( )
	: vtkClientSocket ( )
{
}	// vtkUnixClientSocket::vtkUnixClientSocket


vtkUnixClientSocket::vtkUnixClientSocket (const vtkUnixClientSocket&)
	: vtkClientSocket ( )
{
}	// vtkUnixClientSocket::vtkUnixClientSocket


vtkUnixClientSocket& vtkUnixClientSocket::operator =(const vtkUnixClientSocket&)
{
	return *this;
}	// vtkUnixClientSocket::vtkUnixClientSocket


vtkUnixClientSocket::~vtkUnixClientSocket ( )
{
}	// vtkUnixClientSocket::~vtkUnixClientSocket


vtkUnixClientSocket* vtkUnixClientSocket::New ( )
{
	vtkUnixClientSocket*	socket	= new vtkUnixClientSocket ( );
#ifndef VTK_5
	socket->InitializeObjectBase ( );
#endif  // VTK_5

	return socket;
}	// vtkUnixClientSocket::New


int vtkUnixClientSocket::ConnectToServer (const char* file)
{
// LEGACY CODE :
	if (this->SocketDescriptor != -1)
	{
		vtkWarningMacro("Client Socket already exists. Closing old socket.");
		this->CloseSocket(this->SocketDescriptor);
		this->SocketDescriptor = -1;
	}
// LEGACY CODE :
//	this->SocketDescriptor = this->CreateSocket();
// CODE CP :
	this->SocketDescriptor	= this->CreateUnixSocket ( );
	if (this->SocketDescriptor < 0)
	{
		vtkErrorMacro("Failed to create socket.");
		return -1;
	}
// LEGACY CODE :
//	if (this->Connect(this->SocketDescriptor, hostName, port) == -1)
// CODE CP :
// cout << __FILE__ << ' ' << __LINE__ << endl;
	if (-1 == Connect (this->SocketDescriptor, file))
// LEGACY CODE :
	{
		this->CloseSocket(this->SocketDescriptor);
		this->SocketDescriptor = -1;

		vtkErrorMacro("Failed to connect to UNIX domain server " << file);
		return -1;
	}

	this->ConnectingSide	= true;

	return 0;
}	// vtkUnixClientSocket::ConnectToServer


int vtkUnixClientSocket::CreateUnixSocket ( )
{
	const int	sock	= socket (AF_UNIX, SOCK_STREAM, 0);

	return sock;
}	// vtkUnixClientSocket::CreateUnixSocket


int vtkUnixClientSocket::Connect (int socketdescriptor, const char* file)
{
	struct sockaddr_un	server;
	memset (&server, 0, sizeof (struct sockaddr_un));
	server.sun_family	= AF_UNIX;
	strcpy (server.sun_path, file);

	int	success	= connect (
		socketdescriptor, reinterpret_cast<sockaddr*>(&server),sizeof (server));

	return success;
}	// vtkUnixClientSocket::Connect
