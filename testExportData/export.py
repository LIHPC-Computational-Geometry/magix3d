import os
import sys
import pyMagix3D as Mgx3D
from collections import defaultdict

def generer_script(ctx):
    script = """# Script généré automatiquement pour recréer les entités Magix3D
# Ne pas modifier manuellement

"""
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    script += "import pyMagix3D as Mgx3D\n"
    script += "import itertools\n"
    script += "from collections import defaultdict\n\n"

    script += "ctx = Mgx3D.getStdContext()\n"
    script += "ctx.clearSession()\n"
    script += "gm = ctx.getGeomManager()\n"
    script += "tm = ctx.getTopoManager()\n"
    script += "gpm = ctx.getGroupManager()\n"
    script += "\n"

    script += f"# Anciens points ({len(gm.getVertices())} points) : \n"
    script += "points = {}\n"
    for vertex in gm.getVertices():
        coord = gm.getCoord(vertex)
        groups = gm.getInfos(vertex, 0).groups()
        if 'Hors_Groupe_0D' in groups:
            groups.remove('Hors_Groupe_0D')
        script += f"points['{vertex}'] = ({coord}, {groups})\n"
    script += "\n" 

    script += f"# Anciennes courbes ({len(gm.getCurves())} courbes) : \n"
    script += "curves = {}\n"
    for curve in gm.getCurves():
        vertices = gm.getInfos(curve, 1).vertices()
        groups = gm.getInfos(curve, 1).groups()
        if 'Hors_Groupe_1D' in groups:
            groups.remove('Hors_Groupe_1D')
        script += f"curves['{curve}'] = ({vertices}, {groups})\n"
    script += "\n" 

    script += f"# Anciennes surfaces ({len(gm.getSurfaces())} surfaces) : \n"
    script += "surfaces = {}\n"
    for surface in gm.getSurfaces():
        curves = gm.getInfos(surface, 2).curves()
        groups = gm.getInfos(surface, 2).groups()
        if 'Hors_Groupe_2D' in groups:
            groups.remove('Hors_Groupe_2D')
        script += f"surfaces['{surface}'] = ({curves}, {groups})\n"
    script += "\n" 

    script += f"# Anciens volumes ({len(gm.getVolumes())} volumes) : \n"
    script += "volumes = {}\n"
    for volume in gm.getVolumes():
        surfaces = gm.getInfos(volume, 3).surfaces()
        groups = gm.getInfos(volume, 3).groups()
        #if 'Hors_Groupe_3D' in groups:
        #    groups.remove('Hors_Groupe_3D')
        script += f"volumes['{volume}'] = ({surfaces}, {groups})\n"
    script += "\n" 

    script += f"# Anciennes arêtes ({len(tm.getCoEdges())} arêtes) : \n"
    script += "coedges = {}\n"
    for coedge in tm.getCoEdges():
        vertices = tm.getInfos(coedge, 1).vertices()
        nb = tm.getNbMeshingEdges(coedge)
        script += f"coedges['{coedge}'] = ({vertices}, {nb})\n"
    script += "\n" 

    script += "# Changement d'unité de longueur\n"
    script += "ctx.setLengthUnit(Mgx3D.Unit.centimeter)\n"
    script += "# Import BREP\n"
    script += f"gm.importBREP(\"{brep_file_name}\", True, False)\n\n"
    
    script += "points_mapping = defaultdict(list)\n"
    script += "for old_point, (old_coord, old_groups) in points.items():\n"
    script += "    for point in gm.getVertices():\n"
    script += "        coord = gm.getCoord(point)\n"
    script += "        if coord.getX() == old_coord[0] and coord.getY() == old_coord[1] and coord.getZ() == old_coord[2]:\n"
    script += "            points_mapping[old_point].append(point)\n"
    script += "            if old_groups != []:\n"
    script += "                for group in old_groups:\n"
    script += "                    gm.addToGroup ([point], 0, group)\n"
    script += "print(\"Correspondance des points:\")\n"
    script += "print(points_mapping, '\\n')\n\n"

    script += "curves_mapping = defaultdict(list)\n"
    script += "for old_curve, (old_points, old_groups) in curves.items():\n"
    script += "    new_points = []\n"
    script += "    for point in old_points:\n"
    script += "        if point in points_mapping:\n"
    script += "            new_points.append(points_mapping[point])\n"
    script += "        else:\n"
    script += "            raise ValueError(f\"Le point {point} n'a pas de correspondance dans points_mapping.\")\n"
    script += "    for point_combination in itertools.product(*new_points):\n"
    script += "        for curve in gm.getCurves():\n"
    script += "            vertices = gm.getInfos(curve, 1).vertices()\n"
    script += "            if set(vertices) == set(point_combination):\n"
    script += "                curves_mapping[old_curve].append(curve)\n"
    script += "                if old_groups != []:\n"
    script += "                     for group in old_groups:\n"
    script += "                         gm.addToGroup ([curve], 1, group)\n"
    script += "print(\"Correspondance des courbes:\")\n"
    script += "print(curves_mapping, '\\n')\n\n"

    script += "surfaces_mapping = defaultdict(list)\n"
    script += "for old_surface, (old_curves, old_groups) in surfaces.items():\n"
    script += "    new_curves = []\n"
    script += "    for curve in old_curves:\n"
    script += "        if curve in curves_mapping:\n"
    script += "            new_curves.append(curves_mapping[curve])\n"
    script += "        else:\n"
    script += "            raise ValueError(f\"La courbe {curve} n'a pas de correspondance dans curves_mapping.\")\n"
    script += "    for curve_combination in itertools.product(*new_curves):\n"
    script += "         for surface in gm.getSurfaces():\n"
    script += "            curves = gm.getInfos(surface, 2).curves()\n"
    script += "            if set(curves) == set(curve_combination):\n"
    script += "                 surfaces_mapping[old_surface].append(surface)\n"
    script += "                 if old_groups != []:\n"
    script += "                    for group in old_groups:\n"
    script += "                        gm.addToGroup ([surface], 2, group)\n"
    script += "print(\"Correspondance des surfaces:\")\n"
    script += "print(surfaces_mapping, '\\n')\n\n"

    script += "volumes_mapping = defaultdict(list)\n"
    script += "for old_volume, (old_surfaces, old_groups) in volumes.items():\n"
    script += "    new_surfaces = []\n"
    script += "    for surface in old_surfaces:\n"
    script += "        if surface in surfaces_mapping:\n"
    script += "            new_surfaces.append(surfaces_mapping[surface])\n"
    script += "        else:\n"
    script += "            raise ValueError(f\"La surface {surface} n'a pas de correspondance dans surfaces_mapping.\")\n"
    script += "    for surface_combination in itertools.product(*new_surfaces):\n"
    script += "        for volume in gm.getVolumes():\n"
    script += "            surfaces = gm.getInfos(volume, 3).surfaces()\n"
    script += "            if set(surfaces) == set(surface_combination):\n"
    script += "                volumes_mapping[old_volume].append(volume)\n"
    script += "                if old_groups != []:\n"
    script += "                    for group in old_groups:\n"
    script += "                        gm.addToGroup ([volume], 3, group)\n"
    script += "print(\"Correspondance des volumes:\")\n"
    script += "print(volumes_mapping, '\\n')\n\n"

    script += "def find_in_dicts(key):\n"
    script += "    for d in points_mapping, curves_mapping, surfaces_mapping, volumes_mapping:\n"
    script += "            if key in d:\n"
    script += "                # Renvoyer la première valeur trouvée (suppose que la valeur est une liste)\n"
    script += "                return d[key][0]\n"
    script += "    # Si la clé n'est pas trouvée dans aucun dictionnaire\n"
    script += "    return None\n\n"

    script += "# Création des sommets\n"
    script += "vertices_mapping = {}\n"
    for vertex in tm.getVertices():
        script += f'# Création du sommet {vertex}\n'
        coord = tm.getCoord(vertex)
        script += f'cmd = tm.newTopoVertex(Mgx3D.Point({coord.getX()}, {coord.getY()}, {coord.getZ()}), "")\n'
        script += 'v = cmd.getTopoVertices()[0]\n'
        groups = tm.getInfos(vertex, 0).groups()
        if 'Hors_Groupe_0D' in groups:
            groups.remove('Hors_Groupe_0D')
        if groups != []:
            for group in groups:
                script += f"tm.addToGroup ([v], 0, \"{group}\")\n"
        geom_entity = tm.getInfos(vertex, 0).geom_entity
        if geom_entity:
            script += f"# Le sommet {vertex} est associé à l'entité géométrique {geom_entity}\n"
            script += f"tm.setGeomAssociation([v], find_in_dicts('{geom_entity}'), False)\n"
        script += f'vertices_mapping["{vertex}"] = v\n'
    script += "print(\"Correspondance des sommets:\")\n"
    script += "print(vertices_mapping, '\\n')\n\n"
    script += "\n"

    script += "# Création des arêtes\n"
    script += "coedges_mapping = {}\n"
    for edge in tm.getCoEdges():
        script += f'# Création de l\'arête {edge}\n'
        start_vertex = tm.getInfos(edge, 1).vertices()[0]
        end_vertex = tm.getInfos(edge, 1).vertices()[1]
        script += f'cmd = tm.newTopoEntity([vertices_mapping["{start_vertex}"], vertices_mapping["{end_vertex}"]], 1, "")\n'
        script += 'e = cmd.getEdges()[0]\n'
        groups = tm.getInfos(edge, 1).groups()
        if 'Hors_Groupe_1D' in groups:
            groups.remove('Hors_Groupe_1D')
        if groups != []:
            for group in groups:
                script += f"tm.addToGroup ([e], 1, \"{group}\")\n"
        geom_entity = tm.getInfos(edge, 1).geom_entity
        if geom_entity:
            script += f"# L'arête {edge} est associée à l'entité géométrique {geom_entity}\n"
            script += f"tm.setGeomAssociation([e], find_in_dicts('{geom_entity}'), False)\n"
        script += f'coedges_mapping["{edge}"] = e\n'
    script += "print(\"Correspondance des arêtes:\")\n"
    script += "print(coedges_mapping, '\\n')\n\n"
    script += "\n"

    script += "# Création des cofaces\n"
    script += "cofaces_mapping = {}\n"
    edge_id = 0
    for coface in tm.getCoFaces():
        script += f'# Création de la coface {coface}\n'
        script += f'print("Création de la coface {coface}")\n'
        edges_list = tm.getInfos(coface, 2).edges()
        script += "coface_edges = []\n"
        hasHole = False
        for edge in edges_list:
            v1 = tm.getInfos(edge, 1).vertices()[0]
            v2 = tm.getInfos(edge, 1).vertices()[1]
            if (v1 == v2):
                hasHole = True
            coedges = []
            for coedge in tm.getInfos(edge, 1).coedges():
                coedges.append(f'coedges_mapping["{coedge}"]')
            script += f"cmd = tm.newEdge(vertices_mapping['{v1}'], vertices_mapping['{v2}'], [{', '.join(coedges)}])\n"
            script += f"coface_edges.append('Edge{edge_id:04d}')\n"
            edge_id += 1
        coface_vertices = [f'vertices_mapping["{vertex}"]' for vertex in tm.getInfos(coface, 2).vertices()]
        isStructured = "True" if len(coface_vertices) == 4 else "False"
        # Utiliser tm.getUnstructuredFaces(coface) pour vérifier si la coface est structurée ou non
        script += f'cmd = tm.newCoFace(coface_edges, [{", ".join(coface_vertices)}],{isStructured}, {hasHole})\n'
        script += 'f = cmd.getFaces()[0]\n'
        groups = tm.getInfos(coface, 2).groups()
        if 'Hors_Groupe_2D' in groups:
            groups.remove('Hors_Groupe_2D')
        if groups != []:
            for group in groups:
                script += f"tm.addToGroup ([f], 2, \"{group}\")\n"
        geom_entity = tm.getInfos(coface, 2).geom_entity
        if geom_entity:
            script += f"# La coface {coface} est associée à l'entité géométrique {geom_entity}\n"
            script += f"tm.setGeomAssociation([f], find_in_dicts('{geom_entity}'), False)\n"
        script += f'cofaces_mapping["{coface}"] = f\n'
    script += "print(\"Correspondance des cofaces:\")\n"
    script += "print(cofaces_mapping, '\\n')\n\n"
    script += "\n"

    script += "# Création des faces\n"
    script += "faces_mapping = {}\n"
    faces = set()
    faces_vertices_map = {}
    for coface in tm.getCoFaces():
        for face in tm.getInfos(coface, 2).faces():
            faces.add(face)
            faces_vertices_map['-'.join(sorted(tm.getInfos(face, 2).vertices()))] = face
    sorted_faces = sorted(faces)
    print("Tableau de faces avec leurs sommets")
    for(v, f) in faces_vertices_map.items():
        print(f"Face {f} has vertices {v}")
    face_id = 0
    blocks_faces_0 = defaultdict(list)
    for face in sorted_faces:
            face_cofaces = []
            for coface in tm.getCoFaces():
                if face in tm.getInfos(coface, 2).faces():
                    face_cofaces.append(f'cofaces_mapping["{coface}"]')
            script += f'# Création de la face {face}\n'
            face_vertices = [f'vertices_mapping["{vertex}"]' for vertex in tm.getInfos(face, 2).vertices()]
            script += f'cmd = tm.newFace([{", ".join(face_cofaces)}], [{", ".join(face_vertices)}], {"True" if len(face_vertices)==4 else "False"})\n'
            script += f'faces_mapping["{face}"] = "Face{face_id:04d}"\n'
            face_id += 1
            blocks_faces_0[tm.getInfos(face, 2).blocks()[0]].append(face)
    script += "print(\"Correspondance des faces:\")\n"
    script += "print(faces_mapping, '\\n')\n\n"
    script += "\n"

    # On cherche ici à obtenir les faces de chaque bloc dans le bon ordre 
    # (on ne peut pas l'obtenir avec l'API Python)
    blocks_faces = defaultdict(list)
    tabIndVtxByFaceOnBlock = ((4, 0, 2, 6),
    (5, 1, 3, 7),
    (4, 0, 1, 5),
    (6, 2, 3, 7),
    (2, 0, 1, 3),
    (6, 4, 5, 7)
    )
    for block in tm.getBlocks():
        print(f"Block {block} has vertices {tm.getInfos(block, 3).vertices()}")
        block_vertices = tm.getInfos(block, 3).vertices()
        if (len(block_vertices) == 8):
            for idxs in tabIndVtxByFaceOnBlock:
                face_vertices_key = '-'.join(sorted([block_vertices[i] for i in idxs]))
                print("On cherche la face ayant pour sommets ", face_vertices_key, " -> ", faces_vertices_map[face_vertices_key])
                if face_vertices_key in faces_vertices_map:
                    blocks_faces[block].append(faces_vertices_map[face_vertices_key])
        if blocks_faces[block]:
            print(f"Le bloc {block} a pour faces {blocks_faces[block]} et pour sommets {tm.getInfos(block, 3).vertices()}")
        else:
            print(f"Le bloc {block} a pour faces {blocks_faces_0[block]} et pour sommets {tm.getInfos(block, 3).vertices()}")
    # Si le bloc est dégéné
    
    script += "# Création des blocs\n"
    script += "blocks_mapping = {}\n"
    for block in tm.getBlocks():
        block_vertices = [f'vertices_mapping["{vertex}"]' for vertex in tm.getInfos(block, 3).vertices()]
        if blocks_faces[block]:
            script += f'# Création du bloc {block} ayant pour faces {blocks_faces[block]} et pour sommets {tm.getInfos(block, 3).vertices()}\n'
            block_faces = [f'faces_mapping["{face}"]' for face in blocks_faces[block]]
            # Utiliser tm.getUnstructuredBlocks() pour vérifier si le block est structuré ou non
            script += f'cmd = ctx.getTopoManager().newBlock([{", ".join(block_faces)}], [{", ".join(block_vertices)}], {"True" if len(block_vertices)==4 else "False"}, "")\n'
        else:
            script += f'# Création du bloc {block} ayant pour faces {blocks_faces_0[block]} et pour sommets {tm.getInfos(block, 3).vertices()}\n'
            block_faces = [f'faces_mapping["{face}"]' for face in blocks_faces_0[block]]
            script += f'cmd = ctx.getTopoManager().newBlock([{", ".join(block_faces)}], [{", ".join(block_vertices)}], {"True" if len(block_vertices)==4 else "False"}, "")\n'
        script += 'b = cmd.getBlocks()[0]\n'
        geom_entity = tm.getInfos(block, 3).geom_entity
        if geom_entity:
            script += f"# Le bloc {block} est associé à l'entité géométrique {geom_entity}\n"
            script += f"tm.setGeomAssociation([b], find_in_dicts('{geom_entity}'), False)\n"
        script += f'blocks_mapping["{block}"] = b\n'
    script += "print(\"Correspondance des blocs:\")\n"
    script += "print(blocks_mapping, '\\n')\n\n"

    script += "# Definition de la discrétisation des aretes\n"
    script += 'for old_coedge, (old_vertices, nb_meshing_edges) in coedges.items():\n'
    script += f'    emp = Mgx3D.EdgeMeshingPropertyUniform(nb_meshing_edges)\n'
    script += f"    tm.setMeshingProperty (emp, [coedges_mapping[old_coedge]])\n"

    script += "\n# Création du maillage pour tous les blocs\n"
    script += "mm = ctx.getMeshManager()\n"
    script += "mm.newAllBlocksMesh()\n"

    return script

if __name__ == "__main__":
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    
    if len(sys.argv) < 2:
        print("Usage: python export.py <nom_du_script>")
        sys.exit(1)

    script = sys.argv[1]  # Récupère le nom du script à exécuter
    script_file = os.path.abspath(script)
    path = os.path.dirname(script_file)
    file = os.path.basename(script_file)

    try:
        with open(script_file, "r") as script_file:
            code = script_file.read()
            # Exécuter le script dans le contexte global actuel
            exec(code)

            brep_file_name = f"{path}/{file.replace('.py', '_generated.brep')}"
            gm.exportBREP ([], brep_file_name)

            generated_script = generer_script(ctx)
            generated_script_file_name = f"{path}/{file.replace('.py', '_generated.py')}"
            # Sauvegarde du script généré
            with open(generated_script_file_name, "w") as f:
                f.write(generated_script)

            print(f"Script généré avec succès : {generated_script_file_name}")

    except FileNotFoundError:
        print(f"Erreur : Le fichier {script_file} n'existe pas.")
    except Exception as e:
        print(f"Erreur lors de l'exécution : {e}")
