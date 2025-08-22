/*----------------------------------------------------------------------------*/
#include "Geom/CommandImportCATIA.h"
#include "Geom/GeomManager.h"
#include "Geom/ImportCATIAImplementation.h"
#include "Geom/Curve.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

class GeomVertex;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
CommandImportCATIA::
CommandImportCATIA(Internal::Context& c, const std::string& n,  const bool testVolumicProperties)
: CommandCreateGeom(c, "Import CATIA"), m_filename(n)
{
    m_impl = new ImportCATIAImplementation(c, &getInfoCommand(), m_filename);
    m_impl->setTestVolumicProperties(testVolumicProperties);
}
/*----------------------------------------------------------------------------*/
CommandImportCATIA::~CommandImportCATIA()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandImportCATIA::
internalExecute()
{
//	throw TkUtil::Exception("Import CATIA déconnecté du fait d'une évolution de OCC");

    m_impl->perform(m_createdEntities);

    for(unsigned int i=0; i<m_createdEntities.size();i++)
        m_group_helper.addToGroup(m_group_name, m_createdEntities[i]);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
