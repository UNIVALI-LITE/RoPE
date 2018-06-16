const bluetooth = {
    eventHandlers: {},
    characteristic: ''
}

bluetooth.search = () => {
    const time = 0;
    const message = time % 2 === 0 ? 'connected' : 'connection-failed'
    bluetooth.notify(message)
    let i = 0
    if (message === 'connected') {
        rope.onChange((instructions) => {
            bluetooth.notify('characteristic-changed', instructions)
        })
        rope.startCommunication()
    }
}

bluetooth.setCharacteristic = (characteristic) => {
    rope.write(characteristic)
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
    bluetooth.getEventHandlers(event).forEach(handler => handler.call(this, result))
}

