/*global $ global bluetooth navigator BlocksView */
$(function () {

    const dictionary = {
        f: 'advance', // frente
        t: 'back', // tras
        e: 'left', // esquerda
        d: 'right' // direita
    }

    const app = {
        bluetooth: bluetooth,
        blocks: new BlocksView(),
        sounds: {
            start: new Audio('assets/startsound.wav'),
            stop: new Audio('assets/stopsound.wav'),
            error: new Audio('assets/error.flac'),
            next: new Audio('assets/next.wav'),
        },
        executedIndex: -1,
        commands : '',
        completedCommands : true
    }

    // Event listeners

    $('#magnifying-button').on('click', () => {
        app.startSearch()
    })

    $('#rope-connection').on('click', () => {
        if ($('#rope-connection').hasClass('disconnected')) {
            app.startSearch()
        }
    })

    $('#debug-button').on('click', () => {
        app.toggleDebug()
    })

    $('#go-block').on('click', () => {
        app.bluetooth.setCharacteristic('i') // iniciar
    })

    app.bluetooth.on('connected', () => {
        app.showProgrammingView()
        app.setConnected(true)
        app.bluetooth.setCharacteristic('l');
        clearInterval('changeSleepingImage')
        app.resetProgrammingView()
        app.debug = false
        app.showDebugging(app.debug)
        app.pointPieceToExecute()
    })

    app.bluetooth.on('connection-failed', () => {
        app.showMagnifying(false)
        app.setConnected(false)
        app.showSleepingRoPE()
        app.showConnectionView()
    })

    app.bluetooth.on('characteristic-changed', (characteristic) => {
        app.handleChangeOn(characteristic)
    })

    app.blocks.on('changed', (pieces) => {
        app.setPiecesCharacteristic(pieces)
    })

    app.blocks.on('click', (index) => {
        // If debug is active and execution started, must listen clicks on
        // next piece
        if (app.debug && app.started) {
            if (app.executedIndex + 1 != index) {
                app.sounds.error.play()
            } else {
                app.sounds.next.play()
                app.bluetooth.setCharacteristic('n')
            }
        }
    })

    $(window).on('scroll', () => {
        app.adjustShadowWidth()
    })

    // Methods to update ui

    app.showMagnifying = (show) => {
        if (show) {
            $('#magnifying').show('fast')
        } else {
            $('#magnifying').hide('slow')
        }
    }

    app.showProgrammingView = () => {
        $('#connecting-view').hide(400, () => $('#programming-view').show())
    }

    app.showConnectionView = () => {
        if ($('#connecting-view').is(':visible'))
            return
        $('#programming-view').hide(400, () => $('#connecting-view').show())
    }
    
    function changeSleepingImage(){
        if( $('#rope').attr('src') == 'assets/rope_not_found.svg') {
            $('#rope').attr('src','assets/rope_not_found_2.svg')
        } else {
            $('#rope').attr('src','assets/rope_not_found.svg')
        }
    }
    
    app.showSleepingRoPE = () => {
        setInterval(changeSleepingImage , 2000)
    }

    app.showDebugging = (show) => {
        if(show)
            $('#debug-button').addClass('active')
        else
            $('#debug-button').removeClass('active')
    }

    app.showShadow = () => {
        app.sounds.start.play()
        if (!$('#shadow').length) {
            $('<div id="shadow"></div>').css({
                width: '100%',
                height: '100vh',
                opacity: '0.5',
                background: 'gray',
                position: 'fixed',
                'z-index': '3',
                display: 'none'
            }).prependTo($('#programming-view'))
        }

        $('#shadow').fadeIn(400, 'linear')
        app.blocks.highlightSnapped()
    }

    app.showStopped = () => {
        app.sounds.stop.play()
        app.hideShadow()
        app.blocks.removeSnappedPieces()
        app.blocks.removeRemainingPlaceholders()
        app.blocks.adjustAreaWidth()
        app.blocks.hidePointer()
    }

    app.hideShadow = () => {
        app.blocks.hideHighlight()
        $('#shadow').fadeOut(1000, 'linear')
    }

    app.showAdded = (commands) => {
        app.blocks.setCommands(commands)
    }

    app.showStartedAction = ({ command, index }) => {
        app.blocks.highlight({ command, index })
    }

    app.pointPieceToExecute = () => {
        if (app.debug) {
            app.blocks.pointToIndex(app.executedIndex + 1)
        } else {
            app.blocks.hidePointer()
        }
    }

    app.adjustShadowWidth = () => {
        if ($('#shadow').length) {
            $('#shadow').width('100%')
        }
    }
    
    app.resetProgrammingView = () =>{
        app.hideShadow()
        app.blocks.removeSnappedPieces()
        app.blocks.enableDragging()
    }

    // Methods to dealing with the model

    app.startSearch = () => {
        app.bluetooth.search()
        app.showMagnifying(true)
        app.showConnectionView()
    }

    app.registerServiceWorker = () => {
        if ('serviceWorker' in navigator) {
            navigator.serviceWorker
                .register('service-worker.js').then((reg) => {
                    console.log('Service Worker Registered')
                })
        }
    }

    app.setConnected = (connected) => {
        if (connected){
            $('#rope-connection').addClass('connected').removeClass('disconnected')
        } else {
            $('#rope-connection').addClass('disconnected').removeClass('connected')
        }
    }

    app.handleChangeOn = (characteristic) => {
        
        if(characteristic.indexOf('ini') == -1 &&
           characteristic.indexOf('fim') == -1 &&
           characteristic.indexOf('parou') == -1 &&
           characteristic.indexOf('d:0') == -1 && 
           characteristic.indexOf('d:1') == -1 
           ) { 
               
            if( characteristic.indexOf('alt_cmds') != -1 ) { // novos comandos
                app.commands = characteristic.split(':')[1]
            } else {
            
                if(characteristic.indexOf('&') == -1){ // ainda terá mais comandos
                    app.commands += characteristic   
                } else {   // agora finalizou
                    app.commands += characteristic.substr(0, characteristic.length - 1)
                }
                
            }
            
            if(characteristic.indexOf('&') != -1 ) {
                const commands = app.translate( app.commands )
                app.blocks.setCommands( commands )
            }
            
        } else {
            const characteristicSplit = characteristic.split(':')
            const action = characteristicSplit[0]
            switch (action) {
                case 'iniciou':
                    app.blocks.disableDragging()
                    app.showShadow()
                    app.started = true
                    app.pointPieceToExecute()
                    break
                case 'parou': 
                    app.blocks.enableDragging()
                    app.executedIndex = -1
                    app.started = false
                    app.showStopped()
                    break
                case 'ini':
                    let index = characteristicSplit[1]
                    app.executedIndex = new Number(index)
                    app.showStartedAction({ index })
                    return app.pointPieceToExecute()
                case 'fim':
                    app.blocks.hideHighlight()
                    app.pointPieceToExecute()
                    break;
                case 'd':
                    app.debug = characteristicSplit[1] == "1"
                    app.showDebugging(app.debug)
                    break;
                default:
                    break
            }
        }
    }

    app.translate = (commandsStr) => {
        const commands = []
        for (let i = 0; i < commandsStr.length; i++) {
            const char = commandsStr.charAt(i)
            commands.push(dictionary[char])
        }
        return commands
    }

    app.toggleDebug = () => {
        app.bluetooth.setCharacteristic('d:' + (app.debug ? 0 : 1))
    }

    app.setPiecesCharacteristic = (pieces) => {
        let comandos = ''
        pieces.forEach(piece => {
            const command = piece.$elm.attr('data-command')
            comandos += command.charAt(0)
        })
        app.bluetooth.setCharacteristic('cmds:'+comandos+'&')
    }

    // Start

    app.registerServiceWorker()

})
