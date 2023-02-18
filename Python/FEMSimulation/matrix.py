import copy
from typing import Self, assert_never

import scipy


class Matrix:
    @staticmethod
    def sum(matrices: [[[], ], ]) -> [[], ]:
        return Matrix([[sum(elems) for elems in zip(*rows)] for rows in zip(*matrices)])

    matrix: [[], ]

    def get_determinant(self):
        if len(self.matrix) == len(self.matrix[0]) == 2:
            return self.matrix[0][0] * self.matrix[1][1] - self.matrix[0][1] * self.matrix[1][0]
        else:
            raise NotImplementedError

    def get_vector(self, column_idx: int = 0) -> []:
        return [row[column_idx] for row in self.matrix]

    def transpose(self) -> Self:
        return Matrix(
            [[self.matrix[j][i] for j in range(len(self.matrix))] for i in range(len(self.matrix[0]))],
            assign_instead_of_copy=True)

    def inverse(self) -> Self:
        return Matrix(matrix=scipy.linalg.inv(self.matrix).tolist(),
                      assign_instead_of_copy=True)

    def multiply_by(self, matrix: Self = None, vector: [] = None, value: float = None, in_place: bool = False):
        if matrix is not None:
            return self.multiply_by_matrix(matrix=matrix)
        elif vector is not None:
            return self.multiply_by_vector(vector=vector)
        elif value is not None:
            return self.multiply(value=value, in_place=in_place)
        else:
            raise NotImplementedError()

    def multiply_by_matrix(self, matrix: [[]]) -> Self:
        if len(self.matrix[0]) != len(matrix):
            raise ValueError(f"Cannot multiply. len(self.matrix[0]):{len(self.matrix[0])} len(matrix):{len(matrix)}")

        return Matrix([
            [  # For result column
                sum(
                    [self.matrix[row_idx][idx] * matrix[idx][matrix_col_idx] for idx in
                     range(len(self.matrix[0]))])
                for matrix_col_idx, matrix_elem in enumerate(matrix[0])
            ] for row_idx, matrix_row in enumerate(self.matrix)
        ],
            assign_instead_of_copy=True)

    def multiply_first_vector_by_row(self, row: []) -> Self:
        return Matrix([[elem * vector[0] for elem in row] for vector in self.matrix],
                      assign_instead_of_copy=True)

    def multiply_by_vector(self, vector: []) -> Self:
        return Matrix(
            matrix=[[sum([row_elem * vector_elem for row_elem, vector_elem in zip(row, vector)])] for row in
                    self.matrix],
            assign_instead_of_copy=True)
        # return Matrix([sum([row[idx] * vector[idx] for row in self.matrix]) for idx in range(len(vector))],
        #               assign_instead_of_copy=True)

    def multiply(self, value: float, in_place=False) -> Self:
        if in_place:
            for row_idx in range(len(self.matrix)):
                for col_idx in range(len(self.matrix[0])):
                    self.matrix[row_idx][col_idx] *= value
            return self

        return Matrix([[elem * value for elem in row] for row in self.matrix],
                      assign_instead_of_copy=True)

    def __init__(self, matrix: [[], ] = None,
                 row: [] = None,
                 dimension: int | list[int, int] | tuple[int, int] = None, default_value=None,
                 assign_instead_of_copy=False):
        if matrix is not None:
            if assign_instead_of_copy:
                self.matrix = matrix
            else:
                self.matrix = copy.deepcopy(matrix)
        elif row is not None:
            if assign_instead_of_copy:
                self.matrix = [None]
                self.matrix[0] = row
            else:
                self.matrix = [row[:]]

        elif dimension is not None:
            try:
                dim_n, dim_m = dimension
            except TypeError:
                dim_n = dim_m = dimension
            if isinstance(dim_n, int):
                self.matrix = [[default_value for _ in range(dim_n)] for _ in range(dim_m)]
            else:
                raise ValueError("Matrix have to have exactly 2 dimensions.")
        else:
            raise ValueError("Cannot create matrix. Not enough parameters.")

    def __getitem__(self, idx):
        return self.matrix[idx]

    def __setitem__(self, idx, row):
        self.matrix[idx] = row

    def __add__(self, other: Self):
        return Matrix([[sum(elems) for elems in zip(*rows)] for rows in zip(self.matrix, other)],
                      assign_instead_of_copy=True)

    def __imul__(self, other: [int, float, [float, ], Self]):
        # int, float
        if isinstance(other, (int, float)):
            return self.multiply_by(value=other, in_place=True)
        # Matrix
        if isinstance(other, Matrix):
            return self.multiply_by(matrix=other)
        # iterable(int, float), eg.: tuple(int...), list(float...)
        if hasattr(other, "__iter__") and len(other) and isinstance(other[0], (int, float)):
            return self.multiply_by(vector=other, in_place=True)
        # some other iterable - try before error
        if hasattr(other, "__iter__") and len(other):
            return self.multiply_by(matrix=other, in_place=True)

        assert_never(other)

    def __mul__(self, other: [int, float, [float, ], Self]):
        # int, float
        if isinstance(other, (int, float)):
            return self.multiply_by(value=other)
        # Matrix
        if isinstance(other, Matrix):
            return self.multiply_by(matrix=other)
        # iterable(int, float), eg.: tuple(int...), list(float...)
        if hasattr(other, "__iter__") and len(other) and isinstance(other[0], (int, float)):
            return self.multiply_by(vector=other)
        # some other iterable - try before error
        if hasattr(other, "__iter__") and len(other):
            return self.multiply_by(matrix=other)

        assert_never(other)

    def __iadd__(self, other: Self):
        for row_idx in range(len(self.matrix)):
            for col_idx in range(len(self.matrix[0])):
                self.matrix[row_idx][col_idx] += other[row_idx][col_idx]
        return self

    def __truediv__(self, x: float) -> Self:
        return self.multiply_by(value=1. / x)

    def __repr__(self, precision=4, with_idx=False, with_min_max=False):
        if not self.matrix:
            return ""

        columns_integer_part_width = [max([len(str(int(elem))) for elem in row]) for row in self.transpose().matrix]

        idx_part_len: int = len(str(len(self.matrix[0])))

        repr_ = "\n".join([
            ("[" if not with_idx else f"{str(row_idx).rjust(idx_part_len)}: [")
            + ", ".join(
                [f"{elem:.{precision}f}".rjust(integer_len + 1 + precision)
                 for idx, (integer_len, elem) in enumerate(zip(columns_integer_part_width, row))])
            + ("]" if not with_min_max else "] ("
                                            f"{min(row):.{precision}f}".rjust(min(columns_integer_part_width))
                                            + ", "
                                            + f"{max(row):.{precision}f}".rjust(max(columns_integer_part_width))
                                            + ")")
            for row_idx, row in enumerate(self.matrix)]
        )

        if len(self.matrix) == 1:
            repr_ = f"[{repr_}]"

        return repr_

    def __len__(self):
        return len(self.matrix)

    def __neg__(self):
        return self.multiply_by(value=-1)

    def print(self, precision=4, with_idx=False, with_min_max=False):
        print(self.__repr__(precision=precision, with_idx=with_idx, with_min_max=with_min_max))
