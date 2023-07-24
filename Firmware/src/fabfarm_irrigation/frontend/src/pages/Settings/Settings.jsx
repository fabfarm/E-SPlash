import { useState, useEffect } from 'react';

import DateAndTime from '../../components/DateAndTime';
import Relays from '../../components/Relays';
import WifiCredentials from '../../components/WifiCredentials';
import FirmwareUpdate from '../../components/FirmwareUpdate';

const Settings = () => {
    const [isLoading, setIsLoading] = useState(true);
    const [error, setError] = useState(null);
    const [settingsData, setSettingsData] = useState(null);

    const fetchSettingsData = (url) => {
        setIsLoading(true);

        fetch(url)
            .then((res) => res.json())
            .then((data) => {
                console.log('FETCHED data:', data);

                // Now adjusting data structure. Eventually should be done on the server
                setSettingsData({
                    changedtime: data.data.changedtime,
                });
            })
            .catch((err) => {
                setError(err);
            })
            .finally(() => {
                setIsLoading(false);
            });
    };

    const updateTime = (obj) => {
        jsonData.data.changedtime[0].manualtimeenabled = 0;
        var fl1 = document.getElementById('date').value;
        var fl2 = document.getElementById('time').value;
        jsonData.data.changedtime[0].Ttime = fl2;
        var timex = new Date(fl1);
        var hour = fl2.split(':')[0];
        var min = fl2.split(':')[1];
        var day = timex.getDate();
        var month = timex.getMonth();
        var year = timex.getYear();
        jsonData.data.changedtime[0].min = parseInt(min);
        jsonData.data.changedtime[0].hour = parseInt(hour);
        jsonData.data.changedtime[0].day = day;
        jsonData.data.changedtime[0].month = month + 1;
        jsonData.data.changedtime[0].year = year + 1900;
        console.log(day);
        console.log(jsonData.data.changedtime[0].day);
        console.log(jsonData.data.changedtime.year);
        var json = JSON.stringify(jsonData);
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.open('POST', '/updateData');
        xmlhttp.setRequestHeader('Content-Type', 'application/json;');
        xmlhttp.send(json);
        return;
    };

    function enableInternetUpdate() {
        // receiving event
        jsonData.data.changedtime[0].internettime = document.getElementById('internetTime').checked ? 1 : 0;
        var json = JSON.stringify(jsonData);
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.open('POST', '/updateData');
        xmlhttp.setRequestHeader('Content-Type', 'application/json;');
        xmlhttp.send(json);
        return;
    }

    const removeRelay = (payload) => {
        // displayLoadSpinner();

        let url = `/relay/${payload.relayIndex}`;
        fetch(url, { method: 'DELETE' }).then(() => {
            // Update local state
            jsonDataState.relays.splice(payload.relayIndex, 1);
            updateSchedulingHtml(true);

            closeLoadSpinner();
            displaySuccessToast();
        });
    };

    const addrelay = (obj) => {
        for (var i = 0; i < jsonData.relays.length; i++) {
            var relay = jsonData.relays[i];
            if (relay.active == 0) {
                relay.name = document.getElementById('relayname').value;
                relay.pin = document.getElementById('relaypin').value;
                relay.active = 1;
            } else {
                console.log(' no relays available');
            }
        }
        window.location.reload();
        var json = JSON.stringify(jsonData);
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.open('POST', '/updateData');
        xmlhttp.setRequestHeader('Content-Type', 'application/json;');
        xmlhttp.send(json);
        return;
    };

    const updateCredentials = (obj) => {
        var fl1 = document.getElementById('ssid').value;
        var fl2 = document.getElementById('password').value;
        jsonData.data.credentials[0].ssid = fl1;
        jsonData.data.credentials[0].password = fl2;
        console.log(fl1);
        console.log(fl2);
        var json = JSON.stringify(jsonData);
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.open('POST', '/updateData');
        xmlhttp.setRequestHeader('Content-Type', 'application/json;');
        xmlhttp.send(json);
    };

    useEffect(() => {
        fetchSettingsData('/src/mockData/data.json');
    }, []);

    return (
        <main>
            {!isLoading && !error && (
                <>
                    <DateAndTime updateTime={updateTime} enableInternetUpdate={enableInternetUpdate} />
                    <Relays removeRelay={removeRelay} addrelay={addrelay} />
                    <WifiCredentials updateCredentials={updateCredentials} />
                    <FirmwareUpdate />
                </>
            )}
        </main>
    );
};

export default Settings;
