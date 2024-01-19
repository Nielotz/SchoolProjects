export class Point3D {
    constructor(x, y, z) {
        if (typeof y === "undefined") {
            this.x = x.x;
            this.y = x.y;
            this.z = x.z;
        } else {
            this.x = x;
            this.y = y;
            this.z = z;
        }
    }

    coordinates() {
        return [this.x, this.y, this.z];
    }

    // Return size of point in bytes.
    static getSize() {
        return Float32Array.BYTES_PER_ELEMENT * 3
    }

    add(point) {
        return this.move(point.x, point.y, point.z)
    }

    move(x, y, z) {
        this.x += x;
        this.y += y;
        this.z += z;
        return this;
    }

    moved(x, y, z) {
        return this.copy().move(x, y, z);
    }

    copy() {
        return new Point3D(this.x, this.y, this.z);
    }
}

