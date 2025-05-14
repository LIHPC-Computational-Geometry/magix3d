#ifndef MGX3D_MESH_MESHMANAGER_H_
#define MGX3D_MESH_MESHMANAGER_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandCreator.h"
#include "Utils/Container.h"
#include "Utils/SwigCompletion.h"
/*----------------------------------------------------------------------------*/
#include <string>
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
class Entity;
}

namespace Internal {
class Context;
class InfoCommand;
class M3DCommandResult;
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
class Line;
class CommandMeshExplorer;

/*----------------------------------------------------------------------------*/
/**
 * \class MeshManager
 * \brief Gestionnaire des opérations effectuées au niveau du module
 *        de maillage. Le gestionnaire de maillage est l'objet qui stocke
 *        un lien vers une interface avec le maillage (GMDS) pour une session donnée.
 *
 */
class MeshManager final : public Internal::CommandCreator {
public:

	/*------------------------------------------------------------------------*/
	/** \brief  Strategies possibles pour la création d'un maillage
	 * <P> GRANDCHALLENGE:  Création d'un maillage en une fois, le undo détruit tout le maillage,<BR>
	 *     MODIFIABLE: Création et modification du maillage par partie possible,
	 *    il est mémorisé ce qui a été créé pour pouvoir faire un undo
	 * </P>
	 */
	enum strategy {
		GRANDCHALLENGE,
		MODIFIABLE
	};

	/*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param		Nom unique de l'instance (utile en environnement distribué).
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
    MeshManager(const std::string& name, Internal::Context* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur */
    ~MeshManager();

	/*------------------------------------------------------------------------*/
#ifndef SWIG
	/** Réinitialisation     */
	void clear();

	/*------------------------------------------------------------------------*/
    /// Accesseur sur le maillage
    MeshItf* getMesh() {return m_mesh_itf;}
#endif

    /*------------------------------------------------------------------------*/
    /// Lecture d'un maillage au format lima (mli)
    void readMli(std::string nom, std::string prefix);
	SET_SWIG_COMPLETABLE_METHOD(readMli)

    /*------------------------------------------------------------------------*/
    /// Sauvegarde d'un maillage au format lima (mli)
    void writeMli(std::string nom);
	SET_SWIG_COMPLETABLE_METHOD(writeMli)

    /*------------------------------------------------------------------------*/
    /// Sauvegarde d'un maillage au format vtk (vtk vtp vtu)
    void writeVTK(std::string nom);
	SET_SWIG_COMPLETABLE_METHOD(writeVTK)

    /*------------------------------------------------------------------------*/
    /// Sauvegarde d'un maillage au format CGNS
    void writeCGNS(std::string nom);
	SET_SWIG_COMPLETABLE_METHOD(writeCGNS)

	/*------------------------------------------------------------------------*/
	/** \brief Export dans un fichier au format VTK de la structure de blocs
	 *
	 *  \param n le nom du ficher dans lequel on exporte
	 */
	Mgx3D::Internal::M3DCommandResult* exportBlocksForCGNS(const std::string& n);
	SET_SWIG_COMPLETABLE_METHOD(exportBlocksForCGNS)

	/*------------------------------------------------------------------------*/
    /// Lissage du maillage
    void smooth();

    /*------------------------------------------------------------------------*/
    /// Compare le maillage actuel avec un maillage sur disque, return true si ok
    bool compareWithMesh(std::string nom);
	SET_SWIG_COMPLETABLE_METHOD(compareWithMesh)

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la strategie
    strategy getStrategy() {return m_strategy;}

	/// Modificateur de strategie
    void setStrategy(const strategy& st) {m_strategy = st;}
	SET_SWIG_COMPLETABLE_METHOD(setStrategy)

    /*------------------------------------------------------------------------*/
    /** \brief  Génère le maillage pour une liste de blocs */
    Mgx3D::Internal::M3DCommandResult*
    newBlocksMesh(std::vector<std::string>& blocks_name);
	SET_SWIG_COMPLETABLE_METHOD(newBlocksMesh)

#ifndef	SWIG
    Mgx3D::Internal::M3DCommandResult*
    newBlocksMesh(std::vector<Mgx3D::Topo::Block*>& blocks);
#endif

    /** \brief  Génère le maillage pour une liste de faces (communes) */
    Mgx3D::Internal::M3DCommandResult*
    newFacesMesh(std::vector<std::string>& faces_name);
	SET_SWIG_COMPLETABLE_METHOD(newFacesMesh)

#ifndef	SWIG
    Mgx3D::Internal::M3DCommandResult*
    newFacesMesh(std::vector<Mgx3D::Topo::CoFace*>& faces);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Génère le maillage pour l'ensemble des blocs */
    Mgx3D::Internal::M3DCommandResult* newAllBlocksMesh();

    /** \brief  Génère le maillage pour l'ensemble des faces (communes) */
    Mgx3D::Internal::M3DCommandResult* newAllFacesMesh();

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
    Mgx3D::Internal::M3DCommandResult*
	newSubVolumeBetweenSheets(std::vector<std::string>& blocks_name, std::string narete,
			int pos1, int pos2, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(newSubVolumeBetweenSheets)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
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
    CommandMeshExplorer* newExplorer(CommandMeshExplorer* oldExplo, int inc, std::string narete, bool asCommand);

    /** Terminaison de l'exploration
     *  On renseigne sur l'exporateur précédent
	 *  Si asCommand vaut true la commande est gérée par le undo/redo manager,
	 *  est scriptable et est exécutée. Dans le cas contraire elle n'est pas
	 *  gérée par le undo/redo manager, n'est pas scriptable et n'est pas
	 *  exécutée (juste créée).
     */
    CommandMeshExplorer* endExplorer(CommandMeshExplorer* oldExplo, bool asCommand);
#endif

    /*------------------------------------------------------------------------*/
    int getNbClouds(bool onlyVisible = true) const;
	SET_SWIG_COMPLETABLE_METHOD(getNbClouds)
    int getNbSurfaces(bool onlyVisible = true) const;
	SET_SWIG_COMPLETABLE_METHOD(getNbSurfaces)
    int getNbVolumes(bool onlyVisible = true) const;
	SET_SWIG_COMPLETABLE_METHOD(getNbVolumes)
    int getNbNodes();
    int getNbFaces();
    int getNbRegions();

    /** Retourne une string avec les informations relatives à l'entité */
    std::string getInfos(const std::string& name, int dim) const;
	SET_SWIG_COMPLETABLE_METHOD(getInfos)

#ifndef SWIG
    std::string getInfos(const Cloud* me) const;
    std::string getInfos(const Surface* me) const;
    std::string getInfos(const Volume* me) const;

    /*------------------------------------------------------------------------*/
    /** Ajoute un Nuage au manager */
    void add(Cloud* cl);

    /** Enlève un Nuage au manager */
    void remove(Cloud* cl);

    /** Retourne le Nuage suivant le nom en argument */
    Cloud* getCloud(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne les Nuages */
    void getClouds(std::vector<Cloud*>& AClouds) const;

    /*------------------------------------------------------------------------*/
    /** Ajoute une Ligne au manager */
    void add(Line* ln);

    /** Enlève une Line au manager */
    void remove(Line* ln);

    /** Retourne la Ligne suivant le nom en argument */
    Line* getLine(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne les Lignes */
    void getLines(std::vector<Line*>& ALines) const;

    /*------------------------------------------------------------------------*/
    /** Ajoute une Surface au manager */
    void add(Surface* sf);

    /** Enlève une Surface au manager */
    void remove(Surface* sf);

    /** Retourne la Surface suivant le nom en argument */
    Surface* getSurface(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne les Surfaces */
    void getSurfaces(std::vector<Surface*>& ASurfaces) const;

    /*------------------------------------------------------------------------*/
    /** Ajoute un Volume au manager */
    void add(Volume* vo);

    /** Enlève un Volume au manager */
    void remove(Volume* vo);

    /** Retourne le Volume suivant le nom en argument */
    Volume* getVolume(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne les Volumes */
    void getVolumes(std::vector<Volume*>& AVolumes) const;

    /** Création d'un sous-volume ou réutilisation d'un existant */
    SubVolume* getNewSubVolume(const std::string& gr_name, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /// accès à m_coface_allways_in_groups
    bool coFaceAllwaysInGroups() const {return m_coface_allways_in_groups;}
    /// modif de m_coface_allways_in_groups
    void setCoFaceAllwaysInGroups(bool vis) {m_coface_allways_in_groups = vis;}
#endif

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
