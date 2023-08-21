/*----------------------------------------------------------------------------*/
/*
 * GeomImport.h
 *
 *  Created on: 30 oct. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMIMPORT_H_
#define GEOMIMPORT_H_
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
class InfoCommand;
}
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Volume;
class Surface;
class Curve;
class Vertex;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomImport
 * \brief Classe fournissant les services communs aux imports géométriques
 */
class GeomImport: public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     *  \param onlySolidsAndFaces indique que l'on n'importe pas les sommets et
     *                            courbes libres.
     */
    GeomImport(Internal::Context& c, Internal::InfoCommand* icmd,
            const std::string& n,
            const bool onlySolidAndFaces = false);
    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomImport();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la translation
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform(){};

    /** active ou non la décompostion des courbes */
    void setSplitCompoundCurves(bool flag)
    	{ m_splitCompondCurves = flag; }

    /** active ou non le test comme quoi les volumes sont fermés */
    void setTestVolumicProperties(bool flag)
    	{ m_testVolumicProperties = flag; }

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Crée une nouvelle entité géométrique M3D à partir d'une shape
     *          OCC et d'un nom
     */
    void add(TopoDS_Shape& AShape, const std::string& AName);

    /*------------------------------------------------------------------------*/
    /** \brief  ajoute à un groupe un volume
     */
    virtual void addToGroup(Volume* v, const std::string& AName);

    /** \brief  ajoute à un groupe une surface
     */
    virtual void addToGroup(Surface* s, const std::string& AName);

    /** \brief  ajoute à un groupe une courbe
     */
    virtual void addToGroup(Curve* c, const std::string& AName);

    /** \brief  ajoute à un groupe un sommet
     */
    virtual void addToGroup(Vertex* v, const std::string& AName);

    /*------------------------------------------------------------------------*/
    /** \brief  stockage dans le manager géométrique pour un volume
     *
     *          Attention pour tous les stockages, on ne vérifie pas que l'
     *          élément n'est pas déjà présent et stocké dans le manager
     */
    virtual void store(Volume* v);

    /** \brief  stockage dans le manager géométrique pour une surface
     */
    virtual void store(Surface* s);

    /** \brief  stockage dans le manager géométrique pour une courbe
     */
    virtual void store(Curve* c);

    /** \brief  stockage dans le manager géométrique pour un sommet
     */
    virtual void store(Vertex* v);

    /*------------------------------------------------------------------------*/
    /** \brief  Réalise l'ouverture du fichier géométrique au format
     *          souhaité
     */
    virtual void readFile() = 0;

    /** identification des sommets aux extrémités  */
    virtual void splitManyCurves(std::vector<Curve*>& curvs, std::vector<Vertex*>& verts);

protected:

    /** Informations sur la commande: entités modifiées */
    Internal::InfoCommand* m_icmd;

    /// le nom du fichier en entier
    std::string m_filename;

    /// le nom du fichier sans chemin ni extension
    std::string m_shortfilename;

    /// les entités importées
    std::vector<TopoDS_Shape> m_importedShapes;

    // flags de gestion des entités importées
    bool m_onlySolidsAndFaces;

    /// flag suivant qu'il est nécessaire ou non de tester que les volumes sont fermés
    bool m_testVolumicProperties;

    /// flag suivant que l'on souhaite décomposer les courbes composées en de multiples courbes
    bool m_splitCompondCurves;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMIMPORT_H_ */
/*----------------------------------------------------------------------------*/



