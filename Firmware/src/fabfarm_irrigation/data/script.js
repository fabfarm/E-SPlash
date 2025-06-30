// DEFINITIONS -----------------------------------
// Json payload state (state means it can change)
let jsonDataState = {};
let previousJsonDataState = {};
let mainHtmlState = [];
// Endpoint URL constants
const GET_DATA_ENDPOINT = "/data.json";
const UPDATE_RELAY_TIME = "/relay/update-time";
const ADD_RELAY_TIME = "/relay/add-time";
// Removed ADD_RELAY constant since relays are now hardcoded
let timeoutID = null;
let doRefresh = false;

// Functions
function getNextScheduleEvent() {
	if (!jsonDataState.data || !jsonDataState.data.isScheduleMode || !jsonDataState.relays) {
		return null;
	}
	
	let now = new Date();
	let currentMinutes = now.getHours() * 60 + now.getMinutes();
	let nextEvent = null;
	
	jsonDataState.relays.forEach(relay => {
		if (relay.times) {
			relay.times.forEach(time => {
				let startMinutes = parseTimeHHMMToMinutes(time.startTime);
				let endMinutes = startMinutes + parseInt(time.duration);
				
				// Find next start or end event after current time
				if (startMinutes > currentMinutes) {
					nextEvent = Math.min(nextEvent || Infinity, startMinutes);
				}
				if (endMinutes > currentMinutes) {
					nextEvent = Math.min(nextEvent || Infinity, endMinutes);
				}
				
				// Handle schedules that cross midnight (next day)
				let nextDayStart = startMinutes + (24 * 60);
				let nextDayEnd = endMinutes + (24 * 60);
				nextEvent = Math.min(nextEvent || Infinity, nextDayStart);
				nextEvent = Math.min(nextEvent || Infinity, nextDayEnd);
			});
		}
	});
	
	return nextEvent === Infinity ? null : nextEvent;
}

function getSmartPollInterval() {
	if (!jsonDataState.data || !jsonDataState.data.isScheduleMode) {
		// Manual mode - need frequent updates for real-time feedback
		return 5000; // 5 seconds
	}
	
	let nextEvent = getNextScheduleEvent();
	if (!nextEvent) {
		// No schedules configured - still poll regularly for status updates
		return 10000; // 10 seconds
	}
	
	let now = new Date();
	let currentMinutes = now.getHours() * 60 + now.getMinutes();
	let minutesToNext = Math.abs(nextEvent - currentMinutes);
	
	if (minutesToNext <= 2) {
		// Schedule event within 2 minutes - poll very frequently
		return 3000; // 3 seconds
	} else if (minutesToNext <= 10) {
		// Schedule event within 10 minutes - poll frequently
		return 5000; // 5 seconds
	} else {
		// No immediate schedule events - poll moderately
		return 10000; // 10 seconds
	}
}

function fetchJSONState() {
	fetch(GET_DATA_ENDPOINT)
	  .then(res => res.json())
	  .then(jsonData => {
		const rebuildHtml = !jsonDataState?.data || jsonDataState.data.isScheduleMode !== jsonData.data.isScheduleMode;
			previousJsonDataState = jsonDataState;
			jsonDataState = jsonData;
		updateElementIfChanged("currentTime", jsonDataState.data.currentTime);
		if (doRefresh) {
				updateSchedulingHtml(rebuildHtml);
		  let interval = getSmartPollInterval();
		  timeoutID = setTimeout(fetchJSONState, interval);
			}
	  })
	  .catch(error => {
			console.log(error);
		if (doRefresh) {
				updateSchedulingHtml();
		  let interval = getSmartPollInterval();
		  timeoutID = setTimeout(fetchJSONState, interval);
			}
		});
}

function updateElementIfChanged(elementId, newValue) {
	const element = document.getElementById(elementId);
	if (element?.innerText !== newValue) {
	  element.innerText = newValue;
	}
  }
  
  const startRefresh = () => {
	if (doRefresh) {
		return;
	}
	doRefresh = true;
	fetchJSONState();
  };

  const stopRefresh = () => {
		clearTimeout(timeoutID);
	timeoutID = null;
	doRefresh = false;
  };

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
					`<div class="relay-item">
						<div class="relay-header">
							<span class="relay-name">${relay.name}</span>
							<span class="relay-status ${relay.isEnabled ? 'status-on' : 'status-off'}">
								${relay.isEnabled ? 'ON' : 'OFF'}
							</span>
						</div>
						<div class="d-flex justify-content-center">
							<div class="form-check form-switch">
								<input style="width: 60px; height: 30px; transform: scale(1.5);" onChange="updateRelayEnabled(${i}, this);"
									id="relay${i}.isEnabled" type="checkbox" ${relay.isEnabled ? "checked" : ""} class="form-check-input">
								<label class="form-check-label ms-3 fw-bold" for="relay${i}.isEnabled">
									${relay.isEnabled ? 'Turn OFF' : 'Turn ON'}
								</label>
							</div>
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
					times += `<form id="relay${i}.time${j}" class="mb-3">
								<input type="hidden" name="relayIndex" value="${i}"/>
								<input type="hidden" name="timeIndex" value="${j}"/>
								<div class="border rounded-3 p-3 bg-light">
									<div class="d-flex justify-content-between align-items-center mb-3">
										<h6 class="mb-0 text-primary">Schedule ${j + 1}</h6>
										<button type="button" class="btn btn-outline-danger btn-sm" onClick="removeTime(${i}, ${j})">
											<i class="fas fa-trash"></i> Remove
										</button>
									</div>
									<div class="row g-3 mb-3">
										<div class="col-md-6">
											<label class="form-label fw-bold">Start Time</label>
											<input class="form-control" id="relay${i}.time${j}.startTime" name="startTime" type="time" onchange="updateRelayTimes(${i}, ${j}, this)" required value="${time.startTime}">
										</div>
										<div class="col-md-6">
											<label class="form-label fw-bold">End Time</label>
											<input class="form-control" id="relay${i}.time${j}.endTime" name="endTime" type="time" onchange="updateRelayTimes(${i}, ${j}, this)" required value="${time.endTime}">
										</div>
									</div>
									<div class="mb-3">
										<label class="form-label fw-bold">Duration: <span class="text-primary">${time.duration} minutes</span></label>
										<input class="form-range" id="relay${i}.time${j}.duration" name="duration" type="range" onchange="updateRelayTimes(${i}, ${j}, this)" min="0" max="60" value="${time.duration}" step="1">
									</div>
									<div class="row">
										<div class="col-6">
											<input class="form-control" id="relay${i}.time${j}.durationInput" name="durationInput" onchange="updateRelayTimes(${i}, ${j}, this)" value="${time.duration}" type="number" min="0" max="60">
										</div>
										<div class="col-6 d-flex align-items-center">
											<span class="fw-bold">minutes</span>
										</div>
									</div>
								</div>
							</form>`;
				}
			});
			relaysHtml.push(
				`<div class="relay-item">
					<div class="relay-header">
						<span class="relay-name">${relay.name}</span>
						<span class="relay-status ${relay.isEnabled ? 'status-on' : 'status-off'}">
							${relay.isEnabled ? 'ACTIVE' : 'INACTIVE'}
						</span>
					</div>
					${times}
					<div class="text-center mt-3">
						<button class="btn btn-modern btn-sm" onClick="addTime(${i})">
							<i class="fas fa-plus"></i> Add Schedule
						</button>
					</div>
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
		url = "/mode/manual";
	else
		url = "/mode/scheduled";

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

	if (!(durationToUpdate < 0)) {
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

// Setup functions - removed relay add/remove functions since relays are now hardcoded

  //ui functions
const showToast = (message, isSuccess) => {
	const toast = isSuccess ? document.getElementById("toast-success") : document.getElementById("toast-error");
	const toastBody = isSuccess ? document.getElementById("toast-success-body") : document.getElementById("toast-error-body");
	toast.style.display = "flex";
	toastBody.innerText = message;
	setTimeout(() => {
	  toast.style.display = "none";
	}, 5000);
  }
  
  const displaySuccessToast = () => {
	showToast("Data successfully updated!", true);
  }
  
  const displayErrorToast = (message) => {
	showToast(message, false);
  }
  
  const displayLoadSpinner = () => {
	document.getElementById("spinner-bottom-right").style.display = "flex";
  }
  
  const closeLoadSpinner = () => {
	document.getElementById("spinner-bottom-right").style.display = "none";
  }