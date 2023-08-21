/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitBlocks.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14/5/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSPLITBLOCKS_H_
#define COMMANDSPLITBLOCKS_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
#include <map>
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandSplitBlocks
 *  \brief Commande permettant de découper un ensemble de blocs (structuré) en 2
 *
 */
/*----------------------------------------------------------------------------*/
class CommandSplitBlocks: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour un ensemble de blocs, une arête et un ratio
     *
     *  \param c le contexte
     *  \param blocs les blocs à découper
     *  \param arete l'arête qui sert à déterminer la direction de la coupe structurée
     *  \param ratio un paramètre pour déterminer la position du sommet sur l'arête
     */
    CommandSplitBlocks(Internal::Context& c, std::vector<Topo::Block* > &blocs, CoEdge* arete, double ratio);

    /** \brief  Constructeur pour un ensemble de blocs, une arête et un point dont on déduira le ratio
     *
     *  \param c le contexte
     *  \param blocs les blocs à découper
     *  \param arete l'arête qui sert à déterminer la direction de la coupe structurée
     *  \param pt un point qui donnera un paramètre pour déterminer la position du sommet sur l'arête par projection sur cette dernière
     */
    CommandSplitBlocks(Internal::Context& c, std::vector<Topo::Block* > &blocs, CoEdge* arete, const Point& pt);

    /** \brief  Constructeur pour tous les blocs blocs, une arête et un ratio
     *
     *  \param c le contexte
     *  \param arete l'arête qui sert à déterminer la direction de la coupe structurée
     *  \param ratio un paramètre pour déterminer la position du sommet sur l'arête
     */
    CommandSplitBlocks(Internal::Context& c, CoEdge* arete, double ratio);

    /** \brief  Constructeur pour tous les blocs blocs, une arête et un point dont on déduira le ratio
     *
     *  \param c le contexte
     *  \param arete l'arête qui sert à déterminer la direction de la coupe structurée
     *  \param pt un point qui donnera un paramètre pour déterminer la position du sommet sur l'arête par projection sur cette dernière
     */
    CommandSplitBlocks(Internal::Context& c, CoEdge* arete, const Point& pt);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSplitBlocks();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /// Compte le nombre d'arêtes reliées à chacun des sommets de la topologie concernée par la commande
    virtual void countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités modifiées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:

    /*------------------------------------------------------------------------*/
    void findBlockUnmarkedWithCoEdgeMarked(std::set<Block*>& filtre_blocs,
    		std::set<CoEdge*>& filtre_coedges, Block* &bloc, CoEdge* &arete);

    /*------------------------------------------------------------------------*/
    /// Vérification de la valeur de m_ratio
    void validateRatio();

    /// Vérification et stockage des blocs
    void initBlocks(std::vector<Topo::Block* > &blocs, bool exceptionIfUnstructured);

    /// met à jour le commentaire pour la commande
    void initCommentsBlocks(CoEdge* arete);
    void initCommentsAllBlocks(CoEdge* arete);

    class FindBlockException : public TkUtil::Exception
	{
	public :

    	/*------------------------------------------------------------------------*/
    	/** \brief Constructeur par défaut.
    	 *
    	 *  \param AWhat le texte associé à l'exception
    	 */
    	FindBlockException (const std::string& AWhat = "")
	    : TkUtil::Exception("FindBlockException "+AWhat)
    	{}

    	/*------------------------------------------------------------------------*/
    	/** \brief Constructeur par copie.
    	 *
    	 *  \param AExc l'exception à copier
    	 */
    	FindBlockException (const FindBlockException& AExc)
    	: TkUtil::Exception(AExc.getFullMessage())
    	{}
	};


private:
    /// les blocs structurés à découper
    std::vector<Block* > m_blocs;

    /// l'arête qui sert à déterminer la direction de la coupe structurée
    CoEdge* m_arete;

    /// paramètre pour déterminer la position du sommet sur l'arête
    double m_ratio;

    /// tous les blocs doivent être obligatoirement découpés ou non
    bool m_allBlocksMustBeCut;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSPLITBLOCKS_H_ */
/*----------------------------------------------------------------------------*/
