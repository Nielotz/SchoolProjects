import logging
import math
from typing import NamedTuple

# noinspection PyUnresolvedReferences
from grid.universal_element import UniversalElement
from material.material import Material
from matrix import Matrix


class Grid:
    class Element2D:
        class Node:
            def __init__(self, id_: int, x: float, y: float, bc: bool = False, t: float = None):
                self.id_: int = id_
                self.x: float = x
                self.y: float = y
                self.bc: bool = bc
                self.t: float = t

            def __dict__(self):
                return {
                    "id": self.id_,
                    "x": self.x,
                    "y": self.y,
                    "bc": self.bc,
                    "t": self.t
                }

            def __str__(self) -> str:
                return str(self.__dict__())

            def get_distance_to(self, node: "Grid.Element2D.Node"):
                return math.sqrt((self.x - node.x) ** 2 + (self.y - node.y) ** 2)

        class Edge(NamedTuple):
            edge_nodes: tuple["Grid.Element2D.Node", "Grid.Element2D.Node"]
            characteristic_coordinates: {str: float}
            bc: bool

            def calculate_length(self):
                node1, node2 = self.edge_nodes
                return node1.get_distance_to(node2)

            def debug_log(self, prefix: str = ""):
                logging.debug(f"{prefix}edge_nodes: {[edge.id_ for edge in self.edge_nodes]}")
                logging.debug(f"{prefix}characteristic_coordinates: {self.characteristic_coordinates}")
                logging.debug(f"{prefix}bc: {self.bc}")

        def calculate_transform_jacobians_to_universal_element(self, universal_element: UniversalElement.KsiEta):
            """ Prepare jacobian and vector (dns). """

            # [dx_d]
            nodes_xs = [node.x for node in self.nodes]
            nodes_ys = [node.y for node in self.nodes]

            dxs_dksi: [float, ] = universal_element.interpolate_using_derivatives_over_ksi(nodes_xs)
            dxs_deta: [float, ] = universal_element.interpolate_using_derivatives_over_eta(nodes_xs)
            dys_dksi: [float, ] = universal_element.interpolate_using_derivatives_over_ksi(nodes_ys)
            dys_deta: [float, ] = universal_element.interpolate_using_derivatives_over_eta(nodes_ys)

            # jacobian per point = [4 x jacobian]Matrix{[2][2]}
            jacobians = [Matrix(matrix=[
                [dy_deta, -dy_dksi],
                [-dx_deta, dx_dksi]
            ]) for (dx_dksi, dx_deta, dy_dksi, dy_deta) in zip(dxs_dksi, dxs_deta, dys_dksi, dys_deta)]

            return jacobians

        def __init__(self, nodes: [Node, ], material: Material):
            self.nodes = nodes
            self.material = material

            characteristic_coordinates = ({"y": -1}, {"x": 1}, {"y": 1}, {"x": -1})
            self.edges = [
                self.Edge(
                    edge_nodes=(nodes[node_idx], nodes[node_idx + 1]),
                    characteristic_coordinates=characteristic,
                    bc=nodes[node_idx].bc and nodes[node_idx + 1].bc
                ) for node_idx, characteristic in zip([*range(len(nodes) - 1), -1], characteristic_coordinates)]

            self.hbc: Matrix
            self.h: Matrix
            self.c: Matrix
            self.p: Matrix

        def debug_log(self, prefix: str = ""):
            logging.debug(f"{prefix}Element nodes:")
            for node in self.nodes:
                logging.debug(f"{prefix}\t{str(str(node))}")

    def __init__(self, nodes: [Element2D.Node, ], elements: [[int, ], ], material: Material):
        """
        :param nodes: Nodes of a grid.
        :param elements: List of elements description - node indices making one element.
        """
        self.nodes = nodes
        self.elements = [
            Grid.Element2D(
                [nodes[element_idx] for element_idx in element_indices],
                material=material
            ) for element_indices in elements]

    def debug_log(self, prefix: str = ""):
        logging.debug(f"{prefix}Grid (Nodes({len(self.nodes)})):")
        logging.debug(f"{prefix}\tElements({len(self.elements)}):")
        for element in self.elements:
            element.debug_log(f"{prefix}\t\t")

    def __dict__(self):
        return {
            "Nodes :": self.nodes,
            "Elements :": self.elements,
        }

    def __str__(self):
        return str(self.__dict__())
