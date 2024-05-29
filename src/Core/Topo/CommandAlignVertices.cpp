/*----------------------------------------------------------------------------*/
/*
 * \file CommandAlignVertices.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 19/2/2013
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandAlignVertices.h"

#include "Utils/Common.h"
#include "Topo/Vertex.h"
#include "Topo/TopoHelper.h"

#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/EntityFactory.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

// OCC
#include <Standard_Failure.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandAlignVertices::
CommandAlignVertices(Internal::Context& c,
        std::vector<Vertex*> & vertices)
:CommandEditTopo(c, "Aligne des sommets topologiques entre les 2 sommets aux extrémités")
{
    // on recherche les extrémités de la sélection
	Vertex* v1 = 0;
	Vertex* v2 = 0;

    // pour cela on sélectionne les arêtes dont les 2 sommets sont sélectionnés

    // pour compter le nombre sommets sélectionnés par arête
    std::map<CoEdge*, uint> filtre_coedges;

    for (std::vector<Vertex*>::iterator iter1 = vertices.begin();
            iter1 != vertices.end(); ++iter1){
        const std::vector<CoEdge* >  loc_coedges = (*iter1)->getCoEdges();

        for (std::vector<CoEdge* >::const_iterator iter2 = loc_coedges.begin();
                iter2 != loc_coedges.end(); ++iter2){
            filtre_coedges[*iter2] += 1;
        }
    }

    // les arêtes dont les 2 sommets sont sélectionnés
    std::vector<CoEdge* > coedges;
    for (std::map<CoEdge*, uint>::iterator iter = filtre_coedges.begin();
            iter != filtre_coedges.end(); ++iter){
        if ((*iter).second == 2)
            coedges.push_back((*iter).first);
    }

    // recherche des 2 sommets aux extrémités
    TopoHelper::getVerticesTip(coedges, v1, v2, false);

    // on stocke dans m_vertices les sommets qui bougent
    for (std::vector<Vertex*>::iterator iter1 = vertices.begin();
                iter1 != vertices.end(); ++iter1){
        if (*iter1 != v1 && *iter1 != v2)
            m_vertices.push_back(*iter1);
    }

    if (m_vertices.empty())
        throw TkUtil::Exception (TkUtil::UTF8String ("Il n'y a pas de sommet interne à bouger (il faut sélectionner au moins 3 sommets)", TkUtil::Charset::UTF_8));

    CHECK_NULL_PTR_ERROR(v1);
    CHECK_NULL_PTR_ERROR(v2);
    m_p1 = v1->getCoord();
    m_p2 = v2->getCoord();

}
/*----------------------------------------------------------------------------*/
CommandAlignVertices::
CommandAlignVertices(Internal::Context& c,
		const Utils::Math::Point& p1, const Utils::Math::Point& p2,
        std::vector<Vertex*> & vertices)
:CommandEditTopo(c, "Aligne des sommets topologiques sur une droite définie par 2 points")
, m_vertices(vertices)
, m_p1(p1)
, m_p2(p2)
{
	if (m_p1 == m_p2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Il faut 2 points distincts pour définir une droite", TkUtil::Charset::UTF_8));

    if (m_vertices.empty())
        throw TkUtil::Exception (TkUtil::UTF8String ("Il n'y a pas de sommet à bouger (il faut sélectionner au moins 1 sommet)", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandAlignVertices::
~CommandAlignVertices()
{
}
/*----------------------------------------------------------------------------*/
void CommandAlignVertices::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandAlignVertices::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // création du segment entre les 2 sommets extrémités
    Geom::Vertex* vtx1 = Geom::EntityFactory(getContext()).newVertex(m_p1);
    Geom::Vertex* vtx2 = Geom::EntityFactory(getContext()).newVertex(m_p2);
    Geom::Curve* segment = Geom::EntityFactory(getContext()).newSegment(vtx1, vtx2);
    uint indice = 0;
    for (std::vector<Vertex*>::iterator iter = m_vertices.begin();
            iter != m_vertices.end(); ++iter){

    	indice += 1;
        Vertex* ve = *iter;

        Utils::Math::Point pt1 = ve->getCoord();
        Utils::Math::Point pt2;

        Geom::GeomEntity* ge = ve->getGeomAssociation();
        if (ge == 0){
            // cas sans association, on projette sur le segment entre les 2 extrémités
            try {
                segment->project(pt1, pt2);
            }
            catch (const Standard_Failure& exc){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message << "OCC a échoué, projection impossible pour le sommet "
                        << ve->getName() << ", sur le segment créé";
                throw TkUtil::Exception (message);
            }
            // si le sommet se retrouve à une extrémité, on l'en écarte
            if (pt2 == m_p1 || pt2 == m_p2)
            	pt2 = (indice*m_p1+(1+m_vertices.size()-indice)*m_p2)/(m_vertices.size()+1);

        }
        else if (ge->getType() == Utils::Entity::GeomCurve
                || ge->getType() == Utils::Entity::GeomSurface){
            // cas d'une association du type courbe ou surface,
            // on projette itérativement le point avec l'un puis l'autre
            // TODO [EB] Utiliser si possible OCC pour intersection entre 2 courbes ou courbe avec surface
            try {
                Utils::Math::Point pt3;
                pt2 = pt1;
                uint nb_iter = 0;
                do {
                    // pour mémoriser le pt de départ de l'itération
                    pt3 = pt2;
                    segment->project(pt2);
                    // si le sommet se retrouve à une extrémité, on l'en écarte
                    if (pt2 == m_p1 || pt2 == m_p2)
                    	pt2 = (indice*m_p1+(1+m_vertices.size()-indice)*m_p2)/(m_vertices.size()+1);

                    ge->project(pt2);
                    nb_iter++;
                } while (!(pt2 == pt3) && nb_iter<10);
                //std::cout<<"nb_iter = "<<nb_iter<<" pour sommet "<<ve->getName()<<std::endl;
            }
            catch (const Standard_Failure& exc){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message << "OCC a échoué, impossible de projetter "
                        << ve->getName();
                throw TkUtil::Exception (message);
            }
        }
        else {
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "Alignement impossible pour le sommet \""
                    << ve->getName() << "\", \n";
            message << "il est projetée sur autre chose qu'une courbe ou une surface";
            throw TkUtil::Exception (message);
        }

        if (!(pt1 == pt2)){
            ve->saveVertexGeomProperty(&getInfoCommand(), true);
            ve->setCoord(pt2);
        } // end if (pt1 != pt2)

    } // end for iter


    delete segment;
    delete vtx1;
    delete vtx2;

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandAlignVertices::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
