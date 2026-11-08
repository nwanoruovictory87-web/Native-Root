function BRIGE_PUSH(str) {
  // push object to c
  const data = JSON.stringify(str);
  //backend bride that calls cc
  window.webkit.messageHandlers.myBackend.postMessage(data);
}
function BRIGE_LISTEN(str) {
  console.log(str);
}
