#ifndef MAGIX3D_QTCGNSOPTIONSDIALOG_H
#define MAGIX3D_QTCGNSOPTIONSDIALOG_H

#include <QtUtil/QtDlgClosurePanel.h>
#include <QtUtil/QtTextField.h>

#include <QDialog>
#include <QCheckBox>

namespace Mgx3D {

namespace QtComponents {
        /**
         * Boite de dialogue pemettant de modifier les paramètres d'exportation du maillage
         * au format <I>CGNS</I>.
         */
class QtCGNSOptionsDialog : public QDialog{

    Q_OBJECT

public:
    /**
	 * Constructeur 1.
	 * \param	Widget parent.
	 * \param	Nom de l'application
	 * \param	Nom du fichier <I>CGNS</I> importé.
	 * \see		createGui
	 */
    QtCGNSOptionsDialog(QWidget* parent, const std::string& appTitle, const std::string& fileName);

    virtual ~QtCGNSOptionsDialog();

    /**
	 * \return	la valeur de la dimension choisie pour l'export CGNS.
	 */
    virtual int getDim() const;

protected :

    /**
     * Créé l'IHM.
     * \param	Widget parent.
     * \param	Nom de l'application
     * \param	Nom du fichier <I>CGNS</I> impoprté.
     */
    virtual void createGui(QWidget * parent, const std::string& appTitle, const std::string & filename);

private :

    // Opérations interdites :
    QtCGNSOptionsDialog(QtCGNSOptionsDialog const & other);
    QtCGNSOptionsDialog & operator = (QtCGNSOptionsDialog const & other);

    /** Les cases à cocher correspondant aux différentes questions. */
    QCheckBox                           *_2DCheckBox;
    QCheckBox                           *_3DCheckBox;

	/** La gestion exclusive ou non de la dimension. */
	QButtonGroup                        *_buttonGroup;

    /** La fermeture de la boite de dialogue. */
    QtDlgClosurePanel                   *_closurePanel;
};
}
}

#endif //MAGIX3D_QTCGNSOPTIONSDIALOG_H