// Gmsh - Copyright (C) 1997-2013 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.
//
// Contributor(s):
//   Tristan Carrier François Henrotte

#ifndef _DIRECTION_3D_H_
#define _DIRECTION_3D_H_

#include "GFace.h"
#include "MEdge.h"
#include "MElementOctree.h"
#if defined(HAVE_ANN)
#include <ANN/ANN.h>
#endif
#include "yamakawa.h"
#include "Matrix.h"

struct lowerThan {
  bool operator() (const std::pair<int, Matrix>& lhs, const std::pair<int, Matrix>& rhs) const
  {return lhs.first < rhs.first;}
};

class Frame_field{
 private:
  static std::map<MVertex*, Matrix> temp;
  static std::vector<std::pair<MVertex*, Matrix> > field;
  static std::map<int, Matrix> crossField;
  //static std::map<MVertex*, Matrix, MVertexLessThanNum> crossField;
  static std::map<MEdge, double, Less_Edge> crossDist;
#if defined(HAVE_ANN)
  static ANNpointArray duplicate;
  static ANNkd_tree* kd_tree;
  static ANNpointArray annTreeData;
  static ANNkd_tree* annTree;
  static std::vector<int> vertIndices;
#endif
  Frame_field();
 public:
  static void init_region(GRegion*);
  static void init_face(GFace*);
  static bool translate(GFace*,MElementOctree*,MVertex*,SPoint2,SVector3&,SVector3&);
  static bool improved_translate(GFace*,MVertex*,SVector3&,SVector3&);
  static Matrix search(double,double,double);
  static Matrix combine(double,double,double);
  static bool inside_domain(MElementOctree*,double,double);
  static double get_ratio(GFace*,SPoint2);
  static void print_field1();
  static void print_field2(GRegion*);
  static void print_segment(SPoint3,SPoint3,double,double,std::ofstream&);
  static Recombinator crossData;
  static void init(GRegion* gr);
  static double smooth(GRegion* gr);
  static double findBarycenter(std::map<MVertex*, std::set<MVertex*> >::const_iterator iter, Matrix &m0);
  static void fillTreeVolume(GRegion* gr);
  static Matrix findNearestCross(double x,double y,double z);
  static void save(GRegion* gr, const std::string &filename);
  static void save_energy(GRegion* gr, const std::string& filename);
  static void save_dist(const std::string& filename);
  static GRegion* test();
  static void clear();
};

class Size_field{
 private:
  static std::map<MVertex*,double> boundary;
  static MElementOctree* octree;
  Size_field();
 public:
  static void init_region(GRegion*);
  static void solve(GRegion*);
  static double search(double,double,double);
  static double get_ratio(GFace*,SPoint2);
  static void print_field(GRegion*);
  static GRegion* test();
  static void clear();
};

class Nearest_point{
 private:
  static std::vector<SPoint3> field;
  static std::vector<MElement*> vicinity;
#if defined(HAVE_ANN)
  static ANNpointArray duplicate;
  static ANNkd_tree* kd_tree;
#endif
  Nearest_point();
 public:
  static void init_region(GRegion*);
  static bool search(double,double,double,SVector3&);
  static double T(double,double,double,double,double);
  static SPoint3 closest(MElement*,SPoint3);
  static double clamp(double,double,double);
  static void print_field(GRegion*);
  static void print_segment(SPoint3,SPoint3,std::ofstream&);
  static GRegion* test();
  static void clear();
};

#endif
