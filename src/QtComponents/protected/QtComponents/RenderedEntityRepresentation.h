/**
 * \file		RenderedEntityRepresentation.h
 * \author		Charles PIGNEROL
 * \date		09/10/2013
 */
#ifndef RENDERED_ENTITY_REPRESENTATION_H
#define RENDERED_ENTITY_REPRESENTATION_H

#include <Utils/GraphicalEntityRepresentation.h>

namespace Mgx3D 
{

namespace QtComponents
{

class RenderingManager;


/**
 * \brief		Classe <I>représentant graphique d'entité</I> spécialisée pour
 *				un affichage 3D reposant sur un gestionnaire de rendu 3D.
 *
 */
class RenderedEntityRepresentation :
						public Utils::GraphicalEntityRepresentation
{
	public :

	/**
	 * \param		Entité représentée
	 */
	RenderedEntityRepresentation (Mgx3D::Utils::Entity& entity);

	/**
	 * Destructeur. RAS.
	 * \see		destroyRepresentations
	 */
	virtual ~RenderedEntityRepresentation ( );

	/**
	 * \see		Gestionnaire de rendus 3D pour les affichages.
	 * \see		setRenderingManager
	 */
	virtual Mgx3D::QtComponents::RenderingManager* getRenderingManager ( );
	virtual const Mgx3D::QtComponents::RenderingManager*
												getRenderingManager ( ) const;

	/**
	 * \param		Gestionnaire de rendus 3D pour les affichages si celui-ci
	 *				n'a pas été déjà transmis vis <I>show</I> (ex : mise en
	 *				évidence (highlighting) d'une entité qui vient d'être
	 *				créee).
	 * \see			getRenderingManager
	 */
	virtual void setRenderingManager(Mgx3D::QtComponents::RenderingManager* rm);

	/**
	 * \return		Les propriétés d'affichage à utiliser, personnelles ou
	 * 				globales selon la valeur de retour de
	 * 				<I>RenderingManager::useGlobalDisplayProperties ( )</I>
	 * 				du gestionnaire associé.
	 * \warning		A n'utiliser qu'en lecture, le const est intentionnel.
	 */
	virtual const Utils::DisplayProperties
									getDisplayPropertiesAttributes ( ) const;

	/**
	 * \return		Le masque d'affichage à utiliser, personnel ou global, selon
	 * 				la valeur de retour de
	 * 				<I>RenderingManager::useGlobalDisplayProperties ( )</I>
	 * 				du gestionnaire associé.
	 */
	virtual unsigned long getUsedRepresentationMask ( ) const;

	/**
	 * \return		La couleur à utiliser pour représenter l'entité 
	 * 				pour la valeur de représentation transmise
	 * 				(CLOUDS, CURVES, ISOCURVES, ...). Prend en compte l'éventuel
	 * 				caractere sélectionné et/ou mis en évidence, les préférences
	 * 				utilisateurs.
	 * \see			getBaseColor
	 */
	virtual TkUtil::Color getColor (unsigned long rep) const;

	/**
	 * \return		La couleur à utiliser pour représenter l'entité 
	 * 				pour la valeur de représentation transmise
	 * 				(CLOUDS, CURVES, ISOCURVES, ...). Ne prend pas en compte
	 * 				l'éventuel caractere sélectionné et/ou mis en évidence.
	 * \see			getColor
	 */
	virtual TkUtil::Color getBaseColor (unsigned long rep) const;

	/**
	 * \return		L'épaisseur de point à utiliser pour représenter l'entité
	 *				transmise en argument et pour la valeur de représentation
	 *				transmise (CLOUDS, CURVES, ISOCURVES, ...). Prend en compte
	 *				l'éventuel caractere sélectionné et/ou mis en évidence, les
	 *				préférences utilisateurs.
	 */
	float getPointSize (const Mgx3D::Utils::Entity& entity, unsigned long rep);


	/**
	 * \return		L'épaisseur de trait à utiliser pour représenter l'entité
	 *				transmise en argument et pour la valeur de représentation
	 *				transmise (CLOUDS, CURVES, ISOCURVES, ...). Prend en compte
	 *				l'éventuel caractere sélectionné et/ou mis en évidence, les
	 *				préférences utilisateurs.
	 */
	float getLineWidth (const Mgx3D::Utils::Entity& entity, unsigned long rep);


	protected :

	/**
	 * Détruit la représentation graphique si <I>realyDestroy</I> vaut
	 * <I>true</I>, peut se contenter de la mettre en cache dans le cas
	 * contraire, mais ne doit plus afficher la représentation graphique.
	 * \see			destroyDataOnHide
	 * \warning		Ne fait rien par défaut.
	 */
	virtual void destroyRepresentations (bool realyDestroy);

	/**
	 * \return		<I>true</I> si la représentation graphique doit être
	 * 				détruite lorsque l'entité n'est plus affichée, <I>false</I>
	 * 				si elle doit être conservée en cache. La valeur retournée
	 * 				par défaut dépend du type de l'entité et des préférences
	 * 				utilisateurs <I>QtMgx3DApplication::_geomDestroyOnHide</I>,
	 * 				<I>QtMgx3DApplication::_topoDestroyOnHide</I>, et
	 * 				<I>QtMgx3DApplication::_meshDestroyOnHide</I> prévues à cet
	 * 				effet.
	 * \see			destroyRepresentations
	 */
	virtual bool destroyDataOnHide ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	RenderedEntityRepresentation (RenderedEntityRepresentation&);
	RenderedEntityRepresentation& operator = (
										const RenderedEntityRepresentation&);


	protected :

	/** Eventuel gestionnaire d'affichage où est affiché l'entité. */
	Mgx3D::QtComponents::RenderingManager*			_renderingManager;
};	// class RenderedEntityRepresentation


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// RENDERED_ENTITY_REPRESENTATION_H
