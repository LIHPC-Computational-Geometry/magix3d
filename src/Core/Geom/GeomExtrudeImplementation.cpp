/*----------------------------------------------------------------------------*/
/** \file  GeomExtrudeImplementation.cpp
 *
 *  Created on: 8/11/2019
 *      Author: Eric B
 *      à partir de GeomRevolImplementation
 *
 *
 *
 *  Modified on: 21/02/2022
 *      Author: Simon C
 *      ajout de la possibilité de conserver les entités extrudées
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <set>
/*----------------------------------------------------------------------------*/
#include "Geom/GeomExtrudeImplementation.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
//inclusion de fichiers en-tête d'Open Cascade
#include <TopoDS_Shape.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepTools.hxx>
/*----------------------------------------------------------------------------*/
#include <TkUtil/UTF8String.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
//#define _DEBUG2
/*----------------------------------------------------------------------------*/
GeomExtrudeImplementation::
GeomExtrudeImplementation(Internal::Context& c,
		std::vector<GeomEntity*> entities,
		const Vector& dp,
		const bool keep)
: GeomModificationBaseClass(c),
  m_vector(dp), m_keep(keep)
{
#ifdef _DEBUG2
	std::cout<<"vecteur : "<<dp<<std::endl;
#endif
    m_entities_param.insert(m_entities_param.end(),entities.begin(),entities.end());

}
/*----------------------------------------------------------------------------*/
GeomExtrudeImplementation::~GeomExtrudeImplementation()
{}
/*----------------------------------------------------------------------------*/
void GeomExtrudeImplementation::perform(std::vector<GeomEntity*>& res){
    throw TkUtil::Exception(TkUtil::UTF8String ("Ne pas utiliser", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void GeomExtrudeImplementation::prePerform()
{
    //========================================================================
    // Mise à jour des connectivés de références
    //========================================================================
    init(m_entities_param);
}
/*----------------------------------------------------------------------------*/
void GeomExtrudeImplementation::perform(std::vector<GeomEntity*>& res,
        std::map<Geom::Vertex* ,Geom::Vertex*> & v2v,
        std::map<Geom::Vertex* ,Geom::Vertex*> & v2vOpp,
        std::map<Geom::Vertex* ,Geom::Curve*>  & v2c,
        std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
        std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
        std::map<Geom::Curve*  ,Geom::Surface*>  & c2s,
        std::map<Geom::Surface*,Geom::Surface*>& s2s,
        std::map<Geom::Surface*,Geom::Surface*>& s2sOpp,
        std::map<Geom::Surface*,Geom::Volume*> & s2v)
{
    /* l'extrusion ne peut se faire qu'avec des entites de dimension
     * inférieure à 3
     */
    for(unsigned int i=1;i<m_init_entities.size();i++){
        int dimi = m_init_entities[i]->getDim();
        if(dimi==3)
            throw TkUtil::Exception(TkUtil::UTF8String ("l'extrusion ne s'applique qu'à des objets de dimension inférieur à 3", TkUtil::Charset::UTF_8));
    }

    /* le vecteur d'extrusion ne doit pas être de longueur nulle */
    if(m_vector.abs2() == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Vecteur de longueur nulle", TkUtil::Charset::UTF_8));

    // on fait l'extrusion de chaque entité selon sa dimension et on met à jour les maps de
    // correspondances
    std::list<GeomEntity*>::iterator it;
    for(it=m_ref_entities[0].begin();it!=m_ref_entities[0].end();it++){
        Vertex* current_vertex = dynamic_cast<Vertex*>(*it);
        CHECK_NULL_PTR_ERROR(current_vertex);
        makeExtrude(current_vertex,res,v2v,v2vOpp,v2c);
    }


    for(it=m_ref_entities[1].begin();it!=m_ref_entities[1].end();it++){
        Curve* current_curve= dynamic_cast<Curve*>(*it);
        CHECK_NULL_PTR_ERROR(current_curve);
        makeExtrudeComposite(current_curve,res,v2v,v2vOpp,v2c,c2c,c2cOpp,c2s);
    }


    for(it=m_ref_entities[2].begin();it!=m_ref_entities[2].end();it++){
        Surface* current_surf= dynamic_cast<Surface*>(*it);
        CHECK_NULL_PTR_ERROR(current_surf);
        makeExtrude(current_surf,res,v2v,v2vOpp,v2c,c2c,c2cOpp,c2s,s2s,s2sOpp,s2v);
    }

    if(!m_keep) {
        for (int i = 0; i < 3; i++) {
            std::list<GeomEntity *>::iterator it_rem;
            for (it_rem = m_ref_entities[i].begin(); it_rem != m_ref_entities[i].end(); it_rem++) {
                m_removedEntities.push_back(*it_rem);
            }
        }
    }


}
/*----------------------------------------------------------------------------*/
void GeomExtrudeImplementation::makeExtrude(Vertex* v,std::vector<GeomEntity*>& res,
        std::map<Geom::Vertex*,Geom::Vertex*>& v2v,
        std::map<Geom::Vertex*,Geom::Vertex*>& v2vOpp,
        std::map<Geom::Vertex*,Geom::Curve*>& v2c)
{
#ifdef _DEBUG2
	std::cout<<"GeomExtrudeImplementation::makeExtrude ("<<v->getName()<<")"<<std::endl;
#endif
    TopoDS_Shape s;
    getOCCShape(v, s);

    gp_Vec Vx(m_vector.getX(), m_vector.getY(), m_vector.getZ());
    BRepPrimAPI_MakePrism mkR(s, Vx);

    if (!mkR.IsDone () || mkR.Shape().IsNull())
    {
    	throw TkUtil::Exception(TkUtil::UTF8String ("OCC n'a pas pu effectuer une extrusion d'un sommet", TkUtil::Charset::UTF_8));
    }
    TopoDS_Shape sh = mkR.Shape();


    if (sh.ShapeType()==TopAbs_EDGE){
    	TopoDS_Edge e = TopoDS::Edge(sh);
    	if (BRep_Tool::Degenerated(e)){
    		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    		message <<"Une courbe dégénérée pose pb lors de l'extrusion de l'entité géométrique "<<v->getName();
    		throw TkUtil::Exception(message);
    	}
    	TopTools_IndexedMapOfShape map_result;
    	TopExp::MapShapes(e,TopAbs_VERTEX, map_result);
    	if (map_result.Extent()>2)
    	{
    		throw TkUtil::Exception(TkUtil::UTF8String ("OCC n'a pas pu effectuer une extrusion d'un sommet", TkUtil::Charset::UTF_8));
    	}

    	// ON CREE LA COURBE ET ON L'ASSOCIE
    	Curve* curve =EntityFactory(m_context).newOCCCurve(e);
    	m_newEntities.push_back(curve);
    	m_newCurves.push_back(curve);
    	res.push_back(curve);
    	v2c[v] = curve;

    	//cas general (2 normalement)
    	for(int i = 1; i <= map_result.Extent(); i++)
    	{
    		TopoDS_Vertex v_occ =  TopoDS::Vertex(map_result(i));

    		Vertex* v_m3d=EntityFactory(m_context).newOCCVertex(v_occ);
    		if(OCCGeomRepresentation::areEquals(v_occ,TopoDS::Vertex(s)))
    		{
    			v2v[v]= v_m3d;
    			m_v2v_inv[v_m3d]=v;
#ifdef _DEBUG2
    			std::cout<<" sommet remplacé par "<<v_m3d->getName()<<", relié à "<<curve->getName()<<std::endl;
#endif
    		}
    		else {
    			v2vOpp[v]= v_m3d;
#ifdef _DEBUG2
    			std::cout<<" sommet opposé "<<v_m3d->getName()<<", relié à "<<curve->getName()<<std::endl;
#endif
    		}
    		m_newEntities.push_back(v_m3d);
    		m_newVertices.push_back(v_m3d);
    		res.push_back(v_m3d);
    		// on crée le lien C->V
    		curve->add(v_m3d);
    		// on crée le lien V->C
    		v_m3d->add(curve);
    	}
    }
    else
    	throw TkUtil::Exception(TkUtil::UTF8String ("OCC n'a pas pu effectuer une extrusion d'un sommet : pas une EDGE", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void GeomExtrudeImplementation::makeExtrudeComposite(Curve* curve,std::vector<GeomEntity*>& res,
        std::map<Geom::Vertex*,Geom::Vertex*>  & v2v,
        std::map<Geom::Vertex*,Geom::Vertex*>  & v2vOpp,
        std::map<Geom::Vertex*,Geom::Curve*>   & v2c,
        std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
        std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
        std::map<Geom::Curve*  ,Geom::Surface*>& c2s)
{
#ifdef _DEBUG2
	std::cout<<"GeomExtrudeImplementation::makeExtrude ("<<curve->getName()<<")"<<std::endl;
#endif
    // cas d'une courbe composée de plusieurs shapes
	std::vector<GeomRepresentation*> ppties = curve->getComputationalProperties();

	// les différentes occ_shape de la courbe composite
    std::vector<TopoDS_Edge> v_shape;
    for (uint i=0; i<ppties.size(); i++){
    	OCCGeomRepresentation* occ_rep = dynamic_cast<OCCGeomRepresentation*>(ppties[i]);
    	CHECK_NULL_PTR_ERROR(occ_rep);
    	v_shape.push_back(TopoDS::Edge(occ_rep->getShape()));
    }

    // ON AURA BESOIN DES SOMMETS DE LA COURBE DE DEPART PAR LA SUITE
    std::vector<Vertex*> c_vertices;
    curve->get(c_vertices);

    if(c_vertices.size()!=2){
        std::cerr<<"Courbe (posant problème): "<<curve->getName()<<", avec comme sommet:";
        for (uint i=0; i<c_vertices.size(); i++)
            std::cerr<<" "<<c_vertices[i]->getName();
        std::cerr<<std::endl;
        throw TkUtil::Exception(TkUtil::UTF8String ("Une configuration imprévue a été rencontrée lors de l'extrusion d'une courbe composée: attention une courbe n'a pas deux sommets", TkUtil::Charset::UTF_8));
    }

    // les surfaces créées
    std::vector<TopoDS_Face> v_faces;
    // les courbes opposées
    std::vector<TopoDS_Edge> v_shape_opp;

    gp_Vec Vx(m_vector.getX(), m_vector.getY(), m_vector.getZ());

    // extrusion pour les différentes shapes
    for (uint i=0; i<v_shape.size(); i++){
    	TopoDS_Shape shape = v_shape[i];
        BRepPrimAPI_MakePrism mkR(shape, Vx);

        TopoDS_Shape sh = mkR.Shape();

        if (sh.ShapeType()!=TopAbs_FACE)
        	throw TkUtil::Exception(TkUtil::UTF8String ("configuration imprévue lors de l'extrusion d'une courbe: pas de surface générée", TkUtil::Charset::UTF_8));

        TopoDS_Face f = TopoDS::Face(sh);
        v_faces.push_back(f);

        TopTools_IndexedMapOfShape map_result_vertex;
        TopExp::MapShapes(f,TopAbs_VERTEX, map_result_vertex);


        if(map_result_vertex.Extent()==4 || map_result_vertex.Extent()==3)
        {
        	// les 2 sommets occ de la partie de la courbe occ
        	TopoDS_Vertex v1_occ, v2_occ;

        	TopTools_IndexedMapOfShape local_map_curve;
        	TopExp::MapShapes(shape,TopAbs_VERTEX, local_map_curve);
        	if(local_map_curve.Extent()!=2)
        		throw TkUtil::Exception(TkUtil::UTF8String ("Une configuration imprévue a été rencontrée lors de l'extrusion d'une courbe : il n'y a pas 2 sommets pour une courbe OCC", TkUtil::Charset::UTF_8));
        	v1_occ = TopoDS::Vertex(local_map_curve(1));
        	v2_occ = TopoDS::Vertex(local_map_curve(2));

        	// sommet occ sur l'axe
        	TopoDS_Vertex v_fixe_occ;
        	if (map_result_vertex.Extent()==3){
        		Vertex *v_fixe = 0;
        		for (uint j=0; j<2; j++)
        			if (v2vOpp[c_vertices[j]] == 0)
        				v_fixe = v2v[c_vertices[j]];

        		if (v_fixe == 0){
        			throw TkUtil::Exception(TkUtil::UTF8String ("Une configuration imprévue a été rencontrée lors de l'extrusion d'une courbe composée avec 3 sommets pour une surface: on ne trouve pas de sommet sur l'axe", TkUtil::Charset::UTF_8));
        		}
        		getOCCShape(v_fixe, v_fixe_occ);
        	}

        	// les sommets oppposés (ceux qui ne sont pas dans la courbe)
        	std::vector<TopoDS_Vertex> v_opp_vertices;
        	TopTools_IndexedMapOfShape local_map_face;
        	TopExp::MapShapes(f,TopAbs_VERTEX, local_map_face);

        	for (uint j=1; j<=local_map_face.Extent(); j++)
        		if (BRepTools::Compare(TopoDS::Vertex(local_map_face(j)),v1_occ)==false
        				&& BRepTools::Compare(TopoDS::Vertex(local_map_face(j)),v2_occ)==false)
        			v_opp_vertices.push_back(TopoDS::Vertex(local_map_face(j)));
        	if (map_result_vertex.Extent()==3)
        		v_opp_vertices.push_back(v_fixe_occ);

        	if (v_opp_vertices.size() != 2)
        		throw TkUtil::Exception(TkUtil::UTF8String ("Une configuration imprévue a été rencontrée lors de l'extrusion d'une courbe composée: on ne trouve pas 2 sommets occ en face de la courbe", TkUtil::Charset::UTF_8));

        	// recherche de l'edge contenant les 2 sommets opposés
        	TopoDS_Edge s_opp;
        	bool find_edge = false;

        	// on récupère les 3 ou 4 courbes de la surface obtenues par extrusion
        	TopTools_IndexedMapOfShape map_result_edge;
        	TopExp::MapShapes(f,TopAbs_EDGE, map_result_edge);
        	if(map_result_edge.Extent()!=4 && map_result_edge.Extent()!=3){
        		std::cout<<"map_result_edge.Extent() = "<<map_result_edge.Extent()<<std::endl;
        		std::cout<<"map_result_vertex.Extent() = "<<map_result_vertex.Extent()<<std::endl;
        		throw TkUtil::Exception(TkUtil::UTF8String ("Une configuration imprévue a été rencontrée lors de l'extrusion d'une courbe: la surface obtenue par extrusion n'a pas 3 ou 4 courbes", TkUtil::Charset::UTF_8));
        	}
        	for (uint j=1; j<=map_result_edge.Extent(); j++){
        		TopoDS_Edge loc_edge = TopoDS::Edge(map_result_edge(j));
        		if (findVertices(loc_edge, v_opp_vertices[0], v_opp_vertices[1])){
        			find_edge = true;
        			s_opp = loc_edge;
        		}
        	}
        	v_shape_opp.push_back(s_opp);

        }

    } // end for (uint i=0; i<v_shape.size(); i++)

    Surface* surf = 0;
    if (v_faces.size() == 0)
    	surf = 0;
    else if (v_faces.size() == 1)
    	surf = EntityFactory(m_context).newOCCSurface(v_faces[0]);
    else
    	surf = EntityFactory(m_context).newOCCCompositeSurface(v_faces);

    if (surf){
    	m_newEntities.push_back(surf);
    	m_newSurfaces.push_back(surf);
    	res.push_back(surf);
    	c2s[curve] = surf;
#ifdef _DEBUG2
    	std::cout<<" création de "<<surf->getName()<<std::endl;
#endif
    }
    else {
#ifdef _DEBUG2
    	std::cout<<" pas de surface créée"<<std::endl;
#endif
    }
    Vertex *v1=0, *v2=0, *v1Opp=0, *v2Opp=0;
    v1 = c_vertices[0];
    v2 = c_vertices[1];
    v1Opp = v2vOpp[v1];
    if (v1Opp == 0)
    	v1Opp = v2v[v1];
    v2Opp = v2vOpp[v2];
    if (v2Opp == 0)
    	v2Opp = v2v[v2];
#ifdef _DEBUG2
    std::cout<<" v1 : "<<v1->getName()<<std::endl;
    std::cout<<" v2 : "<<v2->getName()<<std::endl;
    std::cout<<" v1Opp : "<<v1Opp->getName()<<std::endl;
    std::cout<<" v2Opp : "<<v2Opp->getName()<<std::endl;
#endif

    Utils::Math::Point pt1 = v1->getCoord();
    Utils::Math::Point pt2 = v2->getCoord();
    Curve* c_copy=0;
    if (v_shape.size() == 1)
    	c_copy = EntityFactory(m_context).newOCCCurve(v_shape[0]);
    else
    	c_copy = EntityFactory(m_context).newOCCCompositeCurve(v_shape, pt1, pt2);
#ifdef _DEBUG2
    std::cout<<" création de la copie "<<c_copy->getName()<<std::endl;
#endif

    pt1 = v1Opp->getCoord();
    pt2 = v2Opp->getCoord();
    Curve* c_opp=0;
    if (v_shape_opp.size() == 0)
    	c_opp = 0;
    else if (v_shape_opp.size() == 1)
    	c_opp = EntityFactory(m_context).newOCCCurve(v_shape_opp[0]);
    else
    	c_opp = EntityFactory(m_context).newOCCCompositeCurve(v_shape_opp, pt1, pt2);
    if (c_copy){
    	m_newEntities.push_back(c_copy);
    	m_newCurves.push_back(c_copy);
        res.push_back(c_copy);
        c2c[curve] = c_copy;

    }
    if (c_opp){
    	m_newEntities.push_back(c_opp);
    	m_newCurves.push_back(c_opp);
        res.push_back(c_opp);
        c2cOpp[curve] = c_opp;
#ifdef _DEBUG2
        std::cout<<" création de l'opposée "<<c_opp->getName()<<std::endl;
#endif
    }

    v1 = v2v[v1];
    v2 = v2v[v2];
#ifdef _DEBUG2
    std::cout<<" v1 = v2v[v1] ... "<<std::endl;
    std::cout<<" v1 : "<<v1->getName()<<std::endl;
    std::cout<<" v2 : "<<v2->getName()<<std::endl;
    std::cout<<" v1Opp : "<<v1Opp->getName()<<std::endl;
    std::cout<<" v2Opp : "<<v2Opp->getName()<<std::endl;
#endif

    //M3D CONNECTION UPDATE
    CHECK_NULL_PTR_ERROR(v1);
    CHECK_NULL_PTR_ERROR(v2);
    if (c_copy){
    	v1->add(c_copy);
    	v2->add(c_copy);
    	c_copy->add(v1);
    	c_copy->add(v2);
    	if (surf){
    		surf->add(c_copy);
    		c_copy->add(surf);
    	}
    }
    if (c_opp){
    	v1Opp->add(c_opp);
    	v2Opp->add(c_opp);
    	c_opp->add(v1Opp) ;
    	c_opp->add(v2Opp);
    	if (surf){
    		surf->add(c_opp);
    		c_opp->add(surf);
    	}
   }

    // on récupère les courbes "latérales"
    Curve *c11 = 0;
    if (m_v2v_inv[v1])
    	c11 = v2c[m_v2v_inv[v1]];
    Curve *c22 = 0;
    if (m_v2v_inv[v2])
    	c22 = v2c[m_v2v_inv[v2]];

    if (c11 && surf){
    	surf->add(c11);
    	c11->add(surf);
    }
    if (c22 && surf){
    	surf->add(c22);
    	c22->add(surf);
    }
}
/*----------------------------------------------------------------------------*/
void GeomExtrudeImplementation::findOCCEdgeAssociation(TopoDS_Edge& ref,
        TopoDS_Vertex& v1, TopoDS_Vertex& v2,TopoDS_Vertex& v3,TopoDS_Vertex& v4,
        TopoDS_Edge& e12, TopoDS_Edge& e23,TopoDS_Edge& e34,TopoDS_Edge& e41)
{
    TopTools_IndexedMapOfShape map_result_vertex;
    TopExp::MapShapes(ref,TopAbs_VERTEX, map_result_vertex);
    if(map_result_vertex.Extent()!=2)
        throw TkUtil::Exception(TkUtil::UTF8String ("Une configuration imprévue a été rencontrée lors de l'extrusion d'une courbe : 1 arête n'a pas deux extrémités (1)", TkUtil::Charset::UTF_8));

    TopoDS_Vertex v1_e1occ =  TopoDS::Vertex(map_result_vertex(1));
    TopoDS_Vertex v2_e1occ =  TopoDS::Vertex(map_result_vertex(2));

    if(((OCCGeomRepresentation::areEquals(v1_e1occ, TopoDS::Vertex(v1))==true) &&
            (OCCGeomRepresentation::areEquals(v2_e1occ, TopoDS::Vertex(v2))==true)  )||
            ((OCCGeomRepresentation::areEquals(v1_e1occ, TopoDS::Vertex(v2))==true) &&
                    (OCCGeomRepresentation::areEquals(v2_e1occ, TopoDS::Vertex(v1))==true)  ))
        e12 = ref;
    else if(((OCCGeomRepresentation::areEquals(v1_e1occ, TopoDS::Vertex(v2))==true) &&
            (OCCGeomRepresentation::areEquals(v2_e1occ, TopoDS::Vertex(v3))==true)  )||
            ((OCCGeomRepresentation::areEquals(v1_e1occ, TopoDS::Vertex(v3))==true) &&
                    (OCCGeomRepresentation::areEquals(v2_e1occ, TopoDS::Vertex(v2))==true)  ))
        e23= ref;
    else if(((OCCGeomRepresentation::areEquals(v1_e1occ, TopoDS::Vertex(v3))==true) &&
            (OCCGeomRepresentation::areEquals(v2_e1occ, TopoDS::Vertex(v4))==true)  )||
            ((OCCGeomRepresentation::areEquals(v1_e1occ, TopoDS::Vertex(v4))==true) &&
                    (OCCGeomRepresentation::areEquals(v2_e1occ, TopoDS::Vertex(v3))==true)  ))
        e34= ref;
    else if(((OCCGeomRepresentation::areEquals(v1_e1occ, TopoDS::Vertex(v1))==true) &&
            (OCCGeomRepresentation::areEquals(v2_e1occ, TopoDS::Vertex(v4))==true)  )||
            ((OCCGeomRepresentation::areEquals(v1_e1occ, TopoDS::Vertex(v4))==true) &&
                    (OCCGeomRepresentation::areEquals(v2_e1occ, TopoDS::Vertex(v1))==true)  ))
        e41= ref;
    else
        throw TkUtil::Exception(TkUtil::UTF8String ("Une configuration imprévue a été rencontrée lors de l'extrusion d'une courbe : impossible de trouver une association arête-courbe", TkUtil::Charset::UTF_8));


}
/*----------------------------------------------------------------------------*/
void GeomExtrudeImplementation::findOCCEdgeAssociation(TopoDS_Edge& ref,
        TopoDS_Vertex& v1, TopoDS_Vertex& v2,TopoDS_Vertex& v3,
        TopoDS_Edge& e12, TopoDS_Edge& e23,TopoDS_Edge& e31)
{
    TopTools_IndexedMapOfShape map_result_vertex;
    TopExp::MapShapes(ref,TopAbs_VERTEX, map_result_vertex);
    if(map_result_vertex.Extent()!=2)
        throw TkUtil::Exception(TkUtil::UTF8String ("Une configuration imprévue a été rencontrée lors de l'extrusion d'une courbe : 1 arête n'a pas deux extrémités (2)", TkUtil::Charset::UTF_8));

    TopoDS_Vertex v1_e1occ =  TopoDS::Vertex(map_result_vertex(1));
    TopoDS_Vertex v2_e1occ =  TopoDS::Vertex(map_result_vertex(2));

    if(((BRepTools::Compare(v1_e1occ, TopoDS::Vertex(v1))==true) &&
            (BRepTools::Compare(v2_e1occ, TopoDS::Vertex(v2))==true)  )||
            ((BRepTools::Compare(v1_e1occ, TopoDS::Vertex(v2))==true) &&
                    (BRepTools::Compare(v2_e1occ, TopoDS::Vertex(v1))==true)  ))
        e12 = ref;
    else if(((BRepTools::Compare(v1_e1occ, TopoDS::Vertex(v2))==true) &&
            (BRepTools::Compare(v2_e1occ, TopoDS::Vertex(v3))==true)  )||
            ((BRepTools::Compare(v1_e1occ, TopoDS::Vertex(v3))==true) &&
                    (BRepTools::Compare(v2_e1occ, TopoDS::Vertex(v2))==true)  ))
        e23= ref;
    else if(((BRepTools::Compare(v1_e1occ, TopoDS::Vertex(v3))==true) &&
            (BRepTools::Compare(v2_e1occ, TopoDS::Vertex(v1))==true)  )||
            ((BRepTools::Compare(v1_e1occ, TopoDS::Vertex(v1))==true) &&
                    (BRepTools::Compare(v2_e1occ, TopoDS::Vertex(v3))==true)  ))
        e31= ref;
    else
        throw TkUtil::Exception(TkUtil::UTF8String ("Une configuration imprévue a été rencontrée lors de l'extrusion d'une courbe : impossible de trouver une association arête-courbe (2)", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
bool GeomExtrudeImplementation::findVertices(TopoDS_Edge& ref, TopoDS_Vertex& v1, TopoDS_Vertex& v2)
{
    TopTools_IndexedMapOfShape map_result_vertex;
    TopExp::MapShapes(ref,TopAbs_VERTEX, map_result_vertex);
    if(map_result_vertex.Extent()!=2)
        return false;

    TopoDS_Vertex v1_ref =  TopoDS::Vertex(map_result_vertex(1));
    TopoDS_Vertex v2_ref =  TopoDS::Vertex(map_result_vertex(2));

    return ((BRepTools::Compare(v1_ref, v1) && BRepTools::Compare(v2_ref, v2))
    		|| (BRepTools::Compare(v1_ref, v2) && BRepTools::Compare(v2_ref, v1)));
}
/*----------------------------------------------------------------------------*/
void GeomExtrudeImplementation::makeExtrude(Surface* surf,
        std::vector<GeomEntity*>& res,
        std::map<Geom::Vertex* ,Geom::Vertex*> & v2v,
        std::map<Geom::Vertex* ,Geom::Vertex*> & v2vOpp,
        std::map<Geom::Vertex* ,Geom::Curve*>  & v2c,
        std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
        std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
        std::map<Geom::Curve*  ,Geom::Surface*>  & c2s,
        std::map<Geom::Surface*,Geom::Surface*>& s2s,
        std::map<Geom::Surface*,Geom::Surface*>& s2sOpp,
        std::map<Geom::Surface*,Geom::Volume*> & s2v)
{
    TopoDS_Shape s;
    getOCCShape(surf, s);
    gp_Vec Vx(m_vector.getX(), m_vector.getY(), m_vector.getZ());
    BRepPrimAPI_MakePrism mkR(s, Vx);

    if (!mkR.IsDone () || mkR.Shape().IsNull())
    {
        std::cerr<<"GeomExtrudeImplementation::makeExtrude pour "<<surf->getName()<<std::endl;
        std::cerr<<" mkR.IsDone () : "<<mkR.IsDone ()<<std::endl;
        std::cerr<<" mkR.Shape().IsNull() : "<<mkR.Shape().IsNull()<<std::endl;
        throw TkUtil::Exception(TkUtil::UTF8String ("OCC n'a pas pu effectuer une extrusion d'une surface", TkUtil::Charset::UTF_8));
    }
    TopoDS_Shape sh = mkR.Shape();


    if (sh.ShapeType()==TopAbs_SOLID){
        TopoDS_Solid solid = TopoDS::Solid(sh);
        Volume* vol=EntityFactory(m_context).newOCCVolume(solid);
        m_newEntities.push_back(vol);
        m_newVolumes.push_back(vol);
        res.push_back(vol);
        s2v[surf] = vol;

        // on recupere toutes les courbes incidentes à la face de départ
        std::vector<Curve*> curves_of_surf;
        surf->get(curves_of_surf);
        // toutes les surfaces incidentes au volume créé sauf surf et la surface opposée
        // ont été créés à partir des courbes de curves_of_surf
        std::vector<Surface*> lateral_surfs;
        std::vector<TopoDS_Shape> lateral_surfs_occ;
        for(unsigned int i=0;i<curves_of_surf.size();i++){
            Surface* surf_i =c2s[curves_of_surf[i]];
            //la surface peut être =0 si la courbe était sur l'axe de extrusion
            if(surf_i==0)
                continue;
//           std::cout<<"\t surf "<<surf_i->getName()<<std::endl;
            lateral_surfs.push_back(surf_i);
            std::vector<TopoDS_Shape> surf_i_occ;
            getOCCShapes(surf_i,surf_i_occ);
            // cas multi shape...
            lateral_surfs_occ.insert(lateral_surfs_occ.end(), surf_i_occ.begin(), surf_i_occ.end());
        }

        // on cherche maintenant les deux surfaces manquantes (dep et arrivée)
        std::vector<TopoDS_Shape> extrem_surf;
        TopTools_IndexedMapOfShape map_result_surf;
        TopExp::MapShapes(solid,TopAbs_FACE, map_result_surf);
        int nbS = map_result_surf.Extent();
//       std::cerr<<"Nb faces ="<<nbS<<std::endl;
//        std::cerr<<"Nb faces laterales ="<<lateral_surfs.size()<<std::endl;
        for(int i_face=1;i_face<=map_result_surf.Extent();i_face++){
//            std::cout<<"----"<<std::endl;
            TopoDS_Shape face_i =  map_result_surf(i_face);
            bool islateral=false;
            for(unsigned int i_lat=0;i_lat<lateral_surfs_occ.size() &&!islateral;i_lat++){
//                std::cout<<"\t compare "<<i_face<<" with "<<i_lat<<std::endl;
                if(OCCGeomRepresentation::areEquals(TopoDS::Face(face_i),
                        TopoDS::Face(lateral_surfs_occ[i_lat])))
                    islateral=true;
            }
            if(islateral==false)// on a la copie ou l'opposée de surf
                extrem_surf.push_back(face_i);
        }
        if(extrem_surf.size()!=2){
            std::cerr<<"GeomExtrudeImplementation::makeExtrude [extrem_surf.size()="<<extrem_surf.size()<<" différent de 2] pour "<<surf->getName()<<std::endl;
            throw TkUtil::Exception(TkUtil::UTF8String ("OCC n'a pas pu effectuer une extrusion d'une surface", TkUtil::Charset::UTF_8));
        }

        TopoDS_Shape s0 = extrem_surf[0];
        TopoDS_Shape s1 = extrem_surf[1];
        TopoDS_Shape s_copy, s_opp;
        if(s0.IsSame(s)){
            s_copy=s0;
            s_opp=s1;
        }
        else if(s1.IsSame(s)){
            s_copy=s1;
            s_opp=s0;
        }
        else
            throw TkUtil::Exception(TkUtil::UTF8String ("Problème d'association de surfaces lors de l'extrusions d'une surface", TkUtil::Charset::UTF_8));

        Surface* surf_copy=EntityFactory(m_context).newOCCSurface(TopoDS::Face(s_copy));
        Surface* surf_opp=EntityFactory(m_context).newOCCSurface(TopoDS::Face(s_opp));
        m_newEntities.push_back(surf_copy);
        m_newEntities.push_back(surf_opp);
        m_newSurfaces.push_back(surf_copy);
        m_newSurfaces.push_back(surf_opp);
        res.push_back(surf_copy);
        res.push_back(surf_opp);
        s2s[surf]    = surf_copy;
        s2sOpp[surf] = surf_opp;

        //MISE A JOUR DES CONNECTIVITES

        // volume <-> surfaces
        vol->add(surf_copy);
        vol->add(surf_opp);
        surf_copy->add(vol);
        surf_opp->add(vol);
        for(unsigned int i=0;i<lateral_surfs.size();i++){
            Surface* si = lateral_surfs[i];
            si->add(vol);
            vol->add(si);
        }

        //surface <-> courbes
        // les surfaces surf_copy et surf_opp sont nouvelles, elles doivent donc
        // etre connectées à des courbes
        for(unsigned int i=0;i<curves_of_surf.size();i++){
            Curve* ci = curves_of_surf[i];
            c2c[ci]->add(surf_copy);
            surf_copy->add(c2c[ci]);
            if(c2cOpp[ci]!=0){
                c2cOpp[ci]->add(surf_opp);
                surf_opp->add(c2cOpp[ci]);
            }
            else{
                c2c[ci]->add(surf_opp);
                surf_opp->add(c2c[ci]);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
