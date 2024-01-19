import {Point3D} from "./point3d.js";
import {RGBA} from "../../color.js";
import {Point2D} from "./point2d.js";
import {Vertex3D} from "./vertex3d.js";

export class Shape3d {
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

    setVertices(vertices) {
        this.vertices = vertices;
    }

    getVerticesAmount() {
        return this.vertices.length;
    }

    move(x, y, z) {
        return this.moved(x, y, z);
    }

    moved(x, y, z) {
        this.vertices.forEach(vertex => vertex.move(x, y, z));
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

export class Rectangle3D extends Shape3d {
    constructor(position, width, height) {
        super(position);
        this.width = width;
        this.height = height;

        const start = new Point3D(position);
        const right = start.moved(this.width, 0, 0);
        const leftBottom = start.moved(0, this.height, 0);
        const rightBottom = start.moved(this.width, this.height, 0);

        this.vertices = [
            start, leftBottom, right,
            leftBottom.copy(), rightBottom, right.copy()
        ]
    }

    copy() {
        return new Rectangle3D(this.position, this.width, this.height)
            .colored(this.getColor());
    }
}

export class Circle3D extends Shape3d {
    constructor(position, radius, points) {
        super(position);
        // console.log(this.position)
        this.points = points
        this.radius = radius
        this.color = RGBA.WHITE

        const angleBetweenVertices = 2 * Math.PI / points;
        const circleCenter = this.position;
        let prevCircleEdgePoint = this.position.copy().moved(0, radius, 0);  // Start point (top of the circle).
        for (let triangleIdx = 0; triangleIdx < points; triangleIdx++) {
            const angle = (triangleIdx + 1) * angleBetweenVertices;
            const newCircleEdgePoint = circleCenter.copy().moved(Math.sin(angle) * radius, Math.cos(angle) * radius, 0);

            this.vertices.push(
                prevCircleEdgePoint.copy(),
                circleCenter.copy(),
                newCircleEdgePoint.copy()
            );

            prevCircleEdgePoint = newCircleEdgePoint;
        }
    }
}

export class Hexahedron extends Shape3d {
    constructor(position, size) {
        super(position);

        const start = new Point3D(position);
        const x = start.moved(size, 0, 0);
        const y = start.moved(0, size, 0);
        const xy = start.moved(size, size, 0);
        const z = start.moved(0, 0, -size);
        const xz = start.moved(size, 0, -size);
        const yz = start.moved(0, size, -size);
        const xyz = start.moved(size, size, -size);

        const textureLeftBottom = new Point2D(0, 0);
        const textureLeftTop = textureLeftBottom.moved(0, 1);
        const textureRightBottom = textureLeftBottom.moved(1, 0);
        const textureRightTop = textureLeftBottom.moved(1, 1);
        // Organize into triangles points.
        this.vertices = [
            // Front.
            new Vertex3D(xy, textureRightTop), new Vertex3D(y, textureLeftTop), new Vertex3D(start, textureLeftBottom),
            new Vertex3D(xy, textureRightTop), new Vertex3D(start, textureLeftBottom), new Vertex3D(x, textureRightBottom),

            // Right.
            new Vertex3D(xyz, textureRightTop), new Vertex3D(xy, textureLeftTop), new Vertex3D(x, textureLeftBottom),
            new Vertex3D(xyz, textureRightTop), new Vertex3D(x, textureLeftBottom), new Vertex3D(xz, textureRightBottom),

            // Back.
            new Vertex3D(yz, textureRightTop), new Vertex3D(xyz, textureLeftTop), new Vertex3D(xz, textureLeftBottom),
            new Vertex3D(yz, textureRightTop), new Vertex3D(xz, textureLeftBottom), new Vertex3D(z, textureRightBottom),

            // Left.
            new Vertex3D(y, textureRightTop), new Vertex3D(yz, textureLeftTop), new Vertex3D(z, textureLeftBottom),
            new Vertex3D(y, textureRightTop), new Vertex3D(z, textureLeftBottom), new Vertex3D(start, textureRightBottom),

            // Bottom.
            new Vertex3D(x, textureRightTop), new Vertex3D(start, textureLeftTop), new Vertex3D(z, textureLeftBottom),
            new Vertex3D(x, textureRightTop), new Vertex3D(z, textureLeftBottom), new Vertex3D(xz, textureRightBottom),

            // Top.
            new Vertex3D(y, textureRightTop), new Vertex3D(xy, textureLeftTop), new Vertex3D(xyz, textureLeftBottom),
            new Vertex3D(y, textureRightTop), new Vertex3D(xyz, textureLeftBottom), new Vertex3D(yz, textureRightBottom),
        ];
        // console.log(this.vertices)
    }
}