/*global $ global bluetooth navigator*/
$(function () {

    const app = {
        bluetooth: bluetooth,
        blocks: new BlocksView()
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
        $('#programming-view').hide(400, () => $('#connecting-view').show())
    }

    app.showDebugging = () => {
        $('#debug-button').toggleClass('active')
    }

    app.showStarted = () => {
        $('#go-block').css('border', '3px solid green')
    }

    app.showStopped = () => {
        $('#go-block').css('border', '3px solid red')
    }

    app.showAddedCommand = (commands) => {
        $('#go-block').css('border', 'none')
        app.blocks.setCommands(commands)
    }

    // Methods to dealing with the model

    app.startSearch = () => {
        app.bluetooth.search()
        app.showConnectionView()
        app.showMagnifying(true)
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
            case 'started':
                return app.showStarted()
            case 'stopped':
                return app.showStopped()
            case 'updatedCommands':
                let commands = characteristicSplit[1]
                commands = app.translate( commands )
                return app.showAddedCommand( commands )
            default:
                break
        }
    }

    app.translate = (commandsStr) => {
        const dictionary = {
            a: 'advance',
            b: 'back',
            l: 'left',
            r: 'right'
        }
        const commands = []
        for (let i = 0; i < commandsStr.length; i++) {
            const char = commandsStr.charAt(i)
            commands.push(dictionary[char])
        }
        return commands
    }

    app.toggleDebug = () => {
        app.debug = !app.debugging
        app.showDebugging()
    }

    // Start

    // app.registerServiceWorker()

})
