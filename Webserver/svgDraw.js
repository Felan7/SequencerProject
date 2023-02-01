//helper functions, it turned out chrome doesn't support Math.sgn() newDiv
function signum(x) {
    return (x < 0) ? -1 : 1;
}
function absolute(x) {
    return (x < 0) ? -x : x;
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
 * @author alojzije https://gist.github.com/alojzije/11127839
 */
function drawPath(svg, path, startX, startY, endX, endY) {
    // get the path's stroke width (if one wanted to be  really precize, one could use half the stroke size)
    var stroke =  parseFloat(path.attr("stroke-width"));
    // check if the svg is big enough to draw the path, if not, set heigh/width
    if (svg.attr("height") <  endY)                 svg.attr("height", endY);
    if (svg.attr("width" ) < (startX + stroke) )    svg.attr("width", (startX + stroke));
    if (svg.attr("width" ) < (endX   + stroke) )    svg.attr("width", (endX   + stroke));
    
    var deltaX = (endX - startX) * 0.15;
    var deltaY = (endY - startY) * 0.15;
    // for further calculations which ever is the shortest distance
    var delta  =  deltaY < absolute(deltaX) ? deltaY : absolute(deltaX);

    // set sweep-flag (counter/clock-wise)
    // if start element is closer to the left edge,
    // draw the first arc counter-clockwise, and the second one clock-wise
    var arc1 = 0; var arc2 = 1;
    if (startX > endX) {
        arc1 = 1;
        arc2 = 0;
    }
    // draw tha pipe-like path
    // 1. move a bit down, 2. arch,  3. move a bit to the right, 4.arch, 5. move down to the end 
    path.attr("d",  "M"  + startX + " " + startY +
                    " V" + (startY + delta) +
                    " A" + delta + " " +  delta + " 0 0 " + arc1 + " " + (startX + delta*signum(deltaX)) + " " + (startY + 2*delta) +
                    " H" + (endX - delta*signum(deltaX)) + 
                    " A" + delta + " " +  delta + " 0 0 " + arc2 + " " + endX + " " + (startY + 3*delta) +
                    " V" + endY );
}

/**
 * 
 * @param {*} svg 
 * @param {*} path 
 * @param {*} startElem 
 * @param {*} endElem 
 * @author alojzije https://gist.github.com/alojzije/11127839
 */
function connectElements(svg, path, startElem, endElem) {
    var svgContainer= $("#svgContainer");

    // if first element is lower than the second, swap!
    if(startElem.offset().top > endElem.offset().top){
        var temp = startElem;
        startElem = endElem;
        endElem = temp;
    }

    // get (top, left) corner coordinates of the svg container   
    var svgTop  = svgContainer.offset().top;
    var svgLeft = svgContainer.offset().left;

    // get (top, left) coordinates for the two elements
    var startCoord = startElem.offset();
    var endCoord   = endElem.offset();

    // calculate path's start (x,y)  coords
    // we want the x coordinate to visually result in the element's mid point
    var startX = startCoord.left + 0.5*startElem.outerWidth() - svgLeft;    // x = left offset + 0.5*width - svg's left offset
    var startY = startCoord.top  + startElem.outerHeight() - svgTop;        // y = top offset + height - svg's top offset

        // calculate path's end (x,y) coords
    var endX = endCoord.left + 0.5*endElem.outerWidth() - svgLeft;
    var endY = endCoord.top  - svgTop;

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
    const newPath = document.createElementNS(parentElement.namespaceURI,"path");
    
    newPath.setAttributeNS(null, "id", id);  
    newPath.setAttributeNS(null, "d", "M0 0");  
    newPath.setAttributeNS(null, "stroke", "black"); 
    newPath.setAttributeNS(null, "stroke-width", "3px");  
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
var connectionsList = [
    {"from": "teal", "to": "orange"},
    {"from": "red", "to": "green"},
    {"from": "teal", "to": "aqua"},
    {"from": "red", "to": "aqua"},
    {"from": "red", "to": "aqua"},
    {"from": "purple", "to": "teal"},
    {"from": "orange", "to": "green"},
];

/**
 * Adds a connection to {@link connectionsList}.
 * @param {*} from The starting point of the Connection.
 * @param {*} to The End Point of the Connection.
 */
function addConection(from, to) {
    connectionsList.push({"from": from, "to": to});
    console.log(connectionsList);
}

//TO_DO: Refactor rename

/**
 * Draws all the connections according to the {@link connectionsList}.
 * @param {*} parentId 
 */
function connectAll(parentId) {

    // reset svg each time 
    $("#" + parentId).attr("height", "0");
    $("#" + parentId).attr("width", "0");

    //find out how many paths and connections we have
    const pathCount = document.getElementById(parentId).childElementCount;
    const connectionsCount = connectionsList.length

    if (pathCount < connectionsCount) {
        //to few paths
        createPaths(connectionsCount - pathCount, parentId, pathCount)
        
    } else if (pathCount > connectionsCount) {
        //too many paths
        //TO_DO: Destroy unuesd paths
    }

    //draw each connection
    for (let index = 0; index < connectionsList.length; index++) {
        connectElements($("#" + parentId), $("#path" + index), $("#" + connectionsList[index].from), $("#" + connectionsList[index].to))
        
    }

}

//add event handlers to the ready and resize events 
$(document).ready(function() {
    connectAll("svg1");
});

$(window).resize(function () {
    connectAll("svg1");
});