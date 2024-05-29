# -*- coding: utf-8 -*-
import sys
import pyMagix3D as Mgx3D
ctx = Mgx3D.getStdContext()

# Création d'une boite avec une topologie
ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1.2, 1), 10, 12, 10, "Mat1")
# Création d'une boite avec une topologie
ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(1.5, 1, 1), 7, 10, 10, "Mat2")
# Découpage de la face Fa0001
ctx.getTopoManager().splitFace ("Fa0001", "Ar0005", Mgx3D.Point(1, 1, 1), True)
# Collage entre géométries avec topologies
ctx.getGeomManager().glue(["Vol0000", "Vol0001"])
# Création du maillage pour tous les blocs
ctx.getMeshManager().newAllBlocksMesh()
# Sauvegarde du maillage (mli)
ctx.getMeshManager().writeMli("deuxBoitesRP.mli2")