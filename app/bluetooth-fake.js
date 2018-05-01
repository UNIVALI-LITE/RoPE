const bluetooth = {
    eventHandlers: {},
    characteristic: ''
}

bluetooth.search = () => {
    const time = 0;// Math.ceil(Math.random() * 2000 + 2000)
    //setTimeout(() => {
        const message = time % 2 === 0 ? 'connected' : 'connection-failed'
        bluetooth.notify(message)
        let i = 0
        if (message === 'connected') {
            // setInterval(() => {

            //     bluetooth.notify(i++ % 2 == 0 ? 'connection-failed' : 'connected')
            // }, time)
            rope.onChange((instructions) => {
                bluetooth.notify('characteristic-changed', instructions)
            })
            rope.startCommunication()
        }
    //}, 5000)
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

