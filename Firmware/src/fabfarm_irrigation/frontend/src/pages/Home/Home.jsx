import { useState, useEffect } from 'react';

import Dashboard from '../../components/Dashboard';
import Controls from '../../components/Controls';

const Home = () => {
    const [isLoading, setIsLoading] = useState(true);
    const [error, setError] = useState(null);

    const [data, setData] = useState(null);
    // const [currentTimes, setCurrentTimes] = useState([]);

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

    const handleScheduleModeChange = (e) => {
        const url = e.target.checked ? '/mode/scheduled' : '/mode/manual';

        fetch(url)
            .then((res) => res.json())
            .then((data) => {
                setData(data);
            });
    };

    const handleToggleRelay = (e, index) => {
        // Look at updateRelayEnabled
        const url = e.target.checked ? `/relay/${index}/on` : `/relay/${index}/off`;

        fetch(url)
            .then((res) => res.json())
            .then((data) => {
                setData(data);
            });
    };

    // use id instead of pin
    const handleSchedulingInputsChange = (e, relayIndex, scheduleIndex) => {
        setData((prevState) => {
            const newState = Object.assign({}, prevState);

            if (e.target.name === 'startTime') {
                newState.relays[relayIndex].times[scheduleIndex].startTime = e.target.value;
            }

            if (e.target.name === 'duration') {
                newState.relays[relayIndex].times[scheduleIndex].duration = e.target.value;
            }

            return newState;
        });

        // IF i wanna have the sync indicator...

        // if (data.relays[relayIndex].times[scheduleIndex].startTime === e.target.value)

        // setCurrentTimes((prevState) => {
        //     const newState = Object.assign({}, prevState);

        //     if (e.target.name === 'startTime') {
        //         newState[relayIndex][scheduleIndex].startTime = e.target.value;
        //     }

        //     if (e.target.name === 'duration') {
        //         newState[relayIndex][scheduleIndex].duration = e.target.value;
        //     }

        //     return newState;
        // });
    };

    // look at updateRelayTimes
    const postRelayTime = (relayIndex, scheduleIndex) => {
        fetch('/relay/update-time', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                relayIndex: relayIndex,
                timeIndex: scheduleIndex,
                startTime: data.relays[relayIndex].times[scheduleIndex].startTime,
                duration: data.relays[relayIndex].times[scheduleIndex].duration,
            }),
        })
            .then((res) => res.json())
            .then((data) => {
                setData(data);
            });
    };

    const removeTime = (relayIndex, scheduleIndex) => {
        const url = `/relay/${relayIndex}/time/${scheduleIndex}`;
        fetch(url, { method: 'DELETE' })
            .then((res) => res.json())
            .then((data) => {
                setData(data);
            });
    };

    const addTime = (relayIndex) => {
        fetch('/relay/add-time', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ relayIndex }),
        })
            .then((res) => res.json())
            .then((data) => {
                setData(data);
            });
    };

    useEffect(() => {
        fetchData('/src/mockData/data.json');
    }, []);

    return (
        <main>
            {!isLoading && !error && (
                <>
                    <Dashboard data={data} />
                    <Controls
                        data={data}
                        // currentTimes={currentTimes}
                        handleScheduleModeChange={handleScheduleModeChange}
                        handleToggleRelay={handleToggleRelay}
                        handleSchedulingInputsChange={handleSchedulingInputsChange}
                        postRelayTime={postRelayTime}
                        removeTime={removeTime}
                        addTime={addTime}
                    />
                </>
            )}
        </main>
    );
};

export default Home;
