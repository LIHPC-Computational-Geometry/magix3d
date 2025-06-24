//
// Created by calderans on 3/17/25.
//

#include "Topo/CommandExportBlocks.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandExportBlocks::
CommandExportBlocks(Internal::Context &context, const std::string &fileName, const bool& with_geom)
: Internal::CommandInternal(context, "Export des Blocs"),m_filename(fileName), m_impl(context, fileName, with_geom)
{}
/*----------------------------------------------------------------------------*/
CommandExportBlocks::~CommandExportBlocks()
{}
/*----------------------------------------------------------------------------*/
void CommandExportBlocks::
internalExecute()
{

    std::string ext;
    int i = m_filename.rfind('.', m_filename.length());
    if (i != std::string::npos) {
        ext = m_filename.substr(i+1, m_filename.length() - i);
    }

    if(ext != "blk"){
        TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message<<"Erreur dans l'extension du fichier, merci d'utiliser \".blk\"";
        throw TkUtil::Exception (message);

    }
    // écriture du STEP
    m_impl.perform(&getInfoCommand());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/