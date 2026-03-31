/*----------------------------------------------------------------------------*/
#include "Geom/CommandImportBREP.h"
#include "Geom/ImportBREPImplementationForPairing.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandImportBREP::
CommandImportBREP(Internal::Context& c, const std::string& n,
    const bool testVolumicProperties, const bool createGroups)
: CommandCreateGeom(c, "Import BREP"), m_filename(n)
{
    m_impl = new ImportBREPImplementationForPairing(c, &getInfoCommand(), m_filename);
}
/*----------------------------------------------------------------------------*/
CommandImportBREP::~CommandImportBREP()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandImportBREP::
internalExecute()
{
    m_impl->perform(m_createdEntities);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
