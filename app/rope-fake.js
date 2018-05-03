const rope = {
    actions: []
}
rope.buttons = {
    START: 'i',
    ADVANCE: 'a',
    GO_BACK: 'b',
    TURN_LEFT: 'l',
    TURN_RIGHT: 'r',
}
rope.states = {
    STOPPED: {
        handleClick: (button) => {
            if (button === rope.buttons.START) {
                rope.executeActions()
            } else {
                rope.actions.push(button)
                let actionsString = ''
                rope.actions.forEach((a) => actionsString += a)
                rope.onChangeHandler.call(this, 'updatedCommands:' + actionsString)
            }
        }
    },
    EXECUTING: {
        handleClick: (button) => {
            // if (button === rope.buttons.START) {
            //     rope.actions = []
            //     rope.stop()
            // }
        }
    }
}

rope.onChange = (handler) => {
    rope.onChangeHandler = handler
}

rope.startCommunication = () => {
    rope.state = rope.states.STOPPED
    // simulates rope behaviour
    // setInterval(() => {
    //     const index = Math.round(Math.random() * 100) % 4
    //     const key = Object.keys(rope.buttons)[index]
    //     const button = rope.buttons[key]
    //     rope.state.handleClick(button)
    // }, 5000)
}

rope.nothingToExecute = () => {
    return rope.actions.length === 0 ||
        rope.state === rope.states.STOPPED
}

setInterval(() => {
    if (rope.state == rope.states.EXECUTING) {
        if (rope.nothingToExecute()) {
            return rope.stop()
        }
        const action = rope.actions.shift()
        rope.actionIndex++
        rope.onChangeHandler.call(this, `executed:${action}:${rope.actionIndex}`)
    }
}, 2000)

rope.executeActions = () => {
    rope.onChangeHandler.call(this, 'started')
    rope.state = rope.states.EXECUTING
    rope.actionIndex = -1
}

rope.stop = () => {
    rope.state = rope.states.STOPPED
    rope.onChangeHandler.call(this, 'stopped')
}

rope.write = (string) => {
    if (string.length == 1 && string.indexOf('s') != -1) {
        if (rope.actions.length) {
            rope.executeActions()
        }
    } else {
        rope.actions = string.split("")
    }
}

rope.state = rope.states.STOPPED