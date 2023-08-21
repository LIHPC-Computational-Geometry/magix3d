/*----------------------------------------------------------------------------*/
/** \file SysCoord.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18/05/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef UTIL_SYSCOORD_H_
#define UTIL_SYSCOORD_H_
/*----------------------------------------------------------------------------*/
#include "Internal/InternalEntity.h"
#include "SysCoord/SysCoordProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Internal {
class InfoCommand;
}
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {

class CommandEditSysCoord;

/*----------------------------------------------------------------------------*/
/** \class SysCoord
 *  \brief Objet repère
 *
 *  Celui-ci est forcément orthonormé
 */
/*----------------------------------------------------------------------------*/
class SysCoord: public Internal::InternalEntity{

	static const char* typeNameSysCoord;

public:

	/// Constructeur par défaut (repère standard)
	SysCoord(Internal::Context& ctx);

	/// Constructeur avec une simple translation
	SysCoord(Internal::Context& ctx, const Utils::Math::Point& pt);

	/// Constructeur complet, un centre et 2 directions
	SysCoord(Internal::Context& ctx,
			const Utils::Math::Point& pCentre,
			const Utils::Math::Point& pX,
			const Utils::Math::Point& pY);

	/*------------------------------------------------------------------------*/
	/** \brief  Fournit une représentation de l'entité à afficher. Le type de
	 *          représentation fournie dépend de dr. L'instance de
	 *          DisplayRepresentation dr fournit à la fois les paramètres de
	 *          représentation s'ils existent (par exemple filaire, solide,
	 *          discretisation) et stocke la représentation correspondante (
	 *          qui sera différente pour les entités géométriques, topologiques
	 *          ou de maillage)
	 *
	 *
	 *  \param  dr la représentation que l'on demande à afficher
	 *  \param	Lève une exception si checkDestroyed vaut true
	 */
	virtual void getRepresentation(
			Utils::DisplayRepresentation& dr, bool checkDestroyed) const;

	/*------------------------------------------------------------------------*/
	/** \brief  Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
	 * \return	Description, à détruire par l'appelant.
	 */
	virtual Mgx3D::Utils::SerializedRepresentation* getDescription (
			bool alsoComputed) const;

	/*------------------------------------------------------------------------*/
	/** \brief  retourne la dimension de l'entité
	 */
	int getDim() const {return 3;}

	/*------------------------------------------------------------------------*/
	/** \brief  Fournit un résumé textuel de l'entité.
	 */
	virtual std::string getSummary ( ) const;


	/*------------------------------------------------------------------------*/
	/** \brief  Donne les coordonnées d'un point dans ce repère local
	 */
	Utils::Math::Point toLocal(const Utils::Math::Point& pt);
	/*------------------------------------------------------------------------*/
	/** \brief  Donne les coordonnées d'un point (avec coordonnées locales) dans le global
	 */
	Utils::Math::Point toGlobal(const Utils::Math::Point& pt);


	/*------------------------------------------------------------------------*/
	/** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
	 */
	virtual std::string getTypeName() const {return typeNameSysCoord;}

	/** \brief Donne le type de l'objet
	 */
	virtual Utils::Entity::objectType getType() const {return Utils::Entity::SysCoord;}

	/** \brief Donne le nom court du type d'objet (pour le nommage des entités)
	 */
	static std::string getTinyName() {return "Rep";}

	/*------------------------------------------------------------------------*/
	/** \brief Test si l'entité est un repère suivant son nom
	 */
	static bool isA(const std::string& name);


    /*------------------------------------------------------------------------*/
    /** Duplique le SysCoordProperty pour en conserver une copie */
	void saveProperty(Internal::InfoCommand* icmd);


	/// transmet la propriété sauvegardée
	void saveInternals(CommandEditSysCoord* cest);

	/** Echange le SysCoordProperty avec celui donné, retourne l'ancien
	 * C'est à l'appelant de le détruire
	 */
	SysCoordProperty* setProperty(SysCoordProperty* prop);

	/// accès au propriétés
	const SysCoordProperty* getProperty() const {return m_property;}
	SysCoordProperty* getProperty() {return m_property;}

	/// met à jour la matrice inverce
	void update();

private :

	/// propriétés internes (centre eu matrice de rotation)
	SysCoordProperty* m_property;

	/// sauvegarde des propriétés internes
	SysCoordProperty* m_save_property;

	// pour passer du repère de ce repère local au repère par défaut
	Utils::Math::Matrix33 m_inv;

};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* UTIL_SYSCOORD_H_ */
/*----------------------------------------------------------------------------*/

