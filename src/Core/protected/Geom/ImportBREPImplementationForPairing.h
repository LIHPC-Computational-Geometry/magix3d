#ifndef IMPORTBREPIMPLEMENTATION_H_
#define IMPORTBREPIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomModificationBaseClass.h"
#include "Group/GroupHelperForCommand.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Compound.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class ImportBREPImplementationForPairing
 * \brief Classe réalisant l'import du contenu d'un fichier de modélisation au
 *        format BREP (format Open Cascade).
 *
 * Contrairement aux fichiers STEP, les fichiers BREP ne gèrent pas les unités
 * de mesure explicitement. Les paramètres comme xstep.cascade.unit ou
 * write.step.unit n'ont donc aucun effet lors de la lecture ou l'écriture
 * de fichiers BREP
 */
class ImportBREPImplementationForPairing: public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     */
    ImportBREPImplementationForPairing(Internal::Context& c, Internal::InfoCommand* icmd,
            const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ImportBREPImplementationForPairing();

        /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la translation
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform(){};

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Réalise l'ouverture du fichier géométrique au format
     *          souhaité
     */
    virtual void readFile();

    static TopoDS_Shape mergeVerticesSafe(const TopoDS_Shape& shape, double tol);
    static TopoDS_Shape removeDegeneratedEdges(const TopoDS_Shape& shape);
    static TopoDS_Shape fixShape(const TopoDS_Shape& shape);
    static TopoDS_Shape cleanShape(const TopoDS_Shape& shape);

    virtual void store(Volume* v);
    virtual void store(Surface* s);
    virtual void store(Curve* c);
    virtual void store(Vertex* v);

    /// helper pour la gestion des groupes dans les commandes
    Group::GroupHelperForCommand m_group_helper;

    /// Informations sur la commande: entités modifiées
    Internal::InfoCommand* m_icmd;

    /// le nom du fichier en entier
    std::string m_filename;

    /// les entités importées
    TopoDS_Shape m_importedShape;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* IMPORTBREPIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/


