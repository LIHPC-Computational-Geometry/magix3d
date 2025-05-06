#ifndef IMPORTBREPIMPLEMENTATION_H_
#define IMPORTBREPIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomImport.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class ImportBREPImplementation
 * \brief Classe réalisant l'import du contenu d'un fichier de modélisation au
 *        format BREP (format Open Cascade).
 *
 * Contrairement aux fichiers STEP, les fichiers BREP ne gèrent pas les unités
 * de mesure explicitement. Les paramètres comme xstep.cascade.unit ou
 * write.step.unit n'ont donc aucun effet lors de la lecture ou l'écriture
 * de fichiers BREP
 */
class ImportBREPImplementation: public GeomImport {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     */
    ImportBREPImplementation(Internal::Context& c, Internal::InfoCommand* icmd,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ImportBREPImplementation();

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Réalise l'ouverture du fichier géométrique au format
     *          souhaité
     */
    virtual void readFile();

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* IMPORTBREPIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/


