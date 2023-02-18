import simulation


def save_as_legacy_vtk(filename: str,
                       header: str,
                       grid: simulation.Grid,
                       temperatures: [float, ]):
    file_header = f"# vtk DataFile Version 2.0"
    file_format = "ASCII"
    dataset_type = "DATASET UNSTRUCTURED_GRID\n"

    points = f"POINTS {len(grid.nodes)} double\n" \
             + "\n".join([f" ".join((str(node.x), str(node.y), "0")) for node in grid.nodes])

    cells = f"CELLS {len(grid.elements)} {len(grid.elements) * (len(grid.elements[0].nodes) + 1)}\n" \
            + "\n".join([
        f"{len(grid.elements[0].nodes)} " + " ".join([str(node.id_) for node in element.nodes])
        for element in grid.elements])

    cell_types = f"CELL_TYPES {len(grid.elements)}\n" \
                 + "\n".join(["9" for element in grid.elements])

    temperatures_ = f"""POINT_DATA {len(grid.nodes)}
SCALARS Temp double 1
LOOKUP_TABLE default
""" + "\n".join([str(temperature) for temperature in temperatures])

    data = "\n".join((file_header,
                      header,
                      file_format,
                      dataset_type,
                      points,
                      "",
                      cells,
                      "",
                      cell_types,
                      "",
                      temperatures_,
                      ))

    with open(f"{filename}.vtk", "w+") as file:
        file.write(data)
