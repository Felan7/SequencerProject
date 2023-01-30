
function linkSliderToNumberInput(sliderId, numberId) {

    var slider = document.getElementById(sliderId);
    var output = document.getElementById(numberId);

    output.value = slider.value;

    // Update the current slider value (each time you drag the slider handle)
    slider.oninput = function() {
    output.value = this.value;
    }

    output.oninput = function() {
        slider.value = this.value;
    }

    slider.ondblclick = function(){
        slider.value = 0;
        output.value = 0;
    }
}

linkSliderToNumberInput("slider-value-primary", "number-value-primary")
linkSliderToNumberInput("slider-value-secondary", "number-value-secondary")

