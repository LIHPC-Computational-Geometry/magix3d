/*----------------------------------------------------------------------------*/
/** \file GroupEntity.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GROUP_GROUPENTITY_H_
#define MGX3D_GROUP_GROUPENTITY_H_
/*----------------------------------------------------------------------------*/
#include "Internal/InternalEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
class MeshModificationItf;
}
/*----------------------------------------------------------------------------*/
namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
/*!
 * \namespace Mgx3D::Group
 *
 * \brief Espace de nom des classes associées aux groupes
 *
 *
 */
namespace Group {
/*----------------------------------------------------------------------------*/
/**
 * \class GroupEntity
 * \brief Interface définissant les services que doit fournir toute entité
 *        de type groupe. Cette interface mutualise les services communs aux
 *        groupes de différentes dimensions
 *
 */
class GroupEntity : public Internal::InternalEntity{

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    GroupEntity(Internal::Context& ctx, Utils::Property* prop,
            Utils::DisplayProperties* disp,
			   bool isDefaultGroup,
			   uint level);

public:
    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GroupEntity();

    /*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet (mais pas ses dépendances !)
     */
    virtual void setDestroyed(bool b);

    /*------------------------------------------------------------------------*/
    /// retourne les infos sur l'entité
     virtual std::string getInfos() =0;

     /*------------------------------------------------------------------------*/
    virtual void getRepresentation(
			Mgx3D::Utils::DisplayRepresentation& dr, bool checkDestroyed) const;

     /*------------------------------------------------------------------------*/
     /// Identification des groupes avec nom par défaut (type Hors_Groupe...)
     bool isDefaultGroup() const {return m_isDefaultGroup;}

     /*------------------------------------------------------------------------*/
     /// changement de niveau d'un groupe
     void setLevel (uint level) {m_level = level;}

     /// accès au niveau d'un groupe
     uint getLevel () const {return m_level;}

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

     /*------------------------------------------------------------------------*/
	/// retourne les entités du groupe. Méthode à surcharger.
     virtual std::vector<Utils::Entity*> getEntities() const;

     /*------------------------------------------------------------------------*/
     /// Accesseur sur le id ème objet de modification du maillage pour un groupe
     virtual Mesh::MeshModificationItf* getMeshModif(size_t id) {return m_meshModif[id];}

     /// Ajout d'un objet de modification du maillage pour un groupe
     virtual void addMeshModif(Mesh::MeshModificationItf* modif) {m_meshModif.push_back(modif);}

     /// Suppression du dernier objet de modification
     virtual void popBackMeshModif() {m_meshModif.pop_back();}

     /// Retourne le nombre de modifications pour ce groupe
     virtual size_t getNbMeshModif() {return m_meshModif.size();}
#endif


private:
    /// Les modifications du maillage associé au groupe
    std::vector<Mesh::MeshModificationItf*> m_meshModif;

    /// Pour identifier les groupes avec nom par défaut (type Hors_Groupe...)
    bool m_isDefaultGroup;

    /// Distinction des groupes suivant un niveau (~ notion de matériau)
    uint m_level;
};
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUPENTITY_H_ */
/*----------------------------------------------------------------------------*/
