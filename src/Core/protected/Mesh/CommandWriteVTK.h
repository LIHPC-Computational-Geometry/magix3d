/*----------------------------------------------------------------------------*/
/*
 * \file CommandWriteVTK.h
 *
 *  \author legoff
 *
 *  \date 10 jun. 2015
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_COMMANDWRITEVTK_H_
#define MGX3D_MESH_COMMANDWRITEVTK_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
class CommandWriteVTK: public Internal::CommandInternal {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour la sauvegarde de tout le maillage dans un fichier
     *
     *  \param c le contexte
     *  \param nom le fichier dans lequel on effectue la sauvegarde
     */
	CommandWriteVTK(Internal::Context& c, std::string& nom);


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

#endif /* MGX3D_MESH_COMMANDWRITEVTK_H_ */
