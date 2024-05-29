/*----------------------------------------------------------------------------*/
/*
 * \file Cloud.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_CLOUD_H_
#define MGX3D_MESH_CLOUD_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshEntity.h"
#include <TkUtil/UTF8String.h>
#include "Utils/Container.h"
#include "Topo/MeshCloudTopoProperty.h"

namespace gmds {
class Node;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Internal {
class InfoCommand;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
   @brief Nuage de noeuds du maillage
 */
#ifdef SWIG
%name(MeshCloud)
#endif
class Cloud : public MeshEntity {
    static const char* typeNameMeshCloud;

public:

    /// Constructeur
#ifndef SWIG
    Cloud(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp);
#endif

    /// Destructeur
    virtual ~Cloud();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension des entités du groupe
     */
    int getDim() const {return 0;}

    /*------------------------------------------------------------------------*/
    /** \brief  Retourne la boite englobante de l'entité.
	 * \param	En retour, la boite englobante définit comme suit
	 * 			<I>(xmin, xmax, ymin, ymax, zmin, zmax)</I>.
	 */
#ifndef SWIG
	virtual void getBounds (double bounds[6]) const;
#endif	// SWIG

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation affichable de l'entité en se basant
     *          sur la représentation passée en paramètre
     *
     *  \param  dr la représentation que l'on demande à afficher
     *  \param	Lève une exception si checkDestroyed vaut true
     */
#ifndef SWIG
    virtual void getRepresentation(
			Mgx3D::Utils::DisplayRepresentation& dr, bool checkDestroyed) const;
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
     * \return  Description, à détruire par l'appelant.
     */
#ifndef SWIG
    virtual Mgx3D::Utils::SerializedRepresentation* getDescription (
													bool alsoComputed) const;
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameMeshCloud;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::MeshCloud;}

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Nu";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est un Cloud suivant son nom
     */
    static bool isA(std::string& name);

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
#ifndef SWIG
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Cloud &);
#endif

    /*------------------------------------------------------------------------*/
    /// \brief Ajoute une relation vers une arête commune
#ifndef SWIG
    void addCoEdge(Topo::CoEdge* e);
#endif

    /// Retire une relation vers une Edge
#ifndef SWIG
    void removeCoEdge(Topo::CoEdge* e);
#endif

    ///  Fournit l'accès aux arêtes topologiques qui ont participées à la constitution du nuage
    void getCoEdges(std::vector<Topo::CoEdge* >& edges) const;

    /** supprime les relations vers les arêtes */
    void clearCoEdges()
    {m_topo_property->clear();}

    /*------------------------------------------------------------------------*/
    /// \brief Ajoute une relation vers un sommet
#ifndef SWIG
    void addVertex(Topo::Vertex* v);
#endif

    /// Retire une relation vers un Vertex
#ifndef SWIG
    void removeVertex(Topo::Vertex* v);
#endif

    ///  Fournit l'accès aux sommets topologiques qui ont participées à la constitution du nuage
    void getVertices(std::vector<Topo::Vertex* >& vertices) const;

    /** supprime les relations vers les sommets */
    void clearVertices()
    {m_topo_property->clear();}

    /*----------------------------------------------------------------------------*/
    ///  Fournit l'accès aux noeuds GMDS
    void getGMDSNodes(std::vector<gmds::Node >& ANodes) const;

    /*------------------------------------------------------------------------*/
    /** Duplique le MeshCloudTopoProperty pour en conserver une copie
     *  (non modifiée par les accesseurs divers)
     *  Si le MeshCloudTopoProperty a déjà une copie de sauvegarde, il n'est rien fait
     *
     *  Si InfoCommand n'est pas nul, il est informé de la modification de l'entité
     */
    void saveMeshCloudTopoProperty(Internal::InfoCommand* icmd);

    /** Echange le MeshCloudTopoProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    Topo::MeshCloudTopoProperty* setProperty(Topo::MeshCloudTopoProperty* prop);

    /** Transmet les propriétés sauvegardées
      *  dont la destruction est alors à la charge du CommandCreateMesh
      *  On ne garde pas de lien dessus
      */
#ifndef SWIG
    virtual void  saveInternals(Mesh::CommandCreateMesh* ccm);
#endif

private:
    /// Constructeur par copie
    Cloud(const Cloud& cl);

    /// Opérateur de copie
    Cloud & operator = (const Cloud& cl);

    // lien sur les Topo::CoEdge qui ont participées à la constitution des noeuds
    Topo::MeshCloudTopoProperty* m_topo_property;

    /// sauvegarde du m_topo_property
    Topo::MeshCloudTopoProperty* m_save_topo_property;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_CLOUD_H_ */
/*----------------------------------------------------------------------------*/
