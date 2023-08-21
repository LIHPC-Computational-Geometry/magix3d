/**
 * \file		QtMgx3DGroupNamePanel.h
 * \author		Charles PIGNEROL
 * \date		11/12/2012
 */
#ifndef QT_MGX3D_GROUP_NAME_H
#define QT_MGX3D_GROUP_NAME_H


#include "Utils/ValidatedField.h"
#include "Group/GroupManagerIfc.h"
#include "QtComponents/QtMgx3DMainWindow.h"

#include <QComboBox>

#include <vector>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Classe permettant la saisie d'un nom de groupe <I>Magix 3D</I> et offrant
 * une <I>API</I> de validation <I>ValidatedField</I>.
 * \see		ValidatedField
 * \see		GroupManagerIfc
 */
class QtMgx3DGroupNamePanel :
			public QWidget, public Mgx3D::Utils::ValidatedField
{
	public :

	/**
	 * La politique de saisie du nom.
	 * <OL>
	 * <LI>INITIALIZATION</I> : initialisation du panneau, ne fait rien de
	 * particulier.
	 * <LI><I>CREATION</I> : mode création de groupe. Choix parmi la liste des
	 * groupes existants, possibilité de créer un nouveau nom unique.
	 * <LI><I>CONSULTATION</I> : nom non modifiable.
	 * </OL>
	 */
	enum POLICY { INITIALIZATION, CREATION, CONSULTATION };

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Dimension des groupes de données représentés par le panneau.
	 * \param	Politique de saisie du nom.
	 * \param	Nom proposé par défaut.
	 * En mode création le nom du groupe est à créer ou sélectionner dans la
	 * liste des noms existants. Dans le cas contraire il n'est pas modifiable.
	 * \see		getContext
	 */
	QtMgx3DGroupNamePanel (
		QWidget* parent, const std::string& name,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		int dimension, POLICY policy, const std::string& defaultName);

	/**
	 * RAS.
	 */
	virtual ~QtMgx3DGroupNamePanel ( );

	/**
	 * \return		Contexte <I>Magix 3D</I> courant.
	 */
	 virtual Mgx3D::Internal::ContextIfc& getContext ( );

	/**
	 * \param		La politique de saisie du nom.
	 * \see			validate
	 * \see			setPolicy
	 */
	virtual POLICY getPolicy ( ) const;

	/**
	 * Actualise le panneau conformément à la nouvelle politique.
	 * \param		La nouvelle politique de saisie du nom.
	 * \param		Un éventuel nom proposé par défaut.
	 * \see			validate
	 * \see			getPolicy
	 * \see			getGroupNames
	 */
	virtual void setPolicy (POLICY policy, const std::string& defaultName);

	/**
	 * \return		La dimension des groupes de données représentés par le
	 *				panneau.
	 * \see			setDimension
	 */
	virtual int getDimension ( ) const;

	/**
	 * \param		La dimension des groupes de données représentés par le
	 *				panneau.
	 * \see			getDimension
	 */
	virtual void setDimension (int dim);

	/**
	 * @return		Le nom saisi par l'utilisateur.
	 * \see			getDefaultName
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * @return		Le nom proposé par défaut.
	 * \see			getGroupName
	 * \see			setPolicy
	 */
	virtual std::string getDefaultName ( ) const;

	/**
	 * Lève une exception de type <I>TkUtil::Exception</I> si le nom saisi par
	 * l'utilisateur est invalide compte-tenu de la politique appliquée.
	 * \see			getPolicy
	 */
	virtual void validate ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 */
	virtual void autoUpdate ( );



	protected :

	/**
	 * \param		En sortie, la liste des noms de groupes à afficher dans la
	 *				boîte à défilement. Prend en compte la politique, la
	 *				dimension des groupes, le contexte, ...
	 */
	virtual void getGroupNames (std::vector< std::string >& names);


	private :

	/**
	 * Constructeur de copie/opérateur = : Opérations interdites :
	 */
	QtMgx3DGroupNamePanel (const QtMgx3DGroupNamePanel&);
	QtMgx3DGroupNamePanel& operator = (const QtMgx3DGroupNamePanel&);

	/** Le widget de saisie du nom. */
	QComboBox*									_nameComboBox;

	/** La dimension des groupes de données erprésentés par le panneau. */
	int											_dimension;

	/** La fenêtre <I>Magix 3D</I> de rattachement. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*		_mainWindow;

	/** La politique de saisie du nom. */
	POLICY										_policy;

	/** Le nom proposé par défaut. */
	std::string									_defaultName;
};	// class QtMgx3DGroupNamePanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MGX3D_GROUP_NAME_H
