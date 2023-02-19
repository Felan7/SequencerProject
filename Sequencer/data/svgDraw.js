//helper functions, it turned out chrome doesn't support Math.sgn() newDiv
function signum(x) {
  return x < 0 ? -1 : 1;
}
function absolute(x) {
  return x < 0 ? -x : x;
}

/**
 *
 * @param {*} svg
 * @param {*} path
 * @param {*} startX
 * @param {*} startY
 * @param {*} endX
 * @param {*} endY
 *
 */
function drawPath(svg, path, startX, startY, endX, endY) {
  // get the path's stroke width (if one wanted to be  really precise, one could use half the stroke size)
  var stroke = parseFloat(path.attr("stroke-width"));
  // check if the svg is big enough to draw the path, if not, set heigh/width
  if (svg.attr("height") < endY) svg.attr("height", endY);
  if (svg.attr("width") < startX + stroke) svg.attr("width", startX + stroke);
  if (svg.attr("width") < endX + stroke) svg.attr("width", endX + stroke);

  var deltaX = Math.abs(endX - startX);
  var deltaX2 = deltaX / 2;
  var deltaY = Math.abs(endY - startY);
  var deltaY2 = deltaY / 2;
  // for further calculations which ever is the shortest distance
  var delta = deltaY < absolute(deltaX) ? deltaY : absolute(deltaX);

  // set sweep-flag (counter/clock-wise)
  // if start element is closer to the left edge,
  // draw the first arc counter-clockwise, and the second one clock-wise
  var arc1 = 0;
  var arc2 = 1;
  if (startX > endX) {
    arc1 = 1;
    arc2 = 0;
  }
  var min = Math.min(deltaX2, deltaY2) / 2;
  const offsetX = document.getElementById("outer").offsetWidth / 80;
  const offsetY = document.getElementById("outer").offsetHeight / 20;
  if (startX < endX) {
    if (startY == endY) {
      //case 1: straigth line from left to right
      path.attr("d", "M" + startX + " " + startY + " H" + endX + " " + endY);
    } else if (startY > endY) {
      //case 2: line bent up, then right

      // find shortest half line

      //   console.log(
      //     "sx=" +
      //       startX +
      //       " sy=" +
      //       startY +
      //       " ex=" +
      //       endX +
      //       " ey=" +
      //       endY +
      //       " dx=" +
      //       deltaX +
      //       " dy=" +
      //       deltaY +
      //       " min=" +
      //       min
      //   );
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
  var startX = startCoord.left + startElem.outerWidth() + 4; // x = left offset + 0.5*width - svg's left offset
  var startY = startCoord.top + 0.5 * startElem.outerHeight() - svgTop; // y = top offset + height - svg's top offset

  // calculate path's end (x,y) coords
  var endX = endCoord.left - svgLeft;
  var endY = endCoord.top + 0.5 * endElem.outerHeight() - svgTop;

  // call function for drawing the path
  drawPath(svg, path, startX, startY, endX, endY);
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

/**
 * A list of connections to be drawn.
 */
var connectionsList = [];

/**
 * Adds a connection to {@link connectionsList}.
 * @param {*} from The starting point of the Connection.
 * @param {*} to The End Point of the Connection.
 */
function addConection(from, to) {
  connectionsList.push({ from: from, to: to });
  console.log(connectionsList);
  connectAll("svg1");
}

//TO_DO: Refactor rename

/**
 * Draws all the connections according to the {@link connectionsList}.
 * @param {*} parentId
 */
function connectAll(parentId) {
  // reset svg each time
  //   $("#" + parentId).attr("height", "0");
  //   $("#" + parentId).attr("width", "0");

  //find out how many paths and connections we have
  const pathCount = document.getElementById(parentId).childElementCount - 1; //TODO: take only <path> children into account
  const connectionsCount = connectionsList.length;

  if (pathCount < connectionsCount) {
    //to few paths
    createPaths(connectionsCount - pathCount, parentId, pathCount);
  } else if (pathCount > connectionsCount) {
    //too many paths
    //TO_DO: Destroy unuesd paths
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

//add event handlers to the ready and resize events
$(document).ready(function () {
  connectAll("svg1");
});

$(window).resize(function () {
  connectAll("svg1");
});