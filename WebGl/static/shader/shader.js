import vertexShaderSourceCode from "./vertexShader.js.glsl";
import fragmentShaderSourceCode from "./fragmentShader.js.glsl";
import {Debug} from "../debug.js";

class Shader {
    shaderId;
    shaderType;
    sourceCode;
    shaderName;
    gl;

    constructor(gl) {
        this.gl = gl;
    }

    compile() {
        this.shaderId = this.gl.createShader(this.shaderType);
        this.gl.shaderSource(this.shaderId, this.sourceCode);
        this.gl.compileShader(this.shaderId);
        if (!this.gl.getShaderParameter(this.shaderId, this.gl.COMPILE_STATUS)) {
            const errorMessage = this.gl.getShaderInfoLog(this.shaderId);
            Debug.displayError(`Failed to compile ${this.shaderName}: ${errorMessage}`);
            return -1;
        }
        return this.shaderId;
    }

    getShaderId() {
        return this.shaderId;
    }
}

class VertexShader extends Shader {
    constructor(gl) {
        super(gl);
        this.shaderType = this.gl.VERTEX_SHADER;
        this.sourceCode = vertexShaderSourceCode;
        this.shaderName = "VERTEX_SHADER";
    }
}

class FragmentShader extends Shader {
    constructor(gl) {
        super(gl);
        this.shaderType = this.gl.FRAGMENT_SHADER;
        this.sourceCode = fragmentShaderSourceCode;
        this.shaderName = "FRAGMENT_SHADER";
    }
}

export class ShaderProgram extends Shader {
    constructor(gl) {
        super(gl);
        this.shaderName = "SHADER_PROGRAM";
    }

    compile() {
        const vertexShader = new VertexShader(this.gl);
        if (vertexShader.compile() === -1) {
            Debug.displayError("Failed to compile vertexShader");
            return -1;
        }

        const fragmentShader = new FragmentShader(this.gl);
        if (fragmentShader.compile() === -1) {
            Debug.displayError("Failed to compile fragmentShader");
            return -1;
        }

        this.shaderId = this.gl.createProgram();
        this.gl.attachShader(this.shaderId, vertexShader.getShaderId());
        this.gl.attachShader(this.shaderId, fragmentShader.getShaderId());
        this.gl.linkProgram(this.shaderId);
        if (!this.gl.getProgramParameter(this.shaderId, this.gl.LINK_STATUS)) {
            const errorMessage = this.gl.getProgramInfoLog(this.shaderId);
            Debug.displayError(`Failed to link GPU program: ${errorMessage}`);
            this.gl.deleteProgram(this.shaderId)
            return -1;
        }

        return this.shaderId;
    }
}

export class Uniform {
    static config(gl, shaderProgramId) {
        this.gl = gl
        this.shaderProgramId = shaderProgramId
    }

    static getLocation(variableName) {
        const location = Debug.debugGlCall(() =>
            this.gl.getUniformLocation(this.shaderProgramId, variableName));
        if (location === null)
            Debug.displayError(`[Uniform] Cannot find ${variableName}`)
        return location;
    }

    static set_uniform4fv(name, data) {
        const location = this.getLocation(name);
        if (location === null)
            return;
        Debug.debugGlCall(() =>
            this.gl.uniform4fv(location, data));
        return name;
    }

    static setUniformMatrix4fv(name, data) {
        const location = this.getLocation(name);
        if (location === null)
            return;
        Debug.debugGlCall(() =>
            this.gl.uniformMatrix4fv(location, true, data));
        return name;
    }

    static setUniform1i(name, data) {
        const location = this.getLocation(name);
        if (location === null)
            return;
        Debug.debugGlCall(() =>
            this.gl.uniform1i(location, true, data));
        return name;
    }

    static setColor(color) {
        return this.set_uniform4fv("u_color", new Float32Array(color.getRGBA()));
    }

    static setMVP(mvp) {
        // console.log(mvp)
        return this.setUniformMatrix4fv("u_MVP", mvp.flat());
    }

    static setTexture2DSampler(textureSlotId) {
        this.setUniform1i("u_texture", textureSlotId);
    }

}
