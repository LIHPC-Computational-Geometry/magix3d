#ifndef VTK_UNIX_CLIENT_SOCKET_H
#define VTK_UNIX_CLIENT_SOCKET_H

#include <vtkClientSocket.h>

/**
 * Encapsulation VTK de socket de domaine UNIX (AF_UNIX) côté client.
 *
 * Il est regrettable que les classes de sockets VTK n'aient pas davantage de
 * méthodes virtuelles ... (Connect, CreateSocket, BindSocket, ...)
 */
class vtkUnixClientSocket : public vtkClientSocket
{
	public :

	static vtkUnixClientSocket *New ( );
	virtual int ConnectToServer (const char* file);


	protected :

	virtual int CreateUnixSocket ( );
	virtual int Connect (int socketdescriptor, const char* file);

	vtkUnixClientSocket ( );
	virtual ~vtkUnixClientSocket ( );


	private :

	vtkUnixClientSocket (const vtkUnixClientSocket&);
	vtkUnixClientSocket& operator = (const vtkUnixClientSocket&);
};	// class vtkUnixClientSocket

#endif	// VTK_UNIX_CLIENT_SERVER_SOCKET_H
