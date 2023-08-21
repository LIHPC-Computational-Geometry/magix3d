#ifndef VTK_UNIX_SERVER_SOCKET_H
#define VTK_UNIX_SERVER_SOCKET_H

#include <vtkServerSocket.h>

/**
 * Encapsulation VTK de socket de domaine UNIX (AF_UNIX) côté client.
 *
 * Il est regrettable que les classes de sockets VTK n'aient pas davantage de
 * méthodes virtuelles ... (Connect, CreateSocket, BindSocket, ...)
 */
class vtkUnixServerSocket : public vtkServerSocket
{
	public :

	static vtkUnixServerSocket *New ( );
	virtual int CreateServer (const char* file);


	protected :

	virtual int CreateUnixSocket ( );
	virtual int BindSocket (int socketdescriptor, const char* file);

	vtkUnixServerSocket ( );
	virtual ~vtkUnixServerSocket ( );


	private :

	vtkUnixServerSocket (const vtkUnixServerSocket&);
	vtkUnixServerSocket& operator = (const vtkUnixServerSocket&);
};	// class vtkUnixServerSocket

#endif	// VTK_UNIX_SERVER_SOCKET_H
