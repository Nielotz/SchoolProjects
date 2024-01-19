import {Debug} from "./debug.js";

export class Texturer {
    gl
    constructor(gl) {

        Debug.debugGlCall(() => {
        });
        this.gl = gl
        Texturer.gl = gl;

        this.textures = []
        // Debug.debugGlCall(() => this.#initBuffer());

        const defaultTexture = Texture2D.default();
        this.defaultTextureId = this.addTexture(defaultTexture);

        this.textureCoordinates = [
            // Front
            0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
            // Back
            0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
            // Top
            0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
            // Bottom
            0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
            // Right
            0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
            // Left
            0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
        ];

    }

    // #initBuffer() {
    //     const textureCoordinatesBuffer = Debug.debugGlCall(() =>
    //         this.gl.createBuffer());
    //     Debug.debugGlCall(() =>
    //         this.gl.bindBuffer(this.gl.ARRAY_BUFFER, textureCoordinatesBuffer));
    //
    //     Debug.debugGlCall(() => this.gl.bufferData(
    //         this.gl.ARRAY_BUFFER,
    //         new Float32Array(this.textureCoordinates),
    //         this.gl.STATIC_DRAW,
    //     ));
    //
    //     this.textureCoordinatesBuffer = textureCoordinatesBuffer;
    // }

    // getTextureCoordinatesBuffer() {
    //     return this.textureCoordinatesBuffer;
    // }


    // Returns textureId
    addTexture(texture) {
        return this.textures.push(texture) - 1;
    }

    getTextureById(textureId) {
        return this.textures[textureId];
    }

    #getFreeTextureSlot() {
        return this.slots.findIndex(isTaken => isTaken === false);
    }

    #setTextureSlot(slot, isTaken) {
        // this.slots[slot] = isTaken;
    }

    // Activate texture in free slot.
    // Bind texture to that slot.
    // Return used slot (required in fragmentShader).
    activateTexture(textureId) {
        // console.log(`Activating texture ${textureId}`)
        const texture = this.getTextureById(textureId);
        // const textureSlot = this.#getFreeTextureSlot();
        this.gl.activeTexture(this.gl.TEXTURE0);
        // this.#setTextureSlot(textureSlot, true);
        this.gl.bindTexture(this.gl.TEXTURE_2D, texture.getTexture());

        return this.gl.TEXTURE0;
    }

    activateDefaultTexture() {
        return this.activateTexture(this.defaultTextureId);
    }

    static getGlContext() {
        if (this.gl !== null)
            return this.gl;
        throw "Gl context is not set";
    }
}

export class Texture2D {
    constructor(path) {
        this.path = path
        this.gl = Texturer.getGlContext();
        const texture = Debug.debugGlCall(() =>
            this.gl.createTexture());

        Debug.debugGlCall(() =>
            this.gl.bindTexture(this.gl.TEXTURE_2D, texture));
        const level = 0;
        const internalFormat = this.gl.RGBA;
        const srcFormat = this.gl.RGBA;
        const srcType = this.gl.UNSIGNED_BYTE;

        const placeholderPixelWidth = 1;
        const placeholderPixelHeight = 1;
        const placeholderPixel = new Uint8Array([255, 0, 0, 0]);

        const placeholderPixelBorder = 0;
        Debug.debugGlCall(() =>
            this.gl.texImage2D(this.gl.TEXTURE_2D, level, internalFormat, placeholderPixelWidth, placeholderPixelHeight,
                placeholderPixelBorder, srcFormat, srcType, placeholderPixel));

        const image = new Image();
        image.src = this.path;
        image.onload = () => {
            // console.log(`Loaded image ${path}.`);
            // console.log(image);

            this.gl.bindTexture(this.gl.TEXTURE_2D, texture);
            this.gl.texImage2D(
                this.gl.TEXTURE_2D,
                level,
                internalFormat,
                srcFormat,
                srcType,
                image,
            );

            // WebGL1 has different requirements for power of 2 images
            // vs. non power of 2 images so check if the image is a
            // power of 2 in both dimensions.
            if (this.isPowerOf2(image.width) && this.isPowerOf2(image.height)) {
                // Yes, it's a power of 2. Generate mips.
                this.gl.generateMipmap(this.gl.TEXTURE_2D);
            } else {
                // No, it's not a power of 2. Turn off mips and set
                // wrapping to clamp to edge
                this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_S, this.gl.CLAMP_TO_EDGE);
                this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_T, this.gl.CLAMP_TO_EDGE);
                this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
            }
        };
        this.texture = texture;

        this.gl.pixelStorei(this.gl.UNPACK_FLIP_Y_WEBGL, true);
    }


    getTexture() {
        return this.texture;
    }

    static default() {
        return new Texture2D("defaultTexture.png");
    }

    isPowerOf2(value) {
        return (value & (value - 1)) === 0;
    }
}