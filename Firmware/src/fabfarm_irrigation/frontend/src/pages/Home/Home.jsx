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
        // same as above here. Look at updateRelayEnabled

        // !
        // https://stackoverflow.com/questions/43638938/updating-an-object-with-setstate-in-react

        setControlsData((prevState) => {
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

    // for automatic time check updateRelayTimes
    // use id instead of pin
    const handleRelayAutomaticTime = (e, pin) => {
        console.log('handleRelayAutomaticTime', e, pin);

        // probab devo usare una variabile da sola non contrlsData innestato. Perchè è solo per tenere traccia dei cambiamenti.
        // per il resto dovrò fare la chiamata al server che ritornerà lo stto aggiornato.
        // O mesà controlsData e dashboardData li uso solo per settare lo stato con il response del fetch.
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
                        handleRelayAutomaticTime={handleRelayAutomaticTime}
                    />
                </>
            )}
        </main>
    );
};

export default Home;
