/*----------------------------------------------------------------------------*/
/** \file CommandImportTopoMDL.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21/5/2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Topo/CommandImportTopoMDL.h"
#include "Topo/TopoManager.h"
#include "Internal/ImportMDLImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {

/*----------------------------------------------------------------------------*/
CommandImportTopoMDL::
CommandImportTopoMDL(Internal::Context& c, Internal::ImportMDLImplementation* impl)
: CommandCreateTopo(c, "Import (Topo) Mdl")
, m_impl(impl)
{
//    m_impl->setInfoCommand(&getInfoCommand());
}
/*----------------------------------------------------------------------------*/
CommandImportTopoMDL::~CommandImportTopoMDL()
{
}
/*----------------------------------------------------------------------------*/
void CommandImportTopoMDL::
internalExecute()
{
#ifdef USE_MDLPARSER
    // lecture du Mdl et création des entités
    m_impl->performTopo(&getInfoCommand());

    // NB: il n'y a pas de nouveaux blocs, seulement des Face topologiques ...

    //std::vector<TopoEntity*> & res = m_impl->getNewTopoEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // suppression des parties internes de type save... qui n'ont pas lieu d'être
    // puisque l'on est en cours de création
    saveInternalsStats();
    deleteInternalsStats();
#else   // USE_MDLPARSER
    throw TkUtil::Exception (TkUtil::UTF8String ("ImportTopoMDL non actif, USE_MDLPARSER à activer", TkUtil::Charset::UTF_8));
#endif  // USE_MDLPARSER
}
/*----------------------------------------------------------------------------*/
void CommandImportTopoMDL::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	MGX_FORBIDDEN("getPreviewRepresentation non prévu");
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
