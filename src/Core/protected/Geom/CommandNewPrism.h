/*
 * CommandNewPrism.h
 *
 *  Created on: 21 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWPRISM_H_
#define COMMANDNEWPRISM_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateWithOtherGeomEntities.h"
#include "Utils/Vector.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
class GeomNewPrismImplementation;
/*----------------------------------------------------------------------------*/
/** \class CommandNewPrism
 *  \brief Commande permettant de créer un prisme par extrusion d'une face
 *         suivant un vecteur directeur
 */
/*----------------------------------------------------------------------------*/
class CommandNewPrism: public Geom::CommandCreateWithOtherGeomEntities{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param e l'entite géométrique à extruder
     *  \param dv le vecteur directeur
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewPrism(Internal::Context& c,
                    GeomEntity* e,
                    const Utils::Math::Vector& dv,
                    const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction du volume
     */
    virtual ~CommandNewPrism();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    //std::map<Geom::Vertex*,Geom::Vertex*>& getAssociationV2V();
    std::map<Geom::Vertex*,Geom::Vertex*>& getAssociationV2VOpp();
    std::map<Geom::Vertex*,Geom::Curve*>& getAssociationV2C();
    //std::map<Geom::Curve*,Geom::Curve*>& getAssociationC2C();
    std::map<Geom::Curve*,Geom::Curve*>& getAssociationC2COpp();
    std::map<Geom::Curve*,Geom::Surface*>& getAssociationC2S();
    //std::map<Geom::Surface*,Geom::Surface*>& getAssociationS2S();
    std::map<Geom::Surface*,Geom::Surface*>& getAssociationS2SOpp();
    std::map<Geom::Surface*,Geom::Volume*>& getAssociationS2V();
    //std::map<GeomEntity*,std::vector<GeomEntity*> >& getReplacements();
private:
    /// marque la face, les courbes et points
    void marque(Surface* surf, std::map<unsigned long, uint>& filtre_uid);

    /// retourne la courbe marqué parmis les courbes de la surface, 0 si rien de trouvé
    Curve* getMarquedCurve(Surface* surf, std::map<unsigned long, uint>& filtre_uid);

    /// retourne le sommet marqué parmis les sommets de la courbe, 0 si rien de trouvé
    Vertex* getMarquedVertex(Curve* curv, std::map<unsigned long, uint>& filtre_uid);

    /// transmet les groupes du 2D vers le 3D
    void groups2DTo3D();

protected:

    /* parametres de l'operation */
    GeomEntity* m_base;
    Utils::Math::Vector m_dv;

    /* objet gérant la création du prisme*/
    GeomNewPrismImplementation* m_impl;

    //std::map<Geom::Vertex* ,Geom::Vertex*>  m_v2v;
    std::map<Geom::Vertex* ,Geom::Vertex*>  m_v2v_opp;
    std::map<Geom::Vertex* ,Geom::Curve*>   m_v2c;
    //std::map<Geom::Curve*  ,Geom::Curve*>   m_c2c;
    std::map<Geom::Curve*  ,Geom::Curve*>   m_c2c_opp;
    std::map<Geom::Curve*  ,Geom::Surface*> m_c2s;
    //std::map<Geom::Surface*,Geom::Surface*> m_s2s;
    std::map<Geom::Surface*,Geom::Surface*> m_s2s_opp;
    std::map<Geom::Surface*,Geom::Volume*>  m_s2v;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWPRISM_H_ */
/*----------------------------------------------------------------------------*/


