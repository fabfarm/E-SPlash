import { useState, useEffect } from 'react';
import { useData } from '../../hooks/useData';

import Dashboard from '../../components/Dashboard';
import Controls from '../../components/Controls';

const Home = () => {
    const { data } = useData();

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

    // use id instead of pin
    const handleAutoTimeChange = (e, pin) => {
        console.log('handleAutoTimeChange', e, pin);

        // probab devo usare una variabile da sola non contrlsData innestato. Perchè è solo per tenere traccia dei cambiamenti.
        // per il resto dovrò fare la chiamata al server che ritornerà lo stato aggiornato (!che farà il set-time btn)
        // O mesà controlsData e dashboardData li uso solo per settare lo stato con il response del fetch.
    };

    // look at updateRelayTimes
    // better name here
    const setAutomaticTime = (e) => {
        console.log('setAutomaticTime', e);
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
                        handleAutoTimeChange={handleAutoTimeChange}
                        setAutomaticTime={setAutomaticTime}
                    />
                </>
            )}
        </main>
    );
};

export default Home;
