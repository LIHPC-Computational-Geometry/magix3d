/*----------------------------------------------------------------------------*/
/** \file Edge.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 19/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <vector>
#include <string.h>
/*----------------------------------------------------------------------------*/
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/Edge.h"
#include "Topo/CoFace.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Topo/EdgeMeshingPropertyGlobalInterpolate.h"
#include "Topo/EdgeMeshingPropertyTabulated.h"
#include "Topo/CommandEditTopo.h"
#include "Topo/TopoHelper.h"

#include "Mesh/CommandCreateMesh.h"
#include "Mesh/MeshImplementation.h"
#include <Mesh/MeshItf.h>

#include "Utils/Common.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Utils/SerializedRepresentation.h"
#include "Utils/MgxException.h"

#include "Internal/InfoCommand.h"
#include "Internal/InternalPreferences.h"

#include "Geom/GeomEntity.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomModificationBaseClass.h"

#include "Group/Group1D.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/Mutex.h>
/*----------------------------------------------------------------------------*/
// OCC
#include <Standard_Failure.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pln.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Face.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepAlgoAPI_Section.hxx>

#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>

/*----------------------------------------------------------------------------*/
// protection pour les appels OCC et pour la création des points
static TkUtil::Mutex					entityFactoryMutex;
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
const char* CoEdge::typeNameTopoCoEdge = "TopoCoEdge";
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MEMORY
CoEdge::
CoEdge(Internal::Context& ctx,
        Topo::CoEdgeMeshingProperty* emp,
        Topo::Vertex* v1, Topo::Vertex* v2)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoCoEdge),
        ctx.newDisplayProperties(Utils::Entity::TopoCoEdge))
, m_topo_property(new CoEdgeTopoProperty())
, m_save_topo_property (0)
, m_mesh_property(emp?emp->clone():0)
, m_save_mesh_property(0)
, m_mesh_data(new CoEdgeMeshingData())
, m_save_mesh_data(0)
{
    m_topo_property->getVertexContainer().add(v1);
    m_topo_property->getVertexContainer().add(v2);

    // association remontante
    v1->addCoEdge(this);
    v2->addCoEdge(this);

    // om met une discrétisation par défaut
    if (m_mesh_property == 0){
        EdgeMeshingPropertyUniform loc_emp(ctx.getLocalTopoManager().getDefaultNbMeshingEdges());
        m_mesh_property = loc_emp.clone();
    }

	ctx.newGraphicalRepresentation (*this);

#ifdef _DEBUG_MEMORY
    std::cout<<"CoEdge::CoEdge() de "<<getName()<<", avec m_mesh_property en "<<m_mesh_property->getMeshLawName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
CoEdge::
CoEdge(const CoEdge& v)
: TopoEntity(v.getContext(), 0, 0)
, m_topo_property(0)
, m_save_topo_property (0)
, m_mesh_property(0)
, m_save_mesh_property(0)
, m_mesh_data(0)
, m_save_mesh_data(0)
{
    MGX_FORBIDDEN("Constructeur de copie");
}
/*----------------------------------------------------------------------------*/
CoEdge::
~CoEdge()
{
#ifdef _DEBUG
    if (m_topo_property == 0 ||m_mesh_property == 0 || m_mesh_data == 0)
        std::cerr<<"Serait-on passé deux fois dans le destructeur ? pour "<<getName()<<std::endl;
#endif

#ifdef _DEBUG_MEMORY
    std::cout<<"delete de m_mesh_property pour "<<getName()<<" en "<<m_mesh_property<<std::endl;
#endif

    delete m_topo_property;
    delete m_mesh_property;
    delete m_mesh_data;

#ifdef _DEBUG
    m_topo_property = 0;
    m_mesh_property = 0;
    m_mesh_data = 0;

   if (m_save_topo_property)
        std::cerr<<"La sauvegarde du CoEdgeTopoProperty a été oubliée pour "<<getName()<<std::endl;

    if (m_save_mesh_property)
    	delete m_save_mesh_property;
    	//std::cerr<<"La sauvegarde du CoEdgeMeshingProperty a été oubliée pour "<<getName()<<std::endl;

    if (m_save_mesh_data)
    	std::cerr<<"La sauvegarde du CoEdgeMeshingData a été oubliée pour "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CoEdge::
replace(Topo::Vertex* v1, Topo::Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd)
{
//    // on ne fait rien en cas d'arête en cours de destruction
//    if (isDestroyed())
//        return;
#ifdef _DEBUG2
    std::cout<<"CoEdge::replace("<<v1->getName()<<" par "<<v2->getName()<<") dans : "<<std::endl;
    std::cout<<*this<<std::endl;
#endif

    // transmet aux Edges
	if (propagate_up)
		for (uint j=0; j<getNbEdges(); j++)
			getEdge(j)->replace(v1, v2, propagate_up, propagate_down, icmd);

    bool found = false;
    for (uint i=0; i<getNbVertices(); i++)
        if (v1 == getVertex(i)){
            found = true;

            // sauvegarde pour permettre le undo
            v1->saveVertexTopoProperty(icmd);
            v2->saveVertexTopoProperty(icmd);

            saveCoEdgeTopoProperty(icmd);
            m_topo_property->getVertexContainer().set(i,v2);
            v1->removeCoEdge(this);
            v2->addCoEdge(this);
        }

    if (propagate_up && !found)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (sommet non trouvé), avec CoEdge::replace", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CoEdge::
merge(CoEdge* ed, Internal::InfoCommand* icmd)
{
//	std::cout<<"CoEdge::merge entre this: "<<*this<<std::endl;
//	std::cout<<"et autre CoEdge : "<<*ed<<std::endl;


    if (ed == this)
        return;

    // on remplace ed dans les arêtes associées à ed
    // On travaille sur une copie car il y a une mise à jour en même temps des arêtes
    std::vector<Edge* > edges;
    ed->getEdges(edges);

    for (std::vector<Edge* >::iterator iter=edges.begin();
            iter != edges.end(); ++iter)
        (*iter)->replace(ed, this, icmd);

    // récupère l'association côté ed si celle de this est dédtruite, ou si la dimension de la geom est inférieure
    if (getGeomAssociation() && ed->getGeomAssociation()
    		&& ((getGeomAssociation()->isDestroyed() && !ed->getGeomAssociation()->isDestroyed())
    				|| (getGeomAssociation()->getDim() > ed->getGeomAssociation()->getDim()))){
    	saveTopoProperty();
    	setGeomAssociation(ed->getGeomAssociation());
    }

    ed->free(icmd);
}
/*----------------------------------------------------------------------------*/
void CoEdge::
collapse(Internal::InfoCommand* icmd)
{
    //std::cout<<"CoEdge::collapse pour : "<<*this<<std::endl;

    if (getNbVertices() != 2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne CoEdge::collapse avec autre chose que 2 sommets", TkUtil::Charset::UTF_8));
    if (getVertex(0) == getVertex(1))
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne CoEdge::collapse avec 2 sommets identiques", TkUtil::Charset::UTF_8));

    setDestroyed(true);
    // on met au courant la commande de la destruction
    if (icmd)
        icmd->addTopoInfoEntity(this,Internal::InfoCommand::DELETED);

    // on supprime getVertex(1)
    getVertex(0)->merge(getVertex(1), icmd);

    free(icmd);
}
/*----------------------------------------------------------------------------*/
void CoEdge::
setDestroyed(bool b)
{
	if (b && !isFinished() && isMeshed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "On ne peut pas détruire une arête sans avoir détruit le maillage."
                << " Ce que l'on tente de faire pour l'arête "<<getName();
        throw TkUtil::Exception (message);
    }

	if (isDestroyed() == b)
		return;

	Utils::Container<Group::Group1D>& groups = getGroupsContainer();
	if (b)
		for (uint i=0; i<groups.size(); i++){
			Group::Group1D* gr = groups.get(i);
			gr->remove(this);
		}
	else
		for (uint i=0; i<groups.size(); i++){
			Group::Group1D* gr = groups.get(i);
			gr->add(this);
		}

	   TopoEntity::setDestroyed(b);
}
/*----------------------------------------------------------------------------*/
void CoEdge::
free(Internal::InfoCommand* icmd)
{
	saveCoEdgeTopoProperty(icmd);

    // on met au courant la commande de la destruction
    if (icmd)
        icmd->addTopoInfoEntity(this,Internal::InfoCommand::DELETED);

    setDestroyed(true);

    // on retire la relation avec la géométrie
    if (icmd && getGeomAssociation()
             && icmd->getTopoInfoEntity()[this] != Internal::InfoCommand::NONE){
        saveTopoProperty();
        setGeomAssociation(0);
    }

    // on supprime l'arête des relations des sommets vers les arêtes communes
    for (uint i=0; i<getNbVertices(); i++) {
        getVertex(i)->saveVertexTopoProperty(icmd);
        getVertex(i)->removeCoEdge(this, false);
    }

    // idem entre arêtes et arêtes communes
    for (uint i=0; i<getNbEdges(); i++) {
        getEdge(i)->saveEdgeTopoProperty(icmd);
        getEdge(i)->removeCoEdge(this, false);
    }

    clearDependancy();
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_SPLIT
Topo::Vertex* CoEdge::
split(uint nbMeshingEdges, Internal::InfoCommand* icmd)
{
    // cas du découpage en 2 arêtes
#ifdef _DEBUG_SPLIT
    std::cout<<"CoEdge::split("<<nbMeshingEdges<<") avec comme arête commune : "<<*this<<std::endl;
#endif

    if (nbMeshingEdges == 0)
        return getVertex(0);
    else if (nbMeshingEdges == getNbMeshingEdges())
        return getVertex(1);
    else {
        getVertex(0)->saveVertexTopoProperty(icmd);
        getVertex(1)->saveVertexTopoProperty(icmd);

        // utilisation des points placés suivant la projection et la discrétisation
		std::vector<Utils::Math::Point> points;
		Topo::Vertex* new_vertex = 0;

		try {
			getPoints(points);
			new_vertex = new Topo::Vertex(getContext(), points[nbMeshingEdges]);
		}
		catch (TkUtil::Exception& exc){
			// cas d'une arête en dehors de la surface,
			// utilisation d'une méthode plus ancienne mais qui ne tient pas compte de la discrétisation

			double ratio = ((double)nbMeshingEdges) / ((double)getNbMeshingEdges());
			std::vector<Utils::Math::Point> points;

			if (getGeomAssociation()){
				Geom::Curve* curve = dynamic_cast<Geom::Curve*> (getGeomAssociation());
				if (curve){

					curve->getParametricsPoints(getVertex(0)->getCoord(),
							getVertex(1)->getCoord(),
							1, &ratio, points);
				}
				else {
					points.push_back(getVertex(0)->getCoord() + (getVertex(1)->getCoord() - getVertex(0)->getCoord()) * ratio);
					getGeomAssociation()->project(points[0]);
				}
			}
			else {
				points.push_back(getVertex(0)->getCoord() + (getVertex(1)->getCoord() - getVertex(0)->getCoord()) * ratio);
			}

			new_vertex = new Topo::Vertex(getContext(), points[0]);
		} // end catch
        
        if (getGeomAssociation())
            new_vertex->setGeomAssociation(getGeomAssociation());

#ifdef _DEBUG_SPLIT
    std::cout<<" création du sommet "<<new_vertex->getName()<<std::endl;
#endif

        if (icmd)
            icmd->addTopoInfoEntity(new_vertex, Internal::InfoCommand::CREATED);

        // création des 2 nouvelles arêtes et destruction de this
        std::vector<CoEdge*> newCoEdges = split(new_vertex, nbMeshingEdges, icmd, true);

        return new_vertex;
    }
}
/*----------------------------------------------------------------------------*/
std::vector<CoEdge*> CoEdge::split(Topo::Vertex* vtx, uint nbMeshingEdges, Internal::InfoCommand* icmd, bool sortCoEdges)
{
    // les 2 nouvelles discrétisations
    CoEdgeMeshingProperty *cemp1, *cemp2;
    if (getMeshingProperty()->getMeshLaw()!=CoEdgeMeshingProperty::interpolate){
    	// on ne conserve pas les interpolations
    	cemp1 = getMeshingProperty()->clone();
    	cemp2 = getMeshingProperty()->clone();
    	cemp1->setNbEdges(nbMeshingEdges);
        cemp2->setNbEdges(getNbMeshingEdges()-nbMeshingEdges);
    }
    else {
    	cemp1 = new EdgeMeshingPropertyUniform(nbMeshingEdges);
    	cemp2 = new EdgeMeshingPropertyUniform(getNbMeshingEdges()-nbMeshingEdges);
    }


    // création de 2 nouvelles arêtes communes
    Topo::CoEdge* coedge1 = new Topo::CoEdge(getContext(), cemp1, getVertex(0), vtx);
    Topo::CoEdge* coedge2 = new Topo::CoEdge(getContext(), cemp2, vtx,   getVertex(1));
    if (icmd){
        icmd->addTopoInfoEntity(coedge1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(coedge2, Internal::InfoCommand::CREATED);
    }

    // On reprend la même projection
    if (getGeomAssociation()){
        coedge1->setGeomAssociation(getGeomAssociation());
        coedge2->setGeomAssociation(getGeomAssociation());
    }

    Utils::Container<Group::Group1D>& groups = getGroupsContainer();
    for (uint i=0; i<groups.size(); i++){
    	Group::Group1D* gr = groups.get(i);
    	gr->add(coedge1);
    	gr->add(coedge2);
    	coedge1->getGroupsContainer().add(gr);
    	coedge2->getGroupsContainer().add(gr);
    }

    delete cemp1;
    delete cemp2;

    // on ajoute les 2 arêtes
    for (uint i=0; i<getNbEdges(); i++) {
    	uint ratio = getEdge(i)->getRatio(this);
    	// cas où il faut reporter la semi conformité sur les nouvelles arêtes communes
    	if (ratio != 1){
    		getEdge(i)->saveEdgeMeshingProperty(icmd);
    		getEdge(i)->setRatio(coedge1, ratio);
    		getEdge(i)->setRatio(coedge2, ratio);
    	}
        getEdge(i)->saveEdgeTopoProperty(icmd);
        getEdge(i)->addCoEdge(coedge1);
        getEdge(i)->addCoEdge(coedge2);

        coedge1->addEdge(getEdge(i));
        coedge2->addEdge(getEdge(i));
    }

    // on met de côté la liste des Edges touchées
    std::vector<Edge* > edges;
    getEdges(edges);

    // on supprime cette arête (this)
    free(icmd);

    // remet les CoEdges dans l'ordre / arête
    if (sortCoEdges)
        for (std::vector<Edge* >::iterator iter1 = edges.begin();
                iter1 != edges.end(); ++iter1)
            (*iter1)->sortCoEdges();

#ifdef _DEBUG_SPLIT
    std::cout<<" les 2 nouvelles arêtes communes :\n";
    std::cout<<"   coedge1 : "<<*coedge1;
    std::cout<<"   coedge2 : "<<*coedge2;
#endif

    std::vector<CoEdge*> newCoEdges;
    newCoEdges.push_back(coedge1);
    newCoEdges.push_back(coedge2);

    return newCoEdges;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::Vertex*> CoEdge::split(uint nbMeshingEdges1, uint nbMeshingEdges2, Internal::InfoCommand* icmd)
{
    // cas du découpage en 3 arêtes
#ifdef _DEBUG_SPLIT
    std::cout<<"CoEdge::split("<<nbMeshingEdges1<<", "<<nbMeshingEdges2<<") avec comme arête commune : "<<*this<<std::endl;
#endif

    getVertex(0)->saveVertexTopoProperty(icmd);
    getVertex(1)->saveVertexTopoProperty(icmd);

    Topo::Vertex* new_vertex1 = 0;
    Topo::Vertex* new_vertex2 = 0;

    std::vector<Utils::Math::Point> points;

	try {
	    // utilisation des points placés suivant la projection et la discrétisation
		getPoints(points);

		new_vertex1 = new Topo::Vertex(getContext(), points[nbMeshingEdges1]);
		new_vertex2 = new Topo::Vertex(getContext(), points[nbMeshingEdges2]);
	}
	catch (TkUtil::Exception& exc){
		// cas d'une arête en dehors de la surface,
		// utilisation d'une méthode plus ancienne mais qui ne tient pas compte de la discrétisation

		double* l_ratios = new double[2];
		//Utils::Math::Point* l_points = new Utils::Math::Point[2];
		l_ratios[0] = ((double)nbMeshingEdges1) / ((double)getNbMeshingEdges());
		l_ratios[1] = ((double)nbMeshingEdges2) / ((double)getNbMeshingEdges());

		if (getGeomAssociation()){
			Geom::Curve* curve = dynamic_cast<Geom::Curve*> (getGeomAssociation());
			if (curve){

				curve->getParametricsPoints(getVertex(0)->getCoord(),
						getVertex(1)->getCoord(),
						2, l_ratios, points);
			}
			else{
				points.push_back(getVertex(0)->getCoord()
						+ (getVertex(1)->getCoord() - getVertex(0)->getCoord()) * l_ratios[0]);
				points.push_back(getVertex(0)->getCoord()
						+ (getVertex(1)->getCoord() - getVertex(0)->getCoord()) * l_ratios[1]);
				getGeomAssociation()->project(points[0]);
				getGeomAssociation()->project(points[1]);
			}
		}
		else {
			points.push_back(getVertex(0)->getCoord()
					+ (getVertex(1)->getCoord() - getVertex(0)->getCoord()) * l_ratios[0]);
			points.push_back(getVertex(0)->getCoord()
					+ (getVertex(1)->getCoord() - getVertex(0)->getCoord()) * l_ratios[1]);
		}

		delete [] l_ratios;

		new_vertex1 = new Topo::Vertex(getContext(), points[0]);
		new_vertex2 = new Topo::Vertex(getContext(), points[1]);
	} // end catch

#ifdef _DEBUG_SPLIT
    std::cout<<" création des 2 sommets "<<new_vertex1->getName()<<" et " <<new_vertex2->getName()<<std::endl;
    std::cout<<"   new_vertex1 : "<<*new_vertex1<<std::endl;
    std::cout<<"   new_vertex2 : "<<*new_vertex2<<std::endl;
#endif

    if (icmd){
        icmd->addTopoInfoEntity(new_vertex1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(new_vertex2, Internal::InfoCommand::CREATED);
    }

    // les 3 nouvelles discrétisations
    bool permutation = false;
    CoEdgeMeshingProperty *cemp1, *cemp2, *cemp3;
    if (getMeshingProperty()->getMeshLaw()!=CoEdgeMeshingProperty::interpolate){
    	cemp1 = getMeshingProperty()->clone();
    	cemp2 = getMeshingProperty()->clone();
    	cemp3 = getMeshingProperty()->clone();
    }
    else {
    	cemp1 = new EdgeMeshingPropertyUniform(1);
    	cemp2 = new EdgeMeshingPropertyUniform(1);
    	cemp3 = new EdgeMeshingPropertyUniform(1);
    }
    if (nbMeshingEdges1 < nbMeshingEdges2){
        cemp1->setNbEdges(nbMeshingEdges1);
        cemp2->setNbEdges(nbMeshingEdges2 - nbMeshingEdges1);
        cemp3->setNbEdges(getNbMeshingEdges() - nbMeshingEdges2);
    }
    else {
        cemp1->setNbEdges(nbMeshingEdges2);
        cemp2->setNbEdges(nbMeshingEdges1 - nbMeshingEdges2);
        cemp3->setNbEdges(getNbMeshingEdges() - nbMeshingEdges1);
        permutation = true;
    }

    // création de 3 nouvelles arêtes communes
    Topo::CoEdge *coedge1, *coedge2, *coedge3;
    if (!permutation){
        coedge1 = new Topo::CoEdge(getContext(), cemp1, getVertex(0), new_vertex1);
        coedge2 = new Topo::CoEdge(getContext(), cemp2, new_vertex1,  new_vertex2);
        coedge3 = new Topo::CoEdge(getContext(), cemp3, new_vertex2,  getVertex(1));
    }
    else {
        coedge1 = new Topo::CoEdge(getContext(), cemp1, getVertex(0), new_vertex2);
        coedge2 = new Topo::CoEdge(getContext(), cemp2, new_vertex2,  new_vertex1);
        coedge3 = new Topo::CoEdge(getContext(), cemp3, new_vertex1,  getVertex(1));
    }

    if (icmd){
        icmd->addTopoInfoEntity(coedge1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(coedge2, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(coedge3, Internal::InfoCommand::CREATED);
    }

    // On reprend la même projection
    if (getGeomAssociation()){
        coedge1->setGeomAssociation(getGeomAssociation());
        coedge2->setGeomAssociation(getGeomAssociation());
        coedge3->setGeomAssociation(getGeomAssociation());
        new_vertex1->setGeomAssociation(getGeomAssociation());
        new_vertex2->setGeomAssociation(getGeomAssociation());
    }

    Utils::Container<Group::Group1D>& groups = getGroupsContainer();
    for (uint i=0; i<groups.size(); i++){
    	Group::Group1D* gr = groups.get(i);
    	gr->add(coedge1);
    	gr->add(coedge2);
    	gr->add(coedge3);
    	coedge1->getGroupsContainer().add(gr);
    	coedge2->getGroupsContainer().add(gr);
    	coedge3->getGroupsContainer().add(gr);
    }

    delete cemp1;
    delete cemp2;
    delete cemp3;

    // on ajoute les 3 arêtes
    for (uint i=0; i<getNbEdges(); i++) {
    	uint ratio = getEdge(i)->getRatio(this);
    	// cas où il faut reporter la semi conformité sur les nouvelles arêtes communes
    	if (ratio != 1){
    		getEdge(i)->saveEdgeMeshingProperty(icmd);
    		getEdge(i)->setRatio(coedge1, ratio);
    		getEdge(i)->setRatio(coedge2, ratio);
    		getEdge(i)->setRatio(coedge3, ratio);
    	}

        getEdge(i)->saveEdgeTopoProperty(icmd);
        getEdge(i)->addCoEdge(coedge1);
        getEdge(i)->addCoEdge(coedge2);
        getEdge(i)->addCoEdge(coedge3);

        coedge1->addEdge(getEdge(i));
        coedge2->addEdge(getEdge(i));
        coedge3->addEdge(getEdge(i));
    }

    // on met de côté la liste des Edges touchées
    std::vector<Edge* > edges;
    getEdges(edges);

    // on supprime celle-ci (this)
    free(icmd);

    // remet les CoEdges dans l'ordre / arête
    for (std::vector<Edge* >::iterator iter1 = edges.begin();
            iter1 != edges.end(); ++iter1)
        (*iter1)->sortCoEdges();

#ifdef _DEBUG_SPLIT
    std::cout<<" les 3 nouvelles arêtes communes :\n";
    std::cout<<"   coedge1 : "<<*coedge1;
    std::cout<<"   coedge2 : "<<*coedge2;
    std::cout<<"   coedge3 : "<<*coedge3;
    std::cout<<std::endl;
#endif

    std::vector<Topo::Vertex*> newVertices;
    newVertices.push_back(new_vertex1);
    newVertices.push_back(new_vertex2);

    return newVertices;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::Vertex*> CoEdge::
split(std::vector<uint> nbMeshingEdges, Internal::InfoCommand* icmd)
{
//#ifdef _DEBUG_SPLIT
//    std::cout<<"CoEdge::split([";
//    for (uint i=0; i<nbMeshingEdges.size(); i++){
//    	if (i)
//    		std::cout<<", ";
//    	std::cout<<nbMeshingEdges[i];
//    }
//    std::cout<<"]) avec comme arête commune : "<<*this<<std::endl;
//#endif

    if (nbMeshingEdges.size() != 1 && nbMeshingEdges.size() != 2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne CoEdge::split, cela ne doit être appelé que pour 1 ou 2 sommets à créer", TkUtil::Charset::UTF_8));

    // le nombre de sommets qu'il reste à créer
    uint nbVerticesToCreate = nbMeshingEdges.size();

    // on fait un pré-traitement pour les cas extrémité
    std::vector<Topo::Vertex*> newVertices;
    newVertices.resize(nbMeshingEdges.size());
    if (nbMeshingEdges[0] == 0){
        newVertices[0] = getVertex(0);
        nbVerticesToCreate--;
    }
    else if (nbMeshingEdges[0] == getNbMeshingEdges()){
        newVertices[0] = getVertex(1);
        nbVerticesToCreate--;
    }
    else
        newVertices[0] = 0;

    if (nbMeshingEdges.size() == 2){
        if (nbMeshingEdges[1] == 0){
            newVertices[1] = getVertex(0);
            nbVerticesToCreate--;
        }
        else if (nbMeshingEdges[1] == getNbMeshingEdges()){
            newVertices[1] = getVertex(1);
            nbVerticesToCreate--;
        }
        else
            newVertices[1] = 0;
    }

    if (nbVerticesToCreate == 1) {
        if (nbMeshingEdges.size() == 1 || (nbMeshingEdges.size() == 2 && newVertices[0] == 0))
            newVertices[0] = split(nbMeshingEdges[0], icmd);
        else if (nbMeshingEdges.size() == 2 && newVertices[1] == 0)
            newVertices[1] = split(nbMeshingEdges[1], icmd);
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne CoEdge::split, 1", TkUtil::Charset::UTF_8));
    }
    else if (nbVerticesToCreate == 2){
        newVertices = split(nbMeshingEdges[0], nbMeshingEdges[1], icmd);
    }

    return newVertices;
}
/*----------------------------------------------------------------------------*/
Topo::Vertex* CoEdge::getOppositeVertex(Topo::Vertex* v) const
{
    if (v == getVertex(0)){
        return getVertex(1);
    }
    else if (v == getVertex(1)){
        return getVertex(0);
    }
    else{
        std::cerr <<"CoEdge::getOppositeVertex("<<v->getName()<<") dans "<<*this;
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (sommet non trouvé), avec CoEdge::getOppositeVertex", TkUtil::Charset::UTF_8));
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
void CoEdge::
getBlocks(std::vector<Block* >& blocks) const
{
    std::list<Topo::Block*> l_b;

    std::vector<Edge* > local_edges;
    getEdges(local_edges);

    for (std::vector<Edge* >::iterator iter1 = local_edges.begin();
    		iter1 != local_edges.end(); ++iter1){
    	std::vector<CoFace* > loc_cofaces;
    	(*iter1)->getCoFaces(loc_cofaces);

    	for (std::vector<CoFace* >::iterator iter2 = loc_cofaces.begin();
    			iter2 != loc_cofaces.end(); ++iter2){
    		std::vector<Face* > loc_faces;
    		(*iter2)->getFaces(loc_faces);

    		for (std::vector<Face* >::iterator iter3 = loc_faces.begin();
    				iter3 != loc_faces.end(); ++iter3){
    			std::vector<Block* > loc_bl;
    			(*iter3)->getBlocks(loc_bl);

    			l_b.insert(l_b.end(), loc_bl.begin(), loc_bl.end());
    		}
    	}
    }

    l_b.sort(Utils::Entity::compareEntity);
    l_b.unique();

    blocks.insert(blocks.end(),l_b.begin(),l_b.end());
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_REPRESENTATION
void CoEdge::
getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    // on ne tient pas compte du shrink pour les coedges
#ifdef _DEBUG_REPRESENTATION
    std::cout <<"CoEdge::getRepresentation pour coedge "<<getName()<<", type "<<dr.getType()<<", shrink "<<dr.getShrink()<<std::endl;
#endif

    if (dr.getDisplayType()!= Utils::DisplayRepresentation::DISPLAY_TOPO)
        throw TkUtil::Exception (TkUtil::UTF8String ("Invalid display type entity", TkUtil::Charset::UTF_8));

    if (isDestroyed() && (true == checkDestroyed)){
#ifdef _DEBUG_REPRESENTATION
    	std::cout <<"Arête "<<getName()<<" DETRUITE !!!"<<std::endl;
#endif
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Interdit d'afficher une entité détruite ("<<getName()<<")";
        throw TkUtil::Exception(messErr);
    }

    TopoDisplayRepresentation* tdr =
            dynamic_cast<TopoDisplayRepresentation*>(&dr);
    CHECK_NULL_PTR_ERROR(tdr);
    double shrink = tdr->getShrink();

    std::vector<Utils::Math::Point>& points = tdr->getPoints();
    std::vector<size_t>& indices = tdr->getCurveDiscretization();

    points.clear();
    indices.clear();

    // NB: pas d'affichage dans le cas d'une arête avec un seul sommet
    if (getNbVertices() < 2)
        return;

    if (tdr->hasRepresentation(Utils::DisplayRepresentation::WIRE)){

    	if (tdr->hasRepresentation(Utils::DisplayRepresentation::MINIMUMWIRE)
    			&& !tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWMESHSHAPE)){
    		points.push_back(getVertex(0)->getCoord());
    		points.push_back(getVertex(1)->getCoord());
    		indices.push_back(0);
    		indices.push_back(1);
    	} else {
    		// obtention des points avec ou sans projection
    		if (tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWMESHSHAPE))
    			getPoints(points);
    		else
    			getPoints(m_mesh_property, points, false);
        	for (uint i=0; i<m_mesh_property->getNbEdges(); i++){
        		indices.push_back(i);
        		indices.push_back(i+1);
        	}
    	}
    	if (shrink!=0){
    		Utils::Math::Point barycentre;
    		for (uint i=0; i<points.size(); i++)
    			barycentre+=points[i];
    		barycentre/=(double)points.size();
    		for (uint i=0; i<points.size(); i++)
    			points[i] = barycentre + (points[i] - barycentre) * shrink;

    	} // end if (shrink!=0)

    } // end if (tdr->hasRepresentation(Utils::DisplayRepresentation::WIRE))

    if (tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWASSOCIATION) && getGeomAssociation()){
        // on ajoute une flêche entre l'arête et la géométrie
#ifdef _DEBUG_REPRESENTATION
        std::cout<<"  SHOWASSOCIATION -> vers "<<getGeomAssociation()->getName()<<std::endl;
#endif
        Utils::Math::Point pt1 = (getVertex(0)->getCoord() + getVertex(1)->getCoord() ) / 2;
#ifdef _DEBUG_REPRESENTATION
        std::cout<<"  pt1 "<<pt1<<std::endl;
#endif
        Utils::Math::Point pt2;
        getGeomAssociation()->project(pt1, pt2);
#ifdef _DEBUG_REPRESENTATION
        std::cout<<"  pt2 "<<pt2<<std::endl;
#endif
        std::vector<Utils::Math::Point>& vecteur = tdr->getVector();
        vecteur.push_back(pt1);
        vecteur.push_back(pt2);
    }

    if (tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWDISCRETISATIONTYPE)){

    	// récupération des points de la discrétisation de l'arête avec projection
    	std::vector<Utils::Math::Point> points_coedge;
    	getPoints(points_coedge);

        Utils::Math::Point orig;
        if (m_mesh_property->getDirect())
        	orig = points_coedge[0];
        else
        	orig = points_coedge.back();

    	// calcul de la longueur de l'objet à dessiner (en fonction de la longueur de l'arête)
    	double lg = 0;
    	for (uint i=0; i<points_coedge.size()-1; i++)
    		lg += (points_coedge[i+1]-points_coedge[i]).norme();

    	// construction de la flêche de base
        points.push_back(Utils::Math::Point(0,0,0));
        points.push_back(Utils::Math::Point(1,0,0));
        points.push_back(Utils::Math::Point(0.9,0.1,0));
        points.push_back(Utils::Math::Point(0.9,-0.1,0));
        points.push_back(Utils::Math::Point(0.9,0,0.1));
        points.push_back(Utils::Math::Point(0.9,0,-0.1));
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(1);
        indices.push_back(3);
        indices.push_back(1);
        indices.push_back(4);
        indices.push_back(1);
        indices.push_back(5);

        // positions pour les carrés
        std::vector<double> pos_carres;
        if (m_mesh_property->getMeshLaw() == CoEdgeMeshingProperty::uniforme){
        	pos_carres.push_back(0.3);
        	pos_carres.push_back(0.5);
        	pos_carres.push_back(0.7);
        } else if (m_mesh_property->getMeshLaw() == CoEdgeMeshingProperty::geometrique
        		|| m_mesh_property->getMeshLaw() == CoEdgeMeshingProperty::bigeometrique
        		|| m_mesh_property->getMeshLaw() == CoEdgeMeshingProperty::hyperbolique
        		|| m_mesh_property->getMeshLaw() == CoEdgeMeshingProperty::beta_resserrement){
        	pos_carres.push_back(0.1);
        	pos_carres.push_back(0.2);
        	pos_carres.push_back(0.4);
        	pos_carres.push_back(0.8);
        } else if (m_mesh_property->getMeshLaw() == CoEdgeMeshingProperty::specific_size){
        	pos_carres.push_back(0.4);
        	pos_carres.push_back(0.7);
        } else if (m_mesh_property->getMeshLaw() == CoEdgeMeshingProperty::interpolate) {
        	// on ne fait rien
        } else {
        	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CoEdgeMeshingProperty dispose d'une méthode non prise en charge dans CoEdge::getRepresentation", TkUtil::Charset::UTF_8));
        }

        for (uint i=0; i<pos_carres.size(); i++){
        	indices.push_back(points.size());
        	points.push_back(Utils::Math::Point(pos_carres[i],0.1,0.1));
        	indices.push_back(points.size());
        	indices.push_back(points.size());
        	points.push_back(Utils::Math::Point(pos_carres[i],0.1,-0.1));
        	indices.push_back(points.size());
        	indices.push_back(points.size());
        	points.push_back(Utils::Math::Point(pos_carres[i],-0.1,-0.1));
        	indices.push_back(points.size());
        	indices.push_back(points.size());
        	points.push_back(Utils::Math::Point(pos_carres[i],-0.1,0.1));
        	indices.push_back(points.size()-4);
        }

        // rotation pour le mettre dans le bon repère (tangent au début de la discrétisation)
        Utils::Math::Vector vect;
        if (m_mesh_property->getDirect())
        	vect = Utils::Math::Vector(points_coedge[0], points_coedge[1]);
        else
        	vect = Utils::Math::Vector(points_coedge[1], points_coedge[0]);

        // normalisation
        vect /= vect.abs();

        // vecteur normal à la rotation
        Utils::Math::Vector vrot = Utils::Math::Vector(1,0,0)*vect;
        // cas où on prend un autre vecteur pour la rotation
        if (Utils::Math::MgxNumeric::isNearlyZero(vrot.norme2(), Mgx3D::Utils::Math::MgxNumeric::mgxGeomDoubleEpsilonSquare))
        	vrot = Utils::Math::Vector(0,1,0);

        // angle de la rotation en radians = acos((1,0,0).vect)
        double angle = std::acos(vect.getX());
#ifdef _DEBUG_REPRESENTATION
        std::cout <<"  vect : "<<vect<<std::endl;
        std::cout <<"  vrot : "<<vrot<<std::endl;
        std::cout <<"  angle en degrés "<<angle/M_PI*180.0<<std::endl;
#endif
        try {
        	// création de l'opérateur de rotation via OCC
        	gp_Trsf transf;
        	gp_Pnt p1(0,0,0);
        	gp_Dir dir( vrot.getX(), vrot.getY(), vrot.getZ());
        	gp_Ax1 axis(p1,dir);
        	transf.SetRotation(axis, angle);

        	for (uint i=0; i<points.size(); i++){
        		double x = points[i].getX();
        		double y = points[i].getY();
        		double z = points[i].getZ();

        		transf.Transforms(x, y, z);

        		points[i].setXYZ(x, y, z);
        	}
        }
        catch (const Standard_Failure& exc){
        	std::cerr<<"Représentatoin de la discrétisation de l'arête "<<getName()<<" abandonné ..."<<std::endl;
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        	message <<"Représentatoin de la discrétisation de l'arête "<<getName()
				        		<< " en échec.";
        	log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
        	points.clear();
        	indices.clear();
        }

        // représentation du vecteur de rotation
#ifdef _DEBUG_REPRESENTATION
        indices.push_back(0);
        indices.push_back(points.size());
        points.push_back(vrot);
#endif

        // homothétie pour se mettre à l'échelle / arête globale
        double ratio = lg/7;
        for (uint i=0; i<points.size(); i++)
        	points[i] = points[i]*ratio;

        // translation
        for (uint i=0; i<points.size(); i++)
        	points[i] += orig;

    } // end if (tdr->hasRepresentation(Utils::DisplayRepresentation::SHOWDISCRETISATIONTYPE))

#ifdef _DEBUG_REPRESENTATION
    std::cout <<"  points.size() = "<<points.size()<<std::endl;
    for (uint i=0; i<points.size(); i++)
    	std::cout<<points[i]<<std::endl;
    std::cout <<"  getSurfaceDiscretization().size() = "<<tdr->getSurfaceDiscretization().size()<<std::endl;
    std::cout <<"  getCurveDiscretization().size() = "<<tdr->getCurveDiscretization().size()<<std::endl;
    std::cout <<"  getIsoCurveDiscretization().size() = "<<tdr->getIsoCurveDiscretization().size()<<std::endl;
    std::cout <<"  getVector().size() = "<<tdr->getVector().size()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
bool CoEdge::isPreMeshed() const
{
	bool pointsUpToDate = (m_mesh_property->getModificationTime() < m_mesh_data->getPointsTime());
	// force le recalcul du prémaillage
	if (false==getContext ( ).memorizeEdgePreMesh.getValue( ))
		pointsUpToDate = false;

#ifdef _DEBUG2
	std::cout<<"isPreMeshed pour "<<getName()<<(m_mesh_data->isPreMeshed()?" est prémaillée":" non prémaillée")
			<<(pointsUpToDate?" (pas d'actualisation)":" (actualisation nécessaire)")<<std::endl;
#endif

	return m_mesh_data->isPreMeshed() && pointsUpToDate;
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_GETPOINTS
void CoEdge::getPoints(std::vector<Utils::Math::Point> &points) const
{
#ifdef _DEBUG_GETPOINTS
	std::cout<<"getPoints pour "<<getName()<<(isPreMeshed()?" est prémaillée":" non prémaillée")<<std::endl;
#endif

//	// pour éviter l'appel concurrent à la création des points (cas de l'interpolation qui déclanche cette création pour d'autres arêtes)
//	TkUtil::AutoMutex	autoMutex (&entityFactoryMutex);

	// protection pour éviter les appels concurrents pouvant modifier le preMesh
	TkUtil::AutoMutex autoMutex (&preMeshMutex);

	// s'il existe des noeuds ou des points, est-ce que la date de création est compatible ?
	bool pointsUpToDate = (m_mesh_property->getModificationTime() < m_mesh_data->getPointsTime());

	if (isMeshed() && !pointsUpToDate){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message<<"Erreur interne, L'arête "<<getName()<<" a été modifiée après qu'elle soit maillée";
		throw TkUtil::Exception (message);
	}

	if (!isPreMeshed()){
		m_mesh_data->points().clear();
		getPoints(m_mesh_property, m_mesh_data->points(), true);
		getMeshingData()->setPreMeshed(true);
#ifdef _DEBUG_GETPOINTS
		std::cout<<getName()<<" => est prémaillée"<<(pointsUpToDate?"":" (actualisation)")<<std::endl;
#endif

		getMeshingData()->updatePointsTime();
	}

	if (isMeshed()){

	    gmds::IGMesh& gmds_mesh = getContext().getLocalMeshManager().getMesh()->getGMDSMesh();

		for (std::vector<gmds::TCellID>::iterator iter = m_mesh_data->nodes().begin();
		    			iter != m_mesh_data->nodes().end(); ++iter){
			const gmds::Node& nd = gmds_mesh.get<gmds::Node>(*iter);
			points.push_back(Utils::Math::Point(nd.X(), nd.Y(), nd.Z()));
		}
	}
	else if (isPreMeshed()){

		std::vector<Utils::Math::Point>& ref_points = m_mesh_data->points();
		if (ref_points.begin() != points.begin())
			for (uint i=0; i<ref_points.size(); i++)
				points.push_back(ref_points[i]);

	}
	else {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message<<"Erreur interne, L'arête "<<getName()<<" n'a toujours pas de points !!!";
		throw TkUtil::Exception (message);
	}
}
/*----------------------------------------------------------------------------*/
void CoEdge::clearPoints()
{
	// protection pour éviter les appels concurrents pouvant modifier le preMesh
	TkUtil::AutoMutex autoMutex (&preMeshMutex);
	m_mesh_data->points().clear();
}
/*----------------------------------------------------------------------------*/
void CoEdge::computePoints()
{
	getPoints(m_mesh_data->points());
}
/*----------------------------------------------------------------------------*/
Geom::Curve* CoEdge::createBSplineByProj(Utils::Math::Point& pt0,
		Utils::Math::Point& pt1,
		Geom::Surface* surface) const
{
	Geom::Curve* curve = 0;
	Utils::Math::Point vect = (pt1 - pt0);

	// création d'une courbe BSpline s'appuyant sur les points projetés
	std::vector<Utils::Math::Point> points_bspline;
#ifdef _DEBUG_GETPOINTS
	std::cout<<"Construction d'une B-Spline avec les points de 1 à 19"<<std::endl;
#endif

	points_bspline.push_back(pt0);
#ifdef _DEBUG_GETPOINTS
	std::cout<<" points_bspline[0] (pt0): "<<pt0 <<std::endl;
#endif
	const uint nbPts = 20;
	for (uint i=1; i<nbPts; i++){ // nbPts-2 points entre les 2 extrémités
		Utils::Math::Point pt = pt0 + vect*((double)i)/((double)nbPts);
		surface->project(pt);
#ifdef _DEBUG_GETPOINTS
		std::cout<<" points_bspline["<<i<<"]: "<<pt0 + vect*((double)i)/((double)nbPts)<<" => "<<pt<<std::endl;
#endif
		points_bspline.push_back(pt);
	}
	points_bspline.push_back(pt1);
#ifdef _DEBUG_GETPOINTS
	std::cout<<" points_bspline[..] (pt1): "<<pt1 <<std::endl;
#endif
	// vérification qu'il n'y a pas 2 points confondus
	// on prend comme prédicat que si les 2 premiers et les 2 derniers sont confondus
	// alors on doit être dans le cas du demi-cercle,
	// sinon on doit être en dehors de la surface
	bool is_first_near = false;
	bool is_last_near = false;
	bool is_near = false;
	for (uint i=1; i<points_bspline.size(); i++)
		if (points_bspline[i] == points_bspline[i-1]){
			is_near = true;
			if (i==1)
				is_first_near = true;
			if (i==points_bspline.size()-1)
				is_last_near = true;
		}

	if (is_near){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Des points de construction de la projection de l'arête "
				<< getName()
				<< " sont confondus";
		getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
		if (is_first_near && is_last_near){
			std::cerr<<TkUtil::UTF8String ("Projection sur un demi cercle ?", TkUtil::Charset::UTF_8)<<std::endl;
			throw Utils::HalfCircleSurfaceException(TkUtil::UTF8String ("Des points de construction de la projection de l'arête sont confondus, Projection sur un demi cercle ?", TkUtil::Charset::UTF_8));
		}
		else {
			std::cerr<<TkUtil::UTF8String ("Projection en dehors de la surface ?", TkUtil::Charset::UTF_8)<<std::endl;
			throw Utils::OuterSurfaceException(TkUtil::UTF8String ("Des points de construction de la projection de l'arête sont confondus, Projection en dehors de la surface ?", TkUtil::Charset::UTF_8));
		}
	}
#ifdef _DEBUG_GETPOINTS
	std::cout<<" newBSpline..."<<std::endl;
#endif
	curve = Geom::EntityFactory(getContext()).newBSpline(points_bspline, 1, 2);

#ifdef _DEBUG_GETPOINTS
	//std::cout <<"  Utilisation d'une courbe ("<<curve->getName()<<") issue de la projection d'une arête sur la surface"<<std::endl;
	std::cout <<"  Utilisation d'une courbe ("<<curve->getName()<<") issue de la projection de points sur la surface et création d'une B-Spline"<<std::endl;
	for (uint i=0; i<points_bspline.size(); i++)
		std::cout<<" points_bspline["<<i<<"] = "<<points_bspline[i]<<std::endl;
#endif

	return curve;
}

/*----------------------------------------------------------------------------*/
Geom::Curve* CoEdge::createBSplineByProjWithOrthogonalIntersection(Utils::Math::Point& pt0,
		Utils::Math::Point& pt1,
		Geom::Surface* surface) const
{
	// on va commencer par essayer de placer un point sur la surface à l'intersection entre le plan orthogonal à l'arête
	// et cette surface.
	// on continue ensuite comme pour createBSplineByProj en 2 parties

	Geom::Curve* curve = 0;
	Utils::Math::Point vect = (pt1 - pt0);

	// création d'une courbe BSpline s'appuyant sur les points projetés
	std::vector<Utils::Math::Point> points_bspline;
#ifdef _DEBUG_GETPOINTS
	std::cout<<"Construction d'une B-Spline avec un point projeté sur la surface orthogonalement"<<std::endl;
#endif

    Utils::Math::Point plane_pnt = (pt1 + pt0)/2; // point au centre pour le plan
#ifdef _DEBUG_GETPOINTS
    std::cout<<" plane_pnt : "<<plane_pnt<<std::endl;
#endif
    Utils::Math::Vector plane_vec(pt1 - pt0);
    gp_Pln gp_plane(gp_Pnt(plane_pnt.getX(), plane_pnt.getY(), plane_pnt.getZ()),
    		gp_Dir(plane_vec.getX(), plane_vec.getY(), plane_vec.getZ()));
    BRepBuilderAPI_MakeFace mkF(gp_plane);
    TopoDS_Face wf = mkF.Face();
    TopoDS_Shape shape;
    Geom::GeomModificationBaseClass::getOCCShape(surface, shape);

    BRepAlgoAPI_Section intersector(shape, wf);


    TopoDS_Shape section_tool;

    if (intersector.IsDone()){
    	section_tool = intersector.Shape();
    }
    else {
    	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message<<"Echec lors de l'intersection entre plan et surface "<<surface->getName();
    	throw TkUtil::Exception (message);
    }


#ifdef _DEBUG_GETPOINTS
    GProp_GProps pb;
    BRepGProp::LinearProperties(section_tool,pb);
    std::cout<<"longueur de la courbe intersection :"<< pb.Mass()<<std::endl;
#endif

    // recherche point projeté au plus près sur courbe intersection
    gp_Pnt pnt(plane_pnt.getX(),plane_pnt.getY(),plane_pnt.getZ());
    TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(pnt);
    BRepExtrema_DistShapeShape extrema(V, section_tool);
    bool isDone = extrema.IsDone();
    if(!isDone) {
    	isDone = extrema.Perform();
    }
    if(!isDone){
    	std::cerr<<"Erreur interne sur la projection du centre de l'arete"<<std::endl;
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message<<"Echec d'une projection d'un point sur une courbe, pour arête sur "<<surface->getName();
		throw TkUtil::Exception (message);
    }
    gp_Pnt pnt2 = extrema.PointOnShape2(1);
    Utils::Math::Point ptI(pnt2.X(), pnt2.Y(), pnt2.Z());

#ifdef _DEBUG_GETPOINTS
    std::cout<<" ptI : "<<ptI<<std::endl;
#endif


    points_bspline.push_back(pt0);

	const uint nbPts = 10;
	vect = (ptI - pt0);
	for (uint i=1; i<nbPts; i++){ // nbPts-2 points entre les 2 extrémités
		Utils::Math::Point pt = pt0 + vect*((double)i)/((double)nbPts);
		surface->project(pt);
#ifdef _DEBUG_GETPOINTS
		std::cout<<" points_bspline["<<i<<"]: "<<pt0 + vect*((double)i)/((double)nbPts)<<" => "<<pt<<std::endl;
#endif
		points_bspline.push_back(pt);
	}

    points_bspline.push_back(ptI);

	vect = (pt1 - ptI);
	for (uint i=1; i<nbPts; i++){ // nbPts-2 points entre les 2 extrémités
		Utils::Math::Point pt = ptI + vect*((double)i)/((double)nbPts);
		surface->project(pt);
#ifdef _DEBUG_GETPOINTS
		std::cout<<" points_bspline["<<i<<"]: "<<ptI + vect*((double)i)/((double)nbPts)<<" => "<<pt<<std::endl;
#endif
		points_bspline.push_back(pt);
	}

	points_bspline.push_back(pt1);

	// vérification qu'il n'y a pas 2 points confondus
	for (uint i=1; i<points_bspline.size(); i++)
		if (points_bspline[i] == points_bspline[i-1]){
			std::cerr<<TkUtil::UTF8String ("Des points de construction de la projection de l'arête coupée en 2 sont confondus", TkUtil::Charset::UTF_8)<<std::endl;
			throw TkUtil::Exception (TkUtil::UTF8String ("Des points de construction de la projection de l'arête coupée en 2 sont confondus", TkUtil::Charset::UTF_8));
		}
#ifdef _DEBUG_GETPOINTS
	std::cout<<" newBSpline..."<<std::endl;
#endif
	curve = Geom::EntityFactory(getContext()).newBSpline(points_bspline, 1, 2);

#ifdef _DEBUG_GETPOINTS
	//std::cout <<"  Utilisation d'une courbe ("<<curve->getName()<<") issue de la projection d'une arête sur la surface"<<std::endl;
	std::cout <<"  Utilisation d'une courbe ("<<curve->getName()<<") issue de la projection de points sur la surface et création d'une B-Spline"<<std::endl;
	for (uint i=0; i<points_bspline.size(); i++)
		std::cout<<" points_bspline["<<i<<"] = "<<points_bspline[i]<<std::endl;
#endif

	return curve;
}
/*----------------------------------------------------------------------------*/
void CoEdge::
getPoints(CoEdgeMeshingProperty* dni, std::vector<Utils::Math::Point> &points, bool project) const
{
//	// pour protéger OCC et éviter 2 créations de courbes simultanément
//	// mais aussi pour la cohérence sur les numéros d'entités créées (utilisation du getInternalStats)
//	TkUtil::AutoMutex	autoMutex (&entityFactoryMutex);

	// protection pour éviter les appels concurrents pouvant modifier le preMesh
	TkUtil::AutoMutex autoMutex (&preMeshMutex);

	const uint nbBrasI = dni->getNbEdges();
#ifdef _DEBUG_GETPOINTS
	std::cout<<"getPoints avec nbBrasI = "<<nbBrasI<<" pour "<<getName()<<(project?" avec projection":" sans projection")<<std::endl;
#endif
//	dni->initCoeff();

	Utils::Math::Point pt0 = getVertex(0)->getCoord();
	Utils::Math::Point pt1 = getVertex(1)->getCoord();

	if (project){
		Geom::GeomEntity* ge0 = getVertex(0)->getGeomAssociation();
		if (ge0 && !getVertex(0)->isMeshed())
			ge0->project(pt0);
		Geom::GeomEntity* ge1 = getVertex(1)->getGeomAssociation();
		if (ge1 && !getVertex(1)->isMeshed())
			ge1->project(pt1);
	}

	Utils::Math::Point vect = (pt1 - pt0);

#ifdef _DEBUG_GETPOINTS
	std::cout<<" pt0: "<<pt0<<std::endl;
	std::cout<<" pt1: "<<pt1<<std::endl;
	std::cout<<" vect = " <<vect<<" longueur "<<vect.norme()<<std::endl;
	std::cout<<" getGeomAssociation() -> "<<(getGeomAssociation()?getGeomAssociation()->getName():"0")<<std::endl;
	std::cout<<" dni->getMeshLaw() "<<dni->getMeshLawName()<<std::endl;
#endif

	if (project && getGeomAssociation()
	&& dni->getMeshLaw() != CoEdgeMeshingProperty::interpolate
	&& dni->getMeshLaw() != CoEdgeMeshingProperty::globalinterpolate){
		// cas avec projection
		Geom::GeomEntity* ge = getGeomAssociation();
#ifdef _DEBUG_GETPOINTS
		std::cout<<" projection sur "<<ge->getName()<<std::endl;
#endif
		points.push_back(pt0);

		if (nbBrasI > 1){

			// on fait la projection des points en tenant compte d'un paramètre curviligne
			// pour les projections sur courbes

			// mémorisation des ids, pour le cas de la création d'une courbe temporaire
			std::vector<unsigned long> name_manager_before;

			// courbe sur laquelle se fait la projection
			Geom::Curve* curve = 0;
			bool curveToBeDeleted = false;
			if (ge->getType() == Utils::Entity::GeomCurve)
				curve = dynamic_cast<Geom::Curve*> (ge);
			else if (ge->getType() == Utils::Entity::GeomSurface){

				getContext().getNameManager().getInternalStats(name_manager_before);
				Geom::Surface* surface = dynamic_cast<Geom::Surface*> (ge);
				CHECK_NULL_PTR_ERROR(surface);

				try {
					curve = createBSplineByProj(pt0, pt1, surface);
				}
				catch (Utils::HalfCircleSurfaceException& exc){

					try {
						// 2ème essai avec autre méthode:
						// on place le point central de l'arête directement sur la surface
						// pour cela on constitue la courbe intersection entre plan orthogonal à l'arête et la surface
						// puis on projette le centre de l'arête sur cette courbe
						// on projette ensuite sur la surface les points entre les extrémités de l'arête et ce point projeté
						curve = createBSplineByProjWithOrthogonalIntersection(pt0, pt1, surface);
					}
					catch (TkUtil::Exception& exc){

						// remet les compteurs pour les ids
						getContext().getNameManager().setInternalStats(name_manager_before);

						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
						message << "Pb avec création d'une courbe par projection de l'arête "<<getName()<<" sur la surface "<<ge->getName();
						message << "\nLe pb est peut-être lié à une projection sur un demi cercle => couper l'arête en deux";

						// message plus important au niveau des logs
						TkUtil::UTF8String	messageComplet (TkUtil::Charset::UTF_8);
						messageComplet<<message<<", message remonté : "<<exc.getMessage();
						getContext().getLogStream()->log(TkUtil::TraceLog (messageComplet, TkUtil::Log::TRACE_3));

						throw TkUtil::Exception (message);
					}
				}
				catch (TkUtil::Exception& exc){

					// remet les compteurs pour les ids
					getContext().getNameManager().setInternalStats(name_manager_before);

					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
					message << "Pb avec création d'une courbe par projection de l'arête "<<getName()<<" sur la surface "<<ge->getName();
					message << "\nLe pb est peut-être lié à une projection en dehors de la surface => découper l'arête et revoir les associations";

					// message plus important au niveau des logs
					TkUtil::UTF8String	messageComplet (TkUtil::Charset::UTF_8);
					messageComplet<<message<<", message remonté : "<<exc.getMessage();
					getContext().getLogStream()->log(TkUtil::TraceLog (messageComplet, TkUtil::Log::TRACE_3));

					throw TkUtil::Exception (message);
				}

				if (0 == curve){
					// remet les compteurs pour les ids
					getContext().getNameManager().setInternalStats(name_manager_before);

					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
					message << "OCC a échoué, création de la projection de l'arête "
							<< getName() << " sur la surface  "<<ge->getName()
							<<", cas en dehors de la surface par exemple";
					throw TkUtil::Exception (message);
				}
				else
					curveToBeDeleted = true;

			} // end else if (ge->getType() == Utils::Entity::GeomSurface)
			else {
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
				message << "Projection impossible pour l'arête \""
						<< getName() << "\", sur "<<ge->getName()<<" de type "<<ge->getTypeName()<<"\n";
				message << "elle est projetée sur autre chose qu'une courbe ou une surface";
				throw TkUtil::Exception (message);
			}

			if (curve == 0){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
				message << "Erreur interne pour l'arête \""<< getName() << "\", curve == 0\n";
				throw TkUtil::Exception (message);
			}

			try {
				if (curveToBeDeleted)
					dni->initCoeff(curve->getArea());
				else {
					// TODO [EB]: il faut calculer la longueur de l'arête projetée sur la courbe


					// regarde si l'on est dans le cas de l'arête projetée sur l'intégralité de la courbe
					std::vector<Geom::Vertex*> crv_vertices;
					curve->get(crv_vertices);
					if (getNbVertices() == 2 && crv_vertices.size() == 2
							&& ( (getVertex(0)->getCoord() == crv_vertices[0]->getCoord() && getVertex(1)->getCoord() == crv_vertices[1]->getCoord())
									|| (getVertex(0)->getCoord() == crv_vertices[1]->getCoord() && getVertex(1)->getCoord() == crv_vertices[0]->getCoord()) )
					){
						dni->initCoeff(curve->getArea());
#ifdef _DEBUG_GETPOINTS
						std::cout<<"  initCoeff avec "<<curve->computeArea()<<std::endl;
#endif
					}
					else if (curve->isLinear()) {
						// pas de pb pour le cas linéaire
						double dist = getVertex(0)->getCoord().length(getVertex(1)->getCoord());
						dni->initCoeff(dist);
#ifdef _DEBUG_GETPOINTS
						std::cout<<"  initCoeff cas linéaire avec dist"<<dist<<std::endl;
#endif
					}
					else {
						// cas problématique (projection sur une portion de la courbe) ...
						if (dni->needLengthToInitialize()){
							TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			            	message<<" projection sur "<<ge->getName()<<" pour l'arête "<<getName()<<" problématique pour cas bigéométrique ou autre nécessitant la connaissance de la longueur de la portion du contour";
			            	getContext().getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));

						}

						double dist = getVertex(0)->getCoord().length(getVertex(1)->getCoord());
						dni->initCoeff(dist);
						//dni->initCoeff();
#ifdef _DEBUG_GETPOINTS
						std::cout<<"  initCoeff cas autre avec dist"<<dist<<std::endl;
#endif

					}
				} // end else / if (curveToBeDeleted)
			}
			catch (TkUtil::Exception& exc){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "Pb avec initialisation des paramètres de discrétisation de l'arête "
						<< getName() << ", message remonté : "<<exc.getMessage();
				throw TkUtil::Exception (message);
			}

			double* l_ratios = new double[nbBrasI-1];
			for (uint i=0; i<nbBrasI-1; i++){
				l_ratios[i] = dni->nextCoeff();
#ifdef _DEBUG_GETPOINTS
					std::cout<<"  l_ratios["<<i<<"] = "<<l_ratios[i]<<std::endl;
#endif
			}

			if (curve == 0){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "Erreur interne pour l'arête \""
						<< getName() << "\", sur "<<ge->getName()<<" de type "<<ge->getTypeName()
						<<", pb de création de la courbe géométrique\n";
				throw TkUtil::Exception (message);
			}
			try {
				// calcul de la position des points sur la courbe
				if (dni->isPolarCut())
					curve->getPolarParametricsPoints(pt0, pt1,
							nbBrasI-1, l_ratios, points, dni->getPolarCenter());
				else
					curve->getParametricsPoints(pt0, pt1,
							nbBrasI-1, l_ratios, points);
#ifdef _DEBUG_GETPOINTS
				std::cout<<"pt0 : "<<pt0<<std::endl;
				for (uint i=0; i<nbBrasI-1; i++){
					std::cout<<" ratio : "<<l_ratios[i]<<", => point"<<points[i+1];
					std::cout<<" dist au précédent : "<<points[i+1].length(points[i]);
					std::cout<<std::endl;
				}
				std::cout<<"pt1 : "<<pt1<<" dist au précédent : "<<pt1.length(points.back())<<std::endl;
#endif
			}
			catch (TkUtil::Exception& exc){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "Curve::getParametricsPoints a échoué, récupération des paramètres aux extrémités impossible pour l'arête "
						<< getName() << ", sur "<<ge->getName();
				message<<", pt0 : "<<pt0;
				message<<", pt1 : "<<pt1;
				std::vector<Geom::Vertex*> vertices;
				curve->get(vertices);
				if (vertices.size()>0)
					message<<", vertices[0] : "<<vertices[0]->getCoord();
				if (vertices.size()>1)
					message<<", vertices[1] : "<<vertices[1]->getCoord();
				message<<"\nMessage remonté: "<<exc.getMessage();
				throw TkUtil::Exception (message);
			}
			catch (const Standard_Failure& exc){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "OCC a échoué, récupération des paramètres aux extrémités impossible pour l'arête "
						<< getName() << ", sur "<<ge->getName();
				throw TkUtil::Exception (message);
			}

			delete [] l_ratios;

			// on replace les noeuds de l'arête sur la surface
			if (curveToBeDeleted){
				Geom::Surface* surface = dynamic_cast<Geom::Surface*> (ge);
				CHECK_NULL_PTR_ERROR(surface);
				Utils::Math::Point newPt;
				double lnVect = vect.norme();

#ifdef _DEBUG_GETPOINTS
				std::cout<<"reprojection de la bspline sur la surface..."<<std::endl;
#endif

				for (uint i=1; i<points.size()/2; i++){
					surface->project(points[i], newPt);
#ifdef _DEBUG_GETPOINTS
					std::cout<<"dist newPt à pt0 = "<<(newPt-pt0).norme()<<std::endl;
#endif
					double len0 = (newPt-pt0).norme();

					if (!Utils::Math::MgxNumeric::isNearlyZero(len0/lnVect))
						points[i] = newPt;
				}
				for (uint i=points.size()/2; i<points.size()-1; i++){
					surface->project(points[i], newPt);
#ifdef _DEBUG_GETPOINTS
					std::cout<<"dist newPt à pt1 = "<<(newPt-pt1).norme()<<std::endl;
#endif
					double len1 = (newPt-pt1).norme();

					if (!Utils::Math::MgxNumeric::isNearlyZero(len1/lnVect))
						points[i] = newPt;
				}
			}

			if (curveToBeDeleted){
				delete curve;
				// remet les compteurs pour les ids
				getContext().getNameManager().setInternalStats(name_manager_before);
			}

		} // end if (nbBrasI > 1)

		points.push_back(pt1);

	} else if (project
			&& dni->getMeshLaw() == CoEdgeMeshingProperty::interpolate) {
		// cas avec interpolation
		// cela nécessite de laisser le calcul des positions des points à EdgeMeshingPropertyInterpolate

		EdgeMeshingPropertyInterpolate* interpol = dynamic_cast<EdgeMeshingPropertyInterpolate*>(dni);
		CHECK_NULL_PTR_ERROR(interpol);

		if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coedge_list){
			// recherche des arêtes par rapport aux quelles se fait l'interpolation
			std::vector<std::string> coedges_names = interpol->getCoEdges();
			std::vector<CoEdge*> coedges;
			getCoEdges(coedges_names, coedges);

			uint nbMeshingEdges = 0;
			for (uint i=0; i<coedges.size(); i++)
				nbMeshingEdges+=coedges[i]->getNbMeshingEdges();

			// il faut avoir le même nombre de sommets sur les deux arêtes
			if (interpol->getNbEdges() != nbMeshingEdges){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "Erreur, l'arête "<<getName()
							<<" est discrétisée en "<<(short)interpol->getNbEdges()
							<<" alors que les arêtes de référence le sont en "<<(short)nbMeshingEdges;
				throw TkUtil::Exception (message);
			}

			// vérifie que les coedges_ref ne dépendent pas de la coedge de départ
			std::set<const CoEdge*> filtre_coedges; // pour éviter d'y passer trop de temps
			detectLoopReference(this, coedges, filtre_coedges);
			updateModificationTime(this, coedges);

			// cas d'une interpolation avec projection sur une courbe ou une surface
			// on passe par une tabulation
			if (getGeomAssociation()
					&& (getGeomAssociation()->getType() == Utils::Entity::GeomCurve
							|| getGeomAssociation()->getType() == Utils::Entity::GeomSurface)){

#ifdef _DEBUG_GETPOINTS
				std::cout<<"Cas interpolé avec utilisation EdgeMeshingPropertyTabulated via coedges"<<std::endl;
#endif
				// on traite le cas avec projection comme on le fait avec une tabulation
				std::vector<double> tabulation;
				interpol->getTabulations(tabulation, coedges, pt0, pt1);

				// Cas où le sens est inversé à la main par l'utilisateur
				// L'algo actuel oriente l'arête par compraison géométrique ce qui n'est pas
				// adapté à une face courbe ou avec un rebroussement par exemple
				if ((0 != m_mesh_property) && (false == m_mesh_property->getDirect ( )))
					std::reverse (tabulation.begin ( ), tabulation.end ( ));

				Topo::CoEdgeMeshingProperty * empTab = new Topo::EdgeMeshingPropertyTabulated(tabulation);
				getPoints(empTab, points, true);

				delete empTab;

			} // if getType() == GeomCurve
			else {
#ifdef _DEBUG_GETPOINTS
				std::cout<<"Cas interpolé (interpol::getPoints(coedges)) ..."<<std::endl;
#endif
				// on initialise le vecteur de points avec les extrémités
				points.resize(nbMeshingEdges+1);
				points[0] = pt0;
				points[nbMeshingEdges] = pt1;

				// l'interpolation n'accède pas au contexte, donc on lui passe l'arête
				interpol->getPoints(points, coedges);
				// [EB] code mort ...
				//			// s'il y a une projection sur une surface on replace les points dessus
				//			if (getGeomAssociation() && getGeomAssociation()->getType() == Utils::Entity::GeomSurface){
				//				Geom::GeomEntity* ge = getGeomAssociation();
				//				for (uint i=1; i<getNbMeshingEdges(); i++)
				//					ge->project(points[i]);
				//			} // end getGeomAssociation()

			} // end else / if getType() == GeomCurve
		} // if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coedge_list)
		else if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coface) {

			std::string coface_name = interpol->getCoFace();

			CoFace* coface = 0;
			try {
				coface = getContext().getLocalTopoManager().getCoFace(coface_name, false);
			}
			catch (Utils::IsDestroyedException &e){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "Erreur, l'arête "<<getName()<<" a une discrétisation basée sur l'interpolation en utilisant la face "
						<< coface_name <<" qui est détruite";
				throw TkUtil::Exception (message);
			}

			if (coface == 0){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "Erreur, l'arête "<<getName()<<" a une discrétisation basée sur l'interpolation en utilisant une face non renseignée (détruite)";
				throw TkUtil::Exception (message);
			}

			// vérifie que les coedges_ref ne dépendent pas de la coedge de départ
			std::set<const CoEdge*> filtre_coedges; // pour éviter d'y passer trop de temps
			detectLoopReference(this, this, coface, filtre_coedges);

			if (getGeomAssociation()
					&& (getGeomAssociation()->getType() == Utils::Entity::GeomCurve
							|| getGeomAssociation()->getType() == Utils::Entity::GeomSurface)){

#ifdef _DEBUG_GETPOINTS
				std::cout<<"Cas interpolé avec utilisation EdgeMeshingPropertyTabulated via coface"<<std::endl;
#endif
				// on traite le cas avec projection comme on le fait avec une tabulation
				std::vector<double> tabulation;
				interpol->getTabulations(tabulation, this, coface);

				Topo::CoEdgeMeshingProperty * empTab = new Topo::EdgeMeshingPropertyTabulated(tabulation);
				getPoints(empTab, points, true);

				delete empTab;

			} // if getType() == GeomCurve
			else {
#ifdef _DEBUG_GETPOINTS
				std::cout<<"Cas interpolé (interpol::getPoints(coface)) ..."<<std::endl;
#endif
				points.resize(interpol->getNbEdges()+1);
				points[0] = pt0;
				points[interpol->getNbEdges()] = pt1;

				// l'interpolation n'accède pas au contexte, donc on lui passe la face
				interpol->getPoints(points, this, coface);

			} // end else / if getType() == GeomCurve

		} // else if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coface)


	} // else if (project && dni->getMeshLaw() == CoEdgeMeshingProperty::interpolate)
	else if (project
			&& dni->getMeshLaw() == CoEdgeMeshingProperty::globalinterpolate) {
		// cas avec interpolation globale
		// cela nécessite de laisser le calcul des positions des points à EdgeMeshingPropertyGlobalInterpolate

		EdgeMeshingPropertyGlobalInterpolate* interpol = dynamic_cast<EdgeMeshingPropertyGlobalInterpolate*>(dni);
		CHECK_NULL_PTR_ERROR(interpol);

		// recherche des arêtes par rapport aux quelles se fait l'interpolation
		std::vector<CoEdge*> first_coedges;
		std::vector<CoEdge*> second_coedges;
		std::vector<std::string> first_coedges_names = interpol->getFirstCoEdges();
		std::vector<std::string> second_coedges_names = interpol->getSecondCoEdges();
		uint nbMeshingEdges1 = 0;
		uint nbMeshingEdges2 = 0;
		for (uint i=0; i<first_coedges_names.size(); i++){
			CoEdge* coedge = getContext().getLocalTopoManager().getCoEdge(first_coedges_names[i], false);

			if (coedge == 0){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "Erreur, l'arête "<<getName()<<" a une discrétisation basée sur celle de "<<first_coedges_names[i]
						<<" qui n'existe pas (ou n'existe plus)";
				throw TkUtil::Exception (message);
			}
			// cas où on dépend de soit même
			if (coedge == this){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "Erreur, l'arête "<<getName()<<" a une discrétisation basée sur elle même \n";
				message << "(peut-être avez-vous utilisé setParallelMeshingProperty au lieu de setMeshingProperty)";
				throw TkUtil::Exception (message);
			}

			first_coedges.push_back(coedge);

			nbMeshingEdges1+=coedge->getNbMeshingEdges();

		} // end for i<first_coedges_names.size()

		for (uint i=0; i<second_coedges_names.size(); i++){
			CoEdge* coedge = getContext().getLocalTopoManager().getCoEdge(second_coedges_names[i], false);

			if (coedge == 0){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "Erreur, l'arête "<<getName()<<" a une discrétisation basée sur celle de "<<second_coedges_names[i]
						<<" qui n'existe pas (ou n'existe plus)";
				throw TkUtil::Exception (message);
			}
			// cas où on dépend de soit même
			if (coedge == this){
				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
				message << "Erreur, l'arête "<<getName()<<" a une discrétisation basée sur elle même \n";
				message << "(peut-être avez-vous utilisé setParallelMeshingProperty au lieu de setMeshingProperty)";
				throw TkUtil::Exception (message);
			}

			second_coedges.push_back(coedge);

			nbMeshingEdges2+=coedge->getNbMeshingEdges();

		} // end for i<second_coedges_names.size()

		// il faut avoir le même nombre de sommets sur les deux arêtes
		if (interpol->getNbEdges() != nbMeshingEdges1 || interpol->getNbEdges() != nbMeshingEdges2){
			TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
			message << "Erreur, l'arête "<<getName()
					<<" est discrétisée en "<<(short)interpol->getNbEdges()
					<<" alors que les arêtes de référence le sont en "<<(short)nbMeshingEdges1
					<<" et "<<(short)nbMeshingEdges2;
			throw TkUtil::Exception (message);
		}

		// on initialise le vecteur de points avec les extrémités
		points.resize(nbMeshingEdges1+1);
		points[0] = pt0;
		points[nbMeshingEdges1] = pt1;

		// l'interpolation n'accède pas au contexte, donc on lui passe les listes d'arêtes
		interpol->getPoints(points, first_coedges, second_coedges);

	} // else if (project && dni->getMeshLaw() == CoEdgeMeshingProperty::globalinterpolate)
	else {
#ifdef _DEBUG_GETPOINTS
		std::cout<<"Cas sans projection ..."<<std::endl;
#endif
		try {
			// cas sans projection
			dni->initCoeff(vect.norme());
		}
		catch (TkUtil::Exception& exc){
			TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
			message << "Pb avec initialisation des paramètres de discrétisation de l'arête "
					<< getName() << ", message remonté : "<<exc.getMessage();
			throw TkUtil::Exception (message);
		}

		points.push_back(pt0);
		for (uint i=0; i<nbBrasI-1; i++){
			Utils::Math::Point pt = pt0 + vect * dni->nextCoeff();
			points.push_back(pt);
		}
		points.push_back(pt1);
	}



    // cas où il faut rendre la discrétisation orthogonale
    // on ne fait rien si l'arête est projetée sur une courbe
    bool projectOnCurve = false;
    // nombre de couches qui respectent l'orthogonalité
    uint nbLayers = dni->getNbLayers();

    if (getGeomAssociation() && getGeomAssociation()->getType() == Utils::Entity::GeomCurve)
    	projectOnCurve = true;
    if (project && dni->isOrthogonal() && (not projectOnCurve) && nbLayers){
#ifdef _DEBUG_GETPOINTS
    	std::cout<<"Déplacement des points pour être orthogonal sur "<<dni->getNbLayers()<<" couches"<<std::endl;
#endif
    	// recherche de la normale
        Utils::Math::Vector normale;

        Topo::Vertex* vtx0 = getVertex(dni->getSide());
        Topo::Vertex* vtxN = getVertex(1-dni->getSide());
		Utils::Math::Point pt0 = vtx0->getCoord();
		Utils::Math::Point ptN = vtxN->getCoord();
        Utils::Math::Point vect = (ptN - pt0);

    	// recherche des surfaces / orthogonal
        Geom::GeomEntity* ge = vtx0->getGeomAssociation();
        try {
        	if (ge == 0){
        		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        		message << "Problème: on ne sait pas définir la normale pour le sommet "<<vtx0->getName()
                    		<< ", dans l'arête "<<getName()<<" car le sommet n'est pas associé à la géométrie";
        		throw TkUtil::Exception (message);
        	}
        	else if (ge->getType() == Utils::Entity::GeomSurface){
        		// c'est le cas le plus simple
        		Geom::Surface* surface = dynamic_cast<Geom::Surface*>(ge);
        		CHECK_NULL_PTR_ERROR(surface);
        		Utils::Math::Point pt0 = vtx0->getCoord();
        		surface->normal(pt0, normale);
#ifdef _DEBUG_GETPOINTS
        		std::cout<<"  calcul normale à "<<surface->getName()<<" en "<<pt0<<std::endl;
#endif
        	}
        	else if (ge->getType() == Utils::Entity::GeomCurve
        			|| ge->getType() == Utils::Entity::GeomVertex) {

        		// recherche des surfaces qui en dépendent, en évitant la surface sur laquelle ed pourrait être projetée
        		std::vector<Geom::Surface*> surfaces1;
        		ge->get(surfaces1);

        		std::vector<Geom::Surface*> surfaces2;
        		for (uint i=0; i<surfaces1.size(); i++)
        			if (surfaces1[i] != getGeomAssociation())
        				surfaces2.push_back(surfaces1[i]);

        		if (surfaces2.empty()){
        			// c'est le cas en 2D

            		// recherche des courbes qui en dépendent, en évitant la courbe sur laquelle ed pourrait être projetée
            		std::vector<Geom::Curve*> curves1;
            		if (ge->getType() == Utils::Entity::GeomCurve)
            			curves1.push_back(dynamic_cast<Geom::Curve*>(ge));
            		else
            			ge->get(curves1);

            		std::vector<Geom::Curve*> curves2;
            		for (uint i=0; i<curves1.size(); i++)
            			if (curves1[i] != getGeomAssociation())
            				curves2.push_back(curves1[i]);

            		if (curves2.empty()){
            			TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
            			message << "Erreur interne: on ne sait pas définir la normale pour l'arête "
            					<<getName()<<", cas non implémenté, pour 2D sans courbe à la base";
            			throw TkUtil::Exception (message);
            		}
            		else if (curves2.size() == 1){

            			// l'arête doit être dans le plan Z=0
            			if (!Utils::Math::MgxNumeric::isNearlyZero(getVertex(0)->getCoord().getZ())
            			|| !Utils::Math::MgxNumeric::isNearlyZero(getVertex(1)->getCoord().getZ())){
            				TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
            				message << "On ne sait pas définir la normale pour l'arête "
            						<<getName()<<", elle n'est pas dans le plan Z=0";
            				throw TkUtil::Exception (message);
            			}
            			Utils::Math::Point pt0 = vtx0->getCoord();
            			Utils::Math::Vector tangente;
            			curves2[0]->tangent(pt0, tangente);

            			normale = Utils::Math::Vector(tangente.getY(), -tangente.getX(), 0);

            			if (normale.dot(tangente) < 0)
            				normale = Utils::Math::Vector(-tangente.getY(), tangente.getX(), 0);

            		}
            		else {
            			TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
            			message << "Erreur interne: extrémité de l'arête "
            					<<getName()<<" reliée à plus d'une courbe !!! (";
            			for (uint i=0; i<curves2.size(); i++)
            				message <<" "<<curves2[i]->getName();
            			message << " )";
            			throw TkUtil::Exception (message);
            		}
        		}
        		else {
        			// on prend la normale moyenne entre les différentes surfaces
        			for (uint i=0; i<surfaces2.size(); i++){
        				Utils::Math::Vector normaleLoc;
#ifdef _DEBUG_GETPOINTS
        				std::cout<<"  calcul normale à "<<surfaces2[i]->getName()<<" en "<<pt0<<std::endl;
#endif
        				surfaces2[i]->normal(pt0, normaleLoc);
        				normale+=normaleLoc;
        			}
        		}

        	}
        	else {
        		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        		message << "Erreur interne: on ne sait pas définir la normale pour l'arête "
        				<<getName()<<", cas non prévu";
        		throw TkUtil::Exception (message);
        	}

        	double norme = normale.norme();
        	if (Utils::Math::MgxNumeric::isNearlyZero(norme)){
        		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        		message << "Erreur interne: on ne sait pas définir la normale pour l'arête "
        				<<getName()<<", normale indéfinie";
        		throw TkUtil::Exception (message);
        	}

            // nous avons donc une normale ...
            normale/=norme;
            if (Utils::Math::scaMul(vect, normale)<0.0)
            	normale *= -1;
#ifdef _DEBUG_GETPOINTS
        std::cout <<"  orthogonal avec comme normale "<<normale<<std::endl;
        std::cout <<"  vect : "<<vect<<std::endl;
#endif

        }
        catch(Utils::BadNormalException &e) {
        	// cas où on ne peut pas compter sur OCC pour obtenir la normale
        	normale = Topo::TopoHelper::computeNormale(vtx0, this);
#ifdef _DEBUG_GETPOINTS
        	std::cout <<"  computeNormale retourne comme normale "<<normale<<std::endl;
#endif
        }

#ifdef _DEBUG_GETPOINTS
        std::cout<<"dni->getSide() = "<<dni->getSide()<<std::endl;
#endif
        // Nous utilisons les distances pour replacer les points
        if (dni->getSide() == 0){
        	std::vector<Utils::Math::Point> ptInternes;
        	for (uint i=1; i<points.size()-1; i++){
        		double ratio = (points[i] - pt0).norme();
#ifdef _DEBUG_GETPOINTS
        		std::cout<<"ratio = "<<ratio<<" pour point "<<points[i]<<std::endl;
#endif
        		ptInternes.push_back(pt0 + (normale * ratio));
        	}
        	ptInternes.push_back(pt0 + (normale * vect.norme()));

        	// déplace les point pour se raccorder à ptN
        	Mesh::MeshImplementation::courbeDiscretisation(ptN, ptInternes, nbLayers);

        	for (uint i=1; i<points.size()-1; i++)
        		points[i] = ptInternes[i-1];
        }
        else {
        	// cas avec sens inversé
        	std::vector<Utils::Math::Point> ptInternes;
        	for (uint i=points.size()-2; i>=1; i--){
        		double ratio = (points[i] - pt0).norme();
#ifdef _DEBUG_GETPOINTS
        		std::cout<<"ratio = "<<ratio<<" pour point "<<points[i]<<std::endl;
#endif
        		ptInternes.push_back(pt0 + (normale * ratio));
        	}
        	ptInternes.push_back(pt0 + (normale * vect.norme()));

        	// déplace les point pour se raccorder à pt0
        	Mesh::MeshImplementation::courbeDiscretisation(ptN, ptInternes, nbLayers);

        	for (uint i=1; i<points.size()-1; i++)
        		points[i] = ptInternes[ptInternes.size()-1-i];
        }

        // reprojection sur la surface pour le cas où on l'aurait quitté
        if (getGeomAssociation() && getGeomAssociation()->getType() == Utils::Entity::GeomSurface){
        	Geom::GeomEntity* ge = getGeomAssociation();
        	for (uint i=1; i<points.size()-1; i++)
        		ge->project(points[i]);
        }

    } // end if (dni->isOrthogonal())


#ifdef _DEBUG
	if (nbBrasI+1 != points.size()){
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message << "Erreur interne, CoEdge::getPoints ne retourne pas le nombre de points attendu pour "<<getName()<<", "
				<<(long)points.size()<<" au lieu des "<<(long)(nbBrasI+1)<<" prévus";
		throw TkUtil::Exception (message);
	}
#endif
#ifdef _DEBUG_GETPOINTS
	std::cout<<"points retournés : "<<std::endl;
	for (uint i=0; i<points.size(); i++)
		std::cout<<"   "<<points[i]<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* CoEdge::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            TopoEntity::getDescription (alsoComputed));

    // les propriétés spécifiques à ce type d'entité

    Utils::SerializedRepresentation  topoProprietes ("Propriétés topologiques", "");

#ifdef _DEBUG
    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("En cours d'édition", isEdited()));
#endif

    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Méthode de maillage", getMeshLawName()));

    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Nombre de bras", (long)getNbMeshingEdges()));

    if (m_mesh_property->isPolarCut()){
    	 std::vector<double> coords = m_mesh_property->getPolarCenter().getCoords();
    	 topoProprietes.addProperty (
    			 Utils::SerializedRepresentation::Property ("Découpage polaire de centre", coords));
        }

    if (m_mesh_property->isOrthogonal()){
    	int nbLayers = m_mesh_property->getNbLayers();
    	topoProprietes.addProperty (
    			Utils::SerializedRepresentation::Property ("Nombre de couches orthogonales", (long)nbLayers));

    	topoProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Discrétisation orthogonale coté",
                		(m_mesh_property->getSide()==0?std::string("départ"):std::string("arrivée"))));
    }

    // pour afficher les spécificités de certaines méthodes
    m_mesh_property->addDescription(topoProprietes);

#ifdef _DEBUG
    topoProprietes.addProperty (
    		Utils::SerializedRepresentation::Property ("MeshingData est prémaillé", getMeshingData()->isPreMeshed()));
//    topoProprietes.addProperty (
//    		Utils::SerializedRepresentation::Property ("time pour propriétés", (long)getMeshingProperty()->getModificationTime()));
//    topoProprietes.addProperty (
//    		Utils::SerializedRepresentation::Property ("time pour points", (long)getMeshingData()->getPointsTime()));
    topoProprietes.addProperty (
    		Utils::SerializedRepresentation::Property ("Arête est prémaillée", isPreMeshed()));
#endif

    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Est maillée", isMeshed()));

    if (isMeshed()){
        topoProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Nombre de noeuds", (long)m_mesh_data->nodes().size()));
    }


    description->addPropertiesSet (topoProprietes);


    // les relations vers les autres types d'entités topologiques
    // on cache l'existance des Edge et Face

    std::vector<Topo::Vertex* > vtx;
    getVertices(vtx);

    Utils::SerializedRepresentation  topoRelation ("Relations topologiques", "");

    Utils::SerializedRepresentation  vertices ("Sommets topologiques",
            TkUtil::NumericConversions::toStr(vtx.size()));
    for (std::vector<Topo::Vertex*>::iterator iter = vtx.begin( ); vtx.end( )!=iter; iter++)
        vertices.addProperty (
                Utils::SerializedRepresentation::Property (
                        (*iter)->getName ( ),  *(*iter)));
    topoRelation.addPropertiesSet (vertices);

    if (Internal::InternalPreferences::instance ( )._displayEdge.getValue ( )){
    	std::vector<Edge* > ed;
    	getEdges(ed);

    	Utils::SerializedRepresentation  edges ("Arêtes topologiques",
    			TkUtil::NumericConversions::toStr(ed.size()));
    	for (std::vector<Topo::Edge*>::iterator iter = ed.begin( ); ed.end( )!=iter; iter++)
    		edges.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));
    	topoRelation.addPropertiesSet (edges);

    }
    else {
    	std::vector<CoFace* > fa;
    	getCoFaces(fa);

    	Utils::SerializedRepresentation  cofaces ("Faces topologiques",
    			TkUtil::NumericConversions::toStr(fa.size()));
    	for (std::vector<Topo::CoFace*>::iterator iter = fa.begin( ); fa.end( )!=iter; iter++)
    		cofaces.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));
    	topoRelation.addPropertiesSet (cofaces);
    }

    description->addPropertiesSet (topoRelation);

    std::vector<Group::Group1D*> grp;
    getGroups(grp);
    if (!grp.empty()){
    	Utils::SerializedRepresentation  groupe ("Relation vers les groupes",
    			TkUtil::NumericConversions::toStr(grp.size()));
    	for (std::vector<Group::Group1D*>::iterator iter = grp.begin( ); iter!=grp.end( ); ++iter)
    		groupe.addProperty (
    				Utils::SerializedRepresentation::Property (
    						(*iter)->getName ( ),  *(*iter)));

    	description->addPropertiesSet (groupe);

    } // end if (!grp.empty())

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
void CoEdge::
fuse(CoEdge* edge_B,
        Internal::InfoCommand* icmd)
{
    // recherche des couples de sommets confondus
    const std::vector<Topo::Vertex* > & vertices_A = getVertices();
    const std::vector<Topo::Vertex* > & vertices_B = edge_B->getVertices();
    std::map<Topo::Vertex*, Topo::Vertex*> corr_vertex_A_B;

    Vertex::findNearlyVertices(vertices_A, vertices_B, corr_vertex_A_B);

    std::vector<Topo::Vertex* > vertices_B_new;
    for (std::vector<Topo::Vertex* >::const_iterator iter = vertices_A.begin();
            iter != vertices_A.end(); ++iter){
        Topo::Vertex* hv(corr_vertex_A_B[*iter]);
        if (0 == hv){
			TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
            err << "CoEdge::fuse impossible, on ne trouve pas de sommet en corespondance avec le sommet "
                    <<(*iter)->getName();
            throw TkUtil::Exception(err);
        }
        vertices_B_new.push_back(hv);
    }

    // appel à la fusion
    fuse(edge_B, vertices_A, vertices_B_new, icmd);
}
/*----------------------------------------------------------------------------*/
void CoEdge::
fuse(CoEdge* edge_B,
        const std::vector<Topo::Vertex* >& l_sommets_A,
        const std::vector<Topo::Vertex* >& l_sommets_B,
        Internal::InfoCommand* icmd)
{
    // précondition: les listes de sommets doivent être de même taille
    if (l_sommets_A.size() != l_sommets_B.size()){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "CoEdge::fuse impossible avec des listes de sommets de tailles différentes ("
                <<l_sommets_A.size()<<" != "<<l_sommets_B.size()<<")";
        throw TkUtil::Exception(err);
    }

    // on effectue la fusion des sommets
    // (on ne fait que remplacer les sommets de l_sommets_B par ceux de l_sommets_A)
    std::vector<Topo::Vertex* >::const_iterator iterA = l_sommets_A.begin();
    std::vector<Topo::Vertex* >::const_iterator iterB = l_sommets_B.begin();
    for (; iterA != l_sommets_A.end(); ++iterA, ++iterB)
        (*iterA)->merge(*iterB, icmd);

    merge(edge_B, icmd);
}
/*----------------------------------------------------------------------------*/
void CoEdge::
saveCoEdgeMeshingProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);

        //std::cout<<"CoEdge::saveCoEdgeMeshingProperty pour "<<getName()<<", change = "<<(change?"True":"False")<<std::endl;

        if (change && isMeshed()){
        	TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        	message << "On ne peut pas modifier une arête alors qu'elle est déjà maillée.";
        	throw TkUtil::Exception (message);
        }

        if (m_save_mesh_property == 0){
        	m_save_mesh_property = m_mesh_property->clone();
        }
    } // end if (icmd)
}
/*----------------------------------------------------------------------------*/
void CoEdge::
switchCoEdgeMeshingProperty(Internal::InfoCommand* icmd, CoEdgeMeshingProperty* prop)
{
	if (isMeshed()){
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message << "On ne peut pas modifier une arête ("<<getName()<<") alors qu'elle est déjà maillée.";
		throw TkUtil::Exception (message);
	}

    if (icmd && m_save_mesh_property == 0)
        m_save_mesh_property = m_mesh_property;
    else
        delete m_mesh_property;

    m_mesh_property = prop->clone();
    //std::cout<<"CoEdge::switchCoEdgeMeshingProperty pour "<<getName()<<std::endl;
    if (icmd)
        icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);

    m_mesh_property->updateModificationTime();
}
/*----------------------------------------------------------------------------*/
void CoEdge::
saveCoEdgeTopoProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
#ifdef _DEBUG2
        std::cout<<"CoEdge::saveCoEdgeTopoProperty pour "<<getName()<<", valeur initiale: "
        		<<Internal::InfoCommand::type2String(icmd->getTopoInfoEntity()[this])<<std::endl;
#endif
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
#ifdef _DEBUG2
        std::cout<<"CoEdge::saveCoEdgeTopoProperty pour "<<getName()<<", change = "<<(change?"True":"False")<<std::endl;
#endif
        if (change && m_save_topo_property == 0){
        	if (isMeshed()){
        		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        		message << "On ne peut pas modifier une arête alors qu'elle est déjà maillée.";
        		throw TkUtil::Exception (message);
        	}
        	else
        		m_save_topo_property = m_topo_property->clone();
        }
    }
}
/*----------------------------------------------------------------------------*/
void CoEdge::
saveCoEdgeMeshingData(Internal::InfoCommand* icmd)
{
    if (icmd) {
    	bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::OTHERMODIFIED);
        if (change && m_save_mesh_data == 0)
            m_save_mesh_data = m_mesh_data->clone();
    }
}
/*----------------------------------------------------------------------------*/
CoEdgeMeshingProperty* CoEdge::
setProperty(CoEdgeMeshingProperty* prop)
{
    CoEdgeMeshingProperty* tmp = m_mesh_property;
    m_mesh_property = prop;
    m_mesh_property->updateModificationTime();
#ifdef _DEBUG_MEMORY
    std::cout<<"CoEdge::setProperty() de "<<getName()<<", avec m_mesh_property en "<<m_mesh_property->getNbEdges()<<std::endl;
#endif
    return tmp;
}
/*----------------------------------------------------------------------------*/
CoEdgeTopoProperty* CoEdge::
setProperty(CoEdgeTopoProperty* prop)
{
    CoEdgeTopoProperty* tmp = m_topo_property;
    m_topo_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
CoEdgeMeshingData* CoEdge::
setProperty(CoEdgeMeshingData* prop)
{
    CoEdgeMeshingData* tmp = m_mesh_data;
    m_mesh_data = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void CoEdge::
saveInternals(CommandEditTopo* cet)
{
#ifdef _DEBUG2
    std::cout<<"CoEdge::saveInternals pour "<<getName()<<std::endl;
#endif
    TopoEntity::saveInternals (cet);

    if (m_save_mesh_property) {
        cet->addCoEdgeInfoMeshingProperty(this, m_save_mesh_property);
        m_save_mesh_property = 0;
    }
    if (m_save_topo_property) {
        cet->addCoEdgeInfoTopoProperty(this, m_save_topo_property);
        m_save_topo_property = 0;
    }
}
/*----------------------------------------------------------------------------*/
void CoEdge::
saveInternals(Mesh::CommandCreateMesh* ccm)
{
    if (m_save_mesh_data) {
        ccm->addCoEdgeInfoMeshingData(this, m_save_mesh_data);
        m_save_mesh_data = 0;
    }
}
/*----------------------------------------------------------------------------*/
void CoEdge::
check() const
{
    if (isDestroyed()){
    	TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
    	messErr << "On utilise une arête commune détruite : "<<getName();
    	throw TkUtil::Exception(messErr);
    }

    // si elle est maillée, c'est que c'est bon
    // et comme elle a peut-être été perturbée, vaut mieux ne pas faire plus
    if (isMeshed())
    	return;

    for (uint i=0; i<getNbVertices(); i++)
       getVertex(i)->check();

    for (uint i=0; i<getNbEdges(); i++)
        if (getEdge(i)->isDestroyed()){
            std::cerr<<"CoEdge::check() pour l'arête commune "<<getName()<<std::endl;
            throw TkUtil::Exception (TkUtil::UTF8String ("Une arête commune pointe sur une arête détruite", TkUtil::Charset::UTF_8));
        }

    if (getGeomAssociation() && getGeomAssociation()->isDestroyed()){
        TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur avec l'arête commune "<<getName()
                <<", elle pointe sur "<<getGeomAssociation()->getName()
                <<" qui est détruite !";
        throw TkUtil::Exception(messErr);
    }

    // validation des entités utilisées par la méthode de maillage pour cas interpolés
    if (getMeshLaw() == CoEdgeMeshingProperty::interpolate){

    	const EdgeMeshingPropertyInterpolate* empi = dynamic_cast<const EdgeMeshingPropertyInterpolate*>(getMeshingProperty());
    	CHECK_NULL_PTR_ERROR(empi);

    	if (empi->getType() == EdgeMeshingPropertyInterpolate::with_coedge_list){
    		std::vector<std::string> coedges = empi->getCoEdges();
    		for (uint i=0; i<coedges.size(); i++)
    			getContext().getLocalTopoManager().getCoEdge(coedges[i], true);
    	}
    	else if (empi->getType() == EdgeMeshingPropertyInterpolate::with_coface){
    		std::string coface = empi->getCoFace();
    		getContext().getLocalTopoManager().getCoFace(coface, true);
    	}
    }
    if (getMeshLaw() == CoEdgeMeshingProperty::globalinterpolate){

    	const EdgeMeshingPropertyGlobalInterpolate* empi = dynamic_cast<const EdgeMeshingPropertyGlobalInterpolate*>(getMeshingProperty());
    	CHECK_NULL_PTR_ERROR(empi);

    	std::vector<std::string> coedges = empi->getFirstCoEdges();
    	for (uint i=0; i<coedges.size(); i++)
    		getContext().getLocalTopoManager().getCoEdge(coedges[i], true);

    	coedges = empi->getSecondCoEdges();
    	for (uint i=0; i<coedges.size(); i++)
    		getContext().getLocalTopoManager().getCoEdge(coedges[i], true);
    }

    // vérification qu'elle est maillable
    std::vector<Utils::Math::Point> points;
    getPoints(points);
}
/*----------------------------------------------------------------------------*/
bool CoEdge::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const CoEdge & e)
{
    o << "   " << e.getName() << " (uniqueId "<<e.getUniqueId()<<")"
      << (e.isDestroyed()?" (DETRUITE)":"")
      << (e.isEdited()?" (EN COURS D'EDITION)":"")
      << " discrétisée en "<<(short)(e.getNbMeshingEdges());

    if (e.getMeshingProperty()->isPolarCut())
    	o << ", polaire de centre "<<e.getMeshingProperty()->getPolarCenter();

    if (e.getGeomAssociation()){
        o << ", projetée sur " << e.getGeomAssociation()->getName()
        <<(e.getGeomAssociation()->isDestroyed()?" (DETRUITE)":"");

        std::vector<std::string> gn;
        e.getGeomAssociation()->getGroupsName(gn);
        if (!gn.empty()) {
            o << " (groupes:";
            for (size_t i=0; i<gn.size(); i++)
                o << " "<<gn[i];
            o << ")";
        }
        else {
            o << " (sans groupe)";
        }
    }
    else
        o << ", sans projection";

    o << ", avec comme sommets:\n";
    const std::vector<Topo::Vertex* > & vertices = e.getVertices();
    for (uint i=0; i<vertices.size();i++)
        o <<"    "<<*vertices[i]<<"\n";

    const std::vector<Topo::Edge* > & edges = e.getEdges();
    if (edges.empty()){
        o << "    cette arête commune ne pointe sur aucune arête.";
    }
    else {
        o << "    cette arête commune pointe sur les arêtes :";
        for (uint i=0; i<edges.size();i++){
            o << "  " << edges[i]->getName();
            if (edges[i]->isDestroyed())
                o << "  [DETRUITE] ";
        }
    }
    o << "\n";

    return o;
}
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o, const CoEdge & e)
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << e;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
Topo::TopoInfo CoEdge::getInfos() const
{
	Topo::TopoInfo infos;
	infos.name = getName();
	infos.dimension = getDim();
	getVertices(infos.incident_vertices);
	//getCoEdges(infos.incident_coedges);
	getEdges(infos.incident_edges);
	getCoFaces(infos.incident_cofaces);
	getBlocks(infos.incident_blocks);
	infos.geom_entity = getGeomAssociation();
	return infos;
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_getNodes
void CoEdge::
getNodes(Topo::Vertex* v1, Topo::Vertex* v2,
        std::vector<gmds::Node> &vectNd)
{
#ifdef _DEBUG_getNodes
	std::cout<<"getNodes pour "<<getName()<<" entre "<<v1->getName()<<" et "<<v2->getName()<<std::endl;
#endif
	vectNd.clear();

    if (!m_mesh_data->isMeshed()){
        TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        message << "CoEdge::getNodes( \""
                << v1->getName() << "\", "
                << v2->getName() << "\") avec l'arête "
                << getName()<<" ne peut se faire car l'arête n'est pas maillée";
        throw TkUtil::Exception (message);
    }

    gmds::IGMesh& gmds_mesh = getContext().getLocalMeshManager().getMesh()->getGMDSMesh();

    if (v1 == getVertex(0) && v2 == getVertex(1)){
    	for (std::vector<gmds::TCellID>::iterator iter = m_mesh_data->nodes().begin();
    			iter != m_mesh_data->nodes().end(); ++iter)
    		vectNd.push_back(gmds_mesh.get<gmds::Node>(*iter));
        //vectNd.insert(vectNd.begin(), m_mesh_data->nodes().begin(), m_mesh_data->nodes().end());
#ifdef _DEBUG_getNodes
	std::cout<<" sens normal"<<std::endl;
#endif
    }
    else if (v2 == getVertex(0) && v1 == getVertex(1)) {
    	for (std::vector<gmds::TCellID>::reverse_iterator iter = m_mesh_data->nodes().rbegin();
    	    			iter != m_mesh_data->nodes().rend(); ++iter)
    	    		vectNd.push_back(gmds_mesh.get<gmds::Node>(*iter));

        //vectNd.insert(vectNd.begin(), m_mesh_data->nodes().begin(), m_mesh_data->nodes().end());
        //std::reverse(vectNd.begin(), vectNd.end());
#ifdef _DEBUG_getNodes
	std::cout<<" sens inversé"<<std::endl;
#endif
    }
    else {
        TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        message << "CoEdge::getNodes( \""
                << v1->getName() << "\", "
                << v2->getName() << "\") avec l'arête "
                << getName() << " qui a comme sommets:";
        for (uint i=0; i<getNbVertices();i++)
            message <<"  "<<*getVertex(i);
        message << ", a échoué";
        throw TkUtil::Exception (message);
    }

#ifdef _DEBUG_getNodes
	std::cout<<"getNodes retourne pour "<<getName()<<std::endl;
	for (std::vector<gmds::Node>::iterator iter = vectNd.begin();
			iter != vectNd.end(); ++iter)
		std::cout<<"  Nd "<<(*iter).getID()<<" : ["<<(*iter).X()<<", "<<(*iter).Y()<<", "<<(*iter).Z()<<"]"<<std::endl;
#endif

//    TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
//    message << "CoEdge::getNodes( \""
//            << v1->getName() << "\", "
//            << v2->getName() << "\") avec l'arête "
//            << getName()<<" donne un vecteur de "<<vectNd.size()<<" noeuds";
//    std::cout<<message<<std::endl;
//    std::cout<<"m_mesh_data->nodes().size() = "<<m_mesh_data->nodes().size()<<std::endl;
}
/*----------------------------------------------------------------------------*/
uint CoEdge::
getNbVertices() const
{
#ifdef _DEBUG
    if (isDestroyed()){
        TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
        message << "CoEdge::getNbVertices() pour arête "<<getName()<<" détruite !";
        log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));
    }
#endif
    return m_topo_property->getVertexContainer().getNb();
}
/*----------------------------------------------------------------------------*/
void CoEdge::getAllVertices(std::vector<Vertex* >& vertices, const bool unique) const
{
    vertices.clear();
    std::list<Topo::Vertex*> l_v;
    const std::vector<Vertex* > & local_vertices = getVertices();

    l_v.insert(l_v.end(), local_vertices.begin(), local_vertices.end());

    l_v.sort(Utils::Entity::compareEntity);
    l_v.unique();

    vertices.insert(vertices.end(),l_v.begin(),l_v.end());
}
/*----------------------------------------------------------------------------*/
void CoEdge::
getCoFaces(std::vector<CoFace* >& cofaces) const
{
    std::list<Topo::CoFace*> l_f;

    std::vector<Edge* > edges;
    getEdges(edges);

    for (std::vector<Edge* >::iterator iter1 = edges.begin();
            iter1 != edges.end(); ++iter1){
        std::vector<CoFace* > loc_cofaces;
        (*iter1)->getCoFaces(loc_cofaces);

        l_f.insert(l_f.end(), loc_cofaces.begin(), loc_cofaces.end());
    }

    l_f.sort(Utils::Entity::compareEntity);
    l_f.unique();

    cofaces.insert(cofaces.end(),l_f.begin(),l_f.end());
}
/*----------------------------------------------------------------------------*/
uint CoEdge::
getNbCofaces() const
{
	std::vector<CoFace* > cofaces;
	getCoFaces(cofaces);
	return cofaces.size();
}
/*----------------------------------------------------------------------------*/
bool CoEdge::
isEdited() const
{
    return TopoEntity::isEdited()
    || m_save_topo_property != 0
    || m_save_mesh_property != 0
    || m_save_mesh_data != 0;
}
/*----------------------------------------------------------------------------*/
double CoEdge::
computeRatio(const Utils::Math::Point& pt)
{
	// fait la recherche en utilisant les points du maillage, si possible

	try {
		std::vector<Utils::Math::Point> points;
		getPoints(points);
		uint indice = Utils::Math::findNearestPoint(points, pt);
#ifdef _DEBUG_SPLIT
		std::cout<<"computeRatio pour "<<getName()<<" et point "<<pt<<" => "<<(double)indice / (double) getNbMeshingEdges()<<std::endl;
#endif
		return (double)indice / (double) getNbMeshingEdges();
	}
	catch (TkUtil::Exception &e){
		// si la création des points ne peut se faire, on procède autrement
		// cela peut apparaitre si l'arête n'est que partiellement sur une surface par ex

		if (getNbVertices() != 2)
			throw TkUtil::Exception (TkUtil::UTF8String ("Pas possible de projeter un point sur arête avec autre chose que 2 sommets", TkUtil::Charset::UTF_8));

		Point pt0 =  getVertex(0)->getCoord();
		Point pt1 =  getVertex(1)->getCoord();

		double norme = (pt1-pt0).norme2();
		double scale = (pt-pt0).dot(pt1-pt0);

#ifdef _DEBUG_SPLIT
		std::cout<<"computeRatio entre "<<getName()<<" et "<<pt<<" => "<<scale/norme<<std::endl;
#endif

		return scale/norme;
	}

//	if (getGeomAssociation()){
//		Geom::Curve* curve = dynamic_cast<Geom::Curve*> (getGeomAssociation());
//		if (curve){
//			double param1 = 0;
//			double param2 = 0;
//			curve->getParameter(getVertex(0)->getCoord(), param1);
//			curve->getParameter(getVertex(getNbVertices()==1?0:1)->getCoord(), param2);
//
//			Point ptProj;
//			curve->project(pt, ptProj);
//
//			double paramPt = 0;
//			curve->getParameter(ptProj, paramPt);
//
//#ifdef _DEBUG_SPLIT
//			std::cout<<"computeRatio entre "<<curve->getName()<<" et "<<pt<<" => "<<(paramPt-param1)/(param2-param1)<<std::endl;
//#endif
//			return (paramPt-param1)/(param2-param1);
//		}
//		else {
//			// projection certainement sur une surface
//
//			// on recherche le ratio à l'aide de la discrétisation projetée de l'arête
//			std::vector<Utils::Math::Point> points;
//			getPoints(getMeshingProperty(), points, true);
//
//			// vrai dès que l'on trouve le segment sur lequel est projeté le pt
//			bool trouve = false;
//			double long_tot = 0.0;
//			double ratio_tot = 0.0;
//			for (uint i=0; i<points.size()-1; i++){
//
//				Point& pt0 = points[i];
//				Point& pt1 = points[i+1];
//
//				double norme2 = (pt1-pt0).norme2();
//				double norme = std::sqrt(norme2);
//				if (!trouve){
//					double scale = (pt-pt0).dot(pt1-pt0);
//					double ratio = scale/norme2;
//					//std::cout<<"Pt["<<i<<"] "<<pt0<<", ratio = "<<ratio<<std::endl;
//
//					if (ratio >= 0 && ratio <= 1.0){
//						trouve = true;
//						ratio_tot = long_tot + ratio*norme;
//						//std::cout<<"   ratio_tot = "<<ratio_tot<<std::endl;
//					}
//				} // end if (!trouve)
//
//				long_tot += norme;
//				//std::cout<<"   long_tot = "<<long_tot<<", norme = "<<norme<<std::endl;
//
//			} // end for i<points.size()-1
//
//#ifdef _DEBUG_SPLIT
//			std::cout<<"computeRatio entre "<<getGeomAssociation()->getName()<<" et "<<pt<<" => "<<ratio_tot/long_tot<<std::endl;
//#endif
//			return ratio_tot/long_tot;
//
//		} // end else / if (curve)
//	}
//
//	// s'il n'y a pas de courbe, alors on projette sur la droite entre les sommets de la coedge
//	// et on calcul le ratio à partir de ce point
//	if (getNbVertices() != 2)
//		throw TkUtil::Exception (TkUtil::UTF8String ("Pas possible de projeter un point sur arête avec autre chose que 2 sommets", TkUtil::Charset::UTF_8));
//
//	Point pt0 =  getVertex(0)->getCoord();
//	Point pt1 =  getVertex(1)->getCoord();
//
//	double norme = (pt1-pt0).norme2();
//	double scale = (pt-pt0).dot(pt1-pt0);
//
//#ifdef _DEBUG_SPLIT
//	std::cout<<"computeRatio entre "<<getName()<<" et "<<pt<<" => "<<scale/norme<<std::endl;
//#endif
//
//	return scale/norme;
}
/*----------------------------------------------------------------------------*/
void CoEdge::getGroupsName (std::vector<std::string>& gn, bool byGeom, bool byTopo) const
{
	if (byGeom)
		TopoEntity::getGroupsName(gn, byGeom, byTopo);

	if (byTopo)
		for (uint i = 0; i<m_topo_property->getGroupsContainer().size(); ++i)
		        gn.push_back(m_topo_property->getGroupsContainer().get(i)->getName());
}
/*----------------------------------------------------------------------------*/
unsigned long CoEdge::getNbInternalMeshingNodes()
{
	if (getNbVertices() == 1 || getVertex(0) == getVertex(1))
		return getNbMeshingEdges();
	else
		return getNbMeshingEdges() - 1;
}
/*----------------------------------------------------------------------------*/
void CoEdge::
detectLoopReference(const Topo::CoEdge* coedge_dep, std::vector<Topo::CoEdge*>& coedges_ref, std::set<const CoEdge*>& filtre_coedges) const
{
#ifdef _DEBUG2
	std::cout<<"detectLoopReference("<<coedge_dep->getName()<<", [";
	for (uint i=0; i<coedges_ref.size(); i++)
		std::cout<<coedges_ref[i]->getName()<<" ";
	std::cout<<"])"<<std::endl;
#endif


	for (uint i=0; i<coedges_ref.size(); i++){

		if (filtre_coedges.find(coedges_ref[i]) != filtre_coedges.end()){
#ifdef _DEBUG2
			std::cout<<" "<<coedges_ref[i]->getName()<<" déjà validée"<<std::endl;
#endif
			continue;
		}
		if (coedge_dep == coedges_ref[i]){
			TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
			message << "Erreur dans l'utilisation de l'interpolation, l'arête "
					<<coedge_dep->getName()
					<<" se référence elle même indirectement";
			throw TkUtil::Exception (message);
		}

		CoEdgeMeshingProperty* dni = coedges_ref[i]->getMeshingProperty();

		if (dni->getMeshLaw() == CoEdgeMeshingProperty::interpolate){
			EdgeMeshingPropertyInterpolate* interpol = dynamic_cast<EdgeMeshingPropertyInterpolate*>(dni);
			CHECK_NULL_PTR_ERROR(interpol);
			if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coedge_list){
				// recherche des arêtes par rapport aux quelles se fait l'interpolation
				std::vector<std::string> coedges_names = interpol->getCoEdges();

				for (uint j=0; j<coedges_names.size(); j++)
					if (coedges_names[j] == coedge_dep->getName()){
						TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
						message << "Erreur dans l'utilisation de l'interpolation, les arêtes "
								<<coedges_names[j]<<" et "<<coedges_ref[i]->getName()
								<<" se référencent mutuellement";
						throw TkUtil::Exception (message);
					} // if (coedges_names[j] == coedge->getName())
					else if (coedges_names[j] == coedges_ref[i]->getName()) {
						TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
						message << "Erreur dans l'utilisation de l'interpolation, l'arête "
								<<coedges_names[j]
								<<" se référence elle même";
						throw TkUtil::Exception (message);
					}
					else {
						std::vector<Topo::CoEdge*> coedges;
						getCoEdges(coedges_names, coedges);
						std::set<const CoEdge*> filtre_coedges; // pour éviter d'y passer trop de temps
						detectLoopReference(coedge_dep, coedges, filtre_coedges);
						updateModificationTime(coedges_ref[i], coedges);
					}
			} // if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coedge_list)
			else if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coface) {

				std::string coface_name = interpol->getCoFace();

				CoFace* coface = 0;
				try {
					coface = getContext().getLocalTopoManager().getCoFace(coface_name, false);
				}
				catch (Utils::IsDestroyedException &e){
					TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
					message << "Erreur, l'arête "<<getName()<<" a une discrétisation basée sur l'interpolation en utilisant la face "
							<< coface_name <<" qui est détruite";
					throw TkUtil::Exception (message);
				}

				if (coface == 0){
					TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
					message << "Erreur, l'arête "<<getName()<<" a une discrétisation basée sur l'interpolation en utilisant la face "
							<< coface_name <<" qui n'existe pas (ou n'existe plus)";
					throw TkUtil::Exception (message);
				}

				detectLoopReference(coedge_dep, coedges_ref[i], coface, filtre_coedges);

			} // else if (interpol->getType() == EdgeMeshingPropertyInterpolate::with_coface)
		} // if (dni->getMeshLaw() == CoEdgeMeshingProperty::interpolate)
		filtre_coedges.insert(coedges_ref[i]);
	} // for (uint i=0; i<coedges_ref.size(); i++)
}
/*----------------------------------------------------------------------------*/
void CoEdge::
detectLoopReference(const Topo::CoEdge* coedge_dep, const Topo::CoEdge* coedge, Topo::CoFace* coface, std::set<const CoEdge*>& filtre_coedges) const
{
#ifdef _DEBUG2
	std::cout<<"detectLoopReference("<<coedge_dep->getName()<<", "<<coedge->getName()<<", "<<coface->getName()<<")"<<std::endl;
#endif
	// si cette arête a déjà été validée, on passe
	if (filtre_coedges.find(coedge) != filtre_coedges.end()){
#ifdef _DEBUG2
		std::cout<<" déjà validée"<<std::endl;
#endif
		return;
	}

	// rechercher l'edge dans laquelle est cette coedge, à partir de la coface.
	Edge* edge_dep = 0;
	try {
		edge_dep = coface->getEdgeContaining(coedge);
	}
	catch (TkUtil::Exception &e){
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message << "Erreur avec l'interpolation de l'arête "
				<< coedge->getName()
				<<", elle référence la face "<<coface->getName()
				<<" alors qu'elles ne sont pas en relation.";
		throw TkUtil::Exception (message);
	}

	Edge* edge_ref = coface->getOppositeEdge(edge_dep);
	if (edge_ref == 0){
		TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
		message << "Erreur avec l'interpolation de l'arête "
				<< coedge->getName()
				<<", elle référence la face "<<coface->getName()
				<<" mais on ne trouve pas d'arête en face.";
		throw TkUtil::Exception (message);
	}

	// vérifie que les coedges ne dépendent pas de coedge_dep
	std::vector<Topo::CoEdge*> coedges_ref;
	edge_ref->getCoEdges(coedges_ref);
	detectLoopReference(coedge_dep, coedges_ref, filtre_coedges);
	updateModificationTime(coedge, coedges_ref);
}
/*----------------------------------------------------------------------------*/
void CoEdge::updateModificationTime(const Topo::CoEdge* coedge_ref, std::vector<Topo::CoEdge*>& coedges) const
{
	for (uint i=0; i<coedges.size(); i++){
		Topo::CoEdge* coedge = coedges[i];
		if (coedge_ref->getMeshingProperty()->getModificationTime() < coedge->getMeshingProperty()->getModificationTime())
			coedge_ref->getMeshingProperty()->getModificationTime().set(coedge->getMeshingProperty()->getModificationTime());
	}
}
/*----------------------------------------------------------------------------*/
void CoEdge::getCoEdges(std::vector<std::string>& coedges_names, std::vector<Topo::CoEdge*>& coedges) const
{
	for (uint i=0; i<coedges_names.size(); i++){
		CoEdge* coedge = getContext().getLocalTopoManager().getCoEdge(coedges_names[i], false);

		if (coedge == 0){
			TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
			message << "Erreur, l'arête "<<getName()
					<<" a une discrétisation basée sur celle de "<<coedges_names[i]
					<<" qui n'existe pas (ou n'existe plus)";
			throw TkUtil::Exception (message);
		}

		// cas où on dépend de soit même
		if (coedge == this){
			TkUtil::UTF8String   message (TkUtil::Charset::UTF_8);
			message << "Erreur, l'arête "<<getName()<<" a une discrétisation basée sur elle même \n";
			message << "(peut-être avez-vous utilisé setParallelMeshingProperty au lieu de setMeshingProperty)";
			throw TkUtil::Exception (message);
		}
		coedges.push_back(coedge);
	}
}
/*----------------------------------------------------------------------------*/
void CoEdge::setGeomAssociation(Geom::GeomEntity* ge)
{
	m_mesh_property->updateModificationTime();
	TopoEntity::setGeomAssociation(ge);
}
/*----------------------------------------------------------------------------*/
TopoProperty* CoEdge::setProperty(TopoProperty* new_tp)
{
	m_mesh_property->updateModificationTime();
	return TopoEntity::setProperty(new_tp);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
