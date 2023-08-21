/*----------------------------------------------------------------------------*/
/*
 * GeomExtrudeImplementation.h
 *
 *  Created on: 8/11/2019
 *      Author: Eric B
 *      à partir de GeomRevolImplementation
 *
 *
 *
 *  Modified on: 21/02/2022
 *      Author: Simon C
 *      ajout de la possibilité de conserver les entités extrudées
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMEXTRUDEIMPLEMENTATION_H_
#define GEOMEXTRUDEIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <map>
#include <set>
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomModificationBaseClass.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class GeomExtrudeImplementation
 * \brief Classe réalisant la révolution d'entités géométriques
 *
 */
/*----------------------------------------------------------------------------*/
class GeomExtrudeImplementation:  public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param entities les entities dont on fait l'extrusion
     *  \param dp       le vecteur pour l'extrusion
     *  \param keep     indique si l'on conserve (true) ou pas (false) les
     *                  entités de départ
     */
    GeomExtrudeImplementation(   Internal::Context& c,
                               std::vector<GeomEntity*> entities,
							   const Vector& dp, const bool keep);

    virtual ~GeomExtrudeImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'extrusion. Les entités créées sont stockées
     *          dans res
     */
    void perform(std::vector<GeomEntity*>& res,
            std::map<Geom::Vertex* ,Geom::Vertex*> & v2v,
            std::map<Geom::Vertex* ,Geom::Vertex*> & v2vOpp,
            std::map<Geom::Vertex* ,Geom::Curve*>  & v2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
            std::map<Geom::Curve*  ,Geom::Surface*>  & c2s,
            std::map<Geom::Surface*,Geom::Surface*>& s2s,
            std::map<Geom::Surface*,Geom::Surface*>& s2sOpp,
            std::map<Geom::Surface*,Geom::Volume*> & s2v);


    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();


protected:


    void makeExtrude(Vertex *v, std::vector<GeomEntity*>& res,
            std::map<Geom::Vertex*,Geom::Vertex*>& v2v,
            std::map<Geom::Vertex*,Geom::Vertex*>& v2vOpp,
            std::map<Geom::Vertex*,Geom::Curve*>& v2c);

    void makeExtrudeComposite(Curve *c, std::vector<GeomEntity*>& res,
            std::map<Geom::Vertex*,Geom::Vertex*>  & v2v,
            std::map<Geom::Vertex*,Geom::Vertex*>  & v2vOpp,
            std::map<Geom::Vertex*,Geom::Curve*>   & v2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
            std::map<Geom::Curve*  ,Geom::Surface*>& c2s);

    void makeExtrude(Surface *s, std::vector<GeomEntity*>& res,
            std::map<Geom::Vertex* ,Geom::Vertex*> & v2v,
            std::map<Geom::Vertex* ,Geom::Vertex*> & v2vOpp,
            std::map<Geom::Vertex* ,Geom::Curve*>  & v2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
            std::map<Geom::Curve*  ,Geom::Surface*>  & c2s,
            std::map<Geom::Surface*,Geom::Surface*>& s2s,
            std::map<Geom::Surface*,Geom::Surface*>& s2sOpp,
            std::map<Geom::Surface*,Geom::Volume*> & s2v);

    void findOCCEdgeAssociation(
            TopoDS_Edge& ref,
            TopoDS_Vertex& v1, TopoDS_Vertex& v2,TopoDS_Vertex& v3,TopoDS_Vertex& v4,
            TopoDS_Edge& e12, TopoDS_Edge& e23,TopoDS_Edge& e34,TopoDS_Edge& e41);
    void findOCCEdgeAssociation(
            TopoDS_Edge& ref,
            TopoDS_Vertex& v1, TopoDS_Vertex& v2,TopoDS_Vertex& v3,
            TopoDS_Edge& e12, TopoDS_Edge& e23,TopoDS_Edge& e31);

    /// retourne vrai si les 2 sommets appartiennent à la courbe
    bool findVertices(TopoDS_Edge& ref, TopoDS_Vertex& v1, TopoDS_Vertex& v2);

protected:

    /* le vecteur de l'extrusion */
    Utils::Math::Vector m_vector;

    /* entités que l'on a modifiées */
    std::vector<GeomEntity*> m_modifiedEntities;

    /* map inverse entre sommet "2D" et sommet copie*/
    std::map<Geom::Vertex* ,Geom::Vertex*> m_v2v_inv;


    /* entités passées en argument à la commande. */
    std::vector<GeomEntity*> m_entities_param;

    /* pour conserver les entités de départ ou non*/
    bool m_keep;
};
/*----------------------------------------------------------------------------*/
} /* namespace Geom */
/*----------------------------------------------------------------------------*/
} /* namespace Mgx3D */
/*----------------------------------------------------------------------------*/
#endif /* GEOMEXTRUDEIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/
