/*----------------------------------------------------------------------------*/
/*
 * GeomModificationBaseClass.h
 *
 *  Created on: 21 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/

#ifndef GEOMMODIFICATIONBASECLASS_H_
#define GEOMMODIFICATIONBASECLASS_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <vector>
#include <list>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>

#include <BRepBuilderAPI_MakeShape.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Volume;
class Surface;
class Curve;
class Vertex;
class CommandGeomCopy;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomModificationBaseClass
 * \brief Classe regroupant des opérations qui sont utiles pour les
 *        opérations de modification de la géométrie
 *
 */
class GeomModificationBaseClass{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    virtual void prePerform()=0;

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'intersection. Les entités créées sont stockées
     *          dans res
     */
    virtual void perform(std::vector<GeomEntity*>& res)=0;

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation du undo/redo pour les entités qui n'ont pas leur état stocké dans le Memento de la commande
     */
    virtual void performUndo(){}
    virtual void performRedo(){}

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les entités supprimées. N'a de sens
     *          qu'une fois l'opération perform() appelée.
     */
    std::vector<GeomEntity*>& getRemovedEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les nouvelles entités. N'a de sens
     *          qu'une fois l'opération perform() appelée.
     */
    std::vector<GeomEntity*>& getNewEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les entités conservées et
     *          potentiellement modifées. N'a de sens qu'une fois l'opération
     *          perform() appelée.
     */
    std::vector<GeomEntity*> getKeepedEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur les entités conservées et déplacées
     *          qu'une fois l'opération perform() appelée.
     */
    std::vector<GeomEntity*>& getMovedEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur toutes les entités  sur lesquelles
     *          l'algorithme travaille
     */
    std::list<GeomEntity*>& getRefEntities(const int dim);

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur toutes les entités  adjacentes
     */
    std::list<GeomEntity*>& getAdjEntities(const int dim);

    /*------------------------------------------------------------------------*/
    /** \brief  retourne une référence sur une map indiquant par quelles
     *          entités une entité supprimée a été remplacée
     */
    std::map<GeomEntity*,std::vector<GeomEntity*> >& getReplacedEntities();

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomModificationBaseClass();

    /*------------------------------------------------------------------------*/
    /** \brief  accesseur sur le contexte
     */
    virtual Internal::Context& getContext() {return m_context;}

    /*------------------------------------------------------------------------*/
    /** \brief  Récupération des shapes OCC correspondants aux entités M3D
     *          que l'on fusionne.
     */
    static void getOCCShape(GeomEntity* ge, TopoDS_Shape& topoS);
    static void getOCCShapes(GeomEntity* ge, std::vector<TopoDS_Shape>& topoS);

protected:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param c le contexte de travail qui sera toujours utile
     */
    GeomModificationBaseClass(Internal::Context& c);

    /*------------------------------------------------------------------------*/
    /** \brief  Initialise les attributs de cette classe dans les constructeurs
     *          des classes filles.
     */
    virtual void init(std::vector<GeomEntity*>& es);

    /*------------------------------------------------------------------------*/
    /** \brief  Initialise les attributs de cette classe dans les constructeurs
     *          des classes filles.
     */
    virtual void init(Geom::CommandGeomCopy* cmd);

    /*------------------------------------------------------------------------*/
    /** \brief  Initialise les attributs de cette classe dans les constructeurs
     *          des classes filles avec toutes les entités existantes
     */
    virtual void initWithAll();

    /*------------------------------------------------------------------------*/
    /** \brief  initialise les entités de références
     *
     *  \param entity une entité
     */
    void addReference(GeomEntity* entity);

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute les entités filles de entity dans les entités de
     *          référence
     *
     *  \param entity une entité
     */
    virtual void addDownIncidentReference(GeomEntity* entity);

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute les entités filles de entity dans les entités de
     *          référence
     *
     *  \param entity une entité
     */
    void addAdjacencyReference(GeomEntity* entity);

    /*------------------------------------------------------------------------*/
        /** \brief  Crée toutes les entités géométriques M3D nécessaires pour
         *          représenter la forme OCC shape. Toutes les connections
         *          topologiques sont effectués et les entités supprimées,
         *          ajoutées, modifiées et remplacées sont renseignées.
         *
         *  \param shape            une entité OCC
         *  \param replaceVolumes   si TRUE on cherche les volumes remplacées,
         *                          sinon non.
         */
    void createGeomEntities(const TopoDS_Shape& shape,
                            const bool replaceVolumes = false,
							const bool deleteAloneVertices = true);


    /// remplissage de m_removed et identification des entités créées à tord
    void clean(const bool deleteAloneVertices);

    void cleanRefEntities();

    void cleanEntityAndChildren(GeomEntity* e);

    /*------------------------------------------------------------------------*/
    /** \brief  Une fois les entités de référence traitées, cette
     *          méthode récupère les entités OCC et M3D qui sont présentes
     *          à la fois dans l'entité créée et dans les entités de références.
     *          Les entités nouvelles sont elles crées en tant qu'entités M3D
     *          et placées dans m_newEntities.
     */

    void createNewVertices(const TopoDS_Shape& ref_entity,
            std::vector<TopoDS_Shape>& occ_entities,
            std::vector<Vertex*>& m3d_entities,
            bool isAdjacent = false);

    void createNewCurves(const TopoDS_Shape& ref_entity,
            std::vector<TopoDS_Shape>& occ_entities,
            std::vector<Curve*>& m3d_entities,
            bool isAdjacent = false);

    void createNewSurfaces(const TopoDS_Shape& ref_entity, // I
            std::vector<TopoDS_Shape>& occ_entities, // O
            std::vector<Surface*>& m3d_entities, // O
            bool isAdjacent = false);

    void createNewVolumes(const TopoDS_Shape& ref_entity,
            std::vector<TopoDS_Shape>& occ_entities,
            std::vector<Volume*>& m3d_entities,
            bool isAdjacent = false);

    void createNewAdjVolumes(const TopoDS_Shape& ref_entity,
            std::vector<TopoDS_Shape>& occ_entities,
            std::vector<Volume*>& m3d_entities,
            bool isAdjacent = false);

     void connect(std::vector<TopoDS_Shape>& occ_vertices,
                  std::vector<TopoDS_Shape>& occ_curves,
                  std::vector<TopoDS_Shape>& occ_surfaces,
                  std::vector<TopoDS_Shape>& occ_volumes,
                  std::vector<Vertex*>&      m3d_vertices,
                  std::vector<Curve*>&       m3d_curves,
                  std::vector<Surface*>&     m3d_surfaces,
                  std::vector<Volume*>&      m3d_volumes);

     void buildReplacedList(const bool replaceVolumes);
     void computeReplacedVertex (Vertex*  e);
     void computeReplacedCurve  (Curve*   e);
     void computeReplacedSurface(Surface* e);
     void computeReplacedVolume(Volume* e);
     void rebuildAdjacencyEntities(const TopoDS_Shape& shape);
     void rebuildAdjacencyLinks();

     /// recherche une entité géométrique correspondant à une TopoDS_Shape
     GeomEntity* getGeomEntity(TopoDS_Shape &loc_shape,
			 std::vector<TopoDS_Shape>& OCC_vertices,
			 std::vector<TopoDS_Shape>& OCC_curves,
			 std::vector<TopoDS_Shape>& OCC_surfaces,
			 std::vector<TopoDS_Shape>& OCC_volumes,
			 std::vector<Vertex*>&      m3d_vertices,
			 std::vector<Curve*>&       m3d_curves,
			 std::vector<Surface*>&     m3d_surfaces,
			 std::vector<Volume*>&      m3d_volumes);

    /*------------------------------------------------------------------------*/
    /** \brief  Vérifie que les entités dont on veut faire la modification ne sont
     *          pas connectées à des entités de dimension supérieure. Si oui,
     *          une exception est levée.
     */
    void checkValidity(std::set<GeomEntity*>& entities);

    void buildInitialSet(std::set<GeomEntity*>& init_entities, bool force);

protected:

    /** contexte d'exécution*/
    Internal::Context& m_context;

    /** commande de construction des entités à traiter */
    Geom::CommandGeomCopy* m_buildEntitiesCmd;

    /** entités à traiter*/
    std::vector<GeomEntity*> m_init_entities;

    std::list<GeomEntity*> m_ref_entities[4];

    std::list<GeomEntity*> m_adj_entities[4];



    /* entités que l'on a conservées (modifiées ou non)*/
    std::vector<Vertex*> m_toKeepVertices;
    std::vector<Curve*> m_toKeepCurves;
    std::vector<Surface*> m_toKeepSurfaces;
    std::vector<Volume*> m_toKeepVolumes;

    /* entités que l'on a ajoutées */
    std::vector<Vertex*> m_newVertices;
    std::vector<Curve*> m_newCurves;
    std::vector<Surface*> m_newSurfaces;
    std::vector<Volume*> m_newVolumes;

    /*entités ajoutées */
    std::vector<GeomEntity*> m_newEntities;
    /* entités que l'on a supprimées */
    std::vector<GeomEntity*> m_removedEntities;
    /* entités que l'on a déplacées */
    std::vector<GeomEntity*> m_movedEntities;

    /* entités que l'on a remplacées par une autre. Une entité remplacée est
     * aussi supprimée (et donc apparait dans m_removedEntities)*/
    std::map<GeomEntity*,std::vector<GeomEntity*> > m_replacedEntities;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMMODIFICATIONTOOLKIT_H_ */
/*----------------------------------------------------------------------------*/

