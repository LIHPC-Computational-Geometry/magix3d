/*----------------------------------------------------------------------------*/
#include "Geom/ImportBREPImplementation.h"
#include "Geom/OCCHelper.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ImportBREPImplementation::
ImportBREPImplementation(Internal::Context& c, Internal::InfoCommand* icmd,
		const std::string& n)
: GeomImport(c,icmd,n)
{
}
/*----------------------------------------------------------------------------*/
ImportBREPImplementation::~ImportBREPImplementation()
{}
/*----------------------------------------------------------------------------*/
void ImportBREPImplementation::readFile()
{
    m_importedShapes.resize(1);
    m_importedShapes[0] = OCCHelper::readBrepFile(m_filename);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
