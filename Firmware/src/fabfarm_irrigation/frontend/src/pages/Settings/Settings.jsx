import { useState, useEffect } from 'react';

import DateAndTime from '../../components/DateAndTime';
import Relays from '../../components/Relays';
import WifiCredentials from '../../components/WifiCredentials';
import FirmwareUpdate from '../../components/FirmwareUpdate';

const Settings = () => {
    const [isLoading, setIsLoading] = useState(true);
    const [error, setError] = useState(null);
    const [data, setData] = useState(null);

    // const [dateTimeInputs, setDateTimeInputs] = useState();

    const fetchData = (url) => {
        setIsLoading(true);

        fetch(url)
            .then((res) => res.json())
            .then((data) => {
                console.log('fetched data:', data);
                setData(data);
            })
            .catch((err) => {
                setError(err);
            })
            .finally(() => {
                setIsLoading(false);
            });
    };

    const updateTime = (obj) => {
        data.data.changedtime[0].manualtimeenabled = 0;
        var fl1 = document.getElementById('date').value;
        var fl2 = document.getElementById('time').value;
        data.data.changedtime[0].Ttime = fl2;
        var timex = new Date(fl1);
        var hour = fl2.split(':')[0];
        var min = fl2.split(':')[1];
        var day = timex.getDate();
        var month = timex.getMonth();
        var year = timex.getYear();
        data.data.changedtime[0].min = parseInt(min);
        data.data.changedtime[0].hour = parseInt(hour);
        data.data.changedtime[0].day = day;
        data.data.changedtime[0].month = month + 1;
        data.data.changedtime[0].year = year + 1900;
        console.log(day);
        console.log(data.data.changedtime[0].day);
        console.log(data.data.changedtime.year);
        var json = JSON.stringify(data);
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.open('POST', '/updateData');
        xmlhttp.setRequestHeader('Content-Type', 'application/json;');
        xmlhttp.send(json);
        return;
    };

    const removeRelay = (payload) => {
        // displayLoadSpinner();

        let url = `/relay/${payload.relayIndex}`;
        fetch(url, { method: 'DELETE' }).then(() => {
            // Update local state
            dataState.relays.splice(payload.relayIndex, 1);
            updateSchedulingHtml(true);

            closeLoadSpinner();
            displaySuccessToast();
        });
    };

    const addrelay = (obj) => {
        for (var i = 0; i < data.relays.length; i++) {
            var relay = data.relays[i];
            if (relay.active == 0) {
                relay.name = document.getElementById('relayname').value;
                relay.pin = document.getElementById('relaypin').value;
                relay.active = 1;
            } else {
                console.log(' no relays available');
            }
        }
        window.location.reload();
        var json = JSON.stringify(data);
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.open('POST', '/updateData');
        xmlhttp.setRequestHeader('Content-Type', 'application/json;');
        xmlhttp.send(json);
        return;
    };

    const updateCredentials = (obj) => {
        var fl1 = document.getElementById('ssid').value;
        var fl2 = document.getElementById('password').value;
        data.data.credentials[0].ssid = fl1;
        data.data.credentials[0].password = fl2;
        console.log(fl1);
        console.log(fl2);
        var json = JSON.stringify(data);
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.open('POST', '/updateData');
        xmlhttp.setRequestHeader('Content-Type', 'application/json;');
        xmlhttp.send(json);
    };

    const updateFirmware = () => {
        // go to /update
    };

    useEffect(() => {
        fetchData('/src/mockData/data.json');
    }, []);

    return (
        <main>
            {!isLoading && !error && (
                <>
                    <DateAndTime updateTime={updateTime} />
                    <Relays removeRelay={removeRelay} addrelay={addrelay} />
                    <WifiCredentials updateCredentials={updateCredentials} />
                    <FirmwareUpdate updateFirmware={updateFirmware} />
                </>
            )}
        </main>
    );
};

export default Settings;
