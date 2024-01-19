export class Point2D {
    constructor(x, y) {
        if (typeof y === "undefined") {
            this.x = x.x;
            this.y = x.y;
        } else {
            this.x = x;
            this.y = y;
        }
    }

    coordinates() {
        return [this.x, this.y];
    }

    // Return size of point in bytes.
    static getSize() {
        return Float32Array.BYTES_PER_ELEMENT * 2
    }

    add(point) {
        return this.move(point.x, point.y)
    }

    move(x, y) {
        this.x += x;
        this.y += y;
        return this;
    }

    moved(x, y) {
        return this.copy().move(x, y);
    }

    copy() {
        return new Point2D(this.x, this.y);
    }
}

