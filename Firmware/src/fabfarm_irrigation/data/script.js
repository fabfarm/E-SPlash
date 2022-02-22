// DEFINITIONS -----------------------------------
// Json payload state (state means it can change)
let jsonDataState = {};
let mainHtmlState = [];
// Endpoint URL constants
const GET_DATA_ENDPOINT = "/getData";
const UPDATE_DATA_ENDPOINT = "/updateData";
const UPDATE_SCHEDULING_MODE_ENPOINT = "/update/scheduling-mode";
const UPDATE_RELAY_ENABLE_ENPOINT = "/update/relay-enable";
const UPDATE_RELAY_TIME = "/update/relay-time";
const ADD_RELAY_TIME = "/add/relay-time";
const REMOVE_RELAY_TIME = "/remove/relay-time";
const ADD_RELAY = "/add/relay";
const REMOVE_RELAY = "/remove/relay";
// Payload constants
const RANGE = "range";
const END = "end";
const START = "start";
const DURATION_INPUT = "duration-input";
// JSON keys
const DATA_KEY = "data";
const RELAYS_KEY = "relays";
const SCHEDULE_MODE_KEY = "isScheduleMode";
// ------------------------------------------------

// Definition of functions -----------------------------
const fetchJSONState = function () {
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

function reRender() {
  fetch(GET_DATA_ENDPOINT)
    .then((res) => res.json())
    .then((jsonData) => {
      jsonDataState = jsonData;
      updateSchedulingHtml();
    });
}

function updateSchedulingHtml() {
  const mainHtml = [];
  // Scheduling Mode container
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
                                onChange="updateMode({value: ${!jsonDataState
                                  .data.isScheduleMode}});" 
                                id="jasonData.data.isScheduleMode" class="form-check-input" type="checkbox">
                            </div>
                        <span class="m-2 display-6">Automatic</span>
                </div>
        </div>`);

  // Relays Containers
  jsonDataState.relays.forEach((relay, i) => {
    // Manual Mode
    if (!jsonDataState.data.isScheduleMode) {
      mainHtml.push(`
      <div class="shadow d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center flex-column">
            <h6 class="display-6 main-title"><strong>${relay.name} (pin ${
        relay.pin
      })</strong></h6>
                <div class="form-check form-switch">
                          <input
                          style="width: 100px; height: 50px;" 
                          onChange="updateRelayEnabled({ enabled: ${
                            jsonDataState.relays[i].isEnabled
                          }, index: ${i} });" 
                          id="jsonData.relays[${i}].isEnabled" type="checkbox" ${
        relay.isEnabled ? "checked" : ""
      } class="form-check-input">
                </div>
        </div>`);
    }
    // Automatic Mode
    else {
      let times = "";
      relay.times.forEach((time, j) => {
        const endTimeHours =
          (parseInt(time.startTime.split(":")[0]) * 60 +
            parseInt(time.startTime.split(":")[1]) +
            parseInt(time.duration)) /
          60;
        const endTimeHoursR = Math.floor(endTimeHours);
        const endTimeMinutes = (endTimeHours - endTimeHoursR) * 60;
        const endTimeMinutesR = Math.round(endTimeMinutes);
        const finalEndTime = `${
          endTimeHoursR < 10 ? "0" + endTimeHoursR.toString() : endTimeHoursR
        }:${
          endTimeMinutesR < 10
            ? "0" + endTimeMinutesR.toString()
            : endTimeMinutesR
        }`;
        times += `
                                    <div class="d-flex flex-column align-items-stretch w-100 p-2">
                                    <div class="d-flex align-items-center justify-content-end w-100">
                                      <button type="button" class="btn btn-danger" onClick="removeTime({relayIndex: ${i}, timeIndex: ${j}})">Remove Time</button>
                                    </div>
                                      <div class="d-flex align-items-center">
                                        <input class="form-control m-2" 
                                          id="jsonData.relays[${i}].times[${j}].startTime" 
                                          type="time"
                                          onBlur="updateRelayTimes({rIndex: ${i}, inputType: 'start', tIndex: ${j}, startTime: this.value, duration: ${time.duration}, endTime: '${finalEndTime}'})"
                                          required 
                                          value="${time.startTime}"
                                        >
                                        <input class="form-control m-2" 
                                          type="time"
                                          id="jsonData.relays[${i}].times[${j}].endTime" 
                                          required value="${finalEndTime}"
                                          onBlur="updateRelayTimes({rIndex: ${i}, inputType: 'end', tIndex: ${j}, startTime:  '${time.startTime}', duration: ${time.duration}, endTime: this.value})"
                                        >
                                      </div>
                                      <div class="d-flex align-items-center mt-2">
                                        <input type="range" class="form-range" id="jsonData.relays[${i}].times[${j}].duration" 
                                        onChange="updateRelayTimes({rIndex: ${i}, inputType: 'range', tIndex: ${j}, startTime:  '${time.startTime}', duration: this.value, endTime: '${finalEndTime}'})"
                                        min="0" max="60" value="${time.duration}" step="1" >
                                      </div>
                                      <div class="d-flex align-items-center w-100 justify-content-center">
                                        <input id="jsonData.relays[${i}].times[${j}].duration-input" onChange="updateRelayTimes({rIndex: ${i}, inputType: 'duration-input', tIndex: ${j}, startTime:  '${time.startTime}', duration: this.value, endTime: '${finalEndTime}'})" class="form-control w-25" value="${time.duration}" type="number"><span class="m-2">min</span>
                                      </div>
                                    </div>`;
      });
      mainHtml.push(`
      <div class="shadow d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center flex-column">
            <h6 class="display-6 main-title"><strong>${relay.name} (pin ${relay.pin})</strong></h6>
                ${times}
                <button type="button" class="btn btn-primary" onClick="addTime(${i})">Add time</button>
        </div>`);
    }
  });

  // Write HTML to main
  const main = document.getElementById("main");
  main.innerHTML = mainHtml.join(""); //this will REBUILD the page per ping (?)
}

function updateMode(payload) {
  // Update local state
  jsonDataState.data.isScheduleMode = payload.value;
  //Update html
  updateSchedulingHtml();
  // Send server request
  showLoadSpinner(true);
  fetch(UPDATE_SCHEDULING_MODE_ENPOINT, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      value: payload.value,
    }),
  }).then(() => {
    showLoadSpinner(false);
    displaySuccessToast();
  });
}

function updateRelayEnabled(payload) {
  // Update local state
  const relayIndex = payload.index;
  const enabled = !payload.enabled;
  jsonDataState.relays[relayIndex].isEnabled =
    !jsonDataState.relays[relayIndex].enabled;

  // Send request to server
  showLoadSpinner(true);
  fetch(UPDATE_RELAY_ENABLE_ENPOINT, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ enabled, relayIndex }),
  }).then(() => {
    showLoadSpinner(false);
    displaySuccessToast();
  });
}

function updateRelayTimes(payload) {
  const relayIndex = payload.rIndex;
  const relayToUpdate = jsonDataState.relays[relayIndex];
  let startToUpdate = parseTimeHHMMToMinutes(payload.startTime);
  let durationToUpdate = parseInt(payload.duration);
  let endToDisplay = parseTimeHHMMToMinutes(payload.endTime);

  if (payload.inputType === START) {
    // Start is new start
    // Duration does not change
    // End is Start + duration
    endToDisplay = startToUpdate + durationToUpdate;
  }

  if (payload.inputType === RANGE || payload.inputType === DURATION_INPUT) {
    // Start does not change
    // Duration is new duration
    // End is Start + Duration
    endToDisplay = startToUpdate + durationToUpdate;
  }

  if (payload.inputType === END) {
    // Start does not change
    // Duration is End minus Start
    // End is new end
    durationToUpdate = endToDisplay - startToUpdate;
  }

  if (!(durationToUpdate <= 0)) {
    // Update local state
    jsonDataState.relays[payload.rIndex].times[payload.tIndex].startTime =
      parseTimeMinutesToHHMM(startToUpdate);
    jsonDataState.relays[payload.rIndex].times[payload.tIndex].duration =
      durationToUpdate;
    document.getElementById(
      `jsonData.relays[${payload.rIndex}].times[${payload.tIndex}].endTime`
    ).value = parseTimeMinutesToHHMM(endToDisplay);
    document.getElementById(
      `jsonData.relays[${payload.rIndex}].times[${payload.tIndex}].duration`
    ).value = durationToUpdate;
    document.getElementById(
      `jsonData.relays[${payload.rIndex}].times[${payload.tIndex}].duration-input`
    ).value = durationToUpdate;

    showLoadSpinner(true);
    fetch(UPDATE_RELAY_TIME, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        relayIndex: payload.rIndex,
        timeIndex: payload.tIndex,
        startTime: parseTimeMinutesToHHMM(startToUpdate),
        duration: durationToUpdate,
      }),
    }).then(() => {
      showLoadSpinner(false);
      displaySuccessToast();
    });
  } else {
    showLoadSpinner(false);
    document.getElementById("toast-error").style.display = "flex";
    document.getElementById("toast-error-body").innerText =
      "You cannot enter a negative duration value";

    setTimeout(() => {
      document.getElementById("toast-error").style.display = "none";
    }, 5000);
  }
}

function parseTimeHHMMToMinutes(time) {
  const [hour, minutes] = time.split(":");
  const hourInMinutes = parseInt(hour) * 60;
  const finalMinutes = hourInMinutes + parseInt(minutes);

  return finalMinutes;
}

function parseTimeMinutesToHHMM(time) {
  let h = Math.floor(time / 60);
  let m = time % 60;
  h = h < 10 ? "0" + h : h;
  m = m < 10 ? "0" + m : m;
  return h + ":" + m;
}

function displaySuccessToast() {
  document.getElementById("toast-success").style.display = "flex";
  document.getElementById("toast-success-body").innerText =
    "Data successfully updated!";

  setTimeout(() => {
    document.getElementById("toast-success").style.display = "none";
  }, 5000);
}

function showLoadSpinner(show) {
  document.getElementById("spinner-bottom-right").style.display = show
    ? "flex"
    : "none";
}

function closeErrorToast() {
  document.getElementById("toast-error").style.display = "none";
}

function closeSuccessToast() {
  document.getElementById("toast-success").style.display = "none";
}

function addTime(relayIndex) {
  showLoadSpinner(true);
  // Update local state
  jsonDataState.relays[relayIndex].times.push(
    JSON.parse(JSON.stringify({ startTime: "10:00", duration: 30 }))
  );
  updateSchedulingHtml();
  fetch(ADD_RELAY_TIME, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ relayIndex }),
  }).then(() => {
    showLoadSpinner(false);
    displaySuccessToast();
  });
}

function removeTime(payload) {
  showLoadSpinner(true);
  console.log(payload);
  // Update local state
  jsonDataState.relays[payload.relayIndex].times.splice(payload.timeIndex, 1);
  updateSchedulingHtml();
  fetch(REMOVE_RELAY_TIME, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      relayIndex: payload.relayIndex,
      timeIndex: payload.timeIndex,
    }),
  }).then(() => {
    showLoadSpinner(false);
    displaySuccessToast();
  });
}

function addRelay(payload) {
  showLoadSpinner(true);
  // Update local state
  jsonDataState.relays[payload.relayIndex].push(
    JSON.parse(
      JSON.stringify({
        name: payload.name,
        pin: payload.pin,
        isEnabled: 0,
        times: [
          {
            startTime: "10:00",
            duration: 30,
            hour: 10,
            min: 0,
          },
        ],
      })
    )
  );
  updateSchedulingHtml();
  fetch(ADD_RELAY, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      relayIndex: payload.relayIndex,
      name: payload.name,
      pin: payload.pin,
    }),
  }).then(() => {
    showLoadSpinner(false);
    displaySuccessToast();
  });
}

function removeRelay(payload) {
  showLoadSpinner(true);
  // Update local state
  jsonDataState.relays.slice(payload.relayIndex, 1);
  updateSchedulingHtml();
  fetch(REMOVE_RELAY, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      relayIndex: payload.relayIndex,
    }),
  }).then(() => {
    showLoadSpinner(false);
    displaySuccessToast();
  });
}
// -----------------------------------------------------

// Main ------------------------------------------------
// Get JSON data from server first time
fetchJSONState();
// Get JSON data from server each 10 seconds for data and clock update
setInterval(() => {
  fetchJSONState();
  updateSchedulingHtml();
}, 10000);
// Re-render after data fetch
reRender();
// -----------------------------------------------------
