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
                rope.onChangeHandler.call(this, 'started')
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
            if (button === rope.buttons.START) {
                rope.actions = []
                rope.stop()
            }
        }
    }
}

rope.onChange = (handler) => {
    rope.onChangeHandler = handler
}

rope.startCommunication = () => {
    rope.state = rope.states.STOPPED
    // simulates rope behaviour
    setInterval(() => {
        const index = Math.round(Math.random() * 100) % 4
        const key = Object.keys(rope.buttons)[index]
        const button = rope.buttons[key]
        rope.state.handleClick(button)
    }, 5000)
}

rope.nothingToExecute = () => {
    return rope.actions.length === 0 || 
           rope.state === rope.states.STOPPED
}

rope.executeActions = () => {
    rope.state = rope.states.EXECUTING
    rope.actionIndex = -1
    const millins = 2000
    const executing = setInterval(() => {
        if (rope.nothingToExecute()) return
        const action = rope.actions.shift()
        rope.actionIndex++
        rope.onChangeHandler.call(this, `executed:${action}:${rope.actionIndex}`)
        if (!rope.actions.length) {
            rope.stop()
        }
    }, millins)

    setInterval(() => {
        if (rope.nothingToExecute()) {
            clearInterval(executing)
        }
    }, millins)
}

rope.stop = function () {
    rope.state = rope.states.STOPPED
    rope.onChangeHandler.call(this, 'stopped')
}

rope.state = rope.states.STOPPED