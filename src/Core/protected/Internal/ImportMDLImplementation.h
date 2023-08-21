/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/*
 * ImportMDLImplementation.h
 *
 *  Created on: 10 mai 2012
 *      Author: Eric Brière de l'Isle
 */
/*----------------------------------------------------------------------------*/
#ifndef IMPORTMDLIMPLEMENTATION_H_
#define IMPORTMDLIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
#include "Internal/ImportMDLItf.h"
#include "Utils/Unit.h"
/*----------------------------------------------------------------------------*/
#include "Mdl.h"
/*----------------------------------------------------------------------------*/
#include <string>
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class Volume;
class Surface;
class Curve;
class Vertex;
}
namespace Topo {
class Face;
class CoFace;
class Edge;
class CoEdge;
class Vertex;
}
namespace Group {
class GroupEntity;
}
namespace Internal {
/*----------------------------------------------------------------------------*/
class InfoCommand;
/*----------------------------------------------------------------------------*/
/**
 * \class ImportMDLImplementation
 * \brief Classe réalisant l'import du contenu d'un fichier de modélisation au
 *        format MDL, création de la géométrie et de la topologie
 */
class ImportMDLImplementation : public ImportMDLItf{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     *  \param all à vrai si l'on veut importer toutes les entités, faux si l'on ne prend que ce qui est utile au 2D
     *  \param useAreaName vrai si l'on souhaite utiliser les noms des zones comme nom de groupes
     *  \param deg_min degré minimum pour le polynome de la B-Spline
     *  \param deg_max degré maximum pour le polynome de la B-Spline
     */
    ImportMDLImplementation(Internal::Context& c, const std::string& n, const bool all,
    		const bool useAreaName=false, int deg_min=1, int deg_max=2);

    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     *  \param groupe le nom du groupe dont on importe les zones et les entités associées
     */
    ImportMDLImplementation(Internal::Context& c, const std::string& n, std::string groupe);

    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     *  \param zones la liste des zones que l'on importe avec les entités associées
     */
    ImportMDLImplementation(Internal::Context& c, const std::string& n, std::vector<std::string>& zones);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ImportMDLImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'importation de la géométrie
     */
    void performGeom(Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'importation de la topologie
     */
    void performTopo(Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les nouvelles entités géométriques.
     *           N'a de sens qu'une fois l'opération performGeom() appelée.
     */
    std::vector<Geom::GeomEntity*>& getNewGeomEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les nouvelles entités topologiques.
     *           N'a de sens qu'une fois l'opération performTopo() appelée.
     */
    std::vector<Topo::TopoEntity*>& getNewTopoEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  spécifie un préfixe pour les noms de groupes
     */
    void setPrefix(const std::string& prefixName)
    {m_prefixName = prefixName;}

protected:

    /// retourne le Geom::Vertex suivant l'indice du sommet dans le modèle, le créé si nécessaire
    Geom::Vertex* getVertex(uint ptId);

    /// retourne la courbe suivant le nom du contour, la créée si nécessaire
    Geom::Curve* getCurve(const std::string name);

    /// retourne le Topo::Vertex suivant l'indice du sommet dans le modèle, le créé si nécessaire
    Topo::Vertex* getTopoVertex(uint ptId);

    /// retourne et construit si ce n'est pas conforme ou si ce n'est pas déjà fait la coedge pour un contour donné
    Topo::CoEdge* getCoEdge(T_MdlCutData *mcd, bool isConforme);

    /// recherche les noms des arêtes sur un côté logique d'une zone entre 2 positions en nombre de bras
    std::vector<std::string> getCoEdges(const T_MdlCommand& current_command,
    		std::map <std::string, T_MdlCutData*>& cor_model1d_MdlCutData,
			uint id, uint nb0, uint nb1);

    /** Séparation des CoFaces (duplique les sommets) pour être réellement non-conforme
    *  et cela pour permettre le glissement.
    */
    void dupVertexNonConformalRelation(std::vector<Topo::CoFace* >& cofaces);

    /** Mise en place d'un filtre sur les zones
     * Les zones acceptées sont mise dans le set
     */
    bool getFilter(std::set<std::string>& filter);

    /// mise à jour des interpolations / coface
    void MAJInterpolated(std::vector<Topo::CoEdge*>& coedges, Topo::CoFace* coface);

private:

    /** entités géométriques que l'on a ajoutées */
    std::vector<Geom::GeomEntity*> m_newGeomEntities;

    /** entités topologiques que l'on a ajoutées */
    std::vector<Topo::TopoEntity*> m_newTopoEntities;

    /** Option pour importer toutes les entités de modélisation
     *  ou seulement celles utiles à la construction du modèle 2D
     *
     *  Il peut être utile d'importer tout pour y récupérer un tracé de l'ogrid par exemple
     */
    bool m_importAll;

    /// utilisation ou non des noms des zones (sinon on utilise uniquement les groupes)
    bool m_useAreaName;

    /** stockage des info pour la commande en cours */
    Internal::InfoCommand* m_icmd;

    /// nom du groupe dont on importe les zones et les entités associées
    std::string m_groupe;

    /// liste des zones que l'on importe avec les entités associées
    std::vector<std::string> m_zones;

    /// préfixe pour les noms des groupes
    std::string m_prefixName;

    /// degré min pour les b-splines
    int m_deg_min;
    /// degré max pour les b-splines
    int m_deg_max;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* IMPORTMDLIMPLEMENTATION_H_ */
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/


