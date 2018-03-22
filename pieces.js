/*global $ global jQuery*/

// Hey, the main() is bottom
$(function () {
    function Point(x, y) {
        this.x = x
        this.y = y
    }

    function Rectangle($elm) {
        this.setElm($elm)
    }
    Rectangle.prototype.setElm = function($elm) {
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
        var centerX = this.x + (this.width / 2)
        var centerY = this.y + (this.height / 2)
        return new Point(centerX, centerY)
    }
    Rectangle.prototype.moveTo = function (obj) {
        if (obj instanceof Rectangle) {
            obj = obj.$elm
        }
        if (obj instanceof jQuery) {
            var $elm = this.$elm
            this.moving = true
            this.$elm.animate({
                top: obj.position().top,
                left: obj.position().left,
                position: 'absolute'
            }, 100, function(){ 
                this.moving = false
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
    Rectangle.prototype.id = function(){
        return this.$elm[0].id
    }
    Rectangle.prototype.setDragged = function(){
        this.dragged = true
        this.$elm.addClass('dragged').css('z-index',1000)
    }
    Rectangle.prototype.disappear = function() {
        this.$elm.remove()
    }
    Rectangle.prototype.empty = function(){
        return !this.internalRectangle
    }
    Rectangle.prototype.add = function(obj){
        return this.internalRectangle = obj
    }
    Rectangle.prototype.has = function(obj){
        return obj === this.internalRectangle
    }
    Rectangle.prototype.frees = function() {
        this.internalRectangle = undefined
    }
    const EMPTY = -1
    var idCounter = 0
    var placeholderIdCounter = 5
    var pieces = []
    var placeholders = []
    var clickedIds = []
    var $totalArea = $('#total-area')
    var rectArea = new Rectangle($totalArea)
    var $placeholdersArea = $('.placeholders.area')
    
    const organizePostionZ = (piece) => {
        var id = piece.id()
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

    const showPlaceholders = () => {
        console.log( placeholders.map(p=> p.empty() ? 'vazio' : p.internalRectangle.id()))
    }

    const findPieceById = (id) => {
        var foundPiece = pieces
        .filter((piece)=>piece instanceof Rectangle && piece.id() == id)
        if(foundPiece.length){
            return foundPiece[0]
        }
    }
    
    const createPiece = ($elm) => {
        var piece = new Rectangle( $elm )
        pieces.push( piece )
        return piece
    }
    
    const getOrCreatePiece = (e) => {
        var elm = e.target
        var id = elm.id
        var foundPiece = findPieceById(id)
        return foundPiece || createPiece( $(elm) )
    }

    const freesPlaceHolder = (movingPiece) => {
        placeholders
        .filter(placeholder=>placeholder.has(movingPiece))
        .forEach(placeholder=>placeholder.frees())
        showPlaceholders()
    }

    const handleDragStart = (e) => {
        var movingPiece = getOrCreatePiece(e)
        freesPlaceHolder(movingPiece)
        organizePostionZ(movingPiece)
        if (!movingPiece.dragged) {
            clone( movingPiece.$elm )
            movingPiece.setDragged()
        }
    }

    const removeIfOutside = (piece) => {
        if (!rectArea.contains(piece.center())) {
            piece.disappear()
        }
    }

    const snap = (placeholder, piece) => {
        if(!placeholder.empty()){
            return console.log('Não to vazio!!!')
        }
        placeholder.add( piece )
        piece.moveTo(placeholder)
    }

    const snapToPlaceholder = (piece) => {
        placeholders.forEach((placeholder)=>{
            if(placeholder.empty() && placeholder.contains(piece.center())){
                snap(placeholder, piece)
                return 
            }
        })
    }
    
    const handleDragStop = (e) => {
        var piece = getOrCreatePiece(e)
        removeIfOutside(piece)
        snapToPlaceholder(piece)
        showPlaceholders()
    }

    const createPlaceholder = (side) => {
        let $placeholderBase = $($('.placeholder')[0])
        let $placeholderClone = $placeholderBase.clone()
        $placeholderClone.text( ++placeholderIdCounter )
        let placeholder = new Rectangle( $placeholderClone )
        if(side === Rectangle.prototype.LEFT){
            $placeholdersArea.prepend($placeholderClone)
            placeholders.unshift( placeholder )
        } else {
            $placeholdersArea.append($placeholderClone)
            placeholders.push( placeholder )
        }
        return placeholder
    }

    const getOrCreatePlaceholder = (side, placeholderIndex) => {
        if( !placeholders[ placeholderIndex + side ])
            return createPlaceholder(side)
        return placeholders[ placeholderIndex + side]
    }
    
    const movePlaceholderPiece = ( placeholder, moveThisPieceToSide ) => {
        let placeholderIndex = placeholders.indexOf( placeholder )
        let placeholdeToGo = getOrCreatePlaceholder(moveThisPieceToSide, placeholderIndex)
        if(!placeholdeToGo.empty()) {
            placeholdeToGo.internalRectangle.moving = true
            movePlaceholderPiece( placeholdeToGo, moveThisPieceToSide )
        }
        snap( placeholdeToGo, placeholder.internalRectangle )
        placeholder.frees()
    }
    
    const ifHasPieceThenMove = (placeholder, movingPiece)=>{
        if(!placeholder.empty() && 
            placeholder.contains( movingPiece.center() )){
            let moveSide = placeholder.sideOf( movingPiece.center() )
            movePlaceholderPiece( placeholder, moveSide )
        }
    }
    
    const moveSnapedPiece = (movingPiece) => {
        placeholders.forEach((placeholder) => {
            ifHasPieceThenMove(placeholder, movingPiece)
        })
    }
    
    const handleDrag = (e) => {
        var movingPiece = getOrCreatePiece(e)
        movingPiece.setElm($(e.target))
        moveSnapedPiece(movingPiece)
    }

    const clone = ($elm) => {
        var position = $elm.position()
        var $cloned = $elm.clone()
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
        
        var id = ++idCounter
        $cloned.id = id
        $cloned.text( id )
        $cloned[0].id = id
        
        return $cloned
    }

    // main :)
    $('.available.block.piece').on('mousedown', (e) => {
        var $cloned = clone( $(e.target) )
        createPiece( $cloned )
    }).trigger('mousedown')
    
    $('.block.placeholder').each((idx, elm)=>{
        placeholders.push( new Rectangle( $(elm) ))
    })
})