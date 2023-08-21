/*----------------------------------------------------------------------------*/
/*
 * Constants.h
 *
 *  Created on: 22 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_UTILS_CONSTANTS_H_
#define MGX3D_UTILS_CONSTANTS_H_
/*----------------------------------------------------------------------------*/
#include <cmath>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
class Portion{
public:
    enum Type{
        HUITIEME=45,
        CINQUIEME=72,
        SIXIEME=60,
        QUART=90,
		TIERS=120,
        DEMI=180,
        TROIS_QUARTS=270,
        PLEIN=360,
        ENTIER=360,
        ANGLE_DEF=366
    };

    static std::string getName(Type t){
        std::string name = "Mgx3D.Portion.";
        switch(t){
        case(HUITIEME):
            name+="HUITIEME";
            break;
        case(CINQUIEME):
            name+="CINQUIEME";
            break;
        case(SIXIEME):
            name+="SIXIEME";
            break;
        case(QUART):
            name+="QUART";
            break;
        case(TIERS):
            name+="TIERS";
            break;
       case(DEMI):
            name+="DEMI";
            break;
        case(TROIS_QUARTS):
            name+="TROIS_QUARTS";
            break;
        case(ANGLE_DEF):
            name+="ANGLE_DEF";
            break;
        default: // par defaut, on fait une rotation axiale complete
            name+="ENTIER";
            break;
        }
        return name;
    }

   static double getAngleInRadian(Type t) {
        double angle;
        switch(t){
            case Portion::ENTIER:
                angle=2*M_PI;
                break;
            case Portion::CINQUIEME:
                angle=2*M_PI/5;
                break;
            case Portion::SIXIEME:
                angle=2*M_PI/6;
                break;
            case Portion::DEMI:
                angle = M_PI;
                break;
            case Portion::TIERS:
                angle = M_PI/3;
                break;
            case Portion::QUART:
                angle = M_PI/2;
                break;
            case Portion::TROIS_QUARTS:
                angle = 3*M_PI/2;
                break;
            case Portion::HUITIEME:
                angle = M_PI/4;
                break;
            default: // par defaut, on fait une rotation axiale complete
                angle =2*M_PI;
                break;
        }
        return angle;
    }

    static double getAngleInDegree(Type t) {
        double angle;
        switch(t){
            case Portion::ENTIER:
                angle= 360;
                break;
            case Portion::CINQUIEME:
                angle= 72;
                break;
            case Portion::SIXIEME:
                angle= 60;
                break;
            case Portion::DEMI:
                angle = 180;
                break;
            case Portion::TIERS:
                angle = 120;
                break;
            case Portion::QUART:
                angle = 90;
                break;
            case Portion::TROIS_QUARTS:
                angle = 270;
                break;
            case Portion::HUITIEME:
                angle = 45;
                break;
            default: // par defaut, on fait une rotation axiale complete
                angle = 360;
                break;
        }
        return angle;
    }
};/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_UTILS_CONSTANTS_H_ */
/*----------------------------------------------------------------------------*/


