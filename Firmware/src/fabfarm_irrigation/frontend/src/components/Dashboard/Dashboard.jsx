import { useState, useEffect } from 'react';
import './Dashboard.css';

const Dashboard = () => {
    const [data, setData] = useState({});

    const fetchData = (url) => {
        fetch(url)
            .then((res) => res.json())
            .then((data) => {
                console.log('### data:', data);
                setData(data.data);
            });
    };

    useEffect(() => {
        fetchData('/src/mockData/data.json');
    }, []);

    return (
        <section className='dashboard'>
            <header className='page-title'>
                <h2>Dashboard</h2>
            </header>

            <div className='cards-container'>
                <div className='card'>
                    <div>Monday, July 10 2023</div>
                    <div className='card-value'>{data.currentTime}</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>{data.temperature} C</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>{data.humidity}</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>{data.batLevel}</div>
                </div>
            </div>
        </section>
    );
};

export default Dashboard;
