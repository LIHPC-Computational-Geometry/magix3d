/*----------------------------------------------------------------------------*/
/*
 * ImportSTEPImplementation.cpp
 *
 *  Created on: 15 févr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <map>
#include <set>
#include <algorithm>
/*----------------------------------------------------------------------------*/
#include "Geom/ImportSTEPImplementation.h"
#include "Utils/MgxException.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <STEPControl_Reader.hxx>
#include <Interface_Static.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ImportSTEPImplementation::
ImportSTEPImplementation(Internal::Context& c, Internal::InfoCommand* icmd,
		const std::string& n)
: GeomImport(c,icmd,n)
{
}
/*----------------------------------------------------------------------------*/
ImportSTEPImplementation::~ImportSTEPImplementation()
{}
/*----------------------------------------------------------------------------*/
void ImportSTEPImplementation::readFile()
{
    std::string suffix = m_filename;
    int suffix_start = m_filename.find_last_of(".");
    suffix.erase(0,suffix_start+1);
    if (suffix != "step" && suffix != "stp"  )
        throw TkUtil::Exception (TkUtil::UTF8String ("Mauvaise extension de fichier STEP (.stp ou .step)", TkUtil::Charset::UTF_8));

    // récupération du nom du fichier sans chemin ni extension
    int path_end = m_filename.find_last_of("/");
    std::string filename(m_filename, path_end+1, suffix_start-path_end-1);
    //std::cout<<"filename : "<<filename<<std::endl;
    unsigned long id=0;

    STEPControl_Reader aReader;
    TopoDS_Shape aShape;

    if (aReader.ReadFile((Standard_CString)m_filename.c_str()) != IFSelect_RetDone)
        throw TkUtil::Exception (TkUtil::UTF8String ("Impossible d'ouvrir ce fichier STEP", TkUtil::Charset::UTF_8));

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
            throw Utils::UndefinedUnitException(TkUtil::UTF8String ("Il est nécessaire de spécifier une unité avant d'ouvrir un fichier STEP", TkUtil::Charset::UTF_8));
    }	// switch (getLengthUnit ( ))

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
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
