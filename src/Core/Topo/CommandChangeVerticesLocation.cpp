/*----------------------------------------------------------------------------*/
/*
 * \file CommandChangeVerticesLocation.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/10/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandChangeVerticesLocation.h"

#include "Utils/Common.h"
#include "Utils/Spherical.h"
#include "Utils/Cylindrical.h"
#include "Topo/Vertex.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandChangeVerticesLocation::
CommandChangeVerticesLocation(Internal::Context& c,
        std::vector<Vertex*> & vertices,
        const bool changeX,
        const double& xPos,
        const bool changeY,
        const double& yPos,
        const bool changeZ,
        const double& zPos,
		const coordinateType typeCoord,
		CoordinateSystem::SysCoord* rep)
:CommandEditTopo(c, "Modification de la position de sommets topologiques")
, m_vertices(vertices.begin(), vertices.end())
, m_typeCoord(typeCoord)
, m_changeX(changeX)
, m_xPos(xPos)
, m_changeY(changeY)
, m_yPos(yPos)
, m_changeZ(changeZ)
, m_zPos(zPos)
, m_rep(rep)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Modification de la position des sommets topologiques";
	for (uint i=0; i<vertices.size() && i<5; i++)
		comments << " " << vertices[i]->getName();
	if (vertices.size()>5)
		comments << " ... ";

	if (m_typeCoord == cartesian)
		comments << " en coordonnées cartésiennes";
	else if (m_typeCoord == spherical)
		comments << " en coordonnées sphériques";
	else if (m_typeCoord == cylindrical)
		comments << " en coordonnées cylindriques";

	if (m_rep)
		comments <<" dans le repère "<<m_rep->getName();

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandChangeVerticesLocation::
~CommandChangeVerticesLocation()
{
}
/*----------------------------------------------------------------------------*/
void CommandChangeVerticesLocation::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandChangeVerticesLocation::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // liste des blocs modifiés
    std::list<Topo::Block*> l_b;
    // liste des cofaces modifiées
    std::list<Topo::CoFace*> l_f;

    for (std::vector<Vertex*>::iterator iter = m_vertices.begin();
            iter != m_vertices.end(); ++iter){
        Vertex* vtx = *iter;
        vtx->saveVertexGeomProperty(&getInfoCommand(), true);

        Utils::Math::Point newPt;
        Utils::Math::Point oldPt;
        oldPt = vtx->getCoord();

        // changement de repère si nécessaire
        if (m_rep)
        	oldPt = m_rep->toLocal(oldPt);

        if (m_typeCoord == cartesian) {
            newPt.setX(m_changeX?m_xPos:oldPt.getX());
            newPt.setY(m_changeY?m_yPos:oldPt.getY());
            newPt.setZ(m_changeZ?m_zPos:oldPt.getZ());
        }
        else if (m_typeCoord == spherical) {
        	Utils::Math::Spherical sph(oldPt);
            if (m_changeX) sph.setRho(m_xPos);
            if (m_changeY) sph.setTheta(m_yPos);
            if (m_changeZ) sph.setPhi(m_zPos);
            newPt = sph;
        }
        else if (m_typeCoord == cylindrical) {
        	Utils::Math::Cylindrical cyl(oldPt);
            if (m_changeX) cyl.setRho(m_xPos);
            if (m_changeY) cyl.setPhi(m_yPos);
            if (m_changeZ) cyl.setZ(m_zPos);
            newPt = cyl;
        }
        else
        	throw TkUtil::Exception ("Erreur interne, coordinateType de CommandChangeVerticesLocation en dehors des clous");

        if (m_rep)
        	newPt = m_rep->toGlobal(newPt);

        vtx->setCoord(newPt);

        std::vector<Block* > blocks;
        vtx->getBlocks(blocks);
        l_b.insert(l_b.end(), blocks.begin(), blocks.end());

        std::vector<CoFace* > cofaces;
        vtx->getCoFaces(cofaces);
        l_f.insert(l_f.end(), cofaces.begin(), cofaces.end());
    }

    l_b.sort(Utils::Entity::compareEntity);
    l_b.unique();
    l_f.sort(Utils::Entity::compareEntity);
    l_f.unique();

    // recherche la méthode la plus simple possible
    updateMeshLaw(l_f, l_b);

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandChangeVerticesLocation::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
