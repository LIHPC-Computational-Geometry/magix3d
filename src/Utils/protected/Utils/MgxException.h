/*----------------------------------------------------------------------------*/
/** \file    MgxException.h
 *  \author  F. LEDOUX
 *  \date    15/10/2010
 * 
 */
/*----------------------------------------------------------------------------*/
#ifndef _MGX_EXCEPTION_H_
#define _MGX_EXCEPTION_H_
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
// inclusion de fichiers d'en-tête de la STL
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
/** \class BuildingException
 *  \brief Type d'exception lancé si une création d'objets a échouée
 */
/*----------------------------------------------------------------------------*/
class BuildingException : public TkUtil::Exception
{

public :

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par défaut.
     *
     *  \param AWhat le texte associé à l'exception
     */
    /*------------------------------------------------------------------------*/
    BuildingException (const TkUtil::UTF8String& AWhat = "")
    : TkUtil::Exception("BuildingException "+AWhat)
    {}

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par copie.
     *
     *  \param AExc l'exception à copier
     */
    /*------------------------------------------------------------------------*/
    BuildingException (const BuildingException& AExc)
    : TkUtil::Exception(AExc.getFullMessage())
    {}
};
/*----------------------------------------------------------------------------*/
/** \class KillingException
 *  \brief Type d'exception lancé si une destruction d'objets a échouée
 */
/*----------------------------------------------------------------------------*/
class KillingException : public TkUtil::Exception
{

public :

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par défaut.
     *
     *  \param AWhat le texte associé à l'exception
     */
    /*------------------------------------------------------------------------*/
    KillingException (const TkUtil::UTF8String& AWhat = "")
    : TkUtil::Exception("KillingException "+AWhat)
    {}

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par copie.
     *
     *  \param AExc l'exception à copier
     */
    /*------------------------------------------------------------------------*/
    KillingException (const KillingException& AExc)
    : TkUtil::Exception(AExc.getFullMessage())
    {}
};
/*----------------------------------------------------------------------------*/
/** \class UndefinedUnitException
 *  \brief Type d'exception lancé si l'unité n'est pas définie et que cela est bloquant
 */
/*----------------------------------------------------------------------------*/
        class UndefinedUnitException : public TkUtil::Exception
        {

        public :

            /*------------------------------------------------------------------------*/
            /** \brief Constructeur par défaut.
             *
             *  \param AWhat le texte associé à l'exception
             */
            /*------------------------------------------------------------------------*/
            UndefinedUnitException (const TkUtil::UTF8String& AWhat = "")
                    : TkUtil::Exception("UndefinedUnitException "+AWhat)
            {}

            /*------------------------------------------------------------------------*/
            /** \brief Constructeur par copie.
             *
             *  \param AExc l'exception à copier
             */
            /*------------------------------------------------------------------------*/
            UndefinedUnitException (const UndefinedUnitException& AExc)
                    : TkUtil::Exception(AExc.getFullMessage())
            {}
        };
/*----------------------------------------------------------------------------*/
/** \class BadNormalException
 *  \brief Type d'exception lancé si la recherche d'une normale en un point ne peut se faire
 */
/*----------------------------------------------------------------------------*/
class BadNormalException : public TkUtil::Exception
{

public :

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par défaut.
     *
     *  \param AWhat le texte associé à l'exception
     */
    /*------------------------------------------------------------------------*/
	BadNormalException (const TkUtil::UTF8String& AWhat = "")
    : TkUtil::Exception("BadNormalException "+AWhat)
    {}

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par copie.
     *
     *  \param AExc l'exception à copier
     */
    /*------------------------------------------------------------------------*/
	BadNormalException (const BadNormalException& AExc)
    : TkUtil::Exception(AExc.getFullMessage())
    {}
};
/*----------------------------------------------------------------------------*/
/** \class IsDestroyedException
 *  \brief Type d'exception lancé si la recherche d'une entité retourne une entité détruite
 */
/*----------------------------------------------------------------------------*/
class IsDestroyedException : public TkUtil::Exception
{

public :

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par défaut.
     *
     *  \param AWhat le texte associé à l'exception
     */
    /*------------------------------------------------------------------------*/
	IsDestroyedException (const TkUtil::UTF8String& AWhat = "")
    : TkUtil::Exception("IsDestroyedException "+AWhat)
    {}

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par copie.
     *
     *  \param AExc l'exception à copier
     */
    /*------------------------------------------------------------------------*/
	IsDestroyedException (const IsDestroyedException& AExc)
    : TkUtil::Exception(AExc.getFullMessage())
    {}
};
/*----------------------------------------------------------------------------*/
/** \class OuterSurfaceException
 *  \brief Type d'exception lancé si la projection se fait en dehors d'une surface
 */
/*----------------------------------------------------------------------------*/
class OuterSurfaceException : public TkUtil::Exception
{

public :

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par défaut.
     *
     *  \param AWhat le texte associé à l'exception
     */
    /*------------------------------------------------------------------------*/
	OuterSurfaceException (const TkUtil::UTF8String& AWhat = "")
    : TkUtil::Exception("OuterSurfaceException "+AWhat)
    {}

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par copie.
     *
     *  \param AExc l'exception à copier
     */
    /*------------------------------------------------------------------------*/
	OuterSurfaceException (const OuterSurfaceException& AExc)
    : TkUtil::Exception(AExc.getFullMessage())
    {}
};
/*----------------------------------------------------------------------------*/
/** \class HalfCircleSurfaceException
 *  \brief Type d'exception lancé si la projection se fait sur une surface issue
 *  de la révolution d'un demi cercle
 */
/*----------------------------------------------------------------------------*/
class HalfCircleSurfaceException : public TkUtil::Exception
{

public :

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par défaut.
     *
     *  \param AWhat le texte associé à l'exception
     */
    /*------------------------------------------------------------------------*/
	HalfCircleSurfaceException (const TkUtil::UTF8String& AWhat = "")
    : TkUtil::Exception("HalfCircleSurfaceException "+AWhat)
    {}

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur par copie.
     *
     *  \param AExc l'exception à copier
     */
    /*------------------------------------------------------------------------*/
	HalfCircleSurfaceException (const HalfCircleSurfaceException& AExc)
    : TkUtil::Exception(AExc.getFullMessage())
    {}
};
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif	// _MGX_EXCEPTION_H_
/*----------------------------------------------------------------------------*/

