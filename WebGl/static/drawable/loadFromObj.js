import {Shape3d} from "./shape/shape3d.js";
import {Point3D} from "./shape/point3d.js";
import {Vertex3D} from "./shape/vertex3d.js";
import {Point2D} from "./shape/point2d.js";

export async function loadOBJ(text) {
    const objFile = await fetch(new Request(text));
    const data = await objFile.text()

    const points = [null,];

    const faces = []
    const textureCoords = [
        new Point2D(0, 0),
        new Point2D(0, 0),
        new Point2D(1, 1),
    ]

    const normalizeValue = 1 / 40.418;

    data.split("\n").forEach(line => {
        const parts = line.trim().split(" ")
        const header = parts[0];
        const data = parts.slice(1);
        switch (header) {
            case "v":
                const coordinates = data.map(val => Number(val) * normalizeValue);
                points.push(new Point3D(...coordinates));
                // console.log(coordinates)
                break;
            case "f":
                const faceVertices = [];
                data.forEach((vertex, idx) => {
                    const vertexData = vertex.split("/")
                    const vertexIdx = Number(vertexData[0]);
                    const vertexTextureIdx = Number(vertexData[1]);
                    faceVertices.push(new Vertex3D(points[vertexIdx], textureCoords[idx]));
                })
                faces.push(faceVertices);
                break;
            default:
            // console.log(`Skipping "${line}".`)
        }
    });

    const shape = new Shape3d(new Point3D(0, 0, 0));
    let flatVertices = faces.flatMap(face => face.flat());
        // .flatMap(vertex => vertex.position.coordinates())
    // -40.350307
    //  40.417931
    // function minOfArray(arr) {
    //     let max = -Infinity;
    //     const QUANTUM = 32768;
    //
    //     for (let i = 0; i < arr.length; i += QUANTUM) {
    //         const submin = Math.max.apply(
    //             null,
    //             arr.slice(i, Math.min(i + QUANTUM, arr.length)),
    //         );
    //         max = Math.max(submin, max);
    //     }
    //
    //     return max;
    // }
    //
    // console.log(minOfArray(flatVertices));

    shape.setVertices(flatVertices);
    return shape;
}