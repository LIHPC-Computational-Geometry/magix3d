/*----------------------------------------------------------------------------*/
/** \file CommandNewTopo.h
 *
 *  \author Simon Calderan
 *
 *  \date 03/01/2025
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWTOPO_H_
#define COMMANDNEWTOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandCreateTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
    namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandNewTopo
 *  \brief Commande permettant de créer une topologie libre quelconque
 *  avec réutilisation de la topologie existante
 */
/*----------------------------------------------------------------------------*/
        class CommandNewTopo: public CommandEditTopo {

        public:

            /** type de topologie demandée, non structuré, structuré avec projection,
             * un bloc sans projection, ou un bloc pour l'insertion
             */
            enum eTopoType { UNSTRUCTURED_BLOCK, STRUCTURED_BLOCK, CONFORM };

            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur d'entité topologique libre
             *
             *  \param c le contexte
             *  \param sommets la liste des sommets topologiques
             *  \param topoType un énuméré sur le type de topologie souhaitée
             *  \param dim la dimension de l'entité topologique créée
             *  \param groupName le nom du groupe à associer l'entité, peut etre vide
             */
            CommandNewTopo(Internal::Context& c, std::vector<Topo::Vertex*> sommets, eTopoType topoType,
                        int dim, std::string groupName);
            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur pour un sommet topologique uniquement par coordonées géométriques
             *
             *  \param c le contexte
             *  \param point les coordonnées pour créer le sommet topologique
             *  \param groupName le nom du groupe à associer l'entité, peut etre vide
             */
            CommandNewTopo(Internal::Context& c, const Utils::Math::Point point,
                        std::string groupName);

            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur pour un block à partir d'une liste de faces et de vertices
             *
             *  \param c le contexte
             *  \param faces les faces topologiques
             *  \param vertices les sommets topologiques
             *  \param isStructured indique si la topologie créée doit être structurée ou non
             *  \param groupName le nom du groupe à associer l'entité, peut etre vide
             */
            CommandNewTopo(Internal::Context& c, const std::vector<Topo::Face*>& faces,
                        const std::vector<Topo::Vertex*>& vertices, bool isStructured,
                        std::string groupName);
            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur pour une face à partir d'une liste de cofaces et de vertices
             *
             *  \param c le contexte
             *  \param cofaces les cofaces topologiques
             *  \param vertices les sommets topologiques
             *  \param isStructured indique si la topologie créée doit être structurée ou non
             */
            CommandNewTopo(Internal::Context& c, const std::vector<Topo::CoFace*>& cofaces,
                        const std::vector<Topo::Vertex*>& vertices, bool isStructured);
            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur pour une coface à partir d'une liste de edges et de vertices
             *
             *  \param c le contexte
             *  \param edges les edges topologiques
             *  \param vertices les sommets topologiques
             *  \param isStructured indique si la topologie créée doit être structurée ou non
             *  \param hasHole indique si la coface créée doit comporter un trou ou non
             */
            CommandNewTopo(Internal::Context& c, const std::vector<Topo::Edge*>& edges,
                std::vector<Topo::Vertex* > &vertices,
                bool isStructured, bool hasHole);
            /*------------------------------------------------------------------------*/
            /** \brief  Constructeur pour une edge à partir d'une liste de coedges et de 2 vertices
             *
             *  \param c le contexte
             *  \param coedges les coedges topologiques
             *  \param vertices les sommets topologiques
             *  \param isStrucutred indique si la topologie créée doit être structurée ou non
             */
            CommandNewTopo(Internal::Context& c, const std::vector<Topo::CoEdge*>& coedges,
                        const std::vector<Topo::Vertex*>& vertices);
            /*------------------------------------------------------------------------*/
            /** \brief  Destructeur
             */
            virtual ~CommandNewTopo();

            /*------------------------------------------------------------------------*/
            /** \brief  exécute la commande
             */
            virtual void internalExecute();

            /*------------------------------------------------------------------------*/
            /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
             *  une représentation des entités créées
             *
             *  La commande doit ensuite être détruite
             */
            virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

        private:

            /** Création d'un sommet topologique */
            virtual Vertex* createVertex(Utils::Math::Point point, std::string groupName);

            /** Création d'une arête topologique */
            virtual CoEdge* createCoEdge(Vertex* v0,Vertex* v1, std::string groupName);

            /** Création d'une arête topologique */
            virtual Edge* createEdge(Vertex* v0,Vertex* v1, std::vector<CoEdge*>& coedges);

            /** Création d'une face topologique structurée */
            virtual CoFace* createCoFace(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3, std::string groupName);

            /** Création d'une face topologique structurée */
            virtual CoFace* createCoFace(std::vector<Edge*>& edges, std::string groupName);

            /** Création d'un bloc topologique structuré */
            virtual void createBlock(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4, Vertex* v5,
                                        Vertex* v6, Vertex* v7, std::string groupName);

            /** Récupère l'arete topologique commune aux deux sommets si elle existe
             * @return l'arete topologique si elle existe ou nullptr sinon
             * */
            virtual Topo::CoEdge* getCommonCoEdge(const Vertex* v0, const Vertex* v1);

            /** Récupère la coface topologique commune aux quatres sommets si elle existe
             * @return la coface topologique si elle existe ou nullptr sinon
             * */
            virtual Topo::CoFace* getCommonCoFace(const Vertex* v0, const Vertex* v1, const Vertex* v2, const Vertex* v3);

            /** Récupère le bloc topologique commun aux huit sommets si il existe
             * @return le bloc topologique si il existe ou nullptr sinon
             * */
            virtual Topo::Block*  getCommonBlock(const Vertex* v0, const Vertex* v1, const Vertex* v2, const Vertex* v3,
                                                const Vertex* v4, const Vertex* v5, const Vertex* v6, const Vertex* v7);

        private:
            /// dimension de l'entité topo libre
            short m_dim;

            /// nom du groupe
            std::string m_groupName;

            /// les coordonnées pour créer un sommet libre
            Utils::Math::Point m_point;

            /// les sommets topologiques servant à créer l'entité
            std::vector<Topo::Vertex*> m_vertices;

            /// les cofaces topologiques servant à créer l'entité
            std::vector<Topo::CoFace*> m_cofaces;

            /// les arêtes topologiques servant à créer l'entité
            std::vector<Topo::CoEdge*> m_coedges;

            /// les coedges topologiques servant à créer l'entité
            std::vector<Topo::Edge*> m_edges;

            /// les faces topologiques servant à créer l'entité
            std::vector<Topo::Face*> m_faces;

            /// nombre de bras pour la première direction dans le cas structuré
            uint m_ni;
            /// nombre de bras pour la deuxième direction dans le cas structuré
            uint m_nj;
            /// nombre de bras pour la troisième direction dans le cas structuré
            uint m_nk;

            bool m_isStructured;
            bool m_hasHole;
        };
/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWTOPO_H_ */
/*----------------------------------------------------------------------------*/


