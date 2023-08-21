/*----------------------------------------------------------------------------*/
/*
 * \file MeshManagerIfc.h
 *
 *  \author Eric Brière de l'Isle, Charles Pignerol
 *
 *  \date 02/02/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_MESHMANAGERIFC_H_
#define MGX3D_MESH_MESHMANAGERIFC_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResultIfc.h"
#include "Utils/SwigCompletion.h"
#include <string>
#include <vector>
/*----------------------------------------------------------------------------*/
//#include <ConstQualif.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
class Entity;
}

namespace Internal {
class Context;
}
namespace Topo {
class Block;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {

class MeshItf;
class Cloud;
class Line;
class Surface;
class Volume;

class CommandMeshExplorer;

/*----------------------------------------------------------------------------*/
/**
 * \class MeshManagerIfc
 * \brief Gestionnaire des opérations effectuées au niveau du module
 *        de maillage. Le gestionnaire de maillage est l'objet qui stocke
 *        un lien vers une interface avec le maillage (GMDS) pour une session donnée.
 *
 */
class MeshManagerIfc: public Internal::CommandCreator {
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
    MeshManagerIfc(const std::string& name, Internal::ContextIfc* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur */
    virtual ~MeshManagerIfc();

	/*------------------------------------------------------------------------*/
#ifndef SWIG
	/** Réinitialisation     */
	virtual void clear();
#endif
   /*------------------------------------------------------------------------*/
#ifndef SWIG
    /// Accesseur sur le maillage
    virtual MeshItf* getMesh();
#endif

    /*------------------------------------------------------------------------*/
    /// Sauvegarde d'un maillage au format lima (mli)
    virtual void writeMli(std::string nom);
	SET_SWIG_COMPLETABLE_METHOD(writeMli)

    /*------------------------------------------------------------------------*/
    /// Sauvegarde d'un maillage au format vtk (vtk vtu vtp)
    virtual void writeVTK(std::string nom);
	SET_SWIG_COMPLETABLE_METHOD(writeVTK)

    /*------------------------------------------------------------------------*/
    /// Lecture d'un maillage au format lima (mli)
    virtual void readMli(std::string nom, std::string prefix);
	SET_SWIG_COMPLETABLE_METHOD(readMli)

    /*------------------------------------------------------------------------*/
    /// Sauvegarde d'un maillage au format CGNS
    virtual void writeCGNS(std::string nom);
	SET_SWIG_COMPLETABLE_METHOD(writeCGNS)

    /*------------------------------------------------------------------------*/
    /// Lissage du maillage
    virtual void smooth();

    /*------------------------------------------------------------------------*/
    /// Compare le maillage actuel avec un maillage sur disque, return true si ok
    virtual bool compareWithMesh(std::string nom);
	SET_SWIG_COMPLETABLE_METHOD(compareWithMesh)

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la strategie
    virtual strategy getStrategy();
    /// Modificateur de strategie
    virtual void setStrategy(const strategy& st);
	SET_SWIG_COMPLETABLE_METHOD(setStrategy)

    /*------------------------------------------------------------------------*/
    /** \brief  Génère le maillage pour une liste de blocs */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
                        newBlocksMesh(std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(newBlocksMesh)

    /** \brief  Génère le maillage pour une liste de faces (communes) */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
                        newFacesMesh(std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(newFacesMesh)

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
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	newSubVolumeBetweenSheets(std::vector<std::string>& blocks_name, std::string narete,
			int pos1, int pos2, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(newSubVolumeBetweenSheets)


#ifndef SWIG
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
    /** Retourne le Nuage suivant le nom en argument */
    virtual Cloud* getCloud(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne la Ligne suivant le nom en argument */
    virtual Line* getLine(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne la Surface suivant le nom en argument */
    virtual Surface* getSurface(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne les Surfaces */
    virtual void getSurfaces(std::vector<Surface*>& ASurfaces) const;

    /** Retourne le Volume suivant le nom en argument */
    virtual Volume* getVolume(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne les Volumes */
    virtual void getVolumes(std::vector<Volume*>& AVolumes) const;

#endif

//    /** Retourne les Nuages */
//    virtual void getClouds(std::vector<Cloud*>& AClouds) const;
//
//    /** Retourne les Surfaces */
//    virtual void getSurfaces(std::vector<Surface*>& ASurfaces) const;
//
//    /** Retourne les Volumes */
//    virtual void getVolumes(std::vector<Volume*>& AVolumes) const;

    /*------------------------------------------------------------------------*/
    virtual int getNbClouds(bool onlyVisible = true) const;
	SET_SWIG_COMPLETABLE_METHOD(getNbClouds)
    virtual int getNbSurfaces(bool onlyVisible = true) const;
	SET_SWIG_COMPLETABLE_METHOD(getNbSurfaces)
    virtual int getNbVolumes(bool onlyVisible = true) const;
	SET_SWIG_COMPLETABLE_METHOD(getNbVolumes)
    virtual int getNbNodes();
    virtual int getNbFaces();
    virtual int getNbRegions();

    /** Retourne une string avec les informations relatives à l'entité */
    virtual std::string getInfos(const std::string& name, int dim) const;
	SET_SWIG_COMPLETABLE_METHOD(getInfos)


private:
	/**
	 * \brief	Constructeur de copie. Interdit.
	 */
	MeshManagerIfc (const MeshManagerIfc&);

	/**
	 * \brief	Opérateur =. Interdit.
	 */
	MeshManagerIfc& operator = (const MeshManagerIfc&);
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_MESH_MESHMANAGERIFC_H_ */
