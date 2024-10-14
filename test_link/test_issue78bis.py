#!/home/oudotmp/wksMagix3D/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.4.0/python-3.10.10-fuacqaszpri3cx7zwcz3x4b3m6oup2sv/bin/python3.10
# -*- coding: utf-8 -*-

# Logiciel               : Magix3D
# Version                : 2.3.4
# Système d'exploitation : Linux 5.15.0
# Auteur                 : oudotmp
# Date                   : 04/07/2024 14:13:07
# Version API TkUtil     : 6.8.0
# Version de Python      : 3.10.10
# Version de Swig        : 4.1.1
# Version API PythonUtil : 6.2.2
# Version de Qualif      : 2.3.6

# Version de OCC         : 7.1.0

# NE PAS MODIFIER LES LIGNES COMMENCANT PAR "!!!TAG".

import sys


# Vous avez la possibilité ici de sélectionner la version de Magix3D
# sys.path.append("/home/oudotmp/wksMagix3D/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.4.0/magix3d-999-dpwp54us4lxtb56wjr6ad5xjmarofwqr/lib/python3.10/site-packages")
# import maillage

import pyMagix3D as Mgx3D
ctx = Mgx3D.getStdContext()

# Dsactivation du dcalage automatique des noms en cas d'importation  la suite d'autres commandes
ctx.unactivateShiftingNameId()

# Création du sommet Pt0000
# ctx.getGeomManager().newVertex (Mgx3D.Point(0, 0, 0))
cmd0 = ctx.getGeomManager().newVertex (Mgx3D.Point(0, 0, 0))
# Création du sommet Pt0001
# ctx.getGeomManager().newVertex (Mgx3D.Point(1, 0, 0))
cmd1 = ctx.getGeomManager().newVertex (Mgx3D.Point(1, 0, 0))
# Création du sommet Pt0002
# ctx.getGeomManager().newVertex (Mgx3D.Point(1, 2, 0))
cmd2 = ctx.getGeomManager().newVertex (Mgx3D.Point(1, 2, 0))
# Création du sommet Pt0003
# ctx.getGeomManager().newVertex (Mgx3D.Point(0, 2, 0))
cmd3 = ctx.getGeomManager().newVertex (Mgx3D.Point(0, 2, 0))
# Création du segment Crb0000
# ctx.getGeomManager().newSegment("Pt0000", "Pt0001")
cmd4 = ctx.getGeomManager().newSegment(cmd0.getVertices()[0], cmd1.getVertices()[0])
# Création du segment Crb0001
# ctx.getGeomManager().newSegment("Pt0001", "Pt0002")
cmd5 = ctx.getGeomManager().newSegment(cmd1.getVertices()[0], cmd2.getVertices()[0])
# Création du segment Crb0002
# ctx.getGeomManager().newSegment("Pt0002", "Pt0003")
cmd6 = ctx.getGeomManager().newSegment(cmd2.getVertices()[0], cmd3.getVertices()[0])
# Création du segment Crb0003
# ctx.getGeomManager().newSegment("Pt0003", "Pt0000")
cmd7 = ctx.getGeomManager().newSegment(cmd3.getVertices()[0], cmd0.getVertices()[0])
# Création de la surface Surf0000
# ctx.getGeomManager().newPlanarSurface( ["Crb0000", "Crb0001", "Crb0002", "Crb0003"] , "")
cmd8 = ctx.getGeomManager().newPlanarSurface( [cmd4.getCurves()[0], cmd5.getCurves()[0], cmd6.getCurves()[0], cmd7.getCurves()[0]] , "")
# Création d'une face topologique structurée sur une géométrie (Surf0000)
# ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0000")
cmd9 = ctx.getTopoManager().newStructuredTopoOnGeometry (cmd8.getSurfaces()[0])
# Découpage de l'arête Ar0000
# ctx.getTopoManager().splitEdge ("Ar0000", .4)
cmd10 = ctx.getTopoManager().splitEdge (cmd9.getEdges()[0], .4)
# Changement de discrétisation pour les arêtes Ar0002
# emp = Mgx3D.EdgeMeshingPropertyUniform(3)
# ctx.getTopoManager().setMeshingProperty (emp,["Ar0002"])
cmd11 = emp = Mgx3D.EdgeMeshingPropertyUniform(3)
ctx.getTopoManager().setMeshingProperty (emp,[cmd9.getEdges()[2]])
