/*global $ global jQuery*/

// Hey, the main() is bottom
$(function () {
    function Point(x, y) {
        this.x = x
        this.y = y
    }

    function Rectangle($elm) {
        this.$elm = $elm
        this.height = $elm.height()
        this.width = $elm.width()
        this.y = $elm.position().top
        this.x = $elm.position().left
    }
    Rectangle.prototype.contains = function (obj) {
        if (obj instanceof Point) {
            var point = obj
            return this.y <= point.y &&
                this.y + this.height >= point.y &&
                this.x <= point.x &&
                this.x + this.width >= point.x
        }
    }
    Rectangle.prototype.center = function () {
        var x = this.x + (this.width / 2)
        var y = this.y + (this.height / 2)
        return new Point(x, y)
    }
    Rectangle.prototype.moveTo = function (obj) {
        if (obj instanceof jQuery) {
            var $elm = this.$elm
            $elm[0].moving = true
            this.$elm.animate({
                top: obj.position().top,
                left: obj.position().left,
                position: 'absolute'
            }, 100, function(){ 
                $elm[0].moving = false
            })
        }
    }
    Rectangle.prototype.LEFT = -1
    Rectangle.prototype.RIGHT = 1
    Rectangle.prototype.sideOf = function(obj) {
        if(obj instanceof Point){
            return this.center().x > obj.x ? this.LEFT : this.RIGHT
        }
    }
    
    var idCounter = 0
    var placeholders = {}
    var clickedIds = []
    var $totalArea = $('#total-area')
    var $placeholdersArea = $('.placeholders.area')

    var organizePostionZ = ($piece) => {
        var id = $piece[0].id
        var i = clickedIds.indexOf(id)
        if (i != -1) {
            clickedIds.splice(i, 1)
        }
        clickedIds.push(id)
        $('.piece.dragged').each((i, elm) => {
            var zIndex = clickedIds.indexOf(elm.id) + 10
            $(elm).css('z-index', zIndex)
        })
    }

    var showPlaceholders = ()=>{
        let ids = []
        $('.placeholder').each((idx, elm)=>{
            if(placeholders[idx]){
                ids.push( placeholders[idx][0].id )
            } else {
                ids.push(-1)
            }
        })
        // for(let idx in placeholders){
        //     ids.push(placeholders[idx][0].id)
        // }
        console.log(ids)
    };

    var freesPlaceHolder = ($piece) => {
        for (let idx in placeholders) {
            var $snapedPiece = placeholders[idx]
            if ($snapedPiece[0].id === $piece[0].id) {
                delete placeholders[idx]
                break
            }
        }
        showPlaceholders()
    }

    var handleDragStart = (e) => {
        var piece = e.target
        var $piece = $(piece)
        freesPlaceHolder($piece)
        organizePostionZ($piece)
        if (!piece.dragged) {
            piece.dragged = true
            clone($piece)
            $piece.addClass('dragged')
            $piece.css('z-index',1000)
        }
    }

    var removeIfOutside = ($piece) => {
        var rectArea = new Rectangle($totalArea)
        var pieceRect = new Rectangle($piece)
        if (!rectArea.contains(pieceRect.center())) {
            $piece.remove()
        }
    }

    var snap = ($piece, $placeholder) => {
        var pieceRect = new Rectangle($piece)
        pieceRect.moveTo($placeholder)
    }

    var snapToPlaceholder = ($piece) => {
        var pieceRect = new Rectangle($piece)
        $('.placeholder').each((idx, elm) => {
            var $placeholder = $(elm)
            var placeholderRect = new Rectangle($placeholder)
            if (!placeholders[idx] && placeholderRect.contains(pieceRect.center())) {
                snap($piece, $placeholder)
                placeholders[idx] = $piece
                return
            }
        })
    }
    
    var handleDragStop = (e) => {
        var $piece = $(e.target)
        removeIfOutside($piece)
        snapToPlaceholder($piece)
        showPlaceholders()
    }

    var createPlaceholder = (side) => {
        var $placeholderBase = $($('.placeholder')[0])
        var $placeholderClone = $placeholderBase.clone()
        if(side === Rectangle.LEFT){
            $placeholdersArea.prepend($placeholderClone)
        } else {
            $placeholdersArea.append($placeholderClone)
        }
        return $placeholderClone
    }

    var getOrCreatePlaceholder = ($placeholders, placeholderIndex, side) => {
        var placeholderContainsPiece = placeholders[ placeholderIndex + side]
        if( placeholderContainsPiece ){
            var sideOfSide = side + side
            return getOrCreatePlaceholder($placeholders, placeholderIndex, sideOfSide )       
        }
        var elm = $placeholders[placeholderIndex + side]
        if(!elm){
            return createPlaceholder(side)
        }
        return $(elm)
    }
    
    var moveSnapedPiece = ($movingPiece) => {
        var pieceCenter = new Rectangle($movingPiece).center()
        var $placeholders = $('.placeholder')
        $placeholders.each((idx, elm) => {
            var $placeholder = $(elm)
            var placeholderRect = new Rectangle($placeholder)
            if (placeholders[idx] && placeholderRect.contains(pieceCenter)) {
                var elm = placeholders[idx][0]
                if( elm.moving )
                    return
                var moveSide = placeholderRect.sideOf( pieceCenter )
                
                var $placeholderToGo = getOrCreatePlaceholder($placeholders, idx, moveSide)
                
                var $snapedPiece = placeholders[idx]
                freesPlaceHolder($snapedPiece)
                var $freedPiece = $snapedPiece
                var freedPieceRect = new Rectangle($freedPiece)
                freedPieceRect.moveTo($placeholderToGo)
                placeholders[idx+moveSide] = $freedPiece
            }
        })
    }
    
    var handleDrag = (e) => {
        var $piece = $(e.target)
        moveSnapedPiece($piece)
    }

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
            stop: handleDragStop,
            drag: handleDrag
        })
        
        var elm = $cloned[0]
        elm.cloned = true
        elm.dragged = false
        elm.id = ++idCounter
        
        $cloned.text( elm.id )
        
        return $cloned
    }

    // main :)
    $('.available.block.piece').on('mousedown', (e) => {
        var $piece = $(e.target)
        var $clonedPiece = clone($piece)
    }).trigger('mousedown')
})