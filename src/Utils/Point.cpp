/*----------------------------------------------------------------------------*/
/*
 * \file Point.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 29 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/Spherical.h"
#include "Utils/Cylindrical.h"
#include <sys/types.h>
#include <TkUtil/Exception.h>
#include <cstdio>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
namespace Math {
/*----------------------------------------------------------------------------*/
Point::Point(const Spherical & s)
{
	double rho = s.getRho();
    double theta = s.getTheta();
    double phi = s.getPhi();
    m_x = rho*std::sin(theta*M_PI/180) * std::cos (phi*M_PI/180);
    m_y = rho*std::sin(theta*M_PI/180) * std::sin(phi*M_PI/180);
    m_z = rho*std::cos(theta*M_PI/180);
}
/*----------------------------------------------------------------------------*/
Point::Point(const Cylindrical & c)
{
	double rho = c.getRho();
    double phi = c.getPhi();
    m_x = rho*std::cos(phi*M_PI/180);
    m_y = rho*std::sin(phi*M_PI/180);
    m_z = c.getZ();
}
/*----------------------------------------------------------------------------*/

extern "C" Utils::Math::Point minDist3Droites(Utils::Math::Point & p1x, Utils::Math::Point & p2x,
        Utils::Math::Point & p1y, Utils::Math::Point & p2y,
        Utils::Math::Point & p1z, Utils::Math::Point & p2z)
{
    Utils::Math::Point pt;
    double dist = 1.0;

    // les parmètres de position des projections sur les lignes
    double l1 = 0.5;
    double l2 = 0.5;
    double l3 = 0.5;

    // calcul du point Pt initial / paramètres
    pt = intersection3Plans(p1x,p2x, p1y,p2y, p1z,p2z,l1,l2,l3,dist);

    uint niter = 0;
    double delta = 0.5; // variation pour les lambdas
    do {
        double tmp_dist;
        // on tente d'améliorer la distance suivant les 2 directions pour chacunes des droites

        pt = intersection3Plans(p1x,p2x, p1y,p2y, p1z,p2z,l1-delta,l2,l3,tmp_dist);
        if (tmp_dist<dist){
            l1=l1-delta;
            dist = tmp_dist;
        }

        pt = intersection3Plans(p1x,p2x, p1y,p2y, p1z,p2z,l1+delta,l2,l3,tmp_dist);
        if (tmp_dist<dist){
            l1=l1+delta;
            dist = tmp_dist;
        }

        pt = intersection3Plans(p1x,p2x, p1y,p2y, p1z,p2z,l1,l2-delta,l3,tmp_dist);
        if (tmp_dist<dist){
            l2=l2-delta;
            dist = tmp_dist;
        }

        pt = intersection3Plans(p1x,p2x, p1y,p2y, p1z,p2z,l1,l2+delta,l3,tmp_dist);
        if (tmp_dist<dist){
            l2=l2+delta;
            dist = tmp_dist;
        }

        pt = intersection3Plans(p1x,p2x, p1y,p2y, p1z,p2z,l1,l2,l3-delta,tmp_dist);
        if (tmp_dist<dist){
            l3=l3-delta;
            dist = tmp_dist;
        }

        pt = intersection3Plans(p1x,p2x, p1y,p2y, p1z,p2z,l1,l2,l3+delta,tmp_dist);
        if (tmp_dist<dist){
            l3=l3+delta;
            dist = tmp_dist;
        }

        delta/=2.0;
    } while (++niter<10 && dist>1.0e-13 && delta>1.0e-13);


    // calcul du point Pt final / paramètres
    pt = intersection3Plans(p1x,p2x, p1y,p2y, p1z,p2z,l1,l2,l3,dist);

    return pt;
}
//---------------------------------------------------------------------------------------
extern "C" Utils::Math::Point intersection3Plans(Utils::Math::Point & p1x, Utils::Math::Point & p2x,
        Utils::Math::Point & p1y, Utils::Math::Point & p2y,
        Utils::Math::Point & p1z, Utils::Math::Point & p2z,
        double l1, double l2, double l3, double & dist)
{
    double a[3][3];
    double b[3];

    Utils::Math::Point p1xp2x = p2x-p1x;
    Utils::Math::Point M1 = p1x+p1xp2x*l1;
    Utils::Math::Point p1yp2y = p2y-p1y;
    Utils::Math::Point M2 = p1y+p1yp2y*l2;
    Utils::Math::Point p1zp2z = p2z-p1z;
    Utils::Math::Point M3 = p1z+p1zp2z*l3;

    a[0][0] = p1xp2x.getX();
    a[1][0] = p1yp2y.getX();
    a[2][0] = p1zp2z.getX();
    a[0][1] = p1xp2x.getY();
    a[1][1] = p1yp2y.getY();
    a[2][1] = p1zp2z.getY();
    a[0][2] = p1xp2x.getZ();
    a[1][2] = p1yp2y.getZ();
    a[2][2] = p1zp2z.getZ();

    b[0] = Utils::Math::scaMul(p1xp2x,M1);
    b[1] = Utils::Math::scaMul(p1yp2y,M2);
    b[2] = Utils::Math::scaMul(p1zp2z,M3);

    // Resolution par pivot de Gauss
    uint i,j,k;

    for (i = 0; i < 2; i++) {
        int max_i = i;
        double max_v = std::fabs(a[max_i][i]);
        for (j = i+1; j < 3; j++)
            if (std::fabs(a[j][i]) > max_v) {
                max_i = j;
                max_v = std::fabs(a[j][i]);
            }

        if (max_i != i) {
            /* On inverse les deux lignes i et max_i */
            for (j = i; j < 3; j++) {
                double temp = a[i][j];
                a[i][j] = a[max_i][j];
                a[max_i][j] = temp;
            }

            double temp = b[i];
            b[i] = b[max_i];
            b[max_i] = temp;
        }

        /* On fait l'élimination des lignes */
        if (a[i][i] == 0.0) {
            TkUtil::UTF8String   messageErr (TkUtil::Charset::UTF_8);
            messageErr << "le pivot "<<(short)i<<", "<<(short)j<<" est nul";
            throw TkUtil::Exception(messageErr);
        }

        for (j = i + 1; j < 3; j++) {
            double coeff = a[j][i] / a[i][i];
            /* Je fais aussi pour a[j][i] pour verification */
            for (k = i; k < 3; ++k)
                a[j][k] -= a[i][k] * coeff;
            /* et le second membre */
            b[j] -= b[i] * coeff;
        }
    }

    /* Substitution, colonne par colonne */
    b[2] = b[2] / a[2][2];
    b[1] = (b[1] - b[2] * a[1][2]) / a[1][1];
    b[0] = (b[0] - b[1] * a[0][1] - b[2] * a[0][2]) / a[0][0];


    Utils::Math::Point Pt(b[0],b[1],b[2]);

    // la distances aux droites
    dist = (Pt-M1).norme2() + (Pt-M2).norme2() + (Pt-M3).norme2();

    return Pt;

}
/*----------------------------------------------------------------------------*/
extern "C" uint findNearestPoint(std::vector<Utils::Math::Point>& points, const Utils::Math::Point& pt)
{
	if (points.empty())
		throw TkUtil::Exception("findNearestPoint impossible avec points vide");

	uint ind = 0;
	double best_dist = points[0].length2(pt);
	for (uint i=0; i<points.size(); i++){
		double dist = points[i].length2(pt);
		if (dist<best_dist){
			best_dist = dist;
			ind = i;
		}
	}
	//std::cout<<"findNearestPoint sélectionne le point "<<points[ind]<<std::endl;
	return ind;
}
/*----------------------------------------------------------------------------*/
extern "C" Utils::Math::Point bezier4(const Utils::Math::Point& pt1,
		const Utils::Math::Point& pt2,
		const Utils::Math::Point& pt3,
		const Utils::Math::Point& pt4,
		const double & param)
{
	Utils::Math::Point p1 = bezier2(pt1, pt2, param);
	Utils::Math::Point p2 = bezier2(pt2, pt3, param);
	Utils::Math::Point p3 = bezier2(pt3, pt4, param);
	return bezier3(p1, p2, p3, param);
}
/*----------------------------------------------------------------------------*/
extern "C" Utils::Math::Point bezier3(const Utils::Math::Point& pt1,
		const Utils::Math::Point& pt2,
		const Utils::Math::Point& pt3,
		const double & param)
{
	Utils::Math::Point p1 = bezier2(pt1, pt2, param);
	Utils::Math::Point p2 = bezier2(pt2, pt3, param);
	return bezier2(p1, p2, param);
}
/*----------------------------------------------------------------------------*/
extern "C" Utils::Math::Point bezier2(const Utils::Math::Point& pt1,
		const Utils::Math::Point& pt2,
		const double & param)
{
	return pt1 + (pt2-pt1)*param;
}
/*----------------------------------------------------------------------------*/
} // end namespace Math
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

