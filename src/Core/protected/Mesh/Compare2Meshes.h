/*----------------------------------------------------------------------------*/
/*
 * \file Compare2Meshes.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15 juin 2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMPARE2MESHES_H_
#define COMPARE2MESHES_H_
/*----------------------------------------------------------------------------*/
#include "GMDS/IG/IGMesh.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class Compare2Meshes
 *  \brief Compare deux maillages
 *
 *  Sont comparés:
 *    les quantités (nombres de noeuds, de bras, ... de groupes ...)
 *    les coordonées des noeuds
 *    les id des polygones et polyèdres (on ne fait rien pour les bras, car pas construit par Magix3D)
 *    les groupes (existance), leur contenu (id des noeuds, polygones et polyèdres) (rien pour les lignes)
 */
/*----------------------------------------------------------------------------*/
class Compare2Meshes {
public:
    /// constructeur avec 2 maillages chargés
    Compare2Meshes(Internal::Context* context, gmds::IGMesh& mesh1, gmds::IGMesh& mesh2);

    ~Compare2Meshes() {}

    /// effectue la comparaison et retourne true si tout est ok
    bool perform();

    /** retourne le contexte */
    Internal::Context& getContext() {return *(m_context);}

private:
    /** Compare le contenu de 2 listes et donne en retour la partie commune,
     * ainsi que ce qu'il y a en plus dans la première et dans la deuxième
     */
    void diff(std::vector<std::string> &liste1, std::vector<std::string> &liste2,
            std::vector<std::string> &common,
            std::vector<std::string> &add1, std::vector<std::string> &add2);

    /** le contexte */
    Internal::Context* m_context;

    gmds::IGMesh& m_gmds_mesh1;
    gmds::IGMesh& m_gmds_mesh2;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* COMPARE2MESHES_H_ */
