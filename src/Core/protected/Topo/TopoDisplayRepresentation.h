/*----------------------------------------------------------------------------*/
/** \file TopoDisplayRepresentation.h
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle
 *
 /*  \date 7/12/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPODISPLAYREPRESENTATION_H_
#define TOPODISPLAYREPRESENTATION_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Utils/DisplayRepresentation.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class TopoDisplayRepresentation
 *  \brief La classe TopoDisplayRepresention décrit une représentation
 *         de ce qui doit être affichée pour représenter graphiquement une
 *         entité topologique.
 */
/*----------------------------------------------------------------------------*/
class TopoDisplayRepresentation: public Utils::DisplayRepresentation{

public:


    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    TopoDisplayRepresentation(const type t=Utils::DisplayRepresentation::SOLID);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~TopoDisplayRepresentation();

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPODISPLAYREPRESENTATION_H_ */
/*----------------------------------------------------------------------------*/

