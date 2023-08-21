/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/*
 * ExportMDLImplementation.h
 *
 *  Created on: 28/6/2013
 *      Author: Eric Brière de l'Isle
 */
/*----------------------------------------------------------------------------*/
#ifndef EXPORTMDLIMPLEMENTATION_H_
#define EXPORTMDLIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Utils/Point.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <string>
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Mdl.h"
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
 * \class ExportMDLImplementation
 * \brief Classe réalisant l'exportation d'une sélection d'entités géométriques
 *        dans un fichier de modélisation au format MDL
 */
class ExportMDLImplementation{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte
     *  \param ge les entités géométriques à exporter
     *  \param n le nom du fichier dans lequel se fait l'exportation
     */
    ExportMDLImplementation(Internal::Context& c, std::vector<Geom::GeomEntity*>& ge, const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~ExportMDLImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'exportation
     */
    void perform(Internal::InfoCommand* icmd);

private:
    /// les différents types comme dans Magix (2D)
    enum
    {
      IdObPoint, IdObSegment, IdObCurve, IdObLine, IdObLineSup, IdObArea, IdObPerturbation,
      IdObNode, IdObArm, IdObMesh, IdObNodeGroup, IdObArmGroup, IdObMeshGroup,
      IdObLast
    };


    /*------------------------------------------------------------------------*/
    /** \brief  ajoute une commande MDL au conteneur et retourne cette première
     *
     * Attention à ne pas refaire appel à cette méthode lorsque l'on a pas fini d'utiliser la référence en retour.
     */
    T_MdlCommand& nextCommand (std::string nm, int t, size_t id);

    /// sauvegarde d'un sommet dans la structure MDL
    uint save(Geom::Vertex* ve);
    /// sauvegarde d'un point dans la structure MDL
    uint save(Utils::Math::Point pt, std::string& nom);

    /// sauvegarde d'une courbe dans la structure MDL
    uint save(Geom::Curve* cv);

    /// sauvegarde d'une surface dans la structure MDL
    uint save(Geom::Surface* sf);


private:

    /// contexte d'exécution
    Internal::Context& m_context;

    /// fichier dans lequel on exporte
    std::string m_filename;

    /// entités géométriques que l'on exportent
    std::vector<Geom::GeomEntity*> m_geomEntities;

    /// structure pour stockage du MDL
    T_MdlInfo             mdl_info;

    /// conteneur de commandes MDL
    std::vector<T_MdlCommand>  mdl_cmd_list;

    /// permet à partir d'une entité géométrique de retrouver son id pour l'écrivain MDL
    std::map<uint, uint> m_cor_uniqueId_mdlId;

    /// permet à partir d'une entité géométrique (de type curve) de retrouver son command_id pour l'écrivain MDL
    std::map<uint, uint> m_cor_uniqueId_commandId;

    /// tableau qui permet d'avoir une numérotation par type
    size_t m_obs_id[IdObLast];
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* EXPORTMDLIMPLEMENTATION_H_ */
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/


