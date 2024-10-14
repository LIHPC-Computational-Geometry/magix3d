#!/home/oudotmp/wksMagix3D/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.4.0/python-3.10.10-fuacqaszpri3cx7zwcz3x4b3m6oup2sv/bin/python3.10
# -*- coding: utf-8 -*-

# Logiciel               : Magix3D
# Version                : 2.3.4
# Système d'exploitation : Linux 5.15.0
# Auteur                 : oudotmp
# Date                   : 04/07/2024 14:12:45
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

# Création du sommet Pt0000
ctx.getGeomManager().newVertex (Mgx3D.Point(0, 0, 0))
# Création du sommet Pt0001
ctx.getGeomManager().newVertex (Mgx3D.Point(1, 0, 0))
# Création du sommet Pt0002
ctx.getGeomManager().newVertex (Mgx3D.Point(1, 2, 0))
# Création du sommet Pt0003
ctx.getGeomManager().newVertex (Mgx3D.Point(0, 2, 0))
# Création du segment Crb0000
ctx.getGeomManager().newSegment("Pt0000", "Pt0001")
# Création du segment Crb0001
ctx.getGeomManager().newSegment("Pt0001", "Pt0002")
# Création du segment Crb0002
ctx.getGeomManager().newSegment("Pt0002", "Pt0003")
# Création du segment Crb0003
ctx.getGeomManager().newSegment("Pt0003", "Pt0000")
# Création de la surface Surf0000
ctx.getGeomManager().newPlanarSurface( ["Crb0000", "Crb0001", "Crb0002", "Crb0003"] , "")
# Création d'une face topologique structurée sur une géométrie (Surf0000)
ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0000")
# Découpage de l'arête Ar0000
ctx.getTopoManager().splitEdge ("Ar0000", .4)
# Changement de discrétisation pour les arêtes Ar0002
emp = Mgx3D.EdgeMeshingPropertyUniform(3)
ctx.getTopoManager().setMeshingProperty (emp,["Ar0002"])
# Découpage de la face Fa0000 par prolongation
# Erreur interne avec Edge::sortCoEdges()
ctx.getTopoManager().extendSplitFace ("Fa0000", "Som0004")