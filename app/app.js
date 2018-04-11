/*global $ global bluetooth navigator*/
$(function() {

    const app = {
        bluetooth: bluetooth
    }

    // Event listeners
    
    $('#magnifying-button').on('click', () => {
        // app.startSearch()
        app.showProgrammingView()
    })
    
    app.bluetooth.on('connected', () => {
        app.showProgrammingView()
    })
    
    app.bluetooth.on('canceled', () => {
        app.showMagnifying(false)
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
        $('#connecting-view').hide(400, () => $('#programming-view').show() )
    }
    
    // Methods to dealing with the model

    app.startSearch = () => {
        app.bluetooth.search()
        app.showMagnifying(true)
    }

    app.registerServiceWorker = () => {
        if ('serviceWorker' in navigator) {
            navigator.serviceWorker
                .register('service-worker.js').then( (reg) => {
                    console.log('Service Worker Registered')
                })
        }
    }

    // Start

    // app.registerServiceWorker()
    
})
