/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/*
 * ImportMDL2Commandes.h
 *
 *  Created on: 8/7/16
 *      Author: Eric Brière de l'Isle
 */
/*----------------------------------------------------------------------------*/
#ifndef IMPORTMDL2COMMANDES_H_
#define IMPORTMDL2COMMANDES_H_
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
/**
 * \class ImportMDL2Commandes
 * \brief Classe réalisant l'import du contenu d'un fichier de modélisation au
 *        format MDL, création de la géométrie et de la topologie
 *        Création d'une commande par entité
 */
class ImportMDL2Commandes : public ImportMDLItf{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param n le nom du fichier à importer
     *  \param withTopo à vrai si l'on veut importer toutes la topologie en plus de la géométrie
     */
    ImportMDL2Commandes(Internal::Context& c, const std::string& n, const bool withTopo);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ImportMDL2Commandes();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'importation de la géométrie + topologie suivant option
     */
    void perform();

protected:

    void performGeom();
    void performTopo();

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

    /// met les noms des cofaces à jour pour les méthodes interpolées
    void MAJInterpolated(std::vector<Topo::CoEdge*>& coedges, std::string coface);

private:

    /** option pour importer ou non la topologie
     */
    bool m_withTopo;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* IMPORTMDL2COMMANDES_H_ */
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/


