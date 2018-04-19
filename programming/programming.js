/*global $ global jQuery*/

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
    moveTo(obj) {
        if (obj instanceof Rectangle) {
            obj = obj.$elm
        }
        if (obj instanceof jQuery) {
            let $elm = this.$elm
            this.moving = true
            this.$elm.animate({
                top: obj.offset().top,
                left: obj.offset().left
            }, 200, function () {
                this.moving = false
            })
        }
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
        this.$elm.remove()
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

const EMPTY = -1
let idCounter = 0
let placeholderIdCounter = 5
let pieces = []
let placeholders = []
let clickedIds = []
let $programmingArea = $('#programming-view')
let $placeholdersArea = $('#placeholders-area')
let rectArea = new Rectangle($placeholdersArea)
let isTimeToSnap = true
const PIECE_SIZE = 50
const SCREEN_WIDTH = $(window).width()
const snapedPiecesWithoutOverflow = Math.ceil(SCREEN_WIDTH / PIECE_SIZE) - 3

class BlocksView {

    constructor() {
        this.createInitialPieces()
        this.createInitialPlaceholders()
        this.configureScrollListener()
    }

    createInitialPieces() {
        $('.available.block.piece').on('mousedown', (e) => {
            let $cloned = this.clone($(e.target))
            this.createPiece($cloned)
        }).trigger('mousedown')
    }

    createInitialPlaceholders() {
        $('.block.placeholder').each((idx, elm) => {
            placeholders.push(new Rectangle($(elm)))
        })
    }

    configureScrollListener() {
        $(window).on('scroll', () => {
            $('.ready.piece').hide() // because trembles on mobile when scroll
            clearTimeout($.data(this, 'scrollTimer'));
            $.data(this, 'scrollTimer', setTimeout(() => {
                $('.ready.piece').show()
                this.adjustAvailableReadyPieces()
            }, 250))
        })
    }

    clone($elm) {
        let $cloned = $elm.clone()
        $cloned
            .removeClass('available')
            .addClass('ready')
            .css({
                position: 'absolute',
                top: $elm.offset().top,
                left: $elm.offset().left
            })
            .appendTo($programmingArea)
            .draggable({
                start: (e) => this.handleDragStart(e),
                stop: (e) => this.handleDragStop(e),
                drag: (e) => this.handleDrag(e),
                scroll: false
            })

        let id = ++idCounter
        $cloned.id = id
        $cloned[0].id = id
        return $cloned
    }

    getOrCreatePiece(e) {
        let elm = e.target
        let id = elm.id
        let foundPiece = this.findPieceById(id)
        return foundPiece || this.createPiece($(elm))
    }

    createPiece($elm) {
        let piece = new Rectangle($elm)
        pieces.push(piece)
        return piece
    }

    findPieceById(id) {
        let foundPiece = pieces
            .filter((piece) => piece instanceof Rectangle && piece.id() == id)
        if (foundPiece.length) {
            return foundPiece[0]
        }
    }

    handleDragStart(e) {
        this.adjustAvailableReadyPieces()
        let movingPiece = this.getOrCreatePiece(e)
        isTimeToSnap = false
        setTimeout(() => {
            isTimeToSnap = true
        }, 300)
        this.freesPlaceholder(movingPiece)
        this.organizePostionZ(movingPiece)
        if (!movingPiece.dragged) { // first move, create new piece below, to be used after
            this.clone(movingPiece.$elm)
            movingPiece.setDragged()
        }
    }

    handleDrag(e) {
        let movingPiece = this.getOrCreatePiece(e)
        movingPiece.setElm($(e.target))
        this.moveSnapedPieceIfIsTimeToSnap(movingPiece)
    }

    handleDragStop(e) {
        let piece = this.getOrCreatePiece(e)
        if (!this.removeIfOutside(piece)) {
            this.snapToPlaceholder(piece)
        }
        this.movePiecesToLeft()
        this.removePlaceholders()
        this.adjustAreaWidth()
        this.adjustPiecesToPlaceholders()
        this.addRightPlaceholder()
    }

    adjustAvailableReadyPieces() {
        /** Ready pieces are available and not dragged.
            This function adjusts this left position, relative to the document
            because when scroll right they stay left **/
        $('.ready.piece').each((idx, elm) => {
            const command = $(elm).attr('data-command')
            const originalElement = $('.available.' + command)
            $(elm).css('left', originalElement.offset().left)
        })
    }

    adjustPiecesToPlaceholders() {
        placeholders.forEach((placeholder) => {
            if (!placeholder.empty()) {
                placeholder.internalRectangle.moveTo(placeholder)
            }
        })
    }

    organizePostionZ(piece) {
        let id = piece.id()
        let i = clickedIds.indexOf(id)
        if (i != -1) {
            clickedIds.splice(i, 1)
        }
        clickedIds.push(id)
        $('.piece.dragged').each((i, elm) => {
            let zIndex = clickedIds.indexOf(elm.id) + 10
            $(elm).css('z-index', zIndex)
        })
    }

    movePiecesToLeft() {
        let i = 0
        while (i < placeholders.length) {
            let placeholder = placeholders[i]
            if (placeholder.empty()) {
                let j = i
                let rightOccuped
                while (j < placeholders.length && !rightOccuped) {
                    if (!placeholders[j].empty()) {
                        rightOccuped = placeholders[j]
                    }
                    j++
                }
                if (rightOccuped) {
                    this.snap(placeholder, rightOccuped.internalRectangle)
                    rightOccuped.frees()
                }
            }
            i++
        }
    }

    adjustAreaWidth() {
        const snapedPiecesNumber = this.getOccupedPlaceholders().length
        const newWidth = snapedPiecesNumber * PIECE_SIZE + PIECE_SIZE + PIECE_SIZE + PIECE_SIZE
        $placeholdersArea.css('width', newWidth < SCREEN_WIDTH ? SCREEN_WIDTH : newWidth)
        rectArea = new Rectangle($('#placeholders-area'))
    }

    getOccupedPlaceholders() {
        return placeholders.filter(p => !p.empty())
    }

    freesPlaceholder(movingPiece) {
        let freedPlaceholder = placeholders.filter(placeholder => placeholder.has(movingPiece))[0]
        if (freedPlaceholder) {
            freedPlaceholder.frees()
        }
    }

    removeIfOutside(piece) {
        const rect = new Rectangle($('body'))
        if (!rect.contains(piece.center())) {
            piece.disappear()
            pieces.splice(pieces.indexOf(piece), 1)
            return true
        }
    }

    snapToPlaceholder(piece) {
        placeholders.forEach((placeholder) => {
            if (placeholder.empty() && placeholder.contains(piece.center())) {
                this.snap(placeholder, piece)
                return
            }
        })
    }

    snap(placeholder, piece) {
        placeholder.add(piece)
        piece.moveTo(placeholder)
        const audio = new Audio('assets/snapsound.mp3')
        audio.play()
    }

    addRightPlaceholder() {
        const ocuppedPlaceholders = this.getOccupedPlaceholders()
        if (placeholders.length === ocuppedPlaceholders.length) {
            this.createPlaceholder(Rectangle.prototype.RIGHT)
            this.adjustPiecesToPlaceholders()
        }
    }

    removePlaceholders() {
        let ocupped = this.getOccupedPlaceholders().length
        if (placeholders.length > ocupped + 3) {
            placeholders.pop()
            $('.placeholder').last().remove()
            this.adjustPiecesToPlaceholders()
        }
        this.updatePlaceholderElements()
    }

    createPlaceholder(side) {
        let $placeholderBase = $($('.placeholder')[0])
        let $placeholderClone = $placeholderBase.clone()
        let placeholder = new Rectangle($placeholderClone)
        if (side === Rectangle.prototype.LEFT) {
            $placeholdersArea.prepend($placeholderClone)
            placeholders.unshift(placeholder)
        } else {
            $placeholdersArea.append($placeholderClone)
            placeholders.push(placeholder)
        }
        this.updatePlaceholderElements()
        return placeholder
    }

    updatePlaceholderElements() {
        $('.block.placeholder').each((idx, elm) => {
            placeholders[idx].setElm($(elm))
        })
    }

    getOrCreatePlaceholder(side, placeholderIndex) {
        if (!placeholders[placeholderIndex + side]) {
            let newPlacehoder = this.createPlaceholder(side)
            this.adjustPiecesToPlaceholders()
            return newPlacehoder
        }
        return placeholders[placeholderIndex + side]
    }

    moveSnapedPieceIfIsTimeToSnap(movingPiece) {
        if (isTimeToSnap)
            this.moveSnapedPiece(movingPiece)
    }

    moveSnapedPiece(movingPiece) {
        placeholders.forEach((placeholder, placeholderIndex) => {
            this.ifHasPieceThenMove(placeholder, placeholderIndex, movingPiece)
        })
    }

    ifHasPieceThenMove(placeholder, placeholderIndex, movingPiece) {
        if (!placeholder.empty() &&
            placeholder.contains(movingPiece.center())) {
            let moveSide = this.calcMoveSide(placeholder, placeholderIndex, movingPiece)
            this.movePlaceholderPiece(placeholder, moveSide)
        }
    }

    calcMoveSide(placeholder, placeholderIndex, movingPiece) {
        if (placeholderIndex == 0) {
            return Rectangle.prototype.RIGHT
        }
        if (placeholderIndex == placeholders.length - 1) {
            return Rectangle.prototype.LEFT
        }

        let commingSide = placeholder.sideOf(movingPiece.center())
        if (commingSide === Rectangle.prototype.LEFT) {
            if (placeholders[placeholderIndex - 1].empty()) {
                return Rectangle.prototype.LEFT
            }
            if (placeholders[placeholderIndex + 1].empty()) {
                return Rectangle.prototype.RIGHT
            }
        }
        if (commingSide === Rectangle.prototype.RIGHT) {
            if (placeholders[placeholderIndex + 1].empty()) {
                return Rectangle.prototype.RIGHT
            }
            if (placeholders[placeholderIndex - 1].empty()) {
                return Rectangle.prototype.LEFT
            }
        }
        return Rectangle.prototype.RIGHT
    }

    movePlaceholderPiece(placeholder, moveThisPieceToSide) {
        let placeholderIndex = placeholders.indexOf(placeholder)
        let placeholdeToGo = this.getOrCreatePlaceholder(moveThisPieceToSide, placeholderIndex)
        if (!placeholdeToGo.empty()) {
            this.movePlaceholderPiece(placeholdeToGo, moveThisPieceToSide)
        }
        this.snap(placeholdeToGo, placeholder.internalRectangle)
        placeholder.frees()
    }

}


