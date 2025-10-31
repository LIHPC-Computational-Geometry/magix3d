/*----------------------------------------------------------------------------*/
#include "Topo/CommandChangeVerticesLocation.h"
#include "Topo/CoFace.h"
#include "Topo/Vertex.h"
#include "Topo/Block.h"
#include "Utils/Common.h"
#include "Utils/Spherical.h"
#include "Utils/Cylindrical.h"
#include "SysCoord/SysCoord.h"
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
    std::vector<Topo::Block*> blocks;
    // liste des cofaces modifiées
    std::vector<Topo::CoFace*> cofaces;

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

        std::vector<Block* > l_blocks = vtx->getBlocks();
        blocks.insert(blocks.end(), l_blocks.begin(), l_blocks.end());

        std::vector<CoFace* > l_cofaces = vtx->getCoFaces();
        cofaces.insert(cofaces.end(), l_cofaces.begin(), l_cofaces.end());
    }

    std::sort(blocks.begin(), blocks.end(), Utils::Entity::compareEntity);
    auto lastblocks = std::unique(blocks.begin(), blocks.end());
    blocks.erase(lastblocks, blocks.end());

    std::sort(cofaces.begin(), cofaces.end(), Utils::Entity::compareEntity);
    auto lastcofaces = std::unique(cofaces.begin(), cofaces.end());
    cofaces.erase(lastcofaces, cofaces.end());

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
