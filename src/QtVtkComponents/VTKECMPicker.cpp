#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKECMPicker.h"
#include "QtVtkComponents/VTKConfiguration.h"
#include "QtVtkComponents/VTKMgx3DActor.h"
#include "Utils/Entity.h"

#include <TkUtil/NumericServices.h>

#include <vtkBox.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkMath.h>
#include <vtkPoints.h>
#include <vtkProp3DCollection.h>

#include <assert.h>
#include <iostream>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;


// ===================================================================================================================================================
//                                                          LA CLASSE VTKECMBoundingBoxPicker
// ===================================================================================================================================================

VTKECMBoundingBoxPicker::VTKECMBoundingBoxPicker ( )
	: vtkPicker ( )
{
}	// VTKECMBoundingBoxPicker::VTKECMBoundingBoxPicker


VTKECMBoundingBoxPicker::VTKECMBoundingBoxPicker (const VTKECMBoundingBoxPicker&)
{
}	// VTKECMBoundingBoxPicker::VTKECMBoundingBoxPicker


VTKECMBoundingBoxPicker& VTKECMBoundingBoxPicker::operator = (const VTKECMBoundingBoxPicker&)
{
	return *this;
}	// VTKECMBoundingBoxPicker::operator =


VTKECMBoundingBoxPicker::~VTKECMBoundingBoxPicker ( )
{
}	// VTKECMBoundingBoxPicker::~VTKECMBoundingBoxPicker


int VTKECMBoundingBoxPicker::Pick (double selectionX, double selectionY, double selectionZ, vtkRenderer *renderer)
{
	int	picked	= 0;

	// Source d'inspiration : vtkPicker::Pick de VTK 5.10.0
	// Pb : les points d'intersection avec les boites englobantes sont suspects,
	// ils ne semblent pas être les plus proches de l'observateur.
	if (NULL == renderer)
	{
		vtkErrorMacro(<<"VTKECMBoundingBoxPicker::Pick. Must specify renderer!");
		return 0;
	}	// if (NULL == renderer)

	Initialize ( );

	//  Initialize picking process
	Renderer	= renderer;
	SelectionPoint [0]	= selectionX;
	SelectionPoint [1]	= selectionY;
	SelectionPoint [2]	= selectionZ;

	// Invoke start pick method if defined :
	InvokeEvent (vtkCommand::StartPickEvent, NULL);

	// Get camera focal point and position. Convert to display (screen)
	// coordinates. We need a depth value for z-buffer.
	double	cameraPos [4]	= { 0., 0., 0., 0.};
	double	cameraFP [4]	= { 0., 0., 0., 0.};
	vtkCamera*	camera	 = renderer->GetActiveCamera ( );
	camera->GetPosition (cameraPos);
	camera->GetFocalPoint (cameraFP);
	cameraPos [3]	= 1.;
	cameraFP [3]	= 1.;
	renderer->SetWorldPoint (cameraFP[0], cameraFP[1], cameraFP[2], cameraFP[3]);
	renderer->WorldToDisplay ( );
	double*	displayCoords	= renderer->GetDisplayPoint ( );
	selectionZ				= displayCoords[2];
	// Convert the selection point into world coordinates :
	renderer->SetDisplayPoint (selectionX, selectionY, selectionZ);
	renderer->DisplayToWorld ( );
	double*	worldCoords	= renderer->GetWorldPoint ( );
	if (worldCoords [3] == 0.0)
	{
		vtkErrorMacro(<<"VTKECMBoundingBoxPicker::Pick. Bad homogeneous coordinates");
		return 0;
	}	// if (worldCoords [3] == 0.0)
	for (int i = 0; i < 3; i++)
		PickPosition [i] = worldCoords [i] / worldCoords [3];

	// Ici VTK cherche le rayon, segment compris entre l'observateur et le point
	// visé tout en prenant en compte le clipping.
	// ...

	vtkPropCollection*	props	= 0 != PickFromList ? GetPickList ( ) : renderer->GetViewProps ( );
	vtkCollectionSimpleIterator	pit;
	vtkProp*			prop	= 0;
	for (props->InitTraversal (pit); (prop = props->GetNextProp (pit)); )
	{	// Version très simplifiée, VTK gère les arborescence de props, les différents types, ...
		if ((0 == prop->GetPickable ( )) || (0 == prop->GetVisibility ( )))
			continue;
	
		double*	bounds	= prop->GetBounds ( );
		double	t1	= 0., t2	= 0.;
		double	x1 [3]	= { 0., 0., 0. };
		double	x2 [3]	= { 0., 0., 0. };
		int	plane1	= 0, plane2	= 0;
		int	intersect	= vtkBox::IntersectWithLine (bounds, cameraPos, PickPosition, t1, t2, x1, x2, plane1, plane2);
		if (0 < intersect)
		{
			picked	= 1;

			// On retient le point d'intersection le plus proche :
			// (Rem : d'après les premiers tests x1 et x2 sont identiques et
			// au plus proche)
			double*	nearest	=
				vtkMath::Distance2BetweenPoints (cameraPos, x1) < vtkMath::Distance2BetweenPoints (cameraPos, x2) ? x1 : x2;

			// The IsItemPresent method returns "index+1" :
			int	prevIndex	= Prop3Ds->IsItemPresent (prop) - 1;
			if (prevIndex >= 0)
			{
				double	oldp [3]	= { 0., 0., 0. };
				PickedPositions->GetPoint (prevIndex, oldp);
				if (vtkMath::Distance2BetweenPoints (cameraPos, nearest) < vtkMath::Distance2BetweenPoints (cameraPos, oldp))
					PickedPositions->SetPoint (prevIndex, nearest);
			}	// if (prevIndex >= 0)
			else
			{
				Prop3Ds->AddItem (static_cast<vtkProp3D*>(prop));
				PickedPositions->InsertNextPoint (nearest);
				// backwards compatibility: also add to this->Actors
				vtkActor*	actor	= vtkActor::SafeDownCast (prop);
				if (0 != actor)
					Actors->AddItem (actor);
			}	// else if (prevIndex >= 0)
		}	// if (0 < intersect)
	}	// for (props->InitTraversal (pit); (prop = props->GetNextProp (pit)); )

	if (0 != Path)
	{	// Invoke pick method if one defined - prop goes first
		Path->GetFirstNode ( )->GetViewProp ( )->Pick ( );
		InvokeEvent (vtkCommand::PickEvent, NULL);
	}	// if (0 != Path)

	// Invoke end pick method if defined
	InvokeEvent (vtkCommand::EndPickEvent,NULL);

	return picked;
}	// VTKECMBoundingBoxPicker::Pick


// ===================================================================================================================================================
//                                                                    LA CLASSE VTKECMPicker
// ===================================================================================================================================================

VTKECMPicker::VTKECMPicker ( )
	: vtkAbstractPropPicker ( ), _mutex ( ), _tolerance (0.005), _picker (0), _cellPicker (0), _mode (BOUNDING_BOX), _actors ( )
{
//	_picker	= vtkPicker::New ( );
	_picker	= new VTKECMBoundingBoxPicker ( );
	_picker->SetTolerance (_tolerance);
	_cellPicker	= vtkCellPicker::New ( );
	_cellPicker->SetTolerance (_tolerance);
}	// VTKECMPicker::VTKECMPicker


VTKECMPicker::VTKECMPicker (const VTKECMPicker&)
	: vtkAbstractPropPicker ( ), _mutex ( ), _tolerance (0.025), _picker (0), _cellPicker (0), _mode (BOUNDING_BOX), _actors ( )
{
	assert (0 && "VTKECMPicker copy constructor is not allowed.");
}	// VTKECMPicker copy constructor


VTKECMPicker& VTKECMPicker::operator = (const VTKECMPicker&)
{
	assert (0 && "VTKECMPicker::operator = is not allowed.");
	return *this;
}	// VTKECMPicker::operator =


VTKECMPicker::~VTKECMPicker ( )
{
}	// VTKECMPicker::~VTKECMPicker


void VTKECMPicker::PrintSelf (ostream& os, vtkIndent indent)
{
	AutoMutex	autoMutex (&GetMutex ( ));

	vtkAbstractPropPicker::PrintSelf (os, indent);
	os << "Mode : ";
	switch (GetMode ( ))
	{
		case VTKECMPicker::BOUNDING_BOX		: os << "BOUNDING_BOX";	break;
		case VTKECMPicker::DISTANCE			: os << "DISTANCE";		break;
		default								: os << "INCONNU (" << (unsigned long)GetMode ( );
	}	// switch (GetMode ( ))
	os << endl
	   << "Tolérance (boite englobante) : " << GetTolerance ( ) << endl
	   << "Tolérance (proximité) : " << GetCellTolerance ( ) << endl;
}	// VTKECMPicker::PrintSelfA


VTKECMPicker* VTKECMPicker::New ( )
{
	return new VTKECMPicker ( );
}	// VTKECMPicker::New


void VTKECMPicker::SetMode (VTKECMPicker::MODE mode)
{
	_mode	= mode;
}	// VTKECMPicker::SetMode


VTKECMPicker::MODE VTKECMPicker::GetMode ( ) const
{
	return _mode;
}	// VTKECMPicker::GetMode


void VTKECMPicker::SetTolerance (double tolerance)
{
	AutoMutex	autoMutex (&GetMutex ( ));

	if (tolerance < 0.)
	{
		cerr << "Erreur dans VTKECMPicker::SetTolerance : tolérance "
		     << "négative (" << tolerance << ")." << endl;
		return;
	}	// if (tolerance < 0.)

	_tolerance		= tolerance;
	if (0 != _picker)
		_picker->SetTolerance (_tolerance);
}	// VTKECMPicker::SetTolerance


double VTKECMPicker::GetTolerance ( ) const
{
	return _tolerance;
}	// VTKECMPicker::GetTolerance


void VTKECMPicker::SetCellTolerance (double tolerance)
{
	AutoMutex	autoMutex (&GetMutex ( ));

	if (tolerance < 0.)
	{
		cerr << "Erreur dans VTKECMPicker::SetCellTolerance : tolérance "
		     << "négative (" << tolerance << ")." << endl;
		return;
	}	// if (tolerance < 0.)

	if (0 != _cellPicker)
		_cellPicker->SetTolerance (tolerance);
}	// VTKECMPicker::SetCellTolerance


double VTKECMPicker::GetCellTolerance ( ) const
{
	AutoMutex	autoMutex (&GetMutex ( ));

	return 0 == _cellPicker ? 0. : _cellPicker->GetTolerance ( );
}	// VTKECMPicker::GetCellTolerance


int VTKECMPicker::Pick (vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z, vtkRenderer* renderer)
{
	AutoMutex	autoMutex (&GetMutex ( ));
	_actors.clear ( );
	vtkPicker*	picker	= GetPicker ( );

	if ((0 == picker) || (0 == renderer))
		return 0;

	Initialize ( );
	InvokeEvent (vtkCommand::StartPickEvent, 0);

	if (0 != picker->Pick (x, y, z, renderer))
	{
		// On recherche la liste des entités pointées et on les ordonne (ce que
		// ne fait pas VTK).
		vtkProp3DCollection*	props	= picker->GetProp3Ds ( );
		vtkPoints*				points	= picker->GetPickedPositions( );
		vtkCamera*				camera	= renderer->GetActiveCamera ( );
		vtkProp*				prop	= picker->GetViewProp ( );
		double					cameraPos [4]		= {0., 0., 0., 0.};
		double					pointedPoint [3]	= {0., 0., 0.};

		if (0 != camera)
		{	// On recherche le rayon utilisé pour le picking :
			camera->GetPosition (cameraPos);
			// Le point visé : (from vtkPicker::Pick) :
			renderer->SetDisplayPoint (x, y, z);
			renderer->DisplayToWorld ( );
			double*	worldCoords	= renderer->GetWorldPoint ( );
			if (worldCoords [3] == 0.0)
			{
cerr << __FILE__ << ' ' << __FILE__ << " ERREUR in VTKECMPicker::Pick : Bad homogeneous coordinates" << endl;
				return 0;
			}	// if (worldCoords [3] == 0.0)
			for (int i = 0; i < 3; i++)
				pointedPoint [i]	= worldCoords [i] / worldCoords [3];

			// On se créé une liste des props pointées triées selon le mode
			// courrant :
			multimap<double,vtkActor*>		objects;
			size_t							index		= 0;
			double							distance	= 0;
			vtkProp3D*						p			= 0;
			vtkCollectionSimpleIterator		csi;
			props->InitTraversal (csi);
			while (0 != (p = props->GetNextProp3D (csi)))
			{
				bool	add	= true;
				vtkActor*	a	= dynamic_cast<vtkActor*>(p);
				if (0 != a)
				{
					if (false == a->GetPickable ( ))
						continue;

					switch (GetMode ( ))
					{
						default	:
cerr << __FILE__ << ' ' << __FILE__ << " VTKECMPicker::Pick. Mode non supporté de VTKECMPicker (" << (unsigned long)GetMode ( ) << ")." << endl;
							break;
						case VTKECMPicker::BOUNDING_BOX	:
//if ((0 != mgxActor) && (false == mgxActor->IsSolid ( )))
//	add	= false;
							distance	= vtkMath::Distance2BetweenPoints (cameraPos, points->GetPoint (index));
							break;
						case VTKECMPicker::DISTANCE		:
//if ((0 != mgxActor) && (false == mgxActor->IsWire ( )))
//	add	= false;
							distance	= vtkMath::Distance2BetweenPoints (cameraPos, points->GetPoint (index));
							break;
					}	// switch (GetMode ( ))
					if (true == add)
						objects.insert (pair<double, vtkActor*>(distance, a));
				}	// if (0 != a)
				index++;
			}	// while (0 != (p = props->GetNextProp3D (csi)))

			if (0 != objects.size ( ))
			{
				vtkAssemblyPath*	path	= vtkAssemblyPath::New ( );
// Versions > 1.6.2 : tri selon la distance (plus proche au plus éloigné), puis,
// pour une distance donnée, selon l'ancienneté (entité la plus récente à la
// plus ancienne). MGXDDD-333
				RegisterPickedEntities (objects, _actors, *path);
/* Versions <= 1.6.2 : tri uniquement selon la distance
				for (multimap<double,vtkActor*>::iterator it = objects.begin();
				     objects.end ( ) != it; it++)
				{
VTKMgx3DActor*	mgxActor	= dynamic_cast<VTKMgx3DActor*>((*it).second);
					_actors.push_back ((*it).second);
					path->AddNode ((*it).second, 0);
				}	// for (multimap<double,vtkActor*>::iterator it = ...
*/
				SetPath (path);
				path->Delete ( );		path	= 0;
			}	// if (0 != objects.size ( ))
			else	// CP : VTK n'aime pas les vtkAssemblyPath vides de noeuds
				SetPath (NULL);

		}	// if (0 != camera)
	}	// if (0 != picker->Pick (x, y, z, renderer))
	
	// On laisse la possibilité aux classes dérivées de faire un traitement
	// spécialisé avant invocation des commandes :
	CompletePicking ( );

	// Invocation des commandes sur la fin du picking :
	InvokeEvent (vtkCommand::EndPickEvent, 0);

	return 0 == GetPath ( ) ? 0 : 1;
}	// VTKECMPicker::Pick


double* VTKECMPicker::GetSelectionPoint ( )
{
	AutoMutex	autoMutex (&GetMutex ( ));

	static double	pt [3]	= { 0., 0., 0.};
	vtkPicker*		picker	= GetPicker ( );

	if (0 != picker)
		return picker->GetSelectionPoint ( );
	else
	{
		cerr << __FILE__ << ' ' << __LINE__ << " VTKECMPicker::GetSelectionPoint. Absence de picker." << endl;
		return pt;
	}
}	// VTKECMPicker::GetSelectionPoint


void VTKECMPicker::GetPickPosition (double pt [3])
{
	AutoMutex	autoMutex (&GetMutex ( ));

	vtkPicker*	picker	= GetPicker ( );
	if (0 != picker)
		picker->GetPickPosition (pt);
	else
		cerr << __FILE__ << ' ' << __LINE__ << " VTKECMPicker::GetPickPosition. Absence de picker." << endl;
}	// VTKECMPicker::GetPickPosition


double* VTKECMPicker::GetPickPosition ( )
{
	AutoMutex	autoMutex (&GetMutex ( ));

	static double	pt [3]	= { 0., 0., 0.};
	vtkPicker*		picker	= GetPicker ( );

	if (0 != picker)
		return picker->GetPickPosition ( );
	else
	{
		cerr << __FILE__ << ' ' << __LINE__ << " VTKECMPicker::GetPickPosition. Absence de picker." << endl;
		return pt;
	}
}	// VTKECMPicker::GetPickPosition


void VTKECMPicker::GetSelectionPoint (double pt [3])
{
	AutoMutex	autoMutex (&GetMutex ( ));

	vtkPicker*	picker	= GetPicker ( );
	if (0 != picker)
		picker->GetSelectionPoint (pt);
	else
		cerr << __FILE__ << ' ' << __LINE__ << " VTKECMPicker::GetSelectionPoint. Absence de picker." << endl;
}	// VTKECMPicker::GetSelectionPoint


vtkProp3DCollection* VTKECMPicker::GetProp3Ds ( )
{
	AutoMutex	autoMutex (&GetMutex ( ));

	vtkPicker*	picker	= GetPicker ( );

	return 0 == picker ? 0 : picker->GetProp3Ds ( );
}	// VTKECMPicker::GetProp3Ds


vtkPoints* VTKECMPicker::GetPickedPositions ( )
{
	AutoMutex	autoMutex (&GetMutex ( ));

	vtkPicker*	picker	= GetPicker ( );

	return 0 == picker ? 0 : picker->GetPickedPositions ( );
}	// VTKECMPicker::GetPickedPositions


const vector<vtkActor*>& VTKECMPicker::GetPickedActors ( ) const
{
	return _actors;
}	// VTKECMPicker::GetPickedActors


void VTKECMPicker::RegisterPickedEntities (
			const multimap<double,vtkActor*>& objects,
			vector<vtkActor*>& actors, vtkAssemblyPath& path) const
{
	double	lastDistance	= -NumericServices::doubleMachMax ( );
	for (multimap<double,vtkActor*>::const_iterator it = objects.begin ( );
	     objects.end ( ) != it; it++)
	{
		if (fabs (lastDistance - (*it).first)
									<= NumericServices::doubleMachEpsilon ( ))
			continue;

		lastDistance	= (*it).first;
		pair<multimap<double,vtkActor*>::const_iterator,
		     multimap<double,vtkActor*>::const_iterator>	range	=
											objects.equal_range (lastDistance);
		multimap<unsigned long, vtkActor*>					objs;
		for (multimap<double,vtkActor*>::const_iterator ito = range.first;
		     range.second != ito; ito++)
		{
			VTKMgx3DActor*	actor =dynamic_cast<VTKMgx3DActor*>((*ito).second);
			Entity*			entity	= 0 == actor ? 0 : actor->GetEntity ( );
			unsigned long	id	= 0 == entity ?
							      0 : actor->GetEntity ( )->getUniqueId ( );
			objs.insert (pair<unsigned long, vtkActor*>(id, (*ito).second));
		}	// for (multimap<double,vtkActor*>::const_iterator ito = ...
		for (multimap<unsigned long, vtkActor*>::const_reverse_iterator iti =
				objs.rbegin ( ); iti != objs.rend ( ); iti++)
		{
			VTKMgx3DActor*	mgxActor	=
								dynamic_cast<VTKMgx3DActor*>((*iti).second);
			actors.push_back ((*iti).second);
			path.AddNode ((*iti).second, 0);
		}	// for (multimap<unsigned long, vtkActor*>::const_reverse_iterator..
	}	// for (multimap<double,vtkActor*>::const_iterator it = ...
}	// VTKECMPicker::RegisterPickedEntities


void VTKECMPicker::SetPickedActors (const vector<vtkActor*>& actors)
{
	AutoMutex	autoMutex (&GetMutex ( ));

	_actors	= actors;
	if (0 != _actors.size ( ))
	{
		vtkAssemblyPath*	path	= vtkAssemblyPath::New ( );
		for (vector<vtkActor*>::iterator it = _actors.begin ( );
		     _actors.end ( ) != it; it++)
			path->AddNode ((*it), 0);
		SetPath (path);
		path->Delete ( );		path	= 0;
	}	// if (0 != _actors.size ( ))
	else	// CP : VTK n'aime pas les vtkAssemblyPath vides de noeuds ...
		SetPath (NULL);
}	// VTKECMPicker::SetPickedActors


vtkPicker* VTKECMPicker::GetPicker ( )
{
	AutoMutex	autoMutex (&GetMutex ( ));

	switch (GetMode ( ))
	{
		case VTKECMPicker::BOUNDING_BOX	: return _picker;
		case VTKECMPicker::DISTANCE		: return _cellPicker;
		default								:
cerr << __FILE__ << ' ' << __LINE__ << " VTKECMPicker::GetPicker : mode non supporté (" << (unsigned long)GetMode ( ) << ")." << endl;
	}	// switch (GetMode ( ))

	return 0;
}	// VTKECMPicker::GetPicker


Mutex& VTKECMPicker::GetMutex ( ) const
{
	return _mutex;
}	// VTKECMPicker::GetMutex


void VTKECMPicker::CompletePicking ( )
{
}	// VTKECMPicker::CompletePicking


