#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKMgx3DPicker.h"
#include "QtVtkComponents/VTKMgx3DActor.h"
#include "QtVtkComponents/VTKConfiguration.h"

#include <TkUtil/MemoryError.h>

#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkLine.h>		// vtkLine::DistanceToLine
#include <vtkMath.h>
#include <vtkPoints.h>
#include <vtkProp3DCollection.h>

#include <assert.h>
#include <iostream>
#include <map>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;

namespace Mgx3D
{

namespace QtVtkComponents
{


VTKMgx3DPicker::VTKMgx3DPicker ( )
	: VTKECMPicker ( ), _entities ( )
{
}	// VTKMgx3DPicker::VTKMgx3DPicker


VTKMgx3DPicker::VTKMgx3DPicker (const VTKMgx3DPicker&)
	: VTKECMPicker ( ), _entities ( )
{
	assert (0 && "VTKMgx3DPicker copy constructor is not allowed.");
}	// VTKMgx3DPicker copy constructor


VTKMgx3DPicker& VTKMgx3DPicker::operator = (const VTKMgx3DPicker&)
{
	assert (0 && "VTKMgx3DPicker::operator = is not allowed.");
	return *this;
}	// VTKMgx3DPicker::operator =


VTKMgx3DPicker::~VTKMgx3DPicker ( )
{
}	// VTKMgx3DPicker::~VTKMgx3DPicker


void VTKMgx3DPicker::PrintSelf (ostream& os, vtkIndent indent)
{
	VTKECMPicker::PrintSelf (os, indent);
}	// VTKMgx3DPicker::PrintSelfA


VTKMgx3DPicker* VTKMgx3DPicker::New ( )
{
	VTKMgx3DPicker*	picker	= new VTKMgx3DPicker ( );
#ifndef VTK_5
	picker->InitializeObjectBase ( );
#endif	// VTK_5

	return picker;
}	// VTKMgx3DPicker::New


const vector<Entity*>& VTKMgx3DPicker::GetPickedEntities ( ) const
{
	return _entities;
}	// VTKMgx3DPicker::GetPickedEntities


void VTKMgx3DPicker::CompletePicking ( )
{
	AutoMutex	autoMutex (&GetMutex ( ));

	_entities.clear ( );

	// On refait le tri si nécessaire en privilégiant les acteurs de type VTKMgx3DActor :
	vector<vtkActor*>	actors	= GetPickedActors ( );
	vector<vtkActor*>	resorted, others;
	for (vector<vtkActor*>::const_iterator ita = actors.begin ( );
	     actors.end ( ) != ita; ita++)
	{
		VTKMgx3DActor*	actor	= dynamic_cast<VTKMgx3DActor*>(*ita);
		if (0 != actor)
			resorted.push_back (actor);
		else
			others.push_back (*ita);
	}	// for (vector<vtkActor*>::const_iterator ita ...

	// On enlève les doublons (ex : vue surfacique + vue filaire de la même entité) :
	vector<vtkActor*>	tmp;
	for (vector<vtkActor*>::const_iterator ita = resorted.begin ( );
	     resorted.end ( ) != ita; ita++)
	{
		VTKMgx3DActor*	actor	= dynamic_cast<VTKMgx3DActor*>(*ita);
		CHECK_NULL_PTR_ERROR (actor)
		const Entity*	entity	= actor->GetEntity ( );
		bool			founded	= false;
		for (vector<vtkActor*>::const_iterator itt	= tmp.begin ( );
		     tmp.end ( ) != itt; itt++)
		{
			VTKMgx3DActor*	actor2	= dynamic_cast<VTKMgx3DActor*>(*itt);
			CHECK_NULL_PTR_ERROR (actor2)
			const Entity*	entity2	= actor2->GetEntity ( );
			if (entity == entity2)
			{
				founded	= true;
				break;
			}
		}

		if (false == founded)
			tmp.push_back (*ita);
	}	// for (vector<vtkActor*>::const_iterator ita = resorted.begin ( );
	resorted	= tmp;
	actors		= resorted;
	SetPickedActors (resorted);

	if (0 != others.size ( ))
	{
		for (vector<vtkActor*>::iterator ito = others.begin ( );
		     others.end ( ) != ito; ito++)
			resorted.push_back (*ito);
		SetPickedActors (resorted);
		actors	= resorted;
	}	// if (0 != others.size ( ))

	for (vector<vtkActor*>::const_iterator it = actors.begin ( );
	     actors.end ( ) != it; it++)
	{
		VTKMgx3DActor*	actor	= dynamic_cast<VTKMgx3DActor*>(*it);
		if ((0 != actor) && (0 != actor->GetEntity ( )))
			_entities.push_back (actor->GetEntity ( ));
	}	// for (vector<vtkActor*>::const_iterator it = ...
}	// VTKMgx3DPicker::CompletePicking


}	// namespace QtVtkComponents

}	// namespace Mgx3D

