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
#include <SysCoord/SysCoord.h>
#include <SysCoord/SysCoordManagerIfc.h>
#include "Utils/Container.h"
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
class ContextIfc;
class Context;
}

/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
/**
 * \class SysCoordManager
 * \brief Gestionnaire des repères
 *
 * Ceux-ci sont orthonormés
 *
 */
class SysCoordManager: public SysCoordManagerIfc {


public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param		Nom unique de l'instance (utile en environnement distribué).
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
	SysCoordManager(const std::string& name, Internal::ContextIfc* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     * Il se charge de la destruction des entités
     */
    ~SysCoordManager();

	/*------------------------------------------------------------------------*/
	/** Réinitialisation     */
	virtual void clear();

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


    /*------------------------------------------------------------------------*/
    /** \brief translation d'un repère (identifié par un nom unique pour python)
     *  suivant le vecteur de translation défini par dp
     *
     *  \param name nom de l'entité repère
     *  \param dp le vecteur de translation
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    translate(std::string name, const Vector& dp);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
		translate(SysCoord* syscoord, const Vector& dp);

    /*------------------------------------------------------------------------*/
    /** \brief translation d'une copie d'un repère (identifié par un nom unique pour python)
     *  suivant le vecteur de translation défini par dp
     *
     *  \param name nom de l'entité repère à copier et translater
     *  \param dp le vecteur de translation
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copyAndTranslate(std::string name, const Vector& dp, std::string groupName);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    copyAndTranslate(SysCoord* syscoord, const Vector& dp, std::string groupName);

    /*------------------------------------------------------------------------*/
    /** \brief rotation d'une ou plusieurs entités repère
     *
     *  \param name nom de l'entité repère
     *  \param rot      la rotation
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		rotate(std::string name,
				const Utils::Math::Rotation& rot);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
		rotate(SysCoord* syscoord,
				const Utils::Math::Rotation& rot);

    /*------------------------------------------------------------------------*/
    /** \brief rotation d'une copie d'entitée repère
     *
     *  \param name nom de l'entité repère
     *  \param rot la rotation
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		copyAndRotate(std::string name,
			const Utils::Math::Rotation& rot, std::string groupName);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
		copyAndRotate(SysCoord* syscoord,
			const Utils::Math::Rotation& rot, std::string groupName);

    /*------------------------------------------------------------------------*/
    /** \brief création d'entités repère par copie
     *
     *  \param name nom de l'entité repère
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copy(std::string name, std::string groupName);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copy(SysCoord* syscoord, std::string groupName);


    /*------------------------------------------------------------------------*/
	/// retourne le repère à partir de son nom
	virtual SysCoord* getSysCoord (const std::string& name, const bool exceptionIfNotFound=true) const;

	void add(SysCoord* rep) {m_reperes.add(rep);}
	void remove(SysCoord* rep) {m_reperes.remove(rep, true);}




private:
	/** Constructeur de copie : interdit. */
	SysCoordManager (const SysCoordManager&);

	/** Opérateur = : interdit. */
	SysCoordManager& operator = (const SysCoordManager&);

    /** repères accessibles depuis le manager */
    Utils::Container<CoordinateSystem::SysCoord> m_reperes;

};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* SYSCOORD_MANAGER_H_ */
/*----------------------------------------------------------------------------*/
