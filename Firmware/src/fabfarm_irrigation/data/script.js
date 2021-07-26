// keep global json object - we'll update this on element change
var jsonData;
// gets called anytime anything changes
function update(obj) {
    jsonData.data.isScheduleMode = document.getElementById("jasonData.data.isScheduleMode").checked ? 1 : 0;
    console.log(document.getElementById("jasonData.data.isScheduleMode"));
    for(var i = 0; i < jsonData.relays.length; i++) { 
        jsonData.relays[i].isEnabled = document.getElementById(`jsonData.relays[${i}].isEnabled`).checked ? 1 : 0;
        for(j = 0; j < jsonData.relays[i].times.length; j++) {
            //first part of code parses value to the json    // second part reads from?????????
            jsonData.relays[i].times[j].duration  = parseInt(document.getElementById(`jsonData.relays[${i}].times[${j}].duration`).value);
            jsonData.relays[i].times[j].startTime = document.getElementById(`jsonData.relays[${i}].times[${j}].startTime`).value;
            //here I hack my way into separating hours and minutes into a int to the jason!
            var time = document.getElementById(`jsonData.relays[${i}].times[${j}].startTime`).value;
            var timex = (new Date(" Jan 01 2020 " + time));
            //console.log(timex);
            var hour = timex.getHours();
            var min = timex.getMinutes();
            // console.log(hour)
            // console.log(min)
            jsonData.relays[i].times[j].hour = hour;
            jsonData.relays[i].times[j].min = min;
        }
    }
    var json = JSON.stringify(jsonData);
    var xmlhttp = new XMLHttpRequest();   
    xmlhttp.open("POST", "/updateData");
    xmlhttp.setRequestHeader("Content-Type", "application/json;");
    xmlhttp.send(json);
}
 
// send a json every 4 secs 

function setTimer(func) {
    setTimeout(function() {return func()}, 4000);
}

/* Currently we load this once from the server & it's working well.
Once we start pinging the server for updates, we'll run into the issue where this keep running over and over
again and will keep adding elements to the page ! We need to fix this so we build the page based on the json
and either
a) rebuild it per ping (?)
b) only update the relevant elements if it's already built

Convention: the Id of html elements is the path to its value in the json. This lets us pull the values back out
            on update
*/

setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var response = this.response;
            jsonData = JSON.parse(this.responseText);
            document.getElementById("currentTime").innerText = jsonData.data.currentTime;
            document.getElementById("temperature").innerText = jsonData.data.temperature;
            document.getElementById("humidity").innerText = jsonData.data.humidity;
            document.getElementById("batLevel").innerText = jsonData.data.batLevel;
            var html = `Manual 
                    <label class="switch">
                        <input type="checkbox" ${jsonData.data.isScheduleMode ? "checked" : ""} 
                            onChange="update(this);" id="jasonData.data.isScheduleMode"/>
                        <span class="slider"></span> 
                    </label> Automatic`;
            // Build html for relays & their schedulesk
            for(var i = 0; i  < jsonData.relays.length; i++) {
                var relay = jsonData.relays[i];
                html += `
                    <hr/>
                    <div>Zone:<b>${relay.name}</b> (pin ${relay.pin})</div>
                    <br>
                    <span class="w3-hide-small">
                        <label class="switch">
                            <input id="jsonData.relays[${i}].isEnabled" type="checkbox" ${relay.isEnabled ? "checked": ""} 
                                onchange="update(this);"/>
                                <span class="slider"/>
                        </label>
                                <!--</span> On (1) / Off (0) = ${relay.isRunning}-->`;
                // Build list of schedules for this relay
                for(var j = 0; j < relay.times.length; j++) {
                    time = relay.times[j];
                    var times = `
                        <div style="width:100px;">
                        </div> 
                        <label for="time">
                        </label>
                        <div>
                            <span>Time ${j+1}
                            </span>
                            <input id="jsonData.relays[${i}].times[${j}].startTime" type="time" style="width:100px;" 
                                onChange="update(this);" 
                                required value="${time.startTime}" />
                            <input id="jsonData.relays[${i}].times[${j}].duration" type="range" onChange="update(this);" 
                                min="0" max="60" value="${time.duration}" step="1" class="slider2">
                                Value: <span id="value">
                                </span> ${time.duration}
                        </div>`;
                    html += times;
                    }
            }                                   
            var main = document.getElementById("main");
            main.innerHTML = html; //this will REBUILD the page per ping (?)
        }
    }; 
    xhttp.open("GET", "/getData", true);
    xhttp.timeout = 2000;
    xhttp.ontimeout = function(e) {
    };
    xhttp.send();
}, 1000);
