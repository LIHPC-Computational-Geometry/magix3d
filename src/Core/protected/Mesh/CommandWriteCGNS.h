/*----------------------------------------------------------------------------*/
/*
 * \file CommandWriteCGNS.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13/10/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDWRITECGNS_H_
#define COMMANDWRITECGNS_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
class CommandWriteCGNS: public Internal::CommandInternal {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour la sauvegarde de tout le maillage dans un fichier au format CGNS
     *
     *  \param c le contexte
     *  \param nom le fichier dans lequel on effectue la sauvegarde
     */
	CommandWriteCGNS(Internal::Context& c, std::string& nom);


	/*------------------------------------------------------------------------*/
	/** \brief  exécute la commande
	 */
	virtual void internalExecute();

	/*------------------------------------------------------------------------*/
	/** \brief  annule la commande
	 */
	void internalUndo();

	/** \brief  rejoue la commande
	 */
	void internalRedo();

private:
	/// nom du fichier pour la sauvegarde
	std::string m_file_name;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* COMMANDWRITECGNS_H_ */
