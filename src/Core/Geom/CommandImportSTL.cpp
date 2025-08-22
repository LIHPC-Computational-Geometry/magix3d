/*----------------------------------------------------------------------------*/
#include "Geom/CommandImportSTL.h"
#include "Geom/GeomManager.h"
#include "Geom/ImportSTLImplementation.h"
#include "Geom/Curve.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

class GeomVertex;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
CommandImportSTL::
CommandImportSTL(Internal::Context& c, const std::string& n)
: CommandCreateGeom(c, "Import STL"), m_filename(n)
{
    m_impl = new ImportSTLImplementation(c, &getInfoCommand(), m_filename);
}
/*----------------------------------------------------------------------------*/
CommandImportSTL::~CommandImportSTL()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandImportSTL::
internalExecute()
{
    m_impl->perform(m_createdEntities);
#ifdef _DEBUG
    std::cout<<"NB CREATED = "<<m_createdEntities.size()<<std::endl;
#endif
    //stockage dans le manager géométrique des entités créés (donc nouvelles
    //uniquement)
    store(m_createdEntities);

    //gestion de l'appartenance aux groupes
    for(unsigned int i=0;i<m_createdEntities.size();i++)
        m_group_helper.addToGroup(m_group_name, m_createdEntities[i]);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
