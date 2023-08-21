/*----------------------------------------------------------------------------*/
/*
 * ImportIGESImplementation.cpp
 *
 *  Created on: 21 juin 2013
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
#include "Geom/ImportIGESImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCGeomRepresentation.h"

#include "Group/GroupManager.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"

#include "Internal/InfoCommand.h"

#include "Utils/MgxException.h"

#include "GMDS/Utils/Timer.h"
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <IGESControl_Reader.hxx>
#include <Interface_Static.hxx>
#include <Utils/MgxException.h>
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

//        // the precision wil be given by me
//        Interface_Static::SetIVal("read.precision.mode",1);
//
//        Standard_Real precision_value = Interface_Static::RVal("read.precision.val");
//        std::cerr<<"initial precision: "<<precision_value<<std::endl;
//        Interface_Static::SetRVal("read.precision.val",1e-1);
//        precision_value = Interface_Static::RVal("read.precision.val");
//        std::cerr<<"new precision    : "<<precision_value<<std::endl;

        /* Valeur par défaut de 1 changée en 0 pour éviter la création
         * d'une multitude de courbes pour un modèle test
         * 0 -> continuité C0 entre les éléments de la courbe
         * 1 -> si la continuité n'est pas C1, alors la courbe est coupée
         */
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
