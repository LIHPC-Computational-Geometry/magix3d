/*----------------------------------------------------------------------------*/
/*
 * GeomImport.cpp
 *
 *  Created on: 30 oct. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Geom/GeomImport.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/EntityFactory.h"
#include "Geom/OCCApplication.h"

#include "Group/GroupManager.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"

#include "Internal/InfoCommand.h"

#include "GMDS/Utils/Timer.h"
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <BRepAdaptor_Curve.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomImport::
GeomImport(Internal::Context& c, Internal::InfoCommand* icmd,
        const std::string& n, const bool onlySolidsAndFaces)
: GeomModificationBaseClass(c /*on ne garde pas les entités initiales*/),
  m_icmd(icmd), m_filename(n), m_onlySolidsAndFaces(onlySolidsAndFaces),
  m_testVolumicProperties(true),
  m_splitCompondCurves(false)
{
	// récupération du nom du fichier sans chemin ni extension
	std::string suffix = m_filename;
	int suffix_start = m_filename.find_last_of(".");
	suffix.erase(0,suffix_start+1);

	int path_end = m_filename.find_last_of("/");
	m_shortfilename = std::string(m_filename, path_end+1, suffix_start-path_end-1);

#ifdef _DEBUG2
	std::cerr<<"m_shortfilename : "<<m_shortfilename<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
GeomImport::~GeomImport()
{}
/*----------------------------------------------------------------------------*/
void GeomImport::perform(std::vector<GeomEntity*>& res)
{
    //Recuperation de la liste des shapes traduites depuis le fichier
    readFile();

    // nombre de solides non fermés
    uint nb_solide_non_ferme = 0;

    unsigned long id_solid =0;
    unsigned long id_shell =0;
    unsigned long id_face  =0;
    unsigned long id_wire  =0;
    unsigned long id_edge  =0;
    unsigned long id_vertex=0;
    for(unsigned int i=0; i<m_importedShapes.size();i++)
    {
        TopoDS_Shape current_shape = m_importedShapes[i];

        TopExp_Explorer ex;
        ex.Init(current_shape, TopAbs_SOLID);
        for (; ex.More(); ex.Next())
        {
            TopoDS_Solid aSolid = TopoDS::Solid(ex.Current());
            //on test les volumes fermés et ouverts
            Standard_Boolean onlyClosed = Standard_True;
            Standard_Boolean isUseSpan = Standard_True;
            Standard_Real aDefaultTol = 1.e-7;
            Standard_Boolean CGFlag = Standard_False;
            Standard_Boolean IFlag = Standard_False;
            GProp_GProps pb;
            if (m_testVolumicProperties)
            	Standard_Real local_eps =BRepGProp::VolumePropertiesGK (aSolid,
            			pb,
						aDefaultTol,
						onlyClosed,
						isUseSpan,
						CGFlag,
						IFlag);

            if(m_testVolumicProperties && pb.Mass()==0){
                //ce n'est pas un volume fermé
            	nb_solide_non_ferme++;
                TopExp_Explorer ex2;

                for (ex2.Init(current_shape, TopAbs_FACE); ex2.More(); ex2.Next()) {
                    TopoDS_Face aFace = TopoDS::Face(ex2.Current());
					TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
                    name << m_shortfilename << "-face-" << TkUtil::setw (2) << id_face++;
                    add(aFace,name);
                }
                if(!m_onlySolidsAndFaces){
                    for (ex2.Init(current_shape, TopAbs_WIRE, TopAbs_FACE); ex2.More(); ex2.Next()) {
                        TopoDS_Wire aWire= TopoDS::Wire(ex2.Current());
						TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
                        name << m_shortfilename << "-wire-" << TkUtil::setw (2) << id_wire++;
                        add(aWire,name);
                    }
                    for (ex2.Init(current_shape, TopAbs_EDGE, TopAbs_WIRE); ex2.More(); ex2.Next()) {
                        TopoDS_Edge aEdge = TopoDS::Edge(ex2.Current());
						TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
                        name << m_shortfilename << "-edge-" << TkUtil::setw (2) << id_edge++;
                        add(aEdge,name);
                    }
                    for (ex2.Init(current_shape, TopAbs_VERTEX, TopAbs_EDGE); ex2.More(); ex2.Next()) {
                        TopoDS_Vertex aVertex = TopoDS::Vertex(ex2.Current());
						TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
                        name << m_shortfilename << "-vertex-" << TkUtil::setw (2) << id_vertex++;
                        add(aVertex,name);
                    }
                }
            } // if(pb.Mass()==0)
            else {
				TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
                name << m_shortfilename << "-solid-" << TkUtil::setw (2) << id_solid++;
                add(aSolid,name);
            }
        } // end for (; ex.More(); ex.Next()) avec ex.Init(current_shape, TopAbs_SOLID)
        // load all non-solids now
        ex.Init(current_shape, TopAbs_SHELL, TopAbs_SOLID);
        for (; ex.More(); ex.Next())
        {
            // get the shape
            TopoDS_Shell aShell = TopoDS::Shell(ex.Current());
			TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
            name << m_shortfilename << "-shell-" << TkUtil::setw (2) << id_shell++;
            add(aShell,name);
        }
        ex.Init(current_shape, TopAbs_FACE, TopAbs_SHELL);
        for (; ex.More(); ex.Next()) {

            TopoDS_Shape sh =ex.Current();
            TopoDS_Face f = TopoDS::Face(sh);
			TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
            name << m_shortfilename << "-face-" << TkUtil::setw (2) << id_face++;
            add(f,name);
        }
        ex.Init(current_shape, TopAbs_WIRE, TopAbs_FACE);
        for (; ex.More(); ex.Next()) {

            TopoDS_Shape sh =ex.Current();
            TopoDS_Wire aWire = TopoDS::Wire(sh);
			TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
            name << m_shortfilename << "-wire-" << TkUtil::setw (2) << id_wire++;
            add(aWire,name);
        }

        if(!m_onlySolidsAndFaces)
        {
            ex.Init(current_shape, TopAbs_WIRE, TopAbs_FACE);
            for (; ex.More(); ex.Next())
            {
                TopoDS_Shape sh =ex.Current();
                //            sh = OCCGeomRepresentation::cleanShape(sh);
                TopoDS_Wire aWire= TopoDS::Wire(sh);
				TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
                name << m_shortfilename << "-wire-" << TkUtil::setw (2) << id_wire++;
                add(aWire,name);
            }
            ex.Init(current_shape, TopAbs_EDGE, TopAbs_WIRE);
            for (; ex.More(); ex.Next()) {

                TopoDS_Shape sh =ex.Current();
                //            sh = OCCGeomRepresentation::cleanShape(sh);
                TopoDS_Edge e= TopoDS::Edge(sh);
				TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
                name << m_shortfilename << "-edge-" << TkUtil::setw (2) << id_edge++;
                add(e,name);
            }
            ex.Init(current_shape, TopAbs_VERTEX, TopAbs_EDGE);
            for (; ex.More(); ex.Next()) {
                TopoDS_Shape sh =ex.Current();
                //            sh = OCCGeomRepresentation::cleanShape(sh);
                TopoDS_Vertex v = TopoDS::Vertex(sh);
				TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
                name << m_shortfilename << "-vertex-" << TkUtil::setw (2) << id_vertex++;
                add(v,name);
            }
        }
    }
    if (m_testVolumicProperties){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	if (nb_solide_non_ferme == 0)
    		message << "Importation sans aucun volume non fermé";
    	else
    		message << "Importation avec au moins 1 volume non fermé ("<<(short)nb_solide_non_ferme<<")";
    	getContext().getLogDispatcher().log (TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
    }
}
/*----------------------------------------------------------------------------*/
void GeomImport::add(TopoDS_Shape& AShape, const std::string& AName)
{
#ifdef _DEBUG2
    std::cout<<"GeomImport::add("<<AName<<")"<<std::endl;
#endif

    std::vector<Surface*> surfs;
    std::vector<Curve*  > curvs;
    std::vector<Vertex* > verts;

    switch(AShape.ShapeType()){
    case TopAbs_SOLID:
    {
        Volume* vol=EntityFactory(m_context).newOCCVolume(TopoDS::Solid(AShape));
    	addToGroup(vol, AName);
        vol->split(surfs,curvs,verts);
        store(vol);
        for(unsigned int i=0;i<surfs.size();i++){
        	store(surfs[i]);
        	addToGroup(surfs[i],"");
        }
        for(unsigned int i=0;i<curvs.size();i++){
        	store(curvs[i]);
        	addToGroup(curvs[i],"");
        }
        for(unsigned int i=0;i<verts.size();i++){
        	store(verts[i]);
        	addToGroup(verts[i],"");
        }
    }
    break;
    case TopAbs_FACE:
    {
        Surface* surf=EntityFactory(m_context).newOCCSurface(TopoDS::Face(AShape));
        addToGroup(surf, AName);
        surf->split(curvs,verts);
        store(surf);
        for(unsigned int i=0;i<curvs.size();i++){
        	store(curvs[i]);
        	addToGroup(curvs[i],"");
        }
        for(unsigned int i=0;i<verts.size();i++){
        	store(verts[i]);
        	addToGroup(verts[i],"");
        }
    }
    break;
    case TopAbs_WIRE:
    {
    	if (m_splitCompondCurves){
            // version avec courbes multiples
            TopoDS_Wire aWire= TopoDS::Wire(AShape);
            TopExp_Explorer ex;
            for (ex.Init(aWire,TopAbs_EDGE); ex.More(); ex.Next())
            {
                TopoDS_Edge aEdge = TopoDS::Edge(ex.Current());
                Curve* curve=EntityFactory(m_context).newOCCCurve(TopoDS::Edge(aEdge));
                curvs.push_back(curve);
                TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
                name << AName<< "-edge";
                addToGroup(curve, AName);
                store(curve);
            }
            splitManyCurves(curvs, verts);
            for(unsigned int i=0;i<verts.size();i++){
            	store(verts[i]);
            	addToGroup(verts[i],"");
            }
    	}
    	else {
    		Curve* curve=EntityFactory(m_context).newOCCCurve(TopoDS::Wire(AShape));
    		addToGroup(curve, AName);
    		curve->split(verts);
    		store(curve);
    		for(unsigned int i=0;i<verts.size();i++)
    			store(verts[i]);
    	}

//        TopoDS_Wire aWire= TopoDS::Wire(AShape);
//        TopExp_Explorer ex;
//        std::vector<TopoDS_Edge> v_ds_edge;
//        for (ex.Init(aWire,TopAbs_EDGE); ex.More(); ex.Next()){
//        	v_ds_edge.push_back(TopoDS::Edge(ex.Current()));
//        }
//        Utils::Math::Point pt1, pt2;
//        {
//        	BRepAdaptor_Curve brepCurve(v_ds_edge.front());
//        	gp_Pnt res;
//        	res = brepCurve.Value(brepCurve.FirstParameter());
//        	pt1.setXYZ(res.X(), res.Y(), res.Z());
//        }
//        {
//        	BRepAdaptor_Curve brepCurve(v_ds_edge.back());
//        	gp_Pnt res;
//        	res = brepCurve.Value(brepCurve.LastParameter());
//        	pt2.setXYZ(res.X(), res.Y(), res.Z());
//        }
//
//        Curve* curve=EntityFactory(m_context).newOCCCompositeCurve(v_ds_edge, pt1, pt2);
//        addToGroup(curve, AName);
//        curve->split(verts);
//        store(curve);
//        for(unsigned int i=0;i<verts.size();i++)
//        	store(verts[i]);

    }
    break;
    case TopAbs_EDGE:
    {
        Curve* curve=EntityFactory(m_context).newOCCCurve(TopoDS::Edge(AShape));
        addToGroup(curve, AName);
        curve->split(verts);
        store(curve);
        for(unsigned int i=0;i<verts.size();i++)
        	store(verts[i]);
    }
    break;
    case TopAbs_VERTEX:
    {
        Vertex* vertex=EntityFactory(m_context).newOCCVertex(TopoDS::Vertex(AShape));
        addToGroup(vertex, AName);
        store(vertex);
     }
    break;
    case TopAbs_COMPOUND:
        std::cout<<"Les entités COMPOUND ne sont pas gérées\n";
        break;
    case TopAbs_COMPSOLID:
        std::cout<<"Les entités COMPSOLID ne sont pas gérées\n";
        break;
    case TopAbs_SHELL:
    {
        Volume* vol=EntityFactory(m_context).newOCCVolume(TopoDS::Shell(AShape));
    	addToGroup(vol, AName);
        vol->split(surfs,curvs,verts);
        store(vol);
        for(unsigned int i=0;i<surfs.size();i++){
        	store(surfs[i]);
        	addToGroup(surfs[i],"");
        }
        for(unsigned int i=0;i<curvs.size();i++){
        	store(curvs[i]);
        	addToGroup(curvs[i],"");
        }
        for(unsigned int i=0;i<verts.size();i++){
        	store(verts[i]);
        	addToGroup(verts[i],"");
        }
    }
        break;
    }
}
/*----------------------------------------------------------------------------*/
void GeomImport::
addToGroup(Volume* v, const std::string& AName)
{
    Group::Group3D* group = getContext().getLocalGroupManager().getNewGroup3D(AName, m_icmd);
    v->add(group);
    group->add(v);
    m_icmd->addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void GeomImport::
addToGroup(Surface* s, const std::string& AName)
{
    Group::Group2D* group = getContext().getLocalGroupManager().getNewGroup2D(AName, m_icmd);
    s->add(group);
    group->add(s);
    m_icmd->addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void GeomImport::
addToGroup(Curve* c, const std::string& AName)
{
    Group::Group1D* group = getContext().getLocalGroupManager().getNewGroup1D(AName, m_icmd);
    c->add(group);
    group->add(c);
    m_icmd->addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void GeomImport::
addToGroup(Vertex* v, const std::string& AName)
{
    Group::Group0D* group = getContext().getLocalGroupManager().getNewGroup0D(AName, m_icmd);
    v->add(group);
    group->add(v);
    m_icmd->addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void GeomImport::
store(Volume* v)
{
	m_newEntities.push_back(v);
	m_newVolumes.push_back(v);
	m_icmd->addGeomInfoEntity (v, Internal::InfoCommand::CREATED);
	getContext().getLocalGeomManager().addEntity(v);
}
/*----------------------------------------------------------------------------*/
void GeomImport::
store(Surface* s)
{
	m_newEntities.push_back(s);
	m_newSurfaces.push_back(s);
	m_icmd->addGeomInfoEntity (s, Internal::InfoCommand::CREATED);
	getContext().getLocalGeomManager().addEntity(s);
}
/*----------------------------------------------------------------------------*/
void GeomImport::
store(Curve* c)
{
	m_newEntities.push_back(c);
	m_newCurves.push_back(c);
	m_icmd->addGeomInfoEntity (c, Internal::InfoCommand::CREATED);
	getContext().getLocalGeomManager().addEntity(c);
}
/*----------------------------------------------------------------------------*/
void GeomImport::
store(Vertex* v)
{
	m_newEntities.push_back(v);
	m_newVertices.push_back(v);
	m_icmd->addGeomInfoEntity (v, Internal::InfoCommand::CREATED);
	getContext().getLocalGeomManager().addEntity(v);
}
/*----------------------------------------------------------------------------*/
void GeomImport::
splitManyCurves(std::vector<Curve*>& curvs, std::vector<Vertex*>& verts)
{
	std::vector<TopoDS_Vertex> all_vtx;

	for (uint i=0; i<curvs.size(); i++){
		GeomRepresentation* rep = curvs[i]->getComputationalProperty();
		std::vector<TopoDS_Vertex> vtx;
		OCCGeomRepresentation* occ_rep = dynamic_cast<OCCGeomRepresentation*>(rep);
		CHECK_NULL_PTR_ERROR(occ_rep);
		TopExp_Explorer e;
		for(e.Init(occ_rep->getShape(), TopAbs_VERTEX); e.More(); e.Next()){
			TopoDS_Vertex V = TopoDS::Vertex(e.Current());
			vtx.push_back(V);
		}

		if (vtx.size()!=2)
			throw TkUtil::Exception("La courbe (composée initialement) en cours de création n'a pas uniquement 2 sommets");

		if (i==0){
			// on ajoute les 2 sommets
			for (uint j=0; j<vtx.size(); j++){
				all_vtx.push_back(vtx[j]);
				// création du nouveau sommet
				Vertex* v = EntityFactory(getContext()).newOCCVertex(vtx[j]);
				verts.push_back(v);
				// on crée le lien C->V
				curvs[i]->add(v);
				// on crée le lien V->C
				v->add(curvs[i]);
			}
		}
		else if (i==1){
			// il faut trouver le sommet commun avec le premier et permuter l'ordre
			// des 2 premiers si nécessaire
			 bool perm=false;
			 if (OCCGeomRepresentation::areEquals(all_vtx[0],vtx[0]) || OCCGeomRepresentation::areEquals(all_vtx[0],vtx[1])){
				 TopoDS_Vertex V = all_vtx[0];
				 all_vtx[0] = all_vtx[1];
				 all_vtx[1] = V;
				 Vertex* v = verts[0];
				 verts[0] = verts[1];
				 verts[1] = v;
			 }
		}

		if (i>0){
			// relation avec la courbe prédente
			curvs[i]->add(verts.back());
			verts.back()->add(curvs[i]);

			uint j=2;
			if (OCCGeomRepresentation::areEquals(all_vtx.back(),vtx[0]))
				j=1;
			if (OCCGeomRepresentation::areEquals(all_vtx.back(),vtx[1]))
				j=0;
			if (j==2)
				throw TkUtil::Exception("La courbe (composée initialement) n'a pas de sommets communs entre 2 composantes");

			bool add_vtx = true;
			if (i==curvs.size()-1){
				// cas où cela boucle
				if (OCCGeomRepresentation::areEquals(all_vtx.front(),vtx[j]))
					add_vtx = false;
			}

			if (add_vtx){
				// cas d'un nouveau sommet
				all_vtx.push_back(vtx[j]);
				Vertex* v = EntityFactory(getContext()).newOCCVertex(vtx[j]);
				verts.push_back(v);
				curvs[i]->add(v);
				v->add(curvs[i]);
			}
			else {
				// pour le cas où cela boucle
				curvs[i]->add(verts.front());
				verts.front()->add(curvs[i]);
			}
		} // end if (i>0)

	} //  end for i

}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
