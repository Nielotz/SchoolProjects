import {Point2D} from "./point2d.js";
import {RGBA} from "../../color.js";

export class Shape2d {
    constructor(position) {
        this.position = position.copy();
        this.vertices = []
        this.color = RGBA.WHITE
    }

    // Return vertices organized in triangles:
    //   Point11, Point12, Point13,  // Triangle 1
    //   Point21, Point22, Point23  // Triangle 2
    getVertices() {
        return this.vertices;
    }

    getVerticesAmount() {
        return this.vertices.length;
    }

    /**
     * @deprecated
     */
    move(x, y) {
        return this.moved(x, y);
    }

    moved(x, y) {
        this.vertices.forEach(vertex => vertex.move(x, y));
        return this;
    }

    colored(color) {
        this.color = color
        return this
    }

    getColor() {
        return this.color
    }
}

export class Line2D extends Shape2d {
    constructor(points) {
        throw "Not implemented"
        super();
        this.points = points
    }

    copy() {
        return new Line2D(this.points.map(vertex => vertex.copy()))
    }
}

export class Triangle2D extends Shape2d {

    constructor(position) {
        throw "Not implemented"

        super(position);
    }

    copy() {
        return new Triangle2D(this.vertices.map(vertex => vertex.copy()));
    }
}

export class Rectangle2D extends Shape2d {
    constructor(position, width, height) {
        super(position);
        this.width = width;
        this.height = height;

        const start = new Point2D(position);
        const right = start.copy().move(this.width, 0);
        const leftBottom = start.copy().move(0, this.height);
        const rightBottom = start.copy().move(this.width, this.height);

        this.vertices = [
            start, leftBottom, right,
            leftBottom.copy(), rightBottom, right.copy()
        ]
    }

    colored(color) {
        this.color = color;
        return this;
    }

    copy() {
        return new Rectangle2D(this.position, this.width, this.height)
            .colored(this.getColor());
    }
}

export class Circle2D extends Shape2d {
    constructor(position, radius, points) {
        // if (points < 4)
        //     throw "To few points to create a circle.";

        super(position);
        // console.log(this.position)
        this.points = points
        this.radius = radius
        this.color = RGBA.WHITE

        const angleBetweenVertices = 2 * Math.PI / points;
        const circleCenter = this.position;
        let prevCircleEdgePoint = this.position.copy().moved(0, radius);  // Start point (top of the circle).
        for (let triangleIdx = 0; triangleIdx < points; triangleIdx++) {
            const angle = (triangleIdx + 1) * angleBetweenVertices;
            const newCircleEdgePoint = circleCenter.copy().moved(Math.sin(angle) * radius, Math.cos(angle) * radius);

            this.vertices.push(
                prevCircleEdgePoint.copy(),
                circleCenter.copy(),
                newCircleEdgePoint.copy()
            );

            prevCircleEdgePoint = newCircleEdgePoint;
        }
    }

    moved(x, y) {
        this.vertices.forEach(vertex => vertex.move(x, y));
        return this;
    }
}