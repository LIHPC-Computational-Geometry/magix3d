#ifndef COMMANDEXTRUSION_H_
#define COMMANDEXTRUSION_H_
/*----------------------------------------------------------------------------*/
#include <map>
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class CommandExtrusion: public CommandEditGeom {

protected:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    CommandExtrusion(Internal::Context& c, 
        std::string name,
        std::vector<GeomEntity*>& entities, 
        const bool keep);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExtrusion() = default;

    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();

public:
    /*------------------------------------------------------------------------*/
    /** \brief  lors de l'extrusion on stocke le lien entre le sommet
     *          départ et la courbe qui lui correspond. Cette méthode permet
     *          d'accéder à l'association. Si un sommet n'a pas permis de générer
     *          une courbe, alors il est associé à 0.
     *
     *  \return une map d'un sommet à une courbe
     */
    std::map<Geom::Vertex*,Geom::Vertex*>& getAssociationV2V();
    std::map<Geom::Vertex*,Geom::Vertex*>& getAssociationV2VOpp();
    std::map<Geom::Vertex*,Geom::Curve*>& getAssociationV2C();
    std::map<Geom::Curve*,Geom::Curve*>& getAssociationC2C();
    std::map<Geom::Curve*,Geom::Curve*>& getAssociationC2COpp();
    std::map<Geom::Curve*,Geom::Surface*>& getAssociationC2S();
    std::map<Geom::Surface*,Geom::Surface*>& getAssociationS2S();
    std::map<Geom::Surface*,Geom::Surface*>& getAssociationS2SOpp();
    std::map<Geom::Surface*,Geom::Volume*>& getAssociationS2V();
    std::map<GeomEntity*,std::vector<GeomEntity*> >& getReplacements();


    /*------------------------------------------------------------------------*/
    /// affiche sur cout les infos sur les différentes map V2V et autres ...
    void printInfoAssociations() const;

protected:

    /** Reprend les groupes initiaux en les préfixant,
     *  ajoute le groupe correspondant au prefix pour les surfaces
     */
    void prefixGroupsName(const std::string& pre,
            std::map<Geom::Vertex* ,Geom::Vertex*>&  v2v,
            std::map<Geom::Curve*  ,Geom::Curve*>&   c2c,
            std::map<Geom::Surface*,Geom::Surface*>& s2s);

    /// transmet les groupes du 2D vers le 3D
    void groups2DTo3D();


protected:
    /* entités à unir */
    std::vector<GeomEntity*> m_entities;

    /* conservation des entités initiales*/
    bool m_keep;

    std::map<Geom::Vertex* ,Geom::Vertex*>  m_v2v;
    std::map<Geom::Vertex* ,Geom::Vertex*>  m_v2v_opp;
    std::map<Geom::Vertex* ,Geom::Curve*>   m_v2c;
    std::map<Geom::Curve*  ,Geom::Curve*>   m_c2c;
    std::map<Geom::Curve*  ,Geom::Curve*>   m_c2c_opp;
    std::map<Geom::Curve*  ,Geom::Surface*> m_c2s;
    std::map<Geom::Surface*,Geom::Surface*> m_s2s;
    std::map<Geom::Surface*,Geom::Surface*> m_s2s_opp;
    std::map<Geom::Surface*,Geom::Volume*>  m_s2v;
};
/*----------------------------------------------------------------------------*/
} /* namespace Geom */
/*----------------------------------------------------------------------------*/
} /* namespace Mgx3D */
/*----------------------------------------------------------------------------*/
#endif /* COMMANDEXTRUSION_H_ */
/*----------------------------------------------------------------------------*/
