/*----------------------------------------------------------------------------*/
/*
 * \file Volume.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_VOLUME_H_
#define MGX3D_MESH_VOLUME_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshEntity.h"
#include <TkUtil/UTF8String.h>
#include "Utils/Container.h"
#include "Topo/MeshVolumeTopoProperty.h"
/*----------------------------------------------------------------------------*/
namespace gmds {
class Region;
class Node;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class Block;
}
namespace Internal {
class InfoCommand;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
   @brief Volume (groupe de polyèdres) du maillage
 */
#ifdef SWIG
%name(MeshVolume)
#endif
class Volume : public MeshEntity {
    static const char* typeNameMeshVolume;

public:

    /// Constructeur
#ifndef SWIG
    Volume(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp);
#endif

    /// Destructeur
    virtual ~Volume();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension des entités du groupe
     */
    int getDim() const {return 3;}

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
    virtual std::string getTypeName() const {return typeNameMeshVolume;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::MeshVolume;}

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Vo";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est un Volume suivant son nom
     */
    static bool isA(std::string& name);

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
#ifndef SWIG
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Volume &);
#endif

    /*------------------------------------------------------------------------*/
    /// \brief Ajoute une relation vers un bloc
#ifndef SWIG
    void addBlock(Topo::Block* b);
#endif

    /*----------------------------------------------------------------------------*/
    /// Retire une relation vers un bloc
#ifndef SWIG
    void removeBlock(Topo::Block* b);
#endif

    /*----------------------------------------------------------------------------*/
    ///  Fournit l'accès aux blocs topologiques qui ont participées à la constitution du volume
    void getBlocks(std::vector<Topo::Block* >& blocs) const;

    /** supprime les relations vers les blocs */
    void clearBlocks()
    {m_topo_property->clear();}

    /*----------------------------------------------------------------------------*/
    ///  Fournit l'accès aux regions (polyèdres) GMDS
    virtual void getGMDSRegions(std::vector<gmds::Region >& ARegions) const;

    ///  Fournit l'accès aux noeuds GMDS
    virtual void getGMDSNodes(std::vector<gmds::Node >& ANodes) const;

    /*------------------------------------------------------------------------*/
    /** Duplique le MeshVolumeTopoProperty pour en conserver une copie
     *  (non modifiée par les accesseurs divers)
     *  Si le MeshVolumeTopoProperty a déjà une copie de sauvegarde, il n'est rien fait
     *
     *  Si InfoCommand n'est pas nul, il est informé de la modification de l'entité
     */
    void saveMeshVolumeTopoProperty(Internal::InfoCommand* icmd);

    /** Echange le MeshVolumeTopoProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    Topo::MeshVolumeTopoProperty* setProperty(Topo::MeshVolumeTopoProperty* prop);

    /** Transmet les propriétés sauvegardées
      *  dont la destruction est alors à la charge du CommandCreateMesh
      *  On ne garde pas de lien dessus
      */
#ifndef SWIG
    virtual void  saveInternals(Mesh::CommandCreateMesh* ccm);
#endif

    /** retourne vrai si tous les blocs sont structurés */
    virtual bool isStructured();

private:
    /// Constructeur par copie
    Volume(const Volume& cl);

    /// Opérateur de copie
    Volume & operator = (const Volume& cl);

    // lien sur les Topo::Block qui ont participés à la constitution des mailles
    Topo::MeshVolumeTopoProperty* m_topo_property;

    /// sauvegarde du m_topo_property
    Topo::MeshVolumeTopoProperty* m_save_topo_property;

};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_VOLUME_H_ */
/*----------------------------------------------------------------------------*/
