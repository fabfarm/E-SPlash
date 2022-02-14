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
    });
}

function updateHtml() {
  const mainHtml = [];
  mainHtml.push(`<div class="shadow d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center flex-column">
        <h6 class="display-6 main-title"><strong>Scheduling Mode</strong></h6>
            <div class="d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center">
                        <span class="m-2 display-6">Manual</span>
                            <div class="form-check form-switch">
                                <input ${
                                  jsonDataState.data.isScheduleMode
                                    ? "checked"
                                    : ""
                                }
                                style="width: 100px; height: 50px;" 
                                onChange="update({updateMode: true});" 
                                id="jasonData.data.isScheduleMode" class="form-check-input" type="checkbox">
                            </div>
                        <span class="m-2 display-6">Automatic</span>
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
        const endTimeHours =
          (parseInt(time.startTime.split(":")[0]) * 60 +
            parseInt(time.startTime.split(":")[1]) +
            parseInt(time.duration)) /
          60;
        const endTimeHoursR = Math.floor(endTimeHours);
        const endTimeMinutes = (endTimeHours - endTimeHoursR) * 60;
        const endTimeMinutesR = Math.round(endTimeMinutes);
        const finalEndTime = `${endTimeHoursR}:${endTimeMinutesR}`;
        times += `
                                    <div class="d-flex flex-column align-items-stretch w-100 p-2">
                                      <div class="d-flex align-items-center">
                                        <input class="form-control m-2" 
                                          id="jsonData.relays[${i}].times[${j}].startTime" 
                                          type="time"
                                          onChange="update(this);"
                                          required value="${time.startTime}"
                                        >
                                        <input class="form-control m-2" 
                                          type="time"
                                          id="jsonData.relays[${i}].times[${j}].endTime" 
                                          required value="${finalEndTime}"
                                          onChange="update({duration: 'fromEnd'})"
                                        >
                                      </div>
                                      <div class="d-flex align-items-center mt-2">
                                        <input type="range" class="form-range" id="jsonData.relays[${i}].times[${j}].duration" onChange="update({duration: 'fromRange'})" min="0" max="60" value="${time.duration}" step="1" >
                                      </div>
                                      <div class="d-flex align-items-center w-100 justify-content-center">
                                        <input id="jsonData.relays[${i}].times[${j}].duration-input" onChange="update({duration: 'fromInput'})" class="form-control w-25" value="${time.duration}" type="number"><span class="m-2">min</span>
                                      </div>
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

function changeDuration(e) {
  console.log(e.id);
  time.duration = e.target.value;
}

function update(payload) {
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
        if (payload.duration === "fromRange") {
          jsonDataState.relays[i].times[j].duration = parseInt(
            document.getElementById(
              `jsonData.relays[${i}].times[${j}].duration`
            ).value
          );
        }
        if (payload.duration === "fromEnd") {
          const [endHour, endMinutes] = document
            .getElementById(`jsonData.relays[${i}].times[${j}].endTime`)
            .value.split(":");
          const endHoursInMinutes = endHour * 60;
          const finalEndMinutes = endHoursInMinutes + endMinutes;

          const [startHour, startMinutes] = document
            .getElementById(`jsonData.relays[${i}].times[${j}].startTime`)
            .value.split(":");
          const startHoursInMinutes = startHour * 60;
          const finalStartMinutes = startHoursInMinutes + startMinutes;

          const finalDuration = finalEndMinutes - finalStartMinutes;

          console.log(finalDuration);

          jsonDataState.relays[i].times[j].duration = finalDuration;
        }
        if (payload.duration === "fromInput") {
          jsonDataState.relays[i].times[j].duration = parseInt(
            document.getElementById(
              `jsonData.relays[${i}].times[${j}].duration-input`
            ).value
          );
        }
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

  if (payload.updateMode) {
    jsonDataState.data.isScheduleMode = !jsonDataState.data.isScheduleMode;
  }

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
