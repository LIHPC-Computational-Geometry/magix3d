/*----------------------------------------------------------------------------*/
/*
 * \file CommandCreateSubVolumeBetweenSheets.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2/12/2016
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCREATESUBVOLUMEBETWEENSHEETS_H_
#define COMMANDCREATESUBVOLUMEBETWEENSHEETS_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Mesh/SubVolume.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group3D;
}
namespace Topo{
class CoEdge;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandCreateSubVolumeBetweenSheets
 *  \brief Commande de création de sous-volumes entre 2 feuillets
 */
/*----------------------------------------------------------------------------*/
class CommandCreateSubVolumeBetweenSheets: public Internal::CommandInternal {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur d'un ensemble de sous-volumes
     *
     *  \param c le contexte
     *  \param blocks la liste des blocs dans lesquels ont fait l'extraction
     *  \param coedge l'arête par laquelle passe l'exploration
     *  \param pos1 indice du bras (à partir de 1) pour le premier feuillet
     *  \param pos2 indice du bras (à partir de 1) pour le dernier feuillet
     *  \param groupName le nom du volume de maille à créer
     */
    CommandCreateSubVolumeBetweenSheets(Internal::Context& c, std::vector<Topo::Block*>& blocks,
    		Topo::CoEdge* arete, int& pos1, int& pos2, std::string& groupName);


	/*------------------------------------------------------------------------*/
	/** \brief	Destructeur. RAS.
	 */
	virtual ~CommandCreateSubVolumeBetweenSheets();


    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /** \brief  rejoue la commande
     */
    void internalRedo();

protected:
    /// bloc, direction et positions des coupes
    struct BlockDirPos {
        Topo::Block* m_bloc;
        Topo::Block::eDirOnBlock m_dir;
        uint m_pos1;
        uint m_pos2;
    };

    /// parcours les blocs et marque les faces acceptées
    void selectCoFaces(std::map<Topo::CoFace*, uint>& filtre_coface);

    /// on ne prend qu'une fois une arête, on les marque suivant la position de la coupe
    void computePosCoEdge(std::map<Topo::CoFace*, uint>& filtre_coface,
            std::map<Topo::CoEdge*, uint> &filtre1_coedge,
            std::map<Topo::CoEdge*, uint> &filtre2_coedge);

    void computePosBlock(std::map<Topo::CoEdge*, uint> &filtre1_coedge,
			std::map<Topo::CoEdge*, uint> &filtre2_coedge,
            std::vector<BlockDirPos>& bloc_dirPos);

    /// création des sous-volumes suivant la position demandée dans les blocs
    void createSubVolume(std::vector<BlockDirPos>& bloc_dirPos);


private:
    /// liste des blocs auxquels on se restreint
    std::vector<Topo::Block*> m_blocks;

    /// l'arête par laquelle passent les feuillets
    Topo::CoEdge* m_edge;

    /// position du feuillet de départ sur l'arête (en nombre de bras)
    int m_pos1;
    /// position du feuillet d'arrivée sur l'arête (en nombre de bras)
    int m_pos2;

    /// le nom d'un groupe (volume) dans lequel on met les mailles
    std::string m_group_name;

};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCREATESUBVOLUMEBETWEENSHEETS_H_ */
/*----------------------------------------------------------------------------*/
