//
// Created by calderans on 6/20/25.
//

#ifndef MAGIX3D_QTBLOCKSOPTIONSDIALOG_H
#define MAGIX3D_QTBLOCKSOPTIONSDIALOG_H

#include <QtUtil/QtDlgClosurePanel.h>
#include <QtUtil/QtTextField.h>

#include <QDialog>
#include <QCheckBox>

namespace Mgx3D {

namespace QtComponents {

/**
 * Boite de dialogue pemettant de modifier les paramètres d'importation de CAO
 * au format <I>MDL</I>.
 */
class QtBlocksOptionsDialog : public QDialog {

    Q_OBJECT

    public :
    /**
	 * Constructeur 1.
	 * \param	Widget parent.
	 * \param	Nom de l'application
	 * \param	Nom du fichier <I>MDL</I> importé.
	 * \see		createGui
	 */
    QtBlocksOptionsDialog (
            QWidget* parent, const std::string& appTitle, const std::string& fileName);

    /**
	 * RAS.
	 */
    virtual ~QtBlocksOptionsDialog ( );

    /**
	 * \return	<I>true</I> l'association géométrique est prise en compte dans l'opération I/O.
	 */
    virtual bool geometricAssociation ( ) const;


protected :

    /**
     * Créé l'IHM.
     * \param	Widget parent.
     * \param	Nom de l'application
     * \param	Nom du fichier <I>MDL</I> impoprté.
     */
    virtual void createGui (
            QWidget* parent, const std::string& appTitle, const std::string& fileName);


private :

    // Opérations interdites :
    QtBlocksOptionsDialog (const QtBlocksOptionsDialog&);
    QtBlocksOptionsDialog& operator = (const QtBlocksOptionsDialog&);

    /** Les cases à cocher correspondant aux différentes questions. */
    QCheckBox					*_geomAssocCheckBox;

    /** La fermeture de la boite de dialogue. */
    QtDlgClosurePanel*				_closurePanel;
};
}
}


#endif //MAGIX3D_QTBLOCKSOPTIONSDIALOG_H
