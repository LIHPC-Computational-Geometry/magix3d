/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GROUP_GROUP2D_H_
#define MGX3D_GROUP_GROUP2D_H_
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
 * \brief Groupe 2D.
 * Permet l'accès aux entités 2D appartenant à un même groupe, ainsi qu'aux entités
 * de niveaux inférieurs
 */
class Group2D : public GroupEntity{

    static const char* typeNameGroup2D;

public:
    static const uint DIM = 2;

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
    int getDim() const {return DIM;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameGroup2D;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::Group2D;}

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
#endif /* MGX3D_GROUP_GROUP2D_H_ */
/*----------------------------------------------------------------------------*/
