/*----------------------------------------------------------------------------*/
/*
 * \file BlockMeshingProperty.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21 déc. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Topo/BlockMeshingProperty.h"
#include "Topo/Block.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Topo/TopoHelper.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Vector.h"
#include <sys/types.h>
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
BlockMeshingProperty::meshDirLaw BlockMeshingProperty::_computeDir(Block* block, Utils::Math::Point & v1, Utils::Math::Point & v2)
{
    Utils::Math::Vector vect1 (v1, v2);

    std::vector<CoEdge* > iCoedges;
    std::vector<CoEdge* > jCoedges;
    std::vector<CoEdge* > kCoedges;

    block->getOrientedCoEdges(iCoedges, jCoedges, kCoedges);

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

    for (uint i=0; i<kCoedges.size(); i++){
        CoEdge* coedge = kCoedges[i];
        Utils::Math::Vector vect2 (coedge->getVertex(0)->getCoord(), coedge->getVertex(1)->getCoord());
        Utils::Math::Vector vect3 = vect1*vect2;
        if (Utils::Math::MgxNumeric::isNearlyZero(vect3.abs2()))
            return dir_k;
    }

	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    messErr << "Il n'est pas possible d'orienter le bloc "<<block->getName()
            << " avec les 2 points "<< v1 << " et "<< v2;
    throw TkUtil::Exception(messErr);

}
/*----------------------------------------------------------------------------*/
BlockMeshingProperty::meshDirLaw BlockMeshingProperty::_computeDir(Block* block, std::string& coedge_name)
{
    std::vector<CoEdge* > iCoedges;
    std::vector<CoEdge* > jCoedges;
    std::vector<CoEdge* > kCoedges;

    block->getOrientedCoEdges(iCoedges, jCoedges, kCoedges);

    for (uint i=0; i<iCoedges.size(); i++){
        CoEdge* coedge = iCoedges[i];
        if (coedge->getName() == coedge_name)
            return dir_i;
    }

    for (uint i=0; i<jCoedges.size(); i++){
        CoEdge* coedge = jCoedges[i];
        if (coedge->getName() == coedge_name)
            return dir_j;
    }

    for (uint i=0; i<kCoedges.size(); i++){
        CoEdge* coedge = kCoedges[i];
        if (coedge->getName() == coedge_name)
            return dir_k;
    }

	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    messErr << "Il n'est pas possible d'orienter le bloc "<<block->getName()
            << " avec l'arête "<< coedge_name<<", elle n'a pas été trouvée";
    throw TkUtil::Exception(messErr);

}
/*----------------------------------------------------------------------------*/
BlockMeshingProperty::meshSideLaw BlockMeshingProperty::_computeSide(Block* block,
		Utils::Math::Point & v1, Utils::Math::Point & v2, uint dir_bl)
{
    std::vector<Topo::Vertex* > sommets;
    block->getHexaVertices(sommets);

    for (uint j=0; j<4; j++){
    	Topo::Vertex* vtx1 = sommets[Topo::TopoHelper::tabIndVtxByEdgeAndDirOnBlock[dir_bl][j][0]];
    	Topo::Vertex* vtx2 = sommets[Topo::TopoHelper::tabIndVtxByEdgeAndDirOnBlock[dir_bl][j][1]];
    	if (vtx1->getCoord() == v1 || vtx2->getCoord() == v2)
    		return BlockMeshingProperty::side_min;
    	else if (vtx1->getCoord() == v2 || vtx2->getCoord() == v1)
    		return BlockMeshingProperty::side_max;
    }

	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    messErr << "Il n'est pas possible de sélectionner un côté du bloc "<<block->getName()
		<< " avec les 2 points "<< v1 << " et "<< v2;
    throw TkUtil::Exception(messErr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
