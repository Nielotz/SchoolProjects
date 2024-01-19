export class Debug {
    gl = null

    static errorCodeToMessage(errorCode) {
        switch (errorCode) {
            case this.gl.INVALID_ENUM:
                return "An unacceptable value has been specified for an enumerated argument. The command is ignored and the error flag is set.";
            case this.gl.INVALID_VALUE:
                return "A numeric argument is out of range. The command is ignored and the error flag is set.";
            case this.gl.INVALID_OPERATION:
                return "The specified command is not allowed for the current state. The command is ignored and the error flag is set.";
            case this.gl.INVALID_FRAMEBUFFER_OPERATION:
                return "The currently bound framebuffer is not framebuffer complete when trying to render to or to read from it.";
            case this.gl.OUT_OF_MEMORY:
                return "Not enough memory is left to execute the command.";
            case this.gl.CONTEXT_LOST_WEBGL:
                return "If the WebGL context is lost, this error is returned on the first call to getError. Afterwards and until the context has been restored, it returns gl.NO_ERROR. ";
            case this.gl.NO_ERROR:
            default:
                return ""
        }
    }

    static displayError(message) {
        "use strict";
        const errorBoxDiv = document.getElementById('error-box');
        const errorSpan = document.createElement('p');
        errorSpan.innerText = message;
        errorBoxDiv.appendChild(errorSpan);
        console.error(message);

        return -1
    }

    static debugGlCall(call) {
        let errorMessage = this.errorCodeToMessage(this.gl.getError())
        if (errorMessage)
            this.displayError(`An error occurred before a function call, error: ${errorMessage}`);

        const returnValue = call();

        errorMessage = this.errorCodeToMessage(this.gl.getError())
        if (errorMessage)
            this.displayError(errorMessage);

        return returnValue;
    }

    static setGl(gl) {
        this.gl = gl
    }
}