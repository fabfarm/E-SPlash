// DEFINITIONS -----------------------------------
// Json payload state (state means it can change)
let jsonDataState = {};
let mainHtmlState = [];
// Endpoint URL constants
const GET_DATA_ENDPOINT = "/getData";
const UPDATE_DATA_ENDPOINT = "/updateData";
// ------------------------------------------------

// Definition of functions -----------------------------
const iterate = function () {
  fetch(GET_DATA_ENDPOINT)
    .then((res) => res.json())
    .then((jsonData) => {
      jsonDataState = jsonData;
      document.getElementById("currentTime").innerText =
        jsonDataState.data.currentTime;
      document.getElementById("temperature").innerText =
        jsonDataState.data.temperature;
      document.getElementById("humidity").innerText =
        jsonDataState.data.humidity;
      document.getElementById("batLevel").innerText =
        jsonDataState.data.batLevel;
    });
};

function refresh() {
  fetch(GET_DATA_ENDPOINT)
    .then((res) => res.json())
    .then((jsonData) => {
      jsonDataState = jsonData;
      updateHtml();
      //   // Build html for relays & their schedules
      //   for (var i = 0; i < jsonDataState.relays.length; i++) {
      //     var relay = jsonDataState.relays[i];
      //     html += `
      //       <div class="shadow d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center flex-column">

      //                         <div>Zone:<b>${relay.name}</b> (pin ${
      //       relay.pin
      //     })</div>
      //                         <br>
      //                         <span class="w3-hide-small">
      //                             <label class="switch">
      //                                 <input id="jsonData.relays[${i}].isEnabled" type="checkbox" ${
      //       relay.isEnabled ? "checked" : ""
      //     }
      //                                     onchange="update(this);"/>
      //                                     <span class="slider"/>
      //                             </label>
      //                                     <!--</span> On (1) / Off (0) = ${
      //                                       relay.isRunning
      //                                     }-->
      //               </div>`;
      //     // Build list of schedules for this relay
      //     for (var j = 0; j < relay.times.length; j++) {
      //       time = relay.times[j];
      //       var times = `
      //                             <div style="width:100px;"></div>
      //                             <label for="time"></label>
      //                             <div class="relaysContainer">
      //                                 <span>
      //                                     Time ${j + 1}
      //                                 </span>

      //                                 <input id="jsonData.relays[${i}].times[${j}].startTime" type="time" style="width:100px;"
      //                                     onChange="update(this);"
      //                                     required value="${time.startTime}" />
      //                                 <input id="jsonData.relays[${i}].times[${j}].duration" type="range" onChange="update(this);"
      //                                     min="0" max="60" value="${
      //                                       time.duration
      //                                     }" step="1" class="slider2">

      //                                 minutes: <span></span>
      //                             </div>`;
      //       html += times;
      //     }
      //   }

      //   const main = document.getElementById("main");
      //   main.innerHTML = html; //this will REBUILD the page per ping (?)

      //   var sliders = document.getElementsByClassName("relaysContainer"); // get every slider on the page related to a relay and add the oninput property
      //   for (let denRelays = 0; denRelays < sliders.length; denRelays++) {
      //     sliders[denRelays].children[3].innerHTML =
      //       sliders[denRelays].children[2].value; // for the initialisation of the value

      //     sliders[denRelays].children[2].oninput = function () {
      //       sliders[denRelays].children[3].innerHTML = this.value; // for the realtime updating of the slider value
      //     };
      //   }
    });
}

function updateHtml() {
  const mainHtml = [];
  mainHtml.push(`<div class="shadow d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center flex-column">
        <h6 class="display-6 main-title"><strong>Scheduling Mode</strong></h6>
            <div class="d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center">
                        <span class="m-2">Manual</span>
                            <div class="form-check form-switch">
                                <input ${
                                  jsonDataState.data.isScheduleMode
                                    ? "checked"
                                    : ""
                                }
                                style="width: 100px; height: 50px;" 
                                onChange="update(this);" 
                                id="jasonData.data.isScheduleMode" class="form-check-input" type="checkbox">
                            </div>
                        <span class="m-2">Automatic</span>
                </div>
        </div>`);

  jsonDataState.relays.forEach((relay, i) => {
    if (!jsonDataState.data.isScheduleMode) {
      mainHtml.push(`
      <div class="shadow d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center flex-column">
            <h6 class="display-6 main-title"><strong>${relay.name} (pin ${
        relay.pin
      })</strong></h6>
                <div class="form-check form-switch">
                          <input
                          style="width: 100px; height: 50px;" 
                          onChange="update(this);" 
                          id="jsonData.relays[${i}].isEnabled" type="checkbox" ${
        relay.isEnabled ? "checked" : ""
      } class="form-check-input">
                </div>
        </div>`);
    } else {
      let times = "";
      for (var j = 0; j < relay.times.length; j++) {
        time = relay.times[j];
        times += `
                                    <div style="width:100px;"></div>
                                    <label for="time"></label>
                                    <div class="relaysContainer">
                                        <span>
                                            Time ${j + 1}
                                        </span>
    
                                        <input id="jsonData.relays[${i}].times[${j}].startTime" type="time" style="width:100px;"
                                            onChange="update(this);"
                                            required value="${
                                              time.startTime
                                            }" />
                                        <input id="jsonData.relays[${i}].times[${j}].duration" type="range" onChange="update(this);"
                                            min="0" max="60" value="${
                                              time.duration
                                            }" step="1" class="slider2">
    
                                        minutes: <span></span>
                                    </div>`;
      }
      mainHtml.push(`
      <div class="shadow d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center flex-column">
            <h6 class="display-6 main-title"><strong>${relay.name} (pin ${relay.pin})</strong></h6>
                ${times}
        </div>`);
    }
  });

  const main = document.getElementById("main");
  main.innerHTML = mainHtml.join(""); //this will REBUILD the page per ping (?)

  var sliders = document.getElementsByClassName("relaysContainer"); // get every slider on the page related to a relay and add the oninput property
  for (let denRelays = 0; denRelays < sliders.length; denRelays++) {
    sliders[denRelays].children[3].innerHTML =
      sliders[denRelays].children[2].value; // for the initialisation of the value

    sliders[denRelays].children[2].oninput = function () {
      sliders[denRelays].children[3].innerHTML = this.value; // for the realtime updating of the slider value
    };
  }
}

function update() {
  const isScheduleMode = jsonDataState.data.isScheduleMode;

  if (!isScheduleMode) {
    for (var i = 0; i < jsonDataState.relays.length; i++) {
      jsonDataState.relays[i].isEnabled = document.getElementById(
        `jsonData.relays[${i}].isEnabled`
      ).checked
        ? 1
        : 0;
    }
  }
  if (isScheduleMode) {
    for (var i = 0; i < jsonDataState.relays.length; i++) {
      for (j = 0; j < jsonDataState.relays[i].times.length; j++) {
        //first part of code parses value to the json
        jsonDataState.relays[i].times[j].duration = parseInt(
          document.getElementById(`jsonData.relays[${i}].times[${j}].duration`)
            .value
        );
        jsonDataState.relays[i].times[j].startTime = document.getElementById(
          `jsonData.relays[${i}].times[${j}].startTime`
        ).value;
        //here I hack my way into separating hours and minutes into a int to the json!
        var time = document.getElementById(
          `jsonData.relays[${i}].times[${j}].startTime`
        ).value;
        var timex = new Date(" Jan 01 2021 " + time);
        var hour = timex.getHours();
        var min = timex.getMinutes();
        jsonDataState.relays[i].times[j].hour = hour;
        jsonDataState.relays[i].times[j].min = min;
      }
    }
  }

  jsonDataState.data.isScheduleMode = !jsonDataState.data.isScheduleMode;

  fetch(UPDATE_DATA_ENDPOINT, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(jsonDataState),
  }).then((json) => updateHtml());
}
// -----------------------------------------------------

// Main ------------------------------------------------
// Iterate first time
iterate();
// Iterate each 10 seconds for data and clock update
setInterval(iterate(), 10000);
// Refresh
refresh();
// -----------------------------------------------------
