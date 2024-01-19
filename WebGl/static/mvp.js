import {Matrix} from "./matrix.js";
import {Rotator, Scaler, Transformation, Translator} from "./transformer.js";

class Model {
    #matrix;

    constructor(glContext) {
        this.glContext = glContext;
        this.#matrix = Matrix.getSquared(4).zero();
        for (let idx = 0; idx < this.#matrix.getSize()[0]; idx++) {
            this.#matrix.setCell(idx, idx, 1);
        }
    }

    get() {
        return this.#matrix;
    }
}

class View {
    #transformation
    #VIEW_ID = 0;

    constructor(glContext) {
        this.glContext = glContext;
        this.#transformation = new Transformation()
        this.#transformation.addTransformation(this.#VIEW_ID, new Translator(0, 0, 0));
    }

    get() {
        return this.#transformation.getCombinedTransformation(this.#VIEW_ID)
    }

    scale(x, y, z) {
        this.#transformation.addTransformation(this.#VIEW_ID, new Scaler(x, y, z));
        return this;
    }

    move(x, y, z) {
        this.#transformation.addTransformation(this.#VIEW_ID, new Translator(x, y, z));
        return this;
    }

    rotate(x, y, z, theta) {
        this.#transformation.addTransformation(this.#VIEW_ID, new Rotator(x, y, z, theta));
        return this;
    }
}

class Projection {
    #matrix;

    constructor(glContext) {
        this.glContext = glContext;
        this.#matrix = Matrix.identity(4);
    }

    get() {
        return this.#matrix;
    }
}

export class MVP {
    #model
    #view
    #projection

    constructor(glContext) {
        this.glContext = glContext;

        this.#model = new Model(this.glContext);
        this.#view = new View(this.glContext);
        this.#projection = new Projection(this.glContext);
    }

    get() {
        return this.#projection.get()
            .multipled(this.#view.get()
                .multipled(this.#model.get())
            );
    }

    getView() {
        return this.#view;
    }

}