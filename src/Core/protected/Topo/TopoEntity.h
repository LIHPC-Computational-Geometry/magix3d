/*----------------------------------------------------------------------------*/
/** \file TopoEntity.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18 nov. 2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_TOPOENTITY_H_
#define MGX3D_GEOM_TOPOENTITY_H_
/*----------------------------------------------------------------------------*/
#include "Internal/InternalEntity.h"
#include "Topo/TopoProperty.h"
#include "Topo/TopoInfo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class GeomEntity;
}
namespace Mesh {
class CommandCreateMesh;
}
/*----------------------------------------------------------------------------*/
/*!
 * \namespace Mgx3D::Topo
 *
 * \brief Espace de nom des classes associées à la topologie
 *
 *
 */
namespace Topo {

class CommandEditTopo;
class Vertex;

/*----------------------------------------------------------------------------*/
/**
 * \class TopoEntity
 * \brief Interface définissant les services que doit fournir toute entité
 *        topologique. Cette interface mutualise les services communs aux
 *        sommets, arêtes, faces et blocs topologique.
 */
class TopoEntity : public Internal::InternalEntity{

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    TopoEntity(Internal::Context& ctx, Utils::Property* prop,
            Utils::DisplayProperties* disp);

public:
    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~TopoEntity();

    /*------------------------------------------------------------------------*/
    /** \brief  Retourne la boite englobante de l'entité.
	 * \param	En retour, la boite englobante définit comme suit
	 * 			<I>(xmin, xmax, ymin, ymax, zmin, zmax)</I>.
	 */
#ifndef SWIG
	virtual void getBounds (double bounds[6]) const;
#endif	// SWIG

    /*------------------------------------------------------------------------*/
	/// accès à tous les sommets y compris ceux internes
#ifndef SWIG
	virtual void getAllVertices(std::vector<Topo::Vertex* >& vertices, const bool unique=true) const;
#endif  // SWIG

	/*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet (mais pas ses dépendances !)
     */
#ifndef SWIG
    virtual void setDestroyed(bool b);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Retourne un pointeur vers une potentielle association
     * avec la géométrie
     */
    Geom::GeomEntity* getGeomAssociation() const {return m_topo_property->getGeomAssociation();}

    /*------------------------------------------------------------------------*/
    /** \brief  Affecte un pointeur vers une entité géométrique
     */
#ifndef SWIG
    virtual void setGeomAssociation(Geom::GeomEntity* ge);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief   Suppression des dépendances (entités topologiques incidentes)
     */
#ifndef SWIG
    virtual void clearDependancy() =0;
#endif

    /*------------------------------------------------------------------------*/
      /** \brief  Fournit l'accès aux sommets topologiques incidents
       *
       *  \param vertices les sommets incidents
       */
     virtual void getVertices(std::vector<Topo::Vertex* >& vertices) const
      {}

      /*------------------------------------------------------------------------*/
    /// Accesseur sur la liste de noms de groupes
    virtual void getGroupsName (std::vector<std::string>& gn, bool byGeom, bool byTopo) const;

    /*------------------------------------------------------------------------*/
    /** Duplique le TopoProperty pour en conserver une copie
    *  (non modifiée par les accesseurs divers)
    *  Si le TopoProperty a déjà une copie de sauvegarde, il n'est rien fait
    */
#ifndef SWIG
    void saveTopoProperty();
#endif

    /** Change la propriété topologique
     * \return l'ancienne propriété (pour destruction)
     */
#ifndef SWIG
    virtual TopoProperty* setProperty(TopoProperty* new_tp);
#endif

    /** Transmet la propriété topologique sauvegardée
     *  dont la destruction est alors à la charge du CommandEditTopo
     *  On ne garde pas de lien dessus
     */
#ifndef SWIG
    virtual void saveInternals(CommandEditTopo* cet);
#endif

    /** Transmet les propriétés sauvegardées
     *  dont la destruction est alors à la charge du CommandCreateMesh
     *  On ne garde pas de lien dessus
     */
#ifndef SWIG
    virtual void saveInternals(Mesh::CommandCreateMesh* ccm) {}
#endif

    /** Booléen qui permet de savoir si l'on est en train de quitter la session ou non */
#ifndef SWIG
    bool isFinished();
#endif

    /** Booléen qui signale que l'on est en cours d'édition de l'objet
     * (une des propriétées a sa copie non nulle) */
    virtual bool isEdited() const;

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
    /** \brief  Fournit un résumé textuel de l'entité.
     */
#ifndef SWIG
    virtual std::string getSummary ( ) const;
#endif

    /*------------------------------------------------------------------------*/
    /// Met à jour les couleurs dans le display property
    virtual void updateDisplayPropertiesColor();


    /*------------------------------------------------------------------------*/
    /** Nombre de noeuds internes
     * 1 pour un sommet
     * le nombre de noeuds sans ceux du bord pour les coedges, cofaces et blocs
     */
    virtual unsigned long getNbInternalMeshingNodes() {return 0;}


private:
    /// Conteneur des propriétés topologiques
    TopoProperty* m_topo_property;

    /// Sauvegarde de m_topo_property
    TopoProperty* m_save_topo_property;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_TOPOENTITY_H_ */
/*----------------------------------------------------------------------------*/
