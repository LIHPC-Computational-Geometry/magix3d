/*----------------------------------------------------------------------------*/
#include "Geom/ImportIGESImplementation.h"
#include "Geom/Vertex.h"
#include "Utils/MgxException.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Controller.hxx>
#include <Interface_Static.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ImportIGESImplementation::
ImportIGESImplementation(Internal::Context& c, Internal::InfoCommand* icmd, const std::string& n)
: GeomImport(c,icmd,n)
{}
/*----------------------------------------------------------------------------*/
ImportIGESImplementation::~ImportIGESImplementation()
{}
/*----------------------------------------------------------------------------*/
void ImportIGESImplementation::readFile()
{
        /* Valeur par défaut de 1 changée en 0 pour éviter la création
         * d'une multitude de courbes pour un modèle test
         * 0 -> continuité C0 entre les éléments de la courbe
         * 1 -> si la continuité n'est pas C1, alors la courbe est coupée
         * 
         * ATTENTION à partir de OCC 7.8.1, doit être fait avant instanciation du reader
         */
        IGESControl_Controller::Init();
        Interface_Static::SetIVal("read.iges.bspline.continuity",0);

        switch (m_context.getLengthUnit ( ))
        {
            case Utils::Unit::meter		:
                Interface_Static::SetCVal ("xstep.cascade.unit", "M");
                break;
            case Utils::Unit::centimeter	:
                Interface_Static::SetCVal ("xstep.cascade.unit", "CM");
                break;
            case Utils::Unit::milimeter	:
                Interface_Static::SetCVal ("xstep.cascade.unit", "MM");
                break;
            case Utils::Unit::micron		:
                Interface_Static::SetCVal ("xstep.cascade.unit", "UM");
                break;
            default					:
                // on n'accepte plus de ne pas avoir d'unité de spécifiée
                throw Utils::UndefinedUnitException(TkUtil::UTF8String ("Il est nécessaire de spécifier une unité avant d'ouvrir un fichier IGES", TkUtil::Charset::UTF_8));
        }	// switch (getLengthUnit ( ))
        Interface_Static::SetIVal("read.scale.unit", 1); // activer conversion

        std::string suffix = m_filename;
        int suffix_start = m_filename.find_last_of(".");
        suffix.erase(0,suffix_start+1);
        if (suffix != "igs" && suffix != "iges"  )
            throw TkUtil::Exception (TkUtil::UTF8String ("Mauvaise extension de fichier IGES (.igs ou .iges)", TkUtil::Charset::UTF_8));

        // récupération du nom du fichier sans chemin ni extension
        int path_end = m_filename.find_last_of("/");

        IGESControl_Reader aReader;

        if (aReader.ReadFile((Standard_CString)m_filename.c_str()) != IFSelect_RetDone)
            throw TkUtil::Exception (TkUtil::UTF8String ("Impossible d'ouvrir ce fichier IGES", TkUtil::Charset::UTF_8));

        // Root transfers
        Standard_Integer nbr = aReader.NbRootsForTransfer();
        aReader.TransferRoots();

        // Collecting resulting entities
        Standard_Integer nbs = aReader.NbShapes();
        if (nbs == 0)
            throw TkUtil::Exception (TkUtil::UTF8String ("No shapes found in file ", TkUtil::Charset::UTF_8));

        m_importedShapes.resize(nbs);
        for (Standard_Integer i=1; i<=nbs; i++) {
            m_importedShapes[i-1]= aReader.Shape(i);
        }
}
//*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
