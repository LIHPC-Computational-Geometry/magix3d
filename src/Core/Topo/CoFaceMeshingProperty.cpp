/*----------------------------------------------------------------------------*/
/*
 * \file CoFaceMeshingProperty.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 11 oct. 2013
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CoFaceMeshingProperty.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"

#include "Utils/MgxNumeric.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"

#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
bool CoFaceMeshingProperty::operator == (const CoFaceMeshingProperty& mp) const
{
	if (getMeshLaw() != mp.getMeshLaw())
		return false;

	return true;
}
/*----------------------------------------------------------------------------*/
bool CoFaceMeshingProperty::operator != (const CoFaceMeshingProperty& mp) const
{
	return !(*this == mp);
}
/*----------------------------------------------------------------------------*/
CoFaceMeshingProperty::meshDirLaw CoFaceMeshingProperty::_computeDir(CoFace* coface,
        Utils::Math::Point & v1, Utils::Math::Point & v2)
{
    Utils::Math::Vector vect1 (v1, v2);

    std::vector<CoEdge* > iCoedges;
    std::vector<CoEdge* > jCoedges;

    coface->getOrientedCoEdges(iCoedges, jCoedges);

    for (uint i=0; i<iCoedges.size(); i++){
        CoEdge* coedge = iCoedges[i];
        Utils::Math::Vector vect2 (coedge->getVertex(0)->getCoord(), coedge->getVertex(1)->getCoord());
        Utils::Math::Vector vect3 = vect1*vect2;
        if (Utils::Math::MgxNumeric::isNearlyZero(vect3.abs2()))
            return dir_i;
    }

    for (uint i=0; i<jCoedges.size(); i++){
        CoEdge* coedge = jCoedges[i];
        Utils::Math::Vector vect2 (coedge->getVertex(0)->getCoord(), coedge->getVertex(1)->getCoord());
        Utils::Math::Vector vect3 = vect1*vect2;
        if (Utils::Math::MgxNumeric::isNearlyZero(vect3.abs2()))
            return dir_j;
    }

	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    messErr << "Il n'est pas possible d'orienter la face commune "<<coface->getName()
            << " avec les 2 points "<< v1 << " et "<< v2;
    throw TkUtil::Exception(messErr);
}
/*----------------------------------------------------------------------------*/
CoFaceMeshingProperty::meshSideLaw CoFaceMeshingProperty::_computeSide(CoFace* coface,
		Utils::Math::Point & v1, Utils::Math::Point & v2, uint dir)
{
	if (dir == 0){
		if (coface->getVertex(1)->getCoord() == v1 || coface->getVertex(2)->getCoord() == v2)
			return CoFaceMeshingProperty::side_min;
		else if (coface->getVertex(2)->getCoord() == v1 || coface->getVertex(1)->getCoord() == v2)
			return CoFaceMeshingProperty::side_max;
		if (coface->getNbVertices()==4){
			if (coface->getVertex(0)->getCoord() == v1 || coface->getVertex(3)->getCoord() == v2)
				return CoFaceMeshingProperty::side_min;
			else if (coface->getVertex(3)->getCoord() == v1 || coface->getVertex(0)->getCoord() == v2)
				return CoFaceMeshingProperty::side_max;
		}
	} else if (dir == 1){
		if (coface->getVertex(1)->getCoord() == v1 || coface->getVertex(0)->getCoord() == v2)
			return CoFaceMeshingProperty::side_min;
		else if (coface->getVertex(0)->getCoord() == v1 || coface->getVertex(1)->getCoord() == v2)
			return CoFaceMeshingProperty::side_max;
		uint ind_op = (coface->getNbVertices()==4?3:0);
		if (coface->getVertex(2)->getCoord() == v1 || coface->getVertex(ind_op)->getCoord() == v2)
			return CoFaceMeshingProperty::side_min;
		else if (coface->getVertex(ind_op)->getCoord() == v1 || coface->getVertex(2)->getCoord() == v2)
			return CoFaceMeshingProperty::side_max;
	} else
		 throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoFaceMeshingProperty::_computeSide / dir", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    messErr << "Il n'est pas possible de sélectionner un côté de la face "<<coface->getName()
		<< " avec les 2 points "<< v1 << " et "<< v2;
    throw TkUtil::Exception(messErr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
