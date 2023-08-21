/*----------------------------------------------------------------------------*/
/*
 * \file MeshManager.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_MESHMANAGER_H_
#define MGX3D_MESH_MESHMANAGER_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshManagerIfc.h"
#include "Utils/Container.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
class Entity;
}

namespace Internal {
class Context;
class InfoCommand;
}
namespace Topo {
class Block;
class CoFace;
class CoEdge;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {

class MeshItf;
class Cloud;
class Surface;
class Volume;
class SubVolume;

class CommandMeshExplorer;

/*----------------------------------------------------------------------------*/
/**
 * \class MeshManager
 * \brief Gestionnaire des opérations effectuées au niveau du module
 *        de maillage. Le gestionnaire de maillage est l'objet qui stocke
 *        un lien vers une interface avec le maillage (GMDS) pour une session donnée.
 *
 */
class MeshManager: public Mesh::MeshManagerIfc {
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param		Nom unique de l'instance (utile en environnement distribué).
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
    MeshManager(const std::string& name, Internal::Context* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur */
    virtual ~MeshManager();

	/*------------------------------------------------------------------------*/
	/** Réinitialisation     */
	virtual void clear();

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le maillage
    virtual MeshItf* getMesh() {return m_mesh_itf;}

    /*------------------------------------------------------------------------*/
    /// Lecture d'un maillage au format lima (mli)
    virtual void readMli(std::string nom, std::string prefix);

    /*------------------------------------------------------------------------*/
    /// Sauvegarde d'un maillage au format lima (mli)
    virtual void writeMli(std::string nom);

    /*------------------------------------------------------------------------*/
    /// Sauvegarde d'un maillage au format vtk (vtk vtp vtu)
    virtual void writeVTK(std::string nom);

    /*------------------------------------------------------------------------*/
    /// Sauvegarde d'un maillage au format CGNS
    virtual void writeCGNS(std::string nom);

    /*------------------------------------------------------------------------*/
    /// Lissage du maillage
    virtual void smooth();

    /*------------------------------------------------------------------------*/
    /// Compare le maillage actuel avec un maillage sur disque, return true si ok
    virtual bool compareWithMesh(std::string nom);

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la strategie
    virtual strategy getStrategy() {return m_strategy;}
    /// Modificateur de strategie
    virtual void setStrategy(const strategy& st) {m_strategy = st;}

    /*------------------------------------------------------------------------*/
    /** \brief  Génère le maillage pour une liste de blocs */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newBlocksMesh(std::vector<std::string>& blocks_name);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newBlocksMesh(std::vector<Mgx3D::Topo::Block*>& blocks);

    /** \brief  Génère le maillage pour une liste de faces (communes) */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newFacesMesh(std::vector<std::string>& faces_name);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newFacesMesh(std::vector<Mgx3D::Topo::CoFace*>& faces);

    /*------------------------------------------------------------------------*/
    /** \brief  Génère le maillage pour l'ensemble des blocs */
    virtual Mgx3D::Internal::M3DCommandResultIfc* newAllBlocksMesh();

    /** \brief  Génère le maillage pour l'ensemble des faces (communes) */
    virtual Mgx3D::Internal::M3DCommandResultIfc* newAllFacesMesh();

    /*------------------------------------------------------------------------*/
    /** \brief  Construction d'un sous-ensemble d'un volume
     *  constitué des mailles entre 2 feuillets (indices de pos1 à pos2)
     *  \param blocks_name les blocs sur lesquels se restreint la constructions
     *    ces blocs doivent être maillés et structurés
     *  \param narete le nom de l'arête par laquelle passe le feuillet
     *  \param pos1 un indice entre 1 et nombre de bras de cette arête
     *  \param pos2 idem pos1
     *  \param groupName nom du groupe à créer
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	newSubVolumeBetweenSheets(std::vector<std::string>& blocks_name, std::string narete,
			int pos1, int pos2, std::string groupName);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
	newSubVolumeBetweenSheets(std::vector<Mgx3D::Topo::Block*>& blocks, Topo::CoEdge* coedge,
			int pos1, int pos2, std::string groupName);

    /*------------------------------------------------------------------------*/
    /** Construit des sous-volumes pour explorer le maillage volumique
     *  On renseigne sur l'exporateur précédent s'il y a lieu avec un incrément
     *  pour se décaler dans la vue
	 *  Si asCommand vaut true la commande est gérée par le undo/redo manager,
	 *  est scriptable et est exécutée. Dans le cas contraire elle n'est pas
	 *  gérée par le undo/redo manager, n'est pas scriptable et n'est pas
	 *  exécutée (juste créée).
     */
    virtual CommandMeshExplorer* newExplorer(CommandMeshExplorer* oldExplo, int inc, std::string narete, bool asCommand);

    /** Terminaison de l'exploration
     *  On renseigne sur l'exporateur précédent
	 *  Si asCommand vaut true la commande est gérée par le undo/redo manager,
	 *  est scriptable et est exécutée. Dans le cas contraire elle n'est pas
	 *  gérée par le undo/redo manager, n'est pas scriptable et n'est pas
	 *  exécutée (juste créée).
     */
    virtual CommandMeshExplorer* endExplorer(CommandMeshExplorer* oldExplo, bool asCommand);

    /*------------------------------------------------------------------------*/
    virtual int getNbClouds(bool onlyVisible) const;
    virtual int getNbSurfaces(bool onlyVisible) const;
    virtual int getNbVolumes(bool onlyVisible) const;

    virtual int getNbNodes();
    virtual int getNbFaces();
    virtual int getNbRegions();

    /** Retourne une string avec les informations relatives à l'entité */
    virtual std::string getInfos(const std::string& name, int dim) const;

    virtual std::string getInfos(const Cloud* me) const;
    virtual std::string getInfos(const Surface* me) const;
    virtual std::string getInfos(const Volume* me) const;

    /*------------------------------------------------------------------------*/
    /** Ajoute un Nuage au manager */
    virtual void add(Cloud* cl);

    /** Enlève un Nuage au manager */
    virtual void remove(Cloud* cl);

    /** Retourne le Nuage suivant le nom en argument */
    virtual Cloud* getCloud(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne les Nuages */
    virtual void getClouds(std::vector<Cloud*>& AClouds) const;

    /*------------------------------------------------------------------------*/
    /** Ajoute une Ligne au manager */
    virtual void add(Line* ln);

    /** Enlève une Line au manager */
    virtual void remove(Line* ln);

    /** Retourne la Ligne suivant le nom en argument */
    virtual Line* getLine(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne les Lignes */
    virtual void getLines(std::vector<Line*>& ALines) const;

    /*------------------------------------------------------------------------*/
    /** Ajoute une Surface au manager */
    virtual void add(Surface* sf);

    /** Enlève une Surface au manager */
    virtual void remove(Surface* sf);

    /** Retourne la Surface suivant le nom en argument */
    virtual Surface* getSurface(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne les Surfaces */
    virtual void getSurfaces(std::vector<Surface*>& ASurfaces) const;

    /*------------------------------------------------------------------------*/
    /** Ajoute un Volume au manager */
    virtual void add(Volume* vo);

    /** Enlève un Volume au manager */
    virtual void remove(Volume* vo);

    /** Retourne le Volume suivant le nom en argument */
    virtual Volume* getVolume(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne les Volumes */
    virtual void getVolumes(std::vector<Volume*>& AVolumes) const;

    /** Création d'un sous-volume ou réutilisation d'un existant */
    virtual SubVolume* getNewSubVolume(const std::string& gr_name, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /// accès à m_coface_allways_in_groups
    bool coFaceAllwaysInGroups() const {return m_coface_allways_in_groups;}
    /// modif de m_coface_allways_in_groups
    void setCoFaceAllwaysInGroups(bool vis) {m_coface_allways_in_groups = vis;}


private:
    /// Lien sur la structure de maillage et ses algos
    MeshItf* m_mesh_itf;

    // stockage des groupes de noeuds
    Utils::Container<Cloud> m_clouds;

    // stockage des groupes de lignes
    Utils::Container<Line> m_lines;

    // stockage des groupes de polygones
    Utils::Container<Surface> m_surfaces;

    // stockage des groupes de polyèdres
    Utils::Container<Volume> m_volumes;


    /// Stategie pour la conservation des créations et modifications de maillage
    strategy m_strategy;

    /** permet de mettre les polygones des cofaces dans "Hors Groupe 2D" pour les rendre visible
     * même dans le cas où ces cofaces ne sont pas associées à de la géométrie
     */
    bool m_coface_allways_in_groups;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_MESH_MESHMANAGER_H_ */
