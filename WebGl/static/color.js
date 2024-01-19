class ColorRGBA {
    r = 1
    g = 1
    b = 1
    a = 0

    constructor(r, g, b, a) {
        this.r = r
        this.g = g
        this.b = b
        if (typeof a === "undefined")
            this.a = 1
        else
            this.a = a
    }

    getRGBA() {
        return [this.r, this.g, this.b, this.a]
    }
}

export class RGBA {
    static WHITE = new ColorRGBA(1., 1., 1.);
    static RED = new ColorRGBA(1., 0., 0.);
    static GREEN = new ColorRGBA(0., 1., 0.);
    static BLUE = new ColorRGBA(0., 0., 1.);
    // YELLOW = new RGB = kRedRGBA + kGreenRGBA;
    // PURPLE = new RGB = kRedRGBA + kBlueRGBA;
    // WHITE = new RGB = kRedRGBA + kGreenRGBA + kBlueRGBA;
}
