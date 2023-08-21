/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/*
 * ImportMDLItf.h
 *
 *  Created on: 8/7/16
 *      Author: Eric Brière de l'Isle
 */
/*----------------------------------------------------------------------------*/
#ifndef IMPORTMDLITF_H_
#define IMPORTMDLITF_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
#include "Utils/Unit.h"
/*----------------------------------------------------------------------------*/
#include "Mdl.h"
/*----------------------------------------------------------------------------*/
#include <string>
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
 * \class ImportMDLItf
 * \brief Classe réalisant l'import du contenu d'un fichier de modélisation au
 *        format MDL, création de la géométrie et de la topologie
 */
class ImportMDLItf{

public:

    /*------------------------------------------------------------------------*/
    /** retourne l'unité de longueur trouvée dans le fichier de modélisation */
    Utils::Unit::lengthUnit getLengthUnit();

    /*------------------------------------------------------------------------*/
    /** Spécifie le facteur d'homothétie pour mettre dans l'unité de longueur désirée */
    void setScale(const double& factor);

protected:

    /// Constructeur
    ImportMDLItf(Internal::Context& c,
            const std::string& n);

    ImportMDLItf(const ImportMDLItf&)
    : m_context(*new Context("Interdit")), m_filename(0), m_mi(0), m_mdl_info(0), m_do_scale(false), m_scale_factor(1.0)
    {
        MGX_FORBIDDEN("ImportMDLItf::ImportMDLItf is not allowed.");
    }

     ImportMDLItf& operator = (const ImportMDLItf&)
     {
         MGX_FORBIDDEN("ImportMDLItf::operator = is not allowed.");
         return *this;
     }
     virtual ~ImportMDLItf();

    /// démarre la lecture du fichier
    void readMDL();

protected:
    /** contexte d'exécution*/
    Internal::Context& m_context;

    /** fichier à importer */
    std::string m_filename;

    /// lien sur le fichier ouvert
    MdlInfo m_mi;
    /// la structure du modèle lu
    T_MdlInfo* m_mdl_info;

    /// vecteur de toutes les commandes de points
    std::vector<T_MdlCommand*> m_vCmdPt;
    /// vecteur de toutes les commandes de supports
    std::vector<const T_MdlCommand*> m_vCmdSup;
    /// vecteur de toutes les commandes de zones
    std::vector<const T_MdlCommand*> m_vCmdArea;

    /// lien entre les indices de sommet du modèle et le Geom::Vertex correspondant
    std::map<uint, Geom::Vertex*> m_cor_ptId_vertex;

    /// lien entre un nom de contour et la Curve correspondante
    std::map<std::string, Geom::Curve*> m_cor_name_curve;

    /// lien entre un nom de zone et la Surface correspondante
    std::map<std::string, Geom::Surface*> m_cor_name_surface;

    /// lien entre un nom de contour et la commande Mdl (permet d'accélérer les recherches de contours)
    std::map<std::string, T_MdlCommand*> m_cor_name_mdlCmd;

    /// marqueur des Support de ligne utilisé
    std::map<std::string, uint> m_filtre_support;

    /// lien entre les indices de sommet du modèle et le Topo::Vertex correspondant
    std::map<uint, Topo::Vertex*> m_cor_ptId_topoVertex;

    /// lien entre un nom de contour et l'arête commune associée
    std::map<std::string, Topo::CoEdge*> m_cor_name_coedge;

    /// vrai si on met à l'échelle métrique, faux si on laisse tel quel
    bool m_do_scale;

    /// facteur de mise à l'échelle
    double m_scale_factor;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* IMPORTMDLITF_H_ */
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/


