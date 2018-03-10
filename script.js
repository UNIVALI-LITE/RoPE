/*global $ global jQuery*/
$(function () {

    function Point(x, y) {
        this.x = x
        this.y = y
    };

    function Rectangle($elm) {
        this.$elm = $elm
        this.height = $elm.height()
        this.width = $elm.width()
        this.y = $elm.position().top
        this.x = $elm.position().left
    };
    Rectangle.prototype.contains = function (obj) {
        if (obj instanceof Point) {
            return this.y <= obj.y &&
                this.y + this.height >= obj.y &&
                this.x <= obj.x &&
                this.x + this.width >= obj.x
        }
    };
    Rectangle.prototype.center = function () {
        var x = this.x + (this.width / 2)
        var y = this.y + (this.height / 2)
        return new Point(x, y)
    };
    Rectangle.prototype.moveTo = function (obj) {
        if (obj instanceof jQuery) {
            this.$elm.css({
                top: obj.position().top,
                left: obj.position().left,
                position: 'absolute'
            })
        }
    };

    var idCounter = 0
    var placeholders = {}
    var clickedIds = []
    var $totalArea = $('#total-area')

    var organizePostionZ = ($piece) => {
        var id = $piece[0].id
        var i = clickedIds.indexOf(id)
        if (i != -1) {
            clickedIds.splice(i, 1)
        }
        clickedIds.push(id)
        $('.piece.cloned').each((i, elm) => {
            var zIndex = clickedIds.indexOf(elm.id) + 10
            $(elm).css('z-index', zIndex)
        })
    };

    var handleDragStart = (e) => {
        var piece = e.target
        var $piece = $(piece)
        organizePostionZ($piece)
        if (!piece.dragged) {
            piece.dragged = true
            clone($piece)
            $piece.addClass('dragged')
            $piece.css('z-index',1000)
        }
    };

    var removeIfOutside = ($piece) => {
        var rectArea = new Rectangle($totalArea)
        var pieceRect = new Rectangle($piece)
        if (!rectArea.contains(pieceRect.center())) {
            $piece.remove()
        }
    };

    var freesPlaceHolder = ($piece) => {
        for (let idx in placeholders) {
            var $snapedPiece = placeholders[idx]
            if ($snapedPiece[0].id === $piece[0].id) {
                delete placeholders[idx]
                break
            }
        }
    };

    var snapToPlaceholder = ($piece) => {
        var pieceRect = new Rectangle($piece)
        $('.placeholder').each((idx, elm) => {
            var $placeholder = $(elm)
            var placeholderRect = new Rectangle($placeholder)
            if (!placeholders[idx] && placeholderRect.contains(pieceRect.center())) {
                pieceRect.moveTo($placeholder)
                placeholders[idx] = $piece
            }
        })
    };

    var handleDragStop = (e) => {
        var $piece = $(e.target)
        removeIfOutside($piece)
        freesPlaceHolder($piece)
        snapToPlaceholder($piece)
    };

    var clone = ($piece) => {
        var position = $piece.position()
        var $cloned = $piece.clone()
        $cloned
        .removeClass('available')
        .css({
            position: 'absolute',
            top: position.top,
            left: position.left
        })
        .appendTo($totalArea)
        .draggable({
            start: handleDragStart,
            stop: handleDragStop
        });
        
        var elm = $cloned[0]
        elm.cloned = true
        elm.dragged = false
        elm.id = ++idCounter
        
        return $cloned
    };

    $('.available.block.piece').on('mousedown', (e) => {
        var $piece = $(e.target)
        var $clonedPiece = clone($piece)
    }).trigger('mousedown')
})