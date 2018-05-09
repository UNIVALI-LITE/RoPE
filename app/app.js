/*global $ global bluetooth navigator*/
$(function () {

    const dictionary = {
        a: 'advance',
        r: 'back',
        e: 'left',
        d: 'right'
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
        executedIndex: -1
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
        app.bluetooth.setCharacteristic('s')
    })

    app.bluetooth.on('connected', () => {
        app.showProgrammingView()
        app.setConnected(true)
    })

    app.bluetooth.on('connection-failed', () => {
        app.showMagnifying(false)
        app.setConnected(false)
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

    app.showDebugging = () => {
        $('#debug-button').toggleClass('active')
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
        if (connected)
            $('#rope-connection').addClass('connected').removeClass('disconnected')
        else
            $('#rope-connection').addClass('disconnected').removeClass('connected')
    }

    app.handleChangeOn = (characteristic) => {
        const characteristicSplit = characteristic.split(':')
        const action = characteristicSplit[0]
        console.log(characteristic)
        switch (action) {
            case 'iniciou':
                app.blocks.disableDragging()
                app.showShadow()
                app.started = true
                return app.pointPieceToExecute()
            case 'parou': 
                app.blocks.enableDragging()
                app.executedIndex = -1
                app.started = false
                return app.showStopped()
            case 'alterou_comandos':
                let commands = characteristicSplit[1]
                commands = app.translate(commands)
                return app.showAdded(commands)
            case 'ini':
                let command = characteristicSplit[1]
                let index = characteristicSplit[2]
                app.executedIndex = new Number(index)
                app.showStartedAction({ command, index })
                return app.pointPieceToExecute()
            case 'fim':
                app.blocks.hideHighlight()
                app.pointPieceToExecute()
                return
            default:
                break
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
        app.debug = !app.debug
        app.bluetooth.setCharacteristic('d:' + (app.debug ? 1 : 0))
        app.showDebugging()
    }

    app.setPiecesCharacteristic = (pieces) => {
        let characteristic = ''
        pieces.forEach(piece => {
            const command = piece.$elm.attr('data-command')
            characteristic += command.charAt(0)
        })
        app.bluetooth.setCharacteristic(characteristic)
    }

    // Start

    app.registerServiceWorker()

})
