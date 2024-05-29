/*----------------------------------------------------------------------------*/
/** \file SysCoordDisplayRepresentation.h
 *
 *  \author Eric Brière de l'Isle
 *
 /*  \date 23/5/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef SYSCOORDDISPLAYREPRESENTATION_H_
#define SYSCOORDDISPLAYREPRESENTATION_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Utils/DisplayRepresentation.h"
#include "SysCoord/SysCoordProperty.h"
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
/** \class SysCoordDisplayRepresentation
 *  \brief La classe SysCoordDisplayRepresention décrit une représentation
 *         de ce qui doit être affichée pour représenter graphiquement une
 *         entité de type repère.
 */
/*----------------------------------------------------------------------------*/
class SysCoordDisplayRepresentation: public Utils::DisplayRepresentation{

public:


    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    SysCoordDisplayRepresentation(const type t=Utils::DisplayRepresentation::SHOWTRIHEDRON);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~SysCoordDisplayRepresentation();


    /** \brief Retourne la transformation
     */
    const SysCoordProperty& getTransform ( ) const { return m_transform; }
    SysCoordProperty& getTransform ( ) { return m_transform; }


private:
    /// transformation du repère par défaut vers ce repère local
    SysCoordProperty	m_transform;
};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* SYSCOORDDISPLAYREPRESENTATION_H_ */
/*----------------------------------------------------------------------------*/

