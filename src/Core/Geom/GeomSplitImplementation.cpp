/*----------------------------------------------------------------------------*/
#include "Geom/GeomSplitImplementation.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/Surface.h"
#include "Geom/EntityFactory.h"
#include "Geom/OCCHelper.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/        
GeomSplitImplementation::GeomSplitImplementation(Internal::Context& context)
: m_context(context)
{}
/*----------------------------------------------------------------------------*/        
void GeomSplitImplementation::split(Curve* curve, std::vector<Vertex* >&  vert)
{
    auto occ_edges = curve->getOCCEdges();
    EntityFactory factory(m_context);

	// identification des sommets aux extrémités (ceux vus qu'une unique fois)
	if (occ_edges.size() == 1) {
		/* on va explorer la courbe OCC stockée en attribut et créer les entités de
		* dimension directement inférieure, c'est-à-dire les sommets
		*/
		Vertex* v = 0;
		TopExp_Explorer e;
		for(e.Init(occ_edges[0], TopAbs_VERTEX); e.More(); e.Next())
		{

			TopoDS_Vertex V = TopoDS::Vertex(e.Current());
			// on évite de mettre 2 fois le même sommet [EB]
			bool are_same = false;
			if (v){
				// 1 seule représentation pour le vertex
				TopoDS_Vertex Vprec = TopoDS::Vertex(v->getOCCVertex());
				if (Vprec.IsSame(V))
					are_same = true;
			}

			if (!are_same){
				// création du nouveau sommet
				v = factory.newOCCVertex(V);
				vert.push_back(v);
				// on crée le lien C->V
				curve->add(v);
				// on crée le lien V->C
				v->add(curve);
			}
		}
	} else {
		//std::cout<<"Curve::split avec occ_edges.size() = "<<occ_edges.size()<<std::endl;

		std::vector<TopoDS_Vertex> vtx;
		for (uint i=0; i<occ_edges.size(); i++){
			TopExp_Explorer e;
			for(e.Init(occ_edges[i], TopAbs_VERTEX); e.More(); e.Next()){
				TopoDS_Vertex V = TopoDS::Vertex(e.Current());
				vtx.push_back(V);
			}
		}
		//std::cout<<"vtx.size() = "<<vtx.size()<<std::endl;

		TopoDS_Vertex Vdep;
		TopoDS_Vertex Vfin;

		for (uint i=0; i<vtx.size()-1; i++){
			TopoDS_Vertex V1 = vtx[i];
			for (uint j=i+1; j<vtx.size(); j++){
				TopoDS_Vertex V2 = vtx[j];
				if ((!V1.IsNull()) && (!V2.IsNull()) && OCCHelper::areEquals(V1,V2)){
					 vtx[i].Nullify();
					 vtx[j].Nullify();
				}
			}
		} // end for i<vtx.size()

		for (uint i=0; i<vtx.size(); i++){
			TopoDS_Vertex V1 = vtx[i];
			if (!V1.IsNull()){
				if (Vdep.IsNull())
					Vdep = V1;
				else if (Vfin.IsNull())
					Vfin = V1;
				else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
					messErr << "La courbe " << curve->getName() << " est composée de plusieurs parties et on trouve plus de 2 sommets comme extrémité";
					throw TkUtil::Exception(messErr);
				}

			}
		}

		if (!Vdep.IsNull()){
			// création du nouveau sommet
			Vertex* v = factory.newOCCVertex(Vdep);
			vert.push_back(v);
			// on crée le lien C->V
			curve->add(v);
			// on crée le lien V->C
			v->add(curve);
		}

		if (!Vfin.IsNull()){
			Vertex* v = factory.newOCCVertex(Vfin);
			vert.push_back(v);
			curve->add(v);
			v->add(curve);
		}

	} // end else / if (occ_edges.size() == 1)
}
/*----------------------------------------------------------------------------*/
void GeomSplitImplementation::split(Surface* surf, 
    std::vector<Curve* >& curv,
    std::vector<Vertex* >&  vert)
{
    EntityFactory factory(m_context);
    for (auto rep : surf->getOCCFaces()) {
		/* on va explorer la face OCC stocké en attribut et créer les entités de
		* dimension directement inférieure, c'est-à-dire les courbes
		*/
		TopExp_Explorer e;

		/* on crée les faces */
		std::vector<TopoDS_Shape> OCCCurves;
		std::vector<Curve *>      Mgx3DCurves;

		for(e.Init(rep, TopAbs_EDGE); e.More(); e.Next())
		{

			TopoDS_Edge E = TopoDS::Edge(e.Current());

			Curve* c = factory.newOCCCurve(E);
			curv.push_back(c);

			// correspondance entre shapes OCC et géométries Mgx3D
			OCCCurves.push_back(E);
			Mgx3DCurves.push_back(c);
			// on crée le lien V->F
			surf->add(c);
			// on crée le lien F->V
			c->add(surf);
		}


		// maintenant que les aretes sont créées, on crée les
		// sommets
		TopTools_IndexedDataMapOfShapeListOfShape map;
		TopExp::MapShapesAndAncestors(rep, TopAbs_VERTEX, TopAbs_EDGE, map);
		// on a ainsi tous les sommets dans map et pour chaque sommet, on
		// connait les aretes auxquelles il appartient.

		/* on crée les labels contenants les sommets et pour chaque sommet,
		* on fait pointer une ref à partir des labels ayant les aretes
		* correspondantes */
		TopTools_IndexedMapOfShape map_vertices;
		TopExp::MapShapes(rep,TopAbs_VERTEX, map_vertices);
		TopTools_ListOfShape listEdges;

		for(int i = 1; i <= map_vertices.Extent(); i++)
		{
			TopoDS_Vertex V = TopoDS::Vertex(map_vertices(i));
			// creation du sommet
			Vertex* v = factory.newOCCVertex(V);
			vert.push_back(v);

			/* on récupère les arêtes contenant ce sommet. Mais attention, ce nb
			* d'arêtes est trop important car des doublons existent.
			*/
			listEdges = map.FindFromKey(V);

			TopTools_ListIteratorOfListOfShape it_edges;

			// ce vecteur nous sert à ne pas récupérer 2 fois le même sommet
			std::vector<bool> still_done;
			still_done.resize(Mgx3DCurves.size(),0);

			for(it_edges.Initialize(listEdges);it_edges.More();it_edges.Next()){
				TopoDS_Shape shape =  it_edges.Value();
				Curve *c= 0;
				bool not_find_shape = true;
				for(int i =0; i<OCCCurves.size() && not_find_shape; i++){
					if(shape.IsSame(OCCCurves[i]) && !still_done[i]){
						not_find_shape = false;
						c = Mgx3DCurves[i];
						still_done[i] = true;
					}
				}
				// si on a trouvé la shape et qu'elle n'avait pas déjà été traitée
				if(!not_find_shape){
					// on crée le lien C->V
					c->add(v);
					// on crée le lien V->C
					v->add(c);
				}
			}
		}
	}
}
/*----------------------------------------------------------------------------*/
void GeomSplitImplementation::split(Volume* vol,
    std::vector<Surface*>& surf,
    std::vector<Curve*  >& curv,
    std::vector<Vertex* >& vert)
{
    EntityFactory factory(m_context);

    /* on va explorer le solide OCC stocké en attribut et créer les entités de
    * dimension directement inférieure, c'est-à-dire les faces
    */
    TopExp_Explorer e;
    //    if (m_shape.ShapeType()!=TopAbs_SOLID  && m_shape.ShapeType()!=TopAbs_SHELL)
    //        throw TkUtil::Exception("Wrong OCC shape type!!!");

    /* on crée les faces */
    std::vector<TopoDS_Shape> OCCFaces;
    std::vector<Surface*>     Mgx3DSurfaces;
    std::vector<TopoDS_Shape> OCCCurves;
    std::vector<Curve *>      Mgx3DCurves;

    TopoDS_Shape occ_shape = vol->getOCCShape();
    for(e.Init(occ_shape, TopAbs_FACE); e.More(); e.Next())
    {

        TopoDS_Face F = TopoDS::Face(e.Current());

        Surface* s = factory.newOCCSurface(F);
        surf.push_back(s);

        // correspondance entre shapes OCC et géométries Mgx3D
        OCCFaces.push_back(F);
        Mgx3DSurfaces.push_back(s);
        // on crée le lien V->F
        vol->add(s);
        // on crée le lien F->V
        s->add(vol);
    }

    // maintenant que les faces sont créées, on crée les arêtes
    TopTools_IndexedDataMapOfShapeListOfShape map;
    TopExp::MapShapesAndAncestors(occ_shape, TopAbs_EDGE, TopAbs_FACE, map);
    // on a ainsi toutes les arêtes dans map et pour chaque arête, on
    // connait les faces auxquelles elle appartient.

    TopTools_IndexedMapOfShape map_edges;
    TopExp::MapShapes(occ_shape,TopAbs_EDGE, map_edges);
    TopTools_ListOfShape listFaces;


    for(int i = 1; i <= map_edges.Extent(); i++)
    {
        TopoDS_Edge E = TopoDS::Edge(map_edges(i));
        BRepCheck_Analyzer anaAna(E, Standard_False);
        GProp_GProps pb;
        BRepGProp::LinearProperties(E,pb);
        double res = pb.Mass();
        if(res<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon){
            TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Une courbe de taille nulle n'a pas ete creee pour l'entité géométrique "<<vol->getName();
            m_context.getLogDispatcher().log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_3));
            continue;
        }

        Curve* c = factory.newOCCCurve(E);
        curv.push_back(c);

        // correspondance entre shapes OCC et géométries Mgx3D
        OCCCurves.push_back(E);
        Mgx3DCurves.push_back(c);

        /* on récupère les faces contenant cette arête et déjà crées donc
        */
        listFaces = map.FindFromKey(E);
        TopTools_ListIteratorOfListOfShape it_faces;
        for(it_faces.Initialize(listFaces);it_faces.More();it_faces.Next()){
            TopoDS_Shape shape =  it_faces.Value();

        Surface *s= 0;
        bool not_find_shape = true;
        for(int i =0; i<OCCFaces.size() && not_find_shape; i++)
            if(shape.IsSame(OCCFaces[i])){
                not_find_shape = false;
                s = Mgx3DSurfaces[i];
            }

        // on crée le lien S->C
        s->add(c);
        // on crée le lien C->S
        c->add(s);
        }
    }

    // maintenant que les faces et les aretes sont créées, on crée les
    // sommets
    TopExp::MapShapesAndAncestors(occ_shape, TopAbs_VERTEX, TopAbs_EDGE, map);
    // on a ainsi tous les sommets dans map et pour chaque sommet, on
    // connait les aretes auxquelles il appartient.

    /* on crée les labels contenants les sommets et pour chaque sommet,
    * on fait pointer une ref à partir des labels ayant les aretes
    * correspondantes */
    TopTools_IndexedMapOfShape map_vertices;
    TopExp::MapShapes(occ_shape,TopAbs_VERTEX, map_vertices);
    TopTools_ListOfShape listEdges;

    for(int i = 1; i <= map_vertices.Extent(); i++)
    {
        TopoDS_Vertex V = TopoDS::Vertex(map_vertices(i));
        // creation du sommet
        Vertex* v = factory.newOCCVertex(V);
        vert.push_back(v);

        /* on récupère les arêtes contenant ce sommet. Mais attention, ce nb
        * d'arêtes est trop important car des doublons existent.
        */
        listEdges = map.FindFromKey(V);

        TopTools_ListIteratorOfListOfShape it_edges;

        // ce vecteur nous sert à ne pas récupérer 2 fois le même sommet
        std::vector<bool> still_done;
        still_done.resize(Mgx3DCurves.size(),0);

        for(it_edges.Initialize(listEdges);it_edges.More();it_edges.Next()){
        TopoDS_Shape shape =  it_edges.Value();
        Curve *c= 0;
        bool not_find_shape = true;
        for(int i =0; i<OCCCurves.size() && not_find_shape; i++){
            if(shape.IsSame(OCCCurves[i]) && !still_done[i]){
                not_find_shape = false;
                c = Mgx3DCurves[i];
                still_done[i] = true;
            }
        }
        // si on a trouvé la shape et qu'elle n'avait pas déjà été traitée
        if(!not_find_shape){
            // on crée le lien C->V
            c->add(v);
            // on crée le lien V->C
            v->add(c);
        }

        }
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
