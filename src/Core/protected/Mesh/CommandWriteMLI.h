/*----------------------------------------------------------------------------*/
/*
 * \file CommandWriteMLI.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 28 nov. 2013
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDWRITEMLI_H_
#define COMMANDWRITEMLI_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
class CommandWriteMLI: public Internal::CommandInternal {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour la sauvegarde de tout le maillage dans un fichier
     *
     *  \param c le contexte
     *  \param nom le fichier dans lequel on effectue la sauvegarde
     */
	CommandWriteMLI(Internal::Context& c, std::string& nom);


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

#endif /* COMMANDWRITEMLI_H_ */
