/*----------------------------------------------------------------------------*/
/*
 * \file Handle.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 19/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Utils/Handle.h"
//#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
RefCountObject::RefCountObject():m_nbRefs(0)
{}
/*----------------------------------------------------------------------------*/
RefCountObject::~RefCountObject()
{
  killBehaviour();
}
/*----------------------------------------------------------------------------*/
void RefCountObject::killBehaviour(){
//    if(getNbRefs()!=0)
//        throw TkUtil::Exception("Still references!");
}
/*----------------------------------------------------------------------------*/
void RefCountObject::increaseNbRefs()
{
    m_nbRefs+=1;
}
/*----------------------------------------------------------------------------*/
void RefCountObject::decreaseNbRefs()
{
    if(m_nbRefs!=0)
        m_nbRefs-=1;

    if(m_nbRefs==0)
        noMoreHandle();
}
/*----------------------------------------------------------------------------*/
int RefCountObject::getNbRefs() const
{
    return m_nbRefs;
}
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
