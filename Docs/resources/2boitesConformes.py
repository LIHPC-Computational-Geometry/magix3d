# -*- coding: utf-8 -*-
import sys
import pyMagix3D as Mgx3D
ctx = Mgx3D.getStdContext()

# Création d'une boite avec une topologie
ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1.2, 1), 10, 12, 10, "Mat1")
# Création d'une boite avec une topologie
ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(1.5, 1.2, 1), 10, 12, 10, "Mat2")
# Collage entre géométries avec topologies
ctx.getGeomManager().glue(["Vol0000", "Vol0001"])
# Changement de discrétisation pour Ar0022
ctx.getTopoManager().setParallelMeshingProperty (Mgx3D.EdgeMeshingPropertyGeometric(13,1.2),"Ar0022")
# Création du maillage pour tous les blocs
ctx.getMeshManager().newAllBlocksMesh()
# Sauvegarde du maillage (mli)
ctx.getMeshManager().writeMli("deuxBoites.mli2")