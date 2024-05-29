/*----------------------------------------------------------------------------*/
/*
 * \file TypeDedicatedNameManager.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17 janv. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Utils/TypeDedicatedNameManager.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/NumericConversions.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
TypeDedicatedNameManager::
TypeDedicatedNameManager(std::string base_name)
: m_base_name(base_name), m_id(0), m_memorized_id(0)
{
}
/*----------------------------------------------------------------------------*/
std::string TypeDedicatedNameManager::
getName()
{
    TkUtil::UTF8String   name (TkUtil::Charset::UTF_8);
    name << m_base_name << TkUtil::setw (4) << m_id++;
    return name;
}
/*----------------------------------------------------------------------------*/
unsigned long TypeDedicatedNameManager::
getId()
{
    m_id++;
    return m_id;
}
/*----------------------------------------------------------------------------*/
unsigned long TypeDedicatedNameManager::
getLastId()
{
    return m_id;
}
/*----------------------------------------------------------------------------*/
void TypeDedicatedNameManager::
setLastId(unsigned long id)
{
    m_id = id;
}
/*----------------------------------------------------------------------------*/
void TypeDedicatedNameManager::
memorizeLastId()
{
    m_memorized_id =  m_id;
}
/*----------------------------------------------------------------------------*/
std::string TypeDedicatedNameManager::
renameWithShiftingId(const std::string& name) const
{
    // cas sans changement
    if (m_memorized_id == 0)
        return name;

    // on doit récupérer l'id dans le nom
    std::string sub_str(name, m_base_name.size());
    unsigned long old_id = TkUtil::NumericConversions::strToULong(sub_str);

    unsigned long new_id = old_id + m_memorized_id;

    TkUtil::UTF8String   new_name (TkUtil::Charset::UTF_8);
    new_name << m_base_name << TkUtil::setw (4) << new_id;
    return new_name;
}
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
