/*----------------------------------------------------------------------------*/
/*
 * \file CommandModifyMesh.h
 *
 *  \author Nicolas Le Goff
 *
 *  \date 20 mar. 2014
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_COMMANDMODIFYMESH_H_
#define MGX3D_MESH_COMMANDMODIFYMESH_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
class CommandModifyMesh: public Internal::CommandInternal {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte
     * \param le nom de la commande
     */
	CommandModifyMesh(Internal::Context& c, std::string& name);
//public:
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandModifyMesh();

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

protected:

private:

//	/*------------------------------------------------------------------------*/
//	/** \brief  constructeur par recopie
//	 */
//	CommandModifyMesh(const CommandModifyMesh&);
//
//	/*------------------------------------------------------------------------*/
//	/** \brief  surcharge opérateur affectation
//	 */
//	CommandModifyMesh& operator = (const CommandModifyMesh&);


};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_COMMANDMODIFYMESH_H_ */
