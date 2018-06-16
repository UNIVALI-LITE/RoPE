class Point {
    constructor(x, y) {
        this.x = x
        this.y = y
    }
}

class Rectangle {
    constructor($elm) {
        this.setElm($elm)
    }
    setElm($elm) {
        this.$elm = $elm
        this.height = $elm.height()
        this.width = $elm.width()
        this.y = $elm.offset().top
        this.x = $elm.offset().left
    }
    contains(obj) {
        if (obj instanceof Point) {
            let point = obj
            return this.y <= point.y &&
                this.y + this.height >= point.y &&
                this.x <= point.x &&
                this.x + this.width >= point.x
        }
    }
    center() {
        let centerX = this.x + (this.width / 2)
        let centerY = this.y + (this.height / 2)
        return new Point(centerX, centerY)
    }
    getX(){
        return this.$elm.offset().left
    }
    getWidth(){
        return this.$elm.width()
    }
    moveTo(obj) {
        if (obj instanceof Rectangle) {
            obj = obj.$elm
        }
        if (obj instanceof jQuery) {
            const x = obj.offset().left
            const y = obj.offset().top
            this.moveToPoint(new Point(x, y))
        }
        if (obj instanceof Point) {
            this.moveToPoint(obj)
        }
    }
    moveToPoint(point) {
        this.moving = true
        this.$elm.animate({
            top: point.y,
            left: point.x
        }, 200, function () {
            this.moving = false
        })
    }
    sideOf(obj) {
        if (obj instanceof Point) {
            return this.center().x > obj.x ? this.LEFT : this.RIGHT
        }
    }
    id() {
        return this.$elm[0].id
    }
    setDragged() {
        this.dragged = true
        this.$elm
            .addClass('dragged')
            .removeClass('ready')
            .css({ 'z-index': 1000 })
    }
    disappear() {
        this.$elm.fadeOut(200, () => this.$elm.remove())
    }
    empty() {
        return !this.internalRectangle
    }
    add(obj) {
        return this.internalRectangle = obj
    }
    has(obj) {
        return obj === this.internalRectangle
    }
    frees() {
        this.internalRectangle = undefined
    }
}
Rectangle.prototype.LEFT = -1
Rectangle.prototype.RIGHT = 1