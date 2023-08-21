// Gmsh - Copyright (C) 1997-2013 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.

#ifndef _MESH_GFACE_DELAUNAY_INSERTIONFACE_H_
#define _MESH_GFACE_DELAUNAY_INSERTIONFACE_H_

#include "MTriangle.h"
#include "MQuadrangle.h"
#include "STensor3.h"
#include "GEntity.h"
#include <list>
#include <set>
#include <map>

class GModel;
class GFace;
class BDS_Mesh;
class BDS_Point;

struct bidimMeshData 
{
  std::map<MVertex*,int> indices;
  std::vector<double> Us, Vs, vSizes, vSizesBGM;
  std::vector<SMetric3> vMetricsBGM;
  std::map<MVertex* , MVertex*>* equivalence;
  std::map<MVertex*, SPoint2> * parametricCoordinates;
  std::set<MEdge,Less_Edge> internalEdges; // embedded edges 
  inline void addVertex (MVertex* mv, double u, double v, double size, double sizeBGM){
    int index = Us.size();
    if (mv->onWhat()->dim() == 2)mv->setIndex(index);
    else indices[mv] = index;
    if (parametricCoordinates){
      std::map<MVertex*, SPoint2>::iterator it =  parametricCoordinates->find(mv);
      if (it != parametricCoordinates->end()){
	u = it->second.x();
	v = it->second.y();
	//	printf("%g %g\n",u,v);
      }
    }
    Us.push_back(u);
    Vs.push_back(v);
    vSizes.push_back(size);
    vSizesBGM.push_back(sizeBGM);
  }
  inline int getIndex (MVertex *mv) {
    if (mv->onWhat()->dim() == 2)return mv->getIndex();
    return indices[mv];
  }
  inline MVertex * equivalent (MVertex *v1) const {
    if (equivalence){
      std::map<MVertex* , MVertex*>::iterator it = equivalence->find(v1);
      if (it == equivalence->end())return 0;
      return it->second;
    }
    return 0;
  }
  bidimMeshData (std::map<MVertex* , MVertex*>* e = 0, std::map<MVertex*, SPoint2> *p = 0) : equivalence(e), parametricCoordinates(p)  
  {
  }
};


void buildMetric(GFace *gf, double *uv, double *metric);
int inCircumCircleAniso(GFace *gf, double *p1, double *p2, double *p3, 
                        double *p4, double *metric);
int inCircumCircleAniso(GFace *gf, MTriangle *base, const double *uv, 
                        const double *metric, bidimMeshData & data);
void circumCenterMetric(double *pa, double *pb, double *pc, const double *metric,
                        double *x, double &Radius2);
void circumCenterMetric(MTriangle *base, const double *metric, bidimMeshData & data,
                        double *x, double &Radius2);
bool circumCenterMetricInTriangle(MTriangle *base, const double *metric, bidimMeshData &data);
bool invMapUV(MTriangle *t, double *p, bidimMeshData &data,
              double *uv, double tol);

class MTri3
{
 protected :
  bool deleted;
  double circum_radius;
  MTriangle *base;
  MTri3 *neigh[3];

 public :
  static int radiusNorm; // 2 is euclidian norm, -1 is infinite norm  
  bool isDeleted() const { return deleted; }
  void forceRadius(double r) { circum_radius = r; }
  inline double getRadius() const { return circum_radius; }

  MTri3(MTriangle *t, double lc, SMetric3 *m = 0, bidimMeshData * data = 0, GFace *gf = 0);
  inline MTriangle *tri() const { return base; }
  inline void  setNeigh(int iN , MTri3 *n) { neigh[iN] = n; }
  inline MTri3 *getNeigh(int iN ) const { return neigh[iN]; }
  int inCircumCircle(const double *p) const;
  inline int inCircumCircle(double x, double y) const
  {
    const double p[2] = {x, y};
    return inCircumCircle(p);
  }
  inline int inCircumCircle(const MVertex * v) const
  {
    return inCircumCircle(v->x(), v->y());
  }
  inline void setDeleted(bool d){ deleted = d; }
  inline bool assertNeigh() const
  {
    if(deleted) return true;
    for(int i = 0; i < 3; i++)
      if(neigh[i] && (neigh[i]->isNeigh(this) == false)) return false;
    return true;
  }
  inline bool isNeigh(const MTri3 *t) const
  {
    for(int i = 0; i < 3; i++)
      if(neigh[i] == t) return true;
    return false;
  }
};

class compareTri3Ptr
{
 public:
  inline bool operator () (const MTri3 *a, const MTri3 *b)  const
  {
    if(a->getRadius() > b->getRadius()) return true;
    if(a->getRadius() < b->getRadius()) return false;
    return a<b;
  }
};

void connectTriangles(std::list<MTri3*> &);
void connectTriangles(std::vector<MTri3*> &);
void connectTriangles(std::set<MTri3*,compareTri3Ptr> &AllTris);
void bowyerWatson(GFace *gf, int MAXPNT= 1000000000,
		  std::map<MVertex* , MVertex*>* equivalence= 0,  
		  std::map<MVertex*, SPoint2> * parametricCoordinates= 0);
void bowyerWatsonFrontal(GFace *gf,
		  std::map<MVertex* , MVertex*>* equivalence= 0,  
		  std::map<MVertex*, SPoint2> * parametricCoordinates= 0);
void bowyerWatsonFrontalLayers(GFace *gf, bool quad,
		  std::map<MVertex* , MVertex*>* equivalence= 0,  
		  std::map<MVertex*, SPoint2> * parametricCoordinates= 0);
void bowyerWatsonParallelograms(GFace *gf,
		  std::map<MVertex* , MVertex*>* equivalence= 0,  
		  std::map<MVertex*, SPoint2> * parametricCoordinates= 0);
void buildBackGroundMesh (GFace *gf,
		  std::map<MVertex* , MVertex*>* equivalence= 0,  
		  std::map<MVertex*, SPoint2> * parametricCoordinates= 0);

struct edgeXface
{
  MVertex *v[2];
  MTri3 * t1;
  int i1;
  edgeXface(MTri3 *_t, int iFac) : t1(_t), i1(iFac)
  {
    v[0] = t1->tri()->getVertex(iFac == 0 ? 2 : iFac-1);
    v[1] = t1->tri()->getVertex(iFac);
    std::sort(v, v + 2);
  }
  inline bool operator < ( const edgeXface &other) const
  {
    if(v[0] < other.v[0]) return true;
    if(v[0] > other.v[0]) return false;
    if(v[1] < other.v[1]) return true;
    return false;
  }
  inline bool operator == ( const edgeXface &other) const
  {
    if(v[0] == other.v[0] && v[1] == other.v[1]) return true;
    return false;
  }
};


#endif
