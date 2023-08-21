/*----------------------------------------------------------------------------*/
/*
 * \file Surface.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_SURFACE_H_
#define MGX3D_MESH_SURFACE_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshEntity.h"
#include <TkUtil/UTF8String.h>
#include "Utils/Container.h"
#include "Topo/MeshSurfaceTopoProperty.h"
/*----------------------------------------------------------------------------*/
namespace gmds {
class Face;
class Node;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoFace;
}
namespace Internal {
class InfoCommand;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
   @brief Surface (groupe de polygones) du maillage
 */
#ifdef SWIG
%name(MeshSurface)
#endif
class Surface : public MeshEntity {
    static const char* typeNameMeshSurface;

public:

    /// Constructeur
#ifndef SWIG
    Surface(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp);
#endif

    /// Destructeur
    virtual ~Surface();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension des entités du groupe
     */
    int getDim() const {return 2;}

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
    virtual std::string getTypeName() const {return typeNameMeshSurface;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::MeshSurface;}

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Su";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est une Surface suivant son nom
     */
    static bool isA(std::string& name);

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
#ifndef SWIG
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Surface &);
#endif

    /*------------------------------------------------------------------------*/
    /// \brief Ajoute une relation vers une face commune
#ifndef SWIG
    void addCoFace(Topo::CoFace* f);
#endif

    /*----------------------------------------------------------------------------*/
    /// Retire une relation vers une Face commune
#ifndef SWIG
    void removeCoFace(Topo::CoFace* f);
#endif

    /*----------------------------------------------------------------------------*/
    ///  Fournit l'accès aux faces topologiques communes qui ont participées à la constitution de la surface
    void getCoFaces(std::vector<Topo::CoFace* >& faces) const;

    /** supprime les relations vers les cofaces */
    void clearCoFaces()
    {m_topo_property->clear();}

    /*----------------------------------------------------------------------------*/
    ///  Fournit l'accès aux faces GMDS
    virtual void getGMDSFaces(std::vector<gmds::Face >& AFaces) const;

    ///  Fournit l'accès aux noeuds GMDS
    virtual void getGMDSNodes(std::vector<gmds::Node >& ANodes) const;

    /*------------------------------------------------------------------------*/
    /** Duplique le MeshSurfaceTopoProperty pour en conserver une copie
     *  (non modifiée par les accesseurs divers)
     *  Si le MeshSurfaceTopoProperty a déjà une copie de sauvegarde, il n'est rien fait
     *
     *  Si InfoCommand n'est pas nul, il est informé de la modification de l'entité
     */
    void saveMeshSurfaceTopoProperty(Internal::InfoCommand* icmd);

    /** Echange le MeshSurfaceTopoProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    Topo::MeshSurfaceTopoProperty* setProperty(Topo::MeshSurfaceTopoProperty* prop);

    /** Transmet les propriétés sauvegardées
      *  dont la destruction est alors à la charge du CommandCreateMesh
      *  On ne garde pas de lien dessus
      */
#ifndef SWIG
    virtual void  saveInternals(Mesh::CommandCreateMesh* ccm);
#endif

    /** retourne vrai si toutes les cofaces sont structurées */
    virtual bool isStructured();

private:
    /// Constructeur par copie
    Surface(const Surface& cl);

    /// Opérateur de copie
    Surface & operator = (const Surface& cl);

    // lien sur les Topo::CoFace qui ont participées à la constitution des mailles
    Topo::MeshSurfaceTopoProperty* m_topo_property;

    /// sauvegarde du m_topo_property
    Topo::MeshSurfaceTopoProperty* m_save_topo_property;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_SURFACE_H_ */
/*----------------------------------------------------------------------------*/
