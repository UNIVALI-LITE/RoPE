const bluetooth = {
    eventHandlers: {}
}

// Main method

bluetooth.search = () => {
    const time = Math.ceil(Math.random() * 5000 + 5000)
    setTimeout(() => {
        const message = time % 2 === 0 ? 'connected' : 'connection-failed'
        bluetooth.notify(message)
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

