/*----------------------------------------------------------------------------*/
/** \file TopoProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 23 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPO_PROPERTY_H_
#define TOPO_PROPERTY_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class GeomEntity;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
class TopoEntity;
/*----------------------------------------------------------------------------*/
/**
 * \class TopoProperty
 * 
 * Rassemble les propriétés communes aux entités topologiques (lien sur la géométrie).
 */
 
 class TopoProperty {
 public:
     TopoProperty()
     : m_geom_association(0)
     {}

     ~TopoProperty()
     {}

     /*------------------------------------------------------------------------*/
     /** Création d'un clone, on copie toutes les informations */
     TopoProperty* clone();

     /*------------------------------------------------------------------------*/
     /** Affecte un pointeur vers une entité géométrique */
     inline void setGeomAssociation(Geom::GeomEntity* ge) {m_geom_association = ge;}

     /** Retourne un pointeur vers une potentielle association avec la géométrie */
     inline Geom::GeomEntity* getGeomAssociation() const {return m_geom_association;}

     /*------------------------------------------------------------------------*/
     /// pour l'affichage d'informations
     friend std::ostream & operator << (std::ostream & o, const TopoProperty& tp);

 private:

     /// Eventuellement un lien sur un élément de la géométrie (sommet, courbe ou surface)
     Geom::GeomEntity* m_geom_association;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPO_PROPERTY_H_ */
