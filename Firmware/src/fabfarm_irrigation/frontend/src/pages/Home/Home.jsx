import { useState, useEffect } from 'react';
import { useData } from '../../hooks/useData';

import Dashboard from '../../components/Dashboard';
import Controls from '../../components/Controls';

const Home = () => {
    const { data, setData } = useData();

    const [startTime, setStartTime] = useState('');
    const [duration, setDuration] = useState('');

    const handleScheduleModeChange = (e, relayId) => {
        fetch({
            url: `/relays/${relayId}/schedule-mode`,
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: { isScheduleMode: e.target.checked },
        })
            .then((res) => res.json())
            .then((data) => setData(data));
    };

    // should use an id instead of pin
    const handleToggleRelay = (e, relayId) => {
        fetch({
            url: `/relays/${relayId}/enable`,
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: { isEnabled: e.target.checked },
        })
            .then((res) => res.json())
            .then((data) => setData(data));
    };

    const addSchedule = (relayId) => {
        fetch({
            url: `/relays/${relayId}/schedule`,
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: { startTime, duration },
        })
            .then((res) => res.json())
            // or should the server just return { "id": string, "startTime": string , "duration": number }
            // and should I just setData with that prop?
            .then((data) => setData(data));
    };

    return (
        <main>
            {!!data && (
                <>
                    <Dashboard data={data} />
                    <Controls
                        data={data}
                        handleScheduleModeChange={handleScheduleModeChange}
                        handleToggleRelay={handleToggleRelay}
                        scheduleInputs={{
                            startTime,
                            setStartTime,
                            duration,
                            setDuration,
                        }}
                        addSchedule={addSchedule}
                    />
                </>
            )}
        </main>
    );
};

export default Home;
