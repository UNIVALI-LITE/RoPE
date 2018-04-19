/*global $ global bluetooth navigator*/
$(function () {

    const app = {
        bluetooth: bluetooth,
        blocks: new BlocksView()
    }

    // Event listeners

    $('#magnifying-button, #rope-connection.disconnected').on('click', () => {
        //app.startSearch()
        app.showProgrammingView()
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
        app.updateBlocks(characteristic)
    })

    // Methods to update ui

    app.showMagnifying = (show) => {
        if (show) {
            $('#magnifying').show('fast')
        }
        else {
            $('#magnifying').hide('slow')
        }
    }

    app.showProgrammingView = () => {
        $('#connecting-view').hide(400, () => $('#programming-view').show())
    }

    // Methods to dealing with the model

    app.startSearch = () => {
        app.bluetooth.search()
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

    app.updateBlocks = (characteristic) => {

    }

    // Start

    // app.registerServiceWorker()

})
