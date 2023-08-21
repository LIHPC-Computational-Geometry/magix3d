/**
 * \file		QtMgx3DVectorPanel.h
 * \author		Charles PIGNEROL
 * \date		25/11/2013
 */

#ifndef QT_MGX3D_VECTOR_PANEL_H
#define QT_MGX3D_VECTOR_PANEL_H

#include "Internal/ContextIfc.h"

#include <TkUtil/util_config.h>

#include "QtComponents/QtEntityIDTextField.h"
#include "Utils/Vector.h"
#include <QtUtil/QtDoubleTextField.h>

#include <string>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Panneau Qt permettant la saisie d'un vecteur dans Magix 3D. Cette saisie peut
 * être interactive et reposer sur une entité (arête/couple de points).
 */
class QtMgx3DVectorPanel : public QWidget
{
	Q_OBJECT

	public :

	/** Constructeur
	 * \param		widget parent. Ne doit pas être nul.
	 * \param		titre du panneau
	 * \param		true si le panneau est éditable, sinon false.
	 * \param		titre de la première composante.
	 * \param		titre de la seconde composante.
	 * \param		titre de la troisième composante.
	 * \param		valeur initiale de la première composante.
	 * \param		valeur minimale de la première composante.
	 * \param		valeur maximale de la première composante.
	 * \param		valeur initiale de la seconde composante.
	 * \param		valeur minimale de la seconde composante.
	 * \param		valeur maximale de la seconde composante.
	 * \param		valeur initiale de la troisième composante.
	 * \param		valeur minimale de la troisième composante.
	 * \param		valeur maximale de la troisième composante.
	 * \param		Fenêtre principale associée, pour la saisie interactive du
	 *				segment.
	 * \param		Types d'"entités" sélectionnables
	 * 				(ex : <I>FilterEntity::AllEdges</I> si on ne veut que des
	 * 				arêtes, ou
	 * 				<I>FilterEntity::AllEdges | FilterEntity::AllPoints</I> si
	 * 				tous les types de segments ou sommets conviennent.
	 * \param		<I>true</I. si l'organisation des champs de texte doit être
	 * 				verticale, <I>false</I> si elle doit être horizontale.
	 */
	QtMgx3DVectorPanel (QWidget* parent, const std::string& title, bool editable,
	               const std::string& xTitle, const std::string& yTitle,
	               const std::string& zTitle, 
	               double x, double xMin, double xMax,
	               double y, double yMin, double yMax,
	               double z, double zMin, double zMax,
	               Mgx3D::QtComponents::QtMgx3DMainWindow* mainWindow,
	               Mgx3D::Utils::FilterEntity::objectType types,
					bool verticalLayout	= false);

	/** Destructeur. */
	virtual ~QtMgx3DVectorPanel ( );


	public :

	/**
	 * Bloque (<I>true</I>) ou débloque (<I>false</I>) les signaux  <I>Qt</I>
	 * au niveau des champs de texte.
	 * La fonction ne s'appelle pas <I>blockSignals</I> car elle n'est pas
	 * virtuelle ...
	 */
	virtual void blockFieldsSignals (bool block);

	/**
	 * \return			true si l'utilisateur a modifié une composante
	 *					de la donnée représentée.
	 */
	virtual bool isModified ( ) const;

	/**
	 * \param			Nouvelle première composante de la donnée à
	 *					représenter.
	 * \throw			Exception si la valeur est incorrecte. */
	virtual void setDx (double dx);

	/**
	 * \return			la première composante de la donnée représentée.
	 * \throw			Exception si la valeur est incorrecte. */
	virtual double getDx ( ) const;

	/**
	 * \param			Nouvelle seconde composante de la donnée à représenter.
	 * \throw			Exception si la valeur est incorrecte. */
	virtual void setDy (double dy);

	/**
	 * \return			la seconde composante de la donnée représentée.
	 * \throw			Exception si la valeur est incorrecte. */
	virtual double getDy ( ) const;

	/**
	 * \param			Nouvelle troisième composante de la donnée à
	 *					représenter.
	 * \throw			Exception si la valeur est incorrecte. */
	virtual void setDz (double dz);

	/**
	 * \return			la troisième composante de la donnée représentée. 
	 * \throw			Exception si la valeur est incorrecte. */
	virtual double getDz ( ) const;

	/**
	 * \return			Le vecteur
	 */
	virtual Mgx3D::Utils::Math::Vector getVector ( ) const;

	/**
	 * \return			le widget utilisé pour la première composante.
	 */
	QtTextField* getDxTextField ( ) const
	{ return _dxTextField; }

	/**
	 * \return			le widget utilisé pour la seconde composante.
	 */
	QtTextField* getDyTextField ( ) const
	{ return _dyTextField; }

	/**
	 * \return			le widget utilisé pour la troisième composante.
	 */
	QtTextField* getDzTextField ( ) const
	{ return _dzTextField; }

	/**
	 * \param		Nom de l'éventuelle entité 2D associée.
	 */
	virtual void setSegment (const std::string& name);

	/**
	 * \return		Le champ de texte de saisie de l'identifiant d'un segment.
	 */
	Mgx3D::QtComponents::QtEntityIDTextField* getSegmentIDTextField ( )
	{ return _segmentIDTextField; }
	const Mgx3D::QtComponents::QtEntityIDTextField* getSegmentIDTextField ( ) const
	{ return _segmentIDTextField; }

	/**
	 * \return		Le champ de texte de saisie de l'identifiant du point 1.
	 */
	Mgx3D::QtComponents::QtEntityIDTextField* getPoint1IDTextField ( )
	{ return _point1IDTextField; }
	const Mgx3D::QtComponents::QtEntityIDTextField* getPoint1IDTextField ( ) const
	{ return _point1IDTextField; }

	/**
	 * \return		Le champ de texte de saisie de l'identifiant du point 2.
	 */
	Mgx3D::QtComponents::QtEntityIDTextField* getPoint2IDTextField ( )
	{ return _point2IDTextField; }
	const Mgx3D::QtComponents::QtEntityIDTextField* getPoint2IDTextField ( ) const
	{ return _point2IDTextField; }

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * Actualisation de la représentation de la sélection.
	 * \return  <I>true</I> en cas de modification, <I>false</I> dans le cas contraire.
	 */
	virtual bool actualizeGui (bool removeDestroyed);


	/** Nombre de caractères par défaut des champs numériques. Cette
	 * valeur est initialement de 12. */
	static int			numberOfCharacters;


	signals :

	/**
	 * Signal émis lorsque l'utilisateur vient de finir une action de modification du vecteur.
	 */
	void vectorModified ( );


	protected :

	/**
	 * \return	Les coordonnées du point dont le nom est transmis en argument.
	 */
	virtual Utils::Math::Point getPoint (const std::string& name) const;


	protected slots :

	/**
	 * Callback appelé lorsque l'utilisateur sélectionne de manière interactive
	 * un nouveau segment.
	 * Emet le signal <I>vectorModified ( )</I>.
	 */
	virtual void selectedSegmentCallback (const QString name);

	/**
	 * Callback appelé lorsque l'utilisateur sélectionne de manière interactive
	 * un nouveau point.
	 * Emet le signal <I>vectorModified ( )</I>.
	 */
	virtual void selectedPointCallback (const QString name);

	/**
	 * Callback appelé lorsque l'utilisateur modifie les coordonnées du point.
	 * N'est pas appelé lorsque les coordonnées sont modifiées par le programme.
	 * => Efface le contenu du champ de texte contenant l'ID du vertex.
	 * Emet le signal <I>vectorModified ( )</I>.
	 */
	virtual void coordinatesEditedCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtMgx3DVectorPanel (const QtMgx3DVectorPanel&);

	/** Opérateur de copie. Interdit. */
	QtMgx3DVectorPanel& operator = (const QtMgx3DVectorPanel&);

	/** Les valeurs initiales des trois composantes. */
	double				_initialDx, _initialDy, _initialDz;

	/** Les champs de saisie des composantes de la donnée représentée.
	 */
	QtDoubleTextField	*_dxTextField, *_dyTextField, *_dzTextField;

	/**
	 * L'éventuel segment dont on copie les composantes.
	 */
	Mgx3D::QtComponents::QtEntityIDTextField*	_segmentIDTextField;

	/**
	 * Les éventuels points dont on récupère les composantes.
	 */
	Mgx3D::QtComponents::QtEntityIDTextField*	_point1IDTextField;
	Mgx3D::QtComponents::QtEntityIDTextField*	_point2IDTextField;

	/** Le titre du panneau. */
	const std::string	_title;
};	// class QtMgx3DVectorPanel


}	// namespace QtComponents

}	// namespace Mgx3D



#endif	// QT_MGX3D_VECTOR_PANEL_H
