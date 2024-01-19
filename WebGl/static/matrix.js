import {Debug} from "./debug.js";

export class Matrix {
    #data

    constructor(rows, cols, default_val) {
        this.#data = Array.from(
            {length: rows},
            () => Array.from(
                {length: cols},
                () => default_val
            )
        )
    }

    getSize() {
        const rows = this.#data.length;
        if (rows)
            return [rows, this.#data[0].length]
        return [0, 0]

    }

    addColumn(data, colIdx) {
        const size = this.getSize();
        if (size[0] === 0)
            return Debug.displayError("Trying to add column to null Matrix!");
        if (colIdx > size[1] - 1)
            return Debug.displayError(`Trying to add column at idx ${colIdx} into Matrix ${size}`);
        if (data.length !== size[1])
            Debug.displayError(`Trying to add column(${data.length}) into Matrix ${size}`);

        for (let rowIdx = 0; rowIdx < data.length; rowIdx++)
            this.#data[rowIdx][colIdx] += data[rowIdx];
    }

    multipled(other) {
        // https://stackoverflow.com/questions/27205018/multiply-2-matrices-in-javascript
        let aNumRows = this.#data.length, aNumCols = this.#data[0].length,
            bNumRows = other.#data.length, bNumCols = other.#data[0].length,
            m = new Array(aNumRows);  // initialize array of rows
        for (let r = 0; r < aNumRows; ++r) {
            m[r] = new Array(bNumCols); // initialize the current row
            for (let c = 0; c < bNumCols; ++c) {
                m[r][c] = 0;             // initialize the current cell
                for (let i = 0; i < aNumCols; ++i) {
                    m[r][c] += this.#data[r][i] * other.#data[i][c];
                }
            }
        }
        return Matrix.fromMatrix(m);
    }

    setCell(row, cell, val) {
        this.#data[row][cell] = val;
    }

    fill(value) {
        this.#data = this.#data.map(row => row.map(() => value))
        return this;
    }

    zero() {
        this.#data = this.#data.map(row => row.map(() => 0))
        return this;
    }

    one() {
        this.#data = this.#data.map(row => row.map(() => 1))
        return this;
    }

    static identity(size) {
        const matrix = new Matrix(size, size, 0);
        matrix.#data.forEach((row, row_idx) => matrix.#data[row_idx][row_idx] = 1);
        return matrix;
    }

    static getSquared(size) {
        return new Matrix(size, size, null);
    }

    multiplyDiagonal(val) {
        const [rows, cols] = this.getSize();
        if (rows !== cols)
            return Debug.displayError("Cannot multiplyDiagonal of non square matrix.");
        if (val.length > rows)
            return Debug.displayError(
                `Cannot multiplyDiagonal of matrix ${rows}x${cols} with a vector of length ${val.length}.`);
        if (val.length < rows)
            Debug.displayError(`MultiplyDiagonal of matrix ${rows}x${cols} with a vector of length ${val.length}.`);

        val.forEach((value, idx) => this.#data[idx][idx] *= value);
        return this;
    }

    static fromMatrix(data) {
        const matrix = new Matrix(0, 0);
        matrix.#data = data;
        return matrix;
    }

    flat() {
        return this.#data.flat();
    }
}