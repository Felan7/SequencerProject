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
  function formatNumber(number){
    var returnNumber = Math.abs(number);
    returnNumber = returnNumber.toFixed(2);
    var returnString = returnNumber.toString();

    if (Math.abs(number) < 10) {
        returnString = "0" + returnString;
    }


    if (number >= 0){
        returnString = "+" + returnString;
    }else{
        returnString = "-" + returnString;
    }
    return returnString;
  }

  console.log(formatNumber(2.1));
  console.log(formatNumber(11.6));
  console.log(formatNumber(-2.4));
  console.log(formatNumber(-12.2));

  values = { a: -12.34, b: 4.3, trigger: true, gate: false };
  function createNodeCard(id, values) {
    const newCard = document.createElement("div");

    newCard.id = id;
    newCard.draggable = true;
    newCard.ondragstart = function () {
      drag(event);
    };
    newCard.classList.add("col", "card", "m-1");

    const newCardHeader = document.createElement("header");
    newCardHeader.classList.add("card-header");
    newCardHeader.innerHTML = id;
    const newCardBody = document.createElement("div");
    newCardBody.classList.add("card-body", "p-1");
    newCardBody.style.fontFamily = "monospace";
    //create a table structured set of value displays
    const container = document.createElement("div");
    container.classList.add("container-fluid");

    const row1 = document.createElement("section");
    row1.classList.add("row");

    const col1 = document.createElement("article");
    col1.classList.add("col", "p-0");
    col1.innerHTML = "A:" + formatNumber(values.a);

    row1.appendChild(col1);

    const col2 = document.createElement("article");
    col2.classList.add("col");
    col2.innerHTML = "T:" + translateBoolToIcon(values.trigger);

    row1.appendChild(col2);

    const row2 = document.createElement("section");
    row2.classList.add("row");

    const col3 = document.createElement("article");
    col3.classList.add("col", "p-0");
    col3.innerHTML = "B:" + formatNumber(values.b);

    row2.appendChild(col3);

    const col4 = document.createElement("article");
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

  const colorNames = ["teal", "red", "green", "purple", "orange", "aqua"];
  colorNames.forEach((element) => {
    document
      .getElementById("leRow")
      .appendChild(createNodeCard(element, values));
  });

  const rowCount = 5;
  const columnCount = 5;
  for (let indexRow = 0; indexRow < rowCount; indexRow++) {
    const newRow = document.createElement("div");
    newRow.classList.add("row");
    newRow.classList.add("h-100");
    for (let indexCoulumn = 0; indexCoulumn < columnCount; indexCoulumn++) {
      const newColumn = document.createElement("div");
      newColumn.classList.add("col");
      newColumn.classList.add("border");
      newColumn.style.minHeight = "16px";
      //newColumn.innerHTML = indexRow + " " + indexCoulumn;
      newColumn.ondrop = function () {
        drop(event);
      };
      newColumn.ondragover = function () {
        allowDrop(event);
      };
      newRow.appendChild(newColumn);
    }
    document.getElementById("outer").appendChild(newRow);
  }