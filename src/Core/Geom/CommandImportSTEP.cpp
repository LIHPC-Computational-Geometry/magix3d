/*----------------------------------------------------------------------------*/
/** \file CommandImportSTEP.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 04/02/2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandImportSTEP.h"
#include "Geom/GeomManager.h"
#include "Geom/ImportSTEPImplementation.h"
#include "Geom/Curve.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

class GeomVertex;
//class Curve;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
CommandImportSTEP::
CommandImportSTEP(Internal::Context& c, const std::string& n, const bool testVolumicProperties, const bool splitCompoundCurves)
: CommandCreateGeom(c, "Import STEP"), m_filename(n)
{
    m_impl = new ImportSTEPImplementation(c, &getInfoCommand(), m_filename);
    m_impl->setSplitCompoundCurves(splitCompoundCurves);
    m_impl->setTestVolumicProperties(testVolumicProperties);
}
/*----------------------------------------------------------------------------*/
CommandImportSTEP::~CommandImportSTEP()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandImportSTEP::
internalExecute()
{
    m_impl->perform(m_createdEntities);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
