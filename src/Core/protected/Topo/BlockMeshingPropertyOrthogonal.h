/*----------------------------------------------------------------------------*/
/*
 * \file BlockMeshingPropertyOrthogonal.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/7/15
 */
/*----------------------------------------------------------------------------*/


#ifndef MGX3D_TOPO_BLOCKMESHINGPROPERTYORTHOGONAL_H_
#define MGX3D_TOPO_BLOCKMESHINGPROPERTYORTHOGONAL_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Topo/BlockMeshingPropertyDirectional.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'un bloc suivant une direction
 */
class BlockMeshingPropertyOrthogonal : public BlockMeshingPropertyDirectional {

public:

    /*------------------------------------------------------------------------*/
    /// Constructeur avec direction définie par 2 points, orthogonalité du côté du départ
	BlockMeshingPropertyOrthogonal(Utils::Math::Point v1, Utils::Math::Point v2,
			int nbLayers)
: BlockMeshingPropertyDirectional(v1,v2)
, m_side(side_undef)
, m_nbLayers(nbLayers)
{}

	/// Constructeur avec direction définie par une arête
	BlockMeshingPropertyOrthogonal(std::string coedgeName, int nbLayers)
: BlockMeshingPropertyDirectional(coedgeName)
, m_side(side_undef)
, m_nbLayers(nbLayers)
{}

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual BlockMeshingPropertyOrthogonal* clone()
    {return new BlockMeshingPropertyOrthogonal(*this);}
#endif
    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    virtual BlockMeshingProperty::meshLaw getMeshLaw() const
    {return BlockMeshingProperty::orthogonal;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const
    {
        if (m_dir == dir_i)
            return "OrthogonalI";
        else if (m_dir == dir_j)
            return "OrthogonalJ";
        else if (m_dir == dir_k)
            return "OrthogonalK";
        else
            return "Direction à préciser";
    }

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        if (m_method == points)
        	o << getMgx3DAlias() << ".BlockMeshingPropertyOrthogonal("
        	<< m_v1.getScriptCommand() << ", "
        	<< m_v2.getScriptCommand() << ", "
			<< (long)m_nbLayers << ")";
        else
        	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne avec getScriptCommand", TkUtil::Charset::UTF_8));
        return o;
    }
    /*------------------------------------------------------------------------*/
    /// initialisation du côté en plus
    virtual void initDir(Block* bloc)
    {
    	BlockMeshingPropertyDirectional::initDir(bloc);

    	if (m_method == points)
    		m_side = _computeSide(bloc, m_v1, m_v2, getDir());
    	else
    		throw TkUtil::Exception("Erreur interne avec initDir");
    }

    /*------------------------------------------------------------------------*/
    virtual uint getSide() const
    {
    	if (m_side == side_min)
    		return 0;
    	else if (m_side == side_max)
    		return 1;
    	else
    		throw TkUtil::Exception(TkUtil::UTF8String ("BlockMeshingPropertyOrthogonal::getSide() ne peut se faire sans initialiser le côté", TkUtil::Charset::UTF_8));
    }
    /*------------------------------------------------------------------------*/
    /// ajoute la description des propriétés spécifiques
    virtual void addProperties(Utils::SerializedRepresentation& ppt) const
    {
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Nombre de couches orthogonales", (long)m_nbLayers));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Discrétisation orthogonale coté",
    					m_side == side_min?std::string("départ"):std::string("arrivée")));
    }

#endif
    /*------------------------------------------------------------------------*/
    virtual int getNbLayers() const
    { return m_nbLayers; }
    /*------------------------------------------------------------------------*/

private:
    /// coté où on se contraint à être orthogonal
    meshSideLaw m_side;

    /// nombre de couches pour orthogonalité
    int m_nbLayers;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_TOPO_BLOCKMESHINGPROPERTYORTHOGONAL_H_ */
