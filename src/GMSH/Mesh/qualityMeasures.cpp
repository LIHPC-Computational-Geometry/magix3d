// Gmsh - Copyright (C) 1997-2013 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.

#include "qualityMeasures.h"
#include "BDS.h"
#include "MVertex.h"
#include "MTriangle.h"
#include "MQuadrangle.h"
#include "MTetrahedron.h"
#include "Numeric.h"
#include "polynomialBasis.h"
#include "GmshMessage.h"
#include <limits>
#include <string.h>

double qmTriangle(const BDS_Point *p1, const BDS_Point *p2, const BDS_Point *p3,
                  const qualityMeasure4Triangle &cr)
{
  return qmTriangle(p1->X, p1->Y, p1->Z, p2->X, p2->Y, p2->Z, p3->X, p3->Y, p3->Z, cr);
}

double qmTriangle(BDS_Face *t, const qualityMeasure4Triangle &cr)
{
  BDS_Point *n[4];
  t->getNodes(n);
  return qmTriangle(n[0], n[1], n[2], cr);
}

double qmTriangle(MTriangle*t, const qualityMeasure4Triangle &cr)
{
  return qmTriangle(t->getVertex(0), t->getVertex(1), t->getVertex(2), cr);
}

double qmTriangle(const MVertex *v1, const MVertex *v2, const MVertex *v3,
                  const qualityMeasure4Triangle &cr)
{
  return qmTriangle(v1->x(), v1->y(), v1->z(), v2->x(), v2->y(), v2->z(),
                    v3->x(), v3->y(), v3->z(), cr);
}

// Triangle abc
// quality is between 0 and 1

double qmTriangle(const double &xa, const double &ya, const double &za,
                  const double &xb, const double &yb, const double &zb,
                  const double &xc, const double &yc, const double &zc,
                  const qualityMeasure4Triangle &cr)
{
  double quality;
  switch(cr){
  case QMTRI_RHO:
    {
      // quality = rho / R = 2 * inscribed radius / circumradius
      double a [3] = {xc - xb, yc - yb, zc - zb};
      double b [3] = {xa - xc, ya - yc, za - zc};
      double c [3] = {xb - xa, yb - ya, zb - za};
      norme(a);
      norme(b);
      norme(c);
      double pva [3]; prodve(b, c, pva); const double sina = norm3(pva);
      double pvb [3]; prodve(c, a, pvb); const double sinb = norm3(pvb);
      double pvc [3]; prodve(a, b, pvc); const double sinc = norm3(pvc);

      if (sina == 0.0 && sinb == 0.0 && sinc == 0.0) quality = 0.0;
      else quality = 2 * (2 * sina * sinb * sinc / (sina + sinb + sinc));
    }
    break;
    // condition number
  case QMTRI_COND:
    {
      /*
      double a [3] = {xc - xa, yc - ya, zc - za};
      double b [3] = {xb - xa, yb - ya, zb - za};
      double c [3] ; prodve(a, b, c); norme(c);
      double A[3][3] = {{a[0] , b[0] , c[0]} ,
                        {a[1] , b[1] , c[1]} ,
                        {a[2] , b[2] , c[2]}};
      */
      quality = -1;
    }
    break;
  default:
    Msg::Error("Unknown quality measure");
    return 0.;
  }

  return quality;
}

double qmTet(MTetrahedron *t, const qualityMeasure4Tet &cr, double *volume)
{
  return qmTet(t->getVertex(0), t->getVertex(1), t->getVertex(2), t->getVertex(3),
               cr, volume);
}

double qmTet(const MVertex *v1, const MVertex *v2, const MVertex *v3,
             const MVertex *v4, const qualityMeasure4Tet &cr, double *volume)
{
  return qmTet(v1->x(), v1->y(), v1->z(), v2->x(), v2->y(), v2->z(),
               v3->x(), v3->y(), v3->z(), v4->x(), v4->y(), v4->z(), cr, volume);
}

double qmTet(const double &x1, const double &y1, const double &z1,
             const double &x2, const double &y2, const double &z2,
             const double &x3, const double &y3, const double &z3,
             const double &x4, const double &y4, const double &z4,
             const qualityMeasure4Tet &cr, double *volume)
{
  switch(cr){
  case QMTET_ONE:
    return 1.0;
  case QMTET_3:
    {
      double mat[3][3];
      mat[0][0] = x2 - x1;
      mat[0][1] = x3 - x1;
      mat[0][2] = x4 - x1;
      mat[1][0] = y2 - y1;
      mat[1][1] = y3 - y1;
      mat[1][2] = y4 - y1;
      mat[2][0] = z2 - z1;
      mat[2][1] = z3 - z1;
      mat[2][2] = z4 - z1;
      *volume = fabs(det3x3(mat)) / 6.;
      double l = ((x2 - x1) * (x2 - x1) +
                  (y2 - y1) * (y2 - y1) +
                  (z2 - z1) * (z2 - z1));
      l += ((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1) + (z3 - z1) * (z3 - z1));
      l += ((x4 - x1) * (x4 - x1) + (y4 - y1) * (y4 - y1) + (z4 - z1) * (z4 - z1));
      l += ((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2) + (z3 - z2) * (z3 - z2));
      l += ((x4 - x2) * (x4 - x2) + (y4 - y2) * (y4 - y2) + (z4 - z2) * (z4 - z2));
      l += ((x3 - x4) * (x3 - x4) + (y3 - y4) * (y3 - y4) + (z3 - z4) * (z3 - z4));
      return 12. * pow(3 * fabs(*volume), 2. / 3.) / l;
    }
  case QMTET_2:
    {
      double mat[3][3];
      mat[0][0] = x2 - x1;
      mat[0][1] = x3 - x1;
      mat[0][2] = x4 - x1;
      mat[1][0] = y2 - y1;
      mat[1][1] = y3 - y1;
      mat[1][2] = y4 - y1;
      mat[2][0] = z2 - z1;
      mat[2][1] = z3 - z1;
      mat[2][2] = z4 - z1;
      *volume = fabs(det3x3(mat)) / 6.;
      double p0[3] = {x1, y1, z1};
      double p1[3] = {x2, y2, z2};
      double p2[3] = {x3, y3, z3};
      double p3[3] = {x4, y4, z4};
      double s1 = fabs(triangle_area(p0, p1, p2));
      double s2 = fabs(triangle_area(p0, p2, p3));
      double s3 = fabs(triangle_area(p0, p1, p3));
      double s4 = fabs(triangle_area(p1, p2, p3));
      double rhoin = 3. * fabs(*volume) / (s1 + s2 + s3 + s4);
      double l = sqrt((x2 - x1) * (x2 - x1) +
                      (y2 - y1) * (y2 - y1) +
                      (z2 - z1) * (z2 - z1));
      l = std::max(l, sqrt((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1) +
                           (z3 - z1) * (z3 - z1)));
      l = std::max(l, sqrt((x4 - x1) * (x4 - x1) + (y4 - y1) * (y4 - y1) +
                           (z4 - z1) * (z4 - z1)));
      l = std::max(l, sqrt((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2) +
                           (z3 - z2) * (z3 - z2)));
      l = std::max(l, sqrt((x4 - x2) * (x4 - x2) + (y4 - y2) * (y4 - y2) +
                           (z4 - z2) * (z4 - z2)));
      l = std::max(l, sqrt((x3 - x4) * (x3 - x4) + (y3 - y4) * (y3 - y4) +
                           (z3 - z4) * (z3 - z4)));
      return 2. * sqrt(6.) * rhoin / l;
    }
    break;
  case QMTET_COND:
    {
      /// condition number is defined as (see Knupp & Freitag in IJNME) 
      double INVW[3][3] = {{1,-1./sqrt(3.),-1./sqrt(6.)},{0,2/sqrt(3.),-1./sqrt(6.)},{0,0,sqrt(1.5)}};
      double A[3][3] = {{x2-x1,y2-y1,z2-z1},{x3-x1,y3-y1,z3-z1},{x4-x1,y4-y1,z4-z1}};
      double S[3][3],INVS[3][3];
      matmat(A,INVW,S);
      *volume = inv3x3(S,INVS) * 0.70710678118654762;//2/sqrt(2);
      double normS = norm2 (S);
      double normINVS = norm2 (INVS);
      return normS * normINVS;      
    }
  default:
    Msg::Error("Unknown quality measure");
    return 0.;
  }
}

/*
double conditionNumberAndDerivativeOfTet(const double &x1, const double &y1, const double &z1,
					 const double &x2, const double &y2, const double &z2,
					 const double &x3, const double &y3, const double &z3,
					 const double &x4, const double &y4, const double &z4){

  double INVW[3][3] = {{1,-1./sqrt(3.),-1./sqrt(6.)},{0,2/sqrt(3.),-1./sqrt(6.)},{0,0,sqrt(1.5)}};
      double A[3][3] = {{x2-x1,y2-y1,z2-z1},{x3-x1,y3-y1,z3-z1},{x4-x1,y4-y1,z4-z1}};
      double S[3][3],INVS[3][3];
      matmat(A,INVW,S);
      double sigma = inv3x3(S,INVS);
      double normS = norm2 (S);
      double normINVS = norm2 (INVS);
      conditionNumber = normS * normINVS;      
  
}
*/

double qmTriangleAngles (MTriangle *e) {
  double a = 500;
  double worst_quality = std::numeric_limits<double>::max();
  double mat[3][3];
  double mat2[3][3];
  double den = atan(a*(M_PI/9)) + atan(a*(M_PI/9));

  // This matrix is used to "rotate" the triangle to get each vertex
  // as the "origin" of the mapping in turn
  double rot[3][3];
  rot[0][0]=-1; rot[0][1]=1; rot[0][2]=0;
  rot[1][0]=-1; rot[1][1]=0; rot[1][2]=0;
  rot[2][0]= 0; rot[2][1]=0; rot[2][2]=1;
  double tmp[3][3];

  //double minAngle = 120.0;
  for (int i = 0; i < e->getNumPrimaryVertices(); i++) {
    const double u = i == 1 ? 1 : 0;
    const double v = i == 2 ? 1 : 0;
    const double w = 0;
    e->getJacobian(u, v, w, mat);
    e->getPrimaryJacobian(u,v,w,mat2);
    for (int j = 0; j < i; j++) {
      matmat(rot,mat,tmp);
      memcpy(mat, tmp, sizeof(mat));
    }
    //get angle
    double v1[3] = {mat[0][0],  mat[0][1],  mat[0][2] };
    double v2[3] = {mat[1][0],  mat[1][1],  mat[1][2] };
    double v3[3] = {mat2[0][0],  mat2[0][1],  mat2[0][2] };
    double v4[3] = {mat2[1][0],  mat2[1][1],  mat2[1][2] };
    norme(v1);
    norme(v2);
    norme(v3);
    norme(v4);
    double v12[3], v34[3];
    prodve(v1,v2,v12);
    prodve(v3,v4,v34);
    norme(v12);
    norme(v34);
    double orientation;
    prosca(v12,v34,&orientation);

    // If the triangle is "flipped" it's no good
    if (orientation < 0)
      return -std::numeric_limits<double>::max();

    double c;
    prosca(v1,v2,&c);
    double x = acos(c)-M_PI/3;
    //double angle = (x+M_PI/3)/M_PI*180;
    double quality = (atan(a*(x+M_PI/9)) + atan(a*(M_PI/9-x)))/den;
    worst_quality = std::min(worst_quality, quality);

    //minAngle = std::min(angle, minAngle);
    //printf("Angle %g ", angle);
    // printf("Quality %g\n",quality);
  }
  //printf("MinAngle %g \n", minAngle);
  //return minAngle;

  return worst_quality;
}


double qmQuadrangleAngles (MQuadrangle *e) {
  double a = 100;
  double worst_quality = std::numeric_limits<double>::max();
  double mat[3][3];
  double mat2[3][3];
  double den = atan(a*(M_PI/4)) + atan(a*(2*M_PI/4 - (M_PI/4)));

  // This matrix is used to "rotate" the triangle to get each vertex
  // as the "origin" of the mapping in turn
  //double rot[3][3];
  //rot[0][0]=-1; rot[0][1]=1; rot[0][2]=0;
  //rot[1][0]=-1; rot[1][1]=0; rot[1][2]=0;
  //rot[2][0]= 0; rot[2][1]=0; rot[2][2]=1;
  //double tmp[3][3];

  const double u[9] = {-1,-1, 1, 1, 0,0,1,-1,0};
  const double v[9] = {-1, 1, 1,-1, -1,1,0,0,0};

  for (int i = 0; i < 9; i++) {

    e->getJacobian(u[i], v[i], 0, mat);
    e->getPrimaryJacobian(u[i],v[i],0,mat2);
    //for (int j = 0; j < i; j++) {
    //  matmat(rot,mat,tmp);
    //  memcpy(mat, tmp, sizeof(mat));
    //}

    //get angle
    double v1[3] = {mat[0][0],  mat[0][1],  mat[0][2] };
    double v2[3] = {mat[1][0],  mat[1][1],  mat[1][2] };
    double v3[3] = {mat2[0][0],  mat2[0][1],  mat2[0][2] };
    double v4[3] = {mat2[1][0],  mat2[1][1],  mat2[1][2] };
    norme(v1);
    norme(v2);
    norme(v3);
    norme(v4);
    double v12[3], v34[3];
    prodve(v1,v2,v12);
    prodve(v3,v4,v34);
    norme(v12);
    norme(v34);
    double orientation;
    prosca(v12,v34,&orientation);

    // If the if the triangle is "flipped" it's no good
    //    if (orientation < 0)
    //      return -std::numeric_limits<double>::max();

    double c;
    prosca(v1,v2,&c);
    double x = fabs(acos(c))-M_PI/2;
    //double angle = fabs(acos(c))*180/M_PI;
    double quality = (atan(a*(x+M_PI/4)) + atan(a*(2*M_PI/4 - (x+M_PI/4))))/den;
    worst_quality = std::min(worst_quality, quality);
  }
  
  return worst_quality;

}

