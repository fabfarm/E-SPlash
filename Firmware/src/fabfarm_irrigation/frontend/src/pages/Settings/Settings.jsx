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
    const [relayNameInput, setRelayNameInput] = useState('');
    const [relayPinInput, setRelayPinInput] = useState('');
    const [relayStatusInput, setRelayStatusInput] = useState('');
    const [ssidInput, setSsidInput] = useState('');
    const [passwordInput, setPasswordInput] = useState('');

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

    const removeRelay = (relayId) => {
        fetch({
            url: `/relays/${relayId}`,
            method: 'DELETE',
            headers: { 'Content-Type': 'application/json' },
        })
            .then((res) => res.json())
            .then((data) => setData(data));
    };

    const addrelay = () => {
        fetch({
            url: '/relays/add',
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: { name: relayNameInput, pin: relayPinInput, isEnabled: relayStatusInput },
        })
            .then((res) => res.json())
            .then((data) => setData(data));
    };

    const updateCredentials = (obj) => {
        fetch({
            url: '/credentials',
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: { ssid: ssidInput, password: passwordInput },
        })
            .then((res) => res.json())
            .then((data) => setData(data));
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
                    <Relays
                        relays={data.relays}
                        removeRelay={removeRelay}
                        relayNameInput={relayNameInput}
                        setRelayNameInput={setRelayNameInput}
                        relayPinInput={relayPinInput}
                        setRelayPinInput={setRelayPinInput}
                        relayStatusInput={relayStatusInput}
                        setRelayStatusInput={setRelayStatusInput}
                        addrelay={addrelay}
                    />
                    <WifiCredentials
                        ssidInput={ssidInput}
                        setSsidInput={setSsidInput}
                        passwordInput={passwordInput}
                        setPasswordInput={setPasswordInput}
                        updateCredentials={updateCredentials}
                    />
                    <FirmwareUpdate updateFirmware={updateFirmware} />
                </>
            )}
        </main>
    );
};

export default Settings;
