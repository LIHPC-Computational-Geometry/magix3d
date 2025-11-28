/*----------------------------------------------------------------------------*/
/** \file TopoManager.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18/05/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef SYSCOORD_MANAGER_H_
#define SYSCOORD_MANAGER_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResult.h"
#include "Utils/Constants.h"
#include "Utils/SwigCompletion.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
namespace Math {
class Point;
class Vector;
class Rotation;
}
}
using Mgx3D::Utils::Math::Point;
using Mgx3D::Utils::Math::Vector;

namespace Internal {
class Context;
}

/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
class SysCoord;

/**
 * \class SysCoordManager
 * \brief Gestionnaire des repères
 *
 * Ceux-ci sont orthonormés
 *
 */
class SysCoordManager final : public Internal::CommandCreator {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param		Nom unique de l'instance (utile en environnement distribué).
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
	SysCoordManager(const std::string& name, Internal::Context* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     * Il se charge de la destruction des entités
     */
    ~SysCoordManager();

#ifndef SWIG
	/*------------------------------------------------------------------------*/
	/** Réinitialisation     */
	void clear();
#endif

	/*------------------------------------------------------------------------*/
	/** \brief Création d'un repère centré
	 */
	Mgx3D::Internal::M3DCommandResult*
		newSysCoord(std::string groupName);

	/*------------------------------------------------------------------------*/
	/** \brief Création d'un repère à partir d'un centre (simple translation)
	 */
	Mgx3D::Internal::M3DCommandResult*
	   newSysCoord(const Point& p, std::string groupName);

	/*------------------------------------------------------------------------*/
	/** \brief Création d'un repère à partir d'un centre et de 2 points pour donner les directions
	 *
	 * Le repère est ajusté pour être orthonormé
	 */
	Mgx3D::Internal::M3DCommandResult*
	   newSysCoord(const Point& pCentre, const Point& pX, const Point& pY, std::string groupName);


    /*------------------------------------------------------------------------*/
    /** \brief translation d'un repère (identifié par un nom unique pour python)
     *  suivant le vecteur de translation défini par dp
     *
     *  \param name nom de l'entité repère
     *  \param dp le vecteur de translation
     */
    Mgx3D::Internal::M3DCommandResult*
	    translate(std::string name, const Vector& dp);
	SET_SWIG_COMPLETABLE_METHOD(translate)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
		translate(SysCoord* syscoord, const Vector& dp);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief translation d'une copie d'un repère (identifié par un nom unique pour python)
     *  suivant le vecteur de translation défini par dp
     *
     *  \param name nom de l'entité repère à copier et translater
     *  \param dp le vecteur de translation
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResult*
        copyAndTranslate(std::string name, const Vector& dp, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndTranslate)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
	    copyAndTranslate(SysCoord* syscoord, const Vector& dp, std::string groupName);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief rotation d'une ou plusieurs entités repère
     *
     *  \param name nom de l'entité repère
     *  \param rot      la rotation
     */
    Mgx3D::Internal::M3DCommandResult*
		rotate(std::string name,
				const Utils::Math::Rotation& rot);
	SET_SWIG_COMPLETABLE_METHOD(rotate)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
		rotate(SysCoord* syscoord,
				const Utils::Math::Rotation& rot);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief rotation d'une copie d'entitée repère
     *
     *  \param name nom de l'entité repère
     *  \param rot la rotation
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResult*
		copyAndRotate(std::string name,
			const Utils::Math::Rotation& rot, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndRotate)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
		copyAndRotate(SysCoord* syscoord,
			const Utils::Math::Rotation& rot, std::string groupName);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'entités repère par copie
     *
     *  \param name nom de l'entité repère
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResult*
        copy(std::string name, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copy)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        copy(SysCoord* syscoord, std::string groupName);
#endif

#ifndef SWIG
    /*------------------------------------------------------------------------*/
	/// retourne le repère à partir de son nom
	SysCoord* getSysCoord (const std::string& name, const bool exceptionIfNotFound=true) const;

	void add(SysCoord* rep);
	void remove(SysCoord* rep);
#endif

private:
	/** Constructeur de copie : interdit. */
	SysCoordManager (const SysCoordManager&);

	/** Opérateur = : interdit. */
	SysCoordManager& operator = (const SysCoordManager&);

    /** repères accessibles depuis le manager */
    std::vector<CoordinateSystem::SysCoord*> m_reperes;
};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* SYSCOORD_MANAGER_H_ */
/*----------------------------------------------------------------------------*/
