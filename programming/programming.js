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
        this.$elm.fadeOut(500, () => this.$elm.remove())
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
const audio = new Audio('assets/snapsound.mp3')

class BlocksView {

    constructor() {
        this.idCounter = 0
        this.pieces = []
        this.placeholders = []
        this.clickedIds = []
        this.$programmingView = $('#programming-view')
        this.$placeholdersArea = $('#placeholders-area')
        this.isTimeToSnap = true

        this.createInitialPieces()
        this.createInitialPlaceholders()
        this.configureScrollListener()
    }

    createInitialPieces() {
        $('.available.block.piece').on('mousedown', (e) => {
            this.cloneAndCreatePiece($(e.target))
        }).trigger('mousedown')
    }

    cloneAndCreatePiece($elm) {
        let $cloned = this.clone($elm)
        return this.createPiece($cloned)
    }

    createInitialPlaceholders() {
        $('.block.placeholder').each((idx, elm) => {
            this.placeholders.push(new Rectangle($(elm)))
        })
    }

    configureScrollListener() {
        $(window).on('scroll', () => {
            $('.ready.piece').hide() // because trembles on mobile when scroll
            clearTimeout($.data(this, 'scrollTimer'));
            $.data(this, 'scrollTimer', setTimeout(() => {
                $('.ready.piece').show()
                this.adjustPiecesToPlaceholders()
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
            .appendTo(this.$programmingView)
            .draggable({
                start: (e) => this.handleDragStart(e),
                stop: (e) => this.handleDragStop(e),
                drag: (e) => this.handleDrag(e),
                scroll: false
            })

        let id = ++this.idCounter
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
        this.pieces.push(piece)
        return piece
    }

    findPieceById(id) {
        let foundPiece = this.pieces
            .filter((piece) => piece instanceof Rectangle && piece.id() == id)
        if (foundPiece.length) {
            return foundPiece[0]
        }
    }

    handleDragStart(e) {
        this.adjustAvailableReadyPieces()
        let movingPiece = this.getOrCreatePiece(e)
        this.isTimeToSnap = false
        setTimeout(() => {
            this.isTimeToSnap = true
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
        this.markThatThePieceEnteredPlaceholdersArea(movingPiece)
        this.changePieceColorIfOutside(movingPiece)
    }

    handleDragStop(e) {
        let piece = this.getOrCreatePiece(e)
        if (!this.removeIfOutside(piece)) {
            this.snapToPlaceholder(piece)
        }
        this.movePiecesToLeft()
        this.removeRemainingPlaceholders()
        this.adjustAreaWidth()
        this.adjustPiecesToPlaceholders()
        this.addRightPlaceholder()
        this.notifyChangedPieces()
    }

    markThatThePieceEnteredPlaceholdersArea(movingPiece) {
        const placeholdersRectangle = this.getPlaceholdersRectangle()
        if (placeholdersRectangle.contains(movingPiece.center())) {
            movingPiece.enteredPlaceholdersArea = true
        }
    }

    changePieceColorIfOutside(movingPiece) {
        if (movingPiece.enteredPlaceholdersArea) {
            this.addOrRemoveOutsideBorder(movingPiece)
        }
    }

    addOrRemoveOutsideBorder(movingPiece) {
        const nowIsOutside = !this.getPlaceholdersRectangle().contains(movingPiece.center())
        const elm = movingPiece.$elm.find('img')[0]
        const $img = $(elm)
        let src = $img.attr('src')
        const hasOutsideBorder = src.indexOf('_exit') !== -1
        if (nowIsOutside && !hasOutsideBorder) {
            const split = src.split('.')
            src = split[0] + '_exit.svg'
            $img.attr('src', src)
        } else if (!nowIsOutside && hasOutsideBorder) {
            src = src.replace('_exit', '')
            $img.attr('src', src)
        }
    }

    getPlaceholdersRectangle() {
        return new Rectangle($('#placeholders-area'))
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
        this.placeholders.forEach((placeholder) => {
            if (!placeholder.empty()) {
                placeholder.internalRectangle.moveTo(placeholder)
            }
        })
    }

    organizePostionZ(piece) {
        let id = piece.id()
        let i = this.clickedIds.indexOf(id)
        if (i != -1) {
            this.clickedIds.splice(i, 1)
        }
        this.clickedIds.push(id)
        $('.piece.dragged').each((i, elm) => {
            let zIndex = this.clickedIds.indexOf(elm.id) + 10
            $(elm).css('z-index', zIndex)
        })
    }

    movePiecesToLeft() {
        let i = 0
        while (i < this.placeholders.length) {
            let placeholder = this.placeholders[i]
            if (placeholder.empty()) {
                let j = i
                let rightOccuped
                while (j < this.placeholders.length && !rightOccuped) {
                    if (!this.placeholders[j].empty()) {
                        rightOccuped = this.placeholders[j]
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
        const PIECE_SIZE = 50
        const SCREEN_WIDTH = $(window).width()
        const snapedPiecesNumber = this.getOccupedPlaceholders().length
        const newWidth = snapedPiecesNumber * PIECE_SIZE + PIECE_SIZE + PIECE_SIZE + PIECE_SIZE
        this.$placeholdersArea.css('width', newWidth < SCREEN_WIDTH ? SCREEN_WIDTH : newWidth)
    }

    getOccupedPlaceholders() {
        return this.placeholders.filter(p => !p.empty())
    }

    getFreePlaceholders() {
        return this.placeholders.filter(p => p.empty())
    }

    getSnappedPieces() {
        return this.getOccupedPlaceholders().map((placeholder) => placeholder.internalRectangle)
    }

    freesPlaceholder(movingPiece) {
        let freedPlaceholder = this.placeholders.filter(placeholder => placeholder.has(movingPiece))[0]
        if (freedPlaceholder) {
            freedPlaceholder.frees()
        }
    }

    removeIfOutside(piece) {
        const outside = !this.getPlaceholdersRectangle().contains(piece.center())
        if (outside && piece.enteredPlaceholdersArea) {
            piece.disappear()
            this.pieces.splice(this.pieces.indexOf(piece), 1)
            return true
        }
    }

    snapToPlaceholder(piece) {
        this.placeholders.forEach((placeholder) => {
            if (placeholder.empty() && placeholder.contains(piece.center())) {
                this.snap(placeholder, piece)
                return
            }
        })
    }

    snap(placeholder, piece) {
        placeholder.add(piece)
        piece.moveTo(placeholder)
        audio.play()
    }

    addRightPlaceholder() {
        const ocuppedPlaceholders = this.getOccupedPlaceholders()
        if (this.placeholders.length === ocuppedPlaceholders.length) {
            this.createPlaceholder(Rectangle.prototype.RIGHT)
            this.adjustPiecesToPlaceholders()
        }
    }

    removeRemainingPlaceholders() {
        let ocupped = this.getOccupedPlaceholders().length
        if (this.placeholders.length > ocupped + 3) {
            this.placeholders.pop()
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
            this.$placeholdersArea.prepend($placeholderClone)
            this.placeholders.unshift(placeholder)
        } else {
            this.$placeholdersArea.append($placeholderClone)
            this.placeholders.push(placeholder)
        }
        this.updatePlaceholderElements()
        return placeholder
    }

    updatePlaceholderElements() {
        $('.block.placeholder').each((idx, elm) => {
            this.placeholders[idx].setElm($(elm))
        })
    }

    getOrCreatePlaceholder(side, placeholderIndex) {
        if (!this.placeholders[placeholderIndex + side]) {
            let newPlacehoder = this.createPlaceholder(side)
            this.adjustPiecesToPlaceholders()
            return newPlacehoder
        }
        return this.placeholders[placeholderIndex + side]
    }

    moveSnapedPieceIfIsTimeToSnap(movingPiece) {
        if (this.isTimeToSnap)
            this.moveSnapedPiece(movingPiece)
    }

    moveSnapedPiece(movingPiece) {
        this.placeholders.forEach((placeholder, placeholderIndex) => {
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
        if (placeholderIndex == this.placeholders.length - 1) {
            return Rectangle.prototype.LEFT
        }

        let commingSide = placeholder.sideOf(movingPiece.center())
        if (commingSide === Rectangle.prototype.LEFT) {
            if (this.placeholders[placeholderIndex - 1].empty()) {
                return Rectangle.prototype.LEFT
            }
            if (this.placeholders[placeholderIndex + 1].empty()) {
                return Rectangle.prototype.RIGHT
            }
        }
        if (commingSide === Rectangle.prototype.RIGHT) {
            if (this.placeholders[placeholderIndex + 1].empty()) {
                return Rectangle.prototype.RIGHT
            }
            if (this.placeholders[placeholderIndex - 1].empty()) {
                return Rectangle.prototype.LEFT
            }
        }
        return Rectangle.prototype.RIGHT
    }

    movePlaceholderPiece(placeholder, moveThisPieceToSide) {
        let placeholderIndex = this.placeholders.indexOf(placeholder)
        let placeholdeToGo = this.getOrCreatePlaceholder(moveThisPieceToSide, placeholderIndex)
        if (!placeholdeToGo.empty()) {
            this.movePlaceholderPiece(placeholdeToGo, moveThisPieceToSide)
        }
        this.snap(placeholdeToGo, placeholder.internalRectangle)
        placeholder.frees()
    }

    setCommands(commands) {
        /** The expected behaviour is that commands arrived
         * from RoPE will have 
         * a) one more command than this number of pieces (user clickes RoPE's buttons)
         * b) none command (the commands are cleared)
         * 
         * The first n - 1 commands must be equal in RoPE and here.
         * If this is false, than remove all pieces from here
         * and syncronize.
         * If true just add the new RoPE's command on this list.
         */

        const snappedPieces = this.getSnappedPieces()

        if (this.syncronized(snappedPieces, commands.slice(0, -1))) {
            this.addPieceFrom(commands[commands.length - 1])
        } else {
            this.removeSnappedPieces()
            this.removeRemainingPlaceholders()

            if (this.highlightPiece) {
                this.removePiece(this.highlightPiece)
                this.highlightPiece = undefined
            }

            commands.forEach((command) => this.addPieceFrom(command))
        }

        this.adjustAreaWidth()
        this.adjustPiecesToPlaceholders()
    }

    syncronized(pieces, commands) {
        if (pieces.length != commands.length)
            return false
        for (let i = 0; i < commands.length; i++) {
            const command = commands[i]
            if (!pieces[i].$elm.hasClass(command)) {
                return false
            }
        }
        return true
    }

    addPieceFrom(command) {
        const piece = this.cloneAndCreatePiece($('.available.piece.' + command))
        const placeholder = this.getFreePlaceholders()[0]
        piece.moveTo(placeholder)
        this.snap(placeholder, piece)
        this.movePiecesToLeft()
        this.addRightPlaceholder()
    }

    removeSnappedPieces() {
        this.getOccupedPlaceholders().forEach((placeholder) => {
            placeholder.internalRectangle.$elm.remove()
            placeholder.frees()
        })
    }

    highlightSnapped() {
        this.getSnappedPieces().forEach(piece => {
            piece.$elm.css('z-index', 10)
        })
    }

    hideHighlight() {
        if (!this.highlightPiece) return
        this.removePiece(this.highlightPiece)
        this.highlightPiece = undefined
    }

    disableDragging() {
        $('.ui-draggable').draggable('disable')
    }

    enableDragging() {
        $('.ui-draggable-handle').draggable('enable')
    }

    highlight({ command, index }) {
        const placeholder = this.getOccupedPlaceholders()[index]
        if (!placeholder)
            return

        const piece = placeholder.internalRectangle
        if (!this.highlightPiece) {
            const $cloned = this.clone(piece.$elm);
            $cloned.draggable('disable')
            $cloned.find('img').attr('src', 'assets/executing.svg')
            $cloned.appendTo($('#placeholders-area'))
            $cloned.css('z-index', 99)
            this.highlightPiece = new Rectangle($cloned)
        }
        this.highlightPiece.moveTo(piece.$elm)
    }

    removePiece(piece) {
        piece.disappear()
        piece = undefined
    }

    on(event, callback) {
        if (!this[event]) {
            this[event] = []
        }
        this[event].push(callback)
    }

    notify(event, param) {
        this[event].forEach(callback => callback.call(this, param))
    }

    notifyChangedPieces() {
        const pieces = this.getSnappedPieces()
        this.notify('changed', pieces)
    }

    pointToIndex(index) {
        const placeholder = this.placeholders[index]
        if (!placeholder)
            return
        const $pointer = $('#pointer')
        const rect = new Rectangle($pointer)
        const point = new Point(placeholder.x, placeholder.y + 60)
        rect.moveTo( point )
        $pointer.fadeIn(400)
    }

}
