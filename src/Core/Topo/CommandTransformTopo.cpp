/*----------------------------------------------------------------------------*/
#include "Topo/CommandTransformTopo.h"
#include "Topo/TopoManager.h"
#include "Topo/Block.h"
#include "Topo/TopoHelper.h"
#include "Topo/CommandDuplicateTopo.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
#include <gp_Trsf.hxx>
#include <gp_GTrsf.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandTransformTopo::
CommandTransformTopo(Internal::Context& c, std::string name, std::vector<TopoEntity*>& ve)
:CommandEditTopo(c, name)
, m_entities(ve)
, m_buildEntitiesCmd(0)
, m_all_topo(false)
{
}
/*----------------------------------------------------------------------------*/
CommandTransformTopo::CommandTransformTopo(Internal::Context& c,
		std::string name,
		CommandDuplicateTopo* cmd,
		bool all_topo)
:CommandEditTopo(c, name)
, m_buildEntitiesCmd(cmd)
, m_all_topo(all_topo)
{

}
/*----------------------------------------------------------------------------*/
CommandTransformTopo::
CommandTransformTopo(Internal::Context& c, std::string name)
:CommandEditTopo(c, name)
, m_buildEntitiesCmd(0)
, m_all_topo(true)
{
	// on prend tous les blocs et les faces
	std::vector<Topo::Block* > blocs;
	std::vector<Topo::CoFace* > cofaces;

	getTopoManager().getBlocks(blocs);
	getTopoManager().getCoFaces(cofaces);
	m_entities.insert(m_entities.end(), blocs.begin(), blocs.end());
	m_entities.insert(m_entities.end(), cofaces.begin(), cofaces.end());
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
init()
{
	if (m_buildEntitiesCmd){
		// récupération des entités

		std::vector<Topo::Block* > blocs;
		m_buildEntitiesCmd->get(blocs);
		m_entities.insert(m_entities.end(), blocs.begin(), blocs.end());

		std::vector<Topo::CoFace* > cofaces;
		m_buildEntitiesCmd->get(cofaces);
		m_entities.insert(m_entities.end(), cofaces.begin(), cofaces.end());

		// on fait l'impasse sur les autres types
		// on part du fait que l'on a comme topologie soit des blocs pour le 3d, soit des cofaces pour le 2d
	}
}
/*----------------------------------------------------------------------------*/
CommandTransformTopo::
~CommandTransformTopo()
{
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
transform(gp_Trsf* transf)
{
	init();
    // filtre sur les entités pour ne les prendre qu'une fois
    std::map<TopoEntity*, bool> filtre;

    // il faut parcourir toutes les entités et modififier les sommets une seule fois chacun
    for (std::vector<TopoEntity*>::iterator iter = m_entities.begin();
            iter != m_entities.end(); ++iter){

        if ((*iter)->getDim() == 3)
            transform((Block*)(*iter), transf, filtre);
        else if ((*iter)->getDim() == 2)
            transform((CoFace*)(*iter), transf, filtre);
        else if ((*iter)->getDim() == 1)
            transform((CoEdge*)(*iter), transf, filtre);
        else if ((*iter)->getDim() == 0)
            transform((Vertex*)(*iter), transf, filtre);

    } // end for iter
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
transform(gp_GTrsf* transf)
{
	init();
    // filtre sur les entités pour ne les prendre qu'une fois
    std::map<TopoEntity*, bool> filtre;

    // il faut parcourir toutes les entités et modififier les sommets une seule fois chacun
    for (std::vector<TopoEntity*>::iterator iter = m_entities.begin();
            iter != m_entities.end(); ++iter){

        if ((*iter)->getDim() == 3)
            transform((Block*)(*iter), transf, filtre);
        else if ((*iter)->getDim() == 2)
            transform((CoFace*)(*iter), transf, filtre);
        else if ((*iter)->getDim() == 1)
            transform((CoEdge*)(*iter), transf, filtre);
        else if ((*iter)->getDim() == 0)
            transform((Vertex*)(*iter), transf, filtre);

    } // end for iter
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
transform(Block* bl, gp_Trsf* transf, std::map<TopoEntity*, bool>& filtre)
{
    if (filtre[bl] == 1)
        return;

    std::vector<Face* > faces;
    bl->getFaces(faces);
    for (std::vector<Face* >::iterator iter1 = faces.begin();
            iter1 != faces.end(); ++iter1){
        std::vector<CoFace* > cofaces;
        (*iter1)->getCoFaces(cofaces);
        for (std::vector<CoFace* >::iterator iter2 = cofaces.begin();
                iter2 != cofaces.end(); ++iter2){
            transform(*iter2, transf, filtre);
        }
    }

    filtre[bl] = 1;
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
transform(Block* bl, gp_GTrsf* transf, std::map<TopoEntity*, bool>& filtre)
{
    if (filtre[bl] == 1)
        return;

    std::vector<Face* > faces;
    bl->getFaces(faces);
    for (std::vector<Face* >::iterator iter1 = faces.begin();
            iter1 != faces.end(); ++iter1){
        std::vector<CoFace* > cofaces;
        (*iter1)->getCoFaces(cofaces);
        for (std::vector<CoFace* >::iterator iter2 = cofaces.begin();
                iter2 != cofaces.end(); ++iter2){
            transform(*iter2, transf, filtre);
        }
    }

    filtre[bl] = 1;
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
transform(CoFace* co, gp_Trsf* transf, std::map<TopoEntity*, bool>& filtre)
{
    if (filtre[co] == 1)
         return;

    std::vector<Edge* > edges;
    co->getEdges(edges);
    for (std::vector<Edge* >::iterator iter1 = edges.begin();
            iter1 != edges.end(); ++iter1){
        std::vector<CoEdge* > coedges;
        (*iter1)->getCoEdges(coedges);
        for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
                iter2 != coedges.end(); ++iter2){
            transform(*iter2, transf, filtre);
        }
    }

    filtre[co] = 1;
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
transform(CoFace* co, gp_GTrsf* transf, std::map<TopoEntity*, bool>& filtre)
{
    if (filtre[co] == 1)
         return;

    std::vector<Edge* > edges;
    co->getEdges(edges);
    for (std::vector<Edge* >::iterator iter1 = edges.begin();
            iter1 != edges.end(); ++iter1){
        std::vector<CoEdge* > coedges;
        (*iter1)->getCoEdges(coedges);
        for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
                iter2 != coedges.end(); ++iter2){
            transform(*iter2, transf, filtre);
        }
    }

    filtre[co] = 1;
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
transform(CoEdge* co, gp_Trsf* transf, std::map<TopoEntity*, bool>& filtre)
{
    if (filtre[co] == 1)
         return;

    const std::vector<Vertex* > & vertices = co->getVertices();
    for (std::vector<Vertex* >::const_iterator iter1 = vertices.begin();
            iter1 != vertices.end(); ++iter1){
        transform(*iter1, transf, filtre);
    }
    if (co->getMeshingProperty()->isPolarCut()){
    	Utils::Math::Point pt = co->getMeshingProperty()->getPolarCenter();
    	double x = pt.getX();
    	double y = pt.getY();
    	double z = pt.getZ();
    	transf->Transforms(x, y, z);
    	co->saveCoEdgeMeshingProperty(&getInfoCommand());
    	co->getMeshingProperty()->setPolarCenter(Utils::Math::Point(x, y, z));
    }

    filtre[co] = 1;
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
transform(CoEdge* co, gp_GTrsf* transf, std::map<TopoEntity*, bool>& filtre)
{
    if (filtre[co] == 1)
         return;

    const std::vector<Vertex* > & vertices = co->getVertices();
    for (std::vector<Vertex* >::const_iterator iter1 = vertices.begin();
            iter1 != vertices.end(); ++iter1){
        transform(*iter1, transf, filtre);
    }
    if (co->getMeshingProperty()->isPolarCut()){
    	Utils::Math::Point pt = co->getMeshingProperty()->getPolarCenter();
    	double x = pt.getX();
    	double y = pt.getY();
    	double z = pt.getZ();
    	transf->Transforms(x, y, z);
    	co->saveCoEdgeMeshingProperty(&getInfoCommand());
    	co->getMeshingProperty()->setPolarCenter(Utils::Math::Point(x, y, z));
    }

    filtre[co] = 1;
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
transform(Vertex* vtx, gp_Trsf* transf, std::map<TopoEntity*, bool>& filtre)
{
   if (filtre[vtx] == 1)
       return;

   vtx->saveVertexGeomProperty(&getInfoCommand(), true);
   Utils::Math::Point pt = vtx->getCoord();
   double x = pt.getX();
   double y = pt.getY();
   double z = pt.getZ();

   transf->Transforms(x, y, z);

   // si c'est pour toute la topo, alors cela peut être maillé
   vtx->setCoord(Utils::Math::Point(x, y, z), m_all_topo);

   filtre[vtx] = 1;
}
/*----------------------------------------------------------------------------*/
void CommandTransformTopo::
transform(Vertex* vtx, gp_GTrsf* transf, std::map<TopoEntity*, bool>& filtre)
{
   if (filtre[vtx] == 1)
       return;

   vtx->saveVertexGeomProperty(&getInfoCommand(), true);
   Utils::Math::Point pt = vtx->getCoord();
   double x = pt.getX();
   double y = pt.getY();
   double z = pt.getZ();

   transf->Transforms(x, y, z);

   // si c'est pour toute la topo, alors cela peut être maillé
   vtx->setCoord(Utils::Math::Point(x, y, z), m_all_topo);

   filtre[vtx] = 1;
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
