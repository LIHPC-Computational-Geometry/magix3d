//
// Created by calderans on 10/29/24.
//
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include "Topo/CommandAlignOnSurface.h"
#include "Geom/IntersectionSurfaceLine.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandAlignOnSurface::CommandAlignOnSurface(Internal::Context &c, Geom::GeomEntity* surface, Vertex *v, const Point pnt1, const Point pnt2)
:CommandEditTopo(c, "Alignement d'un sommet topologique contraint sur une surface géométrique."),
m_surface(surface),
m_vertex(v),
m_pnt1(pnt1),
m_pnt2(pnt2)
{
    TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

    comments << "Alignement du sommet topologiques";
    comments << " " << m_vertex->getName();
    comments << " contraint sur la surface géométrique";
    comments << " " << m_surface->getName();

    setScriptComments(comments);
    setName(comments);

    if (m_pnt1 == m_pnt2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Il faut 2 points distincts pour définir une droite", TkUtil::Charset::UTF_8));

    CHECK_NULL_PTR_ERROR(m_vertex)
}
/*----------------------------------------------------------------------------*/
CommandAlignOnSurface::~CommandAlignOnSurface() {

}
/*----------------------------------------------------------------------------*/
void CommandAlignOnSurface::internalExecute() {

        TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "CommandAlignOnSurface::execute pour la commande " << getName ( )
                << " de nom unique " << getUniqueName ( );

        //m_vertex->saveTopoProperty();

        m_dir = *new Vector(m_pnt1,m_pnt2);

        Geom::IntersectionSurfaceLine intersector(m_surface,m_pnt1, m_dir);
        intersector.perform();
        int nbPnt = intersector.getNbPnt();
        if(nbPnt != 1){
            TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "Alignement impossible pour le sommet \""
                    << m_vertex->getName() << "\", ";
            message << "sur la surface \""
                    << m_surface->getName() << "\". ";

            if(nbPnt == 0){
                message << "Aucune intersection pour la direction ";
            }else if(nbPnt > 1){
                message << "Intersection multiple pour la direction ";
            }
            message << "("<<m_dir.getX()<< ","<<m_dir.getY()<< ","<<m_dir.getZ()<<") ";
            message << "d'origine "<<"("<<m_dir.getX()<< ","<<m_dir.getY()<< ","<<m_dir.getZ()<<")";
            throw TkUtil::Exception (message);
        }

        Point pnt = intersector.getIntersection();
        m_vertex->saveVertexGeomProperty(&getInfoCommand(), true);
        m_vertex->setCoord(pnt);
        m_vertex->setGeomAssociation(m_surface);

        saveInternalsStats();

        log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandAlignOnSurface::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
    return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/