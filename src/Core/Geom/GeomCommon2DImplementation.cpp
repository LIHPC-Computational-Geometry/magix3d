/*----------------------------------------------------------------------------*/
/** \file GeomCommon2DImplementation.cpp
 *
 *  \author Eric B
 *
 *  \date 7/9/2018
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <map>
#include <set>
#include <algorithm>
/*----------------------------------------------------------------------------*/
#include "Geom/GeomCommon2DImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/EntityFactory.h"
#include "Geom/CommandGeomCopy.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopoDS.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRep_Builder.hxx>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomCommon2DImplementation::
GeomCommon2DImplementation(Internal::Context& c, GeomEntity* e1, GeomEntity* e2)
: GeomModificationBaseClass(c), m_geomCmd(0), m_entity1(e1), m_entity2(e2)
{
}
/*----------------------------------------------------------------------------*/
GeomCommon2DImplementation::
GeomCommon2DImplementation(Internal::Context& c, Geom::CommandGeomCopy* geom_cmd)
: GeomModificationBaseClass(c), m_geomCmd(geom_cmd), m_entity1(0), m_entity2(0)
{
}
/*----------------------------------------------------------------------------*/
GeomCommon2DImplementation::~GeomCommon2DImplementation()
{}
/*----------------------------------------------------------------------------*/
void GeomCommon2DImplementation::prePerform()
{
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================
	if (m_geomCmd){
		std::vector<GeomEntity*>& newEntities = m_geomCmd->getNewEntities();
		uint dim = 0;
		if (m_geomCmd->getRefEntities(2).size())
			dim = 2; // cas de l'intersection de surfaces
		else if (m_geomCmd->getRefEntities(1).size())
			dim = 1; // cas de l'intersection de courbes
		else
			throw TkUtil::Exception("Erreur interne, la copie n'a pas d'entités d'une dimension attendue");

		std::vector<GeomEntity*>entities;
		for (uint i=0; i<newEntities.size(); i++)
			if (newEntities[i]->getDim() == dim)
				entities.push_back(newEntities[i]);

		if (entities.size() != 2)
			throw TkUtil::Exception("Erreur interne, la copie n'a pas donné 2 entités");

		m_entity1 = entities[0];
		m_entity2 = entities[1];
		init(entities);
	}
	else {
		if (m_entity1==0 || m_entity2==0)
			throw TkUtil::Exception("Erreur interne, les entités ne sont pas initialisées");

		std::vector<GeomEntity*> entities;
		entities.push_back(m_entity1);
		entities.push_back(m_entity2);
		init(entities);
	}
}
/*----------------------------------------------------------------------------*/
void GeomCommon2DImplementation::perform(std::vector<GeomEntity*>& res)
{
	if (m_entity1->getDim() == 2 && m_entity2->getDim() == 2)
		commonSurfaces();
	else if (m_entity1->getDim() == 1 && m_entity2->getDim() == 1)
		commonCurves();
	else
		throw TkUtil::Exception("Erreur interne, cas non prévu dans GeomCommon2DImplementation::perform");
}
/*----------------------------------------------------------------------------*/
void GeomCommon2DImplementation::commonSurfaces()
{
    TopoDS_Shape s1, s2;
	if (m_entity1->getComputationalProperties().size()>1){
		// création d'un Shell
		std::vector<TopoDS_Shape> topoS;
		getOCCShapes(m_entity1, topoS);

		BRep_Builder B;
		TopoDS_Shell aShell;
		B.MakeShell(aShell);
		for (uint i=0; i<topoS.size(); i++){
			B.Add(aShell,topoS[i]);
		}
		s1 = aShell;
	}
	else
		getOCCShape(m_entity1, s1);

	if (m_entity2->getComputationalProperties().size()>1){
		// création d'un Shell
		std::vector<TopoDS_Shape> topoS;
		getOCCShapes(m_entity2, topoS);

		BRep_Builder B;
		TopoDS_Shell aShell;
		B.MakeShell(aShell);
		for (uint i=0; i<topoS.size(); i++){
			B.Add(aShell,topoS[i]);
		}
		s2 = aShell;
	}
	else
		getOCCShape(m_entity2, s2);


    BRepAlgoAPI_Section intersector(s1, s2);

    if (intersector.IsDone()){
    	TopoDS_Shape sh = intersector.Shape();

    	TopTools_IndexedMapOfShape  vertices, curves;
    	TopExp::MapShapes(sh,TopAbs_VERTEX, vertices);
    	TopExp::MapShapes(sh,TopAbs_EDGE, curves);

#ifdef _DEBUG
    	std::cout<<"création d'une courbe avec "<<vertices.Extent()<<" sommets et "<<curves.Extent()<<" courbes "<<std::endl;

#endif

    	if (!sh.IsNull() && vertices.Extent())
    		createGeomEntities(sh);
    }
	 else {
     	throw TkUtil::Exception(TkUtil::UTF8String ("Problème OCC lors de l'intersection entre 2 surfaces", TkUtil::Charset::UTF_8));
	 }

    // suppression des volumes incidents
    std::vector<Volume*> adj;
    m_entity1->get(adj);

#ifdef _DEBUG
    for (uint j=0; j<adj.size(); j++)
    	std::cout<<"commonSurfaces: on supprime "<<adj[j]->getName()<<std::endl;
#endif
    m_removedEntities.insert(m_removedEntities.end(), adj.begin(), adj.end());

    m_entity2->get(adj);

#ifdef _DEBUG
    for (uint j=0; j<adj.size(); j++)
    	std::cout<<"commonSurfaces: on supprime "<<adj[j]->getName()<<std::endl;
#endif
    m_removedEntities.insert(m_removedEntities.end(), adj.begin(), adj.end());

}
/*----------------------------------------------------------------------------*/
void GeomCommon2DImplementation::commonCurves()
{
	/* exemple d'utilisation

ctx.getGeomManager().newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "B")
ctx.getGeomManager().newCylinder (Mgx3D.Point(-.5, .5, .5), 1, Mgx3D.Vector(10, 10, 1), 9.000000e+01, "C")
crb=ctx.getGeomManager().copy(["Crb0002", "Crb0020"], True,"TMP")
ctx.getGeomManager().common2D("Crb0021", "Crb0022", "INTER")

	 */
    TopoDS_Shape s1, s2;
    getOCCShape(m_entity1, s1);
    getOCCShape(m_entity2, s2);


    BRepAlgoAPI_Section intersector(s1, s2);

    if (intersector.IsDone()){
    	TopoDS_Shape sh = intersector.Shape();

    	TopTools_IndexedMapOfShape  vertices;
    	TopExp::MapShapes(sh,TopAbs_VERTEX, vertices);

#ifdef _DEBUG
    	std::cout<<"création de "<<vertices.Extent()<<" sommets"<<std::endl;

#endif

    	if (!sh.IsNull() && vertices.Extent())
    		createGeomEntities(sh, false, false); // ne doit pas supprimer les sommets seuls
    }
	 else {
     	throw TkUtil::Exception(TkUtil::UTF8String ("Problème OCC lors de l'intersection entre 2 courbes", TkUtil::Charset::UTF_8));
	 }

    // suppression des surfaces incidentes
    std::vector<Surface*> adj;
    m_entity1->get(adj);

#ifdef _DEBUG
    for (uint j=0; j<adj.size(); j++)
    	std::cout<<"commonCurves: on supprime "<<adj[j]->getName()<<std::endl;
#endif
    m_removedEntities.insert(m_removedEntities.end(), adj.begin(), adj.end());

    m_entity2->get(adj);

#ifdef _DEBUG
    for (uint j=0; j<adj.size(); j++)
    	std::cout<<"commonCurves: on supprime "<<adj[j]->getName()<<std::endl;
#endif
    m_removedEntities.insert(m_removedEntities.end(), adj.begin(), adj.end());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
