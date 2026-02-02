
#include "Internal/Context.h"
#include "QtComponents/QtCGNSOptionsDialog.h"

#include <QBoxLayout>
#include <QButtonGroup>
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
//                        LA CLASSE QtCGNSOptionsDialog
// ===========================================================================

        QtCGNSOptionsDialog::QtCGNSOptionsDialog (
                QWidget* parent, const string& appTitle, const string& fileName)
                : QDialog (parent),
                  _2DCheckBox (0), _3DCheckBox (0), _buttonGroup (0), _closurePanel (0)
        {
            createGui (parent, appTitle, fileName);
        }	// QtCGNSOptionsDialog::QtCGNSOptionsDialog




        QtCGNSOptionsDialog::QtCGNSOptionsDialog (const QtCGNSOptionsDialog&)
                : QDialog (0), _2DCheckBox (0), _3DCheckBox (0), _buttonGroup (0), _closurePanel (0)
        {
            MGX_FORBIDDEN ("QtBlocksOptionsDialog copy constructor is not allowed.");
        }	// QtCGNSOptionsDialog::QtCGNSOptionsDialog (const QtCGNSOptionsDialog&)


        QtCGNSOptionsDialog& QtCGNSOptionsDialog::operator = (const QtCGNSOptionsDialog&)
        {
            MGX_FORBIDDEN ("QtBlocksOptionsDialog assignment operator is not allowed.");
            return *this;
        }	// QtCGNSOptionsDialog::QtCGNSOptionsDialog (const QtCGNSOptionsDialog&)


        QtCGNSOptionsDialog::~QtCGNSOptionsDialog ( )
        {
        }	// QtCGNSOptionsDialog::~QtCGNSOptionsDialog


        void QtCGNSOptionsDialog::createGui (
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
            name << "Fichier CGNS : " << fileName;
            QLabel*	label	= new QLabel (UTF8TOQSTRING (name), this);
            layout->addWidget (label);
            layout->addSpacing (20);

            _buttonGroup	= new QButtonGroup (this);
            _buttonGroup->setExclusive (true);

            // Prendre en compte l'association géométrique ? :
            _2DCheckBox	=
                    new QCheckBox ("dim 2D", this);
            _2DCheckBox->setCheckState (Qt::Checked);
            layout->addWidget (_2DCheckBox);
            _buttonGroup->addButton(_2DCheckBox);

            _3DCheckBox	=
                    new QCheckBox ("dim 3D", this);
            _3DCheckBox->setCheckState (Qt::Unchecked);
            layout->addWidget (_3DCheckBox);
            _buttonGroup->addButton(_3DCheckBox);

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
        }	// QtCGNSOptionsDialog::createGui



        int QtCGNSOptionsDialog::getDim ( ) const
        {
            CHECK_NULL_PTR_ERROR (_2DCheckBox)
            CHECK_NULL_PTR_ERROR (_3DCheckBox)

            if (Qt::Checked == _2DCheckBox->checkState ( ) && Qt::Unchecked == _3DCheckBox->checkState ( ))
                return 2;
            if (Qt::Unchecked == _2DCheckBox->checkState ( ) && Qt::Checked == _3DCheckBox->checkState ( ))
                return 3;

            return 0;
        }	// QtCGNSOptionsDialog::getDim

    }
}
