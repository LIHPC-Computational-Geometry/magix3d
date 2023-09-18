Synthèse des commandes de l'API Python
**************************************

Cette annexe donne un lien entre les noms des fonctions Python associées
aux commandes vers la documentation du panneau associé.

Pour alléger l’écriture, nous utilisons les alias Python suivants:

.. code-block:: python

    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

.. code-block:: python
    :caption: :ref:`creation-point`

    gm.newVertex

.. code-block:: python
    :caption: :ref:`creation-segment`

    gm.newSegment

.. code-block:: python
    :caption: :ref:`creation-arc-cercle`

    gm.newArcCircle

.. code-block:: python
    :caption: :ref:`creation-cercle`

    gm.newCircle

.. code-block:: python
    :caption: :ref:`creation-courbe-par-projection`

    gm.newCurveByCurveProjectionOnSurface

.. code-block:: python
    :caption: :ref:`collage-courbes`

    gm.joinCurves

.. code-block:: python
    :caption: :ref:`creation-surface`

    gm.newPlanarSurface

.. code-block:: python
    :caption: :ref:`creation-par-revolution`

    gm.makeRevol

.. code-block:: python
    :caption: :ref:`copie-surface-distance`

    gm.newSurfaceByOffset

.. code-block:: python
    :caption: :ref:`collage-surfaces`

    gm.joinSurfaces

.. code-block:: python
    :caption: :ref:`intersection-surfaces`

    gm.sectionByPlane

.. code-block:: python
    :caption: :ref:`creation-boite`

    gm.newBox
    gm.newBoxWithTopo

.. code-block:: python
    :caption: :ref:`creation-sphere`

    gm.newSphere
    gm.newSphereWithTopo
    gm.newHollowSphere
    gm.newHollowSphereWithTopo

.. code-block:: python
    :caption: :ref:`creation-cylindre`

    gm.newCylinder
    gm.newCylinderWithTopo
    gm.newHollowCylinder
    gm.newHollowCylinderWithTopo

.. code-block:: python
    :caption: :ref:`creation-cone`

    gm.newCone
    gm.newConeWithTopo


.. code-block:: python
    :caption: :ref:`creation-aiguille`

    gm.newSpherePart
    gm.newSpherePartWithTopo
    gm.newHollowSpherePart
    gm.newSpherePartWithTopo

.. code-block:: python
    :caption: :ref:`creation-prisme` 

    @deprecated gm.newPrism 
    @deprecated gm.newPrismWithTopo 
    gm.makeExtrude
    gm.makeBlocksByExtrude

.. code-block:: python
    :caption: :ref:`creation-volume`

    gm.newVolume

.. code-block:: python
    :caption: :ref:`operations-booleennes`

    gm.glue
    gm.fuse
    gm.common
    gm.cut
    gm.section

.. code-block:: python
    :caption: :ref:`copie-entites-geometriques`

    gm.copy

.. code-block:: python
    :caption: :ref:`destruction-entites-geometriques`

    gm.destroy
    gm.destroyWithTopo

.. code-block:: python
    :caption: :ref:`ajout-groupe`

    gm.addToGroup

.. code-block:: python
    :caption: :ref:`rotation-entites-geometriques`

    gm.rotate
    gm.copyAndRotate
    gm.rotateAll
    gm.copyAndRotateAll

.. code-block:: python
    :caption: :ref:`homothetie-entites-geometriques`

    gm.scale
    gm.copyAndScale
    gm.scaleAll
    gm.copyAndScaleAll

.. code-block:: python
    :caption: :ref:`symetrie-entites-geometriques`

    gm.mirror
    gm.copyAndMirror
    gm.mirrorAll
    gm.copyAndMirrorAll

.. code-block:: python
    :caption: :ref:`fusion-sommets`

    tm.snapVertices

.. code-block:: python
    :caption: :ref:`positionnement-sommet`

    tm.setVertexLocation

.. code-block:: python
    :caption: :ref:`alignement-sommets`

    tm.alignVertices

.. code-block:: python
    :caption: :ref:`projection-sommets-geometrie`

    tm.snapProjectedVertices
    tm.snapAllProjectedVertices

.. code-block:: python
    :caption: :ref:`association-topo-geo`

    tm.setGeomAssociation
    tm.projectVerticesOnNearestGeomEntities
    tm.projectEdgesOnCurves
    tm.projectAllEdgesOnCurves
    tm.projectFacesOnSurfaces
    tm.projectAllFacesOnSurfaces

.. code-block:: python
    :caption: :ref:`decoupage-arete`

    tm.splitEdge

.. code-block:: python
    :caption: :ref:`raboutage-aretes`

    tm.fuseEdges

.. code-block:: python
    :caption: :ref:`fusion-aretes`

    tm.fuse2Edges

.. code-block:: python
    :caption: :ref:`parametrage-discretisation-aretes`

    tm.setMeshingProperty
    tm.setParallelMeshingProperty
    tm.setNbMeshingEdges

.. code-block:: python
    :caption: :ref:`parametrage-discretisation-faces`, :ref:`parametrage-discretisation-blocs`

    tm.setMeshingProperty

.. code-block:: python
    :caption: :ref:`inversion-discretisation`

    tm.reverseDirection

.. code-block:: python
    :caption: :ref:`creation-face-surface`

    tm.newStructuredTopoOnGeometry
    tm.newFreeTopoOnGeometry
    tm.newUnstructuredTopoOnGeometry

.. code-block:: python
    :caption: :ref:`decoupage-faces-arete`

    tm.splitFaces
    tm.splitAllFaces

.. code-block:: python
    :caption: :ref:`decoupage-face-arete`

    tm.splitFace

.. code-block:: python
    :caption: :ref:`decoupage-face-ogrid`

    tm.splitFacesWithOgrid

.. code-block:: python
    :caption: :ref:`prolongation-face-sommet`

    tm.extendSplitFace

.. code-block:: python
    :caption: :ref:`insertion-trou`

    tm.insertHole

.. code-block:: python
    :caption: :ref:`fusion-faces`

    tm.fuse2Faces

.. code-block:: python
    :caption: :ref:`creation-bloc-volume`

    tm.newStructuredTopoOnGeometry
    tm.newTopoOGridOnGeometry
    tm.newFreeTopoOnGeometry
    tm.newUnstructuredTopoOnGeometry
    tm.newInsertionTopoOnGeometry

.. code-block:: python
    :caption: :ref:`creation-bloc-revolution`

    tm.makeBlocksByRevol
    tm.makeBlocksByRevolWithRatioOgrid

.. code-block:: python
    :caption: :ref:`extract-blocs`

    tm.extract

.. code-block:: python
    :caption: :ref:`decoupage-blocs-arete`

    tm.splitBlock
    tm.splitAllBlocks

.. code-block:: python
    :caption: :ref:`decoupage-blocs-ogrid`

    tm.splitBlocksWithOgrid

.. code-block:: python
    :caption: :ref:`prolongation-decoupage-blocs`

    tm.extendSplitBlock

.. code-block:: python
    :caption: :ref:`collage-blocs`

    tm.glue2Blocks
    tm.glue2Topo

.. code-block:: python
    :caption: :ref:`fusion-blocs`

    tm.fuse2Blocks

.. code-block:: python
    :caption: :ref:`deraffinement-blocs`

    tm.unrefine

.. code-block:: python
    :caption: :ref:`destruction-entites-topologiques`

    tm.destroy

.. code-block:: python
    :caption: :ref:`translation-entites-topologiques`

    tm.translate

.. code-block:: python
    :caption: :ref:`rotation-entites-topologiques`

    tm.rotate

.. code-block:: python
    :caption: :ref:`homethetie-entites-topologiques`

    tm.scale

.. code-block:: python
    :caption: :ref:`symetrie-entites-topologiques`

    tm.mirror

.. code-block:: python
    :caption: :ref:`symetrie-entites-topologiques`

    mm.newFacesMesh
    mm.newAllFacesMesh

.. code-block:: python
    :caption: :ref:`mailler-surfaces`

    mm.newFacesMesh
    mm.newAllFacesMesh

.. code-block:: python
    :caption: :ref:`lisser-surfaces`, :ref:`lisser-volumes`

    mm.addSmoothing

.. code-block:: python
    :caption: :ref:`mailler-blocs`

    mm.newBlocksMesh
    mm.newAllBlocksMesh

.. code-block:: python
    :caption: :ref:`extraire-volume-feuillets`

    mm.newSubVolumeBetweenSheets
