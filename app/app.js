/*global $ global bluetooth navigator*/
$(function() {

    let app = {
        bluetooth: bluetooth
    }

    // Event listeners
    
    $('#magnifying-button').on('click', () => {
        app.startSearch()
    })


    // Methods to update ui

    app.showMagnifying = (show) => {
        if (show) {
            $('#magnifying').show()
        }
        else {
            $('#magnifying').hide()
        }
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
                    console.log( reg )
                    console.log('Service Worker Registered')
                })
        }
    }

    // Start

    app.registerServiceWorker()

})
