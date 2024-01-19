import {Point3D} from "./point3d.js";
import {Point2D} from "./point2d.js";

export class Vertex3D{
    // position: Point3D
    // texturePosition: Point2D
    constructor(position, texturePosition) {
        this.position = position;
        this.texturePosition = texturePosition;
    }

    coordinates() {
        return Array.of(...this.position.coordinates(), ...this.texturePosition.coordinates());
    }

    // Return size of point in bytes.
    static getSize() {
        return Float32Array.BYTES_PER_ELEMENT * 5
    }

    copy() {
        return new Vertex3D(this.position.copy(), this.texturePosition.copy());
    }
}

