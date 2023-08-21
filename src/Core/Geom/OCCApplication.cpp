/*----------------------------------------------------------------------------*/
/*
 * OCCApplication.cpp
 *
 *  Created on: 15 févr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Geom/OCCApplication.h"
/*----------------------------------------------------------------------------*/
// fichiers d'en-tête d'Open Cascade
#include <TCollection_ExtendedString.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
void OCCApplication::
Formats(TColStd_SequenceOfExtendedString& theFormats)
{
  TCollection_ExtendedString formats("MDTV-Standard");

  // pour ajouter un second format
  // formats.Insert(1,"XmlOCAF");

  theFormats.Append(formats);
}
/*----------------------------------------------------------------------------*/
Standard_CString OCCApplication::ResourcesName()
{
  return "Standard";
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
