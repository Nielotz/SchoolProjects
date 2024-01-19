import {Debug} from "./debug.js";
import {ShaderProgram, Uniform} from "./shader/shader.js";
import {Hexahedron} from "./drawable/shape/shape3d.js";
import {RGBA} from "./color.js";
import {Point3D} from "./drawable/shape/point3d.js";
import {Point2D} from "./drawable/shape/point2d.js";
import {MVP} from "./mvp.js";
import {Rotator, Transformation} from "./transformer.js";
import {Matrix} from "./matrix.js";
import {Texture2D, Texturer} from "./texture.js";
import {Vertex3D} from "./drawable/shape/vertex3d.js";
import {loadOBJ} from "./drawable/loadFromObj.js";

class App {
    #glContext
    #canvas
    #viewAsShapeId = 0
    #shapes = {}
    #shapesCounter = 1
    #shaderProgram;
    #positionBuffer;
    #mvp
    #transformer
    #texturer
    #textureIdToShapeId = []

    constructor() {
        // Init canvas and glContext
        {
            this.#canvas = document.getElementById('main');
            if (!this.#canvas)
                throw new Error('Could not find HTML canvas element - check for typos, or loading JavaScript file too early');
            console.log("Created canvas");

            this.#glContext = this.#canvas.getContext('webgl2');
            if (!this.#glContext)
                throw new Error('WebGL is not supported on this device - try using a different device or browser');

            Debug.setGl(this.#glContext);
            console.log("Received gl.");
        }

        // Prepare shaders.
        {
            this.#shaderProgram = new ShaderProgram(this.#glContext);
            if (this.#shaderProgram.compile() === -1) {
                Debug.displayError("Failed to compile shader program.")
                return -1
            }
            this.#glContext.useProgram(this.#shaderProgram.getShaderId());

            Uniform.config(this.#glContext, this.#shaderProgram.getShaderId());
            console.log("Prepared Uniform.");
        }

        // Create buffer,
        this.#positionBuffer = Debug.debugGlCall(() =>
            this.#glContext.createBuffer());
        Debug.debugGlCall(() =>
            this.#glContext.bindBuffer(this.#glContext.ARRAY_BUFFER, this.#positionBuffer));


        this.#canvas.width = this.#canvas.clientWidth;
        this.#canvas.height = this.#canvas.clientHeight;

        this.#glContext.viewport(0, 0, this.#canvas.width, this.#canvas.height);
        Debug.debugGlCall(() =>
            this.#glContext.enable(this.#glContext.DEPTH_TEST));
        Debug.debugGlCall(() =>
            this.#glContext.frontFace(this.#glContext.CCW));
        Debug.debugGlCall(() =>
            this.#glContext.enable(this.#glContext.CULL_FACE));
        Debug.debugGlCall(() =>
            this.#glContext.cullFace(this.#glContext.BACK));

        this.mvp = new MVP(this.#glContext);
        this.#transformer = new Transformation();

        this.#texturer = new Texturer(this.#glContext);

    }

    transformScene(transformer) {
        this.addTransformation(this.#viewAsShapeId, transformer);
    }

    moveScene(x, y, z) {
        this.mvp.getView().move(x, y, z);
    }

    scaleScene(x, y, z) {
        this.mvp.getView().scale(x, y, z);
    }

    rotateScene(x, y, z, theta) {
        this.mvp.getView().rotate(x, y, z, theta);
    }

    addShape(shape) {
        const idx = this.#getShapeId();

        this.#shapes[idx] = shape;
        console.log(`Added shape with id: ${idx}`)
        return idx;
    }

    addTransformation(shapeId, transformation) {
        this.#transformer.addTransformation(shapeId, transformation);
    }

    assignTexture(shapeId, texture) {
        this.#textureIdToShapeId[shapeId] = this.#texturer.addTexture(texture);
    }

    #getShapeId() {
        return this.#shapesCounter++;
    }

    mainLoop(onUpdate, waiter) {
        const loop = () => {
            let intervalId = setInterval(() => {
                if (!document.hasFocus())
                    clearInterval(intervalId);
                this.#draw();
                if (typeof onUpdate !== "undefined")
                    onUpdate();
                console.log("Rendering")
            }, 500);
        }
        loop();
    }

    enableObjectRotationsViaMouse(objectId) {
        var myIntervals = []
        var lastMousePos = {x: null, y: null};

        const onMouseMove = (mouseEvent) => {
            if (!lastMousePos.x)
                throw "oops"

            const totalDistance = Math.sqrt(
                Math.pow(lastMousePos.y - mouseEvent.y, 2)
                + Math.pow(lastMousePos.x - mouseEvent.x, 2)
            );

            const distanceAsPercentage = totalDistance / this.#canvas.clientWidth;
            const rotatingRatio = 360

            console.log(`Moving by ${distanceAsPercentage * rotatingRatio}`);

            this.addTransformation(objectId, new Rotator(0, 1, 0, distanceAsPercentage * rotatingRatio));
        }
        const syncWithMouse = (mouseEvent) => {
            lastMousePos.x = mouseEvent.x;
            lastMousePos.y = mouseEvent.y;
            addEventListener("mousemove", onMouseMove);
            console.log("Starting moving");
        }

        const desyncWithMouse = (mouseEvent) => {
            removeEventListener("mousemove", onMouseMove);
            console.log("Stopping moving");
        }

        addEventListener("mousedown", syncWithMouse);
        addEventListener("mouseup", desyncWithMouse);
    }

    #clearScreen() {
        this.#glContext.clearColor(0.08, 0.08, 0.08, 1.0);
        this.#glContext.clear(this.#glContext.COLOR_BUFFER_BIT | this.#glContext.DEPTH_BUFFER_BIT);
    }

    #setVertexAttribPointer() {
        const COORDINATES_POS = 0;
        const TEXTURE_POS = 1;

        Debug.debugGlCall(() =>
            this.#glContext.enableVertexAttribArray(COORDINATES_POS));

        Debug.debugGlCall(() =>
            this.#glContext.vertexAttribPointer(
                COORDINATES_POS,
                3,  // x, y, z
                this.#glContext.FLOAT,
                false,
                Vertex3D.getSize(),
                0
            ));

        Debug.debugGlCall(() =>
            this.#glContext.enableVertexAttribArray(TEXTURE_POS));

        Debug.debugGlCall(() =>
            this.#glContext.vertexAttribPointer(
                TEXTURE_POS,
                2,  // x, y
                this.#glContext.FLOAT,
                false,
                Vertex3D.getSize(),
                Point3D.getSize()  // Offset
            ));
    }

    #getTextureIdByShapeId(shapeId) {
        return this.#textureIdToShapeId[shapeId];
    }

    // Bind buffer
    // Set uniforms: u_color
    // Send Data
    // Return Data.length
    #drawShapes() {
        this.#clearScreen()
        Debug.debugGlCall(() =>
            this.#glContext.bindBuffer(this.#glContext.ARRAY_BUFFER, this.#positionBuffer));

        for (const shapeId of Object.keys(this.#shapes)) {
            const shape = this.#shapes[shapeId];
            this.#setVertexAttribPointer();

            const vertices = shape.getVertices();
            const coordinates = new Float32Array(
                vertices.flatMap(vertex => vertex.coordinates())
            );
            // console.log(coordinates);
            Uniform.setMVP(this.mvp.get());

            const combinedTransformation = this.#transformer.getCombinedTransformation(shapeId) || Matrix.identity(4);
            Uniform.setUniformMatrix4fv("u_transformation", combinedTransformation.flat());

            // const textureId = this.#getTextureIdByShapeId(shapeId);
            // if (textureId === undefined) {
            //     Debug.displayError("Shape does not have a texture.");
            // }
            // const textureSlotId = Debug.debugGlCall(() =>
            //     this.#texturer.activateTexture(textureId));
            const textureSlotId = Debug.debugGlCall(() =>
                this.#texturer.activateDefaultTexture());
            Uniform.setTexture2DSampler(0);

            // Send data.
            Debug.debugGlCall(() =>
                this.#glContext.bufferData(this.#glContext.ARRAY_BUFFER,  // target
                    coordinates,  // source data
                    this.#glContext.STATIC_DRAW
                ) // usage
            );

            const amountOfShapesToDraw = vertices.length;
            Debug.debugGlCall(() =>
                this.#glContext.drawArrays(this.#glContext.TRIANGLES, 0, amountOfShapesToDraw));

        }
    }

    #draw() {
        this.#drawShapes()
    }
}

function main() {
    "use strict";

    const app = new App();

    // const circle = new Circle3D(new Point3D(0, 0, 0), 0.25, 8);
    // app.addShape(circle.colored(RGBA.BLUE));

    // const hexahedron = new Hexahedron(new Point3D(0, 0, 0), 0.2);
    // const hexahedronId = app.addShape(hexahedron);

    const testCube = loadOBJ("res/mountains.obj");

    testCube.then((shape) => {
        console.log(shape)
        app.addShape(shape);
        app.mainLoop();
    })

    // app.assignTexture(hexahed
    //
    //
    // ronId, new Texture2D("res/test.jpg"))


    // app.moveScene(0.3, 0, 0);
    // app.scaleScene(2, 1, 1);
    // app.enableObjectRotationsViaMouse(hexahedronId);
    //
    // app.mainLoop(() =>
    //     app.addTransformation(hexahedronId, new Rotator(1, 1, 0, ((360 / 8) / 2) * (Math.PI / 180)))
    // );
    // const textOverlay = new TextOverlay();
    // textOverlay.addText("A", 0, 0,  "green", "50pt Arial");
    // app.mainLoop(() => textOverlay.draw());

}

main();