/*----------------------------------------------------------------------------*/
/** \file SysCoordManagerIfc.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18/05/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef SYSCOORD_MANAGER_IFC_H_
#define SYSCOORD_MANAGER_IFC_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResultIfc.h"
#include "Utils/Constants.h"
#include "Utils/SwigCompletion.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
namespace Math {
class Point;
class Rotation;
class Vector;
}
}
using namespace Mgx3D::Utils::Math;

/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
class SysCoord;
/*----------------------------------------------------------------------------*/
/**
 * \class SysCoordManagerIfc
 * \brief Interface de gestionnaire des repères
 *
 * Pour une documentation plus complète, voir SysCoordManager
 */
class SysCoordManagerIfc: public Internal::CommandCreator
{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param		Nom unique de l'instance (utile en environnement distribué).
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
    SysCoordManagerIfc(const std::string& name, Internal::ContextIfc* c);

	/**
	 * \brief	Destructeur. RAS.
	 */
	virtual ~SysCoordManagerIfc ( );

	/*------------------------------------------------------------------------*/
#ifndef SWIG
	/** Réinitialisation     */
	virtual void clear();
#endif
   /*------------------------------------------------------------------------*/
	/** \brief Création d'un repère centré
	 */
	virtual Mgx3D::Internal::M3DCommandResultIfc*
	   newSysCoord(std::string groupName);

	/*------------------------------------------------------------------------*/
	/** \brief Création d'un repère à partir d'un centre (simple translation)
	 */
	virtual Mgx3D::Internal::M3DCommandResultIfc*
	newSysCoord(const Point& p, std::string groupName);

	/*------------------------------------------------------------------------*/
	/** \brief Création d'un repère à partir d'un centre et de 2 points pour donner les directions
	 *
	 * Le repère est ajusté pour être orthonormé
	 */
	virtual Mgx3D::Internal::M3DCommandResultIfc*
	   newSysCoord(const Point& pCentre, const Point& pX, const Point& pY, std::string groupName);

#ifndef SWIG
	/// retourne le repère à partir de son nom
	virtual SysCoord* getSysCoord (const std::string& name, const bool exceptionIfNotFound) const;
#endif


    /*------------------------------------------------------------------------*/
    /** \brief translation d'un repère (identifiée par un nom unique pour python)
     *  suivant le vecteur de translation défini par dp
     *
     *  \param name nom de l'entité repère
     *  \param dp le vecteur de translation
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    translate(std::string name, const Vector& dp);
    SET_SWIG_COMPLETABLE_METHOD(translate)

    /*------------------------------------------------------------------------*/
    /** \brief translation d'une copie d'un repère (identifiée par un nom unique pour python)
     *  suivant le vecteur de translation défini par dp
     *
     *  \param name nom de l'entité repère
     *  \param dp le vecteur de translation
     *  \param groupName groupe dans lequel est mise la nouvelle entité
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copyAndTranslate(std::string name, const Vector& dp, std::string groupName);
    SET_SWIG_COMPLETABLE_METHOD(copyAndTranslate)

    /*------------------------------------------------------------------------*/
    /** \brief rotation d'un repère
     *
     *  \param name nom de l'entité repère
     *  \param rot      la rotation
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		rotate(std::string name,
				const Utils::Math::Rotation& rot);
    SET_SWIG_COMPLETABLE_METHOD(rotate)

    /*------------------------------------------------------------------------*/
    /** \brief rotation d'une copie d'un repère
     *
     *  \param name nom de l'entité repère
     *  \param rot la rotation
     *  \param groupName groupe dans lequel est mise la nouvelle entité
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		copyAndRotate(std::string name,
			const Utils::Math::Rotation& rot, std::string groupName);
    SET_SWIG_COMPLETABLE_METHOD(copyAndRotate)

    /*------------------------------------------------------------------------*/
    /** \brief création d'un repère par copie
     *
     *  \param name nom de l'entité repère
     *  \param groupName groupe dans lequel est mise la nouvelle entité
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copy(std::string name, std::string groupName);
    SET_SWIG_COMPLETABLE_METHOD(copy)


protected:

	/** Constructeur de copie : interdit. */
	SysCoordManagerIfc (const SysCoordManagerIfc&);

	/** Opérateur = : interdit. */
	SysCoordManagerIfc& operator = (const SysCoordManagerIfc&);

};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* SYSCOORD_MANAGER_IFC_H_ */
/*----------------------------------------------------------------------------*/
