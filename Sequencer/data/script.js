const nodes = [];
var nextFreeId = 1;

// $("#side-menu").hide();

function linkSliderToNumberInput(sliderId, numberId) {
  var slider = document.getElementById(sliderId);
  var output = document.getElementById(numberId);

  output.value = slider.value;

  // Update the current slider value (each time you drag the slider handle)
  slider.oninput = function () {
    output.value = this.value;
  };

  output.oninput = function () {
    slider.value = this.value;
  };

  slider.ondblclick = function () {
    slider.value = 0;
    output.value = 0;
  };
}

linkSliderToNumberInput("slider-value-primary", "number-value-primary");
linkSliderToNumberInput("slider-value-secondary", "number-value-secondary");

function drop(ev) {
  ev.preventDefault();
  var data = ev.dataTransfer.getData("text");
  ev.target.appendChild(document.getElementById(data));
  connectAll("svg1");
}
function allowDrop(ev) {
  ev.preventDefault();
}

function drag(ev) {
  ev.dataTransfer.setData("text", ev.target.id);
}

function translateBoolToIcon(boolean) {
  if (boolean) {
    return "&#x2611;";
  } else {
    return "&#x2612;";
  }
}

/**
 *
 * @param {Number} number
 * @returns
 */
function formatNumber(number) {
  var returnNumber = Math.abs(number);
  returnNumber = returnNumber.toFixed(2);
  var returnString = returnNumber.toString();

  if (Math.abs(number) < 10) {
    returnString = "0" + returnString;
  }

  if (number >= 0) {
    returnString = "+" + returnString;
  } else {
    returnString = "-" + returnString;
  }
  return returnString;
}

function randomInt(upper) {
  return Math.floor(Math.random() * upper);
}
function randomColor() {
  return (
    "rgb(" + randomInt(255) + "," + randomInt(255) + "," + randomInt(255) + ")"
  );
}

function createNodeCard(
  id,
  values = { a: 0, b: 0, trigger: false, gate: false }
) {
  var color = randomColor();

  const newCard = document.createElement("div");

  newCard.id = id;
  newCard.draggable = true;
  newCard.ondragstart = function (event) {
    if (event.target.parentElement.id != "leRow") {
      event.target.parentElement.classList.add("border");
    }
    drag(event);
  };
  newCard.onclick = function () {
    $(".node-card").removeClass("border-primary");
    $(".node-card").addClass("border-dark");
    newCard.classList.remove("border-dark");
    newCard.classList.add("border-primary");
    switchEditWindowNode(newCard.id);
  };
  newCard.ondblclick = function () {
    showMenu();
  };
  newCard.classList.add(
    "node-card",
    "col",
    "card",
    "m-1",
    "border",
    "border-dark",
    "border-2"
  );

  const newCardHeader = document.createElement("header");
  newCardHeader.classList.add("card-header", "p-1");
  newCardHeader.innerHTML =
    "<span class='badge rounded-pill' style='background-color: " +
    color +
    "'>&nbsp</span> " +
    id;

  const newCardBody = document.createElement("div");
  newCardBody.classList.add("card-body", "p-1");
  newCardBody.style.fontFamily = "monospace";
  //create a table structured set of value displays
  const container = document.createElement("div");
  container.classList.add("container-fluid");

  const row1 = document.createElement("section");
  row1.classList.add("row");

  const col1 = document.createElement("article");
  col1.id = id + "-value-a";
  col1.classList.add("col", "p-0");
  col1.innerHTML = "A:" + formatNumber(values.a);

  row1.appendChild(col1);

  const col2 = document.createElement("article");
  col2.id = id + "-value-trigger";
  col2.classList.add("col");
  col2.innerHTML = "T:" + translateBoolToIcon(values.trigger);

  row1.appendChild(col2);

  const row2 = document.createElement("section");
  row2.classList.add("row");

  const col3 = document.createElement("article");
  col3.id = id + "-value-b";
  col3.classList.add("col", "p-0");
  col3.innerHTML = "B:" + formatNumber(values.b);

  row2.appendChild(col3);

  const col4 = document.createElement("article");
  col4.id = id + "-value-gate";
  col4.classList.add("col");
  col4.innerHTML = "G:" + translateBoolToIcon(values.gate);

  row2.appendChild(col4);

  container.appendChild(row1);
  container.appendChild(row2);

  newCardBody.appendChild(container);

  newCard.appendChild(newCardHeader);
  newCard.appendChild(newCardBody);

  return newCard;
}

const rowCount = innerHeight / 100;
const columnCount = innerWidth / 200;
for (let indexRow = 0; indexRow < rowCount; indexRow++) {
  const newRow = document.createElement("div");
  newRow.classList.add("row");
  newRow.classList.add("h-100");
  for (let indexCoulumn = 0; indexCoulumn < columnCount; indexCoulumn++) {
    const newColumn = document.createElement("div");
    newColumn.classList.add("col", "border", "p-0", "m-2", "rounded");

    newColumn.style.minHeight = "4em";
    //newColumn.innerHTML = indexRow + " " + indexCoulumn;
    newColumn.ondrop = function () {
      event.target.classList.remove("border");
      drop(event);
    };
    newColumn.ondragover = function () {
      allowDrop(event);
    };
    newRow.appendChild(newColumn);
  }
  document.getElementById("outer").appendChild(newRow);
}

function switchEditWindowNode(id) {
  document.getElementById("uid").value = id;
  readDataFromArray(id);
}

function readDataFromArray(id) {
  var node = nodes.find((element) => {
    return element.nodeID == id;
  });

  if (node != undefined) {
    setEditor(node);
  } else {
    $("#uid").val(id);
    $("#number-value-primary").val(0);
    $("#number-value-secondary").val(0);
    $("#slider-value-primary").val(0);
    $("#slider-value-secondary").val(0);
    $("#value-gate").val(false);
    $("#value-trigger").val(false);
  }
}

function setEditor(node) {
  $("#uid").val(node.nodeID);
  $("#number-value-primary").val(node.valueA);
  $("#number-value-secondary").val(node.valueB);
  $("#slider-value-primary").val(node.valueA);
  $("#slider-value-secondary").val(node.valueB);
  $("#value-gate").prop("checked", node.valueGate);
  $("#value-trigger").prop("checked", node.valueTrigger);
}

function isNodeDataValid(node) {
  var nodeId = nodes.find((x) => x.nodeID == node.nextNodes[0]);
  if (nodeId == undefined) {
    return false;
  }

  return true;
}

function dataSubmit() {
  var node = {
    nodeID: document.getElementById("uid").value,
    valueA: document.getElementById("number-value-primary").value,
    valueB: document.getElementById("number-value-secondary").value,
    valueGate: document.getElementById("value-gate").checked,
    valueTrigger: document.getElementById("value-trigger").checked,
    nextNodes: [document.getElementById("next-0").value, document.getElementById("next-1").value],
  };
  if (isNodeDataValid(node)) {
    var nodeJSON = JSON.stringify(node);
    console.log(nodeJSON);

    //update node
    updateNodeCard(document.getElementById("uid").value, node);
    addConection(node.nodeID, node.nextNodes[0]);

    //search array for node
    for (let index = 0; index < nodes.length; index++) {
      if (nodes[index].nodeID == node.nodeID) {
        var nodeInArray = nodes[index];
        //found the node, please update
        nodeInArray.valueA = node.valueA;
        nodeInArray.valueB = node.valueB;
        nodeInArray.valueGate = node.valueGate;
        nodeInArray.valueTrigger = node.valueTrigger;
        nodeInArray.nextNodes = node.nextNodes;
        //we're done here -> quitting time
        return 0;
      }
    }
    //not found -> New array entry
    nodes.push(node);
  } else {
    throw "ERROR: Invalid Node data.";
  }
}

/**
 * Updates the Node Cards value
 * @param {*} id the id of the Node Card.
 * @param {*} nodeData The JavaScriptObject containing the data.
 */
function updateNodeCard(id, nodeData) {
  $("#" + id + "-value-a").html("A:" + formatNumber(nodeData.valueA));
  $("#" + id + "-value-b").html("B:" + formatNumber(nodeData.valueB));
  $("#" + id + "-value-trigger").html(
    "T:" + translateBoolToIcon(nodeData.valueTrigger)
  );
  $("#" + id + "-value-gate").html(
    "G:" + translateBoolToIcon(nodeData.valueGate)
  );
}

function writeToDevice() {
  console.log(JSON.stringify(nodes));
  $.post("/post", JSON.stringify(nodes));
}
function hideMenu() {
  $("#side-menu").hide();
}

function showMenu() {
  $("#side-menu").show();
}

function toggleMenu() {
  if ($("#side-menu").is(":visible")) {
    $("#side-menu").hide();
  } else {
    $("#side-menu").show();
  }
}

function randomInRange(min, max) {
  return Math.random() < 0.5
    ? (1 - Math.random()) * (max - min) + min
    : Math.random() * (max - min) + min;
}

function randomBoolean() {
  return Math.random() > 0.5;
}

function randomiseValues() {
  var node = {
    nodeID: document.getElementById("uid").value,
    valueA: randomInRange(-12, 12),
    valueB: randomInRange(-12, 12),
    valueGate: randomBoolean(),
    valueTrigger: randomBoolean(),
    nextNode: [],
  };

  setEditor(node);
}

function createNewNode() {
  var newNode = {
    nodeID: nextFreeId,
    valueA: 0,
    valueB: 0,
    valueGate: 0,
    valueTrigger: 0,
    nextNode: [],
  };
  nextFreeId++;
  nodes.push(newNode);
  $("#leRow").append(createNodeCard(newNode.nodeID));
  $("#next-0").append("<option>" + newNode.nodeID + "</option>");
  $("#next-1").append("<option>" + newNode.nodeID + "</option>");
}
