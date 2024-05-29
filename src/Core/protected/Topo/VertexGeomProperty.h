/*----------------------------------------------------------------------------*/
/** \file VertexGeomProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/12/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef VERTEX_GEOM_PROPERTY_H_
#define VERTEX_GEOM_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class Edge;
/*----------------------------------------------------------------------------*/
/**
 * \class VertexGeomProperty
 * 
 * Rassemble les propriétés géométriques communes aux sommets topologiques.
 */
 
 class VertexGeomProperty {
 public:
     VertexGeomProperty()
     : m_coord(0,0,0)
     {}

     VertexGeomProperty(const Utils::Math::Point& pt)
     : m_coord(pt)
     {}

     ~VertexGeomProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     VertexGeomProperty* clone() {
         VertexGeomProperty* prop = new VertexGeomProperty();
         prop->m_coord = m_coord;
         return prop;
     }

     /*------------------------------------------------------------------------*/
     /// Accesseur sur les coordonnées
     inline Utils::Math::Point getCoord() const {return m_coord;}

     /// Modificateur sur les coordonnées
     inline void setCoord(const Utils::Math::Point & pt) {m_coord = pt;}

 private:
     /// Coordonnées du sommet topologique
     Utils::Math::Point m_coord;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* VERTEX_GEOM_PROPERTY_H_ */
