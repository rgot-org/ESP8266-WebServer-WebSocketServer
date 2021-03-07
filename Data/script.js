var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var envoi = {};
window.addEventListener('load', onLoad);
function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage; // <-- add this line
}
function onOpen(event) {
    console.log('Connection opened');
    toggle();
}
function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
    let json = JSON.parse(event.data);
    if (json.key == "led") {
        var state = "OFF";
        if (json.value == "1") {
            state = "ON";
        }
        document.getElementById('state').innerHTML = state;
    }
    if (json.key == "somme") {

        document.getElementById('somme').innerHTML = json.value;
    } 
}
function onLoad(event) {
    initWebSocket();
    initButton();   
}
function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
    document.getElementById('button2').addEventListener('click', somme);
}
function toggle() {
    envoi.operation = "toggle";
    websocket.send(JSON.stringify(envoi));
}
function somme() {
    envoi.operation = "somme";
    envoi.nb1 = document.getElementById("nb1").value;
    envoi.nb2 = document.getElementById("nb2").value;
    websocket.send(JSON.stringify(envoi));
}