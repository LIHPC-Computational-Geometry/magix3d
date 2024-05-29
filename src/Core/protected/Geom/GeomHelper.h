/*----------------------------------------------------------------------------*/
/*
 * \file GeomHelper.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14 nov. 2014
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMHELPER_H_
#define GEOMHELPER_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Entity.h"
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class GeomEntity;
class Volume;
class Surface;
class Curve;
class Vertex;
/*----------------------------------------------------------------------------*/
/**
 * \brief       Services utilitaires concernant les entités géométriques
 */
class GeomHelper
{
public:
	/// recherche de la courbe commune entre les deux groupes de courbes
	static Curve* getCommonCurve(std::vector<Curve*>& curves1, std::vector<Curve*>& curves2);

    /// recherche de la surface commune entre les deux groupes de surfaces
	static Surface* getCommonSurface(std::vector<Surface*>& surf1, std::vector<Surface*>& surf2);

	/// recherche du sommet commun aux deux courbes, lève une exception si pas trouvé
	static Vertex* getCommonVertex(Curve* crv1, Curve* crv2);

private :

    /**
     * Constructeurs et destructeurs. Opérations interdites.
     */
    //@{
    GeomHelper ( );
    GeomHelper (const GeomHelper&);
    GeomHelper& operator = (const GeomHelper&);
    ~GeomHelper ( );
    //@}

};  // class GeomHelper
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMHELPER_H_ */
