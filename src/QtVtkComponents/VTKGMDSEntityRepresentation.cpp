/**
 * \file        VTKGMDSEntityRepresentation.cpp
 * \author      Charles PIGNEROL
 * \date        28/11/2011
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtVtkComponents/VTKGMDSEntityRepresentation.h"
#include "QtVtkComponents/VTKMgx3DEntityRepresentation.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <Utils/DisplayProperties.h>

#include "Mesh/MeshImplementation.h"
#include "Mesh/Cloud.h"
#include "Mesh/Line.h"
#include "Mesh/Surface.h"
#include "Mesh/Volume.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>

#include <vtkCellArray.h>
#include <vtkCellType.h>
#include <vtkDoubleArray.h>
#include <vtkExtractEdges.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkLine.h>
#include <vtkTriangle.h>
#include <vtkVertex.h>

#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;

//#define _DEBUG_VTKGMDSEntityRepresentation

namespace Mgx3D
{

namespace QtVtkComponents
{

// ===========================================================================
//                      LA CLASSE VTKGMDSEntityRepresentation
// ===========================================================================

VTKGMDSEntityRepresentation::VTKGMDSEntityRepresentation(Entity& entity) :
        VTKEntityRepresentation(entity)
{
} // VTKGMDSEntityRepresentation::VTKGMDSEntityRepresentation

VTKGMDSEntityRepresentation::VTKGMDSEntityRepresentation(
        VTKGMDSEntityRepresentation& ver) :
        VTKEntityRepresentation(*(ver.getEntity()))
{
    MGX_FORBIDDEN(
            "VTKGMDSEntityRepresentation copy constructor is not allowed.");
} // VTKGMDSEntityRepresentation::VTKGMDSEntityRepresentation

VTKGMDSEntityRepresentation& VTKGMDSEntityRepresentation::operator =(
        const VTKGMDSEntityRepresentation& er)
{
    MGX_FORBIDDEN(
            "VTKGMDSEntityRepresentation assignment operator is not allowed.");
    if (&er != this)
    {
    } // if (&er != this)

    return *this;
} // VTKGMDSEntityRepresentation::operator =

VTKGMDSEntityRepresentation::~VTKGMDSEntityRepresentation()
{
    destroyRepresentations (true);
} // VTKGMDSEntityRepresentation::~VTKGMDSEntityRepresentation

void VTKGMDSEntityRepresentation::createCloudRepresentation()
{
    if ((0 != _cloudGrid) || (0 != _cloudMapper) || (0 != _cloudActor))
    {
        INTERNAL_ERROR(exc, "Représentation déjà créée.",
                "VTKGMDSEntityRepresentation::createCloudRepresentation");
        throw exc;
    } // if ((0 != _cloudGrid) || ...

    // Appel à la méthode spécialisée GMDS de création du nuage VTK
    Mesh::MeshEntity* meshEntity = dynamic_cast<Mesh::MeshEntity*>(getEntity());
    if (0 == meshEntity)
    {
        INTERNAL_ERROR(exc, "Echec lors de la conversion en MeshEntity.",
                "VTKGMDSEntityRepresentation::createCloudRepresentation");
        throw exc;
    }

    VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation(
            meshEntity);

    vtkPoints* points = _cloudGrid->GetPoints ( );
    doShrink(points);

} // VTKGMDSEntityRepresentation::createCloudRepresentation

void VTKGMDSEntityRepresentation::createSurfacicRepresentation()
{
    if ((0 != _surfacicGrid) || (0 != _surfacicMapper) || (0 != _surfacicActor))
    {
        INTERNAL_ERROR(exc, "Représentation déjà créée.",
                "VTKGMDSEntityRepresentation::createSurfacicRepresentation")
        throw exc;
    } // if ((0 != _surfacicMapper) || ...

    // Appel à la méthode spécialisée GMDS de création de surface VTK
    if (0 == _surfacicGrid)
    {
        Mesh::MeshEntity* meshEntity =
                dynamic_cast<Mesh::MeshEntity*>(getEntity());
    	//std::cout<<"VTKGMDSEntityRepresentation::createSurfacicRepresentation pour "<<getEntity()->getName()<<" avec shrink à "<<getEntity ( )->getDisplayProperties().getShrinkFactor()<<std::endl;
        if (0 == meshEntity)
        {
            INTERNAL_ERROR(exc, "Echec lors de la conversion en MeshEntity.",
                    "VTKGMDSEntityRepresentation::createSurfacicRepresentation");
            throw exc;
        }

        VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation(
                meshEntity);
    } // if (0 == _surfacicGrid)

    _surfacicMapper = vtkDataSetMapper::New();
#ifndef VTK_5
    _surfacicMapper->SetInputData(_surfacicGrid);
#else	// VTK_5
    _surfacicMapper->SetInput(_surfacicGrid);
#endif	// VTK_5
    _surfacicMapper->ScalarVisibilityOff();
#if	VTK_MAJOR_VERSION < 8
    _surfacicMapper->SetImmediateModeRendering(!Internal::Resources::instance ( )._useDisplayList.getValue ( ));
#endif	// VTK_MAJOR_VERSION < 8
    _surfacicActor = VTKMgx3DActor::New();
    _surfacicActor->SetEntity(getEntity());
	_surfacicActor->SetRepresentationType (DisplayRepresentation::SOLID);
    CHECK_NULL_PTR_ERROR(getEntity ( ))
    const DisplayProperties& properties = getEntity()->getDisplayProperties();
    const Color& surfacicColor = properties.getSurfacicColor();
    _surfacicActor->GetProperty()->SetColor(surfacicColor.getRed()/255.,
            surfacicColor.getGreen()/255., surfacicColor.getBlue()/255.);
    _surfacicActor->SetMapper(_surfacicMapper);

    vtkPoints* points = _surfacicGrid->GetPoints ( );
    doShrink(points);

//_surfacicMapper->PrintSelf (cout,*vtkIndent::New( ));
} // VTKGMDSEntityRepresentation::createSurfacicRepresentation

void VTKGMDSEntityRepresentation::createVolumicRepresentation()
{
    //std::cout<<"VTKGMDSEntityRepresentation::createVolumicRepresentation()"<<std::endl;

    if ((0 != _volumicGrid)) // || (0 != _volumicMapper) || (0 != _volumicActor))
    {
        INTERNAL_ERROR(exc, "Représentation déjà créée.",
                "VTKGMDSEntityRepresentation::createVolumicRepresentation")
        throw exc;
    } // if ((0 != _volumicMapper) || ...

    // Appel à la méthode spécialisée GMDS de création de volume VTK
    if (0 == _volumicGrid)
    {
        Mesh::MeshEntity* meshEntity =
                dynamic_cast<Mesh::MeshEntity*>(getEntity());
        if (0 == meshEntity)
        {
            INTERNAL_ERROR(exc, "Echec lors de la conversion en MeshEntity.",
                    "VTKGMDSEntityRepresentation::createVolumicRepresentation");
            throw exc;
        }

        VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation(
                meshEntity);
    } // if (0 == _volumicGrid)

    // on n'affiche pas les mailles internes quand la vue filaire est demandée
    // la vue filaire se fera sur le maillage volumique et non pas sur le maillage surfacique
    if (0 == (getRepresentationMask() & CURVES)) {
        _volumicMapper = vtkDataSetMapper::New();

#ifndef VTK_5
        _volumicMapper->SetInputData(_volumicGrid);
#else	// VTK_5
        _volumicMapper->SetInput(_volumicGrid);
#endif	// VTK_5
        _volumicMapper->ScalarVisibilityOff();
#if	VTK_MAJOR_VERSION < 8        
        _volumicMapper->SetImmediateModeRendering(!Internal::Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8
        _volumicActor = VTKMgx3DActor::New();
        _volumicActor->SetEntity(getEntity());
		_volumicActor->SetRepresentationType (DisplayRepresentation::SOLID);
        CHECK_NULL_PTR_ERROR(getEntity ( ))
        const DisplayProperties& properties =
                getEntity()->getDisplayProperties();
        const Color& volumicColor = properties.getVolumicColor();
        _volumicActor->GetProperty()->SetColor(volumicColor.getRed()/255.,
                volumicColor.getGreen()/255., volumicColor.getBlue()/255.);
        _volumicActor->SetMapper(_volumicMapper);
    }

    vtkPoints* points = _volumicGrid->GetPoints ( );
    doShrink(points);

//_volumicMapper->PrintSelf (cout,*vtkIndent::New( ));
} // VTKGMDSEntityRepresentation::createVolumicRepresentation

void VTKGMDSEntityRepresentation::createWireRepresentation()
{
    if ( //(0 != _wireGrid) || le _wireGrid n'est pas renseigné
    (0 != _wireMapper) || (0 != _wireActor))
    {
        INTERNAL_ERROR(exc, "Représentation déjà créée.",
                "VTKGMDSEntityRepresentation::createWireRepresentation")
        throw exc;
    } // if ((0 != _wireGrid) || ...

    CHECK_NULL_PTR_ERROR(getEntity ( ))
    switch (getEntity ( )->getDim ( ))
    {
        case 0 :         
        {
            INTERNAL_ERROR(exc, "Entité de type Nuage.", "VTKGMDSEntityRepresentation::createWireRepresentation")
            throw exc;
        }
        case 1 :
        {
             Mesh::MeshEntity* meshEntity = dynamic_cast<Mesh::MeshEntity*>(getEntity ( ));
             createMeshEntityLineRepresentation (meshEntity);
             return;
        }
    }	// switch (getEntity ( )->getDim ( ))

    // nous allons avoir besoin de _surfacicGrid ou _volumicGrid
    if (0 == _surfacicGrid && 0 == _volumicGrid)
    {
        // Appel à la méthode spécialisée GMDS de création de surface VTK
        Mesh::MeshEntity* meshEntity =
                dynamic_cast<Mesh::MeshEntity*>(getEntity());
        if (0 == meshEntity)
        {
            INTERNAL_ERROR(exc, "Echec lors de la conversion en MeshEntity.",
                    "VTKGMDSEntityRepresentation::createWireRepresentation")
            throw exc;
        } // if (0 == meshEntity)

        VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation(
                meshEntity);
        CHECK_NULL_PTR_ERROR(_surfacicGrid)
    } // if (0 == _surfacicGrid)

    UTF8String	message1 (Charset::UTF_8);
    message1 << "VTKGMDSEntityRepresentation::createWireRepresentation pour "
            << getEntity()->getName();
    getEntity()->log(TkUtil::TraceLog(message1, TkUtil::Log::TRACE_5));

    vtkExtractEdges* edgesExtractor = vtkExtractEdges::New();

    // on créé la représentation sur le volume s'il existe
    if (_volumicGrid)
    {
#ifndef VTK_5
        edgesExtractor->SetInputData(_volumicGrid);
#else	// VTK_5
        edgesExtractor->SetInput(_volumicGrid);
#endif	// VTK_5
    }
    else if (_surfacicGrid)
    {
#ifndef VTK_5
        edgesExtractor->SetInputData(_surfacicGrid);
#else	// VTK_5
        edgesExtractor->SetInput(_surfacicGrid);
#endif	// VTK_5
    }
    else
    {
        INTERNAL_ERROR(exc,
                "Pas de représentation surfacique ou volumique de disponible",
                "VTKGMDSEntityRepresentation::createWireRepresentation")
        throw exc;
    }

    _wireMapper = vtkDataSetMapper::New();
#ifndef VTK_5
    _wireMapper->SetInputConnection (edgesExtractor->GetOutputPort ( ));
#else	// VTK_5
    _wireMapper->SetInput(edgesExtractor->GetOutput());
#endif	// VTK_5
    _wireMapper->ScalarVisibilityOff();
#if	VTK_MAJOR_VERSION < 8
    _wireMapper->SetImmediateModeRendering(!Internal::Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8

    _wireActor = VTKMgx3DActor::New();
    _wireActor->SetEntity(getEntity());
	_wireActor->SetRepresentationType (DisplayRepresentation::WIRE);
    const DisplayProperties& properties = getEntity()->getDisplayProperties();
    const Color& wireColor = properties.getWireColor();
    _wireActor->GetProperty()->SetColor(wireColor.getRed()/255.,
            wireColor.getGreen()/255., wireColor.getBlue()/255.);
    _wireActor->SetMapper(_wireMapper);

    // affichage en pointillés
    Internal::InternalEntity* ie = dynamic_cast<Internal::InternalEntity*>(getEntity());
    if (0 == ie)
    {
    	INTERNAL_ERROR(exc, "Echec lors de la conversion en InternalEntity.",
    			"VTKGMDSEntityRepresentation::createWireRepresentation")
        throw exc;
    } // if (0 == ie)
    if (ie->getContext().getRatioDegrad() > 1)
    {
        _wireActor->GetProperty()->SetLineStipplePattern(
                createLineStipplePattern(2, 1));
        _wireActor->GetProperty()->SetLineStippleRepeatFactor(5);
    }

    edgesExtractor->Delete();
// que détruire ???

//    vtkPoints* points = _volumicGrid->GetPoints ( );
//    doShrink(points);

} // VTKGMDSEntityRepresentation::createWireRepresentation

void VTKGMDSEntityRepresentation::createIsoWireRepresentation()
{
    if ((0 != _isoWireGrid) || (0 != _isoWireMapper) || (0 != _isoWireActor))
    {
        INTERNAL_ERROR(exc, "Représentation déjà créée.",
                "VTKGMDSEntityRepresentation::createIsoWireRepresentation")
        throw exc;
    } // if ((0 != _isoWireGrid) || ...

    throw TkUtil::Exception(
            "VTKGMDSEntityRepresentation::createIsoWireRepresentation() indisponible");
} // VTKGMDSEntityRepresentation::createIsoWireRepresentation

void VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation(
        Mesh::MeshEntity* meshEntity)
{
    if ((0 != _cloudGrid) || (0 != _cloudMapper) || (0 != _cloudActor))
    {
        INTERNAL_ERROR(exc, "Représentation déjà créée.",
                "VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation");
        throw exc;
    } // if ((0 != _cloudGrid) || ...

    if (meshEntity->isDestroyed())
    {
        INTERNAL_ERROR(exc, "L'entité est détruite",
                "VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation")
    }

    // Récupération du maillage GMDS
    Mesh::MeshItf* meshItf = meshEntity->getMeshManager().getMesh();
    Mesh::MeshImplementation* meshImpl =
            dynamic_cast<Mesh::MeshImplementation*>(meshItf);
    if (0 == meshImpl)
    {
        INTERNAL_ERROR(exc,
                "Echec lors de la conversion en MeshImplementation.",
                "VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation");
        throw exc;

    }

    gmds::IGMesh& gmdsMesh = meshImpl->getGMDSMesh();

    try
    {
        _cloudGrid = vtkUnstructuredGrid::New();
        _cloudMapper = vtkDataSetMapper::New();
#ifndef VTK_5
        _cloudMapper->SetInputData(_cloudGrid);
#else	// VTK_5
        _cloudMapper->SetInput(_cloudGrid);
#endif	// VTK_5
        _cloudMapper->ScalarVisibilityOff();
#if	VTK_MAJOR_VERSION < 8
        _cloudMapper->SetImmediateModeRendering(!Internal::Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8
        _cloudActor = VTKMgx3DActor::New();
        _cloudActor->SetEntity(getEntity());
		_cloudActor->SetRepresentationType (
			(DisplayRepresentation::type)(DisplayRepresentation::SOLID | DisplayRepresentation::WIRE));
        CHECK_NULL_PTR_ERROR(getEntity ( ))
        const DisplayProperties& properties =
                getEntity()->getDisplayProperties();
        const Color& cloudColor = properties.getCloudColor();
        _cloudActor->GetProperty()->SetColor(cloudColor.getRed()/255.,
                cloudColor.getGreen()/255., cloudColor.getBlue()/255.);
        _cloudActor->SetMapper(_cloudMapper);
        CHECK_NULL_PTR_ERROR(_cloudGrid)
        vtkPoints* points = vtkPoints::New();
        CHECK_NULL_PTR_ERROR(points)
        points->SetDataTypeToDouble();
        _cloudGrid->Initialize();

        size_t pointsNum = 0;
        vtkIdType id = 0;

        if (0 == meshEntity->getDim())
        {
            // Récupération du groupe GMDS nuage
            Mesh::Cloud* cl = dynamic_cast<Mesh::Cloud*> (meshEntity);
            if (NULL == cl)
            {
                INTERNAL_ERROR(exc, "Echec lors de la conversion en Mesh::Cloud.",
                        "VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation");
                throw exc;
            }

            std::vector<gmds::Node>   nodes;
            cl->getGMDSNodes(nodes);

            pointsNum = nodes.size();
            points->SetNumberOfPoints(pointsNum);
            _cloudGrid->Allocate(pointsNum, 1000);
            for (std::vector<gmds::Node>::const_iterator itp = nodes.begin();
                    nodes.end() != itp; itp++, id++)
            {

                double coords[3] = { (*itp).X(), (*itp).Y(), (*itp).Z()};
                points->SetPoint(id, coords);
            } // for (std::vector<gmds::Node*>::const_iterator itp = nodes.begin ( );
        } // end 0 == meshEntity->getDim()
        else if (2 == meshEntity->getDim())
        {

            Mesh::Surface* surf = dynamic_cast<Mesh::Surface*>(meshEntity);
            if (surf == 0)
            {
                INTERNAL_ERROR(exc, "Ce n'est pas une Mesh::Surface",
                        "VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation");
                throw exc;
            }

            // la liste des Topo::CoFace qui ont contribuées
            std::vector<Topo::CoFace* > faces;
            surf->getCoFaces(faces);

            // calcul le nombre total de noeuds (il y a des doublons sur les arêtes, tant pis)
            for (std::vector<Topo::CoFace* >::iterator iter =
                    faces.begin(); iter != faces.end(); ++iter)
                pointsNum += (*iter)->nodes().size();

            points->SetNumberOfPoints(pointsNum);
            _cloudGrid->Allocate(pointsNum, 1000);

            for (std::vector<Topo::CoFace* >::iterator iter =
                    faces.begin(); iter != faces.end(); ++iter)
            {
                std::vector<gmds::TCellID>& nodes = (*iter)->nodes();
                for (std::vector<gmds::TCellID>::const_iterator itp =
                        nodes.begin(); nodes.end() != itp; itp++, id++)
                {
                    gmds::Node current = gmdsMesh.get<gmds::Node>(*itp);
                    double coords[3] = { current.X(), current.Y(), current.Z()};

                    points->SetPoint(id, coords);
                } // for (std::vector<gmds::Node*>::const_iterator itp = nodes.begin ( );
            } // for (std::vector<Topo::CoFace* >::iterator iter = faces.begin();
        } // end 2 == meshEntity->getDim()
        else if (3 == meshEntity->getDim())
        {

            Mesh::Volume* vol = dynamic_cast<Mesh::Volume*>(meshEntity);
            if (vol == 0)
            {
                INTERNAL_ERROR(exc, "Ce n'est pas un Mesh::Volume",
                        "VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation");
                throw exc;
            }

            // la liste des Topo::Block qui ont contribués
            std::vector<Topo::Block* > blocs;
            vol->getBlocks(blocs);

            // calcul le nombre total de noeuds (il y a des doublons sur les faces, tant pis)
            for (std::vector<Topo::Block* >::iterator iter =
                    blocs.begin(); iter != blocs.end(); ++iter)
                pointsNum += (*iter)->nodes().size();

            points->SetNumberOfPoints(pointsNum);
            _cloudGrid->Allocate(pointsNum, 1000);

            for (std::vector<Topo::Block* >::iterator iter =
                    blocs.begin(); iter != blocs.end(); ++iter)
            {
                std::vector<gmds::TCellID>& nodes = (*iter)->nodes();
                for (std::vector<gmds::TCellID>::const_iterator itp =
                        nodes.begin(); nodes.end() != itp; itp++, id++)
                {
                    gmds::Node current = gmdsMesh.get<gmds::Node>(*itp);
                    double coords[3] = { current.X(), current.Y(), current.Z()};
                    points->SetPoint(id, coords);
                } // for (std::vector<gmds::Node*>::const_iterator itp = nodes.begin ( );
            } // for (std::vector<Topo::Block* >::iterator iter = blocs.begin();
        } // end 3 == meshEntity->getDim()
        else
        {
            INTERNAL_ERROR(exc,
                    "Représentation sous forme de nuage de points non prévue pour cette dimension.",
                    "VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation");
            throw exc;
        }

        // SetPoints   : pas de range checking ou  réallocation.
        _cloudGrid->SetPoints(points);

        // Les points : on part ici du principe qu'on a que des points.
        // => Spécialisation : triangles
        vtkCellArray* cellArray = vtkCellArray::New();
        vtkIdTypeArray* idsArray = vtkIdTypeArray::New();
        // idsArray est forcément du type : nbPts p1 p2 .. pn pour chaque point,
        // à savoir qu'on ne peut s'affranchir de mettre nbPts si toutes les mailles
        // sont de même type (vertex) et que c'est renseigné par ailleurs via
        // _cloudGrid->SetCells (VTK_VERTEX, cellArray);
        // Par rapport au cas général (polygones de types différents) on fait tout
        // de même l'économie du tableau cellTypes de taille
        // pointsNum * sizeof (int).
        idsArray->SetNumberOfValues(2 * pointsNum);
        vtkIdType* cellsPtr = idsArray->GetPointer(0);
        for (id = 0; id < pointsNum; id++)
        {
            size_t pos = 2 * id;
            cellsPtr[pos] = 1;
            cellsPtr[pos + 1] = id;
        } // for (id = 0; id < pointsNum; id++)
        idsArray->Squeeze();
        cellArray->SetCells(pointsNum, idsArray);
        cellArray->Squeeze();
        _cloudGrid->SetCells(VTK_VERTEX, cellArray);
        idsArray->Delete();
        idsArray = 0;
        cellArray->Delete();
        cellArray = 0;
		points->Delete ( );
		points	= 0;

    }
    catch (const gmds::GMDSException &e)
    {
		UTF8String	erreur (Charset::UTF_8);
        erreur << "GMDS: " << e.what();
        throw TkUtil::Exception(erreur);
    }

} // VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation


void VTKGMDSEntityRepresentation::createMeshEntityLineRepresentation(
        Mesh::MeshEntity* meshEntity)
{
    CHECK_NULL_PTR_ERROR (meshEntity)
    if ((0 != _wireGrid) || (0 != _wireMapper) || (0 != _wireActor))
    {
        INTERNAL_ERROR(exc, "Représentation déjà créée.",
                "VTKGMDSEntityRepresentation::createMeshEntityLineRepresentation");
        throw exc;
    } // if ((0 != _wireGrid) || ...

    if (meshEntity->isDestroyed())
    {
        INTERNAL_ERROR(exc, "L'entité est détruite",
                "VTKGMDSEntityRepresentation::createMeshEntityLineRepresentation")
    }

    // Récupération du maillage GMDS
    Mesh::MeshItf* meshItf = meshEntity->getMeshManager().getMesh();
    Mesh::MeshImplementation* meshImpl =
            dynamic_cast<Mesh::MeshImplementation*>(meshItf);
    if (0 == meshImpl)
    {
        INTERNAL_ERROR(exc,
                "Echec lors de la conversion en MeshImplementation.",
                "VTKGMDSEntityRepresentation::createMeshEntityLineRepresentation");
        throw exc;

    }

    gmds::IGMesh& gmdsMesh = meshImpl->getGMDSMesh();
    vector<Math::Point>    points;
    vector<size_t>         segments;
    if (1 == meshEntity->getDim())
    {
        // Récupération du groupe GMDS nuage
        Mesh::Line* line = dynamic_cast<Mesh::Line*> (meshEntity);
        if (NULL == line)
        {
            INTERNAL_ERROR(exc, "Echec lors de la conversion en Mesh::Line.",
                    "VTKGMDSEntityRepresentation::createMeshEntityLineRepresentation");
            throw exc;
        }

        std::vector<gmds::Node>   nodes;
        std::vector<gmds::Edge >  edges;
        line->getGMDSEdges(edges);

        // On gère les noeuds de manière unique :
        std::map<gmds::TCellID, unsigned long> node2id;
        int done = gmdsMesh.getNewMark<gmds::Node> ( );

        for (std::vector<gmds::Edge>::const_iterator ite = edges.begin();
             edges.end() != ite; ite++)
        {
             vector<gmds::Node>	nodes	= (*ite).get<gmds::Node> ( );
             if (2 != nodes.size ( ))
             {
                 TkUtil::UTF8String	msg;
                 msg << "L'arête GMDS " << (unsigned long)(*ite).getID ( ) << " de la ligne "
                     << line->getName ( ) << " est composée de " << (unsigned long)nodes.size ( )
                     << " noeuds.";
                 throw TkUtil::Exception (msg);
             }	// if (2 != nodes.size ( ))
             gmds::Node& node1 = nodes [0], node2 = nodes [1];
             if (false == gmdsMesh.isMarked<gmds::Node> (node1.getID ( ), done))
             {
                 node2id [node1.getID ( )] = points.size ( );
                 points.push_back (Math::Point (node1.X ( ), node1.Y ( ), node1.Z ( )));
                 gmdsMesh.mark<gmds::Node> (node1.getID ( ), done);
             }	// if (false == gmdsMesh.isMarked<gmds::Node> (node1.getID ( ), done))
             if (false == gmdsMesh.isMarked<gmds::Node> (node2.getID ( ), done))
             {
                 node2id [node2.getID ( )] = points.size ( );
                 points.push_back (Math::Point (node2.X ( ), node2.Y ( ), node2.Z ( )));
                 gmdsMesh.mark<gmds::Node> (node1.getID ( ), done);
             }	// if (false == gmdsMesh.isMarked<gmds::Node> (node2.getID ( ), done))
        }	// for (std::vector<gmds::Edge>::const_iterator ite = edges.begin();
        // Démarquer les noeuds :
        for (std::map<gmds::TCellID, unsigned long>::const_iterator itm = node2id.begin ( );
             node2id.end ( ) != itm; itm++)
             gmdsMesh.unmark<gmds::Node> ((*itm).first, done);
        gmdsMesh.freeMark<gmds::Node> (done);

        for (std::vector<gmds::Edge>::const_iterator ite = edges.begin();
             edges.end() != ite; ite++)
        {
            vector<gmds::Node>	nodes	= (*ite).get<gmds::Node> ( );
            gmds::Node& node1 = nodes [0], node2 = nodes [1];
            segments.push_back (node2id [node1.getID ( )]);
            segments.push_back (node2id [node2.getID ( )]);
        }	// for (std::vector<gmds::Edge>::const_iterator ite = edges.begin();
    } // if (1 == meshEntity->getDim()

    VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation(
            getEntity ( ), _wireActor, _wireMapper, _wireGrid, points, segments);

} // VTKGMDSEntityRepresentation::createMeshEntityLineRepresentation


void VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation(
        Mesh::MeshEntity* meshEntity)
{
#ifdef _DEBUG_VTKGMDSEntityRepresentation
	std::cout<<"VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation("<<meshEntity->getName()<<")"<<std::endl;
#endif

    if (0 != _surfacicGrid)
    {
        INTERNAL_ERROR(exc, "Représentation déjà créée.",
                "VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation")
        throw exc;
    } // if (0 != _surfacicGrid)

    // Récupération du maillage GMDS
    Mesh::MeshItf* meshItf = meshEntity->getMeshManager().getMesh();
    Mesh::MeshImplementation* meshImpl =
            dynamic_cast<Mesh::MeshImplementation*>(meshItf);
    if (0 == meshImpl)
    {
        INTERNAL_ERROR(exc,
                "Echec lors de la conversion en MeshImplementation.",
                "VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation");
        throw exc;
    }

	UTF8String	message1 (Charset::UTF_8);
    message1
            << "VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation pour "
            << meshEntity->getName();
    getEntity()->log(TkUtil::TraceLog(message1, TkUtil::Log::TRACE_5));

    _surfacicGrid = vtkUnstructuredGrid::New();
    CHECK_NULL_PTR_ERROR(_surfacicGrid)
    _surfacicGrid->Initialize();

    // on passe par GMDS pour récupérer les noeuds et les mailles
    gmds::IGMesh& gmdsMesh =
    		meshImpl->getGMDSMesh();

    try
    {
        if (2 == meshEntity->getDim())
        {
            	createMeshEntitySurfacicRepresentation2D(meshEntity, gmdsMesh);

        } // end if (2 == meshEntity->getDim())
        else if (3 == meshEntity->getDim())
        {
            	createMeshEntitySurfacicRepresentation3D(meshEntity, gmdsMesh);

        } // if (3 == meshEntity->getDim())
        else
        {
        	INTERNAL_ERROR(exc,
        			"Représentation filaire non prévue pour cette dimension.",
        			"VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation");
        	throw exc;

        }
    }
    catch (const gmds::GMDSException &e)
    {
		UTF8String	erreur (Charset::UTF_8);
        erreur << "GMDS: " << e.what();
        throw TkUtil::Exception(erreur);
    }

} // VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation

void VTKGMDSEntityRepresentation::
createMeshEntitySurfacicRepresentation2D(Mesh::MeshEntity* meshEntity, gmds::IGMesh& gmdsMesh)
{
#ifdef _DEBUG_VTKGMDSEntityRepresentation
	std::cout<<"cas dimension 2"<<std::endl;
#endif

	// Récupération du groupe GMDS
	Mesh::Surface* surf = dynamic_cast<Mesh::Surface*> (meshEntity);
	if (NULL == surf)
	{
		INTERNAL_ERROR(exc, "Echec lors de la conversion en Mesh::Surface.",
				"VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation");
		throw exc;
	}

	std::vector<Topo::CoFace*> cofaces;

	surf->getCoFaces(cofaces);

	if (meshEntity->getContext().getRatioDegrad() == 1 || !surf->isStructured())
		createCoFacesSurfacicRepresentationRatio1(cofaces, gmdsMesh);
	else
		createCoFacesSurfacicRepresentationRatioN(cofaces, gmdsMesh, meshEntity->getContext().getRatioDegrad());

} // createMeshEntitySurfacicRepresentation2D


void VTKGMDSEntityRepresentation::
createMeshEntitySurfacicRepresentation3D(Mesh::MeshEntity* meshEntity, gmds::IGMesh& gmdsMesh)
{
#ifdef _DEBUG_VTKGMDSEntityRepresentation
	std::cout<<"cas dimension 3"<<std::endl;
#endif


	Mesh::Volume* vol = dynamic_cast<Mesh::Volume*>(meshEntity);
	if (vol == 0)
	{
		INTERNAL_ERROR(exc, "Ce n'est pas un Mesh::Volume",
				"VTKGMDSEntityRepresentation::createMeshEntityCloudRepresentation");
		throw exc;
	}
	// la liste des Topo::Block qui ont contribués
	std::vector<Topo::Block* > blocs;
	vol->getBlocks(blocs);

	// la liste des faces externes au groupe de blocs
	// on utilise une map et on marque les faces à chaque fois qu'elles sont vus
	std::map<Topo::CoFace*, int> marque_faces;

	std::vector<Topo::Face* > faces;
	for (std::vector<Topo::Block* >::iterator iter1 =
			blocs.begin(); iter1 != blocs.end(); ++iter1)
	{
		(*iter1)->getFaces(faces);

		std::vector<Topo::CoFace* > cofaces;
		for (std::vector<Topo::Face* >::iterator iter2 =
				faces.begin(); iter2 != faces.end(); ++iter2)
		{
			(*iter2)->getCoFaces(cofaces);

			for (std::vector<Topo::CoFace* >::iterator iter3 =
					cofaces.begin(); iter3 != cofaces.end();
					++iter3)
				marque_faces[*iter3] += 1;
		}
	}


	std::vector<Topo::CoFace*> cofaces;

	for (std::map<Topo::CoFace*, int>::iterator iter = marque_faces.begin();
			iter != marque_faces.end();
			++iter)
		if (iter->second == 1)
			cofaces.push_back(iter->first);

	if (meshEntity->getContext().getRatioDegrad() == 1 || !vol->isStructured())
		createCoFacesSurfacicRepresentationRatio1(cofaces, gmdsMesh);
	else
		createCoFacesSurfacicRepresentationRatioN(cofaces, gmdsMesh, meshEntity->getContext().getRatioDegrad());

} // createMeshEntitySurfacicRepresentation3D


void VTKGMDSEntityRepresentation::
createCoFacesSurfacicRepresentationRatio1(std::vector<Topo::CoFace*> cofaces, gmds::IGMesh& gmdsMesh)
{
#ifdef _DEBUG_VTKGMDSEntityRepresentation
	std::cout<<"cas ratio 1"<<std::endl;
#endif
	vtkPoints* points = vtkPoints::New();
	CHECK_NULL_PTR_ERROR(points)
    points->SetDataTypeToDouble();	// Correctif CP

	// on cumule les noeuds, en évitant les doublons
	std::vector<gmds::Node> nodes;
	std::vector<gmds::Face> polygones;
	std::map<gmds::TCellID, int> node2id;
	uint nbRefIds = 0; /// nombres de références sur les ids dans les polygones
	int done = gmdsMesh.getNewMark<gmds::Node>();

	for (std::vector<Topo::CoFace*>::iterator iter=cofaces.begin(); iter!=cofaces.end(); ++iter){
		Topo::CoFace* face = *iter;

		std::vector<gmds::TCellID>& loc_polygones = face->faces(); //les polygones de la Topo::CoFace

		for (std::vector<gmds::TCellID>::const_iterator iter_p = loc_polygones.begin();
				iter_p != loc_polygones.end(); ++iter_p){

			gmds::Face current = gmdsMesh.get<gmds::Face>(*iter_p);
			std::vector<gmds::Node> nds = current.getAll<gmds::Node>();
			polygones.push_back(current);

			for (std::vector<gmds::Node>::const_iterator iter_n = nds.begin();
					iter_n != nds.end(); ++iter_n){

				gmds::Node current_node =*iter_n;
				gmds::TCellID current_node_id =current_node.getID();
				if (!gmdsMesh.isMarked<gmds::Node>(current_node_id, done)){

					node2id[current_node_id] = nodes.size();
					nodes.push_back(current_node);
					gmdsMesh.mark<gmds::Node>(current_node_id, done);
				}
			} // end for iter_n
			nbRefIds += nds.size();
		} // for (std::vector<gmds::Face*>::const_iterator iter_p = polygones.begin();
	} // end for iter=cofaces.begin()

	// on démarque les noeuds
	for (std::vector<gmds::Node>::iterator iter = nodes.begin();
			iter != nodes.end(); ++iter)
		gmdsMesh.unmark<gmds::Node>(iter->getID(), done);
	gmdsMesh.freeMark<gmds::Node>(done);

	const size_t pointsNum = nodes.size();
	const size_t polygonNum = polygones.size();

	// Les sommets :
	points->SetNumberOfPoints(pointsNum);
	_surfacicGrid->Allocate(polygonNum, 1000);
	_surfacicGrid->SetPoints(points);
	_surfacicPointsVTK2GMDSID.clear();
	vtkIdType id = 0;
	for (std::vector<gmds::Node>::const_iterator itp =
			nodes.begin(); nodes.end() != itp; itp++, id++)
	{
		double coords[3] = { (*itp).X(), (*itp).Y(), (*itp).Z() };
		points->SetPoint(id, coords);
		_surfacicPointsVTK2GMDSID[id] = (*itp).getID();
	} // for (vector<Math::Point>::const_iterator itp = nodes.begin ( );

	int* cellTypes = new int[polygonNum];
	vtkCellArray* cellArray = vtkCellArray::New();
	vtkIdTypeArray* idsArray = vtkIdTypeArray::New();

	idsArray->SetNumberOfValues(polygonNum + nbRefIds);
	vtkIdType* cellsPtr = idsArray->GetPointer(0);
	size_t pos = 0;
	for (id = 0; id < polygonNum; id++)
	{
		cellTypes[id] = VTK_POLYGON;
		gmds::Face poly = polygones[id];

		std::vector<gmds::TCellID> ndsIDs = poly.getAllIDs<gmds::Node>();

		cellsPtr[pos++] = ndsIDs.size();
		for (size_t j = 0; j < ndsIDs.size(); j++)
			cellsPtr[pos++] = node2id[ndsIDs[j]];
	} // for (id = 0; id < polygonNum; id++)

	cellArray->SetCells(polygonNum, idsArray);
	_surfacicGrid->SetCells(cellTypes, cellArray);
	delete[] cellTypes;
	cellTypes = 0;
	idsArray->Delete();
	idsArray = 0;
	cellArray->Delete();
	cellArray = 0;
	points->Delete ( );
	points	= 0;

} // createCoFacesSurfacicRepresentationRatio1

void VTKGMDSEntityRepresentation::
createCoFacesSurfacicRepresentationRatioN(std::vector<Topo::CoFace*> cofaces, gmds::IGMesh& gmdsMesh, int ratio_degrad)
{
#ifdef _DEBUG_VTKGMDSEntityRepresentation
	std::cout<<"cas ratio "<<ratio_degrad<<std::endl;
#endif
	// cas 2D avec dégradation
	vtkPoints* points = vtkPoints::New();
	CHECK_NULL_PTR_ERROR(points)

	uint pointsNum = 0;
	uint polygonNum = 0;

	// on compte les sommets et les éléments, et on teste les prérequis
	for (std::vector<Topo::CoFace* >::iterator iter =
			cofaces.begin(); iter != cofaces.end(); ++iter)
	{
		// on se limite au cas avec des Faces structurées, non dégénérées
//		if ((*iter)->getNbEdges() != 4)
//		{
//			INTERNAL_ERROR(exc,
//					"Le cas avec dégénérescence d'une Face n'est pas implémenté.",
//					"VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation");
//			throw exc;
//		}
		if (!(*iter)->isStructured())
		{
			INTERNAL_ERROR(exc,
					"Le cas non structuré d'une Face n'est pas implémenté.",
					"VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation");
			throw exc;
		}

		uint ni, nj;
		(*iter)->getNbMeshingEdges(ni, nj);
#ifdef _DEBUG_VTKGMDSEntityRepresentation
		std::cout<<" pour "<<(*iter)->getName()<<" getNbMeshingEdges => ni = "<<ni<<", nj = "<<nj<<std::endl;
#endif

		ni = ni / ratio_degrad + 1;
		nj = nj / ratio_degrad + 1;
		if (ni<2)
			ni=2;
		if (nj<2)
			nj=2;

#ifdef _DEBUG_VTKGMDSEntityRepresentation
		std::cout<<" ratio_degrad = "<<ratio_degrad<<" => ni = "<<ni<<", nj = "<<nj<<std::endl;
#endif

		pointsNum += ni * nj;
		polygonNum += (ni - 1) * (nj - 1);
	} // for (iter...
#ifdef _DEBUG_VTKGMDSEntityRepresentation
	std::cout<<" pointsNum = "<<pointsNum<<", polygonNum = "<<polygonNum<<std::endl;
#endif

	points->SetNumberOfPoints(pointsNum);
	_surfacicGrid->Allocate(polygonNum, 1000);
	_surfacicGrid->SetPoints(points);
	_surfacicPointsVTK2GMDSID.clear();

	int* cellTypes = new int[polygonNum];
	vtkCellArray* cellArray = vtkCellArray::New();
	vtkIdTypeArray* idsArray = vtkIdTypeArray::New();

	idsArray->SetNumberOfValues(polygonNum * 5); // ATTENTION aux cas dégénérés ou non-structurés
	vtkIdType* cellsPtr = idsArray->GetPointer(0);

	// remplissage de la structure pour les points et pour les éléments
	vtkIdType idNd = 0;
	vtkIdType idEl = 0;
	size_t pos = 0;
	for (std::vector<Topo::CoFace* >::iterator iter =
			cofaces.begin(); iter != cofaces.end(); ++iter)
	{
		uint niMax =
				(*iter)->getEdge(Topo::CoFace::j_min)->getNbNodes();
		uint njMax =
				(*iter)->getEdge(Topo::CoFace::i_min)->getNbNodes();

#ifdef _DEBUG_VTKGMDSEntityRepresentation
		std::cout<<" pour "<<(*iter)->getName()<<" niMax = "<<niMax<<", njMax = "<<njMax<<std::endl;
#endif

		uint ni, nj;
		(*iter)->getNbMeshingEdges(ni, nj);
		ni = ni / ratio_degrad + 1;
		nj = nj / ratio_degrad + 1;
		if (ni<2)
			ni=2;
		if (nj<2)
			nj=2;

		// on utilise les noeuds du maillage
		std::vector<gmds::TCellID>& nodes = (*iter)->nodes();
#ifdef _DEBUG_VTKGMDSEntityRepresentation
		std::cout<<" nodes.size() = "<<nodes.size()<<std::endl;
#endif

		for (uint i = 0; i < ni; i++)
		{
			for (uint j = 0; j < nj; j++)
			{
				uint indI = i * ratio_degrad;
				uint indJ = j * ratio_degrad;
				if (i == ni - 1)
					indI = niMax - 1;
				if (j == nj - 1)
					indJ = njMax - 1;

#ifdef _DEBUG_VTKGMDSEntityRepresentation
				std::cout<<" indI + indJ * niMax = "<<indI<<"+"<<indJ<<"*"<<niMax<<" = "<<indI + indJ * niMax<<" / "<<nodes.size()<<std::endl;
				if (indI + indJ * niMax >= nodes.size())
					std::cout<<"DEPASSEMENT indice sur nodes"<<std::endl;
#endif

				gmds::Node node = gmdsMesh.get<gmds::Node>(nodes[indI + indJ * niMax]);

				double coords[3] = { node.X(), node.Y(), node.Z() };
				points->SetPoint(idNd, coords);
				_surfacicPointsVTK2GMDSID[idNd] = node.getID();

#ifdef _DEBUG_VTKGMDSEntityRepresentation
				std::cout << "nodeVTK ("<<i<<","<<j<<") id "<<idNd<<" : "<<coords[0]<<", "<<coords[1]<<", "<<coords[2]<<std::endl;
#endif

				if ((ni - 1) != i && (nj - 1) != j)
				{

#ifdef _DEBUG_VTKGMDSEntityRepresentation
					std::cout <<" elemVTK id : "<<idEl<<" : "<<idNd<<", "<<idNd+1<<", "<<idNd+ni+1<<", "<<idNd+ni<<std::endl;
#endif

					cellTypes[idEl++] = VTK_POLYGON;
					cellsPtr[pos++] = 4;

					cellsPtr[pos++] = idNd;
					cellsPtr[pos++] = idNd + 1;
					cellsPtr[pos++] = idNd + nj + 1;
					cellsPtr[pos++] = idNd + nj;
#ifdef _DEBUG_VTKGMDSEntityRepresentation
					if (idNd + nj + 1 >= pointsNum)
						std::cout<<"DEPASSEMENT indices noeuds"<<std::endl;
#endif
				}
				idNd += 1;
			} // for (uint j=0; j<=nj; j++)
		} // for (uint i=0; i<=ni; i++)

	} // for (iter...

	cellArray->SetCells(polygonNum, idsArray);
	_surfacicGrid->SetCells(cellTypes, cellArray);
	delete[] cellTypes;
	cellTypes = 0;
	idsArray->Delete();
	idsArray = 0;
	cellArray->Delete();
	cellArray = 0;

} // createCoFacesSurfacicRepresentationRatioN


void VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation(
        Mesh::MeshEntity* meshEntity)
{
    if (0 != _volumicGrid)
    {
        INTERNAL_ERROR(exc, "Représentation déjà créée.",
                "VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation")
        throw exc;
    } // if (0 != _volumicGrid)

    // Récupération du maillage GMDS
    Mesh::MeshItf* meshItf = meshEntity->getMeshManager().getMesh();
    Mesh::MeshImplementation* meshImpl =
            dynamic_cast<Mesh::MeshImplementation*>(meshItf);
    if (0 == meshImpl)
    {
        INTERNAL_ERROR(exc,
                "Echec lors de la conversion en MeshImplementation.",
                "VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation");
        throw exc;
    }

	UTF8String	message1 (Charset::UTF_8);
    message1
            << "VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation pour "
            << meshEntity->getName();
    getEntity()->log(TkUtil::TraceLog(message1, TkUtil::Log::TRACE_5));

    _volumicGrid = vtkUnstructuredGrid::New();
    CHECK_NULL_PTR_ERROR(_volumicGrid)
    vtkPoints* points = vtkPoints::New();
    CHECK_NULL_PTR_ERROR(points)
    _volumicGrid->Initialize();
//    points->SetDataTypeToDouble ( );

    if (meshEntity->getContext().getRatioDegrad() != 1)
    {
    	MGX_NOT_YET_IMPLEMENTED("Cas du maillage volumique en représentation pleine ne tient pas compte de la dégradation de la représentation du maillage")
//        INTERNAL_ERROR(exc,
//                "Le cas avec ratio pour le maillage volumique d'un volume.",
//                "VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation");
//        throw exc;
    }

    if (3 != meshEntity->getDim())
    {
        INTERNAL_ERROR(exc,
                "Représentation volumique non prévue pour cette dimension.",
                "VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation");
        throw exc;
    }

    try
    {
        // on passe par GMDS pour récupérer les noeuds et les mailles
        gmds::IGMesh& gmdsMesh =
                meshImpl->getGMDSMesh();

        // Récupération des mailles GMDS via le volume
        Mesh::Volume* vol = dynamic_cast<Mesh::Volume*> (meshEntity);
        if (NULL == vol)
        {
            INTERNAL_ERROR(exc, "Echec lors de la conversion en Mesh::Volume.",
                    "VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation");
            throw exc;
        }

        std::vector<gmds::Region>   polyedres;
        vol->getGMDSRegions(polyedres);

        // Constitution d'un vecteur avec les noeuds de la surface
        // et une map pour l'indirection entre gmds::Node vers indice local
        std::vector<gmds::Node> nodes;
        std::map<gmds::TCellID, int> node2id;
        uint nbRefIds = 0; /// nombres de références sur les ids dans les polyêdres

        const int done = gmdsMesh.getNewMark<gmds::Node>();
        for (std::vector<gmds::Region>::const_iterator iter_p =
                polyedres.begin(); iter_p != polyedres.end(); ++iter_p)
        {
            std::vector<gmds::Node> nds = (*iter_p).getAll<gmds::Node>();

            for (std::vector<gmds::Node>::const_iterator iter_n = nds.begin();
                    iter_n != nds.end(); ++iter_n)
            {
                gmds::Node current_node =*iter_n;
                gmds::TCellID current_node_id =current_node.getID();
                if (!gmdsMesh.isMarked<gmds::Node>(current_node_id, done))
                {

                    node2id[current_node_id] = nodes.size();
                    nodes.push_back(current_node);
                    gmdsMesh.mark<gmds::Node>(current_node_id, done);
                }
            }
            nbRefIds += nds.size();
        } // for (std::vector<gmds::Region*>::const_iterator iter_p = polyedres.begin();

        // on démarque les noeuds
        for (std::vector<gmds::Node>::iterator iter = nodes.begin();
                iter != nodes.end(); ++iter)
            gmdsMesh.unmark<gmds::Node>(iter->getID(), done);

        gmdsMesh.freeMark<gmds::Node>(done);

        const size_t pointsNum = nodes.size();
        const size_t polyedreNum = polyedres.size();
        //cout << "PTS : " << pointsNum << " POLYGONES : " << polyedreNum << endl;
        // Les sommets :
        points->SetNumberOfPoints(pointsNum);
        _volumicGrid->Allocate(polyedreNum, 1000);
        _volumicGrid->SetPoints(points);
        vtkIdType id = 0;
        for (std::vector<gmds::Node>::const_iterator itp = nodes.begin();
                nodes.end() != itp; itp++, id++)
        {
            double coords[3] = { (*itp).X(), (*itp).Y(), (*itp).Z() };
            points->SetPoint(id, coords);
        } // for (vector<Math::Point>::const_iterator itp = nodes.begin ( );

        int* cellTypes = new int[polyedreNum];
        vtkCellArray* cellArray = vtkCellArray::New();
        vtkIdTypeArray* idsArray = vtkIdTypeArray::New();

        idsArray->SetNumberOfValues(polyedreNum + nbRefIds);
        vtkIdType* cellsPtr = idsArray->GetPointer(0);
        size_t pos = 0;
        for (id = 0; id < polyedreNum; id++)
        {
            gmds::Region poly = polyedres[id];

            std::vector<gmds::TCellID> nds = poly.getAllIDs<gmds::Node>();
            if (nds.size() == 8)
                cellTypes[id] = VTK_HEXAHEDRON;
            else if (nds.size() == 6)
                cellTypes[id] = VTK_WEDGE;
            else if (nds.size() == 5)
                cellTypes[id] = VTK_PYRAMID;
            else if (nds.size() == 4)
                cellTypes[id] = VTK_TETRA;
            else
            {
                INTERNAL_ERROR(exc,
                        "Représentation volumique d'un polyèdre d'un type non prévu.",
                        "VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation");
                throw exc;
            }

            cellsPtr[pos++] = nds.size();
            for (size_t j = 0; j < nds.size(); j++)
                cellsPtr[pos++] = node2id[nds[j]];
        } // for (id = 0; id < polyedreNum; id++)

        cellArray->SetCells(polyedreNum, idsArray);
        _volumicGrid->SetCells(cellTypes, cellArray);
        delete[] cellTypes;
        cellTypes = 0;
        idsArray->Delete();
        idsArray = 0;
        cellArray->Delete();
        cellArray = 0;
		points->Delete ( );
		points	= 0;

        //std::cout<<"VTKGMDSEntityRepresentation::createMeshEntityVolumicRepresentation ; _volumicGrid->GetNumberOfCells() = "<<_volumicGrid->GetNumberOfCells()<<std::endl;
    }
    catch (const gmds::GMDSException &e)
    {
		UTF8String	erreur (Charset::UTF_8);
        erreur << "GMDS: " << e.what();
        throw TkUtil::Exception(erreur);
    }

} // VTKGMDSEntityRepresentation::createMeshEntitySurfacicRepresentation

void VTKGMDSEntityRepresentation::doShrink(vtkPoints* points)
{
	double shrink = getEntity()->getDisplayProperties().getShrinkFactor();
	if (shrink!=1.0){
		Utils::Math::Point barycentre;
		for (uint id=0; id<points->GetNumberOfPoints(); id++) {
			double coords[3];
			points->GetPoint(id, coords);
			barycentre += Utils::Math::Point(coords[0], coords[1], coords[2]);
		}
		barycentre /= (double)points->GetNumberOfPoints();

		for (uint id=0; id<points->GetNumberOfPoints(); id++) {
			double coords[3];
			points->GetPoint(id, coords);
			Utils::Math::Point pt = Utils::Math::Point(coords[0], coords[1], coords[2]);
			pt = barycentre + (pt - barycentre) * shrink;
			coords[0] = pt.getX();
			coords[1] = pt.getY();
			coords[2] = pt.getZ();
			points->SetPoint(id, coords);
		}

	}
} // VTKGMDSEntityRepresentation::doShrink


void VTKGMDSEntityRepresentation::createAssociationVectorRepresentation ( )
{
}	// VTKGMDSEntityRepresentation::createAssociationVectorRepresentation


bool VTKGMDSEntityRepresentation::getRefinedRepresentation (
	std::vector<Utils::Math::Point>& points, std::vector<size_t>& discretization,
	size_t factor)
{
	return false;	// Raffinement non fait comme demandé
}	// VTKGMDSEntityRepresentation::getRefinedRepresentation



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================

} // namespace QtVtkComponents

} // namespace Mgx3D
