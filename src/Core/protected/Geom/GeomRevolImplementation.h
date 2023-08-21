/*----------------------------------------------------------------------------*/
/*
 * GeomRevolImplementation.h
 *
 *  Created on: 18 juin 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMREVOLIMPLEMENTATION_H_
#define GEOMREVOLIMPLEMENTATION_H_
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
 * \class GeomRevolImplementation
 * \brief Classe réalisant la révolution d'entités géométriques
 *
 */
/*----------------------------------------------------------------------------*/
class GeomRevolImplementation:  public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param entities les entities dont on fait la révolution
     *  \param axis1    premier point definissant l'axe de rotation
     *  \param axis2    second point definissant l'axe de rotation
     *  \param angle    angle de rotation en degré (compris entre ]0,360])
     *  \param keep     indique si l'on conserve (true) ou pas (false) les
     *                  entités de départ
     */
    GeomRevolImplementation(   Internal::Context& c,
                               std::vector<GeomEntity*> entities,
                               const Utils::Math::Point& axis1,
                               const Utils::Math::Point& axis2,
                               const double& angle,
                               const bool keep=false);

    virtual ~GeomRevolImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la révolution. Les entités créées sont stockées
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


    void makeRevol(Vertex *v, std::vector<GeomEntity*>& res,
            std::map<Geom::Vertex*,Geom::Vertex*>& v2v,
            std::map<Geom::Vertex*,Geom::Vertex*>& v2vOpp,
            std::map<Geom::Vertex*,Geom::Curve*>& v2c);

//    void makeRevol(Curve *c, std::vector<GeomEntity*>& res,
//            std::map<Geom::Vertex*,Geom::Vertex*>  & v2v,
//            std::map<Geom::Vertex*,Geom::Vertex*>  & v2vOpp,
//            std::map<Geom::Vertex*,Geom::Curve*>   & v2c,
//            std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
//            std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
//            std::map<Geom::Curve*  ,Geom::Surface*>& c2s);

    void makeRevolComposite(Curve *c, std::vector<GeomEntity*>& res,
            std::map<Geom::Vertex*,Geom::Vertex*>  & v2v,
            std::map<Geom::Vertex*,Geom::Vertex*>  & v2vOpp,
            std::map<Geom::Vertex*,Geom::Curve*>   & v2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
            std::map<Geom::Curve*  ,Geom::Surface*>& c2s);

//    void makeRevol2PI(Curve *c, std::vector<GeomEntity*>& res,
//            std::map<Geom::Vertex*,Geom::Vertex*>  & v2v,
//            std::map<Geom::Vertex*,Geom::Vertex*>  & v2vOpp,
//            std::map<Geom::Vertex*,Geom::Curve*>   & v2c,
//            std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
//            std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
//            std::map<Geom::Curve*  ,Geom::Surface*>& c2s);

    void makeRevol2PIComposite(Curve *c, std::vector<GeomEntity*>& res,
            std::map<Geom::Vertex*,Geom::Vertex*>  & v2v,
            std::map<Geom::Vertex*,Geom::Vertex*>  & v2vOpp,
            std::map<Geom::Vertex*,Geom::Curve*>   & v2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
            std::map<Geom::Curve*  ,Geom::Surface*>& c2s);

    void makeRevol(Surface *s, std::vector<GeomEntity*>& res,
            std::map<Geom::Vertex* ,Geom::Vertex*> & v2v,
            std::map<Geom::Vertex* ,Geom::Vertex*> & v2vOpp,
            std::map<Geom::Vertex* ,Geom::Curve*>  & v2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2c,
            std::map<Geom::Curve*  ,Geom::Curve*>  & c2cOpp,
            std::map<Geom::Curve*  ,Geom::Surface*>  & c2s,
            std::map<Geom::Surface*,Geom::Surface*>& s2s,
            std::map<Geom::Surface*,Geom::Surface*>& s2sOpp,
            std::map<Geom::Surface*,Geom::Volume*> & s2v);

    void makeRevol2PI(Surface *s, std::vector<GeomEntity*>& res,
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

    /* points définissant l'axe de révolution */
    Utils::Math::Point m_axis1;
    Utils::Math::Point m_axis2;

    /* angle de révolution */
    double m_angle;

    /* pour conserver les entités de départ ou non*/
    bool m_keep;
    /* entités que l'on a modifiées */
    std::vector<GeomEntity*> m_modifiedEntities;

    /* map inverse entre sommet "2D" et sommet copie*/
    std::map<Geom::Vertex* ,Geom::Vertex*> m_v2v_inv;


    /* entités passées en argument à la commande. */
    std::vector<GeomEntity*> m_entities_param;
};
/*----------------------------------------------------------------------------*/
} /* namespace Geom */
/*----------------------------------------------------------------------------*/
} /* namespace Mgx3D */
/*----------------------------------------------------------------------------*/
#endif /* GEOMREVOLIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/
