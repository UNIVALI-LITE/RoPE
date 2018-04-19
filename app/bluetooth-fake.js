const bluetooth = {
    eventHandlers: {}
}

// Main method

bluetooth.search = () => {
    const time = Math.ceil(Math.random() * 2000 + 2000)
    setTimeout(() => {
        const message = time % 2 === 0 ? 'connected' : 'connection-failed'
        bluetooth.notify(message)
        let i = 0
        if( message === 'connected' ){
            setInterval(()=>{
                bluetooth.notify( i++ % 2 == 0 ? 'connection-failed' : 'connected' )
            }, time)
        }
    }, time)
}

bluetooth.on = (event, handler) => {
    bluetooth.getEventHandlers(event).push(handler)
}

bluetooth.getEventHandlers = (event) => {
    if (!bluetooth.eventHandlers[event])
        bluetooth.eventHandlers[event] = []
    return bluetooth.eventHandlers[event]
}

bluetooth.notify = (event, result) => {
    bluetooth.getEventHandlers(event).forEach(handler => handler.call(result))
}

