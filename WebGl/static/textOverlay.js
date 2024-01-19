export class TextOverlay {
    #Text = class Text {
        constructor(text, posX, posY, htmlColor, htmlFont) {
            this.text = text;
            this.posX = posX;
            this.posY = posY;
            this.htmlColor = htmlColor;
            this.htmlFont = htmlFont;
        }
    }
    text = []

    constructor() {
        this.canvas = document.getElementById('textOverlay');
        if (!this.canvas) {
            throw new Error('Could not find HTML canvas element - check for typos, or loading JavaScript file too early');
        }
        this.context2d = this.canvas.getContext('2d');
        if (!this.context2d)
            throw new Error('2d context is not supported on this device - try using a different device or browser');

    }

    addText(text, posX, posY, htmlColor, htmlFont) {
        const textIdx = this.text.length;
        this.text.push(new this.#Text(text, posX, posY, htmlColor, htmlFont))
        return textIdx;
    }

    draw() {
        this.text.forEach(text => {
            this.context2d.font = text.htmlFont;
            this.context2d.fillStyle = text.htmlColor;
            this.context2d.fillText(text.text, text.posX, text.posY)
        });
    }
}