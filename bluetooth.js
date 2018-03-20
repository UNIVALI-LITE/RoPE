/* global navigator TextEncoder $ */
let myCharacteristic

function log(text){
    console.log(text);
}

function search(){
    
    let serviceUuid = '0000ffe0-0000-1000-8000-00805f9b34fb'
    let characteristicUuid = '0000ffe1-0000-1000-8000-00805f9b34fb'
    let options = {
        acceptAllDevices: true,
        optionalServices : [ serviceUuid ]
    }
    navigator.bluetooth.requestDevice(options)
    .then(device=>{
        return device.gatt.connect()
    })
    .then(server=>{
        return server.getPrimaryService(serviceUuid);
    })
    .then(service=>{
        return service.getCharacteristic(characteristicUuid)
    })
    .then(characteristic => {
        myCharacteristic = characteristic
        log('> Characteristic UUID:  ' + characteristic.uuid)
        log('> Broadcast:            ' + characteristic.properties.broadcast)
        log('> Read:                 ' + characteristic.properties.read)
        log('> Write w/o response:   ' + characteristic.properties.writeWithoutResponse)
        log('> Write:                ' + characteristic.properties.write)
        log('> Notify:               ' + characteristic.properties.notify)
        log('> Indicate:             ' + characteristic.properties.indicate)
        log('> Signed Write:         ' +
          characteristic.properties.authenticatedSignedWrites)
        log('> Queued Write:         ' + characteristic.properties.reliableWrite)
        log('> Writable Auxiliaries: ' + characteristic.properties.writableAuxiliaries)
    })
    .catch(error => {
        log('Argh! ' + error)
    })
}

function onWriteButtonClick() {
    let encoder = new TextEncoder('utf-8')
    let value = document.querySelector('#description').value;
    alert(value)
    log('Setting Characteristic User Description...')
    myCharacteristic.writeValue(encoder.encode(value))
    .then(_ => {
        log('> Characteristic User Description changed to: ' + value)
    })
    .catch(error => {
        log('Argh! ' + error)
    })
}