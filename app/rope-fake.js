const rope = {
    actions: [],
    actionIndex: -1
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
                rope.onChangeHandler.call(this, 'updated_commands:' + actionsString)
            }
        },
        handleWrite: (string) => {
            if (string === 's') {
                rope.executeActions()
            } else if (string.indexOf('d:') != -1) {
                rope.debugging = string.split(':')[1] == 1
            } else {
                rope.actions = string.split("")
            }
        }
    },
    EXECUTING: {
        handleClick: (button) => {
            // if (button === rope.buttons.START) {
            //     rope.actions = []
            //     rope.stop()
            // }
        },
        handleWrite: (string) => {
            if (string === 's') {
                rope.actions = []
                rope.stop()
            } else if (string.indexOf('d:') != -1) {
                rope.debugging = string.split(':')[1] == 1
            } else if (string.indexOf('n') != -1) {
                rope.mustExecuteNextAction = true
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
    // setInterval(() => {
    //     const index = Math.round(Math.random() * 100) % 4
    //     const key = Object.keys(rope.buttons)[index]
    //     const button = rope.buttons[key]
    //     rope.state.handleClick(button)
    // }, 5000)
    const actionString = 'aralalararabbalararra'
    rope.actions = actionString.split('')
    rope.onChangeHandler.call(this, 'updated_commands:'+actionString)
}

rope.nothingToExecute = () => {
    return rope.actions.length === 0 ||
        rope.state === rope.states.STOPPED
}

rope.executeNextAction = () => {
    if (rope.nothingToExecute()) {
        return rope.stop()
    }
    const action = rope.actions.shift()
    rope.actionIndex++
    rope.onChangeHandler.call(this, `started_action:${action}:${rope.actionIndex}`)
    rope.previousAction = action
}

setInterval(() => {
    if (rope.state == rope.states.EXECUTING) {

        if( rope.previousAction ) {
            rope.onChangeHandler.call(this, `finished_action:${rope.previousAction}:${rope.actionIndex - 1}`)
            rope.previousAction = undefined
        }

        if (!rope.debugging) {
            rope.executeNextAction()
        } else if (rope.debugging && rope.mustExecuteNextAction) {
            rope.executeNextAction()
            rope.mustExecuteNextAction = false
        } else if (rope.nothingToExecute()){
            rope.stop()
        }
    }
}, 2000)

rope.executeActions = () => {
    if (!rope.actions.length) return
    rope.onChangeHandler.call(this, 'started')
    rope.state = rope.states.EXECUTING
    rope.actionIndex = -1
}

rope.stop = () => {
    rope.state = rope.states.STOPPED
    rope.onChangeHandler.call(this, 'stopped')
    rope.actionIndex = -1
}

rope.write = (string) => {
    rope.state.handleWrite(string)
}

rope.state = rope.states.STOPPED