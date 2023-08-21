// Gmsh - Copyright (C) 1997-2013 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.

#ifndef _HIGH_ORDER_H_
#define _HIGH_ORDER_H_

#include "GModel.h"
#include "MFace.h"

// for each pair of vertices (an edge), we build a list of vertices
// that are the high order representation of the edge. The ordering of
// vertices in the list is supposed to be (by construction) consistent
// with the ordering of the pair.
typedef std::map<std::pair<MVertex*, MVertex*>, std::vector<MVertex*> > edgeContainer;

// for each face (a list of vertices) we build a list of vertices that
// are the high order representation of the face
typedef std::map<MFace, std::vector<MVertex*>, Less_Face> faceContainer;

void SetOrder1(GModel *m, bool onlyVisible = false);
void SetOrderN(GModel *m, int order, bool linear=true, bool incomplete=false, bool onlyVisible = false);
void ElasticAnalogy ( GModel *m, double threshold, bool onlyVisible);
void SetHighOrderComplete (GModel *m, bool onlyVisible); // generate complete elements
void SetHighOrderInComplete (GModel *m, bool onlyVisible); // generate in-complete elements
MTriangle* setHighOrder(MTriangle *t, GFace *gf, 
                        edgeContainer &edgeVertices, 
                        faceContainer &faceVertices, 
                        bool linear, bool incomplete, int nPts = 1);
void checkHighOrderTriangles(const char* cc, GModel *m, 
                             std::vector<MElement*> &bad, double &minJGlob);

struct distanceFromMeshToGeometry_t {
  std::map<GEntity*, double> d_max, d2;
};

void computeDistanceFromMeshToGeometry (GModel *m, distanceFromMeshToGeometry_t &dist);
void getMeshInfoForHighOrder (GModel *gm,    int &meshOrder,   bool &complete,  bool &CAD);
#endif
