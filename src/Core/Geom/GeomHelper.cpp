/*----------------------------------------------------------------------------*/
/*
 * \file GeomHelper.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14 nov. 2014
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/GeomHelper.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
/*----------------------------------------------------------------------------*/
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
Curve* GeomHelper::getCommonCurve(std::vector<Curve*>& curves1,
		std::vector<Curve*>& curves2)
{
	Curve* curve = 0;
	std::set<Curve*> filtre_curves;

	for (std::vector<Curve*>::iterator iter = curves1.begin();
			iter != curves1.end(); ++iter){
		filtre_curves.insert(*iter);
	}

	for (std::vector<Curve*>::iterator iter = curves2.begin();
				iter != curves2.end(); ++iter){
		if (filtre_curves.find(*iter) != filtre_curves.end())
			if (curve){
				// on sélectionne la plus courte
				if (curve->getArea()>(*iter)->getArea())
					curve = *iter;
			}
			else
				curve = *iter;
	}

	return curve;
}
/*----------------------------------------------------------------------------*/
Surface* GeomHelper::getCommonSurface(std::vector<Surface*>& surf1,
		std::vector<Surface*>& surf2)
{
	//std::cout<<"getCommonSurface ..."<<std::endl;
	Surface* surface = 0;
	std::set<Surface*> filtre_surfaces;

	for (std::vector<Surface*>::iterator iter = surf1.begin();
			iter != surf1.end(); ++iter){
		filtre_surfaces.insert(*iter);
		//std::cout<<" surf1 : "<<(*iter)->getName()<<std::endl;
	}

	for (std::vector<Surface*>::iterator iter = surf2.begin();
				iter != surf2.end(); ++iter){
		if (filtre_surfaces.find(*iter) != filtre_surfaces.end())
			if (surface){
				// on sélectionne la plus petite
				if (surface->getArea()>(*iter)->getArea())
					surface = *iter;
			}
			else
				surface = *iter;
		//std::cout<<" surf2 : "<<(*iter)->getName()<<std::endl;
	}

	//std::cout<<"  return "<<(surface?surface->getName():"rien")<<std::endl;
	return surface;
}
/*----------------------------------------------------------------------------*/
Vertex* GeomHelper::getCommonVertex(Curve* crv1, Curve* crv2)
{
	std::vector<Vertex*> vertices1;
	std::vector<Vertex*> vertices2;
	crv1->get(vertices1);
	crv2->get(vertices2);

	for (uint i=0; i<vertices1.size(); i++)
		for (uint j=0; j<vertices2.size(); j++)
			if (vertices1[i] == vertices2[j])
				return vertices1[i];

	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur, pas de sommet commun aux deux courbes", TkUtil::Charset::UTF_8));
	return 0;
}

/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
