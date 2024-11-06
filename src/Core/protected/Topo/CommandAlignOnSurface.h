//
// Created by calderans on 10/29/24.
//
/*----------------------------------------------------------------------------*/
#ifndef COMMANDALIGNONSURFACE_H_
#define COMMANDALIGNONSURFACE_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class Surface;
class Vertex;
/*----------------------------------------------------------------------------*/
class CommandAlignOnSurface: public CommandEditTopo{

public:
    CommandAlignOnSurface(Internal::Context& c, Geom::GeomEntity* surface, Vertex* v, const Point pnt1, const Point pnt2);

    virtual ~CommandAlignOnSurface();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    void getPreviewRepresentation(Utils::DisplayRepresentation &dr);


private:

    Geom::GeomEntity* m_surface;
    Vertex* m_vertex;

    Utils::Math::Point m_pnt1;
    Utils::Math::Point m_pnt2;

    Vector m_dir;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDALIGNONSURFACE_H_ */
