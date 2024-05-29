/*----------------------------------------------------------------------------*/
/** \file    OceaneApplication.h
 *  \author  F. LEDOUX
 *  \date    15/10/2004
 */
/*----------------------------------------------------------------------------*/
#ifndef OCCAPPLICATION_H_
#define OCCAPPLICATION_H_
/*----------------------------------------------------------------------------*/
// fichiers d'en-tete d'Open Cascade
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class OCCApplication
 *  \brief Classe dérivant de la classe TDocStd_Application, elle est nécessaire
 *         dans l'utilisation d'OCAF. C'est cette classe qui sert de contexte
 *         dans lequel sera ouvert le document contenant les entités
 *         géométriques.
 *
 *         En ce qui concerne les règles de codages, les méthodes surchargées
 *         ici ne respectent pas le fait que les méthodes doivent commencer par
 *         une lettre minuscule.
 */
/*----------------------------------------------------------------------------*/
class OCCApplication: public TDocStd_Application {

//public:
//    OCCApplication(){;}
    /*------------------------------------------------------------------------*/
    /** \brief  Initialise les formats acceptés par l'application OCAF.
     */
    /*------------------------------------------------------------------------*/
    void Formats(TColStd_SequenceOfExtendedString& theFormats);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur au nom des ressources.
     *
     *  \return Le nom des ressources
     */
    /*------------------------------------------------------------------------*/
    Standard_CString ResourcesName();
};
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
#endif /* OCCAPPLICATION_H_ */
/*----------------------------------------------------------------------------*/

