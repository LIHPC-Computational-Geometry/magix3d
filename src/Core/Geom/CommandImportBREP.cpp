/*----------------------------------------------------------------------------*/
/*
 * CommandImportBREP.cpp
 *
 *  Created on: 22 aout 2024
 *      Author: lelandaisb
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandImportBREP.h"
#include "Geom/ImportBREPImplementation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandImportBREP::
CommandImportBREP(Internal::Context& c, const std::string& n, const bool testVolumicProperties, const bool splitCompoundCurves)
: CommandCreateGeom(c, "Import BREP"), m_filename(n)
{
    m_impl = new ImportBREPImplementation(c, &getInfoCommand(), m_filename);
    m_impl->setSplitCompoundCurves(splitCompoundCurves);
    m_impl->setTestVolumicProperties(testVolumicProperties);
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
