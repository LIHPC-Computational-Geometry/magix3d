//
// Created by calderans on 6/20/25.
//


#include "Internal/Context.h"
#include "QtComponents/QtBlocksOptionsDialog.h"

#include <QBoxLayout>
#include <QtUtil/QtConfiguration.h>
#include <QLabel>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <TkUtil/MemoryError.h>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;


namespace Mgx3D {

    namespace QtComponents {
// ===========================================================================
//                        LA CLASSE QtBlocksOptionsDialog
// ===========================================================================

        QtBlocksOptionsDialog::QtBlocksOptionsDialog (
                QWidget* parent, const string& appTitle, const string& fileName)
                : QDialog (parent),
                  _geomAssocCheckBox (0), _closurePanel (0)
        {
            createGui (parent, appTitle, fileName);
        }	// QtBlocksOptionsDialog::QtBlocksOptionsDialog




        QtBlocksOptionsDialog::QtBlocksOptionsDialog (const QtBlocksOptionsDialog&)
                : QDialog (0), _geomAssocCheckBox (0), _closurePanel (0)
        {
            MGX_FORBIDDEN ("QtMdlOptionsDialog copy constructor is not allowed.");
        }	// QtBlocksOptionsDialog::QtBlocksOptionsDialog (const QtBlocksOptionsDialog&)


        QtBlocksOptionsDialog& QtBlocksOptionsDialog::operator = (const QtBlocksOptionsDialog&)
        {
            MGX_FORBIDDEN ("QtMdlOptionsDialog assignment operator is not allowed.");
            return *this;
        }	// QtBlocksOptionsDialog::QtBlocksOptionsDialog (const QtBlocksOptionsDialog&)


        QtBlocksOptionsDialog::~QtBlocksOptionsDialog ( )
        {
        }	// QtBlocksOptionsDialog::~QtBlocksOptionsDialog


        void QtBlocksOptionsDialog::createGui (
                QWidget* parent, const string& appTitle, const string& fileName)
        {
            setModal (true);
            setWindowModality (Qt::WindowModal);
            setWindowTitle (appTitle.c_str ( ));

            QBoxLayout*	layout	= new QVBoxLayout (this);
            layout->setMargin (QtConfiguration::margin);
            layout->setSizeConstraint (QLayout::SetMinimumSize);
            setLayout (layout);

            // Nom du fichier de sauvegarde de l'état de l'IHM :
            UTF8String	name (Charset::UTF_8);
            name << "Fichier BLK importé : " << fileName;
            QLabel*	label	= new QLabel (UTF8TOQSTRING (name), this);
            layout->addWidget (label);
            layout->addSpacing (20);

            // Importer la topologie ? :
            _geomAssocCheckBox	=
                    new QCheckBox ("Associer la topologie", this);
            _geomAssocCheckBox->setCheckState (Qt::Checked);
            layout->addWidget (_geomAssocCheckBox);

            _closurePanel	= new QtDlgClosurePanel (this, false, "OK", "", "Annuler");
            layout->addWidget (_closurePanel);
            _closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));
            connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
                     SLOT(accept ( )));
            connect (_closurePanel->getCancelButton ( ), SIGNAL(clicked ( )), this,
                     SLOT(reject ( )));
            _closurePanel->getApplyButton ( )->setAutoDefault (false);
            _closurePanel->getApplyButton ( )->setDefault (false);
            _closurePanel->getCancelButton ( )->setAutoDefault (false);
            _closurePanel->getCancelButton ( )->setDefault (false);

            layout->activate ( );
        }	// QtBlocksOptionsDialog::createGui


        bool QtBlocksOptionsDialog::geometricAssociation ( ) const
        {
            CHECK_NULL_PTR_ERROR (_geomAssocCheckBox)
            return Qt::Checked == _geomAssocCheckBox->checkState ( ) ?
                   true : false;
        }	// QtBlocksOptionsDialog::geometricAssociation

    }
}