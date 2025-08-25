/**
 * \file		QtDimensionsSelectorPanel.h
 * \author		Charles PIGNEROL
 * \date		21/11/2013
 */
#ifndef QT_DIMENSIONS_SELECTOR_PANEL_H
#define QT_DIMENSIONS_SELECTOR_PANEL_H


#include "Internal/SelectionManager.h"

#include <QButtonGroup>
#include <QCheckBox>



namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Classe permettant la saisie d'une combinaisons de dimensions selon le type
 * énuméré <I>SelectionManager::DIM</I>.
 */
class QtDimensionsSelectorPanel : public QWidget
{
	Q_OBJECT

	public :

	/**
	 * \param	Widget parent.
	 * \param	Dimensions sélectionnables
	 * \param	Dimensions proposées par défaut
	 * \param	<I>true</I> si la sélection de plusieurs dimensions est
	 * 			autorisée, <I>false</I> si seule une dimension peut être
	 * 			sélectionnée.
	 */
	QtDimensionsSelectorPanel (
				QWidget* parent,
				Mgx3D::Utils::SelectionManager::DIM allowedDimensions,
				Mgx3D::Utils::SelectionManager::DIM defaultDimensions,
				bool allowMultipleDimensions);

	/**
	 * RAS.
	 */
	virtual ~QtDimensionsSelectorPanel ( );

	/**
	 * \return		<I>true</I> si la sélection de plusieurs dimensions est
	 * 				autorisée, <I>false</I> si seule une dimension peut être
	 * 				sélectionnée.
	 */
	virtual bool allowMultipleDimensions ( ) const;

	/**
	 * \return		Les dimensions sélectionnées par l'utilisateur.
	 * \see			setDimensions
	 */
	virtual Mgx3D::Utils::SelectionManager::DIM getDimensions ( ) const;

	/**
	 * \param		Nouvelles dimensions.
	 * \see			getDimensions
	 */
	virtual void setDimensions (Mgx3D::Utils::SelectionManager::DIM dims);

	/**
	 * \return		Les dimensions sélectionnables.
	 * \see			setAllowedDimensions
	 * \see			isDimensionAllowed
	 */
	virtual Mgx3D::Utils::SelectionManager::DIM getAllowedDimensions ( ) const;
	
	/**
	 * \return		true si la dimension transmise en argument est séleectionnable, false dans le cas contraire.
	 * \see			setAllowedDimensions
	 * \see			getAllowedDimensions
	 */
	virtual bool isDimensionAllowed (Mgx3D::Utils::SelectionManager::DIM dimension) const;
	
	/**
	 * Actualise le panneau conformément aux dimensions reçues en argument.
	 * \param		Nouvelles dimensions sélectionnables.
	 * \see			getAllowedDimensions
	 */
	virtual void setAllowedDimensions (Mgx3D::Utils::SelectionManager::DIM dimensions);


	signals :

	/**
	 * Signal émit lorsque la combinaison de dimensions est modifiée.
	 */
	void dimensionsModified ( );


	protected slots :

	/**
	 * Callback sur la modification de la sélection. Emet le signal
	 * <I>dimensionsModified</I>.
	 */
	virtual void dimensionsModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie/opérateur = : Opérations interdites :
	 */
	QtDimensionsSelectorPanel (const QtDimensionsSelectorPanel&);
	QtDimensionsSelectorPanel& operator = (const QtDimensionsSelectorPanel&);

	/** La gestion exclusive ou non de la dimension. */
	QButtonGroup*							_buttonGroup;

	/** Les dimensions possibles. */
	QCheckBox								*_d0CheckBox, *_d1CheckBox, *_d2CheckBox, *_d3CheckBox;
	
	/** Les dimensions autorisées. */
	Mgx3D::Utils::SelectionManager::DIM	_allowedDimensions;
};	// class QtDimensionsSelectorPanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_DIMENSIONS_SELECTOR_PANEL_H
