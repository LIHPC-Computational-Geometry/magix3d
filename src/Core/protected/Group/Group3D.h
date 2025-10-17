/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GROUP_GROUP3D_H_
#define MGX3D_GROUP_GROUP3D_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
/**
 * \brief Groupe 3D.
 * Permet l'accès aux entités 3D appartenant à un même groupe, ainsi qu'aux entités
 * de niveaux inférieurs
 */
class Group3D : public GroupEntity{

    static const char* typeNameGroup3D;

public:
    static const uint DIM = 3;

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    Group3D(Internal::Context& ctx, const std::string & nom,
 		   bool isDefaultGroup, uint level=1);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~Group3D();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension du groupe
     */
    int getDim() const {return DIM;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameGroup3D;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::Group3D;}

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
};
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUP3D_H_ */
/*----------------------------------------------------------------------------*/
