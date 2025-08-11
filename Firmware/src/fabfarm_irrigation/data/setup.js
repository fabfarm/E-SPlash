var jsonData = null;

// Update WiFi credentials
function updateCredentials() {
    var ssid = document.getElementById("ssid").value.trim();
    var password = document.getElementById("password").value.trim();

    if (!ssid) {
        showToast("Please enter a network name (SSID)", false);
        return;
    }

    // Send credentials as form data to the new endpoint
    var formData = new FormData();
    formData.append('ssid', ssid);
    formData.append('password', password);

    fetch("/wifi/add", {
        method: "POST",
        body: formData
    }).then(response => {
        if (response.ok) {
            showToast("WiFi network added successfully!", true);
            document.getElementById("ssid").value = "";
            document.getElementById("password").value = "";
            loadNetworkList(); // Refresh the network list
        } else {
            response.text().then(text => {
                showToast("Error adding WiFi network: " + text, false);
            });
        }
    }).catch(error => {
        showToast("Error adding WiFi network: " + error, false);
    });
}

// Load WiFi network list - read from the new endpoint
function loadNetworkList() {
    var networkList = document.getElementById("network-list");
    networkList.innerHTML = '<div class="loading-message">Loading networks...</div>';

    fetch("/wifi/list")
        .then(response => {
            if (response.ok) {
                return response.text();
            } else {
                throw new Error("Could not load networks (Error: " + response.status + ")");
            }
        })
        .then(data => {
            var networks = data.split('\n').filter(line => line.trim());
            networkList.innerHTML = "";

            if (networks.length === 0) {
                networkList.innerHTML = '<div class="text-muted">No networks configured</div>';
                return;
            }

            networks.forEach(function (network) {
                if (network.trim()) {
                    var parts = network.split(',');
                    if (parts.length >= 1) {
                        var ssid = parts[0].trim();
                        var networkDiv = document.createElement('div');
                        networkDiv.className = 'wifi-network';
                        networkDiv.innerHTML = '<i class="fas fa-wifi me-2"></i>' + ssid;
                        networkList.appendChild(networkDiv);
                    }
                }
            });
        })
        .catch(error => {
            networkList.innerHTML = `<div class="text-muted">${error.message}</div>`;
        });
}

// Update time and date
function updateTime(obj) {
    if (!jsonData) {
        showToast("System data not loaded yet", false);
        return;
    }

    var fl1 = document.getElementById("date").value;
    var fl2 = document.getElementById("time").value;

    if (!fl1 || !fl2) {
        showToast("Please enter both date and time", false);
        return;
    }

    // Ensure the changedtime structure exists
    if (!jsonData.data.changedtime) {
        jsonData.data.changedtime = [{}];
    }
    if (!jsonData.data.changedtime[0]) {
        jsonData.data.changedtime[0] = {};
    }

    jsonData.data.changedtime[0].manualtimeenabled = 0;
    jsonData.data.changedtime[0].Ttime = fl2;
    jsonData.data.changedtime[0].Tdate = fl1;

    var timex = new Date(fl1);
    var hour = fl2.split(":")[0];
    var min = fl2.split(":")[1];
    var day = timex.getDate();
    var month = timex.getMonth();
    var year = timex.getFullYear();

    jsonData.data.changedtime[0].min = parseInt(min);
    jsonData.data.changedtime[0].hour = parseInt(hour);
    jsonData.data.changedtime[0].day = day;
    jsonData.data.changedtime[0].month = month + 1;
    jsonData.data.changedtime[0].year = year;

    var json = JSON.stringify(jsonData);
    fetch("/updateData", {
        method: "POST",
        headers: {
            "Content-Type": "application/json;"
        },
        body: json
    }).then(response => {
        if (response.ok) {
            showToast("Time and date updated successfully!", true);
            refresh(); // Refresh to show updated time
        } else {
            showToast("Error updating time and date", false);
        }
    }).catch(error => {
        showToast("Error updating time and date", false);
    });
}

// Enable/disable internet time
function enableInternetUpdate() {
    if (!jsonData) {
        showToast("System data not loaded yet", false);
        return;
    }

    // Ensure the changedtime structure exists
    if (!jsonData.data.changedtime) {
        jsonData.data.changedtime = [{}];
    }
    if (!jsonData.data.changedtime[0]) {
        jsonData.data.changedtime[0] = {};
    }

    jsonData.data.changedtime[0].internettime = document.getElementById("useInternetTime").checked;
    var json = JSON.stringify(jsonData);
    fetch("/updateData", {
        method: "POST",
        headers: {
            "Content-Type": "application/json;"
        },
        body: json
    }).then(response => {
        if (response.ok) {
            showToast("Internet time setting updated!", true);
        } else {
            showToast("Error updating internet time setting", false);
        }
    }).catch(error => {
        showToast("Error updating internet time setting", false);
    });
}

// Update UTC offset
function updateUTCOffset() {
    if (!jsonData) {
        showToast("System data not loaded yet", false);
        return;
    }

    // Ensure the changedtime structure exists
    if (!jsonData.data.changedtime) {
        jsonData.data.changedtime = [{}];
    }
    if (!jsonData.data.changedtime[0]) {
        jsonData.data.changedtime[0] = {};
    }

    jsonData.data.changedtime[0].utcOffset = parseInt(document.getElementById("utcOffset").value);
    var json = JSON.stringify(jsonData);
    fetch("/updateData", {
        method: "POST",
        headers: {
            "Content-Type": "application/json;"
        },
        body: json
    }).then(response => {
        if (response.ok) {
            showToast("Timezone updated successfully!", true);
        } else {
            showToast("Error updating timezone", false);
        }
    }).catch(error => {
        showToast("Error updating timezone", false);
    });
}

// Edit zone name
function editZoneName(index) {
    var zoneNameElement = document.getElementById('zone-name-' + index);
    var editInputElement = document.getElementById('edit-input-' + index);
    var editBtnElement = document.getElementById('edit-btn-' + index);
    var saveBtnElement = document.getElementById('save-btn-' + index);

    zoneNameElement.style.display = 'none';
    editInputElement.classList.add('show');
    editBtnElement.style.display = 'none';
    saveBtnElement.style.display = 'inline-block';

    document.getElementById('zone-input-' + index).value = jsonData.relays[index].name;
    document.getElementById('zone-input-' + index).focus();
}

// Save zone name
function saveZoneName(index) {
    var newName = document.getElementById('zone-input-' + index).value.trim();

    if (!newName) {
        showToast("Zone name cannot be empty", false);
        return;
    }

    // Update the JSON data
    jsonData.relays[index].name = newName;

    var json = JSON.stringify(jsonData);
    fetch("/updateData", {
        method: "POST",
        headers: {
            "Content-Type": "application/json;"
        },
        body: json
    }).then(response => {
        if (response.ok) {
            showToast("Zone name updated successfully!", true);
            // Update the UI
            document.getElementById('zone-name-' + index).textContent = newName;

            // Hide edit mode
            var zoneNameElement = document.getElementById('zone-name-' + index);
            var editInputElement = document.getElementById('edit-input-' + index);
            var editBtnElement = document.getElementById('edit-btn-' + index);
            var saveBtnElement = document.getElementById('save-btn-' + index);

            zoneNameElement.style.display = 'inline';
            editInputElement.classList.remove('show');
            editBtnElement.style.display = 'inline-block';
            saveBtnElement.style.display = 'none';
        } else {
            showToast("Error updating zone name", false);
        }
    }).catch(error => {
        showToast("Error updating zone name", false);
    });
}

// Render zones
function renderZones() {
    var zonesContainer = document.getElementById("zones-container");

    if (!jsonData || !jsonData.relays || jsonData.relays.length === 0) {
        zonesContainer.innerHTML = '<div class="text-muted">No zone configuration found</div>';
        return;
    }

    var zonesHtml = "";
    var activeZoneCount = 0;

    for (var i = 0; i < jsonData.relays.length; i++) {
        var relay = jsonData.relays[i];
        if (relay.active) {
            activeZoneCount++;
            var status = relay.isEnabled ? "ON" : "OFF";
            var statusClass = relay.isEnabled ? "status-on" : "status-off";

            zonesHtml += `
							<div class="zone-item">
								<div class="zone-header">
									<div class="zone-name" id="zone-name-${i}">${relay.name || 'Zone ' + (i + 1)}</div>
									<div class="d-flex align-items-center gap-2">
										<span class="zone-status ${statusClass}">${status}</span>
										<button class="btn btn-edit btn-sm" id="edit-btn-${i}" onclick="editZoneName(${i})">
											<i class="fas fa-edit"></i> Edit Name
										</button>
										<button class="btn btn-save btn-sm" id="save-btn-${i}" onclick="saveZoneName(${i})" style="display: none;">
											<i class="fas fa-save"></i> Save
										</button>
									</div>
								</div>
								<div class="edit-zone-input" id="edit-input-${i}">
									<input type="text" class="form-control form-control-modern" id="zone-input-${i}" placeholder="Enter zone name" maxlength="20">
								</div>
							</div>
						`;
        }
    }

    if (activeZoneCount === 0) {
        zonesHtml = '<div class="text-muted">No active zones configured</div>';
    }

    zonesContainer.innerHTML = zonesHtml;
}

// Main refresh function
function refresh() {
    fetch("/data.json")
        .then(response => {
            if (response.ok) {
                return response.json();
            } else {
                throw new Error("Connection failed (Status: " + response.status + ")");
            }
        })
        .then(data => {
            jsonData = data;

            // Update time display
            document.getElementById("currentTime").innerText = jsonData.data.currentTime || "Time not available";

            // Update form values safely
            if (jsonData.data.changedtime && jsonData.data.changedtime[0]) {
                var timeData = jsonData.data.changedtime[0];

                // Set time field
                if (timeData.Ttime) {
                    document.getElementById("time").value = timeData.Ttime;
                }

                // Set date field
                if (timeData.Tdate) {
                    // Convert from MM/DD/YYYY to YYYY-MM-DD for date input
                    var dateParts = timeData.Tdate.split('/');
                    if (dateParts.length === 3) {
                        var formattedDate = dateParts[2] + '-' + dateParts[0].padStart(2, '0') + '-' + dateParts[1].padStart(2, '0');
                        document.getElementById("date").value = formattedDate;
                    }
                }

                // Set internet time checkbox
                document.getElementById("useInternetTime").checked = timeData.internettime === true || timeData.internettime === 1;

                // Set UTC offset if available
                if (timeData.utcOffset !== undefined) {
                    document.getElementById("utcOffset").value = timeData.utcOffset;
                }
            }

            // Render zones
            renderZones();

        })
        .catch(error => {
            console.error("Error parsing JSON:", error);
            document.getElementById("currentTime").innerText = "Error loading data";
            document.getElementById("zones-container").innerHTML = '<div class="text-muted">Error parsing system data</div>';
        });
}

// Toast notifications
function showToast(message, isSuccess) {
    var toast = isSuccess ? document.getElementById("toast-success") : document.getElementById("toast-error");
    var toastBody = isSuccess ? document.getElementById("toast-success-body") : document.getElementById("toast-error-body");

    toast.style.display = "flex";
    toastBody.innerText = message;

    setTimeout(() => {
        toast.style.display = "none";
    }, 5000);
}

function closeErrorToast() {
    document.getElementById("toast-error").style.display = "none";
}

function closeSuccessToast() {
    document.getElementById("toast-success").style.display = "none";
}

// Initialize page
document.addEventListener('DOMContentLoaded', function () {
    refresh();
    loadNetworkList();
});

// Fallback initialization
setTimeout(function () {
    if (!jsonData) {
        refresh();
        loadNetworkList();
    }
}, 1000);
