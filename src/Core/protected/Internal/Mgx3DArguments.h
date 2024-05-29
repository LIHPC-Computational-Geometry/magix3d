/**
 * Accès aux arguments de ligne de commande traités par Magix3D et ses applications dérivées.
 */

#ifndef MGX3D_ARGUMENTS_H
#define MGX3D_ARGUMENTS_H


#include <string>
#include <vector>


namespace Mgx3D
{

namespace Internal
{
	/**
	 * @return	Les arguments utilisables par <I>Magix3D</I> et ses applications dérivées. Ces
	 *		arguments sont du type <I>-argname</I>, <I>--argname</I>, <I>-argname argvvalue</I>
	 *		ou <I>--argname argvalue</I>.
	 */
	std::vector<std::string> mgx3dAllowedArgs ( );
}	// namespace Internal

}	// namespace Mgx3D

#endif	// MGX3D_ARGUMENTS_H

