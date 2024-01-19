import {Matrix} from "./matrix.js";
import {Debug} from "./debug.js";

export class Transformation {
    #transformationsLinks;
    #transformations;  // transformationId: Transformation
    #transformationsCounter

    constructor() {
        // this.#transformationsLinks = {}
        this.#transformations = {};  // id: transformationMatrix
        // this.#transformationsCounter = 0;
    }

    addTransformation(id, transformer) {
        // Check whether this id had any transformations.
        if (Object.prototype.hasOwnProperty.call(this.#transformations, id))
            // Apply transformation.
            this.#transformations[id] = this.#transformations[id].multipled(transformer.getTransformationMatrix())
        else
            this.#transformations[id] = transformer.getTransformationMatrix();


        // Add transformation to #transformations
        // Add transformations to
        /*
        const transformerId = this.#generateTransformationId();


        const transformationsIdsOfIds = this.#transformations[id]["transformations"];


        if (transformationsIdsOfIds.includes(transformerId))
            return Debug.displayError(`Transformation id ${transformerId} already in transformations ${this.#transformations}.`);

        transformationsIdsOfIds.push(transformerId);
        return transformerId;
        */
    }

    getCombinedTransformation(id) {
        return this.#transformations[id];
    }

    // #generateTransformationId() {
    //     return this.#transformationsCounter++;
    // }
}

export class Transformer {
    transformationMatrix

    constructor() {
        this.transformationMatrix = Matrix.identity(4);
    }

    getTransformationMatrix() {
        return this.transformationMatrix;
    }
}

export class Translator extends Transformer {
    constructor(x, y, z) {
        super();
        const lastColumnIdx = this.transformationMatrix.getSize()[1] - 1;
        this.transformationMatrix.addColumn([x, y, z, 0], lastColumnIdx);
    }
}

export class Scaler extends Transformer {
    constructor(x, y, z) {
        super();
        this.transformationMatrix.multiplyDiagonal([x, y, z, 1]);
    }
}

export class Rotator extends Transformer {
    constructor(x, y, z, theta) {
        super();
        const cosTheta = Math.cos(theta);
        const revCosTheta = 1 - cosTheta;
        const sinTheta = Math.sin(theta);
        this.transformationMatrix = Matrix.fromMatrix([
            [cosTheta + x * x * revCosTheta, x * y * revCosTheta - z * sinTheta, x * z * revCosTheta + y * sinTheta, 0],
            [y * x * revCosTheta + z * sinTheta, cosTheta + y * y * revCosTheta, y * z * revCosTheta - x * sinTheta, 0],
            [z * x * revCosTheta - y * sinTheta, z * y * revCosTheta + x * sinTheta, cosTheta + z * z * revCosTheta, 0],
            [0, 0, 0, 1]
        ]);
    }
}