/*----------------------------------------------------------------------------*/
/** \file Group1D.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GROUP_GROUP1D_H_
#define MGX3D_GROUP_GROUP1D_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Geom {
class Curve;
}
namespace Topo {
class CoEdge;
}

namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
/**
 * \brief Groupe 1D.
 * Permet l'accès aux entités 1D appartenant à un même groupe, ainsi qu'aux entités
 * de niveaux inférieurs
 */
class Group1D : public GroupEntity{

    static const char* typeNameGroup1D;

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    Group1D(Internal::Context& ctx, const std::string & nom,
 		   bool isDefaultGroup, uint level=1);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~Group1D();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension du groupe
     */
    int getDim() const {return 1;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameGroup1D;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::Group1D;}

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Group1D &);
    friend std::ostream & operator << (std::ostream & , const Group1D &);

    /// retourne les infos sur l'entité
    virtual std::string getInfos();

     /*------------------------------------------------------------------------*/
	/// retourne les entités du groupe.
     virtual std::vector<Utils::Entity*> getEntities() const;

    /*------------------------------------------------------------------------*/
    /// accesseur sur la liste des Curves référencées
    std::vector<Geom::Curve*>& getCurves() {return m_curves;}
    const std::vector<Geom::Curve*>& getCurves() const {return m_curves;}

    /// accesseur sur la liste des aêtes référencées
    std::vector<Topo::CoEdge*>& getCoEdges() {return m_coedges;}
    const std::vector<Topo::CoEdge*>& getCoEdges() const {return m_coedges;}

    bool empty() const {return m_curves.empty() && m_coedges.empty();}

    /*------------------------------------------------------------------------*/
    /// Enlève une Curve du groupe
    void remove(Geom::Curve* crv, const bool exceptionIfNotFound=true);
    void remove(Topo::CoEdge* coedge, const bool exceptionIfNotFound=true);

    /*------------------------------------------------------------------------*/
    /// Ajoute une Curve au groupe
    void add(Geom::Curve* crv);
    void add(Topo::CoEdge* coedge);

    /*------------------------------------------------------------------------*/
    /// Recherche une Curve dans le groupe
    bool find(Geom::Curve* crv);
    /// Recherche une arête dans le groupe
    bool find(Topo::CoEdge* coedge);

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


private:
    /// les Curves référencées par ce groupe
    std::vector<Geom::Curve*> m_curves;

    /// les arêtes topologiques référencés par ce groupe
    std::vector<Topo::CoEdge*> m_coedges;
};
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUP1D_H_ */
/*----------------------------------------------------------------------------*/
