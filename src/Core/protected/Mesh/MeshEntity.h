/*----------------------------------------------------------------------------*/
/*
 * \file MeshEntity.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_ENTITY_H_
#define MGX3D_MESH_ENTITY_H_
/*----------------------------------------------------------------------------*/
#include "Internal/InternalEntity.h"
#include "Utils/Property.h"
#include "Utils/DisplayProperties.h"
#include "Mesh/MeshManagerIfc.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
/*!
 * \namespace Mgx3D::Mesh
 *
 * \brief Espace de nom des classes associées au maillage
 *
 *
 */
namespace Mesh {

class CommandCreateMesh;

/*----------------------------------------------------------------------------*/
/**
 * \class MeshEntity
 * \brief Interface définissant les services que doit fournir toute entité
 *        de groupe de mailles ou de noeuds.
 *        Cette interface mutualise les services communs aux
 *        Volumes, Surfaces et Nuages (Clouds).
 *
 *        Un groupe est retrouvé dans le maillage à l'aide de son nom
 *
 */
class MeshEntity : public Internal::InternalEntity{

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    MeshEntity(Internal::Context& ctx, Utils::Property* prop,
            Utils::DisplayProperties* disp);

public:
    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~MeshEntity();

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** \brief  Retourne la boite englobante de l'entité.
	 * \param	En retour, la boite englobante définit comme suit
	 * 			<I>(xmin, xmax, ymin, ymax, zmin, zmax)</I>.
	 */
	virtual void getBounds (double bounds[6]) const;
#endif

    /*------------------------------------------------------------------------*/
#ifndef SWIG
   /** \brief  Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
     * \return  Description, à détruire par l'appelant.
     */
    virtual Mgx3D::Utils::SerializedRepresentation* getDescription (
													bool alsoComputed) const;
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la liste des noms des éventuelles variables aux
	 *			noeuds.
	 * \see		getCellsValuesNames
	 * \see		setNodesValuesNames
     */
    virtual std::vector<std::string> getNodesValuesNames ( ) const;

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la liste des noms des éventuelles variables aux
	 *			mailles.
	 * \see		getNodesValuesNames
	 * \see		setCellsValuesNames
     */
    virtual std::vector<std::string> getCellsValuesNames ( ) const;

    /*------------------------------------------------------------------------*/
    /** param   la liste des noms des éventuelles variables aux noeuds.
	 * \see		setCellsValuesNames
	 * \see		getNodesValuesNames
     */
    virtual void setNodesValuesNames (const std::vector<std::string>& names);

    /*------------------------------------------------------------------------*/
    /** \brief  la liste des noms des éventuelles variables aux mailles.
	 * \see		setNodesValuesNames
	 * \see		getCellsValuesNames
     */
    virtual void setCellsValuesNames (const std::vector<std::string>& names);

    /*------------------------------------------------------------------------*/
    /** \brief  Fixe une valeur portée par les noeuds.
	 * \see		setNodesValuesNames
	 * \see		getCellsValuesNames
	 *
	 * \param name le nom de la valeur
	 * \param values le tableau contenant cette valeur
     */
    virtual void setNodesValue (const std::string& name, const std::map<int,double>& values);

    /*------------------------------------------------------------------------*/
    /** \brief  Récupère une valeur portée par les noeuds.
	 * \see		setNodesValuesNames
	 * \see		getCellsValuesNames
	 *
	 * \param name le nom de la valeur
	 *
	 * \return le tableau contenant cette valeur
     */
    virtual std::map<int,double> getNodesValue (const std::string& name);

    /*------------------------------------------------------------------------*/
#ifndef SWIG
   /** Transmet les propriétés sauvegardées
     *  dont la destruction est alors à la charge du CommandCreateMesh
     *  On ne garde pas de lien dessus
     */
    virtual void saveInternals(Mesh::CommandCreateMesh* ccm) {}
#endif

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /// Accesseur sur le MeshManager (et donc sur le maillage)
    Mesh::MeshManagerIfc& getMeshManager() const;
#endif

private:
    /// Un accès au maillage via le manager
    Mesh::MeshManagerIfc* m_mesh_manager;

	/// Les listes des éventuelles valeurs aux noeuds et aux mailles.
	std::vector<std::string>	m_nodesValuesNames, m_cellsValuesNames;

	/// Valeurs stockées aux noeuds
	std::map<std::string, std::map<int,double> > m_nodesValues;

protected:
    /// Stockage de la boite englobante pour éviter de la recalculer à chaque fois
    mutable double m_bounds[6];

    /// Pour savoir si m_bounds est à jour
    mutable bool m_bounds_to_be_calculate;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_ENTITY_H_ */
/*----------------------------------------------------------------------------*/
