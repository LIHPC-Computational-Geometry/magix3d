/**
 * \file        VTKEntityRepresentation.cpp
 * \author      Charles PIGNEROL
 * \date        24/11/2011
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtVtkComponents/VTKEntityRepresentation.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Topo/TopoEntity.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Utils/DisplayProperties.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <iostream>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellType.h>
#include <vtkDoubleArray.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkLine.h>
#include <vtkOutlineSource.h>
#include <vtkPointData.h>
#include <vtkTextProperty.h>
#include <vtkTriangle.h>
#include <vtkVertex.h>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::QtComponents;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtVtkComponents
{

// ===========================================================================
//                      LA CLASSE VTKEntityRepresentation
// ===========================================================================


VTKEntityRepresentation::VTKEntityRepresentation (Entity& entity)
	: RenderedEntityRepresentation (entity),
	  _renderer (0),
	  _surfacicGrid (0), _surfacicActor (0), _surfacicMapper (0),
      _volumicGrid (0), _volumicActor (0), _volumicMapper (0),
	  _wireGrid (0), _wireActor (0), _wireMapper (0),
	  _isoWireGrid (0), _isoWireActor (0), _isoWireMapper (0),
	  _discGrid (0), _discActor(0), _discMapper(0),
	  _cloudGrid (0), _cloudActor (0), _cloudMapper (0),
	  _refinedGrid (0), _refineFilter (0),
	  _refinedActor (0), _refinedMapper (0),
	  _textualInfosActor (0), _textualInfosMapper (0),
	  _vectAssArrow (0), _vectAssActor (0), _vectAssMapper (0),
	  _trihedronArrowX (0), _trihedronArrowY (0), _trihedronArrowZ (0),
	  _triedronXActor (0), _triedronYActor (0), _triedronZActor (0),
	  _triedronXMapper (0), _triedronYMapper (0), _triedronZMapper (0),
	  _outlineSource (0), _highlightActor (0), _highlightMapper (0),
	  _highlightForceDiscretisationType (false)
{
}	// VTKEntityRepresentation::VTKEntityRepresentation


VTKEntityRepresentation::VTKEntityRepresentation (VTKEntityRepresentation& ver)
	: RenderedEntityRepresentation (*(ver.getEntity ( ))),
	  _renderer (0),
	  _surfacicGrid (0), _surfacicActor (0), _surfacicMapper (0),
      _volumicGrid (0), _volumicActor (0), _volumicMapper (0),
	  _wireGrid (0), _wireActor (0), _wireMapper (0),
	  _isoWireGrid (0), _isoWireActor (0), _isoWireMapper (0),
	  _discGrid (0), _discActor(0), _discMapper(0),
	  _cloudGrid (0), _cloudActor (0), _cloudMapper (0),
	  _refinedGrid (0), _refineFilter (0),
	  _refinedActor (0), _refinedMapper (0),
	  _textualInfosActor (0), _textualInfosMapper (0),
	  _vectAssArrow (0), _vectAssActor (0), _vectAssMapper (0),
	  _trihedronArrowX (0), _trihedronArrowY (0), _trihedronArrowZ (0),
	  _triedronXActor (0), _triedronYActor (0), _triedronZActor (0),
	  _triedronXMapper (0), _triedronYMapper (0), _triedronZMapper (0),
	  _outlineSource (0), _highlightActor (0), _highlightMapper (0),
	  _highlightForceDiscretisationType (false)
{
	MGX_FORBIDDEN ("VTKEntityRepresentation copy constructor is not allowed.");
}	// VTKEntityRepresentation::VTKEntityRepresentation


VTKEntityRepresentation& VTKEntityRepresentation::operator = (const VTKEntityRepresentation& er)
{
	MGX_FORBIDDEN ("VTKEntityRepresentation assignment operator is not allowed.");
	if (&er != this)
	{
	}	// if (&er != this)

	return *this;
}	// VTKEntityRepresentation::operator =


VTKEntityRepresentation::~VTKEntityRepresentation ( )
{
	destroyRefinedRepresentation ( );
	destroyRepresentations (true);
}	// VTKEntityRepresentation::~VTKEntityRepresentation


void VTKEntityRepresentation::show (VTKRenderingManager& renderingManager, bool display)
{
	vtkRenderer&	renderer	= renderingManager.getRenderer ( );

	Entity*					e	= getEntity ( );
	if (true == display)
	{
		setRenderingManager (&renderingManager);
		_renderer			= &renderer;
		// Caractère sélectionnable : on s'adapte à ce qui est actuellement
		// sélectionnable dans la vue :
		const bool	pickable	= 0 == renderingManager.getSelectionManager( ) ?
			false : renderingManager.getSelectionManager( )->isSelectionActivated (*e);

		if (0 != _surfacicActor)
		{
			if (0 != e)
				_surfacicActor->SetPickable (pickable);
			renderer.AddViewProp (_surfacicActor);
		}
        if (0 != _volumicActor)
		{
			if (0 != e)
				_volumicActor->SetPickable (pickable);
            renderer.AddViewProp (_volumicActor);
		}
		if (0 != _wireActor)
		{
			if (0 != e)
				_wireActor->SetPickable (pickable);
			renderer.AddViewProp (_wireActor);
		}
		if (0 != _isoWireActor)
		{
			if (0 != e)
				_isoWireActor->SetPickable (pickable);
			renderer.AddViewProp (_isoWireActor);
		}
		if (0 != _cloudActor)
		{
			if (0 != e)
				_cloudActor->SetPickable (pickable);
			renderer.AddViewProp (_cloudActor);
		}
		if (0 != _textualInfosActor)
		{
			if (0 != e)
				_textualInfosActor->SetPickable (pickable);
			renderer.AddViewProp (_textualInfosActor);
		}
		if (0 != _vectAssActor)
		{
			if (0 != e)
				_vectAssActor->SetPickable (pickable);
			renderer.AddViewProp (_vectAssActor);
		}
		if (0 != _triedronXActor)
		{
			if (0 != e){
				_triedronXActor->SetPickable (pickable);
				_triedronYActor->SetPickable (pickable);
				_triedronZActor->SetPickable (pickable);
			}
			renderer.AddViewProp (_triedronXActor);
			renderer.AddViewProp (_triedronYActor);
			renderer.AddViewProp (_triedronZActor);
		}
		if (0 != _discActor)
		{
			if (0 != e)
				_discActor->SetPickable (pickable);
			renderer.AddViewProp (_discActor);
		}
	}	// if (true == display)
	else
	{
		destroyRepresentations (destroyDataOnHide ( ));
		// Pour les processus fils MPI du mode standalone/parallèle :
		if (0 != e)
			e->getDisplayProperties ( ).setDisplayed (false);
/*
		// Si _highlightActor est non nul on garde le renderer pour pouvoir
		// mettre fin au highlighting ultérieurement.
		if (0 == _highlightActor)
			_renderer			= 0;
		if (0 != _volumicActor)
			renderer.RemoveViewProp (_volumicActor);
        if (0 != _surfacicActor)
            renderer.RemoveViewProp (_surfacicActor);
		if (0 != _wireActor)
			renderer.RemoveViewProp (_wireActor);
		if (0 != _isoWireActor)
			renderer.RemoveViewProp (_isoWireActor);
		if (0 != _cloudActor)
			renderer.RemoveViewProp (_cloudActor);
		if (0 != _textualInfosActor)
			renderer.RemoveViewProp (_textualInfosActor);
		if (0 != _vectAssActor)
			renderer.RemoveViewProp (_vectAssActor);
		if (0 != _discActor)
			renderer.RemoveViewProp (_discActor);
*/
	}	// else if (true == display)
}	// VTKEntityRepresentation::show


void VTKEntityRepresentation::setSelected (bool selected)
{
	const bool	update	= isSelected ( ) == selected ? false : true;
	RenderedEntityRepresentation::setSelected (selected);
	if ((true == update) && (0 != getEntity ( )))
	{
		updateRepresentationProperties ( );
		if ((true == selected) &&
		    (true == Resources::instance ( )._selectionOnTop.getValue ( )))
			bringToTop ( );
	}
}	// VTKEntityRepresentation::setSelected


void VTKEntityRepresentation::setHighlighted (bool highlighted)
{
	const unsigned long	mask	= getRepresentationMask ( );
	if (true == highlighted)
	{
// 		if (0 == (mask & DISCRETISATIONTYPE))
 		if ((0 == (mask & DISCRETISATIONTYPE)) && (0 != getEntity ( )) &&
		    (Entity::TopoCoEdge == getEntity ( )->getType ( )))
		{
			_highlightForceDiscretisationType	= true;
			// Forcer la création de la représentation du type de
			// discrétisation :
			updateRepresentation (mask | DISCRETISATIONTYPE, false);
		}	// if (0 == (mask & DISCRETISATIONTYPE))
	}	// if (true == highlighted)
	else
	{
		if (true == _highlightForceDiscretisationType)
		{
			_highlightForceDiscretisationType	= false;
			updateRepresentation (mask ^ DISCRETISATIONTYPE, false);
		}	// if (true == _highlightForceDiscretisationType)
	}	// else if (true == highlighted)

	const bool	update	= isHighlighted ( ) == highlighted ? false : true;
	RenderedEntityRepresentation::setHighlighted (highlighted);
	if ((true == update) && (0 != getEntity ( )))
	{
		updateHighlightRepresentation (highlighted);
		if ((true == highlighted) &&
		    (true == Resources::instance ( )._selectionOnTop.getValue ( )))
			bringToTop ( );
	}	// if ((true == update) && (0 != getEntity ( )))
}	// VTKEntityRepresentation::setHighlighted


void VTKEntityRepresentation::createTextualRepresentation ( )
{
	const unsigned long	usedMask	= getUsedRepresentationMask ( );
	if (0 == _textualInfosActor)
		_textualInfosActor	= vtkActor2D::New ( );
	if (0 == _textualInfosMapper)
		_textualInfosMapper	= vtkTextMapper::New ( );
	const string		text	= getRepresentationText (usedMask);
	const Math::Point	p		= getRepresentationTextPosition (usedMask);
	_textualInfosMapper->SetInput (text.c_str ( ));
	_textualInfosActor->SetMapper (_textualInfosMapper);
	_textualInfosActor->GetPositionCoordinate( )->SetCoordinateSystemToWorld( );
	_textualInfosActor->GetPositionCoordinate ( )->SetValue (p.getX ( ), p.getY ( ), p.getZ ( ));
}	// VTKEntityRepresentation::createTextualRepresentation


void VTKEntityRepresentation::updateRepresentation (unsigned long mask, bool force)
{
	const unsigned long	oldMask	= getRepresentationMask ( );
	if ((mask == oldMask) && (false == force))
			return;

	// Modification de la représentation : on enlève tout et on réaffiche ce
	// qu'il faut dans le bon ordre :
	if (0 != _renderer)
	{
		if (0 != (oldMask & CLOUDS))
		{
			if (0 != _cloudActor)
			    _renderer->RemoveViewProp (_cloudActor);
		}	// if (0 != (oldMask & CLOUDS))
		if (0 != (oldMask & CURVES))
		{
			if (0 != _wireActor)
			    _renderer->RemoveViewProp (_wireActor);
		}	// if (0 != (oldMask & CURVES))
		if (0 != (oldMask & ISOCURVES))
		{
			if (0 != _isoWireActor)
			    _renderer->RemoveViewProp (_isoWireActor);
		}	// if (0 != (oldMask & ISOCURVES))
		if (0 != (oldMask & SURFACES))
		{
			if (0 != _surfacicActor)
			    _renderer->RemoveViewProp (_surfacicActor);
		}	// if (0 != (oldMask & SURFACES))
        if (0 != (oldMask & VOLUMES))
        {
            if (0 != _volumicActor)
                _renderer->RemoveViewProp (_volumicActor);
        }   // if (0 != (oldMask & VOLUMES))
        if (0 != (oldMask & ASSOCIATIONS))
        {
			if (0 != _vectAssActor)
				_renderer->RemoveViewProp (_vectAssActor);
        }   // if (0 != (oldMask & ASSOCIATIONS))
        if (0 != (oldMask & TRIHEDRON))
        {
			if (0 != _triedronXActor){
				_renderer->RemoveViewProp (_triedronXActor);
				_renderer->RemoveViewProp (_triedronYActor);
				_renderer->RemoveViewProp (_triedronZActor);
			}
        }   // if (0 != (oldMask & TRIHEDRON))
        if (0 != (oldMask & DISCRETISATIONTYPE))
        {
			if (0 != _discActor)
				_renderer->RemoveViewProp (_discActor);
        }   // if (0 != (oldMask & DISCRETISATIONTYPE))
        if (true == GraphicalEntityRepresentation::displayTextualRepresentation (oldMask))
        {
			if (0 != _textualInfosActor)
				_renderer->RemoveViewProp (_textualInfosActor);
        }   // if (true == ...
	}	// if (0 != _renderer)

	// Actualisation des représentations :
	RenderedEntityRepresentation::updateRepresentation (mask, force);

	if (true == force)
		destroyRepresentations (true);

	// On réaffiche tout :
	// On prend ici en compte le mode d'affichage des propriétés : global
	// ou non (MGXDDD-208) :
	const unsigned long	usedMask	= getUsedRepresentationMask ( );
	// Rem CP : pas de levée d'exception car en cas d'erreur on introduit un
	// bogue supplémentaire (non cohérence du mask et des Add/RemoveViewProp
	if (0 != (usedMask & SURFACES))
	{
	    if (0 == _surfacicGrid)
	        createSurfacicRepresentation ( );
	    if (0 != _renderer)
	        if (0 != _surfacicActor){
	            _renderer->AddViewProp (_surfacicActor);
	        }
	        else
	            cerr << "Erreur interne en " << __FILE__ << ' ' << __LINE__
	            << " : _surfacicActor non instancié." << endl;
	}	// if (0 != (usedMask & SURFACES))
	if (0 != (usedMask & VOLUMES))
	{
	    if (0 == _volumicGrid)
	        createVolumicRepresentation ( );
	    // _volumicActor peut il être nul sans qu'on soit en erreur ???
	    if (0 != _renderer && 0 != _volumicActor){
	        _renderer->AddViewProp (_volumicActor);
	    }
	}   // if (0 != (usedMask & SURFACES))
	if (0 != (usedMask & ISOCURVES))
	{
		if (0 == _isoWireGrid)
			createIsoWireRepresentation ( );
		if (0 != _renderer)
			if (0 != _isoWireActor)
				_renderer->AddViewProp (_isoWireActor);
			else
				cerr << "Erreur interne en " << __FILE__ << ' ' << __LINE__
				     << " : _isoWireActor non instancié." << endl;
	}	// if (0 != (usedMask & ISOCURVES))
	if ((0 != (usedMask & CURVES)))
	{
//		if (0 == _wireGrid)
// CP : ATTENTION à ce test, selon les cas on instancie ou non _wireGrid,
// dans certains cas on utilise un vtkExtractEdges sur une peau et donc
// _wireGrid est nulle.
		if ((0 == _wireActor) || (0 == _wireMapper)){
		    if (0 == (usedMask & MESH_SHAPE))
		        createWireRepresentation ( );
		    else
		        createMeshShapeWireRepresentation();
		}
		if (0 != _renderer)
			if (0 != _wireActor)
				_renderer->AddViewProp (_wireActor);
			else
				cerr << "Erreur interne en " << __FILE__ << ' ' << __LINE__
				     << " : _wireActor non instancié." << endl;
	}	// if (0 != (usedMask & CURVES))
	if (0 != (usedMask & ASSOCIATIONS))
	{
		if (0 == _vectAssActor)
			createAssociationVectorRepresentation ( );
		if (0 != _renderer)
			if (0 != _vectAssActor)
				_renderer->AddViewProp (_vectAssActor);
// CP : else on n'affiche rien, cela peut correspondre au cas où le vecteur
// représentant l'association est de taille nulle.
//			else
//				cerr << "Erreur interne en " << __FILE__ << ' ' << __LINE__
//				     << " : _vectAssActor non instancié." << endl;
	}	// if (0 != (usedMask & ASSOCIATIONS))
	if (0 != (usedMask & TRIHEDRON))
	{
		if (0 == _triedronXActor)
			createTrihedronRepresentation ( );
		if (0 != _renderer)
			if (0 != _triedronXActor){
				_renderer->AddViewProp (_triedronXActor);
				_renderer->AddViewProp (_triedronYActor);
				_renderer->AddViewProp (_triedronZActor);
			}
	}	// if (0 != (usedMask & TRIHEDRON))
	if (0 != (usedMask & CLOUDS))
	{
		if (0 == _cloudGrid){
			if (0 == (usedMask & MESH_SHAPE))
				createCloudRepresentation ( );
			else
				 createMeshShapeCloudRepresentation();
		}
		if (0 != _renderer)
			if (0 != _cloudActor)
				_renderer->AddViewProp (_cloudActor);
			else
				cerr << "Erreur interne en " << __FILE__ << ' ' << __LINE__
				     << " : _cloudActor non instancié." << endl;
	}	// if (0 != (usedMask & CLOUDS))

	if (true == GraphicalEntityRepresentation::displayTextualRepresentation (usedMask))
	{
		if (0 == _textualInfosActor)
			createTextualRepresentation ( );

		if (0 != _renderer)
			if (0 != _textualInfosActor)
				_renderer->AddViewProp (_textualInfosActor);
			else
				cerr << "Erreur interne en " << __FILE__ << ' ' << __LINE__
				     << " : _textualInfosActor non instancié." << endl;
	}	// if (true == GraphicalEntityRepresentation::...

    if (0 != (usedMask & DISCRETISATIONTYPE))
	{
    	if (0 == _discGrid)
    		createDiscretisationRepresentation();
    	if (0 != _renderer)
    		if (0 != _discActor)
    				_renderer->AddViewProp (_discActor);
    		else
    			cerr << "Erreur interne en " << __FILE__ << ' ' << __LINE__
    				<< " : _discActor non instancié." << endl;
    }	// if (0 != (usedMask & DISCRETISATIONTYPE))

	updateRepresentationProperties ( );
}	// VTKEntityRepresentation::updateRepresentation


void VTKEntityRepresentation::updateRepresentationProperties ( )
{
	if (0 == getEntity ( ))
		return;

	//std::cout<<"VTKEntityRepresentation::updateRepresentationProperties pour "<< getEntity()->getName()<<std::endl;

	vtkProperty*				property	= 0;
//	const DisplayProperties&	properties	=
//									getEntity ( )->getDisplayProperties ( );
	const DisplayProperties	properties	= getDisplayPropertiesAttributes ( );
//	const unsigned long			mask	= getRepresentationMask ( );
	const unsigned long			mask	= getUsedRepresentationMask ( );

	// Mode nuage :
	property	= 0 == _cloudActor ? 0 : _cloudActor->GetProperty ( );
	if (0 != property)
	{
		const Color	color	= getColor (CLOUDS);
		const float	pointSize	= getPointSize (*getEntity ( ), CLOUDS);
		property->SetPointSize (pointSize);
//		property->SetPointSize (properties.getPointSize ( ));
		property->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
		property->SetOpacity (properties.getCloudOpacity ( ));
		property->SetRepresentationToPoints ( );
	}	// if (0 != property)

	// Mode filaire :
	property	= 0 == _wireActor ? 0 : _wireActor->GetProperty ( );
	if (0 != property)
	{
		const Color	color	= getColor (CURVES);
		const float	lineWidth	= getLineWidth (*getEntity ( ), CURVES);
		property->SetLineWidth (lineWidth);
		property->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
		property->SetRepresentationToSurface ( );
//property->SetRepresentationToWireframe ( );
property->SetInterpolationToFlat ( );
		property->SetOpacity (properties.getWireOpacity ( ));
	}	// if (0 != property)

	// Mode filaire Iso :
	property	= 0 == _isoWireActor ? 0 : _isoWireActor->GetProperty ( );
	if (0 != property)
	{
		const Color	color	= getColor (ISOCURVES);
		const float	lineWidth	= getLineWidth (*getEntity ( ), ISOCURVES);
		property->SetLineWidth (lineWidth);
		property->SetLineStipplePattern (createLineStipplePattern (4, 2));
		property->SetLineStippleRepeatFactor (5);
		property->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
		property->SetOpacity (properties.getWireOpacity ( ));
//property->SetRepresentationToWireframe ( );
	}	// if (0 != property)

	// Mode surfacique :
	property	= 0 == _surfacicActor ? 0 : _surfacicActor->GetProperty ( );
	if (0 != property)
	{
	    // [EB] on ne change la couleur que des parties filaires pour les maillages
		const Color	color	= getColor (SURFACES);
		property->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
		const float	lineWidth	= getLineWidth (*getEntity ( ), SURFACES);
		property->SetLineWidth (lineWidth);
		property->SetOpacity (properties.getSurfacicOpacity ( ));
		property->SetRepresentationToSurface ( );
	}	// if (0 != property)

	// Mode volumique :
    	property    = 0 == _volumicActor ? 0 : _volumicActor->GetProperty ( );
    	if (0 != property)
   	{
		const Color	color	= getColor (VOLUMES);
        	property->SetColor (
                    color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
		const float	lineWidth	= getLineWidth (*getEntity ( ), VOLUMES);
		property->SetLineWidth (lineWidth);
		property->SetOpacity (properties.getVolumicOpacity ( ));
		property->SetRepresentationToSurface ( );
    	}   // if (0 != property)

	// Informations textuelles :
	vtkTextProperty*	textProperty	= 0 == _textualInfosMapper ?
								0 : _textualInfosMapper->GetTextProperty ( );
	if (0 != textProperty)	// => _textualInfosMapper != 0
	{
		const string	text	= getRepresentationText (mask);
		_textualInfosMapper->SetInput (text.c_str ( ));

	
		const DisplayProperties	properties	= getDisplayPropertiesAttributes( );
		int		fontFamily	= 0,		fontSize	= 12;
		bool	bold		= false,	italic		= false;
		Color	color (0, 0, 0);
		properties.getFontProperties(fontFamily, fontSize, bold, italic, color);
		// Par défaut on utilise la couleur majoritaire de la représentation.
		if (0 != (VOLUMES & mask))
			color	= getColor (VOLUMES);
		else if (0 != (SURFACES & mask))
			color	= getColor (SURFACES);
		else if (0 != (CURVES & mask))
			color	= getColor (CURVES);
		else if (0 != (CLOUDS & mask))
			color	= getColor (CLOUDS);
		textProperty->SetColor (
						color.getRed ( ) / 255., color.getGreen ( ) / 255.,
						color.getBlue ( ) / 255.);
		textProperty->SetFontFamily (fontFamily);
		textProperty->SetFontSize (fontSize);
		textProperty->SetBold (bold);
		textProperty->SetItalic (italic);
		textProperty->ShadowOff ( );
	}	// if (0 != textProperty)
	if (0 != _textualInfosActor)
		_textualInfosActor->PickableOff ( );

	// Actualisation des éventuelles valeurs (noeuds/mailles) affichées :
	vtkUnstructuredGrid*	grid		= 0 == _surfacicGrid ? _volumicGrid : _surfacicGrid;
	vtkDataSetMapper*	mapper		= 0 == _surfacicGrid ? _volumicMapper : _surfacicMapper;
	vtkPoints*		points		= 0 == grid ? 0 : grid->GetPoints ( );
	vtkPointData*		pointData	= 0 == grid ? 0 : grid->GetPointData ( );
	vtkCellData*		cellData	= 0 == grid ? 0 : grid->GetCellData ( );
	double			domain [2]	= { 0., 0. };
	// CP ATTENTION : si on affiche un volume, le fait d'utiliser _surfacicGrid
	// laisse supposer qu'on affiche la peau, donc beaucoup de noeuds et mailles
	// sont écartés. Les valeurs aux noeuds/mailles affichés doivent
	// correspondre à ceux affichés.
	if (0 != mapper)
	{
    		if ((0 != (mask & NODES_VALUES)) && (0 != pointData))
		{
			// On récupère l'éventuel tableau existant :
			vtkDataArray*	data	=
					pointData->GetArray (properties.getValueName ( ).c_str ( ));
			vtkDoubleArray*	values	= dynamic_cast<vtkDoubleArray*>(data);
			if (0 == values)
			{
				values	= vtkDoubleArray::New ( );
				pointData->AddArray (values);
				values->Delete ( );	// => Ref : 2 -> 1, le tableau existe toujours
			}	// if (0 != values)

			double*			ptr		=
						values->WritePointer (0, points->GetNumberOfPoints ( ));
			CHECK_NULL_PTR_ERROR (ptr)
			values->SetNumberOfTuples (points->GetNumberOfPoints ( ));
			std::map<int,double>	nodesValue = getEntity ( )->getNodesValue (properties.getValueName ( ));
			std::map<int,int> vtk2gmdsIDPoints =
				0 == _surfacicGrid ? _volumicPointsVTK2GMDSID : _surfacicPointsVTK2GMDSID;
			double minValue =  HUGE_VALF;
			double maxValue = -HUGE_VALF;
			if (nodesValue.size ( ) < points->GetNumberOfPoints ( ))
			{
				UTF8String	error (Charset::UTF_8);
				error << "La valeur aux noeuds " << properties.getValueName ( )
				      << " de l'entité " << getEntity ( )->getName ( )
				      << " n'a que " << nodesValue.size ( )
				      << " éléments alors qu'elle a "
				      << points->GetNumberOfPoints ( ) << " noeuds.";
				INTERNAL_ERROR (exc, error, "VTKEntityRepresentation::updateRepresentationProperties")
				throw exc;
			}	// if (nodesValue.size ( ) < points->GetNumberOfPoints ( ))

			for (size_t n = 0; n < points->GetNumberOfPoints ( ); n++) {
//				*ptr++	= n;
//				*ptr++	= nodesValue[n];

				*ptr++	= nodesValue[vtk2gmdsIDPoints[n]];
				if(nodesValue[vtk2gmdsIDPoints[n]] < minValue) minValue = nodesValue[vtk2gmdsIDPoints[n]];
				if(nodesValue[vtk2gmdsIDPoints[n]] > maxValue) maxValue = nodesValue[vtk2gmdsIDPoints[n]];
			}
			values->SetName (properties.getValueName ( ).c_str ( ));
			pointData->SetScalars (values);
			pointData->SetActiveScalars (properties.getValueName ( ).c_str ( ));
			values->GetRange (domain);
			if (0 != mapper)
			{
				mapper->ScalarVisibilityOn ( );
//				mapper->SetScalarRange (0., points->GetNumberOfPoints ( ));
				mapper->SetScalarRange (minValue, maxValue);
			}	// if (0 != mapper)
			RenderingManager::ColorTableDefinition	colorTableDefinition (
					properties.getValueName ( ), USHRT_MAX, domain [0], domain [1]);
			VTKRenderingManager::VTKColorTable*	colorTable	=
				0 == _renderingManager ?
				0 : dynamic_cast<VTKRenderingManager::VTKColorTable*>(
					_renderingManager->getColorTable (colorTableDefinition));
			CHECK_NULL_PTR_ERROR (colorTable)
			if (0 != mapper)
			{
				mapper->UseLookupTableScalarRangeOn ( );
				mapper->SetLookupTable (&colorTable->getLookupTable ( ));
			}	// if (0 != mapper)
		}	// if (0 != (mask & NODES_VALUES))
		else if ((0 != cellData) && (0 != (mask & CELLS_VALUES)))
		{	
			// On récupère l'éventuel tableau existant :
			vtkDataArray*	data	= cellData->GetArray (properties.getValueName ( ).c_str ( ));
			vtkDoubleArray*	values	= dynamic_cast<vtkDoubleArray*>(data);
			if (0 == values)
			{
				values	= vtkDoubleArray::New ( );
				cellData->AddArray (values);
				values->Delete ( );	// => Ref : 2 -> 1, le tableau existe toujours
			}	// if (0 != values)

			double*			ptr		=
					values->WritePointer (0, grid->GetNumberOfCells ( ));
			CHECK_NULL_PTR_ERROR (ptr)
			values->SetNumberOfTuples (grid->GetNumberOfCells ( ));
// CP : récupérer ici les vraies valeurs aux mailles :
			for (size_t n = 0; n < grid->GetNumberOfCells ( ); n++)
				*ptr++	= n;
			values->SetName (properties.getValueName ( ).c_str ( ));
			cellData->SetScalars (values);
			cellData->SetActiveScalars (properties.getValueName ( ).c_str ( ));
			values->GetRange (domain);
			if (0 != mapper)
			{
				mapper->ScalarVisibilityOn ( );
				mapper->SetScalarRange (0., grid->GetNumberOfCells ( ));
			}	// if (0 != mapper)
			RenderingManager::ColorTableDefinition	colorTableDefinition (
				properties.getValueName ( ), USHRT_MAX, domain [0], domain [1]);
			VTKRenderingManager::VTKColorTable*	colorTable	=
				0 == _renderingManager ?
				0 : dynamic_cast<VTKRenderingManager::VTKColorTable*>(
					_renderingManager->getColorTable (colorTableDefinition));
			CHECK_NULL_PTR_ERROR (colorTable)
			if (0 != mapper)
			{
				mapper->UseLookupTableScalarRangeOn ( );
				mapper->SetLookupTable (&colorTable->getLookupTable ( ));
			}	// if (0 != mapper)
		}	// if ((0 != cellData) && (0 != (mask & CELLS_VALUES)))
	}	// if (0 != mapper)
}	// VTKEntityRepresentation::updateRepresentationProperties


void VTKEntityRepresentation::updateHighlightRepresentation (bool highlight)
{
	VTKRenderingManager*	renderingManager
						= dynamic_cast<VTKRenderingManager*>(_renderingManager);
	if ((0 == renderingManager) || (0 == _renderer))
		return;

	if (false == highlight)
	{
		if (0 != _highlightActor)
		{
			_renderer->RemoveViewProp (_highlightActor);
			_highlightActor->Delete ( );
		}
		_highlightActor	= 0;
		if (0 != _highlightMapper)
			_highlightMapper->Delete ( );
		_highlightMapper	= 0;
		if (0 != _outlineSource)
			_outlineSource->Delete ( );
		_outlineSource	= 0;

		// On a peut être forcé l'affichage de l'entité pour le highlighting :
		if ((0 != getEntity ( )) &&
		    (false == getEntity ( )->getDisplayProperties ( ).isDisplayed ( )))
			destroyRepresentations (destroyDataOnHide ( ));

		// Si l'entité est représentée alors on actualise si nécessaire sa
		// représentation.
		updateRepresentationProperties ( );

		// L'évènement vient d'ailleurs que la fenêtre graphique => forcer son
		// actualisation :
		if (0 != _renderer->GetRenderWindow ( ))
			_renderer->GetRenderWindow ( )->Render ( );

		return;
	}	// if (false == highlight)

	// On force l'affichage de l'entité si nécessaire pour le highlighting :
	if ((0 != getEntity ( )) &&
	    (false == getEntity ( )->getDisplayProperties ( ).isDisplayed ( )))
			updateRepresentation (getRepresentationMask ( ), true);

	if (true == Resources::instance ( )._useHighLightColor.getValue ( ))
		updateRepresentationProperties ( );

	if ((true == highlight) &&
	    (true == Resources::instance ( )._highLightBoundinBox.getValue ( )))
	{
		if (0 == _highlightActor)
		{
			_highlightActor	= vtkActor::New ( );
			_highlightActor->PickableOff ( );
			_highlightActor->DragableOff ( );
			CHECK_NULL_PTR_ERROR (_highlightActor->GetProperty ( ))
			_highlightActor->GetProperty ( )->SetColor (
					255 * Resources::instance ( )._highlightColor.getRed( ),
					255 * Resources::instance ( )._highlightColor.getGreen( ),
					255 * Resources::instance ( )._highlightColor.getBlue( ));
			_highlightActor->GetProperty ( )->SetLineWidth (
							Resources::instance ( )._highLightWidth.getValue ( ));
			_highlightActor->GetProperty ( )->SetAmbient (1.0);
			_highlightActor->GetProperty ( )->SetDiffuse (0.0);
		}	// if (0 == _highlightActor)
		if (0 == _highlightMapper)
		{
			_highlightMapper	= vtkPolyDataMapper::New ( );
			_outlineSource		= vtkOutlineSource::New ( );
#ifndef VTK_5
			_highlightMapper->SetInputData (_outlineSource->GetOutput ( ));
#else	// VTK_5
			_highlightMapper->SetInput (_outlineSource->GetOutput ( ));
#endif	// VTK_5
			_highlightActor->SetMapper (_highlightMapper);
		}	// if (0 == _highlightMapper)

		if (0 != getEntity ( ))
		{
			double	xMargin = 0., yMargin = 0., zMargin = 0.;
			double	bounds [6];
			renderingManager->getBoundingBoxMargins(xMargin, yMargin, zMargin);
			getEntity ( )->getBounds (bounds);
			bounds [0]	-= xMargin;		bounds [1]	+= xMargin;
			bounds [2]	-= yMargin;		bounds [3]	+= yMargin;
			bounds [4]	-= zMargin;		bounds [5]	+= zMargin;
			if (0 != _outlineSource)
				_outlineSource->SetBounds (bounds);
			_renderer->AddViewProp (_highlightActor);
		}	// if (0 != getEntity ( ))
	}	// if (true == highlight) && ...

	// L'évènement vient d'ailleurs que la fenêtre graphique => forcer son
	// actualisation :
	if (0 != _renderer->GetRenderWindow ( ))
		_renderer->GetRenderWindow ( )->Render ( );
}	// VTKEntityRepresentation::updateHighlightRepresentation


void VTKEntityRepresentation::bringToTop ( )
{
	if (0 != _renderer)
	{
		if (0 != _textualInfosActor)
		{
			_renderer->RemoveViewProp (_textualInfosActor);
			_renderer->AddViewProp (_textualInfosActor);
		}	// if (0 != _textualInfosActor)
		if (0 != _cloudActor)
		{
			_renderer->RemoveViewProp (_cloudActor);
			_renderer->AddViewProp (_cloudActor);
		}	// if (0 != _cloudActor)
		if (0 != _isoWireActor)
		{
			_renderer->RemoveViewProp (_isoWireActor);
			_renderer->AddViewProp (_isoWireActor);
		}	// if (0 != _isoWireActor)
		if (0 != _wireActor)
		{
			_renderer->RemoveViewProp (_wireActor);
			_renderer->AddViewProp (_wireActor);
		}	// if (0 != _wireActor)
		if (0 != _volumicActor)
		{
			_renderer->RemoveViewProp (_volumicActor);
			_renderer->AddViewProp (_volumicActor);
		}	// if (0 != _volumicActor)
		if (0 != _surfacicActor)
		{
			_renderer->RemoveViewProp (_surfacicActor);
			_renderer->AddViewProp (_surfacicActor);
		}	// if (0 != _surfacicActor)
		if (0 != _vectAssActor)
		{
			_renderer->RemoveViewProp (_vectAssActor);
			_renderer->AddViewProp (_vectAssActor);
		}	// if (0 != _vectAssActor)
		if (0 != _triedronXActor)
		{
			_renderer->RemoveViewProp (_triedronXActor);
			_renderer->RemoveViewProp (_triedronYActor);
			_renderer->RemoveViewProp (_triedronZActor);
			_renderer->AddViewProp (_triedronXActor);
			_renderer->AddViewProp (_triedronYActor);
			_renderer->AddViewProp (_triedronZActor);
		}	// if (0 != _trihedron)
		if (0 != _discActor)
		{
			_renderer->RemoveViewProp (_discActor);
			_renderer->AddViewProp (_discActor);
		}	// if (0 != _discActor)
	}	// if (0 != _renderer)
}	// VTKEntityRepresentation::bringToTop


void VTKEntityRepresentation::setRenderingManager (RenderingManager* rm)
{
	_renderer	= 0;
	RenderedEntityRepresentation::setRenderingManager (rm);
	VTKRenderingManager*	vrm	= dynamic_cast<VTKRenderingManager*>(rm);
	if (0 != vrm)
		_renderer	= &vrm->getRenderer ( );
CHECK_NULL_PTR_ERROR (_renderer)
}	// VTKEntityRepresentation::setRenderingManager


vtkUnstructuredGrid* VTKEntityRepresentation::getSurfacicGrid ( )
{
	return _surfacicGrid;
}	// VTKEntityRepresentation::getSurfacicGrid


vtkActor* VTKEntityRepresentation::getSurfacicActor ( )
{
	return _surfacicActor;
}	// VTKEntityRepresentation::getSurfacicActor


vtkDataSetMapper* VTKEntityRepresentation::getSurfacicMapper ( )
{
	return _surfacicMapper;
}	// VTKEntityRepresentation::getSurfacicMapper


vtkUnstructuredGrid* VTKEntityRepresentation::getVolumicGrid ( )
{
	return _volumicGrid;
}	// VTKEntityRepresentation::getVolumicGrid


vtkActor* VTKEntityRepresentation::getVolumicActor ( )
{
	return _volumicActor;
}	// VTKEntityRepresentation::getVolumicActor


vtkDataSetMapper* VTKEntityRepresentation::getVolumicMapper ( )
{
	return _volumicMapper;
}	// VTKEntityRepresentation::getVolumicMapper


vtkUnstructuredGrid* VTKEntityRepresentation::getRefinedGrid ( )
{
	if (0 != _refineFilter)
		return _refineFilter->GetOutput ( );

	return _refinedGrid;
}	// VTKEntityRepresentation::getRefinedGrid


vtkActor* VTKEntityRepresentation::getRefinedActor ( )
{
	return _refinedActor;
}	// VTKEntityRepresentation::getRefinedActor


vtkDataSetMapper* VTKEntityRepresentation::getRefinedMapper ( )
{
	return _refinedMapper;
}	// VTKEntityRepresentation::getRefinedMapper


void VTKEntityRepresentation::destroyRefinedRepresentation ( )
{
	if (0 != _refineFilter)
		_refineFilter->Delete ( );
	_refineFilter	= 0;
	if (0 != _refinedActor)
	{
		if ((0 != _renderer) && (0 != _renderer->HasViewProp (_refinedActor)))
			_renderer->RemoveViewProp (_refinedActor);
		_refinedActor->Delete ( );
		_refinedActor	= 0;
	}	// if (0 != _refinedActor
	if (0 != _refinedMapper)
		_refinedMapper->Delete ( );
	_refinedMapper	= 0;
	if (0 != _refinedGrid)
		_refinedGrid->Delete ( );
	_refinedGrid	= 0;	
}	// VTKEntityRepresentation::destroyRefinedRepresentation


void VTKEntityRepresentation::destroyRepresentations (bool realyDestroy)
{
	if (0 != _highlightActor)
	{
		if ((0 != _renderer) && (0 != _renderer->HasViewProp (_highlightActor)))
			_renderer->RemoveViewProp (_highlightActor);
		if (true == realyDestroy)
		{
			_highlightActor->Delete ( );
			_highlightActor	= 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _highlightActor
	if (true == realyDestroy)
	{
		if (0 != _highlightMapper)
			_highlightMapper->Delete ( );
		_highlightMapper	= 0;
		if (0 != _outlineSource)
			_outlineSource->Delete ( );
		_outlineSource	= 0;
	}	// if (true == realyDestroy)

	if (0 != _vectAssActor)
	{
		if ((0 != _renderer) && (0 != _renderer->HasViewProp (_vectAssActor)))
			_renderer->RemoveViewProp (_vectAssActor);
		if (true == realyDestroy)
		{
			_vectAssActor->Delete ( );
			_vectAssActor	= 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _vectAssActor)
	if (true == realyDestroy)
	{
		if (0 != _vectAssMapper)
			_vectAssMapper->Delete ( );
		_vectAssMapper	= 0;
		if (0 != _vectAssArrow)
			_vectAssArrow->Delete ( );
		_vectAssArrow	= 0;
	}	// if (true == realyDestroy)

	if (0 != _surfacicActor)
	{
		if ((0 != _renderer) && (0 != _renderer->HasViewProp (_surfacicActor)))
			_renderer->RemoveViewProp (_surfacicActor);
		if (true == realyDestroy)
		{
			_surfacicActor->Delete ( );
			_surfacicActor	= 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _surfacicActor)
	if (true == realyDestroy)
	{
		if (0 != _surfacicMapper)
			_surfacicMapper->Delete ( );
		_surfacicMapper	= 0;
		if (0 != _surfacicGrid)
			_surfacicGrid->Delete ( );
		_surfacicGrid	= 0;
	}	// if (true == realyDestroy)

    if (0 != _volumicActor)
	{
		if ((0 != _renderer) && (0 != _renderer->HasViewProp (_volumicActor)))
			_renderer->RemoveViewProp (_volumicActor);
		if (true == realyDestroy)
		{
        	_volumicActor->Delete ( );
		    _volumicActor  = 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _volumicActor)
	if (true == realyDestroy)
	{
   		 if (0 != _volumicMapper)
   		     _volumicMapper->Delete ( );
   		 _volumicMapper = 0;
   		 if (0 != _volumicGrid)
   		     _volumicGrid->Delete ( );
	    _volumicGrid   = 0;
	}	// if (true == realyDestroy)

	if (0 != _isoWireActor)
	{
		if ((0 != _renderer) && (0 != _renderer->HasViewProp (_isoWireActor)))
			_renderer->RemoveViewProp (_isoWireActor);
		if (true == realyDestroy)
		{
			_isoWireActor->Delete ( );
			_isoWireActor	= 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _isoWireActor)
	if (true == realyDestroy)
	{
		if (0 != _isoWireMapper)
			_isoWireMapper->Delete ( );
		_isoWireMapper	= 0;
		if (0 != _isoWireGrid)
			_isoWireGrid->Delete ( );
		_isoWireGrid	= 0;
	}	// if (true == realyDestroy)

	if (0 != _wireActor)
	{
		if ((0 != _renderer) && (0 != _renderer->HasViewProp (_wireActor)))
			_renderer->RemoveViewProp (_wireActor);
		if (true == realyDestroy)
		{
			_wireActor->Delete ( );
			_wireActor	= 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _wireActor)
	if (true == realyDestroy)
	{
		if (0 != _wireMapper)
			_wireMapper->Delete ( );
		_wireMapper	= 0;
		if (0 != _wireGrid)
			_wireGrid->Delete ( );
		_wireGrid	= 0;
	}	// if (true == realyDestroy)

	if (0 != _discActor)
	{
		if ((0 != _renderer) && (0 != _renderer->HasViewProp (_discActor)))
			_renderer->RemoveViewProp (_discActor);
		if (true == realyDestroy)
		{
			_discActor->Delete ( );
			_discActor	= 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _discActor)
	if (true == realyDestroy)
	{
		if (0 != _discMapper)
			_discMapper->Delete ( );
		_discMapper	= 0;
		if (0 != _discGrid)
			_discGrid->Delete ( );
		_discGrid	= 0;
	}	// if (true == realyDestroy)

	if (0 != _cloudActor)
	{
		if ((0 != _renderer) && (0 != _renderer->HasViewProp (_cloudActor)))
			_renderer->RemoveViewProp (_cloudActor);
		if (true == realyDestroy)
		{
			_cloudActor->Delete ( );
			_cloudActor	= 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _cloudActor)
	if (true == realyDestroy)
	{
		if (0 != _cloudMapper)
			_cloudMapper->Delete ( );
		_cloudMapper	= 0;
		if (0 != _cloudGrid)
			_cloudGrid->Delete ( );
		_cloudGrid	= 0;
	}	// if (true == realyDestroy)

	if (0 != _textualInfosActor)
	{
		if ((0 != _renderer) &&
		    (0 != _renderer->HasViewProp (_textualInfosActor)))
			_renderer->RemoveViewProp (_textualInfosActor);
		if (true == realyDestroy)
		{
			_textualInfosActor->Delete ( );
			_textualInfosActor	= 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _textualInfosActor)
	if (true == realyDestroy)
	{
		if (0 != _textualInfosMapper)
			_textualInfosMapper->Delete ( );
		_textualInfosMapper	= 0;
	}	// if (true == realyDestroy)
	if (0 != _triedronXActor)
	{
		if ((0 != _renderer) &&
		    (0 != _renderer->HasViewProp (_triedronXActor))){
			_renderer->RemoveViewProp (_triedronXActor);
			_renderer->RemoveViewProp (_triedronYActor);
			_renderer->RemoveViewProp (_triedronZActor);
		}
		if (true == realyDestroy)
		{
			_triedronXActor->Delete ( );
			_triedronYActor->Delete ( );
			_triedronZActor->Delete ( );
			_triedronXActor	= 0;
			_triedronYActor	= 0;
			_triedronZActor	= 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _triedronXActor)
	if (true == realyDestroy)
	{
		if (0 != _triedronXMapper){
			_triedronXMapper->Delete ( );
			_triedronYMapper->Delete ( );
			_triedronZMapper->Delete ( );
		}
		if (0 != _trihedronArrowX){
			_trihedronArrowX->Delete ( );
			_trihedronArrowY->Delete ( );
			_trihedronArrowZ->Delete ( );
		}
		_triedronXMapper = 0;
		_triedronYMapper = 0;
		_triedronZMapper = 0;
		_trihedronArrowX = 0;
		_trihedronArrowY = 0;
		_trihedronArrowZ = 0;

	}	// if (true == realyDestroy)
}	// VTKEntityRepresentation::destroyRepresentations


int VTKEntityRepresentation::createLineStipplePattern (int on, int off)
{
    int pattern = 1;
    const int   bits    = sizeof (int) * 8;
    int         guard   = 0;

    // Pour sizeof (int) = 4 :
    // - on = 1 off = 1 : PATTERN = 2863311530
    // - on = 4 off = 1 : PATTERN = 4156419963 <= Pas mal
    // - on = 8 off = 1 : PATTERN = 4286562271
    // - on = 1 off = 4 : PATTERN = 2216757314 <= Trop d'espace
    // - on = 1 off = 8 : PATTERN = 2151686160 <= Beaucoup trop d'espace
    const int   total   = on + off;
    for (int i = 0; (i < bits - 1) && (guard < bits); i += total)
    {
        for (int j = 0; (j < on) && (guard < bits); j++, guard++)
        {
            pattern = pattern << 1;
            pattern = pattern | 1;
        }   // for (int j = 0; (j < on) && (guard < bits); j++, guard++)
        for (int k = 0; (k < off) && (guard < bits); k++, guard++)
            pattern = pattern << 1;
    }   // for (int i = 0; i < bytes * 8 - 3; i += 3)

    return pattern;
}   // createLineStipplePattern


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D
