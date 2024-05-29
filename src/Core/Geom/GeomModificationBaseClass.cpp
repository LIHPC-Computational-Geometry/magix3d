/*----------------------------------------------------------------------------*/
/*
 * GeomModification.cpp
 *
 *  Created on: 21 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include "Utils/MgxNumeric.h"
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/WarningLog.h>
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
#include <TkUtil/MemoryError.h>
#include <set>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS.hxx>
#include <TDF_Label.hxx>
#include <TopoDS.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepFeat_Gluer.hxx>
#include <BRep_Builder.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <gp_Pnt.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ReShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <ShapeFix_Shape.hxx>
#include <BRepBndLib.hxx>
#include<Bnd_Box.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Extrema_ExtPC.hxx>

#include <BRepTools.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Shell.hxx>
#include <ShapeFix_Wireframe.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <NCollection_IncAllocator.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
//#define _DEBUG2
//#define _DEBUG_DIFF
/*----------------------------------------------------------------------------*/
GeomModificationBaseClass::GeomModificationBaseClass(Internal::Context& c)
: m_context(c)
, m_buildEntitiesCmd(0)
{}
/*----------------------------------------------------------------------------*/
GeomModificationBaseClass::~GeomModificationBaseClass()
{
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::init(Geom::CommandGeomCopy* cmd)
{
	// on reporte à plus tard l'initialisation
	m_buildEntitiesCmd = cmd;
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::init(std::vector<GeomEntity*>& es)
{

    // pour chacune des entités passées en argument, on ajoute les entités de
    // dimension inférieure et supérieure ou égale en référence.
    for(unsigned int i=0;i<es.size();i++){
        GeomEntity* esi = es[i];
        m_init_entities.push_back(esi);
        addReference(esi);
        addDownIncidentReference(esi);
    }

    for(unsigned int i=0;i<es.size();i++){
        GeomEntity* esi = es[i];
        addAdjacencyReference(esi);
    }

    // une entité a pu être ajoutée plusieurs fois, on élimine maintenant les
    // occurrences multiples
    for(int i=0;i<4;i++){
        m_ref_entities[i].sort(Utils::Entity::compareEntity);
        m_ref_entities[i].unique();
        m_adj_entities[i].sort(Utils::Entity::compareEntity);
        m_adj_entities[i].unique();
    }


#ifdef _DEBUG2
    std::list<GeomEntity*>::iterator adj3_it =m_adj_entities[3].begin();
    for(;adj3_it!=m_adj_entities[3].end();adj3_it++) {
        std::cout<<"+++ VOL ADJ = "<<(*adj3_it)->getName()<<std::endl;
    }

    adj3_it =m_ref_entities[3].begin();
    for(;adj3_it!=m_ref_entities[3].end();adj3_it++) {
        std::cout<<"+++ VOL REF = "<<(*adj3_it)->getName()<<std::endl;
    }
#endif

}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::initWithAll()
{
	Geom::GeomManager& gm = getContext().getLocalGeomManager();

	std::vector<Volume*> volumes = gm.getVolumesObj();
	for (uint i=0; i<volumes.size(); i++){
		m_init_entities.push_back(volumes[i]);
		m_ref_entities[3].push_back(volumes[i]);
	}

	std::vector<Surface*> surfaces = gm.getSurfacesObj();
	for (uint i=0; i<surfaces.size(); i++){
		m_init_entities.push_back(surfaces[i]);
		m_ref_entities[2].push_back(surfaces[i]);
	}

	std::vector<Curve*> curves = gm.getCurvesObj();
	for (uint i=0; i<curves.size(); i++){
		m_init_entities.push_back(curves[i]);
		m_ref_entities[1].push_back(curves[i]);
	}

	std::vector<Vertex*> vertices = gm.getVerticesObj();
	for (uint i=0; i<vertices.size(); i++){
		m_init_entities.push_back(vertices[i]);
		m_ref_entities[0].push_back(vertices[i]);
	}
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::
addReference(GeomEntity* e)
{
    m_ref_entities[e->getDim()].push_back(e);
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::
addDownIncidentReference(GeomEntity* e)
{
    if(e->getDim()>0)
    {
        std::vector<Vertex*> vertices;
        e->get(vertices);
        for(unsigned int i=0;i<vertices.size();i++)
            addReference(vertices[i]);
    }
    if(e->getDim()>1)
    {
        std::vector<Curve*> curves;
        e->get(curves);
        for(unsigned int i=0;i<curves.size();i++)
            addReference(curves[i]);
    }
    if(e->getDim()>2)
    {
        std::vector<Surface*> surfs;
        e->get(surfs);
        for(unsigned int i=0;i<surfs.size();i++)
            addReference(surfs[i]);
    }
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::
addAdjacencyReference(GeomEntity* e)
{
    if(e->getDim()<=1)
    {
        std::vector<Curve*> curves;
        e->get(curves);
        for(unsigned int i=0;i<curves.size();i++){
            GeomEntity* c= curves[i];
            if(std::find(m_ref_entities[1].begin(),m_ref_entities[1].end(),c)==m_ref_entities[1].end())
                m_adj_entities[c->getDim()].push_back(c);
        }
    }
    if(e->getDim()<=2)
    {
        std::vector<Surface*> surfs;
        e->get(surfs);
        for(unsigned int i=0;i<surfs.size();i++)
        {
            GeomEntity* s= surfs[i];
            if(std::find(m_ref_entities[2].begin(),m_ref_entities[2].end(),s)==m_ref_entities[2].end())
                m_adj_entities[s->getDim()].push_back(s);
        }

    }
    if(e->getDim()<=3)
    {
        std::vector<Volume*> vols;
        e->get(vols);
        for(unsigned int i=0;i<vols.size();i++)
        {
            GeomEntity* v= vols[i];
            if(std::find(m_ref_entities[3].begin(),m_ref_entities[3].end(),v)==m_ref_entities[3].end()){
#ifdef _DEBUG2
                std::cout<<"AJOUTER EN ADJ 3: "<<v->getName()<<std::endl;
#endif
                m_adj_entities[v->getDim()].push_back(v);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::getOCCShape(GeomEntity* e, TopoDS_Shape& sh){

    GeomRepresentation* rep = e->getComputationalProperty();

    OCCGeomRepresentation* occ_rep = dynamic_cast<OCCGeomRepresentation*>(rep);

    if(occ_rep==0)
        throw TkUtil::Exception (TkUtil::UTF8String ("impossible de modifier des entités non représentées à l'aide d'OCC", TkUtil::Charset::UTF_8));

    sh = occ_rep->getShape();
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::getOCCShapes(GeomEntity* ge, std::vector<TopoDS_Shape>& topoS)
{
    // cas d'une entité géométrique composée de plusieurs shapes
	std::vector<GeomRepresentation*> ppties = ge->getComputationalProperties();

	// les différentes occ_shape de la composition
    for (uint i=0; i<ppties.size(); i++){
    	OCCGeomRepresentation* occ_rep = dynamic_cast<OCCGeomRepresentation*>(ppties[i]);
    	CHECK_NULL_PTR_ERROR(occ_rep);
    	topoS.push_back(occ_rep->getShape());
    }
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& GeomModificationBaseClass::
getNewEntities()
{
    return m_newEntities;
}
/*----------------------------------------------------------------------------*/
std::list<GeomEntity*>& GeomModificationBaseClass::
getRefEntities(const int dim)
{
    return m_ref_entities[dim];
}
/*----------------------------------------------------------------------------*/
std::list<GeomEntity*>& GeomModificationBaseClass::
getAdjEntities(const int dim)
{
    return m_adj_entities[dim];
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*> GeomModificationBaseClass::
getKeepedEntities(){

    std::vector<GeomEntity*> e;

    e.insert(e.end(),m_toKeepVolumes.begin(),m_toKeepVolumes.end());
    e.insert(e.end(),m_toKeepSurfaces.begin(),m_toKeepSurfaces.end());
    e.insert(e.end(),m_toKeepCurves.begin(),m_toKeepCurves.end());
    e.insert(e.end(),m_toKeepVertices.begin(),m_toKeepVertices.end());

    return e;
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& GeomModificationBaseClass::
getRemovedEntities()
{
    return m_removedEntities;
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& GeomModificationBaseClass::
getMovedEntities()
{
    return m_movedEntities;
}
/*----------------------------------------------------------------------------*/
std::map<GeomEntity*,std::vector<GeomEntity*> >&  GeomModificationBaseClass::
getReplacedEntities()
{
    return m_replacedEntities;
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::clean(const bool deleteAloneVertices)
{
#ifdef _DEBUG2
	std::cout<<"Clean, "<<(deleteAloneVertices?"avec":"sans")<<" destruction des sommets seuls"<<std::endl;
#endif
   //on parcourt les entites de references, et si elles ne sont pas dans keep
    //on les place dans remove
    //=======================================================================
    // TRAITEMENT DES SOMMETS
    //=======================================================================
    std::list<GeomEntity*> refList = m_ref_entities[0];
    std::list<GeomEntity*>::iterator ite;
    for(ite=refList.begin();ite!=refList.end();ite++){
        GeomEntity* e = *ite;
        bool is_found = false;
        for(unsigned int j=0;j<m_toKeepVertices.size()&&!is_found;j++){
            if(e->getUniqueId()==m_toKeepVertices[j]->getUniqueId())
                is_found=true;
        }
        //l'entite peut toujours etre necessaire a une entite adjacente
        if(!is_found){
#ifdef _DEBUG2
            std::cout<<"Clean: on supprime "<<e->getName()<<std::endl;
#endif
            m_removedEntities.push_back(e);
        }
    }
    //=======================================================================
    // TRAITEMENT DES COURBES
    //=======================================================================
    refList = m_ref_entities[1];
    for(ite=refList.begin();ite!=refList.end();ite++){
        GeomEntity* e = *ite;
        bool is_found = false;
        for(unsigned int j=0;j<m_toKeepCurves.size()&&!is_found;j++){
            if(e->getUniqueId()==m_toKeepCurves[j]->getUniqueId())
                is_found=true;
        }
        if(!is_found){
#ifdef _DEBUG2
            std::cout<<"Clean: on supprime "<<e->getName()<<std::endl;
#endif
            m_removedEntities.push_back(e);
        }
    }
    //=======================================================================
    // TRAITEMENT DES SURFACES
    //=======================================================================
    refList = m_ref_entities[2];
    for(ite=refList.begin();ite!=refList.end();ite++){
        GeomEntity* e = *ite;
        bool is_found = false;
        for(unsigned int j=0;j<m_toKeepSurfaces.size()&&!is_found;j++){
            if(e->getUniqueId()==m_toKeepSurfaces[j]->getUniqueId())
                is_found=true;
        }
        if(!is_found){
#ifdef _DEBUG2
            std::cout<<"Clean: on supprime "<<e->getName()<<std::endl;
#endif
            m_removedEntities.push_back(e);
        }
    }
    //=======================================================================
    // TRAITEMENT DES VOLUMES
    //=======================================================================
    refList = m_ref_entities[3];
    for(ite=refList.begin();ite!=refList.end();ite++){
        GeomEntity* e = *ite;
        bool is_found = false;
        for(unsigned int j=0;j<m_toKeepVolumes.size()&&!is_found;j++){
            if(e->getUniqueId()==m_toKeepVolumes[j]->getUniqueId())
                is_found=true;
        }
        if(!is_found){
#ifdef _DEBUG2
            std::cout<<"Clean: on supprime "<<e->getName()<<std::endl;
#endif
            m_removedEntities.push_back(e);
        }
    }

    //=======================================================================
    // TRAITEMENT DES ENTITES CREES A TORT
    //=======================================================================
    short invalid[4]={0,0,0,0};

    if (deleteAloneVertices)
    	for(unsigned int i=0;i<m_newVertices.size();){
    		Vertex* e = m_newVertices[i];
    		//Si l'entité n'est connectée à rien, c'est quelle doit disparaitre
    		std::vector<Curve*> ec;
    		e->get(ec);
    		if(ec.empty()){
    			delete m_newVertices[i]; // [EB] si on ne détruit pas ici, je ne sais pas où ce sera fait
    			Vertex* laste = m_newVertices[m_newVertices.size()-1];
    			m_newVertices[i] = laste;
    			m_newVertices.pop_back();
    			invalid[0]++;
    		}
    		else
    			i++;
    	}
    for(unsigned int i=0;i<m_newCurves.size();){
        Curve* e = m_newCurves[i];
        //Si l'entité n'est connectée à rien, c'est quelle doit disparaitre
        std::vector<Vertex*> ev;
        e->get(ev);
        std::vector<Surface*> es;
        e->get(es);
        if(ev.empty() && es.empty()){
        	delete m_newCurves[i]; // [EB] si on ne détruit pas ici, je ne sais pas où ce sera fait
            Curve* laste = m_newCurves[m_newCurves.size()-1];
            m_newCurves[i] = laste;
            m_newCurves.pop_back();
            invalid[1]++;
        }
        else
        	i++;
    }
    for(unsigned int i=0;i<m_newSurfaces.size();){
        Surface* e = m_newSurfaces[i];
        //Si l'entité n'est connectée à rien, c'est quelle doit disparaitre
        std::vector<Volume*> ev;
        e->get(ev);
        std::vector<Curve*> ec;
        e->get(ec);
        if(ev.empty() && ec.empty()){
        	delete m_newSurfaces[i]; // [EB] si on ne détruit pas ici, je ne sais pas où ce sera fait
            Surface* laste = m_newSurfaces[m_newSurfaces.size()-1];
            m_newSurfaces[i] = laste;
            m_newSurfaces.pop_back();
            invalid[2]++;
        }
        else
        	i++;
    }
    for(unsigned int i=0;i<m_newVolumes.size();){
        Volume* e = m_newVolumes[i];
        //Si l'entité n'est connectée à rien, c'est quelle doit disparaitre
        std::vector<Surface*> es;
        e->get(es);
        if(es.empty()){
        	delete m_newVolumes[i]; // [EB] si on ne détruit pas ici, je ne sais pas où ce sera fait
            Volume* laste = m_newVolumes[m_newVolumes.size()-1];
            m_newVolumes[i] = laste;
            m_newVolumes.pop_back();
            invalid[3]++;
        }
        else
        	i++;
    }

    if(invalid[0]!=0){
        TkUtil::UTF8String   warningText (TkUtil::Charset::UTF_8);
        warningText<<invalid[0]<<" sommet(s) créé(s) à tort puis supprimé(s)";
        m_context.getLogStream()->log(IN_UTIL WarningLog (warningText));
    }
    if(invalid[1]!=0){
        TkUtil::UTF8String   warningText (TkUtil::Charset::UTF_8);
        warningText<<invalid[1]<<" courbe(s) créée(s) à tort puis supprimée(s)";
        m_context.getLogStream()->log(IN_UTIL WarningLog (warningText));
    }
    if(invalid[2]!=0){
        TkUtil::UTF8String   warningText (TkUtil::Charset::UTF_8);
        warningText<<invalid[2]<<" surface(s) créée(s) à tort puis supprimée(s)";
        m_context.getLogStream()->log(IN_UTIL WarningLog (warningText));
    }
    if(invalid[3]!=0){
        TkUtil::UTF8String   warningText (TkUtil::Charset::UTF_8);
        warningText<<invalid[3]<<" volume(s) créé(s) à tort puis supprimé(s)";
        m_context.getLogStream()->log(IN_UTIL WarningLog (warningText));
    }
}

/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::cleanRefEntities(){
    //En fait, on supprime les connectivites de entites de references
    //entre elles. Si un solide adjacent est connecte a une entite de ref
    // mais n'en est pas une, cette adjacence est conservée.
    for(unsigned int i=0; i<4;i++){
        std::list<GeomEntity*>::iterator it = m_ref_entities[i].begin();
        for(;it!=m_ref_entities[i].end();it++){
        	// Attention si l'entite de reference est adjacente, on ne fait rien
        	GeomEntity* e = *it;
        	e->clearRefEntities(m_ref_entities[0],m_ref_entities[1],
        			m_ref_entities[2],m_ref_entities[3]);
        }
    }
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::cleanEntityAndChildren(GeomEntity* e)
{
    //Ici, on supprime les connectivités entre e et ses cellules de dimension
    // inférieure
    std::vector<Surface*> e_surfaces;
    e->get(e_surfaces);

    Volume* ve = dynamic_cast<Volume*>(e);
    CHECK_NULL_PTR_ERROR(ve);
    for(unsigned int i=0;i<e_surfaces.size();i++)
    {
        Surface *si = e_surfaces[i];
        si->remove(ve);
        ve->remove(si);

        std::vector<Curve*> si_curves;
        si->get(si_curves);
        for(unsigned int j=0;j<si_curves.size();j++)
         {
             Curve *cj = si_curves[j];
             cj->remove(si);
             si->remove(cj);

             std::vector<Vertex*> cj_vertices;
             cj->get(cj_vertices);
             for(unsigned int k=0;k<cj_vertices.size();k++)
              {
                  Vertex* vk = cj_vertices[k];
                  cj->remove(vk);
                  vk->remove(cj);
              }
         }

    }

}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::
createGeomEntities(const TopoDS_Shape& shape, const bool replaceVolumes, const bool deleteAloneVertices)
{
#ifdef _DEBUG2
	std::cout<<"GeomModificationBaseClass::createGeomEntities shape ..."<<std::endl;
	std::cout<<(deleteAloneVertices?"avec":"sans")<<" destruction des sommets seuls"<<std::endl;
#endif
	/*shape est le resultat d'une operation quelconque pour laquelle on a conserve les
     * entites de depart m_init et les entites de refereces m_init. C-est-a-dire toutes
     * les entites adjacentes aux entites de m_init (uniquement de dim inferieure)
     */
    //===================================================================
    // 1 - ON NETTOIE LES CONNECTIVITES DES ENTITES DE REFERENCE
    //===================================================================
#ifdef _DEBUG2
    std::list<GeomEntity*>::iterator adj3_it =m_adj_entities[3].begin();
    for(;adj3_it!=m_adj_entities[3].end();adj3_it++)
    {
        std::cout<<"VOL ADJ = "<<(*adj3_it)->getName()<<std::endl;
    }
    adj3_it =m_ref_entities[3].begin();
    for(;adj3_it!=m_ref_entities[3].end();adj3_it++)
    {
        std::cout<<"VOL REF = "<<(*adj3_it)->getName()<<std::endl;
    }
#endif

    cleanRefEntities();

    // Maintenant, on n'a plus aucune connexion entre les entités géométriques
    // de référence
    //===================================================================
    // 2 - ON CREE LES ENTITES GEOMETRIQUES NOUVELLES ET ON RECUPERE LES
    //     ANCIENNES
    //===================================================================
    /* Les listes qui suivent contiennent toutes les entités créées (aussi
     * dans m_new...) mais aussi conservées (aussi dans m_keep... donc)
     */
    std::vector<TopoDS_Shape> OCC_surfaces, OCC_curves, OCC_vertices, OCC_volumes;
    std::vector<Vertex*>      M3D_vertices;
    std::vector<Curve*>       M3D_curves;
    std::vector<Surface*>     M3D_surfaces;
    std::vector<Volume*>      M3D_volumes;

    //Maintenant, on crée tous les nouveaux sommets et on conserve les anciens
    createNewVertices(shape, OCC_vertices, M3D_vertices);
    createNewCurves  (shape, OCC_curves  , M3D_curves);
    createNewSurfaces(shape, OCC_surfaces, M3D_surfaces);
    createNewVolumes (shape, OCC_volumes , M3D_volumes);

    //===================================================================
    // 3 - ON CONNECTE LES ENTITES ENTRE ELLES
    //===================================================================
    //toutes les entités nouvelles et anciennes participant au nouvel
    //objet "shape" sont dans les collections OCC_XXX et M3D_XXX
    connect(OCC_vertices,OCC_curves,OCC_surfaces,OCC_volumes,
            M3D_vertices,M3D_curves,M3D_surfaces,M3D_volumes);

    //===================================================================
    // 4 - ON RECUPERE LES ENTITES A SUPPRIMER
    //===================================================================
    clean(deleteAloneVertices); // On remplit m_removed

#ifdef _DEBUG2
    adj3_it =m_ref_entities[3].begin();
    for(;adj3_it!=m_ref_entities[3].end();adj3_it++)
    {
        std::cout<<"======> VOL REF = "<<(*adj3_it)->getName()<<std::endl;
    }
#endif
    //===================================================================
    // 5 - POUR CHAQUE ENTITE SUPPRIME, ON INDIQUE PAR QUI ELLE EST
    //     REMPLACEE.  LE TRAITEMENT SUR LES VOLUMES EST OPTIONNEL CAR
    //     COUTEUX
    //===================================================================
    buildReplacedList(replaceVolumes);

    //=========================================================================
#ifdef _DEBUG2
    std::map<GeomEntity*, std::vector<GeomEntity*> >::iterator it = m_replacedEntities.begin();
    while(it!=m_replacedEntities.end()){
        GeomEntity* oldE = it->first;
        std::vector<GeomEntity*> newE = it->second;
        std::cout<<oldE->getName()<<" replaced by"<<std::endl;
        for(unsigned int i=0;i<newE.size();i++)
            std::cout<<"\t "<<newE[i]->getName()<<std::endl;
        it++;
    }
#endif
    //===================================================================
    // 5 - LES ENTITES ADJACENTES PEUVENT AVOIR PERDU DES CELLULES ADJ
    // QUI ONT ETE REMPLACEES. ON MODIFIE CES ENTITES ET ON RETABLIT LES
    // CONNECTIONS
    //===================================================================
    rebuildAdjacencyEntities(shape);


    //    std::vector<Volume*>::iterator it_vKeep = m_toKeepVolumes.begin();
    //    while(it_vKeep!=m_toKeepVolumes.end()){
    //        Volume* v = *it_vKeep;
    //        std::cout<<"\t keep "<<v->getName()<<std::endl;
    //        it_vKeep++;
    //    }
    std::vector<GeomEntity*>::iterator it_rem = m_removedEntities.begin();
    std::vector<GeomEntity*> toErase;
    while(it_rem!=m_removedEntities.end()){
        GeomEntity* e = *it_rem;
        if(e->getDim()==3)
        {
            Volume* v = dynamic_cast<Volume*>(e);
            CHECK_NULL_PTR_ERROR(v);
            if(std::find(m_toKeepVolumes.begin(),m_toKeepVolumes.end(),v)==m_toKeepVolumes.end())
                toErase.push_back(e);
        }
        else if(e->getDim()==2)
        {
            Surface* s = dynamic_cast<Surface*>(e);
            CHECK_NULL_PTR_ERROR(s);
            if(std::find(m_toKeepSurfaces.begin(),m_toKeepSurfaces.end(),s)==m_toKeepSurfaces.end())
                toErase.push_back(e);
        }
        else if(e->getDim()==1)
        {
            Curve* c = dynamic_cast<Curve*>(e);
            CHECK_NULL_PTR_ERROR(c);
            if(std::find(m_toKeepCurves.begin(),m_toKeepCurves.end(),c)==m_toKeepCurves.end())
                toErase.push_back(e);
        }
        else if(e->getDim()==0)
        {
            Vertex* v = dynamic_cast<Vertex*>(e);
            CHECK_NULL_PTR_ERROR(v);
            if(std::find(m_toKeepVertices.begin(),m_toKeepVertices.end(),v)==m_toKeepVertices.end())
                toErase.push_back(e);
        }
        it_rem++;
    }

    m_removedEntities=toErase;

    //===================================================================
    // 6 - MISE A JOUR DES ENTITES POUR LA VISUALISATION
    //===================================================================
    m_newEntities.clear();
    m_newEntities.insert(m_newEntities.end(),m_newVolumes.begin(),m_newVolumes.end());
    m_newEntities.insert(m_newEntities.end(),m_newSurfaces.begin(),m_newSurfaces.end());
    m_newEntities.insert(m_newEntities.end(),m_newCurves.begin(),m_newCurves.end());
    m_newEntities.insert(m_newEntities.end(),m_newVertices.begin(),m_newVertices.end());
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::buildReplacedList(const bool replaceVolumes){
    std::vector<GeomEntity*>::iterator it= m_removedEntities.begin();
    std::vector<Volume*> removedVolumes;
    std::vector<GeomEntity*> toKeepInFact;
    while(it!=m_removedEntities.end()){
        GeomEntity* rem_entity = *it;
#ifdef _DEBUG2
        std::cout<<"To remove "<<rem_entity->getName()<<std::endl;
#endif
        switch(rem_entity->getDim()){
        case 0:
        {
        	Vertex* e = dynamic_cast<Vertex*>(rem_entity);
        	CHECK_NULL_PTR_ERROR(e);
        	computeReplacedVertex(e);
        }
        break;
        case 1:
        {
        	Curve* e = dynamic_cast<Curve*>(rem_entity);
        	CHECK_NULL_PTR_ERROR(e);
        	computeReplacedCurve(e);
        }
        break;
        case 2:
        {
        	Surface* e = dynamic_cast<Surface*>(rem_entity);
        	CHECK_NULL_PTR_ERROR(e);
        	computeReplacedSurface(e);
        }
        break;
        case 3:
        {
        	Volume* e = dynamic_cast<Volume*>(rem_entity);
        	CHECK_NULL_PTR_ERROR(e);
        	if(replaceVolumes)
        		computeReplacedVolume(e);
        	else
        		removedVolumes.push_back(e);
        }
        break;
        };
        it++;
    }

    // identification des entités remplacées par elle-même
    std::map<GeomEntity*,std::vector<GeomEntity*> >::iterator it_rep;
    for(it_rep=m_replacedEntities.begin();it_rep!=m_replacedEntities.end();it_rep++)
    {
        GeomEntity *from = it_rep->first;

        std::vector<GeomEntity*> to_entities = it_rep->second;
        std::list<GeomEntity*> to_list;
        to_list.insert(to_list.end(),to_entities.begin(),to_entities.end());
        to_list.sort();
        to_list.unique();

        std::vector<GeomEntity*> to_entities_cleaned;
        to_entities_cleaned.insert(to_entities_cleaned.end(),to_list.begin(),to_list.end());

        if(to_entities_cleaned.size()==1)
        {
            GeomEntity *to = to_entities_cleaned[0];
            if(from==to)
            {
                //ne pas remplacer en fait
                toKeepInFact.push_back(from);
            }
            else
                m_replacedEntities[from]=to_entities_cleaned;
        }
        else
            m_replacedEntities[from]=to_entities_cleaned;
    }

    for(unsigned int i=0;i<toKeepInFact.size();i++){
        GeomEntity* gi = toKeepInFact[i];
#ifdef _DEBUG2
        std::cout<<"ENTITE A CONSERVEE FINALEMENT : "<<gi->getName()<<std::endl;
#endif
        int pos = 0;
        bool stop=false;
        for(unsigned int j=0;j<m_removedEntities.size() && !stop;j++)
            if(gi==m_removedEntities[j]){
                pos=j;
                stop =true;
            }

        m_removedEntities[pos] = m_removedEntities.back();
        m_removedEntities.pop_back();
        m_replacedEntities.erase(gi);
    }

    //Pour les volumes, on regarde si 1 ancien volume est remplace par plusieurs
    // ou si plusieurs anciens volumes participent a en creer 1 nouveau.
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::rebuildAdjacencyEntities(const TopoDS_Shape& shape)
{
    //===================================================================
    // 1 - GESTION DES VOLUMES ADJACENTS
    //===================================================================
    std::vector<GeomEntity*> volumesToTest;
    std::list<GeomEntity*>::iterator it = m_adj_entities[3].begin();
    // ATTENTION SEUL DES VOLUMES NON REF SONT TRAITES
    for(;it!=m_adj_entities[3].end();it++){
        GeomEntity* ei = *it;

        std::list<GeomEntity*>::iterator it_ref = m_ref_entities[3].begin();
#ifdef _DEBUG2
        std::cout<<"NB REF VOLUMES = "<<m_ref_entities[3].size()<<std::endl;
        for(;it_ref!=m_ref_entities[3].end();it_ref++)
        {
            GeomEntity* ge = *it_ref;
            std::cout<<"\t ref volume : "<<ge->getName()<<std::endl;
        }
#endif

//        if(std::find(m_ref_entities[3].begin(),m_ref_entities[3].end(),ei)!=m_ref_entities[3].end()){
//            std::cout<<"REBUILD : ADJ VOLUME "<<ei->getName()<<" A NE PAS TRAITER CAR AUSSI UNE REF"<<std::endl;
//            continue;
//        }
#ifdef _DEBUG2
        std::cout<<"REBUILD ADJ VOLUME "<<ei->getName()<<std::endl;
#endif
       //on regarde si des surfaces du volumes ont été remplacées
        std::vector<Surface*> surfs;
        ei->get(surfs);
        bool surf_modified = false;

        for(unsigned int is=0;is<surfs.size();is++)
        {
#ifdef _DEBUG2
            std::cout<<" \t surf. "<<surfs[is]->getName()<<std::endl;
#endif
            std::map<GeomEntity*,std::vector<GeomEntity*> >::const_iterator it=m_replacedEntities.find(surfs[is]);
            if(it!=m_replacedEntities.end())
            {
                surf_modified=true;
            }
        }
#ifdef _DEBUG2
        std::cout<<"\t **** modif? "<<surf_modified<<std::endl;
#endif
        if(surf_modified)
        {
            //le volume adjacent est modifie mais seulement surfaciquement, il ne faut donc
            //pas appliquer l'operation sur lui !!!
#ifdef _DEBUG2
           std::cout<<"GESTION DE L'ENTITE ADJ "<<ei->getName()<<std::endl;
#endif
            TopoDS_Shape si;
            getOCCShape(ei, si);

            std::vector<GeomEntity*> newRef;
            newRef.push_back(ei);
            init(newRef);
            //on utilise la procedure standard sauf que l'on ne s'intéresse pas aux adjacences maintenant
            cleanEntityAndChildren(ei);

            //============================================================
            // 1 - Intersection de si avec l'objet resultat
            //============================================================
            BRepAlgoAPI_BuilderAlgo splitter;

            TopTools_ListOfShape list_of_arguments;
            list_of_arguments.Append(si);
            list_of_arguments.Append(shape);

            splitter.SetArguments(list_of_arguments);
            splitter.Build();
            //TopoDS_Shape si_result = splitter.Shape();

            TopTools_ListOfShape modSh;
            modSh = splitter.Modified(si);

            // Maintenant, on n'a plus aucune connexion entre les entités géométriques
            // de référence
            //===================================================================
            // 2 - ON CREE LES ENTITES GEOMETRIQUES NOUVELLES ET ON RECUPERE LES
            //     ANCIENNES
            //===================================================================
            /* Les listes qui suivent contiennent toutes les entités créées (aussi
             * dans m_new...) mais aussi conservées (aussi dans m_keep... donc)
             */
            std::vector<TopoDS_Shape> OCC_surfaces, OCC_curves, OCC_vertices, OCC_volumes;
            std::vector<Vertex*>      M3D_vertices;
            std::vector<Curve*>       M3D_curves;
            std::vector<Surface*>     M3D_surfaces;
            std::vector<Volume*>      M3D_volumes;

            //Maintenant, on crée tous les nouveaux sommets et on conserve les anciens


            TopTools_ListIteratorOfListOfShape itSh(modSh);
            for(itSh.Initialize(modSh);itSh.More();itSh.Next()){
                TopoDS_Shape si_result =itSh.Value();
                createNewVertices  (si_result, OCC_vertices, M3D_vertices,true);
                createNewCurves    (si_result, OCC_curves  , M3D_curves,true);
                createNewSurfaces  (si_result, OCC_surfaces, M3D_surfaces,true);
                createNewAdjVolumes(si_result, OCC_volumes , M3D_volumes,true);
            }

            //===================================================================
            // 3 - ON CONNECTE LES ENTITES ENTRE ELLES
            //===================================================================
            //toutes les entités nouvelles et anciennes participant au nouvel
            //objet "shape" sont dans les collections OCC_XXX et M3D_XXX
            connect(OCC_vertices,OCC_curves,OCC_surfaces,OCC_volumes,
                    M3D_vertices,M3D_curves,M3D_surfaces,M3D_volumes);

            //===================================================================
            // 4 - ON RECUPERE LES ENTITES A SUPPRIMER
            //===================================================================
            clean(true); // On remplit m_removed

            buildReplacedList(true);

             //=========================================================================
             std::map<GeomEntity*, std::vector<GeomEntity*> >::iterator it = m_replacedEntities.begin();
             while(it!=m_replacedEntities.end()){
                 GeomEntity* oldE = it->first;
                 std::vector<GeomEntity*> newE = it->second;
#ifdef _DEBUG2
                 std::cout<<oldE->getName()<<" replaced by"<<std::endl;
                 for(unsigned int i=0;i<newE.size();i++)
                     std::cout<<"\t "<<newE[i]->getName()<<std::endl;
#endif
                 it++;
             }

             //===================================================================
             // 5 - LES ENTITES ADJACENTES PEUVENT AVOIR PERDU DES CELLULES ADJ
             // QUI ONT ETE REMPLACES. ON MODIFIE CES ENTITES ET ON RETABLIT LES
             // CONNECTIONS
             //===================================================================
             rebuildAdjacencyLinks();

             //===================================================================
             // 6 - MISE A JOUR DES ENTITES POUR LA VISUALISATION
             //===================================================================
             m_newEntities.clear();
             m_newEntities.insert(m_newEntities.end(),m_newVolumes.begin(),m_newVolumes.end());
             m_newEntities.insert(m_newEntities.end(),m_newSurfaces.begin(),m_newSurfaces.end());
             m_newEntities.insert(m_newEntities.end(),m_newCurves.begin(),m_newCurves.end());
             m_newEntities.insert(m_newEntities.end(),m_newVertices.begin(),m_newVertices.end());
        }
    }
}

/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::rebuildAdjacencyLinks()
{
    //===================================================================
    // 1 - GESTION DES VOLUMES ADJACENTS
    //===================================================================
    std::vector<GeomEntity*> volumesToTest;
    std::list<GeomEntity*>::iterator it = m_adj_entities[3].begin();
    for(;it!=m_adj_entities[3].end();it++){
        GeomEntity* ei = *it;
#ifdef _DEBUG2
        std::cout<<" ADJ VOLUME "<<ei->getName()<<std::endl;
#endif
        //on regarde si des surfaces du volumes ont été remplacées
        std::vector<Surface*> surfs;
        ei->get(surfs);
        bool surf_modified = false;
//        std::cout<<"NB REPLACED ENTITIES = "<<m_replacedEntities.size()<<std::endl;
//        std::cout<<"NB SURFS = "<<surfs.size()<<std::endl;
        for(unsigned int is=0;is<surfs.size();is++){
            std::map<GeomEntity*,std::vector<GeomEntity*> >::const_iterator it
            =m_replacedEntities.find(surfs[is]);

            if(it!=m_replacedEntities.end()){
                //la surface a été remplacée!!!!
//                std::cout<<"\t surface remplacee"<<std::endl;
                surf_modified=true;
                ei->remove(surfs[is]);
                std::vector<GeomEntity*> newSurfs = it->second;
                for(unsigned int inewsurf=0;inewsurf<newSurfs.size();inewsurf++){
                    GeomEntity* si = newSurfs[inewsurf];
                    ei->add(si);
                    si->add(ei);
                }
            }
        }
        if(surf_modified)
            volumesToTest.push_back(ei);

    }
    //===================================================================
    // 2 - GESTION DES AUTRES ENTITES A FAIRE
    //===================================================================
    //===================================================================
    // 3 - MISE A JOUR POTENTIELLE SUR LE SHAPE OCC DES VOLUMES ADJACENTS
    //  SELON L'OPERATION, ON PEUT AVOIR A MODIFER LA SHAPE OCC D'UN
    // VOLUME ADJACENT CAR UNE DE SES SURFACES A ETE REMPLACEE. ON LAISSE
    // CE TRAITEMENT A LA RESPONSABILITÉ DE LA CLASSE FILLE
    //===================================================================
    //manageAdjacentEntities(volumesToTest);
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::computeReplacedVertex(Vertex* e)
{
    //e est une entité supprimée, on cherche celles l'ayant remplacées
    OCCGeomRepresentation* occ_rep =
            dynamic_cast<OCCGeomRepresentation*>(e->getComputationalProperty());
    CHECK_NULL_PTR_ERROR(occ_rep);
    TopoDS_Vertex occ_e = TopoDS::Vertex(occ_rep->getShape());
    //========================================================================
    // Pour un sommet, c'est forcement une entite conservee qui peut remplacer
    // notre sommet
    bool is_replaced  = false;
    for(unsigned int i=0;i<m_toKeepVertices.size() && !is_replaced;i++){
        Vertex* v = m_toKeepVertices[i];
        OCCGeomRepresentation* occ_rep_v =
                dynamic_cast<OCCGeomRepresentation*>(v->getComputationalProperty());
        CHECK_NULL_PTR_ERROR(occ_rep_v);
        TopoDS_Vertex occ_v = TopoDS::Vertex(occ_rep_v->getShape());
        // si on sait que la shape est partiellement remplacee, autant ne
        // plus tester les shapes en egalite
        if(OCCGeomRepresentation::areEquals(occ_v,occ_e)){
            m_replacedEntities[e].push_back(v); //e remplacee par c
            is_replaced = true;
        }
    }
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::computeReplacedCurve(Curve* e)
{
	std::vector<GeomRepresentation*> loc_reps = e->getComputationalProperties();
#ifdef _DEBUG2
	std::cout<<" computeReplacedCurve "<<e->getName()<<" avec "<<loc_reps.size()<<" GeomRepresentation"<<std::endl;
#endif

	for (uint j=0; j<loc_reps.size(); j++){
		OCCGeomRepresentation* occ_rep =
				dynamic_cast<OCCGeomRepresentation*>(loc_reps[j]);
		CHECK_NULL_PTR_ERROR(occ_rep);
		TopoDS_Edge occ_e = TopoDS::Edge(occ_rep->getShape());

		//========================================================================
		// Cela peut être une entité conservée (cas d'un glue par exemple) ou
		// nouvelle
		bool is_fully_replaced  = false;
		bool is_partly_replaced = false;
	#ifdef _DEBUG2
		std::cout<<"m_toKeepCurves :"<<std::endl;
	#endif
		for(unsigned int i=0;i<m_toKeepCurves.size() && !is_fully_replaced;i++){
			Curve* c = m_toKeepCurves[i];
	#ifdef _DEBUG2
			std::cout<<"  "<<c->getName()<<std::endl;
	#endif
			std::vector<GeomRepresentation*> loc_reps_c = c->getComputationalProperties();
			if (loc_reps_c.size() == 1){
				OCCGeomRepresentation* occ_rep_c =
						dynamic_cast<OCCGeomRepresentation*>(loc_reps_c[0]);
				CHECK_NULL_PTR_ERROR(occ_rep_c);
				TopoDS_Edge occ_c = TopoDS::Edge(occ_rep_c->getShape());
				// si on sait que la shape est partiellement remplacee, autant ne
				// plus tester les shapes en egalite
				if(!is_partly_replaced && OCCGeomRepresentation::areEquals(occ_c,occ_e)){
					m_replacedEntities[e].push_back(c); //e remplacee par c
					is_fully_replaced = true;
				}
				if(!is_partly_replaced && !is_fully_replaced && c->contains(e)){
					m_replacedEntities[e].push_back(c); //e remplacee par c
					is_fully_replaced = true;
				}

				if(!is_fully_replaced && e->contains(c)){
					m_replacedEntities[e].push_back(c); //e remplacee par c en partie
					is_partly_replaced = true;
				}
			}
		}
		//========================================================================
	#ifdef _DEBUG2
		std::cout<<"m_newCurves :"<<std::endl;
	#endif
		for(unsigned int i=0;i<m_newCurves.size() && !is_fully_replaced;i++){
			Curve* c = m_newCurves[i];
	#ifdef _DEBUG2
			std::cout<<"  "<<c->getName()<<std::endl;
	#endif
			std::vector<GeomRepresentation*> loc_reps_c = c->getComputationalProperties();
			if (loc_reps_c.size() == 1){
				OCCGeomRepresentation* occ_rep_c =
						dynamic_cast<OCCGeomRepresentation*>(loc_reps_c[0]);
				CHECK_NULL_PTR_ERROR(occ_rep_c);
				TopoDS_Edge occ_c = TopoDS::Edge(occ_rep_c->getShape());

				// si on sait que la shape est partiellement remplacee, autant ne
				// plus tester les shapes en egalite
				if(!is_partly_replaced && OCCGeomRepresentation::areEquals(occ_c,occ_e)){
					m_replacedEntities[e].push_back(c); //e remplacee par c
					is_fully_replaced = true;
				}
				if(!is_partly_replaced && !is_fully_replaced && c->contains(e)){
					m_replacedEntities[e].push_back(c); //e remplacee par c
					is_fully_replaced = true;
				}

				if(!is_fully_replaced && e->contains(c)){
					m_replacedEntities[e].push_back(c); //e remplacee par c en partie
					is_partly_replaced = true;
				}
			}
		}
#ifdef _DEBUG2
	    std::cout<<"is_fully_replaced = "<<is_fully_replaced<<std::endl;
	    std::cout<<"is_partly_replaced = "<<is_partly_replaced<<std::endl;
#endif
	} // end for (uint j=0; j<loc_reps.size(); j++)

#ifdef _DEBUG2
    std::cout<<"  remplacée par :";
    for (uint i=0; i<m_replacedEntities[e].size(); i++)
    	std::cout<<" "<<m_replacedEntities[e][i]->getName();
    std::cout<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::computeReplacedSurface(Surface* e)
{
#ifdef _DEBUG2
	std::cout<<" computeReplacedSurface("<<e->getName()<<")"<<std::endl;
#endif

	std::vector<GeomRepresentation*> loc_reps = e->getComputationalProperties();

	for (uint j=0; j<loc_reps.size(); j++){
		OCCGeomRepresentation* occ_rep =
				dynamic_cast<OCCGeomRepresentation*>(loc_reps[j]);
		CHECK_NULL_PTR_ERROR(occ_rep);
		TopoDS_Face occ_e = TopoDS::Face(occ_rep->getShape());
		//========================================================================
		// Cela peut être une entité conservée (cas d'un glue par exemple) ou
		// nouvelle
		bool is_fully_replaced  = false;
		bool is_partly_replaced = false;
		for(unsigned int i=0;i<m_toKeepSurfaces.size() && !is_fully_replaced;i++){
			Surface* s = m_toKeepSurfaces[i];
#ifdef _DEBUG2
			std::cout<<"  m_toKeepSurfaces["<<i<<"] : "<<s->getName()<<std::endl;
#endif
			std::vector<GeomRepresentation*> loc_reps_s = s->getComputationalProperties();
			if (loc_reps_s.size() == 1){
				OCCGeomRepresentation* occ_rep_s =
						dynamic_cast<OCCGeomRepresentation*>(loc_reps_s[0]);

				CHECK_NULL_PTR_ERROR(occ_rep_s);
				TopoDS_Face occ_s = TopoDS::Face(occ_rep_s->getShape());
				// si on sait que la shape est partiellement remplacee, autant ne
				// plus tester les shapes en egalite
				if(!is_partly_replaced && OCCGeomRepresentation::areEquals(occ_s,occ_e)){
					m_replacedEntities[e].push_back(s); //e remplacee par s
					is_fully_replaced = true;
				}
				if(!is_partly_replaced && !is_fully_replaced && s->contains(e)){
					m_replacedEntities[e].push_back(s); //e remplacee par s
					is_fully_replaced = true;
				}

				if(!is_fully_replaced && e->contains(s)){
					m_replacedEntities[e].push_back(s); //e remplacee par c en partie
					is_partly_replaced = true;
				}
			}
		}

		//========================================================================
		for(unsigned int i=0;i<m_newSurfaces.size() && !is_fully_replaced;i++){
			Surface* s = m_newSurfaces[i];
#ifdef _DEBUG2
			std::cout<<"  m_newSurfaces["<<i<<"] : "<<s->getName()<<std::endl;
#endif
			std::vector<GeomRepresentation*> loc_reps_s = s->getComputationalProperties();
			if (loc_reps_s.size() == 1){
				OCCGeomRepresentation* occ_rep_s =
						dynamic_cast<OCCGeomRepresentation*>(loc_reps_s[0]);
				CHECK_NULL_PTR_ERROR(occ_rep_s);
				TopoDS_Face occ_s = TopoDS::Face(occ_rep_s->getShape());
				// si on sait que la shape est partiellement remplacee, autant ne
				// plus tester les shapes en egalite
				if(!is_partly_replaced && OCCGeomRepresentation::areEquals(occ_s,occ_e)){
					m_replacedEntities[e].push_back(s); //e remplacee par s
					is_fully_replaced = true;
				}
				if(!is_partly_replaced && !is_fully_replaced && s->contains(e)){
					m_replacedEntities[e].push_back(s); //e remplacee par s
					is_fully_replaced = true;
				}

				if(!is_fully_replaced && e->contains(s)){
					m_replacedEntities[e].push_back(s); //e remplacee par c en partie
					is_partly_replaced = true;
				}
			}
		}
	} // end for (uint j=0; j<loc_reps.size(); j++)


#ifdef _DEBUG2
    std::cout<<"  => remplacée par";
    for(uint i=0; i<m_replacedEntities[e].size(); i++)
    	std::cout<<" "<<m_replacedEntities[e][i]->getName();
    std::cout<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::computeReplacedVolume(Volume* e)
{
	//e est une entité supprimée, on cherche celles l'ayant remplacées
	OCCGeomRepresentation* occ_rep =
			dynamic_cast<OCCGeomRepresentation*>(e->getComputationalProperty());
	CHECK_NULL_PTR_ERROR(occ_rep);
	TopoDS_Solid occ_e = TopoDS::Solid(occ_rep->getShape());

#ifdef _DEBUG2
	std::cout<<"Volume a remplacer : "<<e->getName()<<std::endl;
#endif
	//========================================================================
	// Cela peut être une entité conservée (cas d'un glue par exemple) ou
	// nouvelle
	bool is_fully_replaced  = false;
	bool is_partly_replaced = false;
	for(unsigned int i=0;i<m_toKeepVolumes.size() && !is_fully_replaced;i++){
		Volume* s = m_toKeepVolumes[i];
#ifdef _DEBUG2
		std::cout<<"  m_toKeepVolumes["<<i<<"] : "<<s->getName()<<std::endl;
#endif
		OCCGeomRepresentation* occ_rep_s =
				dynamic_cast<OCCGeomRepresentation*>(s->getComputationalProperty());
		CHECK_NULL_PTR_ERROR(occ_rep_s);
		TopoDS_Solid occ_s = TopoDS::Solid(occ_rep_s->getShape());
		// si on sait que la shape est partiellement remplacee, autant ne
		// plus tester les shapes en egalite
		if(!is_partly_replaced && OCCGeomRepresentation::areEquals(occ_s,occ_e)){
			m_replacedEntities[e].push_back(s); //e remplacee par s
			is_fully_replaced = true;
		}
		if(!is_partly_replaced && !is_fully_replaced && s->contains(e)){
			m_replacedEntities[e].push_back(s); //e remplacee par s
			is_fully_replaced = true;
		}

		if(!is_fully_replaced && e->contains(s)){
			m_replacedEntities[e].push_back(s); //e remplacee par c en partie
			is_partly_replaced = true;
		}
	}
	//========================================================================
	for(unsigned int i=0;i<m_newVolumes.size() && !is_fully_replaced;i++){
		Volume* s = m_newVolumes[i];
		OCCGeomRepresentation* occ_rep_s =
				dynamic_cast<OCCGeomRepresentation*>(s->getComputationalProperty());
		CHECK_NULL_PTR_ERROR(occ_rep_s);
		TopoDS_Solid occ_s = TopoDS::Solid(occ_rep_s->getShape());
		// si on sait que la shape est partiellement remplacee, autant ne
		// plus tester les shapes en egalite
#ifdef _DEBUG2
		std::cout<<"\t == comparaison avec "<<s->getName()<<std::endl;
		std::cout<<"\t  partly replaced = "<< is_partly_replaced<<std::endl;
		std::cout<<"\t  fully  replaced = "<< is_fully_replaced<<std::endl;
#endif
		if(!is_partly_replaced && OCCGeomRepresentation::areEquals(occ_s,occ_e)){

			m_replacedEntities[e].push_back(s); //e remplacee par s
			is_fully_replaced = true;
#ifdef _DEBUG2
			std::cout<<"\t totalement remplacé par "<<s->getName()<<std::endl;
#endif
		}
		if(!is_partly_replaced && !is_fully_replaced && s->contains(e)){
			m_replacedEntities[e].push_back(s); //e remplacee par s
			is_fully_replaced = true;
#ifdef _DEBUG2
			std::cout<<"\t contenu dans "<<s->getName()<<std::endl;
#endif
		}

		if(!is_fully_replaced && e->contains(s)){
			m_replacedEntities[e].push_back(s); //e remplacee par c en partie
			is_partly_replaced = true;
#ifdef _DEBUG2
			std::cout<<"\t partiellement remplacé par "<<s->getName()<<std::endl;
#endif

		}
	}

	if(!is_fully_replaced  && !is_partly_replaced )
	{
		if(std::find(m_adj_entities[3].begin(),m_adj_entities[3].end(),e)!=m_adj_entities[3].end())
		{
#ifdef _DEBUG2
			std::cout<<"ON CONSERVE FINALEMENT "<<e->getName()<<std::endl;
#endif
			m_toKeepVolumes.push_back(e);
		}
	}
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::connect(
        std::vector<TopoDS_Shape>& occ_vertices,
        std::vector<TopoDS_Shape>& occ_curves,
        std::vector<TopoDS_Shape>& occ_surfaces,
        std::vector<TopoDS_Shape>& occ_volumes,
        std::vector<Vertex*>&      m3d_vertices,
        std::vector<Curve*>&       m3d_curves,
        std::vector<Surface*>&     m3d_surfaces,
        std::vector<Volume*>&      m3d_volumes)
{
    //    std::cout<<"CANDIDATES SURFACES ================== "<<std::endl;
    //    for(unsigned int i=0;i<m3d_surfaces.size();i++){
    //        Surface* s = m3d_surfaces[i];
    //        std::cout<<"\t -> "<<s->getName()<<std::endl;
    //    }
    //=====================================================================
    // 1- On connecte les volumes aux surfaces (et inversement)
    //=====================================================================
    for(unsigned int i_vol=0;i_vol<m3d_volumes.size();i_vol++)
    {
        TopoDS_Shape occ_volume = occ_volumes[i_vol];
        Volume*      m3d_volume = m3d_volumes[i_vol];
        // TOUTES LES SURFACES
        TopTools_IndexedMapOfShape  faces;
        TopExp::MapShapes(occ_volume,TopAbs_FACE, faces);
        for(int i_face = 1; i_face <= faces.Extent(); i_face++)
        {
            TopoDS_Face F = TopoDS::Face(faces(i_face));
            bool found=false;
            for(unsigned int i_f=0;i_f<occ_surfaces.size()&& !found;i_f++)
            {
                TopoDS_Face F2 = TopoDS::Face(occ_surfaces[i_f]);
                if(OCCGeomRepresentation::areEquals(F,F2))
                {
                    found=true;
                    std::vector<Surface*> this_surf;
                    m3d_volume->get(this_surf);
                    if(std::find(this_surf.begin(),this_surf.end(),m3d_surfaces[i_f])==this_surf.end())
                    {
                        m3d_volume->add(m3d_surfaces[i_f]);
                        m3d_surfaces[i_f]->add(m3d_volume);
                    }
                }
            }
        }
    }
    //=====================================================================
    // 2- On connecte les surfaces aux courbes(et inversement)
    //=====================================================================
    for(unsigned int i_surf=0;i_surf<m3d_surfaces.size();i_surf++)
    {
        TopoDS_Shape occ_surface = occ_surfaces[i_surf];
        Surface*     m3d_surface = m3d_surfaces[i_surf];

        // TOUTES LES COURBES
        // On utilise un explorer plutot qu'un TopTools_IndexedMapOfShape
        // car cela permet de conserver les courbes apparaissant 2 fois
        // au bord.
        TopExp_Explorer exp;
        int compteur=1;
        for(exp.Init(occ_surface, TopAbs_EDGE); exp.More(); exp.Next())
        {
            TopoDS_Edge E = TopoDS::Edge(exp.Current());
            bool found=false;
            for(unsigned int i_e=0;i_e<occ_curves.size()&& !found;i_e++)
            {
                TopoDS_Edge E2 = TopoDS::Edge(occ_curves[i_e]);
                if(OCCGeomRepresentation::areEquals(E,E2))
                {

                    found=true;
                    std::vector<Curve*> this_curv;
                    m3d_surface->get(this_curv);
                    // une courbe ne référence qu'une fois une surface
                    if(std::find(this_curv.begin(),this_curv.end(),m3d_curves[i_e])==this_curv.end())
                    {
                        m3d_curves[i_e]->add(m3d_surface);
                    }
                    // une surface peut référencer plusieurs fois une courbe
                    m3d_surface->add(m3d_curves[i_e]);

                }
            }
        }
    }
    //=====================================================================
    // 3- On connecte les courbes aux sommets(et inversement)
    //=====================================================================
    for(unsigned int i_curv=0;i_curv<m3d_curves.size();i_curv++)
    {
        TopoDS_Shape occ_curve = occ_curves[i_curv];
        Curve*     m3d_curve = m3d_curves[i_curv];

        // TOUS LES SOMMETS
        TopTools_IndexedMapOfShape  vertices;
        TopExp::MapShapes(occ_curve,TopAbs_VERTEX, vertices);
        for(int i_vertex = 1; i_vertex <= vertices.Extent(); i_vertex++)
        {
            TopoDS_Vertex V = TopoDS::Vertex(vertices(i_vertex));
            bool found=false;
            for(unsigned int i_v=0;i_v<occ_vertices.size()&& !found;i_v++)
            {
                TopoDS_Vertex V2 = TopoDS::Vertex(occ_vertices[i_v]);
                if(OCCGeomRepresentation::areEquals(V,V2))
                {
                    found=true;
                    std::vector<Vertex*> this_vert;
                    m3d_curve->get(this_vert);
                    if(std::find(this_vert.begin(),this_vert.end(),m3d_vertices[i_v])==this_vert.end())
                    {
                        m3d_curve->add(m3d_vertices[i_v]);
                        m3d_vertices[i_v]->add(m3d_curve);
                    }
                }
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
/// comparaison géométrique de 2 points pour permettre un ordonnancement spacial
static int diffGpPnt(const gp_Pnt& pnt1, const gp_Pnt& pnt2, double dimX, double dimY, double dimZ)
{
#ifdef _DEBUG_DIFF
	std::cout<<setprecision(10)<<"diffGpPnt entre ["<<pnt1.X()<<","<<pnt1.Y()<<","<<pnt1.Z()<<"] et ["<<pnt2.X()<<","<<pnt2.Y()<<","<<pnt2.Z()<<"] avec dim : "<<dimX<<", "<<dimY<<", "<<dimZ<<", "<<std::endl;
#endif
#ifdef _DEBUG_DIFF_OTHER
	std::cout<<setprecision(10)<<"diffGpPnt entre ["<<pnt1.X()<<","<<pnt1.Y()<<","<<pnt1.Z()<<"] et ["<<pnt2.X()<<","<<pnt2.Y()<<","<<pnt2.Z()<<"] avec dim : "<<dimX<<", "<<dimY<<", "<<dimZ<<", "<<std::endl;
	int res1 = 0;

	// algo historique
	if (pnt1.X()<pnt2.X()-Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon){
		std::cout<<"pnt1.X()<pnt2.X()-eps"<<std::endl;
		res1 = -1;
	}
	else if (pnt1.X()>pnt2.X()+Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon){
		std::cout<<"pnt1.X()>pnt2.X()+eps"<<std::endl;
		res1 = 1;
	}
	else {
		if (pnt1.Y()<pnt2.Y()-Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon){
			std::cout<<"pnt1.Y()<pnt2.Y()-eps"<<std::endl;
			res1 = -1;
		}
		else if (pnt1.Y()>pnt2.Y()+Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon){
			std::cout<<"pnt1.Y()>pnt2.Y()+eps"<<std::endl;
			res1 = 1;
		}
		else {
			if (pnt1.Z()<pnt2.Z()-Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon){
				std::cout<<"pnt1.Z()<pnt2.Z()-eps"<<std::endl;
				res1 = -1;
			}
			else if (pnt1.Z()>pnt2.Z()+Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon){
				std::cout<<"pnt1.Z()>pnt2.Z()+eps"<<std::endl;
				res1 = 1;
			}
			else {
				std::cout<<" c'est jugé équivalent ..."<<std::endl;
				res1 = 0;
			}
		}
	}
#endif
	// nouvel algo
	double tol = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon*100.0;
	int res2 = 0;

	if (pnt1.X()<pnt2.X()-tol*dimX){
#ifdef _DEBUG_DIFF
		std::cout<<"pnt1.X()<pnt2.X()-eps"<<std::endl;
#endif
		res2 = -1;
	}
	else if (pnt1.X()>pnt2.X()+tol*dimX){
#ifdef _DEBUG_DIFF
		std::cout<<"pnt1.X()>pnt2.X()+eps"<<std::endl;
#endif
		res2 = 1;
	}
	else {
		if (pnt1.Y()<pnt2.Y()-tol*dimY){
#ifdef _DEBUG_DIFF
			std::cout<<"pnt1.Y()<pnt2.Y()-eps"<<std::endl;
#endif
			res2 = -1;
		}
		else if (pnt1.Y()>pnt2.Y()+tol*dimY){
#ifdef _DEBUG_DIFF
			std::cout<<"pnt1.Y()>pnt2.Y()+eps"<<std::endl;
#endif
			res2 = 1;
		}
		else {
			if (pnt1.Z()<pnt2.Z()-tol*dimZ){
#ifdef _DEBUG_DIFF
				std::cout<<"pnt1.Z()<pnt2.Z()-eps"<<std::endl;
#endif
				res2 = -1;
			}
			else if (pnt1.Z()>pnt2.Z()+tol*dimZ){
#ifdef _DEBUG_DIFF
				std::cout<<"pnt1.Z()>pnt2.Z()+eps"<<std::endl;
#endif
				res2 = 1;
			}
			else {
#ifdef _DEBUG_DIFF
				std::cout<<" c'est jugé équivalent ..."<<std::endl;
#endif
				res2 = 0;
			}
		}
	}

#ifdef _DEBUG_DIFF_OTHER
	if (res1 != res2){
		std::cout<<"CAS AVEC DIFFERENCE !!!"<<std::endl;
	}
#endif

	return res2;
}
/*----------------------------------------------------------------------------*/
/// comparaison géométrique de 2 listes de points pour permettre un ordonnancement spacial
static int diffListeGpPnt(std::list<gp_Pnt>& lpnt1, std::list<gp_Pnt>& lpnt2)
{
	if (lpnt1.size()<lpnt2.size())
		return -1;
	else if (lpnt1.size()>lpnt2.size())
		return 1;
	else {
		for (std::list<gp_Pnt>::iterator iter1 = lpnt1.begin(), iter2 = lpnt2.begin();
				iter1 != lpnt1.end(); ++iter1, ++iter2){
			int res = diffGpPnt(*iter1, *iter2, 1.0, 1.0, 1.0);
			if (res == -1)
				return -1;
			else if (res == 1)
				return 1;
		}
		return 0;
	}
}
/*----------------------------------------------------------------------------*/
/// comparaison géométrique de 2 vertex pour permettre un ordonnancement spacial
static bool compareOCCVertex(const TopoDS_Vertex v1, const TopoDS_Vertex v2)
{
	gp_Pnt pnt1 = BRep_Tool::Pnt(v1);
	gp_Pnt pnt2 = BRep_Tool::Pnt(v2);

	int res = diffGpPnt(pnt1, pnt2, 1.0, 1.0, 1.0);
	if (res == -1)
		return false;
	else if (res == 1)
		return true;
	else {
#ifdef _DEBUG2
		std::cerr <<" 2 TopoDS_Vertex avec coordonnées identiques à "
				<<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon
				<<" près !"<<std::endl;
#endif
		return true;
	}
}
/*----------------------------------------------------------------------------*/
/// comparaison géométrique de 2 gp_pnt pour permettre un ordonnancement spacial
static bool comparePnt(const gp_Pnt& pnt1, const gp_Pnt& pnt2)
{
	int res = diffGpPnt(pnt1, pnt2, 1.0, 1.0, 1.0);
	if (res == -1)
		return false;
	else if (res == 1)
		return true;
	else
		return true;
}
/*----------------------------------------------------------------------------*/
std::list<gp_Pnt> getOrderedGpPnt(TopoDS_Edge edge)
{
#ifdef _DEBUG3
    	std::cout<<"* getOrderedGpPnt ..."<<std::endl;
#endif
	TopTools_IndexedMapOfShape map_vertices;
	TopExp::MapShapes(edge,TopAbs_VERTEX, map_vertices);
	std::list<gp_Pnt> ordered_pnt;
	for(int i = 1; i <= map_vertices.Extent(); i++){
		TopoDS_Vertex V = TopoDS::Vertex(map_vertices(i));
		gp_Pnt pnt = BRep_Tool::Pnt(V);
		ordered_pnt.push_back(pnt);
	}
	// on ajoute un point au milieu de la courbe
    BRepAdaptor_Curve curve_adaptor(edge);
    Standard_Real f1 = curve_adaptor.FirstParameter();
    Standard_Real l1 = curve_adaptor.LastParameter();
	double param = f1+(l1-f1)*0.5;
	gp_Pnt local_pnt = curve_adaptor.Value( param );
	ordered_pnt.push_back(local_pnt);

	ordered_pnt.sort(comparePnt);

    for(std::list<gp_Pnt>::iterator iter = ordered_pnt.begin();
    		iter != ordered_pnt.end(); ++iter){
    	gp_Pnt& pnt = *iter;
#ifdef _DEBUG3
    	std::cout<<"*  pnt en "<<pnt.X()<<" "<<pnt.Y()<<" "<<pnt.Z()<<std::endl;
#endif
    }

	return ordered_pnt;
}
/*----------------------------------------------------------------------------*/
std::list<gp_Pnt> getOrderedGpPnt(TopoDS_Face face)
{
	TopTools_IndexedMapOfShape map_vertices;
	TopExp::MapShapes(face,TopAbs_VERTEX, map_vertices);
	std::list<gp_Pnt> ordered_pnt;
	for(int i = 1; i <= map_vertices.Extent(); i++){
		TopoDS_Vertex V = TopoDS::Vertex(map_vertices(i));
		gp_Pnt pnt = BRep_Tool::Pnt(V);
		ordered_pnt.push_back(pnt);
	}

	ordered_pnt.sort(comparePnt);
	return ordered_pnt;
}
/*----------------------------------------------------------------------------*/
std::list<gp_Pnt> getOrderedGpPnt(TopoDS_Solid solid)
{
	TopTools_IndexedMapOfShape map_vertices;
	TopExp::MapShapes(solid,TopAbs_VERTEX, map_vertices);
	std::list<gp_Pnt> ordered_pnt;
	for(int i = 1; i <= map_vertices.Extent(); i++){
		TopoDS_Vertex V = TopoDS::Vertex(map_vertices(i));
		gp_Pnt pnt = BRep_Tool::Pnt(V);
		ordered_pnt.push_back(pnt);
	}

	ordered_pnt.sort(comparePnt);
	return ordered_pnt;
}
/*----------------------------------------------------------------------------*/
/// comparaison géométrique de 2 courbes pour permettre un ordonnancement spacial
static bool compareOCCEdge(const TopoDS_Edge c1, const TopoDS_Edge c2)
{
	std::list<gp_Pnt> lpnt1 = getOrderedGpPnt(c1);
	std::list<gp_Pnt> lpnt2 = getOrderedGpPnt(c2);

	int res = diffListeGpPnt(lpnt1, lpnt2);
	if (res == -1)
		return false;
	else if (res == 1)
		return true;
	else {
#ifdef _DEBUG
		std::cerr <<" 2 TopoDS_Edge avec coordonnées identiques à "
				<<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon
				<<" près !"<<std::endl;
#endif
		return true;
	}
}
/*----------------------------------------------------------------------------*/
/// comparaison géométrique de 2 surfaces pour permettre un ordonnancement spacial
static bool compareOCCFace(const TopoDS_Face f1, const TopoDS_Face f2)
{
#ifdef _DEBUG_DIFF
	std::cout<<"compareOCCFace entre 2 surfaces : "<<std::endl;
	GProp_GProps pb;
	BRepGProp::SurfaceProperties(f1,pb);
	std::cout<<"  F1 Mass "<<pb.Mass()<<std::endl;
	BRepGProp::SurfaceProperties(f2,pb);
	std::cout<<"  F2 Mass "<<pb.Mass()<<std::endl;
#endif

	// comparaison dans un premier temps des bounding box
	//double gap = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
	double gap = 0;
    Bnd_Box box1, box2;
    box1.SetGap(gap);
    box2.SetGap(gap);
    BRepBndLib::Add(f1,box1);
    BRepBndLib::Add(f2,box2);

    gp_Pnt minPnt1 = box1.CornerMin();
    gp_Pnt maxPnt1 = box1.CornerMax();
    gp_Pnt minPnt2 = box2.CornerMin();
    gp_Pnt maxPnt2 = box2.CornerMax();

	int res = diffGpPnt(minPnt1, minPnt2, maxPnt1.X()-minPnt1.X(), maxPnt1.Y()-minPnt1.Y(), maxPnt1.Z()-minPnt1.Z());
#ifdef _DEBUG_DIFF
	std::cout<<"diffGpPnt minPnt1 minPnt2 res = "<<res<<std::endl;
#endif
	if (res == -1)
		return false;
	else if (res == 1)
		return true;
	else {
		res = diffGpPnt(maxPnt1, maxPnt2, maxPnt1.X()-minPnt1.X(), maxPnt1.Y()-minPnt1.Y(), maxPnt1.Z()-minPnt1.Z());
#ifdef _DEBUG_DIFF
		std::cout<<"diffGpPnt maxPnt1 maxPnt2 res = "<<res<<std::endl;
#endif
		if (res == -1)
			return false;
		else if (res == 1)
			return true;
		else {
			// donc les boites sont identiques à mgxGeomDoubleEpsilon près

			// comparaison des sommets
			std::list<gp_Pnt> lpnt1 = getOrderedGpPnt(f1);
			std::list<gp_Pnt> lpnt2 = getOrderedGpPnt(f2);

			int res = diffListeGpPnt(lpnt1, lpnt2);
#ifdef _DEBUG_DIFF
			std::cout<<"diffListeGpPnt lpnt1 lpnt2 res = "<<res<<std::endl;
#endif
			if (res == -1)
				return false;
			else if (res == 1)
				return true;
			else {
				GProp_GProps gp1, gp2;
				BRepGProp::SurfaceProperties(f1,gp1);
				BRepGProp::SurfaceProperties(f2,gp2);
				if (gp1.Mass()>gp2.Mass())
					return false;
				else if (gp1.Mass()<gp2.Mass())
					return true;
#ifdef _DEBUG_DIFF
				std::cerr <<" 2 TopoDS_Face avec coordonnées identiques à "
						<<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon
						<<" près !"<<std::endl;
				std::cout <<" 2 TopoDS_Face avec coordonnées identiques à "
						<<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon
						<<" près !"<<std::endl;
				//std::cerr<<"HashCode: "<<(long int)f1.HashCode(INT_MAX)<<" et "<<(long int)f2.HashCode(INT_MAX)<<std::endl;
#endif
				return true;
			}
		}
	}
}
/*----------------------------------------------------------------------------*/
/// comparaison géométrique de 2 volumes pour permettre un ordonnancement spacial
static bool compareOCCSolid(const TopoDS_Solid s1, const TopoDS_Solid s2)
{
	// comparaison dans un premier temps des bounding box
	double gap = 0;
    Bnd_Box box1, box2;
    box1.SetGap(gap);
    box2.SetGap(gap);
    BRepBndLib::Add(s1,box1);
    BRepBndLib::Add(s2,box2);

    gp_Pnt minPnt1 = box1.CornerMin();
    gp_Pnt maxPnt1 = box1.CornerMax();
    gp_Pnt minPnt2 = box2.CornerMin();
    gp_Pnt maxPnt2 = box2.CornerMax();

	int res = diffGpPnt(minPnt1, minPnt2, maxPnt1.X()-minPnt1.X(), maxPnt1.Y()-minPnt1.Y(), maxPnt1.Z()-minPnt1.Z());
	if (res == -1)
		return false;
	else if (res == 1)
		return true;
	else {
		res = diffGpPnt(maxPnt1, maxPnt2, maxPnt1.X()-minPnt1.X(), maxPnt1.Y()-minPnt1.Y(), maxPnt1.Z()-minPnt1.Z());
		if (res == -1)
			return false;
		else if (res == 1)
			return true;
		else {
			// donc les boites sont identiques à mgxGeomDoubleEpsilon près

			// comparaison des sommets
			std::list<gp_Pnt> lpnt1 = getOrderedGpPnt(s1);
			std::list<gp_Pnt> lpnt2 = getOrderedGpPnt(s2);

			int res = diffListeGpPnt(lpnt1, lpnt2);
			if (res == -1)
				return false;
			else if (res == 1)
				return true;
			else {
#ifdef _DEBUG
				std::cerr <<" 2 TopoDS_Solid avec coordonnées identiques à "
						<<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon
						<<" près !"<<std::endl;
#endif
				return true;
			}
		}
	}
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::createNewVertices(const TopoDS_Shape& shape,
        std::vector<TopoDS_Shape>& occ_entities,
        std::vector<Vertex*>& m3d_entities,
        bool isAdjacent)
{
    TopTools_IndexedMapOfShape  vertices;
    TopExp::MapShapes(shape,TopAbs_VERTEX, vertices);
#ifdef _DEBUG2
    std::cout<<"**** createNewVertices ... avec "<<vertices.Extent()<<" entités"<<std::endl;
#endif

    std::list<TopoDS_Vertex> ordered_vertices;
    for(int i = 1; i <= vertices.Extent(); i++){
    	TopoDS_Vertex V = TopoDS::Vertex(vertices(i));
    	ordered_vertices.push_back(V);
#ifdef _DEBUG2
        gp_Pnt pnt = BRep_Tool::Pnt(V);
    	std::cout<<"**** Vertex "<<i<<" en "<<pnt.X()<<" "<<pnt.Y()<<" "<<pnt.Z()<<std::endl;
#endif
    }
    ordered_vertices.sort(compareOCCVertex);

    Vertex *newVertex=0;
    TopoDS_Vertex newOCCVertex;

    //On parcourt tous les sommets de shape
    for (std::list<TopoDS_Vertex>::iterator iter=ordered_vertices.begin();
    		iter!=ordered_vertices.end(); ++iter)
    {
    	TopoDS_Vertex V = *iter;
#ifdef _DEBUG2
        gp_Pnt pnt = BRep_Tool::Pnt(V);
    	std::cout<<"**** Vertex en "<<pnt.X()<<" "<<pnt.Y()<<" "<<pnt.Z()<<std::endl;
#endif
        bool to_keep=false;
        // pour chaque sommet de la nouvelle shape, on regarde si
        // ce sommet n'existe pas déjà dans une shape de référence

        // if the ref entities are not kept new cells must be
        // created to replace them
        std::list<GeomEntity*>::iterator it = m_ref_entities[0].begin();
        for(;it!=m_ref_entities[0].end() && !to_keep; it++)
        {
            //SOMMET COURANT
            Vertex* current = dynamic_cast<Vertex*>(*it);
            CHECK_NULL_PTR_ERROR(current);
            //REPRESENTION OCC ASSOCIEE
            OCCGeomRepresentation* occ_rep =
                    dynamic_cast<OCCGeomRepresentation*>(current->getComputationalProperty());
            CHECK_NULL_PTR_ERROR(occ_rep);
            //SOMMET OCC CORRESPONDANT
            TopoDS_Vertex vrep = TopoDS::Vertex(occ_rep->getShape());
            if(OCCGeomRepresentation::areEquals(V,vrep)){
                // on a trouve que le sommet existe déjà,
                to_keep = true;
                newVertex = current;
                newOCCVertex = V;
                // on indique donc que c'est un sommet à conserver
                m_toKeepVertices.push_back(current);
#ifdef _DEBUG2
                std::cout<<"**** "<<current->getName()<<" a conserver"<<std::endl;
#endif
            }
        }

        //si on ne la pas trouve dans les sommets de reference, on
        //regarde tout de meme s'il n'est pas dans les sommets crees
        //par l'operation avant
        bool found_in_news=false;
        if(!to_keep)
        {
            /* s'il n'est dans aucune des entités de référence, on vérifie
             * qu'il n'a pas deja ete ajoute precedemment  */
            for(unsigned int k=0; k<m_newVertices.size() && !found_in_news;k++)
            {
                Geom::Vertex* current = m_newVertices[k];
                OCCGeomRepresentation* occ_rep =
                        dynamic_cast<OCCGeomRepresentation*>(current->getComputationalProperty());
                CHECK_NULL_PTR_ERROR(occ_rep);
                TopoDS_Vertex vrep = TopoDS::Vertex(occ_rep->getShape());

                if(OCCGeomRepresentation::areEquals(V,vrep)){
                    found_in_news=true;
                    newVertex=current;
                    newOCCVertex=V;
#ifdef _DEBUG2
                    std::cout<<"**** "<<current->getName()<<" deja cree"<<std::endl;
#endif
                }
            }
        }

        if(!to_keep && !found_in_news)
        {
            // on crée la shape M3D correspondante
            newVertex= EntityFactory(m_context).newOCCVertex(V);
            newOCCVertex=V;
            m_newEntities.push_back(newVertex);
            m_newVertices.push_back(newVertex);
#ifdef _DEBUG2
                std::cout<<"**** "<<newVertex->getName()<<" nouveau"<<std::endl;
//            std::cout<<"Creation du sommet "<<newVertex->getName()<<" ("<<newVertex->getX()<<", "<<newVertex->getY()<<", "<<newVertex->getZ()<<")"<<std::endl;
#endif
        }
        if(!found_in_news){ //car sinon on l'ajoute deux fois
            occ_entities.push_back(newOCCVertex);
            m3d_entities.push_back(newVertex);
        }
        else if (!to_keep &&found_in_news && isAdjacent){
            if(std::find(m3d_entities.begin(),m3d_entities.end(),newVertex)==m3d_entities.end()){
                occ_entities.push_back(newOCCVertex);
                m3d_entities.push_back(newVertex);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::createNewCurves(const TopoDS_Shape& shape,
        std::vector<TopoDS_Shape>& occ_entities,
        std::vector<Curve*>&       m3d_entities,
        bool isAdjacent){
    TopTools_IndexedMapOfShape  entities;
    TopExp::MapShapes(shape,TopAbs_EDGE, entities);
#ifdef _DEBUG2
	std::cout<<"**** createNewCurves ... avec "<<entities.Extent()<<" entités"<<std::endl;
#endif

    std::list<TopoDS_Edge> sorted_entities;
    for(int i = 1; i <= entities.Extent(); i++){
    	TopoDS_Edge E = TopoDS::Edge(entities(i));
    	sorted_entities.push_back(E);
    }
    sorted_entities.sort(compareOCCEdge);

    for (std::list<TopoDS_Edge>::iterator iter=sorted_entities.begin();
        		iter!=sorted_entities.end(); ++iter)
    {
    	TopoDS_Edge E = *iter;
        BRepCheck_Analyzer anaAna1(E, Standard_False);
        GProp_GProps pb;
        BRepGProp::LinearProperties(E,pb);
        if(!(pb.Mass()<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)){
            Curve *newEdge=0;
            TopoDS_Edge newOCCEdge;

            bool to_keep=false;
            // pour chaque courbe de la nouvelle shape, on regarde si
            // cette courbe n'existe pas déjà dans une shape de référence
            // if the ref entities are not kept new cells must be created to replace them
            std::list<GeomEntity*>::iterator it = m_ref_entities[1].begin();

            for(;it!=m_ref_entities[1].end() && !to_keep; it++)
            {
                Curve* current = dynamic_cast<Curve*>(*it);
                CHECK_NULL_PTR_ERROR(current);

                std::vector<GeomRepresentation*> loc_reps = current->getComputationalProperties();

                for (uint j=0; j<loc_reps.size(); j++){
                	OCCGeomRepresentation* occ_rep =
                			dynamic_cast<OCCGeomRepresentation*>(loc_reps[j]);
                	CHECK_NULL_PTR_ERROR(occ_rep);
                	TopoDS_Edge ref_edge = TopoDS::Edge(occ_rep->getShape());

                	if(OCCGeomRepresentation::areEquals(E,ref_edge)){
                		// on a trouve que le sommet existe déjà, on conserve
                		// la référence
                		to_keep = true;
                		newEdge = current;
                		newOCCEdge = E;
                		m_toKeepCurves.push_back(current);
#ifdef _DEBUG2
                		std::cout<<"**** "<<current->getName()<<" a conserver"<<std::endl;
#endif
                	}
                } // end for j
            }
            /* s'il n'est dans aucune des entités de référence, on vérifie
             * qu'il n'a pas deja ete ajoute precedemment  */
            bool found_in_news=false;
            if(!to_keep)
            {
                for(unsigned int k=0; k<m_newCurves.size() && !found_in_news;k++)
                {
                    Curve* current = m_newCurves[k];
                    std::vector<GeomRepresentation*> loc_reps = current->getComputationalProperties();
                    if (loc_reps.size() == 1){
                    	OCCGeomRepresentation* occ_rep =
                    			dynamic_cast<OCCGeomRepresentation*>(loc_reps[0]);
                    	CHECK_NULL_PTR_ERROR(occ_rep);
                    	TopoDS_Edge ref_edge = TopoDS::Edge(occ_rep->getShape());

                    	if(OCCGeomRepresentation::areEquals(E,ref_edge)){
                    		found_in_news=true;
                    		newEdge = current;
                    		newOCCEdge = E;
#ifdef _DEBUG2
                    		std::cout<<"**** "<<current->getName()<<" deja cree"<<std::endl;
#endif
                    	}
                    }
                }

            }
            if(!to_keep && !found_in_news)
            {
                newEdge = EntityFactory(m_context).newOCCCurve(E);
                newOCCEdge=E;
                m_newEntities.push_back(newEdge);
                m_newCurves.push_back(newEdge);
#ifdef _DEBUG2
                std::cout<<"**** "<<newEdge->getName()<<" nouvelle"<<std::endl;
#endif
            }
            if(!found_in_news){ //car sinon on l'ajoute deux fois
                occ_entities.push_back(newOCCEdge);
                m3d_entities.push_back(newEdge);
            }

            else if (!to_keep && found_in_news && isAdjacent){
                if(std::find(m3d_entities.begin(),m3d_entities.end(),newEdge)==m3d_entities.end()){
                    occ_entities.push_back(newOCCEdge);
                    m3d_entities.push_back(newEdge);
                }
            }
        }
        else{
#ifdef _DEBUG2
            std::cout<<"\t NULL so skipped"<<std::endl;
#endif
        }
    }
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::createNewSurfaces(
		const TopoDS_Shape&              entity,
        std::vector<TopoDS_Shape>& occ_entities,
        std::vector<Surface*>&     m3d_entities,
        bool isAdjacent)
{
    TopTools_IndexedMapOfShape entities;
    TopExp::MapShapes(entity,TopAbs_FACE, entities);
#ifdef _DEBUG2
    std::cout<<"**** createNewSurfaces ... avec "<<entities.Extent()<<" entités"<<std::endl;
#endif
    std::list<TopoDS_Face> sorted_entities;
    for(int i = 1; i <= entities.Extent(); i++){
    	TopoDS_Face F = TopoDS::Face(entities(i));
    	sorted_entities.push_back(F);
    	OCCGeomRepresentation::buildIncrementalBRepMesh(F, 0.01); // calcul de la triangulation interne
    }
    sorted_entities.sort(compareOCCFace);
#ifdef _DEBUG2
    std::cout<<" ==== end sorted_entities.sort ===="<<std::endl;
#endif

    for (std::list<TopoDS_Face>::iterator iter=sorted_entities.begin();
        		iter!=sorted_entities.end(); ++iter)
    {
    	TopoDS_Face F = *iter;

        GProp_GProps pb;
        BRepGProp::SurfaceProperties(F,pb);
#ifdef _DEBUG2
    	std::cout<<"  F Mass "<<pb.Mass()<<std::endl;
#endif
    	// pour éviter de conserver le plan (de coupe) troué
    	if (pb.Mass() > 0.0) {

    		Surface     *newFace=0;
    		TopoDS_Face  newOCCFace;

    		bool to_keep=false;
    		// pour chaque surface de la nouvelle shape, on regarde si
    		// cette surface n'existe pas déjà dans une shape de référence

    		std::list<GeomEntity*>::iterator it = m_ref_entities[2].begin();
    		for(;it!=m_ref_entities[2].end() && !to_keep; it++)
    		{
    			Surface* current = dynamic_cast<Surface*>(*it);
    			CHECK_NULL_PTR_ERROR(current);

    			std::vector<GeomRepresentation*> loc_reps = current->getComputationalProperties();

    			for (uint j=0; j<loc_reps.size(); j++){
    				OCCGeomRepresentation* occ_rep =
    						dynamic_cast<OCCGeomRepresentation*>(loc_reps[j]);
    				CHECK_NULL_PTR_ERROR(occ_rep);
    				TopoDS_Face rep_face = TopoDS::Face(occ_rep->getShape());
#ifdef _DEBUG2
    				std::cout<<"  areEquals avec "<<current->getName()<<std::endl;
#endif
    				if(OCCGeomRepresentation::areEquals(F,rep_face)){
    					//std::cout<<"\t on garde"<<std::endl;
    					// on a trouve que la face existe déjà, on en conserve la référence
    					to_keep = true;
    					newFace = current;
    					newOCCFace = F;
    					m_toKeepSurfaces.push_back(current);
#ifdef _DEBUG2
    					std::cout<<"**** "<<current->getName()<<" a conserver"<<std::endl;
#endif
    				}
    			} // end for j
    		} // end for it

    		bool found_in_news = false;
    		if(!to_keep)
    		{
    			for(unsigned int k=0; k<m_newSurfaces.size() &&!found_in_news;k++)
    			{
    				Surface* current = m_newSurfaces[k];
    				std::vector<GeomRepresentation*> loc_reps = current->getComputationalProperties();
    				if (loc_reps.size() == 1){
    					OCCGeomRepresentation* occ_rep =
    							dynamic_cast<OCCGeomRepresentation*>(loc_reps[0]);
    					CHECK_NULL_PTR_ERROR(occ_rep);
        				TopoDS_Face rep_face = TopoDS::Face(occ_rep->getShape());

        				if(OCCGeomRepresentation::areEquals(F,rep_face)){
        					found_in_news = true;
        					newFace = current;
        					newOCCFace = F;
    #ifdef _DEBUG2
        					std::cout<<"**** "<<current->getName()<<" deja cree"<<std::endl;
    #endif
        				}
    				}
    			}
    		}

    		if(!to_keep && !found_in_news){
    			// on crée la shape M3D correspondante
    			newFace = EntityFactory(m_context).newOCCSurface(F);
    			newOCCFace = F;
    			m_newEntities.push_back(newFace);
    			m_newSurfaces.push_back(newFace);
#ifdef _DEBUG2
    			std::cout<<"**** "<<newFace->getName()<<" nouvelle"<<std::endl;
#endif
    		}
    		if(!found_in_news)
    		{ //car sinon on l'ajoute deux fois
    			occ_entities.push_back(newOCCFace);
    			m3d_entities.push_back(newFace);
    		}
    		else if (!to_keep && found_in_news && isAdjacent){
    			if(std::find(m3d_entities.begin(),m3d_entities.end(),newFace)==m3d_entities.end()){
    				occ_entities.push_back(newOCCFace);
    				m3d_entities.push_back(newFace);
    			}
    		}
    	}
    	else{
#ifdef _DEBUG2
    		std::cout<<"\t NULL so skipped"<<std::endl;
#endif
    	}

    } // end for i

}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::createNewVolumes(
		const TopoDS_Shape&              entity,
        std::vector<TopoDS_Shape>& occ_entities,
        std::vector<Volume*>&     m3d_entities,
        bool isAdjacent)
{
    TopTools_IndexedMapOfShape entities;
    TopExp::MapShapes(entity,TopAbs_SOLID, entities);
#ifdef _DEBUG2
    std::cout<<"**** createNewVolumes ... avec "<<entities.Extent()<<" entités"<<std::endl;
#endif
    std::list<TopoDS_Solid> sorted_entities;
    for(int i = 1; i <= entities.Extent(); i++){
    	TopoDS_Solid S = TopoDS::Solid(entities(i));
    	sorted_entities.push_back(S);
    }
    sorted_entities.sort(compareOCCSolid);

    for (std::list<TopoDS_Solid>::iterator iter=sorted_entities.begin();
        		iter!=sorted_entities.end(); ++iter)
    {
    	TopoDS_Solid V = *iter;
        Volume      *newVolume=0;
        TopoDS_Solid newOCCVolume;

        bool to_keep=false;
        // pour chaque volume de la nouvelle shape, on regarde si
        // ce volume n'existe pas déjà dans une shape de référence

        std::list<GeomEntity*>::iterator it = m_ref_entities[3].begin();
        int index=0;
        for(;it!=m_ref_entities[3].end() && !to_keep; it++)
        {
            Volume* current = dynamic_cast<Volume*>(*it);
            CHECK_NULL_PTR_ERROR(current);
            OCCGeomRepresentation* occ_rep =
                    dynamic_cast<OCCGeomRepresentation*>(current->getComputationalProperty());
            CHECK_NULL_PTR_ERROR(occ_rep);
//            TopoDS_Shape sh = occ_rep->getShape();
//            std::cout<<index++<<") "<<current->getName()<<" : "<<sh.ShapeType()<<std::endl;
//            if(sh.ShapeType()==TopAbs_COMPOUND)
//                std::cout<<"   --> COMPOUND"<<std::endl;
//            else if(sh.ShapeType()==TopAbs_COMPSOLID)
//                std::cout<<"   --> COMP SOLID"<<std::endl;
//            else if(sh.ShapeType()==TopAbs_SHELL)
//                std::cout<<"   --> SHELL"<<std::endl;
//            else if(sh.ShapeType()==TopAbs_SOLID)
//                std::cout<<"   --> SOLID"<<std::endl;

            TopoDS_Solid rep_volume = TopoDS::Solid(occ_rep->getShape());

            if(OCCGeomRepresentation::areEquals(V,rep_volume))
            {
                // on a trouve que le volume existe déjà, on en conserve la référence
                to_keep = true;
                newVolume = current;
                newOCCVolume = V;
                m_toKeepVolumes.push_back(current);
#ifdef _DEBUG2
               std::cout<<"=*= VOLUME CONSERVE "<<newVolume->getName()<<std::endl;
#endif
            }

        }
        bool found_in_news = false;
        if(!to_keep)
        {
            for(unsigned int k=0; k<m_newVolumes.size() &&!found_in_news;k++)
            {
                Volume* current = m_newVolumes[k];

                OCCGeomRepresentation* occ_rep =
                        dynamic_cast<OCCGeomRepresentation*>(current->getComputationalProperty());
                CHECK_NULL_PTR_ERROR(occ_rep);
                TopoDS_Solid rep_volume = TopoDS::Solid(occ_rep->getShape());

                if(OCCGeomRepresentation::areEquals(V,rep_volume))
                {
                    found_in_news = true;
                    newVolume = current;
                    newOCCVolume = V;
                }
            }
        }
        // On ne l'a pas trouve egale, maintenant il est possible qu'il existe un
        // volume remplissant geometriquement le meme espace mais avec un bord discretise
        // differemment, on regarde cela!!!

        if(!to_keep && !found_in_news){
            // on crée la shape M3D correspondante
            newVolume = EntityFactory(m_context).newOCCVolume(V);
            newOCCVolume= V;
            m_newEntities.push_back(newVolume);
            m_newVolumes.push_back(newVolume);
            occ_entities.push_back(newOCCVolume);
            m3d_entities.push_back(newVolume);
#ifdef _DEBUG2
            std::cout<<"=*= VOLUME NOUVEAU "<<newVolume->getName()<<std::endl;
#endif
        }
        else if( !found_in_news){
            occ_entities.push_back(newOCCVolume);
            m3d_entities.push_back(newVolume);

#ifdef _DEBUG2
            std::cout<<"=*= VOLUME DEJA CREE "<<newVolume->getName()<<std::endl;
#endif
        }
        else if (found_in_news && isAdjacent){
            if(std::find(m3d_entities.begin(),m3d_entities.end(),newVolume)==m3d_entities.end()){
                occ_entities.push_back(newOCCVolume);
                m3d_entities.push_back(newVolume);
#ifdef _DEBUG2
                std::cout<<"=*= VOLUME DEJA CREE ET ADAJCENT "<<newVolume->getName()<<std::endl;
#endif
            }
        }

    }
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::createNewAdjVolumes(
		const TopoDS_Shape&              entity,
        std::vector<TopoDS_Shape>& occ_entities,
        std::vector<Volume*>&     m3d_entities,
        bool isAdjacent)
{
    TopTools_IndexedMapOfShape entities;
    TopExp::MapShapes(entity,TopAbs_SOLID, entities);
    for(int i = 1; i <= entities.Extent(); i++)
    {
        TopoDS_Solid V = TopoDS::Solid(entities(i));
        Volume      *newVolume=0;
        TopoDS_Solid newOCCVolume;

        bool to_keep=false;
        // pour chaque volume de la nouvelle shape, on regarde si
        // ce volume n'existe pas déjà dans une shape de référence
        // ou dans une shape adj

        std::list<GeomEntity*>::iterator it = m_ref_entities[3].begin();
        for(;it!=m_ref_entities[3].end() && !to_keep; it++)
        {
            Volume* current = dynamic_cast<Volume*>(*it);
            CHECK_NULL_PTR_ERROR(current);
            OCCGeomRepresentation* occ_rep =
                    dynamic_cast<OCCGeomRepresentation*>(current->getComputationalProperty());
            CHECK_NULL_PTR_ERROR(occ_rep);
            TopoDS_Solid rep_volume = TopoDS::Solid(occ_rep->getShape());

            if(OCCGeomRepresentation::areEquals(V,rep_volume))
            {
                // on a trouve que le volume existe déjà, on en conserve la référence
                to_keep = true;
                newVolume = current;
                newOCCVolume = V;
                m_toKeepVolumes.push_back(current);
#ifdef _DEBUG2
                std::cout<<"=*= VOLUME CONSERVE "<<newVolume->getName()<<std::endl;
#endif
            }

        }

        //Les volumes adjacents sont comparés uniquement sur le volume
        it = m_adj_entities[3].begin();
        for(;it!=m_adj_entities[3].end() && !to_keep; it++)
        {
            Volume* current = dynamic_cast<Volume*>(*it);
            CHECK_NULL_PTR_ERROR(current);
#ifdef _DEBUG2
            std::cout<<"=> adj compare: "<<current->getName()<<std::endl;
#endif
            OCCGeomRepresentation* occ_rep =
                    dynamic_cast<OCCGeomRepresentation*>(current->getComputationalProperty());
            CHECK_NULL_PTR_ERROR(occ_rep);
            TopoDS_Solid rep_volume = TopoDS::Solid(occ_rep->getShape());

            if(OCCGeomRepresentation::contains(V,rep_volume))
            {
                // on a trouve que le volume existe déjà, on en conserve la référence
                to_keep = true;
                newVolume = current;
                newOCCVolume = V;
                m_toKeepVolumes.push_back(current);
#ifdef _DEBUG2
                std::cout<<"=*= VOLUME ADJ CONSERVE "<<newVolume->getName()<<std::endl;
#endif
            }

        }
        bool found_in_news = false;
        if(!to_keep)
        {
            for(unsigned int k=0; k<m_newVolumes.size() &&!found_in_news;k++)
            {
                Volume* current = m_newVolumes[k];

                OCCGeomRepresentation* occ_rep =
                        dynamic_cast<OCCGeomRepresentation*>(current->getComputationalProperty());
                CHECK_NULL_PTR_ERROR(occ_rep);
                TopoDS_Solid rep_volume = TopoDS::Solid(occ_rep->getShape());

                if(OCCGeomRepresentation::areEquals(V,rep_volume))
                {
                    found_in_news = true;
                    newVolume = current;
                    newOCCVolume = V;
                }
            }
        }
        // On ne la pas trouve egale, maintenant il est possible qu'il existe un
        // volume remplissant geometriquement le meme espace mais avec un bord discretise
        // differemment, on regarde cela!!!

        if(!to_keep && !found_in_news){
            // on crée la shape M3D correspondante
            newVolume = EntityFactory(m_context).newOCCVolume(V);
            newOCCVolume= V;
            m_newEntities.push_back(newVolume);
            m_newVolumes.push_back(newVolume);
            occ_entities.push_back(newOCCVolume);
            m3d_entities.push_back(newVolume);
#ifdef _DEBUG2
            std::cout<<"=*= VOLUME NOUVEAU "<<newVolume->getName()<<std::endl;
#endif
        }
        else if( !found_in_news){
            occ_entities.push_back(newOCCVolume);
            m3d_entities.push_back(newVolume);
#ifdef _DEBUG2

            std::cout<<"=*= VOLUME DEJA CREE "<<newVolume->getName()<<std::endl;
#endif
        }
        else if (found_in_news && isAdjacent){
            if(std::find(m3d_entities.begin(),m3d_entities.end(),newVolume)==m3d_entities.end()){
                occ_entities.push_back(newOCCVolume);
                m3d_entities.push_back(newVolume);
#ifdef _DEBUG2

                std::cout<<"=*= VOLUME DEJA CREE ET ADAJCENT "<<newVolume->getName()<<std::endl;
#endif
            }
        }

    }
}
/*----------------------------------------------------------------------------*/
GeomEntity* GeomModificationBaseClass::
getGeomEntity(TopoDS_Shape &loc_shape,
			 std::vector<TopoDS_Shape>& OCC_vertices,
			 std::vector<TopoDS_Shape>& OCC_curves,
			 std::vector<TopoDS_Shape>& OCC_surfaces,
			 std::vector<TopoDS_Shape>& OCC_volumes,
    		 std::vector<Vertex*>& M3D_vertices,
			 std::vector<Curve*>& M3D_curves,
			 std::vector<Surface*>& M3D_surfaces,
			 std::vector<Volume*>& M3D_volumes)
{
	if (loc_shape.ShapeType () == TopAbs_SOLID ){
		for (uint i=0; i<OCC_volumes.size(); i++)
			if (OCC_volumes[i].IsEqual(loc_shape))
				return M3D_volumes[i];
	}
	else if (loc_shape.ShapeType () == TopAbs_FACE ){
		for (uint i=0; i<OCC_surfaces.size(); i++)
			if (OCC_surfaces[i].IsEqual(loc_shape))
				return M3D_surfaces[i];
	}
	else if (loc_shape.ShapeType () == TopAbs_EDGE ){
		for (uint i=0; i<OCC_curves.size(); i++)
			if (OCC_curves[i].IsSame(loc_shape)) // étrange que IsEqual ne soit pas vrai !
				return M3D_curves[i];
	}
	else if (loc_shape.ShapeType () == TopAbs_VERTEX ){
		for (uint i=0; i<OCC_vertices.size(); i++)
			if (OCC_vertices[i].IsEqual(loc_shape))
				return M3D_vertices[i];
	}
	else {
		std::cout<<" (autre type)";
	}

	// pas trouvé ...
	return 0;
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::checkValidity(std::set<GeomEntity*>& entities)
{
    std::set<GeomEntity*>::iterator it  = entities.begin();
    std::set<GeomEntity*>::iterator ite = entities.end();

    for(;it!=ite;it++){
        GeomEntity* ei = *it;
        int dimi = ei->getDim();

        if(dimi<1){
            std::vector<Curve*> curves;
            ei->get(curves);
            for(unsigned int i=0;i<curves.size();i++){
                if(entities.find(curves[i])==ite){
                    TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message << "Une entité est connectée à des courbes ("<<curves[i]->getName()<<" entre autre) qui ne sont pas modifiées";
                    throw TkUtil::Exception (message);
                }
            }
        }
        if(dimi<2){
            std::vector<Surface*> surfs;
            ei->get(surfs);
            for(unsigned int i=0;i<surfs.size();i++){
                if(entities.find(surfs[i])==ite){
                    TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message << "Une entité est connectée à des surfaces ("<<surfs[i]->getName()<<" entre autre) qui ne sont pas modifiées";
                    throw TkUtil::Exception (message);
                }
            }
        }
        if(dimi<3){
            std::vector<Volume*> vols;
            ei->get(vols);
            for(unsigned int i=0;i<vols.size();i++){
                if(entities.find(vols[i])==ite){
                    TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message << "Une entité est connectée à des volumes ("<<vols[i]->getName()<<" entre autre) qui ne sont pas modifiées";
                    throw TkUtil::Exception (message);
                }
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void GeomModificationBaseClass::
buildInitialSet(std::set<GeomEntity*>& init_entities, bool force)
{
    // on utilise un set pour ne pas ajouter plusieurs fois la meme cellule
    // dans les entites de references (pb sinon à la destruction de la commande)

    std::vector<GeomEntity*> ref_ge;
    ref_ge.insert(ref_ge.end(),m_init_entities.begin(),m_init_entities.end());
    for(unsigned int i=0;i<ref_ge.size();i++){
        GeomEntity* ei=ref_ge[i];
        int dimi = ei->getDim();
        // ei is always added
        init_entities.insert(ei);
        if(dimi>=1 && (ei->needLowerDimensionalEntityModification() || force)){ //vertices must be added to
            std::vector<Vertex*> vertices;
            ei->get(vertices);
            for(unsigned int iv=0;iv<vertices.size();iv++)
                init_entities.insert(vertices[iv]);

        }
        if(dimi>=2 && (ei->needLowerDimensionalEntityModification() || force)){//curves to be added
            std::vector<Curve*> curves;
            ei->get(curves);
            //std::cerr<<"Curves: "<<curves.size()<<std::endl;

            for(unsigned int ic=0;ic<curves.size();ic++)
                init_entities.insert(curves[ic]);


        }
        if(dimi>=3 && (ei->needLowerDimensionalEntityModification() || force)){
            std::vector<Surface*> surfs;
            ei->get(surfs);
            for(unsigned int ic=0;ic<surfs.size();ic++)
                init_entities.insert(surfs[ic]);
        }
    }

}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/

