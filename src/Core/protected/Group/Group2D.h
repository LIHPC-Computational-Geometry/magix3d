/*----------------------------------------------------------------------------*/
/** \file Group2D.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GROUP_GROUP2D_H_
#define MGX3D_GROUP_GROUP2D_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Geom {
class Surface;
}
namespace Topo {
class CoFace;
}
namespace Mesh {
class Surface;
}

namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
/**
 * \brief Groupe 2D.
 * Permet l'accès aux entités 2D appartenant à un même groupe, ainsi qu'aux entités
 * de niveaux inférieurs
 */
class Group2D : public GroupEntity{

    static const char* typeNameGroup2D;

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    Group2D(Internal::Context& ctx, const std::string & nom,
 		   bool isDefaultGroup, uint level=1);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~Group2D();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension du groupe
     */
    int getDim() const {return 2;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameGroup2D;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::Group2D;}

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Group2D &);
    friend std::ostream & operator << (std::ostream & , const Group2D &);

    /// retourne les infos sur l'entité
    virtual std::string getInfos();

     /*------------------------------------------------------------------------*/
	/// retourne les entités du groupe.
     virtual std::vector<Utils::Entity*> getEntities() const;

    /*------------------------------------------------------------------------*/
    /// accesseur sur la liste des Surfaces référencées
    std::vector<Geom::Surface*>& getSurfaces() {return m_surfaces;}
    const std::vector<Geom::Surface*>& getSurfaces() const {return m_surfaces;}

    /// accesseur sur la liste des faces topologiques
    std::vector<Topo::CoFace*>& getCoFaces() {return m_cofaces;}
    const std::vector<Topo::CoFace*>& getCoFaces() const {return m_cofaces;}

    std::vector<Mesh::Surface*>& getMeshSurfaces() {return m_mesh_surfaces;}
    const std::vector<Mesh::Surface*>& getMeshSurfaces() const {return m_mesh_surfaces;}

    bool empty() const {return m_surfaces.empty() && m_cofaces.empty() && m_mesh_surfaces.empty();}

    /*------------------------------------------------------------------------*/
    /// Enlève une Surface du groupe
    void remove(Geom::Surface* surf, const bool exceptionIfNotFound=true);
    void remove(Topo::CoFace* coface, const bool exceptionIfNotFound=true);
    void remove(Mesh::Surface* surf, const bool exceptionIfNotFound=true);

    /*------------------------------------------------------------------------*/
    /// Ajoute une Surface au groupe
    void add(Geom::Surface* surf);
    void add(Topo::CoFace* coface);
    void add(Mesh::Surface* surf);

    /*------------------------------------------------------------------------*/
    /// Recherche une Surface dans le groupe
    bool find(Geom::Surface* surf);
    /// Recherche une face dans le groupe
    bool find(Topo::CoFace* coface);
    /// Recherche une Surface dans le groupe
    bool find(Mesh::Surface* surf);

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
    /// les Surfaces référencées par ce groupe
    std::vector<Geom::Surface*> m_surfaces;

    /// les faces référencées par ce groupe
    std::vector<Topo::CoFace*> m_cofaces;

    /// les surfaces de maillage référencés par ce groupe
    std::vector<Mesh::Surface*> m_mesh_surfaces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUP2D_H_ */
/*----------------------------------------------------------------------------*/
