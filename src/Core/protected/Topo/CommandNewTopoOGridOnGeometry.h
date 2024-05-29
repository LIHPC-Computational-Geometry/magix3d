/*----------------------------------------------------------------------------*/
/** \file CommandNewTopoOGridOnGeometry.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/1/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWTOPOOGRIDONGEOMETRY_H_
#define COMMANDNEWTOPOOGRIDONGEOMETRY_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandCreateTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class CommandCreateGeom;
class PropertyCylinder;
class PropertySphere;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandNewTopoOGridOnGeometry
 *  \brief Commande permettant de créer une topologie en o-grid pour une géométrie du type cylindre
 */
/*----------------------------------------------------------------------------*/
class CommandNewTopoOGridOnGeometry: public Topo::CommandCreateTopo{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param entity un Volume
     *  \param rat le ratio de la distance entre l'axe du cylindre et son rayon pour placer le sommet du o-grid
     */
    CommandNewTopoOGridOnGeometry(Internal::Context& c, Geom::GeomEntity* entity, const double& rat);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param command_geom une commande de création d'entité géométrique
     *  \param rat le ratio de la distance entre l'axe du cylindre et son rayon pour placer le sommet du o-grid
     */
    CommandNewTopoOGridOnGeometry(Internal::Context& c, Geom::CommandCreateGeom* command_geom, const double& rat);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewTopoOGridOnGeometry();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


    /*------------------------------------------------------------------------*/
    void setNaxe(const int naxe)
    {m_naxe = naxe;}

    void setNi(const int ni)
    {m_ni = ni;}

    void setNr(const int nr)
    {m_nr = nr;}

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:

    /** Méthode de vérification et validation de l'entité présente */
    virtual void validGeomEntity();

    /// Création de la topologie avec un unique bloc pour le cylindre
    void createCylinderTopo1Block();

    /// Création de la topologie avec un unique bloc pour le cylindre complet
    void createCylinderTopo1BlockPlein(Geom::PropertyCylinder* propertyCyl);

    /// Création de la topologie avec un unique bloc pour le demi cylindre
    void createCylinderTopo1BlockDemi(Geom::PropertyCylinder* propertyCyl);

    /// Création de la topologie avec un unique bloc pour le quart de cylindre
    void createCylinderTopo1BlockQuart(Geom::PropertyCylinder* propertyCyl);


    /// Création de la topologie en o-grid pour le cylindre
    void createCylinderTopoOGrid();

    /// Création de la topologie en o-grid pour le cylindre complet avec bloc au centre
    void createCylinderTopoOGridPleinNonDeg(Geom::PropertyCylinder* propertyCyl);
    /// Création de la topologie en o-grid pour le cylindre complet sans bloc au centre (cas dégénéré)
    void createCylinderTopoOGridPleinDeg(Geom::PropertyCylinder* propertyCyl);

    /// Création de la topologie en o-grid pour le demi cylindre avec bloc au centre
    void createCylinderTopoOGridDemiNonDeg(Geom::PropertyCylinder* propertyCyl);

    /// Création de la topologie pour le cylindre incomplet sans bloc au centre (cas dégénéré)
    void createCylinderTopoOGridQuartDeg(Geom::PropertyCylinder* propertyCyl);

    /// Création de la topologie en o-grid pour le quart de cylindre avec bloc au centre
    void createCylinderTopoOGridQuartNonDeg(Geom::PropertyCylinder* propertyCyl);



    /// Création de la topologie avec un unique bloc pour la sphère
    void createSphereTopo1Block();

    /// Création de la topologie avec un unique bloc pour la sphère complète
    void createSphereTopo1BlockPlein(Geom::PropertySphere* propertySph);

    /// Création de la topologie avec un unique bloc pour la demi sphère
    void createSphereTopo1BlockDemi(Geom::PropertySphere* propertySph);

    /// Création de la topologie avec un unique bloc pour le quart de sphère
    void createSphereTopo1BlockQuart(Geom::PropertySphere* propertySph);

    /// Création de la topologie avec un unique bloc pour le huitième de sphère
    void createSphereTopo1BlockHuitieme(Geom::PropertySphere* propertySph);


    /// Création de la topologie en o-grid pour le cylindre
    void createSphereTopoOGrid();

    /// Création de la topologie en o-grid pour la sphère complète sans bloc au centre (cas dégénéré)
    void createSphereTopoOGridPleinDeg(Geom::PropertySphere* propertySph);

    /// Création de la topologie en o-grid pour la sphère complète avec bloc au centre
    void createSphereTopoOGridPleinNonDeg(Geom::PropertySphere* propertySph);

    /// Création de la topologie en o-grid pour la demi sphère avec bloc au centre
    void createSphereTopoOGridDemiDeg(Geom::PropertySphere* propertySph);

    /// Création de la topologie en o-grid pour le demi sphère avec bloc au centre
    void createSphereTopoOGridDemiNonDeg(Geom::PropertySphere* propertySph);

    /// Création de la topologie pour le quart de sphère incomplète sans bloc au centre (cas dégénéré)
    void createSphereTopoOGridQuartDeg(Geom::PropertySphere* propertySph);

    /// Création de la topologie en o-grid pour le quart de sphère avec bloc au centre
    void createSphereTopoOGridQuartNonDeg(Geom::PropertySphere* propertySph);

    /// Création de la topologie pour le huitième de sphère incomplète sans bloc au centre (cas dégénéré)
    void createSphereTopoOGridHuitiemeDeg(Geom::PropertySphere* propertySph);

    /// Création de la topologie en o-grid pour le huitième de sphère avec bloc au centre
    void createSphereTopoOGridHuitiemeNonDeg(Geom::PropertySphere* propertySph);


    /// Création de la topologie pour le cylindre creux
    void createHollowCylinderTopo();

    /// Création de la topologie en o-grid pour le quart de cylindre creux
    void createHollowCylinderTopoQuart(Geom::PropertyHollowCylinder* propertyCyl);

    /// Création de la topologie en o-grid pour le demi de cylindre creux
    void createHollowCylinderTopoDemi(Geom::PropertyHollowCylinder* propertyCyl);

    /// Création de la topologie en o-grid pour le de cylindre creux complet
    void createHollowCylinderTopoEntier(Geom::PropertyHollowCylinder* propertyCyl);


    /// Création de la topologie pour la sphère creuse
    void createHollowSphereTopo();

    /// Création de la topologie en o-grid pour le huitième de sphère creuse
    void createHollowSphereTopoHuitieme(Geom::PropertyHollowSphere* propertySph);

    /// Création de la topologie en o-grid pour le quart de sphère creuse
    void createHollowSphereTopoQuart(Geom::PropertyHollowSphere* propertySph);

    /// Création de la topologie en o-grid pour le demi de sphère creuse
    void createHollowSphereTopoDemi(Geom::PropertyHollowSphere* propertySph);

    /// Création de la topologie en o-grid pour le de sphère creuse complète
    void createHollowSphereTopoEntier(Geom::PropertyHollowSphere* propertySph);

    /* [EB] NB : les fonctions snapVertices, projectEdgesOnCurves et projectFacesOnSurfaces
     * n'ont été mise en place que lors de la création des objets creux (cylindre et sphère)
     * Aussi est-ce pour cela que cela n'a pas été utilisé pour les objets pleins
    */
    /// Déplace les sommets suivant leur association
    void snapVertices();

    /// Associe les arêtes aux courbes suivant les associations aux sommets
    void projectEdgesOnCurves();

    /// Associe les faces aux surfaces suivant les associations aux arêtes
    void projectFacesOnSurfaces();


    /// Création de la topologie avec un unique bloc pour le cône
    void createConeTopo1Block();
    /// Création de la topologie avec un unique bloc pour le quart de cône, cas avec premier rayon nul
    void createConeTopo1BlockQuartR0();
    /// Création de la topologie avec un unique bloc pour le quart de cône
    void createConeTopo1BlockQuart();
    /// Création de la topologie avec un unique bloc pour le demi cône, cas avec premier rayon nul
    void createConeTopo1BlockDemiR0();
    /// Création de la topologie avec un unique bloc pour le demi cône
    void createConeTopo1BlockDemi();
    /// Création de la topologie avec un unique bloc pour le cône complet, cas avec premier rayon nul
    void createConeTopo1BlockPleinR0();
    /// Création de la topologie avec un unique bloc pour le cône complet
    void createConeTopo1BlockPlein();

    /// Création de la topologie en o-grid pour le cône
    void createConeTopoOGrid();
    /// Création de la topologie en o-grid pour le quart de cône, cas avec premier rayon nul, sans bloc au centre (cas dégénéré)
    void createConeTopoOGridQuartDegR0();
    /// Création de la topologie en o-grid pour le quart de cône, cas avec premier rayon nul, avec un bloc au centre
    void createConeTopoOGridQuartNonDegR0();
    /// Création de la topologie en o-grid pour le quart de cône, sans bloc au centre (cas dégénéré)
    void createConeTopoOGridQuartDeg();
    /// Création de la topologie en o-grid pour le quart de cône, avec un bloc au centre
    void createConeTopoOGridQuartNonDeg();
    /// Création de la topologie en o-grid pour le demi cône, cas avec premier rayon nul, sans bloc au centre (cas dégénéré)
    void createConeTopoOGridDemiDegR0();
    /// Création de la topologie en o-grid pour le demi cône, cas avec premier rayon nul, avec un bloc au centre
    void createConeTopoOGridDemiNonDegR0();
    /// Création de la topologie en o-grid pour le demi cône, sans bloc au centre (cas dégénéré)
    void createConeTopoOGridDemiDeg();
    /// Création de la topologie en o-grid pour le demi cône, avec un bloc au centre
    void createConeTopoOGridDemiNonDeg();
    /// Création de la topologie en o-grid pour le cône complet, cas avec premier rayon nul, sans bloc au centre (cas dégénéré)
    void createConeTopoOGridPleinDegR0();
    /// Création de la topologie en o-grid pour le cône complet, cas avec premier rayon nul, avec un bloc au centre
    void createConeTopoOGridPleinNonDegR0();
    /// Création de la topologie en o-grid pour le cône complet, sans bloc au centre (cas dégénéré)
    void createConeTopoOGridPleinDeg();
    /// Création de la topologie en o-grid pour le cône complet, avec un bloc au centre
    void createConeTopoOGridPleinNonDeg();


    /*------------------------------------------------------------------------*/

private:

    /// ratio pour placer le sommet du bloc pour le o-grid central
    double m_ratio;

    /// le nombre de bras suivant l'axe du cylindre
    int m_naxe;

    /// le nombre de bras entre le point triple et le plan de symétrie
    int m_ni;

    /// le nombre de couches entre le bloc central et la surface externe
    int m_nr;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWTOPOOGRIDONGEOMETRY_H_ */
/*----------------------------------------------------------------------------*/

