//"private" variables

/**
 * A list of connections to be drawn.
 */
var connectionsList = [];

//event handlers

$(document).ready(function () {
  connectAll("svg1");
});

$(window).resize(function () {
  connectAll("svg1");
});

//"private" functions

/**
 *
 * @param {*} svg
 * @param {*} path
 * @param {*} startX
 * @param {*} startY
 * @param {*} endX
 * @param {*} endY
 */
function drawPath(svg, path, startX, startY, endX, endY) {
  // get the path's stroke width
  var stroke = parseFloat(path.attr("stroke-width"));
  // check if the svg is big enough to draw the path, if not, set heigh/width
  if (svg.attr("height") < endY) svg.attr("height", endY);
  if (svg.attr("width") < startX + stroke) svg.attr("width", startX + stroke);
  if (svg.attr("width") < endX + stroke) svg.attr("width", endX + stroke);

  //calculate differences and their halves
  var deltaX = Math.abs(endX - startX);
  var deltaX2 = deltaX / 2;
  var deltaY = Math.abs(endY - startY);
  var deltaY2 = deltaY / 2;

  var min = Math.min(deltaX2, deltaY2) / 2;
  const offsetX = document.getElementById("outer").offsetWidth / 80;
  const offsetY = document.getElementById("outer").offsetHeight / 20;
  if (startX < endX) {
    if (startY == endY) {
      //case 1: straight line from left to right
      path.attr("d", "M" + startX + " " + startY + " L" + endX + " " + endY);
    } else if (startY > endY) {
      //case 2: line bent up, then right
      path.attr(
        "d",
        "M" +
          startX +
          " " +
          startY +
          " l" +
          (deltaX2 - min) +
          " " +
          0 +
          " q" +
          min +
          " " +
          0 +
          " " +
          min +
          " " +
          -min +
          " l" +
          0 +
          " " +
          -(deltaY - 2 * min) +
          " q" +
          0 +
          " " +
          -min +
          " " +
          min +
          " " +
          -min +
          " l" +
          (deltaX2 - min) +
          " " +
          0
      );
    } else if (startY < endY) {
      //case 3: line bent down, then right
      path.attr(
        "d",
        "M" +
          startX +
          " " +
          startY +
          " l" +
          (deltaX2 - min) +
          " " +
          0 +
          " q" +
          min +
          " " +
          0 +
          " " +
          min +
          " " +
          min +
          " l" +
          0 +
          " " +
          (deltaY - 2 * min) +
          " q" +
          0 +
          " " +
          min +
          " " +
          min +
          " " +
          min +
          " l" +
          (deltaX2 - min) +
          " " +
          0
      );
    }
  } else if (startX > endX) {
    if (startY < endY) {
      //case 4: line bent down, then left, then down, then right again

      path.attr(
        "d",
        "M" +
          startX +
          " " +
          startY +
          " l" +
          offsetX +
          " " +
          0 +
          " l" +
          0 +
          " " +
          deltaY2 +
          " l " +
          -(deltaX + offsetX * 2) +
          " " +
          0 +
          "  l" +
          0 +
          " " +
          deltaY2 +
          "  l" +
          offsetX +
          " " +
          0
      );
    } else if (startY > endY) {
      //case 5: line bent up, then left, then up, then right again
      path.attr(
        "d",
        "M" +
          startX +
          " " +
          startY +
          " L" +
          startX +
          " " +
          (startY + deltaY2) +
          " L " +
          endX +
          " " +
          (startY + deltaY2) +
          " L " +
          endX +
          " " +
          endY
      );
    } else if (startY == endY) {
      //case 6: line bent up, then left, then down, then right again
      path.attr(
        "d",
        "M" +
          startX +
          " " +
          startY +
          " l" +
          offsetX +
          " " +
          0 +
          " l" +
          0 +
          " " +
          -offsetY +
          " l " +
          -(deltaX + 2 * offsetX) +
          " " +
          0 +
          " l " +
          0 +
          " " +
          offsetY
      );
    }
  } else {
    //case something went wrong: Draw a straight line ignoring all fancyness
    console.log("ERROR: Unimplemented line path.");
    path.attr("d", "M" + startX + " " + startY + " L" + endX + " " + endY);
  }
}

/**
 *
 * @param {*} svg
 * @param {*} path
 * @param {*} startElem
 * @param {*} endElem

 */
function connectElements(svg, path, startElem, endElem) {
  var svgContainer = $("#svgContainer");

  // get (top, left) corner coordinates of the svg container
  var svgTop = svgContainer.offset().top;
  var svgLeft = svgContainer.offset().left;

  // get (top, left) coordinates for the two elements
  var startCoord = startElem.offset();

  var endCoord = endElem.offset();

  // calculate path's start (x,y)  coords
  // we want the x coordinate to visually result in the element's mid point
  var startX = startCoord.left + startElem.outerWidth() - svgLeft; // x = left offset + 0.5*width - svg's left offset
  var startY = startCoord.top + 0.5 * startElem.outerHeight() - svgTop; // y = top offset + height - svg's top offset

  // calculate path's end (x,y) coords
  var endX = endCoord.left - svgLeft;
  var endY = endCoord.top + 0.5 * endElem.outerHeight() - svgTop;

  // call function for drawing the path
  drawPath(svg, path, startX, startY, endX, endY);
}

/**
 * Draws all the connections according to the {@link connectionsList}.
 * @param {*} parentId
 */
function connectAll(parentId) {
  // reset svg each time
  $("#" + parentId).attr("height", "0");
  $("#" + parentId).attr("width", "0");

  //find out how many paths and connections we have
  const pathCount = $("#" + parentId).children("path").length - 1;
  const connectionsCount = connectionsList.length;

  if (pathCount < connectionsCount) {
    //to few paths
    createPaths(connectionsCount - pathCount, parentId, pathCount);
  } else if (pathCount > connectionsCount) {
    //too many paths
    for (let index = pathCount - 1; index >= connectionsCount; index--) {
      $("#path" + index).remove();
    }
  }

  //draw each connection
  for (let index = 0; index < connectionsList.length; index++) {
    connectElements(
      $("#" + parentId),
      $("#path" + index),
      $("#" + connectionsList[index].from),
      $("#" + connectionsList[index].to)
    );
  }
}

/**
 * Creath a new SVG-Path with fixed style options and appends it to the given parent <svg>
 * @param {string} id The id of the new path. Should be unique. (This however is not enforced)
 * @param {string} parentId The parent of the new Node.
 */
function createPath(id, parentId) {
  const parentElement = document.getElementById(parentId);
  const newPath = document.createElementNS(parentElement.namespaceURI, "path");

  newPath.setAttributeNS(null, "id", id);
  newPath.setAttributeNS(null, "d", "M0 0");
  newPath.setAttributeNS(null, "stroke", "black");
  newPath.setAttributeNS(null, "stroke-width", "2px");
  newPath.setAttributeNS(null, "opacity", 1);
  newPath.setAttributeNS(null, "fill", "none");
  newPath.setAttributeNS(null, "marker-end", "url(#pointer)");
  newPath.setAttributeNS(null, "marker-start", "url(#start)");

  parentElement.appendChild(newPath);
}

/**
 * Create a number of paths with {@link createPath()}
 * @param {int} n The number of paths to create.
 * @param {string} parentId The parent of the new Nodes.
 * @param {int} startingIndex Optional: The starting index.
 */
function createPaths(n, parentId, startingIndex = 0) {
  for (let index = startingIndex; index <= n + startingIndex; index++) {
    createPath("path" + index, parentId);
  }
}

//"public" functions
/**
 * Adds a connection to {@link connectionsList}.
 * @param {*} from The starting point of the Connection.
 * @param {*} to The End Point of the Connection.
 */
function addConnection(from, to) {
  //check list for existing connection
  if (
    !connectionsList.find((element) => {
      return from == element.from && to == element.to;
    })
  ) {
    //if non-existing -> add new
    connectionsList.push({ from: from, to: to });
    connectAll("svg1");
  }
}

/**
 * Deletes all connections from a given id
 * @param {*} from
 */
function deleteConnections(from) {
  connectionsList = connectionsList.filter((element) => from != element.from);
  connectAll("svg1");
}
