#!/ccc/products/nec/bin/maillage_python
# -*- coding: iso-8859-15 -*-

# Logiciel               : Magix3D
# Version                : 1.7.11
# Système d'exploitation : Linux 2.6.32
# Auteur                 : brieree
# Date                   : 7/11/2017 

import sys


# Décommenter la ligne suivante pour utiliser la version qui a servi pour générer ce script, et commenter les 2 lignes d'après
# sys.path.append("/ccc/cont001/dsku/pegase/home/home_app_S/necdist/necdist/produits/RedHat_6_x86_64/binding/pyMagix3D-1.7.11-icpc.12.1.1-Python_2.7-Swig_1.3")
sys.path.append("/ccc/products/nec/share/packages")
import maillage

import pyMagix3D as Mgx3D
ctx = Mgx3D.getStdContext()

# info sur la version
print "Version Magix3D : ", Mgx3D.getVersion()

# Import Mdl (géom et topo)
ctx.getTopoManager().importMDL("/ccc/cont001/dsku/pegase/home/home_app_S/magix3d/magix3d/tutorial/cavite.mdl",False, False)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0003", .5, .5)
# Construction Topo et Geom 3D avec o-grid par révolution
ctx.getTopoManager().makeBlocksByRevol (["Ar0023"], Mgx3D.Portion.ENTIER)
# Création du cylindre Vol0004
ctx.getGeomManager().newCylinder (Mgx3D.Point(5, 0, 3), 1, Mgx3D.Vector(0, 0, 2), 3.600000e+02, "CUT_TEL")
# Section par un plan entre géométries avec topologies
ctx.getGeomManager().section(["Vol0002"], "Surf0018")
# Destruction de  Vol0004
ctx.getGeomManager().destroy(["Vol0004"], True)
# Modifie le groupe TROU
ctx.getGeomManager().setGroup (["Vol0005"], 3, "TROU")
# Changement de discrétisation pour les blocs Bl0001 Bl0003
ctx.getTopoManager().setMeshingProperty (Mgx3D.BlockMeshingPropertyTransfinite(),["Bl0001", "Bl0003"])
# Découpage en O-grid des blocs structurés Bl0001 Bl0003
ctx.getTopoManager().splitBlocksWithOgrid (["Bl0001", "Bl0003"], ["Fa0036"], .3, 10)
# Affectation d'une projection vers Crb0037 pour les entités topologiques Som0062 Som0070 Som0063 Som0055
ctx.getTopoManager().setGeomAssociation (["Som0062", "Som0070", "Som0063", "Som0055"], "Crb0037", True)
# Affectation d'une projection vers Crb0038 pour les entités topologiques Som0059 Som0067 Som0066 Som0058
ctx.getTopoManager().setGeomAssociation (["Som0059", "Som0067", "Som0066", "Som0058"], "Crb0038", True)
# Création d'un repère suivant un centre
ctx.getSysCoordManager().newSysCoord (Mgx3D.Point(5, 0, 3), "TROU")

# Modification de la position des sommets topologiques Som0063 Som0066 Som0074 Som0075 en coordonnées cylindriques dans le repère Rep0000
ctx.getTopoManager().setVertexCylindricalLocation (["Som0063", "Som0066", "Som0074", "Som0075"], False, 0, True, -45, False, 0, "Rep0000")
# Modification de la position des sommets topologiques Som0055 Som0058 Som0071 Som0072 en coordonnées cylindriques dans le repère Rep0000
ctx.getTopoManager().setVertexCylindricalLocation (["Som0055", "Som0058", "Som0071", "Som0072"], False, 0, True, 45, False, 0, "Rep0000")
# Modification de la position des sommets topologiques Som0062 Som0059 Som0073 en coordonnées cylindriques dans le repère Rep0000
ctx.getTopoManager().setVertexCylindricalLocation (["Som0062", "Som0059", "Som0073"], False, 0, True, 135, False, 0, "Rep0000")
# Modification de la position des sommets topologiques Som0070 Som0067 Som0076 en coordonnées cylindriques dans le repère Rep0000
ctx.getTopoManager().setVertexCylindricalLocation (["Som0070", "Som0067", "Som0076"], False, 0, True, -135, False, 0, "Rep0000")

# Modification de la position des sommets topologiques Som0074 Som0071 en coordonnées cylindriques dans le repère Rep0000
ctx.getTopoManager().setVertexCylindricalLocation (["Som0074", "Som0071"], True, 1.2, False, 0, True, .72, "Rep0000")
ctx.getTopoManager().setVertexCylindricalLocation (["Som0076", "Som0073", "Som0072", "Som0075"], True, 1, False, 0, True, .5, "Rep0000")

# Extraction de blocs dans TMP_TROU
ctx.getTopoManager().extract (["Bl0022"], "TMP_TROU")
# Projection automatique pour les arêtes topologiques Ar0165 Ar0157 Ar0166 Ar0161 Ar0159 ... 
ctx.getTopoManager().projectEdgesOnCurves (["Ar0165", "Ar0157", "Ar0166", "Ar0161", "Ar0159", "Ar0163", "Ar0155", "Ar0164"])
# Découpage de l'arête Ar0160
ctx.getTopoManager().splitEdge ("Ar0160", .7)
# Découpage de l'arête Ar0156
ctx.getTopoManager().splitEdge ("Ar0156", .7)
# Découpage de l'arête Ar0162
ctx.getTopoManager().splitEdge ("Ar0162", .7)
# Découpage de l'arête Ar0158
ctx.getTopoManager().splitEdge ("Ar0158", .7)
# Découpage de l'arête Ar0190
ctx.getTopoManager().splitEdge ("Ar0190", .4286)
# Découpage de l'arête Ar0192
ctx.getTopoManager().splitEdge ("Ar0192", .4286)
# Découpage de l'arête Ar0194
ctx.getTopoManager().splitEdge ("Ar0194", .5714)
# Découpage de l'arête Ar0196
ctx.getTopoManager().splitEdge ("Ar0196", .5714)
# Changement de discrétisation pour les arêtes Ar0191 Ar0198 Ar0200 Ar0193 Ar0204 ... 
emp = Mgx3D.EdgeMeshingPropertyGeometric(10,1.2)
ctx.getTopoManager().setMeshingProperty (emp,["Ar0191", "Ar0198", "Ar0200", "Ar0193", "Ar0204", "Ar0197", "Ar0195", "Ar0202"])
# Changement de discrétisation pour les arêtes Ar0205 Ar0203 Ar0199 Ar0201
emp = Mgx3D.EdgeMeshingPropertyUniform(4)
ctx.getTopoManager().setMeshingProperty (emp,["Ar0205", "Ar0203", "Ar0199", "Ar0201"])
# Découpage de la face Fa0120 par prolongation
ctx.getTopoManager().extendSplitFace ("Fa0120", "Som0085")
# Découpage de la face Fa0135 par prolongation
ctx.getTopoManager().extendSplitFace ("Fa0135", "Som0089")
# Découpage de la face Fa0116 par prolongation
ctx.getTopoManager().extendSplitFace ("Fa0116", "Som0086")
# Découpage de la face Fa0138 par prolongation
ctx.getTopoManager().extendSplitFace ("Fa0138", "Som0090")
# Découpage de la face Fa0117 par prolongation
ctx.getTopoManager().extendSplitFace ("Fa0117", "Som0085")
# Découpage de la face Fa0142 par prolongation
ctx.getTopoManager().extendSplitFace ("Fa0142", "Som0089")
# Découpage de la face Fa0121 par prolongation
ctx.getTopoManager().extendSplitFace ("Fa0121", "Som0091")
# Découpage de la face Fa0146 par prolongation
ctx.getTopoManager().extendSplitFace ("Fa0146", "Som0087")
# Changement de discrétisation pour les arêtes  Ar0197 Ar0195 Ar0191 Ar0193
ctx.getTopoManager().reverseDirection (["Ar0197", "Ar0195", "Ar0191", "Ar0193"])
# Homothétie de la topologie seule  Som0086 Som0090 Som0089 Som0085 Som0092 ... 
ctx.getTopoManager().scale (["Som0086", "Som0090", "Som0089", "Som0085", "Som0092", "Som0088", "Som0087", "Som0091"], 1.1, Mgx3D.Point(5, 0, 4.25), False)
# Découpage en O-grid des blocs structurés Bl0030
ctx.getTopoManager().splitBlocksWithOgrid (["Bl0030"], ["Fa0131", "Fa0130"], .9, 10)
# Changement de discrétisation pour Ar0183
emp = Mgx3D.EdgeMeshingPropertyUniform(4)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0183")
# Fusion entre faces Fa0136 et Fa0132
ctx.getTopoManager().fuse2Faces ("Fa0136","Fa0132")
# Fusion entre faces Fa0134 et Fa0160
ctx.getTopoManager().fuse2Faces ("Fa0134","Fa0160")
# Fusion entre faces Fa0137 et Fa0158
ctx.getTopoManager().fuse2Faces ("Fa0137","Fa0158")
# Fusion entre faces Fa0141 et Fa0128
ctx.getTopoManager().fuse2Faces ("Fa0141","Fa0128")
# Fusion entre faces Fa0139 et Fa0152
ctx.getTopoManager().fuse2Faces ("Fa0139","Fa0152")
# Fusion entre faces Fa0140 et Fa0150
ctx.getTopoManager().fuse2Faces ("Fa0140","Fa0150")
# Fusion entre faces Fa0149 et Fa0133
ctx.getTopoManager().fuse2Faces ("Fa0149","Fa0133")
# Fusion entre faces Fa0147 et Fa0161
ctx.getTopoManager().fuse2Faces ("Fa0147","Fa0161")
# Fusion entre faces Fa0148 et Fa0159
ctx.getTopoManager().fuse2Faces ("Fa0148","Fa0159")
# Fusion entre faces Fa0143 et Fa0156
ctx.getTopoManager().fuse2Faces ("Fa0143","Fa0156")
# Fusion entre faces Fa0145 et Fa0129
ctx.getTopoManager().fuse2Faces ("Fa0145","Fa0129")
# Fusion entre faces Fa0144 et Fa0154
ctx.getTopoManager().fuse2Faces ("Fa0144","Fa0154")
# Fusion entre faces Fa0118 et Fa0164
ctx.getTopoManager().fuse2Faces ("Fa0118","Fa0164")
# Affectation d'une projection vers Vol0005 pour les entités topologiques Bl0035
ctx.getTopoManager().setGeomAssociation (["Bl0035"], "Vol0005", False)
# Affectation d'une projection vers Vol0003 pour les entités topologiques Bl0033 Bl0031 Bl0034 Bl0032
ctx.getTopoManager().setGeomAssociation (["Bl0033", "Bl0031", "Bl0034", "Bl0032"], "Vol0003", False)
# Modifie le groupe TMP_TROU
ctx.getTopoManager().removeFromGroup (["Bl0035", "Bl0032", "Bl0033", "Bl0034", "Bl0031"], 3, "TMP_TROU")
# Projection automatique pour les arêtes topologiques Ar0227 Ar0223 Ar0225 Ar0229
ctx.getTopoManager().projectEdgesOnCurves (["Ar0227", "Ar0223", "Ar0225", "Ar0229"])
# Projection automatique pour les faces topologiques Fa0166 Fa0163 Fa0162 Fa0167
ctx.getTopoManager().projectFacesOnSurfaces (["Fa0166", "Fa0163", "Fa0162", "Fa0167"])
# Affectation d'une projection vers Surf0023 pour les entités topologiques Fa0165
ctx.getTopoManager().setGeomAssociation (["Fa0165"], "Surf0021", True)
# Affectation d'une projection vers Surf0022 pour les entités topologiques Fa0118
ctx.getTopoManager().setGeomAssociation (["Fa0118"], "Surf0022", True)
# Changement de discrétisation pour les arêtes Ar0206
emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, ["Ar0165"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar0206"])
# Changement de discrétisation pour les arêtes Ar0207 Ar0206
emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, ["Ar0165"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar0207", "Ar0206"])
# Changement de discrétisation pour les arêtes Ar0212 Ar0213
emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, ["Ar0166"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar0212", "Ar0213"])
# Changement de discrétisation pour les arêtes Ar0210 Ar0211
emp = Mgx3D.EdgeMeshingPropertyInterpolate(20, ["Ar0161"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar0210", "Ar0211"])
# Changement de discrétisation pour les arêtes Ar0208 Ar0209
emp = Mgx3D.EdgeMeshingPropertyInterpolate(20, ["Ar0157"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar0208", "Ar0209"])
# Création du maillage pour tous les blocs
ctx.getMeshManager().newAllBlocksMesh()
# Sauvegarde du maillage (mli)
ctx.getMeshManager().writeMli("cavite_tel.mli")
