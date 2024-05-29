/*----------------------------------------------------------------------------*/
/** \file SysCoordProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 4/6/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef UTIL_SYSCOORD_PROPERTY_H_
#define UTIL_SYSCOORD_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/Matrix33.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {


/*----------------------------------------------------------------------------*/
/** \class SysCoordProperty
 *  \brief Propriétés internes pour objet repère
 */
/*----------------------------------------------------------------------------*/
class SysCoordProperty{

public:

	/// Constructeur pour repère standard
	SysCoordProperty() {}

	/// Création d'un clone
	SysCoordProperty* clone() {
		SysCoordProperty* prop = new SysCoordProperty();
		prop->m_center = m_center;
		prop->m_transf = m_transf;
		return prop;
	}

	Utils::Math::Point getCenter() const { return m_center;}
	void setCenter(Utils::Math::Point pt) {m_center = pt;}

	Utils::Math::Matrix33 getTransf() const {return m_transf;}

	void setTransf(const Utils::Math::Matrix33 transf) {m_transf = transf;}

	double getTransfCoord(int ligne, int colonne)
	{
		return m_transf.getCoord(ligne, colonne);
	}

	/// initialise le repère avec 3 points
	void initProperties(const Utils::Math::Point& pCentre,
			const Utils::Math::Point& pX,
			const Utils::Math::Point& pY);

private :

    Utils::Math::Point m_center;

    // pour passer du repère par défaut à ce repère local
    Utils::Math::Matrix33 m_transf;

};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* UTIL_SYSCOORD_PROPERTY_H_ */
/*----------------------------------------------------------------------------*/

