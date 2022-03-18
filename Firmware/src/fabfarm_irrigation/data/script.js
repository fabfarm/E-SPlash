// DEFINITIONS -----------------------------------
// Json payload state (state means it can change)
let jsonDataState = {};
let previousJsonDataState = {};
let mainHtmlState = [];
// Endpoint URL constants
const GET_DATA_ENDPOINT = "/data.json";
const UPDATE_RELAY_TIME = "/relay/update-time";
const ADD_RELAY_TIME = "/relay/add-time";
const ADD_RELAY = "/relay/add";
// ------------------------------------------------

// Definition of functions -----------------------------
var timeoutID = null;
var doRefresh = false;

function fetchJSONState() {
	fetch(GET_DATA_ENDPOINT)
		.then((res) => res.json())
		.then((jsonData) => {
			// Check if scheduling mode has changed
			let rebuildHtml = false;
			if(!jsonDataState.data || jsonDataState.data.isScheduleMode != jsonData.data.isScheduleMode)
				rebuildHtml = true;

			previousJsonDataState = jsonDataState;
			jsonDataState = jsonData;

			if(document.getElementById("currentTime").innerText != jsonDataState.data.currentTime)
				document.getElementById("currentTime").innerText = jsonDataState.data.currentTime;
			if(document.getElementById("temperature").innerText != jsonDataState.data.temperature)
					document.getElementById("temperature").innerText = jsonDataState.data.temperature;
			if(document.getElementById("humidity").innerText != jsonDataState.data.humidity)
					document.getElementById("humidity").innerText = jsonDataState.data.humidity;
			if(document.getElementById("batLevel").innerText != jsonDataState.data.batLevel)
					document.getElementById("batLevel").innerText = jsonDataState.data.batLevel;

			if(doRefresh) {
				updateSchedulingHtml(rebuildHtml);
				timeoutID = setTimeout(() => {
					fetchJSONState();
				}, 100);
			}
		}).catch(error => {
			console.log(error);

			if(doRefresh) {
				updateSchedulingHtml();
				timeoutID = setTimeout(() => {
					fetchJSONState();
				}, 100);
			}
		});
}

function startRefresh() {
	if(doRefresh)
		return;
	doRefresh = true;

	fetchJSONState();
}

function stopRefresh() {
	if(timeoutID)
		clearTimeout(timeoutID);
	timeoutID = null;
	doRefresh = false;
}

function updateSchedulingHtml(rebuildHtml = false) {
	document.getElementById("schedulingMode").checked = jsonDataState.data.isScheduleMode;
	if(document.getElementById("relays").innerHTML == "")
		rebuildHtml = true;	

	// Relays Containers
	const relays = document.getElementById("relays");
	const relaysHtml = [];
	jsonDataState.relays.forEach((relay, i) => {
		// Manual Mode
		if (!jsonDataState.data.isScheduleMode) {
			if(!rebuildHtml) {
				document.getElementById(`relay${i}.isEnabled`).checked = relay.isEnabled;
			} else {
				relaysHtml.push(
					`<div class="shadow d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center flex-column">
						<h6 class="display-6 main-title"><strong>${relay.name} (pin ${relay.pin})</strong></h6>
						<div class="form-check form-switch">
							<input style="width: 100px; height: 50px;" onChange="updateRelayEnabled(${i}, this);"
								id="relay${i}.isEnabled" type="checkbox" ${relay.isEnabled ? "checked" : ""} class="form-check-input">
						</div>
				 	</div>`);
	 		}
		}
		// Automatic Mode
		else {
			let times = "";
			relay.times.forEach((time, j) => {
				// Calculate and format the end time
				time.endTime = parseTimeMinutesToHHMM(parseTimeHHMMToMinutes(time.startTime) + parseInt(time.duration));
				if(!rebuildHtml) {
					let previousTime = previousJsonDataState.relays[i].times[j];

					// Only update values when they have changed
					if(time.startTime != previousTime.startTime)
						document.getElementById(`relay${i}.time${j}.startTime`).value = time.startTime;
					if(time.endTime != previousTime.endTime)
						document.getElementById(`relay${i}.time${j}.endTime`).value = time.endTime;
					if(time.duration != previousTime.duration)
						document.getElementById(`relay${i}.time${j}.duration`).value = time.duration;
				} else {
					times += `<form id="relay${i}.time${j}">
								<input type="hidden" name="relayIndex" value="${i}"/>
								<input type="hidden" name="timeIndex" value="${j}"/>
								<div class="d-flex flex-column align-items-stretch w-100 p-2">
									<div class="d-flex align-items-center justify-content-end w-100">
										<button class="btn btn-danger" onClick="removeTime(${i}, ${j})">Remove Time</button>
									</div>
									<div class="d-flex align-items-center">
										<input class="form-control m-2" id="relay${i}.time${j}.startTime" name="startTime" type="time" onchange="updateRelayTimes(${i}, ${j}, this)" required value="${time.startTime}">
										<input class="form-control m-2" id="relay${i}.time${j}.endTime"   name="endTime"   type="time" onchange="updateRelayTimes(${i}, ${j}, this)" required value="${time.endTime}">
									</div>
									<div class="d-flex align-items-center mt-2">
										<input class="form-range" id="relay${i}.time${j}.duration" name="duration" type="range" onchange="updateRelayTimes(${i}, ${j}, this)" min="0" max="60" value="${time.duration}" step="1">
									</div>
									<div class="d-flex align-items-center w-100 justify-content-center">
										<input class="form-control w-25" id="relay${i}.time${j}.durationInput" name="durationInput" onchange="updateRelayTimes(${i}, ${j}, this)" value="${time.duration}" type="number"><span class="m-2">min</span>
									</div>
								</div>
							</form>`;
				}
			});
			relaysHtml.push(
				`<div class="shadow d-flex align-items-center bg-white rounded p-4 m-2 justify-content-center flex-column">
						<h6 class="display-6 main-title"><strong>${relay.name} (pin ${relay.pin})</strong></h6>
						${times}
						<button class="btn btn-primary" onClick="addTime(${i})">Add time</button>
				</div>`);
		}
	});

	if(rebuildHtml)
		relays.innerHTML = relaysHtml.join(""); //this will REBUILD the page per ping (?)
}

function updateSchedulingMode(event) {
	stopRefresh();
	displayLoadSpinner();

	// Build the URL
	let url;
	if(jsonDataState.data.isScheduleMode)
		url = "/mode/scheduled";
	else
		url = "/mode/manual";

	// Send server request
	fetch(url).then(() => {
		// Update local state
		jsonDataState.data.isScheduleMode = event.checked;
		updateSchedulingHtml(true);

		closeLoadSpinner();
		displaySuccessToast();
	
		startRefresh();
	});
}

function updateRelayEnabled(index, event) {
	stopRefresh();
	displayLoadSpinner();

	// Build the URL
	let url;
	if(event.checked)
		url = `/relay/${index}/on`
	else
		url = `/relay/${index}/off`
	
	// Send request to server
	fetch(url).then(() => {
		// Update local state
		const relayIndex = index;
		const enabled = event.checked;
		jsonDataState.relays[relayIndex].isEnabled = event.checked;
	
		closeLoadSpinner();
		displaySuccessToast();

		startRefresh();
	});
}

function updateRelayTimes(relayIndex, timeIndex, event) {
	let startTime = event.form.elements.startTime.value;
	let endTime = event.form.elements.endTime.value;
	let duration = event.form.elements.duration.value;
	if(event.name == "durationInput")
		duration = event.value;

	let startToUpdate = parseTimeHHMMToMinutes(startTime);
	let durationToUpdate = parseInt(duration);
	let endToDisplay = parseTimeHHMMToMinutes(endTime);

    switch(event.name) {
		case "startTime":
			// Start is new start
			// Duration does not change
			// End is Start + duration
			endToDisplay = startToUpdate + durationToUpdate;
		break;
		case "duration":
		case "durationInput":
			// Start does not change
			// Duration is new duration
			// End is Start + Duration
			endToDisplay = startToUpdate + durationToUpdate;
		break;
		case "endTime":
			// Start does not change
			// Duration is End minus Start
			// End is new end
			durationToUpdate = endToDisplay - startToUpdate;
		break;
	}

	if (!(durationToUpdate <= 0)) {
		stopRefresh();
		displayLoadSpinner();

		//let url = `/relay/${relayIndex}/time/${timeIndex}`;
		fetch(UPDATE_RELAY_TIME, {
			method: "POST",
			headers: { "Content-Type": "application/json" },
			body: JSON.stringify({
				relayIndex: relayIndex,
				timeIndex: timeIndex,
				startTime: parseTimeMinutesToHHMM(startToUpdate),
				duration: durationToUpdate,
			}),
		}).then(() => {
			// Update local state
			jsonDataState.relays[relayIndex].times[timeIndex].startTime = parseTimeMinutesToHHMM(startToUpdate);
			jsonDataState.relays[relayIndex].times[timeIndex].duration = durationToUpdate;
			document.getElementById(`relay${relayIndex}.time${timeIndex}.endTime`).value = parseTimeMinutesToHHMM(endToDisplay);
			document.getElementById(`relay${relayIndex}.time${timeIndex}.duration`).value = durationToUpdate;
			document.getElementById(`relay${relayIndex}.time${timeIndex}.durationInput`).value = durationToUpdate;
	
			closeLoadSpinner();
			displaySuccessToast();
	
			startRefresh();
		});
	} else {
		closeLoadSpinner();
		displayErrorToast("You cannot enter a negative duration value");

		startRefresh();
	}
}

function addTime(relayIndex) {
	stopRefresh();
	displayLoadSpinner();

	fetch(ADD_RELAY_TIME, {
		method: "POST",
		headers: { "Content-Type": "application/json" },
		body: JSON.stringify({ relayIndex }),
	}).then(() => {
		// Update local state
		jsonDataState.relays[relayIndex].times.push(
			JSON.parse(JSON.stringify({ startTime: "10:00", duration: 30 }))
		);
		updateSchedulingHtml(true);
	
		closeLoadSpinner();
		displaySuccessToast();

		startRefresh();
	});
}

function removeTime(relayIndex, timeIndex) {
	stopRefresh();
	displayLoadSpinner();

	let url = `/relay/${relayIndex}/time/${timeIndex}`;
	fetch(url, { method: "DELETE" }).then(() => {
		// Update local state
		jsonDataState.relays[relayIndex].times.splice(timeIndex, 1);
		updateSchedulingHtml(true);
	
		closeLoadSpinner();
		displaySuccessToast();

		startRefresh();
	});
}

// Time functions
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

// Setup functions
function addRelay(payload) {
	displayLoadSpinner();

	fetch(ADD_RELAY, {
		method: "POST",
		headers: { "Content-Type": "application/json" },
		body: JSON.stringify({
			relayIndex: payload.relayIndex,
			name: payload.name,
			pin: payload.pin,
		}),
	}).then(() => {
		// Update local state
		jsonDataState.relays[payload.relayIndex].push(
			JSON.parse(
				JSON.stringify({
					name: payload.name,
					pin: payload.pin,
					isEnabled: 0
				})
			)
		);
		updateSchedulingHtml(true);

		closeLoadSpinner();
		displaySuccessToast();
	});
}

function removeRelay(payload) {
	displayLoadSpinner();

	let url = `/relay/${payload.relayIndex}`;
	fetch(url, { method: "DELETE" }).then(() => {
		// Update local state
		jsonDataState.relays.slice(payload.relayIndex, 1);
		updateSchedulingHtml(true);

		closeLoadSpinner();
		displaySuccessToast();
	});
}

// UI functions
function displaySuccessToast() {
	document.getElementById("toast-success").style.display = "flex";
	document.getElementById("toast-success-body").innerText =
		"Data successfully updated!";

	setTimeout(() => {
		document.getElementById("toast-success").style.display = "none";
	}, 5000);
}

function closeErrorToast() {
	document.getElementById("toast-error").style.display = "none";
}

function displayErrorToast(message) {
	document.getElementById("toast-error").style.display = "flex";
	document.getElementById("toast-error-body").innerText = message;

	setTimeout(() => {
		closeErrorToast();
	}, 5000);
}

function closeSuccessToast() {
	document.getElementById("toast-success").style.display = "none";
}

function displayLoadSpinner(show) {
	document.getElementById("spinner-bottom-right").style.display = "flex";
}

function closeLoadSpinner() {
	document.getElementById("spinner-bottom-right").style.display = "none";
}