/* global navigator TextEncoder TextDecoder $ */

const bluetooth = {
    eventHandlers: {},
    encoder: new TextEncoder('utf-8'),
    decoder: new TextDecoder('utf-8')
}

bluetooth.search = () => {
    let serviceUuid = '0000ffe0-0000-1000-8000-00805f9b34fb'
    let characteristicUuid = '0000ffe1-0000-1000-8000-00805f9b34fb'
    navigator.bluetooth.requestDevice({
        filters : [{
            name: "-['.']- RoPE"
        }],
        optionalServices: [serviceUuid]
    })
        .then(device => {
            bluetooth.device = device
            device.addEventListener('gattserverdisconnected', onDisconnected)
            return device.gatt.connect()
        })
        .then(server => {
            return server.getPrimaryService(serviceUuid)
        })
        .then(service => {
            return service.getCharacteristic(characteristicUuid)
        })
        .then(characteristic => {
            
            characteristic.startNotifications()
            .then(_=>{
                log("Notificações iniciadas")
                
                characteristic.addEventListener('characteristicvaluechanged', characteristicChanged)
                
                bluetooth.characteristic = characteristic
                bluetooth.notify('connected', bluetooth.characteristic)
                
            })
        })
        .catch(error => {
            bluetooth.notify('connection-failed', {})
            log('Argh! ' + error)
        })
}

function characteristicChanged(event) {
    const value = bluetooth.decoder.decode( event.target.value ).trim()
    log(`RoPE diz - ${value}` );
    bluetooth.notify('characteristic-changed', value)
}

function onDisconnected() {
    bluetooth.notify('connection-failed')
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
    bluetooth.getEventHandlers(event).forEach(function(handler){
        handler.call(this,result)
    })
}

bluetooth.setCharacteristic = (value) => {
    log(`Tela diz - ${value}` );
    bluetooth.characteristic.writeValue(bluetooth.encoder.encode(value))
        .then(_ => {
            //log('> Characteristic User Description changed to: ' + value)
        })
        .catch(error => {
            log('Argh! ' + error)
        })
}

const log = function (text) {
    console.log(text)
}
