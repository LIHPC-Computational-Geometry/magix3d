/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/** \file CommandImportMDL.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/5/2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Geom/CommandImportMDL.h"
#include "Geom/GeomManager.h"
#include "Internal/ImportMDLImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

/*----------------------------------------------------------------------------*/
CommandImportMDL::
CommandImportMDL(Internal::Context& c, Internal::ImportMDLImplementation* impl)
: CommandCreateGeom(c, "Import Mdl (géométrie)")
, m_impl(impl)
{
}
/*----------------------------------------------------------------------------*/
CommandImportMDL::~CommandImportMDL()
{
}
/*----------------------------------------------------------------------------*/
void CommandImportMDL::
internalExecute()
{
    // lecture du Mdl et création des entités
    m_impl->performGeom(&getInfoCommand());

    // on copie le vecteur des nouvelles entités géométriques
    m_createdEntities = m_impl->getNewGeomEntities();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/
