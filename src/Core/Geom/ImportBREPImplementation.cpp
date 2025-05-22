/*----------------------------------------------------------------------------*/
#include "Geom/ImportBREPImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
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
    // check file extension
    std::string suffix = m_filename;
    int suffix_start = m_filename.find_last_of(".");
    suffix.erase(0,suffix_start+1);
    if (suffix != "brep")
        throw TkUtil::Exception (TkUtil::UTF8String ("Mauvaise extension de fichier BREP (.brep)", TkUtil::Charset::UTF_8));

    // read file
    std::ifstream file(m_filename);
    if (!file.is_open()) {
        throw TkUtil::Exception (TkUtil::UTF8String ("Impossible d'ouvrir ce fichier BREP", TkUtil::Charset::UTF_8));
    }
    
    BRep_Builder builder;
    TopoDS_Shape shape;
    BRepTools::Read(shape, file, builder);

    m_importedShapes.resize(1);
    m_importedShapes[0] = shape;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
