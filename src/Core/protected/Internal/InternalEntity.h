/*----------------------------------------------------------------------------*/
/*
 * \file InternalEntity.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2 mars 2015
 */
/*----------------------------------------------------------------------------*/
#ifndef INTERNALENTITY_H_
#define INTERNALENTITY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Entity.h"
/*----------------------------------------------------------------------------*/
namespace TkUtil {
class LogOutputStream;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
class Property;
class DisplayProperties;
}
/*----------------------------------------------------------------------------*/
namespace Internal {

class Context;

/*----------------------------------------------------------------------------*/
/** \class InternalEntity
 *  \brief Permet de stocker une relation vers le context
 */
/*----------------------------------------------------------------------------*/
class InternalEntity : public Utils::Entity {

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
	InternalEntity(Internal::Context& ctx,
			Utils::Property* prop,
			Utils::DisplayProperties* disp);

public:
    /*------------------------------------------------------------------------*/
    /// Accesseur sur le context
#ifndef SWIG
    Internal::Context& getContext() const {return m_context;}
#endif

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~InternalEntity();

    /*------------------------------------------------------------------------*/
/** \brief  Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
 * \return	Description, à détruire par l'appelant.
 */
#ifndef SWIG
    virtual Mgx3D::Utils::SerializedRepresentation* getDescription (
													bool alsoComputed) const;
#endif

private:

    /// Un accès au context
    Internal::Context&      m_context;
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* INTERNALENTITY_H_ */
