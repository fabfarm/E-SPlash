import { useState, useEffect } from 'react';
import { useData } from '../../hooks/useData';

import DateAndTime from '../../components/DateAndTime';
import Relays from '../../components/Relays';
import WifiCredentials from '../../components/WifiCredentials';
import FirmwareUpdate from '../../components/FirmwareUpdate';

const Settings = () => {
    const { data, setData } = useData();

    const [inputDate, setInputDate] = useState('');
    const [inputTime, setInputTime] = useState('');

    const setDateTime = () => {
        fetch({
            url: '/time/set',
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: { time: inputDate + 'T' + inputTime },
        })
            .then((res) => res.json())
            .then((data) => setData(data));
    };

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

    const updateFirmware = () => {
        // go to /update
    };

    return (
        <main>
            {!!data && (
                <>
                    <DateAndTime
                        currentTime={data.global.time}
                        inputDate={inputDate}
                        setInputDate={setInputDate}
                        inputTime={inputTime}
                        setInputTime={setInputTime}
                        setDateTime={setDateTime}
                    />
                    <Relays removeRelay={removeRelay} addrelay={addrelay} />
                    <WifiCredentials updateCredentials={updateCredentials} />
                    <FirmwareUpdate updateFirmware={updateFirmware} />
                </>
            )}
        </main>
    );
};

export default Settings;
