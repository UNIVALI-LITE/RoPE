/*global $ global jQuery*/

// Hey, the main() is bottom

$(function () {
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
            this.y = $elm.position().top
            this.x = $elm.position().left
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
                    top: obj.position().top,
                    left: obj.position().left
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

    const organizePostionZ = (piece) => {
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

    const showPlaceholders = () => {
        console.log(placeholders.map(p => p.empty() ? 'vazio' : p.internalRectangle.id()))
    }

    const findPieceById = (id) => {
        let foundPiece = pieces
            .filter((piece) => piece instanceof Rectangle && piece.id() == id)
        if (foundPiece.length) {
            return foundPiece[0]
        }
    }

    const createPiece = ($elm) => {
        let piece = new Rectangle($elm)
        pieces.push(piece)
        return piece
    }

    const getOrCreatePiece = (e) => {
        let elm = e.target
        let id = elm.id
        let foundPiece = findPieceById(id)
        return foundPiece || createPiece($(elm))
    }

    const movePiecesToLeft = () => {
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
                    snap(placeholder, rightOccuped.internalRectangle)
                    rightOccuped.frees()
                }
            }
            i++
        }
    }

    const adjustPiecesToPlaceholders = () => {
        placeholders.forEach((placeholder) => {
            if (!placeholder.empty()) {
                placeholder.internalRectangle.moveTo(placeholder)
            }
        })
    }

    const adjustAvailableReadyPieces = () => {
        // Ready pieces are available and not dragged.
        // This function adjusts this left position, relative to the document
        $('.ready.piece').each((idx, elm) => {
            const command = $(elm).attr('data-command')
            const originalElement = $( '.available.' + command )
            $(elm).css('left', originalElement.offset().left)
        })
    }

    const getOccupedPlaceholders = () => {
        return placeholders.filter(p => !p.empty())
    }

    const removePlaceholders = () => {
        let ocupped = getOccupedPlaceholders().length
        if (placeholders.length > ocupped + 3) {
            placeholders.pop()
            $('.placeholder').last().remove()
            adjustPiecesToPlaceholders()
        }
    }

    const freesPlaceHolder = (movingPiece) => {
        let freedPlaceholder = placeholders.filter(placeholder => placeholder.has(movingPiece))[0]
        if (freedPlaceholder) {
            freedPlaceholder.frees()
        }
        showPlaceholders()
    }

    const handleDragStart = (e) => {
        adjustAvailableReadyPieces()
        let movingPiece = getOrCreatePiece(e)
        isTimeToSnap = false
        setTimeout(() => {
            isTimeToSnap = true
        }, 300)
        freesPlaceHolder(movingPiece)
        organizePostionZ(movingPiece)
        if (!movingPiece.dragged) { // first move, create new piece below, to be used after
            clone(movingPiece.$elm)
            movingPiece.setDragged()
        }
    }

    const removeIfOutside = (piece) => {
        const rect = new Rectangle( $( 'body' ) )
        if (!rect.contains(piece.center())) {
            piece.disappear()
            pieces.splice(pieces.indexOf(piece), 1)
            return true
        }
    }

    const snap = (placeholder, piece) => {
        if (!placeholder.empty()) {
            return console.log('Não to vazio!!!')
        }
        placeholder.add(piece)
        piece.moveTo(placeholder)
        const audio = new Audio('assets/snapsound.mp3')
        audio.play()
    }

    const snapToPlaceholder = (piece) => {
        placeholders.forEach((placeholder) => {
            if (placeholder.empty() && placeholder.contains(piece.center())) {
                snap(placeholder, piece)
                return
            }
        })
    }

    const addRightPlaceholder = () => {
        const ocuppedPlaceholders = getOccupedPlaceholders()
        if (placeholders.length === ocuppedPlaceholders.length) {
            createPlaceholder(Rectangle.prototype.RIGHT)
            adjustPiecesToPlaceholders()
        }
    }

    const adjustAreaWidth = () => {
        const snapedPiecesNumber = getOccupedPlaceholders().length
        if (snapedPiecesNumber > snapedPiecesWithoutOverflow) {
            const newWidth = snapedPiecesNumber * PIECE_SIZE + PIECE_SIZE + PIECE_SIZE + PIECE_SIZE
            $placeholdersArea.css('width', newWidth < SCREEN_WIDTH ? SCREEN_WIDTH : newWidth)
            rectArea = new Rectangle($('#placeholders-area'))
        }
    }

    const handleDragStop = (e) => {
        let piece = getOrCreatePiece(e)
        if (!removeIfOutside(piece)) {
            snapToPlaceholder(piece)
        }
        movePiecesToLeft()
        removePlaceholders()
        showPlaceholders()
        adjustAreaWidth()
        addRightPlaceholder()
    }

    const updatePlaceholdersElement = () => {
        $('.block.placeholder').each((idx, elm) => {
            placeholders[idx].setElm($(elm))
        })
    }

    const createPlaceholder = (side) => {
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
        updatePlaceholdersElement()
        return placeholder
    }

    const getOrCreatePlaceholder = (side, placeholderIndex) => {
        if (!placeholders[placeholderIndex + side]) {
            let newPlacehoder = createPlaceholder(side)
            adjustPiecesToPlaceholders()
            return newPlacehoder
        }
        return placeholders[placeholderIndex + side]
    }

    const movePlaceholderPiece = (placeholder, moveThisPieceToSide) => {
        let placeholderIndex = placeholders.indexOf(placeholder)
        let placeholdeToGo = getOrCreatePlaceholder(moveThisPieceToSide, placeholderIndex)
        if (!placeholdeToGo.empty()) {
            movePlaceholderPiece(placeholdeToGo, moveThisPieceToSide)
        }
        snap(placeholdeToGo, placeholder.internalRectangle)
        placeholder.frees()
    }

    const calcMoveSide = (placeholder, placeholderIndex, movingPiece) => {
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

    const ifHasPieceThenMove = (placeholder, placeholderIndex, movingPiece) => {
        if (!placeholder.empty() &&
            placeholder.contains(movingPiece.center())) {
            let moveSide = calcMoveSide(placeholder, placeholderIndex, movingPiece)
            movePlaceholderPiece(placeholder, moveSide)
        }
    }

    const moveSnapedPiece = (movingPiece) => {
        placeholders.forEach((placeholder, placeholderIndex) => {
            ifHasPieceThenMove(placeholder, placeholderIndex, movingPiece)
        })
    }

    const moveSnapedPieceIfIsTimeToSnap = (movingPiece) => {
        if (isTimeToSnap)
            moveSnapedPiece(movingPiece)
    }

    const handleDrag = (e) => {
        let movingPiece = getOrCreatePiece(e)
        movingPiece.setElm($(e.target))
        moveSnapedPieceIfIsTimeToSnap(movingPiece)
    }

    const clone = ($elm) => {
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
                start: handleDragStart,
                stop: handleDragStop,
                drag: handleDrag,
                scroll: false
            })

        let id = ++idCounter
        $cloned.id = id
        $cloned[0].id = id
        return $cloned
    }

    // main :)

    $('.available.block.piece').on('mousedown', (e) => {
        let $cloned = clone($(e.target))
        createPiece($cloned)
    }).trigger('mousedown')

    $('.block.placeholder').each((idx, elm) => {
        placeholders.push(new Rectangle($(elm)))
    })

    $(window).on('scroll', () => {
        $('.ready.piece').hide() // because trembles on mobile when scroll
        clearTimeout($.data(this, 'scrollTimer'));
        $.data(this, 'scrollTimer', setTimeout(() => {
            $('.ready.piece').show()
            adjustAvailableReadyPieces()
        }, 250))
    })
    

})

