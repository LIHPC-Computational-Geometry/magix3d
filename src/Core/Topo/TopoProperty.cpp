/*----------------------------------------------------------------------------*/
/*
 * \file TopoProperty.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 20 déc. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Topo/TopoProperty.h"
#include "Geom/GeomEntity.h"
#include "sys/types.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
TopoProperty* TopoProperty::clone() {
    TopoProperty* tp = new TopoProperty();

#ifdef _DEBUG
//    std::cout<<" TopoProperty::clone() pour "<<*this<<std::endl;
#endif

    tp->m_geom_association = m_geom_association;
//    tp->m_groups_name.insert(tp->m_groups_name.end(), m_groups_name.begin(), m_groups_name.end());

    return tp;
}
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o, const TopoProperty& tp)
{
    if (tp.m_geom_association)
        o << "projeté sur "<< tp.m_geom_association->getName();
    else
        o << "sans association géométrique";
//    if (tp.m_groups_name.size()){
//        o << ", GroupesNames :";
//        for (uint i=0; i<tp.m_groups_name.size(); i++)
//            o << " " << tp.m_groups_name[i];
//    }
//    else
//        o << ", sans GroupesNames";

    return o;
}
/*----------------------------------------------------------------------------*/
 } // end namespace Topo
 /*----------------------------------------------------------------------------*/
 } // end namespace Mgx3D
