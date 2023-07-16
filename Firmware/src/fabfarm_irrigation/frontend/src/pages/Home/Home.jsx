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
                console.log('data:', data);

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

    useEffect(() => {
        fetchData('/src/mockData/data.json');
    }, []);

    return (
        <main>
            {!isLoading && !error && (
                <>
                    <Dashboard data={DashboardData} />
                    <Controls data={ControlsData} />
                </>
            )}
        </main>
    );
};

export default Home;
