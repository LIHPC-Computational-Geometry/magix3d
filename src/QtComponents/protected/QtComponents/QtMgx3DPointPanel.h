/**
 * \file		QtMgx3DPointPanel.h
 * \author		Charles PIGNEROL
 * \date		19/12/2012
 */

#ifndef QT_MGX3D_POINT_PANEL_H
#define QT_MGX3D_POINT_PANEL_H

#include "Internal/ContextIfc.h"

#include <TkUtil/util_config.h>
#include "QtComponents/QtEntityIDTextField.h"

#include <QtUtil/QtDoubleTextField.h>
#include <QComboBox>
#include <QLabel>

#include <string>


namespace Mgx3D
{

namespace CoordinateSystem
{
class SysCoord;
}

namespace QtComponents
{

class QtMgx3DEntityPanel;

/**
 * Panneau Qt permettant la saisie d'un point dans Magix 3D.
 */
class QtMgx3DPointPanel : public QWidget
{
	Q_OBJECT

	public :

	enum coordinateType {cartesian, spherical, cylindrical};

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
	 *				point.
	 * \param		Types de "point" sélectionnable, ou
	 * 				<I>FilterEntity::AllPoints</I> si tous les types de points
	 * 				conviennent.
	 * \param		<I>true</I. si l'organisation des champs de texte doit être
	 * 				verticale, <I>false</I> si elle doit être horizontale.
	 */
	QtMgx3DPointPanel (QWidget* parent, const std::string& title, bool editable,
	               const std::string& xTitle, const std::string& yTitle,
	               const std::string& zTitle, 
	               double x, double xMin, double xMax,
	               double y, double yMin, double yMax,
	               double z, double zMin, double zMax,
	               Mgx3D::QtComponents::QtMgx3DMainWindow* mainWindow,
	               Mgx3D::Utils::FilterEntity::objectType types,
					bool verticalLayout = false);

	/** Destructeur. */
	virtual ~QtMgx3DPointPanel ( );

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
	virtual void setX (double x);

//	/**
//	 * \return			la première composante de la donnée représentée.
//	 * \throw			Exception si la valeur est incorrecte. */
//	virtual double getX ( ) const;

	/**
	 * \param			Nouvelle seconde composante de la donnée à représenter.
	 * \throw			Exception si la valeur est incorrecte. */
	virtual void setY (double y);

//	/**
//	 * \return			la seconde composante de la donnée représentée.
//	 * \throw			Exception si la valeur est incorrecte. */
//	virtual double getY ( ) const;

	/**
	 * \param			Nouvelle troisième composante de la donnée à
	 *					représenter.
	 * \throw			Exception si la valeur est incorrecte. */
	virtual void setZ (double z);

//	/**
//	 * \return			la troisième composante de la donnée représentée.
//	 * \throw			Exception si la valeur est incorrecte. */
//	virtual double getZ ( ) const;

	/**
	 * \return			le widget utilisé pour la première composante.
	 */
	QtTextField* getXTextField ( ) const
	{ return _xTextField; }

	/**
	 * \return			le widget utilisé pour la seconde composante.
	 */
	QtTextField* getYTextField ( ) const
	{ return _yTextField; }

	/**
	 * \return			le widget utilisé pour la troisième composante.
	 */
	QtTextField* getZTextField ( ) const
	{ return _zTextField; }

	/**
	 * \return			le champ de saisie d'un vertex.
	 */
	QtEntityIDTextField* getVertexTextField ( ) const
	{ return _vertexIDTextField; }

	/**
	 * \return			le point saisi
	 */
	virtual Mgx3D::Utils::Math::Point getPoint ( ) const;

	/**
	 * \return		Le nom unique de l'entité représentée.
	 * \see			setUniqueName
	 */
	virtual std::string getUniqueName ( ) const;

	/**
	 * Actualise les coordonnées.
	 * \param		Le nom unique de la nouvelle entité représentée.
	 * \see			setUniqueName
	 */
	virtual void setUniqueName (const std::string& name);

	/**
	 * retourne le repère local ou 0 si pas sélectionné
	 */
	virtual CoordinateSystem::SysCoord* getSysCoord() const;

	/**
	 * Réinitialise le champ de saisie.
	 */
	virtual void reset ( );

	/**
	 * Annule la sélection courante.
	 */
	virtual void clearSelection ( );

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * Actualisation de la représentation de la sélection.
	 * \return	<I>true</I> en cas de modification, <I>false</I> dans le cas
	 * 			contraire.
	 */
	virtual bool actualizeGui (bool removeDestroyed);

	/**
	 * Retourne le type de coordonnées utilisées
	 */
	virtual coordinateType getTypeCoordinate ();

	/** Nombre de caractères par défaut des champs numériques. Cette
	 * valeur est initialement de 12. */
	static int			numberOfCharacters;


	signals :

	/**
	 * Signal émis lorsque l'utilisateur vient de finir une action de
	 * modification du point.
	 */
	void pointModified ( );

	/**
	 * Signal émis lorsqu'un point est ajoutée à la sélection.
	 * \param		Nom du point ajouté.
	 * \see			pointRemovedFromSelection
	 * \see			selectionModified
	 */
	void pointAddedToSelection (QString);

	/**
	 * Signal émis lorsqu'un point est enlevé de la sélection.
	 * \param		Nom du point enlevé.
	 * \see			pointAddedToSelection
	 * \see			selectionModified
	 */
	void pointRemovedFromSelection (QString);


	protected slots :

	/**
	 * Callback appelé lorsque l'utilisateur sélectionne de manière interactive
	 * un nouveau point.
	 * Emet les signaux <I>pointModified</I> et <I>pointAddedToSelection</I>.
	 */
	virtual void selectedVertexCallback (const QString name);

	/**
	 * Invoqué lorsqu'un point est ajouté à la sélection.
	 * Emet le signal pointAddedToSelection.
	 */
	virtual void pointAddedToSelectionCallback (QString pointName);

	/**
	 * Invoqué lorsqu'un point est enlevé de la sélection.
	 * Emet le signal pointRemovedFromSelection.
	 */
	virtual void pointRemovedFromSelectionCallback (QString pointName);

	/**
	 * Invoqué lorsque des points sont ajoutés à la sélection.
	 * \warning		Seul le premier point est traité
	 * Emet le signal pointAddedToSelection.
	 */
	virtual void pointsAddedToSelectionCallback (QString pointsNames);

	/**
	 * Invoqué lorsque des points sont enlevés de la sélection.
	 * \warning		Seul le premier point est traité
	 * Emet le signal pointRemovedFromSelection.
	 */
	virtual void pointsRemovedFromSelectionCallback (QString pointsNames);

	/**
	 * Callback appelé lorsque l'utilisateur modifie les coordonnées du point.
	 * N'est pas appelé lorsque les coordonnées sont modifiées par le programme.
	 * => Efface le contenu du champ de texte contenant l'ID du vertex.
	 * Emet le signal <I>pointModified</I>.
	 */
	virtual void coordinatesEditedCallback ( );

	/**
	 * Callback appelé lorsque les coordonnées du point sont modifiées
	 * (utilisateur et/ou programme).
	 * Emet le signal <I>pointModified</I>.
	 */
	virtual void coordinatesModifiedCallback ( );

	/**
	 * Callback appelé lors du changement de type de coordonnées
	 */
	virtual void coordTypeChangedCallback (int index);


	protected :

	/**
	 * Actualise les coordonnées affichées selon le point sélectionné.
	 * \param		Nom du point sélectionné.
	 */
	virtual void updateCoordinates (const std::string& name);


	/// accès au context
	Internal::Context& getContext() const;

	private :

	/** Constructeur de copie. Interdit. */
	QtMgx3DPointPanel (const QtMgx3DPointPanel&);

	/** Opérateur de copie. Interdit. */
	QtMgx3DPointPanel& operator = (const QtMgx3DPointPanel&);

	/** Les valeurs initiales des trois composantes. */
	double				_initialX, _initialY, _initialZ;

	/** Les noms des types des composantes. */
	QLabel			*_xTextLabel, *_yTextLabel, *_zTextLabel;

	/** Les champs de saisie des composantes de la donnée représentée.
	 */
	QtDoubleTextField	*_xTextField, *_yTextField, *_zTextField;

	/**
	 * L'éventuel vertex dont on copie les coordonnées.
	 */
	Mgx3D::QtComponents::QtEntityIDTextField*	_vertexIDTextField;

	/** ComboBox / type de coordonnées: cartésiennes, sphérique ou cylindrique */
	QComboBox* _coordTypeComboBox;

	/** repère local ou non */
	QtMgx3DEntityPanel*   _sysCoordPanel;

	/** Le titre du panneau. */
	const std::string	_title;
};	// class QtMgx3DPointPanel


}	// namespace QtComponents

}	// namespace Mgx3D



#endif	// QT_MGX3D_POINT_PANEL_H
