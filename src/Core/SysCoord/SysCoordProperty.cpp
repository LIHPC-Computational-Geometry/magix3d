/*----------------------------------------------------------------------------*/
/*
 * \file SysCoordProperty.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6 juin 2018
 */
/*----------------------------------------------------------------------------*/
#include "SysCoord/SysCoordProperty.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
// OCC
#include "gp_Dir.hxx"
#include "gp_Trsf.hxx"
#include "gp_Ax3.hxx"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
void SysCoordProperty::initProperties(const Utils::Math::Point& pCentre,
	const Utils::Math::Point& pX,
	const Utils::Math::Point& pY)
{
	setCenter(pCentre);

	// création des 3 axes normalisés (et orthogonaux)

	Utils::Math::Vector vx (pX-pCentre);
	Utils::Math::Vector vy (pY-pCentre);
	Utils::Math::Vector vz = vx*vy;
	vy = vz*vx;
	vx / vx.norme();
	vy / vy.norme();
	vz / vz.norme();

	gp_Pnt P(pCentre.getX(),pCentre.getY(),pCentre.getZ());
	gp_Dir N(vz.getX(),vz.getY(),vz.getZ());
	gp_Dir Vx(vx.getX(),vx.getY(),vx.getZ());
	gp_Trsf tr;
	tr.SetTransformation(gp_Ax3(P,N,Vx));

	for (uint i=0; i<3; i++)
		for (uint j=0;j<3; j++)
			m_transf.setCoord(i,j, tr.Value(i+1, j+1));
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
