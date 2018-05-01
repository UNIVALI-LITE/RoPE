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
    }, 2000)
}

rope.executeActions = () => {
    rope.state = rope.states.EXECUTING
    const millins = 2000
    const executing = setInterval(() => {
        if (!rope.actions) return
        const action = rope.actions.shift()
        rope.onChangeHandler.call(this, 'executed:' + action)
        if (!rope.actions.length) {
            rope.stop()
        }
    }, millins)

    setInterval(() => {
        if (!rope.actions.length) {
            clearInterval(executing)
        }
    }, millins)
}

rope.stop = function () {
    rope.state = rope.states.STOPPED
    rope.onChangeHandler.call(this, 'stopped')
}

rope.state = rope.states.STOPPED