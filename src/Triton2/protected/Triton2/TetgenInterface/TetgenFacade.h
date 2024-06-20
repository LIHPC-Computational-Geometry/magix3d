/*----------------------------------------------------------------------------*/
/*
 * TetgenFacade.h
 *
 *  Created on: 16 mai 2011
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef TETGENFACADE_H_
#define TETGENFACADE_H_
/*----------------------------------------------------------------------------*/
#include <gmds/ig/Mesh.h>
#include <gmds/cad/GeomVolume.h>
//#include <GMDSGeom/GeomManager.h>
/*----------------------------------------------------------------------------*/
#include <tetgen.h>
/*----------------------------------------------------------------------------*/
#include <sstream>
#include <map>
/*----------------------------------------------------------------------------*/
namespace triton{
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/** \class TetgenFacade
 *  \brief This class encapsulates the different calls to tetgen.
 */
class TetgenFacade{

public:
	/*------------------------------------------------------------------------*/
    /** \brief  Constructor.
     *
     *	\param ABoundary the outter boundary of the domain we want to mesh.
     */
	TetgenFacade();

	/*------------------------------------------------------------------------*/
    /** \brief  This method allows to tetrahedralize a volume defined by the
     * 			boundary mesh ABoundaryMesh. The result will be added in
     * 			AVolMesh. No volume points should be inserted (except Steiners' ?)
     *
     *
     *	\param ABoundaryMesh the outter boundary of the domain we want to mesh.
     *	\param AVolMesh		 the resulting volume mesh
     *	\param AQuality 	 defines a quality target size for each tetrahedron
     */
	void generateCDTMesh(gmds::Mesh& ABoundaryMesh,
						gmds::Mesh& AVolMesh,
						const double AQuality = 2.0);

	/*------------------------------------------------------------------------*/
    /** \brief  This method allows to tetrahedralize a volume defined by the
     * 			boundary mesh ABoundaryMesh. The result will be added in
     * 			AVolMesh. In order to get the expected AQuality, boundary nodes
     * 		    can be added.The radius-edge ratio must be included in [1.0,
     * 			2.0].
     *
     *
     *	\param ABoundaryMesh the outter boundary of the domain we want to mesh.
     *	\param AVolMesh		 the resulting volume mesh
     *	\param AQuality 	 defines a quality target size for each tetrahedron
     */
	void generateTetMesh(gmds::Mesh& ABoundaryMesh,
						gmds::Mesh& AVolMesh,
						const double AQuality = 2.0);
;
/*------------------------------------------------------------------------*/
    /** \brief  This method allows to tetrahedralize a volume defined by the
     * 			boundary mesh ABoundaryMesh. The result will be added in
     * 			AVolMesh. In order to get the expected AQuality, boundary nodes
     * 		    can be added.The radius-edge ratio must be included in [1.0,
     * 			2.0].
     *
     *
     *	\param ABoundaryMesh the outter boundary of the domain we want to mesh.
     *	\param AVolMesh		 the resulting volume mesh
     *	\param AQuality 	 defines a quality target size for each tetrahedron
     */
	void generateTetMeshDistTet(gmds::Mesh& ABoundaryMesh,
						gmds::Mesh& AVolMesh,
						const double AQuality = 2.0);


	void generateTetMesh(std::vector<gmds::Node>& ANodes,
						 std::vector< std::vector<gmds::Face> >& AFacets,
						 gmds::Mesh& AVolMesh,
						 const std::string&  AParam = "Yq1.2pa0.1");//QYpq1.414a0.1");

	void generateTetMesh(std::vector<gmds::Node>& ANodes,
						 std::vector< std::vector<gmds::Face> >& AFacets,
						 std::vector<gmds::Node>& ACreatedNodes,
						 std::vector<gmds::Region>& ACreatedRegions,
						 gmds::Mesh& AVolMesh,
						 const std::string&  AParam = "Yq1.2pa0.1");


	void buildTetgenInput(std::vector<gmds::Node>& ANodes,
			std::vector< std::vector<gmds::Face> >& AFacets);
private:

	/*------------------------------------------------------------------------*/
    /** \brief  Initializes the tetgen internal data structure from AMesh, a
     * 			GMDS mesh given in parameter
     *
     *	\param AMesh the input mesh
     */
	void buildTetgenInput(gmds::Mesh& AMesh);

	/*------------------------------------------------------------------------*/
    /** \brief  Build AMesh from the tetgenOutput_ data structure
     *
     *	\param AMesh the input mesh
     */
	void buildGMDSOutput(gmds::Mesh& AMesh);
	void fillGMDSOutput(gmds::Mesh& AMesh, std::vector<gmds::Node>& ANodes);

	void fillGMDSOutput(gmds::Mesh& AMesh,
			std::vector<gmds::Node>& ANodes,
			std::vector<gmds::Node>& ACreatedNodes,
			 std::vector<gmds::Region>& ACreatedRegions);

private:
	tetgenio tetgenInput_;
	tetgenio tetgenOutput_;
};

/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
#endif /* TETGENFACADE_H_ */
/*----------------------------------------------------------------------------*/
