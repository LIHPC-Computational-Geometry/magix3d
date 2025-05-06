/*----------------------------------------------------------------------------*/
#include "Geom/ImportSTLImplementation.h"
#include "Geom/Vertex.h"
#include "Group/Group0D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <StlAPI.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ImportSTLImplementation::
ImportSTLImplementation(Internal::Context& c, Internal::InfoCommand* icmd, const std::string& n)
: GeomImport(c,icmd,n)
{}
/*----------------------------------------------------------------------------*/
ImportSTLImplementation::~ImportSTLImplementation()
{}
/*----------------------------------------------------------------------------*/
void ImportSTLImplementation::readFile()
{
	std::string suffix = m_filename;
	int suffix_start = m_filename.find_last_of(".");
	suffix.erase(0,suffix_start+1);
	if (suffix != "stl" && suffix != "STL"  )
		throw TkUtil::Exception (TkUtil::UTF8String ("Mauvaise extension de fichier STL (.stl ou .STL)", TkUtil::Charset::UTF_8));

	// création d'un shape: un volume fermé par des surfaces triangulaires
	TopoDS_Shape aShape;
	StlAPI::Read(aShape, m_filename.c_str());
	m_importedShapes.resize(1);
	m_importedShapes[0] = aShape;

	// pas une shape, mais un maillage...
//	OSD_Path aPath(m_filename.c_str());
//	Handle(StlMesh_Mesh) stlMesh = RWStl::ReadFile(aPath);
//	m_importedShapes.resize(1);
//	m_importedShapes[0] = stlMesh;

}
//*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
