/*----------------------------------------------------------------------------*/
/** \file Group0D.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GROUP_GROUP0D_H_
#define MGX3D_GROUP_GROUP0D_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Geom {
class Vertex;
}
namespace Topo {
class Vertex;
}

namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
/**
 * \brief Groupe 0D.
 * Permet l'accès aux entités 0D appartenant à un même groupe, ainsi qu'aux entités
 * de niveaux inférieurs
 */
class Group0D : public GroupEntity{

    static const char* typeNameGroup0D;

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    Group0D(Internal::Context& ctx, const std::string & nom,
 		   bool isDefaultGroup, uint level=1);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~Group0D();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension du groupe
     */
    int getDim() const {return 0;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameGroup0D;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::Group0D;}

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Group0D &);
    friend std::ostream & operator << (std::ostream & , const Group0D &);

    /// retourne les infos sur l'entité
    virtual std::string getInfos();

     /*------------------------------------------------------------------------*/
	/// retourne les entités du groupe.
     virtual std::vector<Utils::Entity*> getEntities() const;

    /*------------------------------------------------------------------------*/
    /// accesseur sur la liste des Vertices référencées
    std::vector<Geom::Vertex*>& getVertices() {return m_vertices;}
    const std::vector<Geom::Vertex*>& getVertices() const {return m_vertices;}

    /// accesseur sur la liste des sommets topologiques référencées
    std::vector<Topo::Vertex*>& getTopoVertices() {return m_topo_vertices;}
    const std::vector<Topo::Vertex*>& getTopoVertices() const {return m_topo_vertices;}

    bool empty() const {return m_vertices.empty() && m_topo_vertices.empty();}

    /*------------------------------------------------------------------------*/
    /// Enlève un Vertex du groupe
    void remove(Geom::Vertex* vtx, const bool exceptionIfNotFound=true);
    void remove(Topo::Vertex* vtx, const bool exceptionIfNotFound=true);

    /*------------------------------------------------------------------------*/
    /// Ajoute un Vertex au groupe
    void add(Geom::Vertex* vtx);
    void add(Topo::Vertex* vtx);

    /*------------------------------------------------------------------------*/
    /// Recherche un Vertex dans le groupe
    bool find(Geom::Vertex* vtx);
    bool find(Topo::Vertex* vtx);

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
    /// les Vertices référencés par ce groupe
    std::vector<Geom::Vertex*> m_vertices;

    /// les Vertices topologiques référencés par ce groupe
    std::vector<Topo::Vertex*> m_topo_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUP0D_H_ */
/*----------------------------------------------------------------------------*/
