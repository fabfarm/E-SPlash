import { useState, useEffect } from 'react';

import Dashboard from '../../components/Dashboard';
import Controls from '../../components/Controls';

const Home = () => {
    const [isLoading, setIsLoading] = useState(true);
    const [error, setError] = useState(null);

    const [DashboardData, setDashboardData] = useState(null);
    const [ControlsData, setControlsData] = useState(null);

    const fetchData = (url) => {
        setIsLoading(true);

        fetch(url)
            .then((res) => res.json())
            .then((data) => {
                console.log('FETCHED data:', data);

                // Now adjusting data structure. Eventually should be done on the server

                setDashboardData({
                    currentTime: data.data.currentTime,
                    temperature: data.data.temperature,
                    humidity: data.data.humidity,
                    batLevel: data.data.batLevel,
                });

                setControlsData({
                    isScheduleMode: data.data.isScheduleMode,
                    relays: data.relays,
                });
            })
            .catch((err) => {
                setError(err);
            })
            .finally(() => {
                setIsLoading(false);
            });
    };

    const handleScheduleModeChange = () => {
        // I should use something like updateSchedulingMode in scrpit.js. That is, instead of updating the state directly
        // I should POST to server and re-set state based on the response.

        setControlsData((prevState) => {
            return {
                ...prevState,
                isScheduleMode: !prevState.isScheduleMode,
            };
        });
    };

    // should use an id instead of pin
    const handleToggleRelay = (pin) => {
        // same as above here. Use updateRelayEnabled

        console.log('!!!pin', pin);

        // const updatedRelaysData = ControlsData.relays.map((relay) => {
        //     if (relay.pin !== pin) return;

        //     relay.isEnabled = !relay.isEnabled;
        //     return relay;
        // })

        // !
        // https://stackoverflow.com/questions/43638938/updating-an-object-with-setstate-in-react

        setControlsData((prevState) => {
            console.log('!!!prevState', prevState);

            // const newRelays = prevState.relays.map((relay) => {
            //     // return { ...relay, isEnabled: !relay.isEnabled };

            //     // if (relay.pin === pin) {
            //     //     // console.log('§§§', relay);
            //     //     // relay.isEnabled = !relay.isEnabled;
            //     //     // console.log('§§§', relay);

            //     //     relay = { ...relay, isEnabled: !relay.isEnabled };
            //     // }

            //     // return relay;

            //     const newRelay = Object.assign({}, relay);

            //     if (relay.pin === pin) {
            //         newRelay.isEnabled = !newRelay.isEnabled;
            //     }

            //     return newRelay;
            // });
            // console.log('*1 newRelays:', newRelays);

            // const newState = {
            //     ...prevState,
            //     // relays: prevState.relays.map((relay) => {
            //     //     console.log('###0', relay);
            //     //     if (relay.pin === pin) {
            //     //         console.log('###1', relay.pin, relay);
            //     //         relay.isEnabled = !relay.isEnabled;
            //     //         console.log('###2', relay.isEnabled);
            //     //     }

            //     //     console.log('###3', relay.pin, relay);
            //     //     return relay;
            //     // }),
            //     relays: newRelays,
            // };

            // console.log('@@@ newState:', newState);
            // return newState;

            // const newState = Object.assign({}, prevState);

            // newState.relays = newState.relays.map((relay) => {
            //     const newRelay = Object.assign({}, relay);

            //     if (relay.pin === pin) {
            //         newRelay.isEnabled = !relay.isEnabled;
            //     }

            //     return newRelay;
            // });

            // return newState;

            // GOOD HERE!
            // return {
            //     ...prevState,
            //     relays: prevState.relays.map((relay) => {
            //         return {
            //             ...relay,
            //             isEnabled: relay.pin === pin ? !relay.isEnabled : relay.isEnabled,
            //         };
            //     }),
            // };

            return {
                ...prevState,
                relays: prevState.relays.map((relay) => {
                    if (relay.pin === pin) {
                        return {
                            ...relay,
                            isEnabled: !relay.isEnabled,
                        };
                    }

                    return relay;
                }),
            };
        });
    };

    useEffect(() => {
        fetchData('/src/mockData/data.json');
    }, []);

    return (
        <main>
            {!isLoading && !error && (
                <>
                    <Dashboard data={DashboardData} />
                    <Controls
                        data={ControlsData}
                        handleScheduleModeChange={handleScheduleModeChange}
                        handleToggleRelay={handleToggleRelay}
                    />
                </>
            )}
        </main>
    );
};

export default Home;
